;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; vrs4_log1pf.S
;
; An implementation of the vrs4_log1pf libm function.
;
; Prototype:
;
;     __m128 vrs4_log1pf(__m128 x);
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

FN_PROTOTYPE_BAS64 vrs4_log1pf

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

StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

StackDeallocate   MACRO size
    add         rsp, size
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
    SaveXmm xmm9, save_xmm9
    SaveXmm xmm10, save_xmm10
    SaveXmm xmm11, save_xmm11
    ;SaveXmm xmm12, save_xmm12
    SaveXmm xmm13, save_xmm13
    SaveXmm xmm14, save_xmm14
    SaveXmm xmm15, save_xmm15    
    
    .ENDPROLOG   

    movups xmm0, XMMWORD PTR [rcx]




    ; compute exponent part
    movaps       xmm1 ,xmm0
    movaps       xmm2 ,xmm0;
    movaps       xmm3 ,xmm0;
    movaps       xmm4 ,xmm0;
    movaps       xmm5 ,xmm0;
    movaps       xmm15 ,xmm0;
    pand        xmm1,XMMWORD PTR L__sign_mask_32 

    cmpleps     xmm1 ,XMMWORD PTR L__real_epsilon ;mask for epsilons
    cmpltps     xmm2 ,XMMWORD PTR L__negative_one ;mask for input less than -1.0
    ;get the mask for exactly -1.0
    cmpeqps     xmm15,XMMWORD PTR L__negative_one 

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    ;calculate log1pf for lower half
    cvtps2pd    xmm5,xmm5
    movapd      xmm14,xmm5
    addpd        xmm5 ,XMMWORD PTR L__one_mask_64 ;xmm5 = 1.0 +x
    movapd      xmm4,xmm5
    movapd      xmm6,xmm5
    movapd      xmm7,xmm5
    pand        xmm4,XMMWORD PTR L__exp_mask_64 
    psrlq        xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       xmm4,XMMWORD PTR L__mask_1023 
    pand        xmm5,XMMWORD PTR L__mantissa_mask 
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) |  f,ONEEXPBITS_DP64);
    por         xmm5,XMMWORD PTR L__one_mask_64 
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    pand        xmm6,XMMWORD PTR L__index_mask1 
    pand        xmm7,XMMWORD PTR L__index_mask2 
    por         xmm6,XMMWORD PTR L__index_mask3 
    psrlq        xmm7 ,45; ((ux & 00000200000000000h) >> 45)
    psrlq        xmm6 ,46; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    paddq       xmm6,xmm7
    pshufd      xmm7,xmm6,0F8h ; [11 11 10 00] =  0F8h
    cvtdq2pd    xmm7,xmm7
    mulpd       xmm7 ,XMMWORD PTR L__index_constant1 ; f1 =index*0.015625
    psubq       xmm6 ,XMMWORD PTR L__index_constant2 ; index -= 64
    movapd      xmm8 ,xmm4; xmm4=xexp=xmm8 
    pshufd       xmm8,xmm8,088h ; [10 00 10 00] = 088h
    movapd      xmm9,XMMWORD PTR L__plus_sixtyone_minus_two 
    pcmpgtd     xmm9   ,xmm8; xmm9 > xmm8 (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    movapd      xmm10,xmm9
    psrldq       xmm10,8
    ;IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    pandn       xmm10 ,xmm9; xmm9 stores the mask for all the numbers which lie between -2 and 61
    pshufd       xmm9, xmm10,050h ; [01 01 00 00] = 050h
    subpd       xmm5 ,xmm7; f2 = f - f1;
    ;ELSE
    movapd      xmm11,XMMWORD PTR L__mask_1023 
    psubq        xmm11,xmm4
    
    ;ux = (unsigned long long)(03ffh - xexp) << EXPSHIFTBITS_DP64;
    ;PUT_BITS_DP64(m2,ux);
    psllq        xmm11 ,52; xmm11 = m2;
    movapd      xmm13 ,xmm11
    mulpd       xmm11 ,xmm14; xmm11 = m2*x
    ;xmm7= xmm13,f1= xmm11 ,m2= m2*x
    subpd       xmm13 ,xmm7; (m2 - f1)
    addpd       xmm13 ,xmm11; xmm13 = f2 = (m2 - f1) + m2*dx

    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    movd        r10 ,xmm6; move lower order index
    psrldq       xmm6,8
    movd        r11 ,xmm6; move higher order index
    lea          r9,XMMWORD PTR L__ln_lead_64_table 
    lea          r8,XMMWORD PTR L__ln_tail_64_table 
    movlpd      xmm6, QWORD PTR [ r9 + r10  * 8 ]
    movhpd      xmm6, QWORD PTR [ r9 + r11  * 8 ] ; xmm6 = z1 = ln_lead_table[index]
    movlpd      xmm8, QWORD PTR [ r8 + r10  * 8 ]
    movhpd      xmm8, QWORD PTR [ r8 + r11  * 8 ] ; xmm8 = q = ln_tail_table[index]
    ;f2 = xmm13/xmm5 f1 = xmm7 
    pand        xmm13 ,xmm9
    pandn       xmm9,xmm5
    por         xmm13 ,xmm9; xmm13 = f2
    movapd      xmm5,xmm13
    mulpd       xmm13 ,XMMWORD PTR L__real_half ; (0.5 * f2)
    addpd       xmm7 ,xmm13;  (f1 + 0.5 * f2);
    divpd       xmm5  ,xmm7; u = f2 / (f1 + 0.5 * f2);
    movapd      xmm7,xmm5
    mulpd       xmm5  ,xmm5; v = u*u
    movapd      xmm11 ,xmm5

    mulpd       xmm5 ,XMMWORD PTR L__real_cb_2 ; v*cb_2
    addpd       xmm5 ,XMMWORD PTR L__real_cb_1 ; cb_1 + v*cb_2
    mulpd       xmm5  ,xmm11; poly = v * (cb_1 + v * cb_2)
    ;poly = xmm5 u = xmm7 q = xmm8
    mulpd       xmm5,xmm7
    addpd       xmm5,xmm7
    addpd       xmm5   ,xmm8; z2 = q + (u + u * poly)
   
    ;xmm5 =  xmm8 ,z2=  xmm7 ,q=  xmm6 ,u= z1 
    pshufd       xmm4, xmm4,0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd    xmm4 ,xmm4; xexp (float)
    mulpd        xmm4 ,XMMWORD PTR L__real_log2 ; (xexp * log2)
    addpd       xmm4 ,xmm6;   (xexp * log2 + z1)
    addpd       xmm4 ,xmm5; r = (xexp * log2 + z1 + z2)
    
    cvtpd2ps    xmm8 ,xmm4
    pand        xmm8,XMMWORD PTR L__mask_upperhalf 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;now calculate for the upper half
    ;calculate log1pf for lower half
    psrldq       xmm3,8
    cvtps2pd    xmm3,xmm3
    movapd      xmm14,xmm3
    addpd        xmm3 ,XMMWORD PTR L__one_mask_64 ;xmm3 = 1.0 +x
    movapd      xmm4,xmm3
    movapd      xmm6,xmm3
    movapd      xmm7,xmm3
    pand        xmm4,XMMWORD PTR L__exp_mask_64 
    psrlq        xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       xmm4,XMMWORD PTR L__mask_1023 
    pand        xmm3,XMMWORD PTR L__mantissa_mask 
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) |  f,ONEEXPBITS_DP64);
    por         xmm3,XMMWORD PTR L__one_mask_64 
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    pand        xmm6,XMMWORD PTR L__index_mask1 
    pand        xmm7,XMMWORD PTR L__index_mask2 
    por         xmm6,XMMWORD PTR L__index_mask3 
    psrlq        xmm7 ,45; ((ux & 00000200000000000h) >> 45)
    psrlq        xmm6 ,46; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    paddq       xmm6,xmm7
    pshufd       xmm7,xmm6,0F8h ; [11 11 10 00] =  0F8h
    cvtdq2pd    xmm7,xmm7
    mulpd       xmm7 ,XMMWORD PTR L__index_constant1 ; f1 =index*0.015625
    psubq       xmm6 ,XMMWORD PTR L__index_constant2 ; index -= 64
    movapd      xmm5 ,xmm4; xmm4=xexp=xmm5 
    pshufd       xmm5, xmm5,088h ; [10 00 10 00] = 088h
    movapd      xmm9,XMMWORD PTR L__plus_sixtyone_minus_two 
    pcmpgtd     xmm9   ,xmm5; xmm9 > xmm5 (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    movapd      xmm10,xmm9
    psrldq       xmm10,8
    ;IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    pandn       xmm10 ,xmm9; xmm9 stores the mask for all the numbers which lie between -2 and 61
    pshufd        xmm9, xmm10, 050h ; [01 01 00 00] = 050h
    subpd       xmm3 ,xmm7; f2 = f - f1;
    ;ELSE
    movapd      xmm11,XMMWORD PTR L__mask_1023 
    psubq        xmm11,xmm4
    
    ;ux = (unsigned long long)(03ffh - xexp) << EXPSHIFTBITS_DP64;
    ;PUT_BITS_DP64(m2,ux);
    psllq        xmm11 ,52; xmm11 = m2;
    movapd      xmm13,xmm11
    mulpd       xmm11 ,xmm14; xmm11 = m2*x
    ;xmm7= xmm13,f1= xmm11 ,m2= m2*x
    subpd       xmm13 ,xmm7; (m2 - f1)
    addpd       xmm13 ,xmm11; xmm13 = f2 = (m2 - f1) + m2*dx

    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    movd        r10 ,xmm6; move lower order index
    psrldq       xmm6,8
    movd        r11 ,xmm6; move higher order index
    lea          r9,XMMWORD PTR L__ln_lead_64_table 
    lea          r8,XMMWORD PTR L__ln_tail_64_table 
    movlpd      xmm6, QWORD PTR [ r9 + r10  * 8 ]
    movhpd      xmm6, QWORD PTR [ r9 + r11  * 8 ] ; xmm6 = z1 = ln_lead_table[index]
    movlpd      xmm5, QWORD PTR [ r8 + r10  * 8 ]
    movhpd      xmm5, QWORD PTR [ r8 + r11  * 8 ] ; xmm5 = q = ln_tail_table[index]
    ;f2 = xmm13/xmm3 f1 = xmm7 
    pand        xmm13 ,xmm9
    pandn       xmm9,xmm3
    por         xmm13 ,xmm9; xmm13 = f2
    movapd      xmm3,xmm13
    mulpd       xmm13 ,XMMWORD PTR L__real_half ; (0.5 * f2)
    addpd       xmm7 ,xmm13;  (f1 + 0.5 * f2);
    divpd       xmm3  ,xmm7; u = f2 / (f1 + 0.5 * f2);
    movapd      xmm7,xmm3
    mulpd       xmm3  ,xmm3; v = u*u
    movapd      xmm11 ,xmm3

    mulpd       xmm3 ,XMMWORD PTR L__real_cb_2 ; v*cb_2
    addpd       xmm3 ,XMMWORD PTR L__real_cb_1 ; cb_1 + v*cb_2
    mulpd       xmm3  ,xmm11;poly =  v * (cb_2 + v * cb_3)
    
    ;poly = xmm3 u = xmm7 q = xmm5
    mulpd       xmm3 ,xmm7
    addpd       xmm3,xmm7
    addpd       xmm3   ,xmm5; z2 = q + (u + u * poly)
    
    ;xmm3 =  xmm5 ,z2=  xmm7 ,q=  xmm6 ,u= z1 
    pshufd       xmm4,xmm4,0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd    xmm4 ,xmm4; xexp (float) 
    mulpd        xmm4 ,XMMWORD PTR L__real_log2 ; (xexp * log2)
    addpd       xmm4 ,xmm6; (xexp * log2 + z1)
    addpd       xmm4 ,xmm3; r= (xexp * log2 + z1 + z2)
    
    cvtpd2ps    xmm3 ,xmm4
    pslldq       xmm3,8
    por         xmm8,xmm3
;end of upper half calculation
;Now club the two results
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    
    ;process inputs less than 03380000033800000h
    movaps    xmm3,xmm0
    pand      xmm3 ,xmm1
    pandn     xmm1,xmm8
    por       xmm1,xmm3
    
    ;now restore if there are some inputs less than -1.0
    movapd    xmm8,xmm2
    pand       xmm8,XMMWORD PTR L__real_qnan 
    pandn     xmm2,xmm1
    por       xmm2 ,xmm8; then OR the Qnans < -1.0
    
    movaps    xmm14,xmm0
    ;get the mask for Nans and Infs
    pand        xmm14  ,XMMWORD PTR L__exp_mask ; remove the mantissa
    pcmpeqd     xmm14 ,XMMWORD PTR L__exponent_compare ; xmm14 stores the mask for nans and Infs


    ;now restore the Nans and Infs
    movaps    xmm3,xmm0
    addps     xmm0,xmm0
    pand      xmm3 ,XMMWORD PTR L__sign_bit_32 ; for negative infities we need to set the result as Qnan
    ;so we get the sign bit and move it to the qnan Bit.
    ;then OR it with Qnan/inf result
    psrld      xmm3,9
    pand      xmm3,xmm14
    pand      xmm0,xmm14
    por       xmm0,xmm3
    pandn     xmm14,xmm2
    por       xmm0,xmm14
    
    ; now restore if there are some inputs with -1.0
    movaps    xmm2,xmm15
    pand       xmm15,XMMWORD PTR L__real_ninf_32 
    pandn     xmm2,xmm0
    por       xmm2 ,xmm15; OR the infinities = -1.0
    movaps xmm0,xmm2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    RestoreXmm xmm6, save_xmm6
    RestoreXmm xmm7, save_xmm7
    RestoreXmm xmm8, save_xmm8
    RestoreXmm xmm9, save_xmm9
    RestoreXmm xmm10, save_xmm10
    RestoreXmm xmm11, save_xmm11
    ;RestoreXmm xmm12, save_xmm12
    RestoreXmm xmm13, save_xmm13
    RestoreXmm xmm14, save_xmm14
    RestoreXmm xmm15, save_xmm15
    StackDeallocate stack_size
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ret
fname        endp
TEXT    ENDS      
        
    
    
data SEGMENT READ
CONST    SEGMENT
ALIGN 16
    
L__sign_mask_32     DQ 07FFFFFFF7FFFFFFFh
                    DQ 07FFFFFFF7FFFFFFFh
L__real_epsilon     DQ 03380000033800000h
                    DQ 03380000033800000h
L__negative_one     DQ 0bf800000bf800000h
                    DQ 0bf800000bf800000h
L__one_mask_64      DQ 03fF0000000000000h; 1
                    DQ 03fF0000000000000h
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
L__index_constant1  DQ 03F90000000000000h
                    DQ 03F90000000000000h
L__index_constant2  DQ 00000000000000040h
                    DQ 00000000000000040h
L__plus_sixtyone_minus_two  DQ 00000003D0000003Dh
                            DQ 0FFFFFFFEFFFFFFFEh
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
L__real_qnan        DQ 07FC000007FC00000h   ; qNaN
                    DQ 07FC000007FC00000h
L__exp_mask         DQ 07F8000007F800000h   ;
                    DQ 07F8000007F800000h
L__exponent_compare DQ 07F8000007F800000h  
                    DQ 07F8000007F800000h
L__sign_bit_32      DQ 08000000080000000h 
                    DQ 08000000080000000h
L__real_ninf_32     DQ 0ff800000ff800000h   ; -inf
                    DQ 0ff800000ff800000h
		
CONST    ENDS
data ENDS

END
