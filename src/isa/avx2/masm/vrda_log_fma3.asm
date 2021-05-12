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
; vrda_log_bdozr.S
;
; A bulldozer implementation of vrd2_log libm function.
;
; Prototype:
;
;     __m128d vrd2_log_bdozr(__m128d x);
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
;define fname ALM_PROTO_FMA3(vrda_log)


include fm.inc
include log_tables.inc


ALM_PROTO_FMA3 vrda_log 

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
L__process_next2:
 sub rdi,02h
 cmp rdi,-1
 jle L__process_first1
 movdqu xmm0,[rsi + rdi * 8]
 jmp L__start
 
ALIGN 16 
L__process_first1:
 jl L__return ;even all,elements are processed rdi == -2
 mov rcx,00h
 movsd xmm0,QWORD PTR[rsi + rcx * 8] 
 
ALIGN 16 
L__start: 
   


    ; compute exponent part
    ;get the mask for negative inputs
    vpand     xmm1,xmm0,QWORD PTR L__sign_bit_64
    vpcmpeqq  xmm1,xmm1,QWORD PTR L__sign_bit_64

    ;get the mask for Nans and Infs
    vpand     xmm14,xmm0,QWORD PTR L__exp_mask  ; remove the mantissa and sign
    vpcmpeqq  xmm14,xmm14,QWORD PTR L__exp_mask
    
    ;get the mask for +/- zero
    vpand     xmm2,xmm0,QWORD PTR L__sign_mask_64
    vpcmpeqq  xmm2,xmm2,QWORD PTR L__Zero_64 ; xmm2 stores the mask for +/- zeros 
;;;;;;;;;;;;;;;;;clear till here;;;;;;;;;;;;;;;;;;;;;
    
    ;calculate log
    vpcmpgtq  xmm4,xmm0,QWORD PTR L__real_log_thresh1 ;TBD probably we can use vpcmpeqq
    vcmplepd  xmm3,xmm0,QWORD PTR L__real_log_thresh2
    vpand     xmm3,xmm4,xmm3 ; xmm3 stores mask for (ux >= log_thresh1 && ux <= log_thresh2)

    ;calculate outside the threshold range first
    
    ;check for IMPBIT_DP_64 and calulate f
    vcmpltpd  xmm5,xmm0,QWORD PTR L__impbit_dp64 ; mask for (ux < IMPBIT_DP64) ;TBD probably we can use vpcmpeqq
    vpor      xmm4,xmm0,QWORD PTR L__constant1 ; PUT_BITS_DP64(ux | 003d000000000000h    f),0
    vsubpd    xmm4,xmm4,QWORD PTR L__constant1 ; f -= corr
    vpand     xmm8,xmm5,QWORD PTR L__expadjust ; xmm8 = expadjust
    ; f = xmm4/xmm7 and the mask is xmm5
    ;vpcmov    xmm5 ,xmm4,xmm0,xmm5
    VANDNPD   xmm13  ,  xmm5,   xmm0 
    VANDPD    xmm5   ,  xmm4,   xmm5 
    VORPD     xmm5   ,  xmm5,   xmm13
    
    ; Now f = ux = xmm5 expadjust = xmm8
    vpand     xmm4,xmm5,QWORD PTR L__exp_mask
    vpsrlq    xmm4,xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    vpsubq    xmm4,xmm4,QWORD PTR L__mask_1023
    vpsubq    xmm4,xmm4,xmm8 ; subtract expadjust

     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    vpand    xmm6,xmm5,QWORD PTR L__index_mask1
    vpand    xmm7,xmm5,QWORD PTR L__index_mask2
    vpor     xmm6,xmm6,QWORD PTR L__index_mask3
    
    vpsrlq     xmm7,xmm7,45 ; ((ux & 00000200000000000h) >> 45)
    vpsrlq     xmm6,xmm6,46 ; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    vpaddq     xmm6,xmm6,xmm7 ; index = xmm6
    vpshufd    xmm7,xmm6,0F8h ; [11 11 10 00] = 0F8h
    vcvtdq2pd  xmm7,xmm7
    
    vmulpd    xmm7,xmm7,QWORD PTR L__index_constant1 ; f1 =index*0.0078125
    vpsubq    xmm6,xmm6,QWORD PTR L__index_constant2 ; index -= 64

    vpand    xmm5,xmm5,QWORD PTR L__mantissa_mask
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) | HALFEXPBITS    f),_DP64;
    vpor    xmm5,xmm5,QWORD PTR L__real_half ; xmm5 = f
    vsubpd    xmm5,xmm5,xmm7 ; xmm5 = f2 = f - f1;
    
    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    vmovd    r10d,xmm6 ; move lower order index
    vpsrldq  xmm6,xmm6,8
    vmovd    r11d,xmm6 ; move higher order index
    lea      r9,QWORD PTR L__ln_lead_64_table
    lea      r8,QWORD PTR L__ln_tail_64_table
    vmovlpd  xmm6,xmm6,QWORD PTR[r9 + r10 * 8]
    vmovhpd  xmm6,xmm6,QWORD PTR[r9 + r11 * 8] ; xmm6 = z1 = ln_lead_table[index]
    vmovlpd  xmm8,xmm8,QWORD PTR[r8 + r10 * 8]
    vmovhpd  xmm8,xmm8,QWORD PTR[r8 + r11 * 8] ; xmm8 = q = ln_tail_table[index]
    ; z1 = xmm6 q = xmm8 f2 = xmm5 f1 = xmm7 

    ;vfmaddpd    xmm7 ,xmm5,QWORD PTR L__real_half,xmm7 ; (f1 + 0.5 * f2);
    vfmadd231pd xmm7,xmm5, QWORD PTR L__real_half
    vdivpd    xmm7,xmm5,xmm7  ; u = f2 / (f1 + 0.5 * f2);
    vmulpd    xmm5,xmm7,xmm7  ; v = u*u

    ;z1 = xmm6 q = xmm8 u = xmm7 v = xmm5
    vmovapd    xmm13,QWORD PTR L__real_cb_3 ; cb_3
    vmovapd    xmm11,xmm5 ;  
    ;vfmaddpd    xmm5 ,xmm13,xmm5 , ; cb_2 + v*cb_3     
    ;vfmaddpd    xmm13 ,xmm11,xmm5, ; (cb_1 + v * (cb_2 + v * cb_3))
    vfmadd213pd  xmm5 ,xmm13, QWORD PTR L__real_cb_2
    vfmadd213pd  xmm5 ,xmm11, QWORD PTR L__real_cb_1
    
    vmulpd    xmm5,xmm11,xmm5 ; poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    ;poly = xmm5 u = xmm7 q = xmm8
    ;vfmaddpd    xmm5 ,xmm5,xmm7,xmm7
    vfmadd132pd   xmm5,xmm7, xmm7
    vaddpd    xmm5,xmm8,xmm5   ; z2 = q + (u + u * poly)

    ;;;;;;;till here common to    log,log10,log2
    vpshufd    xmm4,xmm4,0F8h ; [11 11 10 00] = 0F8h
    vcvtdq2pd    xmm4,xmm4 ; xexp (float) 
    vmovapd    xmm8,xmm4 ; xmm8 = xexp
    ;vfmaddpd    xmm4 ,xmm4,QWORD PTR L__real_log2_lead,xmm6 ; r1 = (xexp * log2_lead + z1)
    ;vfmaddpd    xmm8 ,xmm8,QWORD PTR L__real_log2_tail,xmm5 ; r2 = (xexp * log2_tail + z2)
    vfmadd132pd  xmm4  , xmm6,  QWORD PTR L__real_log2_lead
    vfmadd132pd  xmm8  , xmm5,  QWORD PTR L__real_log2_tail
    
    vaddpd    xmm8,xmm8,xmm4 ; return r1 + r2
    ;;;;END of calculating outside the threshold
    
    ;;;;Now calculate when the value lies within the threshold
    ;xmm0 still contains the input x
    vsubpd    xmm4,xmm0,QWORD PTR L__one_mask_64 ; r = x - 1.0
    vaddpd    xmm5,xmm4,QWORD PTR L__plus_two ; (2.0 + r)
    vdivpd    xmm6,xmm4,xmm5 ; xmm6 = u = r / (2.0 + r); 
    vmulpd    xmm7,xmm4,xmm6 ; correction = r * u ; TBD
    vaddpd    xmm6,xmm6,xmm6 ; u = u + u
    vmulpd    xmm9,xmm6,xmm6 ; v = u * u
    
    ;r1 = xmm4 v = xmm    u,9 = xmm6,correction = xmm7
;TBD I think this needs some reordering to avoid the huge number of stalls.
    vmovapd    xmm10,QWORD PTR L__real_ca_4 ; xmm10 = v
    ;vfmaddpd    xmm10 ,xmm9,xmm10,QWORD PTR L__real_ca_3 ; (ca_3 + (v * ca_4))
    ;vfmaddpd    xmm10 ,xmm10,xmm9,QWORD PTR L__real_ca_2 ; (ca_2 + v * (ca_3 + v * ca_4))
    ;vfmaddpd    xmm10 ,xmm10,xmm9,QWORD PTR L__real_ca_1 ; (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    vfmadd213pd xmm10   ,xmm9,QWORD PTR L__real_ca_3 
    vfmadd213pd xmm10   ,xmm9,QWORD PTR L__real_ca_2 
    vfmadd213pd xmm10   ,xmm9,QWORD PTR L__real_ca_1 
    
    vmulpd    xmm10,xmm10,xmm9 ; v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    vmulpd    xmm10,xmm10,xmm6 ; u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))

    vsubpd    xmm10,xmm10,xmm7 ; r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 +  v * ca_4))) - correction) 
    
    vaddpd    xmm10,xmm10,xmm4
    ;END calculating within the threshold

    ;now restore the proper results
    ;vpcmov    xmm3 ,xmm10,xmm8,xmm3
    VANDNPD  xmm15  ,  xmm3,  xmm8 
    VANDPD    xmm3  ,  xmm10,  xmm3 
    VORPD     xmm3  ,  xmm3,  xmm15
    
    vmovapd    xmm15,QWORD PTR L__real_qnan
    vmovapd    xmm13,QWORD PTR L__real_ninf

    ; now restore the result if input is less than 0.0
    ;vpcmov    xmm1 ,xmm15,xmm3,xmm1
    VANDNPD  xmm8	 ,  xmm1,    xmm3
    VANDPD    xmm1	 ,  xmm15,   xmm1
    VORPD    xmm1 	 ,  xmm1,    xmm8

    ;now restore if there are some inputs with -1.0
    ;vpcmov    xmm8 ,xmm13,xmm1,xmm2
     VANDNPD  xmm3	  ,  xmm2,     xmm1
     VANDPD    xmm8  ,  xmm13,     xmm2
     VORPD    xmm8	  ,  xmm8,     xmm3
    
    ; now restore the Nans and Infs
    vpand    xmm3,xmm0,QWORD PTR L__sign_bit_64 ; for negative infities we need to set the result as Qnan
    vaddpd    xmm0,xmm0,xmm0
    ;so we get the sign bit and move it to the qnan Bit.
    ; then OR it with Qnan/inf result
    vpsrlq    xmm3,xmm3,12
    vpand    xmm3,xmm3,xmm14
    vpand    xmm0,xmm0,xmm14
    vpor    xmm0,xmm0,xmm3

    vpandn    xmm14,xmm14,xmm8
    vpor    xmm0,xmm0,xmm14

 cmp rdi,-1
 je L__store1
 movdqu [rdx + rdi * 8],xmm0
 jmp L__process_next2 
 
ALIGN 16 
L__store1:
 inc rdi
 movsd QWORD PTR[rdx + rdi * 8],xmm0
 
L__return:
    
    RestoreAllXMMRegs
    RestoreReg rsi,save_rsi
    RestoreReg rdi,save_rdi
    StackDeallocate stack_size
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ret
    
fname        endp
TEXT    ENDS

data SEGMENT READ
CONST    SEGMENT


ALIGN 16
L__sign_bit_64        DQ 08000000000000000h 
                      DQ 08000000000000000h
                      DQ 08000000000000000h
                      DQ 08000000000000000h
L__sign_bit_32        DQ 08000000080000000h 
                      DQ 08000000080000000h
L__exp_mask           DQ 07ff0000000000000h   ;
                      DQ 07ff0000000000000h
L__exponent_compare   DQ 07FF000007FF00000h  
                      DQ 07FF000007FF00000h
L__sign_mask_64       DQ 07FFFFFFFFFFFFFFFh
                      DQ 07FFFFFFFFFFFFFFFh
L__Zero_64            DQ 00000000000000000h
                      DQ 00000000000000000h

; The values exp(-1/16)-1 and exp(1/16)-1 */
L__real_log_thresh1   DQ 03fee0faa00000000h ; 0.9394121170043945,
                      DQ 03fee0faa00000000h
L__real_log_thresh2   DQ 03ff1082c00000000h;  1.0644950866699219;  
                      DQ 03ff1082c00000000h
L__impbit_dp64        DQ 00010000000000000h
                      DQ 00010000000000000h
L__constant1          DQ 003d0000000000000h
                      DQ 003d0000000000000h
L__expadjust          DQ 0000000000000003Ch
                      DQ 0000000000000003Ch
L__mask_1023          DQ 000000000000003ffh
                      DQ 000000000000003ffh
L__mantissa_mask      DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                      DQ 0000FFFFFFFFFFFFFh
L__real_half          DQ 03fe0000000000000h ; 1/2
                      DQ 03fe0000000000000h
L__index_mask1        DQ 0000fc00000000000h
                      DQ 0000fc00000000000h
L__index_mask2        DQ 00000200000000000h
                      DQ 00000200000000000h
L__index_mask3        DQ 00010000000000000h
                      DQ 00010000000000000h
L__index_constant1    DQ 03F80000000000000h
                      DQ 03F80000000000000h
L__index_constant2    DQ 00000000000000040h
                      DQ 00000000000000040h

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
L__one_mask_64       DQ 03ff0000000000000h ; 1
                     DQ 03ff0000000000000h
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

L__mask32bits_DP64   DQ 0ffffffff00000000h
                     DQ 0ffffffff00000000h
L__real_qnan         DQ 07ff8000000000000h   ; qNaN
                     DQ 07ff8000000000000h
L__real_ninf         DQ 0fff0000000000000h   ; -inf
                     DQ 0fff0000000000000h


CONST    ENDS
data ENDS
END

