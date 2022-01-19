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
#include "exp_tables.h"

#define fname ALM_PROTO_FMA3(exp2)
#define fname_special alm_exp_special@PLT
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    vucomisd     .L__max_exp2_arg(%rip), %xmm0
    jae          .L__y_is_inf
    jp           .L__y_is_nan
    vucomisd     .L__min_exp2_arg(%rip), %xmm0
    jbe          .L__y_is_zero

.if __enable_IEEE_exceptions
    vmovdqa        %xmm0,%xmm1
    vpand         .L__signbit_off(%rip),%xmm0,%xmm1
    vucomisd      .L__real_x_near0_threshold(%rip),%xmm1
    jb            .L__y_is_one
.endif

    # x * (64)
    vmulsd       .L__real_64(%rip), %xmm0, %xmm1

    # n = int( x * (64))
    vcvttpd2dq   %xmm1, %xmm2   #xmm2 = (int)n
    vcvtdq2pd    %xmm2, %xmm1   #xmm1 = (double)n
    vmovd        %xmm2, %ecx

    # r = x - n * 1/64
    #r *= ln2;
    #vfmaddsd %xmm0,.L__one_by_64(%rip),%xmm1,%xmm2
    vfmadd132sd .L__one_by_64(%rip),%xmm0,%xmm1

    vmulsd    .L__ln_2(%rip),%xmm1 ,%xmm2
    vmovlhps  %xmm2,%xmm2,%xmm2 #xmm2 = r,r

    #j = n & 0x3f
    mov         $0x3f, %rax
    and         %ecx, %eax     #eax = j
    # m = (n - j) / 64
    sar         $6, %ecx       #ecx = m

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    vmovapd    .L__real_1_by_720(%rip),%xmm3
    vfmadd213sd   .L__real_1_by_120(%rip),%xmm2,%xmm3
    vfmadd213sd   .L__real_1_by_24(%rip),%xmm2,%xmm3
    vfmadd213sd   .L__real_1_by_6(%rip),%xmm2,%xmm3
    vfmadd213sd   .L__real_1_by_2(%rip),%xmm2,%xmm3
    vmulsd     %xmm2,%xmm2,%xmm0
    vfmadd213sd   %xmm2,%xmm3,%xmm0

    # (f)*(q) + f2 + f1
    cmp         $0xfffffc02, %ecx # -1022
    lea         .L__two_to_jby64_table(%rip), %rdx
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    lea         .L__two_to_jby64_head_table(%rip), %r10
    vmulsd      (%rdx,%rax,8), %xmm0,%xmm2
    vaddsd      (%r11,%rax,8), %xmm2,%xmm1
    vaddsd      (%r10,%rax,8), %xmm1,%xmm0

    jle         .L__process_denormal
.L__process_normal:
    shl         $52, %rcx
    vmovq        %rcx,%xmm2
    vpaddq       %xmm2,%xmm0,%xmm0
    ret

.p2align 4
.L__process_denormal:
    jl          .L__process_true_denormal
    vucomisd    .L__real_one(%rip), %xmm0
    jae         .L__process_normal
.L__process_true_denormal:
    # here ( e^r < 1 and m = -1022 ) or m <= -1023
    add         $1074, %ecx
    mov         $1, %rax
    shl         %cl, %rax
    vmovq       %rax, %xmm2
    vmulsd      %xmm2,%xmm0,%xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_inf:
    vmovq        %xmm0,%rax
    mov         $0x7ff0000000000000,%rdx
    cmp         %rdx,%rax
    je          .L__x_is_pos_inf
    vmovq        %rdx, %xmm1
    mov         $3, %edi
    call        fname_special
.L__x_is_pos_inf:
    ret

.p2align 4
.L__y_is_nan:
    vaddsd       %xmm0,%xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret

.p2align 4
.L__y_is_zero:
    vmovq        %xmm0,%rax
    mov         $0xfff0000000000000,%rdx
    cmp         %rdx,%rax
    je          .L__x_is_neg_inf
    vpxor       %xmm1, %xmm1, %xmm1    #return value in xmm1,input in xmm0 before calling
    mov         $2, %edi        #code in edi
    call        fname_special
    ret
.L__x_is_neg_inf:
    vpxor        %xmm0, %xmm0,%xmm0
    ret

.p2align 4
.L__y_is_one:
    vmovsd       .L__real_one(%rip),%xmm0
    ret

.else

.p2align 4
.L__y_is_inf:
    mov         $0x7ff0000000000000,%rax
    vmovq       %rax, %xmm0
    ret

.p2align 4
.L__y_is_nan:
    vaddsd      %xmm0,%xmm0,%xmm0
    ret

.p2align 4
.L__y_is_zero:
    vpxor       %xmm2,%xmm2,%xmm0
    ret

.endif

.section .rodata
.align 16
.L__max_exp2_arg:            .quad 0x4090000000000000
.L__min_exp2_arg:            .quad 0xc090c80000000000

	.if __enable_IEEE_exceptions
	.L__signbit_off:            .quad 0x7fffffffffffffff
	.L__real_x_near0_threshold: .quad 0x3c00000000000000
	.endif

.L__real_64:                 .quad 0x4050000000000000    # 64
.L__ln_2:                    .quad 0x3FE62E42FEFA39EF
.L__one_by_64:               .quad 0xbF90000000000000
.L__real_one:                .quad 0x3ff0000000000000
