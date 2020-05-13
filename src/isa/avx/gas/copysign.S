#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#copysign.S
#
# An implementation of the copysign libm function.
#
# The copysign functions produce a value with the magnitude of x and the sign of y.
# They produce a NaN (with the sign of y) if x is a NaN. On implementations that
# represent a signed zero but do not treat negative zero consistently in arithmetic
# operations, the copysign functions regard the sign of zero as positive.
#
#
# Prototype:
#
#     double copysign(float x, float y)
#
#
#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(copysign)



#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function



fname:

	PSLLQ $1,%xmm0
	PSRLQ $1,%xmm0
	PSRLQ $63,%xmm1
	PSLLQ $63,%xmm1
	POR   %xmm1,%xmm0
	
    ret
