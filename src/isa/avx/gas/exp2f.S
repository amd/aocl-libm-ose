#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
#define fname FN_PROTOTYPE_BAS64(exp2f)
#define fname_special _exp2f_special@PLT


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    ucomiss .L__max_exp2f_arg(%rip), %xmm0
    ja .L__y_is_inf
    jp .L__y_is_nan
    ucomiss .L__min_exp2f_arg(%rip), %xmm0
    jb .L__y_is_zero

    cvtps2pd     %xmm0, %xmm0    #xmm0 = (double)x

    # x * (64)
    movapd      %xmm0,%xmm3      #xmm3 = (double)x
    #mulsd       .L__sixtyfour(%rip), %xmm3  #xmm3 = x * (64)
    paddq       .L__sixtyfour(%rip), %xmm3  #xmm3 = x * (64)

    # n = int( x * (64)
    cvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n
    cvtdq2pd    %xmm4, %xmm2  #xmm2 = (double)n

    # r = x - n * 1/64
    # r *= ln(2)
    mulsd       .L__one_by_64(%rip),%xmm2 #xmm2 = n * 1/64
    movd        %xmm4, %ecx     #ecx = n
    subsd       %xmm2, %xmm0    #xmm0 = r
    mulsd       .L__ln2(%rip),%xmm0 #xmm0 = r = r*ln(2)    
    movapd      %xmm0, %xmm1    #xmm1 = r

    # q = r+r*r*(1/2 + (1/6 * r))
    movsd       .L__real_1_by_6(%rip), %xmm3 
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
.L__max_exp2f_arg:                 .long 0x43000000
.L__min_exp2f_arg:                 .long 0xc3150000
.align 16
.L__sixtyfour:                  .quad 0x0060000000000000 # 64
.L__one_by_64:                  .quad 0x3F90000000000000 # 1/64
.L__ln2:                        .quad 0x3FE62E42FEFA39EF # ln(2)
