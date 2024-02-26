/*
 * HEVC video decoder
 *
 * Copyright (C) 2012 - 2013 Guillaume Martres
 * Copyright (C) 2012 - 2013 Gildas Cocherel
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

#include "libavutil/avassert.h"

#include "decode.h"
#include "thread.h"
#include "hevc.h"
#include "hevcdec.h"
#include "refstruct.h"
#include "threadframe.h"

void ff_hevc_unref_frame(HEVCFrame *frame, int flags)
{
    /* frame->frame can be NULL if context init failed */
    if (!frame->frame || !frame->frame->buf[0])
        return;

    frame->flags &= ~flags;
    if (!frame->flags) {
        ff_thread_release_ext_buffer(&frame->tf);
        av_frame_unref(frame->frame_grain);
        frame->needs_fg = 0;

        ff_refstruct_unref(&frame->tab_mvf);

        ff_refstruct_unref(&frame->rpl);
        frame->nb_rpl_elems = 0;
        ff_refstruct_unref(&frame->rpl_tab);
        frame->refPicList = NULL;

        ff_refstruct_unref(&frame->hwaccel_picture_private);
    }
}

const RefPicList *ff_hevc_get_ref_list(const HEVCContext *s,
                                       const HEVCFrame *ref, int x0, int y0)
{
    int x_cb         = x0 >> s->ps.sps->log2_ctb_size;
    int y_cb         = y0 >> s->ps.sps->log2_ctb_size;
    int pic_width_cb = s->ps.sps->ctb_width;
    int ctb_addr_ts  = s->ps.pps->ctb_addr_rs_to_ts[y_cb * pic_width_cb + x_cb];
    return &ref->rpl_tab[ctb_addr_ts]->refPicList[0];
}

void ff_hevc_clear_refs(HEVCContext *s)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++)
        ff_hevc_unref_frame(&s->DPB[i],
                            HEVC_FRAME_FLAG_SHORT_REF |
                            HEVC_FRAME_FLAG_LONG_REF);
}

void ff_hevc_flush_dpb(HEVCContext *s)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++)
        ff_hevc_unref_frame(&s->DPB[i], ~0);
}

static HEVCFrame *alloc_frame(HEVCContext *s)
{
    int i, j, ret;
    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
        HEVCFrame *frame = &s->DPB[i];
        if (frame->frame->buf[0])
            continue;

        ret = ff_thread_get_ext_buffer(s->avctx, &frame->tf,
                                       AV_GET_BUFFER_FLAG_REF);
        if (ret < 0)
            return NULL;

        frame->rpl = ff_refstruct_allocz(s->pkt.nb_nals * sizeof(*frame->rpl));
        if (!frame->rpl)
            goto fail;
        frame->nb_rpl_elems = s->pkt.nb_nals;

        frame->tab_mvf = ff_refstruct_pool_get(s->tab_mvf_pool);
        if (!frame->tab_mvf)
            goto fail;

        frame->rpl_tab = ff_refstruct_pool_get(s->rpl_tab_pool);
        if (!frame->rpl_tab)
            goto fail;
        frame->ctb_count = s->ps.sps->ctb_width * s->ps.sps->ctb_height;
        for (j = 0; j < frame->ctb_count; j++)
            frame->rpl_tab[j] = frame->rpl;

        if (s->sei.picture_timing.picture_struct == AV_PICTURE_STRUCTURE_TOP_FIELD)
            frame->frame->flags |= AV_FRAME_FLAG_TOP_FIELD_FIRST;
        if ((s->sei.picture_timing.picture_struct == AV_PICTURE_STRUCTURE_TOP_FIELD) ||
            (s->sei.picture_timing.picture_struct == AV_PICTURE_STRUCTURE_BOTTOM_FIELD))
            frame->frame->flags |= AV_FRAME_FLAG_INTERLACED;

        ret = ff_hwaccel_frame_priv_alloc(s->avctx, &frame->hwaccel_picture_private);
        if (ret < 0)
            goto fail;

        return frame;
fail:
        ff_hevc_unref_frame(frame, ~0);
        return NULL;
    }
    av_log(s->avctx, AV_LOG_ERROR, "Error allocating frame, DPB full.\n");
    return NULL;
}

int ff_hevc_set_new_ref(HEVCContext *s, AVFrame **frame, int poc)
{
    HEVCFrame *ref;
    int i;

    /* check that this POC doesn't already exist */
    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
        HEVCFrame *frame = &s->DPB[i];

        if (frame->frame->buf[0] && frame->sequence == s->seq_decode &&
            frame->poc == poc) {
            av_log(s->avctx, AV_LOG_ERROR, "Duplicate POC in a sequence: %d.\n",
                   poc);
            return AVERROR_INVALIDDATA;
        }
    }

    ref = alloc_frame(s);
    if (!ref)
        return AVERROR(ENOMEM);

    *frame = ref->frame;
    s->ref = ref;
    s->collocated_ref = NULL;

    if (s->sh.pic_output_flag)
        ref->flags = HEVC_FRAME_FLAG_OUTPUT | HEVC_FRAME_FLAG_SHORT_REF;
    else
        ref->flags = HEVC_FRAME_FLAG_SHORT_REF;

    ref->poc      = poc;
    ref->sequence = s->seq_decode;
    ref->frame->crop_left   = s->ps.sps->output_window.left_offset;
    ref->frame->crop_right  = s->ps.sps->output_window.right_offset;
    ref->frame->crop_top    = s->ps.sps->output_window.top_offset;
    ref->frame->crop_bottom = s->ps.sps->output_window.bottom_offset;

    return 0;
}

static void unref_missing_refs(HEVCContext *s)
{
    for (int i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
         HEVCFrame *frame = &s->DPB[i];
         if (frame->sequence == HEVC_SEQUENCE_COUNTER_INVALID) {
             ff_hevc_unref_frame(frame, ~0);
         }
    }
}

int ff_hevc_output_frame(HEVCContext *s, AVFrame *out, int flush)
{
    if (IS_IRAP(s) && s->no_rasl_output_flag == 1) {
        const static int mask = HEVC_FRAME_FLAG_BUMPING | HEVC_FRAME_FLAG_OUTPUT;
        for (int i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
            HEVCFrame *frame = &s->DPB[i];
            if ((frame->flags & mask) == HEVC_FRAME_FLAG_OUTPUT &&
                frame->sequence != s->seq_decode) {
                if (s->sh.no_output_of_prior_pics_flag == 1)
                    ff_hevc_unref_frame(frame, HEVC_FRAME_FLAG_OUTPUT);
                else
                    frame->flags |= HEVC_FRAME_FLAG_BUMPING;
            }
        }
    }
    do {
        int nb_output = 0;
        int min_poc   = INT_MAX;
        int i, min_idx, ret;

        for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
            HEVCFrame *frame = &s->DPB[i];
            if ((frame->flags & HEVC_FRAME_FLAG_OUTPUT) &&
                frame->sequence == s->seq_output) {
                nb_output++;
                if (frame->poc < min_poc || nb_output == 1) {
                    min_poc = frame->poc;
                    min_idx = i;
                }
            }
        }

        /* wait for more frames before output */
        if (!flush && s->seq_output == s->seq_decode && s->ps.sps &&
            nb_output <= s->ps.sps->temporal_layer[s->ps.sps->max_sub_layers - 1].num_reorder_pics)
            return 0;

        if (nb_output) {
            HEVCFrame *frame = &s->DPB[min_idx];

            ret = av_frame_ref(out, frame->needs_fg ? frame->frame_grain : frame->frame);
            if (frame->flags & HEVC_FRAME_FLAG_BUMPING)
                ff_hevc_unref_frame(frame, HEVC_FRAME_FLAG_OUTPUT | HEVC_FRAME_FLAG_BUMPING);
            else
                ff_hevc_unref_frame(frame, HEVC_FRAME_FLAG_OUTPUT);
            if (ret < 0)
                return ret;

            if (frame->needs_fg && (ret = av_frame_copy_props(out, frame->frame)) < 0)
                return ret;

            if (!(s->avctx->export_side_data & AV_CODEC_EXPORT_DATA_FILM_GRAIN))
                av_frame_remove_side_data(out, AV_FRAME_DATA_FILM_GRAIN_PARAMS);

            av_log(s->avctx, AV_LOG_DEBUG,
                   "Output frame with POC %d.\n", frame->poc);
            return 1;
        }

        if (s->seq_output != s->seq_decode)
            s->seq_output = (s->seq_output + 1) & HEVC_SEQUENCE_COUNTER_MASK;
        else
            break;
    } while (1);

    return 0;
}

void ff_hevc_bump_frame(HEVCContext *s)
{
    int dpb = 0;
    int min_poc = INT_MAX;
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
        HEVCFrame *frame = &s->DPB[i];
        if ((frame->flags) &&
            frame->sequence == s->seq_output &&
            frame->poc != s->poc) {
            dpb++;
        }
    }

    if (s->ps.sps && dpb >= s->ps.sps->temporal_layer[s->ps.sps->max_sub_layers - 1].max_dec_pic_buffering) {
        for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
            HEVCFrame *frame = &s->DPB[i];
            if ((frame->flags) &&
                frame->sequence == s->seq_output &&
                frame->poc != s->poc) {
                if (frame->flags == HEVC_FRAME_FLAG_OUTPUT && frame->poc < min_poc) {
                    min_poc = frame->poc;
                }
            }
        }

        for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
            HEVCFrame *frame = &s->DPB[i];
            if (frame->flags & HEVC_FRAME_FLAG_OUTPUT &&
                frame->sequence == s->seq_output &&
                frame->poc <= min_poc) {
                frame->flags |= HEVC_FRAME_FLAG_BUMPING;
            }
        }

        dpb--;
    }
}

static int init_slice_rpl(HEVCContext *s)
{
    HEVCFrame *frame = s->ref;
    int ctb_count    = frame->ctb_count;
    int ctb_addr_ts  = s->ps.pps->ctb_addr_rs_to_ts[s->sh.slice_segment_addr];
    int i;

    if (s->slice_idx >= frame->nb_rpl_elems)
        return AVERROR_INVALIDDATA;

    for (i = ctb_addr_ts; i < ctb_count; i++)
        frame->rpl_tab[i] = frame->rpl + s->slice_idx;

    frame->refPicList = (RefPicList *)frame->rpl_tab[ctb_addr_ts];

    return 0;
}

int ff_hevc_slice_rpl(HEVCContext *s)
{
    SliceHeader *sh = &s->sh;

    uint8_t nb_list = sh->slice_type == HEVC_SLICE_B ? 2 : 1;
    uint8_t list_idx;
    int i, j, ret;

    ret = init_slice_rpl(s);
    if (ret < 0)
        return ret;

    if (!(s->rps[ST_CURR_BEF].nb_refs + s->rps[ST_CURR_AFT].nb_refs +
          s->rps[LT_CURR].nb_refs) && !s->ps.pps->pps_curr_pic_ref_enabled_flag) {
        av_log(s->avctx, AV_LOG_ERROR, "Zero refs in the frame RPS.\n");
        return AVERROR_INVALIDDATA;
    }

    for (list_idx = 0; list_idx < nb_list; list_idx++) {
        RefPicList  rpl_tmp = { { 0 } };
        RefPicList *rpl     = &s->ref->refPicList[list_idx];

        /* The order of the elements is
         * ST_CURR_BEF - ST_CURR_AFT - LT_CURR for the L0 and
         * ST_CURR_AFT - ST_CURR_BEF - LT_CURR for the L1 */
        int cand_lists[3] = { list_idx ? ST_CURR_AFT : ST_CURR_BEF,
                              list_idx ? ST_CURR_BEF : ST_CURR_AFT,
                              LT_CURR };

        /* concatenate the candidate lists for the current frame */
        while (rpl_tmp.nb_refs < sh->nb_refs[list_idx]) {
            for (i = 0; i < FF_ARRAY_ELEMS(cand_lists); i++) {
                RefPicList *rps = &s->rps[cand_lists[i]];
                for (j = 0; j < rps->nb_refs && rpl_tmp.nb_refs < HEVC_MAX_REFS; j++) {
                    rpl_tmp.list[rpl_tmp.nb_refs]       = rps->list[j];
                    rpl_tmp.ref[rpl_tmp.nb_refs]        = rps->ref[j];
                    rpl_tmp.isLongTerm[rpl_tmp.nb_refs] = i == 2;
                    rpl_tmp.nb_refs++;
                }
            }
            // Construct RefPicList0, RefPicList1 (8-8, 8-10)
            if (s->ps.pps->pps_curr_pic_ref_enabled_flag && rpl_tmp.nb_refs < HEVC_MAX_REFS) {
                rpl_tmp.list[rpl_tmp.nb_refs]           = s->ref->poc;
                rpl_tmp.ref[rpl_tmp.nb_refs]            = s->ref;
                rpl_tmp.isLongTerm[rpl_tmp.nb_refs]     = 1;
                rpl_tmp.nb_refs++;
            }
        }

        /* reorder the references if necessary */
        if (sh->rpl_modification_flag[list_idx]) {
            for (i = 0; i < sh->nb_refs[list_idx]; i++) {
                int idx = sh->list_entry_lx[list_idx][i];

                if (idx >= rpl_tmp.nb_refs) {
                    av_log(s->avctx, AV_LOG_ERROR, "Invalid reference index.\n");
                    return AVERROR_INVALIDDATA;
                }

                rpl->list[i]       = rpl_tmp.list[idx];
                rpl->ref[i]        = rpl_tmp.ref[idx];
                rpl->isLongTerm[i] = rpl_tmp.isLongTerm[idx];
                rpl->nb_refs++;
            }
        } else {
            memcpy(rpl, &rpl_tmp, sizeof(*rpl));
            rpl->nb_refs = FFMIN(rpl->nb_refs, sh->nb_refs[list_idx]);
        }

        // 8-9
        if (s->ps.pps->pps_curr_pic_ref_enabled_flag &&
            !sh->rpl_modification_flag[list_idx] &&
            rpl_tmp.nb_refs > sh->nb_refs[L0]) {
            rpl->list[sh->nb_refs[L0] - 1] = s->ref->poc;
            rpl->ref[sh->nb_refs[L0] - 1]  = s->ref;
        }

        if (sh->collocated_list == list_idx &&
            sh->collocated_ref_idx < rpl->nb_refs)
            s->collocated_ref = rpl->ref[sh->collocated_ref_idx];
    }

    return 0;
}

static HEVCFrame *find_ref_idx(HEVCContext *s, int poc, uint8_t use_msb)
{
    int mask = use_msb ? ~0 : (1 << s->ps.sps->log2_max_poc_lsb) - 1;
    int i;

    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
        HEVCFrame *ref = &s->DPB[i];
        if (ref->frame->buf[0] && ref->sequence == s->seq_decode) {
            if ((ref->poc & mask) == poc && (use_msb || ref->poc != s->poc))
                return ref;
        }
    }

    if (s->nal_unit_type != HEVC_NAL_CRA_NUT && !IS_BLA(s))
        av_log(s->avctx, AV_LOG_ERROR,
               "Could not find ref with POC %d\n", poc);
    return NULL;
}

static void mark_ref(HEVCFrame *frame, int flag)
{
    frame->flags &= ~(HEVC_FRAME_FLAG_LONG_REF | HEVC_FRAME_FLAG_SHORT_REF);
    frame->flags |= flag;
}

static HEVCFrame *generate_missing_ref(HEVCContext *s, int poc)
{
    HEVCFrame *frame;
    int i, y;

    frame = alloc_frame(s);
    if (!frame)
        return NULL;

    if (!s->avctx->hwaccel) {
        if (!s->ps.sps->pixel_shift) {
            for (i = 0; frame->frame->data[i]; i++)
                memset(frame->frame->data[i], 1 << (s->ps.sps->bit_depth - 1),
                       frame->frame->linesize[i] * AV_CEIL_RSHIFT(s->ps.sps->height, s->ps.sps->vshift[i]));
        } else {
            for (i = 0; frame->frame->data[i]; i++)
                for (y = 0; y < (s->ps.sps->height >> s->ps.sps->vshift[i]); y++) {
                    uint8_t *dst = frame->frame->data[i] + y * frame->frame->linesize[i];
                    AV_WN16(dst, 1 << (s->ps.sps->bit_depth - 1));
                    av_memcpy_backptr(dst + 2, 2, 2*(s->ps.sps->width >> s->ps.sps->hshift[i]) - 2);
                }
        }
    }

    frame->poc      = poc;
    frame->sequence = HEVC_SEQUENCE_COUNTER_INVALID;
    frame->flags    = 0;

    if (s->threads_type == FF_THREAD_FRAME)
        ff_thread_report_progress(&frame->tf, INT_MAX, 0);

    return frame;
}

/* add a reference with the given poc to the list and mark it as used in DPB */
static int add_candidate_ref(HEVCContext *s, RefPicList *list,
                             int poc, int ref_flag, uint8_t use_msb)
{
    HEVCFrame *ref = find_ref_idx(s, poc, use_msb);

    if (ref == s->ref || list->nb_refs >= HEVC_MAX_REFS)
        return AVERROR_INVALIDDATA;

    if (!ref) {
        ref = generate_missing_ref(s, poc);
        if (!ref)
            return AVERROR(ENOMEM);
    }

    list->list[list->nb_refs] = ref->poc;
    list->ref[list->nb_refs]  = ref;
    list->nb_refs++;

    mark_ref(ref, ref_flag);
    return 0;
}

int ff_hevc_frame_rps(HEVCContext *s)
{
    const ShortTermRPS *short_rps = s->sh.short_term_rps;
    const LongTermRPS  *long_rps  = &s->sh.long_term_rps;
    RefPicList               *rps = s->rps;
    int i, ret = 0;

    if (!short_rps) {
        rps[0].nb_refs = rps[1].nb_refs = 0;
        return 0;
    }

    unref_missing_refs(s);

    /* clear the reference flags on all frames except the current one */
    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++) {
        HEVCFrame *frame = &s->DPB[i];

        if (frame == s->ref)
            continue;

        mark_ref(frame, 0);
    }

    for (i = 0; i < NB_RPS_TYPE; i++)
        rps[i].nb_refs = 0;

    /* add the short refs */
    for (i = 0; i < short_rps->num_delta_pocs; i++) {
        int poc = s->poc + short_rps->delta_poc[i];
        int list;

        if (!short_rps->used[i])
            list = ST_FOLL;
        else if (i < short_rps->num_negative_pics)
            list = ST_CURR_BEF;
        else
            list = ST_CURR_AFT;

        ret = add_candidate_ref(s, &rps[list], poc, HEVC_FRAME_FLAG_SHORT_REF, 1);
        if (ret < 0)
            goto fail;
    }

    /* add the long refs */
    for (i = 0; i < long_rps->nb_refs; i++) {
        int poc  = long_rps->poc[i];
        int list = long_rps->used[i] ? LT_CURR : LT_FOLL;

        ret = add_candidate_ref(s, &rps[list], poc, HEVC_FRAME_FLAG_LONG_REF, long_rps->poc_msb_present[i]);
        if (ret < 0)
            goto fail;
    }

fail:
    /* release any frames that are now unused */
    for (i = 0; i < FF_ARRAY_ELEMS(s->DPB); i++)
        ff_hevc_unref_frame(&s->DPB[i], 0);

    return ret;
}

int ff_hevc_frame_nb_refs(const HEVCContext *s)
{
    int ret = 0;
    int i;
    const ShortTermRPS *rps = s->sh.short_term_rps;
    const LongTermRPS *long_rps = &s->sh.long_term_rps;

    if (rps) {
        for (i = 0; i < rps->num_negative_pics; i++)
            ret += !!rps->used[i];
        for (; i < rps->num_delta_pocs; i++)
            ret += !!rps->used[i];
    }

    if (long_rps) {
        for (i = 0; i < long_rps->nb_refs; i++)
            ret += !!long_rps->used[i];
    }

    if (s->ps.pps->pps_curr_pic_ref_enabled_flag)
        ret++;

    return ret;
}
