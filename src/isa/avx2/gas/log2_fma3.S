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

#
# log2_bdozr.S
#
# An implementation of the log libm function.
#
# Prototype:
#
#     double log2(double x);
#

#
#   Algorithm:
#
#   Based on:
#   Ping-Tak Peter Tang
#   "Table-driven implementation of the logarithm function in IEEE
#   floating-point arithmetic"
#   ACM Transactions on Mathematical Software (TOMS)
#   Volume 16, Issue 4 (December 1990)
#
#
#   x very close to 1.0 is handled differently, for x everywhere else
#   a brief explanation is given below
#
#   x = (2^m)*A
#   x = (2^m)*(G+g) with (1 <= G < 2) and (g <= 2^(-9))
#   x = (2^m)*2*(G/2+g/2)
#   x = (2^m)*2*(F+f) with (0.5 <= F < 1) and (f <= 2^(-10))
#
#   Y = (2^(-1))*(2^(-m))*(2^m)*A
#   Now, range of Y is: 0.5 <= Y < 1
#
#   F = 0x100 + (first 8 mantissa bits) + (9th mantissa bit)
#   Now, range of F is: 256 <= F <= 512
#   F = F / 512
#   Now, range of F is: 0.5 <= F <= 1
#
#   f = -(Y-F), with (f <= 2^(-10))
#
#   log(x) = m*log(2) + log(2) + log(F-f)
#   log(x) = m*log(2) + log(2) + log(F) + log(1-(f/F))
#   log(x) = m*log(2) + log(2*F) + log(1-r)
#
#   r = (f/F), with (r <= 2^(-9))
#   r = f*(1/F) with (1/F) precomputed to avoid division
#
#   log(x) = m*log(2) + log(G) - poly
#
#   log(G) is precomputed
#   poly = (r + (r^2)/2 + (r^3)/3 + (r^4)/4) + (r^5)/5) + (r^6)/6))
#
#   log(2) and log(G) need to be maintained in extra precision
#   to avoid losing precision in the calculations
#

#include "fn_macros.h"
#define fname ALM_PROTO_FMA3(log2)
#define fname_special alm_log_special@PLT


# local variable storage offsets
.equ    p_temp, 0x0
.equ    stack_size, 0x18


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:


    # compute exponent part
    xor         %rax, %rax
    vpsrlq       $52, %xmm0, %xmm3
    vmovd        %xmm0, %rax
    vpsubq       .L__mask_1023(%rip), %xmm3,%xmm3
    vcvtdq2pd    %xmm3, %xmm6 # xexp replaced VCVTDQ2PD

    #  NaN or inf
    vpand       .L__real_inf(%rip), %xmm0, %xmm5
    vcomisd     .L__real_inf(%rip), %xmm5
    je          .L__x_is_inf_or_nan

    # check for negative numbers or zero
    vpxor       %xmm5, %xmm5,%xmm5
    vcomisd     %xmm5, %xmm0
    jbe         .L__x_is_zero_or_neg

    vpand       .L__real_mant(%rip), %xmm0,%xmm2
    vsubsd      .L__real_one(%rip), %xmm0, %xmm4

    vcomisd      .L__mask_1023_f(%rip), %xmm6
    je          .L__denormal_adjust

.L__continue_common:
    # compute index into the log tables
    vpand       .L__mask_mant_all8(%rip), %xmm0,%xmm14
    vpand       .L__mask_mant9(%rip), %xmm0,%xmm15
    vpsllq       $1, %xmm15,%xmm15
    vpaddq       %xmm14,%xmm15,%xmm1
    vmovd        %xmm1, %rax

    # near one codepath
    vpand        .L__real_notsign(%rip), %xmm4, %xmm4
    vcomisd      .L__real_threshold(%rip), %xmm4
    jb           .L__near_one

    # F, Y
    shr         $44, %rax
    vpor         .L__real_half(%rip), %xmm2, %xmm2
    vpor         .L__real_half(%rip), %xmm1, %xmm1
    lea         .L__log_F_inv(%rip), %r9

    # f = F - Y, r = f * inv
    vsubsd       %xmm2, %xmm1, %xmm1
    vmulsd       (%r9,%rax,8), %xmm1, %xmm1

    lea         .L__log_256_lead(%rip), %r9
    lea         .L__log_256_tail(%rip), %rdx
    vmovsd       (%rdx,%rax,8), %xmm2
    movsd        (%r9,%rax,8), %xmm0
    vaddsd       %xmm6, %xmm0,%xmm0

    # poly
    vmulsd      %xmm1,%xmm1,%xmm7 # r*r
    vmovsd      .L__real_1_over_6(%rip),%xmm3
    vmovsd      .L__real_1_over_3(%rip),%xmm5
    vfmadd213sd    .L__real_1_over_5(%rip),%xmm1,%xmm3 # r*1/6 + 1/5
    vfmadd213sd    .L__real_1_over_2(%rip),%xmm1, %xmm5 # 1/2+r*1/3
    movsd       %xmm7, %xmm4 # r*r
    vfmadd213sd    .L__real_1_over_4(%rip),%xmm1, %xmm3 #1/4+(1/5*r+r*r*1/6)

    vmulsd       %xmm7,%xmm7,%xmm4                   # r*r*r*r
    vfmadd231sd     %xmm7,%xmm5,%xmm1 # r*r*(1/2+r*1/3) + r
    vfmadd231sd     %xmm4, %xmm3,%xmm1 # poly

    # m*log(2) + log(G) - poly
    vfnmadd231sd    .L__real_log2_e(%rip), %xmm1, %xmm2
    vaddsd       %xmm2, %xmm0,%xmm0
    ret

.p2align 4,,15
.L__near_one:

    # r = x - 1.0
    vmovsd       .L__real_two(%rip), %xmm2
    vsubsd      .L__real_one(%rip), %xmm0, %xmm0 # r

    vaddsd       %xmm0, %xmm2,%xmm2
    vdivsd       %xmm2, %xmm0,%xmm1 # r/(2+r) = u/2

    vmovsd       .L__real_ca2(%rip), %xmm4
    vmovsd       .L__real_ca4(%rip), %xmm5

    vmulsd       %xmm1, %xmm0,%xmm6 # correction
    vaddsd       %xmm1, %xmm1,%xmm1 # u

    vmulsd       %xmm1, %xmm1,%xmm2 # u^2
    vfmadd213sd    .L__real_ca1(%rip),%xmm2,%xmm4
    vfmadd213sd    .L__real_ca3(%rip),%xmm2,%xmm5

    vmulsd       %xmm1, %xmm2, %xmm2 # u^3
    vmulsd       %xmm2, %xmm4, %xmm4

    vmulsd       %xmm2, %xmm2, %xmm2
    vmulsd       %xmm1, %xmm2, %xmm2 # u^7

    vmulsd       %xmm2, %xmm5,%xmm5
    vaddsd       %xmm5, %xmm4,%xmm4
    vsubsd       %xmm6, %xmm4,%xmm4


    vpand        .L__mask_lower(%rip), %xmm0,%xmm3
    vsubsd       %xmm3, %xmm0,%xmm0
    vaddsd       %xmm0, %xmm4,%xmm4

    vmulsd       .L__real_log2_e_lead(%rip), %xmm4,%xmm1
    vmulsd       .L__real_log2_e_tail(%rip), %xmm4,%xmm4
    vmulsd       .L__real_log2_e_tail(%rip), %xmm3,%xmm0
    vmulsd       .L__real_log2_e_lead(%rip), %xmm3,%xmm3

    vaddsd       %xmm4, %xmm0,%xmm0
    vaddsd       %xmm1, %xmm0,%xmm0
    vaddsd       %xmm3, %xmm0,%xmm0

    ret

.L__denormal_adjust:
    vpor         .L__real_one(%rip), %xmm2,%xmm2
    vsubsd       .L__real_one(%rip), %xmm2,%xmm2
    vpsrlq       $52, %xmm2,%xmm5
    vpand        .L__real_mant(%rip), %xmm2,%xmm2
    vmovapd      %xmm2, %xmm0
    vpsubd       .L__mask_2045(%rip), %xmm5,%xmm5
    vcvtdq2pd    %xmm5, %xmm6
    jmp         .L__continue_common

.p2align 4,,15
.L__x_is_zero_or_neg:
    jne         .L__x_is_neg
    vmovsd       .L__real_ninf(%rip), %xmm1
    mov         .L__flag_x_zero(%rip), %edi
    call        fname_special
    ret

.p2align 4,,15
.L__x_is_neg:

    vmovsd       .L__real_qnan(%rip), %xmm1
    mov         .L__flag_x_neg(%rip), %edi
    call        fname_special
    ret

.p2align 4,,15
.L__x_is_inf_or_nan:

    cmp         .L__real_inf(%rip), %rax
    je          .L__finish

    cmp         .L__real_ninf(%rip), %rax
    je          .L__x_is_neg

    mov         .L__real_qnanbit(%rip), %r9
    and         %rax, %r9
    jnz         .L__finish

    or          .L__real_qnanbit(%rip), %rax
    movd        %rax, %xmm1
    mov         .L__flag_x_nan(%rip), %edi
    call        fname_special
    jmp         .L__finish

.p2align 4,,15
.L__finish:
    ret


.section .rodata

.align 16

# these codes and the ones in the corresponding .c file have to match
.L__flag_x_zero:        .long 00000001
.L__flag_x_neg:         .long 00000002
.L__flag_x_nan:         .long 00000003

.align 16

.L__real_ninf:      .quad 0x0fff0000000000000   # -inf
                    .quad 0x0000000000000000
.L__real_inf:       .quad 0x7ff0000000000000    # +inf
                    .quad 0x0000000000000000
.L__real_qnan:      .quad 0x7ff8000000000000   # qNaN
                    .quad 0x0000000000000000
.L__real_qnanbit:   .quad 0x0008000000000000
                    .quad 0x0000000000000000
.L__real_mant:      .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                    .quad 0x0000000000000000
.L__mask_1023:      .quad 0x00000000000003ff
                    .quad 0x0000000000000000
.L__mask_001:       .quad 0x0000000000000001
                    .quad 0x0000000000000000

.L__mask_mant_all8: .quad 0x000ff00000000000
                    .quad 0x0000000000000000
.L__mask_mant9:     .quad 0x0000080000000000
                    .quad 0x0000000000000000
.L__real_log2_e:        .quad 0x3ff71547652b82fe
                        .quad 0x0000000000000000

.L__real_log2_e_lead:   .quad 0x3ff7154400000000 # log2e_lead 1.44269180297851562500E+00
                        .quad 0x0000000000000000
.L__real_log2_e_tail:   .quad 0x3ecb295c17f0bbbe # log2e_tail 3.23791044778235969970E-06
                        .quad 0x0000000000000000

.L__real_two:       .quad 0x4000000000000000 # 2
                    .quad 0x0000000000000000

.L__real_one:       .quad 0x3ff0000000000000 # 1
                    .quad 0x0000000000000000

.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x0000000000000000

.L__mask_100:       .quad 0x0000000000000100
                    .quad 0x0000000000000000

.L__real_1_over_512:    .quad 0x3f60000000000000
                        .quad 0x0000000000000000

.L__real_1_over_2:  .quad 0x3fe0000000000000
                    .quad 0x0000000000000000
.L__real_1_over_3:  .quad 0x3fd5555555555555
                    .quad 0x0000000000000000
.L__real_1_over_4:  .quad 0x3fd0000000000000
                    .quad 0x0000000000000000
.L__real_1_over_5:  .quad 0x3fc999999999999a
                    .quad 0x0000000000000000
.L__real_1_over_6:  .quad 0x3fc5555555555555
                    .quad 0x0000000000000000

.L__mask_1023_f:    .quad 0x0c08ff80000000000
                    .quad 0x0000000000000000

.L__mask_2045:      .quad 0x00000000000007fd
                    .quad 0x0000000000000000

.L__real_threshold: .quad 0x3fb0000000000000 # .0625
                    .quad 0x0000000000000000

.L__real_notsign:   .quad 0x7ffFFFFFFFFFFFFF # ^sign bit
                    .quad 0x0000000000000000

.L__real_ca1:       .quad 0x3fb55555555554e6 # 8.33333333333317923934e-02
                    .quad 0x0000000000000000
.L__real_ca2:       .quad 0x3f89999999bac6d4 # 1.25000000037717509602e-02
                    .quad 0x0000000000000000
.L__real_ca3:       .quad 0x3f62492307f1519f # 2.23213998791944806202e-03
                    .quad 0x0000000000000000
.L__real_ca4:       .quad 0x3f3c8034c85dfff0 # 4.34887777707614552256e-04
                    .quad 0x0000000000000000
.L__mask_lower:     .quad 0x0ffffffff00000000
                    .quad 0x0000000000000000



.align 16
.L__log_256_lead:
                    .quad 0x0000000000000000
                    .quad 0x3f7709c460000000
                    .quad 0x3f86fe50b0000000
                    .quad 0x3f91363110000000
                    .quad 0x3f96e79680000000
                    .quad 0x3f9c9363b0000000
                    .quad 0x3fa11cd1d0000000
                    .quad 0x3fa3ed3090000000
                    .quad 0x3fa6bad370000000
                    .quad 0x3fa985bfc0000000
                    .quad 0x3fac4dfab0000000
                    .quad 0x3faf138980000000
                    .quad 0x3fb0eb3890000000
                    .quad 0x3fb24b5b70000000
                    .quad 0x3fb3aa2fd0000000
                    .quad 0x3fb507b830000000
                    .quad 0x3fb663f6f0000000
                    .quad 0x3fb7beee90000000
                    .quad 0x3fb918a160000000
                    .quad 0x3fba7111d0000000
                    .quad 0x3fbbc84240000000
                    .quad 0x3fbd1e34e0000000
                    .quad 0x3fbe72ec10000000
                    .quad 0x3fbfc66a00000000
                    .quad 0x3fc08c5880000000
                    .quad 0x3fc134e1b0000000
                    .quad 0x3fc1dcd190000000
                    .quad 0x3fc2842940000000
                    .quad 0x3fc32ae9e0000000
                    .quad 0x3fc3d11460000000
                    .quad 0x3fc476a9f0000000
                    .quad 0x3fc51bab90000000
                    .quad 0x3fc5c01a30000000
                    .quad 0x3fc663f6f0000000
                    .quad 0x3fc70742d0000000
                    .quad 0x3fc7a9fec0000000
                    .quad 0x3fc84c2bd0000000
                    .quad 0x3fc8edcae0000000
                    .quad 0x3fc98edd00000000
                    .quad 0x3fca2f6320000000
                    .quad 0x3fcacf5e20000000
                    .quad 0x3fcb6ecf10000000
                    .quad 0x3fcc0db6c0000000
                    .quad 0x3fccac1630000000
                    .quad 0x3fcd49ee40000000
                    .quad 0x3fcde73fe0000000
                    .quad 0x3fce840be0000000
                    .quad 0x3fcf205330000000
                    .quad 0x3fcfbc16b0000000
                    .quad 0x3fd02baba0000000
                    .quad 0x3fd0790ad0000000
                    .quad 0x3fd0c62970000000
                    .quad 0x3fd11307d0000000
                    .quad 0x3fd15fa670000000
                    .quad 0x3fd1ac05b0000000
                    .quad 0x3fd1f825f0000000
                    .quad 0x3fd24407a0000000
                    .quad 0x3fd28fab30000000
                    .quad 0x3fd2db10f0000000
                    .quad 0x3fd3263960000000
                    .quad 0x3fd37124c0000000
                    .quad 0x3fd3bbd3a0000000
                    .quad 0x3fd4064630000000
                    .quad 0x3fd4507cf0000000
                    .quad 0x3fd49a7840000000
                    .quad 0x3fd4e43880000000
                    .quad 0x3fd52dbdf0000000
                    .quad 0x3fd5770910000000
                    .quad 0x3fd5c01a30000000
                    .quad 0x3fd608f1b0000000
                    .quad 0x3fd6518fe0000000
                    .quad 0x3fd699f520000000
                    .quad 0x3fd6e221c0000000
                    .quad 0x3fd72a1630000000
                    .quad 0x3fd771d2b0000000
                    .quad 0x3fd7b957a0000000
                    .quad 0x3fd800a560000000
                    .quad 0x3fd847bc30000000
                    .quad 0x3fd88e9c70000000
                    .quad 0x3fd8d54670000000
                    .quad 0x3fd91bba80000000
                    .quad 0x3fd961f900000000
                    .quad 0x3fd9a80230000000
                    .quad 0x3fd9edd670000000
                    .quad 0x3fda337600000000
                    .quad 0x3fda78e140000000
                    .quad 0x3fdabe1870000000
                    .quad 0x3fdb031be0000000
                    .quad 0x3fdb47ebf0000000
                    .quad 0x3fdb8c88d0000000
                    .quad 0x3fdbd0f2e0000000
                    .quad 0x3fdc152a60000000
                    .quad 0x3fdc592fa0000000
                    .quad 0x3fdc9d02f0000000
                    .quad 0x3fdce0a490000000
                    .quad 0x3fdd2414c0000000
                    .quad 0x3fdd6753e0000000
                    .quad 0x3fddaa6220000000
                    .quad 0x3fdded3fd0000000
                    .quad 0x3fde2fed30000000
                    .quad 0x3fde726aa0000000
                    .quad 0x3fdeb4b840000000
                    .quad 0x3fdef6d670000000
                    .quad 0x3fdf38c560000000
                    .quad 0x3fdf7a8560000000
                    .quad 0x3fdfbc16b0000000
                    .quad 0x3fdffd7990000000
                    .quad 0x3fe01f5720000000
                    .quad 0x3fe03fda80000000
                    .quad 0x3fe0604710000000
                    .quad 0x3fe0809cf0000000
                    .quad 0x3fe0a0dc30000000
                    .quad 0x3fe0c10500000000
                    .quad 0x3fe0e11770000000
                    .quad 0x3fe10113b0000000
                    .quad 0x3fe120f9d0000000
                    .quad 0x3fe140c9f0000000
                    .quad 0x3fe1608440000000
                    .quad 0x3fe18028c0000000
                    .quad 0x3fe19fb7b0000000
                    .quad 0x3fe1bf3110000000
                    .quad 0x3fe1de9510000000
                    .quad 0x3fe1fde3d0000000
                    .quad 0x3fe21d1d50000000
                    .quad 0x3fe23c41d0000000
                    .quad 0x3fe25b5150000000
                    .quad 0x3fe27a4c00000000
                    .quad 0x3fe29931f0000000
                    .quad 0x3fe2b80340000000
                    .quad 0x3fe2d6c010000000
                    .quad 0x3fe2f56870000000
                    .quad 0x3fe313fc80000000
                    .quad 0x3fe3327c60000000
                    .quad 0x3fe350e830000000
                    .quad 0x3fe36f3ff0000000
                    .quad 0x3fe38d83e0000000
                    .quad 0x3fe3abb3f0000000
                    .quad 0x3fe3c9d060000000
                    .quad 0x3fe3e7d930000000
                    .quad 0x3fe405ce80000000
                    .quad 0x3fe423b070000000
                    .quad 0x3fe4417f20000000
                    .quad 0x3fe45f3a90000000
                    .quad 0x3fe47ce2f0000000
                    .quad 0x3fe49a7840000000
                    .quad 0x3fe4b7fab0000000
                    .quad 0x3fe4d56a50000000
                    .quad 0x3fe4f2c740000000
                    .quad 0x3fe5101180000000
                    .quad 0x3fe52d4940000000
                    .quad 0x3fe54a6e80000000
                    .quad 0x3fe5678170000000
                    .quad 0x3fe5848220000000
                    .quad 0x3fe5a170a0000000
                    .quad 0x3fe5be4d00000000
                    .quad 0x3fe5db1770000000
                    .quad 0x3fe5f7cff0000000
                    .quad 0x3fe61476a0000000
                    .quad 0x3fe6310b80000000
                    .quad 0x3fe64d8ed0000000
                    .quad 0x3fe66a0080000000
                    .quad 0x3fe68660c0000000
                    .quad 0x3fe6a2af90000000
                    .quad 0x3fe6beed20000000
                    .quad 0x3fe6db1960000000
                    .quad 0x3fe6f73480000000
                    .quad 0x3fe7133e90000000
                    .quad 0x3fe72f37a0000000
                    .quad 0x3fe74b1fd0000000
                    .quad 0x3fe766f720000000
                    .quad 0x3fe782bdb0000000
                    .quad 0x3fe79e73a0000000
                    .quad 0x3fe7ba18f0000000
                    .quad 0x3fe7d5adc0000000
                    .quad 0x3fe7f13220000000
                    .quad 0x3fe80ca620000000
                    .quad 0x3fe82809d0000000
                    .quad 0x3fe8435d50000000
                    .quad 0x3fe85ea0b0000000
                    .quad 0x3fe879d3f0000000
                    .quad 0x3fe894f740000000
                    .quad 0x3fe8b00aa0000000
                    .quad 0x3fe8cb0e30000000
                    .quad 0x3fe8e60200000000
                    .quad 0x3fe900e610000000
                    .quad 0x3fe91bba80000000
                    .quad 0x3fe9367f60000000
                    .quad 0x3fe95134d0000000
                    .quad 0x3fe96bdad0000000
                    .quad 0x3fe9867170000000
                    .quad 0x3fe9a0f8d0000000
                    .quad 0x3fe9bb70f0000000
                    .quad 0x3fe9d5d9f0000000
                    .quad 0x3fe9f033e0000000
                    .quad 0x3fea0a7ed0000000
                    .quad 0x3fea24bad0000000
                    .quad 0x3fea3ee7f0000000
                    .quad 0x3fea590640000000
                    .quad 0x3fea7315d0000000
                    .quad 0x3fea8d16b0000000
                    .quad 0x3feaa708f0000000
                    .quad 0x3feac0eca0000000
                    .quad 0x3feadac1e0000000
                    .quad 0x3feaf488b0000000
                    .quad 0x3feb0e4120000000
                    .quad 0x3feb27eb40000000
                    .quad 0x3feb418730000000
                    .quad 0x3feb5b14f0000000
                    .quad 0x3feb749480000000
                    .quad 0x3feb8e0620000000
                    .quad 0x3feba769b0000000
                    .quad 0x3febc0bf50000000
                    .quad 0x3febda0710000000
                    .quad 0x3febf34110000000
                    .quad 0x3fec0c6d40000000
                    .quad 0x3fec258bc0000000
                    .quad 0x3fec3e9ca0000000
                    .quad 0x3fec579fe0000000
                    .quad 0x3fec7095a0000000
                    .quad 0x3fec897df0000000
                    .quad 0x3feca258d0000000
                    .quad 0x3fecbb2660000000
                    .quad 0x3fecd3e6a0000000
                    .quad 0x3fecec9990000000
                    .quad 0x3fed053f60000000
                    .quad 0x3fed1dd810000000
                    .quad 0x3fed3663b0000000
                    .quad 0x3fed4ee240000000
                    .quad 0x3fed6753e0000000
                    .quad 0x3fed7fb890000000
                    .quad 0x3fed981060000000
                    .quad 0x3fedb05b60000000
                    .quad 0x3fedc899a0000000
                    .quad 0x3fede0cb30000000
                    .quad 0x3fedf8f020000000
                    .quad 0x3fee110860000000
                    .quad 0x3fee291420000000
                    .quad 0x3fee411360000000
                    .quad 0x3fee590630000000
                    .quad 0x3fee70eca0000000
                    .quad 0x3fee88c6b0000000
                    .quad 0x3feea09470000000
                    .quad 0x3feeb855f0000000
                    .quad 0x3feed00b40000000
                    .quad 0x3feee7b470000000
                    .quad 0x3feeff5180000000
                    .quad 0x3fef16e280000000
                    .quad 0x3fef2e6780000000
                    .quad 0x3fef45e080000000
                    .quad 0x3fef5d4da0000000
                    .quad 0x3fef74aef0000000
                    .quad 0x3fef8c0460000000
                    .quad 0x3fefa34e10000000
                    .quad 0x3fefba8c00000000
                    .quad 0x3fefd1be40000000
                    .quad 0x3fefe8e4f0000000
                    .quad 0x3ff0000000000000

.align 16
.L__log_256_tail:
                    .quad 0x0000000000000000
                    .quad 0x3deaf558ee95b37a
                    .quad 0x3debbc2145fe38de
                    .quad 0x3dfea5ec312ed069
                    .quad 0x3df70b48a629b89f
                    .quad 0x3e050a1f0cccdd01
                    .quad 0x3e044cd04bb60514
                    .quad 0x3e01a16898809d2d
                    .quad 0x3e063bf61cc4d81b
                    .quad 0x3dfa4a8ca305071d
                    .quad 0x3e121556bde9f1f0
                    .quad 0x3df9929cfd0e6835
                    .quad 0x3e2f453f35679ee9
                    .quad 0x3e2c26b47913459e
                    .quad 0x3e2a4fe385b009a2
                    .quad 0x3e180ceedb53cb4d
                    .quad 0x3e2592262cf998a7
                    .quad 0x3e1ae28a04f106b8
                    .quad 0x3e2c8c66b55ce464
                    .quad 0x3e2e690927d688b0
                    .quad 0x3de5b5774c7658b4
                    .quad 0x3e0adc16d26859c7
                    .quad 0x3df7fa5b21cbdb5d
                    .quad 0x3e2e160149209a68
                    .quad 0x3e39b4f3c72c4f78
                    .quad 0x3e222418b7fcd690
                    .quad 0x3e2d54aded7a9150
                    .quad 0x3e360f4c7f1aed15
                    .quad 0x3e13c570d0fa8f96
                    .quad 0x3e3b3514c7e0166e
                    .quad 0x3e3307ee9a6271d2
                    .quad 0x3dee9722922c0226
                    .quad 0x3e33f7ad0f3f4016
                    .quad 0x3e3592262cf998a7
                    .quad 0x3e23bc09fca70073
                    .quad 0x3e2f41777bc5f936
                    .quad 0x3dd781d97ee91247
                    .quad 0x3e306a56d76b9a84
                    .quad 0x3e2df9c37c0beb3a
                    .quad 0x3e1905c35651c429
                    .quad 0x3e3b69d927dfc23d
                    .quad 0x3e2d7e57a5afb633
                    .quad 0x3e3bb29bdc81c4db
                    .quad 0x3e38ee1b912d8994
                    .quad 0x3e3864b2df91e96a
                    .quad 0x3e1d8a40770df213
                    .quad 0x3e2d39a9331f27cf
                    .quad 0x3e32411e4e8eea54
                    .quad 0x3e3204d0144751b3
                    .quad 0x3e2268331dd8bd0b
                    .quad 0x3e47606012de0634
                    .quad 0x3e3550aa3a93ec7e
                    .quad 0x3e45a616eb9612e0
                    .quad 0x3e3aec23fd65f8e1
                    .quad 0x3e248f838294639c
                    .quad 0x3e3b62384cafa1a3
                    .quad 0x3e461c0e73048b72
                    .quad 0x3e36cc9a0d8c0e85
                    .quad 0x3e489b355ede26f4
                    .quad 0x3e2b5941acd71f1e
                    .quad 0x3e4d499bd9b32266
                    .quad 0x3e043b9f52b061ba
                    .quad 0x3e46360892eb65e6
                    .quad 0x3e4dba9f8729ab41
                    .quad 0x3e479a3715fc9257
                    .quad 0x3e0d1f6d3f77ae38
                    .quad 0x3e48992d66fb9ec1
                    .quad 0x3e4666f195620f03
                    .quad 0x3e43f7ad0f3f4016
                    .quad 0x3e30a522b65bc039
                    .quad 0x3e319dee9b9489e3
                    .quad 0x3e323352e1a31521
                    .quad 0x3e4b3a19bcaf1aa4
                    .quad 0x3e3f2f060a50d366
                    .quad 0x3e44fdf677c8dfd9
                    .quad 0x3e48a35588aec6df
                    .quad 0x3e28b0e2a19575b0
                    .quad 0x3e2ec30c6e3e04a7
                    .quad 0x3e2705912d25b325
                    .quad 0x3e2dae1b8d59e849
                    .quad 0x3e423e2e1169656a
                    .quad 0x3e349d026e33d675
                    .quad 0x3e423c465e6976da
                    .quad 0x3e366c977e236c73
                    .quad 0x3e44fec0a13af881
                    .quad 0x3e3bdefbd14a0816
                    .quad 0x3e42fe3e91c348e4
                    .quad 0x3e4fc0c868ccc02d
                    .quad 0x3e3ce20a829051bb
                    .quad 0x3e47f10cf32e6bba
                    .quad 0x3e43cf2061568859
                    .quad 0x3e484995cb804b94
                    .quad 0x3e4a52b6acfcfdca
                    .quad 0x3e3b291ecf4dff1e
                    .quad 0x3e21d2c3e64ae851
                    .quad 0x3e4017e4faa42b7d
                    .quad 0x3de975077f1f5f0c
                    .quad 0x3e20327dc8093a52
                    .quad 0x3e3108d9313aec65
                    .quad 0x3e4a12e5301be44a
                    .quad 0x3e1e754d20c519e1
                    .quad 0x3e3f456f394f9727
                    .quad 0x3e29471103e8f00d
                    .quad 0x3e3ef3150343f8df
                    .quad 0x3e41960d9d9c3263
                    .quad 0x3e4204d0144751b3
                    .quad 0x3e4507ff357398fe
                    .quad 0x3e4dc9937fc8cafd
                    .quad 0x3e572f32fe672868
                    .quad 0x3e53e49d647d323e
                    .quad 0x3e33fb81ea92d9e0
                    .quad 0x3e43e387ef003635
                    .quad 0x3e1ac754cb104aea
                    .quad 0x3e4535f0444ebaaf
                    .quad 0x3e253c8ea7b1cdda
                    .quad 0x3e3cf0c0396a568b
                    .quad 0x3e5543ca873c2b4a
                    .quad 0x3e425780181e2b37
                    .quad 0x3e5ee52ed49d71d2
                    .quad 0x3e51e64842e2c386
                    .quad 0x3e5d2ba01bc76a27
                    .quad 0x3e5b39774c30f499
                    .quad 0x3e38740932120aea
                    .quad 0x3e576dab3462a1e8
                    .quad 0x3e409c9f20203b31
                    .quad 0x3e516e7a08ad0d1a
                    .quad 0x3e46172fe015e13b
                    .quad 0x3e49e4558147cf67
                    .quad 0x3e4cfdeb43cfd005
                    .quad 0x3e3a809c03254a71
                    .quad 0x3e47acfc98509e33
                    .quad 0x3e54366de473e474
                    .quad 0x3e5569394d90d724
                    .quad 0x3e32b83ec743664c
                    .quad 0x3e56db22c4808ee5
                    .quad 0x3df7ae84940df0e1
                    .quad 0x3e554042cd999564
                    .quad 0x3e4242b8488b3056
                    .quad 0x3e4e7dc059ab8a9e
                    .quad 0x3e5a71e977d7da5f
                    .quad 0x3e5d30d552ce0ec3
                    .quad 0x3e43208592b6c6b7
                    .quad 0x3e51440e7149afff
                    .quad 0x3e36812c371a1c87
                    .quad 0x3e579a3715fc9257
                    .quad 0x3e57c92f2af8b0ca
                    .quad 0x3e56679d8894dbdf
                    .quad 0x3e2a9f33e77507f0
                    .quad 0x3e4c22a3e377a524
                    .quad 0x3e3723c84a77a4dc
                    .quad 0x3e594a871b636194
                    .quad 0x3e570d6058f62f4d
                    .quad 0x3e4a6274cf0e362f
                    .quad 0x3e42fe3570af1a0b
                    .quad 0x3e596a286955d67e
                    .quad 0x3e442104f127091e
                    .quad 0x3e407826bae32c6b
                    .quad 0x3df8d8844ce77237
                    .quad 0x3e5eaa609080d4b4
                    .quad 0x3e4dc66fbe61efc4
                    .quad 0x3e5c8f11979a5db6
                    .quad 0x3e52dedf0e6f1770
                    .quad 0x3e5cb41e1410132a
                    .quad 0x3e32866d705c553d
                    .quad 0x3e54ec3293b2fbe0
                    .quad 0x3e578b8c2f4d0fe1
                    .quad 0x3e562ad8f7ca2cff
                    .quad 0x3e5a298b5f973a2c
                    .quad 0x3e49381d4f1b95e0
                    .quad 0x3e564c7bdb9bc56c
                    .quad 0x3e5fbb4caef790fc
                    .quad 0x3e51200c3f899927
                    .quad 0x3e526a05c813d56e
                    .quad 0x3e4681e2910108ee
                    .quad 0x3e282cf15d12ecd7
                    .quad 0x3e0a537e32446892
                    .quad 0x3e46f9c1cb6f7010
                    .quad 0x3e4328ddcedf39d8
                    .quad 0x3e164f64c210df9d
                    .quad 0x3e58f676e17cc811
                    .quad 0x3e560ddf1680dd45
                    .quad 0x3e5e2da951c2d91b
                    .quad 0x3e5696777b66d115
                    .quad 0x3e311eb3043f5601
                    .quad 0x3e48000b33f90fd4
                    .quad 0x3e523e2e1169656a
                    .quad 0x3e5b41565d3990cb
                    .quad 0x3e46138b8d9d31e6
                    .quad 0x3e3565afaf7f6248
                    .quad 0x3e4b68e0ba153594
                    .quad 0x3e3d87027ef4ab9a
                    .quad 0x3e556b9c99085939
                    .quad 0x3e5aa02166cccab2
                    .quad 0x3e5991d2aca399a1
                    .quad 0x3e54982259cc625d
                    .quad 0x3e4b9feddaab9820
                    .quad 0x3e3c70c0f683cc68
                    .quad 0x3e213156425e67e5
                    .quad 0x3df79063deab051f
                    .quad 0x3e27e2744b2b8ca5
                    .quad 0x3e4600534df378df
                    .quad 0x3e59322676507a79
                    .quad 0x3e4c4720cb4558b5
                    .quad 0x3e445e4b56add63a
                    .quad 0x3e4af321af5e9bb5
                    .quad 0x3e57f1e1148dad64
                    .quad 0x3e42a4022f65e2e6
                    .quad 0x3e11f2ccbcd0d3cc
                    .quad 0x3e5eaa65b49696e2
                    .quad 0x3e110e6123a74764
                    .quad 0x3e3cf24b2077c3f6
                    .quad 0x3e4fc8d8164754da
                    .quad 0x3e598cfcdb6a2dbc
                    .quad 0x3e24464a6bcdf47b
                    .quad 0x3e41f1774d8b66a6
                    .quad 0x3e459920a2adf6fa
                    .quad 0x3e370d02a99b4c5a
                    .quad 0x3e576b6cafa2532d
                    .quad 0x3e5d23c38ec17936
                    .quad 0x3e541b6b1b0e66c4
                    .quad 0x3e5952662c6bfdc7
                    .quad 0x3e4333f3d6bb35ec
                    .quad 0x3e195120d8486e92
                    .quad 0x3e5db8a405fac56e
                    .quad 0x3e5a4c112ce6312e
                    .quad 0x3e536987e1924e45
                    .quad 0x3e33f98ea94bc1bd
                    .quad 0x3e459718aacb6ec7
                    .quad 0x3df975077f1f5f0c
                    .quad 0x3e13654d88f20500
                    .quad 0x3e40f598530f101b
                    .quad 0x3e5145f6c94f7fd7
                    .quad 0x3e567fead8bcce75
                    .quad 0x3e52e67148cd0a7b
                    .quad 0x3e10d5e5897de907
                    .quad 0x3e5b5ee92c53d919
                    .quad 0x3e5c1c02803f7554
                    .quad 0x3e5d5caa7a35c9f7
                    .quad 0x3e5910459cac3223
                    .quad 0x3e41fbe1bb98afdf
                    .quad 0x3e3b135395510d1e
                    .quad 0x3e47b8f0e7b8e757
                    .quad 0x3e519511f61a96b8
                    .quad 0x3e5117d846ae1f8e
                    .quad 0x3e2b3a9507d6dc1f
                    .quad 0x3e15fa7c78c9e676
                    .quad 0x3e2db76303b21928
                    .quad 0x3e27eb8450ac22ed
                    .quad 0x3e579e0caa9c9ab7
                    .quad 0x3e59de6d7cba1bbe
                    .quad 0x3e1df5f5baf436cb
                    .quad 0x3e3e746344728dbf
                    .quad 0x3e277c23362928b9
                    .quad 0x3e4715137cfeba9f
                    .quad 0x3e58fe55f2856443
                    .quad 0x3e25bd1a025d9e24
                    .quad 0x0000000000000000

.align 16
.L__log_F_inv:
                    .quad 0x4000000000000000
                    .quad 0x3fffe01fe01fe020
                    .quad 0x3fffc07f01fc07f0
                    .quad 0x3fffa11caa01fa12
                    .quad 0x3fff81f81f81f820
                    .quad 0x3fff6310aca0dbb5
                    .quad 0x3fff44659e4a4271
                    .quad 0x3fff25f644230ab5
                    .quad 0x3fff07c1f07c1f08
                    .quad 0x3ffee9c7f8458e02
                    .quad 0x3ffecc07b301ecc0
                    .quad 0x3ffeae807aba01eb
                    .quad 0x3ffe9131abf0b767
                    .quad 0x3ffe741aa59750e4
                    .quad 0x3ffe573ac901e574
                    .quad 0x3ffe3a9179dc1a73
                    .quad 0x3ffe1e1e1e1e1e1e
                    .quad 0x3ffe01e01e01e01e
                    .quad 0x3ffde5d6e3f8868a
                    .quad 0x3ffdca01dca01dca
                    .quad 0x3ffdae6076b981db
                    .quad 0x3ffd92f2231e7f8a
                    .quad 0x3ffd77b654b82c34
                    .quad 0x3ffd5cac807572b2
                    .quad 0x3ffd41d41d41d41d
                    .quad 0x3ffd272ca3fc5b1a
                    .quad 0x3ffd0cb58f6ec074
                    .quad 0x3ffcf26e5c44bfc6
                    .quad 0x3ffcd85689039b0b
                    .quad 0x3ffcbe6d9601cbe7
                    .quad 0x3ffca4b3055ee191
                    .quad 0x3ffc8b265afb8a42
                    .quad 0x3ffc71c71c71c71c
                    .quad 0x3ffc5894d10d4986
                    .quad 0x3ffc3f8f01c3f8f0
                    .quad 0x3ffc26b5392ea01c
                    .quad 0x3ffc0e070381c0e0
                    .quad 0x3ffbf583ee868d8b
                    .quad 0x3ffbdd2b899406f7
                    .quad 0x3ffbc4fd65883e7b
                    .quad 0x3ffbacf914c1bad0
                    .quad 0x3ffb951e2b18ff23
                    .quad 0x3ffb7d6c3dda338b
                    .quad 0x3ffb65e2e3beee05
                    .quad 0x3ffb4e81b4e81b4f
                    .quad 0x3ffb37484ad806ce
                    .quad 0x3ffb2036406c80d9
                    .quad 0x3ffb094b31d922a4
                    .quad 0x3ffaf286bca1af28
                    .quad 0x3ffadbe87f94905e
                    .quad 0x3ffac5701ac5701b
                    .quad 0x3ffaaf1d2f87ebfd
                    .quad 0x3ffa98ef606a63be
                    .quad 0x3ffa82e65130e159
                    .quad 0x3ffa6d01a6d01a6d
                    .quad 0x3ffa574107688a4a
                    .quad 0x3ffa41a41a41a41a
                    .quad 0x3ffa2c2a87c51ca0
                    .quad 0x3ffa16d3f97a4b02
                    .quad 0x3ffa01a01a01a01a
                    .quad 0x3ff9ec8e951033d9
                    .quad 0x3ff9d79f176b682d
                    .quad 0x3ff9c2d14ee4a102
                    .quad 0x3ff9ae24ea5510da
                    .quad 0x3ff999999999999a
                    .quad 0x3ff9852f0d8ec0ff
                    .quad 0x3ff970e4f80cb872
                    .quad 0x3ff95cbb0be377ae
                    .quad 0x3ff948b0fcd6e9e0
                    .quad 0x3ff934c67f9b2ce6
                    .quad 0x3ff920fb49d0e229
                    .quad 0x3ff90d4f120190d5
                    .quad 0x3ff8f9c18f9c18fa
                    .quad 0x3ff8e6527af1373f
                    .quad 0x3ff8d3018d3018d3
                    .quad 0x3ff8bfce8062ff3a
                    .quad 0x3ff8acb90f6bf3aa
                    .quad 0x3ff899c0f601899c
                    .quad 0x3ff886e5f0abb04a
                    .quad 0x3ff87427bcc092b9
                    .quad 0x3ff8618618618618
                    .quad 0x3ff84f00c2780614
                    .quad 0x3ff83c977ab2bedd
                    .quad 0x3ff82a4a0182a4a0
                    .quad 0x3ff8181818181818
                    .quad 0x3ff8060180601806
                    .quad 0x3ff7f405fd017f40
                    .quad 0x3ff7e225515a4f1d
                    .quad 0x3ff7d05f417d05f4
                    .quad 0x3ff7beb3922e017c
                    .quad 0x3ff7ad2208e0ecc3
                    .quad 0x3ff79baa6bb6398b
                    .quad 0x3ff78a4c8178a4c8
                    .quad 0x3ff77908119ac60d
                    .quad 0x3ff767dce434a9b1
                    .quad 0x3ff756cac201756d
                    .quad 0x3ff745d1745d1746
                    .quad 0x3ff734f0c541fe8d
                    .quad 0x3ff724287f46debc
                    .quad 0x3ff713786d9c7c09
                    .quad 0x3ff702e05c0b8170
                    .quad 0x3ff6f26016f26017
                    .quad 0x3ff6e1f76b4337c7
                    .quad 0x3ff6d1a62681c861
                    .quad 0x3ff6c16c16c16c17
                    .quad 0x3ff6b1490aa31a3d
                    .quad 0x3ff6a13cd1537290
                    .quad 0x3ff691473a88d0c0
                    .quad 0x3ff6816816816817
                    .quad 0x3ff6719f3601671a
                    .quad 0x3ff661ec6a5122f9
                    .quad 0x3ff6524f853b4aa3
                    .quad 0x3ff642c8590b2164
                    .quad 0x3ff63356b88ac0de
                    .quad 0x3ff623fa77016240
                    .quad 0x3ff614b36831ae94
                    .quad 0x3ff6058160581606
                    .quad 0x3ff5f66434292dfc
                    .quad 0x3ff5e75bb8d015e7
                    .quad 0x3ff5d867c3ece2a5
                    .quad 0x3ff5c9882b931057
                    .quad 0x3ff5babcc647fa91
                    .quad 0x3ff5ac056b015ac0
                    .quad 0x3ff59d61f123ccaa
                    .quad 0x3ff58ed2308158ed
                    .quad 0x3ff5805601580560
                    .quad 0x3ff571ed3c506b3a
                    .quad 0x3ff56397ba7c52e2
                    .quad 0x3ff5555555555555
                    .quad 0x3ff54725e6bb82fe
                    .quad 0x3ff5390948f40feb
                    .quad 0x3ff52aff56a8054b
                    .quad 0x3ff51d07eae2f815
                    .quad 0x3ff50f22e111c4c5
                    .quad 0x3ff5015015015015
                    .quad 0x3ff4f38f62dd4c9b
                    .quad 0x3ff4e5e0a72f0539
                    .quad 0x3ff4d843bedc2c4c
                    .quad 0x3ff4cab88725af6e
                    .quad 0x3ff4bd3edda68fe1
                    .quad 0x3ff4afd6a052bf5b
                    .quad 0x3ff4a27fad76014a
                    .quad 0x3ff49539e3b2d067
                    .quad 0x3ff4880522014880
                    .quad 0x3ff47ae147ae147b
                    .quad 0x3ff46dce34596066
                    .quad 0x3ff460cbc7f5cf9a
                    .quad 0x3ff453d9e2c776ca
                    .quad 0x3ff446f86562d9fb
                    .quad 0x3ff43a2730abee4d
                    .quad 0x3ff42d6625d51f87
                    .quad 0x3ff420b5265e5951
                    .quad 0x3ff4141414141414
                    .quad 0x3ff40782d10e6566
                    .quad 0x3ff3fb013fb013fb
                    .quad 0x3ff3ee8f42a5af07
                    .quad 0x3ff3e22cbce4a902
                    .quad 0x3ff3d5d991aa75c6
                    .quad 0x3ff3c995a47babe7
                    .quad 0x3ff3bd60d9232955
                    .quad 0x3ff3b13b13b13b14
                    .quad 0x3ff3a524387ac822
                    .quad 0x3ff3991c2c187f63
                    .quad 0x3ff38d22d366088e
                    .quad 0x3ff3813813813814
                    .quad 0x3ff3755bd1c945ee
                    .quad 0x3ff3698df3de0748
                    .quad 0x3ff35dce5f9f2af8
                    .quad 0x3ff3521cfb2b78c1
                    .quad 0x3ff34679ace01346
                    .quad 0x3ff33ae45b57bcb2
                    .quad 0x3ff32f5ced6a1dfa
                    .quad 0x3ff323e34a2b10bf
                    .quad 0x3ff3187758e9ebb6
                    .quad 0x3ff30d190130d190
                    .quad 0x3ff301c82ac40260
                    .quad 0x3ff2f684bda12f68
                    .quad 0x3ff2eb4ea1fed14b
                    .quad 0x3ff2e025c04b8097
                    .quad 0x3ff2d50a012d50a0
                    .quad 0x3ff2c9fb4d812ca0
                    .quad 0x3ff2bef98e5a3711
                    .quad 0x3ff2b404ad012b40
                    .quad 0x3ff2a91c92f3c105
                    .quad 0x3ff29e4129e4129e
                    .quad 0x3ff293725bb804a5
                    .quad 0x3ff288b01288b013
                    .quad 0x3ff27dfa38a1ce4d
                    .quad 0x3ff27350b8812735
                    .quad 0x3ff268b37cd60127
                    .quad 0x3ff25e22708092f1
                    .quad 0x3ff2539d7e9177b2
                    .quad 0x3ff2492492492492
                    .quad 0x3ff23eb79717605b
                    .quad 0x3ff23456789abcdf
                    .quad 0x3ff22a0122a0122a
                    .quad 0x3ff21fb78121fb78
                    .quad 0x3ff21579804855e6
                    .quad 0x3ff20b470c67c0d9
                    .quad 0x3ff2012012012012
                    .quad 0x3ff1f7047dc11f70
                    .quad 0x3ff1ecf43c7fb84c
                    .quad 0x3ff1e2ef3b3fb874
                    .quad 0x3ff1d8f5672e4abd
                    .quad 0x3ff1cf06ada2811d
                    .quad 0x3ff1c522fc1ce059
                    .quad 0x3ff1bb4a4046ed29
                    .quad 0x3ff1b17c67f2bae3
                    .quad 0x3ff1a7b9611a7b96
                    .quad 0x3ff19e0119e0119e
                    .quad 0x3ff19453808ca29c
                    .quad 0x3ff18ab083902bdb
                    .quad 0x3ff1811811811812
                    .quad 0x3ff1778a191bd684
                    .quad 0x3ff16e0689427379
                    .quad 0x3ff1648d50fc3201
                    .quad 0x3ff15b1e5f75270d
                    .quad 0x3ff151b9a3fdd5c9
                    .quad 0x3ff1485f0e0acd3b
                    .quad 0x3ff13f0e8d344724
                    .quad 0x3ff135c81135c811
                    .quad 0x3ff12c8b89edc0ac
                    .quad 0x3ff12358e75d3033
                    .quad 0x3ff11a3019a74826
                    .quad 0x3ff1111111111111
                    .quad 0x3ff107fbbe011080
                    .quad 0x3ff0fef010fef011
                    .quad 0x3ff0f5edfab325a2
                    .quad 0x3ff0ecf56be69c90
                    .quad 0x3ff0e40655826011
                    .quad 0x3ff0db20a88f4696
                    .quad 0x3ff0d24456359e3a
                    .quad 0x3ff0c9714fbcda3b
                    .quad 0x3ff0c0a7868b4171
                    .quad 0x3ff0b7e6ec259dc8
                    .quad 0x3ff0af2f722eecb5
                    .quad 0x3ff0a6810a6810a7
                    .quad 0x3ff09ddba6af8360
                    .quad 0x3ff0953f39010954
                    .quad 0x3ff08cabb37565e2
                    .quad 0x3ff0842108421084
                    .quad 0x3ff07b9f29b8eae2
                    .quad 0x3ff073260a47f7c6
                    .quad 0x3ff06ab59c7912fb
                    .quad 0x3ff0624dd2f1a9fc
                    .quad 0x3ff059eea0727586
                    .quad 0x3ff05197f7d73404
                    .quad 0x3ff04949cc1664c5
                    .quad 0x3ff0410410410410
                    .quad 0x3ff038c6b78247fc
                    .quad 0x3ff03091b51f5e1a
                    .quad 0x3ff02864fc7729e9
                    .quad 0x3ff0204081020408
                    .quad 0x3ff0182436517a37
                    .quad 0x3ff0101010101010
                    .quad 0x3ff0080402010080
                    .quad 0x3ff0000000000000
                    .quad 0x0000000000000000


