#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#include "fn_macros.h"
#include "exp_tables.h"

#define fname FN_PROTOTYPE_BAS64(vrsa_exp2f)
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
.L__process_next4:
    sub         $0x4, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first123
    movdqu      (%rsi, %rdi, 4), %xmm0
    jmp         .L__start
    
.p2align 4    
.L__process_first123:
    je          .L__process_first3
    cmp         $-3, %rdi
    jl          .L__return #multiple of 4elements, all are processed %rdi == -4
    je          .L__process_first1    
    #process first 2    
    mov         $0x0, %rcx
    movsd       (%rsi, %rcx, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first1:
    mov         $0x0, %rcx
    movss       (%rsi, %rcx, 4), %xmm0
    jmp         .L__start    

.p2align 4    
.L__process_first3:
    mov         $0x1, %rcx
    movsd       (%rsi, %rcx, 4), %xmm0    
    dec         %rcx  # zero
    movss       (%rsi, %rcx, 4), %xmm1
    pslldq      $4,%xmm0
    por         %xmm1, %xmm0    
    
.p2align 4    
.L__start:    
    movdqa       %xmm0, %xmm10   # save for later use
    minps        .L__max_exp2f_arg(%rip), %xmm0    
    movhlps      %xmm0, %xmm5    
    cvtps2pd     %xmm5, %xmm5    #xmm5 = x3,x2
    cvtps2pd     %xmm0, %xmm0    #xmm0 = x1,x0

    # x * (64)
    movapd      %xmm5,%xmm8      #xmm8 = x3,x2
    movapd      %xmm0,%xmm3      #xmm3 = x1,x0
    paddq       .L__sixtyfour(%rip), %xmm8  #xmm8 = x3 * (64),x2 * (64)    
    paddq       .L__sixtyfour(%rip), %xmm3  #xmm3 = x1 * (64),x0 * (64)

    # n = int( x * (64)
    cvtpd2dq    %xmm8, %xmm9  #xmm9 = (int)n3,n2    
    cvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n1,n0
    cvtdq2pd    %xmm9, %xmm7  #xmm7 = (double)n3,n2    
    cvtdq2pd    %xmm4, %xmm2  #xmm2 = (double)n1,n0

    # r = x - n * 1/64
    # r *= ln(2)
    mulpd       .L__one_by_64(%rip),%xmm7 #xmm7 = n3 * 1/64,n2 * 1/64    
    mulpd       .L__one_by_64(%rip),%xmm2 #xmm2 = n1 * 1/64,n0 * 1/64
    subpd       %xmm7, %xmm5    #xmm5 = r3,r2    
    subpd       %xmm2, %xmm0    #xmm0 = r1,r0
    mulpd       .L__ln2(%rip),%xmm5 #xmm5 = r3,r2 = r3*ln(2),r2*ln(2)        
    mulpd       .L__ln2(%rip),%xmm0 #xmm0 = r1,r0 = r1*ln(2),r0*ln(2)
    movapd      %xmm5, %xmm6    #xmm6 = r3,r2    
    movapd      %xmm0, %xmm1    #xmm1 = r1,r0

    # q
    movapd       .L__real_1_by_6(%rip), %xmm8     
    movapd       .L__real_1_by_6(%rip), %xmm3     
    mulpd       %xmm5, %xmm8 #xmm8 = 1/6 * r3,1/6 * r2    
    mulpd       %xmm0, %xmm3 #xmm3 = 1/6 * r1,1/6 * r0    
    mulpd       %xmm6, %xmm5 #xmm5 =  r  * r    
    mulpd       %xmm1, %xmm0 #xmm0 =  r  * r
    addpd       .L__real_1_by_2(%rip), %xmm8 #xmm8 = 1/2 + (1/6 * r)    
    addpd       .L__real_1_by_2(%rip), %xmm3 #xmm3 = 1/2 + (1/6 * r)
    mulpd       %xmm8, %xmm5  #xmm5 = r*r*(1/2 + (1/6 * r))    
    mulpd       %xmm3, %xmm0  #xmm0 = r*r*(1/2 + (1/6 * r))
    addpd       %xmm6, %xmm5  #xmm0 = r+r*r*(1/2 + (1/6 * r))    
    addpd       %xmm1, %xmm0  #xmm0 = r+r*r*(1/2 + (1/6 * r))
    
    #j = n & 0x3f
    mov         $0x0000003f0000003f, %rax     #rax = 0x3f,0x3f    
    movd        %xmm4, %rcx     #rcx = n1,n0
    and         %rax, %rcx      #rcx = j1,j0
    movd        %xmm9, %r8     #r8 = n3,n2 
    and         %rax, %r8      #r8 = j3,j2
    
    # f + (f*q)
    lea         .L__two_to_jby64_table(%rip), %r10
    mov         %rcx, %rax
    shl         $32, %rcx
    shr         $32, %rcx       #rcx = j0
    movsd      (%r10,%rcx,8), %xmm1
    shr         $32, %rax       #rax = j1
    movhpd     (%r10,%rax,8), %xmm1
    mulpd       %xmm1, %xmm0
    addpd       %xmm1, %xmm0    
    mov         %r8, %rax
    shl         $32, %r8
    shr         $32, %r8       #r8 = j2
    movsd      (%r10,%r8,8), %xmm1
    shr         $32, %rax       #rax = j3
    movhpd     (%r10,%rax,8), %xmm1
    mulpd       %xmm1, %xmm5
    addpd       %xmm1, %xmm5

    .p2align 4
    # m = (n - j) / 64       
    psrad       $6,%xmm9     
    psrad       $6,%xmm4
    punpckldq   %xmm9, %xmm9
    punpckldq   %xmm4, %xmm4    
    psllq       $52,%xmm9    
    psllq       $52,%xmm4
    paddq       %xmm5, %xmm9    
    paddq       %xmm0, %xmm4
    cvtpd2ps    %xmm9, %xmm5    
    cvtpd2ps    %xmm4, %xmm0
    movlhps     %xmm5, %xmm0

    ##special case for any x < min_exp_arg
    ##remove this code if the above code takes care of this
    movdqa      .L__min_exp2f_arg(%rip), %xmm1        
    cmpps       $1,%xmm10,%xmm1
    pand        %xmm1, %xmm0     #make zeros to put in place of any x < min_exp2_arg
    
    ##special case for any x = nan
    ##remove this code if the above code takes care of this
    movdqa      %xmm10, %xmm1    
    cmpps       $0x0,%xmm10,%xmm10
    pand        %xmm10, %xmm0    
    addps       %xmm1, %xmm1  #make qnan to put in place of any x =nan
    pandn       %xmm1, %xmm10
    por         %xmm10, %xmm0       

    cmp         $-1,  %rdi
    jle          .L__store123
    movdqu       %xmm0, (%rdx, %rdi, 4)
    jmp         .L__process_next4    
    
.p2align 4    
.L__store123:
    je         .L__store3
    cmp         $-3, %rdi    
    je          .L__store1    
    #store 2    
    add        $0x2, %rdi
    movsd      %xmm0, (%rdx,%rdi,4)    
    jmp         .L__return

.p2align 4    
.L__store3:
    movdqa      %xmm0, %xmm1
    psrldq      $4, %xmm0
    inc         %rdi
    movss       %xmm1, (%rdx,%rdi,4)
    inc         %rdi
    movsd      %xmm0, (%rdx,%rdi,4)    
    jmp         .L__return
    
.p2align 4    
.L__store1:
    mov        $0x0, %rdi
    movss      %xmm0, (%rdx,%rdi,4)    

.L__return:    
    ret         
    
.section .rodata
.align 16
.L__max_exp2f_arg:               .octa 0x43000000430000004300000043000000
.L__min_exp2f_arg:               .octa 0xc3150000c3150000c3150000c3150000
.L__sixtyfour:                  .octa 0x00600000000000000060000000000000 # 64
.L__one_by_64:                  .octa 0x3F900000000000003F90000000000000 # 1/64
.L__ln2:                        .octa 0x3FE62E42FEFA39EF3FE62E42FEFA39EF # ln(2)
