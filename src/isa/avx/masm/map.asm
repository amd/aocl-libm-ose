;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without modification,
; are permitted provided that the following conditions are met:
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
; 3. Neither the name of the copyright holder nor the names of its contributors
;    may be used to endorse or promote products derived from this software without
;    specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
; IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
; OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
;

; map.asm
;


include fm.inc

IFDEF AVX_XOP_FMA4_FMA3
text SEGMENT EXECUTE

EXTR_ENT_POINT fma
EXTR_ENT_POINT fmaf
EXTR_ENT_POINT cbrt
EXTR_ENT_POINT cbrtf
EXTR_ENT_POINT sin
EXTR_ENT_POINT sinf
EXTR_ENT_POINT cos
EXTR_ENT_POINT cosf
EXTR_ENT_POINT tan
EXTR_ENT_POINT tanf
EXTR_ENT_POINT vrd2_sin
EXTR_ENT_POINT vrs4_sinf
EXTR_ENT_POINT vrd2_cos
EXTR_ENT_POINT vrs4_cosf
EXTR_ENT_POINT vrd2_tan
EXTR_ENT_POINT vrs4_tanf
EXTR_ENT_POINT vrd2_cbrt
EXTR_ENT_POINT vrs4_cbrtf
EXTR_ENT_POINT log
EXTR_ENT_POINT log2
EXTR_ENT_POINT log10
EXTR_ENT_POINT log1p
EXTR_ENT_POINT logf
EXTR_ENT_POINT log2f
EXTR_ENT_POINT log10f
EXTR_ENT_POINT log1pf
EXTR_ENT_POINT vrd2_log
EXTR_ENT_POINT vrd2_log2
EXTR_ENT_POINT vrd2_log10
EXTR_ENT_POINT vrd2_log1p
EXTR_ENT_POINT vrs4_logf
EXTR_ENT_POINT vrs4_log2f
EXTR_ENT_POINT vrs4_log10f
EXTR_ENT_POINT vrs4_log1pf
EXTR_ENT_POINT exp
EXTR_ENT_POINT exp2
EXTR_ENT_POINT exp10
EXTR_ENT_POINT expm1
EXTR_ENT_POINT expf
EXTR_ENT_POINT exp2f
EXTR_ENT_POINT exp10f
EXTR_ENT_POINT expm1f
EXTR_ENT_POINT pow
EXTR_ENT_POINT powf
EXTR_ENT_POINT vrd2_exp
EXTR_ENT_POINT vrd2_exp2
EXTR_ENT_POINT vrd2_exp10
EXTR_ENT_POINT vrd2_expm1
EXTR_ENT_POINT vrs4_expf
EXTR_ENT_POINT vrs4_exp2f
EXTR_ENT_POINT vrs4_exp10f
EXTR_ENT_POINT vrs4_expm1f
EXTR_ENT_POINT vrda_exp
EXTR_ENT_POINT vrda_exp2
EXTR_ENT_POINT vrda_exp10
EXTR_ENT_POINT vrda_expm1
EXTR_ENT_POINT vrsa_expf
EXTR_ENT_POINT vrsa_exp2f
EXTR_ENT_POINT vrsa_exp10f
EXTR_ENT_POINT vrsa_expm1f
EXTR_ENT_POINT vrda_cbrt
EXTR_ENT_POINT vrda_log
EXTR_ENT_POINT vrda_log2
EXTR_ENT_POINT vrda_log10
EXTR_ENT_POINT vrda_log1p
EXTR_ENT_POINT vrda_sin
EXTR_ENT_POINT vrda_cos
EXTR_ENT_POINT vrsa_cbrtf
EXTR_ENT_POINT vrsa_logf
EXTR_ENT_POINT vrsa_log2f
EXTR_ENT_POINT vrsa_log10f
EXTR_ENT_POINT vrsa_log1pf
EXTR_ENT_POINT vrsa_sinf
EXTR_ENT_POINT vrsa_cosf


TEXT    ENDS   

ENDIF

END
