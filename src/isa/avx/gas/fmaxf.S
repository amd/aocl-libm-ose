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

#fmaxf.S
#
# An implementation of the fmaxf libm function.
#
# The fmax functions determine the maximum numeric value of their arguments.
#
# Prototype:
#
#     float fmaxf(float x, float y)
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fmaxf)
#define fname_specail _fmaxf_special@PLT





#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
        sub $0x18,%rsp
	movd %xmm0 , %r8d
	mov  .L_sign_mask(%rip)  , %eax
	and  %r8d , %eax
	cmp  .L__exp_mask_64(%rip) , %eax
	jg  L_input_x_nan

	movd %xmm1 , %r8d
	mov  .L_sign_mask(%rip)  , %eax
	and  %r8d , %eax
	cmp  .L__exp_mask_64(%rip) , %eax
	jg  L_input_y_nan

	movapd %xmm0 , %xmm3
	movapd %xmm0 , %xmm4
	xorps %xmm5 , %xmm5 # 0

	maxps %xmm1 , %xmm3

	orps %xmm1 , %xmm0 # a | b
	andps %xmm1 , %xmm4
	cmpeqps %xmm5 , %xmm0 # both a,b are zero

	pand %xmm0 , %xmm4 # max zero
	pandn %xmm3 , %xmm0
	por %xmm4 , %xmm0

	add $0x18,%rsp
	ret

L_input_x_nan:
L_input_y_nan:

	call fname_specail
	add $0x18 , %rsp
 ret



.section .rodata
.align 16

.L_sign_mask:              .long 0x7FFFFFFF
                           .long 0x7FFFFFFF         #this zero is necessary
			   .quad 0x0

.L__exp_mask_64:           .long 0x7F800000
                           .long 0x7F800000
			   .quad 0x0

