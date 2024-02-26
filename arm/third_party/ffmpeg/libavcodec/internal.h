/*
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

/**
 * @file
 * common internal api header.
 */

#ifndef AVCODEC_INTERNAL_H
#define AVCODEC_INTERNAL_H

#include <stdint.h>

#include "libavutil/buffer.h"
#include "libavutil/channel_layout.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixfmt.h"
#include "avcodec.h"
#include "config.h"

#if CONFIG_LCMS2
# include "fflcms2.h"
#endif

#define FF_SANE_NB_CHANNELS 512U

#if HAVE_SIMD_ALIGN_64
#   define STRIDE_ALIGN 64 /* AVX-512 */
#elif HAVE_SIMD_ALIGN_32
#   define STRIDE_ALIGN 32
#elif HAVE_SIMD_ALIGN_16
#   define STRIDE_ALIGN 16
#else
#   define STRIDE_ALIGN 8
#endif

typedef struct AVCodecInternal {
    /**
     * When using frame-threaded decoding, this field is set for the first
     * worker thread (e.g. to decode extradata just once).
     */
    int is_copy;

    /**
     * Audio encoders can set this flag during init to indicate that they
     * want the small last frame to be padded to a multiple of pad_samples.
     */
    int pad_samples;

    struct FramePool *pool;

    void *thread_ctx;

    /**
     * This packet is used to hold the packet given to decoders
     * implementing the .decode API; it is unused by the generic
     * code for decoders implementing the .receive_frame API and
     * may be freely used (but not freed) by them with the caveat
     * that the packet will be unreferenced generically in
     * avcodec_flush_buffers().
     */
    AVPacket *in_pkt;
    struct AVBSFContext *bsf;

    /**
     * Properties (timestamps+side data) extracted from the last packet passed
     * for decoding.
     */
    AVPacket *last_pkt_props;

    /**
     * temporary buffer used for encoders to store their bitstream
     */
    uint8_t *byte_buffer;
    unsigned int byte_buffer_size;

    void *frame_thread_encoder;

    /**
     * The input frame is stored here for encoders implementing the simple
     * encode API.
     *
     * Not allocated in other cases.
     */
    AVFrame *in_frame;

    /**
     * When the AV_CODEC_FLAG_RECON_FRAME flag is used. the encoder should store
     * here the reconstructed frame corresponding to the last returned packet.
     *
     * Not allocated in other cases.
     */
    AVFrame *recon_frame;

    /**
     * If this is set, then FFCodec->close (if existing) needs to be called
     * for the parent AVCodecContext.
     */
    int needs_close;

    /**
     * Number of audio samples to skip at the start of the next decoded frame
     */
    int skip_samples;

    /**
     * hwaccel-specific private data
     */
    void *hwaccel_priv_data;

    /**
     * checks API usage: after codec draining, flush is required to resume operation
     */
    int draining;

    /**
     * Temporary buffers for newly received or not yet output packets/frames.
     */
    AVPacket *buffer_pkt;
    AVFrame *buffer_frame;
    int draining_done;

#if FF_API_DROPCHANGED
    /* used when avctx flag AV_CODEC_FLAG_DROPCHANGED is set */
    int changed_frames_dropped;
    int initial_format;
    int initial_width, initial_height;
    int initial_sample_rate;
    AVChannelLayout initial_ch_layout;
#endif

#if CONFIG_LCMS2
    FFIccContext icc; /* used to read and write embedded ICC profiles */
#endif
} AVCodecInternal;

/**
 * Return the index into tab at which {a,b} match elements {[0],[1]} of tab.
 * If there is no such matching pair then size is returned.
 */
int ff_match_2uint16(const uint16_t (*tab)[2], int size, int a, int b);

unsigned int ff_toupper4(unsigned int x);

/**
 * 2^(x) for integer x
 * @return correctly rounded float
 */
static av_always_inline float ff_exp2fi(int x) {
    /* Normal range */
    if (-126 <= x && x <= 128)
        return av_int2float((x+127) << 23);
    /* Too large */
    else if (x > 128)
        return INFINITY;
    /* Subnormal numbers */
    else if (x > -150)
        return av_int2float(1 << (x+149));
    /* Negligibly small */
    else
        return 0;
}

int avpriv_h264_has_num_reorder_frames(AVCodecContext *avctx);

int avpriv_codec_get_cap_skip_frame_fill_param(const AVCodec *codec);

/**
 * Check AVFrame for S12M timecode side data and allocate and fill TC SEI message with timecode info
 *
 * @param frame      Raw frame to get S12M timecode side data from
 * @param rate       The frame rate
 * @param prefix_len Number of bytes to allocate before SEI message
 * @param data       Pointer to a variable to store allocated memory
 *                   Upon return the variable will hold NULL on error or if frame has no S12M timecode info.
 *                   Otherwise it will point to prefix_len uninitialized bytes followed by
 *                   *sei_size SEI message
 * @param sei_size   Pointer to a variable to store generated SEI message length
 * @return           Zero on success, negative error code on failure
 */
int ff_alloc_timecode_sei(const AVFrame *frame, AVRational rate, size_t prefix_len,
                     void **data, size_t *sei_size);

/**
 * Get an estimated video bitrate based on frame size, frame rate and coded
 * bits per pixel.
 */
int64_t ff_guess_coded_bitrate(AVCodecContext *avctx);

#endif /* AVCODEC_INTERNAL_H */
