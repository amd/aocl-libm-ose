#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# vrda_log1p.S
#
# An implementation of the vrda_log1p libm function.
#
# Prototype:
#
#     void vrda_log1p(int len,double *src,double *dst);
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
#define fname FN_PROTOTYPE_BAS64(vrda_log1p)
#define fname_special _vrda_log1p_special@PLT


# local variable storage offsets
.equ    p_temp, 0x0
.equ    stack_size, 0x18


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

# parameters are passed in by Linux as:
# rdi - int len 
# rsi - double *src
# rdx - double *dst

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
    and .L__all_bits_set(%rip),%rsi
    jz  .L__exit_vrda_log1p 
    and .L__all_bits_set(%rip),%rdx
    jz  .L__exit_vrda_log1p
    cmp $0,%edi
    jle  .L__exit_vrda_log1p

    xor %rcx,%rcx
    xor %rax,%rax
.align 32        
.L__vrda_top:
    #fill data in xmm0
    cmp $0,%rdi
    jz .L__exit_vrda_log1p
    pxor %xmm0,%xmm0
    cmp $1,%rdi
    jz  .L__fill_single
    movupd (%rsi,%rax,8),%xmm0 #is data pointer always 16 byte aligned?
    sub $2,%rdi
    add $2,%rax
    jmp .L__start_process_vrd2_log1p
.L__fill_single:
    movlpd (%rsi,%rax,8),%xmm0
    dec %rdi
    inc %rax
.L__start_process_vrd2_log1p:
    # compute exponent part
    movapd      %xmm0, %xmm1 
    movapd      %xmm0, %xmm2 #
    movapd      %xmm0, %xmm3 #
    movapd      %xmm0, %xmm4 #
    movapd      %xmm0, %xmm5 #
    movapd      %xmm0, %xmm14 #
    movapd      %xmm0, %xmm15 #
    pand        .L__sign_mask_64(%rip),%xmm1

    cmplepd     .L__real_epsilon(%rip),%xmm1 #mask for epsilons
    cmpltpd     .L__real_minus_one(%rip),%xmm2 #mask for input less than -1.0
    #get the mask for exactly -1.0
    cmpeqpd       .L__real_minus_one(%rip),%xmm15
    #get the mask for Nans and Infs
    pand        .L__exp_mask(%rip),%xmm14  # remove the mantissa
    pshufd      $0xF5,%xmm14,%xmm14 # [11 11 01 01 ] = $0xF
    pcmpeqd     .L__exponent_compare(%rip),%xmm14 # xmm14 stores the mask for nans and Infs

    
    #calculate log1p
    cmpnlepd    .L__real_log1p_thresh2(%rip), %xmm3
    cmpltpd     .L__real_log1p_thresh1(%rip), %xmm4
    por         %xmm4, %xmm3 # mask which stores (x < log1p_thresh1 || x > log1p_thresh2)

    #calculate outside the threshold range first
    addpd       .L__one_mask_64(%rip), %xmm5 #xmm5 = 1.0 +x
    movapd      %xmm5,%xmm4
    movapd      %xmm5,%xmm6
    movapd      %xmm5,%xmm7
    pand        .L__exp_mask(%rip),%xmm4
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
    pcmpgtd     %xmm8,%xmm9 
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
    mulpd       %xmm0,%xmm11 # xmm11 = m2*x
    #xmm7=f1, xmm13=m2, xmm11 = m2*x
    subpd       %xmm7,%xmm13 # (m2 - f1)
    addpd       %xmm11,%xmm13 # xmm13 = f2

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
    por         %xmm9,%xmm13 # xmm5 = f2
    movapd      %xmm13,%xmm5
    mulpd       .L__real_half(%rip),%xmm13 # (0.5 * f2)
    addpd       %xmm13,%xmm7 #  (f1 + 0.5 * f2);
    divpd       %xmm7,%xmm5  # u = f2 / (f1 + 0.5 * f2);
    movapd      %xmm5,%xmm7  # xmm7 = u
    mulpd       %xmm5,%xmm5  # v = u*u
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
    
    pshufd      $0xF8,%xmm4,%xmm4 # [11 11 10 00] = 0xF8
    cvtdq2pd    %xmm4,%xmm4 # xexp (float) 
    movapd      %xmm4,%xmm8 # xmm8 = xexp
    mulpd       .L__real_log2_lead(%rip), %xmm4
    addpd       %xmm6,%xmm4 # r1 = (xexp * log2_lead + z1)
    mulpd       .L__real_log2_tail(%rip), %xmm8
    addpd       %xmm5,%xmm8 # r2 = (xexp * log2_tail + z2)
    
    addpd       %xmm4,%xmm8 # return r1 + r2
    #END of calculating outside the threshold
    
    #calculate when the value lies within the threshold
    #xmm0 still contains the input x
    movapd     %xmm0,%xmm4 # %xmm4 = r1        
    movapd     %xmm0,%xmm5 # 
    movapd     %xmm0,%xmm6 # 
    movapd     %xmm0,%xmm7 # 
    addpd      .L__plus_two(%rip),%xmm5 # (2.0 + r)
    divpd      %xmm5,%xmm6 # xmm6 = u = r / (2.0 + r); 
    mulpd      %xmm6,%xmm7 # correction = r * u
    addpd      %xmm6,%xmm6 # u = u+ u
    movapd     %xmm6,%xmm9 
    mulpd      %xmm9,%xmm9 # v = u * u
    #v = xmm9, u = xmm6, correction = xmm7
    movapd     %xmm9,%xmm10 # xmm10 = v
    mulpd      .L__real_ca_4(%rip),%xmm10 # (v * ca_4)
    addpd      .L__real_ca_3(%rip),%xmm10 # (ca_3 + (v * ca_4))
    mulpd      %xmm9,%xmm10 # v * (ca_3 + v * ca_4)
    addpd      .L__real_ca_2(%rip),%xmm10 # (ca_2 + v * (ca_3 + v * ca_4))
    mulpd      %xmm9,%xmm10 # v * (ca_2 + v * (ca_3 + v * ca_4))
    addpd      .L__real_ca_1(%rip),%xmm10 # (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      %xmm9,%xmm10 # v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      %xmm6,%xmm10 # u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    subpd     %xmm7,%xmm10  # r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction) 
    addpd     %xmm4,%xmm10
    #END calculating within the threshold

    #now restore the proper results
    pand      %xmm3,%xmm8
    pandn     %xmm10,%xmm3
    por       %xmm8,%xmm3
    movapd    %xmm0,%xmm8 # copy the input

    #process inputs less than 0x3ca0000000000000
    pand      %xmm1, %xmm8 
    pandn     %xmm3,%xmm1
    por       %xmm8,%xmm1

    # now restore if there are some inputs less than -1.0
    movapd    %xmm2,%xmm8
    pand      .L__real_qnan(%rip), %xmm8
    pandn     %xmm1,%xmm2
    por       %xmm8,%xmm2
    # now restore if there are some inputs with -1.0
    movapd    %xmm15,%xmm8
    pandn     %xmm2,%xmm8
    pand      .L__real_ninf(%rip), %xmm15
    por       %xmm15,%xmm8 # first OR the infinities = -1.0
    
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
.L__end_process_vrd2_log1p:    
    #fill data back into memory buffer from the result xmm0
    movlpd %xmm0,(%rdx,%rcx,8)
    inc %rcx
    cmp %rcx,%rax
    jz .L__exit_vrda_log1p
    movhpd %xmm0, (%rdx,%rcx,8)
    inc %rcx
    jmp .L__vrda_top
.L__exit_vrda_log1p:
##########################################
    ret
    
    
    
.section .rodata

.align 16
.L__all_bits_set:     .quad 0xFFFFFFFFFFFFFFFF
                      .quad 0xFFFFFFFFFFFFFFFF
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

