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

//R.obtained=FN_PROTOTYPE(fmin)(P1.input, P2.input)
//double,double,double,ExcFlags
//P1 P2 R E
//unsigned long long int|double, unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = fmin(P1.V3[j],P2.V3[j])
//double double double ExcFlags
//ULP amd_ref_fmin_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static libm_test_special_data_f64
test_fmin_conformance_data[] = {
      {POS_ZERO_F64,NEG_ZERO_F64, 0, NEG_ZERO_F64},  //0
      {NEG_ZERO_F64,NEG_ZERO_F64, 0, NEG_ZERO_F64},  //0
      {POS_ZERO_F64,POS_ZERO_F64, 0, POS_ZERO_F64},  //0
      {NEG_ZERO_F64, NEG_ZERO_F64, 0, POS_ZERO_F64},  //0
      {POS_ZERO_F64,0xBFF0000000000000, 0, 0xBFF0000000000000},  //0,-1
      {NEG_ZERO_F64,0xBFF0000000000000, 0, 0xBFF0000000000000},  //0,-1
      {POS_ZERO_F64, POS_ZERO_F64, 0, 0x3FF0000000000000},  //0,1
      {NEG_ZERO_F64, NEG_ZERO_F64, 0, 0x3FF0000000000000},  //0,1
      {0xBFF0000000000000, 0xBFF0000000000000, 0, POS_ZERO_F64},
      {0xBFF0000000000000, 0xBFF0000000000000, 0, NEG_ZERO_F64},
      {0x3FF0000000000000,POS_ZERO_F64, 0, POS_ZERO_F64},
      {0x3FF0000000000000,NEG_ZERO_F64, 0, NEG_ZERO_F64},
      {0x3FF0000000000000,NEG_INF_F64, 0, NEG_INF_F64},
      {0xBFF0000000000000,NEG_INF_F64, 0, NEG_INF_F64},
      {0x3FF0000000000000, 0x3FF0000000000000, 0, POS_INF_F64},
      {0xBFF0000000000000, 0xBFF0000000000000, 0, POS_INF_F64},
      {POS_INF_F64,0x3FF0000000000000, 0, 0x3FF0000000000000},
      {NEG_INF_F64, NEG_INF_F64, 0, 0xBFF0000000000000},
      {POS_INF_F64,NEG_INF_F64, 0, NEG_INF_F64},
      {NEG_INF_F64,NEG_INF_F64, 0, NEG_INF_F64},
      {POS_INF_F64,POS_INF_F64, 0, POS_INF_F64},
      {NEG_INF_F64, NEG_INF_F64, 0, POS_INF_F64},
      {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, POS_SNAN_F64},  //
      {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, POS_SNAN_F64},  //
      {POS_SNAN_F64, POS_SNAN_F64, FE_INVALID, NEG_SNAN_F64},  //
      {NEG_SNAN_F64, NEG_SNAN_F64, FE_INVALID, NEG_SNAN_F64},  //
      {0x9642159012341275, 0x9642159012341275, 0, 0x7EEBE57590909090},
      {0x7EEBE57590909090, 0x1642159012341275, 0, 0x1642159012341275},
      {0x164243a123703836, 0x1642159078787878, 0, 0x1642159078787878},
      {0x71EBD59616925632, 0x70EAD49726345243, 0, 0x70EAD49726345243},
};