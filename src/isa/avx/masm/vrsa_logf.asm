;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; vrsa_logf.S
;
; An implementation of the vrs4_logf libm function.
;
; Prototype:
;
;     void vrsa_logf(int len, float *src,float *dst);
;
;
;   Algorithm:
;
;   Based on:
;   Ping-Tak Peter Tang
;   "Table-driven implementation of the logarithm function in IEEE
;   floating-point arithmetic"
;   ACM Transactions on Mathematical Software (TOMS)
;   Volume 16, Issue 4 (December 1990)
;
;
;

include fm.inc
include log_tables.inc

; local variable storage offsets if any
save_xmm6       EQU     00h
save_xmm7       EQU     010h
save_xmm8       EQU     020h
save_xmm9       EQU     030h
save_xmm10      EQU     040h
save_xmm11      EQU     050h
save_xmm12      EQU     060h
save_xmm13      EQU     070h
save_xmm14      EQU     080h
save_xmm15      EQU     090h
save_rdi        EQU     0A0h
save_rsi        EQU     0B0h
save_rbx        EQU     0C0h
save_rbp        EQU     0D0h
stack_size      EQU     0E8h ; We take 8 as the last nibble to allow for 
                             ; alligned data movement.



FN_PROTOTYPE_BAS64 vrsa_logf





StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

StackDeallocate   MACRO size
    add         rsp, size
ENDM

SaveReg         MACRO reg64, offset
	mov	QWORD PTR [rsp+offset], reg64	; save rsi
	.SAVEREG reg64, offset
ENDM

RestoreReg         MACRO reg64, offset
	mov	reg64, QWORD PTR [rsp+offset]; save rsi
ENDM

SaveXmm         MACRO xmmreg, offset
                    movdqa      XMMWORD PTR [offset+rsp], xmmreg
                    .SAVEXMM128 xmmreg, offset
ENDM

RestoreXmm      MACRO xmmreg, offset
                    movdqa      xmmreg, XMMWORD PTR [offset+rsp]
ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveXmm xmm6, save_xmm6
    SaveXmm xmm7, save_xmm7
    SaveXmm xmm8, save_xmm8
    ;SaveXmm xmm9, save_xmm9
    ;SaveXmm xmm10, save_xmm10
    SaveXmm xmm11, save_xmm11
    ;SaveXmm xmm12, save_xmm12
    SaveXmm xmm13, save_xmm13
    ;SaveXmm xmm14, save_xmm14
    SaveXmm xmm15, save_xmm15    
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi
    
    .ENDPROLOG
    
    mov	rdi,rcx
    mov	rsi,rdx
    mov	rdx,r8     
    and rsi,QWORD PTR L__all_bits_set
    jz  L__exit
    and rdx,QWORD PTR L__all_bits_set
    jz  L__exit
    cmp edi, 0
    jle L__exit
    
    xor r8,r8
    xor rcx,rcx
ALIGN 16
L__vrsa_top:
    ;fill data in xmm0
    cmp    rdi, 0
    jz     L__exit_vrsa_logf
    pxor   xmm0,xmm0
    cmp    rdi, 1
    jz     L__fill_single
    cmp rdi, 2
    jz  L__fill_two
    cmp rdi, 3
    jz  L__fill_three
    movups xmm0, XMMWORD PTR[rsi + r8 * 4] ;is data pointer always 16 byte aligned?
    sub rdi, 4
    add r8, 4
    jmp L__start_process_vrs4_logf
L__fill_single:
    movss xmm0, DWORD PTR[rsi + r8 * 4] 
    dec rdi
    inc r8
    jmp L__start_process_vrs4_logf
L__fill_two:
    movlps xmm0, QWORD PTR[rsi + r8 * 4]
    sub rdi, 2
    add r8, 2
    jmp L__start_process_vrs4_logf
L__fill_three:
    pxor xmm1,xmm1
    movss xmm0, DWORD PTR[rsi + r8 * 4] 
    inc r8
    movlps xmm1, QWORD PTR [rsi + r8 * 4]
    pslldq xmm1, 4
    por    xmm0,xmm1
    sub rdi, 3
    add r8, 2
L__start_process_vrs4_logf:
 
    ; compute exponent part
    movaps         xmm1 ,xmm0
    movaps         xmm2 ,xmm0;
    movaps         xmm3 ,xmm0;
    movaps         xmm5 ,xmm0;
    movaps         xmm15,xmm0;
    pand           xmm2 ,XMMWORD PTR L__sign_bit_32 
    pcmpeqd        xmm2 ,XMMWORD PTR L__sign_bit_32 ; stores the mask for -ve inputs

    pand           xmm1,XMMWORD PTR L__exp_mask 
    pcmpeqd        xmm1,XMMWORD PTR L__exp_mask ;mask for Nans AN Infs

    ;get the mask for exactly 0.0
    pand           xmm15,XMMWORD PTR L__sign_mask_32 
    pcmpeqd        xmm15,XMMWORD PTR L__Zero_32 
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    ;calculate logf for lower half
    cvtps2pd       xmm5,xmm5
    movapd         xmm4,xmm5
    movapd         xmm6,xmm5
    movapd         xmm7,xmm5
    pand           xmm4,XMMWORD PTR L__exp_mask_64 
    psrlq          xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq          xmm4 ,XMMWORD PTR L__mask_1023 ; xmm4 = xexp
    pand           xmm5,XMMWORD PTR L__mantissa_mask 
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) |  f,HALFEXPBITS_DP64);
    por            xmm5 ,XMMWORD PTR L__real_half ; xmm5 = f
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    pand           xmm6,XMMWORD PTR L__index_mask1 
    pand           xmm7,XMMWORD PTR L__index_mask2 
    por            xmm6,XMMWORD PTR L__index_mask3 
    psrlq          xmm7 ,45; ((ux & 00000200000000000h) >> 45)
    psrlq          xmm6 ,46; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    paddq          xmm6 ,xmm7; index = xmm6
    pshufd         xmm7, xmm6,0F8h ; [11 11 10 00] =  0F8h
    cvtdq2pd       xmm7,xmm7
    mulpd          xmm7 ,XMMWORD PTR L__index_constant1 ; f1 =index*0.0078125
    psubq          xmm6 ,XMMWORD PTR L__index_constant2 ; index -= 64
    subpd          xmm5 ,xmm7; f2 = f - f1;

    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    movd           r10 ,xmm6; move lower order index
    psrldq         xmm6,8
    movd           r11 ,xmm6; move higher order index
    lea            r9,XMMWORD PTR L__ln_lead_64_table 
    lea            rax,XMMWORD PTR L__ln_tail_64_table 
    movlpd         xmm6, QWORD PTR [  r9 +  r10  * 8 ]
    movhpd         xmm6, QWORD PTR [  r9 +  r11  * 8 ] ; xmm6 = z1 = ln_lead_table[index]
    movlpd         xmm8, QWORD PTR [  rax +  r10  * 8 ]
    movhpd         xmm8, QWORD PTR [  rax +  r11  * 8 ] ; xmm8 = q = ln_tail_table[index]

    ;f2 = xmm5 f1 = xmm7 
    movapd         xmm13,xmm5
    mulpd          xmm13 ,XMMWORD PTR L__real_half ; (0.5 * f2)
    addpd          xmm7 ,xmm13;  (f1 + 0.5 * f2);
    divpd          xmm5  ,xmm7; u = f2 / (f1 + 0.5 * f2);
    movapd         xmm7,xmm5
    mulpd          xmm5  ,xmm5; v = u*u
    movapd         xmm11 ,xmm5

    mulpd         xmm5 ,XMMWORD PTR L__real_cb_2 ; v*cb_2
    addpd         xmm5 ,XMMWORD PTR L__real_cb_1 ; cb_1 + v*cb_2
    mulpd         xmm5  ,xmm11; poly = v * (cb_1 + v * cb_2)
    ;poly = xmm5 u = xmm7 q = xmm8

    mulpd         xmm5,xmm7
    addpd         xmm5,xmm7
    addpd         xmm5,xmm8; z2 = q + (u + u * poly)
   
    ;xmm5 = q, xmm8 = z2,  xmm6 = z1 
    pshufd        xmm4, xmm4, 0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd      xmm4 ,xmm4; xexp (float)
    mulpd         xmm4 ,XMMWORD PTR L__real_log2 ; (xexp * log2)
    addpd         xmm4 ,xmm6;   (xexp * log2 + z1)
    addpd         xmm4 ,xmm5; r = (xexp * log2 + z1 + z2)
    
    cvtpd2ps      xmm8 ,xmm4
    pand          xmm8,XMMWORD PTR L__mask_upperhalf 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;now calculate for the upper half
    ;calculate logf for upper half
    psrldq        xmm3,8
    cvtps2pd      xmm3,xmm3
    movapd        xmm4,xmm3
    movapd        xmm6,xmm3
    movapd        xmm7,xmm3
    pand          xmm4,XMMWORD PTR L__exp_mask_64 
    psrlq         xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq         xmm4 ,XMMWORD PTR L__mask_1023 ;
    pand          xmm3,XMMWORD PTR L__mantissa_mask 
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) |  f,HALFEXPBITS_DP64);
    por           xmm3 ,XMMWORD PTR L__real_half ; xmm3 = f
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    pand          xmm6,XMMWORD PTR L__index_mask1 
    pand          xmm7,XMMWORD PTR L__index_mask2 
    por           xmm6,XMMWORD PTR L__index_mask3 
    psrlq         xmm7 ,45; ((ux & 00000200000000000h) >> 45)
    psrlq         xmm6 ,46; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    paddq         xmm6,xmm7
    pshufd        xmm7, xmm6,0F8h ; [11 11 10 00] =  0F8h
    cvtdq2pd      xmm7,xmm7
    mulpd         xmm7 ,XMMWORD PTR L__index_constant1 ; f1 =index*0.0078125
    psubq         xmm6 ,XMMWORD PTR L__index_constant2 ; index -= 64
    subpd         xmm3 ,xmm7; f2 = f - f1;

    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    movd          r10 ,xmm6; move lower order index
    psrldq        xmm6,8
    movd          r11 ,xmm6; move higher order index
    lea           r9,XMMWORD PTR L__ln_lead_64_table 
    lea           rax,XMMWORD PTR L__ln_tail_64_table 
    movlpd        xmm6, QWORD PTR [  r9 +  r10  * 8 ]
    movhpd        xmm6, QWORD PTR [  r9 +  r11  * 8 ] ; xmm6 = z1 = ln_lead_table[index]
    movlpd        xmm5, QWORD PTR [  rax +  r10  * 8 ]
    movhpd        xmm5, QWORD PTR [  rax +  r11  * 8 ] ; xmm5 = q = ln_tail_table[index]
    ;f2 = xmm3 f1 = xmm7 
    movapd        xmm13,xmm3
    mulpd         xmm13 ,XMMWORD PTR L__real_half ; (0.5 * f2)
    addpd         xmm7 ,xmm13;  (f1 + 0.5 * f2);
    divpd         xmm3  ,xmm7; u = f2 / (f1 + 0.5 * f2);
    movapd        xmm7,xmm3
    mulpd         xmm3  ,xmm3; v = u*u
    movapd        xmm11 ,xmm3

    mulpd         xmm3 ,XMMWORD PTR L__real_cb_2 ; v*cb_2
    addpd         xmm3 ,XMMWORD PTR L__real_cb_1 ; cb_1 + v*cb_2
    mulpd         xmm3  ,xmm11;poly =  v * (cb_2 + v * cb_3)
    
    ;poly = xmm3 u = xmm7 q = xmm5
    mulpd         xmm3 ,xmm7
    addpd         xmm3,xmm7
    addpd         xmm3   ,xmm5; z2 = q + (u + u * poly)
    
    ;xmm3 = q, xmm5 = z2 , u = xmm7 ,xmm6 = z1 
    pshufd        xmm4 ,xmm4, 0F8h  ; [11 11 10 00] = 0F8h
    cvtdq2pd      xmm4 ,xmm4; xexp (float) 
    mulpd         xmm4 ,XMMWORD PTR L__real_log2 ; (xexp * log2)
    addpd         xmm4 ,xmm6; (xexp * log2 + z1)
    addpd         xmm4 ,xmm3; r= (xexp * log2 + z1 + z2)
    
    cvtpd2ps      xmm3 ,xmm4
    pslldq        xmm3,8
    por           xmm8,xmm3
;end of upper half calculation
;Now club the two results
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;restore results for -ve inputs 
    movaps        xmm3,xmm2
    pand          xmm3,XMMWORD PTR L__real_qnan 
    pandn         xmm2,xmm8
    por           xmm2,xmm3;
    
    ;now restore the Nans and Infs
    movaps        xmm3,xmm0
    addps         xmm0,xmm0
    pand          xmm3,XMMWORD PTR L__sign_bit_32 ; for negative infities we need to set the result as Qnan
    ;so we get the sign bit and move it to the qnan Bit.
    ;then OR it with Qnan/inf result
    psrld         xmm3,9
    pand          xmm3,xmm1
    pand          xmm0,xmm1
    por           xmm0,xmm3
    pandn         xmm1,xmm2
    por           xmm0,xmm1
    
    ; now restore if there are some inputs with 0.0
    movaps        xmm2,xmm15
    pand          xmm15,XMMWORD PTR L__real_ninf_32 
    pandn         xmm2,xmm0
    por           xmm2 ,xmm15; OR the infinities = -1.0
 
;##########################################
    
;##########################################
L__end_process_vrs4_logf:
    ;fill data back into memory buffer from the result xmm2
    movss DWORD PTR[rdx + rcx * 4], xmm2
    inc rcx
    cmp r8, rcx
    jz L__exit_vrsa_logf
    psrldq xmm2, 4
    movss  DWORD PTR[rdx + rcx * 4],xmm2
    inc rcx
    cmp r8,rcx
    jz L__exit_vrsa_logf
    psrldq xmm2, 4 
    movss DWORD PTR[rdx + rcx * 4], xmm2
    inc rcx
    cmp r8, rcx
    jz L__exit_vrsa_logf
    psrldq xmm2, 4
    movss DWORD PTR[rdx + rcx * 4], xmm2
    inc rcx
    cmp r8, rcx
    jmp L__vrsa_top
L__exit:    
L__exit_vrsa_logf:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    RestoreXmm xmm6, save_xmm6
    RestoreXmm xmm7, save_xmm7
    RestoreXmm xmm8, save_xmm8
    ;RestoreXmm xmm9, save_xmm9
    ;RestoreXmm xmm10, save_xmm10
    RestoreXmm xmm11, save_xmm11
    ;RestoreXmm xmm12, save_xmm12
    RestoreXmm xmm13, save_xmm13
    ;RestoreXmm xmm14, save_xmm14
    RestoreXmm xmm15, save_xmm15
    RestoreReg rdi,save_rdi
    RestoreReg rsi,save_rsi
    StackDeallocate stack_size
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ret
fname        endp
TEXT    ENDS      


data SEGMENT READ
CONST    SEGMENT
ALIGN 16
L__all_bits_set     DQ 0FFFFFFFFFFFFFFFFh
                    DQ 0FFFFFFFFFFFFFFFFh    
L__sign_bit_32      DQ 08000000080000000h 
                    DQ 08000000080000000h
L__exp_mask         DQ 07F8000007F800000h   ;
                    DQ 07F8000007F800000h
L__sign_mask_32     DQ 07FFFFFFF7FFFFFFFh
                    DQ 07FFFFFFF7FFFFFFFh
L__Zero_32          DQ 00000000000000000h
                    DQ 00000000000000000h
L__exp_mask_64      DQ 07FF0000000000000h   ;
                    DQ 07FF0000000000000h
L__mask_1023        DQ 000000000000003ffh
                    DQ 000000000000003ffh
L__mantissa_mask    DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                    DQ 0000FFFFFFFFFFFFFh
L__index_mask1      DQ 0000fc00000000000h
                    DQ 0000fc00000000000h
L__index_mask2      DQ 00000200000000000h
                    DQ 00000200000000000h
L__index_mask3      DQ 00010000000000000h
                    DQ 00010000000000000h
L__index_constant1  DQ 03F80000000000000h
                    DQ 03F80000000000000h
L__index_constant2  DQ 00000000000000040h
                    DQ 00000000000000040h
L__real_half        DQ 03fe0000000000000h ; 1/2
                    DQ 03fe0000000000000h

; Approximating polynomial coefficients for other x 
L__real_cb_2        DQ 03f89999999865edeh ; 124999999978138668903e-02,  
                    DQ 03f89999999865edeh
L__real_cb_1        DQ 03fb5555555555557h ; 833333333333333593622e-02, 
                    DQ 03fb5555555555557h
L__real_log2        DQ 03fe62e42fefa39efh ; log2 = 6931471805599453e-01
                    DQ 03fe62e42fefa39efh 
		       
L__mask_upperhalf   DQ 0FFFFFFFFFFFFFFFFh
                    DQ 00000000000000000h
L__real_ninf_32     DQ 0ff800000ff800000h   ; -inf
                    DQ 0ff800000ff800000h
L__real_qnan        DQ 07FC000007FC00000h   ; qNaN
                    DQ 07FC000007FC00000h

CONST    ENDS
data ENDS

END
