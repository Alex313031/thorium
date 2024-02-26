/*
 * Copyright (c) Lynne
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define VK_NO_PROTOTYPES
#define VK_ENABLE_BETA_EXTENSIONS

#ifdef _WIN32
#include <windows.h> /* Included to prevent conflicts with CreateSemaphore */
#include <versionhelpers.h>
#include "compat/w32dlfcn.h"
#else
#include <dlfcn.h>
#endif

#include "thread.h"
#include <unistd.h>

#include "config.h"
#include "pixdesc.h"
#include "avstring.h"
#include "imgutils.h"
#include "hwcontext.h"
#include "avassert.h"
#include "hwcontext_internal.h"
#include "hwcontext_vulkan.h"

#include "vulkan.h"
#include "vulkan_loader.h"

#if CONFIG_VAAPI
#include "hwcontext_vaapi.h"
#endif

#if CONFIG_LIBDRM
#if CONFIG_VAAPI
#include <va/va_drmcommon.h>
#endif
#ifdef __linux__
#include <sys/sysmacros.h>
#endif
#include <sys/stat.h>
#include <xf86drm.h>
#include <drm_fourcc.h>
#include "hwcontext_drm.h"
#endif

#if CONFIG_CUDA
#include "hwcontext_cuda_internal.h"
#include "cuda_check.h"
#define CHECK_CU(x) FF_CUDA_CHECK_DL(cuda_cu, cu, x)
#endif

typedef struct VulkanQueueCtx {
    VkFence fence;
    VkQueue queue;
    int was_synchronous;
    int qf;
    int qidx;

    /* Buffer dependencies */
    AVBufferRef **buf_deps;
    int nb_buf_deps;
    unsigned int buf_deps_alloc_size;
} VulkanQueueCtx;

typedef struct VulkanDevicePriv {
    /* Vulkan library and loader functions */
    void *libvulkan;

    FFVulkanContext    vkctx;
    FFVkQueueFamilyCtx compute_qf;
    FFVkQueueFamilyCtx transfer_qf;

    /* Properties */
    VkPhysicalDeviceProperties2 props;
    VkPhysicalDeviceMemoryProperties mprops;
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT hprops;

    /* Features */
    VkPhysicalDeviceVulkan11Features device_features_1_1;
    VkPhysicalDeviceVulkan12Features device_features_1_2;
    VkPhysicalDeviceVulkan13Features device_features_1_3;
    VkPhysicalDeviceDescriptorBufferFeaturesEXT desc_buf_features;
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomic_float_features;
    VkPhysicalDeviceCooperativeMatrixFeaturesKHR coop_matrix_features;

    /* Queues */
    pthread_mutex_t **qf_mutex;
    uint32_t nb_tot_qfs;
    uint32_t img_qfs[5];
    uint32_t nb_img_qfs;

    /* Debug callback */
    VkDebugUtilsMessengerEXT debug_ctx;

    /* Settings */
    int use_linear_images;

    /* Option to allocate all image planes in a single allocation */
    int contiguous_planes;

    /* Disable multiplane images */
    int disable_multiplane;

    /* Nvidia */
    int dev_is_nvidia;
} VulkanDevicePriv;

typedef struct VulkanFramesPriv {
    /* Image conversions */
    FFVkExecPool compute_exec;

    /* Image transfers */
    FFVkExecPool upload_exec;
    FFVkExecPool download_exec;

    /* Modifier info list to free at uninit */
    VkImageDrmFormatModifierListCreateInfoEXT *modifier_info;
} VulkanFramesPriv;

typedef struct AVVkFrameInternal {
    pthread_mutex_t update_mutex;

#if CONFIG_CUDA
    /* Importing external memory into cuda is really expensive so we keep the
     * memory imported all the time */
    AVBufferRef *cuda_fc_ref; /* Need to keep it around for uninit */
    CUexternalMemory ext_mem[AV_NUM_DATA_POINTERS];
    CUmipmappedArray cu_mma[AV_NUM_DATA_POINTERS];
    CUarray cu_array[AV_NUM_DATA_POINTERS];
    CUexternalSemaphore cu_sem[AV_NUM_DATA_POINTERS];
#ifdef _WIN32
    HANDLE ext_mem_handle[AV_NUM_DATA_POINTERS];
    HANDLE ext_sem_handle[AV_NUM_DATA_POINTERS];
#endif
#endif
} AVVkFrameInternal;

#define ASPECT_2PLANE (VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT)
#define ASPECT_3PLANE (VK_IMAGE_ASPECT_PLANE_0_BIT | VK_IMAGE_ASPECT_PLANE_1_BIT | VK_IMAGE_ASPECT_PLANE_2_BIT)

static const struct FFVkFormatEntry {
    VkFormat vkf;
    enum AVPixelFormat pixfmt;
    VkImageAspectFlags aspect;
    int vk_planes;
    int nb_images;
    int nb_images_fallback;
    const VkFormat fallback[5];
} vk_formats_list[] = {
    /* Gray formats */
    { VK_FORMAT_R8_UNORM,   AV_PIX_FMT_GRAY8,   VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R8_UNORM   } },
    { VK_FORMAT_R16_UNORM,  AV_PIX_FMT_GRAY16,  VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R16_UNORM  } },
    { VK_FORMAT_R32_SFLOAT, AV_PIX_FMT_GRAYF32, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R32_SFLOAT } },

    /* RGB formats */
    { VK_FORMAT_R16G16B16A16_UNORM,       AV_PIX_FMT_XV36,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R16G16B16A16_UNORM       } },
    { VK_FORMAT_B8G8R8A8_UNORM,           AV_PIX_FMT_BGRA,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_B8G8R8A8_UNORM           } },
    { VK_FORMAT_R8G8B8A8_UNORM,           AV_PIX_FMT_RGBA,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R8G8B8A8_UNORM           } },
    { VK_FORMAT_R8G8B8_UNORM,             AV_PIX_FMT_RGB24,   VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R8G8B8_UNORM             } },
    { VK_FORMAT_B8G8R8_UNORM,             AV_PIX_FMT_BGR24,   VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_B8G8R8_UNORM             } },
    { VK_FORMAT_R16G16B16_UNORM,          AV_PIX_FMT_RGB48,   VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R16G16B16_UNORM          } },
    { VK_FORMAT_R16G16B16A16_UNORM,       AV_PIX_FMT_RGBA64,  VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R16G16B16A16_UNORM       } },
    { VK_FORMAT_R5G6B5_UNORM_PACK16,      AV_PIX_FMT_RGB565,  VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R5G6B5_UNORM_PACK16      } },
    { VK_FORMAT_B5G6R5_UNORM_PACK16,      AV_PIX_FMT_BGR565,  VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_B5G6R5_UNORM_PACK16      } },
    { VK_FORMAT_B8G8R8A8_UNORM,           AV_PIX_FMT_BGR0,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_B8G8R8A8_UNORM           } },
    { VK_FORMAT_R8G8B8A8_UNORM,           AV_PIX_FMT_RGB0,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R8G8B8A8_UNORM           } },
    { VK_FORMAT_A2R10G10B10_UNORM_PACK32, AV_PIX_FMT_X2RGB10, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_A2R10G10B10_UNORM_PACK32 } },

    /* Planar RGB */
    { VK_FORMAT_R8_UNORM,   AV_PIX_FMT_GBRAP,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 4, 4, { VK_FORMAT_R8_UNORM,   VK_FORMAT_R8_UNORM,   VK_FORMAT_R8_UNORM,   VK_FORMAT_R8_UNORM   } },
    { VK_FORMAT_R16_UNORM,  AV_PIX_FMT_GBRAP16,  VK_IMAGE_ASPECT_COLOR_BIT, 1, 4, 4, { VK_FORMAT_R16_UNORM,  VK_FORMAT_R16_UNORM,  VK_FORMAT_R16_UNORM,  VK_FORMAT_R16_UNORM  } },
    { VK_FORMAT_R32_SFLOAT, AV_PIX_FMT_GBRPF32,  VK_IMAGE_ASPECT_COLOR_BIT, 1, 3, 3, { VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32_SFLOAT                       } },
    { VK_FORMAT_R32_SFLOAT, AV_PIX_FMT_GBRAPF32, VK_IMAGE_ASPECT_COLOR_BIT, 1, 4, 4, { VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32_SFLOAT, VK_FORMAT_R32_SFLOAT } },

    /* Two-plane 420 YUV at 8, 10, 12 and 16 bits */
    { VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,                  AV_PIX_FMT_NV12, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R8_UNORM,  VK_FORMAT_R8G8_UNORM   } },
    { VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16, AV_PIX_FMT_P010, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },
    { VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16, AV_PIX_FMT_P012, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },
    { VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,               AV_PIX_FMT_P016, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },

    /* Two-plane 422 YUV at 8, 10 and 16 bits */
    { VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,                  AV_PIX_FMT_NV16, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R8_UNORM,  VK_FORMAT_R8G8_UNORM   } },
    { VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16, AV_PIX_FMT_P210, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },
    { VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16, AV_PIX_FMT_P212, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },
    { VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,               AV_PIX_FMT_P216, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },

    /* Two-plane 444 YUV at 8, 10 and 16 bits */
    { VK_FORMAT_G8_B8R8_2PLANE_444_UNORM,                  AV_PIX_FMT_NV24, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R8_UNORM,  VK_FORMAT_R8G8_UNORM   } },
    { VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16, AV_PIX_FMT_P410, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },
    { VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16, AV_PIX_FMT_P412, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },
    { VK_FORMAT_G16_B16R16_2PLANE_444_UNORM,               AV_PIX_FMT_P416, ASPECT_2PLANE, 2, 1, 2, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16G16_UNORM } },

    /* Three-plane 420, 422, 444 at 8, 10, 12 and 16 bits */
    { VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,    AV_PIX_FMT_YUV420P,   ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R8_UNORM,  VK_FORMAT_R8_UNORM,  VK_FORMAT_R8_UNORM  } },
    { VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM, AV_PIX_FMT_YUV420P10, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM, AV_PIX_FMT_YUV420P12, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM, AV_PIX_FMT_YUV420P16, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,    AV_PIX_FMT_YUV422P,   ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R8_UNORM,  VK_FORMAT_R8_UNORM,  VK_FORMAT_R8_UNORM  } },
    { VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM, AV_PIX_FMT_YUV422P10, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM, AV_PIX_FMT_YUV422P12, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM, AV_PIX_FMT_YUV422P16, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,    AV_PIX_FMT_YUV444P,   ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R8_UNORM,  VK_FORMAT_R8_UNORM,  VK_FORMAT_R8_UNORM  } },
    { VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM, AV_PIX_FMT_YUV444P10, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM, AV_PIX_FMT_YUV444P12, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },
    { VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM, AV_PIX_FMT_YUV444P16, ASPECT_3PLANE, 3, 1, 3, { VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM, VK_FORMAT_R16_UNORM } },

    /* Single plane 422 at 8, 10 and 12 bits */
    { VK_FORMAT_G8B8G8R8_422_UNORM,                     AV_PIX_FMT_YUYV422, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R8G8B8A8_UNORM     } },
    { VK_FORMAT_B8G8R8G8_422_UNORM,                     AV_PIX_FMT_UYVY422, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R8G8B8A8_UNORM     } },
    { VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16, AV_PIX_FMT_Y210,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R16G16B16A16_UNORM } },
    { VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16, AV_PIX_FMT_Y212,    VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, 1, { VK_FORMAT_R16G16B16A16_UNORM } },
};
static const int nb_vk_formats_list = FF_ARRAY_ELEMS(vk_formats_list);

const VkFormat *av_vkfmt_from_pixfmt(enum AVPixelFormat p)
{
    for (int i = 0; i < nb_vk_formats_list; i++)
        if (vk_formats_list[i].pixfmt == p)
            return vk_formats_list[i].fallback;
    return NULL;
}

static const struct FFVkFormatEntry *vk_find_format_entry(enum AVPixelFormat p)
{
    for (int i = 0; i < nb_vk_formats_list; i++)
        if (vk_formats_list[i].pixfmt == p)
            return &vk_formats_list[i];
    return NULL;
}

/* Malitia pura, Khronos */
#define FN_MAP_TO(dst_t, dst_name, src_t, src_name)                                 \
    static av_unused dst_t map_ ##src_name## _to_ ##dst_name(src_t src) \
    {                                                                   \
        dst_t dst = 0x0;                                                \
        MAP_TO(VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT,                   \
               VK_IMAGE_USAGE_SAMPLED_BIT);                             \
        MAP_TO(VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT,                    \
               VK_IMAGE_USAGE_TRANSFER_SRC_BIT);                        \
        MAP_TO(VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT,                    \
               VK_IMAGE_USAGE_TRANSFER_DST_BIT);                        \
        MAP_TO(VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT,                   \
               VK_IMAGE_USAGE_STORAGE_BIT);                             \
        MAP_TO(VK_FORMAT_FEATURE_2_COLOR_ATTACHMENT_BIT,                \
               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);                    \
        MAP_TO(VK_FORMAT_FEATURE_2_VIDEO_DECODE_OUTPUT_BIT_KHR,         \
               VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);                \
        MAP_TO(VK_FORMAT_FEATURE_2_VIDEO_DECODE_DPB_BIT_KHR,            \
               VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR);                \
        MAP_TO(VK_FORMAT_FEATURE_2_VIDEO_ENCODE_DPB_BIT_KHR,            \
               VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR);                \
        MAP_TO(VK_FORMAT_FEATURE_2_VIDEO_ENCODE_INPUT_BIT_KHR,          \
               VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR);                \
        return dst;                                                     \
    }

#define MAP_TO(flag1, flag2) if (src & flag2) dst |= flag1;
FN_MAP_TO(VkFormatFeatureFlagBits2, feats, VkImageUsageFlags, usage)
#undef MAP_TO
#define MAP_TO(flag1, flag2) if (src & flag1) dst |= flag2;
FN_MAP_TO(VkImageUsageFlags, usage, VkFormatFeatureFlagBits2, feats)
#undef MAP_TO
#undef FN_MAP_TO

static int vkfmt_from_pixfmt2(AVHWDeviceContext *dev_ctx, enum AVPixelFormat p,
                              VkImageTiling tiling,
                              VkFormat fmts[AV_NUM_DATA_POINTERS], /* Output format list */
                              int *nb_images,                      /* Output number of images */
                              VkImageAspectFlags *aspect,          /* Output aspect */
                              VkImageUsageFlags *supported_usage,  /* Output supported usage */
                              int disable_multiplane, int need_storage)
{
    AVVulkanDeviceContext *hwctx = dev_ctx->hwctx;
    VulkanDevicePriv *priv = dev_ctx->internal->priv;
    FFVulkanFunctions *vk = &priv->vkctx.vkfn;

    const VkFormatFeatureFlagBits2 basic_flags = VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT |
                                                 VK_FORMAT_FEATURE_2_TRANSFER_SRC_BIT  |
                                                 VK_FORMAT_FEATURE_2_TRANSFER_DST_BIT;

    for (int i = 0; i < nb_vk_formats_list; i++) {
        if (vk_formats_list[i].pixfmt == p) {
            VkFormatProperties3 fprops = {
                .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3,
            };
            VkFormatProperties2 prop = {
                .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2,
                .pNext = &fprops,
            };
            VkFormatFeatureFlagBits2 feats_primary, feats_secondary;
            int basics_primary = 0, basics_secondary = 0;
            int storage_primary = 0, storage_secondary = 0;

            vk->GetPhysicalDeviceFormatProperties2(hwctx->phys_dev,
                                                   vk_formats_list[i].vkf,
                                                   &prop);

            feats_primary = tiling == VK_IMAGE_TILING_LINEAR ?
                             fprops.linearTilingFeatures : fprops.optimalTilingFeatures;
            basics_primary = (feats_primary & basic_flags) == basic_flags;
            storage_primary = !!(feats_primary & VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT);

            if (vk_formats_list[i].vkf != vk_formats_list[i].fallback[0]) {
                vk->GetPhysicalDeviceFormatProperties2(hwctx->phys_dev,
                                                       vk_formats_list[i].fallback[0],
                                                       &prop);
                feats_secondary = tiling == VK_IMAGE_TILING_LINEAR ?
                                  fprops.linearTilingFeatures : fprops.optimalTilingFeatures;
                basics_secondary = (feats_secondary & basic_flags) == basic_flags;
                storage_secondary = !!(feats_secondary & VK_FORMAT_FEATURE_2_STORAGE_IMAGE_BIT);
            } else {
                basics_secondary = basics_primary;
                storage_secondary = storage_primary;
            }

            if (basics_primary &&
                !(disable_multiplane && vk_formats_list[i].vk_planes > 1) &&
                (!need_storage || (need_storage && (storage_primary | storage_secondary)))) {
                if (fmts)
                    fmts[0] = vk_formats_list[i].vkf;
                if (nb_images)
                    *nb_images = 1;
                if (aspect)
                    *aspect = vk_formats_list[i].aspect;
                if (supported_usage)
                    *supported_usage = map_feats_to_usage(feats_primary) |
                                       ((need_storage && (storage_primary | storage_secondary)) ?
                                        VK_IMAGE_USAGE_STORAGE_BIT : 0);
                return 0;
            } else if (basics_secondary &&
                       (!need_storage || (need_storage && storage_secondary))) {
                if (fmts) {
                    for (int j = 0; j < vk_formats_list[i].nb_images_fallback; j++)
                        fmts[j] = vk_formats_list[i].fallback[j];
                }
                if (nb_images)
                    *nb_images = vk_formats_list[i].nb_images_fallback;
                if (aspect)
                    *aspect = vk_formats_list[i].aspect;
                if (supported_usage)
                    *supported_usage = map_feats_to_usage(feats_secondary);
                return 0;
            } else {
                return AVERROR(ENOTSUP);
            }
        }
    }

    return AVERROR(EINVAL);
}

static int load_libvulkan(AVHWDeviceContext *ctx)
{
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    VulkanDevicePriv *p = ctx->internal->priv;

    static const char *lib_names[] = {
#if defined(_WIN32)
        "vulkan-1.dll",
#elif defined(__APPLE__)
        "libvulkan.dylib",
        "libvulkan.1.dylib",
        "libMoltenVK.dylib",
#else
        "libvulkan.so.1",
        "libvulkan.so",
#endif
    };

    for (int i = 0; i < FF_ARRAY_ELEMS(lib_names); i++) {
        p->libvulkan = dlopen(lib_names[i], RTLD_NOW | RTLD_LOCAL);
        if (p->libvulkan)
            break;
    }

    if (!p->libvulkan) {
        av_log(ctx, AV_LOG_ERROR, "Unable to open the libvulkan library!\n");
        return AVERROR_UNKNOWN;
    }

    hwctx->get_proc_addr = (PFN_vkGetInstanceProcAddr)dlsym(p->libvulkan, "vkGetInstanceProcAddr");

    return 0;
}

typedef struct VulkanOptExtension {
    const char *name;
    FFVulkanExtensions flag;
} VulkanOptExtension;

static const VulkanOptExtension optional_instance_exts[] = {
    { VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,          FF_VK_EXT_NO_FLAG                },
};

static const VulkanOptExtension optional_device_exts[] = {
    /* Misc or required by other extensions */
    { VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,               FF_VK_EXT_NO_FLAG                },
    { VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,                  FF_VK_EXT_NO_FLAG                },
    { VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,         FF_VK_EXT_NO_FLAG                },
    { VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,                FF_VK_EXT_DESCRIPTOR_BUFFER,     },
    { VK_EXT_PHYSICAL_DEVICE_DRM_EXTENSION_NAME,              FF_VK_EXT_DEVICE_DRM             },
    { VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,              FF_VK_EXT_ATOMIC_FLOAT           },
    { VK_KHR_COOPERATIVE_MATRIX_EXTENSION_NAME,               FF_VK_EXT_COOP_MATRIX            },

    /* Imports/exports */
    { VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME,               FF_VK_EXT_EXTERNAL_FD_MEMORY     },
    { VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME,          FF_VK_EXT_EXTERNAL_DMABUF_MEMORY },
    { VK_EXT_IMAGE_DRM_FORMAT_MODIFIER_EXTENSION_NAME,        FF_VK_EXT_DRM_MODIFIER_FLAGS     },
    { VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME,            FF_VK_EXT_EXTERNAL_FD_SEM        },
    { VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME,             FF_VK_EXT_EXTERNAL_HOST_MEMORY   },
#ifdef _WIN32
    { VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,            FF_VK_EXT_EXTERNAL_WIN32_MEMORY  },
    { VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,         FF_VK_EXT_EXTERNAL_WIN32_SEM     },
#endif

    /* Video encoding/decoding */
    { VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,                      FF_VK_EXT_VIDEO_QUEUE            },
    { VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,               FF_VK_EXT_VIDEO_DECODE_QUEUE     },
    { VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,                FF_VK_EXT_VIDEO_DECODE_H264      },
    { VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,                FF_VK_EXT_VIDEO_DECODE_H265      },
    { "VK_MESA_video_decode_av1",                             FF_VK_EXT_VIDEO_DECODE_AV1       },
};

static VkBool32 VKAPI_CALL vk_dbg_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                           const VkDebugUtilsMessengerCallbackDataEXT *data,
                                           void *priv)
{
    int l;
    AVHWDeviceContext *ctx = priv;

    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: l = AV_LOG_VERBOSE; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    l = AV_LOG_INFO;    break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: l = AV_LOG_WARNING; break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   l = AV_LOG_ERROR;   break;
    default:                                              l = AV_LOG_DEBUG;   break;
    }

    av_log(ctx, l, "%s\n", data->pMessage);
    for (int i = 0; i < data->cmdBufLabelCount; i++)
        av_log(ctx, l, "\t%i: %s\n", i, data->pCmdBufLabels[i].pLabelName);

    return 0;
}

#define ADD_VAL_TO_LIST(list, count, val)                                      \
    do {                                                                       \
        list = av_realloc_array(list, sizeof(*list), ++count);                 \
        if (!list) {                                                           \
            err = AVERROR(ENOMEM);                                             \
            goto fail;                                                         \
        }                                                                      \
        list[count - 1] = av_strdup(val);                                      \
        if (!list[count - 1]) {                                                \
            err = AVERROR(ENOMEM);                                             \
            goto fail;                                                         \
        }                                                                      \
    } while(0)

#define RELEASE_PROPS(props, count)                                            \
    if (props) {                                                               \
        for (int i = 0; i < count; i++)                                        \
            av_free((void *)((props)[i]));                                     \
        av_free((void *)props);                                                \
    }

static int check_extensions(AVHWDeviceContext *ctx, int dev, AVDictionary *opts,
                            const char * const **dst, uint32_t *num, int debug)
{
    const char *tstr;
    const char **extension_names = NULL;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    int err = 0, found, extensions_found = 0;

    const char *mod;
    int optional_exts_num;
    uint32_t sup_ext_count;
    char *user_exts_str = NULL;
    AVDictionaryEntry *user_exts;
    VkExtensionProperties *sup_ext;
    const VulkanOptExtension *optional_exts;

    if (!dev) {
        mod = "instance";
        optional_exts = optional_instance_exts;
        optional_exts_num = FF_ARRAY_ELEMS(optional_instance_exts);
        user_exts = av_dict_get(opts, "instance_extensions", NULL, 0);
        if (user_exts) {
            user_exts_str = av_strdup(user_exts->value);
            if (!user_exts_str) {
                err = AVERROR(ENOMEM);
                goto fail;
            }
        }
        vk->EnumerateInstanceExtensionProperties(NULL, &sup_ext_count, NULL);
        sup_ext = av_malloc_array(sup_ext_count, sizeof(VkExtensionProperties));
        if (!sup_ext)
            return AVERROR(ENOMEM);
        vk->EnumerateInstanceExtensionProperties(NULL, &sup_ext_count, sup_ext);
    } else {
        mod = "device";
        optional_exts = optional_device_exts;
        optional_exts_num = FF_ARRAY_ELEMS(optional_device_exts);
        user_exts = av_dict_get(opts, "device_extensions", NULL, 0);
        if (user_exts) {
            user_exts_str = av_strdup(user_exts->value);
            if (!user_exts_str) {
                err = AVERROR(ENOMEM);
                goto fail;
            }
        }
        vk->EnumerateDeviceExtensionProperties(hwctx->phys_dev, NULL,
                                               &sup_ext_count, NULL);
        sup_ext = av_malloc_array(sup_ext_count, sizeof(VkExtensionProperties));
        if (!sup_ext)
            return AVERROR(ENOMEM);
        vk->EnumerateDeviceExtensionProperties(hwctx->phys_dev, NULL,
                                               &sup_ext_count, sup_ext);
    }

    for (int i = 0; i < optional_exts_num; i++) {
        tstr = optional_exts[i].name;
        found = 0;
        for (int j = 0; j < sup_ext_count; j++) {
            if (!strcmp(tstr, sup_ext[j].extensionName)) {
                found = 1;
                break;
            }
        }
        if (!found)
            continue;

        av_log(ctx, AV_LOG_VERBOSE, "Using %s extension %s\n", mod, tstr);
        p->vkctx.extensions |= optional_exts[i].flag;
        ADD_VAL_TO_LIST(extension_names, extensions_found, tstr);
    }

    if (debug && !dev) {
        tstr = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        found = 0;
        for (int j = 0; j < sup_ext_count; j++) {
            if (!strcmp(tstr, sup_ext[j].extensionName)) {
                found = 1;
                break;
            }
        }
        if (found) {
            av_log(ctx, AV_LOG_VERBOSE, "Using %s extension %s\n", mod, tstr);
            ADD_VAL_TO_LIST(extension_names, extensions_found, tstr);
            p->vkctx.extensions |= FF_VK_EXT_DEBUG_UTILS;
        } else {
            av_log(ctx, AV_LOG_ERROR, "Debug extension \"%s\" not found!\n",
                   tstr);
            err = AVERROR(EINVAL);
            goto fail;
        }
    }

    if (user_exts_str) {
        char *save, *token = av_strtok(user_exts_str, "+", &save);
        while (token) {
            found = 0;
            for (int j = 0; j < sup_ext_count; j++) {
                if (!strcmp(token, sup_ext[j].extensionName)) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                av_log(ctx, AV_LOG_VERBOSE, "Using %s extension \"%s\"\n", mod, token);
                ADD_VAL_TO_LIST(extension_names, extensions_found, token);
            } else {
                av_log(ctx, AV_LOG_WARNING, "%s extension \"%s\" not found, excluding.\n",
                       mod, token);
            }
            token = av_strtok(NULL, "+", &save);
        }
    }

    *dst = extension_names;
    *num = extensions_found;

    av_free(user_exts_str);
    av_free(sup_ext);
    return 0;

fail:
    RELEASE_PROPS(extension_names, extensions_found);
    av_free(user_exts_str);
    av_free(sup_ext);
    return err;
}

static int check_validation_layers(AVHWDeviceContext *ctx, AVDictionary *opts,
                                   const char * const **dst, uint32_t *num,
                                   int *debug_mode)
{
    static const char default_layer[] = { "VK_LAYER_KHRONOS_validation" };

    int found = 0, err = 0;
    VulkanDevicePriv *priv = ctx->internal->priv;
    FFVulkanFunctions *vk = &priv->vkctx.vkfn;

    uint32_t sup_layer_count;
    VkLayerProperties *sup_layers;

    AVDictionaryEntry *user_layers;
    char *user_layers_str = NULL;
    char *save, *token;

    const char **enabled_layers = NULL;
    uint32_t enabled_layers_count = 0;

    AVDictionaryEntry *debug_opt = av_dict_get(opts, "debug", NULL, 0);
    int debug = debug_opt && strtol(debug_opt->value, NULL, 10);

    /* If `debug=0`, enable no layers at all. */
    if (debug_opt && !debug)
        return 0;

    vk->EnumerateInstanceLayerProperties(&sup_layer_count, NULL);
    sup_layers = av_malloc_array(sup_layer_count, sizeof(VkLayerProperties));
    if (!sup_layers)
        return AVERROR(ENOMEM);
    vk->EnumerateInstanceLayerProperties(&sup_layer_count, sup_layers);

    av_log(ctx, AV_LOG_VERBOSE, "Supported validation layers:\n");
    for (int i = 0; i < sup_layer_count; i++)
        av_log(ctx, AV_LOG_VERBOSE, "\t%s\n", sup_layers[i].layerName);

    /* If `debug=1` is specified, enable the standard validation layer extension */
    if (debug) {
        *debug_mode = debug;
        for (int i = 0; i < sup_layer_count; i++) {
            if (!strcmp(default_layer, sup_layers[i].layerName)) {
                found = 1;
                av_log(ctx, AV_LOG_VERBOSE, "Default validation layer %s is enabled\n",
                       default_layer);
                ADD_VAL_TO_LIST(enabled_layers, enabled_layers_count, default_layer);
                break;
            }
        }
    }

    user_layers = av_dict_get(opts, "validation_layers", NULL, 0);
    if (!user_layers)
        goto end;

    user_layers_str = av_strdup(user_layers->value);
    if (!user_layers_str) {
        err = AVERROR(ENOMEM);
        goto fail;
    }

    token = av_strtok(user_layers_str, "+", &save);
    while (token) {
        found = 0;
        if (!strcmp(default_layer, token)) {
            if (debug) {
                /* if the `debug=1`, default_layer is enabled, skip here */
                token = av_strtok(NULL, "+", &save);
                continue;
            } else {
                /* if the `debug=0`, enable debug mode to load its callback properly */
                *debug_mode = debug;
            }
        }
        for (int j = 0; j < sup_layer_count; j++) {
            if (!strcmp(token, sup_layers[j].layerName)) {
                found = 1;
                break;
            }
        }
        if (found) {
            av_log(ctx, AV_LOG_VERBOSE, "Requested Validation Layer: %s\n", token);
            ADD_VAL_TO_LIST(enabled_layers, enabled_layers_count, token);
        } else {
            av_log(ctx, AV_LOG_ERROR,
                   "Validation Layer \"%s\" not support.\n", token);
            err = AVERROR(EINVAL);
            goto fail;
        }
        token = av_strtok(NULL, "+", &save);
    }

    av_free(user_layers_str);

end:
    av_free(sup_layers);

    *dst = enabled_layers;
    *num = enabled_layers_count;

    return 0;

fail:
    RELEASE_PROPS(enabled_layers, enabled_layers_count);
    av_free(sup_layers);
    av_free(user_layers_str);
    return err;
}

/* Creates a VkInstance */
static int create_instance(AVHWDeviceContext *ctx, AVDictionary *opts)
{
    int err = 0, debug_mode = 0;
    VkResult ret;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    VkApplicationInfo application_info = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "ffmpeg",
        .applicationVersion = VK_MAKE_VERSION(LIBAVUTIL_VERSION_MAJOR,
                                              LIBAVUTIL_VERSION_MINOR,
                                              LIBAVUTIL_VERSION_MICRO),
        .pEngineName        = "libavutil",
        .apiVersion         = VK_API_VERSION_1_3,
        .engineVersion      = VK_MAKE_VERSION(LIBAVUTIL_VERSION_MAJOR,
                                              LIBAVUTIL_VERSION_MINOR,
                                              LIBAVUTIL_VERSION_MICRO),
    };
    VkValidationFeaturesEXT validation_features = {
        .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    };
    VkInstanceCreateInfo inst_props = {
        .sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &application_info,
    };

    if (!hwctx->get_proc_addr) {
        err = load_libvulkan(ctx);
        if (err < 0)
            return err;
    }

    err = ff_vk_load_functions(ctx, vk, p->vkctx.extensions, 0, 0);
    if (err < 0) {
        av_log(ctx, AV_LOG_ERROR, "Unable to load instance enumeration functions!\n");
        return err;
    }

    err = check_validation_layers(ctx, opts, &inst_props.ppEnabledLayerNames,
                                    &inst_props.enabledLayerCount, &debug_mode);
    if (err)
        goto fail;

    /* Check for present/missing extensions */
    err = check_extensions(ctx, 0, opts, &inst_props.ppEnabledExtensionNames,
                           &inst_props.enabledExtensionCount, debug_mode);
    hwctx->enabled_inst_extensions = inst_props.ppEnabledExtensionNames;
    hwctx->nb_enabled_inst_extensions = inst_props.enabledExtensionCount;
    if (err < 0)
        goto fail;

    if (debug_mode) {
        VkValidationFeatureEnableEXT feat_list[] = {
            VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
            VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
            VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
        };
        validation_features.pEnabledValidationFeatures = feat_list;
        validation_features.enabledValidationFeatureCount = FF_ARRAY_ELEMS(feat_list);
        inst_props.pNext = &validation_features;
    }

#ifdef __APPLE__
    for (int i = 0; i < inst_props.enabledExtensionCount; i++) {
        if (!strcmp(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                    inst_props.ppEnabledExtensionNames[i])) {
            inst_props.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            break;
        }
    }
#endif

    /* Try to create the instance */
    ret = vk->CreateInstance(&inst_props, hwctx->alloc, &hwctx->inst);

    /* Check for errors */
    if (ret != VK_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "Instance creation failure: %s\n",
               ff_vk_ret2str(ret));
        err = AVERROR_EXTERNAL;
        goto fail;
    }

    err = ff_vk_load_functions(ctx, vk, p->vkctx.extensions, 1, 0);
    if (err < 0) {
        av_log(ctx, AV_LOG_ERROR, "Unable to load instance functions!\n");
        goto fail;
    }

    if (debug_mode) {
        VkDebugUtilsMessengerCreateInfoEXT dbg = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = vk_dbg_callback,
            .pUserData = ctx,
        };

        vk->CreateDebugUtilsMessengerEXT(hwctx->inst, &dbg,
                                         hwctx->alloc, &p->debug_ctx);
    }

    err = 0;

fail:
    RELEASE_PROPS(inst_props.ppEnabledLayerNames, inst_props.enabledLayerCount);
    return err;
}

typedef struct VulkanDeviceSelection {
    uint8_t uuid[VK_UUID_SIZE]; /* Will use this first unless !has_uuid */
    int has_uuid;
    uint32_t drm_major; /* Will use this second unless !has_drm */
    uint32_t drm_minor; /* Will use this second unless !has_drm */
    uint32_t has_drm; /* has drm node info */
    const char *name; /* Will use this third unless NULL */
    uint32_t pci_device; /* Will use this fourth unless 0x0 */
    uint32_t vendor_id; /* Last resort to find something deterministic */
    int index; /* Finally fall back to index */
} VulkanDeviceSelection;

static const char *vk_dev_type(enum VkPhysicalDeviceType type)
{
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "integrated";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return "discrete";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return "virtual";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:            return "software";
    default:                                     return "unknown";
    }
}

/* Finds a device */
static int find_device(AVHWDeviceContext *ctx, VulkanDeviceSelection *select)
{
    int err = 0, choice = -1;
    uint32_t num;
    VkResult ret;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VkPhysicalDevice *devices = NULL;
    VkPhysicalDeviceIDProperties *idp = NULL;
    VkPhysicalDeviceProperties2 *prop = NULL;
    VkPhysicalDeviceDrmPropertiesEXT *drm_prop = NULL;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;

    ret = vk->EnumeratePhysicalDevices(hwctx->inst, &num, NULL);
    if (ret != VK_SUCCESS || !num) {
        av_log(ctx, AV_LOG_ERROR, "No devices found: %s!\n", ff_vk_ret2str(ret));
        return AVERROR(ENODEV);
    }

    devices = av_malloc_array(num, sizeof(VkPhysicalDevice));
    if (!devices)
        return AVERROR(ENOMEM);

    ret = vk->EnumeratePhysicalDevices(hwctx->inst, &num, devices);
    if (ret != VK_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "Failed enumerating devices: %s\n",
               ff_vk_ret2str(ret));
        err = AVERROR(ENODEV);
        goto end;
    }

    prop = av_calloc(num, sizeof(*prop));
    if (!prop) {
        err = AVERROR(ENOMEM);
        goto end;
    }

    idp = av_calloc(num, sizeof(*idp));
    if (!idp) {
        err = AVERROR(ENOMEM);
        goto end;
    }

    if (p->vkctx.extensions & FF_VK_EXT_DEVICE_DRM) {
        drm_prop = av_calloc(num, sizeof(*drm_prop));
        if (!drm_prop) {
            err = AVERROR(ENOMEM);
            goto end;
        }
    }

    av_log(ctx, AV_LOG_VERBOSE, "GPU listing:\n");
    for (int i = 0; i < num; i++) {
        if (p->vkctx.extensions & FF_VK_EXT_DEVICE_DRM) {
            drm_prop[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRM_PROPERTIES_EXT;
            idp[i].pNext = &drm_prop[i];
        }
        idp[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
        prop[i].sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        prop[i].pNext = &idp[i];

        vk->GetPhysicalDeviceProperties2(devices[i], &prop[i]);
        av_log(ctx, AV_LOG_VERBOSE, "    %d: %s (%s) (0x%x)\n", i,
               prop[i].properties.deviceName,
               vk_dev_type(prop[i].properties.deviceType),
               prop[i].properties.deviceID);
    }

    if (select->has_uuid) {
        for (int i = 0; i < num; i++) {
            if (!strncmp(idp[i].deviceUUID, select->uuid, VK_UUID_SIZE)) {
                choice = i;
                goto end;
             }
        }
        av_log(ctx, AV_LOG_ERROR, "Unable to find device by given UUID!\n");
        err = AVERROR(ENODEV);
        goto end;
    } else if ((p->vkctx.extensions & FF_VK_EXT_DEVICE_DRM) && select->has_drm) {
        for (int i = 0; i < num; i++) {
            if ((select->drm_major == drm_prop[i].primaryMajor &&
                 select->drm_minor == drm_prop[i].primaryMinor) ||
                (select->drm_major == drm_prop[i].renderMajor &&
                 select->drm_minor == drm_prop[i].renderMinor)) {
                choice = i;
                goto end;
             }
        }
        av_log(ctx, AV_LOG_ERROR, "Unable to find device by given DRM node numbers %i:%i!\n",
               select->drm_major, select->drm_minor);
        err = AVERROR(ENODEV);
        goto end;
    } else if (select->name) {
        av_log(ctx, AV_LOG_VERBOSE, "Requested device: %s\n", select->name);
        for (int i = 0; i < num; i++) {
            if (strstr(prop[i].properties.deviceName, select->name)) {
                choice = i;
                goto end;
             }
        }
        av_log(ctx, AV_LOG_ERROR, "Unable to find device \"%s\"!\n",
               select->name);
        err = AVERROR(ENODEV);
        goto end;
    } else if (select->pci_device) {
        av_log(ctx, AV_LOG_VERBOSE, "Requested device: 0x%x\n", select->pci_device);
        for (int i = 0; i < num; i++) {
            if (select->pci_device == prop[i].properties.deviceID) {
                choice = i;
                goto end;
            }
        }
        av_log(ctx, AV_LOG_ERROR, "Unable to find device with PCI ID 0x%x!\n",
               select->pci_device);
        err = AVERROR(EINVAL);
        goto end;
    } else if (select->vendor_id) {
        av_log(ctx, AV_LOG_VERBOSE, "Requested vendor: 0x%x\n", select->vendor_id);
        for (int i = 0; i < num; i++) {
            if (select->vendor_id == prop[i].properties.vendorID) {
                choice = i;
                goto end;
            }
        }
        av_log(ctx, AV_LOG_ERROR, "Unable to find device with Vendor ID 0x%x!\n",
               select->vendor_id);
        err = AVERROR(ENODEV);
        goto end;
    } else {
        if (select->index < num) {
            choice = select->index;
            goto end;
        }
        av_log(ctx, AV_LOG_ERROR, "Unable to find device with index %i!\n",
               select->index);
        err = AVERROR(ENODEV);
        goto end;
    }

end:
    if (choice > -1) {
        av_log(ctx, AV_LOG_VERBOSE, "Device %d selected: %s (%s) (0x%x)\n",
               choice, prop[choice].properties.deviceName,
               vk_dev_type(prop[choice].properties.deviceType),
               prop[choice].properties.deviceID);
        hwctx->phys_dev = devices[choice];
    }

    av_free(devices);
    av_free(prop);
    av_free(idp);
    av_free(drm_prop);

    return err;
}

/* Picks the least used qf with the fewest unneeded flags, or -1 if none found */
static inline int pick_queue_family(VkQueueFamilyProperties *qf, uint32_t num_qf,
                                    VkQueueFlagBits flags)
{
    int index = -1;
    uint32_t min_score = UINT32_MAX;

    for (int i = 0; i < num_qf; i++) {
        const VkQueueFlagBits qflags = qf[i].queueFlags;
        if (qflags & flags) {
            uint32_t score = av_popcount(qflags) + qf[i].timestampValidBits;
            if (score < min_score) {
                index = i;
                min_score = score;
            }
        }
    }

    if (index > -1)
        qf[index].timestampValidBits++;

    return index;
}

static int setup_queue_families(AVHWDeviceContext *ctx, VkDeviceCreateInfo *cd)
{
    uint32_t num;
    float *weights;
    VkQueueFamilyProperties *qf = NULL;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    int graph_index, comp_index, tx_index, enc_index, dec_index;

    /* First get the number of queue families */
    vk->GetPhysicalDeviceQueueFamilyProperties(hwctx->phys_dev, &num, NULL);
    if (!num) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get queues!\n");
        return AVERROR_EXTERNAL;
    }

    /* Then allocate memory */
    qf = av_malloc_array(num, sizeof(VkQueueFamilyProperties));
    if (!qf)
        return AVERROR(ENOMEM);

    /* Finally retrieve the queue families */
    vk->GetPhysicalDeviceQueueFamilyProperties(hwctx->phys_dev, &num, qf);

    av_log(ctx, AV_LOG_VERBOSE, "Queue families:\n");
    for (int i = 0; i < num; i++) {
        av_log(ctx, AV_LOG_VERBOSE, "    %i:%s%s%s%s%s%s%s (queues: %i)\n", i,
               ((qf[i].queueFlags) & VK_QUEUE_GRAPHICS_BIT) ? " graphics" : "",
               ((qf[i].queueFlags) & VK_QUEUE_COMPUTE_BIT) ? " compute" : "",
               ((qf[i].queueFlags) & VK_QUEUE_TRANSFER_BIT) ? " transfer" : "",
               ((qf[i].queueFlags) & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) ? " encode" : "",
               ((qf[i].queueFlags) & VK_QUEUE_VIDEO_DECODE_BIT_KHR) ? " decode" : "",
               ((qf[i].queueFlags) & VK_QUEUE_SPARSE_BINDING_BIT) ? " sparse" : "",
               ((qf[i].queueFlags) & VK_QUEUE_PROTECTED_BIT) ? " protected" : "",
               qf[i].queueCount);

        /* We use this field to keep a score of how many times we've used that
         * queue family in order to make better choices. */
        qf[i].timestampValidBits = 0;
    }

    /* Pick each queue family to use */
    graph_index = pick_queue_family(qf, num, VK_QUEUE_GRAPHICS_BIT);
    comp_index  = pick_queue_family(qf, num, VK_QUEUE_COMPUTE_BIT);
    tx_index    = pick_queue_family(qf, num, VK_QUEUE_TRANSFER_BIT);
    enc_index   = pick_queue_family(qf, num, VK_QUEUE_VIDEO_ENCODE_BIT_KHR);
    dec_index   = pick_queue_family(qf, num, VK_QUEUE_VIDEO_DECODE_BIT_KHR);

    /* Signalling the transfer capabilities on a queue family is optional */
    if (tx_index < 0) {
        tx_index = pick_queue_family(qf, num, VK_QUEUE_COMPUTE_BIT);
        if (tx_index < 0)
            tx_index = pick_queue_family(qf, num, VK_QUEUE_GRAPHICS_BIT);
    }

    hwctx->queue_family_index        = -1;
    hwctx->queue_family_comp_index   = -1;
    hwctx->queue_family_tx_index     = -1;
    hwctx->queue_family_encode_index = -1;
    hwctx->queue_family_decode_index = -1;

#define SETUP_QUEUE(qf_idx)                                                    \
    if (qf_idx > -1) {                                                         \
        int fidx = qf_idx;                                                     \
        int qc = qf[fidx].queueCount;                                          \
        VkDeviceQueueCreateInfo *pc;                                           \
                                                                               \
        if (fidx == graph_index) {                                             \
            hwctx->queue_family_index = fidx;                                  \
            hwctx->nb_graphics_queues = qc;                                    \
            graph_index = -1;                                                  \
        }                                                                      \
        if (fidx == comp_index) {                                              \
            hwctx->queue_family_comp_index = fidx;                             \
            hwctx->nb_comp_queues = qc;                                        \
            comp_index = -1;                                                   \
        }                                                                      \
        if (fidx == tx_index) {                                                \
            hwctx->queue_family_tx_index = fidx;                               \
            hwctx->nb_tx_queues = qc;                                          \
            tx_index = -1;                                                     \
        }                                                                      \
        if (fidx == enc_index) {                                               \
            hwctx->queue_family_encode_index = fidx;                           \
            hwctx->nb_encode_queues = qc;                                      \
            enc_index = -1;                                                    \
        }                                                                      \
        if (fidx == dec_index) {                                               \
            hwctx->queue_family_decode_index = fidx;                           \
            hwctx->nb_decode_queues = qc;                                      \
            dec_index = -1;                                                    \
        }                                                                      \
                                                                               \
        pc = av_realloc((void *)cd->pQueueCreateInfos,                         \
                        sizeof(*pc) * (cd->queueCreateInfoCount + 1));         \
        if (!pc) {                                                             \
            av_free(qf);                                                       \
            return AVERROR(ENOMEM);                                            \
        }                                                                      \
        cd->pQueueCreateInfos = pc;                                            \
        pc = &pc[cd->queueCreateInfoCount];                                    \
                                                                               \
        weights = av_malloc(qc * sizeof(float));                               \
        if (!weights) {                                                        \
            av_free(qf);                                                       \
            return AVERROR(ENOMEM);                                            \
        }                                                                      \
                                                                               \
        memset(pc, 0, sizeof(*pc));                                            \
        pc->sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;     \
        pc->queueFamilyIndex = fidx;                                           \
        pc->queueCount       = qc;                                             \
        pc->pQueuePriorities = weights;                                        \
                                                                               \
        for (int i = 0; i < qc; i++)                                           \
            weights[i] = 1.0f / qc;                                            \
                                                                               \
        cd->queueCreateInfoCount++;                                            \
    }

    SETUP_QUEUE(graph_index)
    SETUP_QUEUE(comp_index)
    SETUP_QUEUE(tx_index)
    SETUP_QUEUE(enc_index)
    SETUP_QUEUE(dec_index)

#undef SETUP_QUEUE

    av_free(qf);

    return 0;
}

/* Only resources created by vulkan_device_create should be released here,
 * resources created by vulkan_device_init should be released by
 * vulkan_device_uninit, to make sure we don't free user provided resources,
 * and there is no leak.
 */
static void vulkan_device_free(AVHWDeviceContext *ctx)
{
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;

    if (hwctx->act_dev)
        vk->DestroyDevice(hwctx->act_dev, hwctx->alloc);

    if (p->debug_ctx)
        vk->DestroyDebugUtilsMessengerEXT(hwctx->inst, p->debug_ctx,
                                          hwctx->alloc);

    if (hwctx->inst)
        vk->DestroyInstance(hwctx->inst, hwctx->alloc);

    if (p->libvulkan)
        dlclose(p->libvulkan);

    RELEASE_PROPS(hwctx->enabled_inst_extensions, hwctx->nb_enabled_inst_extensions);
    RELEASE_PROPS(hwctx->enabled_dev_extensions, hwctx->nb_enabled_dev_extensions);
}

static void vulkan_device_uninit(AVHWDeviceContext *ctx)
{
    VulkanDevicePriv *p = ctx->internal->priv;

    for (uint32_t i = 0; i < p->nb_tot_qfs; i++) {
        pthread_mutex_destroy(p->qf_mutex[i]);
        av_freep(&p->qf_mutex[i]);
    }
    av_freep(&p->qf_mutex);

    ff_vk_uninit(&p->vkctx);
}

static int vulkan_device_create_internal(AVHWDeviceContext *ctx,
                                         VulkanDeviceSelection *dev_select,
                                         int disable_multiplane,
                                         AVDictionary *opts, int flags)
{
    int err = 0;
    VkResult ret;
    AVDictionaryEntry *opt_d;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;

    /*
     * VkPhysicalDeviceVulkan12Features has a timelineSemaphore field, but
     * MoltenVK doesn't implement VkPhysicalDeviceVulkan12Features yet, so we
     * use VkPhysicalDeviceTimelineSemaphoreFeatures directly.
     */
    VkPhysicalDeviceTimelineSemaphoreFeatures timeline_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
    };
    VkPhysicalDeviceCooperativeMatrixFeaturesKHR coop_matrix_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR,
        .pNext = &timeline_features,
    };
    VkPhysicalDeviceShaderAtomicFloatFeaturesEXT atomic_float_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT,
        .pNext = &coop_matrix_features,
    };
    VkPhysicalDeviceDescriptorBufferFeaturesEXT desc_buf_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT,
        .pNext = &atomic_float_features,
    };
    VkPhysicalDeviceVulkan13Features dev_features_1_3 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &desc_buf_features,
    };
    VkPhysicalDeviceVulkan12Features dev_features_1_2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &dev_features_1_3,
    };
    VkPhysicalDeviceVulkan11Features dev_features_1_1 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &dev_features_1_2,
    };
    VkPhysicalDeviceFeatures2 dev_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &dev_features_1_1,
    };

    VkDeviceCreateInfo dev_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    };

    hwctx->device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    hwctx->device_features.pNext = &p->device_features_1_1;
    p->device_features_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    p->device_features_1_1.pNext = &p->device_features_1_2;
    p->device_features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    p->device_features_1_2.pNext = &p->device_features_1_3;
    p->device_features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    p->device_features_1_3.pNext = &p->desc_buf_features;
    p->desc_buf_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
    p->desc_buf_features.pNext = &p->atomic_float_features;
    p->atomic_float_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
    p->atomic_float_features.pNext = &p->coop_matrix_features;
    p->coop_matrix_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR;
    p->coop_matrix_features.pNext = NULL;

    ctx->free = vulkan_device_free;

    /* Create an instance if not given one */
    if ((err = create_instance(ctx, opts)))
        goto end;

    /* Find a device (if not given one) */
    if ((err = find_device(ctx, dev_select)))
        goto end;

    vk->GetPhysicalDeviceFeatures2(hwctx->phys_dev, &dev_features);

    /* Try to keep in sync with libplacebo */
#define COPY_FEATURE(DST, NAME) (DST).features.NAME = dev_features.features.NAME;
    COPY_FEATURE(hwctx->device_features, shaderImageGatherExtended)
    COPY_FEATURE(hwctx->device_features, shaderStorageImageReadWithoutFormat)
    COPY_FEATURE(hwctx->device_features, shaderStorageImageWriteWithoutFormat)
    COPY_FEATURE(hwctx->device_features, fragmentStoresAndAtomics)
    COPY_FEATURE(hwctx->device_features, vertexPipelineStoresAndAtomics)
    COPY_FEATURE(hwctx->device_features, shaderInt64)
    COPY_FEATURE(hwctx->device_features, shaderInt16)
    COPY_FEATURE(hwctx->device_features, shaderFloat64)
#undef COPY_FEATURE

    /* We require timeline semaphores */
    if (!timeline_features.timelineSemaphore) {
        av_log(ctx, AV_LOG_ERROR, "Device does not support timeline semaphores!\n");
        err = AVERROR(ENOSYS);
        goto end;
    }

    p->device_features_1_1.samplerYcbcrConversion = dev_features_1_1.samplerYcbcrConversion;
    p->device_features_1_1.storagePushConstant16 = dev_features_1_1.storagePushConstant16;

    p->device_features_1_2.timelineSemaphore = 1;
    p->device_features_1_2.bufferDeviceAddress = dev_features_1_2.bufferDeviceAddress;
    p->device_features_1_2.hostQueryReset = dev_features_1_2.hostQueryReset;
    p->device_features_1_2.storagePushConstant8 = dev_features_1_2.storagePushConstant8;
    p->device_features_1_2.shaderInt8 = dev_features_1_2.shaderInt8;
    p->device_features_1_2.storageBuffer8BitAccess = dev_features_1_2.storageBuffer8BitAccess;
    p->device_features_1_2.uniformAndStorageBuffer8BitAccess = dev_features_1_2.uniformAndStorageBuffer8BitAccess;
    p->device_features_1_2.shaderFloat16 = dev_features_1_2.shaderFloat16;
    p->device_features_1_2.shaderSharedInt64Atomics = dev_features_1_2.shaderSharedInt64Atomics;
    p->device_features_1_2.vulkanMemoryModel = dev_features_1_2.vulkanMemoryModel;
    p->device_features_1_2.vulkanMemoryModelDeviceScope = dev_features_1_2.vulkanMemoryModelDeviceScope;
    p->device_features_1_2.hostQueryReset = dev_features_1_2.hostQueryReset;

    p->device_features_1_3.dynamicRendering = dev_features_1_3.dynamicRendering;
    p->device_features_1_3.maintenance4 = dev_features_1_3.maintenance4;
    p->device_features_1_3.synchronization2 = dev_features_1_3.synchronization2;
    p->device_features_1_3.computeFullSubgroups = dev_features_1_3.computeFullSubgroups;
    p->device_features_1_3.shaderZeroInitializeWorkgroupMemory = dev_features_1_3.shaderZeroInitializeWorkgroupMemory;
    p->device_features_1_3.dynamicRendering = dev_features_1_3.dynamicRendering;

    p->desc_buf_features.descriptorBuffer = desc_buf_features.descriptorBuffer;
    p->desc_buf_features.descriptorBufferPushDescriptors = desc_buf_features.descriptorBufferPushDescriptors;

    p->atomic_float_features.shaderBufferFloat32Atomics = atomic_float_features.shaderBufferFloat32Atomics;
    p->atomic_float_features.shaderBufferFloat32AtomicAdd = atomic_float_features.shaderBufferFloat32AtomicAdd;

    p->coop_matrix_features.cooperativeMatrix = coop_matrix_features.cooperativeMatrix;

    dev_info.pNext = &hwctx->device_features;

    /* Setup queue family */
    if ((err = setup_queue_families(ctx, &dev_info)))
        goto end;

    if ((err = check_extensions(ctx, 1, opts, &dev_info.ppEnabledExtensionNames,
                                &dev_info.enabledExtensionCount, 0))) {
        for (int i = 0; i < dev_info.queueCreateInfoCount; i++)
            av_free((void *)dev_info.pQueueCreateInfos[i].pQueuePriorities);
        av_free((void *)dev_info.pQueueCreateInfos);
        goto end;
    }

    ret = vk->CreateDevice(hwctx->phys_dev, &dev_info, hwctx->alloc,
                           &hwctx->act_dev);

    for (int i = 0; i < dev_info.queueCreateInfoCount; i++)
        av_free((void *)dev_info.pQueueCreateInfos[i].pQueuePriorities);
    av_free((void *)dev_info.pQueueCreateInfos);

    if (ret != VK_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "Device creation failure: %s\n",
               ff_vk_ret2str(ret));
        for (int i = 0; i < dev_info.enabledExtensionCount; i++)
            av_free((void *)dev_info.ppEnabledExtensionNames[i]);
        av_free((void *)dev_info.ppEnabledExtensionNames);
        err = AVERROR_EXTERNAL;
        goto end;
    }

    /* Tiled images setting, use them by default */
    opt_d = av_dict_get(opts, "linear_images", NULL, 0);
    if (opt_d)
        p->use_linear_images = strtol(opt_d->value, NULL, 10);

    /*
     * The disable_multiplane argument takes precedent over the option.
     */
    p->disable_multiplane = disable_multiplane;
    if (!p->disable_multiplane) {
        opt_d = av_dict_get(opts, "disable_multiplane", NULL, 0);
        if (opt_d)
            p->disable_multiplane = strtol(opt_d->value, NULL, 10);
    }

    hwctx->enabled_dev_extensions = dev_info.ppEnabledExtensionNames;
    hwctx->nb_enabled_dev_extensions = dev_info.enabledExtensionCount;

end:
    return err;
}

static void lock_queue(AVHWDeviceContext *ctx, uint32_t queue_family, uint32_t index)
{
    VulkanDevicePriv *p = ctx->internal->priv;
    pthread_mutex_lock(&p->qf_mutex[queue_family][index]);
}

static void unlock_queue(AVHWDeviceContext *ctx, uint32_t queue_family, uint32_t index)
{
    VulkanDevicePriv *p = ctx->internal->priv;
    pthread_mutex_unlock(&p->qf_mutex[queue_family][index]);
}

static int vulkan_device_init(AVHWDeviceContext *ctx)
{
    int err;
    uint32_t qf_num;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VkQueueFamilyProperties *qf;
    int graph_index, comp_index, tx_index, enc_index, dec_index;

    /* Set device extension flags */
    for (int i = 0; i < hwctx->nb_enabled_dev_extensions; i++) {
        for (int j = 0; j < FF_ARRAY_ELEMS(optional_device_exts); j++) {
            if (!strcmp(hwctx->enabled_dev_extensions[i],
                        optional_device_exts[j].name)) {
                p->vkctx.extensions |= optional_device_exts[j].flag;
                break;
            }
        }
    }

    err = ff_vk_load_functions(ctx, vk, p->vkctx.extensions, 1, 1);
    if (err < 0) {
        av_log(ctx, AV_LOG_ERROR, "Unable to load functions!\n");
        return err;
    }

    p->props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    p->props.pNext = &p->hprops;
    p->hprops.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT;

    vk->GetPhysicalDeviceProperties2(hwctx->phys_dev, &p->props);
    av_log(ctx, AV_LOG_VERBOSE, "Using device: %s\n",
           p->props.properties.deviceName);
    av_log(ctx, AV_LOG_VERBOSE, "Alignments:\n");
    av_log(ctx, AV_LOG_VERBOSE, "    optimalBufferCopyRowPitchAlignment: %"PRIu64"\n",
           p->props.properties.limits.optimalBufferCopyRowPitchAlignment);
    av_log(ctx, AV_LOG_VERBOSE, "    minMemoryMapAlignment:              %"SIZE_SPECIFIER"\n",
           p->props.properties.limits.minMemoryMapAlignment);
    av_log(ctx, AV_LOG_VERBOSE, "    nonCoherentAtomSize:                %"PRIu64"\n",
           p->props.properties.limits.nonCoherentAtomSize);
    if (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_HOST_MEMORY)
        av_log(ctx, AV_LOG_VERBOSE, "    minImportedHostPointerAlignment:    %"PRIu64"\n",
               p->hprops.minImportedHostPointerAlignment);

    p->dev_is_nvidia = (p->props.properties.vendorID == 0x10de);

    vk->GetPhysicalDeviceQueueFamilyProperties(hwctx->phys_dev, &qf_num, NULL);
    if (!qf_num) {
        av_log(ctx, AV_LOG_ERROR, "Failed to get queues!\n");
        return AVERROR_EXTERNAL;
    }

    qf = av_malloc_array(qf_num, sizeof(VkQueueFamilyProperties));
    if (!qf)
        return AVERROR(ENOMEM);

    vk->GetPhysicalDeviceQueueFamilyProperties(hwctx->phys_dev, &qf_num, qf);

    p->qf_mutex = av_calloc(qf_num, sizeof(*p->qf_mutex));
    if (!p->qf_mutex) {
        av_free(qf);
        return AVERROR(ENOMEM);
    }
    p->nb_tot_qfs = qf_num;

    for (uint32_t i = 0; i < qf_num; i++) {
        p->qf_mutex[i] = av_calloc(qf[i].queueCount, sizeof(**p->qf_mutex));
        if (!p->qf_mutex[i]) {
            av_free(qf);
            return AVERROR(ENOMEM);
        }
        for (uint32_t j = 0; j < qf[i].queueCount; j++) {
            err = pthread_mutex_init(&p->qf_mutex[i][j], NULL);
            if (err != 0) {
                av_log(ctx, AV_LOG_ERROR, "pthread_mutex_init failed : %s\n",
                       av_err2str(err));
                av_free(qf);
                return AVERROR(err);
            }
        }
    }

    av_free(qf);

    graph_index = hwctx->nb_graphics_queues ? hwctx->queue_family_index : -1;
    comp_index  = hwctx->nb_comp_queues ? hwctx->queue_family_comp_index : -1;
    tx_index    = hwctx->nb_tx_queues ? hwctx->queue_family_tx_index : -1;
    dec_index   = hwctx->nb_decode_queues ? hwctx->queue_family_decode_index : -1;
    enc_index   = hwctx->nb_encode_queues ? hwctx->queue_family_encode_index : -1;

#define CHECK_QUEUE(type, required, fidx, ctx_qf, qc)                                           \
    do {                                                                                        \
        if (ctx_qf < 0 && required) {                                                           \
            av_log(ctx, AV_LOG_ERROR, "%s queue family is required, but marked as missing"      \
                   " in the context!\n", type);                                                 \
            return AVERROR(EINVAL);                                                             \
        } else if (fidx < 0 || ctx_qf < 0) {                                                    \
            break;                                                                              \
        } else if (ctx_qf >= qf_num) {                                                          \
            av_log(ctx, AV_LOG_ERROR, "Invalid %s family index %i (device has %i families)!\n", \
                   type, ctx_qf, qf_num);                                                       \
            return AVERROR(EINVAL);                                                             \
        }                                                                                       \
                                                                                                \
        av_log(ctx, AV_LOG_VERBOSE, "Using queue family %i (queues: %i)"                        \
               " for%s%s%s%s%s\n",                                                              \
               ctx_qf, qc,                                                                      \
               ctx_qf == graph_index ? " graphics" : "",                                        \
               ctx_qf == comp_index  ? " compute" : "",                                         \
               ctx_qf == tx_index    ? " transfers" : "",                                       \
               ctx_qf == enc_index   ? " encode" : "",                                          \
               ctx_qf == dec_index   ? " decode" : "");                                         \
        graph_index = (ctx_qf == graph_index) ? -1 : graph_index;                               \
        comp_index  = (ctx_qf == comp_index)  ? -1 : comp_index;                                \
        tx_index    = (ctx_qf == tx_index)    ? -1 : tx_index;                                  \
        enc_index   = (ctx_qf == enc_index)   ? -1 : enc_index;                                 \
        dec_index   = (ctx_qf == dec_index)   ? -1 : dec_index;                                 \
        p->img_qfs[p->nb_img_qfs++] = ctx_qf;                                                   \
    } while (0)

    CHECK_QUEUE("graphics", 0, graph_index, hwctx->queue_family_index,        hwctx->nb_graphics_queues);
    CHECK_QUEUE("compute",  1, comp_index,  hwctx->queue_family_comp_index,   hwctx->nb_comp_queues);
    CHECK_QUEUE("upload",   1, tx_index,    hwctx->queue_family_tx_index,     hwctx->nb_tx_queues);
    CHECK_QUEUE("decode",   0, dec_index,   hwctx->queue_family_decode_index, hwctx->nb_decode_queues);
    CHECK_QUEUE("encode",   0, enc_index,   hwctx->queue_family_encode_index, hwctx->nb_encode_queues);

#undef CHECK_QUEUE

    if (!hwctx->lock_queue)
        hwctx->lock_queue = lock_queue;
    if (!hwctx->unlock_queue)
        hwctx->unlock_queue = unlock_queue;

    /* Get device capabilities */
    vk->GetPhysicalDeviceMemoryProperties(hwctx->phys_dev, &p->mprops);

    p->vkctx.device = ctx;
    p->vkctx.hwctx = hwctx;

    ff_vk_load_props(&p->vkctx);
    ff_vk_qf_init(&p->vkctx, &p->compute_qf, VK_QUEUE_COMPUTE_BIT);
    ff_vk_qf_init(&p->vkctx, &p->transfer_qf, VK_QUEUE_TRANSFER_BIT);

    return 0;
}

static int vulkan_device_create(AVHWDeviceContext *ctx, const char *device,
                                AVDictionary *opts, int flags)
{
    VulkanDeviceSelection dev_select = { 0 };
    if (device && device[0]) {
        char *end = NULL;
        dev_select.index = strtol(device, &end, 10);
        if (end == device) {
            dev_select.index = 0;
            dev_select.name  = device;
        }
    }

    return vulkan_device_create_internal(ctx, &dev_select, 0, opts, flags);
}

static int vulkan_device_derive(AVHWDeviceContext *ctx,
                                AVHWDeviceContext *src_ctx,
                                AVDictionary *opts, int flags)
{
    av_unused VulkanDeviceSelection dev_select = { 0 };

    /* If there's only one device on the system, then even if its not covered
     * by the following checks (e.g. non-PCIe ARM GPU), having an empty
     * dev_select will mean it'll get picked. */
    switch(src_ctx->type) {
#if CONFIG_VAAPI
    case AV_HWDEVICE_TYPE_VAAPI: {
        AVVAAPIDeviceContext *src_hwctx = src_ctx->hwctx;

        const char *vendor = vaQueryVendorString(src_hwctx->display);
        if (!vendor) {
            av_log(ctx, AV_LOG_ERROR, "Unable to get device info from VAAPI!\n");
            return AVERROR_EXTERNAL;
        }

        if (strstr(vendor, "AMD"))
            dev_select.vendor_id = 0x1002;

        return vulkan_device_create_internal(ctx, &dev_select, 0, opts, flags);
    }
#endif
#if CONFIG_LIBDRM
    case AV_HWDEVICE_TYPE_DRM: {
        int err;
        struct stat drm_node_info;
        drmDevice *drm_dev_info;
        AVDRMDeviceContext *src_hwctx = src_ctx->hwctx;

        err = fstat(src_hwctx->fd, &drm_node_info);
        if (err) {
            av_log(ctx, AV_LOG_ERROR, "Unable to get node info from DRM fd: %s!\n",
                   av_err2str(AVERROR(errno)));
            return AVERROR_EXTERNAL;
        }

        dev_select.drm_major = major(drm_node_info.st_dev);
        dev_select.drm_minor = minor(drm_node_info.st_dev);
        dev_select.has_drm   = 1;

        err = drmGetDevice(src_hwctx->fd, &drm_dev_info);
        if (err) {
            av_log(ctx, AV_LOG_ERROR, "Unable to get device info from DRM fd: %s!\n",
                   av_err2str(AVERROR(errno)));
            return AVERROR_EXTERNAL;
        }

        if (drm_dev_info->bustype == DRM_BUS_PCI)
            dev_select.pci_device = drm_dev_info->deviceinfo.pci->device_id;

        drmFreeDevice(&drm_dev_info);

        return vulkan_device_create_internal(ctx, &dev_select, 0, opts, flags);
    }
#endif
#if CONFIG_CUDA
    case AV_HWDEVICE_TYPE_CUDA: {
        AVHWDeviceContext *cuda_cu = src_ctx;
        AVCUDADeviceContext *src_hwctx = src_ctx->hwctx;
        AVCUDADeviceContextInternal *cu_internal = src_hwctx->internal;
        CudaFunctions *cu = cu_internal->cuda_dl;

        int ret = CHECK_CU(cu->cuDeviceGetUuid((CUuuid *)&dev_select.uuid,
                                               cu_internal->cuda_device));
        if (ret < 0) {
            av_log(ctx, AV_LOG_ERROR, "Unable to get UUID from CUDA!\n");
            return AVERROR_EXTERNAL;
        }

        dev_select.has_uuid = 1;

        /*
         * CUDA is not able to import multiplane images, so always derive a
         * Vulkan device with multiplane disabled.
         */
        return vulkan_device_create_internal(ctx, &dev_select, 1, opts, flags);
    }
#endif
    default:
        return AVERROR(ENOSYS);
    }
}

static int vulkan_frames_get_constraints(AVHWDeviceContext *ctx,
                                         const void *hwconfig,
                                         AVHWFramesConstraints *constraints)
{
    int count = 0;
    VulkanDevicePriv *p = ctx->internal->priv;

    for (enum AVPixelFormat i = 0; i < nb_vk_formats_list; i++) {
        count += vkfmt_from_pixfmt2(ctx, vk_formats_list[i].pixfmt,
                                    p->use_linear_images ? VK_IMAGE_TILING_LINEAR :
                                                           VK_IMAGE_TILING_OPTIMAL,
                                    NULL, NULL, NULL, NULL, 0, 0) >= 0;
    }

    constraints->valid_sw_formats = av_malloc_array(count + 1,
                                                    sizeof(enum AVPixelFormat));
    if (!constraints->valid_sw_formats)
        return AVERROR(ENOMEM);

    count = 0;
    for (enum AVPixelFormat i = 0; i < nb_vk_formats_list; i++) {
        if (vkfmt_from_pixfmt2(ctx, vk_formats_list[i].pixfmt,
                               p->use_linear_images ? VK_IMAGE_TILING_LINEAR :
                                                      VK_IMAGE_TILING_OPTIMAL,
                               NULL, NULL, NULL, NULL, 0, 0) >= 0) {
            constraints->valid_sw_formats[count++] = vk_formats_list[i].pixfmt;
        }
    }

    constraints->valid_sw_formats[count++] = AV_PIX_FMT_NONE;

    constraints->min_width  = 1;
    constraints->min_height = 1;
    constraints->max_width  = p->props.properties.limits.maxImageDimension2D;
    constraints->max_height = p->props.properties.limits.maxImageDimension2D;

    constraints->valid_hw_formats = av_malloc_array(2, sizeof(enum AVPixelFormat));
    if (!constraints->valid_hw_formats)
        return AVERROR(ENOMEM);

    constraints->valid_hw_formats[0] = AV_PIX_FMT_VULKAN;
    constraints->valid_hw_formats[1] = AV_PIX_FMT_NONE;

    return 0;
}

static int alloc_mem(AVHWDeviceContext *ctx, VkMemoryRequirements *req,
                     VkMemoryPropertyFlagBits req_flags, const void *alloc_extension,
                     VkMemoryPropertyFlagBits *mem_flags, VkDeviceMemory *mem)
{
    VkResult ret;
    int index = -1;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *dev_hwctx = ctx->hwctx;
    VkMemoryAllocateInfo alloc_info = {
        .sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext          = alloc_extension,
        .allocationSize = req->size,
    };

    /* The vulkan spec requires memory types to be sorted in the "optimal"
     * order, so the first matching type we find will be the best/fastest one */
    for (int i = 0; i < p->mprops.memoryTypeCount; i++) {
        const VkMemoryType *type = &p->mprops.memoryTypes[i];

        /* The memory type must be supported by the requirements (bitfield) */
        if (!(req->memoryTypeBits & (1 << i)))
            continue;

        /* The memory type flags must include our properties */
        if ((type->propertyFlags & req_flags) != req_flags)
            continue;

        /* The memory type must be large enough */
        if (req->size > p->mprops.memoryHeaps[type->heapIndex].size)
            continue;

        /* Found a suitable memory type */
        index = i;
        break;
    }

    if (index < 0) {
        av_log(ctx, AV_LOG_ERROR, "No memory type found for flags 0x%x\n",
               req_flags);
        return AVERROR(EINVAL);
    }

    alloc_info.memoryTypeIndex = index;

    ret = vk->AllocateMemory(dev_hwctx->act_dev, &alloc_info,
                             dev_hwctx->alloc, mem);
    if (ret != VK_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "Failed to allocate memory: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR(ENOMEM);
    }

    *mem_flags |= p->mprops.memoryTypes[index].propertyFlags;

    return 0;
}

static void vulkan_free_internal(AVVkFrame *f)
{
    av_unused AVVkFrameInternal *internal = f->internal;

#if CONFIG_CUDA
    if (internal->cuda_fc_ref) {
        AVHWFramesContext *cuda_fc = (AVHWFramesContext *)internal->cuda_fc_ref->data;
        int planes = av_pix_fmt_count_planes(cuda_fc->sw_format);
        AVHWDeviceContext *cuda_cu = cuda_fc->device_ctx;
        AVCUDADeviceContext *cuda_dev = cuda_cu->hwctx;
        AVCUDADeviceContextInternal *cu_internal = cuda_dev->internal;
        CudaFunctions *cu = cu_internal->cuda_dl;

        for (int i = 0; i < planes; i++) {
            if (internal->cu_sem[i])
                CHECK_CU(cu->cuDestroyExternalSemaphore(internal->cu_sem[i]));
            if (internal->cu_mma[i])
                CHECK_CU(cu->cuMipmappedArrayDestroy(internal->cu_mma[i]));
            if (internal->ext_mem[i])
                CHECK_CU(cu->cuDestroyExternalMemory(internal->ext_mem[i]));
#ifdef _WIN32
            if (internal->ext_sem_handle[i])
                CloseHandle(internal->ext_sem_handle[i]);
            if (internal->ext_mem_handle[i])
                CloseHandle(internal->ext_mem_handle[i]);
#endif
        }

        av_buffer_unref(&internal->cuda_fc_ref);
    }
#endif

    pthread_mutex_destroy(&internal->update_mutex);
    av_freep(&f->internal);
}

static void vulkan_frame_free(AVHWFramesContext *hwfc, AVVkFrame *f)
{
    AVVulkanDeviceContext *hwctx = hwfc->device_ctx->hwctx;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    int nb_images = ff_vk_count_images(f);

    VkSemaphoreWaitInfo sem_wait = {
        .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .flags          = 0x0,
        .pSemaphores    = f->sem,
        .pValues        = f->sem_value,
        .semaphoreCount = nb_images,
    };

    vk->WaitSemaphores(hwctx->act_dev, &sem_wait, UINT64_MAX);

    vulkan_free_internal(f);

    for (int i = 0; i < nb_images; i++) {
        vk->DestroyImage(hwctx->act_dev,     f->img[i], hwctx->alloc);
        vk->FreeMemory(hwctx->act_dev,       f->mem[i], hwctx->alloc);
        vk->DestroySemaphore(hwctx->act_dev, f->sem[i], hwctx->alloc);
    }

    av_free(f);
}

static void vulkan_frame_free_cb(void *opaque, uint8_t *data)
{
    vulkan_frame_free(opaque, (AVVkFrame*)data);
}

static int alloc_bind_mem(AVHWFramesContext *hwfc, AVVkFrame *f,
                          void *alloc_pnext, size_t alloc_pnext_stride)
{
    int img_cnt = 0, err;
    VkResult ret;
    AVHWDeviceContext *ctx = hwfc->device_ctx;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VkBindImageMemoryInfo bind_info[AV_NUM_DATA_POINTERS] = { { 0 } };

    AVVulkanDeviceContext *hwctx = ctx->hwctx;

    while (f->img[img_cnt]) {
        int use_ded_mem;
        VkImageMemoryRequirementsInfo2 req_desc = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
            .image = f->img[img_cnt],
        };
        VkMemoryDedicatedAllocateInfo ded_alloc = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
            .pNext = (void *)(((uint8_t *)alloc_pnext) + img_cnt*alloc_pnext_stride),
        };
        VkMemoryDedicatedRequirements ded_req = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
        };
        VkMemoryRequirements2 req = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
            .pNext = &ded_req,
        };

        vk->GetImageMemoryRequirements2(hwctx->act_dev, &req_desc, &req);

        if (f->tiling == VK_IMAGE_TILING_LINEAR)
            req.memoryRequirements.size = FFALIGN(req.memoryRequirements.size,
                                                  p->props.properties.limits.minMemoryMapAlignment);

        /* In case the implementation prefers/requires dedicated allocation */
        use_ded_mem = ded_req.prefersDedicatedAllocation |
                      ded_req.requiresDedicatedAllocation;
        if (use_ded_mem)
            ded_alloc.image = f->img[img_cnt];

        /* Allocate memory */
        if ((err = alloc_mem(ctx, &req.memoryRequirements,
                             f->tiling == VK_IMAGE_TILING_LINEAR ?
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT :
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             use_ded_mem ? &ded_alloc : (void *)ded_alloc.pNext,
                             &f->flags, &f->mem[img_cnt])))
            return err;

        f->size[img_cnt] = req.memoryRequirements.size;
        bind_info[img_cnt].sType  = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
        bind_info[img_cnt].image  = f->img[img_cnt];
        bind_info[img_cnt].memory = f->mem[img_cnt];

        img_cnt++;
    }

    /* Bind the allocated memory to the images */
    ret = vk->BindImageMemory2(hwctx->act_dev, img_cnt, bind_info);
    if (ret != VK_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "Failed to bind memory: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    return 0;
}

enum PrepMode {
    PREP_MODE_WRITE,
    PREP_MODE_EXTERNAL_EXPORT,
    PREP_MODE_EXTERNAL_IMPORT,
    PREP_MODE_DECODING_DST,
    PREP_MODE_DECODING_DPB,
};

static int prepare_frame(AVHWFramesContext *hwfc, FFVkExecPool *ectx,
                         AVVkFrame *frame, enum PrepMode pmode)
{
    int err;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VkImageMemoryBarrier2 img_bar[AV_NUM_DATA_POINTERS];
    int nb_img_bar = 0;

    uint32_t dst_qf = VK_QUEUE_FAMILY_IGNORED;
    VkImageLayout new_layout;
    VkAccessFlags2 new_access;
    VkPipelineStageFlagBits2 src_stage = VK_PIPELINE_STAGE_2_NONE;

    /* This is dirty - but it works. The vulkan.c dependency system doesn't
     * free non-refcounted frames, and non-refcounted hardware frames cannot
     * happen anywhere outside of here. */
    AVBufferRef tmp_ref = {
        .data = (uint8_t *)hwfc,
    };
    AVFrame tmp_frame = {
        .data[0] = (uint8_t *)frame,
        .hw_frames_ctx = &tmp_ref,
    };

    VkCommandBuffer cmd_buf;
    FFVkExecContext *exec = ff_vk_exec_get(ectx);
    cmd_buf = exec->buf;
    ff_vk_exec_start(&p->vkctx, exec);

    err = ff_vk_exec_add_dep_frame(&p->vkctx, exec, &tmp_frame,
                                   VK_PIPELINE_STAGE_2_NONE,
                                   VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
    if (err < 0)
        return err;

    switch (pmode) {
    case PREP_MODE_WRITE:
        new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        new_access = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case PREP_MODE_EXTERNAL_IMPORT:
        new_layout = VK_IMAGE_LAYOUT_GENERAL;
        new_access = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        break;
    case PREP_MODE_EXTERNAL_EXPORT:
        new_layout = VK_IMAGE_LAYOUT_GENERAL;
        new_access = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        dst_qf     = VK_QUEUE_FAMILY_EXTERNAL_KHR;
        src_stage  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        break;
    case PREP_MODE_DECODING_DST:
        new_layout = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR;
        new_access = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case PREP_MODE_DECODING_DPB:
        new_layout = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR;
        new_access = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    }

    ff_vk_frame_barrier(&p->vkctx, exec, &tmp_frame, img_bar, &nb_img_bar,
                        src_stage,
                        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                        new_access, new_layout, dst_qf);

    vk->CmdPipelineBarrier2(cmd_buf, &(VkDependencyInfo) {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pImageMemoryBarriers = img_bar,
            .imageMemoryBarrierCount = nb_img_bar,
        });

    err = ff_vk_exec_submit(&p->vkctx, exec);
    if (err < 0)
        return err;

    /* We can do this because there are no real dependencies */
    ff_vk_exec_discard_deps(&p->vkctx, exec);

    return 0;
}

static inline void get_plane_wh(uint32_t *w, uint32_t *h, enum AVPixelFormat format,
                                int frame_w, int frame_h, int plane)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(format);

    /* Currently always true unless gray + alpha support is added */
    if (!plane || (plane == 3) || desc->flags & AV_PIX_FMT_FLAG_RGB ||
        !(desc->flags & AV_PIX_FMT_FLAG_PLANAR)) {
        *w = frame_w;
        *h = frame_h;
        return;
    }

    *w = AV_CEIL_RSHIFT(frame_w, desc->log2_chroma_w);
    *h = AV_CEIL_RSHIFT(frame_h, desc->log2_chroma_h);
}

static int create_frame(AVHWFramesContext *hwfc, AVVkFrame **frame,
                        VkImageTiling tiling, VkImageUsageFlagBits usage,
                        VkImageCreateFlags flags, int nb_layers,
                        void *create_pnext)
{
    int err;
    VkResult ret;
    AVVulkanFramesContext *hwfc_vk = hwfc->hwctx;
    AVHWDeviceContext *ctx = hwfc->device_ctx;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;

    VkExportSemaphoreCreateInfo ext_sem_info = {
        .sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO,
#ifdef _WIN32
        .handleTypes = IsWindows8OrGreater()
            ? VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT
            : VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT,
#else
        .handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
#endif
    };

    VkSemaphoreTypeCreateInfo sem_type_info = {
        .sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
#ifdef _WIN32
        .pNext         = p->vkctx.extensions & FF_VK_EXT_EXTERNAL_WIN32_SEM ? &ext_sem_info : NULL,
#else
        .pNext         = p->vkctx.extensions & FF_VK_EXT_EXTERNAL_FD_SEM ? &ext_sem_info : NULL,
#endif
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue  = 0,
    };

    VkSemaphoreCreateInfo sem_spawn = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &sem_type_info,
    };

    AVVkFrame *f = av_vk_frame_alloc();
    if (!f) {
        av_log(ctx, AV_LOG_ERROR, "Unable to allocate memory for AVVkFrame!\n");
        return AVERROR(ENOMEM);
    }

    // TODO: check witdh and height for alignment in case of multiplanar (must be mod-2 if subsampled)

    /* Create the images */
    for (int i = 0; (hwfc_vk->format[i] != VK_FORMAT_UNDEFINED); i++) {
        VkImageCreateInfo create_info = {
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext                 = create_pnext,
            .imageType             = VK_IMAGE_TYPE_2D,
            .format                = hwfc_vk->format[i],
            .extent.depth          = 1,
            .mipLevels             = 1,
            .arrayLayers           = nb_layers,
            .flags                 = flags,
            .tiling                = tiling,
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
            .usage                 = usage,
            .samples               = VK_SAMPLE_COUNT_1_BIT,
            .pQueueFamilyIndices   = p->img_qfs,
            .queueFamilyIndexCount = p->nb_img_qfs,
            .sharingMode           = p->nb_img_qfs > 1 ? VK_SHARING_MODE_CONCURRENT :
                                                         VK_SHARING_MODE_EXCLUSIVE,
        };

        get_plane_wh(&create_info.extent.width, &create_info.extent.height,
                     hwfc->sw_format, hwfc->width, hwfc->height, i);

        ret = vk->CreateImage(hwctx->act_dev, &create_info,
                              hwctx->alloc, &f->img[i]);
        if (ret != VK_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "Image creation failure: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR(EINVAL);
            goto fail;
        }

        /* Create semaphore */
        ret = vk->CreateSemaphore(hwctx->act_dev, &sem_spawn,
                                  hwctx->alloc, &f->sem[i]);
        if (ret != VK_SUCCESS) {
            av_log(hwctx, AV_LOG_ERROR, "Failed to create semaphore: %s\n",
                   ff_vk_ret2str(ret));
            return AVERROR_EXTERNAL;
        }

        f->queue_family[i] = p->nb_img_qfs > 1 ? VK_QUEUE_FAMILY_IGNORED : p->img_qfs[0];
        f->layout[i] = create_info.initialLayout;
        f->access[i] = 0x0;
        f->sem_value[i] = 0;
    }

    f->flags     = 0x0;
    f->tiling    = tiling;

    *frame = f;
    return 0;

fail:
    vulkan_frame_free(hwfc, f);
    return err;
}

/* Checks if an export flag is enabled, and if it is ORs it with *iexp */
static void try_export_flags(AVHWFramesContext *hwfc,
                             VkExternalMemoryHandleTypeFlags *comp_handle_types,
                             VkExternalMemoryHandleTypeFlagBits *iexp,
                             VkExternalMemoryHandleTypeFlagBits exp)
{
    VkResult ret;
    AVVulkanFramesContext *hwctx = hwfc->hwctx;
    AVVulkanDeviceContext *dev_hwctx = hwfc->device_ctx->hwctx;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;

    const VkImageDrmFormatModifierListCreateInfoEXT *drm_mod_info =
        ff_vk_find_struct(hwctx->create_pnext,
                          VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_LIST_CREATE_INFO_EXT);
    int has_mods = hwctx->tiling == VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT && drm_mod_info;
    int nb_mods;

    VkExternalImageFormatProperties eprops = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES_KHR,
    };
    VkImageFormatProperties2 props = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
        .pNext = &eprops,
    };
    VkPhysicalDeviceImageDrmFormatModifierInfoEXT phy_dev_mod_info = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT,
        .pNext = NULL,
        .pQueueFamilyIndices   = p->img_qfs,
        .queueFamilyIndexCount = p->nb_img_qfs,
        .sharingMode           = p->nb_img_qfs > 1 ? VK_SHARING_MODE_CONCURRENT :
                                                     VK_SHARING_MODE_EXCLUSIVE,
    };
    VkPhysicalDeviceExternalImageFormatInfo enext = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO,
        .handleType = exp,
        .pNext = has_mods ? &phy_dev_mod_info : NULL,
    };
    VkPhysicalDeviceImageFormatInfo2 pinfo = {
        .sType  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
        .pNext  = !exp ? NULL : &enext,
        .format = av_vkfmt_from_pixfmt(hwfc->sw_format)[0],
        .type   = VK_IMAGE_TYPE_2D,
        .tiling = hwctx->tiling,
        .usage  = hwctx->usage,
        .flags  = VK_IMAGE_CREATE_ALIAS_BIT,
    };

    nb_mods = has_mods ? drm_mod_info->drmFormatModifierCount : 1;
    for (int i = 0; i < nb_mods; i++) {
        if (has_mods)
            phy_dev_mod_info.drmFormatModifier = drm_mod_info->pDrmFormatModifiers[i];

        ret = vk->GetPhysicalDeviceImageFormatProperties2(dev_hwctx->phys_dev,
                                                        &pinfo, &props);

        if (ret == VK_SUCCESS) {
            *iexp |= exp;
            *comp_handle_types |= eprops.externalMemoryProperties.compatibleHandleTypes;
        }
    }
}

static AVBufferRef *vulkan_pool_alloc(void *opaque, size_t size)
{
    int err;
    AVVkFrame *f;
    AVBufferRef *avbuf = NULL;
    AVHWFramesContext *hwfc = opaque;
    AVVulkanFramesContext *hwctx = hwfc->hwctx;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    VulkanFramesPriv *fp = hwfc->internal->priv;
    VkExternalMemoryHandleTypeFlags e = 0x0;
    VkExportMemoryAllocateInfo eminfo[AV_NUM_DATA_POINTERS];

    VkExternalMemoryImageCreateInfo eiinfo = {
        .sType       = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
        .pNext       = hwctx->create_pnext,
    };

#ifdef _WIN32
    if (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_WIN32_MEMORY)
        try_export_flags(hwfc, &eiinfo.handleTypes, &e, IsWindows8OrGreater()
                             ? VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT
                             : VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT);
#else
    if (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_FD_MEMORY)
        try_export_flags(hwfc, &eiinfo.handleTypes, &e,
                         VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT);
#endif

    for (int i = 0; i < av_pix_fmt_count_planes(hwfc->sw_format); i++) {
        eminfo[i].sType       = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
        eminfo[i].pNext       = hwctx->alloc_pnext[i];
        eminfo[i].handleTypes = e;
    }

    err = create_frame(hwfc, &f, hwctx->tiling, hwctx->usage, hwctx->img_flags,
                       hwctx->nb_layers,
                       eiinfo.handleTypes ? &eiinfo : hwctx->create_pnext);
    if (err)
        return NULL;

    err = alloc_bind_mem(hwfc, f, eminfo, sizeof(*eminfo));
    if (err)
        goto fail;

    if ( (hwctx->usage & VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR) &&
        !(hwctx->usage & VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR))
        err = prepare_frame(hwfc, &fp->compute_exec, f, PREP_MODE_DECODING_DPB);
    else if (hwctx->usage & VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR)
        err = prepare_frame(hwfc, &fp->compute_exec, f, PREP_MODE_DECODING_DST);
    else
        err = prepare_frame(hwfc, &fp->compute_exec, f, PREP_MODE_WRITE);
    if (err)
        goto fail;

    avbuf = av_buffer_create((uint8_t *)f, sizeof(AVVkFrame),
                             vulkan_frame_free_cb, hwfc, 0);
    if (!avbuf)
        goto fail;

    return avbuf;

fail:
    vulkan_frame_free(hwfc, f);
    return NULL;
}

static void lock_frame(AVHWFramesContext *fc, AVVkFrame *vkf)
{
    pthread_mutex_lock(&vkf->internal->update_mutex);
}

static void unlock_frame(AVHWFramesContext *fc, AVVkFrame *vkf)
{
    pthread_mutex_unlock(&vkf->internal->update_mutex);
}

static void vulkan_frames_uninit(AVHWFramesContext *hwfc)
{
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    VulkanFramesPriv *fp = hwfc->internal->priv;

    if (fp->modifier_info) {
        if (fp->modifier_info->pDrmFormatModifiers)
            av_freep(&fp->modifier_info->pDrmFormatModifiers);
        av_freep(&fp->modifier_info);
    }

    ff_vk_exec_pool_free(&p->vkctx, &fp->compute_exec);
    ff_vk_exec_pool_free(&p->vkctx, &fp->upload_exec);
    ff_vk_exec_pool_free(&p->vkctx, &fp->download_exec);
}

static int vulkan_frames_init(AVHWFramesContext *hwfc)
{
    int err;
    AVVkFrame *f;
    AVVulkanFramesContext *hwctx = hwfc->hwctx;
    VulkanFramesPriv *fp = hwfc->internal->priv;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    VkImageUsageFlagBits supported_usage;
    const struct FFVkFormatEntry *fmt;
    int disable_multiplane = p->disable_multiplane ||
                             (hwctx->flags & AV_VK_FRAME_FLAG_DISABLE_MULTIPLANE);

    /* Defaults */
    if (!hwctx->nb_layers)
        hwctx->nb_layers = 1;

    /* VK_IMAGE_TILING_OPTIMAL == 0, can't check for it really */
    if (p->use_linear_images &&
        (hwctx->tiling != VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT))
        hwctx->tiling = VK_IMAGE_TILING_LINEAR;


    fmt = vk_find_format_entry(hwfc->sw_format);
    if (!fmt) {
        av_log(hwfc, AV_LOG_ERROR, "Unsupported pixel format: %s!\n",
               av_get_pix_fmt_name(hwfc->sw_format));
        return AVERROR(EINVAL);
    }

    if (hwctx->format[0] != VK_FORMAT_UNDEFINED) {
        if (hwctx->format[0] != fmt->vkf) {
            for (int i = 0; i < fmt->nb_images_fallback; i++) {
                if (hwctx->format[i] != fmt->fallback[i]) {
                    av_log(hwfc, AV_LOG_ERROR, "Incompatible Vulkan format given "
                           "for the current sw_format %s!\n",
                           av_get_pix_fmt_name(hwfc->sw_format));
                    return AVERROR(EINVAL);
                }
            }
        }

        /* Check if the sw_format itself is supported */
        err = vkfmt_from_pixfmt2(hwfc->device_ctx, hwfc->sw_format,
                                 hwctx->tiling, NULL,
                                 NULL, NULL, &supported_usage, 0,
                                 hwctx->usage & VK_IMAGE_USAGE_STORAGE_BIT);
        if (err < 0) {
            av_log(hwfc, AV_LOG_ERROR, "Unsupported sw format: %s!\n",
                   av_get_pix_fmt_name(hwfc->sw_format));
            return AVERROR(EINVAL);
        }
    } else {
        err = vkfmt_from_pixfmt2(hwfc->device_ctx, hwfc->sw_format,
                                 hwctx->tiling, hwctx->format, NULL,
                                 NULL, &supported_usage,
                                 disable_multiplane,
                                 hwctx->usage & VK_IMAGE_USAGE_STORAGE_BIT);
        if (err < 0)
            return err;
    }

    /* Image usage flags */
    if (!hwctx->usage) {
        hwctx->usage = supported_usage & (VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                          VK_IMAGE_USAGE_STORAGE_BIT       |
                                          VK_IMAGE_USAGE_SAMPLED_BIT);
    }

    /* Image creation flags.
     * Only fill them in automatically if the image is not going to be used as
     * a DPB-only image, and we have SAMPLED/STORAGE bits set. */
    if (!hwctx->img_flags) {
        int is_lone_dpb =  (hwctx->usage & VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR) &&
                          !(hwctx->usage & VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR);
        int sampleable = hwctx->usage & (VK_IMAGE_USAGE_SAMPLED_BIT |
                                         VK_IMAGE_USAGE_STORAGE_BIT);
        if (sampleable && !is_lone_dpb) {
            hwctx->img_flags = VK_IMAGE_CREATE_ALIAS_BIT;
            if ((fmt->vk_planes > 1) && (hwctx->format[0] == fmt->vkf))
                hwctx->img_flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT |
                                    VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;
        }
    }

    if (!hwctx->lock_frame)
        hwctx->lock_frame = lock_frame;

    if (!hwctx->unlock_frame)
        hwctx->unlock_frame = unlock_frame;

    err = ff_vk_exec_pool_init(&p->vkctx, &p->compute_qf, &fp->compute_exec,
                               p->compute_qf.nb_queues, 0, 0, 0, NULL);
    if (err)
        return err;

    err = ff_vk_exec_pool_init(&p->vkctx, &p->transfer_qf, &fp->upload_exec,
                               p->transfer_qf.nb_queues*2, 0, 0, 0, NULL);
    if (err)
        return err;

    err = ff_vk_exec_pool_init(&p->vkctx, &p->transfer_qf, &fp->download_exec,
                               p->transfer_qf.nb_queues, 0, 0, 0, NULL);
    if (err)
        return err;

    /* Test to see if allocation will fail */
    err = create_frame(hwfc, &f, hwctx->tiling, hwctx->usage, hwctx->img_flags,
                       hwctx->nb_layers, hwctx->create_pnext);
    if (err)
        return err;

    vulkan_frame_free(hwfc, f);

    /* If user did not specify a pool, hwfc->pool will be set to the internal one
     * in hwcontext.c just after this gets called */
    if (!hwfc->pool) {
        hwfc->internal->pool_internal = av_buffer_pool_init2(sizeof(AVVkFrame),
                                                             hwfc, vulkan_pool_alloc,
                                                             NULL);
        if (!hwfc->internal->pool_internal)
            return AVERROR(ENOMEM);
    }

    return 0;
}

static int vulkan_get_buffer(AVHWFramesContext *hwfc, AVFrame *frame)
{
    frame->buf[0] = av_buffer_pool_get(hwfc->pool);
    if (!frame->buf[0])
        return AVERROR(ENOMEM);

    frame->data[0] = frame->buf[0]->data;
    frame->format  = AV_PIX_FMT_VULKAN;
    frame->width   = hwfc->width;
    frame->height  = hwfc->height;

    return 0;
}

static int vulkan_transfer_get_formats(AVHWFramesContext *hwfc,
                                       enum AVHWFrameTransferDirection dir,
                                       enum AVPixelFormat **formats)
{
    enum AVPixelFormat *fmts;
    int n = 2;

#if CONFIG_CUDA
    n++;
#endif
    fmts = av_malloc_array(n, sizeof(*fmts));
    if (!fmts)
        return AVERROR(ENOMEM);

    n = 0;
    fmts[n++] = hwfc->sw_format;
#if CONFIG_CUDA
    fmts[n++] = AV_PIX_FMT_CUDA;
#endif
    fmts[n++] = AV_PIX_FMT_NONE;

    *formats = fmts;
    return 0;
}

#if CONFIG_LIBDRM
static void vulkan_unmap_from_drm(AVHWFramesContext *hwfc, HWMapDescriptor *hwmap)
{
    vulkan_frame_free(hwfc, hwmap->priv);
}

static const struct {
    uint32_t drm_fourcc;
    VkFormat vk_format;
} vulkan_drm_format_map[] = {
    { DRM_FORMAT_R8,       VK_FORMAT_R8_UNORM       },
    { DRM_FORMAT_R16,      VK_FORMAT_R16_UNORM      },
    { DRM_FORMAT_GR88,     VK_FORMAT_R8G8_UNORM     },
    { DRM_FORMAT_RG88,     VK_FORMAT_R8G8_UNORM     },
    { DRM_FORMAT_GR1616,   VK_FORMAT_R16G16_UNORM   },
    { DRM_FORMAT_RG1616,   VK_FORMAT_R16G16_UNORM   },
    { DRM_FORMAT_ARGB8888, VK_FORMAT_B8G8R8A8_UNORM },
    { DRM_FORMAT_XRGB8888, VK_FORMAT_B8G8R8A8_UNORM },
    { DRM_FORMAT_ABGR8888, VK_FORMAT_R8G8B8A8_UNORM },
    { DRM_FORMAT_XBGR8888, VK_FORMAT_R8G8B8A8_UNORM },

    // All these DRM_FORMATs were added in the same libdrm commit.
#ifdef DRM_FORMAT_XYUV8888
    { DRM_FORMAT_XYUV8888, VK_FORMAT_R8G8B8A8_UNORM     },
    { DRM_FORMAT_XVYU12_16161616, VK_FORMAT_R16G16B16A16_UNORM} ,
    // As we had to map XV36 to a 16bit Vulkan format, reverse mapping will
    // end up yielding Y416 as the DRM format, so we need to recognise it.
    { DRM_FORMAT_Y416,     VK_FORMAT_R16G16B16A16_UNORM },
#endif
};

static inline VkFormat drm_to_vulkan_fmt(uint32_t drm_fourcc)
{
    for (int i = 0; i < FF_ARRAY_ELEMS(vulkan_drm_format_map); i++)
        if (vulkan_drm_format_map[i].drm_fourcc == drm_fourcc)
            return vulkan_drm_format_map[i].vk_format;
    return VK_FORMAT_UNDEFINED;
}

static int vulkan_map_from_drm_frame_desc(AVHWFramesContext *hwfc, AVVkFrame **frame,
                                          const AVFrame *src)
{
    int err = 0;
    VkResult ret;
    AVVkFrame *f;
    int bind_counts = 0;
    AVHWDeviceContext *ctx = hwfc->device_ctx;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VulkanFramesPriv *fp = hwfc->internal->priv;
    const AVDRMFrameDescriptor *desc = (AVDRMFrameDescriptor *)src->data[0];
    VkBindImageMemoryInfo bind_info[AV_DRM_MAX_PLANES];
    VkBindImagePlaneMemoryInfo plane_info[AV_DRM_MAX_PLANES];

    for (int i = 0; i < desc->nb_layers; i++) {
        if (drm_to_vulkan_fmt(desc->layers[i].format) == VK_FORMAT_UNDEFINED) {
            av_log(ctx, AV_LOG_ERROR, "Unsupported DMABUF layer format %#08x!\n",
                   desc->layers[i].format);
            return AVERROR(EINVAL);
        }
    }

    if (!(f = av_vk_frame_alloc())) {
        av_log(ctx, AV_LOG_ERROR, "Unable to allocate memory for AVVkFrame!\n");
        err = AVERROR(ENOMEM);
        goto fail;
    }

    f->tiling = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT;

    for (int i = 0; i < desc->nb_layers; i++) {
        const int planes = desc->layers[i].nb_planes;

        /* Semaphore */
        VkSemaphoreTypeCreateInfo sem_type_info = {
            .sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue  = 0,
        };
        VkSemaphoreCreateInfo sem_spawn = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &sem_type_info,
        };

        /* Image creation */
        VkSubresourceLayout ext_img_layouts[AV_DRM_MAX_PLANES];
        VkImageDrmFormatModifierExplicitCreateInfoEXT ext_img_mod_spec = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_EXPLICIT_CREATE_INFO_EXT,
            .drmFormatModifier = desc->objects[0].format_modifier,
            .drmFormatModifierPlaneCount = planes,
            .pPlaneLayouts = (const VkSubresourceLayout *)&ext_img_layouts,
        };
        VkExternalMemoryImageCreateInfo ext_img_spec = {
            .sType       = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
            .pNext       = &ext_img_mod_spec,
            .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
        };
        VkImageCreateInfo create_info = {
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext                 = &ext_img_spec,
            .imageType             = VK_IMAGE_TYPE_2D,
            .format                = drm_to_vulkan_fmt(desc->layers[i].format),
            .extent.depth          = 1,
            .mipLevels             = 1,
            .arrayLayers           = 1,
            .flags                 = 0x0,
            .tiling                = VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED, /* specs say so */
            .usage                 = VK_IMAGE_USAGE_SAMPLED_BIT |
                                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .samples               = VK_SAMPLE_COUNT_1_BIT,
            .pQueueFamilyIndices   = p->img_qfs,
            .queueFamilyIndexCount = p->nb_img_qfs,
            .sharingMode           = p->nb_img_qfs > 1 ? VK_SHARING_MODE_CONCURRENT :
                                                         VK_SHARING_MODE_EXCLUSIVE,
        };

        /* Image format verification */
        VkExternalImageFormatProperties ext_props = {
            .sType = VK_STRUCTURE_TYPE_EXTERNAL_IMAGE_FORMAT_PROPERTIES_KHR,
        };
        VkImageFormatProperties2 props_ret = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2,
            .pNext = &ext_props,
        };
        VkPhysicalDeviceImageDrmFormatModifierInfoEXT props_drm_mod = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_DRM_FORMAT_MODIFIER_INFO_EXT,
            .drmFormatModifier = ext_img_mod_spec.drmFormatModifier,
            .pQueueFamilyIndices = create_info.pQueueFamilyIndices,
            .queueFamilyIndexCount = create_info.queueFamilyIndexCount,
            .sharingMode = create_info.sharingMode,
        };
        VkPhysicalDeviceExternalImageFormatInfo props_ext = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_IMAGE_FORMAT_INFO,
            .pNext = &props_drm_mod,
            .handleType = ext_img_spec.handleTypes,
        };
        VkPhysicalDeviceImageFormatInfo2 fmt_props = {
            .sType  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
            .pNext  = &props_ext,
            .format = create_info.format,
            .type   = create_info.imageType,
            .tiling = create_info.tiling,
            .usage  = create_info.usage,
            .flags  = create_info.flags,
        };

        /* Check if importing is possible for this combination of parameters */
        ret = vk->GetPhysicalDeviceImageFormatProperties2(hwctx->phys_dev,
                                                          &fmt_props, &props_ret);
        if (ret != VK_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "Cannot map DRM frame to Vulkan: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR_EXTERNAL;
            goto fail;
        }

        /* Set the image width/height */
        get_plane_wh(&create_info.extent.width, &create_info.extent.height,
                     hwfc->sw_format, src->width, src->height, i);

        /* Set the subresource layout based on the layer properties */
        for (int j = 0; j < planes; j++) {
            ext_img_layouts[j].offset     = desc->layers[i].planes[j].offset;
            ext_img_layouts[j].rowPitch   = desc->layers[i].planes[j].pitch;
            ext_img_layouts[j].size       = 0; /* The specs say so for all 3 */
            ext_img_layouts[j].arrayPitch = 0;
            ext_img_layouts[j].depthPitch = 0;
        }

        /* Create image */
        ret = vk->CreateImage(hwctx->act_dev, &create_info,
                              hwctx->alloc, &f->img[i]);
        if (ret != VK_SUCCESS) {
            av_log(ctx, AV_LOG_ERROR, "Image creation failure: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR(EINVAL);
            goto fail;
        }

        ret = vk->CreateSemaphore(hwctx->act_dev, &sem_spawn,
                                  hwctx->alloc, &f->sem[i]);
        if (ret != VK_SUCCESS) {
            av_log(hwctx, AV_LOG_ERROR, "Failed to create semaphore: %s\n",
                   ff_vk_ret2str(ret));
            return AVERROR_EXTERNAL;
        }

        /* We'd import a semaphore onto the one we created using
         * vkImportSemaphoreFdKHR but unfortunately neither DRM nor VAAPI
         * offer us anything we could import and sync with, so instead
         * just signal the semaphore we created. */

        f->queue_family[i] = p->nb_img_qfs > 1 ? VK_QUEUE_FAMILY_IGNORED : p->img_qfs[0];
        f->layout[i] = create_info.initialLayout;
        f->access[i] = 0x0;
        f->sem_value[i] = 0;
    }

    for (int i = 0; i < desc->nb_layers; i++) {
        /* Memory requirements */
        VkImageMemoryRequirementsInfo2 req_desc = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
            .image = f->img[i],
        };
        VkMemoryDedicatedRequirements ded_req = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
        };
        VkMemoryRequirements2 req2 = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
            .pNext = &ded_req,
        };

        /* Allocation/importing */
        VkMemoryFdPropertiesKHR fdmp = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR,
        };
        /* This assumes that a layer will never be constructed from multiple
         * objects. If that was to happen in the real world, this code would
         * need to import each plane separately.
         */
        VkImportMemoryFdInfoKHR idesc = {
            .sType      = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR,
            .fd         = dup(desc->objects[desc->layers[i].planes[0].object_index].fd),
            .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
        };
        VkMemoryDedicatedAllocateInfo ded_alloc = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
            .pNext = &idesc,
            .image = req_desc.image,
        };

        /* Get object properties */
        ret = vk->GetMemoryFdPropertiesKHR(hwctx->act_dev,
                                           VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
                                           idesc.fd, &fdmp);
        if (ret != VK_SUCCESS) {
            av_log(hwfc, AV_LOG_ERROR, "Failed to get FD properties: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR_EXTERNAL;
            close(idesc.fd);
            goto fail;
        }

        vk->GetImageMemoryRequirements2(hwctx->act_dev, &req_desc, &req2);

        /* Only a single bit must be set, not a range, and it must match */
        req2.memoryRequirements.memoryTypeBits = fdmp.memoryTypeBits;

        err = alloc_mem(ctx, &req2.memoryRequirements,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        (ded_req.prefersDedicatedAllocation ||
                         ded_req.requiresDedicatedAllocation) ?
                            &ded_alloc : ded_alloc.pNext,
                        &f->flags, &f->mem[i]);
        if (err) {
            close(idesc.fd);
            return err;
        }

        f->size[i] = req2.memoryRequirements.size;
    }

    for (int i = 0; i < desc->nb_layers; i++) {
        const int planes = desc->layers[i].nb_planes;
        for (int j = 0; j < planes; j++) {
            VkImageAspectFlagBits aspect = j == 0 ? VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT :
                                           j == 1 ? VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT :
                                                    VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT;

            plane_info[bind_counts].sType = VK_STRUCTURE_TYPE_BIND_IMAGE_PLANE_MEMORY_INFO;
            plane_info[bind_counts].pNext = NULL;
            plane_info[bind_counts].planeAspect = aspect;

            bind_info[bind_counts].sType  = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
            bind_info[bind_counts].pNext  = planes > 1 ? &plane_info[bind_counts] : NULL;
            bind_info[bind_counts].image  = f->img[i];
            bind_info[bind_counts].memory = f->mem[i];

            /* Offset is already signalled via pPlaneLayouts above */
            bind_info[bind_counts].memoryOffset = 0;

            bind_counts++;
        }
    }

    /* Bind the allocated memory to the images */
    ret = vk->BindImageMemory2(hwctx->act_dev, bind_counts, bind_info);
    if (ret != VK_SUCCESS) {
        av_log(ctx, AV_LOG_ERROR, "Failed to bind memory: %s\n",
               ff_vk_ret2str(ret));
        err = AVERROR_EXTERNAL;
        goto fail;
    }

    err = prepare_frame(hwfc, &fp->compute_exec, f, PREP_MODE_EXTERNAL_IMPORT);
    if (err)
        goto fail;

    *frame = f;

    return 0;

fail:
    for (int i = 0; i < desc->nb_layers; i++) {
        vk->DestroyImage(hwctx->act_dev, f->img[i], hwctx->alloc);
        vk->DestroySemaphore(hwctx->act_dev, f->sem[i], hwctx->alloc);
    }
    for (int i = 0; i < desc->nb_objects; i++)
        vk->FreeMemory(hwctx->act_dev, f->mem[i], hwctx->alloc);

    av_free(f);

    return err;
}

static int vulkan_map_from_drm(AVHWFramesContext *hwfc, AVFrame *dst,
                               const AVFrame *src, int flags)
{
    int err = 0;
    AVVkFrame *f;

    if ((err = vulkan_map_from_drm_frame_desc(hwfc, &f, src)))
        return err;

    /* The unmapping function will free this */
    dst->data[0] = (uint8_t *)f;
    dst->width   = src->width;
    dst->height  = src->height;

    err = ff_hwframe_map_create(dst->hw_frames_ctx, dst, src,
                                &vulkan_unmap_from_drm, f);
    if (err < 0)
        goto fail;

    av_log(hwfc, AV_LOG_DEBUG, "Mapped DRM object to Vulkan!\n");

    return 0;

fail:
    vulkan_frame_free(hwfc->device_ctx->hwctx, f);
    dst->data[0] = NULL;
    return err;
}

#if CONFIG_VAAPI
static int vulkan_map_from_vaapi(AVHWFramesContext *dst_fc,
                                 AVFrame *dst, const AVFrame *src,
                                 int flags)
{
    int err;
    AVFrame *tmp = av_frame_alloc();
    AVHWFramesContext *vaapi_fc = (AVHWFramesContext*)src->hw_frames_ctx->data;
    AVVAAPIDeviceContext *vaapi_ctx = vaapi_fc->device_ctx->hwctx;
    VASurfaceID surface_id = (VASurfaceID)(uintptr_t)src->data[3];

    if (!tmp)
        return AVERROR(ENOMEM);

    /* We have to sync since like the previous comment said, no semaphores */
    vaSyncSurface(vaapi_ctx->display, surface_id);

    tmp->format = AV_PIX_FMT_DRM_PRIME;

    err = av_hwframe_map(tmp, src, flags);
    if (err < 0)
        goto fail;

    err = vulkan_map_from_drm(dst_fc, dst, tmp, flags);
    if (err < 0)
        goto fail;

    err = ff_hwframe_map_replace(dst, src);

fail:
    av_frame_free(&tmp);
    return err;
}
#endif
#endif

#if CONFIG_CUDA
static int vulkan_export_to_cuda(AVHWFramesContext *hwfc,
                                 AVBufferRef *cuda_hwfc,
                                 const AVFrame *frame)
{
    int err;
    VkResult ret;
    AVVkFrame *dst_f;
    AVVkFrameInternal *dst_int;
    AVHWDeviceContext *ctx = hwfc->device_ctx;
    AVVulkanDeviceContext *hwctx = ctx->hwctx;
    const int planes = av_pix_fmt_count_planes(hwfc->sw_format);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(hwfc->sw_format);
    VulkanDevicePriv *p = ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;

    AVHWFramesContext *cuda_fc = (AVHWFramesContext*)cuda_hwfc->data;
    AVHWDeviceContext *cuda_cu = cuda_fc->device_ctx;
    AVCUDADeviceContext *cuda_dev = cuda_cu->hwctx;
    AVCUDADeviceContextInternal *cu_internal = cuda_dev->internal;
    CudaFunctions *cu = cu_internal->cuda_dl;
    CUarray_format cufmt = desc->comp[0].depth > 8 ? CU_AD_FORMAT_UNSIGNED_INT16 :
                                                     CU_AD_FORMAT_UNSIGNED_INT8;

    dst_f = (AVVkFrame *)frame->data[0];
    dst_int = dst_f->internal;

    if (!dst_int->cuda_fc_ref) {
        dst_int->cuda_fc_ref = av_buffer_ref(cuda_hwfc);
        if (!dst_int->cuda_fc_ref)
            return AVERROR(ENOMEM);

        for (int i = 0; i < planes; i++) {
            CUDA_EXTERNAL_MEMORY_MIPMAPPED_ARRAY_DESC tex_desc = {
                .offset = 0,
                .arrayDesc = {
                    .Depth = 0,
                    .Format = cufmt,
                    .NumChannels = 1 + ((planes == 2) && i),
                    .Flags = 0,
                },
                .numLevels = 1,
            };
            int p_w, p_h;

#ifdef _WIN32
            CUDA_EXTERNAL_MEMORY_HANDLE_DESC ext_desc = {
                .type = IsWindows8OrGreater()
                    ? CU_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32
                    : CU_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_KMT,
                .size = dst_f->size[i],
            };
            VkMemoryGetWin32HandleInfoKHR export_info = {
                .sType      = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
                .memory     = dst_f->mem[i],
                .handleType = IsWindows8OrGreater()
                    ? VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT
                    : VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT,
            };
            VkSemaphoreGetWin32HandleInfoKHR sem_export = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR,
                .semaphore = dst_f->sem[i],
                .handleType = IsWindows8OrGreater()
                    ? VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT
                    : VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_KMT_BIT,
            };
            CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC ext_sem_desc = {
                .type = 10 /* TODO: CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_WIN32 */,
            };

            ret = vk->GetMemoryWin32HandleKHR(hwctx->act_dev, &export_info,
                                              &ext_desc.handle.win32.handle);
            if (ret != VK_SUCCESS) {
                av_log(hwfc, AV_LOG_ERROR, "Unable to export the image as a Win32 Handle: %s!\n",
                       ff_vk_ret2str(ret));
                err = AVERROR_EXTERNAL;
                goto fail;
            }
            dst_int->ext_mem_handle[i] = ext_desc.handle.win32.handle;
#else
            CUDA_EXTERNAL_MEMORY_HANDLE_DESC ext_desc = {
                .type = CU_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD,
                .size = dst_f->size[i],
            };
            VkMemoryGetFdInfoKHR export_info = {
                .sType      = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
                .memory     = dst_f->mem[i],
                .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT_KHR,
            };
            VkSemaphoreGetFdInfoKHR sem_export = {
                .sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR,
                .semaphore = dst_f->sem[i],
                .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
            };
            CUDA_EXTERNAL_SEMAPHORE_HANDLE_DESC ext_sem_desc = {
                .type = 9 /* TODO: CU_EXTERNAL_SEMAPHORE_HANDLE_TYPE_TIMELINE_SEMAPHORE_FD */,
            };

            ret = vk->GetMemoryFdKHR(hwctx->act_dev, &export_info,
                                     &ext_desc.handle.fd);
            if (ret != VK_SUCCESS) {
                av_log(hwfc, AV_LOG_ERROR, "Unable to export the image as a FD: %s!\n",
                       ff_vk_ret2str(ret));
                err = AVERROR_EXTERNAL;
                goto fail;
            }
#endif

            ret = CHECK_CU(cu->cuImportExternalMemory(&dst_int->ext_mem[i], &ext_desc));
            if (ret < 0) {
#ifndef _WIN32
                close(ext_desc.handle.fd);
#endif
                err = AVERROR_EXTERNAL;
                goto fail;
            }

            get_plane_wh(&p_w, &p_h, hwfc->sw_format, hwfc->width, hwfc->height, i);
            tex_desc.arrayDesc.Width = p_w;
            tex_desc.arrayDesc.Height = p_h;

            ret = CHECK_CU(cu->cuExternalMemoryGetMappedMipmappedArray(&dst_int->cu_mma[i],
                                                                       dst_int->ext_mem[i],
                                                                       &tex_desc));
            if (ret < 0) {
                err = AVERROR_EXTERNAL;
                goto fail;
            }

            ret = CHECK_CU(cu->cuMipmappedArrayGetLevel(&dst_int->cu_array[i],
                                                        dst_int->cu_mma[i], 0));
            if (ret < 0) {
                err = AVERROR_EXTERNAL;
                goto fail;
            }

#ifdef _WIN32
            ret = vk->GetSemaphoreWin32HandleKHR(hwctx->act_dev, &sem_export,
                                                 &ext_sem_desc.handle.win32.handle);
#else
            ret = vk->GetSemaphoreFdKHR(hwctx->act_dev, &sem_export,
                                        &ext_sem_desc.handle.fd);
#endif
            if (ret != VK_SUCCESS) {
                av_log(ctx, AV_LOG_ERROR, "Failed to export semaphore: %s\n",
                       ff_vk_ret2str(ret));
                err = AVERROR_EXTERNAL;
                goto fail;
            }
#ifdef _WIN32
            dst_int->ext_sem_handle[i] = ext_sem_desc.handle.win32.handle;
#endif

            ret = CHECK_CU(cu->cuImportExternalSemaphore(&dst_int->cu_sem[i],
                                                         &ext_sem_desc));
            if (ret < 0) {
#ifndef _WIN32
                close(ext_sem_desc.handle.fd);
#endif
                err = AVERROR_EXTERNAL;
                goto fail;
            }
        }
    }

    return 0;

fail:
    vulkan_free_internal(dst_f);
    return err;
}

static int vulkan_transfer_data_from_cuda(AVHWFramesContext *hwfc,
                                          AVFrame *dst, const AVFrame *src)
{
    int err;
    CUcontext dummy;
    AVVkFrame *dst_f;
    AVVkFrameInternal *dst_int;
    VulkanFramesPriv *fp = hwfc->internal->priv;
    const int planes = av_pix_fmt_count_planes(hwfc->sw_format);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(hwfc->sw_format);

    AVHWFramesContext *cuda_fc = (AVHWFramesContext*)src->hw_frames_ctx->data;
    AVHWDeviceContext *cuda_cu = cuda_fc->device_ctx;
    AVCUDADeviceContext *cuda_dev = cuda_cu->hwctx;
    AVCUDADeviceContextInternal *cu_internal = cuda_dev->internal;
    CudaFunctions *cu = cu_internal->cuda_dl;
    CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS s_w_par[AV_NUM_DATA_POINTERS] = { 0 };
    CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS s_s_par[AV_NUM_DATA_POINTERS] = { 0 };

    dst_f = (AVVkFrame *)dst->data[0];

    err = prepare_frame(hwfc, &fp->upload_exec, dst_f, PREP_MODE_EXTERNAL_EXPORT);
    if (err < 0)
        return err;

    err = CHECK_CU(cu->cuCtxPushCurrent(cuda_dev->cuda_ctx));
    if (err < 0)
        return err;

    err = vulkan_export_to_cuda(hwfc, src->hw_frames_ctx, dst);
    if (err < 0) {
        CHECK_CU(cu->cuCtxPopCurrent(&dummy));
        return err;
    }

    dst_int = dst_f->internal;

    for (int i = 0; i < planes; i++) {
        s_w_par[i].params.fence.value = dst_f->sem_value[i] + 0;
        s_s_par[i].params.fence.value = dst_f->sem_value[i] + 1;
    }

    err = CHECK_CU(cu->cuWaitExternalSemaphoresAsync(dst_int->cu_sem, s_w_par,
                                                     planes, cuda_dev->stream));
    if (err < 0)
        goto fail;

    for (int i = 0; i < planes; i++) {
        CUDA_MEMCPY2D cpy = {
            .srcMemoryType = CU_MEMORYTYPE_DEVICE,
            .srcDevice     = (CUdeviceptr)src->data[i],
            .srcPitch      = src->linesize[i],
            .srcY          = 0,

            .dstMemoryType = CU_MEMORYTYPE_ARRAY,
            .dstArray      = dst_int->cu_array[i],
        };

        int p_w, p_h;
        get_plane_wh(&p_w, &p_h, hwfc->sw_format, hwfc->width, hwfc->height, i);

        cpy.WidthInBytes = p_w * desc->comp[i].step;
        cpy.Height = p_h;

        err = CHECK_CU(cu->cuMemcpy2DAsync(&cpy, cuda_dev->stream));
        if (err < 0)
            goto fail;
    }

    err = CHECK_CU(cu->cuSignalExternalSemaphoresAsync(dst_int->cu_sem, s_s_par,
                                                       planes, cuda_dev->stream));
    if (err < 0)
        goto fail;

    for (int i = 0; i < planes; i++)
        dst_f->sem_value[i]++;

    CHECK_CU(cu->cuCtxPopCurrent(&dummy));

    av_log(hwfc, AV_LOG_VERBOSE, "Transferred CUDA image to Vulkan!\n");

    return err = prepare_frame(hwfc, &fp->upload_exec, dst_f, PREP_MODE_EXTERNAL_IMPORT);

fail:
    CHECK_CU(cu->cuCtxPopCurrent(&dummy));
    vulkan_free_internal(dst_f);
    av_buffer_unref(&dst->buf[0]);
    return err;
}
#endif

static int vulkan_map_to(AVHWFramesContext *hwfc, AVFrame *dst,
                         const AVFrame *src, int flags)
{
    av_unused VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;

    switch (src->format) {
#if CONFIG_LIBDRM
#if CONFIG_VAAPI
    case AV_PIX_FMT_VAAPI:
        if (p->vkctx.extensions & FF_VK_EXT_DRM_MODIFIER_FLAGS)
            return vulkan_map_from_vaapi(hwfc, dst, src, flags);
        else
            return AVERROR(ENOSYS);
#endif
    case AV_PIX_FMT_DRM_PRIME:
        if (p->vkctx.extensions & FF_VK_EXT_DRM_MODIFIER_FLAGS)
            return vulkan_map_from_drm(hwfc, dst, src, flags);
        else
            return AVERROR(ENOSYS);
#endif
    default:
        return AVERROR(ENOSYS);
    }
}

#if CONFIG_LIBDRM
typedef struct VulkanDRMMapping {
    AVDRMFrameDescriptor drm_desc;
    AVVkFrame *source;
} VulkanDRMMapping;

static void vulkan_unmap_to_drm(AVHWFramesContext *hwfc, HWMapDescriptor *hwmap)
{
    AVDRMFrameDescriptor *drm_desc = hwmap->priv;

    for (int i = 0; i < drm_desc->nb_objects; i++)
        close(drm_desc->objects[i].fd);

    av_free(drm_desc);
}

static inline uint32_t vulkan_fmt_to_drm(VkFormat vkfmt)
{
    for (int i = 0; i < FF_ARRAY_ELEMS(vulkan_drm_format_map); i++)
        if (vulkan_drm_format_map[i].vk_format == vkfmt)
            return vulkan_drm_format_map[i].drm_fourcc;
    return DRM_FORMAT_INVALID;
}

static int vulkan_map_to_drm(AVHWFramesContext *hwfc, AVFrame *dst,
                             const AVFrame *src, int flags)
{
    int err = 0;
    VkResult ret;
    AVVkFrame *f = (AVVkFrame *)src->data[0];
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VulkanFramesPriv *fp = hwfc->internal->priv;
    AVVulkanDeviceContext *hwctx = hwfc->device_ctx->hwctx;
    AVVulkanFramesContext *hwfctx = hwfc->hwctx;
    const int planes = av_pix_fmt_count_planes(hwfc->sw_format);
    VkImageDrmFormatModifierPropertiesEXT drm_mod = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_DRM_FORMAT_MODIFIER_PROPERTIES_EXT,
    };
    VkSemaphoreWaitInfo wait_info = {
        .sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .flags          = 0x0,
        .semaphoreCount = planes,
    };

    AVDRMFrameDescriptor *drm_desc = av_mallocz(sizeof(*drm_desc));
    if (!drm_desc)
        return AVERROR(ENOMEM);

    err = prepare_frame(hwfc, &fp->compute_exec, f, PREP_MODE_EXTERNAL_EXPORT);
    if (err < 0)
        goto end;

    /* Wait for the operation to finish so we can cleanly export it. */
    wait_info.pSemaphores = f->sem;
    wait_info.pValues     = f->sem_value;

    vk->WaitSemaphores(hwctx->act_dev, &wait_info, UINT64_MAX);

    err = ff_hwframe_map_create(src->hw_frames_ctx, dst, src, &vulkan_unmap_to_drm, drm_desc);
    if (err < 0)
        goto end;

    ret = vk->GetImageDrmFormatModifierPropertiesEXT(hwctx->act_dev, f->img[0],
                                                     &drm_mod);
    if (ret != VK_SUCCESS) {
        av_log(hwfc, AV_LOG_ERROR, "Failed to retrieve DRM format modifier!\n");
        err = AVERROR_EXTERNAL;
        goto end;
    }

    for (int i = 0; (i < planes) && (f->mem[i]); i++) {
        VkMemoryGetFdInfoKHR export_info = {
            .sType      = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
            .memory     = f->mem[i],
            .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
        };

        ret = vk->GetMemoryFdKHR(hwctx->act_dev, &export_info,
                                 &drm_desc->objects[i].fd);
        if (ret != VK_SUCCESS) {
            av_log(hwfc, AV_LOG_ERROR, "Unable to export the image as a FD!\n");
            err = AVERROR_EXTERNAL;
            goto end;
        }

        drm_desc->nb_objects++;
        drm_desc->objects[i].size = f->size[i];
        drm_desc->objects[i].format_modifier = drm_mod.drmFormatModifier;
    }

    drm_desc->nb_layers = planes;
    for (int i = 0; i < drm_desc->nb_layers; i++) {
        VkSubresourceLayout layout;
        VkImageSubresource sub = {
            .aspectMask = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT,
        };
        VkFormat plane_vkfmt = av_vkfmt_from_pixfmt(hwfc->sw_format)[i];

        drm_desc->layers[i].format    = vulkan_fmt_to_drm(plane_vkfmt);
        drm_desc->layers[i].nb_planes = 1;

        if (drm_desc->layers[i].format == DRM_FORMAT_INVALID) {
            av_log(hwfc, AV_LOG_ERROR, "Cannot map to DRM layer, unsupported!\n");
            err = AVERROR_PATCHWELCOME;
            goto end;
        }

        drm_desc->layers[i].planes[0].object_index = FFMIN(i, drm_desc->nb_objects - 1);

        if (f->tiling == VK_IMAGE_TILING_OPTIMAL)
            continue;

        vk->GetImageSubresourceLayout(hwctx->act_dev, f->img[i], &sub, &layout);
        drm_desc->layers[i].planes[0].offset = layout.offset;
        drm_desc->layers[i].planes[0].pitch  = layout.rowPitch;

        if (hwfctx->flags & AV_VK_FRAME_FLAG_CONTIGUOUS_MEMORY)
            drm_desc->layers[i].planes[0].offset += f->offset[i];
    }

    dst->width   = src->width;
    dst->height  = src->height;
    dst->data[0] = (uint8_t *)drm_desc;

    av_log(hwfc, AV_LOG_VERBOSE, "Mapped AVVkFrame to a DRM object!\n");

    return 0;

end:
    av_free(drm_desc);
    return err;
}

#if CONFIG_VAAPI
static int vulkan_map_to_vaapi(AVHWFramesContext *hwfc, AVFrame *dst,
                               const AVFrame *src, int flags)
{
    int err;
    AVFrame *tmp = av_frame_alloc();
    if (!tmp)
        return AVERROR(ENOMEM);

    tmp->format = AV_PIX_FMT_DRM_PRIME;

    err = vulkan_map_to_drm(hwfc, tmp, src, flags);
    if (err < 0)
        goto fail;

    err = av_hwframe_map(dst, tmp, flags);
    if (err < 0)
        goto fail;

    err = ff_hwframe_map_replace(dst, src);

fail:
    av_frame_free(&tmp);
    return err;
}
#endif
#endif

static int vulkan_map_from(AVHWFramesContext *hwfc, AVFrame *dst,
                           const AVFrame *src, int flags)
{
    av_unused VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;

    switch (dst->format) {
#if CONFIG_LIBDRM
    case AV_PIX_FMT_DRM_PRIME:
        if (p->vkctx.extensions & FF_VK_EXT_DRM_MODIFIER_FLAGS)
            return vulkan_map_to_drm(hwfc, dst, src, flags);
        else
            return AVERROR(ENOSYS);
#if CONFIG_VAAPI
    case AV_PIX_FMT_VAAPI:
        if (p->vkctx.extensions & FF_VK_EXT_DRM_MODIFIER_FLAGS)
            return vulkan_map_to_vaapi(hwfc, dst, src, flags);
        else
            return AVERROR(ENOSYS);
#endif
#endif
    default:
        break;
    }
    return AVERROR(ENOSYS);
}

static size_t get_req_buffer_size(VulkanDevicePriv *p, int *stride, int height)
{
    size_t size;
    *stride = FFALIGN(*stride, p->props.properties.limits.optimalBufferCopyRowPitchAlignment);
    size = height*(*stride);
    size = FFALIGN(size, p->props.properties.limits.minMemoryMapAlignment);
    return size;
}

static int transfer_image_buf(AVHWFramesContext *hwfc, AVFrame *f,
                              AVBufferRef **bufs, size_t *buf_offsets,
                              const int *buf_stride, int w,
                              int h, enum AVPixelFormat pix_fmt, int to_buf)
{
    int err;
    AVVkFrame *frame = (AVVkFrame *)f->data[0];
    VulkanFramesPriv *fp = hwfc->internal->priv;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;
    VkImageMemoryBarrier2 img_bar[AV_NUM_DATA_POINTERS];
    int nb_img_bar = 0;

    const int nb_images = ff_vk_count_images(frame);
    int pixfmt_planes = av_pix_fmt_count_planes(pix_fmt);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pix_fmt);

    VkCommandBuffer cmd_buf;
    FFVkExecContext *exec = ff_vk_exec_get(to_buf ? &fp->download_exec :
                                                    &fp->upload_exec);
    cmd_buf = exec->buf;
    ff_vk_exec_start(&p->vkctx, exec);

    err = ff_vk_exec_add_dep_buf(&p->vkctx, exec, bufs, pixfmt_planes, 1);
    if (err < 0)
        return err;

    err = ff_vk_exec_add_dep_frame(&p->vkctx, exec, f,
                                   VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                                   VK_PIPELINE_STAGE_2_TRANSFER_BIT);
    if (err < 0)
        return err;

    ff_vk_frame_barrier(&p->vkctx, exec, f, img_bar, &nb_img_bar,
                        VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                        VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR,
                        to_buf ? VK_ACCESS_TRANSFER_READ_BIT :
                                 VK_ACCESS_TRANSFER_WRITE_BIT,
                        to_buf ? VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL :
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_QUEUE_FAMILY_IGNORED);

    vk->CmdPipelineBarrier2(cmd_buf, &(VkDependencyInfo) {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pImageMemoryBarriers = img_bar,
            .imageMemoryBarrierCount = nb_img_bar,
        });

    /* Schedule a copy for each plane */
    for (int i = 0; i < pixfmt_planes; i++) {
        int idx = FFMIN(i, nb_images - 1);
        VkImageAspectFlags plane_aspect[] = { VK_IMAGE_ASPECT_COLOR_BIT,
                                              VK_IMAGE_ASPECT_PLANE_0_BIT,
                                              VK_IMAGE_ASPECT_PLANE_1_BIT,
                                              VK_IMAGE_ASPECT_PLANE_2_BIT, };

        FFVkBuffer *vkbuf = (FFVkBuffer *)bufs[i]->data;
        VkBufferImageCopy buf_reg = {
            .bufferOffset = buf_offsets[i],
            .bufferRowLength = buf_stride[i] / desc->comp[i].step,
            .imageSubresource.layerCount = 1,
            .imageSubresource.aspectMask = plane_aspect[(pixfmt_planes != nb_images) +
                                                        i*(pixfmt_planes != nb_images)],
            .imageOffset = { 0, 0, 0, },
        };

        uint32_t p_w, p_h;
        get_plane_wh(&p_w, &p_h, pix_fmt, w, h, i);

        buf_reg.bufferImageHeight = p_h;
        buf_reg.imageExtent = (VkExtent3D){ p_w, p_h, 1, };

        if (to_buf)
            vk->CmdCopyImageToBuffer(cmd_buf, frame->img[idx],
                                     img_bar[0].newLayout,
                                     vkbuf->buf,
                                     1, &buf_reg);
        else
            vk->CmdCopyBufferToImage(cmd_buf, vkbuf->buf, frame->img[idx],
                                     img_bar[0].newLayout,
                                     1, &buf_reg);
    }

    err = ff_vk_exec_submit(&p->vkctx, exec);
    if (err < 0)
        return err;

    /* Wait for the operation to complete when downloading */
    if (to_buf)
        ff_vk_exec_wait(&p->vkctx, exec);

    return 0;
}

static int vulkan_transfer_data(AVHWFramesContext *hwfc, const AVFrame *vkf,
                                const AVFrame *swf, int from)
{
    int err = 0;
    VkResult ret;
    AVHWDeviceContext *dev_ctx = hwfc->device_ctx;
    AVVulkanDeviceContext *hwctx = dev_ctx->hwctx;
    VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;
    FFVulkanFunctions *vk = &p->vkctx.vkfn;

    AVFrame tmp;
    FFVkBuffer *vkbufs[AV_NUM_DATA_POINTERS];
    AVBufferRef *bufs[AV_NUM_DATA_POINTERS] = { 0 };
    size_t buf_offsets[AV_NUM_DATA_POINTERS] = { 0 };

    uint32_t p_w, p_h;
    const int planes = av_pix_fmt_count_planes(swf->format);

    int host_mapped[AV_NUM_DATA_POINTERS] = { 0 };
    const int map_host = !!(p->vkctx.extensions & FF_VK_EXT_EXTERNAL_HOST_MEMORY);

    if ((swf->format != AV_PIX_FMT_NONE && !av_vkfmt_from_pixfmt(swf->format))) {
        av_log(hwfc, AV_LOG_ERROR, "Unsupported software frame pixel format!\n");
        return AVERROR(EINVAL);
    }

    if (swf->width > hwfc->width || swf->height > hwfc->height)
        return AVERROR(EINVAL);

    /* Create buffers */
    for (int i = 0; i < planes; i++) {
        size_t req_size;

        VkExternalMemoryBufferCreateInfo create_desc = {
            .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO,
            .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT,
        };

        VkImportMemoryHostPointerInfoEXT import_desc = {
            .sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_HOST_POINTER_INFO_EXT,
            .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT,
        };

        VkMemoryHostPointerPropertiesEXT p_props = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_HOST_POINTER_PROPERTIES_EXT,
        };

        get_plane_wh(&p_w, &p_h, swf->format, swf->width, swf->height, i);

        tmp.linesize[i] = FFABS(swf->linesize[i]);

        /* Do not map images with a negative stride */
        if (map_host && swf->linesize[i] > 0) {
            size_t offs;
            offs = (uintptr_t)swf->data[i] % p->hprops.minImportedHostPointerAlignment;
            import_desc.pHostPointer = swf->data[i] - offs;

            /* We have to compensate for the few extra bytes of padding we
             * completely ignore at the start */
            req_size = FFALIGN(offs + tmp.linesize[i] * p_h,
                               p->hprops.minImportedHostPointerAlignment);

            ret = vk->GetMemoryHostPointerPropertiesEXT(hwctx->act_dev,
                                                        import_desc.handleType,
                                                        import_desc.pHostPointer,
                                                        &p_props);
            if (ret == VK_SUCCESS && p_props.memoryTypeBits) {
                host_mapped[i] = 1;
                buf_offsets[i] = offs;
            }
        }

        if (!host_mapped[i])
            req_size = get_req_buffer_size(p, &tmp.linesize[i], p_h);

        err = ff_vk_create_avbuf(&p->vkctx, &bufs[i], req_size,
                                host_mapped[i] ? &create_desc : NULL,
                                host_mapped[i] ? &import_desc : NULL,
                                from ? VK_BUFFER_USAGE_TRANSFER_DST_BIT :
                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                (host_mapped[i] ?
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 0x0));
        if (err < 0)
            goto end;

        vkbufs[i] = (FFVkBuffer *)bufs[i]->data;
    }

    if (!from) {
        /* Map, copy image TO buffer (which then goes to the VkImage), unmap */
        if ((err = ff_vk_map_buffers(&p->vkctx, vkbufs, tmp.data, planes, 0)))
            goto end;

        for (int i = 0; i < planes; i++) {
            if (host_mapped[i])
                continue;

            get_plane_wh(&p_w, &p_h, swf->format, swf->width, swf->height, i);

            av_image_copy_plane(tmp.data[i], tmp.linesize[i],
                                (const uint8_t *)swf->data[i], swf->linesize[i],
                                FFMIN(tmp.linesize[i], FFABS(swf->linesize[i])),
                                p_h);
        }

        if ((err = ff_vk_unmap_buffers(&p->vkctx, vkbufs, planes, 1)))
            goto end;
    }

    /* Copy buffers into/from image */
    err = transfer_image_buf(hwfc, (AVFrame *)vkf, bufs, buf_offsets,
                             tmp.linesize, swf->width, swf->height, swf->format,
                             from);

    if (from) {
        /* Map, copy buffer (which came FROM the VkImage) to the frame, unmap */
        if ((err = ff_vk_map_buffers(&p->vkctx, vkbufs, tmp.data, planes, 0)))
            goto end;

        for (int i = 0; i < planes; i++) {
            if (host_mapped[i])
                continue;

            get_plane_wh(&p_w, &p_h, swf->format, swf->width, swf->height, i);

            av_image_copy_plane_uc_from(swf->data[i], swf->linesize[i],
                                        (const uint8_t *)tmp.data[i], tmp.linesize[i],
                                        FFMIN(tmp.linesize[i], FFABS(swf->linesize[i])),
                                        p_h);
        }

        if ((err = ff_vk_unmap_buffers(&p->vkctx, vkbufs, planes, 1)))
            goto end;
    }

end:
    for (int i = 0; i < planes; i++)
        av_buffer_unref(&bufs[i]);

    return err;
}

static int vulkan_transfer_data_to(AVHWFramesContext *hwfc, AVFrame *dst,
                                   const AVFrame *src)
{
    av_unused VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;

    switch (src->format) {
#if CONFIG_CUDA
    case AV_PIX_FMT_CUDA:
#ifdef _WIN32
        if ((p->vkctx.extensions & FF_VK_EXT_EXTERNAL_WIN32_MEMORY) &&
            (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_WIN32_SEM))
#else
        if ((p->vkctx.extensions & FF_VK_EXT_EXTERNAL_FD_MEMORY) &&
            (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_FD_SEM))
#endif
            return vulkan_transfer_data_from_cuda(hwfc, dst, src);
#endif
    default:
        if (src->hw_frames_ctx)
            return AVERROR(ENOSYS);
        else
            return vulkan_transfer_data(hwfc, dst, src, 0);
    }
}

#if CONFIG_CUDA
static int vulkan_transfer_data_to_cuda(AVHWFramesContext *hwfc, AVFrame *dst,
                                        const AVFrame *src)
{
    int err;
    CUcontext dummy;
    AVVkFrame *dst_f;
    AVVkFrameInternal *dst_int;
    VulkanFramesPriv *fp = hwfc->internal->priv;
    const int planes = av_pix_fmt_count_planes(hwfc->sw_format);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(hwfc->sw_format);

    AVHWFramesContext *cuda_fc = (AVHWFramesContext*)dst->hw_frames_ctx->data;
    AVHWDeviceContext *cuda_cu = cuda_fc->device_ctx;
    AVCUDADeviceContext *cuda_dev = cuda_cu->hwctx;
    AVCUDADeviceContextInternal *cu_internal = cuda_dev->internal;
    CudaFunctions *cu = cu_internal->cuda_dl;
    CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS s_w_par[AV_NUM_DATA_POINTERS] = { 0 };
    CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS s_s_par[AV_NUM_DATA_POINTERS] = { 0 };

    dst_f = (AVVkFrame *)src->data[0];

    err = prepare_frame(hwfc, &fp->upload_exec, dst_f, PREP_MODE_EXTERNAL_EXPORT);
    if (err < 0)
        return err;

    err = CHECK_CU(cu->cuCtxPushCurrent(cuda_dev->cuda_ctx));
    if (err < 0)
        return err;

    err = vulkan_export_to_cuda(hwfc, dst->hw_frames_ctx, src);
    if (err < 0) {
        CHECK_CU(cu->cuCtxPopCurrent(&dummy));
        return err;
    }

    dst_int = dst_f->internal;

    for (int i = 0; i < planes; i++) {
        s_w_par[i].params.fence.value = dst_f->sem_value[i] + 0;
        s_s_par[i].params.fence.value = dst_f->sem_value[i] + 1;
    }

    err = CHECK_CU(cu->cuWaitExternalSemaphoresAsync(dst_int->cu_sem, s_w_par,
                                                     planes, cuda_dev->stream));
    if (err < 0)
        goto fail;

    for (int i = 0; i < planes; i++) {
        CUDA_MEMCPY2D cpy = {
            .dstMemoryType = CU_MEMORYTYPE_DEVICE,
            .dstDevice     = (CUdeviceptr)dst->data[i],
            .dstPitch      = dst->linesize[i],
            .dstY          = 0,

            .srcMemoryType = CU_MEMORYTYPE_ARRAY,
            .srcArray      = dst_int->cu_array[i],
        };

        int w, h;
        get_plane_wh(&w, &h, hwfc->sw_format, hwfc->width, hwfc->height, i);

        cpy.WidthInBytes = w * desc->comp[i].step;
        cpy.Height = h;

        err = CHECK_CU(cu->cuMemcpy2DAsync(&cpy, cuda_dev->stream));
        if (err < 0)
            goto fail;
    }

    err = CHECK_CU(cu->cuSignalExternalSemaphoresAsync(dst_int->cu_sem, s_s_par,
                                                       planes, cuda_dev->stream));
    if (err < 0)
        goto fail;

    for (int i = 0; i < planes; i++)
        dst_f->sem_value[i]++;

    CHECK_CU(cu->cuCtxPopCurrent(&dummy));

    av_log(hwfc, AV_LOG_VERBOSE, "Transferred Vulkan image to CUDA!\n");

    return prepare_frame(hwfc, &fp->upload_exec, dst_f, PREP_MODE_EXTERNAL_IMPORT);

fail:
    CHECK_CU(cu->cuCtxPopCurrent(&dummy));
    vulkan_free_internal(dst_f);
    av_buffer_unref(&dst->buf[0]);
    return err;
}
#endif

static int vulkan_transfer_data_from(AVHWFramesContext *hwfc, AVFrame *dst,
                                     const AVFrame *src)
{
    av_unused VulkanDevicePriv *p = hwfc->device_ctx->internal->priv;

    switch (dst->format) {
#if CONFIG_CUDA
    case AV_PIX_FMT_CUDA:
#ifdef _WIN32
        if ((p->vkctx.extensions & FF_VK_EXT_EXTERNAL_WIN32_MEMORY) &&
            (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_WIN32_SEM))
#else
        if ((p->vkctx.extensions & FF_VK_EXT_EXTERNAL_FD_MEMORY) &&
            (p->vkctx.extensions & FF_VK_EXT_EXTERNAL_FD_SEM))
#endif
            return vulkan_transfer_data_to_cuda(hwfc, dst, src);
#endif
    default:
        if (dst->hw_frames_ctx)
            return AVERROR(ENOSYS);
        else
            return vulkan_transfer_data(hwfc, src, dst, 1);
    }
}

static int vulkan_frames_derive_to(AVHWFramesContext *dst_fc,
                                   AVHWFramesContext *src_fc, int flags)
{
    return vulkan_frames_init(dst_fc);
}

AVVkFrame *av_vk_frame_alloc(void)
{
    int err;
    AVVkFrame *f = av_mallocz(sizeof(AVVkFrame));
    if (!f)
        return NULL;

    f->internal = av_mallocz(sizeof(*f->internal));
    if (!f->internal) {
        av_free(f);
        return NULL;
    }

    err = pthread_mutex_init(&f->internal->update_mutex, NULL);
    if (err != 0) {
        av_free(f->internal);
        av_free(f);
        return NULL;
    }

    return f;
}

const HWContextType ff_hwcontext_type_vulkan = {
    .type                   = AV_HWDEVICE_TYPE_VULKAN,
    .name                   = "Vulkan",

    .device_hwctx_size      = sizeof(AVVulkanDeviceContext),
    .device_priv_size       = sizeof(VulkanDevicePriv),
    .frames_hwctx_size      = sizeof(AVVulkanFramesContext),
    .frames_priv_size       = sizeof(VulkanFramesPriv),

    .device_init            = &vulkan_device_init,
    .device_uninit          = &vulkan_device_uninit,
    .device_create          = &vulkan_device_create,
    .device_derive          = &vulkan_device_derive,

    .frames_get_constraints = &vulkan_frames_get_constraints,
    .frames_init            = vulkan_frames_init,
    .frames_get_buffer      = vulkan_get_buffer,
    .frames_uninit          = vulkan_frames_uninit,

    .transfer_get_formats   = vulkan_transfer_get_formats,
    .transfer_data_to       = vulkan_transfer_data_to,
    .transfer_data_from     = vulkan_transfer_data_from,

    .map_to                 = vulkan_map_to,
    .map_from               = vulkan_map_from,
    .frames_derive_to       = &vulkan_frames_derive_to,

    .pix_fmts = (const enum AVPixelFormat []) {
        AV_PIX_FMT_VULKAN,
        AV_PIX_FMT_NONE
    },
};
