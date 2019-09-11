; fabs.S
;
; An implementation of the fabs libm function.
;
; Prototype:
;
;     double fabs(double x);
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE fabs


fname_specail TEXTEQU <_fabs_special>
EXTERN       fname_specail      : PROC

; local variable storage offsets
StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
StackAllocate 018h
    .ENDPROLOG 

	movdqa xmm1,xmm0
	andpd xmm1,__fabs_and_mask
	movd r8,xmm1
	cmp r8,inf_mask
	jg  L__rais_exception
	movdqa xmm0,xmm1
	add     rsp, 018h
    ret
L__rais_exception:
	call fname_specail
	add     rsp, 018h
    ret

	
fname        endp
TEXT    ENDS


data SEGMENT READ
CONST    SEGMENT

Align 16
__fabs_and_mask          DQ 07FFFFFFFFFFFFFFFh
                         DQ 00000000000000000h

inf_mask				 DQ 07ff0000000000000h
						 DQ 07ff0000000000000h

CONST    ENDS
data ENDS
END

