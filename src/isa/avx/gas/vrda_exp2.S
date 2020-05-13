#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#include "fn_macros.h"
#include "exp_tables.h"

#define fname FN_PROTOTYPE_BAS64(vrda_exp2)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
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
    movdqa       %xmm0, %xmm10   # save for later use
    # x * (64)
    movapd      %xmm0,%xmm2        
    mulpd       .L__real_64(%rip), %xmm2

    # n = int( x * (64))
    cvttpd2dq    %xmm2, %xmm1  #xmm1 = (int)n1,n0
    cvtdq2pd    %xmm1, %xmm2   #xmm2 = (double)n1,n0
    movd        %xmm1, %rcx    #rcx = (int)n1,n0
    movdqa      %xmm1, %xmm5   #xmm5 = (int)n1,n0

    # r = x - n * 1/64  
    #r *= ln2;      
    mulpd    .L__one_by_64(%rip),%xmm2
    addpd    %xmm0,%xmm2   #xmm2 = r1,r0    
    mulpd    .L__ln_2(%rip),%xmm2    
        
    #j = n & 0x3f    
    mov         $0x0000003f0000003f, %rax
    and         %rcx, %rax     #rax = j1,j0
    mov         %rax, %r8
    shl         $32, %rax
    shr         $32, %rax      #rax = j0
    shr         $32, %r8      #r8 = j1
    
    # m = (n - j) / 64      
    psrad       $6, %xmm5      #xmm5 = m1,m0

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd       .L__real_1_by_720(%rip), %xmm3  #xmm3 = 1/720
    mulpd        %xmm2, %xmm3       #xmm3 = r*1/720
    movapd       .L__real_1_by_6(%rip), %xmm0    #xmm0 = 1/6    
    movapd      %xmm2, %xmm1 #xmm1 = r            
    mulpd       %xmm2, %xmm0    #xmm0 = r*1/6
    addpd       .L__real_1_by_120(%rip), %xmm3  #xmm3 = 1/120 + (r*1/720)
    mulpd       %xmm2, %xmm1    #xmm1 = r*r    
    addpd       .L__real_1_by_2(%rip), %xmm0  #xmm0 = 1/2 + (r*1/6)        
    movapd       %xmm1, %xmm4   #xmm4 = r*r
    mulpd       %xmm1, %xmm4    #xmm4 = (r*r) * (r*r)    
    mulpd       %xmm2, %xmm3    #xmm3 = r * (1/120 + (r*1/720))
    mulpd       %xmm1, %xmm0    #xmm0 = (r*r)*(1/2 + (r*1/6))
    addpd       .L__real_1_by_24(%rip), %xmm3  #xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addpd       %xmm2, %xmm0   #xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulpd       %xmm4, %xmm3   #xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addpd       %xmm3, %xmm0   #xmm0 = r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    
    # (f)*(q) + f2 + f1
    lea         .L__two_to_jby64_table(%rip), %rcx        
    lea         .L__two_to_jby64_tail_table(%rip), %r11       
    lea         .L__two_to_jby64_head_table(%rip), %r10      
    movsd       (%rcx,%rax,8), %xmm1
    movhpd      (%rcx,%r8,8), %xmm1   
    mulpd       %xmm1, %xmm0
    movsd       (%r11,%rax,8), %xmm1
    movhpd      (%r11,%r8,8), %xmm1       
    addpd       %xmm1, %xmm0
    movsd       (%r10,%rax,8), %xmm1
    movhpd      (%r10,%r8,8), %xmm1           
    addpd       %xmm1, %xmm0           
    
    movhlps     %xmm0,%xmm1   #save higher part
    movd        %xmm5,%rcx    #rcx = m1,m0
    mov         $2,%r8d;

.L__denormal_check:    
    cmp         $0xfffffc02, %ecx # -1022    
    jle         .L__process_denormal    
.L__process_normal:
    psllq       $52, %xmm5    
    paddq       %xmm5, %xmm0
    dec         %r8d
    jz          .L__ret  #completed processing of the two elements
    shr         $32,%rcx #get m1 to ecx
    movlhps     %xmm0,%xmm1 #save result0
    movdqa      %xmm1,%xmm0 #put higer part for processing
    movd        %rcx,%xmm5  #move m1 to xmm5
    jmp         .L__denormal_check #repeat for second input

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
    dec         %r8d
    jz          .L__ret  #completed processing of the two elements
    movd        %xmm5, %rcx
    shr         $32,%rcx #get m1 to ecx
    movlhps     %xmm0,%xmm1 #save result0
    movdqa      %xmm1,%xmm0 #put higer part for processing
    movd        %rcx,%xmm5  #move m1 to xmm5
    jmp         .L__denormal_check #repeat for second input      
    
.p2align 4
.L__ret:
    movdqa      %xmm0, %xmm2 #save result1
    movhlps     %xmm1, %xmm0 #get result0 to correct position
    movlhps     %xmm2, %xmm0 #get result1 to correct position

    ##special case for any x < min_exp_arg
    ##remove this code if the above code takes care of this
    movdqa      .L__min_exp2_arg(%rip), %xmm1        
    cmppd       $1,%xmm10,%xmm1
    pand        %xmm1, %xmm0  #make zeros to put in place of any x < min_exp2_arg      
    
    ##special case for any x > max_exp_arg
    ##remove this code if the above code takes care of this
    movdqa      %xmm10, %xmm3    
    movdqa      .L__max_exp2_arg(%rip), %xmm1        
    cmppd       $1,%xmm1,%xmm3
    pand        %xmm3, %xmm0  
    movdqa     .L__real_inf(%rip), %xmm2  #make inf to put in place of any x > max_exp_arg
    pandn       %xmm2, %xmm3
    por         %xmm3, %xmm0     
    
    ##special case for any x = nan
    ##remove this code if the above code takes care of this
    movdqa      %xmm10, %xmm1    
    cmppd       $0x0,%xmm10,%xmm10
    pand        %xmm10, %xmm0    
    addpd       %xmm1, %xmm1  #make qnan to put in place of any x =nan
    pandn       %xmm1, %xmm10
    por         %xmm10, %xmm0     

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
.L__max_exp2_arg:            .octa 0x40900000000000004090000000000000
.L__min_exp2_arg:            .octa 0xc090c80000000000c090c80000000000
.L__real_64:                 .octa 0x40500000000000004050000000000000    # 64
.L__ln_2:                    .octa 0x3FE62E42FEFA39EF3FE62E42FEFA39EF
.L__one_by_64:               .octa 0xbF90000000000000bF90000000000000
.L__real_one:                .octa 0x3ff00000000000003ff0000000000000
.L__real_inf:                .octa 0x7ff00000000000007ff0000000000000

