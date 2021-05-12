;
; Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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
; log1p_fma3.S
;
; A bulldozer implementation of log1p libm function.
;
; Prototype:
;
;     double log1p(double x);
;

;
;   Algorithm:
;
;   Based on:
;   Ping-Tak Peter Tang
;   "Table-driven implementation of the logarithm function in IEEE
;   floating-point arithmetic"
;   ACM Transactions on Mathematical Software (TOMS)
;   Volume    Issue,16 4 (December 1990)
;
;
;

;include "fn_macros.h"
;include "log_tables.h"
;define fname ALM_PROTO_FMA3(log1p)


; local variable storage offsets
include fm.inc
include log_tables.inc

ALM_PROTO_FMA3 log1p

fname_special   TEXTEQU <_log1p_special>

; local variable storage offsets
save_xmm6       EQU     00h
stack_size      EQU     028h ; We take 8 as the last nibble to allow for 
                              ; alligned data movement.

; external function
EXTERN fname_special:PROC

text SEGMENT EXECUTE
ALIGN 16
PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveRegsAVX xmm6,save_xmm6
    .ENDPROLOG 


    ; compute exponent part
    vmovsd    xmm3,QWORD PTR L__real_log1p_thresh2 ; xmm3 = log1p_thresh2
    vmovsd    xmm2,QWORD PTR L__real_log1p_thresh1 ; xmm2 = log1p_thresh1
    vpand      xmm1,xmm0,QWORD PTR L__sign_mask_64
    
    ;check for Nans and Infs
    vpand      xmm4,xmm0,QWORD PTR L__exp_mask  ; remove the mantissa and sign
    vmovq      rax, xmm4 
	cmp        rax, QWORD PTR L__exp_mask
    je         L__x_is_inf_or_nan


    vmovq      rcx, xmm1
	cmp        rcx,QWORD PTR L__real_epsilon ;check for epsilons
    jle        L__x_is_epsilon
    vcomisd    xmm0,QWORD PTR L__real_minus_one ;check input less than -1.0
    jb         L__x_less_than_minus_one
    
    ;get the mask for exactly -1.0
    vcomisd    xmm0,QWORD PTR L__real_minus_one ; check for input = -1.0
    je         L__x_is_minus_one


    ;calculate log1p
    ; IF (x < log1p_thresh1 || x > log1p_thresh2) 
    vminsd     xmm3,xmm0,xmm3 ;max    log1p_thresh2),(x
    vmaxsd     xmm4,xmm0,xmm2 ;min    log1p_thresh1),(x
    vcomisd    xmm3,xmm4 ; if both are equal
    je         L__inside_the_threshold

ALIGN 16
L__outside_the_threshold:    
    ;calculate outside the threshold range
    vaddsd     xmm5,xmm0,QWORD PTR L__one_mask_64  ;xmm5 = 1.0 +x
    vpand      xmm4,xmm5,QWORD PTR L__exp_mask
    vpsrlq     xmm4,xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    vpsubq     xmm4,xmm4,QWORD PTR L__mask_1023
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    vpand      xmm6,xmm5,QWORD PTR L__index_mask1
    vpand      xmm2,xmm5,QWORD PTR L__index_mask2
    vpor       xmm6,xmm6,QWORD PTR L__index_mask3
    
    vpsrlq     xmm2,xmm2,45 ; ((ux & 00000200000000000h) >> 45)
    vpsrlq     xmm6,xmm6,46 ; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    vpaddq     xmm6,xmm6,xmm2 ; index = xmm6
    vcvtdq2pd  xmm2,xmm6
    
    vmulsd     xmm2,xmm2,QWORD PTR L__index_constant1 ; f1 =index*0.0078125
    vpsubq     xmm6,xmm6,QWORD PTR L__index_constant2 ; index -= 64

    vpand      xmm5,xmm5,QWORD PTR L__mantissa_mask
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS    f),_DP64;
    vpor       xmm5,xmm5,QWORD PTR L__one_mask_64 ; xmm5 = f
    
    vpshufd    xmm3,xmm4,0F0h ; [11 11 00 00] = 0F0h higher 64 is not concerned here
    vmovsd    xmm1,QWORD PTR L__plus_sixtyone_minus_two
    vpcmpgtd   xmm1,xmm1,xmm3 
    vpsrldq    xmm3,xmm1,4 
    ; IF (xexp <= -2 || xexp >= MANTLENGTH_DP64 + 8)
    vpandn     xmm1,xmm3,xmm1 ; xmm1 stores the mask for all the numbers which lie between -2 and 61

    ;vpshufd   xmm1,xmm3,050h ; [01 01 00 00] = 050h
    vsubpd     xmm5,xmm5,xmm2 ; f2 = f - f1;
    ;ELSE
    vmovsd    xmm3,QWORD PTR L__mask_1023
    vpsubq     xmm3,xmm3,xmm4
    ;ux = (unsigned long long)(03ffh - xexp) << EXPSHIFTBITS_DP64;
    ;PUT_BITS_DP64[ux + m2];
    vpsllq    xmm3,xmm3,52 ; xmm3 = m2;
    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    vmovd      r10d,xmm6 ; move lower order index

    vsubsd    xmm6,xmm3,xmm2 ; (m2 - f1)
    vfmadd231sd  xmm6 ,xmm3,xmm0 ; xmm6 =f2
    ;xmm2=f   xmm3,1 = m2*x
    ;f2 = xmm6/xmm5 f1 = xmm2 

    ;vpcmov    xmm5 ,xmm6,xmm5,xmm1
	VANDNPD xmm0,xmm1,xmm5
	VANDPD  xmm5,xmm6,xmm1
	VORPD   xmm5,xmm5,xmm0

  

    ;vpsrldq   xmm6,xmm6,8
    ;vmovd     r11,xmm6 ; move higher order index
    lea        r9,QWORD PTR L__ln_lead_64_table
    lea        r8,QWORD PTR L__ln_tail_64_table
    vmovlpd    xmm6,xmm6,QWORD PTR[r9 + r10 * 8]
    ;vmovhpd   xmm6,xmm6,QWORD PTR[r9 + r11 * 8] ; xmm6 = z1 = ln_lead_table[index]
    vmovlpd    xmm3,xmm3,QWORD PTR[r8 + r10 * 8]
    ;vmovhpd   xmm3,xmm3,QWORD PTR[r8 + r11 * 8] ; xmm3 = q = ln_tail_table[index]

    vfmadd231sd   xmm2 ,xmm5,QWORD PTR L__real_half ; (f1 + 0.5 * f2);
    vdivsd     xmm2,xmm5,xmm2  ; u = f2 / (f1 + 0.5 * f2);
    vmulsd     xmm1,xmm2,xmm2  ; v = u*u

    vmovsd    xmm5,QWORD PTR L__real_cb_3 ; cb_3
    vfmadd213sd   xmm5,xmm1,QWORD PTR L__real_cb_2 ; cb_2 + v*cb_3
     
    vfmadd213sd   xmm5,xmm1,QWORD PTR L__real_cb_1 ; (cb_1 + v * (cb_2 + v * cb_3))
    vmulsd     xmm5,xmm1,xmm5 ; poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    ;poly = xmm5 u = xmm2 q = xmm3
    vfmadd213sd   xmm5,xmm2,xmm2
    vaddsd     xmm5,xmm3,xmm5   ; z2 = q + (u + u * poly)

    vcvtdq2pd  xmm4,xmm4 ; xexp (float) 
    vmovapd    xmm3,xmm4 ; xmm3 = xexp
    vfmadd132sd   xmm4,xmm6,QWORD PTR L__real_log2_lead ; r1 = (xexp * log2_lead + z1)
    vfmadd132sd   xmm3,xmm5,QWORD PTR L__real_log2_tail ; r2 = (xexp * log2_tail + z2)
    vaddsd     xmm0,xmm3,xmm4 ; return r1 + r2
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret

    ;;;;END of calculating outside the threshold




ALIGN 16
L__inside_the_threshold:    
    ;;;;Now calculate when the value lies within the threshold
    ;xmm0 still contains the input x = r1
    vmovsd    xmm3,QWORD PTR L__plus_two ; xmm3 = 2.0
    vmovsd    xmm1,QWORD PTR L__real_ca_4 ; xmm1 = ca_4

    vmovsd    xmm5,QWORD PTR L__real_ca_1 ; xmm5 = ca_1
    vmovapd    xmm4,xmm0         ;  r1
    vaddsd     xmm2,xmm4,xmm3  ; (2.0 + r)
    vdivsd     xmm6,xmm4,xmm2   ; xmm6 = u = r / (2.0 + r); 
    vmulsd     xmm2,xmm0,xmm6   ; correction = r * u ; TBD
    vaddsd     xmm6,xmm6,xmm6  ; u = u+ u
    vmulsd     xmm3,xmm6,xmm6   ; v = u * u
    ;r1 = xmm4 v = xmm    u,9 = xmm6,correction = xmm2
    vfmadd213sd   xmm1 ,xmm3,QWORD PTR L__real_ca_3 ; (ca_3 + (v * ca_4))
    vfmadd213sd   xmm1,xmm3,QWORD PTR L__real_ca_2  ; (ca_2 + v * (ca_3 + v * ca_4))
    vfmadd213sd   xmm1,xmm3,xmm5 ; (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    vmulsd     xmm1,xmm1,xmm3        ; v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    vfmsub213sd   xmm1,xmm6,xmm2  ; r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 +  v * ca_4))) - correction) 
    vaddpd     xmm0,xmm1,xmm4
    RestoreRegsAVX xmm6,save_xmm6
     StackDeallocate stack_size
    ret

    ;END calculating within the threshold



ALIGN 16
    ;process inputs = 03ca0000000000000h epsilon
L__x_is_epsilon:
    ;vmovsd    xmm1,xmm0
    ;mov       r8d,DWORD PTR L__flag_x_inexact
    ;call      fname_special
    RestoreRegsAVX xmm6,save_xmm6
     StackDeallocate stack_size
    ret


ALIGN 16
    ; now restore if there are some inputs less than -1.0
L__x_less_than_minus_one:
    vmovsd    xmm1,QWORD PTR L__real_neg_qnan
    mov       r8d,DWORD PTR L__flag_x_neg
    call      fname_special
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN 16
    ; now restore if there are some inputs with -1.0
L__x_is_minus_one:
    vmovsd     xmm1,QWORD PTR L__real_ninf
    mov        r8d,DWORD PTR L__flag_x_zero
    call       fname_special
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN 16
L__x_is_inf_or_nan:
    ;now restore the Nans and Infs
	vmovq  rax,xmm0
    cmp    rax,QWORD PTR L__real_inf
    je     L__finish

    cmp    rax,QWORD PTR L__real_ninf
    je     L__x_less_than_minus_one

    or     rax,QWORD PTR L__real_qnanbit
    movd   xmm1,rax
    mov    r8d,DWORD PTR L__flag_x_nan
    call   fname_special

ALIGN  16
L__finish:
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret

    ;vpand     xmm3,xmm0,QWORD PTR L__sign_bit_64 ; for negative infities we need to set the result as Qnan
    ;vaddpd    xmm0,xmm0,xmm0
    ;so we get the sign bit and move it to the qnan Bit.
    ;then OR it with Qnan/inf result
    ;vpsrlq    xmm3,xmm3,12
    ;vpor      xmm0,xmm0,xmm3
    ;RestoreRegsAVX xmm6,save_xmm6
    ;StackDeallocate stack_size
    ;ret

    
fname        endp
TEXT    ENDS

data SEGMENT READ
CONST    SEGMENT


ALIGN 16
; these codes and the ones in the corresponding .c file have to match
L__flag_x_zero          DD 00000001
L__flag_x_neg           DD 00000002
L__flag_x_nan           DD 00000003
L__flag_x_inexact       DD 00000004

ALIGN 16
L__sign_mask_64       DQ 07FFFFFFFFFFFFFFFh
                      DQ 07FFFFFFFFFFFFFFFh
L__real_epsilon       DQ 03ca0000000000000h
                      DQ 03ca0000000000000h
L__real_neg_qnan    DQ 0fff8000000000000h   ; neg qNaN
                    DQ 0000000000000000h
L__real_qnanbit     DQ 00008000000000000h
                    DQ 00000000000000000h
L__real_minus_one     DQ  0BFF0000000000000h
                      DQ  0BFF0000000000000h
L__real_inf         DQ 07ff0000000000000h    ; +inf
                    DQ 00000000000000000h
L__exp_mask           DQ 07ff0000000000000h   ;
                      DQ 07ff0000000000000h
L__exponent_compare   DQ 07FF000007FF00000h  
                      DQ 07FF000007FF00000h
; The values exp(-1/16)-1 and exp(1/16)-1 */
L__real_log1p_thresh2   DQ 03fb082b577d34ed8h ;  6.44944589178594318568e-02;  
                        DQ 03fb082b577d34ed8h 
L__real_log1p_thresh1   DQ 0bfaf0540438fd5c4h ; -6.05869371865242201114e-02,
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
;L__plus_sixtyone_minus_two    DQ 00000003D0000003Dh
;                              DQ 0FFFFFFFEFFFFFFFEh
L__plus_sixtyone_minus_two    DQ 0FFFFFFFE0000003Dh
                              DQ 0FFFFFFFEFFFFFFFEh
L__real_half          DQ 03fe0000000000000h ; 1/2
                      DQ 03fe0000000000000h

; Approximating polynomial coefficients for other x 
L__real_cb_3         DQ 03f6249423bd94741h ; 2.23219810758559851206e-03;  
                     DQ 03f6249423bd94741h
L__real_cb_2         DQ 03f89999999865edeh ; 1.24999999978138668903e-02, 
                     DQ 03f89999999865edeh
L__real_cb_1         DQ 03fb5555555555557h ; 8.33333333333333593622e-02,
                     DQ 03fb5555555555557h
L__real_log2_lead    DQ 03fe62e42e0000000h ; log2_lead  6.93147122859954833984e-01
                     DQ 03fe62e42e0000000h
L__real_log2_tail    DQ 03e6efa39ef35793ch ; log2_tail  5.76999904754328540596e-08
                     DQ 03e6efa39ef35793ch
L__plus_two          DQ 04000000000000000h
                     DQ 04000000000000000h
; Approximating polynomial coefficients for x near 0.0 
L__real_ca_4         DQ 03f3c8034c85dfff0h ; 4.34887777707614552256e-04, 
                     DQ 03f3c8034c85dfff0h
L__real_ca_3         DQ 03f62492307f1519fh ; 2.23213998791944806202e-03, 
                     DQ 03f62492307f1519fh
L__real_ca_2         DQ 03f89999999bac6d4h ; 1.25000000037717509602e-02, 
                     DQ 03f89999999bac6d4h
L__real_ca_1         DQ 03fb55555555554e6h ; 8.33333333333317923934e-02, 
                     DQ 03fb55555555554e6h
L__real_qnan         DQ 07ff8000000000000h   ; qNaN
                     DQ 07ff8000000000000h
L__real_ninf         DQ 0fff0000000000000h   ; -inf
                     DQ 0fff0000000000000h
L__sign_bit_64       DQ 08000000000000000h 
                     DQ 08000000000000000h

CONST    ENDS
data ENDS
END

