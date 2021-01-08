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

; vrsa_logf_bdozr.S
;
; An implementation of the logf libm function.
;
; Prototype:
;
;     float logf(float x);
;

;
;   Algorithm:
;       Similar to one presnted in log.S
;

include fm.inc

FN_PROTOTYPE_FMA3 vrsa_logf

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

  

    vmovaps    xmm15,DWORD PTR L__real_one
    vmovaps    xmm13,DWORD PTR L__real_inf
    vmovaps    xmm10,DWORD PTR L__real_nan
    ; compute exponent part
    xor    eax,eax   ;eax=0
    vpsrld    xmm3,xmm0,23   ;xmm3 = (ux>>23)
    vpsubd    xmm3,xmm3,DWORD PTR L__mask_127 ; xmm3 = (ux>>23) - 127
    vandps    xmm12,xmm13,xmm0
    ;speacial case processing Nans Infs    and,Negatives e's 
    ; NaN or inf
    vpcmpeqd    xmm6,xmm13,xmm12 ; if((ux & 07f800000h) == 07f800000h) go to DWORD PTR L__x_is_inf_or_nan
;xmm6 stores the mask for Nans and Infs
    vaddps    xmm11,xmm0,xmm0
    vpand    xmm11,xmm6,xmm11 ;xmm11 stores the result for Nan's and INfs
;check for negative numbers
    vxorps    xmm1,xmm1,xmm1
    vpcmpgtd    xmm7,xmm1,xmm0 ; if( x <= 0.0 )
;xmm7 stores the negative mask for negative numbers
    vpor    xmm6,xmm7,xmm6
    ;vpcmov    xmm11 ,xmm10,xmm11,xmm7
    VANDNPD xmm8 , xmm7, xmm11
    VANDPD xmm11 , xmm10, xmm7 
    VORPD xmm11 , xmm11, xmm8 
    
    vpcmpeqd    xmm8,xmm0,DWORD PTR L__real_ef    ;if (x == e)  return 1.0
    vpor    xmm6,xmm8,xmm6
;xmm8 stores the mask for e
    ;vpcmov    xmm7 ,xmm15,xmm11,xmm8
    VANDNPD xmm2  , xmm8, xmm11
    VANDPD xmm7 , xmm15,  xmm8
    VORPD xmm7  , xmm7, xmm2 
    
    ; xmm6 = Mask for Nan Infs e's and negatives
    ; xmm7 = result of Nan Infs e's and negatives

    vpand    xmm2,xmm0,DWORD PTR L__real_mant  ; xmm2 = ux & 0007FFFFFh
    vsubps    xmm4,xmm0,xmm15   ; xmm4 = x - 1.0
    vpcmpeqd    xmm8,xmm3,DWORD PTR L__mask_neg_127 ; if (xexp == 127) 
;xmm8 stores the mask for denormal numbers 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; commenting this part of the code will not process the denormal inputs                
;if 1
    vpor    xmm11,xmm15,xmm2
    vsubps    xmm10,xmm11,xmm15
    vpand    xmm11,xmm10,DWORD PTR L__real_mant
    ;vpcmov    xmm2 ,xmm11,xmm2,xmm8
    VANDNPD   xmm5  ,  xmm8,    xmm2
    VANDPD     xmm2  ,  xmm11,   xmm8
    VORPD     xmm2  ,  xmm2,    xmm5
    
    
    vpsrld    xmm10,xmm10,23
    vpsubd    xmm10,xmm10,DWORD PTR L__mask_253 
    ;vpcmov    xmm3 ,xmm10,xmm3,xmm8
     VANDNPD xmm5,  xmm8,   xmm3 
     VANDPD   xmm3,  xmm10, xmm8 
     VORPD   xmm3,  xmm3,   xmm5 
    
    
;endif    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    vcvtdq2ps    xmm5,xmm3 ; xmm5 = xexp
    ; compute the index into the log tables
    vpand    xmm11,xmm2,DWORD PTR L__mask_mant_all7 ;xmm11 = ux & 0007f0000h 
    vpand    xmm8,xmm2,DWORD PTR L__mask_mant8 ; xmm8 = ux & 0007f0000h 
    vpslld    xmm8,xmm8,1                   ;xmm8  = (ux & 000008000h) << 1
    vpaddd    xmm1,xmm11,xmm8   ;xmm1 = (ux & 0007f0000h) + ((ux & 000008000h) << 1)

    ; near one codepath
    vpand    xmm4,xmm4,DWORD PTR L__real_notsign   ; xmm4 = fabs (x - 1.0)
    vcmpltps    xmm4,xmm4,DWORD PTR L__real_threshold ; if (xmm4 < real_threshold) go to near_one
;xmm4 stores the mask for nearone values
    ; F,Y

    vpsrld    xmm9,xmm1,16
    vpor    xmm2,xmm2,DWORD PTR L__real_half ; xmm2 = Y = ((ux & 0007FFFFFh) | 03f000000h)
    vpor    xmm1,xmm1,DWORD PTR L__real_half ; xmm1 = F = ((ux & 0007f0000h) + ((ux & 000008000h) << 1) | 03f000000h)
    lea    r9,DWORD PTR L__log_F_inv   ;
    lea    r10,DWORD PTR L__log_128_lead_tail
    xor    r8,r8 ; can be removed

;1st 
    vmovd    r8d,xmm9
    vpsrldq    xmm9,xmm9,4 
    vmovss    xmm10,DWORD PTR[r9 + r8 * 4]
    vmovsd    xmm11,QWORD PTR[r10 + r8 * 8]
    vpslldq    xmm10,xmm10,4
;2nd    
    vmovd     r8d,xmm9
    vpsrldq   xmm9,xmm9,4 
    vmovss    xmm13,DWORD PTR[r9 + r8 * 4]
    vmovss    xmm10,xmm10,xmm13
    vmovsd    xmm14,QWORD PTR[r10 + r8 * 8]
    vpslldq   xmm14,xmm14,8
    vmovsd    xmm11,xmm14,xmm11
    vpslldq   xmm10,xmm10,4
;3rd
    vmovd    r8d,xmm9
    vpsrldq   xmm9,xmm9,4
    vmovss    xmm13,DWORD PTR[r9 + r8 * 4]
    vmovss    xmm10,xmm10,xmm13
    vmovsd    xmm12,QWORD PTR[r10 + r8 * 8]
    vpslldq   xmm10,xmm10,4
;4th
    vmovd     r8d,xmm9
    vmovss    xmm13,DWORD PTR[r9 + r8 * 4]
    vmovss    xmm10,xmm10,xmm13
    vmovsd    xmm14,QWORD PTR[r10 + r8 * 8]
    vpslldq   xmm14,xmm14,8
    vmovsd    xmm12,xmm14,xmm12
    
    vpermilps    xmm10,xmm10,01Bh ; 1B = 00011011
    ; f = F - Y,r = f * inv
    vsubps    xmm1,xmm1,xmm2     ; f = F - Y
    vmulps    xmm1,xmm1,xmm10     ; r = f * log_F_inv[index]
    
    ;poly
    vmovaps   xmm3,DWORD PTR L__real_1_over_3
    ;vfmaddps  xmm2 ,xmm1,xmm3,DWORD PTR L__real_1_over_2 ; 1/3*r + 1/2
    vfmadd213ps xmm3, xmm1,DWORD PTR L__real_1_over_2
    vmovdqa xmm2, xmm3
    
    vmulps    xmm14,xmm1,xmm1                       ; r*r
    vmovaps   xmm3,DWORD PTR L__real_log2_tail  ;xmm3  = log2_tail
    vmovaps   xmm10,DWORD PTR L__real_log2_lead ;xmm10 = log2_lead
    ;vfmaddps  xmm1 ,xmm14,xmm2,xmm1  ;poly = r + 1/2*r*r + 1/3*r*r*r
    ;vfmsubps  xmm3 ,xmm5,xmm3,xmm1 ; z2 = (xexp * log2_tail) - poly
    vfmadd231ps      xmm1  , xmm14,    xmm2 
    vfmsub213ps      xmm3   , xmm5,     xmm1 
    
    ; m*log(2) + log(G) - poly
    vmulps    xmm10,xmm10,xmm5         ; z1 = xexp * log2_lead
;   z1=  4321
;   z2=  4321
; z    z1,2 = 44332211 
; t,h   = thththth 
; add the two 
; z    z1,2 
;   xmm3 = z    xmm10,2 = z1,xmm11=lead,tail xmm12=lead/tail
    vpxor    xmm13,xmm13,xmm13
    vunpcklps    xmm9,xmm10,xmm3  ; xmm9 = 2211 in    z1,z2 order
    vunpckhps    xmm10,xmm10,xmm3   ; xmm10  = 4433 in    z1,z2 order
    vinsertf128    ymm9 ,ymm9,xmm10,1   ; ymm9 = z    z1,2 - 44332211 in z2,z1 order
    vinsertf128    ymm10 ,ymm11,xmm12,1 ; thththth in    z1,z2 order
    vaddps    ymm11,ymm10,ymm9      ; z    z1,2 = (xexp * log2_tail) - poly + log_128_tail[index],(xexp * log2_lead) + log_128_lead[index]
    vhaddps    ymm13,ymm11,ymm13
    vextractf128    xmm14,ymm13,1  ;
    vmovlhps    xmm9,xmm13,xmm14      ;xmm9 stores the result

    ; r = x - 1.0;
    vmovaps    xmm2,DWORD PTR L__real_two        ; xmm2 = 2.0
    vpand    xmm11,xmm0,DWORD PTR L__real_notsign
    ;get the zero input masks
    vpxor    xmm12,xmm12,xmm12
    vsubps    xmm0,xmm0,xmm15  ; xmm0 = r = = x - 1.0
    vpcmpeqd    xmm12,xmm11,xmm12  ; xmm12 stores the mask for zero
    
    ; u = r / (2.0 + r)
    vaddps    xmm2,xmm2,xmm0 ; (r+2.0)
    vdivps    xmm1,xmm0,xmm2 ; u = r / (2.0 + r)

    ; correction = r * u
    vmulps    xmm10,xmm0,xmm1 ; correction = u*r

    ; u = u + u;
    vaddps    xmm1,xmm1,xmm1 ; u = u+u 
    vmulps    xmm2,xmm1,xmm1 ; v = u^2

    ; r2 = (u * v * (ca_1 + v * ca_2) - correction)
    vmulps    xmm3,xmm1,xmm2         ; u^3
    vmovaps    xmm5,DWORD PTR L__real_ca2    ; xmm5 = ca_2 
    ;vfmaddps    xmm2 ,xmm5,xmm2, ; ca1 + ca2 * v
    ;vfmsubps    xmm2 ,xmm2,xmm3,xmm10 ; r2 = (ca1 + ca2 * v) * u^3 - correction 
    
     vfmadd213ps    xmm2, xmm5,DWORD PTR L__real_ca1
     vfmsub213ps     xmm2 , xmm3,  xmm10           
    

    ; r + r2
    vaddps    xmm0,xmm0,xmm2 ; return (r + r2)
; till here I have 
; xmm0 = DWORD PTR L__near_one results 
; xmm9 = away from 1.0 results
; Masks 
;xmm4 stores the mask for nearone values
;xmm12 stores the mask for 0.0
;xmm6 = Mask for Nan Infs e's and negatives
;xmm7 = result of Nan Infs e's and negatives

    vmovaps    xmm5,DWORD PTR L__real_ninf
;vmovaps    xmm14,DWORD PTR L__real_inf
    ;vpcmov    xmm0 ,xmm0,xmm9,xmm4 ; xmm0 stores nearone and away from one results
    ;vpcmov    xmm0 ,xmm7,xmm0,xmm6
    ;vpcmov    xmm0 ,xmm5,xmm0,xmm12
    
    VANDNPD xmm14 , xmm4, xmm9 
    VANDPD xmm0 , xmm0, xmm4 
    VORPD xmm0 , xmm0, xmm14
    
    VANDNPD xmm14 , xmm6, xmm0 
    VANDPD xmm0 , xmm7, xmm6 
    VORPD xmm0 , xmm0, xmm14
    
    VANDNPD xmm14 , xmm12, xmm0 
    VANDPD xmm0 , xmm5, xmm12
    VORPD xmm0 , xmm0, xmm14
    
    
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
L__real_one             DQ 03f8000003f800000h   ; 1.0
                        DQ 03f8000003f800000h
L__real_two             DQ 04000000040000000h   ; 1.0
                        DQ 04000000040000000h
L__real_ninf            DQ 0ff800000ff800000h   ; -inf
                        DQ 0ff800000ff800000h
L__real_inf             DQ 07f8000007f800000h   ; +inf
                        DQ 07f8000007f800000h
L__real_nan             DQ 07fc000007fc00000h   ; NaN
                        DQ 07fc000007fc00000h
L__real_ef              DQ 0402DF854402DF854h   ; float e
                        DQ 0402DF854402DF854h
L__real_neg_qnan        DQ 0ffc00000ffc00000h
                        DQ 0ffc00000ffc00000h

L__real_sign            DQ 08000000080000000h  ; sign bit
                        DQ 08000000080000000h
L__real_notsign         DQ 07ffFFFFF7ffFFFFFh  ; ^sign bit
                        DQ 07ffFFFFF7ffFFFFFh
L__real_qnanbit         DQ 00040000000400000h   ; quiet nan bit
                        DQ 00040000000400000h
L__real_mant            DQ 0007FFFFF007FFFFFh   ; mantissa bits
                        DQ 0007FFFFF007FFFFFh
L__mask_127             DQ 00000007f0000007fh   ; 
                        DQ 00000007f0000007fh
L__mask_neg_127         DQ 0FFFFFF81FFFFFF81h   ; 
                        DQ 0FFFFFF81FFFFFF81h

L__mask_mant_all7       DQ 0007f0000007f0000h
                        DQ 0007f0000007f0000h
L__mask_mant8           DQ 00000800000008000h
                        DQ 00000800000008000h

L__real_ca1             DQ 03DAAAAAB3DAAAAABh   ; 8.33333333333317923934e-02
                        DQ 03DAAAAAB3DAAAAABh
L__real_ca2             DQ 03C4CCCCD3C4CCCCDh   ; 1.25000000037717509602e-02
                        DQ 03C4CCCCD3C4CCCCDh

L__real_log2_lead       DQ 03F3170003F317000h   ; 0.693115234375
                        DQ 03F3170003F317000h
L__real_log2_tail       DQ 03805FDF43805FDF4h   ; 0.000031946183
                        DQ 03805FDF43805FDF4h
L__real_half            DQ 03f0000003f000000h   ; 1/2
                        DQ 03f0000003f000000h


ALIGN 16
L__mask_253         DQ 0000000fd000000fdh
                    DQ 0000000fd000000fdh

L__real_threshold   DQ 03d8000003d800000h
                    DQ 03d8000003d800000h

L__real_1_over_3    DQ 03eaaaaab3eaaaaabh
                    DQ 03eaaaaab3eaaaaabh

L__real_1_over_2    DQ 03f0000003f000000h
                    DQ 03f0000003f000000h



ALIGN 16
L__log_128_lead_tail  DD 000000000h      ;lead      
                    DD 000000000h      ;tail
                    DD 03bff0000h      ;lead
                    DD 03429ac41h      ;tail
                    DD 03c7e0000h      ;.
                    DD 035a8b0fch      ;and so on
                    DD 03cbdc000h
                    DD 0368d83eah
                    DD 03cfc1000h
                    DD 0361b0e78h
                    DD 03d1cf000h
                    DD 03687b9feh
                    DD 03d3ba000h
                    DD 03631ec65h
                    DD 03d5a1000h
                    DD 036dd7119h
                    DD 03d785000h
                    DD 035c30045h
                    DD 03d8b2000h
                    DD 0379b7751h
                    DD 03d9a0000h
                    DD 037ebcb0dh
                    DD 03da8d000h
                    DD 037839f83h
                    DD 03db78000h
                    DD 037528ae5h
                    DD 03dc61000h
                    DD 037a2eb18h
                    DD 03dd49000h
                    DD 036da7495h
                    DD 03de2f000h
                    DD 036a91eb7h
                    DD 03df13000h
                    DD 03783b715h
                    DD 03dff6000h
                    DD 0371131dbh
                    DD 03e06b000h
                    DD 0383f3e68h
                    DD 03e0db000h
                    DD 038156a97h
                    DD 03e14a000h
                    DD 038297c0fh
                    DD 03e1b8000h
                    DD 0387e100fh
                    DD 03e226000h
                    DD 03815b665h
                    DD 03e293000h
                    DD 037e5e3a1h
                    DD 03e2ff000h
                    DD 038183853h
                    DD 03e36b000h
                    DD 035fe719dh
                    DD 03e3d5000h
                    DD 038448108h
                    DD 03e43f000h
                    DD 038503290h
                    DD 03e4a9000h
                    DD 0373539e8h
                    DD 03e511000h
                    DD 0385e0ff1h
                    DD 03e579000h
                    DD 03864a740h
                    DD 03e5e1000h
                    DD 03786742dh
                    DD 03e647000h
                    DD 0387be3cdh
                    DD 03e6ae000h
                    DD 03685ad3eh
                    DD 03e713000h
                    DD 03803b715h
                    DD 03e778000h
                    DD 037adcbdch
                    DD 03e7dc000h
                    DD 0380c36afh
                    DD 03e820000h
                    DD 0371652d3h
                    DD 03e851000h
                    DD 038927139h
                    DD 03e882000h
                    DD 038c5fcd7h
                    DD 03e8b3000h
                    DD 038ae55d5h
                    DD 03e8e4000h
                    DD 03818c169h
                    DD 03e914000h
                    DD 038a0fde7h
                    DD 03e944000h
                    DD 038ad09efh
                    DD 03e974000h
                    DD 03862bae1h
                    DD 03e9a3000h
                    DD 038eecd4ch
                    DD 03e9d3000h
                    DD 03798aad2h
                    DD 03ea02000h
                    DD 037421a1ah
                    DD 03ea30000h
                    DD 038c5e10eh
                    DD 03ea5f000h
                    DD 037bf2aeeh
                    DD 03ea8d000h
                    DD 0382d872dh
                    DD 03eabb000h
                    DD 037ee2e8ah
                    DD 03eae8000h
                    DD 038dedfach
                    DD 03eb16000h
                    DD 03802f2b9h
                    DD 03eb43000h
                    DD 038481e9bh
                    DD 03eb70000h
                    DD 0380eaa2bh
                    DD 03eb9c000h
                    DD 038ebfb5dh
                    DD 03ebc9000h
                    DD 038255fddh
                    DD 03ebf5000h
                    DD 038783b82h
                    DD 03ec21000h
                    DD 03851da1eh
                    DD 03ec4d000h
                    DD 0374e1b05h
                    DD 03ec78000h
                    DD 0388f439bh
                    DD 03eca3000h
                    DD 038ca0e10h
                    DD 03ecce000h
                    DD 038cac08bh
                    DD 03ecf9000h
                    DD 03891f65fh
                    DD 03ed24000h
                    DD 0378121cbh
                    DD 03ed4e000h
                    DD 0386c9a9ah
                    DD 03ed78000h
                    DD 038949923h
                    DD 03eda2000h
                    DD 038777bcch
                    DD 03edcc000h
                    DD 037b12d26h
                    DD 03edf5000h
                    DD 038a6ced3h
                    DD 03ee1e000h
                    DD 038ebd3e6h
                    DD 03ee47000h
                    DD 038fbe3cdh
                    DD 03ee70000h
                    DD 038d785c2h
                    DD 03ee99000h
                    DD 0387e7e00h
                    DD 03eec1000h
                    DD 038f392c5h
                    DD 03eeea000h
                    DD 037d40983h
                    DD 03ef12000h
                    DD 038081a7ch
                    DD 03ef3a000h
                    DD 03784c3adh
                    DD 03ef61000h
                    DD 038cce923h
                    DD 03ef89000h
                    DD 0380f5fafh
                    DD 03efb0000h
                    DD 03891fd38h
                    DD 03efd7000h
                    DD 038ac47bch
                    DD 03effe000h
                    DD 03897042bh
                    DD 03f012000h
                    DD 0392952d2h
                    DD 03f025000h
                    DD 0396fced4h
                    DD 03f039000h
                    DD 037f97073h
                    DD 03f04c000h
                    DD 0385e9eaeh
                    DD 03f05f000h
                    DD 03865c84ah
                    DD 03f072000h
                    DD 038130ba3h
                    DD 03f084000h
                    DD 03979cf16h
                    DD 03f097000h
                    DD 03938cac9h
                    DD 03f0aa000h
                    DD 038c3d2f4h
                    DD 03f0bc000h
                    DD 039755dech
                    DD 03f0cf000h
                    DD 038e6b467h
                    DD 03f0e1000h
                    DD 0395c0fb8h
                    DD 03f0f4000h
                    DD 0383ebce0h
                    DD 03f106000h
                    DD 038dcd192h
                    DD 03f118000h
                    DD 039186bdfh
                    DD 03f12a000h
                    DD 0392de74ch
                    DD 03f13c000h
                    DD 0392f0944h
                    DD 03f14e000h
                    DD 0391bff61h
                    DD 03f160000h
                    DD 038e9ed44h
                    DD 03f172000h
                    DD 038686dc8h
                    DD 03f183000h
                    DD 0396b99a7h
                    DD 03f195000h
                    DD 039099c89h
                    DD 03f1a7000h
                    DD 037a27673h
                    DD 03f1b8000h
                    DD 0390bdaa3h
                    DD 03f1c9000h
                    DD 0397069abh
                    DD 03f1db000h
                    DD 0388449ffh
                    DD 03f1ec000h
                    DD 039013538h
                    DD 03f1fd000h
                    DD 0392dc268h
                    DD 03f20e000h
                    DD 03947f423h
                    DD 03f21f000h
                    DD 0394ff17ch
                    DD 03f230000h
                    DD 03945e10eh
                    DD 03f241000h
                    DD 03929e8f5h
                    DD 03f252000h
                    DD 038f85db0h
                    DD 03f263000h
                    DD 038735f99h
                    DD 03f273000h
                    DD 0396c08dbh
                    DD 03f284000h
                    DD 03909e600h
                    DD 03f295000h
                    DD 037b4996fh
                    DD 03f2a5000h
                    DD 0391233cch
                    DD 03f2b5000h
                    DD 0397cead9h
                    DD 03f2c6000h
                    DD 038adb5cdh
                    DD 03f2d6000h
                    DD 03920261ah
                    DD 03f2e6000h
                    DD 03958ee36h
                    DD 03f2f7000h
                    DD 035aa4905h
                    DD 03f307000h
                    DD 037cbd11eh
                    DD 03f317000h
                    DD 03805fdf4h

                    
ALIGN 16
L__log_F_inv        DD 040000000h
                    DD 03ffe03f8h
                    DD 03ffc0fc1h
                    DD 03ffa232dh
                    DD 03ff83e10h
                    DD 03ff6603eh
                    DD 03ff4898dh
                    DD 03ff2b9d6h
                    DD 03ff0f0f1h
                    DD 03fef2eb7h
                    DD 03fed7304h
                    DD 03febbdb3h
                    DD 03fea0ea1h
                    DD 03fe865ach
                    DD 03fe6c2b4h
                    DD 03fe52598h
                    DD 03fe38e39h
                    DD 03fe1fc78h
                    DD 03fe07038h
                    DD 03fdee95ch
                    DD 03fdd67c9h
                    DD 03fdbeb62h
                    DD 03fda740eh
                    DD 03fd901b2h
                    DD 03fd79436h
                    DD 03fd62b81h
                    DD 03fd4c77bh
                    DD 03fd3680dh
                    DD 03fd20d21h
                    DD 03fd0b6a0h
                    DD 03fcf6475h
                    DD 03fce168ah
                    DD 03fcccccdh
                    DD 03fcb8728h
                    DD 03fca4588h
                    DD 03fc907dah
                    DD 03fc7ce0ch
                    DD 03fc6980ch
                    DD 03fc565c8h
                    DD 03fc43730h
                    DD 03fc30c31h
                    DD 03fc1e4bch
                    DD 03fc0c0c1h
                    DD 03fbfa030h
                    DD 03fbe82fah
                    DD 03fbd6910h
                    DD 03fbc5264h
                    DD 03fbb3ee7h
                    DD 03fba2e8ch
                    DD 03fb92144h
                    DD 03fb81703h
                    DD 03fb70fbbh
                    DD 03fb60b61h
                    DD 03fb509e7h
                    DD 03fb40b41h
                    DD 03fb30f63h
                    DD 03fb21643h
                    DD 03fb11fd4h
                    DD 03fb02c0bh
                    DD 03faf3adeh
                    DD 03fae4c41h
                    DD 03fad602bh
                    DD 03fac7692h
                    DD 03fab8f6ah
                    DD 03faaaaabh
                    DD 03fa9c84ah
                    DD 03fa8e83fh
                    DD 03fa80a81h
                    DD 03fa72f05h
                    DD 03fa655c4h
                    DD 03fa57eb5h
                    DD 03fa4a9cfh
                    DD 03fa3d70ah
                    DD 03fa3065eh
                    DD 03fa237c3h
                    DD 03fa16b31h
                    DD 03fa0a0a1h
                    DD 03f9fd80ah
                    DD 03f9f1166h
                    DD 03f9e4cadh
                    DD 03f9d89d9h
                    DD 03f9cc8e1h
                    DD 03f9c09c1h
                    DD 03f9b4c70h
                    DD 03f9a90e8h
                    DD 03f99d723h
                    DD 03f991f1ah
                    DD 03f9868c8h
                    DD 03f97b426h
                    DD 03f97012eh
                    DD 03f964fdah
                    DD 03f95a025h
                    DD 03f94f209h
                    DD 03f944581h
                    DD 03f939a86h
                    DD 03f92f114h
                    DD 03f924925h
                    DD 03f91a2b4h
                    DD 03f90fdbch
                    DD 03f905a38h
                    DD 03f8fb824h
                    DD 03f8f177ah
                    DD 03f8e7835h
                    DD 03f8dda52h
                    DD 03f8d3dcbh
                    DD 03f8ca29ch
                    DD 03f8c08c1h
                    DD 03f8b7034h
                    DD 03f8ad8f3h
                    DD 03f8a42f8h
                    DD 03f89ae41h
                    DD 03f891ac7h
                    DD 03f888889h
                    DD 03f87f781h
                    DD 03f8767abh
                    DD 03f86d905h
                    DD 03f864b8ah
                    DD 03f85bf37h
                    DD 03f853408h
                    DD 03f84a9fah
                    DD 03f842108h
                    DD 03f839930h
                    DD 03f83126fh
                    DD 03f828cc0h
                    DD 03f820821h
                    DD 03f81848eh
                    DD 03f810204h
                    DD 03f808081h
                    DD 03f800000h


CONST    ENDS
data ENDS
END

