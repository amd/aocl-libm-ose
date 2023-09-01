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

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>

/* N.B. Domain: (-inf,inf),(-inf,inf); Range=(0.0,inf) */

/* Test cases to check the conformance of the fdimf() routine. */
static libm_test_special_data_f32
test_fdimf_conformance_data[] = {
    /* Test Inputs of -/+0 */
    {POS_ZERO_F32, POS_ZERO_F32, 0, NEG_ZERO_F32},
    {NEG_ZERO_F32, POS_ZERO_F32, 0, NEG_ZERO_F32},
    {POS_ZERO_F32, POS_ZERO_F32, 0, POS_ZERO_F32},
    {NEG_ZERO_F32, POS_ZERO_F32, 0, POS_ZERO_F32},
    {POS_ZERO_F32, POS_ONE_F32, 0, NEG_ONE_F32},
    {NEG_ZERO_F32, POS_ONE_F32, 0, NEG_ONE_F32},
    {POS_ZERO_F32, POS_ZERO_F32, 0, POS_ONE_F32},
    {NEG_ZERO_F32, POS_ZERO_F32, 0, POS_ONE_F32},
    {NEG_ONE_F32, POS_ZERO_F32, 0, POS_ZERO_F32},
    {NEG_ONE_F32, POS_ZERO_F32, 0, NEG_ZERO_F32},
    {POS_ONE_F32, POS_ONE_F32, 0, POS_ZERO_F32},
    {POS_ONE_F32, POS_ONE_F32, 0, NEG_ZERO_F32},

    /* Test Inputs of -/+inf */
    {POS_ONE_F32, POS_INF_F32, 0, NEG_INF_F32},
    {NEG_ONE_F32, POS_INF_F32, 0, NEG_INF_F32},
    {POS_ONE_F32, POS_ZERO_F32, 0, POS_INF_F32},
    {NEG_ONE_F32, POS_ZERO_F32, 0, POS_INF_F32},
    {POS_INF_F32, POS_INF_F32, 0, POS_ONE_F32},
    {NEG_INF_F32, POS_ZERO_F32, 0, NEG_ONE_F32},
    {POS_INF_F32, POS_INF_F32, 0, NEG_INF_F32},
    {NEG_INF_F32, POS_ZERO_F32, 0, NEG_INF_F32},
    {POS_INF_F32, POS_ZERO_F32, 0, POS_INF_F32},
    {NEG_INF_F32, POS_ZERO_F32, 0, POS_INF_F32},

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, POS_SNAN_F32},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID, POS_SNAN_F32},
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, NEG_SNAN_F32},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID, NEG_SNAN_F32},
    {POS_ONE_F32, POS_SNAN_F32, FE_INVALID, POS_SNAN_F32},
    {NEG_ONE_F32, NEG_SNAN_F32, FE_INVALID, POS_SNAN_F32},
    {POS_ONE_F32, POS_SNAN_F32, FE_INVALID, NEG_SNAN_F32},
    {NEG_ONE_F32, NEG_SNAN_F32, FE_INVALID, NEG_SNAN_F32},
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, POS_ONE_F32},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID, POS_ONE_F32},
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID, NEG_ONE_F32},
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID, NEG_ONE_F32},
    {POS_QNAN_F32, POS_QNAN_F32, 0, POS_QNAN_F32},
    {NEG_QNAN_F32, NEG_QNAN_F32, 0, POS_QNAN_F32},
    {POS_QNAN_F32, POS_QNAN_F32, 0, NEG_QNAN_F32},
    {NEG_QNAN_F32, NEG_QNAN_F32, 0, NEG_QNAN_F32},
    {POS_ONE_F32, POS_QNAN_F32, 0, POS_QNAN_F32},
    {NEG_ONE_F32, NEG_QNAN_F32, 0, POS_QNAN_F32},
    {POS_ONE_F32, POS_QNAN_F32, 0, NEG_QNAN_F32},
    {NEG_ONE_F32, NEG_QNAN_F32, 0, NEG_QNAN_F32},
    {POS_QNAN_F32, POS_QNAN_F32, 0, POS_ONE_F32},
    {NEG_QNAN_F32, NEG_QNAN_F32, 0, POS_ONE_F32},
    {POS_QNAN_F32, POS_QNAN_F32, 0, NEG_ONE_F32},
    {NEG_QNAN_F32, NEG_QNAN_F32, 0, NEG_ONE_F32},

    /* Test some standard inputs */
    {POS_PI_F32, POS_PI_BY2_F32, 0, POS_PI_BY2_F32}, // fdimf(Pi,Pi/2)=Pi/2
    {POS_PI_F32, 0x4096cbe4, 0, NEG_PI_BY2_F32},     // fdimf(Pi,-Pi/2)=3Pi/2
    {POS_PI_BY2_F32, 0, 0, POS_PI_F32},              // fdimf(Pi/2,Pi)=0.0
    {POS_PI_BY2_F32, 0x4096cbe4, 0, NEG_PI_F32},     // fdimf(Pi/2,-Pi)=3Pi/2
    {NEG_PI_F32, 0, 0, POS_PI_BY2_F32},              // fdimf(-Pi,Pi/2)=0.0
    {NEG_PI_F32, 0, 0, NEG_PI_BY2_F32},              // fdimf(-Pi,-Pi/2)=0.0
    {NEG_PI_BY2_F32, 0, 0, POS_PI_F32},              // fdimf(-Pi/2,Pi)=0.0
    {NEG_PI_BY2_F32, POS_PI_BY2_F32, 0, NEG_PI_F32}, // fdimf(-Pi/2,-Pi)=Pi/2

    /* Test some denormalized and extreme inputs */
    {0x96421590, 0x00000000, 0, 0x7EEBE575},
    {0x7EEBE575, 0x7EEBE575, FE_INEXACT, 0x16421590},
    {0x164243a1, 0x11384400, 0, 0x16421590},
    {0x71EBD596, 0x71ebd4ab, FE_INEXACT, 0x69EAD497},
};

/* Test cases to check the conformance of the fdim() routine. */
static libm_test_special_data_f64
test_fdim_conformance_data[] = {
    /* Test Inputs of -/+0 */
    {POS_ZERO_F64, POS_ZERO_F64, 0, NEG_ZERO_F64},
    {NEG_ZERO_F64, POS_ZERO_F64, 0, NEG_ZERO_F64},
    {POS_ZERO_F64, POS_ZERO_F64, 0, POS_ZERO_F64},
    {NEG_ZERO_F64, POS_ZERO_F64, 0, POS_ZERO_F64},
    {POS_ZERO_F64, POS_ONE_F64, 0, NEG_ONE_F64},
    {NEG_ZERO_F64, POS_ONE_F64, 0, NEG_ONE_F64},
    {POS_ZERO_F64, POS_ZERO_F64, 0, POS_ONE_F64},
    {NEG_ZERO_F64, POS_ZERO_F64, 0, POS_ONE_F64},
    {NEG_ONE_F64, POS_ZERO_F64, 0, POS_ZERO_F64},
    {NEG_ONE_F64, POS_ZERO_F64, 0, NEG_ZERO_F64},
    {POS_ONE_F64, POS_ONE_F64, 0, POS_ZERO_F64},
    {POS_ONE_F64, POS_ONE_F64, 0, NEG_ZERO_F64},

    /* Test Inputs of -/+inf */
    {POS_ONE_F64, POS_INF_F64, 0, NEG_INF_F64},
    {NEG_ONE_F64, POS_INF_F64, 0, NEG_INF_F64},
    {POS_ONE_F64, POS_ZERO_F64, 0, POS_INF_F64},
    {NEG_ONE_F64, POS_ZERO_F64, 0, POS_INF_F64},
    {POS_INF_F64, POS_INF_F64, 0, POS_ONE_F64},
    {NEG_INF_F64, POS_ZERO_F64, 0, NEG_ONE_F64},
    {POS_INF_F64, POS_INF_F64, 0, NEG_INF_F64},
    {NEG_INF_F64, POS_ZERO_F64, 0, NEG_INF_F64},
    {POS_INF_F64, POS_ZERO_F64, 0, POS_INF_F64},
    {NEG_INF_F64, POS_ZERO_F64, 0, POS_INF_F64},

    /* Test Inputs that are signalling and quiet NaNs */
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, POS_SNAN_F64},
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, POS_SNAN_F64},
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, NEG_SNAN_F64},
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, NEG_SNAN_F64},
    {POS_ONE_F64, POS_SNAN_F64, FE_INVALID, POS_SNAN_F64},
    {NEG_ONE_F64, NEG_SNAN_F64, FE_INVALID, POS_SNAN_F64},
    {POS_ONE_F64, POS_SNAN_F64, FE_INVALID, NEG_SNAN_F64},
    {NEG_ONE_F64, NEG_SNAN_F64, FE_INVALID, NEG_SNAN_F64},
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, POS_ONE_F64},
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, POS_ONE_F64},
    {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, NEG_ONE_F64},
    {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, NEG_ONE_F64},
    {POS_QNAN_F64, POS_QNAN_F64, 0, POS_QNAN_F64},
    {NEG_QNAN_F64, NEG_QNAN_F64, 0, POS_QNAN_F64},
    {POS_QNAN_F64, POS_QNAN_F64, 0, NEG_QNAN_F64},
    {NEG_QNAN_F64, NEG_QNAN_F64, 0, NEG_QNAN_F64},
    {POS_ONE_F64, POS_QNAN_F64, 0, POS_QNAN_F64},
    {NEG_ONE_F64, NEG_QNAN_F64, 0, POS_QNAN_F64},
    {POS_ONE_F64, POS_QNAN_F64, 0, NEG_QNAN_F64},
    {NEG_ONE_F64, NEG_QNAN_F64, 0, NEG_QNAN_F64},
    {POS_QNAN_F64, POS_QNAN_F64, 0, POS_ONE_F64},
    {NEG_QNAN_F64, NEG_QNAN_F64, 0, POS_ONE_F64},
    {POS_QNAN_F64, POS_QNAN_F64, 0, NEG_ONE_F64},
    {NEG_QNAN_F64, NEG_QNAN_F64, 0, NEG_ONE_F64},

    /* Test some standard inputs */
    {POS_PI_F64, POS_PI_BY2_F64, 0, POS_PI_BY2_F64},     // fdim(Pi,Pi/2)=Pi/2
    {POS_PI_F64, 0x4012d97c7f3321d2, 0, NEG_PI_BY2_F64}, // fdim(Pi,-Pi/2)=3Pi/2
    {POS_PI_BY2_F64, 0, 0, POS_PI_F64},                  // fdim(Pi/2,Pi)=0.0
    {POS_PI_BY2_F64, 0x4012d97c7f3321d2, 0, NEG_PI_F64}, // fdim(Pi/2,-Pi)=3Pi/2
    {NEG_PI_F64, 0, 0, POS_PI_BY2_F64},                  // fdim(-Pi,Pi/2)=0.0
    {NEG_PI_F64, 0, 0, NEG_PI_BY2_F64},                  // fdim(-Pi,-Pi/2)=0.0
    {NEG_PI_BY2_F64, 0, 0, POS_PI_F64},                  // fdim(-Pi/2,Pi)=0.0
    {NEG_PI_BY2_F64, POS_PI_BY2_F64, 0, NEG_PI_F64},     // fdim(-Pi/2,-Pi)=Pi/2

    /* Test some denormalized and extreme inputs */
    {0x9642159012341275, 0x0000000000000000, 0, 0x7EEBE57590909090},
    {0x7EEBE57590909090, 0x7EEBE57590909090, FE_INEXACT, 0x1642159012341275},
    {0x164243a123703836, 0x15d708557bdfdf00, 0, 0x1642159078787878},
    {0x71EBD59616925632, 0x71ebd57b41fb2ffe, FE_INEXACT, 0x70EAD49726345243},
};
