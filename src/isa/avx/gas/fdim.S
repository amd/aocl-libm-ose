#fdim.S
#
# An implementation of the fdim libm function.
#
#  The fdim functions determine the positive difference between their arguments
#	
#	x - y if x > y
#	+0    if x <= y
#
#
#
# Prototype:
#
#     double fdim(double x, double y)
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fdim)
#define fname_special _fdim_special@PLT

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
	movd     %xmm1,%rcx	
	cmp      .L__positive_infinity(%rip),%rax
	je       .L__check_y_pos_inf
.L__continue_1:	
	cmp      .L__negative_infinity(%rip),%rax
	je       .L__check_y_neg_inf
.L__continue_2:
	and      .L__mask_sign(%rip),%rax
	cmp      .L__positive_infinity(%rip),%rax
	ja       .L__x_is_nan
	and      .L__mask_sign(%rip),%rcx
	cmp      .L__positive_infinity(%rip),%rcx
	ja       .L__y_is_nan
.endif

	MOVAPD 	%xmm0,%xmm2	
	SUBSD   %xmm1,%xmm0 	
	CMPNLESD %xmm1,%xmm2
	ANDPD 	 %xmm2,%xmm0
	ret

.if __enable_IEEE_exceptions 

.L__check_y_pos_inf:
    cmp      .L__positive_infinity(%rip),%rcx
    je       .L__return_zero
    jmp      .L__continue_1

.L__check_y_neg_inf:
    cmp      .L__negative_infinity(%rip),%rcx
    je       .L__return_zero
    jmp      .L__continue_2    
	
.L__x_is_nan:
    movdqa      %xmm0,%xmm2
    addsd       %xmm2,%xmm2
    call        fname_special
    ret

.L__y_is_nan:
    movdqa      %xmm1,%xmm2
    addsd       %xmm2,%xmm2
    call        fname_special
    ret
    
.L__return_zero:
    xorps   %xmm0,%xmm0
    ret

.section .rodata
.align 16
.L__positive_infinity:      .quad 0x7ff0000000000000
.L__mask_sign:              .quad 0x7fffffffffffffff
.L__negative_infinity:      .quad 0xfff0000000000000
.endif    
