/*
 * WinCAM Motion Video decoder
 *
 * Copyright (c) 2018 Paul B Mahol
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

#include <stdio.h>

#include "libavutil/imgutils.h"

#include "avcodec.h"
#include "bytestream.h"
#include "codec_internal.h"
#include "decode.h"
#include "zlib_wrapper.h"

#include <zlib.h>

typedef struct WCMVContext {
    int         bpp;
    FFZStream   zstream;
    AVFrame    *prev_frame;
    uint8_t     block_data[65536*8];
} WCMVContext;

static int decode_frame(AVCodecContext *avctx, AVFrame *frame,
                        int *got_frame, AVPacket *avpkt)
{
    WCMVContext *s = avctx->priv_data;
    z_stream *const zstream = &s->zstream.zstream;
    int skip, blocks, zret, ret, intra = 0, flags = 0, bpp = s->bpp;
    GetByteContext gb;
    uint8_t *dst;

    ret = inflateReset(zstream);
    if (ret != Z_OK) {
        av_log(avctx, AV_LOG_ERROR, "Inflate reset error: %d\n", ret);
        return AVERROR_EXTERNAL;
    }

    bytestream2_init(&gb, avpkt->data, avpkt->size);
    blocks = bytestream2_get_le16(&gb);
    if (!blocks)
        flags |= FF_REGET_BUFFER_FLAG_READONLY;

    if ((ret = ff_reget_buffer(avctx, s->prev_frame, flags)) < 0)
        return ret;

    if (blocks > 5) {
        GetByteContext bgb;
        int x = 0, size;

        if (blocks * 8 >= 0xFFFF)
            size = bytestream2_get_le24(&gb);
        else if (blocks * 8 >= 0xFF)
            size = bytestream2_get_le16(&gb);
        else
            size = bytestream2_get_byte(&gb);

        skip = bytestream2_tell(&gb);
        if (size > avpkt->size - skip)
            return AVERROR_INVALIDDATA;

        zstream->next_in   = avpkt->data + skip;
        zstream->avail_in  = size;
        zstream->next_out  = s->block_data;
        zstream->avail_out = sizeof(s->block_data);

        zret = inflate(zstream, Z_FINISH);
        if (zret != Z_STREAM_END) {
            av_log(avctx, AV_LOG_ERROR,
                   "Inflate failed with return code: %d.\n", zret);
            return AVERROR_INVALIDDATA;
        }

        ret = inflateReset(zstream);
        if (ret != Z_OK) {
            av_log(avctx, AV_LOG_ERROR, "Inflate reset error: %d\n", ret);
            return AVERROR_EXTERNAL;
        }

        bytestream2_skip(&gb, size);
        bytestream2_init(&bgb, s->block_data, blocks * 8);

        for (int i = 0; i < blocks; i++) {
            int w, h;

            bytestream2_skip(&bgb, 4);
            w = bytestream2_get_le16(&bgb);
            h = bytestream2_get_le16(&bgb);
            if (x + bpp * (int64_t)w * h > INT_MAX)
                return AVERROR_INVALIDDATA;
            x += bpp * w * h;
        }

        if (x >= 0xFFFF)
            bytestream2_skip(&gb, 3);
        else if (x >= 0xFF)
            bytestream2_skip(&gb, 2);
        else
            bytestream2_skip(&gb, 1);

        skip = bytestream2_tell(&gb);

        zstream->next_in  = avpkt->data + skip;
        zstream->avail_in = avpkt->size - skip;

        bytestream2_init(&gb, s->block_data, blocks * 8);
    } else if (blocks) {
        int x = 0;

        bytestream2_seek(&gb, 2, SEEK_SET);

        for (int i = 0; i < blocks; i++) {
            int w, h;

            bytestream2_skip(&gb, 4);
            w = bytestream2_get_le16(&gb);
            h = bytestream2_get_le16(&gb);
            if (x + bpp * (int64_t)w * h > INT_MAX)
                return AVERROR_INVALIDDATA;
            x += bpp * w * h;
        }

        if (x >= 0xFFFF)
            bytestream2_skip(&gb, 3);
        else if (x >= 0xFF)
            bytestream2_skip(&gb, 2);
        else
            bytestream2_skip(&gb, 1);

        skip = bytestream2_tell(&gb);

        zstream->next_in  = avpkt->data + skip;
        zstream->avail_in = avpkt->size - skip;

        bytestream2_seek(&gb, 2, SEEK_SET);
    }

    if (bytestream2_get_bytes_left(&gb) < 8LL * blocks)
        return AVERROR_INVALIDDATA;

    if (!avctx->frame_num) {
        ptrdiff_t linesize[4] = { s->prev_frame->linesize[0], 0, 0, 0 };
        av_image_fill_black(s->prev_frame->data, linesize, avctx->pix_fmt, 0,
                            avctx->width, avctx->height);
    }

    for (int block = 0; block < blocks; block++) {
        int x, y, w, h;

        x = bytestream2_get_le16(&gb);
        y = bytestream2_get_le16(&gb);
        w = bytestream2_get_le16(&gb);
        h = bytestream2_get_le16(&gb);

        if (blocks == 1 && x == 0 && y == 0 && w == avctx->width && h == avctx->height)
            intra = 1;

        if (x + w > avctx->width || y + h > avctx->height)
            return AVERROR_INVALIDDATA;

        if (w > avctx->width || h > avctx->height)
            return AVERROR_INVALIDDATA;

        dst = s->prev_frame->data[0] + (avctx->height - y - 1) * s->prev_frame->linesize[0] + x * bpp;
        for (int i = 0; i < h; i++) {
            zstream->next_out  = dst;
            zstream->avail_out = w * bpp;

            zret = inflate(zstream, Z_SYNC_FLUSH);
            if (zret != Z_OK && zret != Z_STREAM_END) {
                av_log(avctx, AV_LOG_ERROR,
                       "Inflate failed with return code: %d.\n", zret);
                return AVERROR_INVALIDDATA;
            }

            dst -= s->prev_frame->linesize[0];
        }
    }

    if (intra)
        s->prev_frame->flags |= AV_FRAME_FLAG_KEY;
    else
        s->prev_frame->flags &= ~AV_FRAME_FLAG_KEY;
    s->prev_frame->pict_type = intra ? AV_PICTURE_TYPE_I : AV_PICTURE_TYPE_P;

    if ((ret = av_frame_ref(frame, s->prev_frame)) < 0)
        return ret;

    *got_frame = 1;

    return avpkt->size;
}

static av_cold int decode_init(AVCodecContext *avctx)
{
    WCMVContext *s = avctx->priv_data;

    switch (avctx->bits_per_coded_sample) {
    case 16: avctx->pix_fmt = AV_PIX_FMT_RGB565LE; break;
    case 24: avctx->pix_fmt = AV_PIX_FMT_BGR24;  break;
    case 32: avctx->pix_fmt = AV_PIX_FMT_BGRA;   break;
    default: av_log(avctx, AV_LOG_ERROR, "Unsupported bits_per_coded_sample: %d\n",
                    avctx->bits_per_coded_sample);
             return AVERROR_PATCHWELCOME;
    }

    s->bpp = avctx->bits_per_coded_sample >> 3;

    s->prev_frame = av_frame_alloc();
    if (!s->prev_frame)
        return AVERROR(ENOMEM);

    return ff_inflate_init(&s->zstream, avctx);
}

static av_cold int decode_close(AVCodecContext *avctx)
{
    WCMVContext *s = avctx->priv_data;

    av_frame_free(&s->prev_frame);
    ff_inflate_end(&s->zstream);

    return 0;
}

const FFCodec ff_wcmv_decoder = {
    .p.name           = "wcmv",
    CODEC_LONG_NAME("WinCAM Motion Video"),
    .p.type           = AVMEDIA_TYPE_VIDEO,
    .p.id             = AV_CODEC_ID_WCMV,
    .priv_data_size   = sizeof(WCMVContext),
    .init             = decode_init,
    .close            = decode_close,
    FF_CODEC_DECODE_CB(decode_frame),
    .p.capabilities   = AV_CODEC_CAP_DR1,
    .caps_internal    = FF_CODEC_CAP_INIT_CLEANUP,
};
