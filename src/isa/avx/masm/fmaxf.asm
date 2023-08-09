;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

;fmaxf.S
;
; An implementation of the fmaxf libm function.
;
; Prototype:
;
;     float fmaxf(float x, float y)
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE fmaxf

fname_specail TEXTEQU <_fmaxf_special>
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
	
	movapd xmm3,xmm0
	movapd xmm4,xmm0
    xorps  xmm5, xmm5 ; 0
	
	maxps  xmm3,xmm1

    orps        xmm0, xmm1 ; a | b
    andps       xmm4, xmm1
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
