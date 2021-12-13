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

FN_PROTOTYPE nearbyint


fname_special   TEXTEQU <_nearbyint_special>

mvalue1      EQU     10h 
mvalue2      EQU     20h 

;Define name and any external functions being called
EXTERN       fname_special      : PROC
StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
StackAllocate 028h
    .ENDPROLOG   
							movsd	xmm2,  	L__2p52_mask_64			; 
							movsd	xmm4,  	L__sign_mask_64			;    
							movsd	xmm1, 	xmm0						; copy input to another xmm register       2
							movsd	xmm5, 	xmm0						; copy input to another xmm register       2
							pand	xmm1, 	xmm4						;                                   4
							movsd	xmm3, 	xmm1						; copy input to another xmm register       2\
							movd    rax,    xmm1
							cmp	    rax, 	L__2p52_mask_64				;                                  4
							jg				L__greater_than_2p52		;    1
L__normal_input_case:
																		;sign.u32 =  checkbits.u32[1] & 0x80000000					
																		;xmm4 = sign.u32
							pandn	xmm4,	xmm5						
																		;val_2p52.u32[1] = sign.u32 | 0x43300000					
																		;val_2p52.u32[0] = 0										
							por		xmm2,	xmm4						
																		;val_2p52.f64 = (x + val_2p52.f64) - val_2p52.f64			
							STMXCSR [mvalue1+ rsp]
							mov     eax,[mvalue1 + rsp]
							or		eax,000000800h
							mov    [mvalue2 + rsp],eax 
							LDMXCSR	[mvalue2+ rsp]
							addpd	xmm5, 	xmm2	                  ;val_2p52.u32[1] = ((val_2p52.u32[1] << 1) >> 1) | sign.u32 					
							subpd	xmm2,	xmm5						
							mov     eax,[mvalue1 + rsp]
							and		eax,0ffffffc0h
							mov     [mvalue2+ rsp],eax 
							LDMXCSR	[mvalue2+ rsp]
																	
							pand	xmm2,	XMMWORD PTR L__sign_mask_64						
							por		xmm2,	xmm4						
							movsd	xmm0, 	xmm2						; copy input to another xmm register       2
							add     rsp, 028h
							ret
L__special_case:
L__greater_than_2p52:
							call  fname_special		
							add     rsp, 028h
    						ret											; <result is present in xmm0

    
fname        endp
TEXT    ENDS
    
data SEGMENT READ

CONST    SEGMENT

ALIGN 16    
    
L__sign_mask_64          DQ 07FFFFFFFFFFFFFFFh
                           DQ 00h
L__2p52_mask_64          DQ 04330000000000000h 
                           DQ 00h
L__exp_mask_64           DQ 07FF0000000000000h
                           DQ 00h
                           
                           
CONST    ENDS
data ENDS

END

                           






