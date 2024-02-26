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

#include "avassert.h"

#include "vulkan.h"
#include "vulkan_loader.h"

const VkComponentMapping ff_comp_identity_map = {
    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
};

/* Converts return values to strings */
const char *ff_vk_ret2str(VkResult res)
{
#define CASE(VAL) case VAL: return #VAL
    switch (res) {
    CASE(VK_SUCCESS);
    CASE(VK_NOT_READY);
    CASE(VK_TIMEOUT);
    CASE(VK_EVENT_SET);
    CASE(VK_EVENT_RESET);
    CASE(VK_INCOMPLETE);
    CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
    CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
    CASE(VK_ERROR_INITIALIZATION_FAILED);
    CASE(VK_ERROR_DEVICE_LOST);
    CASE(VK_ERROR_MEMORY_MAP_FAILED);
    CASE(VK_ERROR_LAYER_NOT_PRESENT);
    CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
    CASE(VK_ERROR_FEATURE_NOT_PRESENT);
    CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
    CASE(VK_ERROR_TOO_MANY_OBJECTS);
    CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
    CASE(VK_ERROR_FRAGMENTED_POOL);
    CASE(VK_ERROR_UNKNOWN);
    CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
    CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
    CASE(VK_ERROR_FRAGMENTATION);
    CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
    CASE(VK_PIPELINE_COMPILE_REQUIRED);
    CASE(VK_ERROR_SURFACE_LOST_KHR);
    CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
    CASE(VK_SUBOPTIMAL_KHR);
    CASE(VK_ERROR_OUT_OF_DATE_KHR);
    CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
    CASE(VK_ERROR_VALIDATION_FAILED_EXT);
    CASE(VK_ERROR_INVALID_SHADER_NV);
    CASE(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
    CASE(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
    CASE(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
    CASE(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
    CASE(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
    CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
    CASE(VK_ERROR_NOT_PERMITTED_KHR);
    CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
    CASE(VK_THREAD_IDLE_KHR);
    CASE(VK_THREAD_DONE_KHR);
    CASE(VK_OPERATION_DEFERRED_KHR);
    CASE(VK_OPERATION_NOT_DEFERRED_KHR);
    default: return "Unknown error";
    }
#undef CASE
}

int ff_vk_load_props(FFVulkanContext *s)
{
    FFVulkanFunctions *vk = &s->vkfn;

    s->hprops = (VkPhysicalDeviceExternalMemoryHostPropertiesEXT) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTERNAL_MEMORY_HOST_PROPERTIES_EXT,
    };
    s->coop_matrix_props = (VkPhysicalDeviceCooperativeMatrixPropertiesKHR) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_PROPERTIES_KHR,
        .pNext = &s->hprops,
    };
    s->subgroup_props = (VkPhysicalDeviceSubgroupSizeControlProperties) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES,
        .pNext = &s->coop_matrix_props,
    };
    s->desc_buf_props = (VkPhysicalDeviceDescriptorBufferPropertiesEXT) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT,
        .pNext = &s->subgroup_props,
    };
    s->driver_props = (VkPhysicalDeviceDriverProperties) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES,
        .pNext = &s->desc_buf_props,
    };
    s->props = (VkPhysicalDeviceProperties2) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
        .pNext = &s->driver_props,
    };

    s->atomic_float_feats = (VkPhysicalDeviceShaderAtomicFloatFeaturesEXT) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT,
    };
    s->feats_12 = (VkPhysicalDeviceVulkan12Features) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &s->atomic_float_feats,
    };
    s->feats = (VkPhysicalDeviceFeatures2) {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &s->feats_12,
    };

    vk->GetPhysicalDeviceProperties2(s->hwctx->phys_dev, &s->props);
    vk->GetPhysicalDeviceMemoryProperties(s->hwctx->phys_dev, &s->mprops);
    vk->GetPhysicalDeviceFeatures2(s->hwctx->phys_dev, &s->feats);

    if (s->qf_props)
        return 0;

    vk->GetPhysicalDeviceQueueFamilyProperties2(s->hwctx->phys_dev, &s->tot_nb_qfs, NULL);

    s->qf_props = av_calloc(s->tot_nb_qfs, sizeof(*s->qf_props));
    if (!s->qf_props)
        return AVERROR(ENOMEM);

    s->query_props = av_calloc(s->tot_nb_qfs, sizeof(*s->query_props));
    if (!s->qf_props) {
        av_freep(&s->qf_props);
        return AVERROR(ENOMEM);
    }

    s->video_props = av_calloc(s->tot_nb_qfs, sizeof(*s->video_props));
    if (!s->video_props) {
        av_freep(&s->qf_props);
        av_freep(&s->query_props);
        return AVERROR(ENOMEM);
    }

    for (uint32_t i = 0; i < s->tot_nb_qfs; i++) {
        s->query_props[i] = (VkQueueFamilyQueryResultStatusPropertiesKHR) {
            .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_QUERY_RESULT_STATUS_PROPERTIES_KHR,
        };
        s->video_props[i] = (VkQueueFamilyVideoPropertiesKHR) {
            .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR,
            .pNext = &s->query_props[i],
        };
        s->qf_props[i] = (VkQueueFamilyProperties2) {
            .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2,
            .pNext = &s->video_props[i],
        };
    }

    vk->GetPhysicalDeviceQueueFamilyProperties2(s->hwctx->phys_dev, &s->tot_nb_qfs, s->qf_props);

    if (s->extensions & FF_VK_EXT_COOP_MATRIX) {
        vk->GetPhysicalDeviceCooperativeMatrixPropertiesKHR(s->hwctx->phys_dev,
                                                            &s->coop_mat_props_nb, NULL);

        if (s->coop_mat_props_nb) {
            s->coop_mat_props = av_malloc_array(s->coop_mat_props_nb,
                                                sizeof(VkCooperativeMatrixPropertiesKHR));
            for (int i = 0; i < s->coop_mat_props_nb; i++) {
                s->coop_mat_props[i] = (VkCooperativeMatrixPropertiesKHR) {
                    .sType = VK_STRUCTURE_TYPE_COOPERATIVE_MATRIX_PROPERTIES_KHR,
                };
            }

            vk->GetPhysicalDeviceCooperativeMatrixPropertiesKHR(s->hwctx->phys_dev,
                                                                &s->coop_mat_props_nb,
                                                                s->coop_mat_props);
        }
    }

    return 0;
}

static int vk_qf_get_index(FFVulkanContext *s, VkQueueFlagBits dev_family, int *nb)
{
    int ret, num;

    switch (dev_family) {
    case VK_QUEUE_GRAPHICS_BIT:
        ret = s->hwctx->queue_family_index;
        num = s->hwctx->nb_graphics_queues;
        break;
    case VK_QUEUE_COMPUTE_BIT:
        ret = s->hwctx->queue_family_comp_index;
        num = s->hwctx->nb_comp_queues;
        break;
    case VK_QUEUE_TRANSFER_BIT:
        ret = s->hwctx->queue_family_tx_index;
        num = s->hwctx->nb_tx_queues;
        break;
    case VK_QUEUE_VIDEO_ENCODE_BIT_KHR:
        ret = s->hwctx->queue_family_encode_index;
        num = s->hwctx->nb_encode_queues;
        break;
    case VK_QUEUE_VIDEO_DECODE_BIT_KHR:
        ret = s->hwctx->queue_family_decode_index;
        num = s->hwctx->nb_decode_queues;
        break;
    default:
        av_assert0(0); /* Should never happen */
    }

    if (nb)
        *nb = num;

    return ret;
}

int ff_vk_qf_init(FFVulkanContext *s, FFVkQueueFamilyCtx *qf,
                  VkQueueFlagBits dev_family)
{
    /* Fill in queue families from context if not done yet */
    if (!s->nb_qfs) {
        s->nb_qfs = 0;

        /* Simply fills in all unique queues into s->qfs */
        if (s->hwctx->queue_family_index >= 0)
            s->qfs[s->nb_qfs++] = s->hwctx->queue_family_index;
        if (!s->nb_qfs || s->qfs[0] != s->hwctx->queue_family_tx_index)
            s->qfs[s->nb_qfs++] = s->hwctx->queue_family_tx_index;
        if (!s->nb_qfs || (s->qfs[0] != s->hwctx->queue_family_comp_index &&
                           s->qfs[1] != s->hwctx->queue_family_comp_index))
            s->qfs[s->nb_qfs++] = s->hwctx->queue_family_comp_index;
        if (s->hwctx->queue_family_decode_index >= 0 &&
             (s->qfs[0] != s->hwctx->queue_family_decode_index &&
              s->qfs[1] != s->hwctx->queue_family_decode_index &&
              s->qfs[2] != s->hwctx->queue_family_decode_index))
            s->qfs[s->nb_qfs++] = s->hwctx->queue_family_decode_index;
        if (s->hwctx->queue_family_encode_index >= 0 &&
             (s->qfs[0] != s->hwctx->queue_family_encode_index &&
              s->qfs[1] != s->hwctx->queue_family_encode_index &&
              s->qfs[2] != s->hwctx->queue_family_encode_index &&
              s->qfs[3] != s->hwctx->queue_family_encode_index))
            s->qfs[s->nb_qfs++] = s->hwctx->queue_family_encode_index;
    }

    return (qf->queue_family = vk_qf_get_index(s, dev_family, &qf->nb_queues));
}

void ff_vk_exec_pool_free(FFVulkanContext *s, FFVkExecPool *pool)
{
    FFVulkanFunctions *vk = &s->vkfn;

    for (int i = 0; i < pool->pool_size; i++) {
        FFVkExecContext *e = &pool->contexts[i];

        if (e->fence) {
            vk->WaitForFences(s->hwctx->act_dev, 1, &e->fence, VK_TRUE, UINT64_MAX);
            vk->DestroyFence(s->hwctx->act_dev, e->fence, s->hwctx->alloc);
        }
        pthread_mutex_destroy(&e->lock);

        ff_vk_exec_discard_deps(s, e);

        av_free(e->frame_deps);
        av_free(e->buf_deps);
        av_free(e->queue_family_dst);
        av_free(e->layout_dst);
        av_free(e->access_dst);
        av_free(e->frame_update);
        av_free(e->frame_locked);
        av_free(e->sem_sig);
        av_free(e->sem_sig_val_dst);
        av_free(e->sem_wait);
    }

    if (pool->cmd_bufs)
        vk->FreeCommandBuffers(s->hwctx->act_dev, pool->cmd_buf_pool,
                               pool->pool_size, pool->cmd_bufs);
    if (pool->cmd_buf_pool)
        vk->DestroyCommandPool(s->hwctx->act_dev, pool->cmd_buf_pool, s->hwctx->alloc);
    if (pool->query_pool)
        vk->DestroyQueryPool(s->hwctx->act_dev, pool->query_pool, s->hwctx->alloc);

    av_free(pool->query_data);
    av_free(pool->cmd_bufs);
    av_free(pool->contexts);
}

int ff_vk_exec_pool_init(FFVulkanContext *s, FFVkQueueFamilyCtx *qf,
                         FFVkExecPool *pool, int nb_contexts,
                         int nb_queries, VkQueryType query_type, int query_64bit,
                         const void *query_create_pnext)
{
    int err;
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;

    VkCommandPoolCreateInfo cqueue_create;
    VkCommandBufferAllocateInfo cbuf_create;

    /* Create command pool */
    cqueue_create = (VkCommandPoolCreateInfo) {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags              = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                              VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex   = qf->queue_family,
    };
    ret = vk->CreateCommandPool(s->hwctx->act_dev, &cqueue_create,
                                s->hwctx->alloc, &pool->cmd_buf_pool);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Command pool creation failure: %s\n",
               ff_vk_ret2str(ret));
        err = AVERROR_EXTERNAL;
        goto fail;
    }

    /* Allocate space for command buffers */
    pool->cmd_bufs = av_malloc(nb_contexts*sizeof(*pool->cmd_bufs));
    if (!pool->cmd_bufs) {
        err = AVERROR(ENOMEM);
        goto fail;
    }

    /* Allocate command buffer */
    cbuf_create = (VkCommandBufferAllocateInfo) {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool        = pool->cmd_buf_pool,
        .commandBufferCount = nb_contexts,
    };
    ret = vk->AllocateCommandBuffers(s->hwctx->act_dev, &cbuf_create,
                                     pool->cmd_bufs);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Command buffer alloc failure: %s\n",
               ff_vk_ret2str(ret));
        err = AVERROR_EXTERNAL;
        goto fail;
    }

    /* Query pool */
    if (nb_queries) {
        VkQueryPoolCreateInfo query_pool_info = {
            .sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
            .pNext = query_create_pnext,
            .queryType = query_type,
            .queryCount = nb_queries*nb_contexts,
        };
        ret = vk->CreateQueryPool(s->hwctx->act_dev, &query_pool_info,
                                  s->hwctx->alloc, &pool->query_pool);
        if (ret != VK_SUCCESS) {
            av_log(s, AV_LOG_ERROR, "Query pool alloc failure: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR_EXTERNAL;
            goto fail;
        }

        pool->nb_queries = nb_queries;
        pool->query_status_stride = 2;
        pool->query_results = nb_queries;
        pool->query_statuses = 0; /* if radv supports it, nb_queries; */

#if 0 /* CONFIG_VULKAN_ENCODE */
        /* Video encode quieries produce two results per query */
        if (query_type == VK_QUERY_TYPE_VIDEO_ENCODE_FEEDBACK_KHR) {
            pool->query_status_stride = 3; /* skip,skip,result,skip,skip,result */
            pool->query_results *= 2;
        } else
#endif
        if (query_type == VK_QUERY_TYPE_RESULT_STATUS_ONLY_KHR) {
            pool->query_status_stride = 1;
            pool->query_results = 0;
            pool->query_statuses = nb_queries;
        }

        pool->qd_size = (pool->query_results + pool->query_statuses)*(query_64bit ? 8 : 4);

        /* Allocate space for the query data */
        pool->query_data = av_calloc(nb_contexts, pool->qd_size);
        if (!pool->query_data) {
            err = AVERROR(ENOMEM);
            goto fail;
        }
    }

    /* Allocate space for the contexts */
    pool->contexts = av_calloc(nb_contexts, sizeof(*pool->contexts));
    if (!pool->contexts) {
        err = AVERROR(ENOMEM);
        goto fail;
    }

    pool->pool_size = nb_contexts;

    /* Init contexts */
    for (int i = 0; i < pool->pool_size; i++) {
        FFVkExecContext *e = &pool->contexts[i];
        VkFenceCreateInfo fence_create = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        /* Mutex */
        err = pthread_mutex_init(&e->lock, NULL);
        if (err != 0)
            return AVERROR(err);

        /* Fence */
        ret = vk->CreateFence(s->hwctx->act_dev, &fence_create, s->hwctx->alloc,
                              &e->fence);
        if (ret != VK_SUCCESS) {
            av_log(s, AV_LOG_ERROR, "Failed to create submission fence: %s\n",
                   ff_vk_ret2str(ret));
            return AVERROR_EXTERNAL;
        }

        e->idx = i;
        e->parent = pool;

        /* Query data */
        e->query_data = ((uint8_t *)pool->query_data) + pool->qd_size*i;
        e->query_idx = nb_queries*i;

        /* Command buffer */
        e->buf = pool->cmd_bufs[i];

        /* Queue index distribution */
        e->qi = i % qf->nb_queues;
        e->qf = qf->queue_family;
        vk->GetDeviceQueue(s->hwctx->act_dev, qf->queue_family,
                           e->qi, &e->queue);
    }

    return 0;

fail:
    ff_vk_exec_pool_free(s, pool);
    return err;
}

VkResult ff_vk_exec_get_query(FFVulkanContext *s, FFVkExecContext *e,
                              void **data, int64_t *status)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    const FFVkExecPool *pool = e->parent;

    int32_t *res32 = e->query_data;
    int64_t *res64 = e->query_data;
    int64_t res = 0;
    VkQueryResultFlags qf = 0;

    if (!e->had_submission)
        return VK_NOT_READY;

    qf |= pool->query_64bit ?
          VK_QUERY_RESULT_64_BIT : 0x0;
    qf |= pool->query_statuses ?
          VK_QUERY_RESULT_WITH_STATUS_BIT_KHR : 0x0;

    ret = vk->GetQueryPoolResults(s->hwctx->act_dev, pool->query_pool,
                                  e->query_idx,
                                  pool->nb_queries,
                                  pool->qd_size, e->query_data,
                                  pool->query_64bit ? 8 : 4, qf);
    if (ret != VK_SUCCESS)
        return ret;

    if (pool->query_statuses && pool->query_64bit) {
        for (int i = 0; i < pool->query_statuses; i++) {
            res = (res64[i] < res) || (res >= 0 && res64[i] > res) ?
                  res64[i] : res;
            res64 += pool->query_status_stride;
        }
    } else if (pool->query_statuses) {
        for (int i = 0; i < pool->query_statuses; i++) {
            res = (res32[i] < res) || (res >= 0 && res32[i] > res) ?
                  res32[i] : res;
            res32 += pool->query_status_stride;
        }
    }

    if (data)
        *data = e->query_data;
    if (status)
        *status = res;

    return VK_SUCCESS;
}

FFVkExecContext *ff_vk_exec_get(FFVkExecPool *pool)
{
    uint32_t idx = pool->idx++;
    idx %= pool->pool_size;
    return &pool->contexts[idx];
}

void ff_vk_exec_wait(FFVulkanContext *s, FFVkExecContext *e)
{
    FFVulkanFunctions *vk = &s->vkfn;
    pthread_mutex_lock(&e->lock);
    vk->WaitForFences(s->hwctx->act_dev, 1, &e->fence, VK_TRUE, UINT64_MAX);
    ff_vk_exec_discard_deps(s, e);
    pthread_mutex_unlock(&e->lock);
}

int ff_vk_exec_start(FFVulkanContext *s, FFVkExecContext *e)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    const FFVkExecPool *pool = e->parent;

    VkCommandBufferBeginInfo cmd_start = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };

    /* Wait for the fence to be signalled */
    vk->WaitForFences(s->hwctx->act_dev, 1, &e->fence, VK_TRUE, UINT64_MAX);

    /* vkResetFences is defined as being host-synchronized */
    pthread_mutex_lock(&e->lock);
    vk->ResetFences(s->hwctx->act_dev, 1, &e->fence);
    pthread_mutex_unlock(&e->lock);

    /* Discard queue dependencies */
    ff_vk_exec_discard_deps(s, e);

    ret = vk->BeginCommandBuffer(e->buf, &cmd_start);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Failed to start command recoding: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    if (pool->nb_queries)
        vk->CmdResetQueryPool(e->buf, pool->query_pool,
                              e->query_idx, pool->nb_queries);

    return 0;
}

void ff_vk_exec_discard_deps(FFVulkanContext *s, FFVkExecContext *e)
{
    for (int j = 0; j < e->nb_buf_deps; j++)
        av_buffer_unref(&e->buf_deps[j]);
    e->nb_buf_deps = 0;

    for (int j = 0; j < e->nb_frame_deps; j++) {
        AVFrame *f = e->frame_deps[j];
        if (e->frame_locked[j]) {
            AVHWFramesContext *hwfc = (AVHWFramesContext *)f->hw_frames_ctx->data;
            AVVulkanFramesContext *vkfc = hwfc->hwctx;
            AVVkFrame *vkf = (AVVkFrame *)f->data[0];
            vkfc->unlock_frame(hwfc, vkf);
            e->frame_locked[j] = 0;
        }
        e->frame_update[j] = 0;
        if (f->buf[0])
            av_frame_free(&e->frame_deps[j]);
    }
    e->nb_frame_deps = 0;

    e->sem_wait_cnt = 0;
    e->sem_sig_cnt = 0;
    e->sem_sig_val_dst_cnt = 0;
}

int ff_vk_exec_add_dep_buf(FFVulkanContext *s, FFVkExecContext *e,
                           AVBufferRef **deps, int nb_deps, int ref)
{
    AVBufferRef **dst = av_fast_realloc(e->buf_deps, &e->buf_deps_alloc_size,
                                        (e->nb_buf_deps + nb_deps) * sizeof(*dst));
    if (!dst) {
        ff_vk_exec_discard_deps(s, e);
        return AVERROR(ENOMEM);
    }

    e->buf_deps = dst;

    for (int i = 0; i < nb_deps; i++) {
        e->buf_deps[e->nb_buf_deps] = ref ? av_buffer_ref(deps[i]) : deps[i];
        if (!e->buf_deps[e->nb_buf_deps]) {
            ff_vk_exec_discard_deps(s, e);
            return AVERROR(ENOMEM);
        }
        e->nb_buf_deps++;
    }

    return 0;
}

int ff_vk_exec_add_dep_frame(FFVulkanContext *s, FFVkExecContext *e, AVFrame *f,
                             VkPipelineStageFlagBits2 wait_stage,
                             VkPipelineStageFlagBits2 signal_stage)
{
    uint8_t *frame_locked;
    uint8_t *frame_update;
    AVFrame **frame_deps;
    VkImageLayout *layout_dst;
    uint32_t *queue_family_dst;
    VkAccessFlagBits *access_dst;

    AVHWFramesContext *hwfc = (AVHWFramesContext *)f->hw_frames_ctx->data;
    AVVulkanFramesContext *vkfc = hwfc->hwctx;
    AVVkFrame *vkf = (AVVkFrame *)f->data[0];
    int nb_images = ff_vk_count_images(vkf);

    /* Don't add duplicates */
    for (int i = 0; i < e->nb_frame_deps; i++)
        if (e->frame_deps[i]->data[0] == f->data[0])
            return 1;

#define ARR_REALLOC(str, arr, alloc_s, cnt)                               \
    do {                                                                  \
        arr = av_fast_realloc(str->arr, alloc_s, (cnt + 1)*sizeof(*arr)); \
        if (!arr) {                                                       \
            ff_vk_exec_discard_deps(s, e);                                \
            return AVERROR(ENOMEM);                                       \
        }                                                                 \
        str->arr = arr;                                                   \
    } while (0)

    ARR_REALLOC(e, layout_dst,       &e->layout_dst_alloc,       e->nb_frame_deps);
    ARR_REALLOC(e, queue_family_dst, &e->queue_family_dst_alloc, e->nb_frame_deps);
    ARR_REALLOC(e, access_dst,       &e->access_dst_alloc,       e->nb_frame_deps);

    ARR_REALLOC(e, frame_locked, &e->frame_locked_alloc_size, e->nb_frame_deps);
    ARR_REALLOC(e, frame_update, &e->frame_update_alloc_size, e->nb_frame_deps);
    ARR_REALLOC(e, frame_deps,   &e->frame_deps_alloc_size,   e->nb_frame_deps);

    e->frame_deps[e->nb_frame_deps] = f->buf[0] ? av_frame_clone(f) : f;
    if (!e->frame_deps[e->nb_frame_deps]) {
        ff_vk_exec_discard_deps(s, e);
        return AVERROR(ENOMEM);
    }

    vkfc->lock_frame(hwfc, vkf);
    e->frame_locked[e->nb_frame_deps] = 1;
    e->frame_update[e->nb_frame_deps] = 0;
    e->nb_frame_deps++;

    for (int i = 0; i < nb_images; i++) {
        VkSemaphoreSubmitInfo *sem_wait;
        VkSemaphoreSubmitInfo *sem_sig;
        uint64_t **sem_sig_val_dst;

        ARR_REALLOC(e, sem_wait, &e->sem_wait_alloc, e->sem_wait_cnt);
        ARR_REALLOC(e, sem_sig, &e->sem_sig_alloc, e->sem_sig_cnt);
        ARR_REALLOC(e, sem_sig_val_dst, &e->sem_sig_val_dst_alloc, e->sem_sig_val_dst_cnt);

        e->sem_wait[e->sem_wait_cnt++] = (VkSemaphoreSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = vkf->sem[i],
            .value = vkf->sem_value[i],
            .stageMask = wait_stage,
        };

        e->sem_sig[e->sem_sig_cnt++] = (VkSemaphoreSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = vkf->sem[i],
            .value = vkf->sem_value[i] + 1,
            .stageMask = signal_stage,
        };

        e->sem_sig_val_dst[e->sem_sig_val_dst_cnt] = &vkf->sem_value[i];
        e->sem_sig_val_dst_cnt++;
    }

    return 0;
}

void ff_vk_exec_update_frame(FFVulkanContext *s, FFVkExecContext *e, AVFrame *f,
                             VkImageMemoryBarrier2 *bar, uint32_t *nb_img_bar)
{
    int i;
    for (i = 0; i < e->nb_frame_deps; i++)
        if (e->frame_deps[i]->data[0] == f->data[0])
            break;
    av_assert0(i < e->nb_frame_deps);

    /* Don't update duplicates */
    if (nb_img_bar && !e->frame_update[i])
        (*nb_img_bar)++;

    e->queue_family_dst[i] = bar->dstQueueFamilyIndex;
    e->access_dst[i] = bar->dstAccessMask;
    e->layout_dst[i] = bar->newLayout;
    e->frame_update[i] = 1;
}

int ff_vk_exec_mirror_sem_value(FFVulkanContext *s, FFVkExecContext *e,
                                VkSemaphore *dst, uint64_t *dst_val,
                                AVFrame *f)
{
    uint64_t **sem_sig_val_dst;
    AVVkFrame *vkf = (AVVkFrame *)f->data[0];

    /* Reject unknown frames */
    int i;
    for (i = 0; i < e->nb_frame_deps; i++)
        if (e->frame_deps[i]->data[0] == f->data[0])
            break;
    if (i == e->nb_frame_deps)
        return AVERROR(EINVAL);

    ARR_REALLOC(e, sem_sig_val_dst, &e->sem_sig_val_dst_alloc, e->sem_sig_val_dst_cnt);

    *dst     = vkf->sem[0];
    *dst_val = vkf->sem_value[0];

    e->sem_sig_val_dst[e->sem_sig_val_dst_cnt] = dst_val;
    e->sem_sig_val_dst_cnt++;

    return 0;
}

int ff_vk_exec_submit(FFVulkanContext *s, FFVkExecContext *e)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    VkCommandBufferSubmitInfo cmd_buf_info = (VkCommandBufferSubmitInfo) {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = e->buf,
    };
    VkSubmitInfo2 submit_info = (VkSubmitInfo2) {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pCommandBufferInfos = &cmd_buf_info,
        .commandBufferInfoCount = 1,
        .pWaitSemaphoreInfos = e->sem_wait,
        .waitSemaphoreInfoCount = e->sem_wait_cnt,
        .pSignalSemaphoreInfos = e->sem_sig,
        .signalSemaphoreInfoCount = e->sem_sig_cnt,
    };

    ret = vk->EndCommandBuffer(e->buf);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Unable to finish command buffer: %s\n",
               ff_vk_ret2str(ret));
        ff_vk_exec_discard_deps(s, e);
        return AVERROR_EXTERNAL;
    }

    s->hwctx->lock_queue(s->device, e->qf, e->qi);
    ret = vk->QueueSubmit2(e->queue, 1, &submit_info, e->fence);
    s->hwctx->unlock_queue(s->device, e->qf, e->qi);

    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Unable to submit command buffer: %s\n",
               ff_vk_ret2str(ret));
        ff_vk_exec_discard_deps(s, e);
        return AVERROR_EXTERNAL;
    }

    for (int i = 0; i < e->sem_sig_val_dst_cnt; i++)
        *e->sem_sig_val_dst[i] += 1;

    /* Unlock all frames */
    for (int j = 0; j < e->nb_frame_deps; j++) {
        if (e->frame_locked[j]) {
            AVFrame *f = e->frame_deps[j];
            AVHWFramesContext *hwfc = (AVHWFramesContext *)f->hw_frames_ctx->data;
            AVVulkanFramesContext *vkfc = hwfc->hwctx;
            AVVkFrame *vkf = (AVVkFrame *)f->data[0];

            if (e->frame_update[j]) {
                int nb_images = ff_vk_count_images(vkf);
                for (int i = 0; i < nb_images; i++) {
                    vkf->layout[i] = e->layout_dst[j];
                    vkf->access[i] = e->access_dst[j];
                    vkf->queue_family[i] = e->queue_family_dst[j];
                }
            }
            vkfc->unlock_frame(hwfc, vkf);
            e->frame_locked[j] = 0;
        }
    }

    e->had_submission = 1;

    return 0;
}

int ff_vk_alloc_mem(FFVulkanContext *s, VkMemoryRequirements *req,
                    VkMemoryPropertyFlagBits req_flags, void *alloc_extension,
                    VkMemoryPropertyFlagBits *mem_flags, VkDeviceMemory *mem)
{
    VkResult ret;
    int index = -1;
    FFVulkanFunctions *vk = &s->vkfn;

    VkMemoryAllocateInfo alloc_info = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = alloc_extension,
    };

    /* Align if we need to */
    if ((req_flags != UINT32_MAX) && req_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
        req->size = FFALIGN(req->size, s->props.properties.limits.minMemoryMapAlignment);

    alloc_info.allocationSize = req->size;

    /* The vulkan spec requires memory types to be sorted in the "optimal"
     * order, so the first matching type we find will be the best/fastest one */
    for (int i = 0; i < s->mprops.memoryTypeCount; i++) {
        /* The memory type must be supported by the requirements (bitfield) */
        if (!(req->memoryTypeBits & (1 << i)))
            continue;

        /* The memory type flags must include our properties */
        if ((req_flags != UINT32_MAX) &&
            ((s->mprops.memoryTypes[i].propertyFlags & req_flags) != req_flags))
            continue;

        /* Found a suitable memory type */
        index = i;
        break;
    }

    if (index < 0) {
        av_log(s->device, AV_LOG_ERROR, "No memory type found for flags 0x%x\n",
               req_flags);
        return AVERROR(EINVAL);
    }

    alloc_info.memoryTypeIndex = index;

    ret = vk->AllocateMemory(s->hwctx->act_dev, &alloc_info,
                             s->hwctx->alloc, mem);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Failed to allocate memory: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR(ENOMEM);
    }

    if (mem_flags)
        *mem_flags |= s->mprops.memoryTypes[index].propertyFlags;

    return 0;
}

int ff_vk_create_buf(FFVulkanContext *s, FFVkBuffer *buf, size_t size,
                     void *pNext, void *alloc_pNext,
                     VkBufferUsageFlags usage, VkMemoryPropertyFlagBits flags)
{
    int err;
    VkResult ret;
    int use_ded_mem;
    FFVulkanFunctions *vk = &s->vkfn;

    VkBufferCreateInfo buf_spawn = {
        .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext       = pNext,
        .usage       = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .size        = size, /* Gets FFALIGNED during alloc if host visible
                                but should be ok */
    };

    VkMemoryAllocateFlagsInfo alloc_flags = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
        .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
    };
    VkBufferMemoryRequirementsInfo2 req_desc = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
    };
    VkMemoryDedicatedAllocateInfo ded_alloc = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
        .pNext = alloc_pNext,
    };
    VkMemoryDedicatedRequirements ded_req = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS,
    };
    VkMemoryRequirements2 req = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
        .pNext = &ded_req,
    };

    ret = vk->CreateBuffer(s->hwctx->act_dev, &buf_spawn, NULL, &buf->buf);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Failed to create buffer: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    req_desc.buffer = buf->buf;

    vk->GetBufferMemoryRequirements2(s->hwctx->act_dev, &req_desc, &req);

    /* In case the implementation prefers/requires dedicated allocation */
    use_ded_mem = ded_req.prefersDedicatedAllocation |
                  ded_req.requiresDedicatedAllocation;
    if (use_ded_mem) {
        ded_alloc.buffer = buf->buf;
        ded_alloc.pNext = alloc_pNext;
        alloc_pNext = &ded_alloc;
    }

    if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
        alloc_flags.pNext = alloc_pNext;
        alloc_pNext = &alloc_flags;
    }

    err = ff_vk_alloc_mem(s, &req.memoryRequirements, flags, alloc_pNext,
                          &buf->flags, &buf->mem);
    if (err)
        return err;

    ret = vk->BindBufferMemory(s->hwctx->act_dev, buf->buf, buf->mem, 0);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Failed to bind memory to buffer: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
        VkBufferDeviceAddressInfo address_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = buf->buf,
        };
        buf->address = vk->GetBufferDeviceAddress(s->hwctx->act_dev, &address_info);
    }

    buf->size = size;

    return 0;
}

static void destroy_avvkbuf(void *opaque, uint8_t *data)
{
    FFVulkanContext *s = opaque;
    FFVkBuffer *buf = (FFVkBuffer *)data;
    ff_vk_free_buf(s, buf);
    av_free(buf);
}

int ff_vk_create_avbuf(FFVulkanContext *s, AVBufferRef **ref, size_t size,
                       void *pNext, void *alloc_pNext,
                       VkBufferUsageFlags usage, VkMemoryPropertyFlagBits flags)
{
    int err;
    AVBufferRef *buf;
    FFVkBuffer *vkb = av_mallocz(sizeof(*vkb));
    if (!vkb)
        return AVERROR(ENOMEM);

    err = ff_vk_create_buf(s, vkb, size, pNext, alloc_pNext, usage, flags);
    if (err < 0) {
        av_free(vkb);
        return err;
    }

    buf = av_buffer_create((uint8_t *)vkb, sizeof(*vkb), destroy_avvkbuf, s, 0);
    if (!buf) {
        destroy_avvkbuf(s, (uint8_t *)vkb);
        return AVERROR(ENOMEM);
    }

    *ref = buf;

    return 0;
}

int ff_vk_map_buffers(FFVulkanContext *s, FFVkBuffer **buf, uint8_t *mem[],
                      int nb_buffers, int invalidate)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    VkMappedMemoryRange inval_list[64];
    int inval_count = 0;

    for (int i = 0; i < nb_buffers; i++) {
        void *dst;
        ret = vk->MapMemory(s->hwctx->act_dev, buf[i]->mem, 0,
                            VK_WHOLE_SIZE, 0, &dst);
        if (ret != VK_SUCCESS) {
            av_log(s, AV_LOG_ERROR, "Failed to map buffer memory: %s\n",
                   ff_vk_ret2str(ret));
            return AVERROR_EXTERNAL;
        }
        mem[i] = dst;
    }

    if (!invalidate)
        return 0;

    for (int i = 0; i < nb_buffers; i++) {
        const VkMappedMemoryRange ival_buf = {
            .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .memory = buf[i]->mem,
            .size   = VK_WHOLE_SIZE,
        };
        if (buf[i]->flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            continue;
        inval_list[inval_count++] = ival_buf;
    }

    if (inval_count) {
        ret = vk->InvalidateMappedMemoryRanges(s->hwctx->act_dev, inval_count,
                                               inval_list);
        if (ret != VK_SUCCESS) {
            av_log(s, AV_LOG_ERROR, "Failed to invalidate memory: %s\n",
                   ff_vk_ret2str(ret));
            return AVERROR_EXTERNAL;
        }
    }

    return 0;
}

int ff_vk_unmap_buffers(FFVulkanContext *s, FFVkBuffer **buf, int nb_buffers,
                        int flush)
{
    int err = 0;
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    VkMappedMemoryRange flush_list[64];
    int flush_count = 0;

    if (flush) {
        for (int i = 0; i < nb_buffers; i++) {
            const VkMappedMemoryRange flush_buf = {
                .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                .memory = buf[i]->mem,
                .size   = VK_WHOLE_SIZE,
            };
            if (buf[i]->flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                continue;
            flush_list[flush_count++] = flush_buf;
        }
    }

    if (flush_count) {
        ret = vk->FlushMappedMemoryRanges(s->hwctx->act_dev, flush_count,
                                          flush_list);
        if (ret != VK_SUCCESS) {
            av_log(s, AV_LOG_ERROR, "Failed to flush memory: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR_EXTERNAL; /* We still want to try to unmap them */
        }
    }

    for (int i = 0; i < nb_buffers; i++)
        vk->UnmapMemory(s->hwctx->act_dev, buf[i]->mem);

    return err;
}

void ff_vk_free_buf(FFVulkanContext *s, FFVkBuffer *buf)
{
    FFVulkanFunctions *vk = &s->vkfn;

    if (!buf || !s->hwctx)
        return;

    if (buf->mapped_mem)
        ff_vk_unmap_buffer(s, buf, 0);
    if (buf->buf != VK_NULL_HANDLE)
        vk->DestroyBuffer(s->hwctx->act_dev, buf->buf, s->hwctx->alloc);
    if (buf->mem != VK_NULL_HANDLE)
        vk->FreeMemory(s->hwctx->act_dev, buf->mem, s->hwctx->alloc);
}

static void free_data_buf(void *opaque, uint8_t *data)
{
    FFVulkanContext *ctx = opaque;
    FFVkBuffer *buf = (FFVkBuffer *)data;
    ff_vk_free_buf(ctx, buf);
    av_free(data);
}

static AVBufferRef *alloc_data_buf(void *opaque, size_t size)
{
    AVBufferRef *ref;
    uint8_t *buf = av_mallocz(size);
    if (!buf)
        return NULL;

    ref = av_buffer_create(buf, size, free_data_buf, opaque, 0);
    if (!ref)
        av_free(buf);
    return ref;
}

int ff_vk_get_pooled_buffer(FFVulkanContext *ctx, AVBufferPool **buf_pool,
                            AVBufferRef **buf, VkBufferUsageFlags usage,
                            void *create_pNext, size_t size,
                            VkMemoryPropertyFlagBits mem_props)
{
    int err;
    AVBufferRef *ref;
    FFVkBuffer *data;

    if (!(*buf_pool)) {
        *buf_pool = av_buffer_pool_init2(sizeof(FFVkBuffer), ctx,
                                         alloc_data_buf, NULL);
        if (!(*buf_pool))
            return AVERROR(ENOMEM);
    }

    *buf = ref = av_buffer_pool_get(*buf_pool);
    if (!ref)
        return AVERROR(ENOMEM);

    data = (FFVkBuffer *)ref->data;
    data->stage = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    data->access = VK_ACCESS_2_NONE;

    if (data->size >= size)
        return 0;

    ff_vk_free_buf(ctx, data);
    memset(data, 0, sizeof(*data));

    av_log(ctx, AV_LOG_DEBUG, "Allocating buffer of %"SIZE_SPECIFIER" bytes for pool %p\n",
           size, *buf_pool);

    err = ff_vk_create_buf(ctx, data, size,
                           create_pNext, NULL, usage,
                           mem_props);
    if (err < 0) {
        av_buffer_unref(&ref);
        return err;
    }

    if (mem_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        err = ff_vk_map_buffer(ctx, data, &data->mapped_mem, 0);
        if (err < 0) {
            av_buffer_unref(&ref);
            return err;
        }
    }

    return 0;
}

int ff_vk_add_push_constant(FFVulkanPipeline *pl, int offset, int size,
                            VkShaderStageFlagBits stage)
{
    VkPushConstantRange *pc;

    pl->push_consts = av_realloc_array(pl->push_consts, sizeof(*pl->push_consts),
                                       pl->push_consts_num + 1);
    if (!pl->push_consts)
        return AVERROR(ENOMEM);

    pc = &pl->push_consts[pl->push_consts_num++];
    memset(pc, 0, sizeof(*pc));

    pc->stageFlags = stage;
    pc->offset = offset;
    pc->size = size;

    return 0;
}

int ff_vk_init_sampler(FFVulkanContext *s, VkSampler *sampler,
                       int unnorm_coords, VkFilter filt)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;

    VkSamplerCreateInfo sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = filt,
        .minFilter = sampler_info.magFilter,
        .mipmapMode = unnorm_coords ? VK_SAMPLER_MIPMAP_MODE_NEAREST :
                                      VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = sampler_info.addressModeU,
        .addressModeW = sampler_info.addressModeU,
        .anisotropyEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = unnorm_coords,
    };

    ret = vk->CreateSampler(s->hwctx->act_dev, &sampler_info,
                            s->hwctx->alloc, sampler);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Unable to init sampler: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    return 0;
}

int ff_vk_mt_is_np_rgb(enum AVPixelFormat pix_fmt)
{
    if (pix_fmt == AV_PIX_FMT_ABGR   || pix_fmt == AV_PIX_FMT_BGRA   ||
        pix_fmt == AV_PIX_FMT_RGBA   || pix_fmt == AV_PIX_FMT_RGB24  ||
        pix_fmt == AV_PIX_FMT_BGR24  || pix_fmt == AV_PIX_FMT_RGB48  ||
        pix_fmt == AV_PIX_FMT_RGBA64 || pix_fmt == AV_PIX_FMT_RGB565 ||
        pix_fmt == AV_PIX_FMT_BGR565 || pix_fmt == AV_PIX_FMT_BGR0   ||
        pix_fmt == AV_PIX_FMT_0BGR   || pix_fmt == AV_PIX_FMT_RGB0)
        return 1;
    return 0;
}

const char *ff_vk_shader_rep_fmt(enum AVPixelFormat pixfmt)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pixfmt);
    const int high = desc->comp[0].depth > 8;
    return high ? "rgba16f" : "rgba8";
}

typedef struct ImageViewCtx {
    VkImageView views[AV_NUM_DATA_POINTERS];
    int nb_views;
} ImageViewCtx;

static void destroy_imageviews(void *opaque, uint8_t *data)
{
    FFVulkanContext *s = opaque;
    FFVulkanFunctions *vk = &s->vkfn;
    ImageViewCtx *iv = (ImageViewCtx *)data;

    for (int i = 0; i < iv->nb_views; i++)
        vk->DestroyImageView(s->hwctx->act_dev, iv->views[i], s->hwctx->alloc);

    av_free(iv);
}

int ff_vk_create_imageviews(FFVulkanContext *s, FFVkExecContext *e,
                            VkImageView views[AV_NUM_DATA_POINTERS],
                            AVFrame *f)
{
    int err;
    VkResult ret;
    AVBufferRef *buf;
    FFVulkanFunctions *vk = &s->vkfn;
    AVHWFramesContext *hwfc = (AVHWFramesContext *)f->hw_frames_ctx->data;
    const VkFormat *rep_fmts = av_vkfmt_from_pixfmt(hwfc->sw_format);
    AVVkFrame *vkf = (AVVkFrame *)f->data[0];
    const int nb_images = ff_vk_count_images(vkf);
    const int nb_planes = av_pix_fmt_count_planes(hwfc->sw_format);

    ImageViewCtx *iv = av_mallocz(sizeof(*iv));
    if (!iv)
        return AVERROR(ENOMEM);

    for (int i = 0; i < nb_planes; i++) {
        VkImageAspectFlags plane_aspect[] = { VK_IMAGE_ASPECT_COLOR_BIT,
                                              VK_IMAGE_ASPECT_PLANE_0_BIT,
                                              VK_IMAGE_ASPECT_PLANE_1_BIT,
                                              VK_IMAGE_ASPECT_PLANE_2_BIT, };

        VkImageViewCreateInfo view_create_info = {
            .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext      = NULL,
            .image      = vkf->img[FFMIN(i, nb_images - 1)],
            .viewType   = VK_IMAGE_VIEW_TYPE_2D,
            .format     = rep_fmts[i],
            .components = ff_comp_identity_map,
            .subresourceRange = {
                .aspectMask = plane_aspect[(nb_planes != nb_images) +
                                           i*(nb_planes != nb_images)],
                .levelCount = 1,
                .layerCount = 1,
            },
        };

        ret = vk->CreateImageView(s->hwctx->act_dev, &view_create_info,
                                  s->hwctx->alloc, &iv->views[i]);
        if (ret != VK_SUCCESS) {
            av_log(s, AV_LOG_ERROR, "Failed to create imageview: %s\n",
                   ff_vk_ret2str(ret));
            err = AVERROR_EXTERNAL;
            goto fail;
        }

        iv->nb_views++;
    }

    buf = av_buffer_create((uint8_t *)iv, sizeof(*iv), destroy_imageviews, s, 0);
    if (!buf) {
        err = AVERROR(ENOMEM);
        goto fail;
    }

    /* Add to queue dependencies */
    err = ff_vk_exec_add_dep_buf(s, e, &buf, 1, 0);
    if (err < 0)
        av_buffer_unref(&buf);

    memcpy(views, iv->views, nb_planes*sizeof(*views));

    return err;

fail:
    for (int i = 0; i < iv->nb_views; i++)
        vk->DestroyImageView(s->hwctx->act_dev, iv->views[i], s->hwctx->alloc);
    av_free(iv);
    return err;
}

void ff_vk_frame_barrier(FFVulkanContext *s, FFVkExecContext *e,
                         AVFrame *pic, VkImageMemoryBarrier2 *bar, int *nb_bar,
                         VkPipelineStageFlags src_stage,
                         VkPipelineStageFlags dst_stage,
                         VkAccessFlagBits     new_access,
                         VkImageLayout        new_layout,
                         uint32_t             new_qf)
{
    int i, found;
    AVVkFrame *vkf = (AVVkFrame *)pic->data[0];
    const int nb_images = ff_vk_count_images(vkf);
    for (i = 0; i < e->nb_frame_deps; i++)
        if (e->frame_deps[i]->data[0] == pic->data[0])
            break;
    found = (i < e->nb_frame_deps) && (e->frame_update[i]) ? i : -1;

    for (int i = 0; i < nb_images; i++) {
        bar[*nb_bar] = (VkImageMemoryBarrier2) {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = NULL,
            .srcStageMask = src_stage,
            .dstStageMask = dst_stage,
            .srcAccessMask = found >= 0 ? e->access_dst[found] : vkf->access[i],
            .dstAccessMask = new_access,
            .oldLayout = found >= 0 ? e->layout_dst[found] : vkf->layout[0],
            .newLayout = new_layout,
            .srcQueueFamilyIndex = found >= 0 ? e->queue_family_dst[found] : vkf->queue_family[0],
            .dstQueueFamilyIndex = new_qf,
            .image = vkf->img[i],
            .subresourceRange = (VkImageSubresourceRange) {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .layerCount = 1,
                .levelCount = 1,
            },
        };
        *nb_bar += 1;
    }

    ff_vk_exec_update_frame(s, e, pic, &bar[*nb_bar - nb_images], NULL);
}

int ff_vk_shader_init(FFVulkanPipeline *pl, FFVkSPIRVShader *shd, const char *name,
                      VkShaderStageFlags stage, uint32_t required_subgroup_size)
{
    av_bprint_init(&shd->src, 0, AV_BPRINT_SIZE_UNLIMITED);

    shd->shader.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shd->shader.stage = stage;

    if (required_subgroup_size) {
        shd->shader.flags |= VK_PIPELINE_SHADER_STAGE_CREATE_REQUIRE_FULL_SUBGROUPS_BIT;
        shd->shader.pNext = &shd->subgroup_info;
        shd->subgroup_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO;
        shd->subgroup_info.requiredSubgroupSize = required_subgroup_size;
    }

    shd->name = name;

    GLSLF(0, #version %i                                                  ,460);
    GLSLC(0, #define IS_WITHIN(v1, v2) ((v1.x < v2.x) && (v1.y < v2.y))       );
    GLSLC(0,                                                                  );
    GLSLC(0, #extension GL_EXT_buffer_reference : require                     );
    GLSLC(0, #extension GL_EXT_buffer_reference2 : require                    );

    return 0;
}

void ff_vk_shader_set_compute_sizes(FFVkSPIRVShader *shd, int x, int y, int z)
{
    shd->local_size[0] = x;
    shd->local_size[1] = y;
    shd->local_size[2] = z;

    av_bprintf(&shd->src, "layout (local_size_x = %i, "
               "local_size_y = %i, local_size_z = %i) in;\n\n",
               shd->local_size[0], shd->local_size[1], shd->local_size[2]);
}

void ff_vk_shader_print(void *ctx, FFVkSPIRVShader *shd, int prio)
{
    int line = 0;
    const char *p = shd->src.str;
    const char *start = p;
    const size_t len = strlen(p);

    AVBPrint buf;
    av_bprint_init(&buf, 0, AV_BPRINT_SIZE_UNLIMITED);

    for (int i = 0; i < len; i++) {
        if (p[i] == '\n') {
            av_bprintf(&buf, "%i\t", ++line);
            av_bprint_append_data(&buf, start, &p[i] - start + 1);
            start = &p[i + 1];
        }
    }

    av_log(ctx, prio, "Shader %s: \n%s", shd->name, buf.str);
    av_bprint_finalize(&buf, NULL);
}

void ff_vk_shader_free(FFVulkanContext *s, FFVkSPIRVShader *shd)
{
    FFVulkanFunctions *vk = &s->vkfn;
    av_bprint_finalize(&shd->src, NULL);

    if (shd->shader.module)
        vk->DestroyShaderModule(s->hwctx->act_dev, shd->shader.module, s->hwctx->alloc);
}

int ff_vk_shader_create(FFVulkanContext *s, FFVkSPIRVShader *shd,
                        uint8_t *spirv, size_t spirv_size, const char *entrypoint)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    VkShaderModuleCreateInfo shader_create;

    shd->shader.pName = entrypoint;

    av_log(s, AV_LOG_VERBOSE, "Shader %s compiled! Size: %zu bytes\n",
           shd->name, spirv_size);

    shader_create.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_create.pNext    = NULL;
    shader_create.codeSize = spirv_size;
    shader_create.flags    = 0;
    shader_create.pCode    = (void *)spirv;

    ret = vk->CreateShaderModule(s->hwctx->act_dev, &shader_create, NULL,
                                 &shd->shader.module);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_VERBOSE, "Error creating shader module: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    return 0;
}

static const struct descriptor_props {
    size_t struct_size; /* Size of the opaque which updates the descriptor */
    const char *type;
    int is_uniform;
    int mem_quali;      /* Can use a memory qualifier */
    int dim_needed;     /* Must indicate dimension */
    int buf_content;    /* Must indicate buffer contents */
} descriptor_props[] = {
    [VK_DESCRIPTOR_TYPE_SAMPLER]                = { sizeof(VkDescriptorImageInfo),  "sampler",       1, 0, 0, 0, },
    [VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE]          = { sizeof(VkDescriptorImageInfo),  "texture",       1, 0, 1, 0, },
    [VK_DESCRIPTOR_TYPE_STORAGE_IMAGE]          = { sizeof(VkDescriptorImageInfo),  "image",         1, 1, 1, 0, },
    [VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT]       = { sizeof(VkDescriptorImageInfo),  "subpassInput",  1, 0, 0, 0, },
    [VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER] = { sizeof(VkDescriptorImageInfo),  "sampler",       1, 0, 1, 0, },
    [VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]         = { sizeof(VkDescriptorBufferInfo),  NULL,           1, 0, 0, 1, },
    [VK_DESCRIPTOR_TYPE_STORAGE_BUFFER]         = { sizeof(VkDescriptorBufferInfo), "buffer",        0, 1, 0, 1, },
    [VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC] = { sizeof(VkDescriptorBufferInfo),  NULL,           1, 0, 0, 1, },
    [VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC] = { sizeof(VkDescriptorBufferInfo), "buffer",        0, 1, 0, 1, },
    [VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER]   = { sizeof(VkBufferView),           "samplerBuffer", 1, 0, 0, 0, },
    [VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER]   = { sizeof(VkBufferView),           "imageBuffer",   1, 0, 0, 0, },
};

int ff_vk_pipeline_descriptor_set_add(FFVulkanContext *s, FFVulkanPipeline *pl,
                                      FFVkSPIRVShader *shd,
                                      FFVulkanDescriptorSetBinding *desc, int nb,
                                      int read_only, int print_to_shader_only)
{
    VkResult ret;
    int has_sampler = 0;
    FFVulkanFunctions *vk = &s->vkfn;
    FFVulkanDescriptorSet *set;
    VkDescriptorSetLayoutCreateInfo desc_create_layout;

    if (print_to_shader_only)
        goto print;

    /* Actual layout allocated for the pipeline */
    set = av_realloc_array(pl->desc_set, sizeof(*pl->desc_set),
                           pl->nb_descriptor_sets + 1);
    if (!set)
        return AVERROR(ENOMEM);
    pl->desc_set = set;
    set = &set[pl->nb_descriptor_sets];
    memset(set, 0, sizeof(*set));

    set->binding = av_calloc(nb, sizeof(*set->binding));
    if (!set->binding)
        return AVERROR(ENOMEM);

    set->binding_offset = av_calloc(nb, sizeof(*set->binding_offset));
    if (!set->binding_offset) {
        av_freep(&set->binding);
        return AVERROR(ENOMEM);
    }

    desc_create_layout = (VkDescriptorSetLayoutCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = nb,
        .pBindings = set->binding,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
    };

    for (int i = 0; i < nb; i++) {
        set->binding[i].binding            = i;
        set->binding[i].descriptorType     = desc[i].type;
        set->binding[i].descriptorCount    = FFMAX(desc[i].elems, 1);
        set->binding[i].stageFlags         = desc[i].stages;
        set->binding[i].pImmutableSamplers = desc[i].samplers;

        if (desc[i].type == VK_DESCRIPTOR_TYPE_SAMPLER ||
            desc[i].type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            has_sampler |= 1;
    }

    set->usage = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT |
                 VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    if (has_sampler)
        set->usage |= VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;

    ret = vk->CreateDescriptorSetLayout(s->hwctx->act_dev, &desc_create_layout,
                                        s->hwctx->alloc, &set->layout);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Unable to init descriptor set layout: %s",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    vk->GetDescriptorSetLayoutSizeEXT(s->hwctx->act_dev, set->layout, &set->layout_size);

    set->aligned_size = FFALIGN(set->layout_size, s->desc_buf_props.descriptorBufferOffsetAlignment);

    for (int i = 0; i < nb; i++)
        vk->GetDescriptorSetLayoutBindingOffsetEXT(s->hwctx->act_dev, set->layout,
                                                   i, &set->binding_offset[i]);

    set->read_only = read_only;
    set->nb_bindings = nb;
    pl->nb_descriptor_sets++;

print:
    /* Write shader info */
    for (int i = 0; i < nb; i++) {
        const struct descriptor_props *prop = &descriptor_props[desc[i].type];
        GLSLA("layout (set = %i, binding = %i", pl->nb_descriptor_sets - 1, i);

        if (desc[i].mem_layout)
            GLSLA(", %s", desc[i].mem_layout);
        GLSLA(")");

        if (prop->is_uniform)
            GLSLA(" uniform");

        if (prop->mem_quali && desc[i].mem_quali)
            GLSLA(" %s", desc[i].mem_quali);

        if (prop->type)
            GLSLA(" %s", prop->type);

        if (prop->dim_needed)
            GLSLA("%iD", desc[i].dimensions);

        GLSLA(" %s", desc[i].name);

        if (prop->buf_content)
            GLSLA(" {\n    %s\n}", desc[i].buf_content);
        else if (desc[i].elems > 0)
            GLSLA("[%i]", desc[i].elems);

        GLSLA(";");
        GLSLA("\n");
    }
    GLSLA("\n");

    return 0;
}

int ff_vk_exec_pipeline_register(FFVulkanContext *s, FFVkExecPool *pool,
                                 FFVulkanPipeline *pl)
{
    int err;

    pl->desc_bind = av_calloc(pl->nb_descriptor_sets, sizeof(*pl->desc_bind));
    if (!pl->desc_bind)
        return AVERROR(ENOMEM);

    pl->bound_buffer_indices = av_calloc(pl->nb_descriptor_sets,
                                         sizeof(*pl->bound_buffer_indices));
    if (!pl->bound_buffer_indices)
        return AVERROR(ENOMEM);

    for (int i = 0; i < pl->nb_descriptor_sets; i++) {
        FFVulkanDescriptorSet *set = &pl->desc_set[i];
        int nb = set->read_only ? 1 : pool->pool_size;

        err = ff_vk_create_buf(s, &set->buf, set->aligned_size*nb,
                               NULL, NULL, set->usage,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (err < 0)
            return err;

        err = ff_vk_map_buffer(s, &set->buf, &set->desc_mem, 0);
        if (err < 0)
            return err;

        pl->desc_bind[i] = (VkDescriptorBufferBindingInfoEXT) {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT,
            .usage = set->usage,
            .address = set->buf.address,
        };

        pl->bound_buffer_indices[i] = i;
    }

    return 0;
}

static inline void update_set_descriptor(FFVulkanContext *s, FFVkExecContext *e,
                                         FFVulkanDescriptorSet *set,
                                         int bind_idx, int array_idx,
                                         VkDescriptorGetInfoEXT *desc_get_info,
                                         size_t desc_size)
{
    FFVulkanFunctions *vk = &s->vkfn;
    const size_t exec_offset = set->read_only ? 0 : set->aligned_size*e->idx;
    void *desc = set->desc_mem +                 /* Base */
                 exec_offset +                   /* Execution context */
                 set->binding_offset[bind_idx] + /* Descriptor binding */
                 array_idx*desc_size;            /* Array position */

    vk->GetDescriptorEXT(s->hwctx->act_dev, desc_get_info, desc_size, desc);
}

int ff_vk_set_descriptor_sampler(FFVulkanContext *s, FFVulkanPipeline *pl,
                                 FFVkExecContext *e, int set, int bind, int offs,
                                 VkSampler *sampler)
{
    FFVulkanDescriptorSet *desc_set = &pl->desc_set[set];
    VkDescriptorGetInfoEXT desc_get_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
        .type = desc_set->binding[bind].descriptorType,
    };

    switch (desc_get_info.type) {
    case VK_DESCRIPTOR_TYPE_SAMPLER:
        desc_get_info.data.pSampler = sampler;
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Invalid descriptor type at set %i binding %i: %i!\n",
               set, bind, desc_get_info.type);
        return AVERROR(EINVAL);
        break;
    };

    update_set_descriptor(s, e, desc_set, bind, offs, &desc_get_info,
                          s->desc_buf_props.samplerDescriptorSize);

    return 0;
}

int ff_vk_set_descriptor_image(FFVulkanContext *s, FFVulkanPipeline *pl,
                               FFVkExecContext *e, int set, int bind, int offs,
                               VkImageView view, VkImageLayout layout, VkSampler sampler)
{
    FFVulkanDescriptorSet *desc_set = &pl->desc_set[set];
    VkDescriptorGetInfoEXT desc_get_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
        .type = desc_set->binding[bind].descriptorType,
    };
    VkDescriptorImageInfo desc_img_info = {
        .imageView = view,
        .sampler = sampler,
        .imageLayout = layout,
    };
    size_t desc_size;

    switch (desc_get_info.type) {
    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        desc_get_info.data.pSampledImage = &desc_img_info;
        desc_size = s->desc_buf_props.sampledImageDescriptorSize;
        break;
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        desc_get_info.data.pStorageImage = &desc_img_info;
        desc_size = s->desc_buf_props.storageImageDescriptorSize;
        break;
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        desc_get_info.data.pInputAttachmentImage = &desc_img_info;
        desc_size = s->desc_buf_props.inputAttachmentDescriptorSize;
        break;
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        desc_get_info.data.pCombinedImageSampler = &desc_img_info;
        desc_size = s->desc_buf_props.combinedImageSamplerDescriptorSize;
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Invalid descriptor type at set %i binding %i: %i!\n",
               set, bind, desc_get_info.type);
        return AVERROR(EINVAL);
        break;
    };

    update_set_descriptor(s, e, desc_set, bind, offs, &desc_get_info, desc_size);

    return 0;
}

int ff_vk_set_descriptor_buffer(FFVulkanContext *s, FFVulkanPipeline *pl,
                                FFVkExecContext *e, int set, int bind, int offs,
                                VkDeviceAddress addr, VkDeviceSize len, VkFormat fmt)
{
    FFVulkanDescriptorSet *desc_set = &pl->desc_set[set];
    VkDescriptorGetInfoEXT desc_get_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT,
        .type = desc_set->binding[bind].descriptorType,
    };
    VkDescriptorAddressInfoEXT desc_buf_info = {
        .address = addr,
        .range = len,
        .format = fmt,
    };
    size_t desc_size;

    switch (desc_get_info.type) {
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        desc_get_info.data.pUniformBuffer = &desc_buf_info;
        desc_size = s->desc_buf_props.uniformBufferDescriptorSize;
        break;
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        desc_get_info.data.pStorageBuffer = &desc_buf_info;
        desc_size = s->desc_buf_props.storageBufferDescriptorSize;
        break;
    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        desc_get_info.data.pUniformTexelBuffer = &desc_buf_info;
        desc_size = s->desc_buf_props.uniformTexelBufferDescriptorSize;
        break;
    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        desc_get_info.data.pStorageTexelBuffer = &desc_buf_info;
        desc_size = s->desc_buf_props.storageTexelBufferDescriptorSize;
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Invalid descriptor type at set %i binding %i: %i!\n",
               set, bind, desc_get_info.type);
        return AVERROR(EINVAL);
        break;
    };

    update_set_descriptor(s, e, desc_set, bind, offs, &desc_get_info, desc_size);

    return 0;
}

void ff_vk_update_descriptor_img_array(FFVulkanContext *s, FFVulkanPipeline *pl,
                                       FFVkExecContext *e, AVFrame *f,
                                       VkImageView *views, int set, int binding,
                                       VkImageLayout layout, VkSampler sampler)
{
    AVHWFramesContext *hwfc = (AVHWFramesContext *)f->hw_frames_ctx->data;
    const int nb_planes = av_pix_fmt_count_planes(hwfc->sw_format);

    for (int i = 0; i < nb_planes; i++)
        ff_vk_set_descriptor_image(s, pl, e, set, binding, i,
                                   views[i], layout, sampler);
}

void ff_vk_update_push_exec(FFVulkanContext *s, FFVkExecContext *e,
                            FFVulkanPipeline *pl,
                            VkShaderStageFlagBits stage,
                            int offset, size_t size, void *src)
{
    FFVulkanFunctions *vk = &s->vkfn;
    vk->CmdPushConstants(e->buf, pl->pipeline_layout,
                         stage, offset, size, src);
}

static int init_pipeline_layout(FFVulkanContext *s, FFVulkanPipeline *pl)
{
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;
    VkPipelineLayoutCreateInfo pipeline_layout_info;

    VkDescriptorSetLayout *desc_layouts = av_malloc(pl->nb_descriptor_sets*
                                                    sizeof(desc_layouts));
    if (!desc_layouts)
        return AVERROR(ENOMEM);

    for (int i = 0; i < pl->nb_descriptor_sets; i++)
        desc_layouts[i] = pl->desc_set[i].layout;

    /* Finally create the pipeline layout */
    pipeline_layout_info = (VkPipelineLayoutCreateInfo) {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pSetLayouts            = desc_layouts,
        .setLayoutCount         = pl->nb_descriptor_sets,
        .pushConstantRangeCount = pl->push_consts_num,
        .pPushConstantRanges    = pl->push_consts,
    };

    ret = vk->CreatePipelineLayout(s->hwctx->act_dev, &pipeline_layout_info,
                                   s->hwctx->alloc, &pl->pipeline_layout);
    av_free(desc_layouts);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Unable to init pipeline layout: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    return 0;
}

int ff_vk_init_compute_pipeline(FFVulkanContext *s, FFVulkanPipeline *pl,
                                FFVkSPIRVShader *shd)
{
    int err;
    VkResult ret;
    FFVulkanFunctions *vk = &s->vkfn;

    VkComputePipelineCreateInfo pipeline_create_info;

    err = init_pipeline_layout(s, pl);
    if (err < 0)
        return err;

    pipeline_create_info = (VkComputePipelineCreateInfo) {
        .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
        .layout = pl->pipeline_layout,
        .stage = shd->shader,
    };

    ret = vk->CreateComputePipelines(s->hwctx->act_dev, VK_NULL_HANDLE, 1,
                                     &pipeline_create_info,
                                     s->hwctx->alloc, &pl->pipeline);
    if (ret != VK_SUCCESS) {
        av_log(s, AV_LOG_ERROR, "Unable to init compute pipeline: %s\n",
               ff_vk_ret2str(ret));
        return AVERROR_EXTERNAL;
    }

    pl->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
    pl->wg_size[0] = shd->local_size[0];
    pl->wg_size[1] = shd->local_size[1];
    pl->wg_size[2] = shd->local_size[2];

    return 0;
}

void ff_vk_exec_bind_pipeline(FFVulkanContext *s, FFVkExecContext *e,
                              FFVulkanPipeline *pl)
{
    FFVulkanFunctions *vk = &s->vkfn;
    VkDeviceSize offsets[1024];

    /* Bind pipeline */
    vk->CmdBindPipeline(e->buf, pl->bind_point, pl->pipeline);

    if (pl->nb_descriptor_sets) {
        for (int i = 0; i < pl->nb_descriptor_sets; i++)
            offsets[i] = pl->desc_set[i].read_only ? 0 : pl->desc_set[i].aligned_size*e->idx;

        /* Bind descriptor buffers */
        vk->CmdBindDescriptorBuffersEXT(e->buf, pl->nb_descriptor_sets, pl->desc_bind);
        /* Binding offsets */
        vk->CmdSetDescriptorBufferOffsetsEXT(e->buf, pl->bind_point, pl->pipeline_layout,
                                             0, pl->nb_descriptor_sets,
                                             pl->bound_buffer_indices, offsets);
    }
}

void ff_vk_pipeline_free(FFVulkanContext *s, FFVulkanPipeline *pl)
{
    FFVulkanFunctions *vk = &s->vkfn;

    if (pl->pipeline)
        vk->DestroyPipeline(s->hwctx->act_dev, pl->pipeline, s->hwctx->alloc);
    if (pl->pipeline_layout)
        vk->DestroyPipelineLayout(s->hwctx->act_dev, pl->pipeline_layout,
                                  s->hwctx->alloc);

    for (int i = 0; i < pl->nb_descriptor_sets; i++) {
        FFVulkanDescriptorSet *set = &pl->desc_set[i];
        if (set->buf.mem)
            ff_vk_unmap_buffer(s, &set->buf, 0);
        ff_vk_free_buf(s, &set->buf);
        if (set->layout)
            vk->DestroyDescriptorSetLayout(s->hwctx->act_dev, set->layout,
                                           s->hwctx->alloc);
        av_free(set->binding);
        av_free(set->binding_offset);
    }

    av_freep(&pl->desc_set);
    av_freep(&pl->desc_bind);
    av_freep(&pl->bound_buffer_indices);
    av_freep(&pl->push_consts);
    pl->push_consts_num = 0;
}

void ff_vk_uninit(FFVulkanContext *s)
{
    av_freep(&s->query_props);
    av_freep(&s->qf_props);
    av_freep(&s->video_props);
    av_freep(&s->coop_mat_props);

    av_buffer_unref(&s->frames_ref);
}
