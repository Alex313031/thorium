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

/*
 * APIs internal to the generic codec layer.
 *
 * MUST NOT be included by individual encoders or decoders.
 */

#ifndef AVCODEC_AVCODEC_INTERNAL_H
#define AVCODEC_AVCODEC_INTERNAL_H

struct AVCodecContext;
struct AVFrame;

/**
 * avcodec_receive_frame() implementation for decoders.
 */
int ff_decode_receive_frame(struct AVCodecContext *avctx, struct AVFrame *frame);

/**
 * avcodec_receive_frame() implementation for encoders.
 */
int ff_encode_receive_frame(struct AVCodecContext *avctx, struct AVFrame *frame);

/*
 * Perform encoder initialization and validation.
 * Called when opening the encoder, before the FFCodec.init() call.
 */
int ff_encode_preinit(struct AVCodecContext *avctx);

/**
 * Perform decoder initialization and validation.
 * Called when opening the decoder, before the FFCodec.init() call.
 */
int ff_decode_preinit(struct AVCodecContext *avctx);

void ff_decode_flush_buffers(struct AVCodecContext *avctx);
void ff_encode_flush_buffers(struct AVCodecContext *avctx);

struct AVCodecInternal *ff_decode_internal_alloc(void);
struct AVCodecInternal *ff_encode_internal_alloc(void);

#endif // AVCODEC_AVCODEC_INTERNAL_H
