#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# vrs4_log1pf.S
#
# An implementation of the vrs4_log1pf libm function.
#
# Prototype:
#
#     __m128 vrs4_log1pf(__m128 x);
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
#define fname FN_PROTOTYPE_BAS64(vrs4_log1pf)

#define fname_special _vrs4_log1pf_special@PLT


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
.type fname,@function
fname:

    # compute exponent part
    movaps      %xmm0, %xmm1 
    movaps      %xmm0, %xmm2 #
    movaps      %xmm0, %xmm3 #
    movaps      %xmm0, %xmm4 #
    movaps      %xmm0, %xmm5 #
    movaps      %xmm0, %xmm15 #
    pand        .L__sign_mask_32(%rip),%xmm1

    cmpleps     .L__real_epsilon(%rip),%xmm1 #mask for epsilons
    cmpltps     .L__negative_one(%rip),%xmm2 #mask for input less than -1.0
    #get the mask for exactly -1.0
    cmpeqps     .L__negative_one(%rip),%xmm15

############################################    
    #calculate log1pf for lower half
    cvtps2pd    %xmm5,%xmm5
    movapd      %xmm5,%xmm14
    addpd       .L__one_mask_64(%rip), %xmm5 #xmm5 = 1.0 +x
    movapd      %xmm5,%xmm4
    movapd      %xmm5,%xmm6
    movapd      %xmm5,%xmm7
    pand        .L__exp_mask_64(%rip),%xmm4
    psrlq       $52,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       .L__mask_1023(%rip),%xmm4
    pand        .L__mantissa_mask(%rip),%xmm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS_DP64, f);
    por         .L__one_mask_64(%rip),%xmm5
     
    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    pand        .L__index_mask1(%rip),%xmm6
    pand        .L__index_mask2(%rip),%xmm7
    por         .L__index_mask3(%rip),%xmm6
    psrlq       $45,%xmm7 # ((ux & 0x0000200000000000) >> 45)
    psrlq       $46,%xmm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    paddq       %xmm7,%xmm6
    pshufd      $0xF8,%xmm6,%xmm7 # [11 11 10 00] = $0xF8
    cvtdq2pd    %xmm7,%xmm7
    mulpd       .L__index_constant1(%rip),%xmm7 # f1 =index*0.015625
    psubq       .L__index_constant2(%rip),%xmm6 # index -= 64
    movapd      %xmm4,%xmm8 # xmm4=xexp=xmm8 
    pshufd      $0x88,%xmm8,%xmm8 # [10 00 10 00] = 0x88
    movapd      .L__plus_sixtyone_minus_two(%rip),%xmm9
    pcmpgtd     %xmm8,%xmm9   # xmm9 > xmm8 (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    movapd      %xmm9,%xmm10
    psrldq      $8,%xmm10
    #IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    pandn       %xmm9,%xmm10 # xmm9 stores the mask for all the numbers which lie between -2 and 61
    pshufd      $0x50, %xmm10,%xmm9 # [01 01 00 00] = 0x50
    subpd       %xmm7,%xmm5 # f2 = f - f1;
    #ELSE
    movapd      .L__mask_1023(%rip),%xmm11
    psubq        %xmm4,%xmm11
    
    #ux = (unsigned long long)(0x3ff - xexp) << EXPSHIFTBITS_DP64;
    #PUT_BITS_DP64(ux,m2);
    psllq       $52,%xmm11 # xmm11 = m2;
    movapd      %xmm11,%xmm13 
    mulpd       %xmm14,%xmm11 # xmm11 = m2*x
    #xmm7=f1, xmm13=m2, xmm11 = m2*x
    subpd       %xmm7,%xmm13 # (m2 - f1)
    addpd       %xmm11,%xmm13 # xmm13 = f2 = (m2 - f1) + m2*dx

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
    #f2 = xmm13/xmm5 f1 = xmm7 
    pand        %xmm9,%xmm13 
    pandn       %xmm5,%xmm9
    por         %xmm9,%xmm13 # xmm13 = f2
    movapd      %xmm13,%xmm5
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
    
    cvtpd2ps    %xmm4,%xmm8 
    pand        .L__mask_upperhalf(%rip),%xmm8
#################################
#now calculate for the upper half
    #calculate log1pf for lower half
    psrldq      $8,%xmm3
    cvtps2pd    %xmm3,%xmm3
    movapd      %xmm3,%xmm14
    addpd       .L__one_mask_64(%rip), %xmm3 #xmm3 = 1.0 +x
    movapd      %xmm3,%xmm4
    movapd      %xmm3,%xmm6
    movapd      %xmm3,%xmm7
    pand        .L__exp_mask_64(%rip),%xmm4
    psrlq       $52,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       .L__mask_1023(%rip),%xmm4
    pand        .L__mantissa_mask(%rip),%xmm3
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS_DP64, f);
    por         .L__one_mask_64(%rip),%xmm3
     
    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    pand        .L__index_mask1(%rip),%xmm6
    pand        .L__index_mask2(%rip),%xmm7
    por         .L__index_mask3(%rip),%xmm6
    psrlq       $45,%xmm7 # ((ux & 0x0000200000000000) >> 45)
    psrlq       $46,%xmm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    paddq       %xmm7,%xmm6
    pshufd      $0xF8,%xmm6,%xmm7 # [11 11 10 00] = $0xF8
    cvtdq2pd    %xmm7,%xmm7
    mulpd       .L__index_constant1(%rip),%xmm7 # f1 =index*0.015625
    psubq       .L__index_constant2(%rip),%xmm6 # index -= 64
    movapd      %xmm4,%xmm5 # xmm4=xexp=xmm5 
    pshufd      $0x88,%xmm5,%xmm5 # [10 00 10 00] = 0x88
    movapd      .L__plus_sixtyone_minus_two(%rip),%xmm9
    pcmpgtd     %xmm5,%xmm9   # xmm9 > xmm5 (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    movapd      %xmm9,%xmm10
    psrldq      $8,%xmm10
    #IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    pandn       %xmm9,%xmm10 # xmm9 stores the mask for all the numbers which lie between -2 and 61
    pshufd      $0x50, %xmm10,%xmm9 # [01 01 00 00] = 0x50
    subpd       %xmm7,%xmm3 # f2 = f - f1;
    #ELSE
    movapd      .L__mask_1023(%rip),%xmm11
    psubq        %xmm4,%xmm11
    
    #ux = (unsigned long long)(0x3ff - xexp) << EXPSHIFTBITS_DP64;
    #PUT_BITS_DP64(ux,m2);
    psllq       $52,%xmm11 # xmm11 = m2;
    movapd      %xmm11,%xmm13
    mulpd       %xmm14,%xmm11 # xmm11 = m2*x
    #xmm7=f1, xmm13=m2, xmm11 = m2*x
    subpd       %xmm7,%xmm13 # (m2 - f1)
    addpd       %xmm11,%xmm13 # xmm13 = f2 = (m2 - f1) + m2*dx

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
    #f2 = xmm13/xmm3 f1 = xmm7 
    pand        %xmm9,%xmm13 
    pandn       %xmm3,%xmm9
    por         %xmm9,%xmm13 # xmm13 = f2
    movapd      %xmm13,%xmm3
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
    
    cvtpd2ps    %xmm4,%xmm3 
    pslldq      $8,%xmm3
    por         %xmm3,%xmm8
#end of upper half calculation
#Now club the two results
    
#################################
    
    #process inputs less than 0x3380000033800000
    movaps    %xmm0,%xmm3
    pand      %xmm1,%xmm3 
    pandn     %xmm8,%xmm1
    por       %xmm3,%xmm1
    
    #now restore if there are some inputs less than -1.0
    movapd    %xmm2,%xmm8
    pand      .L__real_qnan(%rip), %xmm8
    pandn     %xmm1,%xmm2
    por       %xmm8,%xmm2 # then OR the Qnans < -1.0
    
    movaps    %xmm0,%xmm14
    #get the mask for Nans and Infs
    pand        .L__exp_mask(%rip),%xmm14  # remove the mantissa
    pcmpeqd     .L__exponent_compare(%rip),%xmm14 # xmm14 stores the mask for nans and Infs


    #now restore the Nans and Infs
    movaps    %xmm0,%xmm3
    addps     %xmm0,%xmm0
    pand      .L__sign_bit_32(%rip),%xmm3 # for negative infities we need to set the result as Qnan
    #so we get the sign bit and move it to the qnan Bit.
    #then OR it with Qnan/inf result
    psrld     $9,%xmm3
    pand      %xmm14,%xmm3
    pand      %xmm14,%xmm0
    por       %xmm3,%xmm0
    pandn     %xmm2,%xmm14
    por       %xmm14,%xmm0
    
    # now restore if there are some inputs with -1.0
    movaps    %xmm15,%xmm2
    pand      .L__real_ninf_32(%rip), %xmm15
    pandn     %xmm0,%xmm2
    por       %xmm15,%xmm2 # OR the infinities = -1.0
    movaps %xmm2,%xmm0
    
##########################################
    ret
    
    
    
.section .rodata

.align 16
.L__sign_mask_32:   .quad 0x7FFFFFFF7FFFFFFF
                    .quad 0x7FFFFFFF7FFFFFFF
.L__real_epsilon:   .quad 0x3380000033800000
                    .quad 0x3380000033800000
.L__negative_one:   .quad 0xbf800000bf800000
                    .quad 0xbf800000bf800000
.L__one_mask_64:    .quad 0x3fF0000000000000# 1
                    .quad 0x3fF0000000000000
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
.L__index_constant1:.quad 0x3F90000000000000
                    .quad 0x3F90000000000000
.L__index_constant2:.quad 0x0000000000000040
                    .quad 0x0000000000000040
.L__plus_sixtyone_minus_two:  .quad 0x0000003D0000003D
                              .quad 0xFFFFFFFEFFFFFFFE
.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x3fe0000000000000
# Approximating polynomial coefficients for other x 
.L__real_cb_2:      .quad 0x3f89999999865ede # 1.24999999978138668903e-02,  
                    .quad 0x3f89999999865ede
.L__real_cb_1:      .quad 0x3fb5555555555557 # 8.33333333333333593622e-02, 
                    .quad 0x3fb5555555555557
.L__real_log2:      .quad 0x3fe62e42fefa39ef # log2 = 6.931471805599453e-01
                    .quad 0x3fe62e42fefa39ef 
.L__mask_upperhalf: .quad 0xFFFFFFFFFFFFFFFF
                    .quad 0x0000000000000000
.L__real_qnan:      .quad 0x7FC000007FC00000   # qNaN
                    .quad 0x7FC000007FC00000
.L__exp_mask:       .quad 0x7F8000007F800000   #
                    .quad 0x7F8000007F800000
.L__exponent_compare: .quad 0x7F8000007F800000  
                      .quad 0x7F8000007F800000
.L__sign_bit_32:    .quad 0x8000000080000000 
                    .quad 0x8000000080000000
.L__real_ninf_32:   .quad 0xff800000ff800000   # -inf
                    .quad 0xff800000ff800000



