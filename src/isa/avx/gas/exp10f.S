#include "fn_macros.h"
#include "exp_tables.h"
#define fname FN_PROTOTYPE_BAS64(exp10f)
#define fname_special _exp10f_special@PLT


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    ucomiss .L__max_exp10f_arg(%rip), %xmm0
    ja .L__y_is_inf
    jp .L__y_is_nan
    ucomiss .L__min_exp10f_arg(%rip), %xmm0
    jb .L__y_is_zero

    cvtps2pd     %xmm0, %xmm0    #xmm0 = (double)x

    # x * (64/log10of(2))
    movapd      %xmm0,%xmm3      #xmm3 = (xouble)x
    mulsd       .L__real_64_by_log10of2(%rip), %xmm3  #xmm3 = x * (64/ln(2)

    # n = int( x * (64/log10of(2)) )
    cvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n
    cvtdq2pd    %xmm4, %xmm2  #xmm2 = (double)n

    # r = x - n * ln(2)/64
    # r *= ln(10)
    mulsd       .L__real_log10of2_by_64(%rip),%xmm2 #xmm2 = n * log10of(2)/64
    movd        %xmm4, %ecx     #ecx = n
    subsd       %xmm2, %xmm0    #xmm0 = r
    mulsd       .L__real_ln10(%rip),%xmm0 #xmm0 = r = r*ln10
    movapd      %xmm0, %xmm1    #xmm1 = r

    # q = r + r*r(1/2 + r*1/6)
    movapd       .L__real_1_by_6(%rip), %xmm3 
    mulsd       %xmm0, %xmm3 #xmm3 = 1/6 * r
    mulsd       %xmm1, %xmm0 #xmm0 =  r  * r
    addsd       .L__real_1_by_2(%rip), %xmm3 #xmm3 = 1/2 + (1/6 * r)
    mulsd       %xmm3, %xmm0  #xmm0 = r*r*(1/2 + (1/6 * r))
    addsd       %xmm1, %xmm0  #xmm0 = r+r*r*(1/2 + (1/6 * r))
    
    #j = n & 0x3f
    mov         $0x3f, %rax     #rax = 0x3f
    and         %ecx, %eax      #eax = j = n & 0x3f

    # f + (f*q)
    lea         .L__two_to_jby64_table(%rip), %r10    
    mulsd       (%r10,%rax,8), %xmm0
    addsd       (%r10,%rax,8), %xmm0

    .p2align 4
    # m = (n - j) / 64        
    psrad       $6,%xmm4
    psllq       $52,%xmm4
    paddq       %xmm0, %xmm4
    cvtpd2ps    %xmm4, %xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_zero:
    movd        %xmm0,%eax
    cmp         $0xff800000,%eax  
    je          .L__x_is_neg_inf
    pxor        %xmm1, %xmm1    #return value in xmm1,input in xmm0 before calling
    mov         $2, %edi        #code in edi
    call        fname_special
    ret         
.L__x_is_neg_inf:
    pxor        %xmm0, %xmm0
    ret  
    
.p2align 4
.L__y_is_inf:
    movd        %xmm0,%eax
    mov         $0x7f800000,%edx
    cmp         %edx,%eax
    je          .L__x_is_pos_inf
    movd        %edx, %xmm1
    mov         $3, %edi
    call        fname_special
.L__x_is_pos_inf:    
    ret     

.p2align 4
.L__y_is_nan:
    movaps %xmm0,%xmm1
    addss  %xmm1,%xmm1
    mov         $1, %edi
    call        fname_special
    ret        

.else

.p2align 4
.L__y_is_zero:
    pxor        %xmm0, %xmm0
    ret         

.p2align 4
.L__y_is_inf:
    mov         $0x7f800000,%edx
    movd        %edx, %xmm0
    ret     

.p2align 4
.L__y_is_nan:
    addss  %xmm0,%xmm0
    ret 
    
.endif    
.section .rodata
.align 16
.L__max_exp10f_arg:                 .long 0x421A209B
.L__min_exp10f_arg:                 .long 0xC23369F4
.L__real_64_by_log10of2:        .quad 0x406A934F0979A371 # 64/log10(2)
.L__real_log10of2_by_64:        .quad 0x3F734413509F79FF # log10of2_by_64
.L__real_ln10:                  .quad 0x40026BB1BBB55516 # ln(10)
