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

# vrd4_pow_fma3.S
#
# An AVX-2 implementation of the 256 bit vector pow function.
#
# Prototype:
#
#      __m256d pow( __m256d x,__m256d y);
#
#   x and y should contain four packed double precision values.
#
#
#   Algorithm:
#       x^y = e^(y*ln(x))
#
# Look in exp, log for the respective algorithms
#
#Important: This implementation will result in unexpected results for inputs that are denormal, NaN and INFINITY
#The maximum error bound is 0.5 ULP as defined by the IEEE- 754 standards


#include "fn_macros.h"
#include "weak_macros.h"
#define fname ALM_PROTO_FMA3(vrd4_pow)
#define fname_special _pow_special@PLT


# local variable storage offsets

.equ    negate_result, 0x0
.equ    stack_size, 0x30


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
# ymm0 = 4 packed double precision values x0, x1, x2, x3
# ymm1 = 4 packed double precision values y0,y1,y2,y3
# result = ymm0 = x0^y0,x1^y1,x2^y2,x3^y3
sub          $stack_size, %rsp

vpand       .L__sign_mask(%rip), %ymm0, %ymm10 #ymm10 contains the sign bits
vpcmpeqq    .L__sign_mask(%rip), %ymm10, %ymm10 #ymm10 to store masks for negative inputs
vxorpd      %ymm4, %ymm4, %ymm4
vmovdqu     %ymm4, negate_result(%rsp)
vptest      %ymm10, %ymm10   #check if the vector has any negative inputs of x
jz          .L__proceed

#-----------------------------------------------------
# Handle negative inputs of x.
#-----------------------------------------------------
vpand       %ymm10, %ymm1, %ymm4 #ymm4 will contain only the values of y for which x is negative

# determine if Y is an integer
# The code does not handle cases where y is not a integer, the result of such a case results in -NAN as x is also negative.

# Extract the exponents of y
vpand      .L__exp_mant_mask(%rip), %ymm4, %ymm4  #ymm4 = get exponent of those y values
vmovapd      %ymm4, %ymm5 #store in ymm5
vpsrlq      $52, %ymm4, %ymm4 #ymm4 = exponents of y
vpand       .L__mask_1023(%rip), %ymm10, %ymm6 #store the bias for floating point double precision = 1023
vpsubq      %ymm6, %ymm4, %ymm4 #subtract 1023 from only exponents of y where x is negative

# check if (exponent of y > 53) y is a even integer
# ymm7= mask = (exponent of y) >> (.L__1_before_mant)
vpand       .L__exp_mant_mask(%rip), %ymm0, %ymm0 #remove sign of the negative values
vpcmpgtq    .L__yexp_53(%rip), %ymm4, %ymm6   #ymm6 will have mask for even integers of y now
vpand       .L__1_before_mant(%rip), %ymm10,  %ymm7
vmovq        %xmm7, %xmm8 #xmm8 = lower 64 bit half of xmm7
vpextrd       $0, %xmm4, %eax #ax = lower double word of xmm4
vmovq        %rax, %xmm9
vpsrlq       %xmm9, %xmm8, %xmm8

vpextrq       $1, %xmm7, %r11 # rbx = upper quadword of xmm7
vmovq        %r11, %xmm12
vpextrd       $2, %xmm4, %ecx #rcx = second dword from xmm4
vmovq        %rcx, %xmm11 #xmm11 = rcx
vpsrlq       %xmm11, %xmm12, %xmm12



vextractf128 $1, %ymm7, %xmm13
vextractf128 $1, %ymm4, %xmm3

vmovq        %xmm13, %xmm15
vpextrd       $0, %xmm3, %r8d
vmovq        %r8, %xmm9
vpsrlq       %xmm9, %xmm15, %xmm15

vpextrq       $1, %xmm13, %r9
vmovq        %r9, %xmm14
vpextrd       $2, %xmm3, %r10d
vmovq        %r10, %xmm13
vpsrlq       %xmm13, %xmm14, %xmm14

vmovddup     %xmm12, %xmm12
vmovsd        %xmm8, %xmm12, %xmm7

vmovddup     %xmm14, %xmm14
vmovsd       %xmm15, %xmm14, %xmm14
vinsertf128  $1, %xmm14, %ymm7, %ymm7


# ymm5 = uy = abs(y)
# check if (mask & uy) == 0 then y is an even integer

vpand       %ymm5, %ymm7, %ymm7
vpcmpeqq     .L__pos_zero(%rip), %ymm7, %ymm7 #ymm7 will have mask for even integers now

# ymm6 and ymm7 contain the masks for values of y which are even integers.
vorpd        %ymm6, %ymm7, %ymm7 #ymm6 also contains the mask for even integers which were determined earlier.

# In the next two steps we are inverting the bits in ymm7 so the ymm7 contains the masks for odd integers of y.
vxorpd       .L__all__ones(%rip), %ymm7, %ymm7
vpand        .L__sign_mask(%rip), %ymm7, %ymm7 #Add the negative for values where y is negative
vmovdqu     %ymm7, negate_result(%rsp) #Store the sign in memory


.L__proceed:
vmovapd      %ymm1, %ymm13 #ymm13 = y
vmovapd      %ymm0, %ymm11
# -----------------------------
# compute log(x) here
# -----------------------------
.L__log_x:
# x = 2^m*(F1 + F2)
# 1 <= (F1 + F2) < 2, m is an integer, F1 = 1 + j/64, j= 0,1,2,3...64;
# |F2| < 1/128
# compute exponent part
vxorpd       %ymm12, %ymm12, %ymm12
vmovdqa      %ymm0, %ymm3
vpsrlq       $52, %ymm3, %ymm3
vmovapd      %ymm0, %ymm4 #%r8
vpsubq       .L__mask_1023(%rip), %ymm3, %ymm3
vmovapd      %ymm0, %ymm2
vpshufd      $0xf8, %xmm3,%xmm9
vextractf128 $1, %ymm3, %xmm6
vpshufd      $0x80, %xmm6, %xmm6
vmovsd       %xmm9, %xmm6, %xmm9
vcvtdq2pd    %xmm9, %ymm6
vpand        .L__real_mant(%rip), %ymm2, %ymm2

.L__continue_common:

# compute index into the log tables

vmovapd      %ymm4, %ymm9 #r8 to r9
vpand       .L__mask_mant_all8(%rip), %ymm4, %ymm4
vpand          .L__mask_mant9(%rip), %ymm9, %ymm9
vpsllq        $1, %ymm9, %ymm9
vpaddq        %ymm9, %ymm4, %ymm4

vmovapd       %ymm4, %ymm8
vpsrlq        $44, %ymm4, %ymm4 # ymm4 will contain the index to the look up tables
vpor         .L__real_half(%rip), %ymm2, %ymm2
vpor         .L__real_half(%rip), %ymm8, %ymm8
lea         .L__log_F_inv_head(%rip), %r9
lea         .L__log_F_inv_tail(%rip), %rdx

# f = F - Y, r = f * inv
# .L__log_F_inv contains 1.0 / (0.5 + 8 bits of the mantissa)
vsubpd       %ymm2, %ymm8, %ymm8

# ymm4 contains indices for x0,x1,x2,x3
# ymm5 is the mask register that specifies for which indices of ymm4 must the values from the look up table be fetched
vpcmpeqw     %ymm5, %ymm5, %ymm5
vgatherqpd   %ymm5, (%rdx, %ymm4, 8), %ymm14

#ymm14 contain values from .L__log_F_inv_tail

vmulpd       %ymm8, %ymm14, %ymm7


vpcmpeqw     %ymm2, %ymm2, %ymm2
vgatherqpd   %ymm2, (%r9, %ymm4, 8), %ymm15

#ymm15 will contain values from .L__log_F_inv_head

vmulpd       %ymm8, %ymm15, %ymm8

vaddpd       %ymm7, %ymm8, %ymm2
vmulpd       %ymm2, %ymm2, %ymm0
vsubpd       %ymm2, %ymm8, %ymm5

lea         .L__log_256_lead(%rip), %r9

# poly
# computation of poly = r = 1/2*r^2 + 1/3*r^3 + 1/4*r^4 + 1/5*r^5 + 1/6*r^6
# (1) = 1/5 + x*(1/6)
# (2) = 1/2 + x*(1/3)


vmovdqa       .L__real_1_over_6(%rip), %ymm3
vmovdqa       .L__real_1_over_3(%rip), %ymm8


vfmadd213pd     .L__real_1_over_5(%rip), %ymm2, %ymm3 # (1)
vfmadd213pd    .L__real_1_over_2(%rip), %ymm2, %ymm8 # (2)

vmovapd      %ymm4, %ymm1
vmulpd       %ymm0, %ymm0, %ymm4 # ymm4 = x^4
vmulpd       %ymm0, %ymm8, %ymm8 # x^2 + x^3*(1/3)

vfmadd213pd  .L__real_1_over_4(%rip), %ymm2, %ymm3 # 1/4 + x*(1)
vfmadd231pd  %ymm4, %ymm3 ,%ymm8 # ymm8 = 1/2*r^2 + 1/3*r^3 + 1/4*r^4 + 1/5*r^5 + 1/6*r^6

vaddpd       %ymm5, %ymm7, %ymm7
vaddpd       %ymm7, %ymm8, %ymm8


lea         .L__log_256_tail(%rip), %rdx

vmovdqu       .L__real_log2_tail(%rip), %ymm5
vfmsub213pd   %ymm8, %ymm6, %ymm5

vpcmpeqw     %ymm7, %ymm7, %ymm7
vgatherqpd   %ymm7, (%rdx, %ymm1, 8), %ymm14 #ymm14 contains values from log_256_tail table

vaddpd       %ymm14 ,%ymm5, %ymm4


vsubpd       %ymm2, %ymm4, %ymm3

vmovdqu      .L__real_log2_lead(%rip), %ymm7

vpcmpeqw     %ymm0, %ymm0, %ymm0
vgatherqpd   %ymm0, (%r9, %ymm1, 8), %ymm15 #ymm15 contains values from log_256_head table

vmovapd      %ymm15, %ymm0
vfmadd231pd  %ymm7, %ymm6, %ymm0


# result of ln(x) is computed from head and tail parts, resH and resT
# res = ln(x) = resH + resT
# resH and resT are in full precision

# resT is computed from head and tail parts, resT_h and resT_t
# resT = resT_h + resT_t

# now
# ymm3 - resT
# ymm0 - resH
# ymm4 - (resT_t)
# ymm2 - (-resT_h)
.L__log_x_continue:

vmovapd      %ymm0, %ymm7
vaddpd       %ymm3, %ymm0, %ymm0
vmovapd      %ymm0, %ymm5
vandpd       .L__real_fffffffff8000000(%rip), %ymm0, %ymm0

# ymm0 - H
# ymm7 - resH
# ymm5 - res

# yhead = y with the last 23 mantissa bits of y masked
# ytail = y - yhead

vmovapd       %ymm13, %ymm14
vandpd       .L__real_fffffffff8000000(%rip), %ymm13, %ymm13 #yhead
vaddpd       %ymm3, %ymm2, %ymm2
vsubpd       %ymm5, %ymm7, %ymm7
vsubpd       %ymm2, %ymm4, %ymm1
vaddpd       %ymm3, %ymm7, %ymm7
vsubpd       %ymm0, %ymm5, %ymm5

vmovapd      %ymm14, %ymm15
vaddpd       %ymm1, %ymm7, %ymm7
vaddpd       %ymm5, %ymm7, %ymm7

# res = H + T
# H has leading 26 bits of precision
# T has full precision

# ymm0 - H
# ymm7 - T


vsubpd       %ymm13, %ymm14, %ymm15 #ytail

# y is split into head and tail
# for y * ln(x) computation

# ymm4 - Yt
# ymm2 - Yh
# ymm0 - H
# ymm7 - T

#v = Yh * H + (Yh*T + (H*Yt + Yt*T))
#vt = v - (H * Yh) + (Yh*T + (Yt*T + H*Yt))
vmovapd   %ymm15, %ymm3



vmulpd   %ymm0, %ymm15, %ymm4 # YtRh

vmulpd   %ymm13, %ymm0, %ymm6 # YhRh



vfmadd213pd %ymm4, %ymm7, %ymm3

vfmadd231pd  %ymm13 , %ymm7, %ymm3

vaddpd   %ymm3, %ymm6, %ymm1
vmovapd   %ymm1, %ymm0


vsubpd   %ymm1, %ymm6, %ymm6
vaddpd   %ymm3, %ymm6, %ymm6


# y * ln(x) = v + vt
# v and vt are in full precision

# ymm0 - v
# ymm6 - vt

# -----------------------------
# compute exp( y * ln(x) ) here
# -----------------------------

# v * (64/ln(2))
# e^x = 2^(x/ln(2))
.L__compute_exp:

vmovdqa    %ymm0, %ymm13
vmovdqu     .L__real_64_by_log2(%rip), %ymm9
vmulpd      %ymm9, %ymm0, %ymm7

# x*(54/ln(2) = n + f
# n = int( v * (64/ln(2)) )
vcvtpd2dq    %ymm7, %xmm4

lea         .L__two_to_jby64_head_table(%rip), %r10
lea         .L__two_to_jby64_tail_table(%rip), %r11

vcvtdq2pd    %xmm4, %ymm1# r1 = x - n * ln(2)/64 head
# n = 64*m + j
vmovdqu       .L__64_in_dword(%rip), %xmm9

vandpd         %xmm9, %xmm4, %xmm5
vmovdqu      .L__real_log2_by_64_head(%rip), %ymm8
vfnmadd231pd  %ymm8, %ymm1, %ymm0
vmovdqa       %ymm0, %ymm2


# r2 = - n * ln(2)/64 tail

# m = (n - j) / 64

vpsubd      %xmm5, %xmm4, %xmm15
vpsrld      $6, %xmm15, %xmm15 #xmm15=m
vmovdqa      %xmm5, %xmm4 #xmm4=j
# r1+r2
# e^x = 2^m * 2^(j/64) * e * (ln(2)/64)
vmovdqu       .L__real_log2_by_64_tail(%rip), %ymm8
vfmadd231pd  %ymm8, %ymm1, %ymm2
vaddpd       %ymm6, %ymm2, %ymm2 # add vt here

# q = x + (1/2)*x^2 + (1/3!)*x^3 + (1/4!)*x^4 + (1/5!)*x^5 + (1/6!)*x^6
# computed using Estrins' approach
# ymm2 = x
# A0 =x
# A1 = 1/2 + 1/3!*x
# A2 = 1/4! + 1/120*x
# A3 = 1/6!
# B0 = A0 + x^2*A1
# B1 = A2 + x^2*A3
#
# q = B0 + x^4*B1
vmovdqu       .L__real_1_by_6(%rip), %ymm0
vmulpd       %ymm2, %ymm2, %ymm1 #ymm1 = x^2

vmovdqa       %ymm2, %ymm6 # A0
vfmadd213pd  .L__real_1_by_2(%rip), %ymm2, %ymm0 # A1

vmovdqu      .L__real_1_by_120(%rip), %ymm8
vfmadd213pd  .L__real_1_by_24(%rip), %ymm2, %ymm8 # A2

vmovdqu       .L__real_1_by_720(%rip), %ymm9 # A3

vfmadd213pd   %ymm6, %ymm1, %ymm0 # B0


vmulpd        %ymm1, %ymm1, %ymm5 # x^4

vfmadd213pd   %ymm8, %ymm1, %ymm9 # B1

vfmadd213pd   %ymm0, %ymm5, %ymm9

vmovdqa       %ymm9, %ymm0

# 2^m is computed by adding 1023(bias) to the exponent and right shifting it by 52

vmovdqu      .L__1023_in_dword(%rip), %xmm11
vpaddd       %xmm11, %xmm15, %xmm15
vpshufd      $0x50, %xmm15, %xmm6
vpshufd      $0xfa, %xmm15, %xmm7
vandpd     .L__real_mask_upper32(%rip), %xmm6, %xmm6
vandpd     .L__real_mask_upper32(%rip), %xmm7, %xmm7
vinsertf128  $0, %xmm6, %ymm15, %ymm15
vinsertf128  $1, %xmm7, %ymm15, %ymm15
vpsllq      $52, %ymm15, %ymm15

# xmm15 = 2^m
# f1, f2
vpcmpeqw     %ymm9, %ymm9, %ymm9
vgatherdpd   %ymm9, (%r11, %xmm4, 8), %ymm5

# (f1+f2)*(1+q)

vfmadd213pd  %ymm5, %ymm0, %ymm5

vpcmpeqw     %ymm1, %ymm1, %ymm1
vgatherdpd   %ymm1, (%r10, %xmm4, 8), %ymm6
vfmadd231pd  %ymm6, %ymm0, %ymm5
vmovdqa      %ymm5, %ymm1
vaddpd       %ymm6, %ymm1, %ymm1

vmulpd      %ymm15, %ymm1, %ymm0
vorpd        negate_result(%rsp), %ymm0, %ymm0

jz          .L__final_check

.L__final_check:
add         $stack_size, %rsp
ret

.section .rodata
.align 16

# these codes and the ones in the corresponding .c file have to match
.L__flag_x_one_y_snan:          .long 1
.L__flag_x_zero_z_inf:          .long 2
.L__flag_x_nan:                 .long 3
.L__flag_y_nan:                 .long 4
.L__flag_x_nan_y_nan:           .long 5
.L__flag_x_neg_y_notint:        .long 6
.L__flag_z_zero:                .long 7
.L__flag_z_denormal:            .long 8
.L__flag_z_inf:                 .long 9



.align 16
.L__lower_exponent:         .quad 0x3F70000000000000 #0.00390625
.L__exp_mantissa6bits:      .quad 0x7FFFC00000000000
.L__mantissa6bits:          .quad 0x000FC00000000000
.L__min_exp:                .quad 0x3EB0C6F7A0B5ED8D #10^-6
.L__max_value:              .quad 0x4059000000000000  #100
.L__ay_max_bound:           .quad 0x43e0000000000000
.L__ay_min_bound:           .quad 0x3c00000000000000
.L__sign_mask:              .quad 0x8000000000000000
.quad 0x8000000000000000
.quad 0x8000000000000000
.quad 0x8000000000000000
.L__sign_and_exp_mask:      .quad 0x0fff0000000000000
.L__exp_mask:               .quad 0x7ff0000000000000
.quad 0x7ff0000000000000
.quad 0x7ff0000000000000
.quad 0x7ff0000000000000
.L__neg_inf:                .quad 0x0fff0000000000000
.L__pos_inf:                .quad 0x7ff0000000000000
.L__pos_one:                .quad 0x3ff0000000000000
.L__pos_zero:               .quad 0x0000000000000000
.quad 0x0000000000000000
.quad 0x0000000000000000
.quad 0x0000000000000000
.align 16
.L__exp_mant_mask:          .quad 0x7fffffffffffffff
.quad 0x7fffffffffffffff
.quad 0x7fffffffffffffff
.quad 0x7fffffffffffffff

.L__all__ones:		    .quad 0xffffffffffffffff
.quad 0xffffffffffffffff
.quad 0xffffffffffffffff
.quad 0xffffffffffffffff

.L__mant_mask:              .quad 0x000fffffffffffff
.L__ind_pattern:            .quad 0x0fff8000000000000

.L__neg_qnan:               .quad 0x0fff8000000000000
.L__qnan:                   .quad 0x7ff8000000000000
.L__qnan_set:               .quad 0x0008000000000000

.L__neg_one:                .quad 0x0bff0000000000000
.L__neg_zero:               .quad 0x8000000000000000

.L__exp_shift:              .quad 0x0000000000000034 # 52
.L__exp_bias:               .quad 0x00000000000003ff # 1023
.L__exp_bias_m1:            .quad 0x00000000000003fe # 1022

.L__yexp_53:                .quad 0x0000000000000035 # 53
.quad 0x0000000000000035
.quad 0x0000000000000035
.quad 0x0000000000000035

.L__mant_full:              .quad 0x000fffffffffffff
.quad 0x000fffffffffffff
.quad 0x000fffffffffffff
.quad 0x000fffffffffffff

.L__1_before_mant:          .quad 0x0010000000000000
.quad 0x0010000000000000
.quad 0x0010000000000000
.quad 0x0010000000000000

.L__mask_mant_all8:         .quad 0x000ff00000000000
.quad 0x000ff00000000000
.quad 0x000ff00000000000
.quad 0x000ff00000000000
.L__mask_mant9:            .quad 0x0000080000000000
.quad 0x0000080000000000
.quad 0x0000080000000000
.quad 0x0000080000000000

.L__pos_2:                  .quad 0x4000000000000000



.align 16
.L__real_fffffffff8000000:  .quad 0x0fffffffff8000000
.quad 0x0fffffffff8000000
.quad 0x0fffffffff8000000
.quad 0x0fffffffff8000000

.L__mask_8000000000000000:  .quad 0x8000000000000000
.quad 0x8000000000000000

.L__real_4090040000000000:  .quad 0x4090040000000000
.quad 0x4090040000000000

.L__real_C090C80000000000:  .quad 0x0C090C80000000000
.quad 0x0C090C80000000000

#---------------------
# log data
#---------------------

.align 16

.L__real_ninf:  .quad 0x0fff0000000000000   # -inf
.quad 0x0000000000000000
.L__real_inf:   .quad 0x7ff0000000000000    # +inf
.quad 0x0000000000000000
.L__real_nan:   .quad 0x7ff8000000000000    # NaN
.quad 0x0000000000000000
.L__real_mant:  .quad 0x000FFFFFFFFFFFFF    # mantissa bits
.quad 0x000FFFFFFFFFFFFF
.quad 0x000FFFFFFFFFFFFF
.quad 0x000FFFFFFFFFFFFF
.L__mask_1023:  .quad 0x00000000000003ff
.quad 0x00000000000003ff
.quad 0x00000000000003ff
.quad 0x00000000000003ff
.L__mask_001:   .quad 0x0000000000000001
.quad 0x0000000000000000


.L__real_log2_lead: .quad 0x3fe62e42e0000000 # log2_lead  6.93147122859954833984e-01
.quad 0x3fe62e42e0000000
.quad 0x3fe62e42e0000000
.quad 0x3fe62e42e0000000

.L__real_log2_tail: .quad 0x3e6efa39ef35793c # log2_tail  5.76999904754328540596e-08
.quad 0x3e6efa39ef35793c
.quad 0x3e6efa39ef35793c
.quad 0x3e6efa39ef35793c

.L__real_two:       .quad 0x4000000000000000 # 2
.quad 0x0000000000000000

.L__real_half:      .quad 0x3fe0000000000000 # 1/2
.quad 0x3fe0000000000000
.quad 0x3fe0000000000000
.quad 0x3fe0000000000000

.L__mask_100:       .quad 0x0000000000000100
.quad 0x0000000000000000

.align 16

.L__real_one:       .quad 0x3ff0000000000000 # 1
.quad 0x3ff0000000000000
.quad 0x3ff0000000000000
.quad 0x3ff0000000000000

.L__real_1_over_2:  .quad 0x3fe0000000000000
.quad 0x3fe0000000000000
.quad 0x3fe0000000000000
.quad 0x3fe0000000000000

.L__real_1_over_3:  .quad 0x3fd5555555555555
.quad 0x3fd5555555555555
.quad 0x3fd5555555555555
.quad 0x3fd5555555555555

.L__real_1_over_4:  .quad 0x3fd0000000000000
.quad 0x3fd0000000000000
.quad 0x3fd0000000000000
.quad 0x3fd0000000000000

.L__real_1_over_5:  .quad 0x3fc999999999999a
.quad 0x3fc999999999999a
.quad 0x3fc999999999999a
.quad 0x3fc999999999999a

.L__real_1_over_6:  .quad 0x3fc5555555555555
.quad 0x3fc5555555555555
.quad 0x3fc5555555555555
.quad 0x3fc5555555555555

.L__real_1_over_7:  .quad 0x3fc2492492492494
.quad 0x3fc2492492492494
.quad 0x3fc2492492492494
.quad 0x3fc2492492492494

.L__mask_1023_f:    .quad 0x0c08ff80000000000
.quad 0x0c08ff80000000000
.quad 0x0c08ff80000000000
.quad 0x0c08ff80000000000

.align 16
.L__mask_2045:      .quad 0x00000000000007fd
.quad 0x0000000000000000

.L__real_threshold: .quad 0x3fc0000000000000 # 0.125
.quad 0x3fc0000000000000

.L__real_notsign:   .quad 0x7ffFFFFFFFFFFFFF # ^sign bit
.quad 0x0000000000000000


.align 16
.L__log_256_lead:
.quad 0x0000000000000000
.quad 0x3f6ff00aa0000000
.quad 0x3f7fe02a60000000
.quad 0x3f87dc4750000000
.quad 0x3f8fc0a8b0000000
.quad 0x3f93cea440000000
.quad 0x3f97b91b00000000
.quad 0x3f9b9fc020000000
.quad 0x3f9f829b00000000
.quad 0x3fa1b0d980000000
.quad 0x3fa39e87b0000000
.quad 0x3fa58a5ba0000000
.quad 0x3fa77458f0000000
.quad 0x3fa95c8300000000
.quad 0x3fab42dd70000000
.quad 0x3fad276b80000000
.quad 0x3faf0a30c0000000
.quad 0x3fb0759830000000
.quad 0x3fb16536e0000000
.quad 0x3fb253f620000000
.quad 0x3fb341d790000000
.quad 0x3fb42edcb0000000
.quad 0x3fb51b0730000000
.quad 0x3fb60658a0000000
.quad 0x3fb6f0d280000000
.quad 0x3fb7da7660000000
.quad 0x3fb8c345d0000000
.quad 0x3fb9ab4240000000
.quad 0x3fba926d30000000
.quad 0x3fbb78c820000000
.quad 0x3fbc5e5480000000
.quad 0x3fbd4313d0000000
.quad 0x3fbe270760000000
.quad 0x3fbf0a30c0000000
.quad 0x3fbfec9130000000
.quad 0x3fc0671510000000
.quad 0x3fc0d77e70000000
.quad 0x3fc1478580000000
.quad 0x3fc1b72ad0000000
.quad 0x3fc2266f10000000
.quad 0x3fc29552f0000000
.quad 0x3fc303d710000000
.quad 0x3fc371fc20000000
.quad 0x3fc3dfc2b0000000
.quad 0x3fc44d2b60000000
.quad 0x3fc4ba36f0000000
.quad 0x3fc526e5e0000000
.quad 0x3fc59338d0000000
.quad 0x3fc5ff3070000000
.quad 0x3fc66acd40000000
.quad 0x3fc6d60fe0000000
.quad 0x3fc740f8f0000000
.quad 0x3fc7ab8900000000
.quad 0x3fc815c0a0000000
.quad 0x3fc87fa060000000
.quad 0x3fc8e928d0000000
.quad 0x3fc9525a90000000
.quad 0x3fc9bb3620000000
.quad 0x3fca23bc10000000
.quad 0x3fca8becf0000000
.quad 0x3fcaf3c940000000
.quad 0x3fcb5b5190000000
.quad 0x3fcbc28670000000
.quad 0x3fcc296850000000
.quad 0x3fcc8ff7c0000000
.quad 0x3fccf63540000000
.quad 0x3fcd5c2160000000
.quad 0x3fcdc1bca0000000
.quad 0x3fce270760000000
.quad 0x3fce8c0250000000
.quad 0x3fcef0adc0000000
.quad 0x3fcf550a50000000
.quad 0x3fcfb91860000000
.quad 0x3fd00e6c40000000
.quad 0x3fd0402590000000
.quad 0x3fd071b850000000
.quad 0x3fd0a324e0000000
.quad 0x3fd0d46b50000000
.quad 0x3fd1058bf0000000
.quad 0x3fd1368700000000
.quad 0x3fd1675ca0000000
.quad 0x3fd1980d20000000
.quad 0x3fd1c898c0000000
.quad 0x3fd1f8ff90000000
.quad 0x3fd22941f0000000
.quad 0x3fd2596010000000
.quad 0x3fd2895a10000000
.quad 0x3fd2b93030000000
.quad 0x3fd2e8e2b0000000
.quad 0x3fd31871c0000000
.quad 0x3fd347dd90000000
.quad 0x3fd3772660000000
.quad 0x3fd3a64c50000000
.quad 0x3fd3d54fa0000000
.quad 0x3fd4043080000000
.quad 0x3fd432ef20000000
.quad 0x3fd4618bc0000000
.quad 0x3fd4900680000000
.quad 0x3fd4be5f90000000
.quad 0x3fd4ec9730000000
.quad 0x3fd51aad80000000
.quad 0x3fd548a2c0000000
.quad 0x3fd5767710000000
.quad 0x3fd5a42ab0000000
.quad 0x3fd5d1bdb0000000
.quad 0x3fd5ff3070000000
.quad 0x3fd62c82f0000000
.quad 0x3fd659b570000000
.quad 0x3fd686c810000000
.quad 0x3fd6b3bb20000000
.quad 0x3fd6e08ea0000000
.quad 0x3fd70d42e0000000
.quad 0x3fd739d7f0000000
.quad 0x3fd7664e10000000
.quad 0x3fd792a550000000
.quad 0x3fd7bede00000000
.quad 0x3fd7eaf830000000
.quad 0x3fd816f410000000
.quad 0x3fd842d1d0000000
.quad 0x3fd86e9190000000
.quad 0x3fd89a3380000000
.quad 0x3fd8c5b7c0000000
.quad 0x3fd8f11e80000000
.quad 0x3fd91c67e0000000
.quad 0x3fd9479410000000
.quad 0x3fd972a340000000
.quad 0x3fd99d9580000000
.quad 0x3fd9c86b00000000
.quad 0x3fd9f323e0000000
.quad 0x3fda1dc060000000
.quad 0x3fda484090000000
.quad 0x3fda72a490000000
.quad 0x3fda9cec90000000
.quad 0x3fdac718c0000000
.quad 0x3fdaf12930000000
.quad 0x3fdb1b1e00000000
.quad 0x3fdb44f770000000
.quad 0x3fdb6eb590000000
.quad 0x3fdb985890000000
.quad 0x3fdbc1e080000000
.quad 0x3fdbeb4d90000000
.quad 0x3fdc149ff0000000
.quad 0x3fdc3dd7a0000000
.quad 0x3fdc66f4e0000000
.quad 0x3fdc8ff7c0000000
.quad 0x3fdcb8e070000000
.quad 0x3fdce1af00000000
.quad 0x3fdd0a63a0000000
.quad 0x3fdd32fe70000000
.quad 0x3fdd5b7f90000000
.quad 0x3fdd83e720000000
.quad 0x3fddac3530000000
.quad 0x3fddd46a00000000
.quad 0x3fddfc8590000000
.quad 0x3fde248810000000
.quad 0x3fde4c71a0000000
.quad 0x3fde744260000000
.quad 0x3fde9bfa60000000
.quad 0x3fdec399d0000000
.quad 0x3fdeeb20c0000000
.quad 0x3fdf128f50000000
.quad 0x3fdf39e5b0000000
.quad 0x3fdf6123f0000000
.quad 0x3fdf884a30000000
.quad 0x3fdfaf5880000000
.quad 0x3fdfd64f20000000
.quad 0x3fdffd2e00000000
.quad 0x3fe011fab0000000
.quad 0x3fe02552a0000000
.quad 0x3fe0389ee0000000
.quad 0x3fe04bdf90000000
.quad 0x3fe05f14b0000000
.quad 0x3fe0723e50000000
.quad 0x3fe0855c80000000
.quad 0x3fe0986f40000000
.quad 0x3fe0ab76b0000000
.quad 0x3fe0be72e0000000
.quad 0x3fe0d163c0000000
.quad 0x3fe0e44980000000
.quad 0x3fe0f72410000000
.quad 0x3fe109f390000000
.quad 0x3fe11cb810000000
.quad 0x3fe12f7190000000
.quad 0x3fe1422020000000
.quad 0x3fe154c3d0000000
.quad 0x3fe1675ca0000000
.quad 0x3fe179eab0000000
.quad 0x3fe18c6e00000000
.quad 0x3fe19ee6b0000000
.quad 0x3fe1b154b0000000
.quad 0x3fe1c3b810000000
.quad 0x3fe1d610f0000000
.quad 0x3fe1e85f50000000
.quad 0x3fe1faa340000000
.quad 0x3fe20cdcd0000000
.quad 0x3fe21f0bf0000000
.quad 0x3fe23130d0000000
.quad 0x3fe2434b60000000
.quad 0x3fe2555bc0000000
.quad 0x3fe2676200000000
.quad 0x3fe2795e10000000
.quad 0x3fe28b5000000000
.quad 0x3fe29d37f0000000
.quad 0x3fe2af15f0000000
.quad 0x3fe2c0e9e0000000
.quad 0x3fe2d2b400000000
.quad 0x3fe2e47430000000
.quad 0x3fe2f62a90000000
.quad 0x3fe307d730000000
.quad 0x3fe3197a00000000
.quad 0x3fe32b1330000000
.quad 0x3fe33ca2b0000000
.quad 0x3fe34e2890000000
.quad 0x3fe35fa4e0000000
.quad 0x3fe37117b0000000
.quad 0x3fe38280f0000000
.quad 0x3fe393e0d0000000
.quad 0x3fe3a53730000000
.quad 0x3fe3b68440000000
.quad 0x3fe3c7c7f0000000
.quad 0x3fe3d90260000000
.quad 0x3fe3ea3390000000
.quad 0x3fe3fb5b80000000
.quad 0x3fe40c7a40000000
.quad 0x3fe41d8fe0000000
.quad 0x3fe42e9c60000000
.quad 0x3fe43f9fe0000000
.quad 0x3fe4509a50000000
.quad 0x3fe4618bc0000000
.quad 0x3fe4727430000000
.quad 0x3fe48353d0000000
.quad 0x3fe4942a80000000
.quad 0x3fe4a4f850000000
.quad 0x3fe4b5bd60000000
.quad 0x3fe4c679a0000000
.quad 0x3fe4d72d30000000
.quad 0x3fe4e7d810000000
.quad 0x3fe4f87a30000000
.quad 0x3fe50913c0000000
.quad 0x3fe519a4c0000000
.quad 0x3fe52a2d20000000
.quad 0x3fe53aad00000000
.quad 0x3fe54b2460000000
.quad 0x3fe55b9350000000
.quad 0x3fe56bf9d0000000
.quad 0x3fe57c57f0000000
.quad 0x3fe58cadb0000000
.quad 0x3fe59cfb20000000
.quad 0x3fe5ad4040000000
.quad 0x3fe5bd7d30000000
.quad 0x3fe5cdb1d0000000
.quad 0x3fe5ddde50000000
.quad 0x3fe5ee02a0000000
.quad 0x3fe5fe1ed0000000
.quad 0x3fe60e32f0000000
.quad 0x3fe61e3ef0000000
.quad 0x3fe62e42e0000000
.quad 0x0000000000000000

.align 16
.L__log_256_tail:
.quad 0x0000000000000000
.quad 0x3db5885e0250435a
.quad 0x3de620cf11f86ed2
.quad 0x3dff0214edba4a25
.quad 0x3dbf807c79f3db4e
.quad 0x3dea352ba779a52b
.quad 0x3dff56c46aa49fd5
.quad 0x3dfebe465fef5196
.quad 0x3e0cf0660099f1f8
.quad 0x3e1247b2ff85945d
.quad 0x3e13fd7abf5202b6
.quad 0x3e1f91c9a918d51e
.quad 0x3e08cb73f118d3ca
.quad 0x3e1d91c7d6fad074
.quad 0x3de1971bec28d14c
.quad 0x3e15b616a423c78a
.quad 0x3da162a6617cc971
.quad 0x3e166391c4c06d29
.quad 0x3e2d46f5c1d0c4b8
.quad 0x3e2e14282df1f6d3
.quad 0x3e186f47424a660d
.quad 0x3e2d4c8de077753e
.quad 0x3e2e0c307ed24f1c
.quad 0x3e226ea18763bdd3
.quad 0x3e25cad69737c933
.quad 0x3e2af62599088901
.quad 0x3e18c66c83d6b2d0
.quad 0x3e1880ceb36fb30f
.quad 0x3e2495aac6ca17a4
.quad 0x3e2761db4210878c
.quad 0x3e2eb78e862bac2f
.quad 0x3e19b2cd75790dd9
.quad 0x3e2c55e5cbd3d50f
.quad 0x3db162a6617cc971
.quad 0x3dfdbeabaaa2e519
.quad 0x3e1652cb7150c647
.quad 0x3e39a11cb2cd2ee2
.quad 0x3e219d0ab1a28813
.quad 0x3e24bd9e80a41811
.quad 0x3e3214b596faa3df
.quad 0x3e303fea46980bb8
.quad 0x3e31c8ffa5fd28c7
.quad 0x3dce8f743bcd96c5
.quad 0x3dfd98c5395315c6
.quad 0x3e3996fa3ccfa7b2
.quad 0x3e1cd2af2ad13037
.quad 0x3e1d0da1bd17200e
.quad 0x3e3330410ba68b75
.quad 0x3df4f27a790e7c41
.quad 0x3e13956a86f6ff1b
.quad 0x3e2c6748723551d9
.quad 0x3e2500de9326cdfc
.quad 0x3e1086c848df1b59
.quad 0x3e04357ead6836ff
.quad 0x3e24832442408024
.quad 0x3e3d10da8154b13d
.quad 0x3e39e8ad68ec8260
.quad 0x3e3cfbf706abaf18
.quad 0x3e3fc56ac6326e23
.quad 0x3e39105e3185cf21
.quad 0x3e3d017fe5b19cc0
.quad 0x3e3d1f6b48dd13fe
.quad 0x3e20b63358a7e73a
.quad 0x3e263063028c211c
.quad 0x3e2e6a6886b09760
.quad 0x3e3c138bb891cd03
.quad 0x3e369f7722b7221a
.quad 0x3df57d8fac1a628c
.quad 0x3e3c55e5cbd3d50f
.quad 0x3e1552d2ff48fe2e
.quad 0x3e37b8b26ca431bc
.quad 0x3e292decdc1c5f6d
.quad 0x3e3abc7c551aaa8c
.quad 0x3e36b540731a354b
.quad 0x3e32d341036b89ef
.quad 0x3e4f9ab21a3a2e0f
.quad 0x3e239c871afb9fbd
.quad 0x3e3e6add2c81f640
.quad 0x3e435c95aa313f41
.quad 0x3e249d4582f6cc53
.quad 0x3e47574c1c07398f
.quad 0x3e4ba846dece9e8d
.quad 0x3e16999fafbc68e7
.quad 0x3e4c9145e51b0103
.quad 0x3e479ef2cb44850a
.quad 0x3e0beec73de11275
.quad 0x3e2ef4351af5a498
.quad 0x3e45713a493b4a50
.quad 0x3e45c23a61385992
.quad 0x3e42a88309f57299
.quad 0x3e4530faa9ac8ace
.quad 0x3e25fec2d792a758
.quad 0x3e35a517a71cbcd7
.quad 0x3e3707dc3e1cd9a3
.quad 0x3e3a1a9f8ef43049
.quad 0x3e4409d0276b3674
.quad 0x3e20e2f613e85bd9
.quad 0x3df0027433001e5f
.quad 0x3e35dde2836d3265
.quad 0x3e2300134d7aaf04
.quad 0x3e3cb7e0b42724f5
.quad 0x3e2d6e93167e6308
.quad 0x3e3d1569b1526adb
.quad 0x3e0e99fc338a1a41
.quad 0x3e4eb01394a11b1c
.quad 0x3e04f27a790e7c41
.quad 0x3e25ce3ca97b7af9
.quad 0x3e281f0f940ed857
.quad 0x3e4d36295d88857c
.quad 0x3e21aca1ec4af526
.quad 0x3e445743c7182726
.quad 0x3e23c491aead337e
.quad 0x3e3aef401a738931
.quad 0x3e21cede76092a29
.quad 0x3e4fba8f44f82bb4
.quad 0x3e446f5f7f3c3e1a
.quad 0x3e47055f86c9674b
.quad 0x3e4b41a92b6b6e1a
.quad 0x3e443d162e927628
.quad 0x3e4466174013f9b1
.quad 0x3e3b05096ad69c62
.quad 0x3e40b169150faa58
.quad 0x3e3cd98b1df85da7
.quad 0x3e468b507b0f8fa8
.quad 0x3e48422df57499ba
.quad 0x3e11351586970274
.quad 0x3e117e08acba92ee
.quad 0x3e26e04314dd0229
.quad 0x3e497f3097e56d1a
.quad 0x3e3356e655901286
.quad 0x3e0cb761457f94d6
.quad 0x3e39af67a85a9dac
.quad 0x3e453410931a909f
.quad 0x3e22c587206058f5
.quad 0x3e223bc358899c22
.quad 0x3e4d7bf8b6d223cb
.quad 0x3e47991ec5197ddb
.quad 0x3e4a79e6bb3a9219
.quad 0x3e3a4c43ed663ec5
.quad 0x3e461b5a1484f438
.quad 0x3e4b4e36f7ef0c3a
.quad 0x3e115f026acd0d1b
.quad 0x3e3f36b535cecf05
.quad 0x3e2ffb7fbf3eb5c6
.quad 0x3e3e6a6886b09760
.quad 0x3e3135eb27f5bbc3
.quad 0x3e470be7d6f6fa57
.quad 0x3e4ce43cc84ab338
.quad 0x3e4c01d7aac3bd91
.quad 0x3e45c58d07961060
.quad 0x3e3628bcf941456e
.quad 0x3e4c58b2a8461cd2
.quad 0x3e33071282fb989a
.quad 0x3e420dab6a80f09c
.quad 0x3e44f8d84c397b1e
.quad 0x3e40d0ee08599e48
.quad 0x3e1d68787e37da36
.quad 0x3e366187d591bafc
.quad 0x3e22346600bae772
.quad 0x3e390377d0d61b8e
.quad 0x3e4f5e0dd966b907
.quad 0x3e49023cb79a00e2
.quad 0x3e44e05158c28ad8
.quad 0x3e3bfa7b08b18ae4
.quad 0x3e4ef1e63db35f67
.quad 0x3e0ec2ae39493d4f
.quad 0x3e40afe930ab2fa0
.quad 0x3e225ff8a1810dd4
.quad 0x3e469743fb1a71a5
.quad 0x3e5f9cc676785571
.quad 0x3e5b524da4cbf982
.quad 0x3e5a4c8b381535b8
.quad 0x3e5839be809caf2c
.quad 0x3e50968a1cb82c13
.quad 0x3e5eae6a41723fb5
.quad 0x3e5d9c29a380a4db
.quad 0x3e4094aa0ada625e
.quad 0x3e5973ad6fc108ca
.quad 0x3e4747322fdbab97
.quad 0x3e593692fa9d4221
.quad 0x3e5c5a992dfbc7d9
.quad 0x3e4e1f33e102387a
.quad 0x3e464fbef14c048c
.quad 0x3e4490f513ca5e3b
.quad 0x3e37a6af4d4c799d
.quad 0x3e57574c1c07398f
.quad 0x3e57b133417f8c1c
.quad 0x3e5feb9e0c176514
.quad 0x3e419f25bb3172f7
.quad 0x3e45f68a7bbfb852
.quad 0x3e5ee278497929f1
.quad 0x3e5ccee006109d58
.quad 0x3e5ce081a07bd8b3
.quad 0x3e570e12981817b8
.quad 0x3e292ab6d93503d0
.quad 0x3e58cb7dd7c3b61e
.quad 0x3e4efafd0a0b78da
.quad 0x3e5e907267c4288e
.quad 0x3e5d31ef96780875
.quad 0x3e23430dfcd2ad50
.quad 0x3e344d88d75bc1f9
.quad 0x3e5bec0f055e04fc
.quad 0x3e5d85611590b9ad
.quad 0x3df320568e583229
.quad 0x3e5a891d1772f538
.quad 0x3e22edc9dabba74d
.quad 0x3e4b9009a1015086
.quad 0x3e52a12a8c5b1a19
.quad 0x3e3a7885f0fdac85
.quad 0x3e5f4ffcd43ac691
.quad 0x3e52243ae2640aad
.quad 0x3e546513299035d3
.quad 0x3e5b39c3a62dd725
.quad 0x3e5ba6dd40049f51
.quad 0x3e451d1ed7177409
.quad 0x3e5cb0f2fd7f5216
.quad 0x3e3ab150cd4e2213
.quad 0x3e5cfd7bf3193844
.quad 0x3e53fff8455f1dbd
.quad 0x3e5fee640b905fc9
.quad 0x3e54e2adf548084c
.quad 0x3e3b597adc1ecdd2
.quad 0x3e4345bd096d3a75
.quad 0x3e5101b9d2453c8b
.quad 0x3e508ce55cc8c979
.quad 0x3e5bbf017e595f71
.quad 0x3e37ce733bd393dc
.quad 0x3e233bb0a503f8a1
.quad 0x3e30e2f613e85bd9
.quad 0x3e5e67555a635b3c
.quad 0x3e2ea88df73d5e8b
.quad 0x3e3d17e03bda18a8
.quad 0x3e5b607d76044f7e
.quad 0x3e52adc4e71bc2fc
.quad 0x3e5f99dc7362d1d9
.quad 0x3e5473fa008e6a6a
.quad 0x3e2b75bb09cb0985
.quad 0x3e5ea04dd10b9aba
.quad 0x3e5802d0d6979674
.quad 0x3e174688ccd99094
.quad 0x3e496f16abb9df22
.quad 0x3e46e66df2aa374f
.quad 0x3e4e66525ea4550a
.quad 0x3e42d02f34f20cbd
.quad 0x3e46cfce65047188
.quad 0x3e39b78c842d58b8
.quad 0x3e4735e624c24bc9
.quad 0x3e47eba1f7dd1adf
.quad 0x3e586b3e59f65355
.quad 0x3e1ce38e637f1b4d
.quad 0x3e58d82ec919edc7
.quad 0x3e4c52648ddcfa37
.quad 0x3e52482ceae1ac12
.quad 0x3e55a312311aba4f
.quad 0x3e411e236329f225
.quad 0x3e5b48c8cd2f246c
.quad 0x3e6efa39ef35793c
.quad 0x0000000000000000

.align 16
.L__log_F_inv_head:
.quad 0x4000000000000000
.quad 0x3fffe00000000000
.quad 0x3fffc00000000000
.quad 0x3fffa00000000000
.quad 0x3fff800000000000
.quad 0x3fff600000000000
.quad 0x3fff400000000000
.quad 0x3fff200000000000
.quad 0x3fff000000000000
.quad 0x3ffee00000000000
.quad 0x3ffec00000000000
.quad 0x3ffea00000000000
.quad 0x3ffe900000000000
.quad 0x3ffe700000000000
.quad 0x3ffe500000000000
.quad 0x3ffe300000000000
.quad 0x3ffe100000000000
.quad 0x3ffe000000000000
.quad 0x3ffde00000000000
.quad 0x3ffdc00000000000
.quad 0x3ffda00000000000
.quad 0x3ffd900000000000
.quad 0x3ffd700000000000
.quad 0x3ffd500000000000
.quad 0x3ffd400000000000
.quad 0x3ffd200000000000
.quad 0x3ffd000000000000
.quad 0x3ffcf00000000000
.quad 0x3ffcd00000000000
.quad 0x3ffcb00000000000
.quad 0x3ffca00000000000
.quad 0x3ffc800000000000
.quad 0x3ffc700000000000
.quad 0x3ffc500000000000
.quad 0x3ffc300000000000
.quad 0x3ffc200000000000
.quad 0x3ffc000000000000
.quad 0x3ffbf00000000000
.quad 0x3ffbd00000000000
.quad 0x3ffbc00000000000
.quad 0x3ffba00000000000
.quad 0x3ffb900000000000
.quad 0x3ffb700000000000
.quad 0x3ffb600000000000
.quad 0x3ffb400000000000
.quad 0x3ffb300000000000
.quad 0x3ffb200000000000
.quad 0x3ffb000000000000
.quad 0x3ffaf00000000000
.quad 0x3ffad00000000000
.quad 0x3ffac00000000000
.quad 0x3ffaa00000000000
.quad 0x3ffa900000000000
.quad 0x3ffa800000000000
.quad 0x3ffa600000000000
.quad 0x3ffa500000000000
.quad 0x3ffa400000000000
.quad 0x3ffa200000000000
.quad 0x3ffa100000000000
.quad 0x3ffa000000000000
.quad 0x3ff9e00000000000
.quad 0x3ff9d00000000000
.quad 0x3ff9c00000000000
.quad 0x3ff9a00000000000
.quad 0x3ff9900000000000
.quad 0x3ff9800000000000
.quad 0x3ff9700000000000
.quad 0x3ff9500000000000
.quad 0x3ff9400000000000
.quad 0x3ff9300000000000
.quad 0x3ff9200000000000
.quad 0x3ff9000000000000
.quad 0x3ff8f00000000000
.quad 0x3ff8e00000000000
.quad 0x3ff8d00000000000
.quad 0x3ff8b00000000000
.quad 0x3ff8a00000000000
.quad 0x3ff8900000000000
.quad 0x3ff8800000000000
.quad 0x3ff8700000000000
.quad 0x3ff8600000000000
.quad 0x3ff8400000000000
.quad 0x3ff8300000000000
.quad 0x3ff8200000000000
.quad 0x3ff8100000000000
.quad 0x3ff8000000000000
.quad 0x3ff7f00000000000
.quad 0x3ff7e00000000000
.quad 0x3ff7d00000000000
.quad 0x3ff7b00000000000
.quad 0x3ff7a00000000000
.quad 0x3ff7900000000000
.quad 0x3ff7800000000000
.quad 0x3ff7700000000000
.quad 0x3ff7600000000000
.quad 0x3ff7500000000000
.quad 0x3ff7400000000000
.quad 0x3ff7300000000000
.quad 0x3ff7200000000000
.quad 0x3ff7100000000000
.quad 0x3ff7000000000000
.quad 0x3ff6f00000000000
.quad 0x3ff6e00000000000
.quad 0x3ff6d00000000000
.quad 0x3ff6c00000000000
.quad 0x3ff6b00000000000
.quad 0x3ff6a00000000000
.quad 0x3ff6900000000000
.quad 0x3ff6800000000000
.quad 0x3ff6700000000000
.quad 0x3ff6600000000000
.quad 0x3ff6500000000000
.quad 0x3ff6400000000000
.quad 0x3ff6300000000000
.quad 0x3ff6200000000000
.quad 0x3ff6100000000000
.quad 0x3ff6000000000000
.quad 0x3ff5f00000000000
.quad 0x3ff5e00000000000
.quad 0x3ff5d00000000000
.quad 0x3ff5c00000000000
.quad 0x3ff5b00000000000
.quad 0x3ff5a00000000000
.quad 0x3ff5900000000000
.quad 0x3ff5800000000000
.quad 0x3ff5800000000000
.quad 0x3ff5700000000000
.quad 0x3ff5600000000000
.quad 0x3ff5500000000000
.quad 0x3ff5400000000000
.quad 0x3ff5300000000000
.quad 0x3ff5200000000000
.quad 0x3ff5100000000000
.quad 0x3ff5000000000000
.quad 0x3ff5000000000000
.quad 0x3ff4f00000000000
.quad 0x3ff4e00000000000
.quad 0x3ff4d00000000000
.quad 0x3ff4c00000000000
.quad 0x3ff4b00000000000
.quad 0x3ff4a00000000000
.quad 0x3ff4a00000000000
.quad 0x3ff4900000000000
.quad 0x3ff4800000000000
.quad 0x3ff4700000000000
.quad 0x3ff4600000000000
.quad 0x3ff4600000000000
.quad 0x3ff4500000000000
.quad 0x3ff4400000000000
.quad 0x3ff4300000000000
.quad 0x3ff4200000000000
.quad 0x3ff4200000000000
.quad 0x3ff4100000000000
.quad 0x3ff4000000000000
.quad 0x3ff3f00000000000
.quad 0x3ff3e00000000000
.quad 0x3ff3e00000000000
.quad 0x3ff3d00000000000
.quad 0x3ff3c00000000000
.quad 0x3ff3b00000000000
.quad 0x3ff3b00000000000
.quad 0x3ff3a00000000000
.quad 0x3ff3900000000000
.quad 0x3ff3800000000000
.quad 0x3ff3800000000000
.quad 0x3ff3700000000000
.quad 0x3ff3600000000000
.quad 0x3ff3500000000000
.quad 0x3ff3500000000000
.quad 0x3ff3400000000000
.quad 0x3ff3300000000000
.quad 0x3ff3200000000000
.quad 0x3ff3200000000000
.quad 0x3ff3100000000000
.quad 0x3ff3000000000000
.quad 0x3ff3000000000000
.quad 0x3ff2f00000000000
.quad 0x3ff2e00000000000
.quad 0x3ff2e00000000000
.quad 0x3ff2d00000000000
.quad 0x3ff2c00000000000
.quad 0x3ff2b00000000000
.quad 0x3ff2b00000000000
.quad 0x3ff2a00000000000
.quad 0x3ff2900000000000
.quad 0x3ff2900000000000
.quad 0x3ff2800000000000
.quad 0x3ff2700000000000
.quad 0x3ff2700000000000
.quad 0x3ff2600000000000
.quad 0x3ff2500000000000
.quad 0x3ff2500000000000
.quad 0x3ff2400000000000
.quad 0x3ff2300000000000
.quad 0x3ff2300000000000
.quad 0x3ff2200000000000
.quad 0x3ff2100000000000
.quad 0x3ff2100000000000
.quad 0x3ff2000000000000
.quad 0x3ff2000000000000
.quad 0x3ff1f00000000000
.quad 0x3ff1e00000000000
.quad 0x3ff1e00000000000
.quad 0x3ff1d00000000000
.quad 0x3ff1c00000000000
.quad 0x3ff1c00000000000
.quad 0x3ff1b00000000000
.quad 0x3ff1b00000000000
.quad 0x3ff1a00000000000
.quad 0x3ff1900000000000
.quad 0x3ff1900000000000
.quad 0x3ff1800000000000
.quad 0x3ff1800000000000
.quad 0x3ff1700000000000
.quad 0x3ff1600000000000
.quad 0x3ff1600000000000
.quad 0x3ff1500000000000
.quad 0x3ff1500000000000
.quad 0x3ff1400000000000
.quad 0x3ff1300000000000
.quad 0x3ff1300000000000
.quad 0x3ff1200000000000
.quad 0x3ff1200000000000
.quad 0x3ff1100000000000
.quad 0x3ff1100000000000
.quad 0x3ff1000000000000
.quad 0x3ff0f00000000000
.quad 0x3ff0f00000000000
.quad 0x3ff0e00000000000
.quad 0x3ff0e00000000000
.quad 0x3ff0d00000000000
.quad 0x3ff0d00000000000
.quad 0x3ff0c00000000000
.quad 0x3ff0c00000000000
.quad 0x3ff0b00000000000
.quad 0x3ff0a00000000000
.quad 0x3ff0a00000000000
.quad 0x3ff0900000000000
.quad 0x3ff0900000000000
.quad 0x3ff0800000000000
.quad 0x3ff0800000000000
.quad 0x3ff0700000000000
.quad 0x3ff0700000000000
.quad 0x3ff0600000000000
.quad 0x3ff0600000000000
.quad 0x3ff0500000000000
.quad 0x3ff0500000000000
.quad 0x3ff0400000000000
.quad 0x3ff0400000000000
.quad 0x3ff0300000000000
.quad 0x3ff0300000000000
.quad 0x3ff0200000000000
.quad 0x3ff0200000000000
.quad 0x3ff0100000000000
.quad 0x3ff0100000000000
.quad 0x3ff0000000000000
.quad 0x3ff0000000000000

.align 16
.L__log_F_inv_tail:
.quad 0x0000000000000000
.quad 0x3effe01fe01fe020
.quad 0x3f1fc07f01fc07f0
.quad 0x3f31caa01fa11caa
.quad 0x3f3f81f81f81f820
.quad 0x3f48856506ddaba6
.quad 0x3f5196792909c560
.quad 0x3f57d9108c2ad433
.quad 0x3f5f07c1f07c1f08
.quad 0x3f638ff08b1c03dd
.quad 0x3f680f6603d980f6
.quad 0x3f6d00f57403d5d0
.quad 0x3f331abf0b7672a0
.quad 0x3f506a965d43919b
.quad 0x3f5ceb240795ceb2
.quad 0x3f6522f3b834e67f
.quad 0x3f6c3c3c3c3c3c3c
.quad 0x3f3e01e01e01e01e
.quad 0x3f575b8fe21a291c
.quad 0x3f6403b9403b9404
.quad 0x3f6cc0ed7303b5cc
.quad 0x3f479118f3fc4da2
.quad 0x3f5ed952e0b0ce46
.quad 0x3f695900eae56404
.quad 0x3f3d41d41d41d41d
.quad 0x3f5cb28ff16c69ae
.quad 0x3f696b1edd80e866
.quad 0x3f4372e225fe30d9
.quad 0x3f60ad12073615a2
.quad 0x3f6cdb2c0397cdb3
.quad 0x3f52cc157b864407
.quad 0x3f664cb5f7148404
.quad 0x3f3c71c71c71c71c
.quad 0x3f6129a21a930b84
.quad 0x3f6f1e0387f1e038
.quad 0x3f5ad4e4ba80709b
.quad 0x3f6c0e070381c0e0
.quad 0x3f560fba1a362bb0
.quad 0x3f6a5713280dee96
.quad 0x3f53f59620f9ece9
.quad 0x3f69f22983759f23
.quad 0x3f5478ac63fc8d5c
.quad 0x3f6ad87bb4671656
.quad 0x3f578b8efbb8148c
.quad 0x3f6d0369d0369d03
.quad 0x3f5d212b601b3748
.quad 0x3f0b2036406c80d9
.quad 0x3f629663b24547d1
.quad 0x3f4435e50d79435e
.quad 0x3f67d0ff2920bc03
.quad 0x3f55c06b15c06b16
.quad 0x3f6e3a5f0fd7f954
.quad 0x3f61dec0d4c77b03
.quad 0x3f473289870ac52e
.quad 0x3f6a034da034da03
.quad 0x3f5d041da2292856
.quad 0x3f3a41a41a41a41a
.quad 0x3f68550f8a39409d
.quad 0x3f5b4fe5e92c0686
.quad 0x3f3a01a01a01a01a
.quad 0x3f691d2a2067b23a
.quad 0x3f5e7c5dada0b4e5
.quad 0x3f468a7725080ce1
.quad 0x3f6c49d4aa21b490
.quad 0x3f63333333333333
.quad 0x3f54bc363b03fccf
.quad 0x3f2c9f01970e4f81
.quad 0x3f697617c6ef5b25
.quad 0x3f6161f9add3c0ca
.quad 0x3f5319fe6cb39806
.quad 0x3f2f693a1c451ab3
.quad 0x3f6a9e240321a9e2
.quad 0x3f63831f3831f383
.quad 0x3f5949ebc4dcfc1c
.quad 0x3f480c6980c6980c
.quad 0x3f6f9d00c5fe7403
.quad 0x3f69721ed7e75347
.quad 0x3f6381ec0313381f
.quad 0x3f5b97c2aec12653
.quad 0x3f509ef3024ae3ba
.quad 0x3f38618618618618
.quad 0x3f6e0184f00c2780
.quad 0x3f692ef5657dba52
.quad 0x3f64940305494030
.quad 0x3f60303030303030
.quad 0x3f58060180601806
.quad 0x3f5017f405fd017f
.quad 0x3f412a8ad278e8dd
.quad 0x3f17d05f417d05f4
.quad 0x3f6d67245c02f7d6
.quad 0x3f6a4411c1d986a9
.quad 0x3f6754d76c7316df
.quad 0x3f649902f149902f
.quad 0x3f621023358c1a68
.quad 0x3f5f7390d2a6c406
.quad 0x3f5b2b0805d5b2b1
.quad 0x3f5745d1745d1746
.quad 0x3f53c31507fa32c4
.quad 0x3f50a1fd1b7af017
.quad 0x3f4bc36ce3e0453a
.quad 0x3f4702e05c0b8170
.quad 0x3f4300b79300b793
.quad 0x3f3f76b4337c6cb1
.quad 0x3f3a62681c860fb0
.quad 0x3f36c16c16c16c17
.quad 0x3f3490aa31a3cfc7
.quad 0x3f33cd153729043e
.quad 0x3f3473a88d0bfd2e
.quad 0x3f36816816816817
.quad 0x3f39f36016719f36
.quad 0x3f3ec6a5122f9016
.quad 0x3f427c29da5519cf
.quad 0x3f4642c8590b2164
.quad 0x3f4ab5c45606f00b
.quad 0x3f4fd3b80b11fd3c
.quad 0x3f52cda0c6ba4eaa
.quad 0x3f56058160581606
.quad 0x3f5990d0a4b7ef87
.quad 0x3f5d6ee340579d6f
.quad 0x3f60cf87d9c54a69
.quad 0x3f6310572620ae4c
.quad 0x3f65798c8ff522a2
.quad 0x3f680ad602b580ad
.quad 0x3f6ac3e24799546f
.quad 0x3f6da46102b1da46
.quad 0x3f15805601580560
.quad 0x3f3ed3c506b39a23
.quad 0x3f4cbdd3e2970f60
.quad 0x3f55555555555555
.quad 0x3f5c979aee0bf805
.quad 0x3f621291e81fd58e
.quad 0x3f65fead500a9580
.quad 0x3f6a0fd5c5f02a3a
.quad 0x3f6e45c223898adc
.quad 0x3f35015015015015
.quad 0x3f4c7b16ea64d422
.quad 0x3f57829cbc14e5e1
.quad 0x3f60877db8589720
.quad 0x3f65710e4b5edcea
.quad 0x3f6a7dbb4d1fc1c8
.quad 0x3f6fad40a57eb503
.quad 0x3f43fd6bb00a5140
.quad 0x3f54e78ecb419ba9
.quad 0x3f600a44029100a4
.quad 0x3f65c28f5c28f5c3
.quad 0x3f6b9c68b2c0cc4a
.quad 0x3f2978feb9f34381
.quad 0x3f4ecf163bb6500a
.quad 0x3f5be1958b67ebb9
.quad 0x3f644e6157dc9a3b
.quad 0x3f6acc4baa3f0ddf
.quad 0x3f26a4cbcb2a247b
.quad 0x3f50505050505050
.quad 0x3f5e0b4439959819
.quad 0x3f66027f6027f602
.quad 0x3f6d1e854b5e0db4
.quad 0x3f4165e7254813e2
.quad 0x3f576646a9d716ef
.quad 0x3f632b48f757ce88
.quad 0x3f6ac1b24652a906
.quad 0x3f33b13b13b13b14
.quad 0x3f5490e1eb208984
.quad 0x3f62385830fec66e
.quad 0x3f6a45a6cc111b7e
.quad 0x3f33813813813814
.quad 0x3f556f472517b708
.quad 0x3f631be7bc0e8f2a
.quad 0x3f6b9cbf3e55f044
.quad 0x3f40e7d95bc609a9
.quad 0x3f59e6b3804d19e7
.quad 0x3f65c8b6af7963c2
.quad 0x3f6eb9dad43bf402
.quad 0x3f4f1a515885fb37
.quad 0x3f60eeb1d3d76c02
.quad 0x3f6a320261a32026
.quad 0x3f3c82ac40260390
.quad 0x3f5a12f684bda12f
.quad 0x3f669d43fda2962c
.quad 0x3f02e025c04b8097
.quad 0x3f542804b542804b
.quad 0x3f63f69b02593f6a
.quad 0x3f6df31cb46e21fa
.quad 0x3f5012b404ad012b
.quad 0x3f623925e7820a7f
.quad 0x3f6c8253c8253c82
.quad 0x3f4b92ddc02526e5
.quad 0x3f61602511602511
.quad 0x3f6bf471439c9adf
.quad 0x3f4a85c40939a85c
.quad 0x3f6166f9ac024d16
.quad 0x3f6c44e10125e227
.quad 0x3f4cebf48bbd90e5
.quad 0x3f62492492492492
.quad 0x3f6d6f2e2ec0b673
.quad 0x3f5159e26af37c05
.quad 0x3f64024540245402
.quad 0x3f6f6f0243f6f024
.quad 0x3f55e60121579805
.quad 0x3f668e18cf81b10f
.quad 0x3f32012012012012
.quad 0x3f5c11f7047dc11f
.quad 0x3f69e878ff70985e
.quad 0x3f4779d9fdc3a219
.quad 0x3f61eace5c957907
.quad 0x3f6e0d5b450239e1
.quad 0x3f548bf073816367
.quad 0x3f6694808dda5202
.quad 0x3f37c67f2bae2b21
.quad 0x3f5ee58469ee5847
.quad 0x3f6c0233c0233c02
.quad 0x3f514e02328a7012
.quad 0x3f6561072057b573
.quad 0x3f31811811811812
.quad 0x3f5e28646f5a1060
.quad 0x3f6c0d1284e6f1d7
.quad 0x3f523543f0c80459
.quad 0x3f663cbeea4e1a09
.quad 0x3f3b9a3fdd5c8cb8
.quad 0x3f60be1c159a76d2
.quad 0x3f6e1d1a688e4838
.quad 0x3f572044d72044d7
.quad 0x3f691713db81577b
.quad 0x3f4ac73ae9819b50
.quad 0x3f6460334e904cf6
.quad 0x3f31111111111111
.quad 0x3f5feef80441fef0
.quad 0x3f6de021fde021fe
.quad 0x3f57b7eacc9686a0
.quad 0x3f69ead7cd391fbc
.quad 0x3f50195609804390
.quad 0x3f6641511e8d2b32
.quad 0x3f4222b1acf1ce96
.quad 0x3f62e29f79b47582
.quad 0x3f24f0d1682e11cd
.quad 0x3f5f9bb096771e4d
.quad 0x3f6e5ee45dd96ae2
.quad 0x3f5a0429a0429a04
.quad 0x3f6bb74d5f06c021
.quad 0x3f54fce404254fce
.quad 0x3f695766eacbc402
.quad 0x3f50842108421084
.quad 0x3f673e5371d5c338
.quad 0x3f4930523fbe3368
.quad 0x3f656b38f225f6c4
.quad 0x3f426e978d4fdf3b
.quad 0x3f63dd40e4eb0cc6
.quad 0x3f397f7d73404146
.quad 0x3f6293982cc98af1
.quad 0x3f30410410410410
.quad 0x3f618d6f048ff7e4
.quad 0x3f2236a3ebc349de
.quad 0x3f60c9f8ee53d18c
.quad 0x3f10204081020408
.quad 0x3f60486ca2f46ea6
.quad 0x3ef0101010101010
.quad 0x3f60080402010080
.quad 0x0000000000000000

#---------------------
# exp data
#---------------------

.align 16

.L__denormal_threshold:         .long 0x0fffffc02 # -1022
.long 0
.quad 0

.L__enable_almost_inf:          .quad 0x7fe0000000000000
.quad 0

.L__real_zero:                  .quad 0x0000000000000000
.quad 0

.L__real_smallest_denormal:     .quad 0x0000000000000001
.quad 0
.L__denormal_tiny_threshold:    .quad 0x0c0874046dfefd9d0
.quad 0

.L__real_p65536:                .quad 0x40f0000000000000    # 65536
.quad 0
.L__real_m68800:                .quad 0x0c0f0cc0000000000   # -68800
.quad 0

.align 32
.L__real_64_by_log2:            .quad 0x40571547652b82fe    # 64/ln(2)
.quad 0x40571547652b82fe
.quad 0x40571547652b82fe
.quad 0x40571547652b82fe

.L_convert_mask:                  .quad 0x4338000000000000
				  .quad	0x4338000000000000
				  .quad 0x4338000000000000
				  .quad 0x4338000000000000

.L_11_bits_mask:                  .quad 0x00000000000007FF
				  .quad 0x00000000000007FF
				  .quad 0x00000000000007FF
				  .quad 0x00000000000007FF


.L__64_in_dword:                .quad 0x0000003F0000003F
.quad 0x0000003F0000003F

.L__1023_in_dword:              .quad 0x000003FF000003FF
.quad 0x000003FF000003FF

.L__real_log2_by_64_head:        .quad 0x3f862e42f0000000    # log2_by_64_head
.quad 0x3f862e42f0000000
.quad 0x3f862e42f0000000
.quad 0x3f862e42f0000000

.L__real_log2_by_64_tail:       .quad 0x0bdfdf473de6af278   # -log2_by_64_tail
.quad 0x0bdfdf473de6af278
.quad 0x0bdfdf473de6af278
.quad 0x0bdfdf473de6af278

.L__real_1_by_720:              .quad 0x3f56c16c16c16c17    # 1/720
.quad 0x3f56c16c16c16c17
.quad 0x3f56c16c16c16c17
.quad 0x3f56c16c16c16c17

.L__real_1_by_120:              .quad 0x3f81111111111111    # 1/120
.quad 0x3f81111111111111
.quad 0x3f81111111111111
.quad 0x3f81111111111111

.L__real_1_by_24:               .quad 0x3fa5555555555555    # 1/24
.quad 0x3fa5555555555555
.quad 0x3fa5555555555555
.quad 0x3fa5555555555555

.L__real_1_by_6:                .quad 0x3fc5555555555555    # 1/6
.quad 0x3fc5555555555555
.quad 0x3fc5555555555555
.quad 0x3fc5555555555555

.L__real_1_by_2:                .quad 0x3fe0000000000000    # 1/2
.quad 0x3fe0000000000000
.quad 0x3fe0000000000000
.quad 0x3fe0000000000000

.align 16
.L__real_mask_upper32:          .quad 0x00000000FFFFFFFF
.quad 0x00000000FFFFFFFF

.align 16
.L__two_to_jby64_head_table:
.quad 0x3ff0000000000000
.quad 0x3ff02c9a30000000
.quad 0x3ff059b0d0000000
.quad 0x3ff0874510000000
.quad 0x3ff0b55860000000
.quad 0x3ff0e3ec30000000
.quad 0x3ff11301d0000000
.quad 0x3ff1429aa0000000
.quad 0x3ff172b830000000
.quad 0x3ff1a35be0000000
.quad 0x3ff1d48730000000
.quad 0x3ff2063b80000000
.quad 0x3ff2387a60000000
.quad 0x3ff26b4560000000
.quad 0x3ff29e9df0000000
.quad 0x3ff2d285a0000000
.quad 0x3ff306fe00000000
.quad 0x3ff33c08b0000000
.quad 0x3ff371a730000000
.quad 0x3ff3a7db30000000
.quad 0x3ff3dea640000000
.quad 0x3ff4160a20000000
.quad 0x3ff44e0860000000
.quad 0x3ff486a2b0000000
.quad 0x3ff4bfdad0000000
.quad 0x3ff4f9b270000000
.quad 0x3ff5342b50000000
.quad 0x3ff56f4730000000
.quad 0x3ff5ab07d0000000
.quad 0x3ff5e76f10000000
.quad 0x3ff6247eb0000000
.quad 0x3ff6623880000000
.quad 0x3ff6a09e60000000
.quad 0x3ff6dfb230000000
.quad 0x3ff71f75e0000000
.quad 0x3ff75feb50000000
.quad 0x3ff7a11470000000
.quad 0x3ff7e2f330000000
.quad 0x3ff8258990000000
.quad 0x3ff868d990000000
.quad 0x3ff8ace540000000
.quad 0x3ff8f1ae90000000
.quad 0x3ff93737b0000000
.quad 0x3ff97d8290000000
.quad 0x3ff9c49180000000
.quad 0x3ffa0c6670000000
.quad 0x3ffa5503b0000000
.quad 0x3ffa9e6b50000000
.quad 0x3ffae89f90000000
.quad 0x3ffb33a2b0000000
.quad 0x3ffb7f76f0000000
.quad 0x3ffbcc1e90000000
.quad 0x3ffc199bd0000000
.quad 0x3ffc67f120000000
.quad 0x3ffcb720d0000000
.quad 0x3ffd072d40000000
.quad 0x3ffd5818d0000000
.quad 0x3ffda9e600000000
.quad 0x3ffdfc9730000000
.quad 0x3ffe502ee0000000
.quad 0x3ffea4afa0000000
.quad 0x3ffefa1be0000000
.quad 0x3fff507650000000
.quad 0x3fffa7c180000000

.align 16
.L__two_to_jby64_tail_table:
.quad 0x0000000000000000
.quad 0x3e6cef00c1dcdef9
.quad 0x3e48ac2ba1d73e2a
.quad 0x3e60eb37901186be
.quad 0x3e69f3121ec53172
.quad 0x3e469e8d10103a17
.quad 0x3df25b50a4ebbf1a
.quad 0x3e6d525bbf668203
.quad 0x3e68faa2f5b9bef9
.quad 0x3e66df96ea796d31
.quad 0x3e368b9aa7805b80
.quad 0x3e60c519ac771dd6
.quad 0x3e6ceac470cd83f5
.quad 0x3e5789f37495e99c
.quad 0x3e547f7b84b09745
.quad 0x3e5b900c2d002475
.quad 0x3e64636e2a5bd1ab
.quad 0x3e4320b7fa64e430
.quad 0x3e5ceaa72a9c5154
.quad 0x3e53967fdba86f24
.quad 0x3e682468446b6824
.quad 0x3e3f72e29f84325b
.quad 0x3e18624b40c4dbd0
.quad 0x3e5704f3404f068e
.quad 0x3e54d8a89c750e5e
.quad 0x3e5a74b29ab4cf62
.quad 0x3e5a753e077c2a0f
.quad 0x3e5ad49f699bb2c0
.quad 0x3e6a90a852b19260
.quad 0x3e56b48521ba6f93
.quad 0x3e0d2ac258f87d03
.quad 0x3e42a91124893ecf
.quad 0x3e59fcef32422cbe
.quad 0x3e68ca345de441c5
.quad 0x3e61d8bee7ba46e1
.quad 0x3e59099f22fdba6a
.quad 0x3e4f580c36bea881
.quad 0x3e5b3d398841740a
.quad 0x3e62999c25159f11
.quad 0x3e668925d901c83b
.quad 0x3e415506dadd3e2a
.quad 0x3e622aee6c57304e
.quad 0x3e29b8bc9e8a0387
.quad 0x3e6fbc9c9f173d24
.quad 0x3e451f8480e3e235
.quad 0x3e66bbcac96535b5
.quad 0x3e41f12ae45a1224
.quad 0x3e55e7f6fd0fac90
.quad 0x3e62b5a75abd0e69
.quad 0x3e609e2bf5ed7fa1
.quad 0x3e47daf237553d84
.quad 0x3e12f074891ee83d
.quad 0x3e6b0aa538444196
.quad 0x3e6cafa29694426f
.quad 0x3e69df20d22a0797
.quad 0x3e640f12f71a1e45
.quad 0x3e69f7490e4bb40b
.quad 0x3e4ed9942b84600d
.quad 0x3e4bdcdaf5cb4656
.quad 0x3e5e2cffd89cf44c
.quad 0x3e452486cc2c7b9d
.quad 0x3e6cc2b44eee3fa4
.quad 0x3e66dc8a80ce9f09
.quad 0x3e39e90d82e90a7e
