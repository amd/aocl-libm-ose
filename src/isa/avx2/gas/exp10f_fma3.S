#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#include "fn_macros.h"
#include "exp_tables.h"
#define fname FN_PROTOTYPE_FMA3(exp10f)
#define fname_special _exp10f_special@PLT

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    vucomiss .L__max_exp10f_arg(%rip), %xmm0
    ja .L__y_is_inf
    jp .L__y_is_nan
    vucomiss .L__min_exp10f_arg(%rip), %xmm0
    jb .L__y_is_zero

    vcvtps2pd     %xmm0, %xmm2    #xmm2 = (double)x

    # x * (64/log10of(2))
    vmulsd       .L__real_64_by_log10of2(%rip), %xmm2,%xmm3  #xmm3 = x * (64/ln(2)

    # n = int( x * (64/log10of(2)) )
    vcvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n
    vcvtdq2pd    %xmm4, %xmm0  #xmm0 = (double)n

    # r = x - n * ln(2)/64
    # r *= ln(10)
    vfmadd132sd    .L__real_mlog10of2_by_64(%rip),%xmm2,%xmm0 #xmm0 = r    
    vmovd       %xmm4, %ecx     #ecx = n
    vmulsd      .L__real_ln10(%rip),%xmm0,%xmm1 #xmm1 = r = r*ln(2) 

    # q = r + r*r(1/2 + r*1/6)
    vmovhpd       .L__real_1_by_6(%rip), %xmm1,%xmm3  #%xmm3 =1/6,r 
    vmovlhps     %xmm3,%xmm1,%xmm1     #xmm1 = r,r        
    vfmadd213pd     .L__real_1_by_2_zero(%rip), %xmm1, %xmm3 #xmm3 = r*r,1/2 + (1/6 * r)
    vmovdqa %xmm3, %xmm5
    vmovhlps      %xmm5, %xmm1,%xmm3 #xmm3 = r,r*r
    vfmadd213sd     %xmm1,%xmm5,%xmm3 #xmm3 = q
    
    #j = n & 0x3f
    mov         $0x3f, %rax     #rax = 0x3f
    and         %ecx, %eax      #eax = j = n & 0x3f

    # f + (f*q)
    lea         .L__two_to_jby64_table(%rip), %r10    
    vmovsd       (%r10,%rax,8), %xmm2
    vfmadd231sd    %xmm2,%xmm3,%xmm2

    .p2align 4
    # m = (n - j) / 64        
    vpsrad       $6,%xmm4,%xmm1
    vpsllq       $52,%xmm1,%xmm4
    vpaddq      %xmm2, %xmm4,%xmm2
    vcvtpd2ps   %xmm2, %xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_zero:
    vmovd       %xmm0,%eax
    cmp         $0xff800000,%eax  
    je          .L__x_is_neg_inf
    vpxor       %xmm1,%xmm1,%xmm1    #return value in xmm1,input in xmm0 before calling
    mov         $2, %edi        #code in edi
    call        fname_special
    ret         
.L__x_is_neg_inf:
    vpxor       %xmm0,%xmm0,%xmm0
    ret  
    
.p2align 4
.L__y_is_inf:
    vmovd       %xmm0,%eax
    mov         $0x7f800000,%edx
    cmp         %edx,%eax
    je          .L__x_is_pos_inf
    vmovd       %edx, %xmm1
    mov         $3, %edi
    call        fname_special
.L__x_is_pos_inf:    
    ret     

.p2align 4
.L__y_is_nan:
    vaddss  %xmm0,%xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret       

.else

.p2align 4
.L__y_is_zero:
    vpxor       %xmm0, %xmm0,%xmm0
    ret         

.p2align 4
.L__y_is_inf:
    mov         $0x7f800000,%edx
    vmovd       %edx, %xmm0
    ret     

.p2align 4
.L__y_is_nan:
    vaddss  %xmm0,%xmm0,%xmm0
    ret 
    
.endif
     
.section .rodata
.align 16
.L__max_exp10f_arg:                 .long 0x421A209B
.L__min_exp10f_arg:                 .long 0xC23369F4
.L__real_64_by_log10of2:        .quad 0x406A934F0979A371 # 64/log10(2)
.L__real_mlog10of2_by_64:       .quad 0xbF734413509F79FF # log10of2_by_64
.L__real_ln10:                  .quad 0x40026BB1BBB55516 # ln(10)
.L__real_1_by_2_zero:           .octa 0x3fe00000000000000000000000000000

