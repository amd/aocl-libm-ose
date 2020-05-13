#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#fdimf.S
#
# An implementation of the fdimf libm function.
#
#  The fdim functions determine the positive difference between their arguments
#	
#	x - y if x > y
#	+0    if x <= y
#
# Prototype:
#
#     float fdimf(float x, float y)
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fdimf)
#define fname_special _fdimf_special@PLT

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
	movd     %xmm0,%eax
	movd     %xmm1,%ecx
	cmp      .L__positive_infinity(%rip),%eax
	je       .L__check_y_pos_inf
.L__continue_1:	
	cmp      .L__negative_infinity(%rip),%eax
	je       .L__check_y_neg_inf
.L__continue_2:	
	and      .L__mask_sign(%rip),%eax
	cmp      .L__positive_infinity(%rip),%eax
	ja       .L__x_is_nan_or_inf

	and      .L__mask_sign(%rip),%ecx
	cmp      .L__positive_infinity(%rip),%ecx
	ja       .L__y_is_nan_or_inf
.endif

	MOVAPD 	%xmm0,%xmm2	
	SUBSS   %xmm1,%xmm0 	
	CMPNLESS %xmm1,%xmm2
	ANDPS 	 %xmm2,%xmm0
	ret

.if __enable_IEEE_exceptions

.L__check_y_pos_inf:
    cmp      .L__positive_infinity(%rip),%ecx
    je       .L__return_zero
    jmp      .L__continue_1

.L__check_y_neg_inf:
    cmp      .L__negative_infinity(%rip),%ecx
    je       .L__return_zero
    jmp      .L__continue_2 
    
.L__x_is_nan_or_inf:
    movdqa      %xmm0,%xmm2
    addss       %xmm2,%xmm2
    call        fname_special
    ret

.L__y_is_nan_or_inf:
    movdqa      %xmm1,%xmm2
    addss       %xmm2,%xmm2
    call        fname_special
    ret

.L__return_zero:
    xorps   %xmm0,%xmm0
    ret
    
.section .rodata
.align 16
.L__positive_infinity:      .quad 0x7f800000
.L__mask_sign:              .quad 0x7fffffff	
.L__negative_infinity:      .quad 0xff800000
.endif

