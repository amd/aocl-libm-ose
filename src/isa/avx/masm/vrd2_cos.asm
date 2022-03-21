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

;
; A vector implementation of the libm cos function.
;
; Prototype:
;
;     __m128d vrd2_cos(__m128d x);
;
;   Computes Cosine of x
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the NAG C implementation.
;   Author: Harsha Jagasia
;   Email:  harsha.jagasia@amd.com

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
p_xmm6		equ		20h		; temporary for get/put bits operation
p_xmm7		equ		30h		; temporary for get/put bits operation
p_xmm8		equ		40h		; temporary for get/put bits operation
p_xmm9		equ		50h		; temporary for get/put bits operation
p_xmm10		equ		60h		; temporary for get/put bits operation
p_xmm11		equ		70h		; temporary for get/put bits operation
p_xmm12		equ		80h		; temporary for get/put bits operation
p_xmm13		equ		90h		; temporary for get/put bits operation
p_xmm14		equ		0A0h		; temporary for get/put bits operation
p_xmm15		equ		0B0h		; temporary for get/put bits operation

r		equ		0C0h		; pointer to r for remainder_piby2
rr		equ		0D0h		; pointer to r for remainder_piby2
region		equ		0E0h		; pointer to r for remainder_piby2

p_original	equ		0F0h		; original x
p_mask		equ		100h		; original x
p_sign		equ		110h		; original x

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
include fm.inc

ALM_PROTO_BAS64 vrd2_cos


EXTRN __amd_remainder_piby2:NEAR

TEXT	SEGMENT	   page	   'CODE'
PUBLIC	fname
fname	proc	frame

	;STARTPROLOG
	sub		rsp,128h
	.ALLOCSTACK	128h
	movdqa	OWORD PTR [rsp+020h],xmm6	; save xmm6
	.SAVEXMM128 xmm6, 020h
	.ENDPROLOG

    movupd xmm0, XMMWORD PTR [rcx]



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;STARTMAIN
;is this needed for cos?
movdqa	OWORD PTR p_original[rsp], xmm0
;is this needed for cos?

andpd 	xmm0, OWORD PTR __real_7fffffffffffffff
movdqa 	OWORD PTR p_temp[rsp], xmm0
mov 	rdx, 3FE921FB54442D18h			;piby4
mov	r10, 411E848000000000h			;5e5
movapd	xmm4, OWORD PTR __real_v2p__27		;for later use

;is this needed for cos?
;cmpltpd	xmm4, xmm0			;lt_v2p_27_mask
;andpd		xmm0, xmm4			;mask out the underflow causing arg with 0
;is this needed for cos?

movapd	xmm2, xmm0				;x
movapd	xmm4, xmm0				;x

mov    	rax, QWORD PTR p_temp[rsp]		;rax = lower arg
mov    	rcx, QWORD PTR p_temp[rsp+8]		;rcx = upper arg
movapd	xmm5, OWORD PTR __real_3fe0000000000000	;0.5 for later use

;cmp	rax, rdx				;is lower arg > piby4
;ja	either_or_both_arg_gt_than_piby4
;cmp	rcx, rdx				;is upper arg> piby4
;ja	either_or_both_arg_gt_than_piby4


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;both_arg_lte_than_piby4:
;__vrd2_cos_piby4_nored:
;	mulpd	xmm2,xmm0				; x2
;	movapd	xmm1, OWORD PTR cosarray+50h		; c6
;	movapd	xmm0, OWORD PTR cosarray+20h		; c3
;	mulpd	xmm5, xmm2				; r = 0.5 *x2
;	movapd	xmm3, xmm2				; move x2 for x4
;	mulpd	xmm1, xmm2				; c6*x2
;	mulpd	xmm0, xmm2				; c3*x2
;	subpd	xmm5, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
;	mulpd	xmm3, xmm2				; x4
;	addpd	xmm1, OWORD PTR cosarray+40h		; c5+x2c6
;	addpd	xmm0, OWORD PTR cosarray+10h		; c2+x2C3
;	addpd   xmm5, OWORD PTR __real_3ff0000000000000	; 1 + (-t)
;	mulpd	xmm3, xmm2				; x6
;	mulpd	xmm1, xmm2				; x2(c5+x2c6)
;	mulpd	xmm0, xmm2				; x2(c2+x2C3)
;	movapd 	xmm4, xmm2				; move x2 for r
;	mulpd 	xmm4, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
;	addpd	xmm1, OWORD PTR cosarray+30h		; c4+x2(c5+x2c6)
;	addpd	xmm0, OWORD PTR cosarray		; c1+x2(c2+x2C3)
;	mulpd	xmm2, xmm2				; x4
;	subpd   xmm5, xmm4				; (1 + (-t)) - r
;	mulpd	xmm1, xmm3				; x6(c4 + x2(c5+x2c6))
;	addpd	xmm0, xmm1				; zc
;	subpd	xmm4, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
;	mulpd	xmm0, xmm2				; x4 * zc
;	addpd   xmm0, xmm5				; x4 * zc + ((1 + (-t)) - r)
;	subpd   xmm0, xmm4				; {x4 * zc + ((1 + (-t)) - r)} - (-t)
;	jmp __vrd2_cos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
either_or_both_arg_gt_than_piby4:
	cmp	rax, r10				;is lower arg >= 5e5
	jae	lower_or_both_arg_gt_5e5
	cmp	rcx, r10				;is upper arg >= 5e5
	jae	upper_arg_gt_5e5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
both_arg_lt_than_5e5:
; xmm0, xmm2, xmm4 = x, xmm5 = 0.5

	mulpd	xmm2,OWORD PTR __real_3fe45f306dc9c883	; x*twobypi
	movapd	xmm3,OWORD PTR __real_3ff921fb54400000	; xmm3=piby2_1
	addpd	xmm2,xmm5				; xmm2 = npi2 = x*twobypi+0.5
	movapd	xmm1,OWORD PTR __real_3dd0b4611a600000	; xmm1=piby2_2
	movapd	xmm6,OWORD PTR __real_3ba3198a2e037073	; xmm6=piby2_2tail
	cvttpd2dq	xmm0,xmm2			; xmm0=convert npi2 to ints
	cvtdq2pd	xmm2,xmm0			; xmm2=and back to double.


;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulpd	xmm3,xmm2				; npi2 * piby2_1
	subpd	xmm4,xmm3				; xmm4 = rhead=x-npi2*piby2_1

;t  = rhead;
       movapd	xmm5, xmm4				; xmm5=t=rhead

;rtail  = npi2 * piby2_2;
       mulpd	xmm1,xmm2				; xmm1= npi2*piby2_2

;rhead  = t - rtail;
       subpd	xmm4, xmm1				; xmm4= rhead = t-rtail

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	xmm6,xmm2     				; npi2 * piby2_2tail
       subpd	xmm5,xmm4				; t-rhead
       subpd	xmm1,xmm5				; rtail-(t - rhead)
       addpd	xmm1,xmm6				; rtail=npi2*piby2_2+(rtail-(t-rhead))

;r = rhead - rtail
;rr=(rhead-r) -rtail
;Sign
;Region
	movdqa		xmm5,xmm0			; Sign
	movdqa		xmm6,xmm0			; Region
	movdqa		xmm0,xmm4			; rhead (handle xmm0 retype)

	paddd		xmm6,OWORD PTR __reald_one_one	; Sign
	pand		xmm6,OWORD PTR __reald_two_two
	punpckldq 	xmm6,xmm6
	psllq		xmm6,62				; xmm6 is in Int format

	subpd	xmm0,xmm1				; rhead - rtail
	pand 	xmm5, OWORD PTR __reald_one_one		; Odd/Even region for Sin/Cos
	mov 	r9, QWORD PTR __reald_one_zero		; Compare value for sincos
	subpd	xmm4, xmm0				; rr=rhead-r
	movd 	r8, xmm5				; Region
	movapd	xmm2,xmm0				; Move for x2
	movdqa	xmm6, xmm6				; handle xmm6 retype
	mulpd	xmm2,xmm0				; x2
	subpd	xmm4, xmm1				; rr=(rhead-r) -rtail


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;xmm0= x, xmm2 = x2, xmm4 = xx, r8 = region, r9 = compare value for sincos path, xmm6 = Sign

align 16
__vrd2_cos_approximate:
	cmp 	r8, 0
	jnz	vrd2_not_cos_piby4

vrd2_cos_piby4:
	mulpd	xmm4, xmm0				; x*xx
	movdqa	xmm5, OWORD PTR __real_3fe0000000000000	; 0.5 (handle xmm5 retype)
	movapd	xmm1, OWORD PTR cosarray+50h		; c6
	movapd	xmm0, OWORD PTR cosarray+20h		; c3
	mulpd	xmm5, xmm2				; r = 0.5 *x2
	movapd	xmm3, xmm2				; copy of x2 for x4
	movapd	OWORD PTR p_temp[rsp], xmm4		; store x*xx
	mulpd	xmm1,xmm2				; c6*x2
	mulpd	xmm0,xmm2				; c3*x2
	subpd	xmm5,OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm3,xmm2				; x4
	addpd	xmm1,OWORD PTR cosarray+40h		; c5+x2c6
	addpd	xmm0,OWORD PTR cosarray+10h		; c2+x2C3
	addpd   xmm5,OWORD PTR __real_3ff0000000000000	; 1 + (-t)
	mulpd	xmm3,xmm2				; x6
	mulpd	xmm1,xmm2				; x2(c5+x2c6)
	mulpd	xmm0,xmm2				; x2(c2+x2C3)
	movapd 	xmm4,xmm2				; copy of x2
	mulpd 	xmm4,OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	addpd	xmm1,OWORD PTR cosarray+30h		; c4 + x2(c5+x2c6)
	addpd	xmm0,OWORD PTR cosarray			; c1+x2(c2+x2C3)
	mulpd	xmm2,xmm2				; x4
	subpd   xmm5,xmm4				; (1 + (-t)) - r
	mulpd	xmm1,xmm3				; x6(c4 + x2(c5+x2c6))
	addpd	xmm0,xmm1				; zc
	subpd	xmm4,OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	subpd	xmm5, OWORD PTR p_temp[rsp]		; ((1 + (-t)) - r) - x*xx
	mulpd	xmm0, xmm2				; x4 * zc
	addpd   xmm0, xmm5				; x4 * zc + ((1 + (-t)) - r -x*xx)
	subpd   xmm0, xmm4				; result - (-t)
	xorpd	xmm0, xmm6				; xor with sign
	jmp 	__vrd2_cos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
vrd2_not_cos_piby4:
	cmp 	r8, 1
	jnz	vrd2_not_cos_sin_piby4

vrd2_cos_sin_piby4:

	movdqa	OWORD PTR p_temp1[rsp],xmm6		; Store Sign
	movapd	OWORD PTR p_temp[rsp], xmm4		; Store rr

	movapd	xmm3, OWORD PTR sincosarray+50h		; s6
	mulpd	xmm3, xmm2				; x2s6
	movdqa	xmm5, OWORD PTR sincosarray+20h		; s3 (handle xmm5 retype)
	movapd	xmm1, xmm2				; move x2 for x4
	mulpd	xmm1, xmm2				; x4
	mulpd	xmm5, xmm2				; x2s3
	addpd	xmm3, OWORD PTR sincosarray+40h		; s5+x2s6
	movapd	xmm4, xmm2				; move x2 for x6
	mulpd	xmm3, xmm2				; x2(s5+x2s6)
	mulpd	xmm4, xmm1				; x6
	addpd	xmm5, OWORD PTR sincosarray+10h		; s2+x2s3
	mulpd	xmm5, xmm2				; x2(s2+x2s3)
	addpd	xmm3, OWORD PTR sincosarray+30h		; s4+x2(s5+x2s6)

	movhlps	xmm1, xmm1				; move high x4 for cos
	mulpd	xmm3, xmm4				; x6(s4+x2(s5+x2s6))
	addpd	xmm5, OWORD PTR sincosarray		; s1+x2(s2+x2s3)
	movapd	xmm4, xmm2				; move low x2 for x3
	mulsd	xmm4, xmm0				; get low x3 for sin term
	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2

	addpd	xmm5, xmm3				; z
	movhlps	xmm6, xmm2				; move high r for cos
	movhlps	xmm3, xmm5				; xmm5 = sin
							; xmm3 = cos


	mulsd	xmm2, QWORD PTR p_temp[rsp]		; 0.5 * x2 * xx

	mulsd	xmm5, xmm4				; sin *x3
	movsd	xmm4, QWORD PTR __real_3ff0000000000000	; 1.0
	mulsd	xmm3, xmm1				; cos *x4
	subsd	xmm4, xmm6 				; t=1.0-r

	movhlps	xmm1, xmm0
	subsd	xmm5, xmm2				; sin - 0.5 * x2 *xx

	mulsd	xmm1, QWORD PTR p_temp[rsp+8]		; x * xx
	movsd	xmm2, QWORD PTR __real_3ff0000000000000	; 1
	subsd	xmm2, xmm4				; 1 - t
	addsd	xmm5, QWORD PTR p_temp[rsp]		; sin+xx

	subsd	xmm2, xmm6				; (1-t) - r
	subsd	xmm2, xmm1				; ((1 + (-t)) - r) - x*xx
	addsd	xmm0, xmm5				; sin + x
	addsd   xmm3, xmm2				; cos+((1-t)-r - x*xx)
	addsd   xmm3, xmm4				; cos+t

	movapd	xmm5, OWORD PTR p_temp1[rsp]		; load sign
	movlhps xmm0, xmm3
	xorpd	xmm0, xmm5
	jmp __vrd2_cos_cleanup


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
vrd2_not_cos_sin_piby4:
	cmp 	r8, r9
	jnz	vrd2_sin_piby4

vrd2_sin_cos_piby4:

	movapd	OWORD PTR p_temp[rsp], xmm4		; rr move to to memory
	movapd	OWORD PTR p_temp1[rsp], xmm0		; r move to to memory
	movapd	OWORD PTR p_sign[rsp],xmm6

	movapd	xmm3, OWORD PTR cossinarray+50h		; s6
	mulpd	xmm3, xmm2				; x2s6
	movdqa	xmm5, OWORD PTR cossinarray+20h		; s3
	movapd	xmm1, xmm2				; move x2 for x4
	mulpd	xmm1, xmm2				; x4
	mulpd	xmm5, xmm2				; x2s3

	addpd	xmm3, OWORD PTR cossinarray+40h		; s5+x2s6
	movapd	xmm4, xmm2				; move for x6
	mulpd	xmm3, xmm2				; x2(s5+x2s6)
	mulpd	xmm4, xmm1				; x6
	addpd	xmm5, OWORD PTR cossinarray+10h		; s2+x2s3
	mulpd	xmm5, xmm2				; x2(s2+x2s3)
	addpd	xmm3, OWORD PTR cossinarray+30h		; s4 + x2(s5+x2s6)

	movhlps	xmm0, xmm0				; high of x for x3
	mulpd	xmm3, xmm4				; x6(s4 + x2(s5+x2s6))
	addpd	xmm5, OWORD PTR cossinarray		; s1+x2(s2+x2s3)

;	movapd	xmm4, xmm2				; move x2 for x3
;	movhlps	xmm4, xmm4				; high of x2 for x3
	movhlps	xmm4, xmm2				; high of x2 for x3

	addpd	xmm3, xmm5				; z

	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2
	mulsd	xmm4, xmm0				; x3 #
	movhlps	xmm5, xmm3				; xmm5 = sin
							; xmm3 = cos

	mulsd	xmm5, xmm4				; sin*x3 #
	movsd	xmm4, QWORD PTR __real_3ff0000000000000	; 1.0 #
	mulsd	xmm3, xmm1				; cos*x4 #

	subsd	xmm4, xmm2 				; t=1.0-r #

	movhlps	xmm6, xmm2				; move 0.5 * x2 for 0.5 * x2 * xx #
	mulsd	xmm6, QWORD PTR p_temp[rsp+8]		; 0.5 * x2 * xx #
	subsd	xmm5, xmm6				; sin - 0.5 * x2 *xx #
	addsd	xmm5, QWORD PTR p_temp[rsp+8]		; sin+xx #


;	movlpd	xmm6, QWORD PTR p_temp[rsp]		; xx #
;	mulsd	xmm6, xmm0				; xx * x #

	movlpd	xmm6, QWORD PTR p_temp1[rsp]		; x
	mulsd	xmm6, QWORD PTR p_temp[rsp]		; x *xx #

	movsd	xmm1, QWORD PTR __real_3ff0000000000000	; 1 #
	subsd	xmm1, xmm4				; 1 -t #
	addsd	xmm0, xmm5				; sin+x #
	subsd	xmm1, xmm2				; (1-t) - r #
	subsd	xmm1, xmm6				; ((1 + (-t)) - r) - x*xx #
	addsd   xmm3, xmm1				; cos+((1 + (-t)) - r) - x*xx #
	addsd   xmm3, xmm4				; cos+t #

	movapd	xmm2, OWORD PTR p_sign[rsp]		; load sign
	movlhps xmm3, xmm0
	movapd	xmm0, xmm3
	xorpd	xmm0, xmm2
	jmp __vrd2_cos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
vrd2_sin_piby4:
	movapd	xmm3, OWORD PTR sinarray+50h	; s6
	mulpd	xmm3,xmm2			; x2s6
	movapd	xmm5, OWORD PTR sinarray+20h	; s3
	movapd	OWORD PTR p_temp[rsp], xmm4	; store xx
	movapd	xmm1,xmm2			; move for x4
	mulpd	xmm1,xmm2			; x4
	movapd	OWORD PTR p_temp1[rsp], xmm0	; store x

	mulpd	xmm5,xmm2			; x2s3
	movapd 	xmm4,xmm0			; move for x3
	addpd	xmm3,OWORD PTR sinarray+40h	; s5+x2s6
	mulpd	xmm1,xmm2			; x6
	mulpd	xmm3,xmm2			; x2(s5+x2s6)
	mulpd	xmm4,xmm2			; x3
	addpd	xmm5,OWORD PTR sinarray+10h	; s2+x2s3
	mulpd	xmm5,xmm2			; x2(s2+x2s3)
	addpd	xmm3,OWORD PTR sinarray+30h	; s4 + x2(s5+x2s6)

	mulpd	xmm2,OWORD PTR __real_3fe0000000000000 ; 0.5 *x2

	movapd	xmm0,OWORD PTR p_temp[rsp]	; load xx
	mulpd	xmm3,xmm1			; x6(s4 + x2(s5+x2s6))
	addpd	xmm5,OWORD PTR sinarray		; s1+x2(s2+x2s3)
	mulpd	xmm2,xmm0			; 0.5 * x2 *xx
	addpd	xmm3,xmm5			; zs
	mulpd	xmm4,xmm3			; *x3
	subpd	xmm4,xmm2			; x3*zs - 0.5 * x2 *xx
	addpd	xmm0,xmm4			; +xx
	addpd	xmm0,OWORD PTR p_temp1[rsp]	; +x

	xorpd	xmm0, xmm6			; xor sign
	jmp __vrd2_cos_cleanup


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
lower_or_both_arg_gt_5e5:
	cmp	rcx, r10				;is upper arg >= 5e5
	jae	both_arg_gt_5e5

lower_arg_gt_5e5:
; Upper Arg is < 5e5, Lower arg is >= 5e5

	movlpd	QWORD PTR r[rsp], xmm0		;Save lower fp arg for remainder_piby2 call
	movhlps	xmm0, xmm0			;Needed since we want to work on upper arg
	movhlps	xmm2, xmm2
	movhlps	xmm4, xmm4

; Work on Upper arg
; xmm0, xmm2, xmm4 = x, xmm5 = 0.5
; Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs

;If upper Arg is <=piby4
       cmp	rcx, rdx					; is upper arg > piby4
       ja	@F

       mov 	ecx, 0						; region = 0
       mov	DWORD PTR region[rsp+4], ecx			; store upper region
       movlpd	QWORD PTR r[rsp+8], xmm0			; store upper r
       xorpd	xmm4, xmm4					; rr = 0
       movlpd	QWORD PTR rr[rsp+8], xmm4			; store upper rr
       jmp	check_lower_arg

align 16
@@:
;If upper Arg is > piby4
	mulsd	xmm2,QWORD PTR __real_3fe45f306dc9c883		; x*twobypi
	addsd	xmm2,xmm5					; xmm2 = npi2=(x*twobypi+0.5)
	movsd	xmm3,QWORD PTR __real_3ff921fb54400000		; xmm3 = piby2_1
	cvttsd2si	ecx,xmm2				; xmm0 = npi2 trunc to ints
	movsd	xmm1,QWORD PTR __real_3dd0b4611a600000		; xmm1 = piby2_2
	cvtsi2sd	xmm2,ecx				; xmm2 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm3,xmm2					; npi2 * piby2_1
	subsd	xmm4,xmm3					; xmm4 = rhead =(x-npi2*piby2_1)
	movsd	xmm6,QWORD PTR __real_3ba3198a2e037073		; xmm6 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm4					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm1,xmm2					; xmm1 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm4, xmm1					; xmm4 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm6,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm4					; t-rhead
       subsd	xmm1,xmm5					; (rtail-(t-rhead))
       addsd	xmm1,xmm6					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp+4], ecx			; store upper region
       movsd	xmm0,xmm4
       subsd	xmm0,xmm1					; xmm0 = r=(rhead-rtail)
       subsd	xmm4, xmm0					; rr=rhead-r
       subsd	xmm4, xmm1					; xmm4 = rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r[rsp+8], xmm0				; store upper r
       movlpd	QWORD PTR rr[rsp+8], xmm4				; store upper rr

;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
align 16
check_lower_arg:
	mov		r11,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrd2_cos_lower_naninf

	lea	 r9, 	QWORD PTR region[rsp]			; lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 rdx,	QWORD PTR r[rsp]
	movlpd	 xmm0,  QWORD PTR r[rsp]	;Restore lower fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrd2_cos_lower_naninf:
	mov	rax, QWORD PTR p_original[rsp]			; upper arg is nan/inf
;	mov	rax, QWORD PTR r[rsp]				; upper arg is nan/inf

	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				; rr = 0
	mov	DWORD PTR region[rsp], r10d			; region =0

align 16
@@:
	jmp __vrd2_cos_reconstruct

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
upper_arg_gt_5e5:
; Upper Arg is >= 5e5, Lower arg is < 5e5
	movhpd	QWORD PTR r[rsp+8], xmm0		;Save upper fp arg for remainder_piby2 call
	movlhps	xmm0, xmm0			;Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
	movlhps	xmm2, xmm2
	movlhps	xmm4, xmm4


; Work on Lower arg
; xmm0, xmm2, xmm4 = x, xmm5 = 0.5
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
	addsd	xmm2,xmm5					; xmm2 = npi2=(x*twobypi+0.5)
	movsd	xmm3,QWORD PTR __real_3ff921fb54400000		; xmm3 = piby2_1
	cvttsd2si	eax,xmm2				; xmm0 = npi2 trunc to ints
	movsd	xmm1,QWORD PTR __real_3dd0b4611a600000		; xmm1 = piby2_2
	cvtsi2sd	xmm2,eax				; xmm2 = npi2 trunc to doubles

;/* Subtract the multiple from x to get an extra-precision remainder */
;rhead  = x - npi2 * piby2_1;
	mulsd	xmm3,xmm2					; npi2 * piby2_1
	subsd	xmm4,xmm3					; xmm4 = rhead =(x-npi2*piby2_1)
	movsd	xmm6,QWORD PTR __real_3ba3198a2e037073		; xmm6 =piby2_2tail

;t  = rhead;
       movsd	xmm5, xmm4					; xmm5 = t = rhead

;rtail  = npi2 * piby2_2;
       mulsd	xmm1,xmm2					; xmm1 =rtail=(npi2*piby2_2)

;rhead  = t - rtail
       subsd	xmm4, xmm1					; xmm4 =rhead=(t-rtail)

;rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	xmm6,xmm2     					; npi2 * piby2_2tail
       subsd	xmm5,xmm4					; t-rhead
       subsd	xmm1,xmm5					; (rtail-(t-rhead))
       addsd	xmm1,xmm6					; rtail=npi2*piby2_2tail+(rtail-(t-rhead));

;r =  rhead - rtail
;rr = (rhead-r) -rtail
       mov	DWORD PTR region[rsp], eax			; store lower region
       movsd	xmm0,xmm4
       subsd	xmm0,xmm1					; xmm0 = r=(rhead-rtail)
       subsd	xmm4, xmm0					; rr=rhead-r
       subsd	xmm4, xmm1					; xmm4 = rr=((rhead-r) -rtail)
       movlpd	QWORD PTR r[rsp], xmm0				; store lower r
       movlpd	QWORD PTR rr[rsp], xmm4				; store lower rr

;Note that volatiles will be trashed by the call
;We do not care since this is the last check
;We will construct r, rr, region and sign
align 16
check_upper_arg:
	mov		r11,07ff0000000000000h			; is upper arg nan/inf
	mov		r10,r11
	and		r10,rcx
	cmp		r10,r11
	jz		__vrd2_cos_upper_naninf

	lea	 r9, 	QWORD PTR region[rsp+4]			; upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 rdx,	QWORD PTR r[rsp+8]
	movlpd	 xmm0,  QWORD PTR r[rsp+8]	;Restore upper fp arg for remainder_piby2 call

;change to MS ABI - shadow space
sub	rsp,020h	

	call	 __amd_remainder_piby2
	
;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrd2_cos_upper_naninf:
	mov	rcx, QWORD PTR p_original[rsp+8]		; upper arg is nan/inf
;	mov	rcx, QWORD PTR r[rsp+8]				; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			; rr = 0
	mov	DWORD PTR region[rsp+4], r10d			; region =0

align 16
@@:
	jmp __vrd2_cos_reconstruct

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
both_arg_gt_5e5:
;Upper Arg is >= 5e5, Lower arg is >= 5e5

	movhlps xmm6, xmm0		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrd2_cos_lower_naninf_of_both_gt_5e5

	lea	 r9, 	QWORD PTR region[rsp]			;lower arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp]
	lea	 rdx,	QWORD PTR r[rsp]
	mov	 QWORD PTR p_temp[rsp], rcx			;Save upper arg
	
;change to MS ABI - shadow space
sub	rsp,020h	
	
	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h	
	
	mov	 rcx, QWORD PTR p_temp[rsp]			;Restore upper arg
	jmp 	@F

__vrd2_cos_lower_naninf_of_both_gt_5e5:				;lower arg is nan/inf
	mov	rax, QWORD PTR p_original[rsp]
;	movd	rax, xmm0
	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				;rr = 0
	mov	DWORD PTR region[rsp], r10d			;region = 0

align 16
@@:
	mov		r11,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r11
	and		r10,rcx
	cmp		r10,r11
	jz		__vrd2_cos_upper_naninf_of_both_gt_5e5

	lea	 r9, 	QWORD PTR region[rsp+4]			;upper arg is **NOT** nan/inf
	lea	 r8, 	QWORD PTR rr[rsp+8]
	lea	 rdx,	QWORD PTR r[rsp+8]
	movapd	 xmm0,  xmm6			;Restore upper fp arg for remainder_piby2 call
	
;change to MS ABI - shadow space
sub	rsp,020h

	call	 __amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h

	jmp 	@F

__vrd2_cos_upper_naninf_of_both_gt_5e5:
	mov	rcx, QWORD PTR p_original[rsp+8]		;upper arg is nan/inf
;	movd	rcx, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			;rr = 0
	mov	DWORD PTR region[rsp+4], r10d			;region = 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
@@:
__vrd2_cos_reconstruct:
;Construct xmm0=x, xmm2 =x2, xmm4=xx, r8=region, xmm6=sign
	movapd	xmm0, OWORD PTR r[rsp]				;x
	movapd	xmm2, xmm0					;move for x2
	mulpd	xmm2, xmm2					;x2

	movapd	xmm4, OWORD PTR rr[rsp]				;xx

	mov	r8,	QWORD PTR region[rsp]
	mov 	r9, 	QWORD PTR __reald_one_zero		;compare value for sincos path
	mov 	r10,	r8
	and	r8, 	QWORD PTR __reald_one_one		;odd/even region for sin/cos
	add	r10,	QWORD PTR __reald_one_one
	and	r10,	QWORD PTR __reald_two_two
	mov	r11,	r10
	and	r11,	__reald_two_zero		;mask out the lower sign bit leaving the upper sign bit
	shl	r10,	62				;shift lower sign bit left by 63 bits
	shl	r11,	30				;shift upper sign bit left by 31 bits
	mov 	QWORD PTR p_temp[rsp],r10		;write out lower sign bit
	mov 	QWORD PTR p_temp[rsp+8],r11		;write out upper sign bit
	movapd	xmm6, OWORD PTR p_temp[rsp]		;write out both sign bits to xmm6

	jmp __vrd2_cos_approximate

;ENDMAIN

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
__vrd2_cos_cleanup:
	movdqa	xmm6,OWORD PTR [rsp+020h]	; restore xmm6
	add	rsp,128h
	ret

fname	endp
TEXT	ENDS
END
