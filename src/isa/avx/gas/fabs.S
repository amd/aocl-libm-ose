# fabs.S
#
# An implementation of the fabs libm function.
#
# Prototype:
#
#     double fabs(double x);
#

#
#   Algorithm: AND the Most Significant Bit of the
#              double precision number with 0 to get the
#              floating point absolute.
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fabs)
#define fname_special _fabs_special@PLT




#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
    #input is in %xmm0, which contains the final result also.
	sub $0x18,%rsp
	movdqa %xmm0,%xmm1
	andpd .L__fabs_and_mask(%rip),%xmm1
	movd %xmm1,%r8
	cmp .L__inf_mask(%rip),%r8
	jg  L__rais_exception
	movdqa %xmm1,%xmm0
	add     $0x18,%rsp
	ret
L__rais_exception:
	call fname_special
	add    $0x18,%rsp
	ret


.section .rodata

.align 16
.L__fabs_and_mask:          .quad 0x7FFFFFFFFFFFFFFF
                            .quad 0x0


.L__inf_mask:    .quad 0x7ff0000000000000
		.quad 0x7ff0000000000000


