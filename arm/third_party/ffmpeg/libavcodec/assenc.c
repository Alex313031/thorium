/*
 * SSA/ASS encoder
 * Copyright (c) 2010  Aurelien Jacobs <aurel@gnuage.org>
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

#include "config_components.h"

#include <string.h>

#include "avcodec.h"
#include "ass.h"
#include "codec_internal.h"
#include "libavutil/avstring.h"
#include "libavutil/internal.h"
#include "libavutil/mem.h"

static av_cold int ass_encode_init(AVCodecContext *avctx)
{
    avctx->extradata = av_malloc(avctx->subtitle_header_size + 1);
    if (!avctx->extradata)
        return AVERROR(ENOMEM);
    memcpy(avctx->extradata, avctx->subtitle_header, avctx->subtitle_header_size);
    avctx->extradata_size = avctx->subtitle_header_size;
    avctx->extradata[avctx->extradata_size] = 0;
    return 0;
}

static int ass_encode_frame(AVCodecContext *avctx,
                            unsigned char *buf, int bufsize,
                            const AVSubtitle *sub)
{
    int len;

    if (sub->num_rects != 1) {
        av_log(avctx, AV_LOG_ERROR, "Only one rect per AVSubtitle is supported in ASS.\n");
        return AVERROR_INVALIDDATA;
    }

    if (sub->rects[0]->type != SUBTITLE_ASS) {
        av_log(avctx, AV_LOG_ERROR, "Only SUBTITLE_ASS type supported.\n");
        return AVERROR(EINVAL);
    }

    len = av_strlcpy(buf, sub->rects[0]->ass, bufsize);

    if (len > bufsize - 1) {
        av_log(avctx, AV_LOG_ERROR, "Buffer too small for ASS event.\n");
        return AVERROR_BUFFER_TOO_SMALL;
    }

    return len;
}

#if CONFIG_SSA_ENCODER
const FFCodec ff_ssa_encoder = {
    .p.name       = "ssa",
    CODEC_LONG_NAME("ASS (Advanced SubStation Alpha) subtitle"),
    .p.type       = AVMEDIA_TYPE_SUBTITLE,
    .p.id         = AV_CODEC_ID_ASS,
    .init         = ass_encode_init,
    FF_CODEC_ENCODE_SUB_CB(ass_encode_frame),
};
#endif

#if CONFIG_ASS_ENCODER
const FFCodec ff_ass_encoder = {
    .p.name       = "ass",
    CODEC_LONG_NAME("ASS (Advanced SubStation Alpha) subtitle"),
    .p.type       = AVMEDIA_TYPE_SUBTITLE,
    .p.id         = AV_CODEC_ID_ASS,
    .init         = ass_encode_init,
    FF_CODEC_ENCODE_SUB_CB(ass_encode_frame),
};
#endif
