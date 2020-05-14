;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; vrsacosf.asm
;
; A vector implementation of the cos libm function.
;
; Prototype:
;
;    vrsa_cosf(int n, float* x, float* y);
;
; Computes Cosine of x for an array of input values.
; Places the results into the supplied y array.
; Does not perform error checking.
; Denormal inputs may produce unexpected results.
; This routine computes 4 single precision Cosine values at a time.
; The four values are passed as packed single in xmm10.
; The four results are returned as packed singles in xmm10.
; Note that this represents a non-standard ABI usage, as no ABI
; ( and indeed C) currently allows returning 2 values for a function.
; It is expected that some compilers may be able to take advantage of this
; interface when implementing vectorized loops.  Using the array implementation
; of the routine requires putting the inputs into memory, and retrieving
; the results from memory.  This routine eliminates the need for this
; overhead if the data does not already reside in memory.
; Author: Harsha Jagasia
; Email:  harsha.jagasia@amd.com

CONST	SEGMENT
__real_7fffffffffffffff DQ 07fffffffffffffffh	;Sign bit zero
			DQ 07fffffffffffffffh
__real_3ff0000000000000 DQ 03ff0000000000000h	; 1.0
			DQ 03ff0000000000000h
__real_v2p__27		DQ 03e40000000000000h	; 2p-27
			DQ 03e40000000000000h
__real_3fe0000000000000 DQ 03fe0000000000000h	; 0.5
			DQ 03fe0000000000000h
__real_3fc5555555555555 DQ 03fc5555555555555h	; 0.166666666666
			DQ 03fc5555555555555h
__real_3fe45f306dc9c883 DQ 03fe45f306dc9c883h	; twobypi
			DQ 03fe45f306dc9c883h
__real_3ff921fb54400000 DQ 03ff921fb54400000h	; piby2_1
			DQ 03ff921fb54400000h
__real_3dd0b4611a626331 DQ 03dd0b4611a626331h	; piby2_1tail
			DQ 03dd0b4611a626331h
__real_3dd0b4611a600000 DQ 03dd0b4611a600000h	; piby2_2
			DQ 03dd0b4611a600000h
__real_3ba3198a2e037073 DQ 03ba3198a2e037073h	; piby2_2tail
			DQ 03ba3198a2e037073h
__real_fffffffff8000000 DQ 0fffffffff8000000h	; mask for stripping head and tail
			DQ 0fffffffff8000000h
__real_8000000000000000	DQ 08000000000000000h	; -0  or signbit
			DQ 08000000000000000h
__reald_one_one		DQ 00000000100000001h	;
			dq 0
__reald_two_two		DQ 00000000200000002h	;
			dq 0
__reald_one_zero	DQ 00000000100000000h	; sin_cos_filter
			dq 0
__reald_zero_one	DQ 00000000000000001h	;
			dq 0
__reald_two_zero	DQ 00000000200000000h	;
			dq 0
__realq_one_one		DQ 00000000000000001h	;
			DQ 00000000000000001h	;
__realq_two_two		DQ 00000000000000002h	;
			DQ 00000000000000002h	;
__real_1_x_mask		DQ 0ffffffffffffffffh	;
			DQ 03ff0000000000000h	;
__real_zero		DQ 00000000000000000h	;
			DQ 00000000000000000h	;
__real_one		DQ 00000000000000001h	;
			DQ 00000000000000001h	;

cosarray:
	DQ	03FA5555555502F31h		;  0.0416667			c1
	DQ	03FA5555555502F31h
	DQ	0BF56C16BF55699D7h		; -0.00138889			c2
	DQ	0BF56C16BF55699D7h
	DQ	03EFA015C50A93B49h		;  2.48016e-005			c3
	DQ	03EFA015C50A93B49h
	DQ	0BE92524743CC46B8h		; -2.75573e-007			c4	
	DQ	0BE92524743CC46B8h

sinarray:
	DQ	0BFC555555545E87Dh		; -0.166667	   		s1
	DQ	0BFC555555545E87Dh
	DQ	03F811110DF01232Dh		; 0.00833333	   		s2
	DQ	03F811110DF01232Dh
	DQ	0BF2A013A88A37196h		; -0.000198413			s3
	DQ	0BF2A013A88A37196h
	DQ	03EC6DBE4AD1572D5h		; 2.75573e-006			s4
	DQ	03EC6DBE4AD1572D5h

sincosarray:
	DQ	0BFC555555545E87Dh		; -0.166667	   		s1
	DQ	03FA5555555502F31h		; 0.0416667		   	c1
	DQ	03F811110DF01232Dh		; 0.00833333	   		s2
	DQ	0BF56C16BF55699D7h
	DQ	0BF2A013A88A37196h		; -0.000198413			s3
	DQ	03EFA015C50A93B49h
	DQ	03EC6DBE4AD1572D5h		; 2.75573e-006			s4
	DQ	0BE92524743CC46B8h

cossinarray:
	DQ	03FA5555555502F31h		; 0.0416667		   	c1
	DQ	0BFC555555545E87Dh		; -0.166667	   		s1
	DQ	0BF56C16BF55699D7h		;				c2
	DQ	03F811110DF01232Dh		
	DQ	03EFA015C50A93B49h		;				c3
	DQ	0BF2A013A88A37196h		
	DQ	0BE92524743CC46B8h		;				c4
	DQ	03EC6DBE4AD1572D5h		

CONST	ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; define local variable storage offsets
p_temp		equ		0		; temporary for get/put bits operation
p_temp1		equ		10h		; temporary for get/put bits operation

save_xmm6	equ		20h		; temporary for get/put bits operation
save_xmm7	equ		30h		; temporary for get/put bits operation
save_xmm8	equ		40h		; temporary for get/put bits operation
save_xmm9	equ		50h		; temporary for get/put bits operation
save_xmm0	equ		60h		; temporary for get/put bits operation
save_xmm11	equ		70h		; temporary for get/put bits operation
save_xmm12	equ		80h		; temporary for get/put bits operation
save_xmm13	equ		90h		; temporary for get/put bits operation
save_xmm14	equ		0A0h		; temporary for get/put bits operation
save_xmm15	equ		0B0h		; temporary for get/put bits operation

r		equ		0C0h		; pointer to r for remainder_piby2
rr		equ		0D0h		; pointer to r for remainder_piby2
region		equ		0E0h		; pointer to r for remainder_piby2

r1		equ		0F0h		; pointer to r for remainder_piby2
rr1		equ		0100h		; pointer to r for remainder_piby2
region1		equ		0110h		; pointer to r for remainder_piby2

p_temp2		equ		0120h		; temporary for get/put bits operation
p_temp3		equ		0130h		; temporary for get/put bits operation

p_temp4		equ		0140h		; temporary for get/put bits operation
p_temp5		equ		0150h		; temporary for get/put bits operation

p_original	equ		0160h		; original x
p_mask		equ		0170h		; original x
p_sign		equ		0180h		; original x

p_original1	equ		0190h		; original x
p_mask1		equ		01A0h		; original x
p_sign1		equ		01B0h		; original x

save_r12	equ		01C0h		; temporary for get/put bits operation
save_r13	equ		01D0h		; temporary for get/put bits operation

save_xa		equ		01E0h		;qword
save_ya		equ		01F0h		;qword

save_nv		equ		0200h		;qword
p_iter		equ		0210h		; qword	storage for number of loop iterations
save_rdi	equ     0220h 
save_rsi	equ     0230h 

stack_size	equ		0248h


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include fm.inc


FN_PROTOTYPE_BAS64 vrsa_cosf


fname_remainder_piby2d2f	TEXTEQU <__amd_remainder_piby2d2f>


EXTRN fname_remainder_piby2d2f:NEAR

.DATA
align 8


	evencos_oddsin_tbl	label	qword

		dq	coscos_coscos_piby4		; 0		*	; Done
		dq	coscos_cossin_piby4		; 1		+	; Done
		dq	coscos_sincos_piby4		; 2			; Done
		dq	coscos_sinsin_piby4		; 3		+	; Done

		dq	cossin_coscos_piby4		; 4			; Done
		dq	cossin_cossin_piby4		; 5		*	; Done
		dq	cossin_sincos_piby4		; 6			; Done
		dq	cossin_sinsin_piby4		; 7			; Done

		dq	sincos_coscos_piby4		; 8			; Done
		dq	sincos_cossin_piby4		; 9			; TBD
		dq	sincos_sincos_piby4		; 10		*	; Done
		dq	sincos_sinsin_piby4		; 11			; Done

		dq	sinsin_coscos_piby4		; 12			; Done
		dq	sinsin_cossin_piby4		; 13		+	; Done
		dq	sinsin_sincos_piby4		; 14			; Done
		dq	sinsin_sinsin_piby4		; 15		*	; Done


;TEXT	SEGMENT	   page	   'CODE'
.CODE

; parameters are passed in by Microsoft C as:
; ecx - int n
; rdx - float *x
; r8  - float *y


	PUBLIC  fname
fname	proc	frame
    cmp          ecx ,00h
    jle         L__return
    cmp          rdx ,00h
    je         L__return
    cmp          r8 ,00h
    je         L__return

	sub		rsp,stack_size
	.ALLOCSTACK	stack_size					; unwind data, needed since we call
								; remainder_piby2 and could have exceptions								; but it costs no performance to include it.

	movdqa	OWORD PTR [rsp+020h],xmm6	; save xmm6
	.SAVEXMM128 xmm6, 020h

	movdqa	OWORD PTR [rsp+030h],xmm7	; save xmm7
	.SAVEXMM128 xmm7, 030h

	movdqa	OWORD PTR [rsp+040h],xmm8	; save xmm8
	.SAVEXMM128 xmm8, 040h

	movdqa	OWORD PTR [rsp+050h],xmm9	; save xmm9
	.SAVEXMM128 xmm9, 050h

	movdqa	OWORD PTR [rsp+060h],xmm10	; save xmm0
	.SAVEXMM128 xmm10, 060h

	movdqa	OWORD PTR [rsp+070h],xmm11	; save xmm11
	.SAVEXMM128 xmm11, 070h

	movdqa	OWORD PTR [rsp+080h],xmm12	; save xmm12
	.SAVEXMM128 xmm12, 080h

	movdqa	OWORD PTR [rsp+090h],xmm13	; save xmm13
	.SAVEXMM128 xmm13, 090h

	mov	QWORD PTR [rsp+save_r12],r12	; save r12
	.SAVEREG r12, save_r12

	mov	QWORD PTR [rsp+save_r13],r13	; save r13
	.SAVEREG r13, save_r13
	
	mov	QWORD PTR [rsp+save_rdi],rdi	; save rdi
	.SAVEREG rdi, save_rdi	

	mov	QWORD PTR [rsp+save_rsi],rsi	; save rsi
	.SAVEREG rsi, save_rsi	
	
	.ENDPROLOG

;START PROCESS INPUT
; save the arguments
	mov		QWORD PTR [rsp+save_xa],rdx	; save x_array pointer
	mov		QWORD PTR [rsp+save_ya],r8	; save y_array pointer
	mov		rax,rcx
	mov		QWORD PTR [rsp+save_nv],rcx	; save number of values
; see if too few values to call the main loop
	shr		rax,2				; get number of iterations
	jz		__vrsa_cleanup			; jump if only single calls
; prepare the iteration counts
	mov		QWORD PTR [rsp+p_iter],rax	; save number of iterations
	shl		rax,2
	sub		rcx,rax				; compute number of extra single calls
	mov		QWORD PTR [rsp+save_nv],rcx	; save number of left over values


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;START LOOP
align 16
__vrsa_top:
; build the input _m128d
	mov		rsi,QWORD PTR [rsp+save_xa]	; get x_array pointer
	movlps	xmm0,QWORD PTR [rsi]
	movhps	xmm0,QWORD PTR [rsi+8]

	prefetch	QWORD PTR [rsi+32]
	add		rsi,16
	mov		QWORD PTR [rsp+save_xa],rsi	; save x_array pointer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; V4 START

	movhlps		xmm8, xmm0
	cvtps2pd	xmm10,xmm0			; convert input to double.
	cvtps2pd	xmm1, xmm8			; convert input to double.

movdqa	xmm6, xmm10
movdqa	xmm7, xmm1
movapd	xmm2, OWORD PTR __real_7fffffffffffffff
;movdqa	OWORD PTR p_original[rsp], xmm10
;movdqa	OWORD PTR p_original1[rsp], xmm1

andpd 	xmm10, xmm2	;Unsign
andpd 	xmm1, xmm2	;Unsign

movd	rax, xmm10				;rax is lower arg
movhpd	QWORD PTR p_temp[rsp+8], xmm10		;
mov    	rcx, QWORD PTR p_temp[rsp+8]		;rcx = upper arg
movd	r8, xmm1				;r8 is lower arg
movhpd	QWORD PTR p_temp1[rsp+8], xmm1		;
mov    	r9, QWORD PTR p_temp1[rsp+8]		;r9 = upper arg


movdqa	xmm12,xmm10
movdqa	xmm13,xmm1

pcmpgtd		xmm12, xmm6
pcmpgtd		xmm13, xmm7
movdqa		xmm6, xmm12
movdqa		xmm7, xmm13
psrldq		xmm12, 4
psrldq		xmm13, 4
psrldq		xmm6, 8
psrldq		xmm7, 8

mov 	rdx, 3FE921FB54442D18h			;piby4	+
mov	r10, 411E848000000000h			;5e5	+
movapd	xmm4, OWORD PTR __real_3fe0000000000000	;0.5 for later use	+

por	xmm12, xmm6
por	xmm13, xmm7

;DELETE
;movd	r12, xmm12				;Move Sign to gpr **
;movd	r13, xmm13				;Move Sign to gpr **
;DELETE

movapd	xmm2, xmm10				;x0
movapd	xmm3, xmm1				;x1
movapd	xmm6, xmm10				;x0
movapd	xmm7, xmm1				;x1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm2 = x, xmm4 =0.5/t, xmm6 =x
; xmm3 = x, xmm5 =0.5/t, xmm7 =x
align 16
either_or_both_arg_gt_than_piby4:
	cmp	rax, r10
	jae	first_or_next3_arg_gt_5e5

	cmp	rcx, r10
	jae	second_or_next2_arg_gt_5e5

	cmp	r8, r10
	jae	third_or_fourth_arg_gt_5e5

	cmp	r9, r10
	jae	fourth_arg_gt_5e5


;      /* Find out what multiple of piby2 */
;        npi2  = (int)(x * twobypi + 0.5);
	movapd	xmm10, XMMWORD PTR __real_3fe45f306dc9c883
	mulpd	xmm2, xmm10						; * twobypi
	mulpd	xmm3, xmm10						; * twobypi

	addpd	xmm2,xmm4						; +0.5, npi2
	addpd	xmm3,xmm4						; +0.5, npi2

	movapd	xmm10,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	movapd	xmm1,XMMWORD PTR __real_3ff921fb54400000		; piby2_1

	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	cvttpd2dq	xmm5,xmm3					; convert packed double to packed integers

	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	movapd	xmm9,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2

	cvtdq2pd	xmm2,xmm4					; and back to double.
	cvtdq2pd	xmm3,xmm5					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */

	movd	r8, xmm4						; Region
	movd	r9, xmm5						; Region

	mov 	rdx, 	QWORD PTR __reald_one_zero			;compare value for cossin path
	mov	r10,  r8
	mov	r11,  r9

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm10,xmm2						; npi2 * piby2_1;
       mulpd	xmm1,xmm3						; npi2 * piby2_1;

;      rtail  = npi2 * piby2_2;
       mulpd	xmm8,xmm2						; rtail
       mulpd	xmm9,xmm3						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm6,xmm10						; rhead  = x - npi2 * piby2_1;
       subpd	xmm7,xmm1						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm10, xmm6						; t
       movapd	xmm1, xmm7						; t

;      rhead  = t - rtail;
       subpd	xmm10, xmm8						; rhead
       subpd	xmm1, xmm9						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm2,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail
       mulpd	xmm3,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm6,xmm10						; t-rhead
       subpd	xmm7,xmm1						; t-rhead

       subpd	xmm8,xmm6						; - ((t - rhead) - rtail)
       subpd	xmm9,xmm7						; - ((t - rhead) - rtail)

       addpd	xmm8,xmm2						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);
       addpd	xmm9,xmm3						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = npi2 (int), xmm10 =rhead, xmm8 =rtail, r8 = region, r10 = region, r12 = Sign
; xmm5  = npi2 (int), xmm1 =rhead, xmm9 =rtail,  r9 = region, r11 = region, r13 = Sign

	and	r8, 	QWORD PTR __reald_one_one		;odd/even region for cos/sin
	and	r9, 	QWORD PTR __reald_one_one		;odd/even region for cos/sin

; NEW
	mov	rax, r10
	mov	rcx, r11

	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
	shr	r11, 1						;~AB+A~B, A is sign and B is upper bit of region

	xor	r10, rax
	xor	r11, rcx
	and	r10, __reald_one_one				;(~AB+A~B)&1
	and	r11, __reald_one_one				;(~AB+A~B)&1

	mov	r12,	r10
	mov	r13,	r11

	and	r12,	rdx				;mask out the lower sign bit leaving the upper sign bit
	and	r13,	rdx				;mask out the lower sign bit leaving the upper sign bit

	shl	r10,	63				;shift lower sign bit left by 63 bits
	shl	r11,	63				;shift lower sign bit left by 63 bits
	shl	r12,	31				;shift upper sign bit left by 31 bits
	shl	r13,	31				;shift upper sign bit left by 31 bits

	mov 	QWORD PTR p_sign[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_sign[rsp+8],r12		;write out upper sign bit
	mov 	QWORD PTR p_sign1[rsp],r11		;write out lower sign bit
	mov 	QWORD PTR p_sign1[rsp+8],r13		;write out upper sign bit
; NEW

;DELETE

;	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
;	shr	r11, 1						;~AB+A~B, A is sign and B is upper bit of region

;	mov	rax, r10
;	mov	rcx, r11

;	not 	r12						;ADDED TO CHANGE THE LOGIC
;	not 	r13						;ADDED TO CHANGE THE LOGIC
;	and	r10, r12
;	and	r11, r13

;	not	rax
;	not	rcx
;	not	r12
;	not	r13
;	and	rax, r12
;	and	rcx, r13

;	or	r10, rax
;	or	r11, rcx
;	and	r10, __reald_one_one				;(~AB+A~B)&1
;	and	r11, __reald_one_one				;(~AB+A~B)&1

;	mov	r12,	r10
;	mov	r13,	r11

;	and	r12,	rdx				;mask out the lower sign bit leaving the upper sign bit
;	and	r13,	rdx				;mask out the lower sign bit leaving the upper sign bit

;	shl	r10,	63				;shift lower sign bit left by 63 bits
;	shl	r11,	63				;shift lower sign bit left by 63 bits
;	shl	r12,	31				;shift upper sign bit left by 31 bits
;	shl	r13,	31				;shift upper sign bit left by 31 bits

;	mov 	QWORD PTR p_sign[rsp],r10		;write out lower sign bit
;	mov 	QWORD PTR p_sign[rsp+8],r12		;write out upper sign bit
;	mov 	QWORD PTR p_sign1[rsp],r11		;write out lower sign bit
;	mov 	QWORD PTR p_sign1[rsp+8],r13		;write out upper sign bit

; DELETE

; GET_BITS_DP64(rhead-rtail, uy);			   		; originally only rhead
; xmm4  = Sign, xmm10 =rhead, xmm8 =rtail
; xmm5  = Sign, xmm1 =rhead, xmm9 =rtail
	movapd	xmm6,xmm10						; rhead
	movapd	xmm7,xmm1						; rhead

	subpd	xmm10,xmm8						; r = rhead - rtail
	subpd	xmm1,xmm9						; r = rhead - rtail

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = Sign, xmm10 = r, xmm6 =rhead, xmm8 =rtail
; xmm5  = Sign, xmm1 = r, xmm7 =rhead, xmm9 =rtail

;	subpd	xmm6, xmm10				;rr=rhead-r
;	subpd	xmm7, xmm1				;rr=rhead-r

	mov	rax,  r8
	mov	rcx,  r9

	movapd	xmm2,xmm10				; move r for r2
	movapd	xmm3,xmm1				; move r for r2

	mulpd	xmm2,xmm10				; r2
	mulpd	xmm3,xmm1				; r2

;	subpd	xmm6, xmm8				;rr=(rhead-r) -rtail
;	subpd	xmm7, xmm9				;rr=(rhead-r) -rtail


	and	rax, QWORD PTR __reald_zero_one
	and	rcx, QWORD PTR __reald_zero_one
	shr	r8,  31
	shr	r9,  31
	or	rax, r8
	or	rcx, r9
	shl	rcx, 2
	or	rax, rcx

	lea	rcx, QWORD PTR evencos_oddsin_tbl	
	jmp	QWORD PTR [rcx + rax*8]	;Jmp table for cos/sin calculation based on even/odd region



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
first_or_next3_arg_gt_5e5:
; rax, rcx, r8, r9

	cmp	rcx, r10				;is upper arg >= 5e5
	jae	both_arg_gt_5e5

lower_arg_gt_5e5:
; Upper Arg is < 5e5, Lower arg is >= 5e5
; xmm10, xmm2, xmm6 = x, xmm4 = 0.5
; Be sure not to use xmm1, xmm3 and xmm7
; Use xmm5, xmm8, xmm0, xmm12
;	    xmm9, xmm11, xmm13


	movlpd	QWORD PTR r[rsp], xmm10		;Save lower fp arg for remainder_piby2 call
	movhlps	xmm10, xmm10			;Needed since we want to work on upper arg
	movhlps	xmm2, xmm2
	movhlps	xmm6, xmm6

; Work on Upper arg
; Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm4					; xmm2 = npi2=(x*twobypi+0.5)
	movsd	xmm8,QWORD PTR __real_3ff921fb54400000		; xmm8 = piby2_1
	cvttsd2si	ecx,xmm2				; ecx = npi2 trunc to ints
	movsd	xmm0,QWORD PTR __real_3dd0b4611a600000		; xmm0 = piby2_2
	cvtsi2sd	xmm2,ecx				; xmm2 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm8,xmm2					; npi2 * piby2_1
	subsd	xmm6,xmm8					; xmm6 = rhead =(x-npi2*piby2_1)
	movsd	xmm12,QWORD PTR __real_3ba3198a2e037073		; xmm12 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm6					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm0,xmm2					; xmm1 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm6, xmm0					; xmm6 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm12,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm6					; t-rhead
       subsd	xmm0,xmm5					; (rtail-(t-rhead))
       addsd	xmm0,xmm12					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp+4], ecx			; store upper region
       movsd	xmm10,xmm6
       subsd	xmm10,xmm0					; xmm10 = r=(rhead-rtail)
       subsd	xmm6, xmm10					; rr=rhead-r
       subsd	xmm6, xmm0					; xmm6 = rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r[rsp+8], xmm10			; store upper r
       movlpd	QWORD PTR rr[rsp+8], xmm6			; store upper rr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Note that volatiles will be trashed by the call
;We will construct r, rr, region and sign

; Work on Lower arg
	mov		r11,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrs4_cosf_lower_naninf

	mov	 QWORD PTR p_temp[rsp], r8
	mov	 QWORD PTR p_temp2[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

;	lea	 r9, 	QWORD PTR region[rsp]			; lower arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 r8, 	QWORD PTR region[rsp]			; lower arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r[rsp]

; changed input from xmm10 to xmm0
;	movlpd	 xmm10,  QWORD PTR r[rsp]	;Restore lower fp arg for remainder_piby2 call
;	movlpd	 xmm0,   QWORD PTR r[rsp]	;Restore lower fp arg for remainder_piby2 call
	mov	 rcx,    QWORD PTR r[rsp]	;Restore lower fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r8, 	QWORD PTR p_temp[rsp]
	mov	 r9, 	QWORD PTR p_temp2[rsp]
	movapd	 xmm1, 	OWORD PTR p_temp1[rsp]
	movapd	 xmm3, 	OWORD PTR p_temp3[rsp]
	movapd	 xmm7,	OWORD PTR p_temp5[rsp]
	jmp 	@F

__vrs4_cosf_lower_naninf:
;	mov	rax, QWORD PTR p_original[rsp]			; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				; r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr[rsp], r10				; rr = 0
	mov	DWORD PTR region[rsp], r10d			; region =0

align 16
@@:

	jmp check_next2_args


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
both_arg_gt_5e5:
;Upper Arg is >= 5e5, Lower arg is >= 5e5
; rax, rcx, r8, r9
; xmm10, xmm2, xmm6 = x, xmm4 = 0.5

	movhlps xmm6, xmm10		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrs4_cosf_lower_naninf_of_both_gt_5e5

	mov	 QWORD PTR p_temp[rsp], rcx			;Save upper arg
	mov	 QWORD PTR p_temp2[rsp], r8
	mov	 QWORD PTR p_temp4[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

;	lea	 r9, 	QWORD PTR region[rsp]			;lower arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 r8, 	QWORD PTR region[rsp]			;lower arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r[rsp]

; added ins- changed input from xmm10 to xmm0
;	movsd	xmm0, xmm10
	movd	rcx,  xmm10

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r8, 	QWORD PTR p_temp2[rsp]
	mov	 r9, 	QWORD PTR p_temp4[rsp]
	movapd	 xmm1, 	OWORD PTR p_temp1[rsp]
	movapd	 xmm3, 	OWORD PTR p_temp3[rsp]
	movapd	 xmm7,	OWORD PTR p_temp5[rsp]

	mov	 rcx, QWORD PTR p_temp[rsp]			;Restore upper arg
	jmp 	@F

__vrs4_cosf_lower_naninf_of_both_gt_5e5:				;lower arg is nan/inf
;	mov	rax, QWORD PTR p_original[rsp]
	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				;r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr[rsp], r10				;rr = 0
	mov	DWORD PTR region[rsp], r10d			;region = 0

align 16
@@:
	mov		r11,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r11
	and		r10,rcx
	cmp		r10,r11
	jz		__vrs4_cosf_upper_naninf_of_both_gt_5e5


	mov	 QWORD PTR p_temp2[rsp], r8
	mov	 QWORD PTR p_temp4[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

;	lea	 r9, 	QWORD PTR region[rsp+4]			;upper arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 r8, 	QWORD PTR region[rsp+4]			;upper arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r[rsp+8]

; changed input from xmm10 to xmm0
;	movapd	 xmm10, xmm6			;Restore upper fp arg for remainder_piby2 call
;	movsd	 xmm0,  xmm6			;Restore upper fp arg for remainder_piby2 call
	movd	 rcx,   xmm6			;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r8, QWORD PTR p_temp2[rsp]
	mov	 r9, QWORD PTR p_temp4[rsp]
	movapd	 xmm1, OWORD PTR p_temp1[rsp]
	movapd	 xmm3, OWORD PTR p_temp3[rsp]
	movapd	 xmm7,OWORD PTR p_temp5[rsp]

	jmp 	@F

__vrs4_cosf_upper_naninf_of_both_gt_5e5:
;	mov	rcx, QWORD PTR p_original[rsp+8]		;upper arg is nan/inf
;	movd	rcx, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				;r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr[rsp+8], r10			;rr = 0
	mov	DWORD PTR region[rsp+4], r10d			;region = 0

align 16
@@:
	jmp check_next2_args

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
second_or_next2_arg_gt_5e5:

; Upper Arg is >= 5e5, Lower arg is < 5e5
; rax, rcx, r8, r9
; xmm10, xmm2, xmm6 = x, xmm4 = 0.5
; Do not use xmm1, xmm3, xmm7
; Restore xmm4 and xmm1, xmm3, xmm7
; Can use xmm8, xmm0, xmm12
;   xmm5, xmm9, xmm11, xmm13

	movhpd	QWORD PTR r[rsp+8], xmm10	;Save upper fp arg for remainder_piby2 call
;	movlhps	xmm10, xmm10			;Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
;	movlhps	xmm2, xmm2
;	movlhps	xmm6, xmm6

; Work on Lower arg
; Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm4					; xmm2 = npi2=(x*twobypi+0.5)
	movsd	xmm8,QWORD PTR __real_3ff921fb54400000		; xmm3 = piby2_1
	cvttsd2si	eax,xmm2				; ecx = npi2 trunc to ints
	movsd	xmm0,QWORD PTR __real_3dd0b4611a600000		; xmm1 = piby2_2
	cvtsi2sd	xmm2,eax				; xmm2 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm8,xmm2					; npi2 * piby2_1
	subsd	xmm6,xmm8					; xmm6 = rhead =(x-npi2*piby2_1)
	movsd	xmm12,QWORD PTR __real_3ba3198a2e037073		; xmm7 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm6					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm0,xmm2					; xmm1 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm6, xmm0					; xmm6 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm12,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm6					; t-rhead
       subsd	xmm0,xmm5					; (rtail-(t-rhead))
       addsd	xmm0,xmm12					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp], eax			; store upper region

;       movsd	xmm10,xmm6
;       subsd	xmm10,xmm0					; xmm10 = r=(rhead-rtail)
;       subsd	xmm6, xmm10					; rr=rhead-r
;       subsd	xmm6, xmm0					; xmm6 = rr=((rhead-r) -rtail)

        subsd	xmm6,xmm0					; xmm10 = r=(rhead-rtail)

;       movlpd	QWORD PTR r[rsp], xmm10				; store upper r
;       movlpd	QWORD PTR rr[rsp], xmm6				; store upper rr

        movlpd	QWORD PTR r[rsp], xmm6				; store upper r


;Work on Upper arg
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
	mov		r11,07ff0000000000000h			; is upper arg nan/inf
	mov		r10,r11
	and		r10,rcx
	cmp		r10,r11
	jz		__vrs4_cosf_upper_naninf

	mov	QWORD PTR p_temp[rsp], r8
	mov	QWORD PTR p_temp2[rsp], r9
	movapd	OWORD PTR p_temp1[rsp], xmm1
	movapd	OWORD PTR p_temp3[rsp], xmm3
	movapd	OWORD PTR p_temp5[rsp], xmm7

;	lea	 r9, 	QWORD PTR region[rsp+4]			; upper arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 r8, 	QWORD PTR region[rsp+4]			; upper arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r[rsp+8]

; changed input from xmm10 to xmm0
;	movlpd	 xmm10, QWORD PTR r[rsp+8]	;Restore upper fp arg for remainder_piby2 call
;	movlpd	 xmm0,  QWORD PTR r[rsp+8]	;Restore upper fp arg for remainder_piby2 call
	mov	 rcx,   QWORD PTR r[rsp+8]	;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	mov	r8,	QWORD PTR p_temp[rsp]
	mov	r9,	QWORD PTR p_temp2[rsp]
	movapd	xmm1, 	OWORD PTR p_temp1[rsp]
	movapd	xmm3, 	OWORD PTR p_temp3[rsp]
	movapd	xmm7, 	OWORD PTR p_temp5[rsp]
	jmp 	@F

__vrs4_cosf_upper_naninf:
;	mov	rcx, QWORD PTR p_original[rsp+8]		; upper arg is nan/inf
;	mov	rcx, QWORD PTR r[rsp+8]				; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				; r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr[rsp+8], r10			; rr = 0
	mov	DWORD PTR region[rsp+4], r10d			; region =0

align 16
@@:

	jmp check_next2_args


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
check_next2_args:

	mov	r10, 411E848000000000h			;5e5	+

	cmp	r8, r10
	jae	first_second_done_third_or_fourth_arg_gt_5e5

	cmp	r9, r10
	jae	first_second_done_fourth_arg_gt_5e5



; Work on next two args, both < 5e5
; xmm1, xmm3, xmm5 = x, xmm4 = 0.5

	movapd	xmm4, OWORD PTR __real_3fe0000000000000			;Restore 0.5

	mulpd	xmm3, XMMWORD PTR __real_3fe45f306dc9c883						; * twobypi
	addpd	xmm3,xmm4						; +0.5, npi2
	movapd	xmm1,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttpd2dq	xmm5,xmm3					; convert packed double to packed integers
	movapd	xmm9,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtdq2pd	xmm3,xmm5					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */
	movd	QWORD PTR region1[rsp], xmm5						; Region

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm1,xmm3						; npi2 * piby2_1;

;      rtail  = npi2 * piby2_2;
       mulpd	xmm9,xmm3						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm7,xmm1						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm1, xmm7						; t

;      rhead  = t - rtail;
       subpd	xmm1, xmm9						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm3,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm7,xmm1						; t-rhead
       subpd	xmm9,xmm7						; - ((t - rhead) - rtail)
       addpd	xmm9,xmm3						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

;       movapd	xmm7,xmm1						; rhead
       subpd	xmm1,xmm9						; r = rhead - rtail
       movapd	OWORD PTR r1[rsp], xmm1

;       subpd	xmm7, xmm1						; rr=rhead-r
;       subpd	xmm7, xmm9						; rr=(rhead-r) -rtail
;       movapd	OWORD PTR rr1[rsp], xmm7

	jmp	__vrs4_cosf_reconstruct


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
third_or_fourth_arg_gt_5e5:
;first two args are < 5e5, third arg >= 5e5, fourth arg >= 5e5 or < 5e5
; rax, rcx, r8, r9
; xmm10, xmm2, xmm6 = x, xmm4 = 0.5
; Do not use xmm1, xmm3, xmm7
; Can use 	xmm9, xmm11, xmm13
; 	xmm5,   xmm8, xmm0, xmm12
; Restore xmm4

; Work on first two args, both < 5e5



	mulpd	xmm2, XMMWORD PTR __real_3fe45f306dc9c883		; * twobypi
	addpd	xmm2,xmm4						; +0.5, npi2
	movapd	xmm10,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtdq2pd	xmm2,xmm4					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */
	movd	QWORD PTR region[rsp], xmm4				; Region

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm10,xmm2						; npi2 * piby2_1;
;      rtail  = npi2 * piby2_2;
       mulpd	xmm8,xmm2						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm6,xmm10						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm10, xmm6						; t

;      rhead  = t - rtail;
       subpd	xmm10, xmm8						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm2,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm6,xmm10						; t-rhead
       subpd	xmm8,xmm6						; - ((t - rhead) - rtail)
       addpd	xmm8,xmm2						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

;       movapd	xmm6,xmm10						; rhead
       subpd	xmm10,xmm8						; r = rhead - rtail
       movapd	OWORD PTR r[rsp], xmm10

;       subpd	xmm6, xmm10						; rr=rhead-r
;       subpd	xmm6, xmm8						; rr=(rhead-r) -rtail
;       movapd	OWORD PTR rr[rsp], xmm6


; Work on next two args, third arg >= 5e5, fourth arg >= 5e5 or < 5e5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
first_second_done_third_or_fourth_arg_gt_5e5:
; rax, rcx, r8, r9
; xmm10, xmm2, xmm6 = x, xmm4 = 0.5


	mov	r10, 411E848000000000h			;5e5	+
	cmp	r9, r10
	jae	both_arg_gt_5e5_higher


; Upper Arg is <5e5, Lower arg is >= 5e5
; r8, r9
; xmm1, xmm3, xmm7 = x, xmm4 = 0.5

	movlpd	QWORD PTR r1[rsp], xmm1		;Save lower fp arg for remainder_piby2 call
	movhlps	xmm1, xmm1			;Needed since we want to work on upper arg
	movhlps	xmm3, xmm3
	movhlps	xmm7, xmm7


; Work on Upper arg
; Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
	movapd	xmm4, OWORD PTR __real_3fe0000000000000		; Restore 0.5

	mulsd	xmm3,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm3,xmm4					; xmm3 = npi2=(x*twobypi+0.5)
	movsd	xmm2,QWORD PTR __real_3ff921fb54400000		; xmm2 = piby2_1
	cvttsd2si	r9d,xmm3				; r9d = npi2 trunc to ints
	movsd	xmm10,QWORD PTR __real_3dd0b4611a600000		; xmm10 = piby2_2
	cvtsi2sd	xmm3,r9d				; xmm3 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm2,xmm3					; npi2 * piby2_1
	subsd	xmm7,xmm2					; xmm7 = rhead =(x-npi2*piby2_1)
	movsd	xmm6,QWORD PTR __real_3ba3198a2e037073		; xmm6 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm7					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm10,xmm3					; xmm10 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm7, xmm10					; xmm7 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm6,xmm3     					; npi2 * piby2_2tail
       subsd	xmm5,xmm7					; t-rhead
       subsd	xmm10,xmm5					; (rtail-(t-rhead))
       addsd	xmm10,xmm6					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region1[rsp+4], r9d			; store upper region


;       movsd	xmm1,xmm7
;       subsd	xmm1,xmm10					; xmm1 = r=(rhead-rtail)
;       subsd	xmm7, xmm1					; rr=rhead-r
;       subsd	xmm7, xmm10					; xmm7 = rr=((rhead-r) -rtail)

       subsd	xmm7,xmm10					; xmm1 = r=(rhead-rtail)

;       movlpd	QWORD PTR r1[rsp+8], xmm1			; store upper r
;       movlpd	QWORD PTR rr1[rsp+8], xmm7			; store upper rr

       movlpd	QWORD PTR r1[rsp+8], xmm7			; store upper r



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign

; Work on Lower arg
	mov		r11,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r11
	and		r10,r8
	cmp		r10,r11
	jz		__vrs4_cosf_lower_naninf_higher

;	lea	 r9, 	QWORD PTR region1[rsp]			; lower arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr1[rsp]

	lea	 r8, 	QWORD PTR region1[rsp]			; lower arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r1[rsp]

; changed input from xmm10 to xmm0
;	movlpd	 xmm10, QWORD PTR r1[rsp]	;Restore lower fp arg for remainder_piby2 call
;	movlpd	 xmm0,  QWORD PTR r1[rsp]	;Restore lower fp arg for remainder_piby2 call
	mov	 rcx,  QWORD PTR r1[rsp]	;Restore lower fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrs4_cosf_lower_naninf_higher:
;	mov	r8, QWORD PTR p_original1[rsp]			; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r8,r11
	mov	QWORD PTR r1[rsp],r8				; r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr1[rsp], r10				; rr = 0
	mov	DWORD PTR region1[rsp], r10d			; region =0

align 16
@@:
	jmp __vrs4_cosf_reconstruct







align 16
both_arg_gt_5e5_higher:
; Upper Arg is >= 5e5, Lower arg is >= 5e5
; r8, r9
; xmm1, xmm3, xmm7 = x, xmm4 = 0.5


	movhlps xmm7, xmm1		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,r8
	cmp		r10,r11
	jz		__vrs4_cosf_lower_naninf_of_both_gt_5e5_higher

	mov	 QWORD PTR p_temp1[rsp], r9			;Save upper arg
;	lea	 r9, 	QWORD PTR region1[rsp]			;lower arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr1[rsp]
	lea	 r8, 	QWORD PTR region1[rsp]			;lower arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r1[rsp]

; changed input from xmm10 to xmm0
;	movapd	 xmm10, xmm1
;	movsd	 xmm0,  xmm1
	movd	 rcx,   xmm1

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r9, QWORD PTR p_temp1[rsp]			;Restore upper arg


	jmp 	@F

__vrs4_cosf_lower_naninf_of_both_gt_5e5_higher:				;lower arg is nan/inf
;	mov	r8, QWORD PTR p_original1[rsp]
	mov	r11,00008000000000000h
	or	r8,r11
	mov	QWORD PTR r1[rsp],r8				;r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr1[rsp], r10				;rr = 0
	mov	DWORD PTR region1[rsp], r10d			;region = 0

align 16
@@:
	mov		r11,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r11
	and		r10,r9
	cmp		r10,r11
	jz		__vrs4_cosf_upper_naninf_of_both_gt_5e5_higher

;	lea	 r9, 	QWORD PTR region1[rsp+4]			;upper arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr1[rsp+8]
	lea	 r8, 	QWORD PTR region1[rsp+4]			;upper arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r1[rsp+8]

; changed input from xmm10 to xmm0
;	movapd	 xmm10,  xmm7			;Restore upper fp arg for remainder_piby2 call
;	movsd	 xmm0,  xmm7			;Restore upper fp arg for remainder_piby2 call
	movd	 rcx,  xmm7			;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrs4_cosf_upper_naninf_of_both_gt_5e5_higher:
;	mov	r9, QWORD PTR p_original1[rsp+8]		;upper arg is nan/inf
;	movd	r9, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r9,r11
	mov	QWORD PTR r1[rsp+8],r9				;r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr1[rsp+8], r10			;rr = 0
	mov	DWORD PTR region1[rsp+4], r10d			;region = 0

align 16
@@:

	jmp __vrs4_cosf_reconstruct

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
fourth_arg_gt_5e5:
;first two args are < 5e5, third arg < 5e5, fourth arg >= 5e5
;rax, rcx, r8, r9
;xmm10, xmm2, xmm6 = x, xmm4 = 0.5

; Work on first two args, both < 5e5

	mulpd	xmm2, XMMWORD PTR __real_3fe45f306dc9c883		; * twobypi
	addpd	xmm2,xmm4						; +0.5, npi2
	movapd	xmm10,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtdq2pd	xmm2,xmm4					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */
	movd	QWORD PTR region[rsp], xmm4				; Region

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm10,xmm2						; npi2 * piby2_1;
;      rtail  = npi2 * piby2_2;
       mulpd	xmm8,xmm2						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm6,xmm10						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm10, xmm6						; t

;      rhead  = t - rtail;
       subpd	xmm10, xmm8						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm2,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm6,xmm10						; t-rhead
       subpd	xmm8,xmm6						; - ((t - rhead) - rtail)
       addpd	xmm8,xmm2						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

;       movapd	xmm6,xmm10						; rhead
       subpd	xmm10,xmm8						; r = rhead - rtail
       movapd	OWORD PTR r[rsp], xmm10

;       subpd	xmm6, xmm10						; rr=rhead-r
;       subpd	xmm6, xmm8						; rr=(rhead-r) -rtail
;       movapd	OWORD PTR rr[rsp], xmm6


; Work on next two args, third arg < 5e5, fourth arg >= 5e5
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
first_second_done_fourth_arg_gt_5e5:

; Upper Arg is >= 5e5, Lower arg is < 5e5
; r8, r9
; xmm1, xmm3, xmm7 = x, xmm4 = 0.5

	movhpd	QWORD PTR r1[rsp+8], xmm1	;Save upper fp arg for remainder_piby2 call
;	movlhps	xmm1, xmm1			;Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
;	movlhps	xmm3, xmm3
;	movlhps	xmm7, xmm7


; Work on Lower arg
; Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg
	movapd	xmm4, OWORD PTR __real_3fe0000000000000		; Restore 0.5
	mulsd	xmm3,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm3,xmm4					; xmm3 = npi2=(x*twobypi+0.5)
	movsd	xmm2,QWORD PTR __real_3ff921fb54400000		; xmm2 = piby2_1
	cvttsd2si	r8d,xmm3				; r8d = npi2 trunc to ints
	movsd	xmm10,QWORD PTR __real_3dd0b4611a600000		; xmm10 = piby2_2
	cvtsi2sd	xmm3,r8d				; xmm3 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm2,xmm3					; npi2 * piby2_1
	subsd	xmm7,xmm2					; xmm7 = rhead =(x-npi2*piby2_1)
	movsd	xmm6,QWORD PTR __real_3ba3198a2e037073		; xmm6 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm7					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm10,xmm3					; xmm10 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm7, xmm10					; xmm7 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm6,xmm3     					; npi2 * piby2_2tail
       subsd	xmm5,xmm7					; t-rhead
       subsd	xmm10,xmm5					; (rtail-(t-rhead))
       addsd	xmm10,xmm6					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region1[rsp], r8d			; store lower region

;       movsd	xmm1, xmm7
;       subsd	xmm1, xmm10					; xmm10 = r=(rhead-rtail)
;       subsd	xmm7, xmm1					; rr=rhead-r
;       subsd	xmm7, xmm10					; xmm6 = rr=((rhead-r) -rtail)

        subsd	xmm7, xmm10					; xmm10 = r=(rhead-rtail)

;       movlpd	QWORD PTR r1[rsp], xmm1				; store upper r
;       movlpd	QWORD PTR rr1[rsp], xmm7			; store upper rr

        movlpd	QWORD PTR r1[rsp], xmm7				; store upper r

;Work on Upper arg
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
	mov		r11,07ff0000000000000h			; is upper arg nan/inf
	mov		r10,r11
	and		r10,r9
	cmp		r10,r11
	jz		__vrs4_cosf_upper_naninf_higher

;	lea	 r9, 	QWORD PTR region1[rsp+4]			; upper arg is **NOT** nan/inf
;	lea	 r8, 	QWORD PTR rr1[rsp+8]
	lea	 r8, 	QWORD PTR region1[rsp+4]			; upper arg is **NOT** nan/inf
	lea	 rdx,	QWORD PTR r1[rsp+8]

; changed input from xmm10 to xmm0
;	movlpd	 xmm10,  QWORD PTR r1[rsp+8]	;Restore upper fp arg for remainder_piby2 call
;	movlpd	 xmm0,  QWORD PTR r1[rsp+8]	;Restore upper fp arg for remainder_piby2 call
	mov	 rcx,   QWORD PTR r1[rsp+8]	;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 fname_remainder_piby2d2f

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrs4_cosf_upper_naninf_higher:
;	mov	r9, QWORD PTR p_original1[rsp+8]		; upper arg is nan/inf
;	mov	r9, QWORD PTR r1[rsp+8]				; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r9,r11
	mov	QWORD PTR r1[rsp+8],r9				; r = x | 0x0008000000000000
;	xor	r10, r10
;	mov	QWORD PTR rr1[rsp+8], r10			; rr = 0
	mov	DWORD PTR region1[rsp+4], r10d			; region =0

align 16
@@:
	jmp	__vrs4_cosf_reconstruct


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
__vrs4_cosf_reconstruct:
;Results
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm10, OWORD PTR r[rsp]
	movapd	xmm1, OWORD PTR r1[rsp]

;	movapd	xmm6, OWORD PTR rr[rsp]
;	movapd	xmm7, OWORD PTR rr1[rsp]

	mov	r8,	QWORD PTR region[rsp]
	mov	r9,	QWORD PTR region1[rsp]
	mov 	rdx, 	QWORD PTR __reald_one_zero		;compare value for cossin path

	mov 	r10,	r8
	mov 	r11,	r9

	and	r8, 	QWORD PTR __reald_one_one		;odd/even region for cos/sin
	and	r9, 	QWORD PTR __reald_one_one		;odd/even region for cos/sin

; NEW
	mov	rax, r10
	mov	rcx, r11

	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
	shr	r11, 1						;~AB+A~B, A is sign and B is upper bit of region

	xor	r10, rax
	xor	r11, rcx
	and	r10, __reald_one_one				;(~AB+A~B)&1
	and	r11, __reald_one_one				;(~AB+A~B)&1

	mov	r12,	r10
	mov	r13,	r11

	and	r12,	rdx				;mask out the lower sign bit leaving the upper sign bit
	and	r13,	rdx				;mask out the lower sign bit leaving the upper sign bit

	shl	r10,	63				;shift lower sign bit left by 63 bits
	shl	r11,	63				;shift lower sign bit left by 63 bits
	shl	r12,	31				;shift upper sign bit left by 31 bits
	shl	r13,	31				;shift upper sign bit left by 31 bits

	mov 	QWORD PTR p_sign[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_sign[rsp+8],r12		;write out upper sign bit
	mov 	QWORD PTR p_sign1[rsp],r11		;write out lower sign bit
	mov 	QWORD PTR p_sign1[rsp+8],r13		;write out upper sign bit
; NEW


;DELETE
;	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
;	shr	r11, 1						;~AB+A~B, A is sign and B is upper bit of region

;	mov	rax, r10
;	mov	rcx, r11

;	not 	r12						;ADDED TO CHANGE THE LOGIC
;	not 	r13						;ADDED TO CHANGE THE LOGIC
;	and	r10, r12
;	and	r11, r13

;	not	rax
;	not	rcx
;	not	r12
;	not	r13
;	and	rax, r12
;	and	rcx, r13

;	or	r10, rax
;	or	r11, rcx
;	and	r10, __reald_one_one				;(~AB+A~B)&1
;	and	r11, __reald_one_one				;(~AB+A~B)&1

;	mov	r12,	r10
;	mov	r13,	r11

;	and	r12,	rdx				;mask out the lower sign bit leaving the upper sign bit
;	and	r13,	rdx				;mask out the lower sign bit leaving the upper sign bit

;	shl	r10,	63				;shift lower sign bit left by 63 bits
;	shl	r11,	63				;shift lower sign bit left by 63 bits
;	shl	r12,	31				;shift upper sign bit left by 31 bits
;	shl	r13,	31				;shift upper sign bit left by 31 bits

;	mov 	QWORD PTR p_sign[rsp],r10		;write out lower sign bit
;	mov 	QWORD PTR p_sign[rsp+8],r12		;write out upper sign bit
;	mov 	QWORD PTR p_sign1[rsp],r11		;write out lower sign bit
;	mov 	QWORD PTR p_sign1[rsp+8],r13		;write out upper sign bit
;DELETE


	mov	rax,  r8
	mov	rcx,  r9

	movapd	xmm2,xmm10
	movapd	xmm3,xmm1

	mulpd	xmm2,xmm10				; r2
	mulpd	xmm3,xmm1				; r2

	and	rax, QWORD PTR __reald_zero_one
	and	rcx, QWORD PTR __reald_zero_one
	shr	r8,  31
	shr	r9,  31
	or	rax, r8
	or	rcx, r9
	shl	rcx, 2
	or	rax, rcx

	lea	rcx, QWORD PTR evencos_oddsin_tbl
	jmp	QWORD PTR [rcx+rax*8]	;Jmp table for cos/sin calculation based on even/odd region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
PUBLIC __vrsa_cosf_cleanup
__vrsa_cosf_cleanup::

	movapd	xmm10, p_sign[rsp]
	movapd	xmm1, p_sign1[rsp]
	xorpd	xmm10, xmm4			; (+) Sign
	xorpd	xmm1, xmm5			; (+) Sign


	cvtpd2ps xmm0, xmm10
	cvtpd2ps xmm11, xmm1
	movlhps	 xmm0, xmm11


; NEW

__vrsa_bottom1:
; store the result _m128d
	mov	rdi,QWORD PTR [rsp+save_ya]	; get y_array pointer
	movlps	QWORD PTR [rdi],xmm0
	movhps	QWORD PTR 8[rdi],xmm0

	prefetch	QWORD PTR [rdi+32]
	add		rdi,16
	mov		QWORD PTR [rsp+save_ya],rdi	; save y_array pointer

	mov	rax,QWORD PTR [rsp+p_iter]	; get number of iterations
	sub	rax,1
	mov	QWORD PTR [rsp+p_iter],rax	; save number of iterations
	jnz	__vrsa_top

; see if we need to do any extras
	mov	rax,QWORD PTR [rsp+save_nv]	; get number of values
	test	rax,rax
	jnz	__vrsa_cleanup

__final_check:

; NEW

	movdqa	xmm6, OWORD PTR [rsp+020h]	; restore xmm6
	movdqa	xmm7, OWORD PTR [rsp+030h]	; restore xmm7
	movdqa	xmm8, OWORD PTR [rsp+040h]	; restore xmm8
	movdqa	xmm9, OWORD PTR [rsp+050h]	; restore xmm9
	movdqa	xmm10,OWORD PTR [rsp+060h]	; restore xmm0
	movdqa	xmm11,OWORD PTR [rsp+070h]	; restore xmm11
	movdqa	xmm12,OWORD PTR [rsp+080h]	; restore xmm12
	movdqa	xmm13,OWORD PTR [rsp+090h]	; restore xmm13
	mov	r12,QWORD PTR [rsp+save_r12]	; restore r12
	mov	r13,QWORD PTR [rsp+save_r13]	; restore r13
	mov	rdi,QWORD PTR [rsp+save_rdi]	; restore rdi	
	mov	rsi,QWORD PTR [rsp+save_rsi]	; restore rsi		

	add	rsp,stack_size
L__return:	
	ret


;NEW

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; we jump here when we have an odd number of cos calls to make at the end
; we assume that rdx is pointing at the next x array element, r8 at the next y array element.
; The number of values left is in save_nv

align	16
__vrsa_cleanup:
	mov		rsi,QWORD PTR [rsp+save_xa]
	mov		rdi,QWORD PTR [rsp+save_ya]


; START WORKING FROM HERE
; fill in a m128d with zeroes and the extra values and then make a recursive call.
	xorps		xmm0,xmm0
	xor		rax,rax
	movss		DWORD PTR p_temp[rsp+4],xmm0
	movlps		QWORD PTR p_temp[rsp+8],xmm0	

	mov		rax,save_nv[rsp]		; get number of values
	
	mov		ecx,DWORD PTR [rsi]		; we know there's at least one
	mov	 	DWORD PTR p_temp[rsp],ecx
	cmp		rax,2
	jl		__vrsacg

	mov		ecx,4[rsi]			; do the second value
	mov	 	DWORD PTR p_temp[rsp+4],ecx
	cmp		rax,3
	jl		__vrsacg

	mov		ecx,8[rsi]			; do the third value
	mov	 	DWORD PTR p_temp[rsp+8],ecx

__vrsacg:
	mov		rcx,4				; parameter for N
	lea		rdx,p_temp[rsp]	; &x parameter
	lea		r8, p_temp2[rsp]; &y parameter
	call		fname				; call recursively to compute four values

; now copy the results to the destination array
	mov		rdi,save_ya[rsp]
	mov		rax,save_nv[rsp]			; get number of values

	mov	 	ecx,DWORD PTR p_temp2[rsp]
	mov		DWORD PTR [rdi],ecx			; we know there's at least one
	cmp		rax,2
	jl		__vrsacgf

	mov	 	ecx,DWORD PTR p_temp2[rsp+4]
	mov		DWORD PTR 4[rdi],ecx			; do the second value
	cmp		rax,3
	jl		__vrsacgf

	mov	 	ecx,DWORD PTR p_temp2[rsp+8]
	mov		DWORD PTR 8[rdi],ecx			; do the third value

__vrsacgf:
	jmp		__final_check

;NEW




fname		endp
;TEXT	ENDS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;JUMP TABLE;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
coscos_coscos_piby4:
	movapd	xmm0, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR cosarray+30h			; c4
	movdqa	xmm5, OWORD PTR cosarray+30h			; c4

	movapd	xmm8, OWORD PTR cosarray+10h			; c2
	movapd	xmm9, OWORD PTR cosarray+10h			; c2

	mulpd	xmm0,  OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subpd	xmm0, OWORD PTR __real_3ff0000000000000		; -t=r-1.0	;trash r
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0	;trash r

	mulpd	xmm2, xmm2					; x4
	mulpd	xmm3, xmm3					; x4

	addpd	xmm4,  OWORD PTR cosarray+20h			; c3+x2c4
	addpd	xmm5,  OWORD PTR cosarray+20h			; c3+x2c4

	addpd	xmm8, OWORD PTR cosarray			; c1+x2c2
	addpd	xmm9, OWORD PTR cosarray			; c1+x2c2

	mulpd	xmm4,  xmm2					; x4(c3+x2c4)
	mulpd	xmm5,  xmm3					; x4(c3+x2c4)

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zc

	mulpd	xmm4, xmm2					; x4 * zc
	mulpd	xmm5, xmm3					; x4 * zc

	subpd   xmm4, xmm0					; + t
	subpd   xmm5, xmm11					; + t

	jmp __vrsa_cosf_cleanup

align 16
cossin_cossin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2,  xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movdqa	xmm4, OWORD PTR sincosarray+30h		; s4
	movdqa	xmm5, OWORD PTR sincosarray+30h		; s4
	movapd	xmm8, OWORD PTR sincosarray+10h		; s2
	movapd	xmm9, OWORD PTR sincosarray+10h		; s2

	movapd	xmm0, xmm2				; move x2 for x4
	movapd	xmm11,xmm3				; move x2 for x4

	mulpd	xmm4, xmm2				; x2s4
	mulpd	xmm5, xmm3				; x2s4
	mulpd	xmm8, xmm2				; x2s2
	mulpd	xmm9, xmm3				; x2s2

	mulpd	xmm0, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR sincosarray+20h		; s4+x2s3
	addpd	xmm5, OWORD PTR sincosarray+20h		; s4+x2s3
	addpd	xmm8, OWORD PTR sincosarray		; s2+x2s1
	addpd	xmm9, OWORD PTR sincosarray		; s2+x2s1

	mulpd	xmm4, xmm0				; x4(s3+x2s4)
	mulpd	xmm5, xmm11				; x4(s3+x2s4)

	movhlps	xmm0, xmm0				; move high x4 for cos term
	movhlps	xmm11, xmm11				; move high x4 for cos term

	movsd	xmm6, xmm2				; move low x2 for x3 for sin term
	movsd	xmm7, xmm3				; move low x2 for x3 for sin term
	mulsd	xmm6, xmm10				; get low x3 for sin term
	mulsd	xmm7, xmm1				; get low x3 for sin term

	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms
	mulpd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms

	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z

	movhlps	xmm12, xmm2				; move high r for cos
	movhlps	xmm13, xmm3				; move high r for cos

	movhlps	xmm8,  xmm4				; xmm4 = sin , xmm8 = cos
	movhlps	xmm9,  xmm5				; xmm4 = sin , xmm8 = cos

	mulsd	xmm4, xmm6				; sin *x3
	mulsd	xmm5, xmm7				; sin *x3

	mulsd	xmm8, xmm0				; cos *x4
	mulsd	xmm9, xmm11				; cos *x4

	subsd	xmm12, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm13, QWORD PTR __real_3ff0000000000000; -t=r-1.0

	addsd	xmm4, xmm10				; sin + x
	addsd	xmm5, xmm1				; sin + x
	subsd   xmm8, xmm12				; cos+t
	subsd   xmm9, xmm13				; cos+t

	movlhps	xmm4, xmm8
	movlhps	xmm5, xmm9

	jmp 	__vrsa_cosf_cleanup
align 16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sincos_cossin_piby4:					
							
	movapd	xmm4, OWORD PTR sincosarray+30h		; s4
	movapd	xmm5, OWORD PTR cossinarray+30h		; s4
	movdqa	xmm8, OWORD PTR sincosarray+10h		; s2
	movdqa	xmm9, OWORD PTR cossinarray+10h		; s2

	movapd	xmm0,  xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4
	movapd	xmm7,  xmm3				; sincos term upper x2 for x3

	mulpd	xmm4, xmm2				; x2s4
	mulpd	xmm5, xmm3				; x2s4
	mulpd	xmm8, xmm2				; x2s2
	mulpd	xmm9, xmm3				; x2s2

	mulpd	xmm0, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR sincosarray+20h		; s3+x2s4
	addpd	xmm5, OWORD PTR cossinarray+20h		; s3+x2s4
	addpd	xmm8, OWORD PTR sincosarray		; s1+x2s2
	addpd	xmm9, OWORD PTR cossinarray		; s1+x2s2

	mulpd	xmm4, xmm0				; x4(s3+x2s4)
	mulpd	xmm5, xmm11				; x4(s3+x2s4)

	movhlps	xmm0, xmm0				; move high x4 for cos term

	movsd	xmm6, xmm2				; move low x2 for x3 for sin term  (cossin)
	mulpd	xmm7, xmm1
	
	mulsd	xmm6, xmm10				; get low x3 for sin term (cossin)
	movhlps	xmm7, xmm7				; get high x3 for sin term (sincos)
	
	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for cos term
	mulpd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for cos term

	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z


	movhlps	xmm12, xmm2				; move high r for cos (cossin)
	
	
	movhlps	xmm8,  xmm4				; xmm8 = cos , xmm4 = sin	(cossin)
	movhlps	xmm9,  xmm5				; xmm9 = sin , xmm5 = cos	(sincos)

	mulsd	xmm4, xmm6				; sin *x3
	mulsd	xmm5, xmm11				; cos *x4
	mulsd	xmm8, xmm0				; cos *x4
	mulsd	xmm9, xmm7				; sin *x3

	subsd	xmm12, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm3, QWORD PTR __real_3ff0000000000000 ; -t=r-1.0

	movhlps	xmm11, xmm1				; move high x for x for sin term    (sincos)

	addsd	xmm4, xmm10				; sin + x	+
	addsd	xmm9, xmm11				; sin + x	+

	subsd   xmm8, xmm12				; cos+t
	subsd   xmm5, xmm3				; cos+t

	movlhps	xmm4, xmm8				; cossin
	movlhps	xmm5, xmm9				; sincos

	jmp	__vrsa_cosf_cleanup
	
align 16
sincos_sincos_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm4, OWORD PTR cossinarray+30h		; s4
	movapd	xmm5, OWORD PTR cossinarray+30h		; s4
	movdqa	xmm8, OWORD PTR cossinarray+10h		; s2
	movdqa	xmm9, OWORD PTR cossinarray+10h		; s2

	movapd	xmm0, xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4
	movapd	xmm6, xmm2				; move x2 for x4
	movapd	xmm7, xmm3				; move x2 for x4

	mulpd	xmm4, xmm2				; x2s6
	mulpd	xmm5, xmm3				; x2s6
	mulpd	xmm8, xmm2				; x2s3
	mulpd	xmm9, xmm3				; x2s3

	mulpd	xmm0, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR cossinarray+20h		; s4+x2s3
	addpd	xmm5, OWORD PTR cossinarray+20h		; s4+x2s3
	addpd	xmm8, OWORD PTR cossinarray		; s2+x2s1
	addpd	xmm9, OWORD PTR cossinarray		; s2+x2s1

	mulpd	xmm4, xmm0				; x4(s4+x2s3)
	mulpd	xmm5, xmm11				; x4(s4+x2s3)

	mulpd	xmm6, xmm10				; get low x3 for sin term
	mulpd	xmm7, xmm1				; get low x3 for sin term
	movhlps	xmm6, xmm6				; move low x2 for x3 for sin term
	movhlps	xmm7, xmm7				; move low x2 for x3 for sin term

	mulsd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for cos terms
	mulsd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for cos terms

	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z

	movhlps	xmm12,  xmm4				; xmm8 = sin , xmm4 = cos
	movhlps	xmm13,  xmm5				; xmm9 = sin , xmm5 = cos

	mulsd	xmm12, xmm6				; sin *x3
	mulsd	xmm13, xmm7				; sin *x3
	mulsd	xmm4, xmm0				; cos *x4
	mulsd	xmm5, xmm11				; cos *x4

	subsd	xmm2, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm3, QWORD PTR __real_3ff0000000000000; -t=r-1.0

	movhlps	xmm0, xmm10				; move high x for x for sin term
	movhlps	xmm11, xmm1				; move high x for x for sin term
							; Reverse 10 and 0

	addsd	xmm12, xmm0				; sin + x
	addsd	xmm13, xmm11				; sin + x

	subsd   xmm4, xmm2				; cos+t
	subsd   xmm5, xmm3				; cos+t

	movlhps	xmm4, xmm12
	movlhps	xmm5, xmm13
	jmp 	__vrsa_cosf_cleanup
	
align 16
cossin_sincos_piby4:					
							
	movapd	xmm4, OWORD PTR cossinarray+30h		; s4
	movapd	xmm5, OWORD PTR sincosarray+30h		; s4
	movdqa	xmm8, OWORD PTR cossinarray+10h		; s2
	movdqa	xmm9, OWORD PTR sincosarray+10h		; s2

	movapd	xmm0,  xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4
	movapd	xmm7,  xmm2				; upper x2 for x3 for sin term (sincos)

	mulpd	xmm4, xmm2				; x2s4
	mulpd	xmm5, xmm3				; x2s4
	mulpd	xmm8, xmm2				; x2s2
	mulpd	xmm9, xmm3				; x2s2

	mulpd	xmm0,  xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR cossinarray+20h		; s3+x2s4
	addpd	xmm5, OWORD PTR sincosarray+20h		; s3+x2s4
	addpd	xmm8, OWORD PTR cossinarray		; s1+x2s2
	addpd	xmm9, OWORD PTR sincosarray		; s1+x2s2

	mulpd	xmm4, xmm0				; x4(s3+x2s4)
	mulpd	xmm5, xmm11				; x4(s3+x2s4)

	movhlps	xmm11, xmm11				; move high x4 for cos term

	movsd	xmm6, xmm3				; move low x2 for x3 for sin term  (cossin)
	mulpd	xmm7, xmm10
	
	mulsd	xmm6, xmm1				; get low x3 for sin term (cossin)
	movhlps	xmm7, xmm7				; get high x3 for sin term (sincos)
	
	mulsd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for cos term
	mulpd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for cos term

	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z


	movhlps	xmm12, xmm3				; move high r for cos (cossin)
	
	
	movhlps	xmm8,  xmm4				; xmm8 = sin , xmm4 = cos	(sincos)
	movhlps	xmm9,  xmm5				; xmm9 = cos , xmm5 = sin	(cossin)

	mulsd	xmm4, xmm0				; cos *x4
	mulsd	xmm5, xmm6				; sin *x3
	mulsd	xmm8, xmm7				; sin *x3
	mulsd	xmm9, xmm11				; cos *x4

	subsd	xmm2,  QWORD PTR __real_3ff0000000000000 ; -t=r-1.0
	subsd	xmm12, QWORD PTR __real_3ff0000000000000 ; -t=r-1.0

	movhlps	xmm11, xmm10				; move high x for x for sin term    (sincos)

	subsd	xmm4, xmm2				; cos-(-t)
	subsd	xmm9, xmm12				; cos-(-t)

	addsd   xmm8, xmm11				; sin + x
	addsd   xmm5, xmm1				; sin + x

	movlhps	xmm4, xmm8				; cossin
	movlhps	xmm5, xmm9				; sincos

	jmp	__vrsa_cosf_cleanup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
coscos_sinsin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr: 	SIN
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr:	COS	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm0, xmm2					; x2	; SIN
	movapd	xmm11, xmm3					; x2	; COS
	movapd	xmm1, xmm3					; copy of x2 for x4

	movdqa	xmm4, OWORD PTR sinarray+30h			; c4
	movdqa	xmm5, OWORD PTR cosarray+30h			; c4
	movapd	xmm8, OWORD PTR sinarray+10h			; c2
	movapd	xmm9, OWORD PTR cosarray+10h			; c2

	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	mulpd	xmm0, xmm2					; x4
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm1, xmm3					; x4

	addpd	xmm4,  OWORD PTR sinarray+20h			; c3+x2c4
	addpd	xmm5,  OWORD PTR cosarray+20h			; c3+x2c4
	addpd	xmm8,  OWORD PTR sinarray			; c1+x2c2
	addpd	xmm9,  OWORD PTR cosarray			; c1+x2c2

	mulpd	xmm2, xmm10					; x3
	
	mulpd	xmm4, xmm0					; x4(c3+x2c4)
	mulpd	xmm5, xmm1					; x4(c3+x2c4)

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zc

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm1					; x4 * zc

	addpd	xmm4, xmm10					; +x
	subpd   xmm5, xmm11					; +t

	jmp __vrsa_cosf_cleanup

align 16
sinsin_coscos_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr: 	COS
; p_sign1  = Sign, xmm1  = r, xmm3 = r2, xmm7 =rr:	SIN	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm0, xmm2					; x2	; COS
	movapd	xmm11, xmm3					; x2	; SIN
	movapd	xmm10, xmm2					; copy of x2 for x4

	movdqa	xmm4, OWORD PTR cosarray+30h			; c4
	movdqa	xmm5, OWORD PTR sinarray+30h			; s4
	movapd	xmm8, OWORD PTR cosarray+10h			; c2
	movapd	xmm9, OWORD PTR sinarray+10h			; s2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; s4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; s2*x2

	mulpd	xmm10, xmm2					; x4
	mulpd	xmm11, xmm3					; x4

	addpd	xmm4,  OWORD PTR cosarray+20h			; c3+x2c4
	addpd	xmm5,  OWORD PTR sinarray+20h			; s3+x2c4
	addpd	xmm8,  OWORD PTR cosarray			; c1+x2c2
	addpd	xmm9,  OWORD PTR sinarray			; s1+x2c2

	mulpd	xmm3, xmm1					; x3
	mulpd	xmm0, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	
	mulpd	xmm4, xmm10					; x4(c3+x2c4)
	mulpd	xmm5, xmm11					; x4(s3+x2s4)

	subpd	xmm0, OWORD PTR __real_3ff0000000000000		; -t=r-1.0
	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zs

	mulpd	xmm4, xmm10					; x4 * zc
	mulpd	xmm5, xmm3					; x3 * zc

	subpd	xmm4, xmm0					; +t
	addpd   xmm5, xmm1					; +x

	jmp __vrsa_cosf_cleanup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
coscos_cossin_piby4:				;Derive from cossin_coscos
	movhlps	xmm0,  xmm2					; x2 for 0.5x2 for upper cos
	movsd	xmm6,  xmm2					; lower x2 for x3 for lower sin
	movapd	xmm11, xmm3					; x2 for 0.5x2
	movapd	xmm12, xmm2					; x2 for x4
	movapd	xmm13, xmm3					; x2 for x4
	
	movsd	xmm7,  QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR sincosarray+30h			; c4
	movdqa	xmm5, OWORD PTR cosarray+30h			; c4
	movapd	xmm8, OWORD PTR sincosarray+10h			; c2
	movapd	xmm9, OWORD PTR cosarray+10h			; c2


	mulsd	xmm0,  QWORD PTR __real_3fe0000000000000	; 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	subsd	xmm7,  xmm0					;  t=1.0-r  for cos
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR sincosarray+20h			; c4+x2c3
	addpd	xmm5, OWORD PTR cosarray+20h			; c4+x2c3
	addpd	xmm8, OWORD PTR sincosarray			; c2+x2c1
	addpd	xmm9, OWORD PTR cosarray			; c2+x2c1


	movapd	xmm2, xmm12					; upper=x4
	movsd	xmm2, xmm6					; lower=x2
	mulsd	xmm2, xmm10					; lower=x2*x

	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zc

	mulpd	xmm4, xmm2					; upper= x4 * zc
								; lower=x3 * zs
	mulpd	xmm5, xmm13					; x4 * zc
								

	movlhps	xmm10, xmm7					; 
	addpd	xmm4, xmm10					; +x for lower sin, +t for upper cos
	subpd   xmm5, xmm11					; -(-t)

	jmp __vrsa_cosf_cleanup
align 16
coscos_sincos_piby4:				;Derive from cossin_coscos
	movsd	xmm0,  xmm2					; x2 for 0.5x2 for lower cos
	movapd	xmm11, xmm3					; x2 for 0.5x2
	movapd	xmm12, xmm2					; x2 for x4
	movapd	xmm13, xmm3					; x2 for x4
	movsd	xmm7, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR cossinarray+30h			; cs4
	movdqa	xmm5, OWORD PTR cosarray+30h			; c4
	movapd	xmm8, OWORD PTR cossinarray+10h			; cs2
	movapd	xmm9, OWORD PTR cosarray+10h			; c2

	mulsd	xmm0,  QWORD PTR __real_3fe0000000000000	; 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	subsd	xmm7,  xmm0					; t=1.0-r  for cos
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR cossinarray+20h			; c4+x2c3
	addpd	xmm5, OWORD PTR cosarray+20h			; c4+x2c3
	addpd	xmm8, OWORD PTR cossinarray			; c2+x2c1
	addpd	xmm9, OWORD PTR cosarray			; c2+x2c1

	mulpd	xmm2, xmm10					; upper=x3 for sin
	mulsd	xmm2, xmm10					; lower=x4 for cos					

	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zc

	mulpd	xmm4, xmm2					; lower= x4 * zc
								; upper= x3 * zs
	mulpd	xmm5, xmm13					; x4 * zc
								

	movsd	xmm10, xmm7					
	addpd	xmm4, xmm10					; +x for upper sin, +t for lower cos
	subpd   xmm5, xmm11					; -(-t)

	jmp __vrsa_cosf_cleanup
align 16
cossin_coscos_piby4:				
	movhlps	xmm0, xmm3					; x2 for 0.5x2 for upper cos
	movapd	xmm11, xmm2					; x2 for 0.5x2
	movapd	xmm12, xmm2					; x2 for x4
	movapd	xmm13, xmm3					; x2 for x4
	movsd	xmm6, xmm3					; lower x2 for x3 for sin
	movsd	xmm7, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR cosarray+30h			; cs4
	movdqa	xmm5, OWORD PTR sincosarray+30h			; c4
	movapd	xmm8, OWORD PTR cosarray+10h			; cs2
	movapd	xmm9, OWORD PTR sincosarray+10h			; c2


	mulsd	xmm0,  QWORD PTR __real_3fe0000000000000		; 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	subsd	xmm7, xmm0					; t=1.0-r  for cos
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR cosarray+20h			; c4+x2c3
	addpd	xmm5, OWORD PTR sincosarray+20h			; c4+x2c3
	addpd	xmm8, OWORD PTR cosarray			; c2+x2c1
	addpd	xmm9, OWORD PTR sincosarray			; c2+x2c1

	movapd	xmm3, xmm13					; upper=x4
	movsd	xmm3, xmm6					; lower x2
	mulsd	xmm3, xmm1					; lower x2*x

	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zc

	mulpd	xmm4, xmm12					; x4 * zc								
	mulpd	xmm5, xmm3					; upper= x4 * zc
								; lower=x3 * zs

	movlhps	xmm1, xmm7					
	addpd	xmm5, xmm1					; +x for lower sin, +t for upper cos
	subpd   xmm4, xmm11					; -(-t)

	jmp __vrsa_cosf_cleanup
	
align 16
cossin_sinsin_piby4:		; Derived from sincos_coscos

	movhlps	xmm0,  xmm3					; x2
	movapd	xmm7,  xmm3
	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4
	movsd	xmm11, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR sinarray+30h			; c4
	movdqa	xmm5, OWORD PTR sincosarray+30h			; c4
	movapd	xmm8, OWORD PTR sinarray+10h			; c2
	movapd	xmm9, OWORD PTR sincosarray+10h			; c2

	mulsd	xmm0, QWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	mulpd	xmm12, xmm2					; x4
	subsd	xmm11, xmm0					; t=1.0-r for cos	
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR sinarray+20h			; c3+x2c4
	addpd	xmm5, OWORD PTR sincosarray+20h			; c3+x2c4
	addpd	xmm8, OWORD PTR sinarray			; c1+x2c2
	addpd	xmm9, OWORD PTR sincosarray			; c1+x2c2

	mulpd	xmm2, xmm10					; x3
	movapd	xmm3, xmm13					; upper x4 for cos
	movsd	xmm3, xmm7					; lower x2 for sin
	mulsd	xmm3, xmm1					; lower x3=x2*x for sin
	
	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	movlhps	xmm1, xmm11					; t for upper cos and x for lower sin
	
	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zs

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm3					; upper=x4 * zc
								; lower=x3 * zs						

	addpd   xmm4, xmm10					; +x
	addpd	xmm5, xmm1					; +t upper, +x lower


	jmp __vrsa_cosf_cleanup	
align 16
sincos_coscos_piby4:				
	movsd	xmm0,  xmm3					; x2 for 0.5x2 for lower cos
	movapd	xmm11, xmm2					; x2 for 0.5x2
	movapd	xmm12, xmm2					; x2 for x4
	movapd	xmm13, xmm3					; x2 for x4
	movsd	xmm7, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR cosarray+30h			; cs4
	movdqa	xmm5, OWORD PTR cossinarray+30h			; c4
	movapd	xmm8, OWORD PTR cosarray+10h			; cs2
	movapd	xmm9, OWORD PTR cossinarray+10h			; c2

	mulsd	xmm0,  QWORD PTR __real_3fe0000000000000	; 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	subsd	xmm7,  xmm0					; t=1.0-r  for cos
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR cosarray+20h			; c4+x2c3
	addpd	xmm5, OWORD PTR cossinarray+20h			; c4+x2c3
	addpd	xmm8, OWORD PTR cosarray			; c2+x2c1
	addpd	xmm9, OWORD PTR cossinarray			; c2+x2c1

	mulpd	xmm3, xmm1					; upper=x3 for sin
	mulsd	xmm3, xmm1					; lower=x4 for cos					

	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zc

	mulpd	xmm4, xmm12					; x4 * zc								
	mulpd	xmm5, xmm3					; lower= x4 * zc
								; upper= x3 * zs

	movsd	xmm1, xmm7					
	subpd   xmm4, xmm11					; -(-t)
	addpd	xmm5, xmm1					; +x for upper sin, +t for lower cos


	jmp __vrsa_cosf_cleanup

align 16
sincos_sinsin_piby4:		; Derived from sincos_coscos

	movsd	xmm0,  xmm3					; x2
	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4
	movsd	xmm11, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR sinarray+30h			; c4
	movdqa	xmm5, OWORD PTR cossinarray+30h			; c4
	movapd	xmm8, OWORD PTR sinarray+10h			; c2
	movapd	xmm9, OWORD PTR cossinarray+10h			; c2

	mulsd	xmm0, QWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	mulpd	xmm12, xmm2					; x4
	subsd	xmm11, xmm0					; t=1.0-r for cos	
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR sinarray+20h			; c3+x2c4
	addpd	xmm5, OWORD PTR cossinarray+20h			; c3+x2c4
	addpd	xmm8, OWORD PTR sinarray			; c1+x2c2
	addpd	xmm9, OWORD PTR cossinarray			; c1+x2c2

	mulpd	xmm2, xmm10					; x3
	mulpd	xmm3, xmm1					; upper x3 for sin
	mulsd	xmm3, xmm1					; lower x4 for cos

	movhlps	xmm6, xmm1

	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	movlhps	xmm11, xmm6					; upper =t ; lower =x
	
	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zszc

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm3					; lower=x4 * zc
								; upper=x3 * zs
								
	addpd   xmm4, xmm10					; +x
	addpd	xmm5, xmm11					; +t lower, +x upper

	jmp __vrsa_cosf_cleanup

align 16
sinsin_cossin_piby4:		; Derived from sincos_coscos

	movhlps	xmm0,  xmm2					; x2
	movapd	xmm7,  xmm2
	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4
	movsd	xmm11, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR sincosarray+30h			; c4
	movdqa	xmm5, OWORD PTR sinarray+30h			; c4
	movapd	xmm8, OWORD PTR sincosarray+10h			; c2
	movapd	xmm9, OWORD PTR sinarray+10h			; c2

	mulsd	xmm0, QWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	mulpd	xmm12, xmm2					; x4
	subsd	xmm11, xmm0					; t=1.0-r for cos	
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR sincosarray+20h			; c3+x2c4
	addpd	xmm5, OWORD PTR sinarray+20h			; c3+x2c4
	addpd	xmm8, OWORD PTR sincosarray			; c1+x2c2
	addpd	xmm9, OWORD PTR sinarray			; c1+x2c2

	mulpd	xmm3, xmm1					; x3
	movapd	xmm2, xmm12					; upper x4 for cos
	movsd	xmm2, xmm7					; lower x2 for sin
	mulsd	xmm2, xmm10					; lower x3=x2*x for sin
	
	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	movlhps	xmm10, xmm11					; t for upper cos and x for lower sin
	
	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zszc

	mulpd	xmm5, xmm3					; x3 * zs
	mulpd	xmm4, xmm2					; upper=x4 * zc
								; lower=x3 * zs

	addpd	xmm5, xmm1					; +x
	addpd   xmm4, xmm10					; +t upper, +x lower

	jmp __vrsa_cosf_cleanup	

align 16
sinsin_sincos_piby4:		; Derived from sincos_coscos

	movsd	xmm0,  xmm2					; x2
	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4
	movsd	xmm11, QWORD PTR __real_3ff0000000000000

	movdqa	xmm4, OWORD PTR cossinarray+30h			; c4
	movdqa	xmm5, OWORD PTR sinarray+30h			; c4
	movapd	xmm8, OWORD PTR cossinarray+10h			; c2
	movapd	xmm9, OWORD PTR sinarray+10h			; c2

	mulsd	xmm0, QWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2
	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	mulpd	xmm12, xmm2					; x4
	subsd	xmm11, xmm0					; t=1.0-r for cos	
	mulpd	xmm13, xmm3					; x4

	addpd	xmm4, OWORD PTR cossinarray+20h			; c3+x2c4
	addpd	xmm5, OWORD PTR sinarray+20h			; c3+x2c4
	addpd	xmm8, OWORD PTR cossinarray			; c1+x2c2
	addpd	xmm9, OWORD PTR sinarray			; c1+x2c2

	mulpd	xmm3, xmm1					; x3
	mulpd	xmm2, xmm10					; upper x3 for sin
	mulsd	xmm2, xmm10					; lower x4 for cos

	movhlps	xmm6, xmm10

	mulpd	xmm4, xmm12					; x4(c3+x2c4)
	mulpd	xmm5, xmm13					; x4(c3+x2c4)

	movlhps	xmm11, xmm6
	
	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zszc

	mulpd	xmm5, xmm3					; x3 * zs
	mulpd	xmm4, xmm2					; lower=x4 * zc
								; upper=x3 * zs

	addpd	xmm5, xmm1					; +x
	addpd   xmm4, xmm11					; +t lower, +x upper

	jmp __vrsa_cosf_cleanup	

align 16
sinsin_sinsin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm10 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;x2 = x * x;
  ;(x + x * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));
  
  ;x + x3 * ((c1 + x2 *c2) + x4 * (c3 + x2 * c4));
  

	movapd	xmm0, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR sinarray+30h			; c4
	movdqa	xmm5, OWORD PTR sinarray+30h			; c4

	mulpd	xmm0,  xmm2					; x4
	mulpd	xmm11, xmm3					; x4

	movapd	xmm8, OWORD PTR sinarray+10h			; c2
	movapd	xmm9, OWORD PTR sinarray+10h			; c2

	mulpd	xmm4,  xmm2					; c4*x2
	mulpd	xmm5,  xmm3					; c4*x2

	mulpd	xmm8,  xmm2					; c2*x2
	mulpd	xmm9,  xmm3					; c2*x2

	addpd	xmm4, OWORD PTR sinarray+20h			; c3+x2c4
	addpd	xmm5, OWORD PTR sinarray+20h			; c3+x2c4

	mulpd	xmm2, xmm10					; x3
	mulpd	xmm3, xmm1					; x3

	addpd	xmm8, OWORD PTR sinarray			; c1+x2c2
	addpd	xmm9, OWORD PTR sinarray			; c1+x2c2

	mulpd	xmm4, xmm0					; x4(c3+x2c4)
	mulpd	xmm5, xmm11					; x4(c3+x2c4)
	
	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zs

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm3					; x3 * zs

	addpd	xmm4, xmm10					; +x
	addpd	xmm5, xmm1					; +x

	jmp __vrsa_cosf_cleanup

END
