# truncf.S
#
# An implementation of the truncf libm function.
#
#
# The trunf functions round their argument to the integer value, in floating format,
# nearest to but no larger in magnitude than the argument.
#
#
# Prototype:
#
#     float truncf(float x);
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE(truncf )

WEAK_ACML_LIB_ALIAS fasttruncf, FN_PROTOTYPE(truncf)

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
WEAK_MATH_LIB_ALIAS truncf,fname
fname:

.if __enable_IEEE_exceptions      
	movd     %xmm0,%eax
	and      .L__mask_sign(%rip),%eax
	cmp      .L__2p23(%rip),%eax
	jae      .L__nan_inf_or_int
.endif	

	MOVAPD %xmm0,%xmm1	
	CVTTSS2SIQ %xmm0,%rax	
	CMP .L__Erro_mask(%rip),%rax
	jz .Error_val		
	CVTSI2SSQ %rax,%xmm0	
	PSRLD $31,%xmm1
	PSLLD $31,%xmm1	
	POR %xmm1,%xmm0	
	ret
	
.Error_val:
	MOVAPD %xmm1,%xmm2
	CMPEQSS	%xmm1,%xmm1			
	ADDSS %xmm2,%xmm2		
	PAND %xmm1,%xmm0
	PANDN %xmm2,%xmm1
	POR %xmm1,%xmm0
	ret

.if __enable_IEEE_exceptions      
.p2align 4
.L__nan_inf_or_int:
       cmp      .L__positive_infinity(%rip),%eax
       jae      .L__x_is_nan_or_inf
       ret
.L__x_is_nan_or_inf:
       addss        %xmm0,%xmm0    
       ret
.endif


.section .rodata
.align 16
.L__Erro_mask:  	      .quad 0x8000000000000000

.if __enable_IEEE_exceptions 
.L__2p23:                     .long 0x4B000000
.L__positive_infinity:        .long 0x7f800000
.L__mask_sign:                .long 0x7fffffff
.endif

