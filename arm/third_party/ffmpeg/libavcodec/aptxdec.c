/*
 * Audio Processing Technology codec for Bluetooth (aptX)
 *
 * Copyright (C) 2017  Aurelien Jacobs <aurel@gnuage.org>
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

#include "libavutil/channel_layout.h"
#include "aptx.h"
#include "codec_internal.h"
#include "decode.h"

/*
 * Half-band QMF synthesis filter realized with a polyphase FIR filter.
 * Join 2 subbands and upsample by 2.
 * So for each 2 subbands sample that goes in, a pair of samples goes out.
 */
av_always_inline
static void aptx_qmf_polyphase_synthesis(FilterSignal signal[NB_FILTERS],
                                         const int32_t coeffs[NB_FILTERS][FILTER_TAPS],
                                         int shift,
                                         int32_t low_subband_input,
                                         int32_t high_subband_input,
                                         int32_t samples[NB_FILTERS])
{
    int32_t subbands[NB_FILTERS];
    int i;

    subbands[0] = low_subband_input + high_subband_input;
    subbands[1] = low_subband_input - high_subband_input;

    for (i = 0; i < NB_FILTERS; i++) {
        aptx_qmf_filter_signal_push(&signal[i], subbands[1-i]);
        samples[i] = aptx_qmf_convolution(&signal[i], coeffs[i], shift);
    }
}

/*
 * Two stage QMF synthesis tree.
 * Join 4 subbands and upsample by 4.
 * So for each 4 subbands sample that goes in, a group of 4 samples goes out.
 */
static void aptx_qmf_tree_synthesis(QMFAnalysis *qmf,
                                    int32_t subband_samples[4],
                                    int32_t samples[4])
{
    int32_t intermediate_samples[4];
    int i;

    /* Join 4 subbands into 2 intermediate subbands upsampled to 2 samples. */
    for (i = 0; i < 2; i++)
        aptx_qmf_polyphase_synthesis(qmf->inner_filter_signal[i],
                                     aptx_qmf_inner_coeffs, 22,
                                     subband_samples[2*i+0],
                                     subband_samples[2*i+1],
                                     &intermediate_samples[2*i]);

    /* Join 2 samples from intermediate subbands upsampled to 4 samples. */
    for (i = 0; i < 2; i++)
        aptx_qmf_polyphase_synthesis(qmf->outer_filter_signal,
                                     aptx_qmf_outer_coeffs, 21,
                                     intermediate_samples[0+i],
                                     intermediate_samples[2+i],
                                     &samples[2*i]);
}


static void aptx_decode_channel(Channel *channel, int32_t samples[4])
{
    int32_t subband_samples[4];
    int subband;
    for (subband = 0; subband < NB_SUBBANDS; subband++)
        subband_samples[subband] = channel->prediction[subband].previous_reconstructed_sample;
    aptx_qmf_tree_synthesis(&channel->qmf, subband_samples, samples);
}

static void aptx_unpack_codeword(Channel *channel, uint16_t codeword)
{
    channel->quantize[0].quantized_sample = sign_extend(codeword >>  0, 7);
    channel->quantize[1].quantized_sample = sign_extend(codeword >>  7, 4);
    channel->quantize[2].quantized_sample = sign_extend(codeword >> 11, 2);
    channel->quantize[3].quantized_sample = sign_extend(codeword >> 13, 3);
    channel->quantize[3].quantized_sample = (channel->quantize[3].quantized_sample & ~1)
                                          | aptx_quantized_parity(channel);
}

static void aptxhd_unpack_codeword(Channel *channel, uint32_t codeword)
{
    channel->quantize[0].quantized_sample = sign_extend(codeword >>  0, 9);
    channel->quantize[1].quantized_sample = sign_extend(codeword >>  9, 6);
    channel->quantize[2].quantized_sample = sign_extend(codeword >> 15, 4);
    channel->quantize[3].quantized_sample = sign_extend(codeword >> 19, 5);
    channel->quantize[3].quantized_sample = (channel->quantize[3].quantized_sample & ~1)
                                          | aptx_quantized_parity(channel);
}

static int aptx_decode_samples(AptXContext *ctx,
                                const uint8_t *input,
                                int32_t samples[NB_CHANNELS][4])
{
    int channel, ret;

    for (channel = 0; channel < NB_CHANNELS; channel++) {
        ff_aptx_generate_dither(&ctx->channels[channel]);

        if (ctx->hd)
            aptxhd_unpack_codeword(&ctx->channels[channel],
                                   AV_RB24(input + 3*channel));
        else
            aptx_unpack_codeword(&ctx->channels[channel],
                                 AV_RB16(input + 2*channel));
        ff_aptx_invert_quantize_and_prediction(&ctx->channels[channel], ctx->hd);
    }

    ret = aptx_check_parity(ctx->channels, &ctx->sync_idx);

    for (channel = 0; channel < NB_CHANNELS; channel++)
        aptx_decode_channel(&ctx->channels[channel], samples[channel]);

    return ret;
}

static int aptx_decode_frame(AVCodecContext *avctx, AVFrame *frame,
                             int *got_frame_ptr, AVPacket *avpkt)
{
    AptXContext *s = avctx->priv_data;
    int pos, opos, channel, sample, ret;

    if (avpkt->size < s->block_size) {
        av_log(avctx, AV_LOG_ERROR, "Packet is too small\n");
        return AVERROR_INVALIDDATA;
    }

    /* get output buffer */
    frame->ch_layout.nb_channels = NB_CHANNELS;
    frame->format = AV_SAMPLE_FMT_S32P;
    frame->nb_samples = 4 * (avpkt->size / s->block_size);
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    for (pos = 0, opos = 0; opos < frame->nb_samples; pos += s->block_size, opos += 4) {
        int32_t samples[NB_CHANNELS][4];

        if (aptx_decode_samples(s, &avpkt->data[pos], samples)) {
            av_log(avctx, AV_LOG_ERROR, "Synchronization error\n");
            return AVERROR_INVALIDDATA;
        }

        for (channel = 0; channel < NB_CHANNELS; channel++)
            for (sample = 0; sample < 4; sample++)
                AV_WN32A(&frame->data[channel][4*(opos+sample)],
                         samples[channel][sample] * 256);
    }

    *got_frame_ptr = 1;
    return s->block_size * frame->nb_samples / 4;
}

#if CONFIG_APTX_DECODER
const FFCodec ff_aptx_decoder = {
    .p.name                = "aptx",
    CODEC_LONG_NAME("aptX (Audio Processing Technology for Bluetooth)"),
    .p.type                = AVMEDIA_TYPE_AUDIO,
    .p.id                  = AV_CODEC_ID_APTX,
    .priv_data_size        = sizeof(AptXContext),
    .init                  = ff_aptx_init,
    FF_CODEC_DECODE_CB(aptx_decode_frame),
    .p.capabilities        = AV_CODEC_CAP_DR1,
    CODEC_OLD_CHANNEL_LAYOUTS(AV_CH_LAYOUT_STEREO)
    .p.ch_layouts          = (const AVChannelLayout[]) { AV_CHANNEL_LAYOUT_STEREO, { 0 } },
    .p.sample_fmts         = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_S32P,
                                                             AV_SAMPLE_FMT_NONE },
};
#endif

#if CONFIG_APTX_HD_DECODER
const FFCodec ff_aptx_hd_decoder = {
    .p.name                = "aptx_hd",
    CODEC_LONG_NAME("aptX HD (Audio Processing Technology for Bluetooth)"),
    .p.type                = AVMEDIA_TYPE_AUDIO,
    .p.id                  = AV_CODEC_ID_APTX_HD,
    .priv_data_size        = sizeof(AptXContext),
    .init                  = ff_aptx_init,
    FF_CODEC_DECODE_CB(aptx_decode_frame),
    .p.capabilities        = AV_CODEC_CAP_DR1,
    CODEC_OLD_CHANNEL_LAYOUTS(AV_CH_LAYOUT_STEREO)
    .p.ch_layouts          = (const AVChannelLayout[]) { AV_CHANNEL_LAYOUT_STEREO, { 0 } },
    .p.sample_fmts         = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_S32P,
                                                             AV_SAMPLE_FMT_NONE },
};
#endif
