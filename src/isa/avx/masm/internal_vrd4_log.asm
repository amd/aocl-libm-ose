;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; vrd4log.asm
;
; A vector implementation of the log libm function.
;
; Prototype:
;
;    __m128d,__m128d __vrd4_log(__m128d x1, __m128d x2);
;
;   Computes the natural log of x.
;   Returns proper C99 values, but may not raise status flags properly.
;   Less than 1 ulp of error.  This version can compute 4 logs in
;   192 cycles, or 48 per value
;
; This routine computes 4 double precision log values at a time.
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

CONST	SEGMENT
__real_one				DQ 03ff0000000000000h	; 1.0
						dq 03ff0000000000000h					; for alignment
__real_two				DQ 04000000000000000h	; 1.0
						dq 04000000000000000h
__real_ninf				DQ 0fff0000000000000h	; -inf
						dq 0fff0000000000000h
__real_inf				DQ 07ff0000000000000h	; +inf
						dq 07ff0000000000000h
__real_nan				DQ 07ff8000000000000h	; NaN
						dq 07ff8000000000000h

__real_zero				DQ 00000000000000000h	; 0.0
						dq 00000000000000000h

__real_sign				DQ 08000000000000000h	; sign bit
						dq 08000000000000000h
__real_notsign			DQ 07ffFFFFFFFFFFFFFh	; ^sign bit
						dq 07ffFFFFFFFFFFFFFh
__real_threshold		DQ 03F9EB85000000000h	; .03
						dq 03F9EB85000000000h
__real_qnanbit			DQ 00008000000000000h	; quiet nan bit
						dq 00008000000000000h
__real_mant				DQ 0000FFFFFFFFFFFFFh	; mantissa bits
						dq 0000FFFFFFFFFFFFFh
__real_3f80000000000000	DQ 03f80000000000000h	; /* 0.0078125 = 1/128 */
						dq 03f80000000000000h
__mask_1023				DQ 000000000000003ffh	; 
						dq 000000000000003ffh
__mask_040				DQ 00000000000000040h	; 
						dq 00000000000000040h
__mask_001				DQ 00000000000000001h	; 
						dq 00000000000000001h

__real_ca1				DQ 03fb55555555554e6h	; 8.33333333333317923934e-02
						dq 03fb55555555554e6h
__real_ca2				DQ 03f89999999bac6d4h	; 1.25000000037717509602e-02
						dq 03f89999999bac6d4h
__real_ca3				DQ 03f62492307f1519fh	; 2.23213998791944806202e-03
						dq 03f62492307f1519fh
__real_ca4				DQ 03f3c8034c85dfff0h	; 4.34887777707614552256e-04
						dq 03f3c8034c85dfff0h


__real_cb1				DQ 03fb5555555555557h	; 8.33333333333333593622e-02
						dq 03fb5555555555557h
__real_cb2				DQ 03f89999999865edeh	; 1.24999999978138668903e-02
						dq 03f89999999865edeh
__real_cb3				DQ 03f6249423bd94741h	; 2.23219810758559851206e-03
						dq 03f6249423bd94741h
__real_log2_lead  		DQ 03fe62e42e0000000h	; log2_lead	  6.93147122859954833984e-01
						dq 03fe62e42e0000000h
__real_log2_tail 		DQ 03e6efa39ef35793ch	; log2_tail	  5.76999904754328540596e-08
						dq 03e6efa39ef35793ch

__real_half				DQ 03fe0000000000000h	; 1/2
						dq 03fe0000000000000h


__np_ln_lead_table:
	DQ	0000000000000000h 		; 0.00000000000000000000e+00
	DQ	3f8fc0a800000000h		; 1.55041813850402832031e-02
	DQ	3f9f829800000000h		; 3.07716131210327148438e-02
	DQ	3fa7745800000000h		; 4.58095073699951171875e-02
	DQ	3faf0a3000000000h		; 6.06245994567871093750e-02
	DQ	3fb341d700000000h		; 7.52233862876892089844e-02
	DQ	3fb6f0d200000000h		; 8.96121263504028320312e-02
	DQ	3fba926d00000000h		; 1.03796780109405517578e-01
	DQ	3fbe270700000000h		; 1.17783010005950927734e-01
	DQ	3fc0d77e00000000h		; 1.31576299667358398438e-01
	DQ	3fc2955280000000h		; 1.45181953907012939453e-01
	DQ	3fc44d2b00000000h		; 1.58604979515075683594e-01
	DQ	3fc5ff3000000000h		; 1.71850204467773437500e-01
	DQ	3fc7ab8900000000h		; 1.84922337532043457031e-01
	DQ	3fc9525a80000000h		; 1.97825729846954345703e-01
	DQ	3fcaf3c900000000h		; 2.10564732551574707031e-01
	DQ	3fcc8ff780000000h		; 2.23143517971038818359e-01
	DQ	3fce270700000000h		; 2.35566020011901855469e-01
	DQ	3fcfb91800000000h		; 2.47836112976074218750e-01
	DQ	3fd0a324c0000000h		; 2.59957492351531982422e-01
	DQ	3fd1675c80000000h		; 2.71933674812316894531e-01
	DQ	3fd22941c0000000h		; 2.83768117427825927734e-01
	DQ	3fd2e8e280000000h		; 2.95464158058166503906e-01
	DQ	3fd3a64c40000000h		; 3.07025015354156494141e-01
	DQ	3fd4618bc0000000h		; 3.18453729152679443359e-01
	DQ	3fd51aad80000000h		; 3.29753279685974121094e-01
	DQ	3fd5d1bd80000000h		; 3.40926527976989746094e-01
	DQ	3fd686c800000000h		; 3.51976394653320312500e-01
	DQ	3fd739d7c0000000h		; 3.62905442714691162109e-01
	DQ	3fd7eaf800000000h		; 3.73716354370117187500e-01
	DQ	3fd89a3380000000h		; 3.84411692619323730469e-01
	DQ	3fd9479400000000h		; 3.94993782043457031250e-01
	DQ	3fd9f323c0000000h		; 4.05465066432952880859e-01
	DQ	3fda9cec80000000h		; 4.15827870368957519531e-01
	DQ	3fdb44f740000000h		; 4.26084339618682861328e-01
	DQ	3fdbeb4d80000000h		; 4.36236739158630371094e-01
	DQ	3fdc8ff7c0000000h		; 4.46287095546722412109e-01
	DQ	3fdd32fe40000000h		; 4.56237375736236572266e-01
	DQ	3fddd46a00000000h		; 4.66089725494384765625e-01
	DQ	3fde744240000000h		; 4.75845873355865478516e-01
	DQ	3fdf128f40000000h		; 4.85507786273956298828e-01
	DQ	3fdfaf5880000000h		; 4.95077252388000488281e-01
	DQ	3fe02552a0000000h		; 5.04556000232696533203e-01
	DQ	3fe0723e40000000h		; 5.13945698738098144531e-01
	DQ	3fe0be72e0000000h		; 5.23248136043548583984e-01
	DQ	3fe109f380000000h		; 5.32464742660522460938e-01
	DQ	3fe154c3c0000000h		; 5.41597247123718261719e-01
	DQ	3fe19ee6a0000000h		; 5.50647079944610595703e-01
	DQ	3fe1e85f40000000h		; 5.59615731239318847656e-01
	DQ	3fe23130c0000000h		; 5.68504691123962402344e-01
	DQ	3fe2795e00000000h		; 5.77315330505371093750e-01
	DQ	3fe2c0e9e0000000h		; 5.86049020290374755859e-01
	DQ	3fe307d720000000h		; 5.94707071781158447266e-01
	DQ	3fe34e2880000000h		; 6.03290796279907226562e-01
	DQ	3fe393e0c0000000h		; 6.11801505088806152344e-01
	DQ	3fe3d90260000000h		; 6.20240390300750732422e-01
	DQ	3fe41d8fe0000000h		; 6.28608644008636474609e-01
	DQ	3fe4618bc0000000h		; 6.36907458305358886719e-01
	DQ	3fe4a4f840000000h		; 6.45137906074523925781e-01
	DQ	3fe4e7d800000000h		; 6.53301239013671875000e-01
	DQ	3fe52a2d20000000h		; 6.61398470401763916016e-01
	DQ	3fe56bf9c0000000h		; 6.69430613517761230469e-01
	DQ	3fe5ad4040000000h		; 6.77398800849914550781e-01
	DQ	3fe5ee02a0000000h		; 6.85303986072540283203e-01
	DQ	3fe62e42e0000000h		; 6.93147122859954833984e-01
	dq 0					; for alignment

__np_ln_tail_table:
	DQ	00000000000000000h ; 0	; 0.00000000000000000000e+00
	DQ	03e361f807c79f3dbh		; 5.15092497094772879206e-09
	DQ	03e6873c1980267c8h		; 4.55457209735272790188e-08
	DQ	03e5ec65b9f88c69eh		; 2.86612990859791781788e-08
	DQ	03e58022c54cc2f99h		; 2.23596477332056055352e-08
	DQ	03e62c37a3a125330h		; 3.49498983167142274770e-08
	DQ	03e615cad69737c93h		; 3.23392843005887000414e-08
	DQ	03e4d256ab1b285e9h		; 1.35722380472479366661e-08
	DQ	03e5b8abcb97a7aa2h		; 2.56504325268044191098e-08
	DQ	03e6f34239659a5dch		; 5.81213608741512136843e-08
	DQ	03e6e07fd48d30177h		; 5.59374849578288093334e-08
	DQ	03e6b32df4799f4f6h		; 5.06615629004996189970e-08
	DQ	03e6c29e4f4f21cf8h		; 5.24588857848400955725e-08
	DQ	03e1086c848df1b59h		; 9.61968535632653505972e-10
	DQ	03e4cf456b4764130h		; 1.34829655346594463137e-08
	DQ	03e63a02ffcb63398h		; 3.65557749306383026498e-08
	DQ	03e61e6a6886b0976h		; 3.33431709374069198903e-08
	DQ	03e6b8abcb97a7aa2h		; 5.13008650536088382197e-08
	DQ	03e6b578f8aa35552h		; 5.09285070380306053751e-08
	DQ	03e6139c871afb9fch		; 3.20853940845502057341e-08
	DQ	03e65d5d30701ce64h		; 4.06713248643004200446e-08
	DQ	03e6de7bcb2d12142h		; 5.57028186706125221168e-08
	DQ	03e6d708e984e1664h		; 5.48356693724804282546e-08
	DQ	03e556945e9c72f36h		; 1.99407553679345001938e-08
	DQ	03e20e2f613e85bdah		; 1.96585517245087232086e-09
	DQ	03e3cb7e0b42724f6h		; 6.68649386072067321503e-09
	DQ	03e6fac04e52846c7h		; 5.89936034642113390002e-08
	DQ	03e5e9b14aec442beh		; 2.85038578721554472484e-08
	DQ	03e6b5de8034e7126h		; 5.09746772910284482606e-08
	DQ	03e6dc157e1b259d3h		; 5.54234668933210171467e-08
	DQ	03e3b05096ad69c62h		; 6.29100830926604004874e-09
	DQ	03e5c2116faba4cddh		; 2.61974119468563937716e-08
	DQ	03e665fcc25f95b47h		; 4.16752115011186398935e-08
	DQ	03e5a9a08498d4850h		; 2.47747534460820790327e-08
	DQ	03e6de647b1465f77h		; 5.56922172017964209793e-08
	DQ	03e5da71b7bf7861dh		; 2.76162876992552906035e-08
	DQ	03e3e6a6886b09760h		; 7.08169709942321478061e-09
	DQ	03e6f0075eab0ef64h		; 5.77453510221151779025e-08
	DQ	03e33071282fb989bh		; 4.43021445893361960146e-09
	DQ	03e60eb43c3f1bed2h		; 3.15140984357495864573e-08
	DQ	03e5faf06ecb35c84h		; 2.95077445089736670973e-08
	DQ	03e4ef1e63db35f68h		; 1.44098510263167149349e-08
	DQ	03e469743fb1a71a5h		; 1.05196987538551827693e-08
	DQ	03e6c1cdf404e5796h		; 5.23641361722697546261e-08
	DQ	03e4094aa0ada625eh		; 7.72099925253243069458e-09
	DQ	03e6e2d4c96fde3ech		; 5.62089493829364197156e-08
	DQ	03e62f4d5e9a98f34h		; 3.53090261098577946927e-08
	DQ	03e6467c96ecc5cbeh		; 3.80080516835568242269e-08
	DQ	03e6e7040d03dec5ah		; 5.66961038386146408282e-08
	DQ	03e67bebf4282de36h		; 4.42287063097349852717e-08
	DQ	03e6289b11aeb783fh		; 3.45294525105681104660e-08
	DQ	03e5a891d1772f538h		; 2.47132034530447431509e-08
	DQ	03e634f10be1fb591h		; 3.59655343422487209774e-08
	DQ	03e6d9ce1d316eb93h		; 5.51581770357780862071e-08
	DQ	03e63562a19a9c442h		; 3.60171867511861372793e-08
	DQ	03e54e2adf548084ch		; 1.94511067964296180547e-08
	DQ	03e508ce55cc8c97ah		; 1.54137376631349347838e-08
	DQ	03e30e2f613e85bdah		; 3.93171034490174464173e-09
	DQ	03e6db03ebb0227bfh		; 5.52990607758839766440e-08
	DQ	03e61b75bb09cb098h		; 3.29990737637586136511e-08
	DQ	03e496f16abb9df22h		; 1.18436010922446096216e-08
	DQ	03e65b3f399411c62h		; 4.04248680368301346709e-08
	DQ	03e586b3e59f65355h		; 2.27418915900284316293e-08
	DQ	03e52482ceae1ac12h		; 1.70263791333409206020e-08
	DQ	03e6efa39ef35793ch		; 5.76999904754328540596e-08
	dq 0					; for alignment
CONST	ENDS


; define local variable storage offsets
p_x				equ		0			; temporary for error checking operation
p_idx			equ		010h		; index storage
p_xexp			equ		020h		; index storage

p_x2			equ		030h		; temporary for error checking operation
p_idx2			equ		040h		; index storage
p_xexp2			equ		050h		; index storage

save_xa			equ		060h		;qword
save_ya			equ		068h		;qword
save_nv			equ		070h		;qword
p_iter			equ		078h		; qword	storage for number of loop iterations

save_rbx		equ		080h		;qword
save_rdi		equ		088h		;qword

save_rsi		equ		090h		;qword


save_xmm6		equ		0a0h		;xmmword
save_xmm7		equ		0b0h		;xmmword
save_xmm8		equ		0c0h		;xmmword
save_xmm9		equ		0d0h		;xmmword


p2_temp			equ		0e0h		; second temporary for get/put bits operation
p2_temp1		equ		0f0h		; second temporary for exponent multiply

p_n1			equ		0100h		; temporary for near one check
p_n12			equ		0108h		; temporary for near one check


stack_size		equ		0118h

fname		textequ		<__amd_internal_vrd4_log>

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
	movdqa	XMMWORD PTR [rsp+save_xmm7],xmm7	; save xmm7
	.SAVEXMM128 xmm7, save_xmm7
	movdqa	XMMWORD PTR [rsp+save_xmm8],xmm8	; save xmm8
	.SAVEXMM128 xmm8, save_xmm8
	movdqa	XMMWORD PTR [rsp+save_xmm9],xmm9	; save xmm9
	.SAVEXMM128 xmm9, save_xmm9
	.ENDPROLOG

; process 4 values at a time.

;		movapd	xmm7,xmm1
		movdqa	XMMWORD PTR [rsp+p_x2],xmm1	; save the input values
	movdqa	XMMWORD PTR [rsp+p_x],xmm0	; save the input values
; compute the logs

;  if NaN or inf

;      /* Store the exponent of x in xexp and put
;         f into the range [0.5,1) */

	pxor	xmm1,xmm1
;	movdqa	xmm3,XMMWORD PTR [rsp+p_x]
	movdqa	xmm3,xmm0
	psrlq	xmm3,52
	psubq	xmm3,QWORD PTR __mask_1023
	packssdw	xmm3,xmm1
	cvtdq2pd	xmm6,xmm3			; xexp
	movdqa	xmm2,xmm0
	subpd	xmm2,QWORD PTR __real_one
	movapd	XMMWORD PTR [rsp+p_xexp],xmm6
	andpd	xmm2,QWORD PTR __real_notsign
	xor		rax,rax
; do near one check
;
	movdqa	xmm3,xmm0
	pand	xmm3,XMMWORD PTR __real_mant

	cmppd	xmm2,XMMWORD PTR __real_threshold,1
	movmskpd	ecx,xmm2
	movdqa	xmm4,xmm3
	mov			DWORD PTR [rsp+p_n1],ecx

;/* Now  x = 2**xexp  * f,  1/2 <= f < 1. */
	psrlq	xmm3,45
	movdqa	xmm2,xmm3
	psrlq	xmm3,1
	paddq	xmm3,QWORD PTR __mask_040
	pand	xmm2,XMMWORD PTR __mask_001
	paddq	xmm3,xmm2

	packssdw	xmm3,xmm1
	cvtdq2pd	xmm1,xmm3
		pxor	xmm7,xmm7
		movdqa	xmm2, XMMWORD PTR [rsp+p_x2]
		movapd	xmm5,XMMWORD PTR [rsp+p_x2]
		psrlq	xmm2,52
		psubq	xmm2,QWORD PTR __mask_1023
		packssdw	xmm2,xmm7
		subpd	xmm5,QWORD PTR __real_one
		andpd	xmm5,QWORD PTR __real_notsign
		cvtdq2pd	xmm6,xmm2			; xexp
	xor		rcx,rcx
		cmppd	xmm5,XMMWORD PTR __real_threshold,1
	movq	QWORD PTR p_idx[rsp],xmm3

; reduce and get u
	por		xmm4, XMMWORD PTR __real_half
	movdqa	xmm2,xmm4
		movapd	XMMWORD PTR [rsp+p_xexp2],xmm6

	; do near one check
		movmskpd	edx,xmm5
		mov			DWORD PTR [rsp+p_n12],edx

	mulpd	xmm1, QWORD PTR __real_3f80000000000000				; f1 = index/128


	lea		rdx,__np_ln_lead_table
	mov		eax,DWORD PTR p_idx[rsp]
		movdqa	xmm6,XMMWORD PTR [rsp+p_x2]

	movapd	xmm5, QWORD PTR __real_half							; .5
	subpd	xmm2,xmm1											; f2 = f - f1
		pand	xmm6,XMMWORD PTR __real_mant
	mulpd	xmm5,xmm2
	addpd	xmm1,xmm5

		movdqa	xmm8,xmm6
		psrlq	xmm6,45
		movdqa	xmm4,xmm6

		psrlq	xmm6,1
		paddq	xmm6,QWORD PTR __mask_040
		pand	xmm4,XMMWORD PTR __mask_001
		paddq	xmm6,xmm4
; do error checking here for scheduling.  Saves a bunch of cycles as
; compared to doing this at the start of the routine.
;  if NaN or inf
	movapd	xmm3,xmm0
	andpd	xmm3,QWORD PTR __real_inf
	cmppd	xmm3,XMMWORD PTR __real_inf,0
	movmskpd	r8d,xmm3
; delaying this divide helps, but moving the other one does not.
; it was after the paddq
;	divpd	xmm2,xmm1				; u
;	xorpd	xmm1,xmm1
		packssdw	xmm6,xmm7
		por		xmm8, XMMWORD PTR __real_half
		movq	QWORD PTR p_idx2[rsp],xmm6
		cvtdq2pd	xmm9,xmm6

	divpd	xmm2,xmm1				; u
	cmppd	xmm0,QWORD PTR __real_zero,2
		mulpd	xmm9, QWORD PTR __real_3f80000000000000				; f1 = index/128
	movmskpd	r9d,xmm0

; compute the index into the log tables
;

	movlpd	xmm0, QWORD PTR [rdx+rax*8-512]		; z1
	mov		ecx,DWORD PTR p_idx[rsp+4]
	movhpd	xmm0, QWORD PTR [rdx+rcx*8-512]		; z1
; solve for ln(1+u)
	movapd	xmm1,xmm2				; u
	mulpd	xmm2,xmm2				; u^2
	movapd	xmm5,xmm2
	movapd	xmm3,QWORD PTR __real_cb3
	mulpd	xmm3,xmm2				;Cu2
	mulpd	xmm5,xmm1				; u^3
	addpd	xmm3,QWORD PTR __real_cb2 ;B+Cu2

	mulpd	xmm2,xmm5				; u^5
	movapd	xmm4, QWORD PTR __real_log2_lead

	mulpd	xmm5,QWORD PTR __real_cb1 ;Au3
	addpd	xmm1,xmm5				; u+Au3
	mulpd	xmm2,xmm3				; u5(B+Cu2)

	movapd	xmm5,XMMWORD PTR [rsp+p_xexp]		; xexp 
	addpd	xmm1,xmm2				; poly
; recombine		
	mulpd	xmm4,xmm5				; xexp * log2_lead
	addpd	xmm0,xmm4				;r1
	lea		rdx,__np_ln_tail_table
	movlpd	xmm4, QWORD PTR [rdx+rax*8-512]		;z2	+=q
	movhpd	xmm4, QWORD PTR [rdx+rcx*8-512]		;z2	+=q
		lea		rdx,__np_ln_lead_table
		mov		eax,DWORD PTR p_idx2[rsp]
		mov		ecx,DWORD PTR p_idx2[rsp+4]
	addpd	xmm1,xmm4

	mulpd	xmm5,QWORD PTR __real_log2_tail

		movapd	xmm4, QWORD PTR __real_half							; .5
		subpd	xmm8,xmm9											; f2 = f - f1
		mulpd	xmm4,xmm8
		addpd	xmm9,xmm4

	addpd	xmm1,xmm5				;r2
		divpd	xmm8,xmm9				; u
		movapd	xmm3,XMMWORD PTR [rsp+p_x2]
		andpd	xmm3,QWORD PTR __real_inf
		cmppd	xmm3,XMMWORD PTR __real_inf,0
		movmskpd	r10d,xmm3
;		xorpd	xmm9,xmm9
		movapd	xmm6,XMMWORD PTR [rsp+p_x2]
		cmppd	xmm6,XMMWORD PTR __real_zero,2
		movmskpd	r11d,xmm6

; check for negative numbers or zero
	test		r9d,3
	addpd	xmm0,xmm1
	jnz		__z_or_n 
__vlog1:


; check for nans/infs
	test		r8d,3
	jnz		__log_naninf

__vlog2:


	; It seems like a good idea to try and interleave
	; even more of the following code sooner into the
	; program.  But there were conflicts with the table
	; index registers, making the problem difficult.
	; After a lot of work in a branch of this file,
	; I was not able to match the speed of this version.
	; CodeAnalyst shows that there is lots of unused add
	; pipe time around the divides, but the processor
	; doesn't seem to be able to schedule in those slots.

		movlpd	xmm7, QWORD PTR [rdx+rax*8-512]		;z2	+=q
		movhpd	xmm7, QWORD PTR [rdx+rcx*8-512]		;z2	+=q

; check for near one
	mov			r9d,DWORD PTR [rsp+p_n1]
	test			r9d,3
	jnz			__near_one1
__vlog2n:


	; solve for ln(1+u)
		movapd	xmm9,xmm8				; u
		mulpd	xmm8,xmm8				; u^2
		movapd	xmm5,xmm8
		movapd	xmm3,QWORD PTR __real_cb3
		mulpd	xmm3,xmm8				;Cu2
		mulpd	xmm5,xmm9				; u^3
		addpd	xmm3,QWORD PTR __real_cb2 ;B+Cu2

		mulpd	xmm8,xmm5				; u^5
		movapd	xmm4, QWORD PTR __real_log2_lead

		mulpd	xmm5,QWORD PTR __real_cb1 ;Au3
		addpd	xmm9,xmm5				; u+Au3
		mulpd	xmm8,xmm3				; u5(B+Cu2)

		movapd	xmm5,XMMWORD PTR [rsp+p_xexp2]		; xexp 
		addpd	xmm9,xmm8				; poly
	; recombine		
		mulpd	xmm4,xmm5
		addpd	xmm7,xmm4				;r1

		lea		rdx,__np_ln_tail_table
		movlpd	xmm2, QWORD PTR [rdx+rax*8-512]		;z2	+=q
		movhpd	xmm2, QWORD PTR [rdx+rcx*8-512]		;z2	+=q
		addpd	xmm9,xmm2

		mulpd	xmm5,QWORD PTR __real_log2_tail

		addpd	xmm9,xmm5				;r2

	; check for nans/infs
		test		r10d,3
		addpd	xmm7,xmm9
		jnz		__log_naninf2
__vlog3:
; check for negative numbers or zero
		test		r11d,3
		jnz		__z_or_n2 

__vlog4:


	mov			r9d,DWORD PTR [rsp+p_n12]
	test			r9d,3
	jnz			__near_one2

__vlog4n:

; store the result _m128d
		movapd	xmm1,xmm7


__finish:
	movdqa	xmm9,XMMWORD PTR [rsp+save_xmm9]	; restore xmm9
	movdqa	xmm8,XMMWORD PTR [rsp+save_xmm8]	; restore xmm8
	movdqa	xmm7,XMMWORD PTR [rsp+save_xmm7]	; restore xmm7
	movdqa	xmm6,XMMWORD PTR [rsp+save_xmm6]	; restore xmm6
	mov		rdi,[rsp+save_rdi]		; restore rdi
	mov		rsi,[rsp+save_rsi]		; restore rsi
	mov		rbx,[rsp+save_rbx]		; restore rbx
	add		rsp,stack_size
	ret

	align	16
both_nearone:
; saves 10 cycles
;      r = x - 1.0;
	movapd	xmm2, XMMWORD PTR __real_two
	subpd	xmm0, XMMWORD PTR __real_one	   ; r
;      u          = r / (2.0 + r);
	addpd	xmm2,xmm0
	movapd	xmm1,xmm0
	divpd	xmm1,xmm2		; u
	movapd	xmm4, XMMWORD PTR __real_ca4	  ;D
	movapd	xmm5, XMMWORD PTR __real_ca3	  ;C
;      correction = r * u;
	movapd	xmm6,xmm0
	mulpd	xmm6,xmm1		; correction
;      u          = u + u;
	addpd	xmm1,xmm1		;u
	movapd	xmm2,xmm1
	mulpd	xmm2,xmm2		;v =u^2
;      r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction);
	mulpd	xmm5,xmm1		; Cu
	movapd	xmm3,xmm1
	mulpd	xmm3,xmm2		; u^3
	mulpd	xmm2, XMMWORD PTR __real_ca2	;Bu^2
	mulpd	xmm4, xmm3		;Du^3

	addpd	xmm2, XMMWORD PTR __real_ca1	; +A
	movapd	xmm1,xmm3
	mulpd	xmm1,xmm1		; u^6
	addpd	xmm5,xmm4		;Cu+Du3

	mulpd	xmm2,xmm3		;u3(A+Bu2)
	mulpd	xmm1,xmm5		;u6(Cu+Du3)
	addpd	xmm2,xmm1
	subpd	xmm2,xmm6		; -correction
	
;      return r + r2;
	addpd	xmm0,xmm2
	ret

	align	16
__near_one1:
	cmp	r9d,3
	jnz		__n1nb1

	movapd	xmm0,QWORD PTR [rsp+p_x]
	call	both_nearone
	jmp		__vlog2n

	align	16
__n1nb1:
	test	r9d,1
	jz		__lnn12

	movlpd	xmm0,QWORD PTR [rsp+p_x]
	call	__ln1

__lnn12:
	test	r9d,2		; second number?
	jz		__lnn1e
	movlpd	QWORD PTR [rsp+p_x],xmm0
	movlpd	xmm0,QWORD PTR [rsp+p_x+8]
	call	__ln1
	movlpd	QWORD PTR [rsp+p_x+8],xmm0
	movapd	xmm0,QWORD PTR [rsp+p_x]

__lnn1e:
	jmp		__vlog2n


	align	16
__near_one2:
	cmp	r9d,3
	jnz		__n1nb2

	movapd	xmm8,xmm0
	movapd	xmm0,QWORD PTR [rsp+p_x2]
	call	both_nearone
	movapd	xmm7,xmm0
	movapd	xmm0,xmm8
	jmp		__vlog4n

	align	16
__n1nb2:
	movapd	xmm8,xmm0
	test	r9d,1
	jz		__lnn22

	movapd	xmm0,xmm7
	movlpd	xmm0,QWORD PTR [rsp+p_x2]
	call	__ln1
	movapd	xmm7,xmm0

__lnn22:
	test	r9d,2		; second number?
	jz		__lnn2e
	movlpd	QWORD PTR [rsp+p_x2],xmm7
	movlpd	xmm0,QWORD PTR [rsp+p_x2+8]
	call	__ln1
	movlpd	QWORD PTR [rsp+p_x2+8],xmm0
	movapd	xmm7,QWORD PTR [rsp+p_x2]

__lnn2e:
	movapd	xmm0,xmm8
	jmp		__vlog4n

	align	16

__ln1:
; saves 10 cycles
;      r = x - 1.0;
	movlpd	xmm2, QWORD PTR __real_two
	subsd	xmm0, QWORD PTR __real_one	   ; r
;      u          = r / (2.0 + r);
	addsd	xmm2,xmm0
	movsd	xmm1,xmm0
	divsd	xmm1,xmm2		; u
	movlpd	xmm4, QWORD PTR __real_ca4	  ;D
	movlpd	xmm5, QWORD PTR __real_ca3	  ;C
;      correction = r * u;
	movsd	xmm6,xmm0
	mulsd	xmm6,xmm1		; correction
;      u          = u + u;
	addsd	xmm1,xmm1		;u
	movsd	xmm2,xmm1
	mulsd	xmm2,xmm2		;v =u^2
;      r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction);
	mulsd	xmm5,xmm1		; Cu
	movsd	xmm3,xmm1
	mulsd	xmm3,xmm2		; u^3
	mulsd	xmm2, QWORD PTR __real_ca2	;Bu^2
	mulsd	xmm4, xmm3		;Du^3

	addsd	xmm2, QWORD PTR __real_ca1	; +A
	movsd	xmm1,xmm3
	mulsd	xmm1,xmm1		; u^6
	addsd	xmm5,xmm4		;Cu+Du3

	mulsd	xmm2,xmm3		;u3(A+Bu2)
	mulsd	xmm1,xmm5		;u6(Cu+Du3)
	addsd	xmm2,xmm1
	subsd	xmm2,xmm6		; -correction
	
;      return r + r2;
	addsd	xmm0,xmm2
	ret

	align	16

; at least one of the numbers was a nan or infinity
__log_naninf:
	test		r8d,1		; first number?
	jz		__lninf2

	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	movapd	xmm1,xmm0		; save the inputs
	mov		rdx,QWORD PTR [rsp+p_x]
	movlpd	xmm0,QWORD PTR [rsp+p_x]
	call	__lni
	shufpd	xmm0,xmm1,2
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__lninf2:
	test		r8d,2		; second number?
	jz		__lninfe
	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	movapd	xmm1,xmm0		; save the inputs
	mov		rdx,QWORD PTR [rsp+p_x+8]
	movlpd	xmm0,QWORD PTR [rsp+p_x+8]
	call	__lni
	shufpd	xmm1,xmm0,0
	movapd	xmm0,xmm1
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__lninfe:
	jmp		__vlog2		; continue processing if not

; at least one of the numbers was a nan or infinity
__log_naninf2:
	movapd	xmm2,xmm0
	test		r10d,1		; first number?
	jz		__lninf22

	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	movapd	xmm1,xmm7		; save the inputs
	mov		rdx,QWORD PTR [rsp+p_x2]
	movlpd	xmm0,QWORD PTR [rsp+p_x2]
	call	__lni
	shufpd	xmm0,xmm7,2
	movapd	xmm7,xmm0
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__lninf22:
	test		r10d,2		; second number?
	jz		__lninfe2

	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	mov		rdx,QWORD PTR [rsp+p_x2+8]
	movlpd	xmm0,QWORD PTR [rsp+p_x2+8]
	call	__lni
	shufpd	xmm7,xmm0,0
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__lninfe2:
	movapd	xmm0,xmm2
	jmp		__vlog3		; continue processing if not

; a subroutine to treat one number for nan/infinity
; the number is expected in rdx and returned in the low
; half of xmm0
__lni:
	mov		rax,0000FFFFFFFFFFFFFh
	test	rdx,rax
	jnz		__lnan					; jump if mantissa not zero, so it's a NaN
; inf
	rcl		rdx,1
	jnc		__lne2					; log(+inf) = inf
; negative x
	movlpd	xmm0,__real_nan
	ret

;NaN
__lnan:	
	mov		rax,00008000000000000h	; convert to quiet
	or		rdx,rax
__lne:	
	movd	xmm0,rdx
__lne2:	
	ret

	align	16

; at least one of the numbers was a zero, a negative number, or both.
__z_or_n:
	test		r9d,1		; first number?
	jz		__zn2

	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	movapd	xmm1,xmm0		; save the inputs
	mov		rax,QWORD PTR [rsp+p_x]
	call	__zni
	shufpd	xmm0,xmm1,2
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__zn2:
	test		r9d,2		; second number?
	jz		__zne
	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	movapd	xmm1,xmm0		; save the inputs
	mov		rax,QWORD PTR [rsp+p_x+8]
	call	__zni
	shufpd	xmm1,xmm0,0
	movapd	xmm0,xmm1
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__zne:
	jmp		__vlog1		

__z_or_n2:
	movapd	xmm2,xmm0
	test		r11d,1		; first number?
	jz		__zn22

;	movapd	xmm1,xmm7		; save the inputs
	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	mov		rax,QWORD PTR [rsp+p_x2]
	call	__zni
	shufpd	xmm0,xmm7,2
	movapd	xmm7,xmm0
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__zn22:
	test		r11d,2		; second number?
	jz		__zne2

	mov		QWORD PTR [rsp+p2_temp],rax
	mov		QWORD PTR [rsp+p2_temp+8],rdx
	mov		rax,QWORD PTR [rsp+p_x2+8]
	call	__zni
	shufpd	xmm7,xmm0,0
	mov		rax,QWORD PTR [rsp+p2_temp]
	mov		rdx,QWORD PTR [rsp+p2_temp+8]

__zne2:
	movapd	xmm0,xmm2
	jmp		__vlog4		
; a subroutine to treat one number for zero or negative values
; the number is expected in rax and returned in the low
; half of xmm0
__zni:
	shl		rax,1
	jnz		__zn_x		 ; if just a carry, then must be negative
	movlpd	xmm0,__real_ninf  ; C99 specs -inf for +-0
	ret
__zn_x:
	movlpd	xmm0,__real_nan
	ret


fname		endp


TEXT	ENDS
END
