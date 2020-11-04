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
; vrsapowxf.asm
;
; An array implementation of the powf libm function.
; This routine raises the x array to a constant y power.
;
; Prototype:
;
;     void vrsa_powxf(int n, float *x, float y, float *z);
;
;   Places the results into the supplied z array.
; Does not perform error handling, but does return C99 values for error
; inputs.   Denormal results are truncated to 0.
;
;

CONST	SEGMENT

__mask_sign				DQ 08000000080000000h	; a sign bit mask
						dq 08000000080000000h

__mask_nsign			DQ 07FFFFFFF7FFFFFFFh	; a not sign bit mask
						dq 07FFFFFFF7FFFFFFFh

; used by inty
__mask_127				DQ 00000007F0000007Fh	; EXPBIAS_SP32
						dq 00000007F0000007Fh

__mask_mant				DQ 0007FFFFF007FFFFFh	; mantissa bit mask
						dq 0007FFFFF007FFFFFh

__mask_1				DQ 00000000100000001h	; 1
						dq 00000000100000001h

__mask_2				DQ 00000000200000002h	; 2
						dq 00000000200000002h

__mask_24				DQ 00000001800000018h	; 24
						dq 00000001800000018h

__mask_23				DQ 00000001700000017h	; 23
						dq 00000001700000017h

; used by special case checking

__float_one				DQ 03f8000003f800000h	; one
						dq 03f8000003f800000h

__mask_inf				DQ 07f8000007F800000h	; inifinity
						dq 07f8000007F800000h

__mask_ninf				DQ 0ff800000fF800000h	; -inifinity
						dq 0ff800000fF800000h

__mask_NaN				DQ 07fC000007FC00000h	; NaN
						dq 07fC000007FC00000h

__mask_sigbit			DQ 00040000000400000h	; QNaN bit
						dq 00040000000400000h

__mask_impbit			DQ 00080000000800000h	; implicit bit
						dq 00080000000800000h

__mask_ly				DQ 04f0000004f000000h	; large y
						dq 04f0000004f000000h


CONST	ENDS


; define local variable storage offsets
p_temp	  		equ		00h		; xmmword
p_negateres		equ		10h		; qword

p_xexp	  		equ		20h		; qword

save_rbx		equ		030h		;qword
save_rsi		equ		038h		;qword
save_rdi		equ		040h		;qword

p_y		  		equ		048h		; y value

p_ax	  		equ		050h		; absolute x
p_sx	  		equ		060h		; sign of x's

p_ay	  		equ		070h		; absolute y
p_yexp	  		equ		080h		; unbiased exponent of y

p_inty  		equ		090h		; integer y indicator

p_xptr	  		equ		0a0h		; ptr to x values
p_zptr	  		equ		0b0h		; ptr to z values

p_nv			equ		0b8h		;qword
p_iter			equ		0c0h		; qword	storage for number of loop iterations

p2_temp			equ		0d0h		;qword
p2_temp1		equ		0f0h		;qword

stack_size		equ		0118h	; allocate 40h more than
								; we need to avoid bank conflicts

EXTRN	__amd_internal_vrd4_exp:NEAR
EXTRN	__amd_internal_vrd4_log:NEAR


include fm.inc

FN_PROTOTYPE  vrsa_powxf


TEXT	SEGMENT	   page	   'CODE'

; parameters are passed in by Microsoft C as:
; ecx - int    n
; rdx - float *x
; xmm2 - float y,   r8 - float * y
; r9  - float *z

	PUBLIC  fname
fname	proc	frame
	sub		rsp,stack_size
	.ALLOCSTACK	stack_size
;	movd	edx,xmm0						; edx is ux
	mov		QWORD PTR [rsp+save_rbx],rbx	; save rbx
	.SAVEREG rbx, save_rbx
	mov		QWORD PTR [rsp+save_rsi],rsi	; save rsi
	.SAVEREG rsi, save_rsi
	mov		QWORD PTR [rsp+save_rdi],rdi	; save rdi
	.SAVEREG rdi, save_rdi
		.ENDPROLOG

	movss	DWORD PTR p_y[rsp],xmm2		; save y
	mov		QWORD PTR p_xptr[rsp],rdx		; save pointer to x
	mov		QWORD PTR p_zptr[rsp],r9		; save pointer to z

	xor		rax,rax
	cmp		rcx,0						; just return if count is zero
	jz		__final_check
	mov		eax,ecx
	mov     rcx,rax
	mov		QWORD PTR [rsp+p_nv],rcx	; save number of values

;
; classify y
; vector 32 bit integer method
;  /* See whether y is an integer.
;     inty = 0 means not an integer.
;     inty = 1 means odd integer.
;     inty = 2 means even integer.
;  */
;	movdqa  xmm4,XMMWORD PTR [rdx]
; get yexp
	mov		r8d,DWORD PTR p_y[rsp]						; r8 is uy
	mov		r9d,07fffffffh
	and		r9d,r8d						; r9 is ay

; if |y| == 0	then return 1
	cmp		r9d,0							; is y a zero?
	jz		y_zero

	mov		eax,07f800000h				; EXPBITS_SP32
	and		eax,r9d						; y exp

	xor		edi,edi
	shr		eax,23			;>> EXPSHIFTBITS_SP32
	sub		eax,126		; - EXPBIAS_SP32 + 1   - eax is now the unbiased exponent
	mov		ebx,1
	cmp		eax,ebx			; if (yexp < 1)
	cmovl	ebx,edi
	jl		save_inty

	mov		ecx,24
	cmp		eax,ecx			; if (yexp >24)
	jle		@F
	mov		ebx,2
	jmp		save_inty
@@:							; else 1<=yexp<=24
	sub		ecx,eax			; build mask for mantissa
	shl		ebx,cl
	dec		ebx				; rbx = mask = (1 << (24 - yexp)) - 1

	mov		eax,r8d	
	and		eax,ebx			; if ((uy & mask) != 0)
	cmovnz	ebx,edi			;   inty = 0;
	jnz		save_inty

	not		ebx				; else if (((uy & ~mask) >> (24 - yexp)) & 0x00000001)
	mov		eax,r8d	
	and		eax,ebx
	shr		eax,cl
	inc		edi			
	and		eax,edi
	mov		ebx,edi			;  inty = 1
	jnz		save_inty
	inc		ebx				; else	inty = 2		


save_inty:
	mov		DWORD PTR p_y[rsp+4],r8d		; save an extra copy of y
	mov		DWORD PTR p_inty[rsp],ebx		; save inty

	mov		rax,QWORD PTR [rsp+p_nv]	; get number of values
	mov     rcx,rax
; see if too few values to call the main loop
	shr		rax,2						; get number of iterations
	jz		__vsa_cleanup				; jump if only single calls
; prepare the iteration counts
	mov		QWORD PTR [rsp+p_iter],rax	; save number of iterations
	shl		rax,2
	sub		rcx,rax						; compute number of extra single calls
	mov		QWORD PTR [rsp+p_nv],rcx	; save number of left over values

; process the array 4 values at a time.

__vsa_top:
; build the input _m128
; first get x
	mov		rsi,QWORD PTR [rsp+p_xptr]	; get x_array pointer
	movups	xmm0,XMMWORD PTR [rsi]
	prefetch	QWORD PTR [rsi+64]


	movaps	xmm2,xmm0
	andps	xmm0,XMMWORD PTR __mask_nsign		; get abs x
	andps	xmm2,XMMWORD PTR __mask_sign			; mask for the sign bits
	movaps	XMMWORD PTR p_ax[rsp],xmm0		; save them
	movaps	XMMWORD PTR p_sx[rsp],xmm2		; save them
; convert all four x's to double	
	cvtps2pd   xmm0,QWORD PTR p_ax[rsp]
	cvtps2pd   xmm1,QWORD PTR p_ax+8[rsp]
;
; do x special case checking
;
;	movdqa	xmm5,xmm4
;	pcmpeqd	xmm5,xmm3						; is y not an integer? ff's if so
;	pand	xmm5,XMMWORD PTR __mask_NaN		; these values will be NaNs, if x<0
	pxor	xmm3,xmm3
	xor		eax,eax
	mov		ecx,07FC00000h
	cmp		ebx,0							; is y not an integer?
	cmovz	eax,ecx							; then set to return a NaN.  else 0.
	mov		ecx,080000000h
	cmp		ebx,1							; is y an odd integer?
	cmovz	eax,ecx							; maybe set sign bit if so
	movd	xmm5,eax
	pshufd	xmm5,xmm5,0
;	shufps	xmm5,xmm5,0
;	movdqa	xmm2,xmm4
;	pcmpeqd	xmm2,XMMWORD PTR __mask_1		; is it odd? ff's if so
;	pand	xmm2,XMMWORD PTR __mask_sign	; these values might get their sign bit set
;	por		xmm5,xmm2

;	cmpps	xmm3,XMMWORD PTR p_sx[rsp],0	; if the signs are set
	pcmpeqd	xmm3,XMMWORD PTR p_sx[rsp]		; if the signs are set
	pandn	xmm3,xmm5						; then negateres gets the values as shown below
	movdqa	XMMWORD PTR p_negateres[rsp],xmm3	; save negateres

;  /* p_negateres now means the following.
;     7FC00000 means x<0, y not an integer, return NaN.
;     80000000 means x<0, y is odd integer, so set the sign bit.
;     0 means even integer, and/or x>=0.
;  */

; **** Here starts the main calculations  ****
; The algorithm used is x**y = exp(y*log(x))
;  Extra precision is required in intermediate steps to meet the 1ulp requirement
;
; log(x) calculation
	call		__amd_internal_vrd4_log					; get the double precision log value 
											; for all four x's
; y* logx
	cvtps2pd   xmm2,QWORD PTR p_y[rsp]		;convert the two packed single y's to double

;  /* just multiply by y */
	mulpd	xmm0,xmm2
	mulpd	xmm1,xmm2

;  /* The following code computes r = exp(w) */
	call		__amd_internal_vrd4_exp					; get the double exp value 
											; for all four y*log(x)'s
;
; convert all four results to double	
	cvtpd2ps	xmm0,xmm0
	cvtpd2ps	xmm1,xmm1
	movlhps		xmm0,xmm1

; perform special case and error checking on input values

; special case checking is done first in the scalar version since
; it allows for early fast returns.  But for vectors, we consider them
; to be rare, so early returns are not necessary.  So we first compute
; the x**y values, and then check for special cases.

; we do some of the checking in reverse order of the scalar version.
; apply the negate result flags
	orps	xmm0,XMMWORD PTR p_negateres[rsp]	; get negateres

; if y is infinite or so large that the result would overflow or underflow
	mov		edx,DWORD PTR p_y[rsp]			; get y
	and 	edx,07fffffffh					; develop ay
;	mov		eax,04f000000h
	cmp		edx,04f000000h
	ja		y_large
rnsx3:

; if x is infinite 
	movdqa	xmm4,XMMWORD PTR p_ax[rsp]
	cmpps	xmm4,XMMWORD PTR __mask_inf,0	; equal to infinity, ffs if so.
	movmskps edx,xmm4
	test	edx,0fh
	jnz		x_infinite
rnsx1:
; if x is zero
	xorps	xmm4,xmm4 
	cmpps	xmm4,XMMWORD PTR p_ax[rsp],0	; equal to zero, ffs if so.
	movmskps edx,xmm4
	test	edx,0fh
	jnz		x_zero
rnsx2:
; if y is NAN 
	movss	xmm4,DWORD PTR p_y[rsp]			; get y
	ucomiss	xmm4,xmm4						; comparing y to itself should
											; be true, unless y is a NaN. parity flag if NaN.
	jp		y_NaN
rnsx4:
; if x is NAN 
	movdqa	xmm4,XMMWORD PTR p_ax[rsp]			; get x
	cmpps	xmm4,xmm4,4						; a compare not equal  of x to itself should
											; be false, unless x is a NaN. ff's if NaN.
	movmskps ecx,xmm4
	test	ecx,0fh
	jnz		x_NaN
rnsx5:

; if x == +1, return +1 for all x
	movdqa	xmm3,XMMWORD PTR __float_one	; one
	mov		rdx,QWORD PTR p_xptr[rsp]		; get pointer to x
	movdqa	xmm2,xmm3
	movdqu	xmm5,XMMWORD PTR [rdx]
	cmpps	xmm2,xmm5,4			; not equal to +1.0?, ffs if not equal.
	andps	xmm0,xmm2						; keep the others
	andnps	xmm2,xmm3						; mask for ones
	orps	xmm0,xmm2

__vsa_bottom:

; update the x and y pointers
	add		rsi,16
	mov		QWORD PTR [rsp+p_xptr],rsi	; save x_array pointer
; store the result _m128d
	mov		rdi,QWORD PTR [rsp+p_zptr]	; get z_array pointer
	movups	XMMWORD PTR [rdi],xmm0
;	prefetchw	QWORD PTR [rdi+64]
	prefetch	QWORD PTR [rdi+64]
	add		rdi,16
	mov		QWORD PTR [rsp+p_zptr],rdi	; save z_array pointer


	mov		rax,QWORD PTR [rsp+p_iter]	; get number of iterations
	sub		rax,1
	mov		QWORD PTR [rsp+p_iter],rax	; save number of iterations
	jnz		__vsa_top


; see if we need to do any extras
	mov		rax,QWORD PTR [rsp+p_nv]	; get number of values
	test	rax,rax
	jnz		__vsa_cleanup

__final_check:

	mov		rbx,[rsp+save_rbx]		; restore rbx
	mov		rsi,[rsp+save_rsi]		; restore rsi
	mov		rdi,[rsp+save_rdi]		; restore rdi
	add		rsp,stack_size
	ret

	align 16
; we jump here when we have an odd number of calls to make at the
; end
__vsa_cleanup:
	mov		rsi,QWORD PTR [rsp+p_xptr]
	mov		r8d,DWORD PTR p_y[rsp]						; r8 is uy

; fill in a m128 with zeroes and the extra values and then make a recursive call.
	xorps		xmm0,xmm0
	xor		rax,rax
	movaps	XMMWORD PTR p2_temp[rsp],xmm0
	movaps	XMMWORD PTR p2_temp+16[rsp],xmm0

	mov		rax,p_nv[rsp]	; get number of values
	mov		ecx,[rsi]			; we know there's at least one
	mov	 	p2_temp[rsp],ecx
	mov	 	p2_temp+16[rsp],r8d
	cmp		rax,2
	jl		__vsacg
	
	mov		ecx,4[rsi]			; do the second value
	mov	 	p2_temp+4[rsp],ecx
	mov	 	p2_temp+20[rsp],r8d
	cmp		rax,3
	jl		__vsacg

	mov		ecx,8[rsi]			; do the third value
	mov	 	p2_temp+8[rsp],ecx
	mov	 	p2_temp+24[rsp],r8d

__vsacg:
	mov		rcx,4				; parameter for N
	lea		rdx,p2_temp[rsp]	; &x parameter
	movaps	xmm0,p2_temp+16[rsp]	; y parameter
	lea		r9,p2_temp1[rsp]	; &z parameter
	call	fname				; call recursively to compute four values

; now copy the results to the destination array
	mov		rdi,p_zptr[rsp]
	mov		rax,p_nv[rsp]	; get number of values
	mov	 	ecx,p2_temp1[rsp]
	mov		[rdi],ecx			; we know there's at least one
	cmp		rax,2
	jl		__vsacgf
	
	mov	 	ecx,p2_temp1+4[rsp]
	mov		4[rdi],ecx			; do the second value
	cmp		rax,3
	jl		__vsacgf

	mov	 	ecx,p2_temp1+8[rsp]
	mov		8[rdi],ecx			; do the third value

__vsacgf:
	jmp		__final_check


	align 16
y_zero:
; if |y| == 0	then return 1
	mov		ecx,03f800000h	; one
; fill all results with a one
	mov		r9,QWORD PTR p_zptr[rsp]	; &z parameter
	mov		rax,QWORD PTR [rsp+p_nv]	; get number of values
__yzt:
	mov		DWORD PTR [r9], ecx			; store a 1
	add		r9,4
	sub		rax,1
	test	rax,rax
	jnz		__yzt
	jmp		__final_check
;      /* y is a NaN.
y_NaN:
	mov		r8d,DWORD PTR p_y[rsp] 
	or		r8d,000400000h				; convert to QNaNs
	movd	xmm0,r8d					; propagate to all results
	shufps	xmm0,xmm0,0
	jmp	   	rnsx4

;       x is a NaN.
x_NaN:
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	movdqu	xmm4,XMMWORD PTR [rcx]			; get x
	movdqa	xmm3,xmm4
	movdqa	xmm5,xmm4
	movdqa	xmm2,XMMWORD PTR __mask_sigbit	; get the signalling bits
	cmpps	xmm4,xmm4,0						; a compare equal  of x to itself should
											; be true, unless x is a NaN. 0's if NaN.
	cmpps	xmm3,xmm3,4						; compare not equal, ff's if NaN.
	andps	xmm0,xmm4						; keep the other results
	andps	xmm2,xmm3						; get just the right signalling bits
	andps	xmm3,xmm5						; mask for the NaNs
	orps	xmm3,xmm2	; convert to QNaNs
	orps	xmm0,xmm3						; combine
	jmp	   	rnsx5

;      /* y is infinite or so large that the result would
;         overflow or underflow.
y_large:
	movdqa	XMMWORD PTR p_temp[rsp],xmm0

	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		eax,DWORD PTR [rcx]
	mov		ebx,DWORD PTR p_y[rsp]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp[rsp],eax

	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		eax,DWORD PTR 4[rcx]
	mov		ebx,DWORD PTR p_y[rsp]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+4[rsp],eax

	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		eax,DWORD PTR 8[rcx]
	mov		ebx,DWORD PTR p_y[rsp]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+8[rsp],eax

	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		eax,DWORD PTR 12[rcx]
	mov		ebx,DWORD PTR p_y[rsp]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+12[rsp],eax

	movdqa	xmm0,XMMWORD PTR p_temp[rsp]
	jmp 	rnsx3

; a subroutine to treat an individual x,y pair when y is large or infinity
; assumes x in eax, y in ebx.
; returns result in eax
np_special6:
; handle |x|==1 cases first
	mov		r8d,07FFFFFFFh
	and		r8d,eax
	cmp		r8d,03f800000h					  ; jump if |x| !=1 
	jnz		nps6
	mov		eax,03f800000h					  ; return 1 for all |x|==1
	jmp 	npx64

; cases where  |x| !=1
nps6:
	mov		ecx,07f800000h
	xor		eax,eax							  ; assume 0 return
	test	ebx,080000000h
	jnz		nps62							  ; jump if y negative
; y = +inf
	cmp		r8d,03f800000h
	cmovg	eax,ecx							  ; return inf if |x| < 1
	jmp 	npx64
nps62:
; y = -inf
	cmp		r8d,03f800000h
	cmovl	eax,ecx							  ; return inf if |x| < 1
	jmp 	npx64

npx64:													 
	ret

; handle cases where x is +/- infinity.  edx is the mask
	align 16
x_infinite:
	movdqa	XMMWORD PTR p_temp[rsp],xmm0

	test	edx,1
	jz		xinfa
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		eax,DWORD PTR [rcx]
	mov		ebx,DWORD PTR p_y[rsp]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp[rsp],eax
xinfa:
	test	edx,2
	jz		xinfb
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR 4[rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+4[rsp],eax
xinfb:
	test	edx,4
	jz		xinfc
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR 8[rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+8[rsp],eax
xinfc:
	test	edx,8
	jz		xinfd
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR 12[rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+12[rsp],eax
xinfd:
	movdqa	xmm0,XMMWORD PTR p_temp[rsp]
	jmp 	rnsx1

; a subroutine to treat an individual x,y pair when x is +/-infinity
; assumes x in eax,y in ebx, inty in ecx.
; returns result in eax
np_special_x1:											; x is infinite
	test	eax,080000000h								; is x positive
	jnz		nsx11										; jump if not
	test	ebx,080000000h								; is y positive
	jz		nsx13											; just return if so
	xor		eax,eax										; else return 0
	jmp 	nsx13

nsx11:
	cmp		ecx,1										; if inty ==1
	jnz		nsx12										; jump if not
	test	ebx,080000000h								; is y positive
	jz		nsx13											; just return if so
	mov		eax,080000000h								; else return -0
	jmp 	nsx13
nsx12:													; inty <>1
	and		eax,07FFFFFFFh								; return -x (|x|)  if y<0
	test	ebx,080000000h								; is y positive
	jz		nsx13											; 
	xor		eax,eax										; return 0  if y >=0
nsx13:
	ret


; handle cases where x is +/- zero.  edx is the mask of x,y pairs with |x|=0
	align 16
x_zero:
	movdqa	XMMWORD PTR p_temp[rsp],xmm0

	test	edx,1
	jz		xzera
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR [rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp[rsp],eax
xzera:
	test	edx,2
	jz		xzerb
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR 4[rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+4[rsp],eax
xzerb:
	test	edx,4
	jz		xzerc
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR 8[rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+8[rsp],eax
xzerc:
	test	edx,8
	jz		xzerd
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		ebx,DWORD PTR p_y[rsp]
	mov		eax,DWORD PTR 12[rcx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+12[rsp],eax
xzerd:
	movdqa	xmm0,XMMWORD PTR p_temp[rsp]
	jmp 	rnsx2

; a subroutine to treat an individual x,y pair when x is +/-0
; assumes x in eax,y in ebx, inty in ecx.
; returns result in eax
	align 16
np_special_x2:
	cmp		ecx,1										; if inty ==1
	jz		nsx21										; jump if so
; handle cases of x=+/-0, y not integer
	xor		eax,eax
	mov		ecx,07f800000h
	test	ebx,080000000h								; is ypos
	cmovnz	eax,ecx
	jmp		nsx23
; y is an integer
nsx21:
	xor		r8d,r8d
	mov		ecx,07f800000h
	test	ebx,080000000h								; is ypos
	cmovnz	r8d,ecx										; set to infinity if not
	and		eax,080000000h								; pickup the sign of x
	or		eax,r8d										; and include it in the result
nsx23:
	ret
fname		endp

TEXT	ENDS
END
