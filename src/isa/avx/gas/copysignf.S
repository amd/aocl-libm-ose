#copysignf.S
#
# An implementation of the copysignf libm function.
#
# The copysign functions produce a value with the magnitude of x and the sign of y.
# They produce a NaN (with the sign of y) if x is a NaN. On implementations that
# represent a signed zero but do not treat negative zero consistently in arithmetic
# operations, the copysign functions regard the sign of zero as positive.
#
# Prototype:
#
#     float copysignf(float x, float y)#
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE(copysignf)

WEAK_ACML_LIB_ALIAS fastcopysignf, FN_PROTOTYPE(copysignf)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function

WEAK_MATH_LIB_ALIAS copysignf,fname

fname:
	#PANDN .L__fabsf_and_mask, %xmm1
	#POR %xmm1,%xmm0 

	PSLLD $1,%xmm0
	PSRLD $1,%xmm0
	PSRLD $31,%xmm1
	PSLLD $31,%xmm1
	POR   %xmm1,%xmm0
	
    ret

#.align 16
#.L__sign_mask:               .long 0x7FFFFFFF
                             .long 0x0
                             .quad 0x0

