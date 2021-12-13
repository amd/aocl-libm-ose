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

#define fname ALM_PROTO_BAS64(exp10)
#define fname_special _exp10_special@PLT

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    ucomisd      .L__max_exp10_arg(%rip), %xmm0
    jae          .L__y_is_inf
    jp           .L__y_is_nan
    ucomisd      .L__min_exp10_arg(%rip), %xmm0
    jbe          .L__y_is_zero

.if __enable_IEEE_exceptions
    movdqa       %xmm0,%xmm1
    pand         .L__signbit_off(%rip),%xmm1
    ucomisd      .L__real_x_near0_threshold(%rip),%xmm1
    jb           .L__y_is_one    
.endif

    # x * (64/log10(2))
    movapd      %xmm0,%xmm1        
    mulsd       .L__real_64_by_log10of2(%rip), %xmm1

    # n = int( x * (64/log10(2)) )
    cvttpd2dq    %xmm1, %xmm2   #xmm2 = (int)n
    cvtdq2pd    %xmm2, %xmm1   #xmm1 = (double)n
    movd        %xmm2, %ecx
    movapd     %xmm1,%xmm2
    # r1 = x - n * log10(2)/64 head    
    mulsd    .L__log10of2_by_64_mhead(%rip),%xmm1
        
    #j = n & 0x3f    
    mov         $0x3f, %rax
    and         %ecx, %eax     #eax = j
    # m = (n - j) / 64      
    sar         $6, %ecx       #ecx = m        

    # r2 = - n * log10(2)/64 tail
    mulsd    .L__log10of2_by_64_mtail(%rip),%xmm2 #xmm2 = r2
    addsd    %xmm1,%xmm0   #xmm0 = r1
    
    # r1 *= ln10;
    # r2 *= ln10;
    mulsd   .L__ln10(%rip),%xmm0
    mulsd   .L__ln10(%rip),%xmm2

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
    movq        %xmm0,%rax
    mov         $0xfff0000000000000,%rdx    
    cmp         %rdx,%rax  
    je          .L__x_is_neg_inf
    pxor        %xmm1, %xmm1    #return value in xmm1,input in xmm0 before calling
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
    pxor        %xmm0,%xmm0
    ret      
    
.endif

.section .rodata
.align 16
.L__max_exp10_arg:          .quad 0x40734413509f79ff
.L__min_exp10_arg:            .quad 0xc07434e6420f4374

    .if __enable_IEEE_exceptions
    .L__signbit_off:            .quad 0x7fffffffffffffff
    .L__real_x_near0_threshold: .quad 0x3c00000000000000
    .endif

.L__real_64_by_log10of2:    .quad 0x406A934F0979A371    # 64/log10(2)
.L__ln10:                   .quad 0x40026BB1BBB55516

.align 16
.L__log10of2_by_64_mhead: .quad 0xbF73441350000000
.L__log10of2_by_64_mtail: .quad 0xbda3ef3fde623e25
.L__real_one:                   .quad 0x3ff0000000000000

