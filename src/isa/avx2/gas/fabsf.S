# fabsf.S
#
# An implementation of the fabsf libm function.
#
# Prototype:
#
#     float fabsf(float x);
#

#
#   Algorithm: AND the Most Significant Bit of the 
#              single precision number with 0 to get the 
#              floating point absolute.
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_FMA3(fabsf)
#define fname_special _fabsf_special@PLT




#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
    #input is in xmm0, which contains the final result also.

	sub $0x18,%rsp
	movdqa %xmm0,%xmm1
	andps .L__fabsf_and_mask(%rip),%xmm1
	movd %xmm1,%r8d
	cmp .L__inf_mask(%rip),%r8d
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
.L__fabsf_and_mask:          .long 0x7FFFFFFF
                             .long 0x0
                             .quad 0x0


.L__inf_mask:		     .long 0x7f800000
			      .long 0x0
                              .quad 0x0


