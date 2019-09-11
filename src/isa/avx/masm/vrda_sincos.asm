;
; vrda_sincos.asm
;
; An array implementation of the sincos libm function.
;
; Prototype:
;
;    void vrda_sincos(int n, double *x, double *ys, double *yc);
;
;Computes Sine of x for an array of input values.


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
__real_jt_mask		DQ 0000000000000000Fh	;
			DQ 00000000000000000h	;
__real_naninf_upper_sign_mask	DQ 000000000ffffffffh	;
				DQ 000000000ffffffffh	;
__real_naninf_lower_sign_mask	DQ 0ffffffff00000000h	;
				DQ 0ffffffff00000000h	;

cosarray:
	DQ	03fa5555555555555h		; 0.0416667		   	c1
	DQ	03fa5555555555555h
	DQ	0bf56c16c16c16967h		; -0.00138889	   		c2
	DQ	0bf56c16c16c16967h
	DQ	03efa01a019f4ec90h		; 2.48016e-005			c3
	DQ	03efa01a019f4ec90h
	DQ	0be927e4fa17f65f6h		; -2.75573e-007			c4
	DQ	0be927e4fa17f65f6h
	DQ	03e21eeb69037ab78h		; 2.08761e-009			c5
	DQ	03e21eeb69037ab78h
	DQ	0bda907db46cc5e42h		; -1.13826e-011	   		c6
	DQ	0bda907db46cc5e42h
sinarray:
	DQ	0bfc5555555555555h		; -0.166667	   		s1
	DQ	0bfc5555555555555h
	DQ	03f81111111110bb3h		; 0.00833333	   		s2
	DQ	03f81111111110bb3h
	DQ	0bf2a01a019e83e5ch		; -0.000198413			s3
	DQ	0bf2a01a019e83e5ch
	DQ	03ec71de3796cde01h		; 2.75573e-006			s4
	DQ	03ec71de3796cde01h
	DQ	0be5ae600b42fdfa7h		; -2.50511e-008			s5
	DQ	0be5ae600b42fdfa7h
	DQ	03de5e0b2f9a43bb8h		; 1.59181e-010	   		s6
	DQ	03de5e0b2f9a43bb8h
sincosarray:
	DQ	0bfc5555555555555h		; -0.166667	   		s1
	DQ	03fa5555555555555h		; 0.0416667		   	c1
	DQ	03f81111111110bb3h		; 0.00833333	   		s2
	DQ	0bf56c16c16c16967h
	DQ	0bf2a01a019e83e5ch		; -0.000198413			s3
	DQ	03efa01a019f4ec90h
	DQ	03ec71de3796cde01h		; 2.75573e-006			s4
	DQ	0be927e4fa17f65f6h
	DQ	0be5ae600b42fdfa7h		; -2.50511e-008			s5
	DQ	03e21eeb69037ab78h
	DQ	03de5e0b2f9a43bb8h		; 1.59181e-010	   		s6
	DQ	0bda907db46cc5e42h


cossinarray:
	DQ	03fa5555555555555h		; 0.0416667		   	c1
	DQ	0bfc5555555555555h		; -0.166667	   		s1
	DQ	0bf56c16c16c16967h
	DQ	03f81111111110bb3h		; 0.00833333	   		s2
	DQ	03efa01a019f4ec90h
	DQ	0bf2a01a019e83e5ch		; -0.000198413			s3
	DQ	0be927e4fa17f65f6h
	DQ	03ec71de3796cde01h		; 2.75573e-006			s4
	DQ	03e21eeb69037ab78h
	DQ	0be5ae600b42fdfa7h		; -2.50511e-008			s5
	DQ	0bda907db46cc5e42h
	DQ	03de5e0b2f9a43bb8h		; 1.59181e-010	   		s6



CONST	ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; define local variable storage offsets

save_xmm6	equ		00h		; temporary for get/put bits operation
save_xmm7	equ		10h		; temporary for get/put bits operation
save_xmm8	equ		20h		; temporary for get/put bits operation
save_xmm9	equ		30h		; temporary for get/put bits operation
save_xmm10	equ		40h		; temporary for get/put bits operation
save_xmm11	equ		50h		; temporary for get/put bits operation
save_xmm12	equ		60h		; temporary for get/put bits operation
save_xmm13	equ		70h		; temporary for get/put bits operation
save_xmm14	equ		80h		; temporary for get/put bits operation
save_xmm15	equ		90h		; temporary for get/put bits operation

save_rdi	equ		0A0h
save_rsi	equ		0B0h
save_rbx	equ		0C0h

r		equ		0D0h		; pointer to r for remainder_piby2
rr		equ		0E0h		; pointer to r for remainder_piby2
rsq		equ		0F0h
region		equ		0100h		; pointer to r for remainder_piby2

r1		equ		0110h		; pointer to r for remainder_piby2
rr1		equ		0120h		; pointer to r for remainder_piby2
rsq1		equ		0130h
region1		equ		0140h		; pointer to r for remainder_piby2

p_temp		equ		0150h		; temporary for get/put bits operation
p_temp1		equ		0160h		; temporary for get/put bits operation

p_temp2		equ		0170h		; temporary for get/put bits operation
p_temp3		equ		0180h		; temporary for get/put bits operation

p_temp4		equ		0190h		; temporary for get/put bits operation
p_temp5		equ		01A0h		; temporary for get/put bits operation

p_temp6		equ		01B0h		; temporary for get/put bits operation
p_temp7		equ		01C0h		; temporary for get/put bits operation

p_original	equ		01D0h		; original x
p_mask		equ		01E0h		; original x
p_signs		equ		01F0h		; original x
p_signc		equ		0200h		; original x
p_region	equ		0210h

p_original1	equ		0220h		; original x
p_mask1		equ		0230h		; original x
p_signs1	equ		0240h		; original x
p_signc1	equ		0250h		; original x
p_region1	equ		0260h

save_r12	equ		0270h		; temporary for get/put bits operation
save_r13	equ		0280h		; temporary for get/put bits operation

save_r14	equ		0290h		; temporary for get/put bits operation
save_r15	equ		02A0h		; temporary for get/put bits operation

save_xa		equ		02B0h		;qword ; leave space for 4 args*****
save_ysa	equ		02C0h		;qword ; leave space for 4 args*****
save_yca	equ		02D0h		;qword ; leave space for 4 args*****

save_nv		equ		02E0h		;qword
p_iter		equ		02F0h		; qword	storage for number of loop iterations


stack_size	equ		0308h


include fm.inc

FN_PROTOTYPE vrda_sincos


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

EXTRN __amd_remainder_piby2:NEAR


;TEXT	SEGMENT	   page	   'CODE'
.CODE

; parameters are passed in by Microsoft C as:
; ecx - int n
; rdx - double *x
; r8  - double *ys
; r9  - double *yc

	PUBLIC  fname
fname	proc	frame


	sub		rsp,stack_size
	.ALLOCSTACK	stack_size				; unwind data, needed since we call
								; remainder_piby2 and could have exceptions								; but it costs no performance to include it.

	movdqa	OWORD PTR [rsp+save_xmm6],xmm6		; save xmm6
	.SAVEXMM128 xmm6, save_xmm6

	movdqa	OWORD PTR [rsp+save_xmm7],xmm7		; save xmm7
	.SAVEXMM128 xmm7, save_xmm7

	movdqa	OWORD PTR [rsp+save_xmm8],xmm8		; save xmm8
	.SAVEXMM128 xmm8, save_xmm8

	movdqa	OWORD PTR [rsp+save_xmm9],xmm9		; save xmm9
	.SAVEXMM128 xmm9, save_xmm9

	movdqa	OWORD PTR [rsp+save_xmm10],xmm10	; save xmm10
	.SAVEXMM128 xmm10, save_xmm10

	movdqa	OWORD PTR [rsp+save_xmm11],xmm11	; save xmm11
	.SAVEXMM128 xmm11, save_xmm11

	movdqa	OWORD PTR [rsp+save_xmm12],xmm12	; save xmm12
	.SAVEXMM128 xmm12, save_xmm12

	movdqa	OWORD PTR [rsp+save_xmm13],xmm13	; save xmm13
	.SAVEXMM128 xmm13, save_xmm13

	movdqa	OWORD PTR [rsp+save_xmm14],xmm14	; save xmm14
	.SAVEXMM128 xmm14, save_xmm14

	movdqa	OWORD PTR [rsp+save_xmm15],xmm15	; save xmm15
	.SAVEXMM128 xmm15, save_xmm15

	mov	QWORD PTR [rsp+save_r12],r12		; save r12
	.SAVEREG r12, save_r12

	mov	QWORD PTR [rsp+save_r13],r13		; save r13
	.SAVEREG r13, save_r13

	mov		QWORD PTR [rsp+save_rsi],rsi	; save rsi
	.SAVEREG rsi, save_rsi

	mov		QWORD PTR [rsp+save_rdi],rdi	; save rdi
	.SAVEREG rdi, save_rdi

	mov		QWORD PTR [rsp+save_rbx],rbx	; save rbx
	.SAVEREG rbx, save_rbx

	.ENDPROLOG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;START PROCESS INPUT
; save the arguments
	mov		QWORD PTR [rsp+save_xa],rdx	; save x_array pointer
	mov		QWORD PTR [rsp+save_ysa],r8	; save ysin_array pointer
	mov		QWORD PTR [rsp+save_yca],r9	; save ycos_array pointer

;DEBUG
;	mov	rsi,QWORD PTR [rsp+save_xa]	; get ysin_array pointer
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movapd	xmm0, OWORD PTR [rsi]

;	movlpd	QWORD PTR [rdi],xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx],xmm0
;	movhpd	QWORD PTR 8[rbx],xmm0

;	movlpd	QWORD PTR 16[rdi],xmm1
;	movhpd	QWORD PTR 24[rdi],xmm1
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1

;	jmp	__final_check
;DEBUG

	mov		rax,rcx
	mov		QWORD PTR [rsp+save_nv],rcx	; save number of values
; see if too few values to call the main loop
	shr		rax,2				; get number of iterations
	jz		__vrda_cleanup			; jump if only single calls
; prepare the iteration counts
	mov		QWORD PTR [rsp+p_iter],rax	; save number of iterations
	shl		rax,2
	sub		rcx,rax				; compute number of extra single calls
	mov		QWORD PTR [rsp+save_nv],rcx	; save number of left over values

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;START LOOP
align 16
__vrda_top:
; build the input _m128d
	movapd	xmm2, OWORD PTR __real_7fffffffffffffff	;+
	mov	rdx,  QWORD PTR __real_7fffffffffffffff

	mov		rsi,QWORD PTR [rsp+save_xa]	; get x_array pointer
	movlpd	xmm0,QWORD PTR [rsi]
	movhpd	xmm0,QWORD PTR [rsi+8]
	mov	rax, QWORD PTR [rsi]
	mov	rcx, QWORD PTR [rsi+8]
	movdqa	xmm6, xmm0				;+
	movdqa  OWORD PTR p_original[rsp], xmm0

	prefetch	QWORD PTR [rsi+64]
	add		rsi,32
	mov		QWORD PTR [rsp+save_xa],rsi	; save x_array pointer

	movlpd	xmm1,QWORD PTR [rsi-16]
	movhpd	xmm1,QWORD PTR [rsi-8]
	mov	r8, QWORD PTR [rsi-16]
	mov	r9, QWORD PTR [rsi-8]
	movdqa	xmm7, xmm1				;+
	movdqa  OWORD PTR p_original1[rsp], xmm1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;STARTMAIN

andpd 	xmm0, xmm2	;Unsign
andpd 	xmm1, xmm2	;Unsign

and	rax, rdx
and	rcx, rdx
and	r8,  rdx
and	r9,  rdx

movdqa	xmm12,xmm0
movdqa	xmm13,xmm1

pcmpgtd		xmm12, xmm6
pcmpgtd		xmm13, xmm7
movdqa		xmm6,  xmm12
movdqa		xmm7,  xmm13
psrldq		xmm12, 4
psrldq		xmm13, 4
psrldq		xmm6,  8
psrldq		xmm7,  8

mov 	rdx, 3FE921FB54442D18h			;piby4	+
mov	r10, 411E848000000000h			;5e5	+
movapd	xmm4, OWORD PTR __real_3fe0000000000000	;0.5 for later use	+

por	xmm12, xmm6
por	xmm13, xmm7
movd	r12, xmm12				;Move Sign to gpr **
movd	r13, xmm13				;Move Sign to gpr **


;DEBUG
;	movapd	xmm4, xmm12
;	movapd	xmm5, xmm13
;	jmp	__vrd4_sin_cleanup
;DEBUG



movapd	xmm2, xmm0				;x0
movapd	xmm3, xmm1				;x1
movapd	xmm6, xmm0				;x0
movapd	xmm7, xmm1				;x1

;DEBUG
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movlpd	QWORD PTR [rdi],xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx],xmm0
;	movhpd	QWORD PTR 8[rbx],xmm0

;	movlpd	QWORD PTR 16[rdi],xmm1
;	movhpd	QWORD PTR 24[rdi],xmm1
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1

;	jmp	__final_check
;DEBUG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm2 = x, xmm4 =0.5, xmm6 =x
; xmm3 = x, xmm5 =0.5, xmm7 =x
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
	movapd	xmm0, XMMWORD PTR __real_3fe45f306dc9c883
	mulpd	xmm2, xmm0						; * twobypi
	mulpd	xmm3, xmm0						; * twobypi

	addpd	xmm2,xmm4						; +0.5, npi2
	addpd	xmm3,xmm4						; +0.5, npi2

	movapd	xmm0,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	movapd	xmm1,XMMWORD PTR __real_3ff921fb54400000		; piby2_1

	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers

	xorpd	xmm12, xmm12

	cvttpd2dq	xmm5,xmm3					; convert packed double to packed integers

	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	movapd	xmm9,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2

	cvtdq2pd	xmm2,xmm4					; and back to double.
	cvtdq2pd	xmm3,xmm5					; and back to double.


;      /* Subtract the multiple from x to get an extra-precision remainder */

	movd	r8, xmm4						; Region
	movd	r9, xmm5						; Region

	mov 	rdx, 	QWORD PTR __reald_one_zero			; compare value for cossin path
	mov	r10,  r8						; For Sign of Sin
	mov	r11,  r9

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm0,xmm2						; npi2 * piby2_1;
       mulpd	xmm1,xmm3						; npi2 * piby2_1;

;      rtail  = npi2 * piby2_2;
       mulpd	xmm8,xmm2						; rtail
       mulpd	xmm9,xmm3						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm6,xmm0						; rhead  = x - npi2 * piby2_1;
       subpd	xmm7,xmm1						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm0, xmm6						; t
       movapd	xmm1, xmm7						; t

;      rhead  = t - rtail;
       subpd	xmm0, xmm8						; rhead
       subpd	xmm1, xmm9						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm2,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail
       mulpd	xmm3,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm6,xmm0						; t-rhead
       subpd	xmm7,xmm1						; t-rhead

       subpd	xmm8,xmm6						; - ((t - rhead) - rtail)
       subpd	xmm9,xmm7						; - ((t - rhead) - rtail)

       addpd	xmm8,xmm2						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);
       addpd	xmm9,xmm3						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = npi2 (int), xmm0 =rhead, xmm8 =rtail
; xmm5  = npi2 (int), xmm1 =rhead, xmm9 =rtail

	pand	xmm4, XMMWORD PTR __reald_one_one			;odd/even region for cos/sin
	pand	xmm5, XMMWORD PTR __reald_one_one			;odd/even region for cos/sin

	pcmpeqd		xmm4, xmm12
	pcmpeqd		xmm5, xmm12

	punpckldq	xmm4, xmm4
	punpckldq	xmm5, xmm5

;DEBUG
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movlpd	QWORD PTR [rdi], xmm4
;	movhpd	QWORD PTR 8[rdi],xmm4
;	movlpd	QWORD PTR [rbx], xmm4
;	movhpd	QWORD PTR 8[rbx],xmm4

;	movlpd	QWORD PTR 16[rdi],xmm5
;	movhpd	QWORD PTR 24[rdi],xmm5
;	movlpd	QWORD PTR 16[rbx],xmm5
;	movhpd	QWORD PTR 24[rbx],xmm5

;	jmp	__final_check
;DEBUG


	movapd		OWORD PTR p_region[rsp],  xmm4
	movapd		OWORD PTR p_region1[rsp], xmm5

	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
	shr	r11, 1						;~AB+A~B, A is sign and B is upper bit of region

	mov	rax, r10
	mov	rcx, r11

	not 	r12						;ADDED TO CHANGE THE LOGIC
	not 	r13						;ADDED TO CHANGE THE LOGIC
	and	r10, r12
	and	r11, r13

	not	rax
	not	rcx
	not	r12
	not	r13
	and	rax, r12
	and	rcx, r13

	or	r10, rax
	or	r11, rcx
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

	mov 	QWORD PTR p_signs[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_signs[rsp+8],r12		;write out upper sign bit
	mov 	QWORD PTR p_signs1[rsp],r11		;write out lower sign bit
	mov 	QWORD PTR p_signs1[rsp+8],r13		;write out upper sign bit

; GET_BITS_DP64(rhead-rtail, uy);			   		; originally only rhead
; xmm4  = Sign, xmm0 =rhead, xmm8 =rtail
; xmm5  = Sign, xmm1 =rhead, xmm9 =rtail
	movapd	xmm6,xmm0						; rhead
	movapd	xmm7,xmm1						; rhead

	subpd	xmm0,xmm8						; r = rhead - rtail
	subpd	xmm1,xmm9						; r = rhead - rtail

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = Sign, xmm0 = r, xmm6 =rhead, xmm8 =rtail
; xmm5  = Sign, xmm1 = r, xmm7 =rhead, xmm9 =rtail

	subpd	xmm6, xmm0				;rr=rhead-r
	subpd	xmm7, xmm1				;rr=rhead-r

	movapd	xmm2,xmm0				;move r for r2
	movapd	xmm3,xmm1				;move r for r2

	mulpd	xmm2,xmm0				;r2
	mulpd	xmm3,xmm1				;r2

	subpd	xmm6, xmm8				;rr=(rhead-r) -rtail
	subpd	xmm7, xmm9				;rr=(rhead-r) -rtail


	add	r8,QWORD PTR __reald_one_one
	add	r9,QWORD PTR __reald_one_one

	and	r8,QWORD PTR __reald_two_two
	and	r9,QWORD PTR __reald_two_two

	shr	r8,1
	shr	r9,1

	mov	r12,	r8
	mov	r13,	r9

	and	r12,	QWORD PTR __reald_one_zero	;mask out the lower sign bit leaving the upper sign bit
	and	r13,	QWORD PTR __reald_one_zero	;mask out the lower sign bit leaving the upper sign bit

	shl	r8,	63				;shift lower sign bit left by 63 bits
	shl	r9,	63				;shift lower sign bit left by 63 bits

	shl	r12,	31				;shift upper sign bit left by 31 bits
	shl	r13,	31				;shift upper sign bit left by 31 bits

	mov 	QWORD PTR p_signc[rsp],r8		;write out lower sign bit
	mov 	QWORD PTR p_signc[rsp+8],r12		;write out upper sign bit
	mov 	QWORD PTR p_signc1[rsp],r9		;write out lower sign bit
	mov 	QWORD PTR p_signc1[rsp+8],r13		;write out upper sign bit

;DEBUG
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movapd	xmm0, QWORD PTR p_region[rsp]
;	movapd	xmm1, QWORD PTR p_region1[rsp]

;	movlpd	QWORD PTR [rdi], xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx], xmm0
;	movhpd	QWORD PTR 8[rbx],xmm0

;	movlpd	QWORD PTR 16[rdi],xmm1
;	movhpd	QWORD PTR 24[rdi],xmm1
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1

;	jmp	__final_check
;DEBUG

;DEBUG
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movapd	xmm0, QWORD PTR p_signs[rsp]
;	movapd	xmm1, QWORD PTR p_signc[rsp]

;	movlpd	QWORD PTR [rdi], xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx], xmm1
;	movhpd	QWORD PTR 8[rbx],xmm1

;	movapd	xmm0, QWORD PTR p_signs1[rsp]
;	movapd	xmm1, QWORD PTR p_signc1[rsp]

;	movlpd	QWORD PTR 16[rdi],xmm0
;	movhpd	QWORD PTR 24[rdi],xmm0
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1
;
;	jmp	__final_check
;DEBUG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
sinsin_sinsin_piby4:

	movapd	OWORD PTR p_temp[rsp],  xmm0			; copy of x
	movapd	OWORD PTR p_temp1[rsp], xmm1			; copy of x

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR sinarray+50h			; s6
	movdqa	xmm5, OWORD PTR sinarray+50h			; s6
	movapd	xmm8, OWORD PTR sinarray+20h			; s3
	movapd	xmm9, OWORD PTR sinarray+20h			; s3

	movdqa	xmm12, OWORD PTR cosarray+50h			; c6
	movdqa	xmm13, OWORD PTR cosarray+50h			; c6
	movapd	xmm14, OWORD PTR cosarray+20h			; c3
	movapd	xmm15, OWORD PTR cosarray+20h			; c3

	movapd	OWORD PTR p_temp2[rsp], xmm2			; copy of x2
	movapd	OWORD PTR p_temp3[rsp], xmm3			; copy of x2

	mulpd	xmm4,  xmm2					; s6*x2
	mulpd	xmm5,  xmm3					; s6*x2
	mulpd	xmm8,  xmm2					; s3*x2
	mulpd	xmm9,  xmm3					; s3*x2

	mulpd	xmm12,  xmm2					; s6*x2
	mulpd	xmm13,  xmm3					; s6*x2
	mulpd	xmm14,  xmm2					; s3*x2
	mulpd	xmm15,  xmm3					; s3*x2

	mulpd	xmm10, xmm2					; x4
	mulpd	xmm11, xmm3					; x4

	addpd	xmm4, OWORD PTR sinarray+40h			; s5+x2s6
	addpd	xmm5, OWORD PTR sinarray+40h			; s5+x2s6
	addpd	xmm8, OWORD PTR sinarray+10h			; s2+x2C3
	addpd	xmm9, OWORD PTR sinarray+10h			; s2+x2C3

	addpd	xmm12, OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm13, OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm14, OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm15, OWORD PTR cosarray+10h			; c2+x2C3

	mulpd	xmm10, xmm2					; x6
	mulpd	xmm11, xmm3					; x6

	mulpd	xmm4, xmm2					; x2(s5+x2s6)
	mulpd	xmm5, xmm3					; x2(s5+x2s6)
	mulpd	xmm8, xmm2					; x2(s2+x2C3)
	mulpd	xmm9, xmm3					; x2(s2+x2C3)

	mulpd	xmm12, xmm2					; x2(s5+x2s6)
	mulpd	xmm13, xmm3					; x2(s5+x2s6)
	mulpd	xmm14, xmm2					; x2(s2+x2C3)
	mulpd	xmm15, xmm3					; x2(s2+x2C3)

	mulpd	xmm2, OWORD PTR __real_3fe0000000000000		; 0.5 *x2
	mulpd	xmm3, OWORD PTR __real_3fe0000000000000		; 0.5 *x2

	addpd	xmm4, OWORD PTR sinarray+30h			; s4 + x2(s5+x2s6)
	addpd	xmm5, OWORD PTR sinarray+30h			; s4 + x2(s5+x2s6)
	addpd	xmm8, OWORD PTR sinarray			; s1 + x2(s2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; s1 + x2(s2+x2C3)

	movapd	OWORD PTR p_temp4[rsp], xmm2			; copy of r
	movapd	OWORD PTR p_temp5[rsp], xmm3			; copy of r

	movapd	xmm0, xmm2					; r
	movapd	xmm1, xmm3					; r

	addpd	xmm12, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm13, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm14, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm15, OWORD PTR cosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm2, xmm6					; 0.5 * x2 *xx
	mulpd	xmm3, xmm7					; 0.5 * x2 *xx

	subpd	xmm0, OWORD PTR __real_3ff0000000000000		; -t=r-1.0
	subpd	xmm1, OWORD PTR __real_3ff0000000000000		; -t=r-1.0

	mulpd	xmm4, xmm10					; x6(s4 + x2(s5+x2s6))
	mulpd	xmm5, xmm11					; x6(s4 + x2(s5+x2s6))

	mulpd	xmm12, xmm10					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm13, xmm11					; x6(c4 + x2(c5+x2c6))

	addpd   xmm0, OWORD PTR __real_3ff0000000000000		; 1+(-t)
	addpd   xmm1, OWORD PTR __real_3ff0000000000000		; 1+(-t)

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zs

	addpd	xmm12, xmm14					; zc
	addpd	xmm13, xmm15					; zc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = 0.5 * x2 *xx, xmm4 = zs, xmm12 = zc, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = 0.5 * x2 *xx, xmm5 = zs, xmm13 = zc, xmm7 =rr

; Free
; xmm10, xmm8, xmm14
; xmm11, xmm9, xmm15
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm10, OWORD PTR p_temp2[rsp]			; x2 for x3
	movapd	xmm11, OWORD PTR p_temp3[rsp]			; x2 for x3

	movapd	xmm8, xmm10					; x2 for x4
	movapd	xmm9, xmm11					; x2 for x4

	movapd	xmm14, OWORD PTR p_temp[rsp]			; x for x*xx
	movapd	xmm15, OWORD PTR p_temp1[rsp]			; x for x*xx

	subpd   xmm0, OWORD PTR p_temp4[rsp]			; (1 + (-t)) - r
	subpd   xmm1, OWORD PTR p_temp5[rsp]			; (1 + (-t)) - r

	mulpd	xmm10, xmm14					; x3
	mulpd	xmm11, xmm15					; x3

	mulpd	xmm8, xmm8					; x4
	mulpd	xmm9, xmm9					; x4

	mulpd	xmm14, xmm6					; x*xx
	mulpd	xmm15, xmm7					; x*xx

	mulpd	xmm4, xmm10					; x3 * zs
	mulpd	xmm5, xmm11					; x3 * zs

	mulpd	xmm12, xmm8					; x4 * zc
	mulpd	xmm13, xmm9					; x4 * zc

	subpd	xmm4, xmm2					; x3*zs-0.5 * x2 *xx
	subpd	xmm5, xmm3					; x3*zs-0.5 * x2 *xx

	subpd   xmm0, xmm14					; ((1 + (-t)) - r) -x*xx
	subpd   xmm1, xmm15					; ((1 + (-t)) - r) -x*xx


	movapd	xmm10, OWORD PTR p_temp4[rsp]			; r for t
	movapd	xmm11, OWORD PTR p_temp5[rsp]			; r for t

	addpd	xmm4, xmm6					; sin+xx
	addpd	xmm5, xmm7					; sin+xx

	addpd   xmm12, xmm0					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addpd   xmm13, xmm1					; x4*zc + (((1 + (-t)) - r) - x*xx)

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0

	movapd	xmm2, OWORD PTR p_region[rsp]
	movapd	xmm3, OWORD PTR p_region1[rsp]

	movapd	xmm8, xmm2
	movapd	xmm9, xmm3

	addpd	xmm4, OWORD PTR p_temp[rsp]			; sin+xx+x
	addpd	xmm5, OWORD PTR p_temp1[rsp]			; sin+xx+x

	subpd	xmm12, xmm10					; cos + (-t)
	subpd	xmm13, xmm11					; cos + (-t)

; xmm4	= sin, xmm5  = sin
; xmm12	= cos, xmm13 = cos

	andnpd	xmm8, xmm4
	andnpd	xmm9, xmm5

	andpd	xmm4, xmm2
	andpd	xmm5, xmm3

	andnpd	xmm2, xmm12
	andnpd	xmm3, xmm13

	andpd	xmm12,OWORD PTR p_region[rsp]
	andpd	xmm13,OWORD PTR p_region1[rsp]

	orpd	xmm4, xmm2
	orpd	xmm5, xmm3

	orpd	xmm12, xmm8
	orpd	xmm13, xmm9

;DEBUG
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movlpd	QWORD PTR [rdi], xmm4
;	movhpd	QWORD PTR 8[rdi],xmm4
;	movlpd	QWORD PTR [rbx], xmm12
;	movhpd	QWORD PTR 8[rbx],xmm12

;	movlpd	QWORD PTR 16[rdi],xmm5
;	movhpd	QWORD PTR 24[rdi],xmm5
;	movlpd	QWORD PTR 16[rbx],xmm13
;	movhpd	QWORD PTR 24[rbx],xmm13

;	jmp	__final_check
;DEBUG
	jmp __vrd4_sin_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
first_or_next3_arg_gt_5e5:
; rax, rcx, r8, r9

;DEBUG
;	movapd	xmm4, xmm0
;	movapd	xmm5, xmm1
;	movapd	xmm12, xmm0
;	movapd	xmm13, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG

	cmp	rcx, r10				;is upper arg >= 5e5
	jae	both_arg_gt_5e5

lower_arg_gt_5e5:
; Upper Arg is < 5e5, Lower arg is >= 5e5
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5
; Be sure not to use xmm1, xmm3 and xmm7
; Use xmm5, xmm8, xmm10, xmm12
;	    xmm9, xmm11, xmm13


	movlpd	QWORD PTR r[rsp], xmm0		;Save lower fp arg for remainder_piby2 call
	movhlps	xmm0, xmm0			;Needed since we want to work on upper arg
	movhlps	xmm2, xmm2
	movhlps	xmm6, xmm6

; Work on Upper arg
; Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm4					; xmm2 = npi2=(x*twobypi+0.5)
	movsd	xmm8,QWORD PTR __real_3ff921fb54400000		; xmm8 = piby2_1
	cvttsd2si	ecx,xmm2				; ecx = npi2 trunc to ints
	movsd	xmm10,QWORD PTR __real_3dd0b4611a600000		; xmm10 = piby2_2
	cvtsi2sd	xmm2,ecx				; xmm2 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm8,xmm2					; npi2 * piby2_1
	subsd	xmm6,xmm8					; xmm6 = rhead =(x-npi2*piby2_1)
	movsd	xmm12,QWORD PTR __real_3ba3198a2e037073		; xmm12 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm6					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm10,xmm2					; xmm1 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm6, xmm10					; xmm6 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm12,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm6					; t-rhead
       subsd	xmm10,xmm5					; (rtail-(t-rhead))
       addsd	xmm10,xmm12					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp+4], ecx			; store upper region
       movsd	xmm0,xmm6
       subsd	xmm0,xmm10					; xmm0 = r=(rhead-rtail)
       subsd	xmm6, xmm0					; rr=rhead-r
       subsd	xmm6, xmm10					; xmm6 = rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r[rsp+8], xmm0			; store upper r
       movlpd	QWORD PTR rr[rsp+8], xmm6			; store upper rr

;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, xmm1
;	movapd	xmm12, OWORD PTR r[rsp]
;	movapd	xmm13, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Note that volatiles will be trashed by the call
;We will construct r, rr, region and sign

; Work on Lower arg
	mov		r11,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrd4_sin_lower_naninf

	mov	 QWORD PTR p_temp[rsp], r8
	mov	 QWORD PTR p_temp2[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

	lea	 r9, 	QWORD PTR region[rsp]			; lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 rdx,	QWORD PTR r[rsp]
	movlpd	 xmm0,  QWORD PTR r[rsp]	;Restore lower fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r8, QWORD PTR p_temp[rsp]
	mov	 r9, QWORD PTR p_temp2[rsp]
	movapd	 xmm1, OWORD PTR p_temp1[rsp]
	movapd	 xmm3, OWORD PTR p_temp3[rsp]
	movapd	 xmm7,OWORD PTR p_temp5[rsp]
	jmp 	@F

__vrd4_sin_lower_naninf:
	mov	rax, QWORD PTR p_original[rsp]			; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				; rr = 0
	mov	DWORD PTR region[rsp], r10d			; region =0
	and 	r12, QWORD PTR __real_naninf_lower_sign_mask	; Sign
align 16
@@:


;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, xmm1
;	movapd	xmm12, OWORD PTR r[rsp]
;	movapd	xmm13, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG



	jmp check_next2_args


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
both_arg_gt_5e5:
;Upper Arg is >= 5e5, Lower arg is >= 5e5
; rax, rcx, r8, r9
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5


;DEBUG
;	movapd	xmm4, xmm0
;	movapd	xmm5, xmm1
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG




	movhlps xmm6, xmm0		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrd4_sin_lower_naninf_of_both_gt_5e5

	mov	 QWORD PTR p_temp[rsp], rcx			;Save upper arg
	mov	 QWORD PTR p_temp2[rsp], r8
	mov	 QWORD PTR p_temp4[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

	lea	 r9, 	QWORD PTR region[rsp]			;lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 rdx,	QWORD PTR r[rsp]

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	 rcx, QWORD PTR p_temp[rsp]			;Restore upper arg
	mov	 r8, QWORD PTR p_temp2[rsp]
	mov	 r9, QWORD PTR p_temp4[rsp]
	movapd	 xmm1, OWORD PTR p_temp1[rsp]
	movapd	 xmm3, OWORD PTR p_temp3[rsp]
	movapd	 xmm7,OWORD PTR p_temp5[rsp]

	jmp 	@F

__vrd4_sin_lower_naninf_of_both_gt_5e5:				;lower arg is nan/inf
	mov	rax, QWORD PTR p_original[rsp]
	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				;rr = 0
	mov	DWORD PTR region[rsp], r10d			;region = 0
	and 	r12, QWORD PTR __real_naninf_lower_sign_mask	; Sign

align 16
@@:
	mov		r11,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r11
	and		r10,rcx
	cmp		r10,r11
	jz		__vrd4_sin_upper_naninf_of_both_gt_5e5


	mov	 QWORD PTR p_temp[rsp], r8
	mov	 QWORD PTR p_temp2[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

	lea	 r9, 	QWORD PTR region[rsp+4]			;upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 rdx,	QWORD PTR r[rsp+8]
	movapd	 xmm0,  xmm6			;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r8, QWORD PTR p_temp[rsp]
	mov	 r9, QWORD PTR p_temp2[rsp]
	movapd	 xmm1, OWORD PTR p_temp1[rsp]
	movapd	 xmm3, OWORD PTR p_temp3[rsp]
	movapd	 xmm7,OWORD PTR p_temp5[rsp]

	jmp 	@F

__vrd4_sin_upper_naninf_of_both_gt_5e5:
	mov	rcx, QWORD PTR p_original[rsp+8]		;upper arg is nan/inf
;	movd	rcx, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			;rr = 0
	mov	DWORD PTR region[rsp+4], r10d			;region = 0
	and 	r12, QWORD PTR __real_naninf_upper_sign_mask	; Sign
align 16
@@:
	jmp check_next2_args

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
second_or_next2_arg_gt_5e5:
; Upper Arg is >= 5e5, Lower arg is < 5e5
; rax, rcx, r8, r9
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5
; Do not use xmm1, xmm3, xmm7
; Restore xmm4 and xmm1, xmm3, xmm7
; Can use xmm8, xmm10, xmm12
;   xmm5, xmm9, xmm11, xmm13

	movhpd	QWORD PTR r[rsp+8], xmm0	;Save upper fp arg for remainder_piby2 call
;	movlhps	xmm0, xmm0			;Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
;	movlhps	xmm2, xmm2
;	movlhps	xmm6, xmm6

; Work on Lower arg
; Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm4					; xmm2 = npi2=(x*twobypi+0.5)
	movsd	xmm8,QWORD PTR __real_3ff921fb54400000		; xmm3 = piby2_1
	cvttsd2si	eax,xmm2				; ecx = npi2 trunc to ints
	movsd	xmm10,QWORD PTR __real_3dd0b4611a600000		; xmm1 = piby2_2
	cvtsi2sd	xmm2,eax				; xmm2 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm8,xmm2					; npi2 * piby2_1
	subsd	xmm6,xmm8					; xmm6 = rhead =(x-npi2*piby2_1)
	movsd	xmm12,QWORD PTR __real_3ba3198a2e037073		; xmm7 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm6					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm10,xmm2					; xmm1 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm6, xmm10					; xmm6 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm12,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm6					; t-rhead
       subsd	xmm10,xmm5					; (rtail-(t-rhead))
       addsd	xmm10,xmm12					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp], eax			; store upper region
       movsd	xmm0,xmm6
       subsd	xmm0,xmm10					; xmm0 = r=(rhead-rtail)
       subsd	xmm6, xmm0					; rr=rhead-r
       subsd	xmm6, xmm10					; xmm6 = rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r[rsp], xmm0				; store upper r
       movlpd	QWORD PTR rr[rsp], xmm6				; store upper rr

;Work on Upper arg
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
	mov		r11,07ff0000000000000h			; is upper arg nan/inf
	mov		r10,r11
	and		r10,rcx
	cmp		r10,r11
	jz		__vrd4_sin_upper_naninf


	mov	 QWORD PTR p_temp[rsp], r8
	mov	 QWORD PTR p_temp2[rsp], r9
	movapd	 OWORD PTR p_temp1[rsp], xmm1
	movapd	 OWORD PTR p_temp3[rsp], xmm3
	movapd	 OWORD PTR p_temp5[rsp], xmm7

	lea	 r9, 	QWORD PTR region[rsp+4]			; upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 rdx,	QWORD PTR r[rsp+8]
	movlpd	 xmm0,  QWORD PTR r[rsp+8]	;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	r8,  QWORD PTR p_temp[rsp]
	mov	r9,  QWORD PTR p_temp2[rsp]
	movapd	xmm1, OWORD PTR p_temp1[rsp]
	movapd	xmm3, OWORD PTR p_temp3[rsp]
	movapd	xmm7, OWORD PTR p_temp5[rsp]
	jmp 	@F

__vrd4_sin_upper_naninf:
	mov	rcx, QWORD PTR p_original[rsp+8]		; upper arg is nan/inf
;	mov	rcx, QWORD PTR r[rsp+8]				; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			; rr = 0
	mov	DWORD PTR region[rsp+4], r10d			; region =0
	and 	r12, QWORD PTR __real_naninf_upper_sign_mask	; Sign

align 16
@@:
	jmp check_next2_args


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
check_next2_args:

;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, xmm1
;	movapd	xmm12, OWORD PTR r[rsp]
;	movapd	xmm13, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG

	mov	r10, 411E848000000000h			;5e5	+

	cmp	r8, r10
	jae	first_second_done_third_or_fourth_arg_gt_5e5

	cmp	r9, r10
	jae	first_second_done_fourth_arg_gt_5e5

; Work on next two args, both < 5e5
; xmm1, xmm3, xmm5 = x, xmm4 = 0.5
	movapd	xmm4, OWORD PTR __real_3fe0000000000000			; Restore 0.5

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

       movapd	xmm7,xmm1						; rhead
       subpd	xmm1,xmm9						; r = rhead - rtail
       movapd	OWORD PTR r1[rsp], xmm1

       subpd	xmm7, xmm1						; rr=rhead-r
       subpd	xmm7, xmm9						; rr=(rhead-r) -rtail
       movapd	OWORD PTR rr1[rsp], xmm7

	jmp	__vrd4_sin_reconstruct


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
third_or_fourth_arg_gt_5e5:
;first two args are < 5e5, third arg >= 5e5, fourth arg >= 5e5 or < 5e5
; rax, rcx, r8, r9
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5
; Do not use xmm1, xmm3, xmm7
; Can use 	xmm9, xmm11, xmm13
; 	xmm5,   xmm8, xmm10, xmm12
; Restore xmm4

; Work on first two args, both < 5e5



	mulpd	xmm2, XMMWORD PTR __real_3fe45f306dc9c883						; * twobypi
	addpd	xmm2,xmm4						; +0.5, npi2
	movapd	xmm0,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtdq2pd	xmm2,xmm4					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */
	movd	QWORD PTR region[rsp], xmm4						; Region

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm0,xmm2						; npi2 * piby2_1;
;      rtail  = npi2 * piby2_2;
       mulpd	xmm8,xmm2						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm6,xmm0						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm0, xmm6						; t

;      rhead  = t - rtail;
       subpd	xmm0, xmm8						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm2,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm6,xmm0						; t-rhead
       subpd	xmm8,xmm6						; - ((t - rhead) - rtail)
       addpd	xmm8,xmm2						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

       movapd	xmm6,xmm0						; rhead
       subpd	xmm0,xmm8						; r = rhead - rtail
       movapd	OWORD PTR r[rsp], xmm0

       subpd	xmm6, xmm0						; rr=rhead-r
       subpd	xmm6, xmm8						; rr=(rhead-r) -rtail
       movapd	OWORD PTR rr[rsp], xmm6


; Work on next two args, third arg >= 5e5, fourth arg >= 5e5 or < 5e5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
first_second_done_third_or_fourth_arg_gt_5e5:
; rax, rcx, r8, r9
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5


;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, xmm1
;	movapd	xmm12, OWORD PTR r[rsp]
;	movapd	xmm13, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG

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
	movapd	xmm4, OWORD PTR __real_3fe0000000000000	;0.5 for later use

; Work on Upper arg
; Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
	mulsd	xmm3,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm3,xmm4					; xmm3 = npi2=(x*twobypi+0.5)
	movsd	xmm2,QWORD PTR __real_3ff921fb54400000		; xmm2 = piby2_1
	cvttsd2si	r9d,xmm3				; r9d = npi2 trunc to ints
	movsd	xmm0,QWORD PTR __real_3dd0b4611a600000		; xmm0 = piby2_2
	cvtsi2sd	xmm3,r9d				; xmm3 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm2,xmm3					; npi2 * piby2_1
	subsd	xmm7,xmm2					; xmm7 = rhead =(x-npi2*piby2_1)
	movsd	xmm6,QWORD PTR __real_3ba3198a2e037073		; xmm6 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm7					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm0,xmm3					; xmm0 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm7, xmm0					; xmm7 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm6,xmm3     					; npi2 * piby2_2tail
       subsd	xmm5,xmm7					; t-rhead
       subsd	xmm0,xmm5					; (rtail-(t-rhead))
       addsd	xmm0,xmm6					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region1[rsp+4], r9d			; store upper region
       movsd	xmm1,xmm7
       subsd	xmm1,xmm0					; xmm1 = r=(rhead-rtail)
       subsd	xmm7, xmm1					; rr=rhead-r
       subsd	xmm7, xmm0					; xmm7 = rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r1[rsp+8], xmm1			; store upper r
       movlpd	QWORD PTR rr1[rsp+8], xmm7			; store upper rr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign

; Work on Lower arg
	mov		r11,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r11
	and		r10,r8
	cmp		r10,r11
	jz		__vrd4_sin_lower_naninf_higher

	lea	 r9, 	QWORD PTR region1[rsp]			; lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr1[rsp]
	lea	 rdx,	QWORD PTR r1[rsp]
	movlpd	 xmm0,  QWORD PTR r1[rsp]	;Restore lower fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrd4_sin_lower_naninf_higher:
	mov	r8, QWORD PTR p_original1[rsp]			; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r8,r11
	mov	QWORD PTR r1[rsp],r8				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp], r10				; rr = 0
	mov	DWORD PTR region1[rsp], r10d			; region =0
	and 	r13, QWORD PTR __real_naninf_lower_sign_mask	; Sign

align 16
@@:
	jmp __vrd4_sin_reconstruct







align 16
both_arg_gt_5e5_higher:
; Upper Arg is >= 5e5, Lower arg is >= 5e5
; r8, r9
; xmm1, xmm3, xmm7 = x, xmm4 = 0.5


;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movd	xmm5, r8
;	movapd	xmm12, OWORD PTR r[rsp]
;	movd	xmm13, r8
;	jmp __vrd4_sin_cleanup
;DEBUG

	movhlps xmm7, xmm1		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,r8
	cmp		r10,r11
	jz		__vrd4_sin_lower_naninf_of_both_gt_5e5_higher

	mov	 QWORD PTR p_temp1[rsp], r9			;Save upper arg
	lea	 r9, 	QWORD PTR region1[rsp]			;lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr1[rsp]
	lea	 rdx,	QWORD PTR r1[rsp]
	movapd	 xmm0, xmm1

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r9, QWORD PTR p_temp1[rsp]			;Restore upper arg


;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	mov	QWORD PTR r1[rsp+8], r9
;	movapd	xmm5, OWORD PTR r1[rsp]
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrd4_sin_cleanup
;DEBUG



	jmp 	@F

__vrd4_sin_lower_naninf_of_both_gt_5e5_higher:				;lower arg is nan/inf
	mov	r8, QWORD PTR p_original1[rsp]
	mov	r11,00008000000000000h
	or	r8,r11
	mov	QWORD PTR r1[rsp],r8				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp], r10				;rr = 0
	mov	DWORD PTR region1[rsp], r10d			;region = 0
	and 	r13, QWORD PTR __real_naninf_lower_sign_mask	; Sign

align 16
@@:
	mov		r11,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r11
	and		r10,r9
	cmp		r10,r11
	jz		__vrd4_sin_upper_naninf_of_both_gt_5e5_higher

	lea	 r9, 	QWORD PTR region1[rsp+4]			;upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr1[rsp+8]
	lea	 rdx,	QWORD PTR r1[rsp+8]
	movapd	 xmm0,  xmm7			;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrd4_sin_upper_naninf_of_both_gt_5e5_higher:
	mov	r9, QWORD PTR p_original1[rsp+8]		;upper arg is nan/inf
;	movd	r9, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r9,r11
	mov	QWORD PTR r1[rsp+8],r9				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp+8], r10			;rr = 0
	mov	DWORD PTR region1[rsp+4], r10d			;region = 0
	and 	r13, QWORD PTR __real_naninf_upper_sign_mask	; Sign

align 16
@@:

;DEBUG
;	movapd	xmm4, OWORD PTR region[rsp]
;	movapd	xmm5, OWORD PTR region1[rsp]
;	movapd	xmm12, OWORD PTR region[rsp]
;	movapd	xmm13, OWORD PTR region1[rsp]
;	jmp __vrd4_sin_cleanup
;DEBUG


	jmp __vrd4_sin_reconstruct

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
fourth_arg_gt_5e5:
;first two args are < 5e5, third arg < 5e5, fourth arg >= 5e5
;rax, rcx, r8, r9
;xmm0, xmm2, xmm6 = x, xmm4 = 0.5

; Work on first two args, both < 5e5

	mulpd	xmm2, XMMWORD PTR __real_3fe45f306dc9c883						; * twobypi
	addpd	xmm2,xmm4						; +0.5, npi2
	movapd	xmm0,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtdq2pd	xmm2,xmm4					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */
	movd	QWORD PTR region[rsp], xmm4						; Region

;      rhead  = x - npi2 * piby2_1;
       mulpd	xmm0,xmm2						; npi2 * piby2_1;
;      rtail  = npi2 * piby2_2;
       mulpd	xmm8,xmm2						; rtail

;      rhead  = x - npi2 * piby2_1;
       subpd	xmm6,xmm0						; rhead  = x - npi2 * piby2_1;

;      t  = rhead;
       movapd	xmm0, xmm6						; t

;      rhead  = t - rtail;
       subpd	xmm0, xmm8						; rhead

;      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm2,XMMWORD PTR __real_3ba3198a2e037073		; npi2 * piby2_2tail

       subpd	xmm6,xmm0						; t-rhead
       subpd	xmm8,xmm6						; - ((t - rhead) - rtail)
       addpd	xmm8,xmm2						; rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

       movapd	xmm6,xmm0						; rhead
       subpd	xmm0,xmm8						; r = rhead - rtail
       movapd	OWORD PTR r[rsp], xmm0

       subpd	xmm6, xmm0						; rr=rhead-r
       subpd	xmm6, xmm8						; rr=(rhead-r) -rtail
       movapd	OWORD PTR rr[rsp], xmm6


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
	movapd	xmm4, OWORD PTR __real_3fe0000000000000	;0.5 for later use

; Work on Lower arg
; Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

	mulsd	xmm3,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm3,xmm4					; xmm3 = npi2=(x*twobypi+0.5)
	movsd	xmm2,QWORD PTR __real_3ff921fb54400000		; xmm2 = piby2_1
	cvttsd2si	r8d,xmm3				; r8d = npi2 trunc to ints
	movsd	xmm0,QWORD PTR __real_3dd0b4611a600000		; xmm0 = piby2_2
	cvtsi2sd	xmm3,r8d				; xmm3 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm2,xmm3					; npi2 * piby2_1
	subsd	xmm7,xmm2					; xmm7 = rhead =(x-npi2*piby2_1)
	movsd	xmm6,QWORD PTR __real_3ba3198a2e037073		; xmm6 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm7					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm0,xmm3					; xmm0 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm7, xmm0					; xmm7 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm6,xmm3     					; npi2 * piby2_2tail
       subsd	xmm5,xmm7					; t-rhead
       subsd	xmm0,xmm5					; (rtail-(t-rhead))
       addsd	xmm0,xmm6					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region1[rsp], r8d			; store lower region
       movsd	xmm1, xmm7
       subsd	xmm1, xmm0					; xmm0 = r=(rhead-rtail)
       subsd	xmm7, xmm1					; rr=rhead-r
       subsd	xmm7, xmm0					; xmm6 = rr=((rhead-r) -rtail)

       movlpd	QWORD PTR r1[rsp], xmm1				; store upper r
       movlpd	QWORD PTR rr1[rsp], xmm7				; store upper rr

;Work on Upper arg
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
	mov		r11,07ff0000000000000h			; is upper arg nan/inf
	mov		r10,r11
	and		r10,r9
	cmp		r10,r11
	jz		__vrd4_sin_upper_naninf_higher

	lea	 r9, 	QWORD PTR region1[rsp+4]			; upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr1[rsp+8]
	lea	 rdx,	QWORD PTR r1[rsp+8]
	movlpd	 xmm0,  QWORD PTR r1[rsp+8]	;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrd4_sin_upper_naninf_higher:
	mov	r9, QWORD PTR p_original1[rsp+8]		; upper arg is nan/inf
;	mov	r9, QWORD PTR r1[rsp+8]				; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r9,r11
	mov	QWORD PTR r1[rsp+8],r9				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp+8], r10			; rr = 0
	mov	DWORD PTR region1[rsp+4], r10d			; region =0
	and 	r13, QWORD PTR __real_naninf_upper_sign_mask	; Sign

align 16
@@:
	jmp	__vrd4_sin_reconstruct


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
__vrd4_sin_reconstruct:
;Results
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm0, OWORD PTR r[rsp]
	movapd	xmm1, OWORD PTR r1[rsp]

	movapd	xmm6, OWORD PTR rr[rsp]
	movapd	xmm7, OWORD PTR rr1[rsp]

	mov	r8,	QWORD PTR region[rsp]
	mov	r9,	QWORD PTR region1[rsp]

	movlpd	xmm4,	QWORD PTR region[rsp]
	movlpd	xmm5,	QWORD PTR region1[rsp]

	pand	xmm4, XMMWORD PTR __reald_one_one			;odd/even region for cos/sin
	pand	xmm5, XMMWORD PTR __reald_one_one			;odd/even region for cos/sin

	xorpd	xmm12, xmm12
	pcmpeqd		xmm4, xmm12
	pcmpeqd		xmm5, xmm12

	punpckldq	xmm4, xmm4
	punpckldq	xmm5, xmm5

	movapd		OWORD PTR p_region[rsp],  xmm4
	movapd		OWORD PTR p_region1[rsp], xmm5

	mov 	rdx, 	QWORD PTR __reald_one_zero		;compare value for cossin path
	mov 	r10,	r8
	mov 	r11,	r9

	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
	shr	r11, 1						;~AB+A~B, A is sign and B is upper bit of region

	mov	rax, r10
	mov	rcx, r11

	not 	r12						;ADDED TO CHANGE THE LOGIC
	not 	r13						;ADDED TO CHANGE THE LOGIC
	and	r10, r12
	and	r11, r13

	not	rax
	not	rcx
	not	r12
	not	r13
	and	rax, r12
	and	rcx, r13

	or	r10, rax
	or	r11, rcx
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

	mov 	QWORD PTR p_signs[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_signs[rsp+8],r12		;write out upper sign bit
	mov 	QWORD PTR p_signs1[rsp],r11		;write out lower sign bit
	mov 	QWORD PTR p_signs1[rsp+8],r13		;write out upper sign bit

	movapd	xmm2,xmm0				; r
	movapd	xmm3,xmm1				; r

	mulpd	xmm2,xmm0				; r2
	mulpd	xmm3,xmm1				; r2

	add	r8,QWORD PTR __reald_one_one
	add	r9,QWORD PTR __reald_one_one

	and	r8,QWORD PTR __reald_two_two
	and	r9,QWORD PTR __reald_two_two

	shr	r8,1
	shr	r9,1

	mov	rax,	r8
	mov	rcx,	r9

	and	rax,	QWORD PTR __reald_one_zero	;mask out the lower sign bit leaving the upper sign bit
	and	rcx,	QWORD PTR __reald_one_zero	;mask out the lower sign bit leaving the upper sign bit

	shl	r8,	63				;shift lower sign bit left by 63 bits
	shl	r9,	63				;shift lower sign bit left by 63 bits

	shl	rax,	31				;shift upper sign bit left by 31 bits
	shl	rcx,	31				;shift upper sign bit left by 31 bits

	mov 	QWORD PTR p_signc[rsp],r8		;write out lower sign bit
	mov 	QWORD PTR p_signc[rsp+8],rax		;write out upper sign bit
	mov 	QWORD PTR p_signc1[rsp],r9		;write out lower sign bit
	mov 	QWORD PTR p_signc1[rsp+8],rcx		;write out upper sign bit

	jmp 	sinsin_sinsin_piby4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
__vrd4_sin_cleanup:

	xorpd	xmm4, p_signs[rsp]		; (+) Sign
	xorpd	xmm5, p_signs1[rsp]		; (+) Sign

	xorpd	xmm12, p_signc[rsp]		; (+) Sign
	xorpd	xmm13, p_signc1[rsp]		; (+) Sign

;DEBUG
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

;	movlpd	QWORD PTR [rdi], xmm4
;	movhpd	QWORD PTR 8[rdi],xmm4
;	movlpd	QWORD PTR [rbx], xmm12
;	movhpd	QWORD PTR 8[rbx],xmm12

;	movlpd	QWORD PTR 16[rdi],xmm5
;	movhpd	QWORD PTR 24[rdi],xmm5
;	movlpd	QWORD PTR 16[rbx],xmm13
;	movhpd	QWORD PTR 24[rbx],xmm13
;
;	jmp	__final_check
;DEBUG

__vrda_bottom1:
; store the result _m128d
	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer

	movlpd	QWORD PTR [rdi],xmm4
	movhpd	QWORD PTR 8[rdi],xmm4

	movlpd	QWORD PTR [rbx],xmm12
	movhpd	QWORD PTR 8[rbx],xmm12

__vrda_bottom2:

	prefetch	QWORD PTR [rdi+64]
	prefetch	QWORD PTR [rbx+64]

	add		rdi,32
	add		rbx,32

	mov		QWORD PTR [rsp+save_ysa],rdi	; save ysin_array pointer
	mov		QWORD PTR [rsp+save_yca],rbx	; save ycos_array pointer

; store the result _m128d
	movlpd	QWORD PTR [rdi-16],xmm5
	movhpd	QWORD PTR [rdi-8],xmm5

	movlpd	QWORD PTR [rbx-16],xmm13
	movhpd	QWORD PTR [rbx-8],xmm13

	mov	rax,QWORD PTR [rsp+p_iter]	; get number of iterations
	sub	rax,1
	mov	QWORD PTR [rsp+p_iter],rax	; save number of iterations
	jnz	__vrda_top

; see if we need to do any extras
	mov	rax,QWORD PTR [rsp+save_nv]	; get number of values
	test	rax,rax
	jnz	__vrda_cleanup

__final_check:

	movdqa	xmm6, OWORD PTR [rsp+save_xmm6]	; restore xmm6
	movdqa	xmm7, OWORD PTR [rsp+save_xmm7]	; restore xmm7
	movdqa	xmm8, OWORD PTR [rsp+save_xmm8]	; restore xmm8
	movdqa	xmm9, OWORD PTR [rsp+save_xmm9]	; restore xmm9
	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]	; restore xmm10
	movdqa	xmm11,OWORD PTR [rsp+save_xmm11]	; restore xmm11
	movdqa	xmm12,OWORD PTR [rsp+save_xmm12]	; restore xmm12
	movdqa	xmm13,OWORD PTR [rsp+save_xmm13]	; restore xmm13
	movdqa	xmm14,OWORD PTR [rsp+save_xmm14]	; restore xmm12
	movdqa	xmm15,OWORD PTR [rsp+save_xmm15]	; restore xmm13

	mov	r12,QWORD PTR [rsp+save_r12]	; restore r12
	mov	r13,QWORD PTR [rsp+save_r13]	; restore r13
	mov	rsi,QWORD PTR [rsp+save_rsi]	; restore rsi
	mov	rdi,QWORD PTR [rsp+save_rdi]	; restore rdi
	mov	rbx,QWORD PTR [rsp+save_rbx]	; restore rbx

	add	rsp,stack_size
	ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; we jump here when we have an odd number of cos calls to make at the end
; we assume that rdx is pointing at the next x array element, r8 at the next y array element.
; The number of values left is in save_nv

align	16
__vrda_cleanup:

;DEBUG
;	mov	rsi,QWORD PTR [rsp+save_xa]
;	mov	rdi,QWORD PTR [rsp+save_ysa]	; get ysin_array pointer
;	mov	rbx,QWORD PTR [rsp+save_yca]	; get ycos_array pointer


;	movapd 	xmm0, OWORD PTR [rsi]
;	movapd 	xmm1, OWORD PTR [rsi+16]

;	movlpd	QWORD PTR [rdi],xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx],xmm0
;	movhpd	QWORD PTR 8[rbx],xmm0

;	movlpd	QWORD PTR 16[rdi],xmm1
;	movhpd	QWORD PTR 24[rdi],xmm1
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1

;	jmp	__final_check
;DEBUG
	mov		rax,save_nv[rsp]	; get number of values
	test	rax,rax
	jz		__final_check

	mov		rsi,QWORD PTR [rsp+save_xa]

; fill in a m128d with zeroes and the extra values and then make a recursive call.
	xorpd		xmm0,xmm0
	movlpd		QWORD PTR p_temp[rsp+8],xmm0
	movapd		XMMWORD PTR p_temp[rsp+16],xmm0

	mov		rcx,[rsi]			; we know there's at least one
	mov	 	QWORD PTR p_temp[rsp],rcx
	cmp		rax,2
	jl		__vrdacg

	mov		rcx,8[rsi]			; do the second value
	mov	 	QWORD PTR p_temp[rsp+8],rcx
	cmp		rax,3
	jl		__vrdacg

	mov		rcx,16[rsi]			; do the third value
	mov	 	QWORD PTR p_temp[rsp+16],rcx

__vrdacg:
	mov		rcx,4				; parameter for N
	lea		rdx,p_temp[rsp]	; &x parameter
	lea		r8, p_temp2[rsp]; &ys parameter
	lea		r9, p_temp4[rsp]; &yc parameter


;DEBUG
;	mov		rsi,QWORD PTR [rsp+save_xa]
;	mov		rdi,QWORD PTR [rsp+save_ysa]
;	mov		rbx,QWORD PTR [rsp+save_yca]


;	movapd 	xmm0, OWORD PTR p_temp[rsp]
;	movapd 	xmm1, OWORD PTR p_temp[rsp+16]

;	movlpd	QWORD PTR [rdi],xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx],xmm0
;	movhpd	QWORD PTR 8[rbx],xmm0

;	movlpd	QWORD PTR 16[rdi],xmm1
;	movhpd	QWORD PTR 24[rdi],xmm1
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1

;	jmp	__final_check
;DEBUG





	call		fname			; call recursively to compute four values

;DEBUG
;	mov		rdi,QWORD PTR [rsp+save_ysa]
;	mov		rbx,QWORD PTR [rsp+save_yca]


;	movapd 	xmm0, OWORD PTR p_temp1[rsp]
;	movapd 	xmm1, OWORD PTR p_temp2[rsp]

;	movlpd	QWORD PTR [rdi],xmm0
;	movhpd	QWORD PTR 8[rdi],xmm0
;	movlpd	QWORD PTR [rbx],xmm1
;	movhpd	QWORD PTR 8[rbx],xmm1

;	movapd 	xmm0, OWORD PTR p_temp1[rsp+16]
;	movapd 	xmm1, OWORD PTR p_temp2[rsp+16]

;	movlpd	QWORD PTR 16[rdi],xmm1
;	movhpd	QWORD PTR 24[rdi],xmm1
;	movlpd	QWORD PTR 16[rbx],xmm1
;	movhpd	QWORD PTR 24[rbx],xmm1

;	jmp	__final_check
;DEBUG

; now copy the results to the destination array
	mov		rdi,QWORD PTR [rsp+save_ysa]
	mov		rbx,QWORD PTR [rsp+save_yca]
	mov		rax,save_nv[rsp]	; get number of values

	mov	 	rcx,p_temp2[rsp]
	mov		[rdi],rcx			; we know there's at least one
	mov	 	rdx,p_temp4[rsp]
	mov		[rbx],rdx			; we know there's at least one
	cmp		rax,2
	jl		__vrdacgf

	mov	 	rcx,p_temp2[rsp+8]
	mov		8[rdi],rcx			; do the second value
	mov	 	rdx,p_temp4[rsp+8]
	mov		8[rbx],rdx			; do the second value
	cmp		rax,3
	jl		__vrdacgf

	mov	 	rcx,p_temp2[rsp+16]
	mov		16[rdi],rcx			; do the third value
	mov	 	rdx,p_temp4[rsp+16]
	mov		16[rbx],rdx			; do the third value

__vrdacgf:
	jmp		__final_check

fname		endp
;TEXT	ENDS

END
