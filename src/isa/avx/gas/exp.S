#
# exp.S
#
# An implementation of the exp libm function.
#
# Prototype:
#
#     double exp(double x);
#

#
#   Algorithm:
#   
#   e^x = 2^(x/ln(2)) = 2^(x*(64/ln(2))/64)
#
#   x*(64/ln(2)) = n + f, |f| <= 0.5, n is integer
#   n = 64*m + j,   0 <= j < 64
#   
#   e^x = 2^((64*m + j + f)/64)
#       = (2^m) * (2^(j/64)) * 2^(f/64)
#       = (2^m) * (2^(j/64)) * e^(f*(ln(2)/64))
#
#   f = x*(64/ln(2)) - n
#   r = f*(ln(2)/64) = x - n*(ln(2)/64)
#
#   e^x = (2^m) * (2^(j/64)) * e^r
#
#   (2^(j/64)) is precomputed
#
#   e^r = 1 + r + (r^2)/2! + (r^3)/3! + (r^4)/4! + (r^5)/5! + (r^5)/5!
#   e^r = 1 + q
#
#   q = r + (r^2)/2! + (r^3)/3! + (r^4)/4! + (r^5)/5! + (r^5)/5!
#

#include "fn_macros.h"
#include "exp_tables.h"

#define fname FN_PROTOTYPE_BAS64(exp)
#define fname_special _exp_special@PLT


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:

    ucomisd      .L__max_exp_arg(%rip), %xmm0
    ja           .L__y_is_inf
    jp           .L__y_is_nan
    ucomisd      .L__denormal_tiny_threshold(%rip), %xmm0
    jbe          .L__y_is_zero

.if __enable_IEEE_exceptions
    movdqa       %xmm0,%xmm1
    pand         .L__signbit_off(%rip),%xmm1
    ucomisd      .L__real_x_near0_threshold(%rip),%xmm1
    jb           .L__y_is_one    
.endif

    # x * (64/ln(2))
    movapd      %xmm0,%xmm1        
    mulsd       .L__real_64_by_log2(%rip), %xmm1

    # n = int( x * (64/ln(2)) )
    cvttpd2dq    %xmm1, %xmm2   #xmm2 = (int)n
    cvtdq2pd    %xmm2, %xmm1   #xmm1 = (double)n
    movd        %xmm2, %ecx
    movapd     %xmm1,%xmm2
    # r1 = x - n * ln(2)/64 head    
    mulsd    .L__log2_by_64_mhead(%rip),%xmm1
        
    #j = n & 0x3f    
    mov         $0x3f, %rax
    and         %ecx, %eax     #eax = j
    # m = (n - j) / 64      
    sar         $6, %ecx       #ecx = m
        

    # r2 = - n * ln(2)/64 tail
    mulsd    .L__log2_by_64_mtail(%rip),%xmm2
    addsd    %xmm1,%xmm0   #xmm0 = r1

    # r1+r2
    addsd       %xmm0, %xmm2 #xmm2 = r

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd       .L__real_1_by_720(%rip), %xmm3  #xmm3 = 1/720
    mulsd       %xmm2, %xmm3    #xmm3 = r*1/720
    movapd       .L__real_1_by_6(%rip), %xmm0    #xmm0 = 1/6    
    movapd      %xmm2, %xmm1 #xmm1 = r            
    mulsd       %xmm2, %xmm0    #xmm0 = r*1/6
    addsd       .L__real_1_by_120(%rip), %xmm3  #xmm3 = 1/120 + (r*1/720)
    mulsd       %xmm2, %xmm1    #xmm1 = r*r    
    addsd       .L__real_1_by_2(%rip), %xmm0  #xmm0 = 1/2 + (r*1/6)        
    movapd       %xmm1, %xmm4   #xmm4 = r*r
    mulsd       %xmm1, %xmm4    #xmm4 = (r*r) * (r*r)    
    mulsd       %xmm2, %xmm3    #xmm3 = r * (1/120 + (r*1/720))
    mulsd       %xmm1, %xmm0    #xmm0 = (r*r)*(1/2 + (r*1/6))
    addsd       .L__real_1_by_24(%rip), %xmm3  #xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addsd       %xmm2, %xmm0   #xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulsd       %xmm4, %xmm3   #xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addsd       %xmm3, %xmm0   #xmm0 = r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    
    # (f)*(q) + f2 + f1
    cmp         $0xfffffc02, %ecx # -1022    
    lea         .L__two_to_jby64_table(%rip), %rdx        
    lea         .L__two_to_jby64_tail_table(%rip), %r11       
    lea         .L__two_to_jby64_head_table(%rip), %r10      
    mulsd       (%rdx,%rax,8), %xmm0
    addsd       (%r11,%rax,8), %xmm0
    addsd       (%r10,%rax,8), %xmm0        

    jle         .L__process_denormal 
.L__process_normal:
    shl         $52, %rcx    
    movd        %rcx,%xmm2
    paddq       %xmm2, %xmm0
    ret

.p2align 4
.L__process_denormal:
    jl          .L__process_true_denormal
    ucomisd     .L__real_one(%rip), %xmm0
    jae         .L__process_normal
.L__process_true_denormal:
    # here ( e^r < 1 and m = -1022 ) or m <= -1023
    add         $1074, %ecx
    mov         $1, %rax    
    shl         %cl, %rax
    movd         %rax, %xmm2
    mulsd       %xmm2, %xmm0
    ret        
   
.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_inf:
    movq        %xmm0,%rax
    mov         $0x7ff0000000000000,%rdx
    cmp         %rdx,%rax
    je          .L__x_is_pos_inf
    movd        %rdx, %xmm1
    mov         $3, %edi
    call        fname_special
.L__x_is_pos_inf:    
    ret  
    
.p2align 4
.L__y_is_nan:
    movapd      %xmm0,%xmm1
    addsd       %xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret

.p2align 4
.L__y_is_zero:
    ucomisd     .L__min_exp_arg(%rip),%xmm0
    jbe         .L__return_zero
    movapd      .L__real_smallest_denormal(%rip), %xmm1
    jmp         .L__return_smallest_denormal
    
.p2align 4        
.L__return_zero:    
    movq        %xmm0,%rax
    mov         $0xfff0000000000000,%rdx    
    cmp         %rdx,%rax  
    je          .L__x_is_neg_inf
    pxor        %xmm1, %xmm1    #return value in xmm1,input in xmm0 before calling
.L__return_smallest_denormal:   
    mov         $2, %edi        #code in edi
    call        fname_special
    ret         
.L__x_is_neg_inf:
    pxor        %xmm0, %xmm0
    ret  
    
.p2align 4       
.L__y_is_one:
    movsd       .L__real_one(%rip),%xmm0
    ret

.else

.p2align 4
.L__y_is_inf:
    mov         $0x7ff0000000000000,%rax
    movd       %rax, %xmm0
    ret     

.p2align 4
.L__y_is_nan:
    addsd       %xmm0,%xmm0
    ret

.p2align 4
.L__y_is_zero:
    ucomisd      .L__min_exp_arg(%rip),%xmm0
    jbe          .L__return_zero
    movapd       .L__real_smallest_denormal(%rip), %xmm0
    ret
    
.p2align 4        
.L__return_zero:    
    pxor        %xmm0,%xmm0
    ret  
    
.endif

.section .rodata
.align 16
.L__max_exp_arg:            .quad 0x40862e42fefa39ef
.L__denormal_tiny_threshold:.quad 0xc0874046dfefd9d0
    
    .if __enable_IEEE_exceptions
    .L__signbit_off:            .quad 0x7fffffffffffffff
    .L__real_x_near0_threshold: .quad 0x3c00000000000000
    .endif
    
.L__min_exp_arg:            .quad 0xc0874910d52d3051
.L__real_64_by_log2:        .quad 0x40571547652b82fe    # 64/ln(2)
.L__real_one:               .quad 0x3ff0000000000000
.L__log2_by_64_mhead:       .quad 0xbf862e42fefa0000
.L__real_smallest_denormal: .quad 0x0000000000000001
.L__log2_by_64_mtail:       .quad 0xbd1cf79abc9e3b39

