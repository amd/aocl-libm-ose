/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define ALM_OVERRIDE 1

#include <libm/arch/zen3.h>
#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

/*
C implementation of Linearfrac

Signature:
    v_f64x4_t linearfrac(v_f64x4_t a, v_f64x4_t b, double scalea, double shifta, double scaleb, double shiftb)

Implementation notes:

    Linearfrac function performs a linear fraction transformation of vector a by vector b
    with scalar parameters
    y[i] = (scalea.a[i]+shifta)/(scaleb.b[i]+shiftb)

*/

v_f64x4_t
ALM_PROTO_ARCH_ZN3(vrd4_linearfrac)(v_f64x4_t a, v_f64x4_t b, double scalea, double shifta, double scaleb, double shiftb)
{
    /* Broadcast scalar values scalea, scaleb, shifta and shiftb to vectors*/  
    v_f64x4_t scalea_v = _mm256_broadcast_sd(&scalea);
    v_f64x4_t scaleb_v = _mm256_broadcast_sd(&scaleb);
    v_f64x4_t shifta_v = _mm256_broadcast_sd(&shifta);
    v_f64x4_t shiftb_v = _mm256_broadcast_sd(&shiftb);
 
    /* transa = (a * scalea) + shifta */
    v_f64x4_t transa = _mm256_fmadd_pd(scalea_v, a, shifta_v);
    /* transb = (b * scaleb) + shiftb */
    v_f64x4_t transb = _mm256_fmadd_pd(scaleb_v, b, shiftb_v);
 
    /* result = (transa / transb) = ((a * scalea) + shifta) / ((b * scaleb) + shiftb)*/
    v_f64x4_t result = _mm256_div_pd(transa, transb);
 
    return result;
    
}

