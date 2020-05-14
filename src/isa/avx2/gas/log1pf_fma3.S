#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# log1pf_bdozr.S
#
# An implementation of the log1pf libm function.
#
# Prototype:
#
#     float log1pf(float x);
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
#define fname FN_PROTOTYPE_FMA3(log1pf)
#define fname_special _log1pf_special@PLT


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
    vmovd      .L__real_qnan(%rip),%xmm3
    vmovd      .L__real_ninf_32(%rip),%xmm5
    vmovd      .L__negative_one(%rip),%xmm6
    vpand      .L__sign_mask_32(%rip),%xmm0,%xmm1

    #get the mask for Nans and Infs
    vpand          .L__exp_mask(%rip),%xmm0,%xmm14  # remove the mantissa
    vucomiss        .L__exponent_compare(%rip),%xmm14 # check for nans and Infs
    je             .L__input_is_Nan_Inf

    vcomiss    .L__real_epsilon(%rip),%xmm1 #mask for <= epsilons
    jb         .L__input_is_less_than_epsilon

    vcomiss    %xmm6,%xmm0 #check for input less than -1.0
    jb         .L__input_less_than_minus_one
    
    #get the mask for exactly -1.0
    vcomiss     %xmm6,%xmm0
    je         .L__input_is_minus_one

############################################
    #calculate log1pf for lower half
    vmovapd        .L__one_mask_64(%rip),%xmm3
    vmovapd        .L__exp_mask_64(%rip),%xmm4
    vmovapd        .L__mantissa_mask(%rip),%xmm11
    vmovapd        .L__index_mask1(%rip),%xmm6
    vmovapd        .L__index_mask2(%rip),%xmm7
    vmovapd        .L__index_mask3(%rip),%xmm8
#vmovaps        .L__selector_constant(%rip),%xmm13

    vcvtss2sd      %xmm0,%xmm5,%xmm5
    vmovapd        %xmm5,%xmm14
    vaddsd         %xmm3,%xmm5,%xmm5 #xmm5 = 1.0 +x

    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    vpand          %xmm6,%xmm5,%xmm6
    vpand          %xmm7,%xmm5,%xmm7
    vpor           %xmm8,%xmm6,%xmm6
    vpsrlq         $45,%xmm7,%xmm7
    vpsrlq         $46,%xmm6,%xmm6
    vpaddd         %xmm6,%xmm7,%xmm6
    vcvtdq2pd      %xmm6,%xmm7 # it will always be a positive integer
    vmovapd        .L__index_constant1(%rip),%xmm8 # 
    
    vmovapd        .L__mask_1023_32bit(%rip),%xmm12
    vpand          %xmm4,%xmm5,%xmm4
    vpsrlq         $52,%xmm4,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    vpsubd         %xmm12,%xmm4,%xmm4
    vpand          %xmm11,%xmm5,%xmm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS_DP64, f);
    vpor           %xmm3,%xmm5,%xmm5 # f = ymm5
    vmulsd         %xmm8,%xmm7,%xmm7 # f1 =index*0.015625
    vpsubd         .L__index_constant2(%rip),%xmm6,%xmm6 # index -= 64
    
# xmm4=xexp
    vmovaps        .L__plus_sixtyone(%rip),%xmm9
    vmovaps        .L__minus_two(%rip),%xmm10
    vmovaps        .L__mask_1023(%rip),%xmm11
    #     IF( (xexp <= -2 ) || (xexp >= MANTLENGTH_DP64 + 8))
    vpcmpgtq       %xmm4,%xmm9,%xmm9   #( xmm9>xmm4) (61 > xexp)
    vpcmpgtq       %xmm4,%xmm10,%xmm10 #(xmm10>xmm4) (-2 > xexp)
    vpandn         %xmm9,%xmm10,%xmm9  # xmm9 stores the mask for all the numbers which lie between -2 and 61

    vmovaps        %xmm9,%xmm10
    vsubsd         %xmm7,%xmm5,%xmm5 # f2 = f - f1;
    #ELSE
    vpsubd         %xmm4,%xmm11,%xmm11
    
    #ux = (unsigned long long)(0x3ff - xexp) << EXPSHIFTBITS_DP64;
    #PUT_BITS_DP64(ux,m2);
    vpsllq         $52,%xmm11,%xmm11 # xmm11 = m2;
    vmulsd         %xmm14,%xmm11,%xmm13 # xmm13 = m2*x
    #ymm7=f1, ymm11=m2, ymm13 = m2*x
    vsubsd         %xmm7,%xmm11,%xmm11 # (m2 - f1)
    vaddsd         %xmm13,%xmm11,%xmm11 # xmm11 = f2 = (m2 - f1) + m2*dx

    # z1 = ln_lead_table[index];
    # q = ln_tail_table[index];
    lea            .L__ln_lead_64_table(%rip), %r9
    lea            .L__ln_tail_64_table(%rip), %r8
    
    vmovd          %xmm6,%r10d # move 1st order index
    vmovq          (%r9,%r10,8),%xmm6
    vmovq          (%r8,%r10,8),%xmm8

    vmovapd        .L__real_half(%rip),%xmm12
    vmovapd        .L__real_cb_1(%rip),%xmm13
    vmovapd        .L__real_cb_2(%rip),%xmm15
    #f2 = ymm11/ymm5 f1 = xmm7 
    #vpcmov  %xmm9,%xmm5,%xmm11,%xmm11
    vandnpd %xmm5, %xmm9, %xmm14
    vandpd %xmm9, %xmm11, %xmm11
    vorpd %xmm14, %xmm11, %xmm11  

    vmovapd        %xmm11,%xmm5
    
    vfmadd231sd       %xmm12,%xmm11,%xmm7#(f1 + 0.5 * f2);
    vdivsd         %xmm7,%xmm5,%xmm7  # u = f2 / (f1 + 0.5 * f2);
    vmulsd         %xmm7,%xmm7,%xmm5  # v = u*u

    vfmadd231sd       %xmm5,%xmm15,%xmm13
    vmulsd         %xmm5,%xmm13,%xmm5  # poly = v * (cb_1 + v * cb_2)
    vmovapd        .L__real_log2(%rip),%xmm13
    #poly = xmm5 u = xmm7 q = xmm8
    vfmadd213sd       %xmm7,%xmm7,%xmm5 # (u + u * poly)
    vaddsd         %xmm8,%xmm5,%xmm5   # z2 = q + (u + u * poly)
    #xmm5 = z2, xmm8 = q, xmm7 = u, xmm6 = z1 
    vcvtdq2pd      %xmm4,%xmm4 # xexp (float)
    
    vfmadd213sd       %xmm6,%xmm13,%xmm4
    vaddsd         %xmm5,%xmm4,%xmm4 # r = (xexp * log2 + z1 + z2)
    
    vcvtsd2ss      %xmm4,%xmm0,%xmm0 #Final result
    # xmm0 is the result of log1p  
    ret
##########################################
#########################################
##########################################


.L__input_is_Nan_Inf:    
    #now restore the Nans and Infs
    vmovd     %xmm0,%eax
    cmp       .L__real_inf(%rip),%eax
    je        .L__finish

    cmp       .L__real_ninf(%rip),%eax
    je        .L__input_less_than_minus_one
    
    or        .L__real_qnanbit(%rip),%eax
    movd      %eax,%xmm1
    mov       .L__flag_x_nan(%rip),%edi
    call      fname_special

.align 16
.L__finish:
    ret


#vpand          .L__sign_bit_32(%rip),%xmm0,%xmm3 # for negative infities we need to set the result as Qnan
#vaddss         %xmm0,%xmm0,%xmm0
    #so we get the sign bit and move it to the qnan Bit.
    #then OR it with Qnan/inf result
#vpsrld         $9,%xmm3,%xmm3
#vpor           %xmm3,%xmm0,%xmm0
#ret
    
    #process inputs less than 0x3380000033800000
.L__input_is_less_than_epsilon:    
    #result is equal to the input
    ret 

.L__input_less_than_minus_one:    
    #now restore if there are some inputs less than -1.0
    vmovaps     %xmm3,%xmm0
    vmovss      .L__real_neg_qnan(%rip),%xmm1
    mov         .L__flag_x_neg(%rip),%edi
    call        fname_special
    ret

.L__input_is_minus_one:
    #now restore if there are some inputs with -1.0
    vmovaps     %xmm5,%xmm0
    vmovss      .L__real_ninf(%rip),%xmm1
    mov         .L__flag_x_zero(%rip),%edi
    call        fname_special
    ret

   
    
.section .rodata
.align 16
# these codes and the ones in the corresponding .c file have to match
.L__flag_x_zero:         .long 00000001
.L__flag_x_neg:          .long 00000002
.L__flag_x_nan:          .long 00000003


.align 16
.L__real_neg_qnan:  .quad 0xffC00000ffC00000   # neg qNaN
                    .quad 0xffC00000ffC00000
.L__real_qnan:      .quad 0x7FC000007FC00000   # qNaN
                    .quad 0x7FC000007FC00000
.L__real_qnanbit:   .quad 0x0040000000400000   # quiet nan bit
                    .quad 0x0040000000400000
.L__real_ninf_32:   .quad 0xff800000ff800000   # -inf
                    .quad 0xff800000ff800000
.L__negative_one:   .quad 0xbf800000bf800000
                    .quad 0xbf800000bf800000
.L__sign_mask_32:   .quad 0x7FFFFFFF7FFFFFFF
                    .quad 0x7FFFFFFF7FFFFFFF
.L__real_epsilon:   .quad 0x3380000033800000
                    .quad 0x3380000033800000
.L__one_mask_64:    .quad 0x3fF0000000000000# 1
                    .quad 0x3fF0000000000000
.L__exp_mask_64:    .quad 0x7FF0000000000000#
                    .quad 0x7FF0000000000000
.L__mantissa_mask:  .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                    .quad 0x000FFFFFFFFFFFFF
.L__index_mask1:    .quad 0x000fc00000000000
                    .quad 0x000fc00000000000
.L__index_mask2:    .quad 0x0000200000000000
                    .quad 0x0000200000000000
.L__index_mask3:    .quad 0x0010000000000000
                    .quad 0x0010000000000000
.L__selector_constant: .quad 0x0000000300000001
                       .quad 0x0000000700000005
.L__index_constant1:.quad 0x3F90000000000000
                    .quad 0x3F90000000000000
.L__mask_1023_32bit: .quad 0x000003ff000003ff
                     .quad 0x000003ff000003ff
.L__index_constant2:.quad 0x0000004000000040
                    .quad 0x0000004000000040
.L__plus_sixtyone:  .quad 0x0000003D0000003D
                    .quad 0x0000003D0000003D
.L__minus_two:      .quad 0xFFFFFFFEFFFFFFFE
                    .quad 0xFFFFFFFEFFFFFFFE
.L__mask_1023:        .quad 0x00000000000003ff
                      .quad 0x00000000000003ff
.L__mask_127_32bit:  .quad 0x0000007f0000007f
                     .quad 0x0000007f0000007f
.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x3fe0000000000000
# Approximating polynomial coefficients for other x 
.L__real_cb_1:      .quad 0x3fb5555555555557 # 8.33333333333333593622e-02, 
                    .quad 0x3fb5555555555557
.L__real_cb_2:      .quad 0x3f89999999865ede # 1.24999999978138668903e-02,  
                    .quad 0x3f89999999865ede
.L__real_log2:      .quad 0x3fe62e42fefa39ef # log2 = 6.931471805599453e-01
                    .quad 0x3fe62e42fefa39ef 
.L__exp_mask:       .quad 0x7F8000007F800000   #
                    .quad 0x7F8000007F800000
.L__exponent_compare: .quad 0x7F8000007F800000  
                      .quad 0x7F8000007F800000
.L__sign_bit_32:    .quad 0x8000000080000000 
                    .quad 0x8000000080000000
.L__real_ninf:      .quad 0xff800000ff800000   # -inf
                    .quad 0xff800000ff800000
.L__real_inf:       .quad 0x7f8000007f800000   # +inf
                    .quad 0x7f8000007f800000



