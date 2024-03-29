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

ALM_PROTO_BAS64 vrs4_exp2f

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


    movdqa    		xmm10,xmm0			; save for later use
    minps     		xmm0,XMMWORD PTR  L__max_exp2f_arg
    movhlps   		xmm5,xmm0
    cvtps2pd  		xmm5,xmm5			;xmm5 = x3,x2
    cvtps2pd  		xmm0,xmm0			;xmm0 = x1,x0

    							; x * (64)
    movapd    		xmm8,xmm5			;xmm8 = x3,x2
    movapd    		xmm3,xmm0			;xmm3 = x1,x0
    paddq     		xmm8,L__sixtyfour		;xmm8 = x3 * (64),x2 * (64)    
    paddq     		xmm3,L__sixtyfour		;xmm3 = x1 * (64),x0 * (64)

    							; n = int( x * (64)
    cvtpd2dq  		xmm9,xmm8			;xmm9 = (int)n3,n2    
    cvtpd2dq  		xmm4,xmm3			;xmm4 = (int)n1,n0
    cvtdq2pd  		xmm7,xmm9			;xmm7 = (double)n3,n2    
    cvtdq2pd  		xmm2,xmm4			;xmm2 = (double)n1,n0

    							; r = x - n * 1/64
    							; r *= ln(2)
    mulpd     		xmm7,L__one_by_64		;xmm7 = n3 * 1/64,n2 * 1/64    
    mulpd     		xmm2,L__one_by_64		;xmm2 = n1 * 1/64,n0 * 1/64
    subpd     		xmm5,xmm7			;xmm5 = r3,r2    
    subpd     		xmm0,xmm2			;xmm0 = r1,r0
    mulpd     		xmm5,L__ln2			;xmm5 = r3,r2 = r3*ln(2),r2*ln(2)        
    mulpd     		xmm0,L__ln2			;xmm0 = r1,r0 = r1*ln(2),r0*ln(2)
    movapd    		xmm6,xmm5			;xmm6 = r3,r2    
    movapd    		xmm1,xmm0			;xmm1 = r1,r0

    							; q
    movapd    		xmm8,L__real_1_by_6
    movapd    		xmm3,L__real_1_by_6
    mulpd     		xmm8,xmm5			;xmm8 = 1/6 * r3,1/6 * r2    
    mulpd     		xmm3,xmm0			;xmm3 = 1/6 * r1,1/6 * r0    
    mulpd     		xmm5,xmm6			;xmm5 =  r  * r    
    mulpd     		xmm0,xmm1			;xmm0 =  r  * r
    addpd     		xmm8,L__real_1_by_2		;xmm8 = 1/2 + (1/6 * r)    
    addpd     		xmm3,L__real_1_by_2		;xmm3 = 1/2 + (1/6 * r)
    mulpd     		xmm5,xmm8			;xmm5 = r*r*(1/2 + (1/6 * r))    
    mulpd     		xmm0,xmm3			;xmm0 = r*r*(1/2 + (1/6 * r))
    addpd     		xmm5,xmm6			;xmm0 = r+r*r*(1/2 + (1/6 * r))    
    addpd     		xmm0,xmm1			;xmm0 = r+r*r*(1/2 + (1/6 * r))

    							;j = n & 0x3f
    mov       		rax,00000003f0000003fh		;rax = 0x3f,0x3f    
    movd      		rcx,xmm4			;rcx = n1,n0
    and       		rcx,rax				;rcx = j1,j0
    movd      		rbx,xmm9			;rbx = n3,n2 
    and       		rbx,rax				;rbx = j3,j2

    							; f + (f*q)
    lea       		r10,L__two_to_jby64_table
    mov       		rax,rcx
    shl       		rcx,32
    shr       		rcx,32				;rcx = j0
    movsd     		xmm1,QWORD PTR [r10+rcx*8]
    shr       		rax,32				;rax = j1
    movhpd    		xmm1,QWORD PTR [r10+rax*8]
    mulpd     		xmm0,xmm1
    addpd     		xmm0,xmm1
    mov       		rax,rbx
    shl       		rbx,32
    shr       		rbx,32				;rbx = j2
    movsd     		xmm1,QWORD PTR [r10+rbx*8]
    shr       		rax,32				;rax = j3
    movhpd    		xmm1,QWORD PTR [r10+rax*8]
    mulpd     		xmm5,xmm1
    addpd     		xmm5,xmm1

    ALIGN 16
    							; m = (n - j) / 64       
    psrad     		xmm9,6
    psrad     		xmm4,6
    punpckldq 		xmm9,xmm9
    punpckldq 		xmm4,xmm4
    psllq     		xmm9,52
    psllq     		xmm4,52

    							;2 ^m * (f + (f*q))
    paddq     		xmm9,xmm5
    paddq     		xmm4,xmm0
    cvtpd2ps  		xmm5,xmm9
    cvtpd2ps  		xmm0,xmm4
    movlhps   		xmm0,xmm5

    							;																														;special case for any x < min_exp_arg
    							;																														;remove this code if the above code takes care of this
    movdqa    		xmm1,XMMWORD PTR  L__min_exp2f_arg
    cmpltps     	xmm1,xmm10
    pand      		xmm0,xmm1			;make zeros to put in place of any x < min_exp2_arg

    							;																														;special case for any x = nan
    							;																														;remove this code if the above code takes care of this
    movdqa    		xmm1,xmm10
    cmpeqps     	xmm10,xmm10
    pand      		xmm0,xmm10
    addps     		xmm1,xmm1			;make qnan to put in place of any x =nan
    pandn     		xmm10,xmm1
    por       		xmm0,xmm10


	movdqa	xmm6,OWORD PTR [rsp+save_xmm6]	; restore xmm6
	movdqa	xmm7,OWORD PTR [rsp+save_xmm7]	; restore xmm7
	movdqa	xmm8,OWORD PTR [rsp+save_xmm8]	; restore xmm8	
	movdqa	xmm9,OWORD PTR [rsp+save_xmm9]	; restore xmm9
	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]	; restore xmm10
				

	mov	rbx,QWORD PTR [rsp+save_rbx]	; restore rbx
	
    add         	rsp, stack_size    
    ret               

fname        endp
TEXT    ENDS

data SEGMENT READ

CONST    SEGMENT
ALIGN 16

L__max_exp2f_arg              	DQ 04300000043000000h 
				DQ 04300000043000000h
L__min_exp2f_arg              	DQ 0c3150000c3150000h 
				DQ 0c3150000c3150000h
L__sixtyfour                  	DQ 00060000000000000h 
				DQ 00060000000000000h  	; 64
L__one_by_64                  	DQ 03F90000000000000h 
				DQ 03F90000000000000h  	; 1/64
L__ln2                      	DQ 03FE62E42FEFA39EFh 
				DQ 03FE62E42FEFA39EFh  	; ln(2)


CONST    ENDS
data ENDS

END

