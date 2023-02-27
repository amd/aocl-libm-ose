/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

/******************************************
 * Implementation Notes:
 * This is a literal translation of ASM routine for fmin + optimizations
 *
 * Prototype:
 * double fmin(double x, double y)
 *
 * Algorithm:
 * As designed in the ASM fmin double variant routine.
 * The same algorithm is extended into C, for double variant,
 * with optimizations wherever possible.
 */

#include "fn_macros.h"
#include "libm_util_amd.h"
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm/typehelper.h>


#define INF 0x7ff0000000000000

double ALM_PROTO_OPT(fmin)(double x, double y)
{
    uint64_t  ux, uy;

    ux   = asuint64 (x);
    uy   = asuint64 (y);

    if(unlikely(((ux & ~SIGNBIT_DP64) > INF) || (uy & ~SIGNBIT_DP64) > INF)) {
        /* NaN */
        return _fmin_special(x, y);
    }

    if (x<=y)
        return x;
    else
        return y;
}
