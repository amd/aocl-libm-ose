;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

include exp_tables.inc
include fm.inc

FN_PROTOTYPE_BAS64 vrsa_exp10f

p_temp		equ		0		; temporary for get/put bits operation
p_temp1		equ		10h		; temporary for get/put bits operation

save_xmm6  equ		20h
save_xmm7  equ		30h
save_xmm8  equ		40h
save_xmm9  equ		50h
save_xmm10  equ		60h
save_rsi	equ		70h
save_rbx	equ		80h
save_rdi	equ		90h


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

	mov	QWORD PTR [rsp+save_rsi],rsi	; save rsi
	.SAVEREG rsi, save_rsi
	
	mov	QWORD PTR [rsp+save_rbx],rbx	; save rbx
	.SAVEREG rbx, save_rbx	
	    
   	mov	QWORD PTR [rsp+save_rdi],rdi	; save rdi
	.SAVEREG rdi, save_rdi	
	    
    .ENDPROLOG  

    mov			rdi,rcx
    mov			rsi, rdx
    mov			rdx, r8

    cmp          edi ,00h
    jle         L__return
    cmp          rsi ,00h
    je         L__return
    cmp          rdx ,00h
    je         L__return

ALIGN 16    
L__process_next4:
    sub          rdi,04h
    cmp          rdi,-1
    jle          L__process_first123
    movdqu       xmm0,XMMWORD PTR [ rsi +  rdi  *  4 ]
    jmp         L__start
    
ALIGN 16    
L__process_first123:
    je          L__process_first3
    cmp          rdi,-3
    jl          L__return ;multiple of  all ,4elementsare processed rdi == -4
    je          L__process_first1    
    ;process first 2    
    mov          rcx,00h
    movsd        xmm0,QWORD PTR [ rsi +  rcx  *  4 ]
    jmp         L__start

ALIGN 16
L__process_first1:
    mov          rcx,00h
    movss        xmm0,DWORD PTR [ rsi +  rcx  *  4 ]
    jmp         L__start    

ALIGN 16    
L__process_first3:
    mov          rcx,01h
    movsd        xmm0,QWORD PTR [ rsi +  rcx  *  4 ]    
    dec         rcx  ; zero
    movss        xmm1,DWORD PTR [ rsi +  rcx  *  4 ]
    pslldq      xmm0,4
    por          xmm0    ,xmm1
    
ALIGN 16    
L__start:    
    movdqa        xmm10   ,xmm0; save for later use
    minps         xmm0    ,XMMWORD PTR L__max_exp10f_arg
    movhlps       xmm5  ,xmm0
    cvtps2pd      xmm5    ,xmm5;xmm5 = x2,x3
    cvtps2pd      xmm0    ,xmm0;xmm0 = (double)x

    ; x * (64/log10of(2))
    movapd      xmm8      ,xmm5;xmm8 = (double)x2    ,x3
    movapd      xmm3      ,xmm0;xmm3 = (double)x0,x1
    mulpd        xmm8  ,L__real_64_by_log10of2;xmm8 = x3 * (64/ln(2),x2 * (64/ln(2)    
    mulpd        xmm3  ,L__real_64_by_log10of2;xmm3 = x1 * (64/ln(2),x0 * (64/ln(2)

    ; n = int( x * (64/log10of(2)) )
    cvtpd2dq     xmm9  ,xmm8;xmm9 = (int)n2    ,n3
    cvtpd2dq     xmm4  ,xmm3;xmm4 = (int)n0,n1
    cvtdq2pd     xmm7  ,xmm9;xmm7 = (double)n2    ,n3
    cvtdq2pd     xmm2  ,xmm4;xmm2 = (double)n0,n1

    ; r = x - n * ln(2)/64
    ; r *= ln(10)
    mulpd       xmm7 ,L__real_log10of2_by_64;xmm7 = n3 * 1/n2 ,64* 1/64    
    mulpd       xmm2 ,L__real_log10of2_by_64;xmm2 = n1 * 1/n0 ,64* 1/64
    subpd        xmm5    ,xmm7;xmm5 = r2    ,r3
    subpd        xmm0    ,xmm2;xmm0 = r0,r1
    mulpd       xmm5 ,L__real_ln10;xmm5 = r2 ,r3= r3*ln(2),r2*ln(2)        
    mulpd       xmm0 ,L__real_ln10;xmm0 = r0 ,r1= r1*ln(2),r0*ln(2)
    movapd       xmm6    ,xmm5;xmm6 = r2    ,r3
    movapd       xmm1    ,xmm0;xmm1 = r0,r1

    ; q = r + r*r(1/2 + r*1/6)
    movapd        xmm8     ,L__real_1_by_6
    movapd        xmm3 ,L__real_1_by_6
    mulpd        xmm8 ,xmm5;xmm8 = 1/6 * 1,r3/6 * r2    
    mulpd        xmm3 ,xmm0;xmm3 = 1/6 * 1,r1/6 * r0
    mulpd        xmm5 ,xmm6;xmm5 =  r  * r    
    mulpd        xmm0 ,xmm1;xmm0 =  r  * r
    addpd        xmm8 ,L__real_1_by_2;xmm8 = 1/2 + (1/6 * r)    
    addpd        xmm3 ,L__real_1_by_2;xmm3 = 1/2 + (1/6 * r)
    mulpd        xmm5  ,xmm8;xmm5 = r*r*(1/2 + (1/6 * r))    
    mulpd        xmm0  ,xmm3;xmm0 = r*r*(1/2 + (1/6 * r))
    addpd        xmm5  ,xmm6;xmm5 = r+r*r*(1/2 + (1/6 * r))    
    addpd        xmm0  ,xmm1;xmm0 = r+r*r*(1/2 + (1/6 * r))
    
    ;j = n & 03fh
    mov          rax     ,00000003f0000003fh;rax = 03fh    ,03fh
    movd         rcx     ,xmm4;rcx = n0,n1
    and          rcx      ,rax;rcx = j0,j1
    movd         rbx     ,xmm9;rbx = n2 ,n3
    and          rbx      ,rax;rbx = j2,j3
    
    ; f + (f*q)
    lea          r10,L__two_to_jby64_table
    mov          rax,rcx
    shl          rcx,32
    shr          rcx       ,32;rcx = j0
    movsd       xmm1,QWORD PTR [ r10 + rcx  * 8 ]
    shr          rax       ,32;rax = j1
    movhpd      xmm1,QWORD PTR [ r10 +  rax  * 8 ]
    mulpd        xmm0,xmm1
    addpd        xmm0    ,xmm1
    mov          rax,rbx
    shl          rbx,32
    shr          rbx       ,32;rbx = j2
    movsd       xmm1,QWORD PTR [ r10 +  rbx  *  8 ]
    shr          rax       ,32;rax = j3
    movhpd      xmm1,QWORD PTR [ r10 +  rax  * 8 ]
    mulpd        xmm5,xmm1
    addpd        xmm5,xmm1

    ALIGN 16
    ; m = (n - j) / 64        
    psrad       xmm9     ,6
    psrad       xmm4,6
    punpckldq    xmm9,xmm9
    punpckldq    xmm4    ,xmm4
    psllq       xmm9    ,52
    psllq       xmm4,52
    paddq        xmm9    ,xmm5
    paddq        xmm4,xmm0
    cvtpd2ps     xmm5    ,xmm9
    cvtpd2ps     xmm0,xmm4
    movlhps      xmm0,xmm5

    ;;special case for any x < min_exp_arg
    ;;remove this code if the above code takes care of this
    movdqa       xmm1        ,XMMWORD PTR L__min_exp10f_arg
    cmpltps       xmm1,xmm10
    pand         xmm0    ,xmm1;make zeros to put in place of any x < min_exp2_arg    
    
    ;;special case for any x = nan
    ;;remove this code if the above code takes care of this
    movdqa       xmm1    ,xmm10
    cmpeqps       xmm10,xmm10
    pand         xmm0    ,xmm10
    addps        xmm1  ,xmm1;make qnan to put in place of any x =nan
    pandn        xmm10,xmm1
    por          xmm0       ,xmm10

    cmp          rdi,-1
    jle          L__store123
    movdqu        XMMWORD PTR [ rdx +  rdi  *  4 ],xmm0
    jmp         L__process_next4    
    
ALIGN 16    
L__store123:
    je         L__store3
    cmp         rdi,-3
    je          L__store1    
    ;store 2    
    add         rdi,02h
    movsd       QWORD PTR [ rdx +  rdi  * 4 ],xmm0    
    jmp         L__return

ALIGN 16    
L__store3:
    movdqa       xmm1,xmm0
    psrldq       xmm0,4
    inc         rdi
    movss        DWORD PTR [ rdx +  rdi  * 4 ],xmm1
    inc         rdi
    movsd       QWORD PTR [ rdx +  rdi  * 4 ],xmm0    
    jmp         L__return
    
ALIGN 16    
L__store1:
    mov         rdi,00h
    movss       DWORD PTR [ rdx +  rdi  * 4 ] ,xmm0   
    
L__return:    

	movdqa	xmm6,OWORD PTR [rsp+save_xmm6]	; restore xmm6
	movdqa	xmm7,OWORD PTR [rsp+save_xmm7]	; restore xmm7
	movdqa	xmm8,OWORD PTR [rsp+save_xmm8]	; restore xmm8	
	movdqa	xmm9,OWORD PTR [rsp+save_xmm9]	; restore xmm9
	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]	; restore xmm10
				
	mov	rsi,QWORD PTR [rsp+save_rsi]	; restore rsi
	mov	rbx,QWORD PTR [rsp+save_rbx]	; restore rbx
	mov	rdi,QWORD PTR [rsp+save_rdi]	; restore rdi	
	
    add         	 	rsp, stack_size    
    ret         

    
fname        endp
TEXT    ENDS

data SEGMENT READ

CONST    SEGMENT
ALIGN 16

L__max_exp10f_arg            DQ 0421A209B421A209Bh 
DQ 0421A209B421A209Bh
L__min_exp10f_arg            DQ 0C23369F4C23369F4h 
DQ 0C23369F4C23369F4h
L__real_64_by_log10of2       DQ 0406A934F0979A371h 
DQ 0406A934F0979A371h ; 64/log10(2)
L__real_log10of2_by_64       DQ 03F734413509F79FFh 
DQ 03F734413509F79FFh ; log10of2_by_64
L__real_ln10                 DQ 040026BB1BBB55516h 
DQ 040026BB1BBB55516h ; ln(10)


CONST    ENDS
data ENDS

END


