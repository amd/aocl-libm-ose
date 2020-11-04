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

;
; vrdacos.asm
;
; An array implementation of the cos libm function.
;
; Prototype:
;
;    void vrda_cos(int n, double *x, double *y);
;
;Computes Cosine of x for an array of input values.
;Places the results into the supplied y array.
;Does not perform error checking.
;Denormal inputs may produce unexpected results
;Author: Harsha Jagasia
;Email:  harsha.jagasia@amd.com

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
p_temp		equ		0h		; temporary for get/put bits operation
p_temp1		equ		10h		; temporary for get/put bits operation

save_xmm6		equ		20h		; temporary for get/put bits operation
save_xmm7		equ		30h		; temporary for get/put bits operation
save_xmm8		equ		40h		; temporary for get/put bits operation
save_xmm9		equ		50h		; temporary for get/put bits operation
save_xmm10		equ		60h		; temporary for get/put bits operation
save_xmm11		equ		70h		; temporary for get/put bits operation
save_xmm12		equ		80h		; temporary for get/put bits operation
save_xmm13		equ		90h		; temporary for get/put bits operation
save_xmm14		equ		0A0h		; temporary for get/put bits operation
save_xmm15		equ		0B0h		; temporary for get/put bits operation
save_rdi		equ		0C0h
save_rsi		equ		0D0h

r		equ		0E0h		; pointer to r for remainder_piby2
rr		equ		0F0h		; pointer to r for remainder_piby2
region		equ		0100h		; pointer to r for remainder_piby2

r1		equ		0110h		; pointer to r for remainder_piby2
rr1		equ		0120h		; pointer to r for remainder_piby2
region1		equ		0130h		; pointer to r for remainder_piby2

p_temp2		equ		0140h		; temporary for get/put bits operation
p_temp3		equ		0150h		; temporary for get/put bits operation

p_temp4		equ		0160h		; temporary for get/put bits operation
p_temp5		equ		0170h		; temporary for get/put bits operation

p_original	equ		0180h		; original x
p_mask		equ		0190h		; original x
p_sign		equ		01A0h		; original x

p_original1	equ		01B0h		; original x
p_mask1		equ		01C0h		; original x
p_sign1		equ		01D0h		; original x

save_xa		equ		01E0h		;qword
save_ya		equ		01F0h		;qword

save_nv		equ		0200h		;qword
p_iter		equ		0210h		; qword	storage for number of loop iterations


stack_size	equ		0228h

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
include fm.inc
FN_PROTOTYPE_BAS64 vrda_cos

EXTRN __amd_remainder_piby2:NEAR

.DATA
align 8
	evencos_oddsin_tbl	label	qword
		dq	coscos_coscos_piby4		; 0		*
		dq	coscos_cossin_piby4		; 1		+
		dq	coscos_sincos_piby4		; 2
		dq	coscos_sinsin_piby4		; 3		+

		dq	cossin_coscos_piby4		; 4
		dq	cossin_cossin_piby4		; 5		*
		dq	cossin_sincos_piby4		; 6
		dq	cossin_sinsin_piby4		; 7

		dq	sincos_coscos_piby4		; 8
		dq	sincos_cossin_piby4		; 9
		dq	sincos_sincos_piby4		; 10		*
		dq	sincos_sinsin_piby4		; 11

		dq	sinsin_coscos_piby4		; 12
		dq	sinsin_cossin_piby4		; 13		+
		dq	sinsin_sincos_piby4		; 14
		dq	sinsin_sinsin_piby4		; 15		*

;TEXT	SEGMENT	   page	   'CODE'
.CODE

; parameters are passed in by Microsoft C as:
; ecx - int n
; rdx - double *x
; r8  - double *y

	PUBLIC  fname
fname	proc	frame
    cmp          ecx ,00h
    jle         L__return
    cmp          rdx ,00h
    je         L__return
    cmp          r8 ,00h
    je         L__return

	sub		rsp,stack_size
	.ALLOCSTACK	stack_size				; unwind data, needed since we call
								; remainder_piby2 and could have exceptions								; but it costs no performance to include it.

	movdqa	OWORD PTR [rsp+save_xmm6],xmm6	; save xmm6
	.SAVEXMM128 xmm6, 020h

	movdqa	OWORD PTR [rsp+save_xmm7],xmm7	; save xmm7
	.SAVEXMM128 xmm7, 030h

	movdqa	OWORD PTR [rsp+save_xmm8],xmm8	; save xmm8
	.SAVEXMM128 xmm8, 040h

	movdqa	OWORD PTR [rsp+save_xmm9],xmm9	; save xmm9
	.SAVEXMM128 xmm9, 050h

	movdqa	OWORD PTR [rsp+save_xmm10],xmm10	; save xmm10
	.SAVEXMM128 xmm10, 060h

	movdqa	OWORD PTR [rsp+save_xmm11],xmm11	; save xmm11
	.SAVEXMM128 xmm11, 070h

	movdqa	OWORD PTR [rsp+save_xmm12],xmm12	; save xmm12
	.SAVEXMM128 xmm12, 080h

	movdqa	OWORD PTR [rsp+save_xmm13],xmm13	; save xmm13
	.SAVEXMM128 xmm13, 090h

	mov		QWORD PTR [rsp+save_rsi],rsi	; save rsi
	.SAVEREG rsi, save_rsi

	mov		QWORD PTR [rsp+save_rdi],rdi	; save rdi
	.SAVEREG rdi, save_rdi

	.ENDPROLOG

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;START PROCESS INPUT
; save the arguments
	mov		QWORD PTR [rsp+save_xa],rdx	; save x_array pointer
	mov		QWORD PTR [rsp+save_ya],r8	; save y_array pointer
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
	movapd	xmm2, OWORD PTR __real_7fffffffffffffff
	mov		rsi,QWORD PTR [rsp+save_xa]	; get x_array pointer
	movlpd	xmm0,QWORD PTR [rsi]
	movhpd	xmm0,QWORD PTR [rsi+8]
	prefetch	QWORD PTR [rsi+64]
	add		rsi,32
	mov		QWORD PTR [rsp+save_xa],rsi	; save x_array pointer

	movdqa	OWORD PTR p_original[rsp], xmm0

	movlpd	xmm1,QWORD PTR [rsi-16]
	movhpd	xmm1,QWORD PTR [rsi-8]

	movdqa	OWORD PTR p_original1[rsp], xmm1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;STARTMAIN

;movapd	xmm2, OWORD PTR __real_7fffffffffffffff	;moved above
;movdqa	OWORD PTR p_original[rsp], xmm0		;moved above
;movdqa	OWORD PTR p_original1[rsp], xmm1

andpd 	xmm0, xmm2	;Unsign
andpd 	xmm1, xmm2	;Unsign

movd	rax, xmm0				;rax is lower arg
movhpd	QWORD PTR p_temp[rsp+8], xmm0		;
mov    	rcx, QWORD PTR p_temp[rsp+8]		;rcx = upper arg
movd	r8, xmm1				;rax is lower arg
movhpd	QWORD PTR p_temp1[rsp+8], xmm1		;
mov    	r9, QWORD PTR p_temp1[rsp+8]		;rcx = upper arg

mov 	rdx, 3FE921FB54442D18h			;piby4	+
mov	r10, 411E848000000000h			;5e5	+

movapd	xmm4, OWORD PTR __real_3fe0000000000000	;0.5 for later use

movapd	xmm2, xmm0				;x0
movapd	xmm3, xmm1				;x1
movapd	xmm6, xmm0				;x0
movapd	xmm7, xmm1				;x1

;DEBUG
;	movapd	xmm4, xmm0
;	movapd	xmm5, xmm1
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG

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
	movapd	xmm0, XMMWORD PTR __real_3fe45f306dc9c883
	mulpd	xmm2, xmm0						; * twobypi
	mulpd	xmm3, xmm0						; * twobypi

	addpd	xmm2,xmm4						; +0.5, npi2
	addpd	xmm3,xmm4						; +0.5, npi2

	movapd	xmm0,XMMWORD PTR __real_3ff921fb54400000		; piby2_1
	movapd	xmm1,XMMWORD PTR __real_3ff921fb54400000		; piby2_1

	cvttpd2dq	xmm4,xmm2					; convert packed double to packed integers
	cvttpd2dq	xmm5,xmm3					; convert packed double to packed integers

	movapd	xmm8,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2
	movapd	xmm9,XMMWORD PTR __real_3dd0b4611a600000		; piby2_2

	cvtdq2pd	xmm2,xmm4					; and back to double.
	cvtdq2pd	xmm3,xmm5					; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */

	movd	rax, xmm4						; Region
	movd	rcx, xmm5						; Region

	mov	r8,  rax
	mov	r9,  rcx

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

;	paddd		xmm4,	XMMWORD PTR __reald_one_one		; Sign
;	paddd		xmm5,	XMMWORD PTR __reald_one_one		; Sign
;	pand		xmm4,	XMMWORD PTR __reald_two_two
;	pand		xmm5,	XMMWORD PTR __reald_two_two
;	punpckldq 	xmm4,	xmm4
;	punpckldq 	xmm5,	xmm5
;	psllq		xmm4,	62
;	psllq		xmm5,	62


	add r8, QWORD PTR __reald_one_one
	add r9, QWORD PTR __reald_one_one
	and r8, QWORD PTR __reald_two_two
	and r9, QWORD PTR __reald_two_two

	mov r10, r8
	mov r11, r9
	shl r8, 62
	and r10,QWORD PTR __reald_two_zero
	shl r10, 30
	shl r9, 62
	and r11,QWORD PTR __reald_two_zero
	shl r11, 30

	mov	QWORD PTR p_sign[rsp], r8
	mov	QWORD PTR p_sign[rsp+8],r10
	mov	QWORD PTR p_sign1[rsp], r9
	mov	QWORD PTR p_sign1[rsp+8],r11

; GET_BITS_DP64(rhead-rtail, uy);			   		; originally only rhead
; xmm4  = Sign, xmm0 =rhead, xmm8 =rtail
; xmm5  = Sign, xmm1 =rhead, xmm9 =rtail
	movapd	xmm6,xmm0						; rhead
	movapd	xmm7,xmm1						; rhead

	and	rax, QWORD PTR __reald_one_one		; Region
	and	rcx, QWORD PTR __reald_one_one		; Region

	subpd	xmm0,xmm8						; r = rhead - rtail
	subpd	xmm1,xmm9						; r = rhead - rtail

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; xmm4  = Sign, xmm0 = r, xmm6 =rhead, xmm8 =rtail
; xmm5  = Sign, xmm1 = r, xmm7 =rhead, xmm9 =rtail

	subpd	xmm6, xmm0				;rr=rhead-r
	subpd	xmm7, xmm1				;rr=rhead-r

	mov	r8,  rax
	mov	r9,  rcx

	movapd	xmm2,xmm0
	movapd	xmm3,xmm1

	mulpd	xmm2,xmm0				; r2
	mulpd	xmm3,xmm1				; r2

	subpd	xmm6, xmm8				;rr=(rhead-r) -rtail
	subpd	xmm7, xmm9				;rr=(rhead-r) -rtail


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
align 16
first_or_next3_arg_gt_5e5:
; rax, rcx, r8, r9

;DEBUG
;	movapd	xmm4, xmm0
;	movapd	xmm5, xmm1
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG

	cmp	rcx, r10				;is upper arg >= 5e5
	jae	both_arg_gt_5e5

lower_arg_gt_5e5:
; Upper Arg is < 5e5, Lower arg is >= 5e5
; xmm0, xmm2, xmm6 = x, xmm4 = 0.5
; Be sure not to use xmm1, xmm3 and xmm7
; Use xmm5, xmm8, xmm10, xmm12
;	    xmm9, xmm11, xmm13


;DEBUG
;	movapd	xmm4, xmm0
;	movapd	xmm5, xmm1
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG


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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Note that volatiles will be trashed by the call
;We will construct r, rr, region and sign

; Work on Lower arg
	mov		r11,07ff0000000000000h			; is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrd4_cos_lower_naninf


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

__vrd4_cos_lower_naninf:
	mov	rax, QWORD PTR p_original[rsp]			; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rax,r11
	mov	QWORD PTR r[rsp],rax				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp], r10				; rr = 0
	mov	DWORD PTR region[rsp], r10d			; region =0

align 16
@@:


;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, xmm1
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
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
;	jmp __vrda_cos_cleanup
;DEBUG




	movhlps xmm6, xmm0		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,rax
	cmp		r10,r11
	jz		__vrd4_cos_lower_naninf_of_both_gt_5e5

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

__vrd4_cos_lower_naninf_of_both_gt_5e5:				;lower arg is nan/inf
	mov	rax, QWORD PTR p_original[rsp]
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
	jz		__vrd4_cos_upper_naninf_of_both_gt_5e5



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

__vrd4_cos_upper_naninf_of_both_gt_5e5:
	mov	rcx, QWORD PTR p_original[rsp+8]		;upper arg is nan/inf
;	movd	rcx, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	rcx,r11
	mov	QWORD PTR r[rsp+8],rcx				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr[rsp+8], r10			;rr = 0
	mov	DWORD PTR region[rsp+4], r10d			;region = 0

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
	jz		__vrd4_cos_upper_naninf

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

	mov	 r8,  QWORD PTR p_temp[rsp]
	mov	 r9,  QWORD PTR p_temp2[rsp]
	movapd	xmm1, OWORD PTR p_temp1[rsp]
	movapd	xmm3, OWORD PTR p_temp3[rsp]
	movapd	xmm7, OWORD PTR p_temp5[rsp]
	jmp 	@F

__vrd4_cos_upper_naninf:
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
	jmp check_next2_args


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
check_next2_args:

;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, xmm1
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
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

	jmp	__vrd4_cos_reconstruct


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
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
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
	jz		__vrd4_cos_lower_naninf_higher

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

__vrd4_cos_lower_naninf_higher:
	mov	r8, QWORD PTR p_original1[rsp]			; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r8,r11
	mov	QWORD PTR r1[rsp],r8				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp], r10				; rr = 0
	mov	DWORD PTR region1[rsp], r10d			; region =0

align 16
@@:
	jmp __vrd4_cos_reconstruct







align 16
both_arg_gt_5e5_higher:
; Upper Arg is >= 5e5, Lower arg is >= 5e5
; r8, r9
; xmm1, xmm3, xmm7 = x, xmm4 = 0.5


;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movd	xmm5, r8
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG

	movhlps xmm7, xmm1		;Save upper fp arg for remainder_piby2 call

	mov		r11,07ff0000000000000h			;is lower arg nan/inf
	mov		r10,r11
	and		r10,r8
	cmp		r10,r11
	jz		__vrd4_cos_lower_naninf_of_both_gt_5e5_higher

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
;	jmp __vrda_cos_cleanup
;DEBUG



	jmp 	@F

__vrd4_cos_lower_naninf_of_both_gt_5e5_higher:				;lower arg is nan/inf
	mov	r8, QWORD PTR p_original1[rsp]
	mov	r11,00008000000000000h
	or	r8,r11
	mov	QWORD PTR r1[rsp],r8				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp], r10				;rr = 0
	mov	DWORD PTR region1[rsp], r10d			;region = 0

align 16
@@:
	mov		r11,07ff0000000000000h			;is upper arg nan/inf
	mov		r10,r11
	and		r10,r9
	cmp		r10,r11
	jz		__vrd4_cos_upper_naninf_of_both_gt_5e5_higher

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

__vrd4_cos_upper_naninf_of_both_gt_5e5_higher:
	mov	r9, QWORD PTR p_original1[rsp+8]		;upper arg is nan/inf
;	movd	r9, xmm6					;upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r9,r11
	mov	QWORD PTR r1[rsp+8],r9				;r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp+8], r10			;rr = 0
	mov	DWORD PTR region1[rsp+4], r10d			;region = 0

align 16
@@:

;DEBUG
;	movapd	xmm4, OWORD PTR r[rsp]
;	movapd	xmm5, OWORD PTR r1[rsp]
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG


	jmp __vrd4_cos_reconstruct

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
	jz		__vrd4_cos_upper_naninf_higher

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

__vrd4_cos_upper_naninf_higher:
	mov	r9, QWORD PTR p_original1[rsp+8]		; upper arg is nan/inf
;	mov	r9, QWORD PTR r1[rsp+8]				; upper arg is nan/inf
	mov	r11,00008000000000000h
	or	r9,r11
	mov	QWORD PTR r1[rsp+8],r9				; r = x | 0x0008000000000000
	xor	r10, r10
	mov	QWORD PTR rr1[rsp+8], r10			; rr = 0
	mov	DWORD PTR region1[rsp+4], r10d			; region =0

align 16
@@:
	jmp	__vrd4_cos_reconstruct


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
__vrd4_cos_reconstruct:
;Results
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;DEBUG
;	movapd	xmm4, OWORD PTR region[rsp]
;	movapd	xmm5, OWORD PTR region1[rsp]
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG

	movapd	xmm0, OWORD PTR r[rsp]
	movapd	xmm1, OWORD PTR r1[rsp]

	movapd	xmm6, OWORD PTR rr[rsp]
	movapd	xmm7, OWORD PTR rr1[rsp]

	mov	rax,QWORD PTR region[rsp]
	mov	rcx,QWORD PTR region1[rsp]

	mov	r8, rax
	mov	r9, rcx

	add r8, QWORD PTR __reald_one_one
	add r9, QWORD PTR __reald_one_one
	and r8, QWORD PTR __reald_two_two
	and r9, QWORD PTR __reald_two_two

	mov r10, r8
	mov r11, r9
	shl r8, 62
	and r10,QWORD PTR __reald_two_zero
	shl r10, 30
	shl r9, 62
	and r11,QWORD PTR __reald_two_zero
	shl r11, 30

	mov	QWORD PTR p_sign[rsp], r8
	mov	QWORD PTR p_sign[rsp+8],r10
	mov	QWORD PTR p_sign1[rsp], r9
	mov	QWORD PTR p_sign1[rsp+8],r11

	and	rax, QWORD PTR __reald_one_one		; Region
	and	rcx, QWORD PTR __reald_one_one		; Region

	mov	r8,  rax
	mov	r9,  rcx

	movapd	xmm2,xmm0
	movapd	xmm3,xmm1

	mulpd	xmm2,xmm0				; r2
	mulpd	xmm3,xmm1				; r2

	and	rax, QWORD PTR __reald_zero_one
	and	rcx, QWORD PTR __reald_zero_one
	shr	r8,  31
	shr	r9,  31
	or	rax, r8
	or	rcx, r9
	shl	rcx, 2
	or	rax, rcx



;DEBUG
;	movd	xmm4, rax
;	movd	xmm5, rax
;	xorpd	xmm0, xmm0
;	xorpd	xmm1, xmm1
;	jmp __vrda_cos_cleanup
;DEBUG

	lea	rcx, QWORD PTR evencos_oddsin_tbl
	jmp	QWORD PTR [rcx + rax*8]	;Jmp table for cos/sin calculation based on even/odd region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
PUBLIC	__vrda_cos_cleanup
__vrda_cos_cleanup::

	movapd	xmm0, p_sign[rsp]
	movapd	xmm1, p_sign1[rsp]

	xorpd	xmm0, xmm4			; (+) Sign
	xorpd	xmm1, xmm5			; (+) Sign

__vrda_bottom1:
; store the result _m128d
	mov	rdi,QWORD PTR [rsp+save_ya]	; get y_array pointer
	movlpd	QWORD PTR [rdi],xmm0
	movhpd	QWORD PTR 8[rdi],xmm0

__vrda_bottom2:

	prefetch	QWORD PTR [rdi+64]
	add		rdi,32
	mov		QWORD PTR [rsp+save_ya],rdi	; save y_array pointer

; store the result _m128d
	movlpd	QWORD PTR [rdi-16],xmm1
	movhpd	QWORD PTR [rdi-8],xmm1

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
	movdqa	xmm10,OWORD PTR [rsp+save_xmm10]; restore xmm10
	movdqa	xmm11,OWORD PTR [rsp+save_xmm11]; restore xmm11
	movdqa	xmm12,OWORD PTR [rsp+save_xmm12]; restore xmm12
	movdqa	xmm13,OWORD PTR [rsp+save_xmm13]; restore xmm13
	mov	rsi,QWORD PTR [rsp+save_rsi]	; restore rsi
	mov	rdi,QWORD PTR [rsp+save_rdi]	; restore rdi

	add	rsp,stack_size
L__return:	
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; we jump here when we have an odd number of cos calls to make at the end
; we assume that rdx is pointing at the next x array element, r8 at the next y array element.
; The number of values left is in save_nv

align	16
__vrda_cleanup:
	mov		rax,save_nv[rsp]	; get number of values
	test	rax,rax
	jz		__final_check
	mov		rsi,QWORD PTR [rsp+save_xa]
	mov		rdi,QWORD PTR [rsp+save_ya]

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
	lea		r8, p_temp2[rsp]; &y parameter
	call		fname				; call recursively to compute four values

; now copy the results to the destination array
	mov		rdi,save_ya[rsp]
	mov		rax,save_nv[rsp]	; get number of values
	mov	 	rcx,p_temp2[rsp]
	mov		[rdi],rcx			; we know there's at least one
	cmp		rax,2
	jl		__vrdacgf

	mov	 	rcx,p_temp2[rsp+8]
	mov		8[rdi],rcx			; do the second value
	cmp		rax,3
	jl		__vrdacgf

	mov	 	rcx,p_temp2[rsp+16]
	mov		16[rdi],rcx			; do the third value

__vrdacgf:
	jmp		__final_check

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
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
PUBLIC coscos_coscos_piby4
coscos_coscos_piby4:
	movapd	xmm10, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR cosarray+50h			; c6
	movdqa	xmm5, OWORD PTR cosarray+50h			; c6

	movapd	xmm8, OWORD PTR cosarray+20h			; c3
	movapd	xmm9, OWORD PTR cosarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; r
	movapd	OWORD PTR p_temp3[rsp], xmm11			; r

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subpd	xmm10, OWORD PTR __real_3ff0000000000000		; -t=r-1.0	;trash r
	subpd	xmm11, OWORD PTR __real_3ff0000000000000		; -t=r-1.0	;trash r

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm5,  OWORD PTR cosarray+40h			; c5+x2c6

	addpd	xmm8, OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR cosarray+10h			; c2+x2C3

	addpd   xmm10, OWORD PTR __real_3ff0000000000000		; 1 + (-t)	;trash t
	addpd   xmm11, OWORD PTR __real_3ff0000000000000		; 1 + (-t)	;trash t

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)

	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)

	addpd	xmm8, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR cosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zc

	mulpd	xmm2, xmm2					; x4 recalculate
	mulpd	xmm3, xmm3					; x4 recalculate

	movapd   xmm12, OWORD PTR p_temp2[rsp]			; r
	movapd   xmm13, OWORD PTR p_temp3[rsp]			; r

	mulpd	xmm6, xmm0					; x * xx
	mulpd	xmm7, xmm1					; x * xx

	subpd   xmm10, xmm12					; (1 + (-t)) - r
	subpd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x4 * zc
	mulpd	xmm5, xmm3					; x4 * zc

	subpd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx
	subpd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	addpd   xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addpd   xmm5, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)

	subpd	xmm12, OWORD PTR __real_3ff0000000000000		; t relaculate, -t = r-1
	subpd	xmm13, OWORD PTR __real_3ff0000000000000		; t relaculate, -t = r-1

	subpd   xmm4, xmm12					; + t
	subpd   xmm5, xmm13					; + t

	jmp __vrda_cos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
cossin_cossin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movapd	OWORD PTR p_temp[rsp],  xmm6		; Store rr
	movapd	OWORD PTR p_temp1[rsp], xmm7		; Store rr

	movdqa	xmm4, OWORD PTR sincosarray+50h		; s6
	movdqa	xmm5, OWORD PTR sincosarray+50h		; s6
	movapd	xmm8, OWORD PTR sincosarray+20h		; s3
	movapd	xmm9, OWORD PTR sincosarray+20h		; s3

	movapd	xmm10, xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4

	mulpd	xmm4, xmm2				; x2s6
	mulpd	xmm5, xmm3				; x2s6
	mulpd	xmm8, xmm2				; x2s3
	mulpd	xmm9, xmm3				; x2s3

	mulpd	xmm10, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR sincosarray+40h		; s5+x2s6
	addpd	xmm5, OWORD PTR sincosarray+40h		; s5+x2s6
	addpd	xmm8, OWORD PTR sincosarray+10h		; s2+x2s3
	addpd	xmm9, OWORD PTR sincosarray+10h		; s2+x2s3

	movapd	xmm12, xmm2				; move x2 for x6
	movapd	xmm13, xmm3				; move x2 for x6

	mulpd	xmm4, xmm2				; x2(s5+x2s6)
	mulpd	xmm5, xmm3				; x2(s5+x2s6)
	mulpd	xmm8, xmm2				; x2(s2+x2s3)
	mulpd	xmm9, xmm3				; x2(s2+x2s3)

	mulpd	xmm12, xmm10				; x6
	mulpd	xmm13, xmm11				; x6

	addpd	xmm4, OWORD PTR sincosarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm5, OWORD PTR sincosarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm8, OWORD PTR sincosarray		; s1+x2(s2+x2s3)
	addpd	xmm9, OWORD PTR sincosarray		; s1+x2(s2+x2s3)

	movhlps	xmm10, xmm10				; move high x4 for cos term
	movhlps	xmm11, xmm11				; move high x4 for cos term
	mulpd	xmm4, xmm12				; x6(s4+x2(s5+x2s6))
	mulpd	xmm5, xmm13				; x6(s4+x2(s5+x2s6))

	movsd	xmm6, xmm2				; move low x2 for x3 for sin term
	movsd	xmm7, xmm3				; move low x2 for x3 for sin term
	mulsd	xmm6, xmm0				; get low x3 for sin term
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
	mulsd	xmm8, xmm10				; cos *x4
	mulsd	xmm9, xmm11				; cos *x4

	mulsd	xmm2, QWORD PTR p_temp[rsp]		; 0.5 * x2 * xx for sin term
	mulsd	xmm3, QWORD PTR p_temp1[rsp]		; 0.5 * x2 * xx for sin term
	movsd	xmm6, xmm12				; Keep high r for cos term
	movsd	xmm7, xmm13				; Keep high r for cos term

	subsd	xmm12, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm13, QWORD PTR __real_3ff0000000000000; -t=r-1.0

	subsd	xmm4, xmm2				; sin - 0.5 * x2 *xx
	subsd	xmm5, xmm3				; sin - 0.5 * x2 *xx

	movhlps	xmm10, xmm0				; move high x for x*xx for cos term
	movhlps	xmm11, xmm1				; move high x for x*xx for cos term

	mulsd	xmm10, QWORD PTR p_temp[rsp+8]		; x * xx
	mulsd	xmm11, QWORD PTR p_temp1[rsp+8]		; x * xx

	movsd	xmm2,  xmm12				; move -t for cos term
	movsd	xmm3,  xmm13				; move -t for cos term

	addsd	xmm12, QWORD PTR __real_3ff0000000000000; 1+(-t)
	addsd	xmm13, QWORD PTR __real_3ff0000000000000; 1+(-t)
	addsd	xmm4, QWORD PTR p_temp[rsp]		; sin+xx
	addsd	xmm5, QWORD PTR p_temp1[rsp]		; sin+xx
	subsd	xmm12, xmm6				; (1-t) - r
	subsd	xmm13, xmm7				; (1-t) - r
	subsd	xmm12, xmm10				; ((1 + (-t)) - r) - x*xx
	subsd	xmm13, xmm11				; ((1 + (-t)) - r) - x*xx
	addsd	xmm4, xmm0				; sin + x
	addsd	xmm5, xmm1				; sin + x
	addsd   xmm8, xmm12				; cos+((1-t)-r - x*xx)
	addsd   xmm9, xmm13				; cos+((1-t)-r - x*xx)
	subsd   xmm8, xmm2				; cos+t
	subsd   xmm9, xmm3				; cos+t

	movlhps	xmm4, xmm8
	movlhps	xmm5, xmm9
	jmp 	__vrda_cos_cleanup

align 16
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sincos_cossin_piby4:					; changed from sincos_sincos
							; xmm1 is cossin and xmm0 is sincos

	movapd	OWORD PTR p_temp[rsp],  xmm6		; Store rr
	movapd	OWORD PTR p_temp1[rsp], xmm7		; Store rr
	movapd	OWORD PTR p_temp3[rsp], xmm1		; Store r for the sincos term

	movapd	xmm4, OWORD PTR sincosarray+50h		; s6
	movapd	xmm5, OWORD PTR cossinarray+50h		; s6
	movdqa	xmm8, OWORD PTR sincosarray+20h		; s3
	movdqa	xmm9, OWORD PTR cossinarray+20h		; s3

	movapd	xmm10, xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4

	mulpd	xmm4, xmm2				; x2s6
	mulpd	xmm5, xmm3				; x2s6
	mulpd	xmm8, xmm2				; x2s3
	mulpd	xmm9, xmm3				; x2s3

	mulpd	xmm10, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR sincosarray+40h		; s5+x2s6
	addpd	xmm5, OWORD PTR cossinarray+40h		; s5+x2s6
	addpd	xmm8, OWORD PTR sincosarray+10h		; s2+x2s3
	addpd	xmm9, OWORD PTR cossinarray+10h		; s2+x2s3

	movapd	xmm12, xmm2				; move x2 for x6
	movapd	xmm13, xmm3				; move x2 for x6

	mulpd	xmm4, xmm2				; x2(s5+x2s6)
	mulpd	xmm5, xmm3				; x2(s5+x2s6)
	mulpd	xmm8, xmm2				; x2(s2+x2s3)
	mulpd	xmm9, xmm3				; x2(s2+x2s3)

	mulpd	xmm12, xmm10				; x6
	mulpd	xmm13, xmm11				; x6

	addpd	xmm4, OWORD PTR sincosarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm5, OWORD PTR cossinarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm8, OWORD PTR sincosarray		; s1+x2(s2+x2s3)
	addpd	xmm9, OWORD PTR cossinarray		; s1+x2(s2+x2s3)

	movhlps	xmm10, xmm10				; move high x4 for cos term

	mulpd	xmm4, xmm12				; x6(s4+x2(s5+x2s6))
	mulpd	xmm5, xmm13				; x6(s4+x2(s5+x2s6))

	movsd	xmm6, xmm2				; move low x2 for x3 for sin term  (cossin)
	movhlps	xmm7, xmm3				; move high x2 for x3 for sin term (sincos)

	mulsd	xmm6, xmm0				; get low x3 for sin term
	mulsd	xmm7, QWORD PTR p_temp3[rsp+8]		; get high x3 for sin term

	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms
	mulpd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms


	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z

	movhlps	xmm12, xmm2				; move high r for cos (cossin)
	movhlps	xmm13, xmm3				; move high 0.5*x2 for sin term (sincos)

	movhlps	xmm8,  xmm4				; xmm8 = cos , xmm4 = sin	(cossin)
	movhlps	xmm9,  xmm5				; xmm9 = sin , xmm5 = cos	(sincos)

	mulsd	xmm4, xmm6				; sin *x3
	mulsd	xmm5, xmm11				; cos *x4
	mulsd	xmm8, xmm10				; cos *x4
	mulsd	xmm9, xmm7				; sin *x3

	mulsd	xmm2, QWORD PTR p_temp[rsp]		; low  0.5 * x2 * xx for sin term (cossin)
	mulsd	xmm13, QWORD PTR p_temp1[rsp+8]		; high 0.5 * x2 * xx for sin term (sincos)

	movsd	xmm6, xmm12				; Keep high r for cos term
	movsd	xmm7, xmm3				; Keep low r for cos term

	subsd	xmm12, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm3, QWORD PTR __real_3ff0000000000000 ; -t=r-1.0

	subsd	xmm4, xmm2				; sin - 0.5 * x2 *xx	(cossin)
	subsd	xmm9, xmm13				; sin - 0.5 * x2 *xx	(sincos)

	movhlps	xmm10, xmm0				; move high x for x*xx for cos term (cossin)
	movhlps	xmm11, xmm1				; move high x for x for sin term    (sincos)

	mulsd	xmm10,  QWORD PTR p_temp[rsp+8]		; x * xx
	mulsd	xmm1,   QWORD PTR p_temp1[rsp]		; x * xx

	movsd	xmm2,   xmm12				; move -t for cos term
	movsd	xmm13,  xmm3				; move -t for cos term

	addsd	xmm12,QWORD PTR __real_3ff0000000000000	; 1+(-t)
	addsd	xmm3, QWORD PTR __real_3ff0000000000000 ; 1+(-t)

	addsd	xmm4, QWORD PTR p_temp[rsp]		; sin+xx	+
	addsd	xmm9, QWORD PTR p_temp1[rsp+8]		; sin+xx	+

	subsd	xmm12,xmm6				; (1-t) - r
	subsd	xmm3, xmm7				; (1-t) - r

	subsd	xmm12,xmm10				; ((1 + (-t)) - r) - x*xx
	subsd	xmm3, xmm1				; ((1 + (-t)) - r) - x*xx

	addsd	xmm4, xmm0				; sin + x	+
	addsd	xmm9, xmm11				; sin + x	+

	addsd   xmm8, xmm12				; cos+((1-t)-r - x*xx)
	addsd   xmm5, xmm3				; cos+((1-t)-r - x*xx)

	subsd   xmm8, xmm2				; cos+t
	subsd   xmm5, xmm13				; cos+t

	movlhps	xmm4, xmm8				; cossin
	movlhps	xmm5, xmm9				; sincos

	jmp	__vrda_cos_cleanup

align 16
sincos_sincos_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movapd	OWORD PTR p_temp[rsp],  xmm6		; Store rr
	movapd	OWORD PTR p_temp1[rsp], xmm7		; Store rr
	movapd	OWORD PTR p_temp2[rsp], xmm0		; Store r
	movapd	OWORD PTR p_temp3[rsp], xmm1		; Store r


	movapd	xmm4, OWORD PTR cossinarray+50h		; s6
	movapd	xmm5, OWORD PTR cossinarray+50h		; s6
	movdqa	xmm8, OWORD PTR cossinarray+20h		; s3
	movdqa	xmm9, OWORD PTR cossinarray+20h		; s3

	movapd	xmm10, xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4

	mulpd	xmm4, xmm2				; x2s6
	mulpd	xmm5, xmm3				; x2s6
	mulpd	xmm8, xmm2				; x2s3
	mulpd	xmm9, xmm3				; x2s3

	mulpd	xmm10, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR cossinarray+40h		; s5+x2s6
	addpd	xmm5, OWORD PTR cossinarray+40h		; s5+x2s6
	addpd	xmm8, OWORD PTR cossinarray+10h		; s2+x2s3
	addpd	xmm9, OWORD PTR cossinarray+10h		; s2+x2s3

	movapd	xmm12, xmm2				; move x2 for x6
	movapd	xmm13, xmm3				; move x2 for x6

	mulpd	xmm4, xmm2				; x2(s5+x2s6)
	mulpd	xmm5, xmm3				; x2(s5+x2s6)
	mulpd	xmm8, xmm2				; x2(s2+x2s3)
	mulpd	xmm9, xmm3				; x2(s2+x2s3)

	mulpd	xmm12, xmm10				; x6
	mulpd	xmm13, xmm11				; x6

	addpd	xmm4, OWORD PTR cossinarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm5, OWORD PTR cossinarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm8, OWORD PTR cossinarray		; s1+x2(s2+x2s3)
	addpd	xmm9, OWORD PTR cossinarray		; s1+x2(s2+x2s3)

	mulpd	xmm4, xmm12				; x6(s4+x2(s5+x2s6))
	mulpd	xmm5, xmm13				; x6(s4+x2(s5+x2s6))

	movhlps	xmm6, xmm2				; move low x2 for x3 for sin term
	movhlps	xmm7, xmm3				; move low x2 for x3 for sin term
	mulsd	xmm6, QWORD PTR p_temp2[rsp+8]		; get low x3 for sin term
	mulsd	xmm7, QWORD PTR p_temp3[rsp+8]		; get low x3 for sin term

	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms
	mulpd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms

	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z

	movhlps	xmm12, xmm2				; move high 0.5*x2 for sin term
	movhlps	xmm13, xmm3				; move high 0.5*x2 for sin term
							; Reverse 12 and 2

	movhlps	xmm8,  xmm4				; xmm8 = sin , xmm4 = cos
	movhlps	xmm9,  xmm5				; xmm9 = sin , xmm5 = cos

	mulsd	xmm8, xmm6				; sin *x3
	mulsd	xmm9, xmm7				; sin *x3
	mulsd	xmm4, xmm10				; cos *x4
	mulsd	xmm5, xmm11				; cos *x4

	mulsd	xmm12, QWORD PTR p_temp[rsp+8]		; 0.5 * x2 * xx for sin term
	mulsd	xmm13, QWORD PTR p_temp1[rsp+8]		; 0.5 * x2 * xx for sin term
	movsd	xmm6, xmm2				; Keep high r for cos term
	movsd	xmm7, xmm3				; Keep high r for cos term

	subsd	xmm2, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm3, QWORD PTR __real_3ff0000000000000; -t=r-1.0

	subsd	xmm8, xmm12				; sin - 0.5 * x2 *xx
	subsd	xmm9, xmm13				; sin - 0.5 * x2 *xx

	movhlps	xmm10, xmm0				; move high x for x for sin term
	movhlps	xmm11, xmm1				; move high x for x for sin term
							; Reverse 10 and 0

	mulsd	xmm0, QWORD PTR p_temp[rsp]		; x * xx
	mulsd	xmm1, QWORD PTR p_temp1[rsp]		; x * xx

	movsd	xmm12,  xmm2				; move -t for cos term
	movsd	xmm13,  xmm3				; move -t for cos term

	addsd	xmm2, QWORD PTR __real_3ff0000000000000; 1+(-t)
	addsd	xmm3, QWORD PTR __real_3ff0000000000000; 1+(-t)
	addsd	xmm8, QWORD PTR p_temp[rsp+8]		; sin+xx
	addsd	xmm9, QWORD PTR p_temp1[rsp+8]		; sin+xx

	subsd	xmm2, xmm6				; (1-t) - r
	subsd	xmm3, xmm7				; (1-t) - r

	subsd	xmm2, xmm0				; ((1 + (-t)) - r) - x*xx
	subsd	xmm3, xmm1				; ((1 + (-t)) - r) - x*xx

	addsd	xmm8, xmm10				; sin + x
	addsd	xmm9, xmm11				; sin + x

	addsd   xmm4, xmm2				; cos+((1-t)-r - x*xx)
	addsd   xmm5, xmm3				; cos+((1-t)-r - x*xx)

	subsd   xmm4, xmm12				; cos+t
	subsd   xmm5, xmm13				; cos+t

	movlhps	xmm4, xmm8
	movlhps	xmm5, xmm9
	jmp 	__vrda_cos_cleanup

align 16
cossin_sincos_piby4:					; changed from sincos_sincos
							; xmm1 is cossin and xmm0 is sincos
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	movapd	OWORD PTR p_temp[rsp],  xmm6		; Store rr
	movapd	OWORD PTR p_temp1[rsp], xmm7		; Store rr
	movapd	OWORD PTR p_temp2[rsp], xmm0		; Store r


	movapd	xmm4, OWORD PTR cossinarray+50h		; s6
	movapd	xmm5, OWORD PTR sincosarray+50h		; s6
	movdqa	xmm8, OWORD PTR cossinarray+20h		; s3
	movdqa	xmm9, OWORD PTR sincosarray+20h		; s3

	movapd	xmm10, xmm2				; move x2 for x4
	movapd	xmm11, xmm3				; move x2 for x4

	mulpd	xmm4, xmm2				; x2s6
	mulpd	xmm5, xmm3				; x2s6
	mulpd	xmm8, xmm2				; x2s3
	mulpd	xmm9, xmm3				; x2s3

	mulpd	xmm10, xmm2				; x4
	mulpd	xmm11, xmm3				; x4

	addpd	xmm4, OWORD PTR cossinarray+40h		; s5+x2s6
	addpd	xmm5, OWORD PTR sincosarray+40h		; s5+x2s6
	addpd	xmm8, OWORD PTR cossinarray+10h		; s2+x2s3
	addpd	xmm9, OWORD PTR sincosarray+10h		; s2+x2s3

	movapd	xmm12, xmm2				; move x2 for x6
	movapd	xmm13, xmm3				; move x2 for x6

	mulpd	xmm4, xmm2				; x2(s5+x2s6)
	mulpd	xmm5, xmm3				; x2(s5+x2s6)
	mulpd	xmm8, xmm2				; x2(s2+x2s3)
	mulpd	xmm9, xmm3				; x2(s2+x2s3)

	mulpd	xmm12, xmm10				; x6
	mulpd	xmm13, xmm11				; x6

	addpd	xmm4, OWORD PTR cossinarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm5, OWORD PTR sincosarray+30h		; s4+x2(s5+x2s6)
	addpd	xmm8, OWORD PTR cossinarray		; s1+x2(s2+x2s3)
	addpd	xmm9, OWORD PTR sincosarray		; s1+x2(s2+x2s3)

	movhlps	xmm11, xmm11				; move high x4 for cos term	+

	mulpd	xmm4, xmm12				; x6(s4+x2(s5+x2s6))
	mulpd	xmm5, xmm13				; x6(s4+x2(s5+x2s6))

	movhlps	xmm6, xmm2				; move low x2 for x3 for sin term
	movsd	xmm7, xmm3				; move low x2 for x3 for sin term   	+
	mulsd	xmm6, QWORD PTR p_temp2[rsp+8]		; get low x3 for sin term
	mulsd	xmm7, xmm1				; get low x3 for sin term		+

	mulpd 	xmm2, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms
	mulpd 	xmm3, QWORD PTR __real_3fe0000000000000	; 0.5*x2 for sin and cos terms

	addpd	xmm4,  xmm8				; z
	addpd	xmm5,  xmm9				; z

	movhlps	xmm12, xmm2				; move high 0.5*x2 for sin term
	movhlps	xmm13, xmm3				; move high r for cos

	movhlps	xmm8,  xmm4				; xmm8 = sin , xmm4 = cos
	movhlps	xmm9,  xmm5				; xmm9 = cos , xmm5 = sin

	mulsd	xmm8, xmm6				; sin *x3
	mulsd	xmm9, xmm11				; cos *x4
	mulsd	xmm4, xmm10				; cos *x4
	mulsd	xmm5, xmm7				; sin *x3

	mulsd	xmm12, QWORD PTR p_temp[rsp+8]		; 0.5 * x2 * xx for sin term
	mulsd	xmm3, QWORD PTR p_temp1[rsp]		; 0.5 * x2 * xx for sin term

	movsd	xmm6, xmm2				; Keep high r for cos term
	movsd	xmm7, xmm13				; Keep high r for cos term

	subsd	xmm2, QWORD PTR __real_3ff0000000000000; -t=r-1.0
	subsd	xmm13, QWORD PTR __real_3ff0000000000000; -t=r-1.0

	subsd	xmm8, xmm12				; sin - 0.5 * x2 *xx
	subsd	xmm5, xmm3				; sin - 0.5 * x2 *xx

	movhlps	xmm10, xmm0				; move high x for x for sin term
	movhlps	xmm11, xmm1				; move high x for x*xx for cos term

	mulsd	xmm0, QWORD PTR p_temp[rsp]		; x * xx
	mulsd	xmm11, QWORD PTR p_temp1[rsp+8]		; x * xx

	movsd	xmm12,  xmm2				; move -t for cos term
	movsd	xmm3,  xmm13				; move -t for cos term

	addsd	xmm2, QWORD PTR __real_3ff0000000000000 ; 1+(-t)
	addsd	xmm13, QWORD PTR __real_3ff0000000000000; 1+(-t)

	addsd	xmm8, QWORD PTR p_temp[rsp+8]		; sin+xx
	addsd	xmm5, QWORD PTR p_temp1[rsp]		; sin+xx

	subsd	xmm2, xmm6				; (1-t) - r
	subsd	xmm13, xmm7				; (1-t) - r

	subsd	xmm2, xmm0				; ((1 + (-t)) - r) - x*xx
	subsd	xmm13, xmm11				; ((1 + (-t)) - r) - x*xx


	addsd	xmm8, xmm10				; sin + x
	addsd	xmm5, xmm1				; sin + x

	addsd   xmm4, xmm2				; cos+((1-t)-r - x*xx)
	addsd   xmm9, xmm13				; cos+((1-t)-r - x*xx)

	subsd   xmm4, xmm12				; cos+t
	subsd   xmm9, xmm3				; cos+t

	movlhps	xmm4, xmm8
	movlhps	xmm5, xmm9
	jmp 	__vrda_cos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
coscos_sinsin_piby4:

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR sinarray+50h			; c6
	movdqa	xmm5, OWORD PTR cosarray+50h			; c6
	movapd	xmm8, OWORD PTR sinarray+20h			; c3
	movapd	xmm9, OWORD PTR cosarray+20h			; c3

	movapd	OWORD PTR p_temp2[rsp], xmm2			; store x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2
	movapd	OWORD PTR p_temp3[rsp], xmm11			; store r
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	mulpd	xmm10, xmm2					; x4
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0

	movapd	xmm12, xmm2					; copy of x2 for 0.5*x2
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4,  OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm5,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm8,  OWORD PTR sinarray+10h			; c2+x2C3
	addpd	xmm9,  OWORD PTR cosarray+10h			; c2+x2C3

	addpd   xmm11, OWORD PTR __real_3ff0000000000000	; 1 + (-t)

	mulpd	xmm10, xmm2					; x6
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, OWORD PTR __real_3fe0000000000000	; 0.5 *x2
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR sinarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR cosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm10					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zc

	mulpd	xmm2, xmm0					; x3 recalculate
	mulpd	xmm3, xmm3					; x4 recalculate

	movapd   xmm13, OWORD PTR p_temp3[rsp]			; r

	mulpd	xmm12, xmm6					; 0.5 * x2 *xx
	mulpd	xmm7, xmm1					; x * xx

	subpd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm3					; x4 * zc

	subpd	xmm4, xmm12					; -0.5 * x2 *xx
	subpd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	addpd	xmm4, xmm6					; x3 * zs +xx
	addpd   xmm5, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)

	subpd	xmm13, OWORD PTR __real_3ff0000000000000	; t relaculate, -t = r-1
	addpd	xmm4, xmm0					; +x
	subpd   xmm5, xmm13					; + t

	jmp __vrda_cos_cleanup

align 16
sinsin_coscos_piby4:

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR cosarray+50h			; c6
	movdqa	xmm5, OWORD PTR sinarray+50h			; c6
	movapd	xmm8, OWORD PTR cosarray+20h			; c3
	movapd	xmm9, OWORD PTR sinarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	movapd	OWORD PTR p_temp3[rsp], xmm3			; store x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2
	movapd	OWORD PTR p_temp2[rsp], xmm10			; store r
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	mulpd	xmm11, xmm3					; x4

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for 0.5*x2

	addpd	xmm4,  OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm5,  OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm8,  OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm9,  OWORD PTR sinarray+10h			; c2+x2C3

	addpd   xmm10, OWORD PTR __real_3ff0000000000000	; 1 + (-t)

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm11, xmm3					; x6

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, OWORD PTR __real_3fe0000000000000	; 0.5 *x2

	addpd	xmm4, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm11					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zs

	mulpd	xmm2, xmm2					; x4 recalculate
	mulpd	xmm3, xmm1					; x3 recalculate

	movapd   xmm12, OWORD PTR p_temp2[rsp]			; r

	mulpd	xmm6, xmm0					; x * xx
	mulpd	xmm13, xmm7					; 0.5 * x2 *xx
	subpd   xmm10, xmm12					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x4 * zc
	mulpd	xmm5, xmm3					; x3 * zs

	subpd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx;;;;;;;;;;;;;;;;;;;;;
	subpd	xmm5, xmm13					; -0.5 * x2 *xx
	addpd   xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addpd	xmm5, xmm7					; +xx
	subpd	xmm12, OWORD PTR __real_3ff0000000000000	; t relaculate, -t = r-1
	addpd	xmm5, xmm1					; +x
	subpd   xmm4, xmm12					; + t

	jmp __vrda_cos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
coscos_cossin_piby4:				;Derive from cossin_coscos
	movapd	xmm10, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR sincosarray+50h			; c6
	movdqa	xmm5, OWORD PTR cosarray+50h			; c6
	movapd	xmm8, OWORD PTR sincosarray+20h			; c3
	movapd	xmm9, OWORD PTR cosarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; r
	movapd	OWORD PTR p_temp3[rsp], xmm11			; r
	movapd	OWORD PTR p_temp[rsp], xmm6			; rr
	movhlps	xmm10, xmm10					; get upper r for t for cos

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subsd	xmm10, QWORD PTR __real_3ff0000000000000	; -t=r-1.0  for cos
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR sincosarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR sincosarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR cosarray+10h			; c2+x2C3

	addsd   xmm10, QWORD PTR __real_3ff0000000000000	; 1 + (-t)
	addpd   xmm11, OWORD PTR __real_3ff0000000000000	; 1 + (-t)

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12,xmm2					; x6
	mulpd	xmm13,xmm3					; x6

	addpd	xmm4, OWORD PTR sincosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR sincosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR cosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zc

	movsd	xmm8, xmm0					; lower x for sin
	mulsd	xmm8, xmm2					; lower x3 for sin

	mulpd	xmm2, xmm2					; x4
	mulpd	xmm3, xmm3					; upper x4 for cos
	movsd	xmm2, xmm8					; lower x3 for sin

	movsd	 xmm9,  xmm6					; lower xx
								; note using odd reg

	movlpd   xmm12, QWORD PTR p_temp2[rsp+8]		; upper r for cos term
	movapd   xmm13, OWORD PTR p_temp3[rsp]			; r

	mulpd	xmm6, xmm0					; x * xx for upper cos term
	mulpd	xmm7, xmm1					; x * xx
	movhlps	xmm6, xmm6
	mulsd	xmm9, QWORD PTR p_temp2[rsp] 			; xx * 0.5*x2 for sin term

	subsd   xmm10, xmm12					; (1 + (-t)) - r
	subpd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x4 * zc
	mulpd	xmm5, xmm3					; x4 * zc
								; x3 * zs

	movhlps	xmm8, xmm4					; xmm8= cos, xmm4= sin

	subsd	xmm4, xmm9					; x3zs - 0.5*x2*xx

	subsd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx
	subpd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	addsd   xmm8, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addpd   xmm5, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm4, QWORD PTR p_temp[rsp]			; +xx


	subsd	xmm12, QWORD PTR __real_3ff0000000000000	; -t = r-1
	subpd	xmm13, OWORD PTR __real_3ff0000000000000	; -t = r-1

	subsd   xmm8, xmm12					; + t
	addsd	xmm4, xmm0					; +x
	subpd   xmm5, xmm13					; + t

	movlhps	xmm4, xmm8

	jmp __vrda_cos_cleanup

align 16
coscos_sincos_piby4:		;Derive from sincos_coscos
	movapd	xmm10, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR cossinarray+50h			; c6
	movdqa	xmm5, OWORD PTR cosarray+50h			; c6
	movapd	xmm8, OWORD PTR cossinarray+20h			; c3
	movapd	xmm9, OWORD PTR cosarray+20h			; c3

	mulpd	xmm10,OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	mulpd	xmm11,OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; r
	movapd	OWORD PTR p_temp3[rsp], xmm11			; r
	movapd	OWORD PTR p_temp[rsp], xmm6			; rr

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subsd	xmm10, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos
	subpd	xmm11, OWORD PTR __real_3ff0000000000000	; -t=r-1.0

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR cossinarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR cossinarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR cosarray+10h			; c2+x2C3

	addsd   xmm10, QWORD PTR __real_3ff0000000000000	; 1 + (-t) for cos
	addpd   xmm11, OWORD PTR __real_3ff0000000000000	; 1 + (-t)

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)
	mulpd	xmm8,  xmm2					; x2(c2+x2C3)
	mulpd	xmm9,  xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR cossinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cossinarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR cosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zszc
	addpd	xmm5, xmm9					; z

	mulpd	xmm2, xmm0					; upper x3 for sin
	mulsd	xmm2, xmm0					; lower x4 for cos
	mulpd	xmm3, xmm3					; x4

	movhlps	xmm9,  xmm6					; upper xx for sin term
								; note using odd reg

	movlpd  xmm12, QWORD PTR p_temp2[rsp]			; lower r for cos term
	movapd  xmm13, OWORD PTR p_temp3[rsp]			; r


	mulpd	xmm6, xmm0					; x * xx for lower cos term
	mulpd	xmm7, xmm1					; x * xx

	mulsd	xmm9, QWORD PTR p_temp2[rsp+8] 			; xx * 0.5*x2 for upper sin term

	subsd   xmm10, xmm12					; (1 + (-t)) - r
	subpd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; lower=x4 * zc
								; upper=x3 * zs
	mulpd	xmm5, xmm3
								; x4 * zc

	movhlps	xmm8, xmm4					; xmm8= sin, xmm4= cos
	subsd	xmm8, xmm9					; x3zs - 0.5*x2*xx


	subsd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx
	subpd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	addsd   xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addpd   xmm5, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm8, QWORD PTR p_temp[rsp+8]			; +xx

	movhlps	xmm0, xmm0					; upper x for sin
	subsd	xmm12, QWORD PTR __real_3ff0000000000000	; -t = r-1
	subpd	xmm13, OWORD PTR __real_3ff0000000000000	; -t = r-1

	subsd   xmm4, xmm12					; + t
	subpd   xmm5, xmm13					; + t
	addsd	xmm8, xmm0					; +x

	movlhps	xmm4, xmm8

	jmp __vrda_cos_cleanup

align 16
cossin_coscos_piby4:
	movapd	xmm10, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR cosarray+50h			; c6
	movdqa	xmm5, OWORD PTR sincosarray+50h			; c6
	movapd	xmm8, OWORD PTR cosarray+20h			; c3
	movapd	xmm9, OWORD PTR sincosarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; r
	movapd	OWORD PTR p_temp3[rsp], xmm11			; r
	movapd	OWORD PTR p_temp1[rsp], xmm7			; rr
	movhlps	xmm11, xmm11					; get upper r for t for cos

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	subsd	xmm11, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR sincosarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR sincosarray+10h			; c2+x2C3

	addpd   xmm10, OWORD PTR __real_3ff0000000000000		; 1 + (-t)	;trash t
	addsd   xmm11, QWORD PTR __real_3ff0000000000000		; 1 + (-t)	;trash t

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sincosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sincosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zcs

;;;
	movsd	xmm9, xmm1					; lower x for sin
	mulsd	xmm9, xmm3					; lower x3 for sin

	mulpd	xmm2, xmm2					; x4
	mulpd	xmm3, xmm3					; upper x4 for cos
	movsd	xmm3, xmm9					; lower x3 for sin

	movsd	 xmm8,  xmm7					; lower xx
								; note using even reg

	movapd   xmm12, OWORD PTR p_temp2[rsp]			; r
	movlpd   xmm13, QWORD PTR p_temp3[rsp+8]		; upper r for cos term

	mulpd	xmm6, xmm0					; x * xx
	mulpd	xmm7, xmm1					; x * xx for upper cos term
	movhlps	xmm7, xmm7
	mulsd	xmm8, QWORD PTR p_temp3[rsp] 			; xx * 0.5*x2 for sin term

	subpd   xmm10, xmm12					; (1 + (-t)) - r
	subsd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x4 * zc
	mulpd	xmm5, xmm3					; x4 * zc
								; x3 * zs

	movhlps	xmm9, xmm5					; xmm9= cos, xmm5= sin

	subsd	xmm5, xmm8					; x3zs - 0.5*x2*xx

	subpd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx
	subsd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	addpd   xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd   xmm9, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm5, QWORD PTR p_temp1[rsp]			; +xx


	subpd	xmm12, OWORD PTR __real_3ff0000000000000		; t relaculate, -t = r-1
	subsd	xmm13, QWORD PTR __real_3ff0000000000000		; t relaculate, -t = r-1

	subpd   xmm4, xmm12					; + t
	subsd   xmm9, xmm13					; + t
	addsd	xmm5, xmm1					; +x

	movlhps	xmm5, xmm9

	jmp __vrda_cos_cleanup

align 16
cossin_sinsin_piby4:		; Derived from sincos_sinsin
	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR sinarray+50h			; c6
	movdqa	xmm5, OWORD PTR sincosarray+50h			; c6
	movapd	xmm8, OWORD PTR sinarray+20h			; c3
	movapd	xmm9, OWORD PTR sincosarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp3[rsp], xmm11			; r
	movapd	OWORD PTR p_temp1[rsp], xmm7			; rr

	movhlps	xmm11, xmm11
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subsd	xmm11, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR sincosarray+40h		; c5+x2c6
	addpd	xmm8, OWORD PTR sinarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR sincosarray+10h			; c2+x2C3

	mulpd	xmm10, xmm6					; 0.5*x2*xx
	addsd   xmm11, QWORD PTR __real_3ff0000000000000	; 1 + (-t) for cos

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sincosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR sinarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sincosarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zczs

	movsd	xmm12, xmm3
	mulsd	xmm12, xmm1					; low x3 for sin

	mulpd	xmm2, xmm0					; x3
	mulpd	xmm3, xmm3					; high x4 for cos
	movsd	xmm3, xmm12					; low x3 for sin

	movhlps	xmm8,  xmm1					; upper x for cos term
								; note using even reg
	movlpd  xmm13, QWORD PTR p_temp3[rsp+8]			; upper r for cos term

	mulsd	xmm8, QWORD PTR p_temp1[rsp+8]			; x * xx for upper cos term

	mulsd	xmm7, QWORD PTR p_temp3[rsp] 			; xx * 0.5*x2 for lower sin term

	subsd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm3					; lower=x4 * zc
								; upper=x3 * zs

	movhlps	xmm9, xmm5					; xmm9= cos, xmm5= sin

	subsd	xmm5, xmm7					; x3zs - 0.5*x2*xx

	subsd   xmm11, xmm8					; ((1 + (-t)) - r) - x*xx

	subpd	xmm4, xmm10					; x3*zs - 0.5*x2*xx
	addsd   xmm9, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm5, QWORD PTR p_temp1[rsp]			; +xx

	addpd	xmm4,  xmm6					; +xx
	subsd	xmm13, QWORD PTR __real_3ff0000000000000	; -t = r-1


	addsd	xmm5, xmm1					; +x
	addpd	xmm4, xmm0					; +x
	subsd   xmm9, xmm13					; + t

	movlhps	xmm5, xmm9

	jmp __vrda_cos_cleanup

align 16
sincos_coscos_piby4:
	movapd	xmm10, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR cosarray+50h			; c6
	movdqa	xmm5, OWORD PTR cossinarray+50h			; c6
	movapd	xmm8, OWORD PTR cosarray+20h			; c3
	movapd	xmm9, OWORD PTR cossinarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; r
	movapd	OWORD PTR p_temp3[rsp], xmm11			; r
	movapd	OWORD PTR p_temp1[rsp], xmm7			; rr

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subpd	xmm10, OWORD PTR __real_3ff0000000000000	; -t=r-1.0
	subsd	xmm11, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR cosarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR cossinarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR cosarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR cossinarray+10h			; c2+x2C3

	addpd   xmm10, OWORD PTR __real_3ff0000000000000	; 1 + (-t)
	addsd   xmm11, QWORD PTR __real_3ff0000000000000	; 1 + (-t) for cos

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR cosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR cossinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR cossinarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zc
	addpd	xmm5, xmm9					; zszc

	mulpd	xmm2, xmm2					; x4
	mulpd	xmm3, xmm1					; upper x3 for sin
	mulsd	xmm3, xmm1					; lower x4 for cos

	movhlps	xmm8,  xmm7					; upper xx for sin term
								; note using even reg

	movapd  xmm12, OWORD PTR p_temp2[rsp]			; r
	movlpd  xmm13, QWORD PTR p_temp3[rsp]			; lower r for cos term

	mulpd	xmm6, xmm0					; x * xx
	mulpd	xmm7, xmm1					; x * xx for lower cos term

	mulsd	xmm8, QWORD PTR p_temp3[rsp+8] 			; xx * 0.5*x2 for upper sin term

	subpd   xmm10, xmm12					; (1 + (-t)) - r
	subsd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x4 * zc
	mulpd	xmm5, xmm3					; lower=x4 * zc
								; upper=x3 * zs

	movhlps	xmm9, xmm5					; xmm9= sin, xmm5= cos

	subsd	xmm9, xmm8					; x3zs - 0.5*x2*xx

	subpd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx
	subsd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	addpd   xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd   xmm5, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm9, QWORD PTR p_temp1[rsp+8]			; +xx

	movhlps	xmm1, xmm1					; upper x for sin
	subpd	xmm12, OWORD PTR __real_3ff0000000000000	; -t = r-1
	subsd	xmm13, QWORD PTR __real_3ff0000000000000	; -t = r-1

	subpd   xmm4, xmm12					; + t
	subsd   xmm5, xmm13					; + t
	addsd	xmm9, xmm1					; +x

	movlhps	xmm5, xmm9

	jmp __vrda_cos_cleanup


align 16
sincos_sinsin_piby4:		; Derived from sincos_coscos
	movapd	xmm10, xmm2					; r
	movapd	xmm11, xmm3					; r

	movdqa	xmm4, OWORD PTR sinarray+50h			; c6
	movdqa	xmm5, OWORD PTR cossinarray+50h			; c6
	movapd	xmm8, OWORD PTR sinarray+20h			; c3
	movapd	xmm9, OWORD PTR cossinarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp3[rsp], xmm11			; r
	movapd	OWORD PTR p_temp1[rsp], xmm7			; rr

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subsd	xmm11, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR cossinarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR sinarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR cossinarray+10h			; c2+x2C3

	mulpd	xmm10, xmm6					; 0.5x2*xx
	addsd   xmm11, QWORD PTR __real_3ff0000000000000	; 1 + (-t) for cos

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR cossinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR sinarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR cossinarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zszc

	mulpd	xmm2, xmm0					; x3
	mulpd	xmm3, xmm1					; upper x3 for sin
	mulsd	xmm3, xmm1					; lower x4 for cos

	movhlps	xmm8,  xmm7					; upper xx for sin term
								; note using even reg

	movlpd  xmm13, QWORD PTR p_temp3[rsp]			; lower r for cos term

	mulpd	xmm7, xmm1					; x * xx for lower cos term

	mulsd	xmm8, QWORD PTR p_temp3[rsp+8] 			; xx * 0.5*x2 for upper sin term

	subsd   xmm11, xmm13					; (1 + (-t)) - r

	mulpd	xmm4, xmm2					; x3 * zs
	mulpd	xmm5, xmm3					; lower=x4 * zc
								; upper=x3 * zs

	movhlps	xmm9, xmm5					; xmm9= sin, xmm5= cos

	subsd	xmm9, xmm8					; x3zs - 0.5*x2*xx

	subsd   xmm11, xmm7					; ((1 + (-t)) - r) - x*xx

	subpd	xmm4, xmm10					; x3*zs - 0.5*x2*xx
	addsd   xmm5, xmm11					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm9, QWORD PTR p_temp1[rsp+8]			; +xx

	movhlps	xmm1,  xmm1					; upper x for sin
	addpd	xmm4,  xmm6					; +xx
	subsd	xmm13, QWORD PTR __real_3ff0000000000000	; -t = r-1

	addsd	xmm9, xmm1					; +x
	addpd	xmm4, xmm0					; +x
	subsd   xmm5, xmm13					; + t

	movlhps	xmm5, xmm9

	jmp __vrda_cos_cleanup


align 16
sinsin_cossin_piby4:		; Derived from sincos_sinsin
	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR sincosarray+50h			; c6
	movdqa	xmm5, OWORD PTR sinarray+50h			; c6
	movapd	xmm8, OWORD PTR sincosarray+20h			; c3
	movapd	xmm9, OWORD PTR sinarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000	; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; x2
	movapd	OWORD PTR p_temp[rsp], xmm6			; xx

	movhlps	xmm10, xmm10
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subsd	xmm10, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR sincosarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR sincosarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR sinarray+10h			; c2+x2C3

	mulpd	xmm11, xmm7					; 0.5*x2*xx
	addsd   xmm10, QWORD PTR __real_3ff0000000000000	; 1 + (-t) for cos

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR sincosarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR sincosarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zczs
	addpd	xmm5, xmm9					; zs


	movsd	xmm13, xmm2
	mulsd	xmm13, xmm0					; low x3 for sin

	mulpd	xmm3, xmm1					; x3
	mulpd	xmm2, xmm2					; high x4 for cos
	movsd	xmm2, xmm13					; low x3 for sin


	movhlps	xmm9,  xmm0					; upper x for cos term								; note using even reg
	movlpd  xmm12, QWORD PTR p_temp2[rsp+8]			; upper r for cos term
	mulsd	xmm9, QWORD PTR p_temp[rsp+8]			; x * xx for upper cos term
	mulsd	xmm6, QWORD PTR p_temp2[rsp] 			; xx * 0.5*x2 for lower sin term
	subsd   xmm10, xmm12					; (1 + (-t)) - r
	mulpd	xmm5, xmm3					; x3 * zs
	mulpd	xmm4, xmm2					; lower=x4 * zc
								; upper=x3 * zs

	movhlps	xmm8, xmm4					; xmm8= cos, xmm4= sin
	subsd	xmm4, xmm6					; x3zs - 0.5*x2*xx

	subsd   xmm10, xmm9					; ((1 + (-t)) - r) - x*xx

	subpd	xmm5, xmm11					; x3*zs - 0.5*x2*xx

	addsd   xmm8, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm4, QWORD PTR p_temp[rsp]			; +xx

	addpd	xmm5,  xmm7					; +xx
	subsd	xmm12, QWORD PTR __real_3ff0000000000000	; -t = r-1

	addsd	xmm4, xmm0					; +x
	addpd	xmm5, xmm1					; +x
	subsd   xmm8, xmm12					; + t
	movlhps	xmm4, xmm8

	jmp __vrda_cos_cleanup

align 16
sinsin_sincos_piby4:		; Derived from sincos_coscos

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR cossinarray+50h			; c6
	movdqa	xmm5, OWORD PTR sinarray+50h			; c6
	movapd	xmm8, OWORD PTR cossinarray+20h			; c3
	movapd	xmm9, OWORD PTR sinarray+20h			; c3

	mulpd	xmm10, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2
	mulpd	xmm11, OWORD PTR __real_3fe0000000000000		; r = 0.5 *x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2

	movapd	OWORD PTR p_temp2[rsp], xmm10			; r
	movapd	OWORD PTR p_temp[rsp], xmm6			; rr

	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	subsd	xmm10, QWORD PTR __real_3ff0000000000000	; -t=r-1.0 for cos

	movapd	xmm12, xmm2					; copy of x2 for x4
	movapd	xmm13, xmm3					; copy of x2 for x4

	addpd	xmm4, OWORD PTR cossinarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR cossinarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR sinarray+10h			; c2+x2C3

	mulpd	xmm11, xmm7					; 0.5x2*xx
	addsd   xmm10, QWORD PTR __real_3ff0000000000000	; 1 + (-t) for cos

	mulpd	xmm12, xmm2					; x4
	mulpd	xmm13, xmm3					; x4

	mulpd	xmm4,  xmm2					; x2(c5+x2c6)
	mulpd	xmm5,  xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm12, xmm2					; x6
	mulpd	xmm13, xmm3					; x6

	addpd	xmm4, OWORD PTR cossinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR cossinarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	mulpd	xmm4, xmm12					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm13					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zszc

	mulpd	xmm3, xmm1					; x3
	mulpd	xmm2, xmm0					; upper x3 for sin
	mulsd	xmm2, xmm0					; lower x4 for cos

	movhlps	xmm9,  xmm6					; upper xx for sin term
								; note using even reg

	movlpd  xmm12, QWORD PTR p_temp2[rsp]			; lower r for cos term

	mulpd	xmm6, xmm0					; x * xx for lower cos term

	mulsd	xmm9, QWORD PTR p_temp2[rsp+8] 			; xx * 0.5*x2 for upper sin term

	subsd   xmm10, xmm12					; (1 + (-t)) - r

	mulpd	xmm5, xmm3					; x3 * zs
	mulpd	xmm4, xmm2					; lower=x4 * zc
								; upper=x3 * zs

	movhlps	xmm8, xmm4					; xmm9= sin, xmm5= cos

	subsd	xmm8, xmm9					; x3zs - 0.5*x2*xx

	subsd   xmm10, xmm6					; ((1 + (-t)) - r) - x*xx

	subpd	xmm5, xmm11					; x3*zs - 0.5*x2*xx
	addsd   xmm4, xmm10					; x4*zc + (((1 + (-t)) - r) - x*xx)
	addsd	xmm8, QWORD PTR p_temp[rsp+8]			; +xx

	movhlps	xmm0,  xmm0					; upper x for sin
	addpd	xmm5,  xmm7					; +xx
	subsd	xmm12, QWORD PTR __real_3ff0000000000000	; -t = r-1


	addsd	xmm8, xmm0					; +x
	addpd	xmm5, xmm1					; +x
	subsd   xmm4, xmm12					; + t

	movlhps	xmm4, xmm8

	jmp __vrda_cos_cleanup


align 16
sinsin_sinsin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; p_sign0  = Sign, xmm0 = r, xmm2 = r2, xmm6 =rr
; p_sign1  = Sign, xmm1 = r, xmm3 = r2, xmm7 =rr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	xmm10, xmm2					; x2
	movapd	xmm11, xmm3					; x2

	movdqa	xmm4, OWORD PTR sinarray+50h			; c6
	movdqa	xmm5, OWORD PTR sinarray+50h			; c6
	movapd	xmm8, OWORD PTR sinarray+20h			; c3
	movapd	xmm9, OWORD PTR sinarray+20h			; c3

	movapd	OWORD PTR p_temp2[rsp], xmm2			; copy of x2
	movapd	OWORD PTR p_temp3[rsp], xmm3			; copy of x2

	mulpd	xmm4,  xmm2					; c6*x2
	mulpd	xmm5,  xmm3					; c6*x2
	mulpd	xmm8,  xmm2					; c3*x2
	mulpd	xmm9,  xmm3					; c3*x2

	mulpd	xmm10, xmm2					; x4
	mulpd	xmm11, xmm3					; x4

	addpd	xmm4, OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm5, OWORD PTR sinarray+40h			; c5+x2c6
	addpd	xmm8, OWORD PTR sinarray+10h			; c2+x2C3
	addpd	xmm9, OWORD PTR sinarray+10h			; c2+x2C3

	mulpd	xmm10, xmm2					; x6
	mulpd	xmm11, xmm3					; x6

	mulpd	xmm4, xmm2					; x2(c5+x2c6)
	mulpd	xmm5, xmm3					; x2(c5+x2c6)
	mulpd	xmm8, xmm2					; x2(c2+x2C3)
	mulpd	xmm9, xmm3					; x2(c2+x2C3)

	mulpd	xmm2, OWORD PTR __real_3fe0000000000000		; 0.5 *x2
	mulpd	xmm3, OWORD PTR __real_3fe0000000000000		; 0.5 *x2

	addpd	xmm4, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm5, OWORD PTR sinarray+30h			; c4 + x2(c5+x2c6)
	addpd	xmm8, OWORD PTR sinarray			; c1 + x2(c2+x2C3)
	addpd	xmm9, OWORD PTR sinarray			; c1 + x2(c2+x2C3)

	mulpd	xmm2, xmm6					; 0.5 * x2 *xx
	mulpd	xmm3, xmm7					; 0.5 * x2 *xx

	mulpd	xmm4, xmm10					; x6(c4 + x2(c5+x2c6))
	mulpd	xmm5, xmm11					; x6(c4 + x2(c5+x2c6))

	addpd	xmm4, xmm8					; zs
	addpd	xmm5, xmm9					; zs

	movapd	xmm10, OWORD PTR p_temp2[rsp]			; x2
	movapd	xmm11, OWORD PTR p_temp3[rsp]			; x2

	mulpd	xmm10, xmm0					; x3
	mulpd	xmm11, xmm1					; x3

	mulpd	xmm4, xmm10					; x3 * zs
	mulpd	xmm5, xmm11					; x3 * zs

	subpd	xmm4, xmm2					; -0.5 * x2 *xx
	subpd	xmm5, xmm3					; -0.5 * x2 *xx

	addpd	xmm4, xmm6					; +xx
	addpd	xmm5, xmm7					; +xx

	addpd	xmm4, xmm0					; +x
	addpd	xmm5, xmm1					; +x

	jmp __vrda_cos_cleanup

END
