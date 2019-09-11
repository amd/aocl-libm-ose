;fmax.S
;
; An implementation of the fmax libm function.
;
; Prototype:
;
;     double fmax(double x, double y)
;

;
;   Algorithm:
;


include fm.inc
FN_PROTOTYPE fmax


fname_specail TEXTEQU <_fmax_special>
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

	movd	r8,xmm0
    mov     rax, L_sign_mask
    and     rax, r8
    cmp     rax, L__exp_mask_64
    jg      L_input_x_nan

	movd	r8,xmm1
    mov     rax, L_sign_mask
    and     rax, r8
    cmp     rax, L__exp_mask_64
    jg      L_input_y_nan
	
	movapd xmm3,xmm0
	movapd xmm4,xmm0
    xorpd  xmm5, xmm5 ; 0
	
	maxpd  xmm3,xmm1

    orpd        xmm0, xmm1 ; a | b
    andpd       xmm4, xmm1
    cmpeqpd     xmm0, xmm5 ; both a,b are zero

    pand        xmm4, xmm0 ; max zero
    pandn       xmm0, xmm3
    por         xmm0, xmm4		
		  
	add   rsp, 018h
    ret

L_input_x_nan:
L_input_y_nan:

	call fname_specail
	add   rsp, 018h
    ret



fname        endp
TEXT    ENDS

data SEGMENT READ

CONST    SEGMENT    

ALIGN 16 

L_sign_mask                DQ 07FFFFFFFFFFFFFFFh
                           DQ 07FFFFFFFFFFFFFFFh         ;this zero is necessary

L__exp_mask_64              DQ 07FF0000000000000h
                           DQ 07FF0000000000000h


CONST    ENDS
data ENDS

END
