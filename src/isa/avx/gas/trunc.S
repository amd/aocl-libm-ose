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

# trunc.S
#
# An implementation of the trunc libm function.
#
# The trunc functions round their argument to the integer value, in floating format,
# nearest to but no larger in magnitude than the argument.
#
#
# Prototype:
#
#     double trunc(double x);
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(trunc)



#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function

fname:

.if __enable_IEEE_exceptions      
	movd     %xmm0,%rax
	and      .L__mask_sign(%rip),%rax
	cmp      .L__2p53(%rip),%rax
	jae      .L__nan_inf_or_int
.endif	

	MOVAPD %xmm0,%xmm1	
	CVTTSD2SI %xmm0,%rax
	CMP .L__Erro_mask(%rip),%rax
	jz .Error_val	
	CVTSI2SD %rax,%xmm0	
	PSRLQ $63,%xmm1
	PSLLQ $63,%xmm1	
	POR %xmm1,%xmm0	
	ret
	
.Error_val:
	MOVAPD %xmm1,%xmm2
	CMPEQSD	%xmm1,%xmm1			
	ADDSD %xmm2,%xmm2	
	PAND %xmm1,%xmm0
	PANDN %xmm2,%xmm1
	POR %xmm1,%xmm0
        ret
        
.if __enable_IEEE_exceptions      
.p2align 4
.L__nan_inf_or_int:
       cmp      .L__positive_infinity(%rip),%rax
       jae      .L__x_is_nan_or_inf
       ret
.L__x_is_nan_or_inf:
       addsd        %xmm0,%xmm0    
       ret
.endif

.section .rodata
.align 16
.L__Erro_mask:  	    .quad 0x8000000000000000

.if __enable_IEEE_exceptions
.L__2p53:                   .quad 0x4340000000000000
.L__positive_infinity:      .quad 0x7ff0000000000000
.L__mask_sign:              .quad 0x7fffffffffffffff			        
.endif

