;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;copysignf.S
;
; An implementation of the copysignf libm function.
;
; Prototype:
;
;     float copysignf(float x, float y);
;

;
;   Algorithm:
;


include fm.inc
FN_PROTOTYPE copysignf



; local variable storage offsets

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    .ENDPROLOG 

	PSLLD xmm0,1	
	PSRLD xmm0,1	
	PSRLD xmm1,31	
	PSLLD xmm1,31	
	POR   xmm0,xmm1
    ret

fname        endp
TEXT    ENDS
END



