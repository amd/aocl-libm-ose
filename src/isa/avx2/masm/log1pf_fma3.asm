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
; log1pf_fma3.S
;
; An implementation of the log1pf libm function.
;
; Prototype:
;
;     float log1pf(float x);
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


include fm.inc
include log_tables.inc

ALM_PROTO_FMA3 log1pf

; local variable storage offsets
save_xmm6       EQU     00h
save_xmm7       EQU     020h
save_tmp       EQU     040h
stack_size      EQU     068h ; We take 8 as the last nibble to allow for 
                              ; alligned data movement.

fname_special   TEXTEQU <alm_logf_special>
EXTERN fname_special:PROC
 
text SEGMENT EXECUTE
ALIGN 16
PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    
    .ENDPROLOG   



    ; compute exponent part
    vmovd    xmm3,DWORD PTR L__real_qnan
    vmovd    xmm5,DWORD PTR L__real_ninf
    vmovd    xmm6,DWORD PTR L__negative_one
    vpand    xmm1,xmm0,DWORD PTR L__sign_mask_32

    ;get the mask for Nans and Infs
    vpand    xmm2,xmm0,DWORD PTR L__exp_mask  ; remove the mantissa
	vmovd    eax, xmm2
	cmp      eax, DWORD PTR L__exp_mask
    je       L__input_is_Nan_Inf

    vmovd    ecx, xmm1
	cmp      ecx,DWORD PTR L__real_epsilon ;check for epsilons
	jle      L__input_is_less_than_epsilon

    vcomiss  xmm0,xmm6 ;check for input less than -1.0
    jb       L__input_less_than_minus_one
    
    ;get the mask for exactly -1.0
    vcomiss  xmm0,xmm6
    je       L__input_is_minus_one

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;calculate log1pf for lower half
    vmovapd    xmm3,XMMWORD PTR L__one_mask_64
    vmovapd    xmm4,XMMWORD PTR L__exp_mask_64
    vmovapd    xmm6,XMMWORD PTR L__index_mask1
    vmovapd    xmm2,XMMWORD PTR L__index_mask2



    vcvtss2sd  xmm5,xmm5,xmm0
    vmovsd    QWORD PTR [save_tmp + rsp] ,xmm5
    vaddsd     xmm5,xmm5,xmm3 ;xmm5 = 1.0 +x

    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    vpand      xmm6,xmm5,xmm6
    vpand      xmm2,xmm5,xmm2
    vpor       xmm6,xmm6,XMMWORD PTR L__index_mask3
    vpsrlq     xmm2,xmm2,45
    vpsrlq     xmm6,xmm6,46
    vpaddd     xmm6,xmm2,xmm6
    vcvtdq2pd  xmm7,xmm6 ; it will always be a positive integer
    

    vpand      xmm4,xmm5,xmm4
    vpsrlq     xmm4,xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    vpsubd     xmm4,xmm4,XMMWORD PTR L__mask_1023_32bit
    vpand      xmm5,xmm5,XMMWORD PTR L__mantissa_mask
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS    f),_DP64;
    vpor       xmm5,xmm5,xmm3 ; f = ymm5
    vmulsd     xmm7,xmm7,QWORD PTR L__index_constant1 ; f1 =index*0.015625
    vpsubd     xmm6,xmm6,DWORD PTR L__index_constant2 ; index -= 64
    
; xmm4=xexp
    vmovaps    xmm2,DWORD PTR L__plus_sixtyone
    vmovaps    xmm1,DWORD PTR L__minus_two
    vmovaps    xmm3,DWORD PTR L__mask_1023
    ;     IF( (xexp <= -2 ) || (xexp >= MANTLENGTH_DP64 + 8))
    vpcmpgtq   xmm2,xmm2,xmm4   ;( xmm2>xmm4) (61 > xexp)
    vpcmpgtq   xmm1,xmm1,xmm4 ;(xmm1>xmm4) (-2 > xexp)
    vpandn     xmm2,xmm1,xmm2  ; xmm2 stores the mask for all the numbers which lie between -2 and 61

    vmovaps   xmm1,xmm2
    vsubsd    xmm5,xmm5,xmm7 ; f2 = f - f1;
    ;ELSE
    vpsubd    xmm3,xmm3,xmm4
    
    ;ux = (unsigned long long)(03ffh - xexp) << EXPSHIFTBITS_DP64;
    ;PUT_BITS_DP64[ux + m2];
    vpsllq    xmm3,xmm3,52 ; xmm3 = m2;
    vmulsd    xmm1,xmm3,QWORD PTR [save_tmp + rsp] ; xmm1 = m2*x
    ;ymm7=f    ymm11,1=m2,ymm13 = m2*x
    vsubsd    xmm3,xmm3,xmm7 ; (m2 - f1)
    vaddsd    xmm3,xmm3,xmm1 ; xmm3 = f2 = (m2 - f1) + m2*dx

    ; z1 = ln_lead_table[index];
    ; q = ln_tail_table[index];
    lea    r9,DWORD PTR L__ln_lead_64_table
    lea    r8,DWORD PTR L__ln_tail_64_table
    
    vmovd    r10d,xmm6 ; move 1st order index
    vmovq    xmm6,QWORD PTR[r9 + r10 * 8]



    vmovapd    xmm1,XMMWORD PTR L__real_cb_2
    ;f2 = ymm11/ymm5 f1 = xmm7 
    ;vpcmov    xmm5 ,xmm3,xmm5,xmm2
	VANDNPD xmm0,xmm2,xmm5
	VANDPD  xmm5,xmm3,xmm2
	VORPD   xmm5,xmm5,xmm0
    
	vmovdqa xmm2,xmm7
    vfmadd231sd    xmm2,xmm5,QWORD PTR L__real_half;(f1 + 0.5 * f2);
    vdivsd    xmm2,xmm5,xmm2  ; u = f2 / (f1 + 0.5 * f2);
    vmulsd    xmm5,xmm2,xmm2  ; v = u*u

    vfmadd213sd    xmm1 ,xmm5,QWORD PTR L__real_cb_1
    vmulsd    xmm5,xmm1,xmm5  ; poly = v * (cb_1 + v * cb_2)
    ;poly = xmm5 u = xmm2 
    vfmadd213sd   xmm5,xmm2,xmm2 ; (u + u * poly)
    vaddsd    xmm5,xmm5,QWORD PTR[r8 + r10 * 8]   ; z2 = q + (u + u * poly)
    ;xmm5 = z    2 = q,xmm2 = u,xmm6 = z1 
    vcvtdq2pd    xmm4,xmm4 ; xexp (float)
    
    vfmadd132sd   xmm4,xmm6,QWORD PTR L__real_log2
    vaddsd    xmm4,xmm4,xmm5 ; r = (xexp * log2 + z1 + z2)
    
    vcvtsd2ss    xmm0,xmm0,xmm4 ;Final result
    ; xmm0 is the result of log1p  
RestoreRegsAVX xmm6,save_xmm6
RestoreRegsAVX xmm7,save_xmm7     
	 StackDeallocate stack_size
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


L__input_is_Nan_Inf:    
    ;now restore the Nans and Infs
	vmovd  eax,xmm0
    cmp    eax,DWORD PTR L__real_inf
    je     L__finish

    cmp    eax,DWORD PTR L__real_ninf
    je     L__input_less_than_minus_one

    or     eax,DWORD PTR L__real_qnanbit
    movd   xmm1,eax
    mov    r8d,DWORD PTR L__flag_x_nan
    call   fname_special
    
L__finish:
RestoreRegsAVX xmm6,save_xmm6
RestoreRegsAVX xmm7,save_xmm7
    StackDeallocate stack_size
    ret

    ;vpand    xmm3,xmm0,DWORD PTR L__sign_bit_32 ; for negative infities we need to set the result as Qnan
    ;vaddss    xmm0,xmm0,xmm0
    ;so we get the sign bit and move it to the qnan Bit.
    ;then OR it with Qnan/inf result
    ;vpsrld    xmm3,xmm3,9
    ;vpor    xmm0,xmm0,xmm3
    ;RestoreAllXMMRegs
    ; StackDeallocate stack_size
    ;ret

    
    ;process inputs less than 03380000033800000h
L__input_is_less_than_epsilon:    
    ;result is equal to the input
RestoreRegsAVX xmm6,save_xmm6
RestoreRegsAVX xmm7,save_xmm7
     StackDeallocate stack_size
    ret
 

L__input_less_than_minus_one:    
    ;now restore if there are some inputs less than -1.0
    vmovaps   xmm0,xmm3
    vmovss    xmm1,DWORD PTR L__real_neg_qnan
    mov       r8d,DWORD PTR L__flag_x_neg
    call      fname_special
RestoreRegsAVX xmm6,save_xmm6
RestoreRegsAVX xmm7,save_xmm7
    StackDeallocate stack_size
    ret


L__input_is_minus_one:
    ;now restore if there are some inputs with -1.0
    vmovaps    xmm0,xmm5
	vmovss     xmm1,DWORD PTR L__real_ninf
    mov        r8d,DWORD PTR L__flag_x_zero
    call       fname_special
RestoreRegsAVX xmm6,save_xmm6
RestoreRegsAVX xmm7,save_xmm7     
	 StackDeallocate stack_size
    ret


   
    
fname        endp
TEXT    ENDS

data SEGMENT READ
CONST    SEGMENT


ALIGN 16
; these codes and the ones in the corresponding .c file have to match
L__flag_x_zero         DD 00000001
L__flag_x_neg          DD 00000002
L__flag_x_nan          DD 00000003

ALIGN 16
L__real_neg_qnan    DQ 0ffC00000ffC00000h   ; neg qNaN
                    DQ 0ffC00000ffC00000h
L__real_qnan        DQ 07FC000007FC00000h   ; qNaN
                    DQ 07FC000007FC00000h
L__real_qnanbit     DQ 000040000000400000h   ; quiet nan bit
                    DQ 000040000000400000h
L__negative_one     DQ 0bf800000bf800000h
                    DQ 0bf800000bf800000h
L__sign_mask_32     DQ 07FFFFFFF7FFFFFFFh
                    DQ 07FFFFFFF7FFFFFFFh
L__real_epsilon     DQ 03380000033800000h
                    DQ 03380000033800000h
L__one_mask_64      DQ 03fF0000000000000h; 1
                    DQ 03fF0000000000000h
L__exp_mask_64      DQ 07FF0000000000000h;
                    DQ 07FF0000000000000h
L__mantissa_mask    DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                    DQ 0000FFFFFFFFFFFFFh
L__index_mask1      DQ 0000fc00000000000h
                    DQ 0000fc00000000000h
L__index_mask2      DQ 00000200000000000h
                    DQ 00000200000000000h
L__index_mask3      DQ 00010000000000000h
                    DQ 00010000000000000h
L__selector_constant   DQ 00000000300000001h
                       DQ 00000000700000005h
L__index_constant1  DQ 03F90000000000000h
                    DQ 03F90000000000000h
L__mask_1023_32bit   DQ 0000003ff000003ffh
                     DQ 0000003ff000003ffh
L__index_constant2  DQ 00000004000000040h
                    DQ 00000004000000040h
L__plus_sixtyone    DQ 00000003D0000003Dh
                    DQ 00000003D0000003Dh
L__minus_two        DQ 0FFFFFFFEFFFFFFFEh
                    DQ 0FFFFFFFEFFFFFFFEh
L__mask_1023        DQ 000000000000003ffh
                    DQ 000000000000003ffh
L__mask_127_32bit   DQ 00000007f0000007fh
                    DQ 00000007f0000007fh
L__real_half        DQ 03fe0000000000000h ; 1/2
                    DQ 03fe0000000000000h
; Approximating polynomial coefficients for other x 
L__real_cb_1        DQ 03fb5555555555557h ; 8.33333333333333593622e-02,
                    DQ 03fb5555555555557h
L__real_cb_2        DQ 03f89999999865edeh ; 1.24999999978138668903e-02, 
                    DQ 03f89999999865edeh
L__real_log2        DQ 03fe62e42fefa39efh ; log2 = 6.931471805599453e-01
                    DQ 03fe62e42fefa39efh 
L__exp_mask         DQ 07F8000007F800000h   ;
                    DQ 07F8000007F800000h
L__exponent_compare DQ 07F8000007F800000h  
                    DQ 07F8000007F800000h
L__sign_bit_32      DQ 08000000080000000h 
                    DQ 08000000080000000h
L__real_ninf           DQ 00ff800000ff800000h   ; -inf
                        DQ 00ff800000ff800000h
L__real_inf            DQ 007f8000007f800000h   ; +inf
                        DQ 007f8000007f800000h


CONST    ENDS
data ENDS
END

