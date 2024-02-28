#
# Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

#define fname ALM_PROTO_FMA3(expm1f)
#define fname_special alm_expf_special@PLT
#ifdef __ELF__
    .section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    vucomiss .L__max_expm1f_arg(%rip),%xmm0         ##if(x > max_expm1_arg)
    ja .L__y_is_inf
    jp .L__y_is_nan
    vucomiss .L__log_OnePlus_OneByFour(%rip),%xmm0 ##if(x < log_OnePlus_OneByFour)
    jae .L__Normal_Flow
    vucomiss .L__log_OneMinus_OneByFour(%rip),%xmm0 ##if(x > log_OneMinus_OneByFour)
    ja .L__Small_Arg
    vucomiss .L__min_expm1f_arg(%rip),%xmm0         ##if(x < min_expm1_arg)
    jb .L__y_is_minusOne

    .p2align 4
.L__Normal_Flow:
    #x_double = (double)x;
    vcvtps2pd    %xmm0, %xmm2    #xmm2 = (double)x

    # x * (64/ln(2))
    vmulsd       .L__real_64_by_log2(%rip), %xmm2,%xmm3  #xmm3 = x * (64/ln(2)

    # n = int( x * (64/ln(2)) )
    vcvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n
    vcvtdq2pd    %xmm4, %xmm0  #xmm0 = (double)n

    # r = x - n * ln(2)/64
    vfmadd132sd    .L__real_mlog2_by_64(%rip),%xmm2,%xmm0 #xmm0 = r
    vmovd       %xmm4, %ecx     #ecx = n

    #j = n & 0x3f
    mov         $0x3f, %rax     #rax = 0x3f
    and         %ecx, %eax      #eax = j = n & 0x3f
    # m = (n - j) / 64
    sar         $6, %ecx        #ecx = m

    # q = r+r*r*(1/2 + (1/6 * r))
    vmovhpd       .L__real_1_by_6(%rip), %xmm0,%xmm3  #%xmm3 =1/6,r
    vmovlhps     %xmm3,%xmm0,%xmm1     #xmm1 = r,r
    vfmadd213pd     .L__real_1_by_2_zero(%rip), %xmm1, %xmm3 #xmm3 = r*r,1/2 + (1/6 * r)
    vmovdqa %xmm3, %xmm2
    vmovhlps      %xmm2, %xmm1,%xmm3 #xmm3 = r,r*r
    vfmadd213sd     %xmm1,%xmm2,%xmm3 #xmm0 = q
    vmovdqa %xmm3, %xmm0

    #f
    lea         .L__two_to_jby64_table(%rip), %r10
    vmovsd       (%r10,%rax,8), %xmm3 #xmm3 = f

    #f*q
    #vmulsd       %xmm3, %xmm0, %xmm2  #xmm2 = f*q

    #twopmm.u64 = (1023 - (long)m) << 52;
    #f - twopmm
    mov         $1023, %eax
    sub         %ecx, %eax
    shl         $52, %rax
    vmovq        %rax, %xmm1
    vsubsd      %xmm1, %xmm3,%xmm2  #xmm2 = f - twopmm

    #z = f * q + (f - twopmm.f64) ;
    #z = z* 2^m
    vfmadd213sd    %xmm2,%xmm0,%xmm3 #xmm0 = z
    vmovdqa %xmm3, %xmm1
    vpsrad       $6,%xmm4,%xmm3
    vpsllq       $52,%xmm3,%xmm4
    vpaddq      %xmm1, %xmm4,%xmm2
    vcvtpd2ps   %xmm2, %xmm0
    ret

.p2align 4
.L__Small_Arg:
    #log(1-1/4) < x < log(1+1/4)
	#q = x*x*x*(A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))));
	vucomiss .L__minus_zero(%rip), %xmm0
	je     .L__ret
	vmulss .L__A5_f(%rip),%xmm0,%xmm3
	vaddss .L__A4_f(%rip),%xmm3,%xmm1
	vfmadd213ss .L__A3_f(%rip),%xmm0,%xmm1
	vfmadd213ss .L__A2_f(%rip),%xmm0,%xmm1
	vfmadd213ss .L__A1_f(%rip),%xmm0,%xmm1 #xmm1 = (A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))))
	vmulss  %xmm0,%xmm1,%xmm1
	vmulss  %xmm0,%xmm1,%xmm1
	vmulss  %xmm0,%xmm1,%xmm1       #xmm1 = q
	vcvtps2pd %xmm0,%xmm2           #xmm2 = (double)x
	vmulsd %xmm2,%xmm2,%xmm0	#xmm0 = x*x - in double
	vfmadd132sd .L__real_1_by_2(%rip),%xmm2,%xmm0
	vmovdqa %xmm0, %xmm3
	vcvtps2pd %xmm1,%xmm0
	vaddsd %xmm3,%xmm0,%xmm2
	vcvtpd2ps %xmm2,%xmm0
.L__ret:
    ret

.p2align 4
.L__y_is_minusOne:
    mov         $0xBF800000, %edx
    vmovd       %edx, %xmm0
    #call       fname_special
    ret

.p2align 4
.L__y_is_inf:
    mov         $0x7f800000,%edx
    vmovd       %edx, %xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_nan:
    vaddss      %xmm0,%xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret

.else

.p2align 4
.L__y_is_nan:
    vaddss  %xmm0,%xmm0,%xmm0
    ret

.endif

.section .rodata
.align 16
.L__max_expm1f_arg:         .long 0x42B19999
.L__log_OnePlus_OneByFour:  .long 0x3E647FBF
.L__log_OneMinus_OneByFour: .long 0xBE934B11
.L__min_expm1f_arg:         .long 0xC18AA122
.L__real_64_by_log2:        .quad 0x40571547652b82fe # 64/ln(2)
.L__real_mlog2_by_64:       .quad 0xbf862e42fefa39ef # log2_by_64
.L__A1_f:                   .long 0x3E2AAAAA
.L__A2_f:                   .long 0x3D2AAAA0
.L__A3_f:                   .long 0x3C0889FF
.L__A4_f:                   .long 0x3AB64DE5
.L__A5_f:                   .long 0x394AB327
.L__minus_zero:             .long 0x80000000
.L__real_1_by_2_zero:           .octa 0x3fe00000000000000000000000000000

