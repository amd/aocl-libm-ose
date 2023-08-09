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
; vrsa_log1pf_fma3.S
;
; An implementation of the vrsa_log1pf libm function.
;
; Prototype:
;
;     __m128 vrsa_log1pf(__m128 x);
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

ALM_PROTO_FMA3 vrsa_log1pf

; local variable storage offsets
save_rdi	    equ		00h
save_rsi	    equ		10h
save_xmm6       EQU     020h
save_xmm7       EQU     040h
save_xmm8       EQU     060h
save_xmm9       EQU     080h
save_xmm10      EQU     0A0h
save_xmm11      EQU     0C0h
save_xmm12      EQU     0E0h
save_xmm13      EQU     0100h
save_xmm14      EQU     0120h
save_xmm15      EQU     0140h

stack_size      EQU     0168h 		; We take 8 as the last nibble to allow for 
                             		; alligned data movement.

 
text SEGMENT EXECUTE
ALIGN 16
PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi 
    SaveAllXMMRegs
    .ENDPROLOG


 mov rdi,rcx
 mov rsi, rdx
 mov rdx, r8
 
 cmp edi,00h 
 jle L__return
 cmp rsi,00h
 je L__return
 cmp rdx,00h
 je L__return
 
ALIGN 16 
L__process_next4:
 sub rdi,04h
 cmp rdi,-1
 jle L__process_first123
 movdqu xmm0,XMMWORD PTR [rsi + rdi * 4]
 jmp L__start
 
ALIGN 16 
L__process_first123:
 je L__process_first3
 cmp rdi,-3
 jl L__return ;multiple of all,4elements are processed rdi == -4
 je L__process_first1 
 ;process first 2 
 mov rcx,00h
 movsd xmm0,QWORD PTR[rsi + rcx * 4]
 jmp L__start

ALIGN 16
L__process_first1:
 mov rcx,00h
 movss xmm0,DWORD PTR[rsi + rcx * 4]
 jmp L__start 

ALIGN 16 
L__process_first3:
 mov rcx,01h
 movsd xmm0,QWORD PTR[rsi + rcx * 4] 
 dec rcx ; zero
 movss xmm1,DWORD PTR[rsi + rcx * 4]
 pslldq xmm0,4
 por xmm0,xmm1 
 
ALIGN 16 
L__start: 

   ; compute exponent part
    vmovaps    xmm3,DWORD PTR L__real_qnan
    vmovaps    xmm5,DWORD PTR L__real_ninf_32
    vmovaps    xmm6,DWORD PTR L__negative_one
    vpand    xmm1,xmm0,DWORD PTR L__sign_mask_32

    vcmpleps    xmm1,xmm1,DWORD PTR L__real_epsilon ;mask for epsilons
    vcmpltps    xmm2,xmm0,xmm6 ;mask for input less than -1.0
    ;get the mask for exactly -1.0
    vcmpeqps    xmm15,xmm0,xmm6
 
    ;process inputs less than 03380000033800000h
    vpand    xmm4,xmm0,xmm1 
    ;now restore if there are some inputs less than -1.0
    vpor    xmm6,xmm2,xmm1 ; store the mask for final use
    ;vpcmov    xmm1 ,xmm3,xmm4,xmm2
	vandnpd xmm11  , xmm2, xmm4 
	vandpd  xmm1   , xmm3, xmm2 
	vorpd   xmm1   , xmm1, xmm11

    ; now restore if there are some inputs with -1.0
    vpor    xmm2,xmm6,xmm15 ; store the mask for final use
    ;vpcmov    xmm1 ,xmm5,xmm1,xmm15
	vandnpd xmm11 , xmm15,  xmm1
	vandpd xmm1  , xmm5, xmm15
	vorpd xmm1  , xmm1, xmm11
;xmm2 = mask for -1.0,<-1    and,.0 less than epsilon
;xmm1 = result for -1.0,<-1    and,.0 less than epsilon
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;calculate log1pf for lower half
    vbroadcastsd    ymm3,QWORD PTR L__one_mask_64
    vbroadcastsd    ymm4,QWORD PTR L__exp_mask_64
    vbroadcastsd    ymm11,QWORD PTR L__mantissa_mask
    vbroadcastsd    ymm6,QWORD PTR L__index_mask1
    vbroadcastsd    ymm7,QWORD PTR L__index_mask2
    vbroadcastsd    ymm8,QWORD PTR L__index_mask3
    vmovaps    xmm13,DWORD PTR L__selector_constant

    vcvtps2pd    ymm5,xmm0
    vmovapd    ymm14,ymm5
    vaddpd    ymm5,ymm5,ymm3 ;ymm5 = 1.0 +x

    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    vandpd    ymm6,ymm5,ymm6
    vandpd    ymm7,ymm5,ymm7
    vorpd    ymm6,ymm6,ymm8
    vextractf128  xmm8,ymm7,1
  ;  vpermil2ps    xmm7,xmm7,xmm8,xmm13,0  
    vshufps  xmm7, xmm7,xmm8,221
    vextractf128  xmm8,ymm6,1
  ;  vpermil2ps    xmm6,xmm6,xmm8,xmm13,0
    vshufps  xmm6, xmm6, xmm8,221
    vpsrld    xmm7,xmm7,13
    vpsrld    xmm6,xmm6,14
    vpaddd    xmm6,xmm7,xmm6
    vcvtdq2pd    ymm7,xmm6
    vbroadcastsd    ymm8,QWORD PTR L__index_constant1 ; 
    
    vmovaps       xmm12,DWORD PTR L__mask_1023_32bit
    vandpd        ymm4,ymm5,ymm4
    vextractf128  xmm15,ymm4,1
  ;  vpermil2ps    xmm4 ,xmm4,xmm15,xmm13,0
    vshufps xmm4, xmm4, xmm15,221
    vpsrld        xmm4,xmm4,20
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    vpsubd    xmm4,xmm4,xmm12
    vandps        ymm5,ymm5,ymm11
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS    f),_DP64;
    vorps     ymm5,ymm5,ymm3 ; f = ymm5
    vmulpd    ymm7,ymm7,ymm8 ; f1 =index*0.015625
    vpsubd    xmm6,xmm6,DWORD PTR L__index_constant2 ; index -= 64
    
; xmm4=xexp
    vmovaps    xmm9,DWORD PTR L__plus_sixtyone
    vmovaps    xmm10,DWORD PTR L__minus_two
    vmovaps    xmm11,DWORD PTR L__mask_127_32bit
    ;     IF( (xexp <= -2 ) || (xexp >= MANTLENGTH_DP64 + 8))
    vpcmpgtd   xmm9,xmm9,xmm4   ; (xmm9>xmm4) (61 > xexp)
    vpcmpgtd   xmm10,xmm10,xmm4 ;(xmm10>xmm4) (-2 > xexp)
    vpandn     xmm9,xmm10,xmm9 ; xmm9 stores the mask for all the numbers which lie between -2 and 61
    vmovaps    xmm10,xmm9
    vpsrldq    xmm10,xmm10,8
    vinsertf128  ymm9,ymm9,xmm10,1
    vpermilps    ymm9,ymm9,050h ; 050h = 01 01 00 00
    vsubpd    ymm5,ymm5,ymm7 ; f2 = f - f1;
    ;ELSE
    vpsubd    xmm11,xmm11,xmm4
    
    ;ux = (unsigned long long)(03ffh - xexp) << EXPSHIFTBITS_DP64;
    ;PUT_BITS_DP64[ux + m2];
    vpslld    xmm11,xmm11,23 ; xmm11 = m2;
    vcvtps2pd    ymm11,xmm11
    vmulpd    ymm13,ymm11,ymm14 ; xmm13 = m2*x
    ;ymm7=f    ymm11,1=m2,ymm13 = m2*x
    vsubpd    ymm11,ymm11,ymm7 ; (m2 - f1)
    vaddpd    ymm11,ymm11,ymm13 ; xmm11 = f2 = (m2 - f1) + m2*dx
    ; z1 = ln_lead_table[index];
    ; q = ln_tail_table[index];
    lea    r9,DWORD PTR L__ln_lead_64_table
    lea    r8,DWORD PTR L__ln_tail_64_table
    
    vmovd    r10d,xmm6 ; move 1st order index
    vpsrldq    xmm6,xmm6,4
    vmovlpd    xmm14,xmm14,QWORD PTR[r9 + r10 * 8]
    vmovlpd    xmm12,xmm12,QWORD PTR[r8 + r10 * 8]

    vmovd    r11d,xmm6 ; move 2nd order index
    vpsrldq    xmm6,xmm6,4
    vmovhpd    xmm14,xmm14,QWORD PTR[r9 + r11 * 8]
    vmovhpd    xmm12,xmm12,QWORD PTR[r8 + r11 * 8]

    vmovd    r10d,xmm6 ; move 3rd order index
    vpsrldq    xmm6,xmm6,4
    vmovlpd    xmm15,xmm15,QWORD PTR[r9 + r10 * 8]
    vmovlpd    xmm13,xmm13,QWORD PTR[r8 + r10 * 8]

    vmovd    r11d,xmm6 ; move 4th order index
    vmovhpd    xmm15,xmm15,QWORD PTR[r9 + r11 * 8]
    vmovhpd    xmm13,xmm13,QWORD PTR[r8 + r11 * 8]

    vinsertf128    ymm6 ,ymm14,xmm15,1 ; ymm6 = z1 = ln_lead_table[index]
    vinsertf128    ymm8 ,ymm12,xmm13,1 ; ymm8 = q = ln_tail_table[index]
    
    vbroadcastsd    ymm12,QWORD PTR L__real_half
    vbroadcastsd    ymm13,QWORD PTR L__real_cb_1
    vbroadcastsd    ymm15,QWORD PTR L__real_cb_2
    ;f2 = ymm11/ymm5 f1 = xmm7 
    ;vpcmov    ymm11 ,ymm11,ymm5,ymm9
	vandnpd ymm14   ,  ymm9, 	 ymm5 
	vandpd   ymm11  ,  ymm11,	 ymm9 
	vorpd    ymm11  ,  ymm11,	 ymm14

    vmovapd   ymm5,ymm11
    
    ;vfmaddpd  ymm7 ,ymm11,ymm12,ymm7;(f1 + 0.5 * f2);
	vfmadd231pd  ymm7,ymm11,ymm12
    vdivpd    ymm7,ymm5,ymm7  ; u = f2 / (f1 + 0.5 * f2);
    vmulpd    ymm5,ymm7,ymm7  ; v = u*u

    ;vfmaddpd      ymm14 ,ymm5,ymm15,ymm13
	vfmadd231pd    ymm13,ymm5,ymm15
    vmulpd        ymm5,ymm13,ymm5  ; poly = v * (cb_1 + v * cb_2)
    vbroadcastsd  ymm13,QWORD PTR L__real_log2
    ;poly = xmm5 u = xmm7 q = xmm8
    ;vfmaddpd      ymm5 ,ymm5,ymm7,ymm7 ; (u + u * poly)
	vfmadd132pd  ymm5,ymm7,ymm7
    vaddpd        ymm5,ymm5,ymm8   ; z2 = q + (u + u * poly)
    ;xmm5 = z    xmm8,2 = q,xmm7 = u,xmm6 = z1 
    vcvtdq2pd     ymm4,xmm4 ; xexp (float)
    
    ;vfmaddpd  ymm4 ,ymm4,ymm13,ymm6
	vfmadd213pd  ymm4,ymm13,ymm6
    vaddpd    ymm4,ymm4,ymm5 ; r = (xexp * log2 + z1 + z2)
    
    vcvtpd2ps xmm4,ymm4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; xmm0 is the result of log1p  
;xmm2 = mask for -1.0,<-1    and,.0 less than epsilon
;xmm1 = result for -1.0,<-1    and,.0 less than epsilon
    ;vpcmov    xmm2 ,xmm1,xmm4,xmm2
	vandnpd    xmm14  ,  xmm2, xmm4 
	vandpd     xmm2   ,  xmm1, xmm2 
	vorpd      xmm2   ,  xmm2, xmm14

    ;get the mask for Nans and Infs
    vpand    xmm14,xmm0,DWORD PTR L__exp_mask  ; remove the mantissa
    vpcmpeqd    xmm14,xmm14,DWORD PTR L__exponent_compare ; xmm14 stores the mask for nans and Infs

    ;now restore the Nans and Infs
    vpand    xmm3,xmm0,DWORD PTR L__sign_bit_32 ; for negative infities we need to set the result as Qnan
    vaddps    xmm0,xmm0,xmm0
    ;so we get the sign bit and move it to the qnan Bit.
    ;then OR it with Qnan/inf result
    vpsrld    xmm3,xmm3,9
    
    vpor    xmm0,xmm0,xmm3
    ;vpcmov    xmm0 ,xmm0,xmm2,xmm14
	vandnpd   xmm3  ,  xmm14, xmm2 
	vandpd    xmm0  ,  xmm0,  xmm14 
	vorpd      xmm0  ,  xmm0,  xmm3 


    cmp rdi,-1
 jle L__store123
 movdqu XMMWORD PTR[rdx + rdi * 4],xmm0
 jmp L__process_next4 
 
ALIGN 16 
L__store123:
 je L__store3
 cmp rdi,-3 
 je L__store1 
 ;store 2 
 add rdi,02h
 movsd QWORD PTR[rdx + rdi * 4],xmm0 
 jmp L__return

ALIGN 16 
L__store3:
 movdqa xmm1,xmm0
 psrldq xmm0,4
 inc rdi
 movss DWORD PTR[rdx + rdi * 4],xmm1
 inc rdi
 movsd QWORD PTR[rdx + rdi * 4],xmm0 
 jmp L__return
 
ALIGN 16 
L__store1:
 mov rdi,00h
 movss DWORD PTR[rdx + rdi * 4],xmm0 
 
L__return:
 RestoreAllXMMRegs 
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size 

   
    ret
    
    
    
fname        endp
TEXT    ENDS

data SEGMENT READ
CONST    SEGMENT


ALIGN 16
L__real_qnan        DQ 07FC000007FC00000h   ; qNaN
                    DQ 07FC000007FC00000h
L__real_ninf_32     DQ 0ff800000ff800000h   ; -inf
                    DQ 0ff800000ff800000h
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
L__mask_127_32bit    DQ 00000007f0000007fh
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
L__exponent_compare   DQ 07F8000007F800000h  
                      DQ 07F8000007F800000h
L__sign_bit_32      DQ 08000000080000000h 
                    DQ 08000000080000000h



CONST    ENDS
data ENDS
END

