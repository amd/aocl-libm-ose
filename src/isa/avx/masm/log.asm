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
; log.asm
;
; An implementation of the log libm function.
;
; Prototype:
;
;     double log(double x);
;

;
;   Algorithm:
;
;   Based on:
;   Ping-Tak Peter Tang
;   "Table-driven implementation of the logarithm function in IEEE
;   floating-point arithmetic"
;   ACM Transactions on Mathematical Software (TOMS)
;   Volume 16, Issue 4 (December 1990)
;
;
;   x very close to 1.0 is handled differently, for x everywhere else
;   a brief explanation is given below
;
;   x = (2^m)*A
;   x = (2^m)*(G+g) with (1 <= G < 2) and (g <= 2^(-9))
;   x = (2^m)*2*(G/2+g/2)
;   x = (2^m)*2*(F+f) with (0.5 <= F < 1) and (f <= 2^(-10))
;   
;   Y = (2^(-1))*(2^(-m))*(2^m)*A
;   Now, range of Y is: 0.5 <= Y < 1
;
;   F = 0x100 + (first 8 mantissa bits) + (9th mantissa bit)
;   Now, range of F is: 256 <= F <= 512
;   F = F / 512
;   Now, range of F is: 0.5 <= F <= 1
;
;   f = -(Y-F), with (f <= 2^(-10))
;
;   log(x) = m*log(2) + log(2) + log(F-f)
;   log(x) = m*log(2) + log(2) + log(F) + log(1-(f/F))
;   log(x) = m*log(2) + log(2*F) + log(1-r)
;
;   r = (f/F), with (r <= 2^(-9))
;   r = f*(1/F) with (1/F) precomputed to avoid division
;
;   log(x) = m*log(2) + log(G) - poly
;
;   log(G) is precomputed
;   poly = (r + (r^2)/2 + (r^3)/3 + (r^4)/4) + (r^5)/5) + (r^6)/6))
;
;   log(2) and log(G) need to be maintained in extra precision
;   to avoid losing precision in the calculations
;

include fm.inc

ALM_PROTO_BAS64 log

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
    lea         r9, __log_F_inv

    ; check for negative numbers or zero
    xorpd       xmm5, xmm5
    comisd      xmm0, xmm5
    jbe         __x_is_zero_or_neg

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

    ; m*log(2) + log(G) - poly
    movsd       xmm5, QWORD PTR __real_log2_tail
    mulsd       xmm5, xmm6
    subsd       xmm5, xmm1

    movsd       xmm0, QWORD PTR [r9+rax*8]
    lea         rdx, QWORD PTR __log_256_tail
    movsd       xmm2, QWORD PTR [rdx+rax*8]
    addsd       xmm2, xmm5

    movsd       xmm4, QWORD PTR __real_log2_lead
    mulsd       xmm4, xmm6
    addsd       xmm0, xmm4

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

    addsd       xmm4, __real_ca1
    addsd       xmm5, __real_ca3

    mulsd       xmm2, xmm1 ; u^3
    mulsd       xmm4, xmm2

    mulsd       xmm2, xmm2
    mulsd       xmm2, xmm1 ; u^7
    mulsd       xmm5, xmm2

    addsd       xmm4, xmm5
    subsd       xmm4, xmm6
    addsd       xmm0, xmm4
 
    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
    add         rsp, stack_size
    ret

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

__real_ninf     DQ 0fff0000000000000h   ; -inf
                DQ 0000000000000000h
__real_inf      DQ 7ff0000000000000h    ; +inf
                DQ 0000000000000000h
__real_neg_qnan DQ 0fff8000000000000h   ; neg qNaN
                DQ 0000000000000000h
__real_qnanbit  DQ 0008000000000000h
                DQ 0000000000000000h
__real_mant     DQ 000FFFFFFFFFFFFFh    ; mantissa bits
                DQ 0000000000000000h
__mask_1023     DQ 00000000000003ffh
                DQ 0000000000000000h
__mask_001      DQ 0000000000000001h
                DQ 0000000000000000h

__mask_mant_all8    DQ 000ff00000000000h
                    DQ 0000000000000000h
__mask_mant9        DQ 0000080000000000h
                    DQ 0000000000000000h

__real_log2_lead    DQ 3fe62e42e0000000h ; log2_lead  6.93147122859954833984e-01
                    DQ 0000000000000000h
__real_log2_tail    DQ 3e6efa39ef35793ch ; log2_tail  5.76999904754328540596e-08
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

ALIGN 16
__log_256_lead:
                    DQ 0000000000000000h
                    DQ 3f6ff00aa0000000h
                    DQ 3f7fe02a60000000h
                    DQ 3f87dc4750000000h
                    DQ 3f8fc0a8b0000000h
                    DQ 3f93cea440000000h
                    DQ 3f97b91b00000000h
                    DQ 3f9b9fc020000000h
                    DQ 3f9f829b00000000h
                    DQ 3fa1b0d980000000h
                    DQ 3fa39e87b0000000h
                    DQ 3fa58a5ba0000000h
                    DQ 3fa77458f0000000h
                    DQ 3fa95c8300000000h
                    DQ 3fab42dd70000000h
                    DQ 3fad276b80000000h
                    DQ 3faf0a30c0000000h
                    DQ 3fb0759830000000h
                    DQ 3fb16536e0000000h
                    DQ 3fb253f620000000h
                    DQ 3fb341d790000000h
                    DQ 3fb42edcb0000000h
                    DQ 3fb51b0730000000h
                    DQ 3fb60658a0000000h
                    DQ 3fb6f0d280000000h
                    DQ 3fb7da7660000000h
                    DQ 3fb8c345d0000000h
                    DQ 3fb9ab4240000000h
                    DQ 3fba926d30000000h
                    DQ 3fbb78c820000000h
                    DQ 3fbc5e5480000000h
                    DQ 3fbd4313d0000000h
                    DQ 3fbe270760000000h
                    DQ 3fbf0a30c0000000h
                    DQ 3fbfec9130000000h
                    DQ 3fc0671510000000h
                    DQ 3fc0d77e70000000h
                    DQ 3fc1478580000000h
                    DQ 3fc1b72ad0000000h
                    DQ 3fc2266f10000000h
                    DQ 3fc29552f0000000h
                    DQ 3fc303d710000000h
                    DQ 3fc371fc20000000h
                    DQ 3fc3dfc2b0000000h
                    DQ 3fc44d2b60000000h
                    DQ 3fc4ba36f0000000h
                    DQ 3fc526e5e0000000h
                    DQ 3fc59338d0000000h
                    DQ 3fc5ff3070000000h
                    DQ 3fc66acd40000000h
                    DQ 3fc6d60fe0000000h
                    DQ 3fc740f8f0000000h
                    DQ 3fc7ab8900000000h
                    DQ 3fc815c0a0000000h
                    DQ 3fc87fa060000000h
                    DQ 3fc8e928d0000000h
                    DQ 3fc9525a90000000h
                    DQ 3fc9bb3620000000h
                    DQ 3fca23bc10000000h
                    DQ 3fca8becf0000000h
                    DQ 3fcaf3c940000000h
                    DQ 3fcb5b5190000000h
                    DQ 3fcbc28670000000h
                    DQ 3fcc296850000000h
                    DQ 3fcc8ff7c0000000h
                    DQ 3fccf63540000000h
                    DQ 3fcd5c2160000000h
                    DQ 3fcdc1bca0000000h
                    DQ 3fce270760000000h
                    DQ 3fce8c0250000000h
                    DQ 3fcef0adc0000000h
                    DQ 3fcf550a50000000h
                    DQ 3fcfb91860000000h
                    DQ 3fd00e6c40000000h
                    DQ 3fd0402590000000h
                    DQ 3fd071b850000000h
                    DQ 3fd0a324e0000000h
                    DQ 3fd0d46b50000000h
                    DQ 3fd1058bf0000000h
                    DQ 3fd1368700000000h
                    DQ 3fd1675ca0000000h
                    DQ 3fd1980d20000000h
                    DQ 3fd1c898c0000000h
                    DQ 3fd1f8ff90000000h
                    DQ 3fd22941f0000000h
                    DQ 3fd2596010000000h
                    DQ 3fd2895a10000000h
                    DQ 3fd2b93030000000h
                    DQ 3fd2e8e2b0000000h
                    DQ 3fd31871c0000000h
                    DQ 3fd347dd90000000h
                    DQ 3fd3772660000000h
                    DQ 3fd3a64c50000000h
                    DQ 3fd3d54fa0000000h
                    DQ 3fd4043080000000h
                    DQ 3fd432ef20000000h
                    DQ 3fd4618bc0000000h
                    DQ 3fd4900680000000h
                    DQ 3fd4be5f90000000h
                    DQ 3fd4ec9730000000h
                    DQ 3fd51aad80000000h
                    DQ 3fd548a2c0000000h
                    DQ 3fd5767710000000h
                    DQ 3fd5a42ab0000000h
                    DQ 3fd5d1bdb0000000h
                    DQ 3fd5ff3070000000h
                    DQ 3fd62c82f0000000h
                    DQ 3fd659b570000000h
                    DQ 3fd686c810000000h
                    DQ 3fd6b3bb20000000h
                    DQ 3fd6e08ea0000000h
                    DQ 3fd70d42e0000000h
                    DQ 3fd739d7f0000000h
                    DQ 3fd7664e10000000h
                    DQ 3fd792a550000000h
                    DQ 3fd7bede00000000h
                    DQ 3fd7eaf830000000h
                    DQ 3fd816f410000000h
                    DQ 3fd842d1d0000000h
                    DQ 3fd86e9190000000h
                    DQ 3fd89a3380000000h
                    DQ 3fd8c5b7c0000000h
                    DQ 3fd8f11e80000000h
                    DQ 3fd91c67e0000000h
                    DQ 3fd9479410000000h
                    DQ 3fd972a340000000h
                    DQ 3fd99d9580000000h
                    DQ 3fd9c86b00000000h
                    DQ 3fd9f323e0000000h
                    DQ 3fda1dc060000000h
                    DQ 3fda484090000000h
                    DQ 3fda72a490000000h
                    DQ 3fda9cec90000000h
                    DQ 3fdac718c0000000h
                    DQ 3fdaf12930000000h
                    DQ 3fdb1b1e00000000h
                    DQ 3fdb44f770000000h
                    DQ 3fdb6eb590000000h
                    DQ 3fdb985890000000h
                    DQ 3fdbc1e080000000h
                    DQ 3fdbeb4d90000000h
                    DQ 3fdc149ff0000000h
                    DQ 3fdc3dd7a0000000h
                    DQ 3fdc66f4e0000000h
                    DQ 3fdc8ff7c0000000h
                    DQ 3fdcb8e070000000h
                    DQ 3fdce1af00000000h
                    DQ 3fdd0a63a0000000h
                    DQ 3fdd32fe70000000h
                    DQ 3fdd5b7f90000000h
                    DQ 3fdd83e720000000h
                    DQ 3fddac3530000000h
                    DQ 3fddd46a00000000h
                    DQ 3fddfc8590000000h
                    DQ 3fde248810000000h
                    DQ 3fde4c71a0000000h
                    DQ 3fde744260000000h
                    DQ 3fde9bfa60000000h
                    DQ 3fdec399d0000000h
                    DQ 3fdeeb20c0000000h
                    DQ 3fdf128f50000000h
                    DQ 3fdf39e5b0000000h
                    DQ 3fdf6123f0000000h
                    DQ 3fdf884a30000000h
                    DQ 3fdfaf5880000000h
                    DQ 3fdfd64f20000000h
                    DQ 3fdffd2e00000000h
                    DQ 3fe011fab0000000h
                    DQ 3fe02552a0000000h
                    DQ 3fe0389ee0000000h
                    DQ 3fe04bdf90000000h
                    DQ 3fe05f14b0000000h
                    DQ 3fe0723e50000000h
                    DQ 3fe0855c80000000h
                    DQ 3fe0986f40000000h
                    DQ 3fe0ab76b0000000h
                    DQ 3fe0be72e0000000h
                    DQ 3fe0d163c0000000h
                    DQ 3fe0e44980000000h
                    DQ 3fe0f72410000000h
                    DQ 3fe109f390000000h
                    DQ 3fe11cb810000000h
                    DQ 3fe12f7190000000h
                    DQ 3fe1422020000000h
                    DQ 3fe154c3d0000000h
                    DQ 3fe1675ca0000000h
                    DQ 3fe179eab0000000h
                    DQ 3fe18c6e00000000h
                    DQ 3fe19ee6b0000000h
                    DQ 3fe1b154b0000000h
                    DQ 3fe1c3b810000000h
                    DQ 3fe1d610f0000000h
                    DQ 3fe1e85f50000000h
                    DQ 3fe1faa340000000h
                    DQ 3fe20cdcd0000000h
                    DQ 3fe21f0bf0000000h
                    DQ 3fe23130d0000000h
                    DQ 3fe2434b60000000h
                    DQ 3fe2555bc0000000h
                    DQ 3fe2676200000000h
                    DQ 3fe2795e10000000h
                    DQ 3fe28b5000000000h
                    DQ 3fe29d37f0000000h
                    DQ 3fe2af15f0000000h
                    DQ 3fe2c0e9e0000000h
                    DQ 3fe2d2b400000000h
                    DQ 3fe2e47430000000h
                    DQ 3fe2f62a90000000h
                    DQ 3fe307d730000000h
                    DQ 3fe3197a00000000h
                    DQ 3fe32b1330000000h
                    DQ 3fe33ca2b0000000h
                    DQ 3fe34e2890000000h
                    DQ 3fe35fa4e0000000h
                    DQ 3fe37117b0000000h
                    DQ 3fe38280f0000000h
                    DQ 3fe393e0d0000000h
                    DQ 3fe3a53730000000h
                    DQ 3fe3b68440000000h
                    DQ 3fe3c7c7f0000000h
                    DQ 3fe3d90260000000h
                    DQ 3fe3ea3390000000h
                    DQ 3fe3fb5b80000000h
                    DQ 3fe40c7a40000000h
                    DQ 3fe41d8fe0000000h
                    DQ 3fe42e9c60000000h
                    DQ 3fe43f9fe0000000h
                    DQ 3fe4509a50000000h
                    DQ 3fe4618bc0000000h
                    DQ 3fe4727430000000h
                    DQ 3fe48353d0000000h
                    DQ 3fe4942a80000000h
                    DQ 3fe4a4f850000000h
                    DQ 3fe4b5bd60000000h
                    DQ 3fe4c679a0000000h
                    DQ 3fe4d72d30000000h
                    DQ 3fe4e7d810000000h
                    DQ 3fe4f87a30000000h
                    DQ 3fe50913c0000000h
                    DQ 3fe519a4c0000000h
                    DQ 3fe52a2d20000000h
                    DQ 3fe53aad00000000h
                    DQ 3fe54b2460000000h
                    DQ 3fe55b9350000000h
                    DQ 3fe56bf9d0000000h
                    DQ 3fe57c57f0000000h
                    DQ 3fe58cadb0000000h
                    DQ 3fe59cfb20000000h
                    DQ 3fe5ad4040000000h
                    DQ 3fe5bd7d30000000h
                    DQ 3fe5cdb1d0000000h
                    DQ 3fe5ddde50000000h
                    DQ 3fe5ee02a0000000h
                    DQ 3fe5fe1ed0000000h
                    DQ 3fe60e32f0000000h
                    DQ 3fe61e3ef0000000h
                    DQ 3fe62e42e0000000h
                    DQ 0000000000000000h

ALIGN 16
__log_256_tail:
                    DQ 0000000000000000h
                    DQ 3db5885e0250435ah
                    DQ 3de620cf11f86ed2h
                    DQ 3dff0214edba4a25h
                    DQ 3dbf807c79f3db4eh
                    DQ 3dea352ba779a52bh
                    DQ 3dff56c46aa49fd5h
                    DQ 3dfebe465fef5196h
                    DQ 3e0cf0660099f1f8h
                    DQ 3e1247b2ff85945dh
                    DQ 3e13fd7abf5202b6h
                    DQ 3e1f91c9a918d51eh
                    DQ 3e08cb73f118d3cah
                    DQ 3e1d91c7d6fad074h
                    DQ 3de1971bec28d14ch
                    DQ 3e15b616a423c78ah
                    DQ 3da162a6617cc971h
                    DQ 3e166391c4c06d29h
                    DQ 3e2d46f5c1d0c4b8h
                    DQ 3e2e14282df1f6d3h
                    DQ 3e186f47424a660dh
                    DQ 3e2d4c8de077753eh
                    DQ 3e2e0c307ed24f1ch
                    DQ 3e226ea18763bdd3h
                    DQ 3e25cad69737c933h
                    DQ 3e2af62599088901h
                    DQ 3e18c66c83d6b2d0h
                    DQ 3e1880ceb36fb30fh
                    DQ 3e2495aac6ca17a4h
                    DQ 3e2761db4210878ch
                    DQ 3e2eb78e862bac2fh
                    DQ 3e19b2cd75790dd9h
                    DQ 3e2c55e5cbd3d50fh
                    DQ 3db162a6617cc971h
                    DQ 3dfdbeabaaa2e519h
                    DQ 3e1652cb7150c647h
                    DQ 3e39a11cb2cd2ee2h
                    DQ 3e219d0ab1a28813h
                    DQ 3e24bd9e80a41811h
                    DQ 3e3214b596faa3dfh
                    DQ 3e303fea46980bb8h
                    DQ 3e31c8ffa5fd28c7h
                    DQ 3dce8f743bcd96c5h
                    DQ 3dfd98c5395315c6h
                    DQ 3e3996fa3ccfa7b2h
                    DQ 3e1cd2af2ad13037h
                    DQ 3e1d0da1bd17200eh
                    DQ 3e3330410ba68b75h
                    DQ 3df4f27a790e7c41h
                    DQ 3e13956a86f6ff1bh
                    DQ 3e2c6748723551d9h
                    DQ 3e2500de9326cdfch
                    DQ 3e1086c848df1b59h
                    DQ 3e04357ead6836ffh
                    DQ 3e24832442408024h
                    DQ 3e3d10da8154b13dh
                    DQ 3e39e8ad68ec8260h
                    DQ 3e3cfbf706abaf18h
                    DQ 3e3fc56ac6326e23h
                    DQ 3e39105e3185cf21h
                    DQ 3e3d017fe5b19cc0h
                    DQ 3e3d1f6b48dd13feh
                    DQ 3e20b63358a7e73ah
                    DQ 3e263063028c211ch
                    DQ 3e2e6a6886b09760h
                    DQ 3e3c138bb891cd03h
                    DQ 3e369f7722b7221ah
                    DQ 3df57d8fac1a628ch
                    DQ 3e3c55e5cbd3d50fh
                    DQ 3e1552d2ff48fe2eh
                    DQ 3e37b8b26ca431bch
                    DQ 3e292decdc1c5f6dh
                    DQ 3e3abc7c551aaa8ch
                    DQ 3e36b540731a354bh
                    DQ 3e32d341036b89efh
                    DQ 3e4f9ab21a3a2e0fh
                    DQ 3e239c871afb9fbdh
                    DQ 3e3e6add2c81f640h
                    DQ 3e435c95aa313f41h
                    DQ 3e249d4582f6cc53h
                    DQ 3e47574c1c07398fh
                    DQ 3e4ba846dece9e8dh
                    DQ 3e16999fafbc68e7h
                    DQ 3e4c9145e51b0103h
                    DQ 3e479ef2cb44850ah
                    DQ 3e0beec73de11275h
                    DQ 3e2ef4351af5a498h
                    DQ 3e45713a493b4a50h
                    DQ 3e45c23a61385992h
                    DQ 3e42a88309f57299h
                    DQ 3e4530faa9ac8aceh
                    DQ 3e25fec2d792a758h
                    DQ 3e35a517a71cbcd7h
                    DQ 3e3707dc3e1cd9a3h
                    DQ 3e3a1a9f8ef43049h
                    DQ 3e4409d0276b3674h
                    DQ 3e20e2f613e85bd9h
                    DQ 3df0027433001e5fh
                    DQ 3e35dde2836d3265h
                    DQ 3e2300134d7aaf04h
                    DQ 3e3cb7e0b42724f5h
                    DQ 3e2d6e93167e6308h
                    DQ 3e3d1569b1526adbh
                    DQ 3e0e99fc338a1a41h
                    DQ 3e4eb01394a11b1ch
                    DQ 3e04f27a790e7c41h
                    DQ 3e25ce3ca97b7af9h
                    DQ 3e281f0f940ed857h
                    DQ 3e4d36295d88857ch
                    DQ 3e21aca1ec4af526h
                    DQ 3e445743c7182726h
                    DQ 3e23c491aead337eh
                    DQ 3e3aef401a738931h
                    DQ 3e21cede76092a29h
                    DQ 3e4fba8f44f82bb4h
                    DQ 3e446f5f7f3c3e1ah
                    DQ 3e47055f86c9674bh
                    DQ 3e4b41a92b6b6e1ah
                    DQ 3e443d162e927628h
                    DQ 3e4466174013f9b1h
                    DQ 3e3b05096ad69c62h
                    DQ 3e40b169150faa58h
                    DQ 3e3cd98b1df85da7h
                    DQ 3e468b507b0f8fa8h
                    DQ 3e48422df57499bah
                    DQ 3e11351586970274h
                    DQ 3e117e08acba92eeh
                    DQ 3e26e04314dd0229h
                    DQ 3e497f3097e56d1ah
                    DQ 3e3356e655901286h
                    DQ 3e0cb761457f94d6h
                    DQ 3e39af67a85a9dach
                    DQ 3e453410931a909fh
                    DQ 3e22c587206058f5h
                    DQ 3e223bc358899c22h
                    DQ 3e4d7bf8b6d223cbh
                    DQ 3e47991ec5197ddbh
                    DQ 3e4a79e6bb3a9219h
                    DQ 3e3a4c43ed663ec5h
                    DQ 3e461b5a1484f438h
                    DQ 3e4b4e36f7ef0c3ah
                    DQ 3e115f026acd0d1bh
                    DQ 3e3f36b535cecf05h
                    DQ 3e2ffb7fbf3eb5c6h
                    DQ 3e3e6a6886b09760h
                    DQ 3e3135eb27f5bbc3h
                    DQ 3e470be7d6f6fa57h
                    DQ 3e4ce43cc84ab338h
                    DQ 3e4c01d7aac3bd91h
                    DQ 3e45c58d07961060h
                    DQ 3e3628bcf941456eh
                    DQ 3e4c58b2a8461cd2h
                    DQ 3e33071282fb989ah
                    DQ 3e420dab6a80f09ch
                    DQ 3e44f8d84c397b1eh
                    DQ 3e40d0ee08599e48h
                    DQ 3e1d68787e37da36h
                    DQ 3e366187d591bafch
                    DQ 3e22346600bae772h
                    DQ 3e390377d0d61b8eh
                    DQ 3e4f5e0dd966b907h
                    DQ 3e49023cb79a00e2h
                    DQ 3e44e05158c28ad8h
                    DQ 3e3bfa7b08b18ae4h
                    DQ 3e4ef1e63db35f67h
                    DQ 3e0ec2ae39493d4fh
                    DQ 3e40afe930ab2fa0h
                    DQ 3e225ff8a1810dd4h
                    DQ 3e469743fb1a71a5h
                    DQ 3e5f9cc676785571h
                    DQ 3e5b524da4cbf982h
                    DQ 3e5a4c8b381535b8h
                    DQ 3e5839be809caf2ch
                    DQ 3e50968a1cb82c13h
                    DQ 3e5eae6a41723fb5h
                    DQ 3e5d9c29a380a4dbh
                    DQ 3e4094aa0ada625eh
                    DQ 3e5973ad6fc108cah
                    DQ 3e4747322fdbab97h
                    DQ 3e593692fa9d4221h
                    DQ 3e5c5a992dfbc7d9h
                    DQ 3e4e1f33e102387ah
                    DQ 3e464fbef14c048ch
                    DQ 3e4490f513ca5e3bh
                    DQ 3e37a6af4d4c799dh
                    DQ 3e57574c1c07398fh
                    DQ 3e57b133417f8c1ch
                    DQ 3e5feb9e0c176514h
                    DQ 3e419f25bb3172f7h
                    DQ 3e45f68a7bbfb852h
                    DQ 3e5ee278497929f1h
                    DQ 3e5ccee006109d58h
                    DQ 3e5ce081a07bd8b3h
                    DQ 3e570e12981817b8h
                    DQ 3e292ab6d93503d0h
                    DQ 3e58cb7dd7c3b61eh
                    DQ 3e4efafd0a0b78dah
                    DQ 3e5e907267c4288eh
                    DQ 3e5d31ef96780875h
                    DQ 3e23430dfcd2ad50h
                    DQ 3e344d88d75bc1f9h
                    DQ 3e5bec0f055e04fch
                    DQ 3e5d85611590b9adh
                    DQ 3df320568e583229h
                    DQ 3e5a891d1772f538h
                    DQ 3e22edc9dabba74dh
                    DQ 3e4b9009a1015086h
                    DQ 3e52a12a8c5b1a19h
                    DQ 3e3a7885f0fdac85h
                    DQ 3e5f4ffcd43ac691h
                    DQ 3e52243ae2640aadh
                    DQ 3e546513299035d3h
                    DQ 3e5b39c3a62dd725h
                    DQ 3e5ba6dd40049f51h
                    DQ 3e451d1ed7177409h
                    DQ 3e5cb0f2fd7f5216h
                    DQ 3e3ab150cd4e2213h
                    DQ 3e5cfd7bf3193844h
                    DQ 3e53fff8455f1dbdh
                    DQ 3e5fee640b905fc9h
                    DQ 3e54e2adf548084ch
                    DQ 3e3b597adc1ecdd2h
                    DQ 3e4345bd096d3a75h
                    DQ 3e5101b9d2453c8bh
                    DQ 3e508ce55cc8c979h
                    DQ 3e5bbf017e595f71h
                    DQ 3e37ce733bd393dch
                    DQ 3e233bb0a503f8a1h
                    DQ 3e30e2f613e85bd9h
                    DQ 3e5e67555a635b3ch
                    DQ 3e2ea88df73d5e8bh
                    DQ 3e3d17e03bda18a8h
                    DQ 3e5b607d76044f7eh
                    DQ 3e52adc4e71bc2fch
                    DQ 3e5f99dc7362d1d9h
                    DQ 3e5473fa008e6a6ah
                    DQ 3e2b75bb09cb0985h
                    DQ 3e5ea04dd10b9abah
                    DQ 3e5802d0d6979674h
                    DQ 3e174688ccd99094h
                    DQ 3e496f16abb9df22h
                    DQ 3e46e66df2aa374fh
                    DQ 3e4e66525ea4550ah
                    DQ 3e42d02f34f20cbdh
                    DQ 3e46cfce65047188h
                    DQ 3e39b78c842d58b8h
                    DQ 3e4735e624c24bc9h
                    DQ 3e47eba1f7dd1adfh
                    DQ 3e586b3e59f65355h
                    DQ 3e1ce38e637f1b4dh
                    DQ 3e58d82ec919edc7h
                    DQ 3e4c52648ddcfa37h
                    DQ 3e52482ceae1ac12h
                    DQ 3e55a312311aba4fh
                    DQ 3e411e236329f225h
                    DQ 3e5b48c8cd2f246ch
                    DQ 3e6efa39ef35793ch
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
