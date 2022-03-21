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
; logf_fma3.S
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

ALM_PROTO_FMA3 logf

; local variable storage offsets
save_xmm6       EQU     00h
stack_size      EQU     028h ; We take 8 as the last nibble to allow for 
                              ; alligned data movement.
                              

 
fname_special   TEXTEQU <alm_logf_special>
EXTERN fname_special:PROC

text SEGMENT EXECUTE
ALIGN 16
PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveRegsAVX xmm6,save_xmm6
    
    .ENDPROLOG  


    ; compute exponent part
    vmovss    xmm6,DWORD PTR L__real_inf 
    xor       rax,rax   ;rax=0
    vpsrld    xmm3,xmm0,23   ;xmm3 = (ux>>23)
    vmovd     eax,xmm0  ;eax = x
    vpsubd    xmm3,xmm3,DWORD PTR L__mask_127 ; xmm3 = (ux>>23) - 127
    vcvtdq2ps xmm5,xmm3 ; xmm5 = float( (ux>>23) - 127) = xexp

    ;  NaN or inf
    vpand      xmm1,xmm0,xmm6 ; xmm1 = (ux & 07f800000h)
    vcomiss    xmm1,xmm6 ; if((ux & 07f800000h) == 07f800000h) go to DWORD PTR L__x_is_inf_or_nan
    je         L__x_is_inf_or_nan

    ; check for negative numbers or zero
    vpxor      xmm1,xmm1,xmm1
    vcomiss    xmm0,xmm1 ; if( x <= 0.0 ) go to DWORD PTR L__x_is_zero_or_neg 
    jbe        L__x_is_zero_or_neg

    vpand     xmm2,xmm0,DWORD PTR L__real_mant  ; xmm2 = ux & 0007FFFFFh
    vsubss    xmm4,xmm0,DWORD PTR L__real_one   ; xmm4 = x - 1.0

    vcomiss   xmm5,DWORD PTR L__real_neg127 ; if (xexp == 127) go to DWORD PTR L__denormal_adjust 
    je        L__denormal_adjust

L__continue_common:

    ; compute the index into the log tables
;mov    r9d,eax                      ;r9  = ux 
    vpand     xmm1,xmm0,DWORD PTR L__mask_mant_all7 ;xmm1 = ux & 0007f0000h 
    vpand     xmm3,xmm0,DWORD PTR L__mask_mant8    ;xmm3  = ux & 000008000h
    vpslld    xmm3,xmm3,1                      ;xmm3  = (ux & 000008000h) << 1
    vpaddd    xmm1,xmm3,xmm1                    ;eax = (ux & 0007f0000h) + ((ux & 000008000h) << 1)
    vmovd     eax,xmm1
    ; check e as a special case
    vcomiss   xmm0,DWORD PTR L__real_ef        ;if (x == e)  return 1.0
    je        L__logf_e                      

    ; near one codepath
    vandps     xmm4,xmm4,DWORD PTR L__real_notsign   ; xmm4 = fabs (x - 1.0)
    vcomiss    xmm4,DWORD PTR L__real_threshold ; if (xmm4 < real_threshold) gp to near_one
    jb         L__near_one

    ; F,Y
    shr    eax,16                  ; eax = (ux & 0007f0000h) + ((ux & 000008000h) << 1) >> 16;
    vpor    xmm2,xmm2,DWORD PTR L__real_half ; xmm2 = Y = ((ux & 0007FFFFFh) | 03f000000h)
    vpor    xmm1,xmm1,DWORD PTR L__real_half ; xmm1 = F = ((ux & 0007f0000h) + ((ux & 000008000h) << 1) | 03f000000h)
    lea     r9,QWORD PTR L__log_F_inv   ; 

    ; f = F - Y,r = f * inv
    vsubss    xmm1,xmm1,xmm2       ; f = F _ Y
    vmulss    xmm1,xmm1,DWORD PTR [r9 + rax * 4] ; r = f * log_F_inv[index]

    ; poly
    vmovapd    xmm2,QWORD PTR L__real_1_over_3
    vfmadd213ss   xmm2 ,xmm1,DWORD PTR L__real_1_over_2 ; 1/3*r + 1/2
    vmulss     xmm0,xmm1,xmm1                          ; r*r
    vmovss     xmm3,DWORD PTR L__real_log2_tail;

    lea    r9,DWORD PTR L__log_128_tail
    lea    r10,DWORD PTR L__log_128_lead

    vfmadd231ss    xmm1,xmm2,xmm0 ; poly = r + 1/2*r*r + 1/3*r*r*r 
    vfmsub213ss    xmm3,xmm5,xmm1   ; (xexp * log2_tail) - poly

    ; m*log(2) + log(G) - poly
    vmovss    xmm0,DWORD PTR L__real_log2_lead ;xmm0 = log2_lead
    vfmadd213ss  xmm0,xmm5,[r10 + rax * 4]
    vaddss    xmm3,xmm3,DWORD PTR [r9 + rax * 4]             ; z2 = (xexp * log2_tail) - poly + log_128_tail[index]
    vaddss    xmm0,xmm0,xmm3                   ; return z1 + z2
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN  16
L__logf_e:
    vmovss    xmm0,DWORD PTR L__real_one
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN  16
L__near_one:
    ; r = x - 1.0;
    vmovss    xmm2,DWORD PTR L__real_two  ; xmm2 = 2.0
    vsubss    xmm0,xmm0,DWORD PTR L__real_one  ; xmm0 = r = = x - 1.0

    ; u = r / (2.0 + r)
    vaddss    xmm2,xmm2,xmm0 ; (r+2.0)
    vdivss    xmm1,xmm0,xmm2 ; u = r / (2.0 + r)

    ; correction = r * u
    vmulss    xmm4,xmm0,xmm1 ; correction = u*r

    ; u = u + u;
    vaddss    xmm1,xmm1,xmm1 ; u = u+u 
    vmulss    xmm2,xmm1,xmm1  ; v = u^2

    ; r2 = (u * v * (ca_1 + v * ca_2) - correction)
    vmulss    xmm3,xmm1,xmm2         ; u^3
    vmovss    xmm5,DWORD PTR L__real_ca2
    vfmadd213ss    xmm2,xmm5,DWORD PTR L__real_ca1
    vfmsub213ss    xmm2,xmm3,xmm4 ; r2 = (ca1 + ca2 * v) * u^3 - correction

    ; r + r2
    vaddss    xmm0,xmm0,xmm2 ; return (r + r2)
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN  16
L__denormal_adjust:
    vmovss     xmm3,DWORD PTR L__real_one
    vpor       xmm2,xmm2,xmm3  ; xmm2 = temp = ((ux &0007FFFFFh) | 03f800000h)
    vsubss     xmm2,xmm2,xmm3  ; xmm2 = temp -1.0
    vpsrld     xmm5,xmm2,23                 ; xmm5 = (utemp >> 23)
    vpand      xmm2,xmm2,DWORD PTR L__real_mant ; xmm2 = (utemp & 0007FFFFFh)
    vmovaps    xmm0,xmm2                ; eax = xmm2 
    vpsubd     xmm5,xmm5,DWORD PTR L__mask_253  ; xmm5 = (utemp >> 23) - 253
    vcvtdq2ps  xmm5,xmm5               ; xmm5 = (float) ((utemp >> 23) - 253)
    jmp        L__continue_common        ;  

ALIGN  16
L__x_is_zero_or_neg:
    jne        L__x_is_neg

    vmovss     xmm1,DWORD PTR L__real_ninf
    mov        r8d,DWORD PTR L__flag_x_zero
    call       fname_special
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
 

ALIGN  16
L__x_is_neg:

    vmovss    xmm1,DWORD PTR L__real_neg_qnan
    mov       r8d,DWORD PTR L__flag_x_neg
    call       fname_special
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
     

ALIGN  16
L__x_is_inf_or_nan:

    cmp    eax,DWORD PTR L__real_inf
    je     L__finish

    cmp    eax,DWORD PTR L__real_ninf
    je     L__x_is_neg

    or     eax,DWORD PTR L__real_qnanbit
    vmovd  xmm1,eax
    mov    r8d,DWORD PTR L__flag_x_nan
    call   fname_special
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
   

ALIGN  16
L__finish:
    
    RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret



fname        endp
TEXT    ENDS

data SEGMENT READ
CONST    SEGMENT


ALIGN 16

; these codes and the ones in the corresponding .c file have to match
L__flag_x_zero          DD 00000001
L__flag_x_neg           DD 00000002
L__flag_x_nan           DD 00000003

ALIGN 16

L__real_one             DQ 003f8000003f800000h   ; 1.0
                        DQ 003f8000003f800000h
L__real_two             DQ 004000000040000000h   ; 1.0
                        DQ 004000000040000000h
L__real_neg_qnan        DQ 0ffC00000ffC00000h   ; neg qNaN
                        DQ 0ffC00000ffC00000h
L__real_ninf            DQ 00ff800000ff800000h   ; -inf
                        DQ 00ff800000ff800000h
L__real_inf             DQ 007f8000007f800000h   ; +inf
                        DQ 007f8000007f800000h
L__real_nan             DQ 007fc000007fc00000h   ; NaN
                        DQ 007fc000007fc00000h
L__real_ef              DQ 00402DF854402DF854h   ; float e
                        DQ 00402DF854402DF854h


L__real_sign            DQ 008000000080000000h   ; sign bit
                        DQ 008000000080000000h
L__real_notsign         DQ 007ffFFFFF7ffFFFFFh   ; ^sign bit
                        DQ 007ffFFFFF7ffFFFFFh
L__real_qnanbit         DQ 000040000000400000h   ; quiet nan bit
                        DQ 000040000000400000h
L__real_mant            DQ 00007FFFFF007FFFFFh   ; mantissa bits
                        DQ 00007FFFFF007FFFFFh
L__mask_127             DQ 000000007f0000007fh   ; 
                        DQ 000000007f0000007fh

L__mask_mant_all7       DQ 000000000007f0000h
                        DQ 000000000007f0000h
L__mask_mant8           DQ 00000000000008000h
                        DQ 00000000000008000h

L__real_ca1             DQ 003DAAAAAB3DAAAAABh   ; 8.33333333333317923934e-02
                        DQ 003DAAAAAB3DAAAAABh
L__real_ca2             DQ 003C4CCCCD3C4CCCCDh   ; 1.25000000037717509602e-02
                        DQ 003C4CCCCD3C4CCCCDh

L__real_log2_lead       DQ 003F3170003F317000h   ; 0.693115234375
                        DQ 003F3170003F317000h
L__real_log2_tail       DQ 003805FDF43805FDF4h   ; 0.000031946183
                        DQ 003805FDF43805FDF4h
L__real_half            DQ 003f0000003f000000h   ; 1/2
                        DQ 003f0000003f000000h


ALIGN 16

L__real_neg127      DD 00c2fe0000h
                    DD 0
                    DQ 0

L__mask_253         DD 0000000fdh
                    DD 0
                    DQ 0

L__real_threshold   DD 03d800000h
                    DD 0
                    DQ 0

L__mask_01          DD 000000001h
                    DD 0
                    DQ 0

L__mask_80          DD 000000080h
                    DD 0
                    DQ 0

L__real_3b800000    DD 03b800000h
                    DD 0
                    DQ 0

L__real_1_over_3    DD 03eaaaaabh
                    DD 0
                    DQ 0

L__real_1_over_2    DD 03f000000h
                    DD 0
                    DQ 0

ALIGN 16
L__log_128_lead     DD 000000000h
                    DD 03bff0000h
                    DD 03c7e0000h
                    DD 03cbdc000h
                    DD 03cfc1000h
                    DD 03d1cf000h
                    DD 03d3ba000h
                    DD 03d5a1000h
                    DD 03d785000h
                    DD 03d8b2000h
                    DD 03d9a0000h
                    DD 03da8d000h
                    DD 03db78000h
                    DD 03dc61000h
                    DD 03dd49000h
                    DD 03de2f000h
                    DD 03df13000h
                    DD 03dff6000h
                    DD 03e06b000h
                    DD 03e0db000h
                    DD 03e14a000h
                    DD 03e1b8000h
                    DD 03e226000h
                    DD 03e293000h
                    DD 03e2ff000h
                    DD 03e36b000h
                    DD 03e3d5000h
                    DD 03e43f000h
                    DD 03e4a9000h
                    DD 03e511000h
                    DD 03e579000h
                    DD 03e5e1000h
                    DD 03e647000h
                    DD 03e6ae000h
                    DD 03e713000h
                    DD 03e778000h
                    DD 03e7dc000h
                    DD 03e820000h
                    DD 03e851000h
                    DD 03e882000h
                    DD 03e8b3000h
                    DD 03e8e4000h
                    DD 03e914000h
                    DD 03e944000h
                    DD 03e974000h
                    DD 03e9a3000h
                    DD 03e9d3000h
                    DD 03ea02000h
                    DD 03ea30000h
                    DD 03ea5f000h
                    DD 03ea8d000h
                    DD 03eabb000h
                    DD 03eae8000h
                    DD 03eb16000h
                    DD 03eb43000h
                    DD 03eb70000h
                    DD 03eb9c000h
                    DD 03ebc9000h
                    DD 03ebf5000h
                    DD 03ec21000h
                    DD 03ec4d000h
                    DD 03ec78000h
                    DD 03eca3000h
                    DD 03ecce000h
                    DD 03ecf9000h
                    DD 03ed24000h
                    DD 03ed4e000h
                    DD 03ed78000h
                    DD 03eda2000h
                    DD 03edcc000h
                    DD 03edf5000h
                    DD 03ee1e000h
                    DD 03ee47000h
                    DD 03ee70000h
                    DD 03ee99000h
                    DD 03eec1000h
                    DD 03eeea000h
                    DD 03ef12000h
                    DD 03ef3a000h
                    DD 03ef61000h
                    DD 03ef89000h
                    DD 03efb0000h
                    DD 03efd7000h
                    DD 03effe000h
                    DD 03f012000h
                    DD 03f025000h
                    DD 03f039000h
                    DD 03f04c000h
                    DD 03f05f000h
                    DD 03f072000h
                    DD 03f084000h
                    DD 03f097000h
                    DD 03f0aa000h
                    DD 03f0bc000h
                    DD 03f0cf000h
                    DD 03f0e1000h
                    DD 03f0f4000h
                    DD 03f106000h
                    DD 03f118000h
                    DD 03f12a000h
                    DD 03f13c000h
                    DD 03f14e000h
                    DD 03f160000h
                    DD 03f172000h
                    DD 03f183000h
                    DD 03f195000h
                    DD 03f1a7000h
                    DD 03f1b8000h
                    DD 03f1c9000h
                    DD 03f1db000h
                    DD 03f1ec000h
                    DD 03f1fd000h
                    DD 03f20e000h
                    DD 03f21f000h
                    DD 03f230000h
                    DD 03f241000h
                    DD 03f252000h
                    DD 03f263000h
                    DD 03f273000h
                    DD 03f284000h
                    DD 03f295000h
                    DD 03f2a5000h
                    DD 03f2b5000h
                    DD 03f2c6000h
                    DD 03f2d6000h
                    DD 03f2e6000h
                    DD 03f2f7000h
                    DD 03f307000h
                    DD 03f317000h

ALIGN 16
L__log_128_tail     DD 000000000h
                    DD 03429ac41h
                    DD 035a8b0fch
                    DD 0368d83eah
                    DD 0361b0e78h
                    DD 03687b9feh
                    DD 03631ec65h
                    DD 036dd7119h
                    DD 035c30045h
                    DD 0379b7751h
                    DD 037ebcb0dh
                    DD 037839f83h
                    DD 037528ae5h
                    DD 037a2eb18h
                    DD 036da7495h
                    DD 036a91eb7h
                    DD 03783b715h
                    DD 0371131dbh
                    DD 0383f3e68h
                    DD 038156a97h
                    DD 038297c0fh
                    DD 0387e100fh
                    DD 03815b665h
                    DD 037e5e3a1h
                    DD 038183853h
                    DD 035fe719dh
                    DD 038448108h
                    DD 038503290h
                    DD 0373539e8h
                    DD 0385e0ff1h
                    DD 03864a740h
                    DD 03786742dh
                    DD 0387be3cdh
                    DD 03685ad3eh
                    DD 03803b715h
                    DD 037adcbdch
                    DD 0380c36afh
                    DD 0371652d3h
                    DD 038927139h
                    DD 038c5fcd7h
                    DD 038ae55d5h
                    DD 03818c169h
                    DD 038a0fde7h
                    DD 038ad09efh
                    DD 03862bae1h
                    DD 038eecd4ch
                    DD 03798aad2h
                    DD 037421a1ah
                    DD 038c5e10eh
                    DD 037bf2aeeh
                    DD 0382d872dh
                    DD 037ee2e8ah
                    DD 038dedfach
                    DD 03802f2b9h
                    DD 038481e9bh
                    DD 0380eaa2bh
                    DD 038ebfb5dh
                    DD 038255fddh
                    DD 038783b82h
                    DD 03851da1eh
                    DD 0374e1b05h
                    DD 0388f439bh
                    DD 038ca0e10h
                    DD 038cac08bh
                    DD 03891f65fh
                    DD 0378121cbh
                    DD 0386c9a9ah
                    DD 038949923h
                    DD 038777bcch
                    DD 037b12d26h
                    DD 038a6ced3h
                    DD 038ebd3e6h
                    DD 038fbe3cdh
                    DD 038d785c2h
                    DD 0387e7e00h
                    DD 038f392c5h
                    DD 037d40983h
                    DD 038081a7ch
                    DD 03784c3adh
                    DD 038cce923h
                    DD 0380f5fafh
                    DD 03891fd38h
                    DD 038ac47bch
                    DD 03897042bh
                    DD 0392952d2h
                    DD 0396fced4h
                    DD 037f97073h
                    DD 0385e9eaeh
                    DD 03865c84ah
                    DD 038130ba3h
                    DD 03979cf16h
                    DD 03938cac9h
                    DD 038c3d2f4h
                    DD 039755dech
                    DD 038e6b467h
                    DD 0395c0fb8h
                    DD 0383ebce0h
                    DD 038dcd192h
                    DD 039186bdfh
                    DD 0392de74ch
                    DD 0392f0944h
                    DD 0391bff61h
                    DD 038e9ed44h
                    DD 038686dc8h
                    DD 0396b99a7h
                    DD 039099c89h
                    DD 037a27673h
                    DD 0390bdaa3h
                    DD 0397069abh
                    DD 0388449ffh
                    DD 039013538h
                    DD 0392dc268h
                    DD 03947f423h
                    DD 0394ff17ch
                    DD 03945e10eh
                    DD 03929e8f5h
                    DD 038f85db0h
                    DD 038735f99h
                    DD 0396c08dbh
                    DD 03909e600h
                    DD 037b4996fh
                    DD 0391233cch
                    DD 0397cead9h
                    DD 038adb5cdh
                    DD 03920261ah
                    DD 03958ee36h
                    DD 035aa4905h
                    DD 037cbd11eh
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

