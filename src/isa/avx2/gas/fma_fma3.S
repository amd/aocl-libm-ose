#
# fma_bdozr.S
#
# A bulldozer implementation of fma libm function.
#
# Prototype:
#
#     double fma(double x, double y, double z);
#

#
#   Algorithm:
#
#
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(fma)


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
   vfmadd213sd     %xmm2,%xmm1,%xmm0 # 
   ret
    

