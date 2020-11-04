;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

FN_PROTOTYPE_BAS64 vrd2_exp2

p_temp		equ		0		; temporary for get/put bits operation
p_temp1		equ		10h		; temporary for get/put bits operation

save_xmm10  equ		20h
save_rbx	equ		30h


stack_size      EQU     0A8h



StackAllocate   MACRO size
                    sub         rsp, size
                    .ALLOCSTACK size
                ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    
	movdqa	OWORD PTR [rsp+save_xmm10],xmm10		; save xmm10
	.SAVEXMM128 xmm10, save_xmm10
	
	mov	QWORD PTR [rsp+save_rbx],rbx	; save rbx
	.SAVEREG rbx, save_rbx	
	    
    .ENDPROLOG   

    movupd xmm0, XMMWORD PTR [rcx]


    movdqa      		xmm10,xmm0					; save for later use
    										; x* (64)
    movapd      		xmm2,xmm0
    mulpd       		xmm2,L__real_64

    										; n= int( x * (64))
    cvttpd2dq   		xmm1,xmm2					;xmm1 = (int)n1,n0
    cvtdq2pd    		xmm2,xmm1					;xmm2 = (double)n1,n0
    movd        		rcx,xmm1					;rcx = (int)n1,n0
    movdqa      		xmm5,xmm1					;xmm5 = (int)n1,n0

    										; r= x - n * 1/64  
    										;r *= ln2																																																																																																																																																																																																																																																												;      
    mulpd    			xmm2,L__one_by_64
    addpd    			xmm2,xmm0					;xmm2 = r1,r0    
    mulpd    			xmm2,L__ln_2

    										;j = n & 0x3f    
    mov         		rax,00000003f0000003fh
    and         		rax,rcx						;rax = j1,j0
    mov         		rbx,rax
    shl         		rax,32
    shr         		rax,32						;rax = j0
    shr         		rbx,32						;rbx = j1

    										; m= (n - j) / 64      
    psrad       		xmm5,6						;xmm5 = m1,m0

    										; q= r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    										; q= r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd      		xmm3,L__real_1_by_720				;xmm3 = 1/720
    mulpd       		xmm3,xmm2					;xmm3 = r*1/720
    movapd      		xmm0,L__real_1_by_6				;xmm0 = 1/6    
    movapd      		xmm1,xmm2					;xmm1 = r            
    mulpd       		xmm0,xmm2					;xmm0 = r*1/6
    addpd       		xmm3,L__real_1_by_120				;xmm3 = 1/120 + (r*1/720)
    mulpd       		xmm1,xmm2					;xmm1 = r*r    
    addpd       		xmm0,L__real_1_by_2				;xmm0 = 1/2 + (r*1/6)        
    movapd      		xmm4,xmm1					;xmm4 = r*r
    mulpd       		xmm4,xmm1					;xmm4 = (r*r) * (r*r)    
    mulpd       		xmm3,xmm2					;xmm3 = r * (1/120 + (r*1/720))
    mulpd       		xmm0,xmm1					;xmm0 = (r*r)*(1/2 + (r*1/6))
    addpd       		xmm3,L__real_1_by_24				;xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addpd       		xmm0,xmm2					;xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulpd       		xmm3,xmm4					;xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addpd       		xmm0,xmm3					;xmm0 = r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

    										; (f)*(q) + f2 + f1
    lea         		rcx,L__two_to_jby64_table
    lea         		r11,L__two_to_jby64_tail_table
    lea         		r10,L__two_to_jby64_head_table
    movsd       		xmm1,QWORD PTR  [rcx+rax*8]
    movhpd      		xmm1,QWORD PTR  [rcx+rbx*8]
    mulpd       		xmm0,xmm1
    movsd       		xmm1,QWORD PTR  [r11+rax*8]
    movhpd      		xmm1,QWORD PTR  [r11+rbx*8]
    addpd       		xmm0,xmm1
    movsd       		xmm1,QWORD PTR  [r10+rax*8]
    movhpd      		xmm1,QWORD PTR  [r10+rbx*8]
    addpd       		xmm0,xmm1

    movhlps     		xmm1,xmm0					;save higher part
    movd        		rcx,xmm5					;rcx = m1,m0
    mov         		ebx,2						;

L__denormal_check:    
    cmp        			ecx,0fffffc02h					; -1022    
    jle         		L__process_denormal
L__process_normal:
    psllq       		xmm5,52
    paddq       		xmm0,xmm5
    dec         		ebx
    jz          		L__ret						;completed processing of the two elements
    shr         		rcx,32						;get m1 to ecx
    movlhps     		xmm1,xmm0					;save result0
    movdqa      		xmm0,xmm1					;put higer part for processing
    movd        		xmm5,rcx					;move m1 to xmm5
    jmp         		L__denormal_check				;repeat for second input

ALIGN 16
L__process_denormal:
    jl          		L__process_true_denormal
    ucomisd     		xmm0,L__real_one
    jae         		L__process_normal
L__process_true_denormal:
    										; here ( e^r < 1 and m = -1022 ) or m <= -1023    
    add        			ecx,1074
    mov        			rax,1
    shl        			rax,cl
    movd       			xmm2,rax
    mulsd      			xmm0,xmm2
    dec        			ebx
    jz         			L__ret						;completed processing of the two elements
    movd       			rcx,xmm5
    shr        			rcx,32						;get m1 to ecx
    movlhps    			xmm1,xmm0					;save result0
    movdqa     			xmm0,xmm1					;put higer part for processing
    movd       			xmm5,rcx					;move m1 to xmm5
    jmp        			L__denormal_check				;repeat for second input      

ALIGN 16
L__ret:
    movdqa     			xmm2,xmm0					;save result1
    movhlps    			xmm0,xmm1					;get result0 to correct position
    movlhps    			xmm0,xmm2					;get result1 to correct position

    										;																																																																																																																																																							;special case for any x < min_exp_arg
    										;																																																																																																																																																							;remove this code if the above code takes care of this
    movdqa     			xmm1,XMMWORD PTR L__min_exp2_arg
    cmpltpd      		xmm1,xmm10
    pand       			xmm0,xmm1					;make zeros to put in place of any x < min_exp2_arg      

    										;																																																																																																																																																							;special case for any x > max_exp_arg
    										;																																																																																																																																																							;remove this code if the above code takes care of this
    movdqa     			xmm3,xmm10
    movdqa     			xmm1,XMMWORD PTR L__max_exp2_arg
    cmpltpd      		xmm3,xmm1
    pand       			xmm0,xmm3
    movdqa     			xmm2,XMMWORD PTR L__real_inf				;make inf to put in place of any x > max_exp_arg
    pandn      			xmm3,xmm2
    por        			xmm0,xmm3

    										;																																																																																																																																																							;special case for any x = nan
    										;																																																																																																																																																							;remove this code if the above code takes care of this
    movdqa     			xmm1,xmm10
    cmpeqpd      		xmm10,xmm10
    pand       			xmm0,xmm10
    addpd      			xmm1,xmm1					;make qnan to put in place of any x =nan
    pandn      			xmm10,xmm1
    por        			xmm0,xmm10

	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]	; restore xmm10
	mov	rbx,QWORD PTR [rsp+save_rbx]	; restore rbx

    add        			rsp, stack_size    
    ret          

fname        endp
TEXT    ENDS

data SEGMENT READ

CONST    SEGMENT
ALIGN 16


L__max_exp2_arg            	DQ 04090000000000000h 
				DQ 04090000000000000h
L__min_exp2_arg            	DQ 0c090c80000000000h 
				DQ 0c090c80000000000h
L__real_64                 	DQ 04050000000000000h 
				DQ 04050000000000000h    																																																																																																																																																																																																																																																												; 64
L__ln_2                    	DQ 03FE62E42FEFA39EFh 
				DQ 03FE62E42FEFA39EFh
L__one_by_64               	DQ 0bF90000000000000h 
				DQ 0bF90000000000000h
L__real_one                	DQ 03ff0000000000000h 
				DQ 03ff0000000000000h
L__real_inf                	DQ 07ff0000000000000h 
				DQ 07ff0000000000000h

CONST    ENDS
data ENDS

END
