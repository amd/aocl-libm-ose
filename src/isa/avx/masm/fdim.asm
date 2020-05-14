;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;fdim.S
;
; An implementation of the fdim libm function.
;
; Prototype:
;
;     double fdim(double x, double y)
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE  fdim


fname_special   TEXTEQU <_fdim_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG 
	movd     rax,xmm0
	and      rax,L__mask_sign
	cmp      rax,L__positive_infinity
	ja       L__x_is_nan
	movd     rax,xmm1
	and      rax,L__mask_sign
	cmp      rax,L__positive_infinity
	ja       L__y_is_nan

	MOVAPD 	 xmm2,xmm0	
	SUBSD    xmm0,xmm1 	
	CMPNLESD xmm2,xmm1
	ANDPD 	 xmm0,xmm2
	add      rsp, stack_size
    ret

L__x_is_nan:
    movd        rax, xmm0    
    or          rax, L__real_qnanbit
    movd        xmm2, rax
    call        fname_special
	add      rsp, stack_size
    ret

L__y_is_nan:
    movd        rax, xmm1
    or          rax, L__real_qnanbit
    movd        xmm2, rax
    call        fname_special
	add      rsp, stack_size
    ret

fname        endp
TEXT    ENDS

data SEGMENT READ
CONST    SEGMENT
    
ALIGN 16
L__positive_infinity      DQ 07ff0000000000000h
L__mask_sign              DQ 07fffffffffffffffh
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit

CONST    ENDS
data ENDS

END

