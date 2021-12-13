#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#include "fn_macros.h"
#include "exp_tables.h"

#define fname ALM_PROTO_BAS64(vrs4_expm1f)

#ifdef __ELF__
    .section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    movdqa       %xmm0, %xmm10   # save for later use
    movhlps      %xmm0, %xmm5
        
    #x_double = (double)x;
    cvtps2pd     %xmm5, %xmm5    #xmm0 = (double)x3,x2
    cvtps2pd     %xmm0, %xmm0    #xmm0 = (double)x1,x0    
    
    # x * (64/ln(2))
    movapd      %xmm5,%xmm8      #xmm8 = (xouble)x3,x2    
    movapd      %xmm0,%xmm3      #xmm3 = (xouble)x1,x0
    mulpd       .L__real_64_by_log2(%rip), %xmm8  #xmm8 = x3 * (64/ln(2),x2 * (64/ln(2)    
    mulpd       .L__real_64_by_log2(%rip), %xmm3  #xmm3 = x1 * (64/ln(2),x0 * (64/ln(2)
   
    # n = int( x * (64/ln(2)) )
    cvtpd2dq    %xmm8, %xmm9  #xmm9 = (int)n3,n2
    cvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n1,n0
    cvtdq2pd    %xmm9, %xmm7  #xmm7 = (double)n3,n2    
    cvtdq2pd    %xmm4, %xmm2  #xmm2 = (double)n1,n0
    
    # r = x - n * ln(2)/64
    mulpd       .L__real_log2_by_64(%rip),%xmm7 #xmm7 = n3 * ln(2)/64,n2 * ln(2)/64    
    mulpd       .L__real_log2_by_64(%rip),%xmm2 #xmm2 = n1 * ln(2)/64,n0 * ln(2)/64
    subpd       %xmm7, %xmm5    #xmm5 = r3,r2    
    subpd       %xmm2, %xmm0    #xmm0 = r1,r0
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
    addpd       %xmm6, %xmm5  #xmm5 = r+r*r*(1/2 + (1/6 * r))    
    addpd       %xmm1, %xmm0  #xmm0 = r+r*r*(1/2 + (1/6 * r))

    #j = n & 0x3f
    mov         $0x0000003f0000003f, %rax     #rax = 0x3f
    movd        %xmm4, %rcx     #rcx = n1,n0    
    and         %rax, %rcx      #rcx = j1,j0
    movd        %xmm9, %r8     #r8 = n3,n2 
    and         %rax, %r8      #r8 = j3,j2
    movd        %rcx, %xmm3
    movd        %r8, %xmm1
    movlhps     %xmm1, %xmm3    #xmm3 = j3,j2,j1,j0
    
    # m = (n - j) / 64    
    psrad       $6,%xmm9     
    psrad       $6,%xmm4
    movlhps     %xmm9, %xmm4 #xmm4 = m3,m2,m1,m0    
    
    #process each input seperately
    lea         .L__two_to_jby64_table(%rip), %r10    
    movd        %xmm3, %eax
    movd        %xmm4, %ecx
    movhlps     %xmm0, %xmm6
    mov         $4, %r11d
    jmp         .L__start

.p2align 4
.L__check_n_prepare_next:
    dec         %r11d
    jz          .L__ret
    psrldq      $4,%xmm3
    movd        %xmm3, %eax
    psrldq      $4,%xmm4
    movd        %xmm4, %ecx 
    psrldq      $4,%xmm10       
    cmp         $3,%r11d
    je          .L__output1_inputq2
    cmp         $2,%r11d
    je          .L__output2_inputq3
    cmp         $1,%r11d
    je          .L__output3_inputq4
    
.p2align 4
.L__output1_inputq2:
    pxor      %xmm9,%xmm9
    pslldq    $12, %xmm0
    psrldq    $12, %xmm0
    por       %xmm0,%xmm9
    movdqa    %xmm6, %xmm0    
    jmp .L__start
    
.p2align 4
.L__output2_inputq3:    
    pslldq    $12, %xmm0
    psrldq    $8, %xmm0
    por       %xmm0,%xmm9
    movdqa    %xmm5, %xmm0    
    jmp .L__start
    
.p2align 4
.L__output3_inputq4:
    pslldq    $12, %xmm0
    psrldq    $4, %xmm0
    por       %xmm0,%xmm9
    movhlps   %xmm5, %xmm0    

.p2align 4
.L__start:
    ucomiss .L__max_expm1_arg(%rip),%xmm10         ##if(x > max_expm1_arg)
    ja .L__y_is_inf
    jp .L__y_is_nan
    ucomiss .L__log_OnePlus_OneByFour(%rip),%xmm10 ##if(x < log_OnePlus_OneByFour)
    jae .L__Normal_Flow
    ucomiss .L__log_OneMinus_OneByFour(%rip),%xmm10 ##if(x > log_OneMinus_OneByFour)
    ja .L__Small_Arg
    ucomiss .L__min_expm1_arg(%rip),%xmm10         ##if(x < min_expm1_arg)
    jb .L__y_is_minusOne

.p2align 4    
.L__Normal_Flow:    
    #f
    movsd       (%r10,%rax,8), %xmm2 #xmm2 = f
    
    #f*q
    mulsd       %xmm2, %xmm0  #xmm0 = f*q
    
    #twopmm.u64 = (1023 - (long)m) << 52;    
    #f - twopmm
    mov         $1023, %eax
    sub         %ecx, %eax
    shl         $52, %rax    
    movd        %rax, %xmm1
    subsd       %xmm1, %xmm2
    
    #z = f * q + (f - twopmm.f64) ;
    #z = z* 2^m
    addsd       %xmm2, %xmm0    
    shl         $52, %rcx
    movd        %rcx, %xmm1
    paddq       %xmm0, %xmm1
    cvtpd2ps    %xmm1, %xmm0
    jmp        .L__check_n_prepare_next
    
.p2align 4
.L__Small_Arg:
    #log(1-1/4) < x < log(1+1/4)
    #q = x*x*x*(A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))));
    movapd %xmm10,%xmm0
    ucomiss .L__minus_zero(%rip), %xmm0
    je     .L__check_n_prepare_next
    movdqa %xmm0,%xmm1
    mulss .L__A5_f(%rip),%xmm1
    addss .L__A4_f(%rip),%xmm1
    mulss %xmm0,%xmm1
    addss .L__A3_f(%rip),%xmm1
    mulss %xmm0,%xmm1
    addss .L__A2_f(%rip),%xmm1
    mulss %xmm0,%xmm1
    addss .L__A1_f(%rip),%xmm1
    mulss %xmm0,%xmm1
    mulss %xmm0,%xmm1
    mulss %xmm0,%xmm1
    cvtps2pd %xmm0,%xmm2
    movdqa %xmm2,%xmm0
    mulsd %xmm0,%xmm2
    mulsd .L__real_1_by_2(%rip),%xmm2
    addsd %xmm2,%xmm0
    cvtps2pd %xmm1,%xmm2
    addsd %xmm0,%xmm2
    cvtpd2ps %xmm2,%xmm0        
    jmp    .L__check_n_prepare_next         
        
.p2align 4
.L__y_is_minusOne:
    mov         $0xBF800000, %eax
    movd        %eax, %xmm0
    jmp    .L__check_n_prepare_next     
    
.p2align 4
.L__y_is_inf:
    mov         $0x7f800000,%eax
    movd        %eax, %xmm0
    jmp    .L__check_n_prepare_next     
    
.p2align 4
.L__y_is_nan:
    movapd %xmm10,%xmm0
    addss  %xmm0,%xmm0
    jmp    .L__check_n_prepare_next     
    
.L__ret:
    pslldq    $12, %xmm0
    por       %xmm0,%xmm9
    movdqa    %xmm9, %xmm0    
    ret
    
.section .rodata
.align 16
.L__max_expm1_arg:          .long 0x42B19999
.L__log_OnePlus_OneByFour:  .long 0x3E647FBF
.L__log_OneMinus_OneByFour: .long 0xBE934B11
.L__min_expm1_arg:          .long 0xC18AA122
.L__real_64_by_log2:        .octa 0x40571547652b82fe40571547652b82fe # 64/ln(2)
.L__real_log2_by_64:        .octa 0x3f862e42fefa39ef3f862e42fefa39ef # log2_by_64
.L__A1_f:                   .long 0x3E2AAAAA   
.L__A2_f:                   .long 0x3D2AAAA0
.L__A3_f:                   .long 0x3C0889FF     
.L__A4_f:                   .long 0x3AB64DE5
.L__A5_f:                   .long 0x394AB327
.L__minus_zero:             .long 0x80000000
    
    

    

