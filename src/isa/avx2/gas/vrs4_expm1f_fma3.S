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

#define fname ALM_PROTO_FMA3(vrs4_expm1f)
#ifdef __ELF__
    .section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:

    vmovdqa       %xmm0, %xmm10   # save for later use

    #x_double = (double)x;
    vcvtps2pd     %xmm0, %ymm2    #ymm2 = (double)x3,x2,x1,x0

    # x * (64/ln(2))
    vmulpd       .L__real_64_by_log2(%rip), %ymm2, %ymm3 #ymm3 = x3 * (64/ln(2),x2 * (64/ln(2),x1 * (64/ln(2),x0 * (64/ln(2)

    # n = int( x * (64/ln(2)) )
    vcvtpd2dq    %ymm3, %xmm4  #xmm4 = (int)n3,n2,n1,n0
    vcvtdq2pd    %xmm4, %ymm0  #ymm0 = (double)n3,n2,n1,n0

    # r = x - n * ln(2)/64
    vfmadd132pd    .L__real_mlog2_by_64(%rip),%ymm2,%ymm0 #ymm0 = r3,r2,r1,r0

    # q = r+r*r*(1/2 + (1/6 * r))
    vmovdqa     .L__real_4_1_by_6s(%rip), %ymm3
    vfmadd213pd    .L__real_4_1_by_2s(%rip),%ymm0,%ymm3 #ymm3 = (1/2 + (1/6 * r))
    vmovdqa %ymm3, %ymm2
    vmulpd       %ymm0,%ymm0,%ymm3   #ymm3 = r3*r3,r2*r2,r1*r1,r0*r0
    vfmadd231pd     %ymm3,%ymm2,%ymm0 #ymm0 = q3,q2,q1,q0

    # m = (n - j) / 64
    #j = n & 0x3f
    vpsrad      $6,%xmm4,%xmm5    #xmm5 = m3,m2,m1,m0
    vpslld      $26,%xmm4,%xmm3
    vpsrld      $26,%xmm3,%xmm6   #xmm6 = j3,j2,j1,j0

    #process each input seperately
    lea         .L__two_to_jby64_table(%rip), %r10
    vmovd        %xmm6, %eax
    vmovd        %xmm5, %ecx
    vextractf128 $1,%ymm0, %xmm7
    vmovhlps     %xmm0, %xmm8,%xmm8
    mov         $4, %r11d
    jmp         .L__start

.p2align 4
.L__check_n_prepare_next:
    dec         %r11d
    jz          .L__ret
    vpsrldq      $4,%xmm6,%xmm6
    vmovd        %xmm6, %eax
    vpsrldq      $4,%xmm5,%xmm5
    vmovd        %xmm5, %ecx
    vpsrldq      $4,%xmm10,%xmm10
    cmp         $3,%r11d
    je          .L__output1_inputq2
    cmp         $2,%r11d
    je          .L__output2_inputq3
    cmp         $1,%r11d
    je          .L__output3_inputq4

.p2align 4
.L__output1_inputq2:
    vpxor      %xmm9,%xmm9,%xmm9
    vpslldq    $12, %xmm0,%xmm0
    vpsrldq    $12, %xmm0,%xmm0
    vpor       %xmm0,%xmm9,%xmm9
    movdqa    %xmm8, %xmm0
    jmp .L__start

.p2align 4
.L__output2_inputq3:
    vpslldq    $12, %xmm0,%xmm0
    vpsrldq    $8, %xmm0, %xmm0
    vpor       %xmm0,%xmm9,%xmm9
    vmovdqa    %xmm7, %xmm0
    jmp .L__start

.p2align 4
.L__output3_inputq4:
    vpslldq    $12, %xmm0,%xmm0
    vpsrldq    $4, %xmm0,%xmm0
    vpor       %xmm0,%xmm9,%xmm9
    vmovhlps   %xmm7,%xmm0, %xmm0

.p2align 4
.L__start:
    ucomiss .L__max_expm1_arg(%rip),%xmm10         ##if(x > max_expm1_arg)
    ja .L__y_is_inf
    jp .L__y_is_nan
    ucomiss .L__log_OnePlus_OneByFour(%rip),%xmm10 ##if(x < log_OnePlus_OneByFour)
    jae .L__Normal_Flow
    ucomiss .L__log_OneMinus_OneByFour(%rip),%xmm10 ##if(x > log_OneMinus_OneByFour)
    ja .L__Small_Arg
    ucomiss .L__min_expm1_arg(%rip),%xmm10         ##if(x < min_expm1_arg)
    jb .L__y_is_minusOne

.p2align 4
.L__Normal_Flow:
    #f
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
    vfmadd213sd    %xmm2,%xmm0,%xmm3 #xmm3 = z
    shl         $52, %rcx
    movd        %rcx, %xmm2
    vpaddq      %xmm3, %xmm2,%xmm3
    vcvtpd2ps   %xmm3, %xmm0
    jmp        .L__check_n_prepare_next

.p2align 4
.L__Small_Arg:
    #log(1-1/4) < x < log(1+1/4)
	#q = x*x*x*(A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))));
	movapd %xmm10,%xmm0
	vucomiss .L__minus_zero(%rip), %xmm0
	je     .L__check_n_prepare_next
	vmulss .L__A5_f(%rip),%xmm0,%xmm3
	vaddss .L__A4_f(%rip),%xmm3,%xmm1
	vfmadd213ss .L__A3_f(%rip),%xmm0,%xmm1
	vfmadd213ss .L__A2_f(%rip),%xmm0,%xmm1
	vfmadd213ss .L__A1_f(%rip),%xmm0,%xmm1 #xmm1 = (A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))))
	vmulss  %xmm0,%xmm1,%xmm1
	vmulss  %xmm0,%xmm1,%xmm3
	vmulss  %xmm0,%xmm3,%xmm1       #xmm1 = q
	vcvtps2pd %xmm0,%xmm2           #xmm2 = (double)x
	vmulsd %xmm2,%xmm2,%xmm0	#xmm0 = x*x - in double
	vfmadd231sd .L__real_1_by_2(%rip),%xmm0,%xmm2
	vcvtps2pd %xmm1,%xmm0
	vaddsd %xmm2,%xmm0,%xmm2
	vcvtpd2ps %xmm2,%xmm0
        jmp    .L__check_n_prepare_next

.p2align 4
.L__y_is_minusOne:
    mov         $0xBF800000, %eax
    vmovd        %eax, %xmm0
    jmp    .L__check_n_prepare_next

.p2align 4
.L__y_is_inf:
    mov         $0x7f800000,%eax
    vmovd        %eax, %xmm0
    jmp    .L__check_n_prepare_next

.p2align 4
.L__y_is_nan:
    vaddss  %xmm10,%xmm10,%xmm0
    jmp    .L__check_n_prepare_next

.L__ret:
    vpslldq    $12, %xmm0,%xmm0
    vpor       %xmm0,%xmm9,%xmm9
    vmovdqa    %xmm9, %xmm0
    ret

.section .rodata
.align 16
.L__max_expm1_arg:          .long 0x42B19999
.L__log_OnePlus_OneByFour:  .long 0x3E647FBF
.L__log_OneMinus_OneByFour: .long 0xBE934B11
.L__min_expm1_arg:          .long 0xC18AA122
.L__A1_f:                   .long 0x3E2AAAAA
.L__A2_f:                   .long 0x3D2AAAA0
.L__A3_f:                   .long 0x3C0889FF
.L__A4_f:                   .long 0x3AB64DE5
.L__A5_f:                   .long 0x394AB327
.L__minus_zero:             .long 0x80000000
.align 32
.L__real_64_by_log2:            .octa 0x40571547652b82fe40571547652b82fe # 64/ln(2)
                                .octa 0x40571547652b82fe40571547652b82fe # 64/ln(2)
.L__real_mlog2_by_64:           .octa 0xbf862e42fefa39efbf862e42fefa39ef # log2_by_64
                                .octa 0xbf862e42fefa39efbf862e42fefa39ef # log2_by_64
.L__real_4_1_by_2s:             .octa 0x3fe00000000000003fe0000000000000
                                .octa 0x3fe00000000000003fe0000000000000
.L__real_4_1_by_6s:             .octa 0x3fc55555555555553fc5555555555555    # 1/6
                                .octa 0x3fc55555555555553fc5555555555555




