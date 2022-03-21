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

# fabs.S
#
# An implementation of the fabs libm function.
#
# Prototype:
#
#     double fabs(double x);
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"

#define fname ALM_PROTO_BAS64(nearbyint)
#define fname_special _nearbyint_special@PLT



# local variable storage offsets

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.equ mvalue1,    0x10 
.equ mvalue2,    0x20 


.text
.align 16
.p2align 4,,15
.globl fname
ALM_FUNC_TYPE_ASM(f_name)

fname:  
    
    sub $0x28,%rsp                                                    
    movsd .L__2p52_mask_64(%rip),%xmm2                             
    movsd .L__sign_mask_64(%rip),%xmm4                               
    movsd %xmm0,%xmm1                                                 
    movsd %xmm0,%xmm5  # 
    pand  %xmm4,%xmm1 
    movsd  %xmm1,%xmm3  #  
    movd   %xmm1,%rax
    cmp    .L__2p52_mask_64(%rip),%rax 
    jg    .L__greater_than_2p52      # xmm0 is Nan 

.L__normal_input_case:
    #sign.u32 = checkbits.u32[1] & 0x80000000;
    #xmm4 = sign.u32
    pandn %xmm5,%xmm4
    #val_2p52.u32[1] = sign.u32 | 0x43300000;
    #val_2p52.u32[0] = 0;
    por   %xmm4,%xmm2
    #val_2p52.f64 = (x + val_2p52.f64) - val_2p52.f64;

    STMXCSR mvalue1(%rsp)
    mov     mvalue1(%rsp),%eax
    or	    $0x000000800,%eax
    mov    %eax,mvalue2(%rsp) 
    LDMXCSR mvalue2(%rsp)
    addpd  %xmm2,%xmm5	                  
    subpd  %xmm5,%xmm2						
    mov   mvalue1(%rsp),%eax
    and	 $0xffffffc0,%eax
    mov   %eax,mvalue2(%rsp) 
    LDMXCSR	mvalue2(%rsp)
    pand	.L__sign_mask_64(%rip),%xmm2	
    por		%xmm4,	%xmm2						
    movsd	%xmm2, 	%xmm0						
    add $0x28,%rsp
    ret
.L__special_case:
.L__greater_than_2p52:
    call fname_special
    add $0x28,%rsp
    ret

.section .rodata

.align 16
.L__sign_mask_64:          .quad 0x7FFFFFFFFFFFFFFF
                           .quad 0
.L__2p52_mask_64:          .quad 0x4330000000000000 
                           .quad 0
.L__exp_mask_64:           .quad 0x7FF0000000000000
                           .quad 0






