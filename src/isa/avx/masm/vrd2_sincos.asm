;
; A vector implementation of the libm sincos function.
;
; Prototype:
;
;      vrd2_sincos(__m128d x, __m128d* ys, __m128d* yc);
;
;   Computes Sine and Cosine of x.


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
__real_ffffffffffffffff DQ 0ffffffffffffffffh	;Sign bit one
			DQ 0ffffffffffffffffh
__real_naninf_upper_sign_mask	DQ 000000000ffffffffh	;
				DQ 000000000ffffffffh	;
__real_naninf_lower_sign_mask	DQ 0ffffffff00000000h	;
				DQ 0ffffffff00000000h	;

cosarray:
	DQ	03fa5555555555555h		;  0.0416667		   	c1
	DQ	03fa5555555555555h
	DQ	0bf56c16c16c16967h		; -0.00138889	   		c2
	DQ	0bf56c16c16c16967h
	DQ	03efa01a019f4ec90h		;  2.48016e-005			c3
	DQ	03efa01a019f4ec90h
	DQ	0be927e4fa17f65f6h		; -2.75573e-007			c4
	DQ	0be927e4fa17f65f6h
	DQ	03e21eeb69037ab78h		;  2.08761e-009			c5
	DQ	03e21eeb69037ab78h
	DQ	0bda907db46cc5e42h		; -1.13826e-011	   		c6
	DQ	0bda907db46cc5e42h
sinarray:
	DQ	0bfc5555555555555h		; -0.166667	   		s1
	DQ	0bfc5555555555555h
	DQ	03f81111111110bb3h		;  0.00833333	   		s2
	DQ	03f81111111110bb3h
	DQ	0bf2a01a019e83e5ch		; -0.000198413			s3
	DQ	0bf2a01a019e83e5ch
	DQ	03ec71de3796cde01h		;  2.75573e-006			s4
	DQ	03ec71de3796cde01h
	DQ	0be5ae600b42fdfa7h		; -2.50511e-008			s5
	DQ	0be5ae600b42fdfa7h
	DQ	03de5e0b2f9a43bb8h		;  1.59181e-010	   		s6
	DQ	03de5e0b2f9a43bb8h
sincosarray:
	DQ	0bfc5555555555555h		; -0.166667	   		s1
	DQ	03fa5555555555555h		;  0.0416667		   	c1
	DQ	03f81111111110bb3h		;  0.00833333	   		s2
	DQ	0bf56c16c16c16967h		;				c2
	DQ	0bf2a01a019e83e5ch		; -0.000198413			s3
	DQ	03efa01a019f4ec90h
	DQ	03ec71de3796cde01h		;  2.75573e-006			s4
	DQ	0be927e4fa17f65f6h
	DQ	0be5ae600b42fdfa7h		; -2.50511e-008			s5
	DQ	03e21eeb69037ab78h
	DQ	03de5e0b2f9a43bb8h		;  1.59181e-010	   		s6
	DQ	0bda907db46cc5e42h
cossinarray:
	DQ	03fa5555555555555h		;  0.0416667		   	c1
	DQ	0bfc5555555555555h		; -0.166667	   		s1
	DQ	0bf56c16c16c16967h		;				c2
	DQ	03f81111111110bb3h		;  0.00833333	   		s2
	DQ	03efa01a019f4ec90h
	DQ	0bf2a01a019e83e5ch		; -0.000198413			s3
	DQ	0be927e4fa17f65f6h
	DQ	03ec71de3796cde01h		;  2.75573e-006			s4
	DQ	03e21eeb69037ab78h
	DQ	0be5ae600b42fdfa7h		; -2.50511e-008			s5
	DQ	0bda907db46cc5e42h
	DQ	03de5e0b2f9a43bb8h		;  1.59181e-010	   		s6
CONST	ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
p_temp		equ		00h		; temporary for get/put bits operation
p_temp1		equ		10h		; temporary for get/put bits operation

save_xmm6	equ		20h		; temporary for get/put bits operation
save_xmm7	equ		30h		; temporary for get/put bits operation
save_xmm8	equ		40h		; temporary for get/put bits operation
save_xmm9	equ		50h		; temporary for get/put bits operation
save_xmm10	equ		60h		; temporary for get/put bits operation
save_xmm11	equ		70h		; temporary for get/put bits operation
save_xmm12	equ		80h		; temporary for get/put bits operation
save_xmm13	equ		90h		; temporary for get/put bits operation
save_xmm14	equ		0A0h		; temporary for get/put bits operation
save_xmm15	equ		0B0h		; temporary for get/put bits operation

save_rdi	equ		0C0h
save_rsi	equ		0D0h

r		equ		0E0h		; pointer to r for remainder_piby2
rr		equ		0F0h		; pointer to r for remainder_piby2
region		equ		100h		; pointer to r for remainder_piby2

p_original	equ		110h		; original x
p_mask		equ		120h		; original x
p_sign		equ		130h		; original x
p_sign1		equ		140h		; original x
p_x		equ		150h		;x
p_xx		equ		160h		;xx
p_x2		equ		170h		;x2
p_sin		equ		180h		;sin
p_cos		equ		190h		;cos

p_temp2		equ		1A0h		; temporary for get/put bits operation

stack_size	equ		1B8h

include fm.inc

FN_PROTOTYPE vrd2_sincos


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EXTRN __amd_remainder_piby2:NEAR

TEXT	SEGMENT	   page	   'CODE'
PUBLIC	fname
fname	proc	frame

	;STARTPROLOG
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

	.ENDPROLOG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;STARTMAIN

movdqa 	xmm6, xmm0				;move to mem to get into integer regs **
movdqa  OWORD PTR p_original[rsp], xmm0		;move to mem to get into integer regs -

andpd 	xmm0, OWORD PTR __real_7fffffffffffffff	;Unsign			-

mov	QWORD PTR p_sin[rsp],rdx		; save address for sin return
mov	QWORD PTR p_cos[rsp],r8			; save address for cos return

movd	rax, xmm0				;rax is lower arg
movhpd	QWORD PTR p_temp[rsp+8], xmm0		;
mov    	rcx, QWORD PTR p_temp[rsp+8]		;rcx = upper arg
movdqa	xmm8,xmm0

pcmpgtd		xmm8, xmm6
movdqa		xmm6, xmm8
psrldq		xmm8, 4
psrldq		xmm6, 8

mov 	rdx, 3FE921FB54442D18h			;piby4
mov	r10, 411E848000000000h			;5e5
movapd	xmm4, OWORD PTR __real_3fe0000000000000	;0.5 for later use

por	xmm8, xmm6
movd	r11, xmm8				;Move Sign to gpr **

movapd	xmm2, xmm0				;x
movapd	xmm6, xmm0				;x

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
either_or_both_arg_gt_than_piby4:

	cmp	rax, r10				;is lower arg >= 5e5
	jae	lower_or_both_arg_gt_5e5
	cmp	rcx, r10				;is upper arg >= 5e5
	jae	upper_arg_gt_5e5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
both_arg_lt_than_5e5:
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5

;DEBUG
;	movapd	xmm4, xmm2
;	movapd	xmm5, xmm2
;	jmp __vrd2_sincos_cleanup
;DEBUG

	mulpd	xmm2, XMMWORD PTR __real_3fe45f306dc9c883		; * twobypi
	addpd	xmm2,xmm4						; +0.5, npi2
	movapd	xmm0,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtdq2pd	xmm2,xmm4					; and back to double.


;      /* Subtract the multiple from x to get an extra-precision remainder */

	movd	r8, xmm4						; Region

	mov 	rdx,  QWORD PTR __reald_one_zero			;compare value for cossin path
	mov	r10,  r8
	mov	rcx,  r8

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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = npi2 (int), xmm0 =rhead, xmm8 =rtail

	and	r8, 	QWORD PTR __reald_one_one		;odd/even region for cos/sin

	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
	mov	rax, r10
	not 	r11						;ADDED TO CHANGE THE LOGIC
	and	r10, r11
	not	rax
	not	r11
	and	rax, r11
	or	r10, rax
	and	r10, __reald_one_one				;(~AB+A~B)&1
	mov	r11,	r10
	and	r11,	rdx				;mask out the lower sign bit leaving the upper sign bit
	shl	r10,	63				;shift lower sign bit left by 63 bits
	shl	r11,	31				;shift upper sign bit left by 31 bits
	mov 	QWORD PTR p_sign[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_sign[rsp+8],r11		;write out upper sign bit

; xmm4  = Sign, xmm0 =rhead, xmm8 =rtail

	movapd	xmm6,xmm0						; rhead
	subpd	xmm0,xmm8						; r = rhead - rtail

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = Sign, xmm0 = r, xmm6 =rhead, xmm8 =rtail

	subpd	xmm6, xmm0				;rr=rhead-r
	movapd	xmm2,xmm0				;move r for r2
	mulpd	xmm2,xmm0				;r2
	subpd	xmm6, xmm8				;rr=(rhead-r) -rtail

	mov 	r9, QWORD PTR __reald_one_zero		; Compare value for cossin	+


	add	rcx,QWORD PTR __reald_one_one
	and	rcx,QWORD PTR __reald_two_two
	shr	rcx,1

	mov	rdx,	rcx
	and	rdx,	r9				;mask out the lower sign bit leaving the upper sign bit
	shl	rcx,	63				;shift lower sign bit left by 63 bits
	shl	rdx,	31				;shift upper sign bit left by 31 bits
	mov 	QWORD PTR p_sign1[rsp],rcx		;write out lower sign bit
	mov 	QWORD PTR p_sign1[rsp+8],rdx		;write out upper sign bit

;DEBUG
;	movapd	xmm4, xmm2
;	movapd	xmm5, xmm2
;	jmp __vrd2_sincos_cleanup
;DEBUG
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

align 16
__vrd2_sincos_approximate:
	cmp 	r8, 0
	jnz	vrd2_not_sin_piby4

vrd2_sin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr

	movdqa	xmm4, OWORD PTR cosarray+50h			; c6
	movdqa	xmm5, OWORD PTR sinarray+50h			; c6
	movapd	xmm8, OWORD PTR cosarray+20h			; c3
	movapd	xmm9, OWORD PTR sinarray+20h			; c3

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm2					; x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm2					; c6*x2
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm2					; c3*x2

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	movapd	OWORD PTR p_temp[rsp], xmm2			; store x2

	addpd	xmm4,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm5,  OWORD PTR sinarray+40h			; c5+x2c6
	movapd	OWORD PTR p_temp2[rsp], xmm10			; store r
	addpd	xmm8,  OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm9,  OWORD PTR sinarray+10h			; c2+x2C3

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm11, xmm2					; x4

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm2					; x2(c5+x2c6)
	movapd	OWORD PTR p_temp1[rsp], xmm10 			; store t
	movapd	xmm3, xmm11					; Keep x4
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm2					; x2(c2+x2C3)

	addpd   xmm10, OWORD PTR __real_3ff0000000000000	; 1 + (-t)
	mulpd	xmm11, xmm2					; x6

	addpd	xmm4, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	subpd   xmm10, OWORD PTR p_temp2[rsp]			; (1 + (-t)) - r
	mulpd	xmm2,  xmm0					; x3 recalculate

	mulpd	xmm4, xmm11					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm11					; x6(c4 + x2(c5+x2c6))

	movapd	xmm1, xmm0
	movapd	xmm7, xmm6
	mulpd	xmm1, xmm6					; x*xx
	mulpd	xmm7, OWORD PTR p_temp2[rsp]			; xx * 0.5x2

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zs

	subpd   xmm10, xmm1					; ((1 + (-t)) - r) -x*xx

	mulpd	xmm4, xmm3					; x4 * zc
	mulpd	xmm5, xmm2					; x3 * zs

	addpd	xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	xmm5, xmm7					; x3*zs - 0.5 * x2 *xx

	addpd	xmm5, xmm6					; sin + xx
	subpd	xmm4, OWORD PTR p_temp1[rsp]			; cos - (-t)
	addpd	xmm5, xmm0					; sin + x

	jmp 	__vrd2_sincos_cleanup

align 16
vrd2_not_sin_piby4:
	cmp 	r8, QWORD PTR __reald_one_one
	jnz	vrd2_not_cos_piby4

vrd2_cos_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr

	movdqa	xmm5, OWORD PTR cosarray+50h			; c6
	movdqa	xmm4, OWORD PTR sinarray+50h			; c6
	movapd	xmm9, OWORD PTR cosarray+20h			; c3
	movapd	xmm8, OWORD PTR sinarray+20h			; c3

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm2					; x2

	mulpd	xmm5,  xmm2					; c6*x2
	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm9,  xmm2					; c3*x2
	mulpd	xmm8,  xmm2					; c3*x2

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	movapd	OWORD PTR p_temp[rsp], xmm2			; store x2

	addpd	xmm5,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm4,  OWORD PTR sinarray+40h			; c5+x2c6
	movapd	OWORD PTR p_temp2[rsp], xmm10			; store r
	addpd	xmm9,  OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm8,  OWORD PTR sinarray+10h			; c2+x2C3

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm11, xmm2					; x4

	mulpd	xmm5, xmm2					; x2(c5+x2c6)
	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	movapd	OWORD PTR p_temp1[rsp], xmm10 			; store t
	movapd	xmm3, xmm11					; Keep x4
	mulpd	xmm9, xmm2					; x2(c2+x2C3)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)

	addpd   xmm10, OWORD PTR __real_3ff0000000000000	; 1 + (-t)
	mulpd	xmm11, xmm2					; x6

	addpd	xmm5, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm4, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm9, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm8, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	subpd   xmm10, OWORD PTR p_temp2[rsp]			; (1 + (-t)) - r
	mulpd	xmm2,  xmm0					; x3 recalculate

	mulpd	xmm5, xmm11					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm4, xmm11					; x6(c4 + x2(c5+x2c6))

	movapd	xmm1, xmm0
	movapd	xmm7, xmm6
	mulpd	xmm1, xmm6					; x*xx
	mulpd	xmm7, OWORD PTR p_temp2[rsp]			; xx * 0.5x2

	addpd	xmm5, xmm9					; zc
	addpd	xmm4, xmm8					; zs

	subpd   xmm10, xmm1					; ((1 + (-t)) - r) -x*xx

	mulpd	xmm5, xmm3					; x4 * zc
	mulpd	xmm4, xmm2					; x3 * zs

	addpd	xmm5, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	xmm4, xmm7					; x3*zs - 0.5 * x2 *xx

	addpd	xmm4, xmm6					; sin + xx
	subpd	xmm5, OWORD PTR p_temp1[rsp]			; cos - (-t)
	addpd	xmm4, xmm0					; sin + x

	jmp 	__vrd2_sincos_cleanup

align 16
vrd2_not_cos_piby4:
	cmp 	r8, 1
	jnz	vrd2_cossin_piby4

vrd2_sincos_piby4:
	movdqa	xmm4, OWORD PTR cosarray+50h			; c6
	movdqa	xmm5, OWORD PTR sinarray+50h			; c6
	movapd	xmm8, OWORD PTR cosarray+20h			; c3
	movapd	xmm9, OWORD PTR sinarray+20h			; c3

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm2					; x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm2					; c6*x2
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm2					; c3*x2

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	movapd	OWORD PTR p_temp[rsp], xmm2			; store x2

	addpd	xmm4,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm5,  OWORD PTR sinarray+40h			; c5+x2c6
	movapd	OWORD PTR p_temp2[rsp], xmm10			; store r
	addpd	xmm8,  OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm9,  OWORD PTR sinarray+10h			; c2+x2C3

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm11, xmm2					; x4

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm2					; x2(c5+x2c6)
	movapd	OWORD PTR p_temp1[rsp], xmm10 			; store t
	movapd	xmm3, xmm11					; Keep x4
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm2					; x2(c2+x2C3)

	addpd   xmm10, OWORD PTR __real_3ff0000000000000	; 1 + (-t)
	mulpd	xmm11, xmm2					; x6

	addpd	xmm4, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	subpd   xmm10, OWORD PTR p_temp2[rsp]			; (1 + (-t)) - r
	mulpd	xmm2,  xmm0					; x3 recalculate

	mulpd	xmm4, xmm11					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm11					; x6(c4 + x2(c5+x2c6))

	movapd	xmm1, xmm0
	movapd	xmm7, xmm6
	mulpd	xmm1, xmm6					; x*xx
	mulpd	xmm7, OWORD PTR p_temp2[rsp]			; xx * 0.5x2

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zs

	subpd   xmm10, xmm1					; ((1 + (-t)) - r) -x*xx

	mulpd	xmm4, xmm3					; x4 * zc
	mulpd	xmm5, xmm2					; x3 * zs

	addpd	xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	xmm5, xmm7					; x3*zs - 0.5 * x2 *xx

	addpd	xmm5, xmm6					; sin + xx
	subpd	xmm4, OWORD PTR p_temp1[rsp]			; cos - (-t)
	addpd	xmm5, xmm0					; sin + x

	movsd	xmm1, xmm4
	movsd	xmm4, xmm5
	movsd	xmm5, xmm1

	jmp 	__vrd2_sincos_cleanup

align 16
vrd2_cossin_piby4:
	movdqa	xmm5, OWORD PTR cosarray+50h			; c6
	movdqa	xmm4, OWORD PTR sinarray+50h			; c6
	movapd	xmm9, OWORD PTR cosarray+20h			; c3
	movapd	xmm8, OWORD PTR sinarray+20h			; c3

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm2					; x2

	mulpd	xmm5,  xmm2					; c6*x2
	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm9,  xmm2					; c3*x2
	mulpd	xmm8,  xmm2					; c3*x2

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	movapd	OWORD PTR p_temp[rsp], xmm2			; store x2

	addpd	xmm5,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm4,  OWORD PTR sinarray+40h			; c5+x2c6
	movapd	OWORD PTR p_temp2[rsp], xmm10			; store r
	addpd	xmm9,  OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm8,  OWORD PTR sinarray+10h			; c2+x2C3

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm11, xmm2					; x4

	mulpd	xmm5, xmm2					; x2(c5+x2c6)
	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	movapd	OWORD PTR p_temp1[rsp], xmm10 			; store t
	movapd	xmm3, xmm11					; Keep x4
	mulpd	xmm9, xmm2					; x2(c2+x2C3)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)

	addpd   xmm10, OWORD PTR __real_3ff0000000000000	; 1 + (-t)
	mulpd	xmm11, xmm2					; x6

	addpd	xmm5, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm4, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm9, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm8, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	subpd   xmm10, OWORD PTR p_temp2[rsp]			; (1 + (-t)) - r
	mulpd	xmm2,  xmm0					; x3 recalculate

	mulpd	xmm5, xmm11					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm4, xmm11					; x6(c4 + x2(c5+x2c6))

	movapd	xmm1, xmm0
	movapd	xmm7, xmm6
	mulpd	xmm1, xmm6					; x*xx
	mulpd	xmm7, OWORD PTR p_temp2[rsp]			; xx * 0.5x2

	addpd	xmm5, xmm9					; zc
	addpd	xmm4, xmm8					; zs

	subpd   xmm10, xmm1					; ((1 + (-t)) - r) -x*xx

	mulpd	xmm5, xmm3					; x4 * zc
	mulpd	xmm4, xmm2					; x3 * zs

	addpd	xmm5, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	xmm4, xmm7					; x3*zs - 0.5 * x2 *xx

	addpd	xmm4, xmm6					; sin + xx
	subpd	xmm5, OWORD PTR p_temp1[rsp]			; cos - (-t)
	addpd	xmm4, xmm0					; sin + x

	movsd	xmm1, xmm5
	movsd	xmm5, xmm4
	movsd	xmm4, xmm1

	jmp 	__vrd2_sincos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
lower_or_both_arg_gt_5e5:
	cmp	rcx, r10				;is upper arg >= 5e5
	jae	both_arg_gt_5e5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
lower_arg_gt_5e5:
; Upper Arg is < 5e5, Lower arg is >= 5e5

	movlpd	QWORD PTR r[rsp], xmm0		;Save lower fp arg for remainder_piby2 call

	movhlps	xmm0, xmm0			;Needed since we want to work on upper arg
	movhlps	xmm2, xmm2
	movhlps	xmm6, xmm6

; Work on Upper arg
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5
; Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs

;If upper Arg is <=piby4
       cmp	rcx, rdx					; is upper arg > piby4
       ja	@F

       mov 	ecx, 0						; region = 0
       mov	DWORD PTR region[rsp+4], ecx			; store upper region
       movlpd	QWORD PTR r[rsp+8], xmm0			; store upper r (unsigned - sign is adjusted later based on sign)
       xorpd	xmm4, xmm4					; rr = 0
       movlpd	QWORD PTR rr[rsp+8], xmm4			; store upper rr
       jmp	check_lower_arg

;If upper Arg is > piby4
align 16
@@:
	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm4					; npi2=(x*twobypi+0.5)
	movsd	xmm3,QWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttsd2si	ecx,xmm2				; npi2 trunc to ints
	movsd	xmm1,QWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtsi2sd	xmm2,ecx				; npi2 trunc to doubles

	;/* Subtract the multiple from x to get an extra-precision remainder */
	;rhead  = x - npi2 * piby2_1;
	mulsd	xmm3,xmm2					; npi2 * piby2_1
	subsd	xmm6,xmm3					; rhead =(x-npi2*piby2_1)
	movsd	xmm8,QWORD PTR __real_3ba3198a2e037073		; piby2_2tail

	;t  = rhead;
       movsd	xmm5, xmm6					; t = rhead

	;rtail  = npi2 * piby2_2;
       mulsd	xmm1,xmm2					; rtail=(npi2*piby2_2)

	;rhead  = t - rtail
       subsd	xmm6, xmm1					; rhead=(t-rtail)

	;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm8,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm6					; t-rhead
       subsd	xmm1,xmm5					; (rtail-(t-rhead))
       addsd	xmm1,xmm8					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

	;r =  rhead - rtail
	;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp+4], ecx			; store upper region
       movsd	xmm0,xmm6
       subsd	xmm0,xmm1					; r=(rhead-rtail)

       subsd	xmm6,xmm0					; rr=rhead-r
       subsd	xmm6,xmm1					; xmm4 = rr=((rhead-r) -rtail)

       movlpd	QWORD PTR r[rsp+8], xmm0			; store upper r
       movlpd	QWORD PTR rr[rsp+8], xmm6			; store upper rr

;If lower Arg is > 5e5
;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
align 16
check_lower_arg:
	mov		r9,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r9
	and		r10,rax
	cmp		r10,r9
	jz		__vrd2_cos_lower_naninf

	lea	 r9, 	QWORD PTR region[rsp]			; lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 rdx,	QWORD PTR r[rsp]
	movlpd	 xmm0,  QWORD PTR r[rsp]	;Restore lower fp arg for remainder_piby2 call
	mov	QWORD PTR p_temp[rsp], r11	;Save Sign

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	r11, QWORD PTR p_temp[rsp]	;Restore Sign

	jmp 	__vrd2_cos_reconstruct

__vrd2_cos_lower_naninf:
	mov	rax, QWORD PTR p_original[rsp]			; upper arg is nan/inf
;	mov	rax, QWORD PTR r[rsp]
	mov	r9,00008000000000000h
	or	rax,r9
	mov	QWORD PTR r[rsp],rax				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				; rr = 0
	mov	DWORD PTR region[rsp], r10d			; region =0
;	mov 	r11, 0						; Sign
	and 	r11, QWORD PTR __real_naninf_lower_sign_mask	; Sign
	jmp 	__vrd2_cos_reconstruct

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
upper_arg_gt_5e5:
; Upper Arg is >= 5e5, Lower arg is < 5e5
	movhpd	QWORD PTR r[rsp+8], xmm0		;Save upper fp arg for remainder_piby2 call
;	movlhps	xmm0, xmm0				;Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
;	movlhps	xmm2, xmm2
;	movlhps	xmm6, xmm6

; Work on Lower arg
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5
; Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

;If lower Arg is <=piby4
       cmp	rax, rdx					; is upper arg > piby4
       ja	@F

       mov 	eax, 0						; region = 0
       mov	DWORD PTR region[rsp], eax			; store upper region
       movlpd	QWORD PTR r[rsp], xmm0				; store upper r
       xorpd	xmm4, xmm4					; rr = 0
       movlpd	QWORD PTR rr[rsp], xmm4				; store upper rr
       jmp check_upper_arg

align 16
@@:
;If upper Arg is > piby4
	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm4					; npi2=(x*twobypi+0.5)
	movsd	xmm3,QWORD PTR __real_3ff921fb54400000		; piby2_1
	cvttsd2si	eax,xmm2				; npi2 trunc to ints
	movsd	xmm1,QWORD PTR __real_3dd0b4611a600000		; piby2_2
	cvtsi2sd	xmm2,eax				; npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm3,xmm2					; npi2 * piby2_1;
	subsd	xmm6,xmm3					; rhead =(x-npi2*piby2_1)
	movsd	xmm8,QWORD PTR __real_3ba3198a2e037073		; piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm6					; t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm1,xmm2					; rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm6, xmm1					; rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm8,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm6					; t-rhead
       subsd	xmm1,xmm5					; (rtail-(t-rhead))
       addsd	xmm1,xmm8					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp], eax			; store lower region
       movsd	xmm0,xmm6
       subsd	xmm0,xmm1					; r=(rhead-rtail)
       subsd	xmm6, xmm0					; rr=rhead-r
       subsd	xmm6, xmm1					; rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r[rsp], xmm0				; store lower r
       movlpd	QWORD PTR rr[rsp], xmm6				; store lower rr

;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
align 16
check_upper_arg:
	mov		r9,07ff0000000000000h			; is upper arg nan/inf
	mov		r10,r9
	and		r10,rcx
	cmp		r10,r9
	jz		__vrd2_cos_upper_naninf

	lea	 r9, 	QWORD PTR region[rsp+4]			; upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 rdx,	QWORD PTR r[rsp+8]
	movlpd	 xmm0,  QWORD PTR r[rsp+8]	;Restore upper fp arg for remainder_piby2 call
	mov	QWORD PTR p_temp[rsp], r11	;Save Sign
	
;change to MS ABI - shadow space
sub	rsp,020h	

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	r11, QWORD PTR p_temp[rsp]	;Restore Sign

	jmp __vrd2_cos_reconstruct

__vrd2_cos_upper_naninf:
	mov	rcx, QWORD PTR p_original[rsp+8]		; upper arg is nan/inf
;	mov	rcx, QWORD PTR r[rsp+8]				; upper arg is nan/inf
	mov	r9,00008000000000000h
	or	rcx,r9
	mov	QWORD PTR r[rsp+8],rcx				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			; rr = 0
	mov	DWORD PTR region[rsp+4], r10d			; region =0
;	mov 	r11, 0						; Sign
	and 	r11, QWORD PTR __real_naninf_upper_sign_mask	; Sign
	jmp __vrd2_cos_reconstruct


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
both_arg_gt_5e5:
;Upper Arg is >= 5e5, Lower arg is >= 5e5

;DEBUG
;	xorpd	xmm4, xmm4
;	xorpd	xmm5, xmm5
;	movapd	OWORD PTR p_sign[rsp], xmm4
;	movapd	OWORD PTR p_sign1[rsp], xmm5
;	jmp 	__vrd2_sincos_cleanup
;DEBUG

	movhlps xmm6, xmm0					;Save upper fp arg for remainder_piby2 call
	mov		r9,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r9
	and		r10,rax
	cmp		r10,r9
	jz		__vrd2_cos_lower_naninf_of_both_gt_5e5

	lea	 r9, 	QWORD PTR region[rsp]			;lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 rdx,	QWORD PTR r[rsp]
	mov	 QWORD PTR p_temp[rsp], rcx			;Save upper arg
	mov	 QWORD PTR p_temp1[rsp], r11	;Save Sign

;change to MS ABI - shadow space
sub	rsp,020h	

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	 r11, QWORD PTR p_temp1[rsp]	;Restore Sign
	mov	 rcx, QWORD PTR p_temp[rsp]			;Restore upper arg
	jmp 	@F

__vrd2_cos_lower_naninf_of_both_gt_5e5:				;lower arg is nan/inf
	mov	rax, QWORD PTR p_original[rsp]
;	movd	rax, xmm0
	mov	r9,00008000000000000h
	or	rax,r9
	mov	QWORD PTR r[rsp],rax				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				;rr = 0
	mov	DWORD PTR region[rsp], r10d			;region = 0
;	mov 	r11, 0						; Sign
	and 	r11, QWORD PTR __real_naninf_lower_sign_mask	; Sign

align 16
@@:
	mov		r9,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r9
	and		r10,rcx
	cmp		r10,r9
	jz		__vrd2_cos_upper_naninf_of_both_gt_5e5

	lea	 r9, 	QWORD PTR region[rsp+4]			;upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 rdx,	QWORD PTR r[rsp+8]
	movapd	 xmm0,  xmm6			;Restore upper fp arg for remainder_piby2 call
	mov	QWORD PTR p_temp[rsp], r11	;Save Sign

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	mov	r11, QWORD PTR p_temp[rsp]	;Restore Sign

	jmp 	@F

__vrd2_cos_upper_naninf_of_both_gt_5e5:
	mov	rcx, QWORD PTR p_original[rsp+8]		;upper arg is nan/inf
;	movd	rcx, xmm6					;upper arg is nan/inf
	mov	r9,00008000000000000h
	or	rcx,r9
	mov	QWORD PTR r[rsp+8],rcx				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			;rr = 0
	mov	DWORD PTR region[rsp+4], r10d			;region = 0
;	mov 	r11, 0						; Sign
	and 	r11, QWORD PTR __real_naninf_upper_sign_mask	; Sign

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
@@:
__vrd2_cos_reconstruct:
;Construct p_sign=Sign for Sin term, p_sign1=Sign for Cos term, xmm0 = r, xmm2 = r2, xmm6 =rr, r8=region
	movapd	xmm0, OWORD PTR r[rsp]				;x
	movapd	xmm2, xmm0					;move for x2
	mulpd	xmm2, xmm2					;x2
	movapd	xmm6, OWORD PTR rr[rsp]				;xx

	mov	r8,	QWORD PTR region[rsp]
	mov 	r9, 	QWORD PTR __reald_one_zero		;compare value for cossin path
	mov 	r10,	r8
	mov	rax,	r8
	and	r8, 	QWORD PTR __reald_one_one		;odd/even region for cos/sin

	shr	r10, 1						;~AB+A~B, A is sign and B is upper bit of region
	mov	rcx, r10
	not 	r11						;ADDED TO CHANGE THE LOGIC
	and	r10, r11
	not	rcx
	not	r11
	and	rcx, r11
	or	r10, rcx
	and	r10, __reald_one_one				;(~AB+A~B)&1

	mov	r11,	r10
	and	r11,	r9				;mask out the lower sign bit leaving the upper sign bit
	shl	r10,	63				;shift lower sign bit left by 63 bits
	shl	r11,	31				;shift upper sign bit left by 31 bits
	mov 	QWORD PTR p_sign[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_sign[rsp+8],r11		;write out upper sign bit
;	movapd	xmm6, OWORD PTR p_sign0[rsp]		;write out both sign bits to xmm6

	add	rax,QWORD PTR __reald_one_one
	and	rax,QWORD PTR __reald_two_two
	shr	rax,1

	mov	rdx,	rax
	and	rdx,	r9				;mask out the lower sign bit leaving the upper sign bit
	shl	rax,	63				;shift lower sign bit left by 63 bits
	shl	rdx,	31				;shift upper sign bit left by 31 bits
	mov 	QWORD PTR p_sign1[rsp],rax		;write out lower sign bit
	mov 	QWORD PTR p_sign1[rsp+8],rdx		;write out upper sign bit
;	movapd	xmm6, OWORD PTR p_sign1[rsp]		;write out both sign bits to xmm6


	jmp __vrd2_sincos_approximate


;ENDMAIN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
__vrd2_sincos_cleanup:

	xorpd	xmm5, OWORD PTR p_sign[rsp]		; SIN sign
	xorpd	xmm4, OWORD PTR p_sign1[rsp]		; COS sign

;DEBUG
;	movapd	xmm5, OWORD PTR p_sign[rsp]		; SIN sign
;	movapd	xmm4, OWORD PTR p_sign1[rsp]		; COS sign
;DEBUG

	mov	rdx,QWORD PTR p_sin[rsp]
	mov	r8, QWORD PTR p_cos[rsp]

	movapd	 OWORD PTR [rdx], xmm5			; save the sin
	movapd	 OWORD PTR [r8] , xmm4			; save the cos

	movdqa	xmm6, OWORD PTR [rsp+save_xmm6]		; restore xmm6
	movdqa	xmm7, OWORD PTR [rsp+save_xmm7]		; restore xmm7
	movdqa	xmm8, OWORD PTR [rsp+save_xmm8]		; restore xmm8
	movdqa	xmm9, OWORD PTR [rsp+save_xmm9]		; restore xmm9
	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]	; restore xmm10
	movdqa	xmm11,OWORD PTR [rsp+save_xmm11]	; restore xmm11

	add	rsp,stack_size
	ret

fname	endp
TEXT	ENDS
END
