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
; logf.asm
;
; An implementation of the logf libm function.
;
; Prototype:
;
;     float logf(float x);
;

;
;   Algorithm:
;       Similar to one presnted in log.asm
;

include fm.inc

ALM_PROTO_BAS64 logf

fname_special   TEXTEQU <alm_logf_special>

; define local variable storage offsets

p_temp          EQU     30h

;arg_x           EQU     0h
;arg_y           EQU     10h
;arg_code        EQU     20h

stack_size      EQU     78h

; external function
EXTERN fname_special:PROC

; macros

StackAllocate   MACRO size
                    sub         rsp, size
                    .ALLOCSTACK size
                ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG

    ; compute exponent part
    xor         eax, eax
    movdqa      xmm3, xmm0
    movss       xmm4, xmm0
    psrld       xmm3, 23
    movd        eax, xmm0
    psubd       xmm3, XMMWORD PTR __mask_127
    movdqa      xmm2, xmm0
    cvtdq2ps    xmm5, xmm3 ; xexp

    ;  NaN or inf
    movdqa      xmm1, xmm0
    andps       xmm1, XMMWORD PTR __real_inf
    comiss      xmm1, DWORD PTR __real_inf
    je          __x_is_inf_or_nan

    pand        xmm2, XMMWORD PTR __real_mant
    subss       xmm4, DWORD PTR __real_one

    comiss      xmm5, DWORD PTR __real_neg127
    je          __denormal_adjust

__continue_common:

    ; compute the index into the log tables
    mov         r9d, eax
    and         eax, DWORD PTR __mask_mant_all7
    and         r9d, DWORD PTR __mask_mant8
    shl         r9d, 1
    add         eax, r9d
    mov         DWORD PTR [p_temp+rsp], eax

    ; check e as a special case
    comiss      xmm0, DWORD PTR __real_ef
    je          __logf_e

    ; near one codepath
    andps       xmm4, XMMWORD PTR __real_notsign
    comiss      xmm4, DWORD PTR __real_threshold
    jb          __near_one

    ; F, Y
    movss       xmm1, DWORD PTR [p_temp+rsp]
    shr         eax, 16
    por         xmm2, XMMWORD PTR __real_half
    por         xmm1, XMMWORD PTR __real_half
    lea         r9, QWORD PTR __log_F_inv

    ; check for negative numbers or zero
    xorps       xmm4, xmm4
    comiss      xmm0, xmm4
    jbe         __x_is_zero_or_neg

    ; f = F - Y, r = f * inv
    subss       xmm1, xmm2
    mulss       xmm1, DWORD PTR [r9+rax*4]

    movss       xmm2, xmm1
    movss       xmm0, xmm1

    ; poly
    mulss       xmm2, DWORD PTR __real_1_over_3
    mulss       xmm0, xmm1
    addss       xmm2, DWORD PTR __real_1_over_2
    movss       xmm3, DWORD PTR __real_log2_tail

    lea         r9, QWORD PTR __log_128_tail
    lea         r10, QWORD PTR __log_128_lead

    mulss       xmm2, xmm0
    mulss       xmm3, xmm5
    addss       xmm1, xmm2

    ; m*log(2) + log(G) - poly
    movss       xmm0, DWORD PTR __real_log2_lead
    subss       xmm3, xmm1 ; z2
    mulss       xmm0, xmm5
    addss       xmm3, DWORD PTR [r9+rax*4] ; z2
    addss       xmm0, DWORD PTR [r10+rax*4] ; z1

    addss       xmm0, xmm3

    add         rsp, stack_size
    ret

ALIGN 16
__logf_e:
    movss       xmm0, DWORD PTR __real_one
    add         rsp, stack_size
    ret

ALIGN 16
__near_one:
    ; r = x - 1.0;
    movss       xmm2, DWORD PTR __real_two
    subss       xmm0, DWORD PTR __real_one

    ; u = r / (2.0 + r)
    addss       xmm2, xmm0
    movss       xmm1, xmm0
    divss       xmm1, xmm2 ; u

    ; correction = r * u
    movss       xmm4, xmm0
    mulss       xmm4, xmm1

    ; u = u + u;
    addss       xmm1, xmm1
    movss       xmm2, xmm1
    mulss       xmm2, xmm2 ; v = u^2

    ; r2 = (u * v * (ca_1 + v * ca_2) - correction)
    movss       xmm3, xmm1
    mulss       xmm3, xmm2 ; u^3
    mulss       xmm2, DWORD PTR __real_ca2 ; Bu^2
    addss       xmm2, DWORD PTR __real_ca1 ; +A
    mulss       xmm2, xmm3
    subss       xmm2, xmm4 ; -correction

    ; r + r2
    addss       xmm0, xmm2
    add         rsp, stack_size
    ret

ALIGN 16
__denormal_adjust:
    por         xmm2, XMMWORD PTR __real_one
    subss       xmm2, DWORD PTR __real_one
    movdqa      xmm5, xmm2
    pand        xmm2, XMMWORD PTR __real_mant
    movd        eax, xmm2
    psrld       xmm5, 23
    psubd       xmm5, XMMWORD PTR __mask_253
    cvtdq2ps    xmm5, xmm5
    jmp         __continue_common

ALIGN 16
__x_is_zero_or_neg:
    jne         __x_is_neg

    movss       xmm1, DWORD PTR __real_ninf
    mov         r8d, DWORD PTR __flag_x_zero
    call        fname_special
    jmp         __finish

ALIGN 16
__x_is_neg:

    movss       xmm1, DWORD PTR __real_neg_qnan
    mov         r8d, DWORD PTR __flag_x_neg
    call        fname_special
    jmp         __finish

ALIGN 16
__x_is_inf_or_nan:

    cmp         eax, DWORD PTR __real_inf
    je          __finish

    cmp         eax, DWORD PTR __real_ninf
    je          __x_is_neg

    or          eax, DWORD PTR __real_qnanbit
    movd        xmm1, eax
    mov         r8d, DWORD PTR __flag_x_nan
    call        fname_special
    jmp         __finish    

ALIGN 16
__finish:
    add         rsp, stack_size
    ret

fname endp

text ENDS

data SEGMENT READ

ALIGN 16

; these codes and the ones in the corresponding .c file have to match
__flag_x_zero           DD 00000001
__flag_x_neg            DD 00000002
__flag_x_nan            DD 00000003

ALIGN 16

__real_one              DQ 03f8000003f800000h   ; 1.0
                        DQ 03f8000003f800000h
__real_two              DQ 04000000040000000h   ; 1.0
                        DQ 04000000040000000h
__real_ninf             DQ 0ff800000ff800000h   ; -inf
                        DQ 0ff800000ff800000h
__real_inf              DQ 07f8000007f800000h   ; +inf
                        DQ 07f8000007f800000h
__real_nan              DQ 07fc000007fc00000h   ; NaN
                        DQ 07fc000007fc00000h
__real_ef               DQ 0402DF854402DF854h   ; float e
                        DQ 0402DF854402DF854h
__real_neg_qnan         DQ 0ffc00000ffc00000h
                        DQ 0ffc00000ffc00000h

__real_sign             DQ 08000000080000000h   ; sign bit
                        DQ 08000000080000000h
__real_notsign          DQ 07ffFFFFF7ffFFFFFh   ; ^sign bit
                        DQ 07ffFFFFF7ffFFFFFh
__real_qnanbit          DQ 00040000000400000h   ; quiet nan bit
                        DQ 00040000000400000h
__real_mant             DQ 0007FFFFF007FFFFFh   ; mantissa bits
                        DQ 0007FFFFF007FFFFFh
__mask_127              DQ 00000007f0000007fh   ; 
                        DQ 00000007f0000007fh

__mask_mant_all7        DQ 00000000007f0000h
                        DQ 00000000007f0000h
__mask_mant8            DQ 0000000000008000h
                        DQ 0000000000008000h

__real_ca1              DQ 03DAAAAAB3DAAAAABh   ; 8.33333333333317923934e-02
                        DQ 03DAAAAAB3DAAAAABh
__real_ca2              DQ 03C4CCCCD3C4CCCCDh   ; 1.25000000037717509602e-02
                        DQ 03C4CCCCD3C4CCCCDh

__real_log2_lead        DQ 03F3170003F317000h   ; 0.693115234375
                        DQ 03F3170003F317000h
__real_log2_tail        DQ 03805FDF43805FDF4h   ; 0.000031946183
                        DQ 03805FDF43805FDF4h
__real_half             DQ 03f0000003f000000h   ; 1/2
                        DQ 03f0000003f000000h


ALIGN 16

__real_neg127       DD 0c2fe0000h
                    DD 0
                    DQ 0

__mask_253          DD 000000fdh
                    DD 0
                    DQ 0

__real_threshold    DD 3d800000h
                    DD 0
                    DQ 0

__mask_01           DD 00000001h
                    DD 0
                    DQ 0

__mask_80           DD 00000080h
                    DD 0
                    DQ 0

__real_3b800000     DD 3b800000h
                    DD 0
                    DQ 0

__real_1_over_3     DD 3eaaaaabh
                    DD 0
                    DQ 0

__real_1_over_2     DD 3f000000h
                    DD 0
                    DQ 0

ALIGN 16
__log_128_lead:
                    DD 00000000h
                    DD 3bff0000h
                    DD 3c7e0000h
                    DD 3cbdc000h
                    DD 3cfc1000h
                    DD 3d1cf000h
                    DD 3d3ba000h
                    DD 3d5a1000h
                    DD 3d785000h
                    DD 3d8b2000h
                    DD 3d9a0000h
                    DD 3da8d000h
                    DD 3db78000h
                    DD 3dc61000h
                    DD 3dd49000h
                    DD 3de2f000h
                    DD 3df13000h
                    DD 3dff6000h
                    DD 3e06b000h
                    DD 3e0db000h
                    DD 3e14a000h
                    DD 3e1b8000h
                    DD 3e226000h
                    DD 3e293000h
                    DD 3e2ff000h
                    DD 3e36b000h
                    DD 3e3d5000h
                    DD 3e43f000h
                    DD 3e4a9000h
                    DD 3e511000h
                    DD 3e579000h
                    DD 3e5e1000h
                    DD 3e647000h
                    DD 3e6ae000h
                    DD 3e713000h
                    DD 3e778000h
                    DD 3e7dc000h
                    DD 3e820000h
                    DD 3e851000h
                    DD 3e882000h
                    DD 3e8b3000h
                    DD 3e8e4000h
                    DD 3e914000h
                    DD 3e944000h
                    DD 3e974000h
                    DD 3e9a3000h
                    DD 3e9d3000h
                    DD 3ea02000h
                    DD 3ea30000h
                    DD 3ea5f000h
                    DD 3ea8d000h
                    DD 3eabb000h
                    DD 3eae8000h
                    DD 3eb16000h
                    DD 3eb43000h
                    DD 3eb70000h
                    DD 3eb9c000h
                    DD 3ebc9000h
                    DD 3ebf5000h
                    DD 3ec21000h
                    DD 3ec4d000h
                    DD 3ec78000h
                    DD 3eca3000h
                    DD 3ecce000h
                    DD 3ecf9000h
                    DD 3ed24000h
                    DD 3ed4e000h
                    DD 3ed78000h
                    DD 3eda2000h
                    DD 3edcc000h
                    DD 3edf5000h
                    DD 3ee1e000h
                    DD 3ee47000h
                    DD 3ee70000h
                    DD 3ee99000h
                    DD 3eec1000h
                    DD 3eeea000h
                    DD 3ef12000h
                    DD 3ef3a000h
                    DD 3ef61000h
                    DD 3ef89000h
                    DD 3efb0000h
                    DD 3efd7000h
                    DD 3effe000h
                    DD 3f012000h
                    DD 3f025000h
                    DD 3f039000h
                    DD 3f04c000h
                    DD 3f05f000h
                    DD 3f072000h
                    DD 3f084000h
                    DD 3f097000h
                    DD 3f0aa000h
                    DD 3f0bc000h
                    DD 3f0cf000h
                    DD 3f0e1000h
                    DD 3f0f4000h
                    DD 3f106000h
                    DD 3f118000h
                    DD 3f12a000h
                    DD 3f13c000h
                    DD 3f14e000h
                    DD 3f160000h
                    DD 3f172000h
                    DD 3f183000h
                    DD 3f195000h
                    DD 3f1a7000h
                    DD 3f1b8000h
                    DD 3f1c9000h
                    DD 3f1db000h
                    DD 3f1ec000h
                    DD 3f1fd000h
                    DD 3f20e000h
                    DD 3f21f000h
                    DD 3f230000h
                    DD 3f241000h
                    DD 3f252000h
                    DD 3f263000h
                    DD 3f273000h
                    DD 3f284000h
                    DD 3f295000h
                    DD 3f2a5000h
                    DD 3f2b5000h
                    DD 3f2c6000h
                    DD 3f2d6000h
                    DD 3f2e6000h
                    DD 3f2f7000h
                    DD 3f307000h
                    DD 3f317000h

ALIGN 16
__log_128_tail:
                    DD 00000000h
                    DD 3429ac41h
                    DD 35a8b0fch
                    DD 368d83eah
                    DD 361b0e78h
                    DD 3687b9feh
                    DD 3631ec65h
                    DD 36dd7119h
                    DD 35c30045h
                    DD 379b7751h
                    DD 37ebcb0dh
                    DD 37839f83h
                    DD 37528ae5h
                    DD 37a2eb18h
                    DD 36da7495h
                    DD 36a91eb7h
                    DD 3783b715h
                    DD 371131dbh
                    DD 383f3e68h
                    DD 38156a97h
                    DD 38297c0fh
                    DD 387e100fh
                    DD 3815b665h
                    DD 37e5e3a1h
                    DD 38183853h
                    DD 35fe719dh
                    DD 38448108h
                    DD 38503290h
                    DD 373539e8h
                    DD 385e0ff1h
                    DD 3864a740h
                    DD 3786742dh
                    DD 387be3cdh
                    DD 3685ad3eh
                    DD 3803b715h
                    DD 37adcbdch
                    DD 380c36afh
                    DD 371652d3h
                    DD 38927139h
                    DD 38c5fcd7h
                    DD 38ae55d5h
                    DD 3818c169h
                    DD 38a0fde7h
                    DD 38ad09efh
                    DD 3862bae1h
                    DD 38eecd4ch
                    DD 3798aad2h
                    DD 37421a1ah
                    DD 38c5e10eh
                    DD 37bf2aeeh
                    DD 382d872dh
                    DD 37ee2e8ah
                    DD 38dedfach
                    DD 3802f2b9h
                    DD 38481e9bh
                    DD 380eaa2bh
                    DD 38ebfb5dh
                    DD 38255fddh
                    DD 38783b82h
                    DD 3851da1eh
                    DD 374e1b05h
                    DD 388f439bh
                    DD 38ca0e10h
                    DD 38cac08bh
                    DD 3891f65fh
                    DD 378121cbh
                    DD 386c9a9ah
                    DD 38949923h
                    DD 38777bcch
                    DD 37b12d26h
                    DD 38a6ced3h
                    DD 38ebd3e6h
                    DD 38fbe3cdh
                    DD 38d785c2h
                    DD 387e7e00h
                    DD 38f392c5h
                    DD 37d40983h
                    DD 38081a7ch
                    DD 3784c3adh
                    DD 38cce923h
                    DD 380f5fafh
                    DD 3891fd38h
                    DD 38ac47bch
                    DD 3897042bh
                    DD 392952d2h
                    DD 396fced4h
                    DD 37f97073h
                    DD 385e9eaeh
                    DD 3865c84ah
                    DD 38130ba3h
                    DD 3979cf16h
                    DD 3938cac9h
                    DD 38c3d2f4h
                    DD 39755dech
                    DD 38e6b467h
                    DD 395c0fb8h
                    DD 383ebce0h
                    DD 38dcd192h
                    DD 39186bdfh
                    DD 392de74ch
                    DD 392f0944h
                    DD 391bff61h
                    DD 38e9ed44h
                    DD 38686dc8h
                    DD 396b99a7h
                    DD 39099c89h
                    DD 37a27673h
                    DD 390bdaa3h
                    DD 397069abh
                    DD 388449ffh
                    DD 39013538h
                    DD 392dc268h
                    DD 3947f423h
                    DD 394ff17ch
                    DD 3945e10eh
                    DD 3929e8f5h
                    DD 38f85db0h
                    DD 38735f99h
                    DD 396c08dbh
                    DD 3909e600h
                    DD 37b4996fh
                    DD 391233cch
                    DD 397cead9h
                    DD 38adb5cdh
                    DD 3920261ah
                    DD 3958ee36h
                    DD 35aa4905h
                    DD 37cbd11eh
                    DD 3805fdf4h

ALIGN 16
__log_F_inv:
                    DD 40000000h
                    DD 3ffe03f8h
                    DD 3ffc0fc1h
                    DD 3ffa232dh
                    DD 3ff83e10h
                    DD 3ff6603eh
                    DD 3ff4898dh
                    DD 3ff2b9d6h
                    DD 3ff0f0f1h
                    DD 3fef2eb7h
                    DD 3fed7304h
                    DD 3febbdb3h
                    DD 3fea0ea1h
                    DD 3fe865ach
                    DD 3fe6c2b4h
                    DD 3fe52598h
                    DD 3fe38e39h
                    DD 3fe1fc78h
                    DD 3fe07038h
                    DD 3fdee95ch
                    DD 3fdd67c9h
                    DD 3fdbeb62h
                    DD 3fda740eh
                    DD 3fd901b2h
                    DD 3fd79436h
                    DD 3fd62b81h
                    DD 3fd4c77bh
                    DD 3fd3680dh
                    DD 3fd20d21h
                    DD 3fd0b6a0h
                    DD 3fcf6475h
                    DD 3fce168ah
                    DD 3fcccccdh
                    DD 3fcb8728h
                    DD 3fca4588h
                    DD 3fc907dah
                    DD 3fc7ce0ch
                    DD 3fc6980ch
                    DD 3fc565c8h
                    DD 3fc43730h
                    DD 3fc30c31h
                    DD 3fc1e4bch
                    DD 3fc0c0c1h
                    DD 3fbfa030h
                    DD 3fbe82fah
                    DD 3fbd6910h
                    DD 3fbc5264h
                    DD 3fbb3ee7h
                    DD 3fba2e8ch
                    DD 3fb92144h
                    DD 3fb81703h
                    DD 3fb70fbbh
                    DD 3fb60b61h
                    DD 3fb509e7h
                    DD 3fb40b41h
                    DD 3fb30f63h
                    DD 3fb21643h
                    DD 3fb11fd4h
                    DD 3fb02c0bh
                    DD 3faf3adeh
                    DD 3fae4c41h
                    DD 3fad602bh
                    DD 3fac7692h
                    DD 3fab8f6ah
                    DD 3faaaaabh
                    DD 3fa9c84ah
                    DD 3fa8e83fh
                    DD 3fa80a81h
                    DD 3fa72f05h
                    DD 3fa655c4h
                    DD 3fa57eb5h
                    DD 3fa4a9cfh
                    DD 3fa3d70ah
                    DD 3fa3065eh
                    DD 3fa237c3h
                    DD 3fa16b31h
                    DD 3fa0a0a1h
                    DD 3f9fd80ah
                    DD 3f9f1166h
                    DD 3f9e4cadh
                    DD 3f9d89d9h
                    DD 3f9cc8e1h
                    DD 3f9c09c1h
                    DD 3f9b4c70h
                    DD 3f9a90e8h
                    DD 3f99d723h
                    DD 3f991f1ah
                    DD 3f9868c8h
                    DD 3f97b426h
                    DD 3f97012eh
                    DD 3f964fdah
                    DD 3f95a025h
                    DD 3f94f209h
                    DD 3f944581h
                    DD 3f939a86h
                    DD 3f92f114h
                    DD 3f924925h
                    DD 3f91a2b4h
                    DD 3f90fdbch
                    DD 3f905a38h
                    DD 3f8fb824h
                    DD 3f8f177ah
                    DD 3f8e7835h
                    DD 3f8dda52h
                    DD 3f8d3dcbh
                    DD 3f8ca29ch
                    DD 3f8c08c1h
                    DD 3f8b7034h
                    DD 3f8ad8f3h
                    DD 3f8a42f8h
                    DD 3f89ae41h
                    DD 3f891ac7h
                    DD 3f888889h
                    DD 3f87f781h
                    DD 3f8767abh
                    DD 3f86d905h
                    DD 3f864b8ah
                    DD 3f85bf37h
                    DD 3f853408h
                    DD 3f84a9fah
                    DD 3f842108h
                    DD 3f839930h
                    DD 3f83126fh
                    DD 3f828cc0h
                    DD 3f820821h
                    DD 3f81848eh
                    DD 3f810204h
                    DD 3f808081h
                    DD 3f800000h

data ENDS

END
