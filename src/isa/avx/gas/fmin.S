#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#fmin.S
#
# An implementation of the fmin libm function.
#
# The fmin functions determine the minimum numeric value of their arguments
#
# Prototype:
#
#     double fmin(double x, double y)
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fmin)
#define fname_specail _fmin_special@PLT




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
	 movd %xmm0 ,%r8
	 mov .L_sign_mask(%rip) ,%rax
	 and %r8 ,%rax
	 cmp .L__exp_mask_64(%rip) ,%rax
	 jg L_input_x_nan

	 movd %xmm1 ,%r8
	 mov .L_sign_mask(%rip) ,%rax
	 and %r8 ,%rax
	 cmp .L__exp_mask_64(%rip) ,%rax
	 jg L_input_y_nan


	 xorpd %xmm5 ,%xmm5 # 0
	 movapd %xmm0 ,%xmm3
	 movapd %xmm0 ,%xmm4

	 minpd %xmm1 ,%xmm3 # min(a,b)

	 orpd %xmm1 ,%xmm0 # a | b
	 movapd %xmm0 ,%xmm4
	 cmpeqpd %xmm5 ,%xmm0 # both a,b are zero

	 pand %xmm0 ,%xmm4 # max zero
	 pandn %xmm3 ,%xmm0
	 por %xmm4 ,%xmm0

	 add $0x18,%rsp
	 ret

L_input_x_nan:
L_input_y_nan:

	 call fname_specail
	 add $0x18,%rsp
 ret

.section .rodata
.align 16


.L_sign_mask:           .quad 0x7FFFFFFFFFFFFFFF
                        .quad  0x7FFFFFFFFFFFFFFF         #this zero is necessary

.L__exp_mask_64:         .quad 0x7FF0000000000000
                         .quad 0x7FF0000000000000
