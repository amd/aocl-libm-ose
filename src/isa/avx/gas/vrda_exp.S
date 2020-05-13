#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#include "fn_macros.h"
#include "exp_tables.h"

#define fname FN_PROTOTYPE_BAS64(vrda_exp)
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
    movdqa      %xmm0, %xmm10   # save for later use

    # x * (64/ln(2))
    movapd      %xmm0,%xmm1        
    mulpd       .L__real_64_by_log2(%rip), %xmm1

    # n = int( x * (64/ln(2)) )
    cvttpd2dq  %xmm1, %xmm2  #xmm2 = (int)n1,n0
    cvtdq2pd   %xmm2, %xmm1   #xmm1 = (double)n1,n0
    movd       %xmm2, %rcx    #rcx = (int)n1,n0
    movdqa     %xmm2, %xmm5    #rcx = (int)n1,n0
    movapd     %xmm1,%xmm2     #xmm2 = (double)n1,n0
    
    # r1 = x - n * ln(2)/64 head    
    mulpd    .L__log2_by_64_mhead(%rip),%xmm1
        
    #j = n & 0x3f    
    mov         $0x0000003f0000003f, %rax
    and         %rcx, %rax     #rax = j1,j0
    mov         %rax, %r8
    shl         $32, %rax
    shr         $32, %rax      #rax = j0
    shr         $32, %r8      #r8 = j1
        
    # m = (n - j) / 64      
    psrad         $6, %xmm5      #xmm5 = m1,m0        

    # r2 = - n * ln(2)/64 tail
    mulpd    .L__log2_by_64_mtail(%rip),%xmm2 #xmm2 = r21,r20
    addpd    %xmm1,%xmm0   #xmm0 = r11

    # r1+r2
    addpd       %xmm0, %xmm2 #xmm2 = r

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd       .L__real_1_by_720(%rip), %xmm3  #xmm3 = 1/720
    mulpd       %xmm2, %xmm3    #xmm3 = r*1/720
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
    movdqa      .L__min_exp_arg(%rip), %xmm1        
    cmppd       $1,%xmm10,%xmm1
    pand        %xmm1, %xmm0    #make zeros to put in place of any x < min_exp2_arg
    
    movdqa      .L__denormal_tiny_threshold(%rip), %xmm2
    cmppd       $1,%xmm10,%xmm2
    pandn        %xmm1,%xmm2
    movdqa       %xmm2, %xmm1
    pandn        %xmm0, %xmm1
    pand         .L__real_smallest_denormal(%rip), %xmm2
    por          %xmm2, %xmm1
    movdqa       %xmm1, %xmm0
    
    
    ##special case for any x > max_exp_arg
    ##remove this code if the above code takes care of this
    movdqa      %xmm10, %xmm3    
    movdqa      .L__max_exp_arg(%rip), %xmm1        
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
.L__max_exp_arg:            .octa 0x40862e42fefa39f040862e42fefa39f0  #40862e42fefa39ef40862e42fefa39ef
.L__denormal_tiny_threshold:.octa 0xc0874046dfefd9d0c0874046dfefd9d0
.L__min_exp_arg:            .octa 0xc0874910d52d3051c0874910d52d3051
.L__real_64_by_log2:        .octa 0x40571547652b82fe40571547652b82fe    # 64/ln(2)
.L__log2_by_64_mhead: .octa 0xbf862e42fefa0000bf862e42fefa0000
.L__log2_by_64_mtail: .octa 0xbd1cf79abc9e3b39bd1cf79abc9e3b39
.L__real_one:                   .octa 0x3ff00000000000003ff0000000000000
.L__real_smallest_denormal:     .octa 0x00000000000000010000000000000001
.L__real_inf:             .octa 0x7ff00000000000007ff0000000000000




