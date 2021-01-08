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
#define fname FN_PROTOTYPE_FMA3(cos)
#define fname_special  _cos_special@PLT

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


    VMOVQ  %xmm0,%r9
    MOV   %r9,%rax
    AND   .L__sign_mask(%rip),%r9            #clear sign

cos_early_exit_s_1:

    CMP  .L_mask_3fe(%rip),%r9
    JG    cos_early_exit_s
    CMP  .L_mask_3f2(%rip),%r9
    JGE    compute_cos_pyby_4
    CMP  .L_mask_3e4(%rip),%r9
    JGE   compute_1_xx_5
    VMOVQ .L_one(%rip),%xmm0
    JMP return_cos_s

compute_1_xx_5:

    VMULSD    .L_point_five(%rip),%xmm0,%xmm1                         #
    VFNMADD213SD   .L_one(%rip),%xmm1,%xmm0         # %xmm9 1.0 - x*x*(double2)0.5#
    JMP return_cos_s

compute_cos_pyby_4:
    VPXOR  %xmm1,%xmm1,%xmm1
    cos_piby4_s_fma3
    JMP    return_cos_s

cos_early_exit_s:
    
    MOV   .L__inf_mask_64(%rip),%r8
    AND   %r8,%rax
    CMP   %r8,%rax
    JZ    cos_naninf    

range_reduce:

    VMOVAPD %xmm0, %xmm6 
    VMOVQ   %r9,%xmm0                                # r9 x with the sign cleared
    cmp  .L_e_5(%rip), %r9
    JGE  cos_remainder_piby2  

    range_e_5_s_fma3
    JMP cos_exit_s

cos_remainder_piby2:
    call_remainder_piby2_fma3

cos_exit_s:

    VMOVQ %xmm7,%rax
    and   $0x1,%rax
    cmp   $0x1,%rax
    JZ    sin_piby4_compute

cos_piby4_compute:
    cos_piby4_s_fma3
    JMP cos_exit_s_1

sin_piby4_compute:
    sin_piby4_s_fma3

cos_exit_s_1:

    VPCMPEQQ    .L_int_one(%rip),%xmm7,%xmm1
    VPCMPEQQ    .L_int_two(%rip),%xmm7,%xmm2
    VORPD    %xmm1,%xmm2,%xmm3

    VANDPD    .L_signbit(%rip),%xmm3,%xmm3
    VXORPD    %xmm3,%xmm0,%xmm0

return_cos_s:

    ret

cos_naninf:
    call    fname_special
    ret



