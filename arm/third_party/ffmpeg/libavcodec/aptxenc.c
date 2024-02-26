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
#include "audio_frame_queue.h"
#include "codec_internal.h"
#include "encode.h"
#include "internal.h"

typedef struct AptXEncContext {
    AptXContext common;
    AudioFrameQueue afq;
} AptXEncContext;

/*
 * Half-band QMF analysis filter realized with a polyphase FIR filter.
 * Split into 2 subbands and downsample by 2.
 * So for each pair of samples that goes in, one sample goes out,
 * split into 2 separate subbands.
 */
av_always_inline
static void aptx_qmf_polyphase_analysis(FilterSignal signal[NB_FILTERS],
                                        const int32_t coeffs[NB_FILTERS][FILTER_TAPS],
                                        int shift,
                                        int32_t samples[NB_FILTERS],
                                        int32_t *low_subband_output,
                                        int32_t *high_subband_output)
{
    int32_t subbands[NB_FILTERS];
    int i;

    for (i = 0; i < NB_FILTERS; i++) {
        aptx_qmf_filter_signal_push(&signal[i], samples[NB_FILTERS-1-i]);
        subbands[i] = aptx_qmf_convolution(&signal[i], coeffs[i], shift);
    }

    *low_subband_output  = av_clip_intp2(subbands[0] + subbands[1], 23);
    *high_subband_output = av_clip_intp2(subbands[0] - subbands[1], 23);
}

/*
 * Two stage QMF analysis tree.
 * Split 4 input samples into 4 subbands and downsample by 4.
 * So for each group of 4 samples that goes in, one sample goes out,
 * split into 4 separate subbands.
 */
static void aptx_qmf_tree_analysis(QMFAnalysis *qmf,
                                   int32_t samples[4],
                                   int32_t subband_samples[4])
{
    int32_t intermediate_samples[4];
    int i;

    /* Split 4 input samples into 2 intermediate subbands downsampled to 2 samples */
    for (i = 0; i < 2; i++)
        aptx_qmf_polyphase_analysis(qmf->outer_filter_signal,
                                    aptx_qmf_outer_coeffs, 23,
                                    &samples[2*i],
                                    &intermediate_samples[0+i],
                                    &intermediate_samples[2+i]);

    /* Split 2 intermediate subband samples into 4 final subbands downsampled to 1 sample */
    for (i = 0; i < 2; i++)
        aptx_qmf_polyphase_analysis(qmf->inner_filter_signal[i],
                                    aptx_qmf_inner_coeffs, 23,
                                    &intermediate_samples[2*i],
                                    &subband_samples[2*i+0],
                                    &subband_samples[2*i+1]);
}

av_always_inline
static int32_t aptx_bin_search(int32_t value, int32_t factor,
                               const int32_t *intervals, int32_t nb_intervals)
{
    int32_t idx = 0;
    int i;

    for (i = nb_intervals >> 1; i > 0; i >>= 1)
        if (MUL64(factor, intervals[idx + i]) <= ((int64_t)value << 24))
            idx += i;

    return idx;
}

static void aptx_quantize_difference(Quantize *quantize,
                                     int32_t sample_difference,
                                     int32_t dither,
                                     int32_t quantization_factor,
                                     ConstTables *tables)
{
    const int32_t *intervals = tables->quantize_intervals;
    int32_t quantized_sample, dithered_sample, parity_change;
    int32_t d, mean, interval, inv, sample_difference_abs;
    int64_t error;

    sample_difference_abs = FFABS(sample_difference);
    sample_difference_abs = FFMIN(sample_difference_abs, (1 << 23) - 1);

    quantized_sample = aptx_bin_search(sample_difference_abs >> 4,
                                       quantization_factor,
                                       intervals, tables->tables_size);

    d = rshift32_clip24(MULH(dither, dither), 7) - (1 << 23);
    d = rshift64(MUL64(d, tables->quantize_dither_factors[quantized_sample]), 23);

    intervals += quantized_sample;
    mean = (intervals[1] + intervals[0]) / 2;
    interval = (intervals[1] - intervals[0]) * (-(sample_difference < 0) | 1);

    dithered_sample = rshift64_clip24(MUL64(dither, interval) + ((int64_t)av_clip_intp2(mean + d, 23) << 32), 32);
    error = ((int64_t)sample_difference_abs << 20) - MUL64(dithered_sample, quantization_factor);
    quantize->error = FFABS(rshift64(error, 23));

    parity_change = quantized_sample;
    if (error < 0)
        quantized_sample--;
    else
        parity_change--;

    inv = -(sample_difference < 0);
    quantize->quantized_sample               = quantized_sample ^ inv;
    quantize->quantized_sample_parity_change = parity_change    ^ inv;
}

static void aptx_encode_channel(Channel *channel, int32_t samples[4], int hd)
{
    int32_t subband_samples[4];
    int subband;
    aptx_qmf_tree_analysis(&channel->qmf, samples, subband_samples);
    ff_aptx_generate_dither(channel);
    for (subband = 0; subband < NB_SUBBANDS; subband++) {
        int32_t diff = av_clip_intp2(subband_samples[subband] - channel->prediction[subband].predicted_sample, 23);
        aptx_quantize_difference(&channel->quantize[subband], diff,
                                 channel->dither[subband],
                                 channel->invert_quantize[subband].quantization_factor,
                                 &ff_aptx_quant_tables[hd][subband]);
    }
}

static void aptx_insert_sync(Channel channels[NB_CHANNELS], int32_t *idx)
{
    if (aptx_check_parity(channels, idx)) {
        int i;
        Channel *c;
        static const int map[] = { 1, 2, 0, 3 };
        Quantize *min = &channels[NB_CHANNELS-1].quantize[map[0]];
        for (c = &channels[NB_CHANNELS-1]; c >= channels; c--)
            for (i = 0; i < NB_SUBBANDS; i++)
                if (c->quantize[map[i]].error < min->error)
                    min = &c->quantize[map[i]];

        /* Forcing the desired parity is done by offsetting by 1 the quantized
         * sample from the subband featuring the smallest quantization error. */
        min->quantized_sample = min->quantized_sample_parity_change;
    }
}

static uint16_t aptx_pack_codeword(Channel *channel)
{
    int32_t parity = aptx_quantized_parity(channel);
    return (((channel->quantize[3].quantized_sample & 0x06) | parity) << 13)
         | (((channel->quantize[2].quantized_sample & 0x03)         ) << 11)
         | (((channel->quantize[1].quantized_sample & 0x0F)         ) <<  7)
         | (((channel->quantize[0].quantized_sample & 0x7F)         ) <<  0);
}

static uint32_t aptxhd_pack_codeword(Channel *channel)
{
    int32_t parity = aptx_quantized_parity(channel);
    return (((channel->quantize[3].quantized_sample & 0x01E) | parity) << 19)
         | (((channel->quantize[2].quantized_sample & 0x00F)         ) << 15)
         | (((channel->quantize[1].quantized_sample & 0x03F)         ) <<  9)
         | (((channel->quantize[0].quantized_sample & 0x1FF)         ) <<  0);
}

static void aptx_encode_samples(AptXContext *ctx,
                                int32_t samples[NB_CHANNELS][4],
                                uint8_t *output)
{
    int channel;
    for (channel = 0; channel < NB_CHANNELS; channel++)
        aptx_encode_channel(&ctx->channels[channel], samples[channel], ctx->hd);

    aptx_insert_sync(ctx->channels, &ctx->sync_idx);

    for (channel = 0; channel < NB_CHANNELS; channel++) {
        ff_aptx_invert_quantize_and_prediction(&ctx->channels[channel], ctx->hd);
        if (ctx->hd)
            AV_WB24(output + 3*channel,
                    aptxhd_pack_codeword(&ctx->channels[channel]));
        else
            AV_WB16(output + 2*channel,
                    aptx_pack_codeword(&ctx->channels[channel]));
    }
}

static int aptx_encode_frame(AVCodecContext *avctx, AVPacket *avpkt,
                             const AVFrame *frame, int *got_packet_ptr)
{
    AptXEncContext *const s0 = avctx->priv_data;
    AptXContext *const s = &s0->common;
    int pos, ipos, channel, sample, output_size, ret;

    if ((ret = ff_af_queue_add(&s0->afq, frame)) < 0)
        return ret;

    output_size = s->block_size * frame->nb_samples/4;
    if ((ret = ff_get_encode_buffer(avctx, avpkt, output_size, 0)) < 0)
        return ret;

    for (pos = 0, ipos = 0; pos < output_size; pos += s->block_size, ipos += 4) {
        int32_t samples[NB_CHANNELS][4];

        for (channel = 0; channel < NB_CHANNELS; channel++)
            for (sample = 0; sample < 4; sample++)
                samples[channel][sample] = (int32_t)AV_RN32A(&frame->data[channel][4*(ipos+sample)]) >> 8;

        aptx_encode_samples(s, samples, avpkt->data + pos);
    }

    ff_af_queue_remove(&s0->afq, frame->nb_samples, &avpkt->pts, &avpkt->duration);
    *got_packet_ptr = 1;
    return 0;
}

static av_cold int aptx_close(AVCodecContext *avctx)
{
    AptXEncContext *const s = avctx->priv_data;
    ff_af_queue_close(&s->afq);
    return 0;
}

static av_cold int aptx_encode_init(AVCodecContext *avctx)
{
    AptXEncContext *const s = avctx->priv_data;

    ff_af_queue_init(avctx, &s->afq);

    if (!avctx->frame_size || avctx->frame_size % 4)
        avctx->frame_size = 1024;
    avctx->internal->pad_samples = 4;

    return ff_aptx_init(avctx);
}

#if CONFIG_APTX_ENCODER
const FFCodec ff_aptx_encoder = {
    .p.name                = "aptx",
    CODEC_LONG_NAME("aptX (Audio Processing Technology for Bluetooth)"),
    .p.type                = AVMEDIA_TYPE_AUDIO,
    .p.id                  = AV_CODEC_ID_APTX,
    .p.capabilities        = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE,
    .priv_data_size        = sizeof(AptXEncContext),
    .init                  = aptx_encode_init,
    FF_CODEC_ENCODE_CB(aptx_encode_frame),
    .close                 = aptx_close,
    CODEC_OLD_CHANNEL_LAYOUTS(AV_CH_LAYOUT_STEREO)
    .p.ch_layouts          = (const AVChannelLayout[]) { AV_CHANNEL_LAYOUT_STEREO, { 0 } },
    .p.sample_fmts         = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_S32P,
                                                             AV_SAMPLE_FMT_NONE },
    .p.supported_samplerates = (const int[]) {8000, 16000, 24000, 32000, 44100, 48000, 0},
};
#endif

#if CONFIG_APTX_HD_ENCODER
const FFCodec ff_aptx_hd_encoder = {
    .p.name                = "aptx_hd",
    CODEC_LONG_NAME("aptX HD (Audio Processing Technology for Bluetooth)"),
    .p.type                = AVMEDIA_TYPE_AUDIO,
    .p.id                  = AV_CODEC_ID_APTX_HD,
    .p.capabilities        = AV_CODEC_CAP_DR1 | AV_CODEC_CAP_ENCODER_REORDERED_OPAQUE,
    .priv_data_size        = sizeof(AptXEncContext),
    .init                  = aptx_encode_init,
    FF_CODEC_ENCODE_CB(aptx_encode_frame),
    .close                 = aptx_close,
    CODEC_OLD_CHANNEL_LAYOUTS(AV_CH_LAYOUT_STEREO)
    .p.ch_layouts          = (const AVChannelLayout[]) { AV_CHANNEL_LAYOUT_STEREO, { 0 } },
    .p.sample_fmts         = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_S32P,
                                                             AV_SAMPLE_FMT_NONE },
    .p.supported_samplerates = (const int[]) {8000, 16000, 24000, 32000, 44100, 48000, 0},
};
#endif
