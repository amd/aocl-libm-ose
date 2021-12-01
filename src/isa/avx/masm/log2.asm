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
; log2.asm
;
; An implementation of the log2 libm function.
;
; Prototype:
;
;     double log2(double x);
;

;
;   Algorithm:
;       Similar to one presnted in log.asm
;

include fm.inc
ALM_PROTO_BAS64 log2

fname_special   TEXTEQU <alm_log_special>

; define local variable storage offsets

p_temp          EQU     30h
save_xmm6       EQU     40h

;arg_x           EQU     0h
;arg_y           EQU     10h
;arg_code        EQU     20h

stack_size      EQU     98h

; external function
EXTERN fname_special:PROC

; macros

StackAllocate   MACRO size
                    sub         rsp, size
                    .ALLOCSTACK size
                ENDM

SaveXmm         MACRO xmmreg, offset
                    movdqa      XMMWORD PTR [offset+rsp], xmmreg
                    .SAVEXMM128 xmmreg, offset
                ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveXmm xmm6, save_xmm6
    .ENDPROLOG

    ; compute exponent part
    xor         rax, rax
    movdqa      xmm3, xmm0
    movsd       xmm4, xmm0
    psrlq       xmm3, 52
    movd        rax, xmm0
    psubq       xmm3, XMMWORD PTR __mask_1023
    movdqa      xmm2, xmm0
    cvtdq2pd    xmm6, xmm3 ; xexp

    ;  NaN or inf
    movdqa      xmm5, xmm0
    andpd       xmm5, XMMWORD PTR __real_inf
    comisd      xmm5, QWORD PTR __real_inf
    je          __x_is_inf_or_nan

    ; check for negative numbers or zero
    xorpd       xmm5, xmm5
    comisd      xmm0, xmm5
    jbe         __x_is_zero_or_neg

    pand        xmm2, XMMWORD PTR __real_mant
    subsd       xmm4, QWORD PTR __real_one

    comisd      xmm6, QWORD PTR __mask_1023_f
    je          __denormal_adjust

__continue_common:    

    ; compute index into the log tables
    mov         r9, rax
    and         rax, QWORD PTR __mask_mant_all8
    and         r9, QWORD PTR __mask_mant9
    shl         r9, 1
    add         rax, r9
    mov         QWORD PTR [p_temp+rsp], rax

    ; near one codepath
    andpd       xmm4, XMMWORD PTR __real_notsign
    comisd      xmm4, QWORD PTR __real_threshold
    jb          __near_one

    ; F, Y
    movsd       xmm1, QWORD PTR [p_temp+rsp]
    shr         rax, 44
    por         xmm2, XMMWORD PTR __real_half
    por         xmm1, XMMWORD PTR __real_half
    lea         r9, QWORD PTR __log_F_inv

    ; f = F - Y, r = f * inv
    subsd       xmm1, xmm2
    mulsd       xmm1, QWORD PTR [r9+rax*8]

    movsd       xmm2, xmm1
    movsd       xmm0, xmm1
    lea         r9, QWORD PTR __log_256_lead

    ; poly
    movsd       xmm3, QWORD PTR __real_1_over_6
    movsd       xmm1, QWORD PTR __real_1_over_3
    mulsd       xmm3, xmm2                         
    mulsd       xmm1, xmm2                         
    mulsd       xmm0, xmm2                         
    movsd       xmm4, xmm0
    addsd       xmm3, QWORD PTR __real_1_over_5
    addsd       xmm1, QWORD PTR __real_1_over_2
    mulsd       xmm4, xmm0                         
    mulsd       xmm3, xmm2                         
    mulsd       xmm1, xmm0                         
    addsd       xmm3, QWORD PTR __real_1_over_4
    addsd       xmm1, xmm2                         
    mulsd       xmm3, xmm4                         
    addsd       xmm1, xmm3                         

    mulsd       xmm1, QWORD PTR __real_log2_e

    ; m + log2(G) - poly
    movsd       xmm0, QWORD PTR [r9+rax*8]
    lea         rdx, QWORD PTR __log_256_tail
    movsd       xmm2, QWORD PTR [rdx+rax*8]
    subsd       xmm2, xmm1
   
    addsd       xmm0, xmm6 
    addsd       xmm0, xmm2

    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
    add         rsp, stack_size
    ret

ALIGN 16
__near_one:

    ; r = x - 1.0
    movsd       xmm2, QWORD PTR __real_two
    subsd       xmm0, QWORD PTR __real_one ; r

    addsd       xmm2, xmm0
    movsd       xmm1, xmm0
    divsd       xmm1, xmm2 ; r/(2+r) = u/2

    movsd       xmm4, QWORD PTR __real_ca2
    movsd       xmm5, QWORD PTR __real_ca4

    movsd       xmm6, xmm0
    mulsd       xmm6, xmm1 ; correction

    addsd       xmm1, xmm1 ; u
    movsd       xmm2, xmm1

    mulsd       xmm2, xmm1 ; u^2

    mulsd       xmm4, xmm2
    mulsd       xmm5, xmm2

    addsd       xmm4, QWORD PTR __real_ca1
    addsd       xmm5, QWORD PTR __real_ca3

    mulsd       xmm2, xmm1 ; u^3
    mulsd       xmm4, xmm2

    mulsd       xmm2, xmm2
    mulsd       xmm2, xmm1 ; u^7
    mulsd       xmm5, xmm2

    addsd       xmm4, xmm5
    subsd       xmm4, xmm6

    movdqa      xmm3, xmm0
    pand        xmm3, XMMWORD PTR __mask_lower
    subsd       xmm0, xmm3
    addsd       xmm4, xmm0

    movsd       xmm0, xmm3
    movsd       xmm1, xmm4

    mulsd       xmm4, QWORD PTR __real_log2_e_tail
    mulsd       xmm0, QWORD PTR __real_log2_e_tail
    mulsd       xmm1, QWORD PTR __real_log2_e_lead
    mulsd       xmm3, QWORD PTR __real_log2_e_lead

    addsd       xmm0, xmm4
    addsd       xmm0, xmm1
    addsd       xmm0, xmm3

    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
    add         rsp, stack_size
    ret

ALIGN 16
__denormal_adjust:
    por         xmm2, XMMWORD PTR __real_one
    subsd       xmm2, QWORD PTR __real_one
    movsd       xmm5, xmm2
    pand        xmm2, XMMWORD PTR __real_mant
    movd        rax, xmm2
    psrlq       xmm5, 52
    psubd       xmm5, XMMWORD PTR __mask_2045
    cvtdq2pd    xmm6, xmm5
    jmp         __continue_common

ALIGN 16
__x_is_zero_or_neg:
    jne         __x_is_neg

    movsd       xmm1, QWORD PTR __real_ninf
    mov         r8d, DWORD PTR __flag_x_zero
    call        fname_special
    jmp         __finish

ALIGN 16
__x_is_neg:

    movsd       xmm1, QWORD PTR __real_neg_qnan
    mov         r8d, DWORD PTR __flag_x_neg
    call        fname_special
    jmp         __finish

ALIGN 16
__x_is_inf_or_nan:

    cmp         rax, QWORD PTR __real_inf
    je          __finish

    cmp         rax, QWORD PTR __real_ninf
    je          __x_is_neg

    or          rax, QWORD PTR __real_qnanbit
    movd        xmm1, rax
    mov         r8d, DWORD PTR __flag_x_nan
    call        fname_special
    jmp         __finish    

ALIGN 16
__finish:
    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
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

__real_ninf         DQ 0fff0000000000000h   ; -inf
                    DQ 0000000000000000h
__real_inf          DQ 7ff0000000000000h    ; +inf
                    DQ 0000000000000000h
__real_neg_qnan     DQ 0fff8000000000000h   ; neg qNaN
                    DQ 0000000000000000h
__real_qnan         DQ 7ff8000000000000h   ; qNaN
                    DQ 0000000000000000h
__real_qnanbit      DQ 0008000000000000h
                    DQ 0000000000000000h
__real_mant         DQ 000FFFFFFFFFFFFFh    ; mantissa bits
                    DQ 0000000000000000h
__mask_1023         DQ 00000000000003ffh
                    DQ 0000000000000000h
__mask_001          DQ 0000000000000001h
                    DQ 0000000000000000h

__mask_mant_all8        DQ 000ff00000000000h
                        DQ 0000000000000000h
__mask_mant9            DQ 0000080000000000h
                        DQ 0000000000000000h

__real_log2_e           DQ 3ff71547652b82feh
                        DQ 0000000000000000h

__real_log2_e_lead      DQ 3ff7154400000000h ; log2e_lead 1.44269180297851562500E+00
                        DQ 0000000000000000h
__real_log2_e_tail      DQ 3ecb295c17f0bbbeh ; log2e_tail 3.23791044778235969970E-06
                        DQ 0000000000000000h

__real_two          DQ 4000000000000000h ; 2
                    DQ 0000000000000000h

__real_one          DQ 3ff0000000000000h ; 1
                    DQ 0000000000000000h

__real_half         DQ 3fe0000000000000h ; 1/2
                    DQ 0000000000000000h

__mask_100          DQ 0000000000000100h
                    DQ 0000000000000000h

__real_1_over_512       DQ 3f60000000000000h
                        DQ 0000000000000000h

__real_1_over_2     DQ 3fe0000000000000h
                    DQ 0000000000000000h
__real_1_over_3     DQ 3fd5555555555555h
                    DQ 0000000000000000h
__real_1_over_4     DQ 3fd0000000000000h
                    DQ 0000000000000000h
__real_1_over_5     DQ 3fc999999999999ah
                    DQ 0000000000000000h
__real_1_over_6     DQ 3fc5555555555555h
                    DQ 0000000000000000h

__mask_1023_f       DQ 0c08ff80000000000h
                    DQ 0000000000000000h

__mask_2045         DQ 00000000000007fdh
                    DQ 0000000000000000h

__real_threshold    DQ 3fb0000000000000h ; .0625
                    DQ 0000000000000000h

__real_notsign      DQ 7ffFFFFFFFFFFFFFh ; ^sign bit
                    DQ 0000000000000000h

__real_ca1          DQ 3fb55555555554e6h ; 8.33333333333317923934e-02
                    DQ 0000000000000000h
__real_ca2          DQ 3f89999999bac6d4h ; 1.25000000037717509602e-02
                    DQ 0000000000000000h
__real_ca3          DQ 3f62492307f1519fh ; 2.23213998791944806202e-03
                    DQ 0000000000000000h
__real_ca4          DQ 3f3c8034c85dfff0h ; 4.34887777707614552256e-04
                    DQ 0000000000000000h

__mask_lower        DQ 0ffffffff00000000h
                    DQ 0000000000000000h

ALIGN 16
__log_256_lead:
                    DQ 0000000000000000h
                    DQ 3f7709c460000000h
                    DQ 3f86fe50b0000000h
                    DQ 3f91363110000000h
                    DQ 3f96e79680000000h
                    DQ 3f9c9363b0000000h
                    DQ 3fa11cd1d0000000h
                    DQ 3fa3ed3090000000h
                    DQ 3fa6bad370000000h
                    DQ 3fa985bfc0000000h
                    DQ 3fac4dfab0000000h
                    DQ 3faf138980000000h
                    DQ 3fb0eb3890000000h
                    DQ 3fb24b5b70000000h
                    DQ 3fb3aa2fd0000000h
                    DQ 3fb507b830000000h
                    DQ 3fb663f6f0000000h
                    DQ 3fb7beee90000000h
                    DQ 3fb918a160000000h
                    DQ 3fba7111d0000000h
                    DQ 3fbbc84240000000h
                    DQ 3fbd1e34e0000000h
                    DQ 3fbe72ec10000000h
                    DQ 3fbfc66a00000000h
                    DQ 3fc08c5880000000h
                    DQ 3fc134e1b0000000h
                    DQ 3fc1dcd190000000h
                    DQ 3fc2842940000000h
                    DQ 3fc32ae9e0000000h
                    DQ 3fc3d11460000000h
                    DQ 3fc476a9f0000000h
                    DQ 3fc51bab90000000h
                    DQ 3fc5c01a30000000h
                    DQ 3fc663f6f0000000h
                    DQ 3fc70742d0000000h
                    DQ 3fc7a9fec0000000h
                    DQ 3fc84c2bd0000000h
                    DQ 3fc8edcae0000000h
                    DQ 3fc98edd00000000h
                    DQ 3fca2f6320000000h
                    DQ 3fcacf5e20000000h
                    DQ 3fcb6ecf10000000h
                    DQ 3fcc0db6c0000000h
                    DQ 3fccac1630000000h
                    DQ 3fcd49ee40000000h
                    DQ 3fcde73fe0000000h
                    DQ 3fce840be0000000h
                    DQ 3fcf205330000000h
                    DQ 3fcfbc16b0000000h
                    DQ 3fd02baba0000000h
                    DQ 3fd0790ad0000000h
                    DQ 3fd0c62970000000h
                    DQ 3fd11307d0000000h
                    DQ 3fd15fa670000000h
                    DQ 3fd1ac05b0000000h
                    DQ 3fd1f825f0000000h
                    DQ 3fd24407a0000000h
                    DQ 3fd28fab30000000h
                    DQ 3fd2db10f0000000h
                    DQ 3fd3263960000000h
                    DQ 3fd37124c0000000h
                    DQ 3fd3bbd3a0000000h
                    DQ 3fd4064630000000h
                    DQ 3fd4507cf0000000h
                    DQ 3fd49a7840000000h
                    DQ 3fd4e43880000000h
                    DQ 3fd52dbdf0000000h
                    DQ 3fd5770910000000h
                    DQ 3fd5c01a30000000h
                    DQ 3fd608f1b0000000h
                    DQ 3fd6518fe0000000h
                    DQ 3fd699f520000000h
                    DQ 3fd6e221c0000000h
                    DQ 3fd72a1630000000h
                    DQ 3fd771d2b0000000h
                    DQ 3fd7b957a0000000h
                    DQ 3fd800a560000000h
                    DQ 3fd847bc30000000h
                    DQ 3fd88e9c70000000h
                    DQ 3fd8d54670000000h
                    DQ 3fd91bba80000000h
                    DQ 3fd961f900000000h
                    DQ 3fd9a80230000000h
                    DQ 3fd9edd670000000h
                    DQ 3fda337600000000h
                    DQ 3fda78e140000000h
                    DQ 3fdabe1870000000h
                    DQ 3fdb031be0000000h
                    DQ 3fdb47ebf0000000h
                    DQ 3fdb8c88d0000000h
                    DQ 3fdbd0f2e0000000h
                    DQ 3fdc152a60000000h
                    DQ 3fdc592fa0000000h
                    DQ 3fdc9d02f0000000h
                    DQ 3fdce0a490000000h
                    DQ 3fdd2414c0000000h
                    DQ 3fdd6753e0000000h
                    DQ 3fddaa6220000000h
                    DQ 3fdded3fd0000000h
                    DQ 3fde2fed30000000h
                    DQ 3fde726aa0000000h
                    DQ 3fdeb4b840000000h
                    DQ 3fdef6d670000000h
                    DQ 3fdf38c560000000h
                    DQ 3fdf7a8560000000h
                    DQ 3fdfbc16b0000000h
                    DQ 3fdffd7990000000h
                    DQ 3fe01f5720000000h
                    DQ 3fe03fda80000000h
                    DQ 3fe0604710000000h
                    DQ 3fe0809cf0000000h
                    DQ 3fe0a0dc30000000h
                    DQ 3fe0c10500000000h
                    DQ 3fe0e11770000000h
                    DQ 3fe10113b0000000h
                    DQ 3fe120f9d0000000h
                    DQ 3fe140c9f0000000h
                    DQ 3fe1608440000000h
                    DQ 3fe18028c0000000h
                    DQ 3fe19fb7b0000000h
                    DQ 3fe1bf3110000000h
                    DQ 3fe1de9510000000h
                    DQ 3fe1fde3d0000000h
                    DQ 3fe21d1d50000000h
                    DQ 3fe23c41d0000000h
                    DQ 3fe25b5150000000h
                    DQ 3fe27a4c00000000h
                    DQ 3fe29931f0000000h
                    DQ 3fe2b80340000000h
                    DQ 3fe2d6c010000000h
                    DQ 3fe2f56870000000h
                    DQ 3fe313fc80000000h
                    DQ 3fe3327c60000000h
                    DQ 3fe350e830000000h
                    DQ 3fe36f3ff0000000h
                    DQ 3fe38d83e0000000h
                    DQ 3fe3abb3f0000000h
                    DQ 3fe3c9d060000000h
                    DQ 3fe3e7d930000000h
                    DQ 3fe405ce80000000h
                    DQ 3fe423b070000000h
                    DQ 3fe4417f20000000h
                    DQ 3fe45f3a90000000h
                    DQ 3fe47ce2f0000000h
                    DQ 3fe49a7840000000h
                    DQ 3fe4b7fab0000000h
                    DQ 3fe4d56a50000000h
                    DQ 3fe4f2c740000000h
                    DQ 3fe5101180000000h
                    DQ 3fe52d4940000000h
                    DQ 3fe54a6e80000000h
                    DQ 3fe5678170000000h
                    DQ 3fe5848220000000h
                    DQ 3fe5a170a0000000h
                    DQ 3fe5be4d00000000h
                    DQ 3fe5db1770000000h
                    DQ 3fe5f7cff0000000h
                    DQ 3fe61476a0000000h
                    DQ 3fe6310b80000000h
                    DQ 3fe64d8ed0000000h
                    DQ 3fe66a0080000000h
                    DQ 3fe68660c0000000h
                    DQ 3fe6a2af90000000h
                    DQ 3fe6beed20000000h
                    DQ 3fe6db1960000000h
                    DQ 3fe6f73480000000h
                    DQ 3fe7133e90000000h
                    DQ 3fe72f37a0000000h
                    DQ 3fe74b1fd0000000h
                    DQ 3fe766f720000000h
                    DQ 3fe782bdb0000000h
                    DQ 3fe79e73a0000000h
                    DQ 3fe7ba18f0000000h
                    DQ 3fe7d5adc0000000h
                    DQ 3fe7f13220000000h
                    DQ 3fe80ca620000000h
                    DQ 3fe82809d0000000h
                    DQ 3fe8435d50000000h
                    DQ 3fe85ea0b0000000h
                    DQ 3fe879d3f0000000h
                    DQ 3fe894f740000000h
                    DQ 3fe8b00aa0000000h
                    DQ 3fe8cb0e30000000h
                    DQ 3fe8e60200000000h
                    DQ 3fe900e610000000h
                    DQ 3fe91bba80000000h
                    DQ 3fe9367f60000000h
                    DQ 3fe95134d0000000h
                    DQ 3fe96bdad0000000h
                    DQ 3fe9867170000000h
                    DQ 3fe9a0f8d0000000h
                    DQ 3fe9bb70f0000000h
                    DQ 3fe9d5d9f0000000h
                    DQ 3fe9f033e0000000h
                    DQ 3fea0a7ed0000000h
                    DQ 3fea24bad0000000h
                    DQ 3fea3ee7f0000000h
                    DQ 3fea590640000000h
                    DQ 3fea7315d0000000h
                    DQ 3fea8d16b0000000h
                    DQ 3feaa708f0000000h
                    DQ 3feac0eca0000000h
                    DQ 3feadac1e0000000h
                    DQ 3feaf488b0000000h
                    DQ 3feb0e4120000000h
                    DQ 3feb27eb40000000h
                    DQ 3feb418730000000h
                    DQ 3feb5b14f0000000h
                    DQ 3feb749480000000h
                    DQ 3feb8e0620000000h
                    DQ 3feba769b0000000h
                    DQ 3febc0bf50000000h
                    DQ 3febda0710000000h
                    DQ 3febf34110000000h
                    DQ 3fec0c6d40000000h
                    DQ 3fec258bc0000000h
                    DQ 3fec3e9ca0000000h
                    DQ 3fec579fe0000000h
                    DQ 3fec7095a0000000h
                    DQ 3fec897df0000000h
                    DQ 3feca258d0000000h
                    DQ 3fecbb2660000000h
                    DQ 3fecd3e6a0000000h
                    DQ 3fecec9990000000h
                    DQ 3fed053f60000000h
                    DQ 3fed1dd810000000h
                    DQ 3fed3663b0000000h
                    DQ 3fed4ee240000000h
                    DQ 3fed6753e0000000h
                    DQ 3fed7fb890000000h
                    DQ 3fed981060000000h
                    DQ 3fedb05b60000000h
                    DQ 3fedc899a0000000h
                    DQ 3fede0cb30000000h
                    DQ 3fedf8f020000000h
                    DQ 3fee110860000000h
                    DQ 3fee291420000000h
                    DQ 3fee411360000000h
                    DQ 3fee590630000000h
                    DQ 3fee70eca0000000h
                    DQ 3fee88c6b0000000h
                    DQ 3feea09470000000h
                    DQ 3feeb855f0000000h
                    DQ 3feed00b40000000h
                    DQ 3feee7b470000000h
                    DQ 3feeff5180000000h
                    DQ 3fef16e280000000h
                    DQ 3fef2e6780000000h
                    DQ 3fef45e080000000h
                    DQ 3fef5d4da0000000h
                    DQ 3fef74aef0000000h
                    DQ 3fef8c0460000000h
                    DQ 3fefa34e10000000h
                    DQ 3fefba8c00000000h
                    DQ 3fefd1be40000000h
                    DQ 3fefe8e4f0000000h
                    DQ 3ff0000000000000h

ALIGN 16
__log_256_tail:
                    DQ 0000000000000000h
                    DQ 3deaf558ee95b37ah
                    DQ 3debbc2145fe38deh
                    DQ 3dfea5ec312ed069h
                    DQ 3df70b48a629b89fh
                    DQ 3e050a1f0cccdd01h
                    DQ 3e044cd04bb60514h
                    DQ 3e01a16898809d2dh
                    DQ 3e063bf61cc4d81bh
                    DQ 3dfa4a8ca305071dh
                    DQ 3e121556bde9f1f0h
                    DQ 3df9929cfd0e6835h
                    DQ 3e2f453f35679ee9h
                    DQ 3e2c26b47913459eh
                    DQ 3e2a4fe385b009a2h
                    DQ 3e180ceedb53cb4dh
                    DQ 3e2592262cf998a7h
                    DQ 3e1ae28a04f106b8h
                    DQ 3e2c8c66b55ce464h
                    DQ 3e2e690927d688b0h
                    DQ 3de5b5774c7658b4h
                    DQ 3e0adc16d26859c7h
                    DQ 3df7fa5b21cbdb5dh
                    DQ 3e2e160149209a68h
                    DQ 3e39b4f3c72c4f78h
                    DQ 3e222418b7fcd690h
                    DQ 3e2d54aded7a9150h
                    DQ 3e360f4c7f1aed15h
                    DQ 3e13c570d0fa8f96h
                    DQ 3e3b3514c7e0166eh
                    DQ 3e3307ee9a6271d2h
                    DQ 3dee9722922c0226h
                    DQ 3e33f7ad0f3f4016h
                    DQ 3e3592262cf998a7h
                    DQ 3e23bc09fca70073h
                    DQ 3e2f41777bc5f936h
                    DQ 3dd781d97ee91247h
                    DQ 3e306a56d76b9a84h
                    DQ 3e2df9c37c0beb3ah
                    DQ 3e1905c35651c429h
                    DQ 3e3b69d927dfc23dh
                    DQ 3e2d7e57a5afb633h
                    DQ 3e3bb29bdc81c4dbh
                    DQ 3e38ee1b912d8994h
                    DQ 3e3864b2df91e96ah
                    DQ 3e1d8a40770df213h
                    DQ 3e2d39a9331f27cfh
                    DQ 3e32411e4e8eea54h
                    DQ 3e3204d0144751b3h
                    DQ 3e2268331dd8bd0bh
                    DQ 3e47606012de0634h
                    DQ 3e3550aa3a93ec7eh
                    DQ 3e45a616eb9612e0h
                    DQ 3e3aec23fd65f8e1h
                    DQ 3e248f838294639ch
                    DQ 3e3b62384cafa1a3h
                    DQ 3e461c0e73048b72h
                    DQ 3e36cc9a0d8c0e85h
                    DQ 3e489b355ede26f4h
                    DQ 3e2b5941acd71f1eh
                    DQ 3e4d499bd9b32266h
                    DQ 3e043b9f52b061bah
                    DQ 3e46360892eb65e6h
                    DQ 3e4dba9f8729ab41h
                    DQ 3e479a3715fc9257h
                    DQ 3e0d1f6d3f77ae38h
                    DQ 3e48992d66fb9ec1h
                    DQ 3e4666f195620f03h
                    DQ 3e43f7ad0f3f4016h
                    DQ 3e30a522b65bc039h
                    DQ 3e319dee9b9489e3h
                    DQ 3e323352e1a31521h
                    DQ 3e4b3a19bcaf1aa4h
                    DQ 3e3f2f060a50d366h
                    DQ 3e44fdf677c8dfd9h
                    DQ 3e48a35588aec6dfh
                    DQ 3e28b0e2a19575b0h
                    DQ 3e2ec30c6e3e04a7h
                    DQ 3e2705912d25b325h
                    DQ 3e2dae1b8d59e849h
                    DQ 3e423e2e1169656ah
                    DQ 3e349d026e33d675h
                    DQ 3e423c465e6976dah
                    DQ 3e366c977e236c73h
                    DQ 3e44fec0a13af881h
                    DQ 3e3bdefbd14a0816h
                    DQ 3e42fe3e91c348e4h
                    DQ 3e4fc0c868ccc02dh
                    DQ 3e3ce20a829051bbh
                    DQ 3e47f10cf32e6bbah
                    DQ 3e43cf2061568859h
                    DQ 3e484995cb804b94h
                    DQ 3e4a52b6acfcfdcah
                    DQ 3e3b291ecf4dff1eh
                    DQ 3e21d2c3e64ae851h
                    DQ 3e4017e4faa42b7dh
                    DQ 3de975077f1f5f0ch
                    DQ 3e20327dc8093a52h
                    DQ 3e3108d9313aec65h
                    DQ 3e4a12e5301be44ah
                    DQ 3e1e754d20c519e1h
                    DQ 3e3f456f394f9727h
                    DQ 3e29471103e8f00dh
                    DQ 3e3ef3150343f8dfh
                    DQ 3e41960d9d9c3263h
                    DQ 3e4204d0144751b3h
                    DQ 3e4507ff357398feh
                    DQ 3e4dc9937fc8cafdh
                    DQ 3e572f32fe672868h
                    DQ 3e53e49d647d323eh
                    DQ 3e33fb81ea92d9e0h
                    DQ 3e43e387ef003635h
                    DQ 3e1ac754cb104aeah
                    DQ 3e4535f0444ebaafh
                    DQ 3e253c8ea7b1cddah
                    DQ 3e3cf0c0396a568bh
                    DQ 3e5543ca873c2b4ah
                    DQ 3e425780181e2b37h
                    DQ 3e5ee52ed49d71d2h
                    DQ 3e51e64842e2c386h
                    DQ 3e5d2ba01bc76a27h
                    DQ 3e5b39774c30f499h
                    DQ 3e38740932120aeah
                    DQ 3e576dab3462a1e8h
                    DQ 3e409c9f20203b31h
                    DQ 3e516e7a08ad0d1ah
                    DQ 3e46172fe015e13bh
                    DQ 3e49e4558147cf67h
                    DQ 3e4cfdeb43cfd005h
                    DQ 3e3a809c03254a71h
                    DQ 3e47acfc98509e33h
                    DQ 3e54366de473e474h
                    DQ 3e5569394d90d724h
                    DQ 3e32b83ec743664ch
                    DQ 3e56db22c4808ee5h
                    DQ 3df7ae84940df0e1h
                    DQ 3e554042cd999564h
                    DQ 3e4242b8488b3056h
                    DQ 3e4e7dc059ab8a9eh
                    DQ 3e5a71e977d7da5fh
                    DQ 3e5d30d552ce0ec3h
                    DQ 3e43208592b6c6b7h
                    DQ 3e51440e7149afffh
                    DQ 3e36812c371a1c87h
                    DQ 3e579a3715fc9257h
                    DQ 3e57c92f2af8b0cah
                    DQ 3e56679d8894dbdfh
                    DQ 3e2a9f33e77507f0h
                    DQ 3e4c22a3e377a524h
                    DQ 3e3723c84a77a4dch
                    DQ 3e594a871b636194h
                    DQ 3e570d6058f62f4dh
                    DQ 3e4a6274cf0e362fh
                    DQ 3e42fe3570af1a0bh
                    DQ 3e596a286955d67eh
                    DQ 3e442104f127091eh
                    DQ 3e407826bae32c6bh
                    DQ 3df8d8844ce77237h
                    DQ 3e5eaa609080d4b4h
                    DQ 3e4dc66fbe61efc4h
                    DQ 3e5c8f11979a5db6h
                    DQ 3e52dedf0e6f1770h
                    DQ 3e5cb41e1410132ah
                    DQ 3e32866d705c553dh
                    DQ 3e54ec3293b2fbe0h
                    DQ 3e578b8c2f4d0fe1h
                    DQ 3e562ad8f7ca2cffh
                    DQ 3e5a298b5f973a2ch
                    DQ 3e49381d4f1b95e0h
                    DQ 3e564c7bdb9bc56ch
                    DQ 3e5fbb4caef790fch
                    DQ 3e51200c3f899927h
                    DQ 3e526a05c813d56eh
                    DQ 3e4681e2910108eeh
                    DQ 3e282cf15d12ecd7h
                    DQ 3e0a537e32446892h
                    DQ 3e46f9c1cb6f7010h
                    DQ 3e4328ddcedf39d8h
                    DQ 3e164f64c210df9dh
                    DQ 3e58f676e17cc811h
                    DQ 3e560ddf1680dd45h
                    DQ 3e5e2da951c2d91bh
                    DQ 3e5696777b66d115h
                    DQ 3e311eb3043f5601h
                    DQ 3e48000b33f90fd4h
                    DQ 3e523e2e1169656ah
                    DQ 3e5b41565d3990cbh
                    DQ 3e46138b8d9d31e6h
                    DQ 3e3565afaf7f6248h
                    DQ 3e4b68e0ba153594h
                    DQ 3e3d87027ef4ab9ah
                    DQ 3e556b9c99085939h
                    DQ 3e5aa02166cccab2h
                    DQ 3e5991d2aca399a1h
                    DQ 3e54982259cc625dh
                    DQ 3e4b9feddaab9820h
                    DQ 3e3c70c0f683cc68h
                    DQ 3e213156425e67e5h
                    DQ 3df79063deab051fh
                    DQ 3e27e2744b2b8ca5h
                    DQ 3e4600534df378dfh
                    DQ 3e59322676507a79h
                    DQ 3e4c4720cb4558b5h
                    DQ 3e445e4b56add63ah
                    DQ 3e4af321af5e9bb5h
                    DQ 3e57f1e1148dad64h
                    DQ 3e42a4022f65e2e6h
                    DQ 3e11f2ccbcd0d3cch
                    DQ 3e5eaa65b49696e2h
                    DQ 3e110e6123a74764h
                    DQ 3e3cf24b2077c3f6h
                    DQ 3e4fc8d8164754dah
                    DQ 3e598cfcdb6a2dbch
                    DQ 3e24464a6bcdf47bh
                    DQ 3e41f1774d8b66a6h
                    DQ 3e459920a2adf6fah
                    DQ 3e370d02a99b4c5ah
                    DQ 3e576b6cafa2532dh
                    DQ 3e5d23c38ec17936h
                    DQ 3e541b6b1b0e66c4h
                    DQ 3e5952662c6bfdc7h
                    DQ 3e4333f3d6bb35ech
                    DQ 3e195120d8486e92h
                    DQ 3e5db8a405fac56eh
                    DQ 3e5a4c112ce6312eh
                    DQ 3e536987e1924e45h
                    DQ 3e33f98ea94bc1bdh
                    DQ 3e459718aacb6ec7h
                    DQ 3df975077f1f5f0ch
                    DQ 3e13654d88f20500h
                    DQ 3e40f598530f101bh
                    DQ 3e5145f6c94f7fd7h
                    DQ 3e567fead8bcce75h
                    DQ 3e52e67148cd0a7bh
                    DQ 3e10d5e5897de907h
                    DQ 3e5b5ee92c53d919h
                    DQ 3e5c1c02803f7554h
                    DQ 3e5d5caa7a35c9f7h
                    DQ 3e5910459cac3223h
                    DQ 3e41fbe1bb98afdfh
                    DQ 3e3b135395510d1eh
                    DQ 3e47b8f0e7b8e757h
                    DQ 3e519511f61a96b8h
                    DQ 3e5117d846ae1f8eh
                    DQ 3e2b3a9507d6dc1fh
                    DQ 3e15fa7c78c9e676h
                    DQ 3e2db76303b21928h
                    DQ 3e27eb8450ac22edh
                    DQ 3e579e0caa9c9ab7h
                    DQ 3e59de6d7cba1bbeh
                    DQ 3e1df5f5baf436cbh
                    DQ 3e3e746344728dbfh
                    DQ 3e277c23362928b9h
                    DQ 3e4715137cfeba9fh
                    DQ 3e58fe55f2856443h
                    DQ 3e25bd1a025d9e24h
                    DQ 0000000000000000h

ALIGN 16
__log_F_inv:
                    DQ 4000000000000000h
                    DQ 3fffe01fe01fe020h
                    DQ 3fffc07f01fc07f0h
                    DQ 3fffa11caa01fa12h
                    DQ 3fff81f81f81f820h
                    DQ 3fff6310aca0dbb5h
                    DQ 3fff44659e4a4271h
                    DQ 3fff25f644230ab5h
                    DQ 3fff07c1f07c1f08h
                    DQ 3ffee9c7f8458e02h
                    DQ 3ffecc07b301ecc0h
                    DQ 3ffeae807aba01ebh
                    DQ 3ffe9131abf0b767h
                    DQ 3ffe741aa59750e4h
                    DQ 3ffe573ac901e574h
                    DQ 3ffe3a9179dc1a73h
                    DQ 3ffe1e1e1e1e1e1eh
                    DQ 3ffe01e01e01e01eh
                    DQ 3ffde5d6e3f8868ah
                    DQ 3ffdca01dca01dcah
                    DQ 3ffdae6076b981dbh
                    DQ 3ffd92f2231e7f8ah
                    DQ 3ffd77b654b82c34h
                    DQ 3ffd5cac807572b2h
                    DQ 3ffd41d41d41d41dh
                    DQ 3ffd272ca3fc5b1ah
                    DQ 3ffd0cb58f6ec074h
                    DQ 3ffcf26e5c44bfc6h
                    DQ 3ffcd85689039b0bh
                    DQ 3ffcbe6d9601cbe7h
                    DQ 3ffca4b3055ee191h
                    DQ 3ffc8b265afb8a42h
                    DQ 3ffc71c71c71c71ch
                    DQ 3ffc5894d10d4986h
                    DQ 3ffc3f8f01c3f8f0h
                    DQ 3ffc26b5392ea01ch
                    DQ 3ffc0e070381c0e0h
                    DQ 3ffbf583ee868d8bh
                    DQ 3ffbdd2b899406f7h
                    DQ 3ffbc4fd65883e7bh
                    DQ 3ffbacf914c1bad0h
                    DQ 3ffb951e2b18ff23h
                    DQ 3ffb7d6c3dda338bh
                    DQ 3ffb65e2e3beee05h
                    DQ 3ffb4e81b4e81b4fh
                    DQ 3ffb37484ad806ceh
                    DQ 3ffb2036406c80d9h
                    DQ 3ffb094b31d922a4h
                    DQ 3ffaf286bca1af28h
                    DQ 3ffadbe87f94905eh
                    DQ 3ffac5701ac5701bh
                    DQ 3ffaaf1d2f87ebfdh
                    DQ 3ffa98ef606a63beh
                    DQ 3ffa82e65130e159h
                    DQ 3ffa6d01a6d01a6dh
                    DQ 3ffa574107688a4ah
                    DQ 3ffa41a41a41a41ah
                    DQ 3ffa2c2a87c51ca0h
                    DQ 3ffa16d3f97a4b02h
                    DQ 3ffa01a01a01a01ah
                    DQ 3ff9ec8e951033d9h
                    DQ 3ff9d79f176b682dh
                    DQ 3ff9c2d14ee4a102h
                    DQ 3ff9ae24ea5510dah
                    DQ 3ff999999999999ah
                    DQ 3ff9852f0d8ec0ffh
                    DQ 3ff970e4f80cb872h
                    DQ 3ff95cbb0be377aeh
                    DQ 3ff948b0fcd6e9e0h
                    DQ 3ff934c67f9b2ce6h
                    DQ 3ff920fb49d0e229h
                    DQ 3ff90d4f120190d5h
                    DQ 3ff8f9c18f9c18fah
                    DQ 3ff8e6527af1373fh
                    DQ 3ff8d3018d3018d3h
                    DQ 3ff8bfce8062ff3ah
                    DQ 3ff8acb90f6bf3aah
                    DQ 3ff899c0f601899ch
                    DQ 3ff886e5f0abb04ah
                    DQ 3ff87427bcc092b9h
                    DQ 3ff8618618618618h
                    DQ 3ff84f00c2780614h
                    DQ 3ff83c977ab2beddh
                    DQ 3ff82a4a0182a4a0h
                    DQ 3ff8181818181818h
                    DQ 3ff8060180601806h
                    DQ 3ff7f405fd017f40h
                    DQ 3ff7e225515a4f1dh
                    DQ 3ff7d05f417d05f4h
                    DQ 3ff7beb3922e017ch
                    DQ 3ff7ad2208e0ecc3h
                    DQ 3ff79baa6bb6398bh
                    DQ 3ff78a4c8178a4c8h
                    DQ 3ff77908119ac60dh
                    DQ 3ff767dce434a9b1h
                    DQ 3ff756cac201756dh
                    DQ 3ff745d1745d1746h
                    DQ 3ff734f0c541fe8dh
                    DQ 3ff724287f46debch
                    DQ 3ff713786d9c7c09h
                    DQ 3ff702e05c0b8170h
                    DQ 3ff6f26016f26017h
                    DQ 3ff6e1f76b4337c7h
                    DQ 3ff6d1a62681c861h
                    DQ 3ff6c16c16c16c17h
                    DQ 3ff6b1490aa31a3dh
                    DQ 3ff6a13cd1537290h
                    DQ 3ff691473a88d0c0h
                    DQ 3ff6816816816817h
                    DQ 3ff6719f3601671ah
                    DQ 3ff661ec6a5122f9h
                    DQ 3ff6524f853b4aa3h
                    DQ 3ff642c8590b2164h
                    DQ 3ff63356b88ac0deh
                    DQ 3ff623fa77016240h
                    DQ 3ff614b36831ae94h
                    DQ 3ff6058160581606h
                    DQ 3ff5f66434292dfch
                    DQ 3ff5e75bb8d015e7h
                    DQ 3ff5d867c3ece2a5h
                    DQ 3ff5c9882b931057h
                    DQ 3ff5babcc647fa91h
                    DQ 3ff5ac056b015ac0h
                    DQ 3ff59d61f123ccaah
                    DQ 3ff58ed2308158edh
                    DQ 3ff5805601580560h
                    DQ 3ff571ed3c506b3ah
                    DQ 3ff56397ba7c52e2h
                    DQ 3ff5555555555555h
                    DQ 3ff54725e6bb82feh
                    DQ 3ff5390948f40febh
                    DQ 3ff52aff56a8054bh
                    DQ 3ff51d07eae2f815h
                    DQ 3ff50f22e111c4c5h
                    DQ 3ff5015015015015h
                    DQ 3ff4f38f62dd4c9bh
                    DQ 3ff4e5e0a72f0539h
                    DQ 3ff4d843bedc2c4ch
                    DQ 3ff4cab88725af6eh
                    DQ 3ff4bd3edda68fe1h
                    DQ 3ff4afd6a052bf5bh
                    DQ 3ff4a27fad76014ah
                    DQ 3ff49539e3b2d067h
                    DQ 3ff4880522014880h
                    DQ 3ff47ae147ae147bh
                    DQ 3ff46dce34596066h
                    DQ 3ff460cbc7f5cf9ah
                    DQ 3ff453d9e2c776cah
                    DQ 3ff446f86562d9fbh
                    DQ 3ff43a2730abee4dh
                    DQ 3ff42d6625d51f87h
                    DQ 3ff420b5265e5951h
                    DQ 3ff4141414141414h
                    DQ 3ff40782d10e6566h
                    DQ 3ff3fb013fb013fbh
                    DQ 3ff3ee8f42a5af07h
                    DQ 3ff3e22cbce4a902h
                    DQ 3ff3d5d991aa75c6h
                    DQ 3ff3c995a47babe7h
                    DQ 3ff3bd60d9232955h
                    DQ 3ff3b13b13b13b14h
                    DQ 3ff3a524387ac822h
                    DQ 3ff3991c2c187f63h
                    DQ 3ff38d22d366088eh
                    DQ 3ff3813813813814h
                    DQ 3ff3755bd1c945eeh
                    DQ 3ff3698df3de0748h
                    DQ 3ff35dce5f9f2af8h
                    DQ 3ff3521cfb2b78c1h
                    DQ 3ff34679ace01346h
                    DQ 3ff33ae45b57bcb2h
                    DQ 3ff32f5ced6a1dfah
                    DQ 3ff323e34a2b10bfh
                    DQ 3ff3187758e9ebb6h
                    DQ 3ff30d190130d190h
                    DQ 3ff301c82ac40260h
                    DQ 3ff2f684bda12f68h
                    DQ 3ff2eb4ea1fed14bh
                    DQ 3ff2e025c04b8097h
                    DQ 3ff2d50a012d50a0h
                    DQ 3ff2c9fb4d812ca0h
                    DQ 3ff2bef98e5a3711h
                    DQ 3ff2b404ad012b40h
                    DQ 3ff2a91c92f3c105h
                    DQ 3ff29e4129e4129eh
                    DQ 3ff293725bb804a5h
                    DQ 3ff288b01288b013h
                    DQ 3ff27dfa38a1ce4dh
                    DQ 3ff27350b8812735h
                    DQ 3ff268b37cd60127h
                    DQ 3ff25e22708092f1h
                    DQ 3ff2539d7e9177b2h
                    DQ 3ff2492492492492h
                    DQ 3ff23eb79717605bh
                    DQ 3ff23456789abcdfh
                    DQ 3ff22a0122a0122ah
                    DQ 3ff21fb78121fb78h
                    DQ 3ff21579804855e6h
                    DQ 3ff20b470c67c0d9h
                    DQ 3ff2012012012012h
                    DQ 3ff1f7047dc11f70h
                    DQ 3ff1ecf43c7fb84ch
                    DQ 3ff1e2ef3b3fb874h
                    DQ 3ff1d8f5672e4abdh
                    DQ 3ff1cf06ada2811dh
                    DQ 3ff1c522fc1ce059h
                    DQ 3ff1bb4a4046ed29h
                    DQ 3ff1b17c67f2bae3h
                    DQ 3ff1a7b9611a7b96h
                    DQ 3ff19e0119e0119eh
                    DQ 3ff19453808ca29ch
                    DQ 3ff18ab083902bdbh
                    DQ 3ff1811811811812h
                    DQ 3ff1778a191bd684h
                    DQ 3ff16e0689427379h
                    DQ 3ff1648d50fc3201h
                    DQ 3ff15b1e5f75270dh
                    DQ 3ff151b9a3fdd5c9h
                    DQ 3ff1485f0e0acd3bh
                    DQ 3ff13f0e8d344724h
                    DQ 3ff135c81135c811h
                    DQ 3ff12c8b89edc0ach
                    DQ 3ff12358e75d3033h
                    DQ 3ff11a3019a74826h
                    DQ 3ff1111111111111h
                    DQ 3ff107fbbe011080h
                    DQ 3ff0fef010fef011h
                    DQ 3ff0f5edfab325a2h
                    DQ 3ff0ecf56be69c90h
                    DQ 3ff0e40655826011h
                    DQ 3ff0db20a88f4696h
                    DQ 3ff0d24456359e3ah
                    DQ 3ff0c9714fbcda3bh
                    DQ 3ff0c0a7868b4171h
                    DQ 3ff0b7e6ec259dc8h
                    DQ 3ff0af2f722eecb5h
                    DQ 3ff0a6810a6810a7h
                    DQ 3ff09ddba6af8360h
                    DQ 3ff0953f39010954h
                    DQ 3ff08cabb37565e2h
                    DQ 3ff0842108421084h
                    DQ 3ff07b9f29b8eae2h
                    DQ 3ff073260a47f7c6h
                    DQ 3ff06ab59c7912fbh
                    DQ 3ff0624dd2f1a9fch
                    DQ 3ff059eea0727586h
                    DQ 3ff05197f7d73404h
                    DQ 3ff04949cc1664c5h
                    DQ 3ff0410410410410h
                    DQ 3ff038c6b78247fch
                    DQ 3ff03091b51f5e1ah
                    DQ 3ff02864fc7729e9h
                    DQ 3ff0204081020408h
                    DQ 3ff0182436517a37h
                    DQ 3ff0101010101010h
                    DQ 3ff0080402010080h
                    DQ 3ff0000000000000h
                    DQ 0000000000000000h

data ENDS

END
