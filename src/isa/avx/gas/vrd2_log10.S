#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# vrd2_log10.S
#
# An implementation of the vrd2_log10 libm function.
#
# Prototype:
#
#     __m128d vrd2_log10(__m128d x);
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
#define fname FN_PROTOTYPE_BAS64(vrd2_log10)

#define fname_special _vrd2_log10_special@PLT


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
    movapd      %xmm0, %xmm1 
    movapd      %xmm0, %xmm2 #
    movapd      %xmm0, %xmm3 #
    movapd      %xmm0, %xmm4 #
    movapd      %xmm0, %xmm5 #
    movapd      %xmm0, %xmm14 #
    #get the mask for negative inputs
    pand        .L__sign_bit_64(%rip),%xmm1 
    pshufd      $0xF5,%xmm1,%xmm1 # [11 11 01 01 ] = $0xF5
    cmpeqpd      .L__sign_bit_32(%rip),%xmm1 # xmm1 stors the mask for negative inputs

    #get the mask for Nans and Infs
    pand        .L__exp_mask(%rip),%xmm14  # remove the mantissa
    pshufd      $0xF5,%xmm14,%xmm14 # [11 11 01 01 ] = $0xF
    pcmpeqd     .L__exponent_compare(%rip),%xmm14 # xmm14 stores the mask for nans and Infs

    
    #get the mask for +/- zero
    pand        .L__sign_mask_64(%rip),%xmm2 
    cmpeqpd     .L__Zero_64(%rip),%xmm2 # xmm2 stores the mask for +/- zeros 
#################clear till here#####################

    #calculate log10
    cmpnltpd     .L__real_log_thresh1(%rip), %xmm4
    cmplepd      .L__real_log_thresh2(%rip), %xmm3
    pand         %xmm4, %xmm3 # xmm3 stores mask for (ux >= log_thresh1 && ux <= log_thresh2)

    #calculate outside the threshold range first
    
    #check for IMPBIT_DP_64 and calulate f
    movapd      %xmm5,%xmm4 # copy input x
    cmpltpd     .L__impbit_dp64(%rip),%xmm5 # mask for (ux < IMPBIT_DP64)
    movapd      %xmm5,%xmm8 # copy mask
    por         .L__constant1(%rip), %xmm4 # PUT_BITS_DP64(ux | 0x03d0000000000000, f)
    subpd       .L__constant1(%rip),%xmm4 # f -= corr
    pand        .L__expadjust(%rip),%xmm8 # xmm8 = expadjust
    movapd      %xmm0,%xmm7 # copy input
    # f = xmm4/xmm7 and the mask is xmm5
    pand        %xmm5,%xmm4
    pandn       %xmm7,%xmm5
    por         %xmm4,%xmm5
    # Now f = ux = xmm5 expadjust = xmm8
    
    movapd      %xmm5,%xmm6
    movapd      %xmm5,%xmm4
    movapd      %xmm5,%xmm7
    pand        .L__exp_mask(%rip),%xmm4
    psrlq       $52,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    psubq       .L__mask_1023(%rip),%xmm4
    psubq       %xmm8,%xmm4 # subtract expadjust

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
    subpd       %xmm7,%xmm5 # xmm5 = f2 = f - f1;
    
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
    # z1 = xmm6 q = xmm8 f2 = xmm5 f1 = xmm7 

    movapd      %xmm5,%xmm13
    mulpd       .L__real_half(%rip),%xmm13 # (0.5 * f2)
    addpd       %xmm13,%xmm7 #  (f1 + 0.5 * f2);
    divpd       %xmm7,%xmm5  # u = f2 / (f1 + 0.5 * f2);
    movapd      %xmm5,%xmm7  # xmm7 = u
    mulpd       %xmm5,%xmm5  # v = u*u
    
    #z1 = xmm6 q = xmm8 u = xmm7 v = %xmm5
    movapd      %xmm5,%xmm11 # v 
    mulpd       .L__real_cb_3(%rip),%xmm5 # v*cb_3
    addpd       .L__real_cb_2(%rip),%xmm5 # cb_2 + v*cb_3
    mulpd       %xmm11,%xmm5  # v * (cb_2 + v * cb_3)
    addpd       .L__real_cb_1(%rip),%xmm5 # (cb_1 + v * (cb_2 + v * cb_3))
    mulpd       %xmm11,%xmm5 # poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    #poly = xmm5 u = xmm7 q = xmm8
    mulpd       %xmm7,%xmm5 
    addpd       %xmm7,%xmm5
    addpd       %xmm8,%xmm5   # z2 = q + (u + u * poly)
    #######till here common to log2,log10,log
    pshufd      $0xF8,%xmm4,%xmm4 # [11 11 10 00] = 0xF8
    cvtdq2pd    %xmm4,%xmm4 # xexp (float) 
    movapd      %xmm4,%xmm8 # xmm8 = xexp
    mulpd       .L__real_log2_lead(%rip), %xmm4
    addpd       %xmm6,%xmm4 # r1 = (xexp * log2_lead + z1)
    mulpd       .L__real_log2_tail(%rip), %xmm8
    addpd       %xmm5,%xmm8 # r2 = (xexp * log2_tail + z2)
    #xmm8 = r2 xmm4 = r1 xmm6 =z1 xmm5 = z2
    movapd      %xmm4,%xmm9 # copy r1
    movapd      %xmm8,%xmm10 # copy r2
    mulpd       .L__real_log10e_lead(%rip),%xmm9 # (log10e_lead*r1)
    mulpd       .L__real_log10e_lead(%rip),%xmm10 #(log10e_lead*r2)
    mulpd       .L__real_log10e_tail(%rip),%xmm4 # (log10e_tail*r1)
    mulpd       .L__real_log10e_tail(%rip),%xmm8 # (log10e_tail*r2)
    addpd       %xmm4,%xmm8  # (log10e_tail*r1) + (log10e_tail*r2)
    addpd       %xmm10,%xmm8 # (log10e_lead*r1) + (log10e_lead*r2)
    addpd       %xmm9,%xmm8 # result = (((log10e_tail*r2) + log10e_tail*r1) + log10e_lead*r2) + log10e_lead*r1;
    ####END of calculating outside the threshold
    
    ####Now calculate when the value lies within the threshold
    #xmm0 still contains the input x
    movapd     %xmm0,%xmm4 # %xmm4 = r1 
    subpd      .L__one_mask_64(%rip), %xmm4 # r = x - 1.0
    movapd     %xmm4,%xmm5 # 
    movapd     %xmm4,%xmm6 # 
    movapd     %xmm4,%xmm7 # 
    addpd      .L__plus_two(%rip),%xmm5 # (2.0 + r)
    divpd      %xmm5,%xmm6 # xmm6 = u = r / (2.0 + r); 
    mulpd      %xmm6,%xmm7 # correction = r * u
    addpd      %xmm6,%xmm6 # u = u+ u
    movapd     %xmm6,%xmm9 
    mulpd      %xmm9,%xmm9 # v = u * u
    
    #r1 = xmm4 v = xmm9, u = xmm6, correction = xmm7
    movapd     %xmm9,%xmm10 # xmm10 = v
    mulpd      .L__real_ca_4(%rip),%xmm10 # (v * ca_4)
    addpd      .L__real_ca_3(%rip),%xmm10 # (ca_3 + (v * ca_4))
    mulpd      %xmm9,%xmm10 # v * (ca_3 + v * ca_4)
    addpd      .L__real_ca_2(%rip),%xmm10 # (ca_2 + v * (ca_3 + v * ca_4))
    mulpd      %xmm9,%xmm10 # v * (ca_2 + v * (ca_3 + v * ca_4))
    addpd      .L__real_ca_1(%rip),%xmm10 # (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      %xmm9,%xmm10 # v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      %xmm6,%xmm10 # u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    subpd      %xmm7,%xmm10  # r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction)
    ##### till here common to all log,log10,log2
    # r1 = xmm4 r2 = xmm10;
    movapd    %xmm4,%xmm7 # xmm7 = r
    pand      .L__mask32bits_DP64(%rip),%xmm4 # r1 = r1 & 0xffffffff00000000
    subpd     %xmm4,%xmm7 # (r-r1)
    addpd     %xmm7, %xmm10 # r2 = r2 + (r - r1)
    #r1 = xmm4 r2 = xmm10
    ######
    movapd      %xmm4,%xmm9 # copy r1
    movapd      %xmm10,%xmm11 # copy r2
    mulpd       .L__real_log10e_lead(%rip),%xmm9 # (log10e_lead*r1)
    mulpd       .L__real_log10e_lead(%rip),%xmm11 #(log10e_lead*r2)
    mulpd       .L__real_log10e_tail(%rip),%xmm4 # (log10e_tail*r1)
    mulpd       .L__real_log10e_tail(%rip),%xmm10 # (log10e_tail*r2)
    addpd       %xmm4,%xmm10  # (log10e_tail*r1) + (log10e_tail*r2)
    addpd       %xmm11,%xmm10 # (log10e_lead*r1) + (log10e_lead*r2)
    addpd       %xmm9,%xmm10 # result = (((log10e_tail*r2) + log10e_tail*r1) + log10e_lead*r2) + log10e_lead*r1;
    
    ######
    #END calculating within the threshold

    #now restore the proper results
    pand      %xmm3,%xmm10
    pandn     %xmm8,%xmm3
    por       %xmm10,%xmm3 # xmm3 stores the result


    movapd    %xmm0,%xmm8 # copy the input

    # now restore the result if input is less than 0.0
    movapd    %xmm1,%xmm8
    pand      .L__real_qnan(%rip), %xmm8
    pandn     %xmm3,%xmm1
    por       %xmm8,%xmm1



    # now restore if there are some inputs with -1.0
    movapd    %xmm2,%xmm8
    pandn     %xmm1,%xmm8
    pand      .L__real_ninf(%rip), %xmm2
    por       %xmm2,%xmm8 # first OR the infinities = -1.0
    
    # now restore the Nans and Infs
    movapd    %xmm0,%xmm3
    addpd     %xmm0,%xmm0
    pand      .L__sign_bit_64(%rip),%xmm3 # for negative infities we need to set the result as Qnan
    #so we get the sign bit and move it to the qnan Bit.
    # then OR it with Qnan/inf result
    psrlq     $12,%xmm3
    pand      %xmm14,%xmm3
    pand      %xmm14,%xmm0
    por       %xmm3,%xmm0
    pandn     %xmm8,%xmm14
    por       %xmm14,%xmm0
    
##########################################
    ret
    
    
    
.section .rodata

.align 16
.L__sign_bit_64:      .quad 0x8000000000000000 
                      .quad 0x8000000000000000
.L__sign_bit_32:      .quad 0x8000000080000000 
                      .quad 0x8000000080000000
.L__exp_mask:         .quad 0x7ff0000000000000   #
                      .quad 0x7ff0000000000000
.L__exponent_compare: .quad 0x7FF000007FF00000  
                      .quad 0x7FF000007FF00000
.L__sign_mask_64:     .quad 0x7FFFFFFFFFFFFFFF
                      .quad 0x7FFFFFFFFFFFFFFF
.L__Zero_64:          .quad 0x0000000000000000
                      .quad 0x0000000000000000

# The values exp(-1/16)-1 and exp(1/16)-1 */
.L__real_log_thresh1: .quad 0x3fee0faa00000000 # 0.9394121170043945, 
                      .quad 0x3fee0faa00000000
.L__real_log_thresh2: .quad 0x3ff1082c00000000#  1.0644950866699219;  
                      .quad 0x3ff1082c00000000
.L__impbit_dp64:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
.L__constant1:        .quad 0x03d0000000000000
                      .quad 0x03d0000000000000
.L__expadjust:        .quad 0x000000000000003C
                      .quad 0x000000000000003C
.L__mask_1023:        .quad 0x00000000000003ff
                      .quad 0x00000000000003ff
.L__mantissa_mask:    .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                      .quad 0x000FFFFFFFFFFFFF
.L__real_half:        .quad 0x3fe0000000000000 # 1/2
                      .quad 0x3fe0000000000000
.L__index_mask1:      .quad 0x000fc00000000000
                      .quad 0x000fc00000000000
.L__index_mask2:      .quad 0x0000200000000000
                      .quad 0x0000200000000000
.L__index_mask3:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
.L__index_constant1:  .quad 0x3F80000000000000
                      .quad 0x3F80000000000000
.L__index_constant2:  .quad 0x0000000000000040
                      .quad 0x0000000000000040

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
.L__real_log10e_lead:.quad 0x3fdbcb7800000000 # 4.34293746948242187500e-01, 
                     .quad 0x3fdbcb7800000000 
.L__real_log10e_tail:.quad 0x3ea8a93728719535 # 7.3495500964015109100644e-7
                     .quad 0x3ea8a93728719535
.L__one_mask_64:     .quad 0x3ff0000000000000 # 1
                     .quad 0x3ff0000000000000
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

.L__mask32bits_DP64: .quad 0xffffffff00000000
                     .quad 0xffffffff00000000
.L__real_qnan:       .quad 0x7ff8000000000000   # qNaN
                     .quad 0x7ff8000000000000
.L__real_ninf:       .quad 0xfff0000000000000   # -inf
                     .quad 0xfff0000000000000




