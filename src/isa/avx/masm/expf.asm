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

;
; expf.S
;
; An implementation of the expf libm function.
;
; Prototype:
;
;     float expf(float x);
;

;
;   Algorithm:
;       Similar to one presnted in exp.S
;

include exp_tables.inc
include fm.inc

ALM_PROTO_BAS64 expf

fname_special   TEXTEQU <_expf_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG   

    movdqa                  xmm1, xmm0
    andps                   xmm1, L__positive_infinity
    ucomiss                 xmm1, L__positive_infinity
    je                      L__x_is_nan_or_inf  
    
    ucomiss 			xmm0,		L__max_exp_arg			
    jae								L__y_is_inf
    ucomiss 			xmm0,		L__min_exp_arg			
    jb 								L__y_is_zero

    cvtps2pd     		xmm0,		xmm0									;xmm0 = (double)x

    																		;x * (64/ln(2))
    movapd      		xmm3,		xmm0									;xmm3 = (xouble)x
    mulsd       		xmm3,  		L__real_64_by_log2						;xmm3 = x * (64/ln(2)

    																		;n = int( x * (64/ln(2)) )
    cvtpd2dq    		xmm4,  		xmm3									;xmm4 = (int)n
    cvtdq2pd    		xmm2,  		xmm4									;xmm2 = (double)n

    																		;r = x - n * ln(2)/64
    mulsd       		xmm2, 		L__real_log2_by_64						;xmm2 = n * ln(2)/64
    movd        		ecx,    	xmm4									;ecx = n
    subsd       		xmm0,    	xmm2									;xmm0 = r
    movapd      		xmm1,    	xmm0									;xmm1 = r

    																		;q
    movsd       		xmm3, 		L__real_1_by_6			
    mulsd       		xmm3, 		xmm0									;xmm3 = 1/6 * r
    mulsd       		xmm0, 		xmm1									;xmm0 =  r  * r
    addsd       		xmm3, 		L__real_1_by_2							;xmm3 = 1/2 + (1/6 * r)
    mulsd       		xmm0,  		xmm3									;xmm0 = r*r*(1/2 + (1/6 * r))
    addsd       		xmm0,  		xmm1									;xmm0 = r+r*r*(1/2 + (1/6 * r))

    																		;j = n & 0x3f
    mov         		rax,     	03fh									;rax = 0x3f
    and         		eax,      	ecx									;eax = j = n & 0x3f
    																		;m = (n - j) / 64    
    sar         		ecx,        6										;ecx = m
    shl         		rcx,		52							

    																		;(f)*(1+q)
    lea         		r10,    	L__two_to_jby64_table	
    movsd       		xmm2,		QWORD PTR [ r10+rax *8] 				    
    mulsd       		xmm0,		xmm2							
    addsd       		xmm0,		xmm2							

    movd        		xmm1,		rcx							
    paddq       		xmm1,		xmm0							
    cvtpd2ps    		xmm0,		xmm1							
    add         rsp, stack_size
    ret       


ALIGN 16 
L__x_is_nan_or_inf:
    movd        eax, xmm0    
    cmp         eax, L__positive_infinity
    je          L__finish
    cmp         eax, L__negative_infinity
    je          L__return_zero_without_exception    
    or          eax, L__real_qnanbit
    movd        xmm1, eax
    mov         r8d, 1
    call        fname_special
    jmp         L__finish 
    
ALIGN 16
L__y_is_zero:
    pxor        		xmm1,    	xmm1									;return value in xmm1,input in xmm0 before calling
    mov         		r8d,        2							 			;code in edi
    call        					fname_special
    jmp                 L__finish        

ALIGN 16
L__y_is_inf:
    mov         		edx,		07f800000h					
    movd        		xmm1,		edx				
    mov         		r8d,		3							 	
    call        					fname_special
    jmp                 L__finish  

ALIGN 16    
L__return_zero_without_exception:
    pxor xmm0,xmm0
        
ALIGN 16
L__finish:    
    add         rsp, stack_size
    ret       
    
fname        endp
TEXT    ENDS
    
data SEGMENT READ

CONST    SEGMENT

ALIGN 16
L__max_exp_arg                 DD 042B17218h
L__min_exp_arg                 DD 0C2CE8ED0h
L__real_64_by_log2            DQ 040571547652b82feh ; 64/ln(2)
L__real_log2_by_64            DQ 03f862e42fefa39efh ; log2_by_64
L__negative_infinity          DD 0ff800000h
L__real_qnanbit               DD 00400000h
L__positive_infinity          DD 07f800000h
                              DQ 0
CONST    ENDS
data ENDS

END
