#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#define fname ALM_PROTO_BAS64(vrs4_exp10f)

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    movdqa       %xmm0, %xmm10   # save for later use
    minps        .L__max_exp10f_arg(%rip), %xmm0    
    movhlps      %xmm0, %xmm5  
    cvtps2pd     %xmm5, %xmm5    #xmm5 = x3,x2
    cvtps2pd     %xmm0, %xmm0    #xmm0 = (double)x

    # x * (64/log10of(2))
    movapd      %xmm5,%xmm8      #xmm8 = (double)x3,x2    
    movapd      %xmm0,%xmm3      #xmm3 = (double)x1,x0
    mulpd       .L__real_64_by_log10of2(%rip), %xmm8  #xmm8 = x3 * (64/ln(2),x2 * (64/ln(2)    
    mulpd       .L__real_64_by_log10of2(%rip), %xmm3  #xmm3 = x1 * (64/ln(2),x0 * (64/ln(2)

    # n = int( x * (64/log10of(2)) )
    cvtpd2dq    %xmm8, %xmm9  #xmm9 = (int)n3,n2    
    cvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n1,n0
    cvtdq2pd    %xmm9, %xmm7  #xmm7 = (double)n3,n2    
    cvtdq2pd    %xmm4, %xmm2  #xmm2 = (double)n1,n0

    # r = x - n * ln(2)/64
    # r *= ln(10)
    mulpd       .L__real_log10of2_by_64(%rip),%xmm7 #xmm7 = n3 * 1/64,n2 * 1/64    
    mulpd       .L__real_log10of2_by_64(%rip),%xmm2 #xmm2 = n1 * 1/64,n0 * 1/64
    subpd       %xmm7, %xmm5    #xmm5 = r3,r2    
    subpd       %xmm2, %xmm0    #xmm0 = r1,r0
    mulpd       .L__real_ln10(%rip),%xmm5 #xmm5 = r3,r2 = r3*ln(2),r2*ln(2)        
    mulpd       .L__real_ln10(%rip),%xmm0 #xmm0 = r1,r0 = r1*ln(2),r0*ln(2)
    movapd      %xmm5, %xmm6    #xmm6 = r3,r2    
    movapd      %xmm0, %xmm1    #xmm1 = r1,r0

    # q = r + r*r(1/2 + r*1/6)
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

    #2 ^m * (f + (f*q))    
    paddq       %xmm5, %xmm9    
    paddq       %xmm0, %xmm4
    cvtpd2ps    %xmm9, %xmm5    
    cvtpd2ps    %xmm4, %xmm0
    movlhps     %xmm5, %xmm0

    ##special case for any x < min_exp_arg
    ##remove this code if the above code takes care of this
    movdqa      .L__min_exp10f_arg(%rip), %xmm1        
    cmpps       $1,%xmm10,%xmm1
    pand        %xmm1, %xmm0    #make zeros to put in place of any x < min_exp2_arg    
    
    ##special case for any x = nan
    ##remove this code if the above code takes care of this
    movdqa      %xmm10, %xmm1    
    cmpps       $0x0,%xmm10,%xmm10
    pand        %xmm10, %xmm0    
    addps       %xmm1, %xmm1  #make qnan to put in place of any x =nan
    pandn       %xmm1, %xmm10
    por         %xmm10, %xmm0       
    ret 
    
.section .rodata
.align 16
.L__max_exp10f_arg:                .octa 0x421A209B421A209B421A209B421A209B
.L__min_exp10f_arg:                .octa 0xC23369F4C23369F4C23369F4C23369F4
.L__real_64_by_log10of2:        .octa 0x406A934F0979A371406A934F0979A371 # 64/log10(2)
.L__real_log10of2_by_64:        .octa 0x3F734413509F79FF3F734413509F79FF # log10of2_by_64
.L__real_ln10:                  .octa 0x40026BB1BBB5551640026BB1BBB55516 # ln(10)




