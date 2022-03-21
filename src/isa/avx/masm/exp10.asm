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

include exp_tables.inc
include fm.inc

ALM_PROTO_BAS64 exp10

fname_special   TEXTEQU <alm_exp_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG   

    movdqa                  xmm1, xmm0							
    andpd                   xmm1, L__positive_infinity
    ucomisd                 xmm1, L__positive_infinity
    je                      L__x_is_nan_or_inf    

    ucomisd  				xmm0,			L__max_exp10_arg				
    jae      								L__y_is_inf
    ucomisd  				xmm0,			L__min_exp10_arg				
    jbe      								L__y_is_zero
    																		;x * (64/log10(2))
    movapd   				xmm1,   		xmm0	
    psllq                   xmm1, 1
    psrlq                   xmm1, 1    
    ucomisd                 xmm1, L__real_x_near0_threshold
    jb                            L__y_is_one
	
    movapd   				xmm1,   		xmm0	
    mulsd    				xmm1,			L__real_64_by_log10of2		

	;n = int( x * (64/log10(2)) )
    cvttpd2dq				xmm2,   		xmm1							;xmm2 = (int)n
    cvtdq2pd 				xmm1,   		xmm2							;xmm1 = (double)n
    movd     				ecx,			xmm2							
    movapd   				xmm2,			xmm1							
    																		;r1 = x - n * log10(2)/64 head    
    mulsd   				xmm1,			L__log10of2_by_64_mhead		

    																		;j = n & 0x3f    
    mov      				rax,			03fh							
    and      				eax,    		ecx								;eax = j
    																		;m = (n - j) / 64      
    sar      				ecx,    		6								;ecx = m        

    																		;r2 = - n * log10(2)/64 tail
    mulsd   				xmm2, 			L__log10of2_by_64_mtail			;xmm2 = r2
    addsd  	 				xmm0,   		xmm1							;xmm0 = r1

    																		;r1 *= ln10																																																								;
    																		;r2 *= ln10																																																								;
    mulsd   				xmm0,			L__ln10						
    mulsd   				xmm2,			L__ln10						

    																		;r1+r2
    addsd    				xmm2, 			xmm0							;xmm2 = r

    																		;q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    																		;q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd   				xmm3,  			L__real_1_by_720				;xmm3 = 1/720
    mulsd    				xmm3,   		xmm2							;xmm3 = r*1/720
    movapd   				xmm0,   		L__real_1_by_6					;xmm0 = 1/6    
    movapd   				xmm1, 			xmm2							;xmm1 = r            
    mulsd    				xmm0,   		xmm2							;xmm0 = r*1/6
    addsd    				xmm3,  			L__real_1_by_120				;xmm3 = 1/120 + (r*1/720)
    mulsd    				xmm1,   		xmm2							;xmm1 = r*r    
    addsd    				xmm0,  			L__real_1_by_2					;xmm0 = 1/2 + (r*1/6)        
    movapd   				xmm4,   		xmm1							;xmm4 = r*r
    mulsd    				xmm4,   		xmm1							;xmm4 = (r*r) * (r*r)    
    mulsd    				xmm3,   		xmm2							;xmm3 = r * (1/120 + (r*1/720))
    mulsd    				xmm0,   		xmm1							;xmm0 = (r*r)*(1/2 + (r*1/6))
    addsd    				xmm3,  			L__real_1_by_24					;xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addsd    				xmm0,   		xmm2							;xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulsd    				xmm3,   		xmm4							;xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addsd    				xmm0,   		xmm3							;xmm0 = r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

    																		;(f)*(q) + f2 + f1
    cmp      				ecx, 			0fffffc02h						;-1022    
    lea      				rdx,    		L__two_to_jby64_table		
    lea      				r11,    		L__two_to_jby64_tail_table	
    lea      				r10,    		L__two_to_jby64_head_table	
    mulsd    				xmm0,			QWORD PTR [rdx+rax*8]			
    addsd    				xmm0,			QWORD PTR [r11+rax*8]			
    addsd    				xmm0,   		QWORD PTR [r10+rax*8]			

    jle      								L__process_denormal 
L__process_normal:
    shl        				rcx,    		52								
    movd       				xmm2,			rcx								
    paddq      				xmm0,			xmm2							
    add         rsp, stack_size
    ret   

ALIGN 16
L__process_denormal:
    jl          							L__process_true_denormal
    ucomisd     			xmm0,			L__real_one					
    jae         							L__process_normal
L__process_true_denormal:    																		;here ( e^r < 1 and m = -1022 ) or m <= -1023
    add         			ecx,			1074							
    mov         			rax,    		1								
    shl         			rax,			cl								
    movd        			xmm2,			rax								
    mulsd       			xmm0,			xmm2							
    jmp       L__finish      

L__y_is_one:
    movsd       xmm0, L__real_one
    jmp         L__finish

ALIGN 16 
L__x_is_nan_or_inf:
    movd        rax, xmm0    
    cmp         rax, L__positive_infinity
    je          L__finish
    cmp         rax, L__negative_infinity
    je          L__return_zero_without_exception    
    or          rax, L__real_qnanbit
    movd        xmm1, rax
    mov         r8d, 1
    call        fname_special
    jmp         L__finish 

ALIGN 16
L__y_is_inf:
    mov       				rax,			07ff0000000000000h				
    movd      				xmm1,			rax								
    mov       				r8d,			3								
    call        fname_special
    jmp       L__finish

ALIGN 16
L__y_is_zero:
    pxor       				xmm1,			xmm1							
    mov        				r8d,			2								
    call        fname_special
    jmp       L__finish

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
L__max_exp10_arg          DQ 040734413509f79ffh
L__min_exp10_arg            DQ 0c07434e6420f4374h
L__real_64_by_log10of2    DQ 0406A934F0979A371h  ; 64/log10(2)
L__ln10                   DQ 040026BB1BBB55516h
L__positive_infinity      DQ 07ff0000000000000h
L__negative_infinity      DQ 0fff0000000000000h
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit
L__real_x_near0_threshold        DQ 3c00000000000000h

ALIGN 16
L__log10of2_by_64_mhead DQ 0bF73441350000000h
L__log10of2_by_64_mtail DQ 0bda3ef3fde623e25h
L__real_one                   DQ 03ff0000000000000h

CONST    ENDS
data ENDS

END

