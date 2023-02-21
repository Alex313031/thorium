/*
 * AAC encoder
 * Copyright (C) 2008 Konstantin Shishkov
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

#ifndef AVCODEC_AACENC_H
#define AVCODEC_AACENC_H

#include "libavutil/channel_layout.h"
#include "libavutil/float_dsp.h"
#include "libavutil/mem_internal.h"

#include "avcodec.h"
#include "put_bits.h"

#include "aac.h"
#include "audio_frame_queue.h"
#include "psymodel.h"

#include "lpc.h"

typedef enum AACCoder {
    AAC_CODER_ANMR = 0,
    AAC_CODER_TWOLOOP,
    AAC_CODER_FAST,

    AAC_CODER_NB,
}AACCoder;

typedef struct AACEncOptions {
    int coder;
    int pns;
    int tns;
    int ltp;
    int pce;
    int pred;
    int mid_side;
    int intensity_stereo;
} AACEncOptions;

struct AACEncContext;

typedef struct AACCoefficientsEncoder {
    void (*search_for_quantizers)(AVCodecContext *avctx, struct AACEncContext *s,
                                  SingleChannelElement *sce, const float lambda);
    void (*encode_window_bands_info)(struct AACEncContext *s, SingleChannelElement *sce,
                                     int win, int group_len, const float lambda);
    void (*quantize_and_encode_band)(struct AACEncContext *s, PutBitContext *pb, const float *in, float *out, int size,
                                     int scale_idx, int cb, const float lambda, int rtz);
    void (*encode_tns_info)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*encode_ltp_info)(struct AACEncContext *s, SingleChannelElement *sce, int common_window);
    void (*encode_main_pred)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*adjust_common_pred)(struct AACEncContext *s, ChannelElement *cpe);
    void (*adjust_common_ltp)(struct AACEncContext *s, ChannelElement *cpe);
    void (*apply_main_pred)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*apply_tns_filt)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*update_ltp)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*ltp_insert_new_frame)(struct AACEncContext *s);
    void (*set_special_band_scalefactors)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*search_for_pns)(struct AACEncContext *s, AVCodecContext *avctx, SingleChannelElement *sce);
    void (*mark_pns)(struct AACEncContext *s, AVCodecContext *avctx, SingleChannelElement *sce);
    void (*search_for_tns)(struct AACEncContext *s, SingleChannelElement *sce);
    void (*search_for_ltp)(struct AACEncContext *s, SingleChannelElement *sce, int common_window);
    void (*search_for_ms)(struct AACEncContext *s, ChannelElement *cpe);
    void (*search_for_is)(struct AACEncContext *s, AVCodecContext *avctx, ChannelElement *cpe);
    void (*search_for_pred)(struct AACEncContext *s, SingleChannelElement *sce);
} AACCoefficientsEncoder;

extern const AACCoefficientsEncoder ff_aac_coders[];

typedef struct AACQuantizeBandCostCacheEntry {
    float rd;
    float energy;
    int bits;
    char cb;
    char rtz;
    uint16_t generation;
} AACQuantizeBandCostCacheEntry;

typedef struct AACPCEInfo {
    AVChannelLayout layout;
    int num_ele[4];                              ///< front, side, back, lfe
    int pairing[3][8];                           ///< front, side, back
    int index[4][8];                             ///< front, side, back, lfe
    uint8_t config_map[16];                      ///< configs the encoder's channel specific settings
    uint8_t reorder_map[16];                     ///< maps channels from lavc to aac order
} AACPCEInfo;

/**
 * AAC encoder context
 */
typedef struct AACEncContext {
    AVClass *av_class;
    AACEncOptions options;                       ///< encoding options
    PutBitContext pb;
    AVTXContext *mdct1024;                       ///< long (1024 samples) frame transform context
    av_tx_fn mdct1024_fn;
    AVTXContext *mdct128;                        ///< short (128 samples) frame transform context
    av_tx_fn mdct128_fn;
    AVFloatDSPContext *fdsp;
    AACPCEInfo pce;                              ///< PCE data, if needed
    float *planar_samples[16];                   ///< saved preprocessed input

    int profile;                                 ///< copied from avctx
    int needs_pce;                               ///< flag for non-standard layout
    LPCContext lpc;                              ///< used by TNS
    int samplerate_index;                        ///< MPEG-4 samplerate index
    int channels;                                ///< channel count
    const uint8_t *reorder_map;                  ///< lavc to aac reorder map
    const uint8_t *chan_map;                     ///< channel configuration map

    ChannelElement *cpe;                         ///< channel elements
    FFPsyContext psy;
    struct FFPsyPreprocessContext* psypp;
    const AACCoefficientsEncoder *coder;
    int cur_channel;                             ///< current channel for coder context
    int random_state;
    float lambda;
    int last_frame_pb_count;                     ///< number of bits for the previous frame
    float lambda_sum;                            ///< sum(lambda), for Qvg reporting
    int lambda_count;                            ///< count(lambda), for Qvg reporting
    enum RawDataBlockType cur_type;              ///< channel group type cur_channel belongs to

    AudioFrameQueue afq;
    DECLARE_ALIGNED(16, int,   qcoefs)[96];      ///< quantized coefficients
    DECLARE_ALIGNED(32, float, scoefs)[1024];    ///< scaled coefficients

    uint16_t quantize_band_cost_cache_generation;
    AACQuantizeBandCostCacheEntry quantize_band_cost_cache[256][128]; ///< memoization area for quantize_band_cost

    void (*abs_pow34)(float *out, const float *in, const int size);
    void (*quant_bands)(int *out, const float *in, const float *scaled,
                        int size, int is_signed, int maxval, const float Q34,
                        const float rounding);

    struct {
        float *samples;
    } buffer;
} AACEncContext;

void ff_aac_dsp_init_x86(AACEncContext *s);
void ff_aac_coder_init_mips(AACEncContext *c);
void ff_quantize_band_cost_cache_init(struct AACEncContext *s);


#endif /* AVCODEC_AACENC_H */
