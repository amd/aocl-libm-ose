#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(cosf)
#define fname_special   _cosf_special@PLT

#include "trig_func.h"

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:


cosf_early_exit_s:

    VMOVD  %xmm0,%eax
    MOV   .L__inf_mask_32(%rip),%r8d
    AND   %r8d,%eax
    CMP   %r8d,%eax
    JZ    cosf_naninf   

    VCVTSS2SD    %xmm0,%xmm0,%xmm5
    VMOVQ    %xmm5,%r9
    AND    .L__sign_mask(%rip),%r9            #clear sign



cosf_early_exit_s_1:

    CMP   .L_mask_3fe(%rip),%r9
    JG    range_reduce

    CMP   .L_mask_3f8(%rip),%r9
    JGE    compute_cosf_pyby_4

    CMP   .L_mask_3f2(%rip),%r9
    JGE   compute_1_xx_5

    VMOVQ .L_one(%rip),%xmm0
    JMP return_cosf_s

compute_1_xx_5:

    VMULSD    .L_point_five(%rip),%xmm5,%xmm0                          #
    VFNMADD213SD    .L_one(%rip),%xmm5,%xmm0         # %xmm9 1.0 - x*x*(double2)0.5#
    JMP return_cosf_s

compute_cosf_pyby_4:
    MOVSD %xmm5,%xmm0
    cos_piby4_sf_fma3
    JMP    return_cosf_s



range_reduce:

    VMOVQ   %r9,%xmm0                                # %r9 x with the sign cleared
    cmp  .L_e_5(%rip),%r9
    JGE  cosf_remainder_piby2

#cosf_range_e_5_s:
    range_e_5_sf_fma3
    JMP cosf_exit_s

cosf_remainder_piby2:
    call_remainder_piby2_f

cosf_exit_s:


        VMOVQ %xmm4,%rax
    and   $0x1,%rax
    cmp   $0x1,%rax
 
    JNZ    cosf_piby4_compute

#sinf_piby4_compute:
    sin_piby4_sf_fma3
    JMP cosf_exit_s_1

cosf_piby4_compute:
    cos_piby4_sf_fma3

cosf_exit_s_1:
    
    VPCMPEQQ   .L_int_one(%rip),%xmm4, %xmm1
    VPCMPEQQ   .L_int_two(%rip),%xmm4,%xmm2
    VORPD      %xmm1,%xmm2,%xmm3

    VANDPD    .L_signbit(%rip),%xmm3,%xmm3
    VXORPD    %xmm3,%xmm0,%xmm0

return_cosf_s:

    VCVTSD2SS    %xmm0,%xmm0,%xmm0

    ret

cosf_naninf:
    call    fname_special

    ret
