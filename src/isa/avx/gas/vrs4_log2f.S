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

#
# vrs4_log2f.S
#
# An implementation of the vrs4_log2f libm function.
#
# Prototype:
#
#     __m128 vrs4_log2f(__m128 x);
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
#define fname ALM_PROTO_BAS64(vrs4_log2f)

#define fname_special _vrs4_log2f_special@PLT


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
    movaps      %xmm0, %xmm1 
    movaps      %xmm0, %xmm2 #
    movaps      %xmm0, %xmm3 #
    movaps      %xmm0, %xmm5 #
    movaps      %xmm0, %xmm15 #
    pand        .L__sign_bit_32(%rip),%xmm2
    pcmpeqd     .L__sign_bit_32(%rip),%xmm2 # stores the mask for -ve inputs

    pand        .L__exp_mask(%rip),%xmm1
    pcmpeqd     .L__exp_mask(%rip),%xmm1 #mask for Nans AN Infs

    #get the mask for exactly 0.0
    pand        .L__sign_mask_32(%rip),%xmm15
    pcmpeqd     .L__Zero_32(%rip),%xmm15
    
############################################    
    #calculate log2f for lower half
    cvtps2pd    %xmm5,%xmm5
    movapd      %xmm5,%xmm4
    movapd      %xmm5,%xmm6
    movapd      %xmm5,%xmm7
    pand        .L__exp_mask_64(%rip),%xmm4
    psrlq       $52,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       .L__mask_1023(%rip),%xmm4 # xmm4 = xexp
    pand        .L__mantissa_mask(%rip),%xmm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | HALFEXPBITS_DP64, f);
    por         .L__real_half(%rip),%xmm5 # xmm5 = f
     
    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    pand        .L__index_mask1(%rip),%xmm6
    pand        .L__index_mask2(%rip),%xmm7
    por         .L__index_mask3(%rip),%xmm6
    psrlq       $45,%xmm7 # ((ux & 0x0000200000000000) >> 45)
    psrlq       $46,%xmm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    paddq       %xmm7,%xmm6 # index = xmm6
    pshufd      $0xF8,%xmm6,%xmm7 # [11 11 10 00] = $0xF8
    cvtdq2pd    %xmm7,%xmm7
    mulpd       .L__index_constant1(%rip),%xmm7 # f1 =index*0.0078125
    psubq       .L__index_constant2(%rip),%xmm6 # index -= 64
    subpd       %xmm7,%xmm5 # f2 = f - f1;

    #  z1 = ln_lead_table[index];
    #  q = ln_tail_table[index];
    movd        %xmm6,%r10 # move lower order index
    psrldq      $8,%xmm6
    movd        %xmm6,%r11 # move higher order index
    lea         .L__ln_lead_64_table(%rip), %r9
    lea         .L__ln_tail_64_table(%rip), %r8
    movlpd      (%r9,%r10,8), %xmm6
    movhpd      (%r9,%r11,8), %xmm6 # xmm6 = z1 = ln_lead_table[index]
    movlpd      (%r8,%r10,8), %xmm8
    movhpd      (%r8,%r11,8), %xmm8 # xmm8 = q = ln_tail_table[index]

    #f2 = xmm5 f1 = xmm7 
    movapd      %xmm5,%xmm13
    mulpd       .L__real_half(%rip),%xmm13 # (0.5 * f2)
    addpd       %xmm13,%xmm7 #  (f1 + 0.5 * f2);
    divpd       %xmm7,%xmm5  # u = f2 / (f1 + 0.5 * f2);
    movapd      %xmm5,%xmm7
    mulpd       %xmm5,%xmm5  # v = u*u
    movapd      %xmm5,%xmm11 

    mulpd       .L__real_cb_2(%rip),%xmm5 # v*cb_2
    addpd       .L__real_cb_1(%rip),%xmm5 # cb_1 + v*cb_2
    mulpd       %xmm11,%xmm5  # poly = v * (cb_1 + v * cb_2)
    #poly = xmm5 u = xmm7 q = xmm8

    mulpd       %xmm7,%xmm5
    addpd       %xmm7,%xmm5
    addpd       %xmm8,%xmm5   # z2 = q + (u + u * poly)
   
    #xmm5 = z2, xmm8 = q, xmm7 = u, xmm6 = z1 
    pshufd      $0xF8,%xmm4,%xmm4 # [11 11 10 00] = 0xF8
    cvtdq2pd    %xmm4,%xmm4 # xexp (float)
    mulpd       .L__real_log2(%rip), %xmm4 # (xexp * log2)
    addpd       %xmm6,%xmm4 #   (xexp * log2 + z1)
    addpd       %xmm5,%xmm4 # r = (xexp * log2 + z1 + z2)
    mulpd       .L__real_log2e(%rip),%xmm4
    cvtpd2ps    %xmm4,%xmm8 
    pand        .L__mask_upperhalf(%rip),%xmm8
#################################
#now calculate for the upper half
    #calculate log2f for upper half
    psrldq      $8,%xmm3
    cvtps2pd    %xmm3,%xmm3
    movapd      %xmm3,%xmm4
    movapd      %xmm3,%xmm6
    movapd      %xmm3,%xmm7
    pand        .L__exp_mask_64(%rip),%xmm4
    psrlq       $52,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       .L__mask_1023(%rip),%xmm4 #
    pand        .L__mantissa_mask(%rip),%xmm3
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | HALFEXPBITS_DP64, f);
    por         .L__real_half(%rip),%xmm3 # xmm3 = f
     
    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    pand        .L__index_mask1(%rip),%xmm6
    pand        .L__index_mask2(%rip),%xmm7
    por         .L__index_mask3(%rip),%xmm6
    psrlq       $45,%xmm7 # ((ux & 0x0000200000000000) >> 45)
    psrlq       $46,%xmm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    paddq       %xmm7,%xmm6
    pshufd      $0xF8,%xmm6,%xmm7 # [11 11 10 00] = $0xF8
    cvtdq2pd    %xmm7,%xmm7
    mulpd       .L__index_constant1(%rip),%xmm7 # f1 =index*0.0078125
    psubq       .L__index_constant2(%rip),%xmm6 # index -= 64
    subpd       %xmm7,%xmm3 # f2 = f - f1;

    #  z1 = ln_lead_table[index];
    #  q = ln_tail_table[index];
    movd        %xmm6,%r10 # move lower order index
    psrldq      $8,%xmm6
    movd        %xmm6,%r11 # move higher order index
    lea         .L__ln_lead_64_table(%rip), %r9
    lea         .L__ln_tail_64_table(%rip), %r8
    movlpd      (%r9,%r10,8), %xmm6
    movhpd      (%r9,%r11,8), %xmm6 # xmm6 = z1 = ln_lead_table[index]
    movlpd      (%r8,%r10,8), %xmm5
    movhpd      (%r8,%r11,8), %xmm5 # xmm5 = q = ln_tail_table[index]
    #f2 = xmm3 f1 = xmm7 
    movapd      %xmm3,%xmm13
    mulpd       .L__real_half(%rip),%xmm13 # (0.5 * f2)
    addpd       %xmm13,%xmm7 #  (f1 + 0.5 * f2);
    divpd       %xmm7,%xmm3  # u = f2 / (f1 + 0.5 * f2);
    movapd      %xmm3,%xmm7
    mulpd       %xmm3,%xmm3  # v = u*u
    movapd      %xmm3,%xmm11 

    mulpd       .L__real_cb_2(%rip),%xmm3 # v*cb_2
    addpd       .L__real_cb_1(%rip),%xmm3 # cb_1 + v*cb_2
    mulpd       %xmm11,%xmm3  #poly =  v * (cb_2 + v * cb_3)
    
    #poly = xmm3 u = xmm7 q = xmm5
    mulpd       %xmm7,%xmm3 
    addpd       %xmm7,%xmm3
    addpd       %xmm5,%xmm3   # z2 = q + (u + u * poly)
    
    #xmm3 = z2, xmm5 = q, xmm7 = u, xmm6 = z1 
    pshufd      $0xF8,%xmm4,%xmm4 # [11 11 10 00] = 0xF8
    cvtdq2pd    %xmm4,%xmm4 # xexp (float) 
    mulpd       .L__real_log2(%rip), %xmm4 # (xexp * log2)
    addpd       %xmm6,%xmm4 # (xexp * log2 + z1)
    addpd       %xmm3,%xmm4 # r= (xexp * log2 + z1 + z2)
    mulpd       .L__real_log2e(%rip),%xmm4 # (log2e*r)
    
    cvtpd2ps    %xmm4,%xmm3 
    pslldq      $8,%xmm3
    por         %xmm3,%xmm8
#end of upper half calculation
#Now club the two results
    
#################################
    
    #restore results for -ve inputs 
    movaps    %xmm2,%xmm3
    pand      .L__real_qnan(%rip), %xmm3
    pandn     %xmm8,%xmm2
    por       %xmm3,%xmm2 #
    
    #now restore the Nans and Infs
    movaps    %xmm0,%xmm3
    addps     %xmm0,%xmm0
    pand      .L__sign_bit_32(%rip),%xmm3 # for negative infities we need to set the result as Qnan
    #so we get the sign bit and move it to the qnan Bit.
    #then OR it with Qnan/inf result
    psrld     $9,%xmm3
    pand      %xmm1,%xmm3
    pand      %xmm1,%xmm0
    por       %xmm3,%xmm0
    pandn     %xmm2,%xmm1
    por       %xmm1,%xmm0
    
    # now restore if there are some inputs with 0.0
    movaps    %xmm15,%xmm2
    pand      .L__real_ninf_32(%rip), %xmm15
    pandn     %xmm0,%xmm2
    por       %xmm15,%xmm2 # OR the infinities = -1.0
    movaps %xmm2,%xmm0
    
##########################################
    ret
    
    
    
.section .rodata

.align 16
.L__sign_bit_32:    .quad 0x8000000080000000 
                    .quad 0x8000000080000000
.L__exp_mask:       .quad 0x7F8000007F800000   #
                    .quad 0x7F8000007F800000
.L__sign_mask_32:   .quad 0x7FFFFFFF7FFFFFFF
                    .quad 0x7FFFFFFF7FFFFFFF
.L__Zero_32:        .quad 0x0000000000000000
                    .quad 0x0000000000000000
.L__exp_mask_64:    .quad 0x7FF0000000000000   #
                    .quad 0x7FF0000000000000
.L__mask_1023:      .quad 0x00000000000003ff
                    .quad 0x00000000000003ff
.L__mantissa_mask:  .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                    .quad 0x000FFFFFFFFFFFFF
.L__index_mask1:    .quad 0x000fc00000000000
                    .quad 0x000fc00000000000
.L__index_mask2:    .quad 0x0000200000000000
                    .quad 0x0000200000000000
.L__index_mask3:    .quad 0x0010000000000000
                    .quad 0x0010000000000000
.L__index_constant1:.quad 0x3F80000000000000
                    .quad 0x3F80000000000000
.L__index_constant2:.quad 0x0000000000000040
                    .quad 0x0000000000000040
.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x3fe0000000000000

# Approximating polynomial coefficients for other x 
.L__real_cb_2:      .quad 0x3f89999999865ede # 1.24999999978138668903e-02,  
                    .quad 0x3f89999999865ede
.L__real_cb_1:      .quad 0x3fb5555555555557 # 8.33333333333333593622e-02, 
                    .quad 0x3fb5555555555557
.L__real_log2:      .quad 0x3fe62e42fefa39ef # log2 = 6.931471805599453e-01
                    .quad 0x3fe62e42fefa39ef 
.L__real_log2e:     .quad 0x3ff71547652b82fe# log2e = 1.44269504088896340735e+00
                    .quad 0x3ff71547652b82fe

.L__mask_upperhalf: .quad 0xFFFFFFFFFFFFFFFF
                    .quad 0x0000000000000000
.L__real_ninf_32:   .quad 0xff800000ff800000   # -inf
                    .quad 0xff800000ff800000
.L__real_qnan:      .quad 0x7FC000007FC00000   # qNaN
                    .quad 0x7FC000007FC00000

