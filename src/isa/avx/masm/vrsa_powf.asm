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
; vrsapowf.asm
;
; An array implementation of the powf libm function.
;
; Prototype:
;
;     void vrsa_powf(int n, float *x, float *y, float *z);
;
;   Computes x raised to the y power.  
;
;   Places the results into the supplied z array.
; Does not perform error handling, but does return C99 values for error
; inputs.   Denormal results are truncated to 0.

;
;

include fm.inc
FN_PROTOTYPE vrsa_powf


fname_exp   TEXTEQU <__amd_internal_vrd4_exp>
fname_log   TEXTEQU <__amd_internal_vrd4_log>

EXTRN	fname_exp:NEAR
EXTRN	fname_log:NEAR

CONST	SEGMENT
__np_ln_table:
   dq 0000000000000000h   ;                   0
   dq 3F8FC0A8B0FC03E4h   ;0.015504186535965254
   dq 3F9F829B0E783300h   ;0.030771658666753687
   dq 3FA77458F632DCFCh   ;0.045809536031294201
   dq 3FAF0A30C01162A6h   ; 0.06062462181643484
   dq 3FB341D7961BD1D1h   ;0.075223421237587532
   dq 3FB6F0D28AE56B4Ch   ;0.089612158689687138
   dq 3FBA926D3A4AD563h   ; 0.10379679368164356
   dq 3FBE27076E2AF2E6h   ; 0.11778303565638346
   dq 3FC0D77E7CD08E59h   ; 0.13157635778871926
   dq 3FC29552F81FF523h   ; 0.14518200984449789
   dq 3FC44D2B6CCB7D1Eh   ; 0.15860503017663857
   dq 3FC5FF3070A793D4h   ; 0.17185025692665923
   dq 3FC7AB890210D909h   ; 0.18492233849401199
   dq 3FC9525A9CF456B4h   ; 0.19782574332991987
   dq 3FCAF3C94E80BFF3h   ; 0.21056476910734964
   dq 3FCC8FF7C79A9A22h   ; 0.22314355131420976
   dq 3FCE27076E2AF2E6h   ; 0.23556607131276691
   dq 3FCFB9186D5E3E2Bh   ; 0.24783616390458127
   dq 3FD0A324E27390E3h   ; 0.25995752443692605
   dq 3FD1675CABABA60Eh   ; 0.27193371548364176
   dq 3FD22941FBCF7966h   ; 0.28376817313064462
   dq 3FD2E8E2BAE11D31h   ;  0.2954642128938359
   dq 3FD3A64C556945EAh   ; 0.30702503529491187
   dq 3FD4618BC21C5EC2h   ; 0.31845373111853459
   dq 3FD51AAD872DF82Dh   ; 0.32975328637246798
   dq 3FD5D1BDBF5809CAh   ; 0.34092658697059319
   dq 3FD686C81E9B14AFh   ;  0.3519764231571782
   dq 3FD739D7F6BBD007h   ; 0.36290549368936847
   dq 3FD7EAF83B82AFC3h   ; 0.37371640979358406
   dq 3FD89A3386C1425Bh   ; 0.38441169891033206
   dq 3FD947941C2116FBh   ; 0.39499380824086899
   dq 3FD9F323ECBF984Ch   ; 0.40546510810816438
   dq 3FDA9CEC9A9A084Ah   ; 0.41582789514371099
   dq 3FDB44F77BCC8F63h   ; 0.42608439531090009
   dq 3FDBEB4D9DA71B7Ch   ; 0.43623676677491807
   dq 3FDC8FF7C79A9A22h   ; 0.44628710262841953
   dq 3FDD32FE7E00EBD5h   ; 0.45623743348158757
   dq 3FDDD46A04C1C4A1h   ; 0.46608972992459924
   dq 3FDE744261D68788h   ; 0.47584590486996392
   dq 3FDF128F5FAF06EDh   ; 0.48550781578170082
   dq 3FDFAF588F78F31Fh   ; 0.49507726679785152
   dq 3FE02552A5A5D0FFh   ; 0.50455601075239531
   dq 3FE0723E5C1CDF40h   ; 0.51394575110223428
   dq 3FE0BE72E4252A83h   ; 0.52324814376454787
   dq 3FE109F39E2D4C97h   ; 0.53246479886947184
   dq 3FE154C3D2F4D5EAh   ; 0.54159728243274441
   dq 3FE19EE6B467C96Fh   ;  0.5506471179526623
   dq 3FE1E85F5E7040D0h   ; 0.55961578793542266
   dq 3FE23130D7BEBF43h   ; 0.56850473535266877
   dq 3FE2795E1289B11Bh   ; 0.57731536503482361
   dq 3FE2C0E9ED448E8Ch   ; 0.58604904500357824
   dq 3FE307D7334F10BEh   ; 0.59470710774669278
   dq 3FE34E289D9CE1D3h   ; 0.60329085143808425
   dq 3FE393E0D3562A1Ah   ; 0.61180154110599294
   dq 3FE3D9026A7156FBh   ; 0.62024040975185757
   dq 3FE41D8FE84672AEh   ; 0.62860865942237409
   dq 3FE4618BC21C5EC2h   ; 0.63690746223706918
   dq 3FE4A4F85DB03EBBh   ;  0.6451379613735847
   dq 3FE4E7D811B75BB1h   ; 0.65330127201274568
   dq 3FE52A2D265BC5ABh   ; 0.66139848224536502
   dq 3FE56BF9D5B3F399h   ; 0.66943065394262924
   dq 3FE5AD404C359F2Dh   ; 0.67739882359180614
   dq 3FE5EE02A9241675h   ; 0.68530400309891937
   dq 3FE62E42FEFA39EFh   ; 0.69314718055994529
	dq 0					; for alignment



__two_to_jby32_table   label   qword
	DQ	03FF0000000000000h ; 1 
	DQ	03FF059B0D3158574h		; 1.0219
	DQ	03FF0B5586CF9890Fh		; 1.04427
	DQ	03FF11301D0125B51h		; 1.06714
	DQ	03FF172B83C7D517Bh		; 1.09051
	DQ	03FF1D4873168B9AAh		; 1.11439
	DQ	03FF2387A6E756238h		; 1.13879
	DQ	03FF29E9DF51FDEE1h		; 1.16372
	DQ	03FF306FE0A31B715h		; 1.18921
	DQ	03FF371A7373AA9CBh		; 1.21525
	DQ	03FF3DEA64C123422h		; 1.24186
	DQ	03FF44E086061892Dh		; 1.26905
	DQ	03FF4BFDAD5362A27h		; 1.29684
	DQ	03FF5342B569D4F82h		; 1.32524
	DQ	03FF5AB07DD485429h		; 1.35426
	DQ	03FF6247EB03A5585h		; 1.38391
	DQ	03FF6A09E667F3BCDh		; 1.41421
	DQ	03FF71F75E8EC5F74h		; 1.44518
	DQ	03FF7A11473EB0187h		; 1.47683
	DQ	03FF82589994CCE13h		; 1.50916
	DQ	03FF8ACE5422AA0DBh		; 1.54221
	DQ	03FF93737B0CDC5E5h		; 1.57598
	DQ	03FF9C49182A3F090h		; 1.61049
	DQ	03FFA5503B23E255Dh		; 1.64576
	DQ	03FFAE89F995AD3ADh		; 1.68179
	DQ	03FFB7F76F2FB5E47h		; 1.71862
	DQ	03FFC199BDD85529Ch		; 1.75625
	DQ	03FFCB720DCEF9069h		; 1.79471
	DQ	03FFD5818DCFBA487h		; 1.83401
	DQ	03FFDFC97337B9B5Fh		; 1.87417
	DQ	03FFEA4AFA2A490DAh		; 1.91521
	DQ	03FFF50765B6E4540h		; 1.95714

__real_3f80000000000000	DQ 03f80000000000000h	; /* 0.0078125 = 1/128 */
						dq 0


__real_3FA5555555545D4E	DQ 03FA5555555545D4Eh	; 4.16666666662260795726e-02 used in splitexp
						dq 0
__real_3FC5555555548F7C	DQ 03FC5555555548F7Ch	; 1.66666666665260878863e-01 used in splitexp
						dq 0


__real_one				DQ 03ff0000000000000h	; 1.0
						dq 0					; for alignment
__real_two				DQ 04000000000000000h	; 1.0
						dq 0

__real_ca1				DQ 03fb55555555554e6h	; 8.33333333333317923934e-02
						dq 0					; for alignment
__real_ca2				DQ 03f89999999bac6d4h	; 1.25000000037717509602e-02
						dq 0					; for alignment

; from Maple:
;Digits :=40;
;minimax(2*ln(1+v/2), v=0..1/256, 7,1 ,'maxerror');
__real_cb1				DQ 03fb555555555552ch	; 0.08333333333333275459088388736767942281572 from maple
						dq 0					; for alignment
__real_cb2				DQ 03F8999998EAB53DBh	; 0.01249999968187325554473232707489405493533
						dq 0					; for alignment

__real_log2		  		DQ 03FE62E42FEFA39EFh	; log2_lead	  6.9314718055994530941723e-01
						dq 0					; for alignment

__real_half				DQ 03fe0000000000000h	; 1/2
						dq 0					; for alignment

__real_infinity			DQ 07ff0000000000000h	; 
						dq 0					; for alignment
__real_thirtytwo_by_log2 DQ 040471547652b82feh	; thirtytwo_by_log2
						dq 0
__real_log2_by_32       DQ 03F962E42FEFA39EFh	; log2_by_32
						dq 0

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


save_rbx		equ		030h		;qword
save_rsi		equ		038h		;qword
save_rdi		equ		040h		;qword


p_ax	  		equ		050h		; absolute x
p_sx	  		equ		060h		; sign of x's

p_ay	  		equ		070h		; absolute y
p_yexp	  		equ		080h		; unbiased exponent of y

p_inty  		equ		090h		; integer y indicators

p_xptr	  		equ		0a0h		; ptr to x values
p_yptr	  		equ		0a8h		; ptr to y values
p_zptr	  		equ		0b0h		; ptr to z values

p_nv			equ		0b8h		;qword
p_iter			equ		0c0h		; qword	storage for number of loop iterations

p2_temp			equ		0d0h		;qword
p2_temp1		equ		0f0h		;qword

stack_size		equ		0118h	; allocate 40h more than
								; we need to avoid bank conflicts





TEXT	SEGMENT	   page	   'CODE'

; parameters are passed in by Microsoft C as:
; ecx - int    n
; rdx - float *x
; r8  - float *y
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
; save the arguments

    cmp         ecx, 00h
    jle         L__return
    cmp         rdx, 00h
    je          L__return
    cmp         r8, 00h
    je          L__return
    cmp         r9, 00h
    je          L__return
        
	mov		QWORD PTR p_xptr[rsp],rdx		; save pointer to x
	mov		QWORD PTR p_yptr[rsp],r8		; save pointer to y
	mov		QWORD PTR p_zptr[rsp],r9		; save pointer to z

	xor		rax,rax
	mov		eax,ecx
	mov     rcx,rax
	mov		QWORD PTR [rsp+p_nv],rcx	; save number of values
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
; classify y
; vector 32 bit integer method	 25 cycles to here
;  /* See whether y is an integer.
;     inty = 0 means not an integer.
;     inty = 1 means odd integer.
;     inty = 2 means even integer.
;  */
	mov		rdi,QWORD PTR [rsp+p_yptr]	; get y_array pointer
	movups  xmm4,XMMWORD PTR [rdi]
	prefetch	QWORD PTR [rdi+64]
	pxor	xmm3,xmm3
	pand	xmm4,XMMWORD PTR __mask_nsign		; get abs y in integer format
	movdqa  XMMWORD PTR p_ay[rsp],xmm4			; save it

; see if the number is less than 1.0
	psrld	xmm4,23			;>> EXPSHIFTBITS_SP32

	psubd	xmm4,XMMWORD PTR __mask_127			; yexp, unbiased exponent
	movdqa  XMMWORD PTR p_yexp[rsp],xmm4		; save it
	paddd	xmm4,XMMWORD PTR __mask_1			; yexp+1
	pcmpgtd	xmm4,xmm3		; 0 if exp less than 126 (2^0) (y < 1.0), else FFs
; xmm4 is ffs if abs(y) >=1.0, else 0

; see if the mantissa has fractional bits
;build mask for mantissa
	movdqa  xmm2,XMMWORD PTR __mask_23
	psubd	xmm2,XMMWORD PTR p_yexp[rsp]		; 24-yexp
	pmaxsw	xmm2,xmm3							; no shift counts less than 0
	movdqa  XMMWORD PTR p_temp[rsp],xmm2		; save the shift counts
; create mask for all four values
; SSE can't individual shifts so have to do each one seperately
	mov		rcx,QWORD PTR p_temp[rsp]
	mov		rbx,1
	shl		ebx,cl			;1 << (24 - yexp)
	shr		rcx,32
	mov		eax,1
	shl		eax,cl			;1 << (24 - yexp)
	shl		rax,32
	add		rbx,rax
	mov		QWORD PTR p_temp+[rsp],rbx
	mov		rcx,QWORD PTR p_temp+8[rsp]
	mov		rbx,1
	shl		ebx,cl			;1 << (24 - yexp)
	shr		rcx,32
	mov		eax,1
	shl		eax,cl			;1 << (24 - yexp)
	shl		rax,32
	add		rax,rbx
	mov		QWORD PTR p_temp+8[rsp],rax
	movdqa  xmm5,XMMWORD PTR p_temp[rsp]
	psubd	xmm5,XMMWORD PTR __mask_1	;= mask = (1 << (24 - yexp)) - 1
	
; now use the mask to see if there are any fractional bits
	movdqu  xmm2,XMMWORD PTR [rdi] ; get uy
	pand	xmm2,xmm5		; uy & mask
	pcmpeqd	xmm2,xmm3		; 0 if not zero (y has fractional mantissa bits), else FFs
	pand	xmm2,xmm4		; either 0s or ff
; xmm2 now accounts for y< 1.0 or y>=1.0 and y has fractional mantissa bits,
; it has the value 0 if we know it's non-integer or ff if integer.

; now see if it's even or odd.

; if yexp > 24, then it has to be even
	movdqa  xmm4,XMMWORD PTR __mask_24
	psubd	xmm4,XMMWORD PTR p_yexp[rsp]		; 24-yexp
	paddd	xmm5,XMMWORD PTR __mask_1	; mask+1 = least significant integer bit
	pcmpgtd	xmm4,xmm3		 ; if 0, then must be even, else ff's

 	pand	xmm5,xmm4		; set the integer bit mask to zero if yexp>24
 	paddd	xmm4,XMMWORD PTR __mask_2
 	por		xmm4,XMMWORD PTR __mask_2
 	pand	xmm4,xmm2		 ; result can be 0, 2, or 3

; now for integer numbers, see if odd or even
	pand	xmm5,XMMWORD PTR __mask_mant	; mask out exponent bits
	movdqu	xmm2,XMMWORD PTR [rdi] ;  & uy -> even or odd
	pand    xmm5,xmm2 ;  & uy -> even or odd
	movdqa	xmm2,XMMWORD PTR __float_one
	pcmpeqd	xmm2,XMMWORD PTR p_ay[rsp]	; is ay equal to 1, ff's if so, then it's odd
	pand	xmm2,XMMWORD PTR __mask_nsign ; strip the sign bit so the gt comparison works.
	por		xmm5,xmm2
	pcmpgtd	xmm5,xmm3		 ; if odd then ff's, else 0's for even
	paddd	xmm5,XMMWORD PTR __mask_2 ; gives us 2 for even, 1 for odd
	pand	xmm4,xmm5

	movdqa		XMMWORD PTR p_inty[rsp],xmm4		; save inty
;
; do more x special case checking
;
	movdqa	xmm5,xmm4
	pcmpeqd	xmm5,xmm3						; is not an integer? ff's if so
	pand	xmm5,XMMWORD PTR __mask_NaN		; these values will be NaNs, if x<0
	movdqa	xmm2,xmm4
	pcmpeqd	xmm2,XMMWORD PTR __mask_1		; is it odd? ff's if so
	pand	xmm2,XMMWORD PTR __mask_sign	; these values will get their sign bit set
	por		xmm5,xmm2

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
	call		fname_log					; get the double precision log value 
											; for all four x's
; y* logx
; convert all four y's to double	
;	mov		rdi,QWORD PTR [rsp+p_yptr]	; get y_array pointer
	cvtps2pd   xmm2,QWORD PTR [rdi]
	cvtps2pd   xmm3,QWORD PTR 8[rdi]

;  /* just multiply by y */
	mulpd	xmm0,xmm2
	mulpd	xmm1,xmm3

;  /* The following code computes r = exp(w) */
	call		fname_exp					; get the double exp value 
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
;	mov		rdi,QWORD PTR p_yptr[rsp]		; get pointer to y
; apply the negate result flags
	orps	xmm0,XMMWORD PTR p_negateres[rsp]	; get negateres

; if y is infinite or so large that the result would overflow or underflow
	movdqa	xmm4,XMMWORD PTR p_ay[rsp]
	cmpps	xmm4,XMMWORD PTR __mask_ly,5	; y not less than large value, ffs if so.
	movmskps edx,xmm4
	test	edx,0fh
	jnz		y_large
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
;	mov		rdi,QWORD PTR p_yptr[rsp]		; get pointer to y
	movdqu	xmm4,XMMWORD PTR [rdi]			; get y
	cmpps	xmm4,xmm4,4						; a compare not equal  of y to itself should
											; be false, unless y is a NaN. ff's if NaN.
	movmskps ecx,xmm4
	test	ecx,0fh
	jnz		y_NaN
rnsx4:
; if x is NAN 
;	mov		rsi,QWORD PTR p_xptr[rsp]		; get pointer to x
	movdqu	xmm4,XMMWORD PTR [rsi]			; get x
	cmpps	xmm4,xmm4,4						; a compare not equal  of x to itself should
											; be false, unless x is a NaN. ff's if NaN.
	movmskps ecx,xmm4
	test	ecx,0fh
	jnz		x_NaN
rnsx5:

; if |y| == 0	then return 1
	movdqa	xmm3,XMMWORD PTR __float_one	; one
	xorps	xmm2,xmm2
	cmpps	xmm2,XMMWORD PTR p_ay[rsp],4	; not equal to 0.0?, ffs if not equal.
	andps	xmm0,xmm2						; keep the others
	andnps	xmm2,xmm3						; mask for ones
	orps	xmm0,xmm2
; if x == +1, return +1 for all x
;	mov		rsi,QWORD PTR p_xptr[rsp]		; get pointer to y
	movdqa	xmm2,xmm3
	movdqu	xmm5,XMMWORD PTR [rsi]		    ; get x
	cmpps	xmm2,xmm5,4		; not equal to +1.0?, ffs if not equal.
	andps	xmm0,xmm2						; keep the others
	andnps	xmm2,xmm3						; mask for ones
	orps	xmm0,xmm2

__powf_cleanup2:

; update the x and y pointers
;	mov		rsi,QWORD PTR [rsp+p_xptr]	; get x_array pointer
;	mov		rdi,QWORD PTR [rsp+p_yptr]	; get y_array pointer
	add		rdi,16
	add		rsi,16
	mov		QWORD PTR [rsp+p_xptr],rsi	; save x_array pointer
	mov		QWORD PTR [rsp+p_yptr],rdi	; save y_array pointer
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
L__return:	
	add		rsp,stack_size
	ret

	align 16
; we jump here when we have an odd number of log calls to make at the
; end
__vsa_cleanup:
	mov		rsi,QWORD PTR [rsp+p_xptr]
	mov		rdi,QWORD PTR [rsp+p_yptr]

; fill in a m128 with zeroes and the extra values and then make a recursive call.
	xorps		xmm0,xmm0
	xor		rax,rax
	movaps	XMMWORD PTR p2_temp[rsp],xmm0
	movaps	XMMWORD PTR p2_temp+16[rsp],xmm0

	mov		rax,p_nv[rsp]	; get number of values
	mov		ecx,[rsi]			; we know there's at least one
	mov	 	p2_temp[rsp],ecx
	mov		edx,[rdi]			; we know there's at least one
	mov	 	p2_temp+16[rsp],edx
	cmp		rax,2
	jl		__vsacg
	
	mov		ecx,4[rsi]			; do the second value
	mov	 	p2_temp+4[rsp],ecx
	mov		edx,4[rdi]			; we know there's at least one
	mov	 	p2_temp+20[rsp],edx
	cmp		rax,3
	jl		__vsacg

	mov		ecx,8[rsi]			; do the third value
	mov	 	p2_temp+8[rsp],ecx
	mov		edx,8[rdi]			; we know there's at least one
	mov	 	p2_temp+24[rsp],edx

__vsacg:
	mov		rcx,4				; parameter for N
	lea		rdx,p2_temp[rsp]	; &x parameter
	lea		r8,p2_temp+16[rsp]	; &y parameter
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
;      /* y is a NaN.
y_NaN:
	mov		rdx,QWORD PTR p_yptr[rsp]		; get pointer to y
	movdqu	xmm4,XMMWORD PTR [rdx]			; get y
	movdqa	xmm3,xmm4
	movdqa	xmm5,xmm4
	movdqa	xmm2,XMMWORD PTR __mask_sigbit	; get the signalling bits
	cmpps	xmm4,xmm4,0						; a compare equal  of y to itself should
											; be true, unless y is a NaN. 0's if NaN.
	cmpps	xmm3,xmm3,4						; compare not equal, ff's if NaN.
	andps	xmm0,xmm4						; keep the other results
	andps	xmm2,xmm3						; get just the right signalling bits
	andps	xmm3,xmm5						; mask for the NaNs
	orps	xmm3,xmm2	; convert to QNaNs
	orps	xmm0,xmm3						; combine
	jmp	   	rnsx4

;      /* y is a NaN.
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

	test	edx,1
	jz		ylrga
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR [rcx]
	mov		ebx,DWORD PTR [rbx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp[rsp],eax
ylrga:
	test	edx,2
	jz		ylrgb
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 4[rcx]
	mov		ebx,DWORD PTR 4[rbx]
	mov		ecx,DWORD PTR p_inty+4[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+4[rsp],eax
ylrgb:
	test	edx,4
	jz		ylrgc
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 8[rcx]
	mov		ebx,DWORD PTR 8[rbx]
	mov		ecx,DWORD PTR p_inty+8[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+8[rsp],eax
ylrgc:
	test	edx,8
	jz		ylrgd
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 12[rcx]
	mov		ebx,DWORD PTR 12[rbx]
	mov		ecx,DWORD PTR p_inty+12[rsp]
	sub		rsp,8
	call	np_special6					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+12[rsp],eax
ylrgd:
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
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR [rcx]
	mov		ebx,DWORD PTR [rbx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp[rsp],eax
xinfa:
	test	edx,2
	jz		xinfb
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 4[rcx]
	mov		ebx,DWORD PTR 4[rbx]
	mov		ecx,DWORD PTR p_inty+4[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+4[rsp],eax
xinfb:
	test	edx,4
	jz		xinfc
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 8[rcx]
	mov		ebx,DWORD PTR 8[rbx]
	mov		ecx,DWORD PTR p_inty+8[rsp]
	sub		rsp,8
	call	np_special_x1					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+8[rsp],eax
xinfc:
	test	edx,8
	jz		xinfd
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 12[rcx]
	mov		ebx,DWORD PTR 12[rbx]
	mov		ecx,DWORD PTR p_inty+12[rsp]
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
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR [rcx]
	mov		ebx,DWORD PTR [rbx]
	mov		ecx,DWORD PTR p_inty[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp[rsp],eax
xzera:
	test	edx,2
	jz		xzerb
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 4[rcx]
	mov		ebx,DWORD PTR 4[rbx]
	mov		ecx,DWORD PTR p_inty+4[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+4[rsp],eax
xzerb:
	test	edx,4
	jz		xzerc
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 8[rcx]
	mov		ebx,DWORD PTR 8[rbx]
	mov		ecx,DWORD PTR p_inty+8[rsp]
	sub		rsp,8
	call	np_special_x2					; call the handler for one value
	add		rsp,8
	mov		DWORD PTR p_temp+8[rsp],eax
xzerc:
	test	edx,8
	jz		xzerd
	mov		rcx,QWORD PTR p_xptr[rsp]		; get pointer to x
	mov		rbx,QWORD PTR p_yptr[rsp]		; get pointer to y
	mov		eax,DWORD PTR 12[rcx]
	mov		ebx,DWORD PTR 12[rbx]
	mov		ecx,DWORD PTR p_inty+12[rsp]
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
