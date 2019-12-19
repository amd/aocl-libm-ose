#fminf.S
#
# An implementation of the fminf libm function.
#
# The fmin functions determine the minimum numeric value of their arguments
#
#
# Prototype:
#
#     float fminf(float x, float y)
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fminf)
#define fname_specail _fminf_special@PLT




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
	 movd %xmm0,%r8d
	 mov .L_sign_mask(%rip),%eax
	 and %r8d,%eax
	 cmp .L__exp_mask_64(%rip),%eax
	 jg L_input_x_nan

	 movd %xmm1,%r8d
	 mov .L_sign_mask(%rip),%eax
	 and %r8d,%eax
	 cmp .L__exp_mask_64(%rip),%eax
	 jg L_input_y_nan

	 xorps %xmm5,%xmm5 # 0
	 movaps %xmm0,%xmm3
	 movaps %xmm0,%xmm4

	 minps %xmm1,%xmm3 # min(a ,b)

	 orps %xmm1,%xmm0 # a | b
	 movaps %xmm0,%xmm4
	 cmpeqps %xmm5,%xmm0 # both a ,b are zero

	 pand %xmm0,%xmm4 # max zero
	 pandn %xmm3,%xmm0
	 por %xmm4,%xmm0

	 add $0x18,%rsp
	 ret

L_input_x_nan:
L_input_y_nan:

	call fname_specail
	add  $0x18,%rsp
    ret

.section .rodata
.align 16

.L_sign_mask:              .long 0x7FFFFFFF
                           .long 0x7FFFFFFF         #this zero is necessary
						   .quad 0x0

.L__exp_mask_64:           .long 0x7F800000
                           .long 0x7F800000
						   .quad 0x0
