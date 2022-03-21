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
#include "exp_tables.h"
#define fname ALM_PROTO_FMA3(exp2f)
#define fname_special alm_expf_special@PLT

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    vucomiss .L__max_exp2f_arg(%rip), %xmm0
    ja .L__y_is_inf
    jp .L__y_is_nan
    vucomiss .L__min_exp2f_arg(%rip), %xmm0
    jb .L__y_is_zero

    vcvtps2pd     %xmm0, %xmm2    #xmm2 = (double)x

    # x * (64)
    vpaddq       .L__sixtyfour(%rip), %xmm2,%xmm3  #xmm3 = x * (64)

    # n = int( x * (64)
    vcvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n
    vcvtdq2pd    %xmm4, %xmm0  #xmm0 = (double)n

    # r = x - n * 1/64
    # r *= ln(2)
    vfmadd231sd    .L__mone_by_64(%rip),%xmm0,%xmm2 #xmm2 = r
    vmovd        %xmm4, %ecx     #ecx = n
    vmulsd      .L__ln2(%rip),%xmm2,%xmm1 #xmm1 = r = r*ln(2)

    # q = r+r*r*(1/2 + (1/6 * r))
    vmovhpd       .L__real_1_by_6(%rip), %xmm1,%xmm3  #%xmm3 =1/6,r
    vmovlhps     %xmm3,%xmm1,%xmm1     #xmm1 = r,r
    vfmadd213pd     .L__real_1_by_2_zero(%rip), %xmm1, %xmm3 #xmm2 = r*r,1/2 + (1/6 * r)
    vmovdqa %xmm3, %xmm2
    vmovhlps      %xmm2, %xmm1,%xmm3 #xmm3 = r,r*r
    vfmadd213sd     %xmm1,%xmm3,%xmm2 #xmm0 = q
    vmovdqa %xmm2, %xmm0

    #j = n & 0x3f
    mov         $0x3f, %rax     #rax = 0x3f
    and         %ecx, %eax      #eax = j = n & 0x3f

    # f + (f*q)
    lea         .L__two_to_jby64_table(%rip), %r10
    vmovsd       (%r10,%rax,8), %xmm2
    vfmadd213sd    %xmm2,%xmm0,%xmm2
    vmovdqa %xmm2, %xmm3

    .p2align 4
    # m = (n - j) / 64
    vpsrad       $6,%xmm4,%xmm1
    vpsllq       $52,%xmm1,%xmm4
    vpaddq      %xmm3, %xmm4,%xmm2
    vcvtpd2ps   %xmm2, %xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_zero:
    vmovd       %xmm0,%eax
    cmp         $0xff800000,%eax
    je          .L__x_is_neg_inf
    vpxor       %xmm1,%xmm1,%xmm1    #return value in xmm1,input in xmm0 before calling
    mov         $2, %edi        #code in edi
    call        fname_special
    ret
.L__x_is_neg_inf:
    vpxor       %xmm0,%xmm0,%xmm0
    ret

.p2align 4
.L__y_is_inf:
    vmovd       %xmm0,%eax
    mov         $0x7f800000,%edx
    cmp         %edx,%eax
    je          .L__x_is_pos_inf
    vmovd       %edx, %xmm1
    mov         $3, %edi
    call        fname_special
.L__x_is_pos_inf:
    ret

.p2align 4
.L__y_is_nan:
    vaddss  %xmm0,%xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret

.else

.p2align 4
.L__y_is_zero:
    vpxor       %xmm0, %xmm0,%xmm0
    ret

.p2align 4
.L__y_is_inf:
    mov         $0x7f800000,%edx
    vmovd       %edx, %xmm0
    ret

.p2align 4
.L__y_is_nan:
    vaddss  %xmm0,%xmm0,%xmm0
    ret

.endif

.section .rodata
.align 16
.L__max_exp2f_arg:                 .long 0x43000000
.L__min_exp2f_arg:                 .long 0xc3150000
.align 16
.L__real_1_by_2_zero:           .octa 0x3fe00000000000000000000000000000
.L__sixtyfour:                  .quad 0x0060000000000000 # 64
.L__mone_by_64:                 .quad 0xbF90000000000000 # 1/64
.L__ln2:                        .quad 0x3FE62E42FEFA39EF # ln(2)
