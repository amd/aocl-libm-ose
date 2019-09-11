;fdimf.S
;
; An implementation of the fdimf libm function.
;
; Prototype:
;
;     float fdimf(float x, float y)
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE fdimf


fname_special   TEXTEQU <_fdimf_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG 
	movd     eax,xmm0
	and      eax,L__mask_sign
	cmp      eax,L__positive_infinity
	ja       L__x_is_nan
	movd     eax,xmm1
	and      eax,L__mask_sign
	cmp      eax,L__positive_infinity
	ja       L__y_is_nan

	MOVAPD 	 xmm2, xmm0	
	SUBSS    xmm0, xmm1 	
	CMPNLESS xmm2, xmm1
	ANDPS 	 xmm0, xmm2
	add      rsp, stack_size
    ret

L__x_is_nan:
    movd        eax, xmm0    
    or          eax, L__real_qnanbit
    movd        xmm2, eax
    call        fname_special
	add         rsp, stack_size
    ret

L__y_is_nan:
    movd        eax, xmm1
    or          eax, L__real_qnanbit
    movd        xmm2, eax
    call        fname_special
	add         rsp, stack_size
    ret

fname        endp
TEXT    ENDS

data SEGMENT READ

CONST    SEGMENT

ALIGN 16
L__positive_infinity          DD 07f800000h
L__mask_sign                  DD 07fffffffh
L__real_qnanbit               DD 00400000h

CONST    ENDS
data ENDS
END
