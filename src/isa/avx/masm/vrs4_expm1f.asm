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

include exp_tables.inc
include fm.inc

FN_PROTOTYPE_BAS64 vrs4_expm1f

p_temp		equ		0		; temporary for get/put bits operation
p_temp1		equ		10h		; temporary for get/put bits operation

save_xmm6  equ		20h
save_xmm7  equ		30h
save_xmm8  equ		40h
save_xmm9  equ		50h
save_xmm10  equ		60h
save_rbx	equ		70h



stack_size      EQU     0A8h



StackAllocate   MACRO size
                    sub         rsp, size
                    .ALLOCSTACK size
                ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    
	movdqa	OWORD PTR [rsp+save_xmm6],xmm6		; save xmm6
	.SAVEXMM128 xmm6, save_xmm6
	
	movdqa	OWORD PTR [rsp+save_xmm7],xmm7		; save xmm7
	.SAVEXMM128 xmm7, save_xmm7

	movdqa	OWORD PTR [rsp+save_xmm8],xmm8		; save xmm8
	.SAVEXMM128 xmm8, save_xmm8
	
	movdqa	OWORD PTR [rsp+save_xmm9],xmm9		; save xmm9
	.SAVEXMM128 xmm9, save_xmm9
	
	movdqa	OWORD PTR [rsp+save_xmm10],xmm10		; save save_xmm10
	.SAVEXMM128 xmm10, save_xmm10			

	mov	QWORD PTR [rsp+save_rbx],rbx	; save rbx
	.SAVEREG rbx, save_rbx	
	    
    .ENDPROLOG  

    movups xmm0, XMMWORD PTR [rcx]


    movdqa     			xmm10,xmm0				;save for later use
    movhlps    			xmm5,xmm0

    									;x_double = (double)x																																																																																																																																												;
    cvtps2pd   			xmm5,xmm5				;xmm0 = (double)x3,x2
    cvtps2pd   			xmm0,xmm0				;xmm0 = (double)x1,x0    

    									;x * (64/ln(2))
    movapd     			xmm8,xmm5				;xmm8 = (xouble)x3,x2    
    movapd     			xmm3,xmm0				;xmm3 = (xouble)x1,x0
    mulpd      			xmm8,L__real_64_by_log2			;xmm8 = x3 * (64/ln(2),x2 * (64/ln(2)    
    mulpd      			xmm3,L__real_64_by_log2			;xmm3 = x1 * (64/ln(2),x0 * (64/ln(2)

    									;n = int( x * (64/ln(2)) )
    cvtpd2dq   			xmm9,xmm8				;xmm9 = (int)n3,n2
    cvtpd2dq   			xmm4,xmm3				;xmm4 = (int)n1,n0
    cvtdq2pd   			xmm7,xmm9				;xmm7 = (double)n3,n2    
    cvtdq2pd   			xmm2,xmm4				;xmm2 = (double)n1,n0

    									;r = x - n * ln(2)/64
    mulpd      			xmm7,L__real_log2_by_64			;xmm7 = n3 * ln(2)/64,n2 * ln(2)/64    
    mulpd      			xmm2,L__real_log2_by_64			;xmm2 = n1 * ln(2)/64,n0 * ln(2)/64
    subpd      			xmm5,xmm7				;xmm5 = r3,r2    
    subpd      			xmm0,xmm2				;xmm0 = r1,r0
    movapd     			xmm6,xmm5				;xmm6 = r3,r2    
    movapd     			xmm1,xmm0				;xmm1 = r1,r0

    									;q
    movapd     			xmm8,L__real_1_by_6
    movapd     			xmm3,L__real_1_by_6
    mulpd      			xmm8,xmm5				;xmm8 = 1/6 * r3,1/6 * r2    
    mulpd      			xmm3,xmm0				;xmm3 = 1/6 * r1,1/6 * r0
    mulpd      			xmm5,xmm6				;xmm5 =  r  * r    
    mulpd      			xmm0,xmm1				;xmm0 =  r  * r
    addpd      			xmm8,L__real_1_by_2			;xmm8 = 1/2 + (1/6 * r)    
    addpd      			xmm3,L__real_1_by_2			;xmm3 = 1/2 + (1/6 * r)
    mulpd      			xmm5,xmm8				;xmm5 = r*r*(1/2 + (1/6 * r))    
    mulpd      			xmm0,xmm3				;xmm0 = r*r*(1/2 + (1/6 * r))
    addpd      			xmm5,xmm6				;xmm5 = r+r*r*(1/2 + (1/6 * r))    
    addpd      			xmm0,xmm1				;xmm0 = r+r*r*(1/2 + (1/6 * r))

    									;j = n & 0x3f
    mov        			rax,00000003f0000003fh			;rax = 0x3f
    movd       			rcx,xmm4				;rcx = n1,n0    
    and        			rcx,rax					;rcx = j1,j0
    movd       			rbx,xmm9				;rbx = n3,n2 
    and        			rbx,rax					;rbx = j3,j2
    movd       			xmm3,rcx
    movd       			xmm1,rbx
    movlhps    			xmm3,xmm1				;xmm3 = j3,j2,j1,j0

    									;m = (n - j) / 64    
    psrad      			xmm9,6
    psrad      			xmm4,6
    movlhps    			xmm4,xmm9				;xmm4 = m3,m2,m1,m0    

    									;process each input seperately
    lea        			r10,L__two_to_jby64_table
    movd       			eax,xmm3
    movd       			ecx,xmm4
    movhlps    			xmm6,xmm0
    mov        			r11d,4
    jmp        			L__start

ALIGN 16
L__check_n_prepare_next:
    dec        			r11d
    jz         			L__ret
    psrldq     			xmm3,4
    movd       			eax,xmm3
    psrldq     			xmm4,4
    movd       			ecx,xmm4
    psrldq     			xmm10,4
    cmp        			r11d,3
    je         			L__output1_inputq2
    cmp        			r11d,2
    je         			L__output2_inputq3
    cmp        			r11d,1
    je         			L__output3_inputq4

ALIGN 16
L__output1_inputq2:
    pxor      			xmm9,xmm9
    pslldq    			xmm0,12
    psrldq    			xmm0,12
    por       			xmm9,xmm0
    movdqa    			xmm0,xmm6
    jmp				L__start

ALIGN 16
L__output2_inputq3  :  
    pslldq    			xmm0,12
    psrldq    			xmm0,8
    por       			xmm9,xmm0
    movdqa    			xmm0,xmm5
    jmp 			L__start

ALIGN 16
L__output3_inputq4:
    pslldq   			xmm0,12
    psrldq   			xmm0,4
    por      			xmm9,xmm0
    movhlps  			xmm0,xmm5

ALIGN 16
L__start:
    ucomiss 			xmm10,DWORD PTR L__max_expm1_arg			;																														;if(x > max_expm1_arg)
    ja 				L__y_is_inf
    jp 				L__y_is_nan
    ucomiss 			xmm10,DWORD PTR L__log_OnePlus_OneByFour		;																															;if(x < log_OnePlus_OneByFour)
    jae 			L__Normal_Flow
    ucomiss 			xmm10,DWORD PTR L__log_OneMinus_OneByFour		;																															;if(x > log_OneMinus_OneByFour)
    ja 				L__Small_Arg
    ucomiss 			xmm10,DWORD PTR L__min_expm1_arg			;																														;if(x < min_expm1_arg)
    jb 				L__y_is_minusOne

ALIGN 16    
L__Normal_Flow:    
    									;f
    movsd      			xmm2,QWORD PTR  [r10+rax*8]			;xmm2 = f

    									;f*q
    mulsd      			xmm0,xmm2				;xmm0 = f*q

    									;twopmm.u64 = (1023 - DW)m) << 52																																																																																																																																												;    
    									;f - twopmm
    mov        			eax,1023
    sub        			eax,ecx
    shl        			rax,52
    movd       			xmm1,rax
    subsd      			xmm2,xmm1

    									;z = f * q + (f - twopmm.f64) 																																																																																																																																												;
    									;z = z* 2^m
    addsd      			xmm0,xmm2
    shl        			rcx,52
    movd       			xmm1,rcx
    paddq      			xmm1,xmm0
    cvtpd2ps   			xmm0,xmm1
    jmp        			L__check_n_prepare_next

ALIGN 16
L__Small_Arg:
    									;log(1-1/4) < x < log(1+1/4)
									;q = x*x*x*(A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))))																																																																																																																																												;
	movapd 			xmm0,xmm10
	ucomiss 		xmm0,L__minus_zero
	je    			L__check_n_prepare_next
	movdqa 			xmm1,xmm0
	mulss 			xmm1,L__A5_f
	addss 			xmm1,L__A4_f
	mulss			xmm1,xmm0
	addss 			xmm1,L__A3_f
	mulss 			xmm1,xmm0
	addss 			xmm1,L__A2_f
	mulss 			xmm1,xmm0
	addss 			xmm1,L__A1_f
	mulss 			xmm1,xmm0
	mulss 			xmm1,xmm0
	mulss 			xmm1,xmm0
	cvtps2pd 		xmm2,xmm0
	movdqa 			xmm0,xmm2
	mulsd 			xmm2,xmm0
	mulsd 			xmm2,L__real_1_by_2
	addsd 			xmm0,xmm2
	cvtps2pd 		xmm2,xmm1
	addsd 			xmm2,xmm0
	cvtpd2ps 		xmm0,xmm2
    jmp    			L__check_n_prepare_next

ALIGN 16
L__y_is_minusOne:
    mov   			eax,0BF800000h
    movd  			xmm0,eax
    jmp   			L__check_n_prepare_next

ALIGN 16
L__y_is_inf:
    mov   			eax,07f800000h
    movd  			xmm0,eax
    jmp   			L__check_n_prepare_next

ALIGN 16
L__y_is_nan:
    movapd			xmm0,xmm10
    addss 			xmm0,xmm0
    jmp   			L__check_n_prepare_next

L__ret:
    pslldq   			xmm0,12
    por      			xmm9,xmm0
    movdqa   			xmm0,xmm9



	movdqa	xmm6,OWORD PTR [rsp+save_xmm6]	; restore xmm6
	movdqa	xmm7,OWORD PTR [rsp+save_xmm7]	; restore xmm7
	movdqa	xmm8,OWORD PTR [rsp+save_xmm8]	; restore xmm8	
	movdqa	xmm9,OWORD PTR [rsp+save_xmm9]	; restore xmm9
	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]	; restore xmm10
				

	mov	rbx,QWORD PTR [rsp+save_rbx]	; restore rbx

	
	
    add         	 	rsp, stack_size        
    ret

fname        endp
TEXT    ENDS

data SEGMENT READ

CONST    SEGMENT
ALIGN 16

L__max_expm1_arg          DD 042B19999h
L__log_OnePlus_OneByFour  DD 03E647FBFh
L__log_OneMinus_OneByFour DD 0BE934B11h
L__min_expm1_arg          DD 0C18AA122h
L__real_64_by_log2        DQ 040571547652b82feh
			  DQ 040571547652b82feh 			;64/ln(2)
L__real_log2_by_64        DQ 03f862e42fefa39efh
			  DQ 03f862e42fefa39efh 			;log2_by_64
L__A1_f                   DD 03E2AAAAAh   
L__A2_f                   DD 03D2AAAA0h
L__A3_f                   DD 03C0889FFh     
L__A4_f                   DD 03AB64DE5h
L__A5_f                   DD 0394AB327h
L__minus_zero             DD 080000000h

CONST    ENDS
data ENDS

END
