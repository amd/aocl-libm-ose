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

