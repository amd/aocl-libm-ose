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
; log10f.asm
;
; An implementation of the log10f libm function.
;
; Prototype:
;
;     float log10f(float x);
;

;
;   Algorithm:
;       Similar to one presnted in log.asm
;

include fm.inc

ALM_PROTO_BAS64 log10f

fname_special   TEXTEQU <_log10f_special>

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
    movss       xmm3, DWORD PTR __real_log10_2_tail

    lea         r9, QWORD PTR __log_128_tail
    lea         r10, QWORD PTR __log_128_lead

    mulss       xmm2, xmm0
    mulss       xmm3, xmm5
    addss       xmm1, xmm2

    mulss       xmm1, DWORD PTR __real_log10_e

    ; m*log(10) + log10(G) - poly
    movss       xmm0, DWORD PTR __real_log10_2_lead
    subss       xmm3, xmm1 ; z2
    mulss       xmm0, xmm5
    addss       xmm3, DWORD PTR [r9+rax*4]
    addss       xmm0, DWORD PTR [r10+rax*4]

    addss       xmm0, xmm3

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

    mulss       xmm2, DWORD PTR __real_log10_e_tail
    mulss       xmm0, DWORD PTR __real_log10_e_tail
    mulss       xmm1, DWORD PTR __real_log10_e_lead
    mulss       xmm5, DWORD PTR __real_log10_e_lead

    addss       xmm0, xmm2
    addss       xmm0, xmm1
    addss       xmm0, xmm5

    add         rsp, stack_size
    ret

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

__real_log10_e_lead     DQ 3EDE00003EDE0000h    ; log10e_lead  0.4335937500
                        DQ 3EDE00003EDE0000h
__real_log10_e_tail     DQ 3A37B1523A37B152h    ; log10e_tail  0.0007007319
                        DQ 3A37B1523A37B152h

__real_log10_2_lead     DQ 3e9a00003e9a0000h
                        DQ 0000000000000000h
__real_log10_2_tail     DQ 39826a1339826a13h
                        DQ 0000000000000000h
__real_log10_e          DQ 3ede5bd93ede5bd9h
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
                    DD 3b5d4000h
                    DD 3bdc8000h
                    DD 3c24c000h
                    DD 3c5ac000h
                    DD 3c884000h
                    DD 3ca2c000h
                    DD 3cbd4000h
                    DD 3cd78000h
                    DD 3cf1c000h
                    DD 3d05c000h
                    DD 3d128000h
                    DD 3d1f4000h
                    DD 3d2c0000h
                    DD 3d388000h
                    DD 3d450000h
                    DD 3d518000h
                    DD 3d5dc000h
                    DD 3d6a0000h
                    DD 3d760000h
                    DD 3d810000h
                    DD 3d870000h
                    DD 3d8d0000h
                    DD 3d92c000h
                    DD 3d98c000h
                    DD 3d9e8000h
                    DD 3da44000h
                    DD 3daa0000h
                    DD 3dafc000h
                    DD 3db58000h
                    DD 3dbb4000h
                    DD 3dc0c000h
                    DD 3dc64000h
                    DD 3dcc0000h
                    DD 3dd18000h
                    DD 3dd6c000h
                    DD 3ddc4000h
                    DD 3de1c000h
                    DD 3de70000h
                    DD 3dec8000h
                    DD 3df1c000h
                    DD 3df70000h
                    DD 3dfc4000h
                    DD 3e00c000h
                    DD 3e034000h
                    DD 3e05c000h
                    DD 3e088000h
                    DD 3e0b0000h
                    DD 3e0d8000h
                    DD 3e100000h
                    DD 3e128000h
                    DD 3e150000h
                    DD 3e178000h
                    DD 3e1a0000h
                    DD 3e1c8000h
                    DD 3e1ec000h
                    DD 3e214000h
                    DD 3e23c000h
                    DD 3e260000h
                    DD 3e288000h
                    DD 3e2ac000h
                    DD 3e2d4000h
                    DD 3e2f8000h
                    DD 3e31c000h
                    DD 3e344000h
                    DD 3e368000h
                    DD 3e38c000h
                    DD 3e3b0000h
                    DD 3e3d4000h
                    DD 3e3fc000h
                    DD 3e420000h
                    DD 3e440000h
                    DD 3e464000h
                    DD 3e488000h
                    DD 3e4ac000h
                    DD 3e4d0000h
                    DD 3e4f4000h
                    DD 3e514000h
                    DD 3e538000h
                    DD 3e55c000h
                    DD 3e57c000h
                    DD 3e5a0000h
                    DD 3e5c0000h
                    DD 3e5e4000h
                    DD 3e604000h
                    DD 3e624000h
                    DD 3e648000h
                    DD 3e668000h
                    DD 3e688000h
                    DD 3e6ac000h
                    DD 3e6cc000h
                    DD 3e6ec000h
                    DD 3e70c000h
                    DD 3e72c000h
                    DD 3e74c000h
                    DD 3e76c000h
                    DD 3e78c000h
                    DD 3e7ac000h
                    DD 3e7cc000h
                    DD 3e7ec000h
                    DD 3e804000h
                    DD 3e814000h
                    DD 3e824000h
                    DD 3e834000h
                    DD 3e840000h
                    DD 3e850000h
                    DD 3e860000h
                    DD 3e870000h
                    DD 3e880000h
                    DD 3e88c000h
                    DD 3e89c000h
                    DD 3e8ac000h
                    DD 3e8bc000h
                    DD 3e8c8000h
                    DD 3e8d8000h
                    DD 3e8e8000h
                    DD 3e8f4000h
                    DD 3e904000h
                    DD 3e914000h
                    DD 3e920000h
                    DD 3e930000h
                    DD 3e93c000h
                    DD 3e94c000h
                    DD 3e958000h
                    DD 3e968000h
                    DD 3e978000h
                    DD 3e984000h
                    DD 3e994000h
                    DD 3e9a0000h

ALIGN 16
__log_128_tail:
                    DD 00000000h
                    DD 367a8e44h
                    DD 368ed49fh
                    DD 36c21451h
                    DD 375211d6h
                    DD 3720ea11h
                    DD 37e9eb59h
                    DD 37b87be7h
                    DD 37bf2560h
                    DD 33d597a0h
                    DD 37806a05h
                    DD 3820581fh
                    DD 38223334h
                    DD 378e3bach
                    DD 3810684fh
                    DD 37feb7aeh
                    DD 36a9d609h
                    DD 37a68163h
                    DD 376a8b27h
                    DD 384c8fd6h
                    DD 3885183eh
                    DD 3874a760h
                    DD 380d1154h
                    DD 38ea42bdh
                    DD 384c1571h
                    DD 38ba66b8h
                    DD 38e7da3bh
                    DD 38eee632h
                    DD 38d00911h
                    DD 388bbedeh
                    DD 378a0512h
                    DD 3894c7a0h
                    DD 38e30710h
                    DD 36db2829h
                    DD 3729d609h
                    DD 38fa0e82h
                    DD 38bc9a75h
                    DD 383a9297h
                    DD 38dc83c8h
                    DD 37eac335h
                    DD 38706ac3h
                    DD 389574c2h
                    DD 3892d068h
                    DD 38615032h
                    DD 3917acf4h
                    DD 3967a126h
                    DD 38217840h
                    DD 38b420abh
                    DD 38f9c7b2h
                    DD 391103bdh
                    DD 39169a6bh
                    DD 390dd194h
                    DD 38eda471h
                    DD 38a38950h
                    DD 37f6844ah
                    DD 395e1cdbh
                    DD 390fcffch
                    DD 38503e9dh
                    DD 394b00fdh
                    DD 38a9910ah
                    DD 39518a31h
                    DD 3882d2c2h
                    DD 392488e4h
                    DD 397b0affh
                    DD 388a22d8h
                    DD 3902bd5eh
                    DD 39342f85h
                    DD 39598811h
                    DD 3972e6b1h
                    DD 34d53654h
                    DD 360ca25eh
                    DD 39785cc0h
                    DD 39630710h
                    DD 39424ed7h
                    DD 39165101h
                    DD 38be5421h
                    DD 37e7b0c0h
                    DD 394fd0c3h
                    DD 38efaaaah
                    DD 37a8f566h
                    DD 3927c744h
                    DD 383fa4d5h
                    DD 392d9e39h
                    DD 3803feaeh
                    DD 390a268ch
                    DD 39692b80h
                    DD 38789b4fh
                    DD 3909307dh
                    DD 394a601ch
                    DD 35e67edch
                    DD 383e386dh
                    DD 38a7743dh
                    DD 38dccec3h
                    DD 38ff57e0h
                    DD 39079d8bh
                    DD 390651a6h
                    DD 38f7bad9h
                    DD 38d0ab82h
                    DD 38979e7dh
                    DD 381978eeh
                    DD 397816c8h
                    DD 39410cb2h
                    DD 39015384h
                    DD 3863fa28h
                    DD 39f41065h
                    DD 39c7668ah
                    DD 39968afah
                    DD 39430db9h
                    DD 38a18cf3h
                    DD 39eb2907h
                    DD 39a9e10ch
                    DD 39492800h
                    DD 385a53d1h
                    DD 39ce0cf7h
                    DD 3979c7b2h
                    DD 389f5d99h
                    DD 39ceefcbh
                    DD 39646a39h
                    DD 380d7a9bh
                    DD 39ad6650h
                    DD 390ac3b8h
                    DD 39d9a9a8h
                    DD 39548a99h
                    DD 39f73c4bh
                    DD 3980960eh
                    DD 374b3d5ah
                    DD 39888f1eh
                    DD 37679a07h
                    DD 39826a13h

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
