/*
 * Copyright (C) 2008 David Conrad
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

#include <speex/speex.h>
#include <speex/speex_header.h>
#include <speex/speex_stereo.h>
#include <speex/speex_callbacks.h>

#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "avcodec.h"
#include "codec_internal.h"
#include "decode.h"

typedef struct LibSpeexContext {
    SpeexBits bits;
    SpeexStereoState stereo;
    void *dec_state;
    int frame_size;
    int pktsize;
} LibSpeexContext;


static av_cold int libspeex_decode_init(AVCodecContext *avctx)
{
    LibSpeexContext *s = avctx->priv_data;
    const SpeexMode *mode;
    SpeexHeader *header = NULL;
    int spx_mode, channels = avctx->ch_layout.nb_channels;

    if (avctx->extradata && avctx->extradata_size >= 80) {
        header = speex_packet_to_header(avctx->extradata,
                                        avctx->extradata_size);
        if (!header)
            av_log(avctx, AV_LOG_WARNING, "Invalid Speex header\n");
    }
    if (avctx->codec_tag == MKTAG('S', 'P', 'X', 'N')) {
        int quality;
        if (!avctx->extradata || avctx->extradata && avctx->extradata_size < 47) {
            av_log(avctx, AV_LOG_ERROR, "Missing or invalid extradata.\n");
            return AVERROR_INVALIDDATA;
        }

        quality = avctx->extradata[37];
        if (quality > 10) {
            av_log(avctx, AV_LOG_ERROR, "Unsupported quality mode %d.\n", quality);
            return AVERROR_PATCHWELCOME;
        }

        s->pktsize = ((const int[]){5,10,15,20,20,28,28,38,38,46,62})[quality];

        spx_mode           = 0;
    } else if (header) {
        avctx->sample_rate = header->rate;
        channels           = header->nb_channels;
        spx_mode           = header->mode;
        speex_header_free(header);
    } else {
        switch (avctx->sample_rate) {
        case 8000:  spx_mode = 0; break;
        case 16000: spx_mode = 1; break;
        case 32000: spx_mode = 2; break;
        default:
            /* libspeex can handle any mode if initialized as ultra-wideband */
            av_log(avctx, AV_LOG_WARNING, "Invalid sample rate: %d\n"
                                          "Decoding as 32kHz ultra-wideband\n",
                                          avctx->sample_rate);
            spx_mode = 2;
        }
    }

    mode = speex_lib_get_mode(spx_mode);
    if (!mode) {
        av_log(avctx, AV_LOG_ERROR, "Unknown Speex mode %d", spx_mode);
        return AVERROR_INVALIDDATA;
    }
    s->frame_size      =  160 << spx_mode;
    if (!avctx->sample_rate)
        avctx->sample_rate = 8000 << spx_mode;

    if (channels < 1 || channels > 2) {
        /* libspeex can handle mono or stereo if initialized as stereo */
        av_log(avctx, AV_LOG_ERROR, "Invalid channel count: %d.\n"
                                    "Decoding as stereo.\n", channels);
        channels = 2;
    }
    av_channel_layout_uninit(&avctx->ch_layout);
    avctx->ch_layout = channels == 2 ? (AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO :
                                       (AVChannelLayout)AV_CHANNEL_LAYOUT_MONO;

    speex_bits_init(&s->bits);
    s->dec_state = speex_decoder_init(mode);
    if (!s->dec_state) {
        av_log(avctx, AV_LOG_ERROR, "Error initializing libspeex decoder.\n");
        return -1;
    }

    if (channels == 2) {
        SpeexCallback callback;
        callback.callback_id = SPEEX_INBAND_STEREO;
        callback.func = speex_std_stereo_request_handler;
        callback.data = &s->stereo;
        s->stereo = (SpeexStereoState)SPEEX_STEREO_STATE_INIT;
        speex_decoder_ctl(s->dec_state, SPEEX_SET_HANDLER, &callback);
    }

    return 0;
}

static int libspeex_decode_frame(AVCodecContext *avctx, AVFrame *frame,
                                 int *got_frame_ptr, AVPacket *avpkt)
{
    uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    LibSpeexContext *s = avctx->priv_data;
    int16_t *output;
    int ret, consumed = 0;
    avctx->sample_fmt = AV_SAMPLE_FMT_S16;

    /* get output buffer */
    frame->nb_samples = s->frame_size;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;
    output = (int16_t *)frame->data[0];

    /* if there is not enough data left for the smallest possible frame or the
       next 5 bits are a terminator code, reset the libspeex buffer using the
       current packet, otherwise ignore the current packet and keep decoding
       frames from the libspeex buffer. */
    if (speex_bits_remaining(&s->bits) < 5 ||
        speex_bits_peek_unsigned(&s->bits, 5) == 0xF) {
        /* check for flush packet */
        if (!buf || !buf_size) {
            *got_frame_ptr = 0;
            return buf_size;
        }
        if (s->pktsize && buf_size == 62)
            buf_size = s->pktsize;
        /* set new buffer */
        speex_bits_read_from(&s->bits, buf, buf_size);
        consumed = avpkt->size;
    }

    /* decode a single frame */
    ret = speex_decode_int(s->dec_state, &s->bits, output);
    if (ret <= -2) {
        av_log(avctx, AV_LOG_ERROR, "Error decoding Speex frame.\n");
        return AVERROR_INVALIDDATA;
    }
    if (avctx->ch_layout.nb_channels == 2)
        speex_decode_stereo_int(output, s->frame_size, &s->stereo);

    *got_frame_ptr = 1;

    if (!avctx->bit_rate)
        speex_decoder_ctl(s->dec_state, SPEEX_GET_BITRATE, &avctx->bit_rate);
    return consumed;
}

static av_cold int libspeex_decode_close(AVCodecContext *avctx)
{
    LibSpeexContext *s = avctx->priv_data;

    speex_bits_destroy(&s->bits);
    speex_decoder_destroy(s->dec_state);

    return 0;
}

static av_cold void libspeex_decode_flush(AVCodecContext *avctx)
{
    LibSpeexContext *s = avctx->priv_data;
    speex_bits_reset(&s->bits);
}

const FFCodec ff_libspeex_decoder = {
    .p.name         = "libspeex",
    CODEC_LONG_NAME("libspeex Speex"),
    .p.type         = AVMEDIA_TYPE_AUDIO,
    .p.id           = AV_CODEC_ID_SPEEX,
    .p.capabilities =
#if FF_API_SUBFRAMES
                      AV_CODEC_CAP_SUBFRAMES |
#endif
                      AV_CODEC_CAP_DELAY | AV_CODEC_CAP_DR1 | AV_CODEC_CAP_CHANNEL_CONF,
    .p.wrapper_name = "libspeex",
    .caps_internal  = FF_CODEC_CAP_NOT_INIT_THREADSAFE,
    .priv_data_size = sizeof(LibSpeexContext),
    .init           = libspeex_decode_init,
    .close          = libspeex_decode_close,
    FF_CODEC_DECODE_CB(libspeex_decode_frame),
    .flush          = libspeex_decode_flush,
};
