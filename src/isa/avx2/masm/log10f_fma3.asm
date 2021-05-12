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
; log10f_fma3.S
;
; An implementation of the logf libm function.
;
; Prototype
;
;     float log10f(float x);
;
;
;   Algorithm
;       Similar to one presnted in log.S
;




include fm.inc
ALM_PROTO_FMA3 log10f
fname_special   TEXTEQU <_log10f_special>


; local variable storage offsets
save_xmm6       EQU     00h
stack_size      EQU     028h ; We take 8 as the last nibble to allow for 
                              ; alligned data movement.



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
    vcvtdq2ps    xmm5,xmm3 ; xmm5 = float( (ux>>23) - 127) = xexp

    ;  NaN or inf
    vpand    xmm1,xmm0,xmm6 ; xmm1 = (ux & 07f800000h)
    vcomiss    xmm1,xmm6 ; if((ux & 07f800000h) == 07f800000h) go to L__x_is_inf_or_nan
    je           L__x_is_inf_or_nan

    ; check for negative numbers or zero
    vpxor    xmm1,xmm1,xmm1
    vcomiss    xmm0,xmm1 ; if( x <= 0.0 ) go to L__x_is_zero_or_neg 
    jbe          L__x_is_zero_or_neg

    vpand    xmm2,xmm0,L__real_mant  ; xmm2 = ux & 0007FFFFFh
    vsubss    xmm4,xmm0,DWORD PTR L__real_one   ; xmm4 = x - 1.0

    vcomiss    xmm5,DWORD PTR L__real_neg127 ; if (xexp == 127) go to L__denormal_adjust 
    je           L__denormal_adjust

L__continue_common:

    ; compute the index into the log tables
    vpand    xmm1,xmm0,DWORD PTR L__mask_mant_all7 ;xmm1 = ux & 0007f0000h 
    vpand    xmm3,xmm0,DWORD PTR L__mask_mant8    ;xmm3  = ux & 000008000h
    vpslld    xmm3,xmm3,1                      ;xmm3  = (ux & 000008000h) << 1
    vpaddd    xmm1,xmm3,xmm1                    ;eax = (ux & 0007f0000h) + ((ux & 000008000h) << 1)
    vmovd    eax,xmm1
    ; check e as a special case
;vcomiss      L__real_ef,xmm0        ;if (x == e)  return 1.0
;je           L__logf_e                      

    ; near one codepath
    vandps    xmm4,xmm4,DWORD PTR L__real_notsign   ; xmm4 = fabs (x - 1.0)
    vcomiss    xmm4,DWORD PTR L__real_threshold ; if (xmm4 < real_threshold) gp to near_one
    jb           L__near_one

    ; F,Y
    shr    eax,16                  ; eax = (ux & 0007f0000h) + ((ux & 000008000h) << 1) >> 16;
    vpor    xmm2,xmm2,L__real_half ; xmm2 = Y = ((ux & 0007FFFFFh) | 03f000000h)
    vpor    xmm1,xmm1,L__real_half ; xmm1 = F = ((ux & 0007f0000h) + ((ux & 000008000h) << 1) | 03f000000h)
    lea    r9,L__log_F_inv   ; 

    ; f = F - Y,r = f * inv
    vsubss    xmm1,xmm1,xmm2       ; f = F _ Y
	vmulss    xmm1,xmm1,DWORD PTR [r9 + rax*4]
	
	; r = f * log_F_inv[index]

    ; poly
    vmovapd    xmm2,QWORD PTR L__real_1_over_3
    vfmadd213ss    xmm2,xmm1,DWORD PTR L__real_1_over_2 ; 1/3*r + 1/2
    vmulss    xmm0,xmm1,xmm1                          ; r*r
    vmovss    xmm3,DWORD PTR L__real_log10_2_tail
    lea    r9,L__log_128_tail
    lea    r10,L__log_128_lead

    vfmadd231ss    xmm1,xmm2,xmm0
    vmovss    xmm0,DWORD PTR L__real_log10_2_lead
    ;addss       xmm2,xmm1
    
    vmulss    xmm1,xmm1,DWORD PTR L__real_log10_e
    vfmsub213ss    xmm3,xmm5,xmm1 ; z2 

    ; m*log(10) + log10(G) - poly
    vfmadd213ss    xmm0,xmm5,[r10 + rax*4]
    vaddss    xmm3,xmm3,DWORD PTR[r9 + rax*4]
    vaddss    xmm0,xmm0,xmm3

     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
    


ALIGN 16
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

    vpand    xmm5,xmm0,L__mask_lower
    vsubss    xmm0,xmm0,xmm5
    vaddss    xmm2,xmm2,xmm0

    vmulss    xmm1,xmm2,DWORD PTR L__real_log10_e_lead
    vmulss    xmm0,xmm5,DWORD PTR L__real_log10_e_tail
    vmulss    xmm5,xmm5,DWORD PTR L__real_log10_e_lead
    
    vfmadd231ss    xmm0,xmm2,DWORD PTR L__real_log10_e_tail
    vaddss    xmm0,xmm0,xmm1
    vaddss    xmm0,xmm0,xmm5

     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN 16
L__denormal_adjust:
    vmovss    xmm3,DWORD PTR L__real_one
    vpor    xmm2,xmm2,xmm3  ; xmm2 = temp = ((ux &0007FFFFFh) | 03f800000h)
    vsubss    xmm2,xmm2,xmm3  ; xmm2 = temp -1.0
    vpsrld    xmm5,xmm2,23                 ; xmm5 = (utemp >> 23)
    vpand    xmm2,xmm2,DWORD PTR L__real_mant ; xmm2 = (utemp & 0007FFFFFh)
    vmovaps    xmm0,xmm2                ;
    vpsubd    xmm5,xmm5,DWORD PTR L__mask_253  ; xmm5 = (utemp >> 23) - 253
    vcvtdq2ps    xmm5,xmm5               ; xmm5 = (float) ((utemp >> 23) - 253)
    jmp         L__continue_common        ;  

ALIGN 16
L__x_is_zero_or_neg:
    jne         L__x_is_neg

    vmovss    xmm1,DWORD PTR L__real_ninf
    mov    r8d,DWORD PTR L__flag_x_zero
    call        fname_special

     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
 

ALIGN 16
L__x_is_neg:

    vmovss    xmm1,DWORD PTR L__real_neg_qnan
    mov       r8d,DWORD PTR L__flag_x_neg
    call      fname_special

     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
     

ALIGN 16
L__x_is_inf_or_nan:

    cmp    eax,DWORD PTR L__real_inf
    je          L__finish

    cmp    eax,DWORD PTR L__real_ninf
    je          L__x_is_neg

    or    eax,DWORD PTR L__real_qnanbit
    movd    xmm1,eax
    mov    r8d,DWORD PTR L__flag_x_nan
    call        fname_special

     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
   

ALIGN 16
L__finish:

     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret

fname        endp
TEXT ENDS


data SEGMENT READ
CONST    SEGMENT         
ALIGN 16

; these codes and the ones in the corresponding .c file have to match
L__flag_x_zero        DD 00000001
L__flag_x_neg         DD 00000002
L__flag_x_nan         DD 00000003

ALIGN 16

L__real_one           DQ 003f8000003f800000h   ; 1.0
                        DQ 003f8000003f800000h
L__real_two           DQ 004000000040000000h   ; 1.0
                        DQ 004000000040000000h
L__real_neg_qnan        DQ 0ffC00000ffC00000h   ; neg qNaN
                        DQ 0ffC00000ffC00000h
L__real_ninf          DQ 00ff800000ff800000h   ; -inf
                        DQ 00ff800000ff800000h
L__real_inf           DQ 007f8000007f800000h   ; +inf
                        DQ 007f8000007f800000h
L__real_nan           DQ 007fc000007fc00000h   ; NaN
                        DQ 007fc000007fc00000h
L__real_ef            DQ 00402DF854402DF854h   ; float e
                        DQ 00402DF854402DF854h
L__real_sign          DQ 008000000080000000h   ; sign bit
                        DQ 008000000080000000h
L__real_notsign       DQ 007ffFFFFF7ffFFFFFh   ; ^sign bit
                        DQ 007ffFFFFF7ffFFFFFh
L__real_qnanbit       DQ 000040000000400000h   ; quiet nan bit
                        DQ 000040000000400000h
L__real_mant          DQ 00007FFFFF007FFFFFh   ; mantissa bits
                        DQ 00007FFFFF007FFFFFh
L__mask_127           DQ 000000007f0000007fh   ; 
                        DQ 000000007f0000007fh

L__mask_mant_all7     DQ 000000000007f0000h
                        DQ 000000000007f0000h
L__mask_mant8         DQ 00000000000008000h
                        DQ 00000000000008000h

L__real_ca1           DQ 003DAAAAAB3DAAAAABh   ; 8.33333333333317923934e-02
                        DQ 003DAAAAAB3DAAAAABh
L__real_ca2           DQ 003C4CCCCD3C4CCCCDh   ; 1.25000000037717509602e-02
                        DQ 003C4CCCCD3C4CCCCDh

L__real_log2_lead     DQ 003F3170003F317000h   ; 0.693115234375
                        DQ 003F3170003F317000h
L__real_log2_tail     DQ 003805FDF43805FDF4h   ; 0.000031946183
                        DQ 003805FDF43805FDF4h
L__real_half          DQ 003f0000003f000000h   ; 1/2
                        DQ 003f0000003f000000h
L__real_log10_e_lead  DQ 03EDE00003EDE0000h    ; log10e_lead  0.4335937500
                        DQ 03EDE00003EDE0000h
L__real_log10_e_tail  DQ 03A37B1523A37B152h    ; log10e_tail  0.0007007319
                        DQ 03A37B1523A37B152h

L__real_log10_2_lead  DQ 03e9a00003e9a0000h
                        DQ 00000000000000000h
L__real_log10_2_tail  DQ 039826a1339826a13h
                        DQ 00000000000000000h
L__real_log10_e       DQ 03ede5bd93ede5bd9h
                        DQ 00000000000000000h

L__mask_lower         DQ 00ffff0000ffff0000h
                        DQ 00ffff0000ffff0000h

ALIGN 16
L__real_neg127      DD 0c2fe0000h
                    DD 00h
                    DQ 00h

L__mask_253       DD 0000000fdh
                    DD 0
                    DQ 0

L__real_threshold DD 03d800000h
                    DD 0
                    DQ 0

L__mask_01        DD 000000001h
                    DD 0
                    DQ 0

L__mask_80        DD 000000080h
                    DD 0
                    DQ 0

L__real_3b800000  DD 03b800000h
                    DD 0
                    DQ 0

L__real_1_over_3  DD 03eaaaaabh
                    DD 0
                    DQ 0

L__real_1_over_2  DD 03f000000h
                    DD 0
                    DQ 0

ALIGN 16
L__log_128_lead     DD 000000000h
                    DD 03b5d4000h
                    DD 03bdc8000h
                    DD 03c24c000h
                    DD 03c5ac000h
                    DD 03c884000h
                    DD 03ca2c000h
                    DD 03cbd4000h
                    DD 03cd78000h
                    DD 03cf1c000h
                    DD 03d05c000h
                    DD 03d128000h
                    DD 03d1f4000h
                    DD 03d2c0000h
                    DD 03d388000h
                    DD 03d450000h
                    DD 03d518000h
                    DD 03d5dc000h
                    DD 03d6a0000h
                    DD 03d760000h
                    DD 03d810000h
                    DD 03d870000h
                    DD 03d8d0000h
                    DD 03d92c000h
                    DD 03d98c000h
                    DD 03d9e8000h
                    DD 03da44000h
                    DD 03daa0000h
                    DD 03dafc000h
                    DD 03db58000h
                    DD 03dbb4000h
                    DD 03dc0c000h
                    DD 03dc64000h
                    DD 03dcc0000h
                    DD 03dd18000h
                    DD 03dd6c000h
                    DD 03ddc4000h
                    DD 03de1c000h
                    DD 03de70000h
                    DD 03dec8000h
                    DD 03df1c000h
                    DD 03df70000h
                    DD 03dfc4000h
                    DD 03e00c000h
                    DD 03e034000h
                    DD 03e05c000h
                    DD 03e088000h
                    DD 03e0b0000h
                    DD 03e0d8000h
                    DD 03e100000h
                    DD 03e128000h
                    DD 03e150000h
                    DD 03e178000h
                    DD 03e1a0000h
                    DD 03e1c8000h
                    DD 03e1ec000h
                    DD 03e214000h
                    DD 03e23c000h
                    DD 03e260000h
                    DD 03e288000h
                    DD 03e2ac000h
                    DD 03e2d4000h
                    DD 03e2f8000h
                    DD 03e31c000h
                    DD 03e344000h
                    DD 03e368000h
                    DD 03e38c000h
                    DD 03e3b0000h
                    DD 03e3d4000h
                    DD 03e3fc000h
                    DD 03e420000h
                    DD 03e440000h
                    DD 03e464000h
                    DD 03e488000h
                    DD 03e4ac000h
                    DD 03e4d0000h
                    DD 03e4f4000h
                    DD 03e514000h
                    DD 03e538000h
                    DD 03e55c000h
                    DD 03e57c000h
                    DD 03e5a0000h
                    DD 03e5c0000h
                    DD 03e5e4000h
                    DD 03e604000h
                    DD 03e624000h
                    DD 03e648000h
                    DD 03e668000h
                    DD 03e688000h
                    DD 03e6ac000h
                    DD 03e6cc000h
                    DD 03e6ec000h
                    DD 03e70c000h
                    DD 03e72c000h
                    DD 03e74c000h
                    DD 03e76c000h
                    DD 03e78c000h
                    DD 03e7ac000h
                    DD 03e7cc000h
                    DD 03e7ec000h
                    DD 03e804000h
                    DD 03e814000h
                    DD 03e824000h
                    DD 03e834000h
                    DD 03e840000h
                    DD 03e850000h
                    DD 03e860000h
                    DD 03e870000h
                    DD 03e880000h
                    DD 03e88c000h
                    DD 03e89c000h
                    DD 03e8ac000h
                    DD 03e8bc000h
                    DD 03e8c8000h
                    DD 03e8d8000h
                    DD 03e8e8000h
                    DD 03e8f4000h
                    DD 03e904000h
                    DD 03e914000h
                    DD 03e920000h
                    DD 03e930000h
                    DD 03e93c000h
                    DD 03e94c000h
                    DD 03e958000h
                    DD 03e968000h
                    DD 03e978000h
                    DD 03e984000h
                    DD 03e994000h
                    DD 03e9a0000h

ALIGN 16
L__log_128_tail     DD 000000000h
                    DD 0367a8e44h
                    DD 0368ed49fh
                    DD 036c21451h
                    DD 0375211d6h
                    DD 03720ea11h
                    DD 037e9eb59h
                    DD 037b87be7h
                    DD 037bf2560h
                    DD 033d597a0h
                    DD 037806a05h
                    DD 03820581fh
                    DD 038223334h
                    DD 0378e3bach
                    DD 03810684fh
                    DD 037feb7aeh
                    DD 036a9d609h
                    DD 037a68163h
                    DD 0376a8b27h
                    DD 0384c8fd6h
                    DD 03885183eh
                    DD 03874a760h
                    DD 0380d1154h
                    DD 038ea42bdh
                    DD 0384c1571h
                    DD 038ba66b8h
                    DD 038e7da3bh
                    DD 038eee632h
                    DD 038d00911h
                    DD 0388bbedeh
                    DD 0378a0512h
                    DD 03894c7a0h
                    DD 038e30710h
                    DD 036db2829h
                    DD 03729d609h
                    DD 038fa0e82h
                    DD 038bc9a75h
                    DD 0383a9297h
                    DD 038dc83c8h
                    DD 037eac335h
                    DD 038706ac3h
                    DD 0389574c2h
                    DD 03892d068h
                    DD 038615032h
                    DD 03917acf4h
                    DD 03967a126h
                    DD 038217840h
                    DD 038b420abh
                    DD 038f9c7b2h
                    DD 0391103bdh
                    DD 039169a6bh
                    DD 0390dd194h
                    DD 038eda471h
                    DD 038a38950h
                    DD 037f6844ah
                    DD 0395e1cdbh
                    DD 0390fcffch
                    DD 038503e9dh
                    DD 0394b00fdh
                    DD 038a9910ah
                    DD 039518a31h
                    DD 03882d2c2h
                    DD 0392488e4h
                    DD 0397b0affh
                    DD 0388a22d8h
                    DD 03902bd5eh
                    DD 039342f85h
                    DD 039598811h
                    DD 03972e6b1h
                    DD 034d53654h
                    DD 0360ca25eh
                    DD 039785cc0h
                    DD 039630710h
                    DD 039424ed7h
                    DD 039165101h
                    DD 038be5421h
                    DD 037e7b0c0h
                    DD 0394fd0c3h
                    DD 038efaaaah
                    DD 037a8f566h
                    DD 03927c744h
                    DD 0383fa4d5h
                    DD 0392d9e39h
                    DD 03803feaeh
                    DD 0390a268ch
                    DD 039692b80h
                    DD 038789b4fh
                    DD 03909307dh
                    DD 0394a601ch
                    DD 035e67edch
                    DD 0383e386dh
                    DD 038a7743dh
                    DD 038dccec3h
                    DD 038ff57e0h
                    DD 039079d8bh
                    DD 0390651a6h
                    DD 038f7bad9h
                    DD 038d0ab82h
                    DD 038979e7dh
                    DD 0381978eeh
                    DD 0397816c8h
                    DD 039410cb2h
                    DD 039015384h
                    DD 03863fa28h
                    DD 039f41065h
                    DD 039c7668ah
                    DD 039968afah
                    DD 039430db9h
                    DD 038a18cf3h
                    DD 039eb2907h
                    DD 039a9e10ch
                    DD 039492800h
                    DD 0385a53d1h
                    DD 039ce0cf7h
                    DD 03979c7b2h
                    DD 0389f5d99h
                    DD 039ceefcbh
                    DD 039646a39h
                    DD 0380d7a9bh
                    DD 039ad6650h
                    DD 0390ac3b8h
                    DD 039d9a9a8h
                    DD 039548a99h
                    DD 039f73c4bh
                    DD 03980960eh
                    DD 0374b3d5ah
                    DD 039888f1eh
                    DD 037679a07h
                    DD 039826a13h


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

