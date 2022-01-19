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
# vrd2_log1p_bdozr.S
#
# A bulldozer implementation of vrd2_log libm function.
#
# Prototype:
#
#     __m128d vrd2_log1p(__m128d x);
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
#

#include "fn_macros.h"
#include "log_tables.h"
#define fname ALM_PROTO_FMA3(vrda_log1p)


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

cmp         $0x0, %edi
    jle         .L__return
    cmp         $0x0, %rsi
    je         .L__return
    cmp         $0x0, %rdx
    je         .L__return


.p2align 4
.L__process_next2:
    sub         $0x2, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first1
    movdqu      (%rsi, %rdi, 8), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first1:
    jl          .L__return #even elements, all are processed %rdi == -2
    mov         $0x0, %rcx
    movsd       (%rsi, %rcx, 8), %xmm0

.p2align 4
.L__start:



    # compute exponent part
    vpand        .L__sign_mask_64(%rip),%xmm0,%xmm1
    vpcmpeqq     .L__real_epsilon(%rip),%xmm1,%xmm1 #mask for epsilons
    vcmpltpd     .L__real_minus_one(%rip),%xmm0,%xmm2 #mask for input less than -1.0
    #get the mask for exactly -1.0
    vcmpeqpd     .L__real_minus_one(%rip),%xmm0,%xmm15

    #get the mask for Nans and Infs
    vpand        .L__exp_mask(%rip),%xmm0,%xmm14  # remove the mantissa and sign
    vpcmpeqq     .L__exp_mask(%rip),%xmm14,%xmm14

    #calculate log1p
    vcmpnlepd    .L__real_log1p_thresh2(%rip), %xmm0,%xmm3
    vcmpltpd     .L__real_log1p_thresh1(%rip), %xmm0,%xmm4
    vpor         %xmm4, %xmm3, %xmm3 #

    #calculate outside the threshold range first
    vaddpd       .L__one_mask_64(%rip), %xmm0,%xmm5  #xmm5 = 1.0 +x

    vpand       .L__exp_mask(%rip),%xmm5,%xmm4
    vpsrlq       $52,%xmm4,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    vpsubq       .L__mask_1023(%rip),%xmm4,%xmm4

    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    vpand        .L__index_mask1(%rip), %xmm5, %xmm6
    vpand        .L__index_mask2(%rip), %xmm5, %xmm7
    vpor         .L__index_mask3(%rip), %xmm6, %xmm6

    vpsrlq       $45,%xmm7, %xmm7 # ((ux & 0x0000200000000000) >> 45)
    vpsrlq       $46,%xmm6, %xmm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    vpaddq       %xmm7,%xmm6,%xmm6 # index = xmm6
    vpshufd      $0xF8,%xmm6,%xmm7 # [11 11 10 00] = $0xF8
    vcvtdq2pd    %xmm7,%xmm7

    vmulpd       .L__index_constant1(%rip),%xmm7,%xmm7 # f1 =index*0.0078125
    vpsubq       .L__index_constant2(%rip),%xmm6,%xmm6 # index -= 64

    vpand        .L__mantissa_mask(%rip),%xmm5,%xmm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS_DP64, f);
    vpor         .L__one_mask_64(%rip),%xmm5,%xmm5 # xmm5 = f
    #vsubpd       %xmm7,%xmm5,%xmm5 # xmm5 = f2 = f - f1;

    vpshufd      $0x88,%xmm4,%xmm8 # [10 00 10 00] = 0x88
    vmovapd      .L__plus_sixtyone_minus_two(%rip),%xmm9
    vpcmpgtd     %xmm8,%xmm9,%xmm9
    vpsrldq      $8,%xmm9,%xmm10
    #IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    vpandn       %xmm9,%xmm10,%xmm10 # xmm9 stores the mask for all the numbers which lie between -2 and 61

    vpshufd      $0x50, %xmm10,%xmm9 # [01 01 00 00] = 0x50
    vsubpd       %xmm7,%xmm5,%xmm5 # f2 = f - f1;
    #ELSE
    vmovapd      .L__mask_1023(%rip),%xmm11
    vpsubq        %xmm4,%xmm11,%xmm11

    #ux = (unsigned long long)(0x3ff - xexp) << EXPSHIFTBITS_DP64;
    #PUT_BITS_DP64(ux,m2);
    vpsllq       $52,%xmm11,%xmm11 # xmm11 = m2;
    #movapd      %xmm11,%xmm13
    vsubpd       %xmm7,%xmm11,%xmm13 # (m2 - f1)
    vfmadd231pd     %xmm0,%xmm11,%xmm13 # xmm13 =f2
    #xmm7=f1, xmm13=m2, xmm11 = m2*x

    #  z1 = ln_lead_table[index];
    #  q = ln_tail_table[index];
    vmovd        %xmm6,%r10 # move lower order index
    vpsrldq      $8,%xmm6,%xmm6
    vmovd        %xmm6,%r11 # move higher order index
    lea         .L__ln_lead_64_table(%rip), %r9
    lea         .L__ln_tail_64_table(%rip), %r8
    vmovlpd      (%r9,%r10,8), %xmm6,%xmm6
    vmovhpd      (%r9,%r11,8), %xmm6,%xmm6 # xmm6 = z1 = ln_lead_table[index]
    vmovlpd      (%r8,%r10,8), %xmm8, %xmm8
    vmovhpd      (%r8,%r11,8), %xmm8, %xmm8 # xmm8 = q = ln_tail_table[index]
    #f2 = xmm13/xmm5 f1 = xmm7
    #vpcmov       %xmm9,%xmm5,%xmm13,%xmm13
    VANDNPD %xmm5, %xmm9, %xmm11
    VANDPD %xmm9, %xmm13, %xmm13
    VORPD %xmm11, %xmm13, %xmm13

    vfmadd231pd     .L__real_half(%rip),%xmm13,%xmm7 # (f1 + 0.5 * f2);
    vdivpd       %xmm7,%xmm13,%xmm7  # u = f2 / (f1 + 0.5 * f2);
    vmulpd       %xmm7,%xmm7,%xmm5  # v = u*u

    vmovapd      .L__real_cb_3(%rip), %xmm13 # cb_3
    vmovapd      %xmm5,%xmm11 #
    vfmadd213pd  .L__real_cb_2(%rip),%xmm13,%xmm5 # cb_2 + v*cb_3

    vfmadd213pd  .L__real_cb_1(%rip),%xmm11,%xmm5 # (cb_1 + v * (cb_2 + v * cb_3))
    vmulpd      %xmm5,%xmm11,%xmm5 # poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    #poly = xmm5 u = xmm7 q = xmm8
    vfmadd132pd    %xmm7,%xmm7,%xmm5
    vaddpd      %xmm5,%xmm8,%xmm5   # z2 = q + (u + u * poly)

    vpshufd      $0xF8,%xmm4,%xmm4 # [11 11 10 00] = 0xF8
    vcvtdq2pd    %xmm4,%xmm4 # xexp (float)
    vmovapd      %xmm4,%xmm8 # xmm8 = xexp
    vfmadd132pd     .L__real_log2_lead(%rip), %xmm6, %xmm4 # r1 = (xexp * log2_lead + z1)
    vfmadd132pd    .L__real_log2_tail(%rip), %xmm5, %xmm8 # r2 = (xexp * log2_tail + z2)
    vaddpd       %xmm4,%xmm8, %xmm8 # return r1 + r2
    ####END of calculating outside the threshold

    ####Now calculate when the value lies within the threshold
    #xmm0 still contains the input x = r1
    vmovapd     %xmm0,%xmm4 #  r1
    #movapd     %xmm4,%xmm7 #
    vaddpd      .L__plus_two(%rip),%xmm4, %xmm5 # (2.0 + r)
    vdivpd      %xmm5,%xmm4,%xmm6 # xmm6 = u = r / (2.0 + r);
    vmulpd      %xmm6,%xmm0,%xmm7 # correction = r * u # TBD
    vaddpd      %xmm6,%xmm6, %xmm6 # u = u+ u
    vmulpd      %xmm6,%xmm6,%xmm9 # v = u * u
    #r1 = xmm4 v = xmm9, u = xmm6, correction = xmm7
#TBD I think this needs some reordering to avoid the huge number of stalls.
    vmovapd     .L__real_ca_4(%rip),%xmm10 # xmm10 = ca_4
    vfmadd213pd    .L__real_ca_3(%rip),%xmm9,%xmm10 # (ca_3 + (v * ca_4))
    vfmadd213pd    .L__real_ca_2(%rip),%xmm9,%xmm10 # (ca_2 + v * (ca_3 + v * ca_4))
    vfmadd213pd   .L__real_ca_1(%rip),%xmm9,%xmm10 # (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))

    vmulpd      %xmm9,%xmm10,%xmm10 # v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    #vmulpd      %xmm6,%xmm10,%xmm10 # u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))

    vfmsub213pd  %xmm7,%xmm6,%xmm10 # r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 +  v * ca_4))) - correction)

    vaddpd     %xmm4,%xmm10,%xmm10
    #END calculating within the threshold



    #now restore the proper results
    #vpcmov     %xmm3,%xmm10,%xmm8,%xmm3
    VANDNPD %xmm10, %xmm3, %xmm7
    VANDPD %xmm3, %xmm8, %xmm3
    VORPD %xmm7, %xmm3, %xmm3

    vmovapd   .L__real_qnan(%rip), %xmm8
    vmovapd .L__real_ninf(%rip),%xmm13

    #process inputs = 0x3ca0000000000000
    #vpcmov     %xmm1,%xmm3,%xmm0,%xmm1
    VANDNPD %xmm3,%xmm1, %xmm7
    VANDPD %xmm1, %xmm0, %xmm1
    VORPD %xmm7, %xmm1, %xmm1

    # now restore if there are some inputs less than -1.0
    #vpcmov    %xmm2,%xmm1,%xmm8,%xmm2
    VANDNPD %xmm1, %xmm2, %xmm7
    VANDPD %xmm2, %xmm8, %xmm2
    VORPD %xmm7, %xmm2, %xmm2


    # now restore if there are some inputs with -1.0
    #vpcmov    %xmm15,%xmm2,%xmm13,%xmm8
    VANDNPD %xmm2, %xmm15, %xmm7
    VANDPD %xmm15, %xmm13, %xmm8
    VORPD %xmm7, %xmm8, %xmm8

    # now restore the Nans and Infs
    vpand      .L__sign_bit_64(%rip),%xmm0,%xmm3 # for negative infities we need to set the result as Qnan
    vaddpd     %xmm0,%xmm0,%xmm0
    #so we get the sign bit and move it to the qnan Bit.
    #then OR it with Qnan/inf result
    vpsrlq     $12,%xmm3,%xmm3
    vpand      %xmm14,%xmm3,%xmm3
    vpand      %xmm14,%xmm0,%xmm0
    vpor       %xmm3,%xmm0,%xmm0

    vpandn     %xmm8,%xmm14,%xmm14
    vpor       %xmm14,%xmm0,%xmm0
##########################################

cmp         $-1,  %rdi
    je          .L__store1
    movdqu       %xmm0, (%rdx, %rdi, 8)
    jmp         .L__process_next2

.p2align 4
.L__store1:
    inc        %rdi
    movsd      %xmm0, (%rdx,%rdi,8)

.L__return:
    ret

.section .rodata

.align 16
.L__sign_mask_64:     .quad 0x7FFFFFFFFFFFFFFF
                      .quad 0x7FFFFFFFFFFFFFFF
.L__real_epsilon:     .quad 0x3ca0000000000000
                      .quad 0x3ca0000000000000
.L__real_minus_one:   .quad  0xBFF0000000000000
                      .quad  0xBFF0000000000000
.L__exp_mask:         .quad 0x7ff0000000000000   #
                      .quad 0x7ff0000000000000
.L__exponent_compare: .quad 0x7FF000007FF00000
                      .quad 0x7FF000007FF00000
# The values exp(-1/16)-1 and exp(1/16)-1 */
.L__real_log1p_thresh2: .quad 0x3fb082b577d34ed8 #  6.44944589178594318568e-02;
                        .quad 0x3fb082b577d34ed8
.L__real_log1p_thresh1: .quad 0xbfaf0540438fd5c4 # -6.05869371865242201114e-02,
                        .quad 0xbfaf0540438fd5c4
.L__one_mask_64:      .quad 0x3ff0000000000000 # 1
                      .quad 0x3ff0000000000000
.L__mask_1023:        .quad 0x00000000000003ff
                      .quad 0x00000000000003ff
.L__mantissa_mask:    .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                      .quad 0x000FFFFFFFFFFFFF

.L__index_mask1:      .quad 0x000fc00000000000
                      .quad 0x000fc00000000000
.L__index_mask2:      .quad 0x0000200000000000
                      .quad 0x0000200000000000
.L__index_mask3:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
.L__index_constant1:  .quad 0x3F90000000000000
                      .quad 0x3F90000000000000
.L__index_constant2:  .quad 0x0000000000000040
                      .quad 0x0000000000000040
.L__plus_sixtyone_minus_two:  .quad 0x0000003D0000003D
                              .quad 0xFFFFFFFEFFFFFFFE
.L__real_half:        .quad 0x3fe0000000000000 # 1/2
                      .quad 0x3fe0000000000000

# Approximating polynomial coefficients for other x
.L__real_cb_3:       .quad 0x3f6249423bd94741 # 2.23219810758559851206e-03;
                     .quad 0x3f6249423bd94741
.L__real_cb_2:       .quad 0x3f89999999865ede # 1.24999999978138668903e-02,
                     .quad 0x3f89999999865ede
.L__real_cb_1:       .quad 0x3fb5555555555557 # 8.33333333333333593622e-02,
                     .quad 0x3fb5555555555557
.L__real_log2_lead:  .quad 0x3fe62e42e0000000 # log2_lead  6.93147122859954833984e-01
                     .quad 0x3fe62e42e0000000
.L__real_log2_tail:  .quad 0x3e6efa39ef35793c # log2_tail  5.76999904754328540596e-08
                     .quad 0x3e6efa39ef35793c
.L__plus_two:        .quad 0x4000000000000000
                     .quad 0x4000000000000000
# Approximating polynomial coefficients for x near 0.0
.L__real_ca_4:       .quad 0x3f3c8034c85dfff0 # 4.34887777707614552256e-04,
                     .quad 0x3f3c8034c85dfff0
.L__real_ca_3:       .quad 0x3f62492307f1519f # 2.23213998791944806202e-03,
                     .quad 0x3f62492307f1519f
.L__real_ca_2:       .quad 0x3f89999999bac6d4 # 1.25000000037717509602e-02,
                     .quad 0x3f89999999bac6d4
.L__real_ca_1:       .quad 0x3fb55555555554e6 # 8.33333333333317923934e-02,
                     .quad 0x3fb55555555554e6
.L__real_qnan:       .quad 0x7ff8000000000000   # qNaN
                     .quad 0x7ff8000000000000
.L__real_ninf:       .quad 0xfff0000000000000   # -inf
                     .quad 0xfff0000000000000
.L__sign_bit_64:     .quad 0x8000000000000000
                     .quad 0x8000000000000000

