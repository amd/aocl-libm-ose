;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; vrd4exp.asm
;
; A vector implementation of the exp libm function.
;
; Prototype:
;
;    __m128d,__m128d __vrd4_exp(__m128d x1, __m128d x2);
;
;   Computes e raised to the x power for an array of input values.
;   Places the results into the supplied y array.
; Does not perform error checking.   Denormal results are truncated to 0.
;
; This routine computes 4 double precision exponent values at a time.
; The four values are passed as packed doubles in xmm0 and xmm1.
; The four results are returned as packed doubles in xmm0 and xmm1.
; Note that this represents a non-standard ABI usage, as no ABI 
; ( and indeed C) currently allows returning 2 values for a function.
; It is expected that some compilers may be able to take advantage of this
; interface when implementing vectorized loops.  Using the array implementation
; of the routine requires putting the inputs into memory, and retrieving
; the results from memory.  This routine eliminates the need for this
; overhead if the data does not already reside in memory.
; This routine is derived directly from the array version.
;
;

CONST	SEGMENT
__real_3ff0000000000000 DQ 03ff0000000000000h	; 1.0
						dq 03ff0000000000000h					; for alignment
__real_4040000000000000	DQ 04040000000000000h	; 32
						dq 04040000000000000h
__real_40F0000000000000	DQ 040F0000000000000h	; 65536, to protect against really large numbers
						dq 040F0000000000000h
__real_C0F0000000000000	DQ 0C0F0000000000000h	; -65536, to protect against really large - numbers
						dq 0C0F0000000000000h
__real_3FA0000000000000	DQ 03FA0000000000000h	; 1/32
						dq 03FA0000000000000h
__real_3fe0000000000000	DQ 03fe0000000000000h	; 1/2
						dq 03fe0000000000000h
__real_infinity			DQ 07ff0000000000000h	; 
						dq 07ff0000000000000h					; for alignment
__real_ninfinity		DQ 0fff0000000000000h	; 
						dq 0fff0000000000000h					; for alignment
__real_thirtytwo_by_log2 DQ 040471547652b82feh	; thirtytwo_by_log2
						dq 040471547652b82feh
__real_log2_by_32_lead  DQ 03f962e42fe000000h	; log2_by_32_lead
						dq 03f962e42fe000000h
__real_log2_by_32_tail  DQ 0Bdcf473de6af278eh	; -log2_by_32_tail
						dq 0Bdcf473de6af278eh
__real_3f56c1728d739765	DQ 03f56c1728d739765h	; 1.38889490863777199667e-03 used in splitexp
						dq 03f56c1728d739765h
__real_3F811115B7AA905E	DQ 03F811115B7AA905Eh	; 8.33336798434219616221e-03 used in splitexp
						dq 03F811115B7AA905Eh
__real_3FA5555555545D4E	DQ 03FA5555555545D4Eh	; 4.16666666662260795726e-02 used in splitexp
						dq 03FA5555555545D4Eh
__real_3FC5555555548F7C	DQ 03FC5555555548F7Ch	; 1.66666666665260878863e-01 used in splitexp
						dq 03FC5555555548F7Ch


__two_to_jby32_lead_table:
	DQ	03ff0000000000000h ; 1 
	DQ	03ff059b0d0000000h		; 1.0219
	DQ	03ff0b55860000000h		; 1.04427
	DQ	03ff11301d0000000h		; 1.06714
	DQ	03ff172b830000000h		; 1.09051
	DQ	03ff1d48730000000h		; 1.11439
	DQ	03ff2387a60000000h		; 1.13879
	DQ	03ff29e9df0000000h		; 1.16372
	DQ	03ff306fe00000000h		; 1.18921
	DQ	03ff371a730000000h		; 1.21525
	DQ	03ff3dea640000000h		; 1.24186
	DQ	03ff44e0860000000h		; 1.26905
	DQ	03ff4bfdad0000000h		; 1.29684
	DQ	03ff5342b50000000h		; 1.32524
	DQ	03ff5ab07d0000000h		; 1.35426
	DQ	03ff6247eb0000000h		; 1.38391
	DQ	03ff6a09e60000000h		; 1.41421
	DQ	03ff71f75e0000000h		; 1.44518
	DQ	03ff7a11470000000h		; 1.47683
	DQ	03ff8258990000000h		; 1.50916
	DQ	03ff8ace540000000h		; 1.54221
	DQ	03ff93737b0000000h		; 1.57598
	DQ	03ff9c49180000000h		; 1.61049
	DQ	03ffa5503b0000000h		; 1.64576
	DQ	03ffae89f90000000h		; 1.68179
	DQ	03ffb7f76f0000000h		; 1.71862
	DQ	03ffc199bd0000000h		; 1.75625
	DQ	03ffcb720d0000000h		; 1.79471
	DQ	03ffd5818d0000000h		; 1.83401
	DQ	03ffdfc9730000000h		; 1.87417
	DQ	03ffea4afa0000000h		; 1.91521
	DQ	03fff507650000000h		; 1.95714
	dq 0					; for alignment
__two_to_jby32_trail_table:
	DQ	00000000000000000h ; 0 
	DQ	03e48ac2ba1d73e2ah		; 1.1489e-008
	DQ	03e69f3121ec53172h		; 4.83347e-008
	DQ	03df25b50a4ebbf1bh		; 2.67125e-010
	DQ	03e68faa2f5b9bef9h		; 4.65271e-008
	DQ	03e368b9aa7805b80h		; 5.24924e-009
	DQ	03e6ceac470cd83f6h		; 5.38622e-008
	DQ	03e547f7b84b09745h		; 1.90902e-008
	DQ	03e64636e2a5bd1abh		; 3.79764e-008
	DQ	03e5ceaa72a9c5154h		; 2.69307e-008
	DQ	03e682468446b6824h		; 4.49684e-008
	DQ	03e18624b40c4dbd0h		; 1.41933e-009
	DQ	03e54d8a89c750e5eh		; 1.94147e-008
	DQ	03e5a753e077c2a0fh		; 2.46409e-008
	DQ	03e6a90a852b19260h		; 4.94813e-008
	DQ	03e0d2ac258f87d03h		; 8.48872e-010
	DQ	03e59fcef32422cbfh		; 2.42032e-008
	DQ	03e61d8bee7ba46e2h		; 3.3242e-008
	DQ	03e4f580c36bea881h		; 1.45957e-008
	DQ	03e62999c25159f11h		; 3.46453e-008
	DQ	03e415506dadd3e2ah		; 8.0709e-009
	DQ	03e29b8bc9e8a0388h		; 2.99439e-009
	DQ	03e451f8480e3e236h		; 9.83622e-009
	DQ	03e41f12ae45a1224h		; 8.35492e-009
	DQ	03e62b5a75abd0e6ah		; 3.48493e-008
	DQ	03e47daf237553d84h		; 1.11085e-008
	DQ	03e6b0aa538444196h		; 5.03689e-008
	DQ	03e69df20d22a0798h		; 4.81896e-008
	DQ	03e69f7490e4bb40bh		; 4.83654e-008
	DQ	03e4bdcdaf5cb4656h		; 1.29746e-008
	DQ	03e452486cc2c7b9dh		; 9.84533e-009
	DQ	03e66dc8a80ce9f09h		; 4.25828e-008
	dq 0					; for alignment
CONST	ENDS



; define local variable storage offsets
p_temp			equ		0		; temporary for get/put bits operation
p_temp1			equ		10h		; temporary for exponent multiply

save_rbx		equ		038h		;qword
save_rdi		equ		040h		;qword

save_rsi		equ		048h		;qword

save_xmm6		equ		050h		;xmmword
save_xmm7		equ		060h		;xmmword


p2_temp			equ		80h		; second temporary for get/put bits operation
p2_temp1		equ		90h		; second temporary for exponent multiply

save_xmm8		equ		0a0h		;xmmword
save_xmm9		equ		0b0h		;xmmword

stack_size		equ		0c8h



fname		textequ		<__amd_internal_vrd4_exp>

; parameters are passed in by Microsoft C as:
; xmm0 - __m128d x1
; xmm1 - __m128d x2

TEXT	SEGMENT	   page	   'CODE'
	PUBLIC  fname
fname	proc	frame
	sub		rsp,stack_size
	.ALLOCSTACK	stack_size
	mov		QWORD PTR [rsp+save_rbx],rbx	; save rdi
	.SAVEREG rbx, save_rbx
	mov		QWORD PTR [rsp+save_rdi],rdi	; save rdi
	.SAVEREG rdi, save_rdi
	mov		QWORD PTR [rsp+save_rsi],rsi	; save rsi
	.SAVEREG rsi, save_rsi
	movdqa	XMMWORD PTR [rsp+save_xmm6],xmm6	; save xmm6
	.SAVEXMM128 xmm6, save_xmm6
	movdqa	XMMWORD PTR [rsp+save_xmm7],xmm7
	.SAVEXMM128 xmm7, save_xmm7
	movdqa	XMMWORD PTR [rsp+save_xmm8],xmm8	; save xmm6
	.SAVEXMM128 xmm8, save_xmm8
	movdqa	XMMWORD PTR [rsp+save_xmm9],xmm9
	.SAVEXMM128 xmm9, save_xmm9
	.ENDPROLOG
		movapd	xmm6,xmm1

; process 4 values at a time.

	movapd	xmm3,XMMWORD PTR __real_thirtytwo_by_log2	;
	
;      Step 1. Reduce the argument.
;        /* Find m, z1 and z2 such that exp(x) = 2**m * (z1 + z2) */
;    r = x * thirtytwo_by_logbaseof2;
		movapd	xmm7,xmm3
	movapd	XMMWORD PTR p_temp[rsp],xmm0
		movapd	XMMWORD PTR p2_temp[rsp],xmm6
	maxpd	xmm0,QWORD PTR __real_C0F0000000000000  ; protect against very large, non-infinite numbers 
		maxpd	xmm6,QWORD PTR __real_C0F0000000000000  ; protect against very large, non-infinite numbers 

	mulpd	xmm3, xmm0 
		mulpd	xmm7, xmm6 

; save x for later.
	minpd	xmm3,QWORD PTR __real_40F0000000000000  ; protect against very large, non-infinite numbers 

;    /* Set n = nearest integer to r */
	cvtpd2dq	xmm4,xmm3
	lea		rdi,__two_to_jby32_lead_table
	lea		rsi,__two_to_jby32_trail_table
	cvtdq2pd	xmm1,xmm4
	minpd	xmm7,QWORD PTR __real_40F0000000000000  ; protect against very large, non-infinite numbers 

 ;    r1 = x - n * logbaseof2_by_32_lead;
	movapd	xmm2,XMMWORD PTR __real_log2_by_32_lead	;
	mulpd	xmm2,xmm1								;   
	movq	QWORD PTR p_temp1[rsp],xmm4
	subpd	xmm0,xmm2					 			; r1 in xmm0, 

		cvtpd2dq	xmm2,xmm7
		cvtdq2pd	xmm8,xmm2

;    r2 =   - n * logbaseof2_by_32_trail;
	mulpd	xmm1,XMMWORD PTR __real_log2_by_32_tail	; r2 in xmm1 
;    j = n & 0x0000001f;
	mov		r9,01fh
	mov		r8,r9
	mov		ecx,DWORD PTR p_temp1[rsp]
	and		r9d,ecx
		movq	QWORD PTR p2_temp1[rsp],xmm2
		movapd	xmm9,XMMWORD PTR __real_log2_by_32_lead	
		mulpd	xmm9,xmm8								   
		subpd	xmm6,xmm9					 			; r1b in xmm6
		mulpd	xmm8,XMMWORD PTR __real_log2_by_32_tail	; r2b in xmm8 

	mov		edx,DWORD PTR p_temp1+4[rsp]
	and		r8d,edx
;    f1 = two_to_jby32_lead_table[j];
;    f2 = two_to_jby32_trail_table[j];

;    *m = (n - j) / 32;
	sub		ecx,r9d
	sar		ecx,5					;m
	sub		edx,r8d
	sar		edx,5


	movapd	xmm2,xmm0
	addpd	xmm2,xmm1;    r = r1 + r2

		mov		r11,01fh
		mov		r10,r11
		mov		ebx,DWORD PTR p2_temp1[rsp]
		and		r11d,ebx
;      Step 2. Compute the polynomial.
;    q = r1 + (r2 +	
;              r*r*( 5.00000000000000008883e-01 +
;                      r*( 1.66666666665260878863e-01 +
;                      r*( 4.16666666662260795726e-02 +
;                      r*( 8.33336798434219616221e-03 +
;                      r*( 1.38889490863777199667e-03 ))))));
;    q = r + r^2/2 + r^3/6 + r^4/24 + r^5/120 + r^6/720	
	movapd	xmm1,xmm2
	movapd	xmm3,XMMWORD PTR __real_3f56c1728d739765	; 	1/720
	movapd	xmm0,XMMWORD PTR __real_3FC5555555548F7C	; 	1/6
; deal with infinite results
	mov		rax,1024
	movsxd	rcx,ecx
	cmp		rcx,rax

	mulpd	xmm3,xmm2								; *x
	mulpd	xmm0,xmm2								; *x
	mulpd	xmm1,xmm2								; x*x
	movapd	xmm4,xmm1

	cmovg	rcx,rax					; if infinite, then set rcx to multiply
									; by infinity
	movsxd	rdx,edx
	cmp		rdx,rax

		movapd	xmm9,xmm6
		addpd	xmm9,xmm8;    rb = r1b + r2b
	addpd	xmm3,XMMWORD PTR __real_3F811115B7AA905E	; 	+ 1/120
	addpd	xmm0,XMMWORD PTR __real_3fe0000000000000	; 	+ .5
	mulpd	xmm4,xmm1								; x^4
	mulpd	xmm3,xmm2								; *x

	cmovg	rdx,rax					; if infinite, then set rcx to multiply
									; by infinity
; deal with denormal results
	xor		rax,rax
	add		rcx,1023				; add bias

	mulpd	xmm0,xmm1								; *x^2
	addpd	xmm3,XMMWORD PTR __real_3FA5555555545D4E	; 	+ 1/24
	addpd	xmm0,xmm2								; 	+ x
	mulpd	xmm3,xmm4								; *x^4

; check for infinity or nan
	movapd	xmm2,XMMWORD PTR p_temp[rsp]	  

	cmovs	rcx,rax					; if denormal, then multiply by 0
	shl		rcx,52					; build 2^n

		sub		ebx,r11d
		movapd	xmm1,xmm9
	addpd	xmm0,xmm3								; q = final sum
		movapd	xmm7,XMMWORD PTR __real_3f56c1728d739765	; 	1/720
		movapd	xmm3,XMMWORD PTR __real_3FC5555555548F7C	; 	1/6

;    *z2 = f2 + ((f1 + f2) * q);
	movlpd	xmm5, QWORD PTR [rsi+r9*8]		; f2
	movlpd	xmm4, QWORD PTR [rsi+r8*8]		; f2
	addsd	xmm4,QWORD PTR [rdi+r8*8]		; f1 + f2
	addsd	xmm5,QWORD PTR [rdi+r9*8]		; f1 + f2
		mov		r8d,DWORD PTR p2_temp1+4[rsp]
		and		r10d,r8d
		sar		ebx,5					;m
		mulpd	xmm7,xmm9								; *x
		mulpd	xmm3,xmm9								; *x
		mulpd	xmm1,xmm9								; x*x
		sub		r8d,r10d
		sar		r8d,5
; check for infinity or nan
	andpd	xmm2,XMMWORD PTR __real_infinity
	cmppd	xmm2,XMMWORD PTR __real_infinity,0
	add		rdx,1023				; add bias
	shufpd	xmm5,xmm4,0
		movapd	xmm4,xmm1

	cmovs	rdx,rax					; if denormal, then multiply by 0
	shl		rdx,52					; build 2^n

	mulpd	xmm0,xmm5
	mov		QWORD PTR p_temp1[rsp],rcx ; get 2^n to memory		
	mov		QWORD PTR p_temp1+8[rsp],rdx ; get 2^n to memory		
	addpd	xmm0,xmm5						;z = z1 + z2   done with 1,2,3,4,5
		mov		rax,1024
		movsxd	rbx,ebx
		cmp		rbx,rax
; end of splitexp				
;        /* Scale (z1 + z2) by 2.0**m */
;          r = scaleDouble_1(z, n);


		cmovg	rbx,rax					; if infinite, then set rcx to multiply
										; by infinity
		movsxd	rdx,r8d
		cmp		rdx,rax

	movmskpd	r8d,xmm2

		addpd	xmm7,XMMWORD PTR __real_3F811115B7AA905E	; 	+ 1/120
		addpd	xmm3,XMMWORD PTR __real_3fe0000000000000	; 	+ .5
		mulpd	xmm4,xmm1								; x^4
		mulpd	xmm7,xmm9								; *x
		cmovg	rdx,rax					; if infinite, then set rcx to multiply


		xor		rax,rax
		add		rbx,1023				; add bias

		mulpd	xmm3,xmm1								; *x^2
		addpd	xmm7,XMMWORD PTR __real_3FA5555555545D4E	; 	+ 1/24
		addpd	xmm3,xmm9								; 	+ x
		mulpd	xmm7,xmm4								; *x^4

		cmovs	rbx,rax					; if denormal, then multiply by 0
		shl		rbx,52					; build 2^n

;      Step 3. Reconstitute.

	mulpd	xmm0,XMMWORD PTR p_temp1[rsp]	; result *= 2^n
		addpd	xmm3,xmm7								; q = final sum

		movlpd	xmm5, QWORD PTR [rsi+r11*8]		; f2
		movlpd	xmm4, QWORD PTR [rsi+r10*8]		; f2
		addsd	xmm4,QWORD PTR [rdi+r10*8]		; f1 + f2
		addsd	xmm5,QWORD PTR [rdi+r11*8]		; f1 + f2

		add		rdx,1023				; add bias
		cmovs	rdx,rax					; if denormal, then multiply by 0
		shufpd	xmm5,xmm4,0
		shl		rdx,52					; build 2^n

		mulpd	xmm3,xmm5
		mov		QWORD PTR p2_temp1[rsp],rbx ; get 2^n to memory		
		mov		QWORD PTR p2_temp1+8[rsp],rdx ; get 2^n to memory		
		addpd	xmm3,xmm5						;z = z1 + z2   done with 1,2,3,4,5

		movapd	xmm2,XMMWORD PTR p2_temp[rsp]	  
		andpd	xmm2,XMMWORD PTR __real_infinity
		cmppd	xmm2,XMMWORD PTR __real_infinity,0
		movmskpd	ebx,xmm2
	test		r8d,3
		mulpd	xmm3,XMMWORD PTR p2_temp1[rsp]	; result *= 2^n
; we'd like to avoid a branch, and can use cmp's and and's to
; eliminate them.  But it adds cycles for normal cases which
; are supposed to be exceptions.  Using this branch with the
; check above results in faster code for the normal cases.
	jnz			__exp_naninf	  

__vda_bottom1:
; store the result _m128d
		test		ebx,3
		jnz			__exp_naninf2	  

__vda_bottom2:

	movapd	xmm1,xmm3


;
;
__final_check:
	movdqa	xmm9,XMMWORD PTR [rsp+save_xmm9]	; restore xmm9
	movdqa	xmm8,XMMWORD PTR [rsp+save_xmm8]	; restore xmm8
	movdqa	xmm7,XMMWORD PTR [rsp+save_xmm7]	; restore xmm7
	movdqa	xmm6,XMMWORD PTR [rsp+save_xmm6]	; restore xmm6
	mov		rdi,[rsp+save_rdi]		; restore rdi
	mov		rsi,[rsp+save_rsi]		; restore rsi
	mov		rbx,[rsp+save_rbx]		; restore rbx
	add		rsp,stack_size
	ret

; at least one of the numbers needs special treatment
__exp_naninf:
	lea		rcx,QWORD PTR p_temp[rsp]
	call  __naninf
	jmp		__vda_bottom1
__exp_naninf2:
	lea		rcx,QWORD PTR p2_temp[rsp]
	mov			r8d,ebx
	movapd	xmm4,xmm0
	movapd	xmm0,xmm3
	call  __naninf
	movapd	xmm3,xmm0
	movapd	xmm0,xmm4
	jmp		__vda_bottom2

; This subroutine checks a double pair for nans and infinities and
; produces the proper result from the exceptional inputs
; Register assumptions:
; Inputs:
; r8d - mask of errors
; xmm0 - computed result vector
; rcx - pointing to memory image of inputs
; Outputs:
; xmm0 - new result vector
; xmm2,rax,rdx, all modified.
__naninf:
; check the first number
	test	r8d,1
	jz		__check2

	mov		rdx,QWORD PTR [rcx]
	mov		rax,0000FFFFFFFFFFFFFh
	test	rdx,rax
	jnz		__enan1					; jump if mantissa not zero, so it's a NaN
; inf
	mov		rax,rdx
	rcl		rax,1
	jnc		__r1				; exp(+inf) = inf
	xor		rdx,rdx				; exp(-inf) = 0
	jmp		__r1

;NaN
__enan1:	
	mov		rax,00008000000000000h	; convert to quiet
	or		rdx,rax
__r1:
	movd	xmm2,rdx
	shufpd	xmm2,xmm0,2
	movsd	xmm0,xmm2
; check the second number
__check2:
	test	r8d,2
	jz		__r3
	mov		rdx,QWORD PTR [rcx+8]
	mov		rax,0000FFFFFFFFFFFFFh
	test	rdx,rax
	jnz		__enan2					; jump if mantissa not zero, so it's a NaN
; inf
	mov		rax,rdx
	rcl		rax,1
	jnc		__r2				; exp(+inf) = inf
	xor		rdx,rdx				; exp(-inf) = 0
	jmp		__r2

;NaN
__enan2:	
	mov		rax,00008000000000000h	; convert to quiet
	or		rdx,rax
__r2:
	movd	xmm2,rdx
	shufpd	xmm0,xmm2,0
__r3:
	ret


fname		endp
TEXT	ENDS
END
