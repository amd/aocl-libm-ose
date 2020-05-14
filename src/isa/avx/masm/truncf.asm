;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

; truncf.S
;
; An implementation of the truncf libm function.
;
; Prototype:
;
;     float truncf(float x);
;

;
;   Algorithm:
;


include fm.inc
FN_PROTOTYPE truncf

fname_special   TEXTEQU <_truncf_special>

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
		
	MOVAPD		xmm1,xmm0					
	CVTTSS2SI	rax,xmm0
	CMP			rax,L__Erro_mask
	jz			Error_val	
	CVTSI2SS	xmm0,rax	
	PSRLD		xmm1,31
	PSLLD		xmm1,31	
	POR			xmm0,xmm1		
	add         rsp, stack_size			
	ret			
				
Error_val:		
	MOVAPD		xmm2,xmm1
	CMPEQSS		xmm1,xmm1			
	ADDSS		xmm2,xmm2				
	PAND		xmm0,xmm1
	PANDN		xmm1,xmm2
	POR			xmm0,xmm1	
	add         rsp, stack_size
    ret

ALIGN 16
L__x_is_nan:
    movd        eax, xmm0    
    or          eax, L__real_qnanbit
    movd        xmm1, eax
    call        fname_special
	add         rsp, stack_size
    ret

fname        endp
TEXT    ENDS


data SEGMENT READ
CONST    SEGMENT

Align 16
L__Erro_mask  		DQ 08000000000000000h
L__positive_infinity          DD 07f800000h
L__mask_sign                  DD 07fffffffh
L__real_qnanbit               DD 00400000h

CONST    ENDS
data ENDS
END						    
					    


				    
