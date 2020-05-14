;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;copysign.S
;
; An implementation of the copysign libm function.
;
; Prototype:
;
;     double copysign(float x, float y);
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE copysign



; local variable storage offsets

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    .ENDPROLOG 

	PSLLQ xmm0,1   
	PSRLQ xmm0,1   
	PSRLQ xmm1,63  
	PSLLQ xmm1,63  
	POR   xmm0,xmm1
    ret

fname        endp
TEXT    ENDS
END

