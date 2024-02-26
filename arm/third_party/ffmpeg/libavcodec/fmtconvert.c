/*
 * Format Conversion Utils
 * Copyright (c) 2000, 2001 Fabrice Bellard
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
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

#include "config.h"
#include "libavutil/attributes.h"
#include "fmtconvert.h"

static void int32_to_float_fmul_scalar_c(float *dst, const int32_t *src,
                                         float mul, int len)
{
    int i;
    for(i=0; i<len; i++)
        dst[i] = src[i] * mul;
}

static void int32_to_float_fmul_array8_c(FmtConvertContext *c, float *dst,
                                         const int32_t *src, const float *mul,
                                         int len)
{
    int i;
    for (i = 0; i < len; i += 8)
        c->int32_to_float_fmul_scalar(&dst[i], &src[i], *mul++, 8);
}

av_cold void ff_fmt_convert_init(FmtConvertContext *c)
{
    c->int32_to_float_fmul_scalar = int32_to_float_fmul_scalar_c;
    c->int32_to_float_fmul_array8 = int32_to_float_fmul_array8_c;

#if ARCH_AARCH64
    ff_fmt_convert_init_aarch64(c);
#elif ARCH_ARM
    ff_fmt_convert_init_arm(c);
#elif ARCH_PPC
    ff_fmt_convert_init_ppc(c);
#elif ARCH_RISCV
    ff_fmt_convert_init_riscv(c);
#elif ARCH_X86
    ff_fmt_convert_init_x86(c);
#endif
#if HAVE_MIPSFPU
    ff_fmt_convert_init_mips(c);
#endif
}
