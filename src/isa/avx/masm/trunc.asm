;
; Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without modification,
; are permitted provided that the following conditions are met:
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
; 3. Neither the name of the copyright holder nor the names of its contributors
;    may be used to endorse or promote products derived from this software without
;    specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
; IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
; OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
;

; trunc.S
;
; An implementation of the trunc libm function.
;
; Prototype:
;
;     double trunc(double x);
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE trunc

fname_special   TEXTEQU <_trunc_special>

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

	MOVAPD		xmm1,xmm0					
	CVTTSD2SI	rax,xmm0
	CMP			rax,L__Erro_mask
	jz			Error_val	
	CVTSI2SD	xmm0,rax	
	PSRLQ		xmm1,63
	PSLLQ		xmm1,63	
	POR			xmm0,xmm1				
	add      rsp, stack_size
	ret			
				
Error_val:		
	MOVAPD		xmm2,xmm1
	CMPEQSD		xmm1,xmm1			
	ADDSD		xmm2,xmm2				
	PAND		xmm0,xmm1
	PANDN		xmm1,xmm2
	POR			xmm0,xmm1	
	add      rsp, stack_size
    ret

ALIGN 16
L__x_is_nan:
    movd        rax, xmm0    
    or          rax, L__real_qnanbit
    movd        xmm1, rax
    call        fname_special
	add      rsp, stack_size
    ret

fname        endp
TEXT    ENDS


data SEGMENT READ
CONST    SEGMENT

Align 16
L__Erro_mask  		DQ 08000000000000000h
L__positive_infinity      DQ 07ff0000000000000h
L__mask_sign              DQ 07fffffffffffffffh
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit

CONST    ENDS
data ENDS
END						    
		
