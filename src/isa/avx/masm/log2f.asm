;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
; log2f.asm
;
; An implementation of the log2f libm function.
;
; Prototype:
;
;     float log2f(float x);
;

;
;   Algorithm:
;       Similar to one presnted in log.asm
;

include fm.inc

FN_PROTOTYPE_BAS64 log2f

fname_special   TEXTEQU <_log2f_special>

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

    ; check for negative numbers or zero
    xorps       xmm1, xmm1
    comiss      xmm0, xmm1
    jbe         __x_is_zero_or_neg

    pand        xmm2, XMMWORD PTR __real_mant
    subss       xmm4, DWORD PTR __real_one

    comiss      xmm5, DWORD PTR __real_neg127
    je          __denormal_adjust

__continue_common:

    ; compute index into the log tables
    mov         r9d, eax
    and         eax, DWORD PTR __mask_mant_all7
    and         r9d, DWORD PTR __mask_mant8
    shl         r9d, 1
    add         eax, r9d
    mov         DWORD PTR [p_temp+rsp], eax

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

    ; f = F - Y, r = f * inv
    subss       xmm1, xmm2
    mulss       xmm1, DWORD PTR [r9+rax*4]

    movss       xmm2, xmm1
    movss       xmm0, xmm1

    ; poly
    mulss       xmm2, DWORD PTR __real_1_over_3
    mulss       xmm0, xmm1
    addss       xmm2, DWORD PTR __real_1_over_2

    lea         r9, QWORD PTR __log_128_tail
    lea         r10, QWORD PTR __log_128_lead

    mulss       xmm2, xmm0
    movss       xmm3, DWORD PTR [r9+rax*4]
    addss       xmm1, xmm2

    mulss       xmm1, DWORD PTR __real_log2_e

    ; m + log2(G) - poly*log2_e
    subss       xmm3, xmm1 
    movss       xmm0, xmm3
    addss       xmm5, DWORD PTR [r10+rax*4]
    addss       xmm0, xmm5

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

    movdqa      xmm5, xmm0
    pand        xmm5, XMMWORD PTR __mask_lower
    subss       xmm0, xmm5
    addss       xmm2, xmm0

    movss       xmm0, xmm5
    movss       xmm1, xmm2

    mulss       xmm2, DWORD PTR __real_log2_e_tail
    mulss       xmm0, DWORD PTR __real_log2_e_tail
    mulss       xmm1, DWORD PTR __real_log2_e_lead
    mulss       xmm5, DWORD PTR __real_log2_e_lead

    addss       xmm0, xmm2
    addss       xmm0, xmm1
    addss       xmm0, xmm5

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

__real_log2_e_lead      DQ 03FB800003FB80000h   ; 1.4375000000
                        DQ 03FB800003FB80000h
__real_log2_e_tail      DQ 03BAA3B293BAA3B29h   ; 0.0051950408889633
                        DQ 03BAA3B293BAA3B29h

__real_log2_e           DQ 3fb8aa3b3fb8aa3bh
                        DQ 0000000000000000h

__mask_lower            DQ 0ffff0000ffff0000h
                        DQ 0ffff0000ffff0000h

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
                    DD 3c37c000h
                    DD 3cb70000h
                    DD 3d08c000h
                    DD 3d35c000h
                    DD 3d624000h
                    DD 3d874000h
                    DD 3d9d4000h
                    DD 3db30000h
                    DD 3dc8c000h
                    DD 3dde4000h
                    DD 3df38000h
                    DD 3e044000h
                    DD 3e0ec000h
                    DD 3e194000h
                    DD 3e238000h
                    DD 3e2e0000h
                    DD 3e380000h
                    DD 3e424000h
                    DD 3e4c4000h
                    DD 3e564000h
                    DD 3e604000h
                    DD 3e6a4000h
                    DD 3e740000h
                    DD 3e7dc000h
                    DD 3e83c000h
                    DD 3e888000h
                    DD 3e8d4000h
                    DD 3e920000h
                    DD 3e96c000h
                    DD 3e9b8000h
                    DD 3ea00000h
                    DD 3ea4c000h
                    DD 3ea94000h
                    DD 3eae0000h
                    DD 3eb28000h
                    DD 3eb70000h
                    DD 3ebb8000h
                    DD 3ec00000h
                    DD 3ec44000h
                    DD 3ec8c000h
                    DD 3ecd4000h
                    DD 3ed18000h
                    DD 3ed5c000h
                    DD 3eda0000h
                    DD 3ede8000h
                    DD 3ee2c000h
                    DD 3ee70000h
                    DD 3eeb0000h
                    DD 3eef4000h
                    DD 3ef38000h
                    DD 3ef78000h
                    DD 3efbc000h
                    DD 3effc000h
                    DD 3f01c000h
                    DD 3f040000h
                    DD 3f060000h
                    DD 3f080000h
                    DD 3f0a0000h
                    DD 3f0c0000h
                    DD 3f0dc000h
                    DD 3f0fc000h
                    DD 3f11c000h
                    DD 3f13c000h
                    DD 3f15c000h
                    DD 3f178000h
                    DD 3f198000h
                    DD 3f1b4000h
                    DD 3f1d4000h
                    DD 3f1f0000h
                    DD 3f210000h
                    DD 3f22c000h
                    DD 3f24c000h
                    DD 3f268000h
                    DD 3f288000h
                    DD 3f2a4000h
                    DD 3f2c0000h
                    DD 3f2dc000h
                    DD 3f2f8000h
                    DD 3f318000h
                    DD 3f334000h
                    DD 3f350000h
                    DD 3f36c000h
                    DD 3f388000h
                    DD 3f3a4000h
                    DD 3f3c0000h
                    DD 3f3dc000h
                    DD 3f3f8000h
                    DD 3f414000h
                    DD 3f42c000h
                    DD 3f448000h
                    DD 3f464000h
                    DD 3f480000h
                    DD 3f498000h
                    DD 3f4b4000h
                    DD 3f4d0000h
                    DD 3f4e8000h
                    DD 3f504000h
                    DD 3f51c000h
                    DD 3f538000h
                    DD 3f550000h
                    DD 3f56c000h
                    DD 3f584000h
                    DD 3f5a0000h
                    DD 3f5b8000h
                    DD 3f5d0000h
                    DD 3f5ec000h
                    DD 3f604000h
                    DD 3f61c000h
                    DD 3f638000h
                    DD 3f650000h
                    DD 3f668000h
                    DD 3f680000h
                    DD 3f698000h
                    DD 3f6b0000h
                    DD 3f6cc000h
                    DD 3f6e4000h
                    DD 3f6fc000h
                    DD 3f714000h
                    DD 3f72c000h
                    DD 3f744000h
                    DD 3f75c000h
                    DD 3f770000h
                    DD 3f788000h
                    DD 3f7a0000h
                    DD 3f7b8000h
                    DD 3f7d0000h
                    DD 3f7e8000h
                    DD 3f800000h

ALIGN 16
__log_128_tail:
                    DD 00000000h
                    DD 374a16ddh
                    DD 37f2d0b8h
                    DD 381a3aa2h
                    DD 37b4dd63h
                    DD 383f5721h
                    DD 384e27e8h
                    DD 380bf749h
                    DD 387dbeb2h
                    DD 37216e46h
                    DD 3684815bh
                    DD 383b045fh
                    DD 390b119bh
                    DD 391a32eah
                    DD 38ba789eh
                    DD 39553f30h
                    DD 3651cfdeh
                    DD 39685a9dh
                    DD 39057a05h
                    DD 395ba0efh
                    DD 396bc5b6h
                    DD 3936d9bbh
                    DD 38772619h
                    DD 39017ce9h
                    DD 3902d720h
                    DD 38856dd8h
                    DD 3941f6b4h
                    DD 3980b652h
                    DD 3980f561h
                    DD 39443f13h
                    DD 38926752h
                    DD 39c8c763h
                    DD 391e12f3h
                    DD 39b7bf89h
                    DD 36d1cfdeh
                    DD 38c7f233h
                    DD 39087367h
                    DD 38e95d3fh
                    DD 38256316h
                    DD 39d38e5ch
                    DD 396ea247h
                    DD 350e4788h
                    DD 395d829fh
                    DD 39c30f2fh
                    DD 39fd7ee7h
                    DD 3872e9e7h
                    DD 3897d694h
                    DD 3824923ah
                    DD 39ea7c06h
                    DD 39a7fa88h
                    DD 391aa879h
                    DD 39dace65h
                    DD 39215a32h
                    DD 39af3350h
                    DD 3a7b5172h
                    DD 389cf27fh
                    DD 3902806bh
                    DD 3909d8a9h
                    DD 38c9faa1h
                    DD 37a33dcah
                    DD 3a6623d2h
                    DD 3a3c7a61h
                    DD 3a083a84h
                    DD 39930161h
                    DD 35d1cfdeh
                    DD 3a2d0ebdh
                    DD 399f1aadh
                    DD 3a67ff6dh
                    DD 39ecfea8h
                    DD 3a7b26f3h
                    DD 39ec1fa6h
                    DD 3a675314h
                    DD 399e12f3h
                    DD 3a2d4b66h
                    DD 370c3845h
                    DD 399ba329h
                    DD 3a1044d3h
                    DD 3a49a196h
                    DD 3a79fe83h
                    DD 3905c7aah
                    DD 39802391h
                    DD 39abe796h
                    DD 39c65a9dh
                    DD 39cfa6c5h
                    DD 39c7f593h
                    DD 39af6ff7h
                    DD 39863e4dh
                    DD 391910c1h
                    DD 369d5be7h
                    DD 3a541616h
                    DD 3a1ee960h
                    DD 39c38ed2h
                    DD 38e61600h
                    DD 3a4fedb4h
                    DD 39f6b4abh
                    DD 38f8d3b0h
                    DD 3a3b3faah
                    DD 399fb693h
                    DD 3a5cfe71h
                    DD 39c5740bh
                    DD 3a611eb0h
                    DD 39b079c4h
                    DD 3a4824d7h
                    DD 39439a54h
                    DD 3a1291eah
                    DD 3a6d3673h
                    DD 3981c731h
                    DD 3a0da88fh
                    DD 3a53945ch
                    DD 3895ae91h
                    DD 3996372ah
                    DD 39f9a832h
                    DD 3a27eda4h
                    DD 3a4c764fh
                    DD 3a6a7c06h
                    DD 370321ebh
                    DD 3899ab3fh
                    DD 38f02086h
                    DD 390a1707h
                    DD 39031e44h
                    DD 38c6b362h
                    DD 382bf195h
                    DD 3a768e36h
                    DD 3a5c503bh
                    DD 3a3c1179h
                    DD 3a15de1dh
                    DD 39d3845dh
                    DD 395f263fh
                    DD 00000000h

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
