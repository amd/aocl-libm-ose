; fabsf.S
;
; An implementation of the fabs libm function.
;
; Prototype:
;
;     double fabsf(double x);
;

;
;   Algorithm:
;


include fm.inc
FN_PROTOTYPE fabsf


fname_specail TEXTEQU <_fabsf_special>
EXTERN       fname_specail      : PROC


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
	andps xmm1,L__fabsf_and_mask
	movd r8,xmm1
	cmp r8d,inf_mask
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
L__fabsf_and_mask            DD 07FFFFFFFh
                             DD 0h
                             DQ 0h

inf_mask				 DD 07f800000h
						 DD 0h
						 DQ 0h



CONST    ENDS
data ENDS
END



