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
; vrd2_log1p.S
;
; An implementation of the vrd2_log1p libm function.
;
; Prototype:
;
;     __m128d vrd2_log1p(__m128d x);
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

ALM_PROTO_BAS64 vrd2_log1p

; local variable storage offsets if any

save_xmm6       EQU      00h
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

    movupd xmm0, XMMWORD PTR [rcx]


    ; compute exponent part
    movapd      xmm1 ,xmm0
    movapd      xmm2 ,xmm0;
    movapd      xmm3 ,xmm0;
    movapd      xmm4 ,xmm0;
    movapd      xmm5 ,xmm0;
    movapd      xmm14,xmm0;
    movapd      xmm15,xmm0;
    pand        xmm1,XMMWORD PTR L__sign_mask_64

    cmplepd     xmm1 ,XMMWORD PTR L__real_epsilon;mask for epsilons
    cmpltpd     xmm2 ,XMMWORD PTR L__real_minus_one;mask for input less than -1.0
    ;get the mask for exactly -1.0
    cmpeqpd     xmm15,XMMWORD PTR L__real_minus_one
    ;get the mask for Nans and Infs
    pand        xmm14, XMMWORD PTR L__exp_mask; remove the mantissa
    pshufd      xmm14,xmm14,0F5h ; [11 11 01 01 ] = 0Fh
    pcmpeqd     xmm14,XMMWORD PTR L__exponent_compare; xmm14 stores the mask for nans and Infs

    
    ;calculate log1p
    cmpnlepd     xmm3,XMMWORD PTR L__real_log1p_thresh2
    cmpltpd      xmm4,XMMWORD PTR L__real_log1p_thresh1
    por          xmm3,xmm4; mask which stores (x < log1p_thresh1 || x > log1p_thresh2)

    ;calculate outside the threshold range first
    addpd        xmm5 ,L__one_mask_64;xmm5 = 1.0 +x
    movapd      xmm4,xmm5
    movapd      xmm6,xmm5
    movapd      xmm7,xmm5
    pand        xmm4,XMMWORD PTR L__exp_mask
    psrlq       xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    psubq       xmm4,XMMWORD PTR L__mask_1023
    pand        xmm5,XMMWORD PTR L__mantissa_mask
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) |  ONEEXPBITS_DP64, f);
    por         xmm5,XMMWORD PTR L__one_mask_64
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    pand        xmm6,XMMWORD PTR L__index_mask1
    pand        xmm7,XMMWORD PTR L__index_mask2
    por         xmm6,XMMWORD PTR L__index_mask3
    psrlq       xmm7 ,45; ((ux & 00000200000000000h) >> 45)
    psrlq       xmm6 ,46; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    paddq       xmm6,xmm7
    pshufd      xmm7,xmm6,0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd    xmm7,xmm7
    mulpd       xmm7 ,XMMWORD PTR L__index_constant1; f1 =index*0.015625
    psubq       xmm6 ,XMMWORD PTR L__index_constant2; index -= 64
    movapd      xmm8 ,xmm4; xmm4=xexp=xmm8 
    pshufd      xmm8,xmm8,088h ; [10 00 10 00] = 088h
    movapd      xmm9,XMMWORD PTR L__plus_sixtyone_minus_two
    pcmpgtd     xmm9 ,xmm8
    movapd      xmm10,xmm9
    psrldq      xmm10,8
    ;IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    pandn       xmm10,xmm9; xmm9 stores the mask for all the numbers which lie between -2 and 61
    pshufd      xmm9,xmm10,050h ; [01 01 00 00] = 050h
    subpd       xmm5,xmm7; f2 = f - f1;
    ;ELSE
    movapd      xmm11,XMMWORD PTR L__mask_1023
    psubq       xmm11,xmm4
    
    ;ux = (unsigned long long)(03ffh - xexp) << EXPSHIFTBITS_DP64;
    ;PUT_BITS_DP64(m2,ux);
    psllq       xmm11 ,52; xmm11 = m2;
    movapd      xmm13,xmm11
    mulpd       xmm11 ,xmm0; xmm11 = m2*x
    ;xmm7=f1 xmm13=m2 xmm11=m2*x
    subpd       xmm13 ,xmm7; (m2 - f1)
    addpd       xmm13 ,xmm11; xmm13 = f2

    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    movd        r10,xmm6; move lower order index
    psrldq      xmm6,8
    movd        r11,xmm6; move higher order index
    lea         r9,XMMWORD PTR L__ln_lead_64_table
    lea         r8,XMMWORD PTR L__ln_tail_64_table
    movlpd      xmm6 ,QWORD PTR [ r9 + r10  * 8 ] 
    movhpd      xmm6 ,QWORD PTR [ r9 + r11  * 8 ] ; xmm6 = z1 = ln_lead_table[index]
    movlpd      xmm8 ,QWORD PTR [ r8 + r10  * 8 ] 
    movhpd      xmm8 ,QWORD PTR [ r8 + r11  * 8 ] ; xmm8 = q = ln_tail_table[index]
    ;f2 = xmm13/xmm5 f1 = xmm7 
    pand        xmm13,xmm9
    pandn       xmm9 ,xmm5
    por         xmm13,xmm9; xmm5 = f2
    movapd      xmm5 ,xmm13
    mulpd       xmm13,XMMWORD PTR L__real_half; (0.5 * f2)
    addpd       xmm7 ,xmm13;  (f1 + 0.5 * f2);
    divpd       xmm5 ,xmm7; u = f2 / (f1 + 0.5 * f2);
    movapd      xmm7 ,xmm5; xmm7 = u
    mulpd       xmm5 ,xmm5; v = u*u
    movapd      xmm11,xmm5; v 

    mulpd       xmm5 ,XMMWORD PTR L__real_cb_3; v*cb_3
    addpd       xmm5 ,XMMWORD PTR L__real_cb_2; cb_2 + v*cb_3
    mulpd       xmm5 ,xmm11; v * (cb_2 + v * cb_3)
    addpd       xmm5 ,XMMWORD PTR L__real_cb_1; (cb_1 + v * (cb_2 + v * cb_3))
    mulpd       xmm5 ,xmm11; poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    ;poly = xmm5 u = xmm7 q = xmm8
    mulpd       xmm5,xmm7
    addpd       xmm5,xmm7
    addpd       xmm5,xmm8; z2 = q + (u + u * poly)
    
    pshufd      xmm4,xmm4,0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd    xmm4 ,xmm4; xexp (float) 
    movapd      xmm8 ,xmm4; xmm8 = xexp
    mulpd       xmm4,XMMWORD PTR L__real_log2_lead
    addpd       xmm4 ,xmm6; r1 = (xexp * log2_lead + z1)
    mulpd       xmm8,XMMWORD PTR L__real_log2_tail
    addpd       xmm8 ,xmm5; r2 = (xexp * log2_tail + z2)
    
    addpd       xmm8 ,xmm4; return r1 + r2
    ;END of calculating outside the threshold
    
    ;calculate when the value lies within the threshold
    ;xmm0 still contains the input x
    movapd     xmm4 ,xmm0; xmm4 = r1        
    movapd     xmm5 ,xmm0; 
    movapd     xmm6 ,xmm0; 
    movapd     xmm7 ,xmm0; 
    addpd      xmm5 ,XMMWORD PTR L__plus_two; (2.0 + r)
    divpd      xmm6 ,xmm5; xmm6 = u = r / (2.0 + r); 
    mulpd      xmm7 ,xmm6; correction = r * u
    addpd      xmm6 ,xmm6; u = u+ u
    movapd     xmm9 ,xmm6
    mulpd      xmm9 ,xmm9; v = u * u
    ;v =  u ,xmm9=  correction ,xmm6= xmm7
    movapd     xmm10 ,xmm9; xmm10 = v
    mulpd      xmm10 ,XMMWORD PTR L__real_ca_4; (v * ca_4)
    addpd      xmm10 ,XMMWORD PTR L__real_ca_3; (ca_3 + (v * ca_4))
    mulpd      xmm10 ,xmm9; v * (ca_3 + v * ca_4)
    addpd      xmm10 ,XMMWORD PTR L__real_ca_2; (ca_2 + v * (ca_3 + v * ca_4))
    mulpd      xmm10 ,xmm9; v * (ca_2 + v * (ca_3 + v * ca_4))
    addpd      xmm10 ,XMMWORD PTR L__real_ca_1; (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      xmm10 ,xmm9; v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      xmm10 ,xmm6; u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    subpd      xmm10 ,xmm7; r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction) 
    addpd      xmm10 ,xmm4
    ;END calculating within the threshold

    ;now restore the proper results
    pand      xmm8,xmm3
    pandn     xmm3,xmm10
    por       xmm3,xmm8
    movapd    xmm8,xmm0; copy the input

    ;process inputs less than 03ca0000000000000h
    pand      xmm8 ,xmm1
    pandn     xmm1,xmm3
    por       xmm1,xmm8

    ; now restore if there are some inputs less than -1.0
    movapd    xmm8,xmm2
    pand      xmm8,XMMWORD PTR L__real_qnan
    pandn     xmm2,xmm1
    por       xmm2,xmm8
    ; now restore if there are some inputs with -1.0
    movapd    xmm8,xmm15
    pandn     xmm8,xmm2
    pand      xmm15,XMMWORD PTR L__real_ninf
    por       xmm8 ,xmm15; first OR the infinities = -1.0
    
    ; now restore the Nans and Infs
    movapd    xmm3,xmm0
    addpd     xmm0,xmm0
    pand      xmm3,XMMWORD PTR L__sign_bit_64; for negative infities we need to set the result as Qnan
    ;so we get the sign bit and move it to the qnan Bit.
    ; then OR it with Qnan/inf result
    psrlq     xmm3,12
    pand      xmm3,xmm14
    pand      xmm0,xmm14
    por       xmm0,xmm3
    pandn     xmm14,xmm8
    por       xmm0,xmm14
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    RestoreXmm xmm6, save_xmm6
    RestoreXmm xmm7, save_xmm7
    RestoreXmm xmm8, save_xmm8
    RestoreXmm xmm9, save_xmm9
    RestoreXmm xmm10, save_xmm10
    RestoreXmm xmm11, save_xmm11
    RestoreXmm xmm13, save_xmm13
    ;RestoreXmm xmm12, save_xmm12
    RestoreXmm xmm14, save_xmm14
    RestoreXmm xmm15, save_xmm15    
    StackDeallocate stack_size
;##########################################
    ret
fname        endp
TEXT    ENDS    
    
    
data SEGMENT READ
CONST    SEGMENT
ALIGN 16
    
L__sign_mask_64       DQ 07FFFFFFFFFFFFFFFh
                      DQ 07FFFFFFFFFFFFFFFh
L__real_epsilon       DQ 03ca0000000000000h
                      DQ 03ca0000000000000h
L__real_minus_one     DQ 0BFF0000000000000h
                      DQ 0BFF0000000000000h
L__exp_mask           DQ 07ff0000000000000h   ;
                      DQ 07ff0000000000000h
L__exponent_compare   DQ 07FF000007FF00000h  
                      DQ 07FF000007FF00000h
; The values exp(-1/16)-1 and exp(1/16)-1 */
L__real_log1p_thresh2 DQ 03fb082b577d34ed8h ;  644944589178594318568e-02;  
                      DQ 03fb082b577d34ed8h 
L__real_log1p_thresh1 DQ 0bfaf0540438fd5c4h ; -605869371865242201114e-02, 
                      DQ 0bfaf0540438fd5c4h
L__one_mask_64        DQ 03ff0000000000000h ; 1
                      DQ 03ff0000000000000h
L__mask_1023          DQ 000000000000003ffh
                      DQ 000000000000003ffh
L__mantissa_mask      DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                      DQ 0000FFFFFFFFFFFFFh

L__index_mask1        DQ 0000fc00000000000h
                      DQ 0000fc00000000000h
L__index_mask2        DQ 00000200000000000h
                      DQ 00000200000000000h
L__index_mask3        DQ 00010000000000000h
                      DQ 00010000000000000h
L__index_constant1    DQ 03F90000000000000h
                      DQ 03F90000000000000h
L__index_constant2    DQ 00000000000000040h
                      DQ 00000000000000040h
L__plus_sixtyone_minus_two  DQ 00000003D0000003Dh
                            DQ 0FFFFFFFEFFFFFFFEh
L__real_half          DQ 03fe0000000000000h ; 1/2
                      DQ 03fe0000000000000h

; Approximating polynomial coefficients for other x 
L__real_cb_3          DQ 03f6249423bd94741h ; 223219810758559851206e-03;  
                      DQ 03f6249423bd94741h
L__real_cb_2          DQ 03f89999999865edeh ; 124999999978138668903e-02,  
                      DQ 03f89999999865edeh
L__real_cb_1          DQ 03fb5555555555557h ; 833333333333333593622e-02, 
                      DQ 03fb5555555555557h
L__real_log2_lead     DQ 03fe62e42e0000000h ; log2_lead  693147122859954833984e-01
                      DQ 03fe62e42e0000000h
L__real_log2_tail     DQ 03e6efa39ef35793ch ; log2_tail  576999904754328540596e-08
                      DQ 03e6efa39ef35793ch
L__plus_two           DQ 04000000000000000h
                      DQ 04000000000000000h
; Approximating polynomial coefficients for x near 00 
L__real_ca_4          DQ 03f3c8034c85dfff0h ; 434887777707614552256e-04,  
                      DQ 03f3c8034c85dfff0h
L__real_ca_3          DQ 03f62492307f1519fh ; 223213998791944806202e-03,  
                      DQ 03f62492307f1519fh
L__real_ca_2          DQ 03f89999999bac6d4h ; 125000000037717509602e-02,  
                      DQ 03f89999999bac6d4h
L__real_ca_1          DQ 03fb55555555554e6h ; 833333333333317923934e-02,  
                      DQ 03fb55555555554e6h
L__real_qnan          DQ 07ff8000000000000h   ; qNaN
                      DQ 07ff8000000000000h
L__real_ninf          DQ 0fff0000000000000h   ; -inf
                      DQ 0fff0000000000000h
L__sign_bit_64        DQ 08000000000000000h 
                      DQ 08000000000000000h
						 
CONST    ENDS
data ENDS

END
