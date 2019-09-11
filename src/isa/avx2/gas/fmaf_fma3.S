#
# fmaf_bdozr.S
#
# A bulldozer implementation of fmaf libm function.
#
# Prototype:
#
#     float fmaf(float x, float y, float z);
#

#
#   Algorithm:
#
#
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(fmaf)


# local variable storage offsets

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
   vfmadd213ss     %xmm2,%xmm1,%xmm0 # 
   ret
    

