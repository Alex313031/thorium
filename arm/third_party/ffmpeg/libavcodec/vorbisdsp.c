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

#include "config.h"
#include "libavutil/attributes.h"
#include "vorbisdsp.h"

static void vorbis_inverse_coupling_c(float *mag, float *ang, ptrdiff_t blocksize)
{
    for (ptrdiff_t i = 0;  i < blocksize;  i++) {
        float angi = ang[i], magi = mag[i];

        if (magi > 0.f) {
            if (angi > 0.f) {
                ang[i] = magi - angi;
            } else {
                ang[i] = magi;
                mag[i] = magi + angi;
            }
        } else {
            if (angi > 0.f) {
                ang[i] = magi + angi;
            } else {
                ang[i] = magi;
                mag[i] = magi - angi;
            }
        }
    }
}

av_cold void ff_vorbisdsp_init(VorbisDSPContext *dsp)
{
    dsp->vorbis_inverse_coupling = vorbis_inverse_coupling_c;

#if ARCH_AARCH64
    ff_vorbisdsp_init_aarch64(dsp);
#elif ARCH_ARM
    ff_vorbisdsp_init_arm(dsp);
#elif ARCH_PPC
    ff_vorbisdsp_init_ppc(dsp);
#elif ARCH_RISCV
    ff_vorbisdsp_init_riscv(dsp);
#elif ARCH_X86
    ff_vorbisdsp_init_x86(dsp);
#endif
}
