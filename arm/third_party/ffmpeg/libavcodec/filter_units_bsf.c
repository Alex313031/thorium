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

#include <stdlib.h>

#include "libavutil/common.h"
#include "libavutil/opt.h"

#include "bsf.h"
#include "bsf_internal.h"
#include "cbs.h"


typedef struct FilterUnitsContext {
    const AVClass *class;

    CodedBitstreamContext *cbc;
    CodedBitstreamFragment fragment;

    const char *pass_types;
    const char *remove_types;
    enum AVDiscard discard;
    int discard_flags;

    enum {
        NOOP,
        PASS,
        REMOVE,
    } mode;
    CodedBitstreamUnitType *type_list;
    int nb_types;
} FilterUnitsContext;


static int filter_units_make_type_list(const char *list_string,
                                       CodedBitstreamUnitType **type_list,
                                       int *nb_types)
{
    CodedBitstreamUnitType *list = NULL;
    int pass, count;

    for (pass = 1; pass <= 2; pass++) {
        long value, range_start, range_end;
        const char *str;
        char *value_end;

        count = 0;
        for (str = list_string; *str;) {
            value = strtol(str, &value_end, 0);
            if (str == value_end)
                goto invalid;
            str = (const char *)value_end;
            if (*str == '-') {
                ++str;
                range_start = value;
                range_end   = strtol(str, &value_end, 0);
                if (str == value_end)
                    goto invalid;

                for (value = range_start; value < range_end; value++) {
                    if (pass == 2)
                        list[count] = value;
                    ++count;
                }
            } else {
                if (pass == 2)
                    list[count] = value;
                ++count;
            }
            if (*str == '|')
                ++str;
        }
        if (pass == 1) {
            list = av_malloc_array(count, sizeof(*list));
            if (!list)
                return AVERROR(ENOMEM);
        }
    }

    *type_list = list;
    *nb_types  = count;
    return 0;

invalid:
    av_freep(&list);
    return AVERROR(EINVAL);
}

static int filter_units_filter(AVBSFContext *bsf, AVPacket *pkt)
{
    FilterUnitsContext      *ctx = bsf->priv_data;
    CodedBitstreamFragment *frag = &ctx->fragment;
    int err, i, j;

    err = ff_bsf_get_packet_ref(bsf, pkt);
    if (err < 0)
        return err;

    if (ctx->mode == NOOP && ctx->discard <= AVDISCARD_DEFAULT)
        return 0;

    err = ff_cbs_read_packet(ctx->cbc, frag, pkt);
    if (err < 0) {
        av_log(bsf, AV_LOG_ERROR, "Failed to read packet.\n");
        goto fail;
    }

    ff_cbs_discard_units(ctx->cbc, frag, ctx->discard, ctx->discard_flags);
    if (ctx->mode != NOOP) {
        for (i = frag->nb_units - 1; i >= 0; i--) {
            for (j = 0; j < ctx->nb_types; j++) {
                if (frag->units[i].type == ctx->type_list[j])
                    break;
            }
            if (ctx->mode == REMOVE ? j <  ctx->nb_types
                                    : j >= ctx->nb_types)
                ff_cbs_delete_unit(frag, i);
        }
    }

    if (frag->nb_units == 0) {
        // Don't return packets with nothing in them.
        err = AVERROR(EAGAIN);
        goto fail;
    }

    err = ff_cbs_write_packet(ctx->cbc, pkt, frag);
    if (err < 0) {
        av_log(bsf, AV_LOG_ERROR, "Failed to write packet.\n");
        goto fail;
    }

fail:
    if (err < 0)
        av_packet_unref(pkt);
    ff_cbs_fragment_reset(frag);

    return err;
}

static int filter_units_init(AVBSFContext *bsf)
{
    FilterUnitsContext *ctx = bsf->priv_data;
    int err;

    if (ctx->pass_types && ctx->remove_types) {
        av_log(bsf, AV_LOG_ERROR, "Exactly one of pass_types or "
               "remove_types is required.\n");
        return AVERROR(EINVAL);
    }

    if (ctx->pass_types) {
        ctx->mode = PASS;
        err = filter_units_make_type_list(ctx->pass_types,
                                          &ctx->type_list, &ctx->nb_types);
        if (err < 0) {
            av_log(bsf, AV_LOG_ERROR, "Failed to parse pass_types.\n");
            return err;
        }
    } else if (ctx->remove_types) {
        ctx->mode = REMOVE;
        err = filter_units_make_type_list(ctx->remove_types,
                                          &ctx->type_list, &ctx->nb_types);
        if (err < 0) {
            av_log(bsf, AV_LOG_ERROR, "Failed to parse remove_types.\n");
            return err;
        }
    } else if (ctx->discard == AVDISCARD_NONE) {
        return 0;
    }

    err = ff_cbs_init(&ctx->cbc, bsf->par_in->codec_id, bsf);
    if (err < 0)
        return err;

    if (ctx->discard == AVDISCARD_NONE) {
        // Don't actually decompose anything, we only want the unit data.
        ctx->cbc->decompose_unit_types    = ctx->type_list;
        ctx->cbc->nb_decompose_unit_types = 0;
    }

    if (bsf->par_in->extradata) {
        CodedBitstreamFragment *frag = &ctx->fragment;

        err = ff_cbs_read_extradata(ctx->cbc, frag, bsf->par_in);
        if (err < 0) {
            av_log(bsf, AV_LOG_ERROR, "Failed to read extradata.\n");
        } else {
            err = ff_cbs_write_extradata(ctx->cbc, bsf->par_out, frag);
            if (err < 0)
                av_log(bsf, AV_LOG_ERROR, "Failed to write extradata.\n");
        }

        ff_cbs_fragment_reset(frag);
    }

    return err;
}

static void filter_units_close(AVBSFContext *bsf)
{
    FilterUnitsContext *ctx = bsf->priv_data;

    av_freep(&ctx->type_list);

    ff_cbs_fragment_free(&ctx->fragment);
    ff_cbs_close(&ctx->cbc);
}

#define OFFSET(x) offsetof(FilterUnitsContext, x)
#define FLAGS (AV_OPT_FLAG_VIDEO_PARAM|AV_OPT_FLAG_BSF_PARAM)
static const AVOption filter_units_options[] = {
    { "pass_types",   "List of unit types to pass through the filter.",
        OFFSET(pass_types),   AV_OPT_TYPE_STRING,
        { .str = NULL }, .flags = FLAGS },
    { "remove_types", "List of unit types to remove in the filter.",
        OFFSET(remove_types), AV_OPT_TYPE_STRING,
        { .str = NULL }, .flags = FLAGS },

    { "discard", "Remove the selected frames",
        OFFSET(discard), AV_OPT_TYPE_INT,
        { .i64 = AVDISCARD_NONE }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "none" , "discard none",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_NONE }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "default" , "discard none, but can be changed after dynamically",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_DEFAULT }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "nonref", "discard all non-reference frames",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_NONREF }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "bidir", "discard all bidirectional frames",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_BIDIR }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "nonintra", "discard all frames except I frames",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_NONINTRA }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "nonkey", "discard all frames except keyframes",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_NONKEY }, INT_MIN, INT_MAX, FLAGS, "discard"},
    { "all", "discard all frames",
        0, AV_OPT_TYPE_CONST,
        { .i64 = AVDISCARD_ALL }, INT_MIN, INT_MAX, FLAGS, "discard"},

    { "discard_flags", "flags to control the discard frame behavior",
        OFFSET(discard_flags), AV_OPT_TYPE_FLAGS,
        { .i64 = DISCARD_FLAG_NONE }, INT_MIN, INT_MAX, FLAGS, "discard_flags"},
    { "keep_non_vcl", "non-vcl units even if the picture has been dropped",
        0, AV_OPT_TYPE_CONST,
        { .i64 = DISCARD_FLAG_KEEP_NON_VCL }, INT_MIN, INT_MAX, FLAGS, "discard_flags"},
    { NULL }
};

static const AVClass filter_units_class = {
    .class_name = "filter_units",
    .item_name  = av_default_item_name,
    .option     = filter_units_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

const FFBitStreamFilter ff_filter_units_bsf = {
    .p.name         = "filter_units",
    .p.codec_ids    = ff_cbs_all_codec_ids,
    .p.priv_class   = &filter_units_class,
    .priv_data_size = sizeof(FilterUnitsContext),
    .init           = &filter_units_init,
    .close          = &filter_units_close,
    .filter         = &filter_units_filter,
};
