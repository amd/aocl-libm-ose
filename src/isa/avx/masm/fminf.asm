;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;fminf.S
;
; An implementation of the fminf libm function.
;
; Prototype:
;
;     float fminf(float x, float y)
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE fminf


fname_specail TEXTEQU <_fminf_special>
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

	movd	r8d,xmm0
    mov     eax, L_sign_mask
    and     eax, r8d
    cmp     eax, L__exp_mask_64
    jg      L_input_x_nan

	movd	r8d,xmm1
    mov     eax, L_sign_mask
    and     eax, r8d
    cmp     eax, L__exp_mask_64
    jg      L_input_y_nan

    xorps       xmm5, xmm5 ; 0
	movaps xmm3,xmm0
	movaps xmm4,xmm0

    minps       xmm3, xmm1 ; min(a,b)

    orps        xmm0, xmm1 ; a | b
    movaps      xmm4, xmm0
    cmpeqps     xmm0, xmm5 ; both a,b are zero

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

L_sign_mask                DD 07FFFFFFFh
                           DD 07FFFFFFFh         ;this zero is necessary
						   DQ 00h

L__exp_mask_64              DD 07F800000h
                           DD 07F800000h
						   DQ 00h

CONST    ENDS
data ENDS

END
