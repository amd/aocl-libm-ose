#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
#define fname ALM_PROTO_FMA3(tan)
#define fname_special   _tan_special@PLT
#define fname_special_underflow _sin_special_underflow@PLT
#include "trig_func.h"

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:

    VMOVQ %xmm0,%r9
    MOV   %r9,%rax
    AND   .L__sign_mask(%rip),%r9            #clear sign

tan_early_exit_s_1:

    CMP   .L_mask_3fe(%rip),%r9
    JG    tan_early_exit_s

    CMP  .L_mask_3f2(%rip),%r9
    JGE   compute_tan_pyby_4

    CMP   .L_mask_3e4(%rip),%r9
    JGE   compute_x_xxx_0_333

##  Additional code to raise underflow exception
    xor      %rax,%rax
   cmp      %rax,%r9
   je        return_tan_s
   call     fname_special_underflow
   JMP return_tan_c


compute_x_xxx_0_333:

    VMULSD    %xmm0,%xmm0,%xmm1                         #
    VMULSD    %xmm1,%xmm0,%xmm1                         # %xmm1 x3

    VFMADD231SD  .L_point_333(%rip),%xmm1,%xmm0         #  x - x*x*x*0.166666666666666666#
    JMP return_tan_c

compute_tan_pyby_4:

    tan_piby4_s_zero_fma3
    JMP    return_tan_s

tan_early_exit_s:
    MOV   .L__inf_mask_64(%rip),%r8
    AND   %r8,%rax
    CMP   %r8,%rax
    JZ    tan_naninf


range_reduce:

    VMOVAPD   %xmm0,%xmm11
    VMOVQ   %r9,%xmm0                                # %r9 x with the sign cleared

    cmp  .L_e_5(%rip),%r9
    JGE  tan_remainder_piby2

#tan_range_e_5_s:
    range_e_5_s_fma3
    JMP tan_exit_s

tan_remainder_piby2:
    call_remainder_piby2_fma3

tan_exit_s:

    VANDPD    .L_int_one(%rip),%xmm7,%xmm3
     VMOVQ     %xmm7,%r8
      AND      $0x1,%r8
    tan_piby4_s_fma3

    VANDPD  .L_signbit(%rip),%xmm11,%xmm11
    VXORPD  %xmm11,%xmm0,%xmm0


return_tan_s:

    ret
return_tan_c:

    ret


tan_naninf:
    call    fname_special

    ret
