;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; log10.asm
;
; An implementation of the log10 libm function.
;
; Prototype:
;
;     double log10(double x);
;

;
;   Algorithm:
;       Similar to one presnted in log.asm
;

include fm.inc
FN_PROTOTYPE_BAS64 log10


fname_special   TEXTEQU <_log10_special>

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
    lea         r9, QWORD PTR __log_F_inv

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

    mulsd       xmm1, QWORD PTR __real_log10_e

    ; m*log(10) + log10(G) - poly
    movsd       xmm5, QWORD PTR __real_log10_2_tail
    mulsd       xmm5, xmm6
    subsd       xmm5, xmm1

    movsd       xmm0, QWORD PTR [r9+rax*8]
    lea         rdx, QWORD PTR __log_256_tail
    movsd       xmm2, QWORD PTR [rdx+rax*8]
    addsd       xmm2, xmm5

    movsd       xmm4, QWORD PTR __real_log10_2_lead
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

    mulsd       xmm4, QWORD PTR __real_log10_e_tail
    mulsd       xmm0, QWORD PTR __real_log10_e_tail
    mulsd       xmm1, QWORD PTR __real_log10_e_lead
    mulsd       xmm3, QWORD PTR __real_log10_e_lead

    addsd       xmm0, xmm4
    addsd       xmm0, xmm1
    addsd       xmm0, xmm3

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

__mask_mant_all8        DQ 000ff00000000000h
                        DQ 0000000000000000h
__mask_mant9            DQ 0000080000000000h
                        DQ 0000000000000000h

__real_log10_e          DQ 3fdbcb7b1526e50eh
                        DQ 0000000000000000h

__real_log10_e_lead     DQ 3fdbcb7800000000h ; log10e_lead 4.34293746948242187500e-01
                        DQ 0000000000000000h
__real_log10_e_tail     DQ 3ea8a93728719535h ; log10e_tail 7.3495500964015109100644e-7
                        DQ 0000000000000000h

__real_log10_2_lead     DQ 3fd3441350000000h
                        DQ 0000000000000000h
__real_log10_2_tail     DQ 3e03ef3fde623e25h
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
                    DQ 3f5bbd9e90000000h
                    DQ 3f6bafd470000000h
                    DQ 3f74b99560000000h
                    DQ 3f7b9476a0000000h
                    DQ 3f81344da0000000h
                    DQ 3f849b0850000000h
                    DQ 3f87fe71c0000000h
                    DQ 3f8b5e9080000000h
                    DQ 3f8ebb6af0000000h
                    DQ 3f910a83a0000000h
                    DQ 3f92b5b5e0000000h
                    DQ 3f945f4f50000000h
                    DQ 3f96075300000000h
                    DQ 3f97adc3d0000000h
                    DQ 3f9952a4f0000000h
                    DQ 3f9af5f920000000h
                    DQ 3f9c97c370000000h
                    DQ 3f9e3806a0000000h
                    DQ 3f9fd6c5b0000000h
                    DQ 3fa0ba01a0000000h
                    DQ 3fa187e120000000h
                    DQ 3fa25502c0000000h
                    DQ 3fa32167c0000000h
                    DQ 3fa3ed1190000000h
                    DQ 3fa4b80180000000h
                    DQ 3fa58238e0000000h
                    DQ 3fa64bb910000000h
                    DQ 3fa7148340000000h
                    DQ 3fa7dc98c0000000h
                    DQ 3fa8a3fad0000000h
                    DQ 3fa96aaac0000000h
                    DQ 3faa30a9d0000000h
                    DQ 3faaf5f920000000h
                    DQ 3fabba9a00000000h
                    DQ 3fac7e8d90000000h
                    DQ 3fad41d510000000h
                    DQ 3fae0471a0000000h
                    DQ 3faec66470000000h
                    DQ 3faf87aeb0000000h
                    DQ 3fb02428c0000000h
                    DQ 3fb08426f0000000h
                    DQ 3fb0e3d290000000h
                    DQ 3fb1432c30000000h
                    DQ 3fb1a23440000000h
                    DQ 3fb200eb60000000h
                    DQ 3fb25f5210000000h
                    DQ 3fb2bd68e0000000h
                    DQ 3fb31b3050000000h
                    DQ 3fb378a8e0000000h
                    DQ 3fb3d5d330000000h
                    DQ 3fb432afa0000000h
                    DQ 3fb48f3ed0000000h
                    DQ 3fb4eb8120000000h
                    DQ 3fb5477730000000h
                    DQ 3fb5a32160000000h
                    DQ 3fb5fe8040000000h
                    DQ 3fb6599440000000h
                    DQ 3fb6b45df0000000h
                    DQ 3fb70eddb0000000h
                    DQ 3fb7691400000000h
                    DQ 3fb7c30160000000h
                    DQ 3fb81ca630000000h
                    DQ 3fb8760300000000h
                    DQ 3fb8cf1830000000h
                    DQ 3fb927e640000000h
                    DQ 3fb9806d90000000h
                    DQ 3fb9d8aea0000000h
                    DQ 3fba30a9d0000000h
                    DQ 3fba885fa0000000h
                    DQ 3fbadfd070000000h
                    DQ 3fbb36fcb0000000h
                    DQ 3fbb8de4d0000000h
                    DQ 3fbbe48930000000h
                    DQ 3fbc3aea40000000h
                    DQ 3fbc910870000000h
                    DQ 3fbce6e410000000h
                    DQ 3fbd3c7da0000000h
                    DQ 3fbd91d580000000h
                    DQ 3fbde6ec00000000h
                    DQ 3fbe3bc1a0000000h
                    DQ 3fbe9056b0000000h
                    DQ 3fbee4aba0000000h
                    DQ 3fbf38c0c0000000h
                    DQ 3fbf8c9680000000h
                    DQ 3fbfe02d30000000h
                    DQ 3fc019c2a0000000h
                    DQ 3fc0434f70000000h
                    DQ 3fc06cbd60000000h
                    DQ 3fc0960c80000000h
                    DQ 3fc0bf3d00000000h
                    DQ 3fc0e84f10000000h
                    DQ 3fc11142f0000000h
                    DQ 3fc13a18a0000000h
                    DQ 3fc162d080000000h
                    DQ 3fc18b6a90000000h
                    DQ 3fc1b3e710000000h
                    DQ 3fc1dc4630000000h
                    DQ 3fc2048810000000h
                    DQ 3fc22cace0000000h
                    DQ 3fc254b4d0000000h
                    DQ 3fc27c9ff0000000h
                    DQ 3fc2a46e80000000h
                    DQ 3fc2cc20b0000000h
                    DQ 3fc2f3b690000000h
                    DQ 3fc31b3050000000h
                    DQ 3fc3428e20000000h
                    DQ 3fc369d020000000h
                    DQ 3fc390f680000000h
                    DQ 3fc3b80160000000h
                    DQ 3fc3def0e0000000h
                    DQ 3fc405c530000000h
                    DQ 3fc42c7e70000000h
                    DQ 3fc4531cd0000000h
                    DQ 3fc479a070000000h
                    DQ 3fc4a00970000000h
                    DQ 3fc4c65800000000h
                    DQ 3fc4ec8c30000000h
                    DQ 3fc512a640000000h
                    DQ 3fc538a630000000h
                    DQ 3fc55e8c50000000h
                    DQ 3fc5845890000000h
                    DQ 3fc5aa0b40000000h
                    DQ 3fc5cfa470000000h
                    DQ 3fc5f52440000000h
                    DQ 3fc61a8ad0000000h
                    DQ 3fc63fd850000000h
                    DQ 3fc6650cd0000000h
                    DQ 3fc68a2880000000h
                    DQ 3fc6af2b80000000h
                    DQ 3fc6d415e0000000h
                    DQ 3fc6f8e7d0000000h
                    DQ 3fc71da170000000h
                    DQ 3fc74242e0000000h
                    DQ 3fc766cc40000000h
                    DQ 3fc78b3da0000000h
                    DQ 3fc7af9730000000h
                    DQ 3fc7d3d910000000h
                    DQ 3fc7f80350000000h
                    DQ 3fc81c1620000000h
                    DQ 3fc8401190000000h
                    DQ 3fc863f5c0000000h
                    DQ 3fc887c2e0000000h
                    DQ 3fc8ab7900000000h
                    DQ 3fc8cf1830000000h
                    DQ 3fc8f2a0a0000000h
                    DQ 3fc9161270000000h
                    DQ 3fc9396db0000000h
                    DQ 3fc95cb280000000h
                    DQ 3fc97fe100000000h
                    DQ 3fc9a2f950000000h
                    DQ 3fc9c5fb70000000h
                    DQ 3fc9e8e7b0000000h
                    DQ 3fca0bbdf0000000h
                    DQ 3fca2e7e80000000h
                    DQ 3fca512960000000h
                    DQ 3fca73bea0000000h
                    DQ 3fca963e70000000h
                    DQ 3fcab8a8f0000000h
                    DQ 3fcadafe20000000h
                    DQ 3fcafd3e30000000h
                    DQ 3fcb1f6930000000h
                    DQ 3fcb417f40000000h
                    DQ 3fcb638070000000h
                    DQ 3fcb856cf0000000h
                    DQ 3fcba744b0000000h
                    DQ 3fcbc907f0000000h
                    DQ 3fcbeab6c0000000h
                    DQ 3fcc0c5130000000h
                    DQ 3fcc2dd750000000h
                    DQ 3fcc4f4950000000h
                    DQ 3fcc70a740000000h
                    DQ 3fcc91f130000000h
                    DQ 3fccb32740000000h
                    DQ 3fccd44980000000h
                    DQ 3fccf55810000000h
                    DQ 3fcd165300000000h
                    DQ 3fcd373a60000000h
                    DQ 3fcd580e60000000h
                    DQ 3fcd78cf00000000h
                    DQ 3fcd997c70000000h
                    DQ 3fcdba16a0000000h
                    DQ 3fcdda9dd0000000h
                    DQ 3fcdfb11f0000000h
                    DQ 3fce1b7330000000h
                    DQ 3fce3bc1a0000000h
                    DQ 3fce5bfd50000000h
                    DQ 3fce7c2660000000h
                    DQ 3fce9c3ce0000000h
                    DQ 3fcebc40e0000000h
                    DQ 3fcedc3280000000h
                    DQ 3fcefc11d0000000h
                    DQ 3fcf1bdee0000000h
                    DQ 3fcf3b99d0000000h
                    DQ 3fcf5b42a0000000h
                    DQ 3fcf7ad980000000h
                    DQ 3fcf9a5e70000000h
                    DQ 3fcfb9d190000000h
                    DQ 3fcfd932f0000000h
                    DQ 3fcff882a0000000h
                    DQ 3fd00be050000000h
                    DQ 3fd01b76a0000000h
                    DQ 3fd02b0430000000h
                    DQ 3fd03a8910000000h
                    DQ 3fd04a0540000000h
                    DQ 3fd05978e0000000h
                    DQ 3fd068e3f0000000h
                    DQ 3fd0784670000000h
                    DQ 3fd087a080000000h
                    DQ 3fd096f210000000h
                    DQ 3fd0a63b30000000h
                    DQ 3fd0b57bf0000000h
                    DQ 3fd0c4b450000000h
                    DQ 3fd0d3e460000000h
                    DQ 3fd0e30c30000000h
                    DQ 3fd0f22bc0000000h
                    DQ 3fd1014310000000h
                    DQ 3fd1105240000000h
                    DQ 3fd11f5940000000h
                    DQ 3fd12e5830000000h
                    DQ 3fd13d4f00000000h
                    DQ 3fd14c3dd0000000h
                    DQ 3fd15b24a0000000h
                    DQ 3fd16a0370000000h
                    DQ 3fd178da50000000h
                    DQ 3fd187a940000000h
                    DQ 3fd1967060000000h
                    DQ 3fd1a52fa0000000h
                    DQ 3fd1b3e710000000h
                    DQ 3fd1c296c0000000h
                    DQ 3fd1d13eb0000000h
                    DQ 3fd1dfdef0000000h
                    DQ 3fd1ee7770000000h
                    DQ 3fd1fd0860000000h
                    DQ 3fd20b91a0000000h
                    DQ 3fd21a1350000000h
                    DQ 3fd2288d70000000h
                    DQ 3fd2370010000000h
                    DQ 3fd2456b30000000h
                    DQ 3fd253ced0000000h
                    DQ 3fd2622b00000000h
                    DQ 3fd2707fd0000000h
                    DQ 3fd27ecd40000000h
                    DQ 3fd28d1360000000h
                    DQ 3fd29b5220000000h
                    DQ 3fd2a989a0000000h
                    DQ 3fd2b7b9e0000000h
                    DQ 3fd2c5e2e0000000h
                    DQ 3fd2d404b0000000h
                    DQ 3fd2e21f50000000h
                    DQ 3fd2f032c0000000h
                    DQ 3fd2fe3f20000000h
                    DQ 3fd30c4470000000h
                    DQ 3fd31a42b0000000h
                    DQ 3fd32839e0000000h
                    DQ 3fd3362a10000000h
                    DQ 3fd3441350000000h

ALIGN 16
__log_256_tail:
                    DQ 0000000000000000h
                    DQ 3db20abc22b2208fh
                    DQ 3db10f69332e0dd4h
                    DQ 3dce950de87ed257h
                    DQ 3dd3f3443b626d69h
                    DQ 3df45aeaa5363e57h
                    DQ 3dc443683ce1bf0bh
                    DQ 3df989cd60c6a511h
                    DQ 3dfd626f201f2e9fh
                    DQ 3de94f8bb8dabdcdh
                    DQ 3e0088d8ef423015h
                    DQ 3e080413a62b79adh
                    DQ 3e059717c0eed3c4h
                    DQ 3dad4a77add44902h
                    DQ 3e0e763ff037300eh
                    DQ 3de162d74706f6c3h
                    DQ 3e0601cc1f4dbc14h
                    DQ 3deaf3e051f6e5bfh
                    DQ 3e097a0b1e1af3ebh
                    DQ 3dc0a38970c002c7h
                    DQ 3e102e000057c751h
                    DQ 3e155b00eecd6e0eh
                    DQ 3ddf86297003b5afh
                    DQ 3e1057b9b336a36dh
                    DQ 3e134bc84a06ea4fh
                    DQ 3e1643da9ea1bcadh
                    DQ 3e1d66a7b4f7ea2ah
                    DQ 3df6b2e038f7fcefh
                    DQ 3df3e954c670f088h
                    DQ 3e047209093acab3h
                    DQ 3e1d708fe7275da7h
                    DQ 3e1fdf9e7771b9e7h
                    DQ 3e0827bfa70a0660h
                    DQ 3e1601cc1f4dbc14h
                    DQ 3e0637f6106a5e5bh
                    DQ 3e126a13f17c624bh
                    DQ 3e093eb2ce80623ah
                    DQ 3e1430d1e91594deh
                    DQ 3e1d6b10108fa031h
                    DQ 3e16879c0bbaf241h
                    DQ 3dff08015ea6bc2bh
                    DQ 3e29b63dcdc6676ch
                    DQ 3e2b022cbcc4ab2ch
                    DQ 3df917d07ddd6544h
                    DQ 3e1540605703379eh
                    DQ 3e0cd18b947a1b60h
                    DQ 3e17ad65277ca97eh
                    DQ 3e11884dc59f5fa9h
                    DQ 3e1711c46006d082h
                    DQ 3e2f092e3c3108f8h
                    DQ 3e1714c5e32be13ah
                    DQ 3e26bba7fd734f9ah
                    DQ 3dfdf48fb5e08483h
                    DQ 3e232f9bc74d0b95h
                    DQ 3df973e848790c13h
                    DQ 3e1eccbc08c6586eh
                    DQ 3e2115e9f9524a98h
                    DQ 3e2f1740593131b8h
                    DQ 3e1bcf8b25643835h
                    DQ 3e1f5fa81d8bed80h
                    DQ 3e244a4df929d9e4h
                    DQ 3e129820d8220c94h
                    DQ 3e2a0b489304e309h
                    DQ 3e1f4d56aba665feh
                    DQ 3e210c9019365163h
                    DQ 3df80f78fe592736h
                    DQ 3e10528825c81ccah
                    DQ 3de095537d6d746ah
                    DQ 3e1827bfa70a0660h
                    DQ 3e06b0a8ec45933ch
                    DQ 3e105af81bf5dba9h
                    DQ 3e17e2fa2655d515h
                    DQ 3e0d59ecbfaee4bfh
                    DQ 3e1d8b2fda683fa3h
                    DQ 3e24b8ddfd3a3737h
                    DQ 3e13827e61ae1204h
                    DQ 3e2c8c7b49e90f9fh
                    DQ 3e29eaf01597591dh
                    DQ 3e19aaa66e317b36h
                    DQ 3e2e725609720655h
                    DQ 3e261c33fc7aac54h
                    DQ 3e29662bcf61a252h
                    DQ 3e1843c811c42730h
                    DQ 3e2064bb0b5acb36h
                    DQ 3e0a340c842701a4h
                    DQ 3e1a8e55b58f79d6h
                    DQ 3de92d219c5e9d9ah
                    DQ 3e3f63e60d7ffd6ah
                    DQ 3e2e9b0ed9516314h
                    DQ 3e2923901962350ch
                    DQ 3e326f8838785e81h
                    DQ 3e3b5b6a4caba6afh
                    DQ 3df0226adc8e761ch
                    DQ 3e3c4ad7313a1aedh
                    DQ 3e1564e87c738d17h
                    DQ 3e338fecf18a6618h
                    DQ 3e3d929ef5777666h
                    DQ 3e39483bf08da0b8h
                    DQ 3e3bdd0eeeaa5826h
                    DQ 3e39c4dd590237bah
                    DQ 3e1af3e9e0ebcac7h
                    DQ 3e35ce5382270dach
                    DQ 3e394f74532ab9bah
                    DQ 3e07342795888654h
                    DQ 3e0c5a000be34bf0h
                    DQ 3e2711c46006d082h
                    DQ 3e250025b4ed8cf8h
                    DQ 3e2ed18bcef2d2a0h
                    DQ 3e21282e0c0a7554h
                    DQ 3e0d70f33359a7cah
                    DQ 3e2b7f7e13a84025h
                    DQ 3e33306ec321891eh
                    DQ 3e3fc7f8038b7550h
                    DQ 3e3eb0358cd71d64h
                    DQ 3e3a76c822859474h
                    DQ 3e3d0ec652de86e3h
                    DQ 3e2fa4cce08658afh
                    DQ 3e3b84a2d2c00a9eh
                    DQ 3e20a5b0f2c25bd1h
                    DQ 3e3dd660225bf699h
                    DQ 3e08b10f859bf037h
                    DQ 3e3e8823b590cbe1h
                    DQ 3e361311f31e96f6h
                    DQ 3e2e1f875ca20f9ah
                    DQ 3e2c95724939b9a5h
                    DQ 3e3805957a3e58e2h
                    DQ 3e2ff126ea9f0334h
                    DQ 3e3953f5598e5609h
                    DQ 3e36c16ff856c448h
                    DQ 3e24cb220ff261f4h
                    DQ 3e35e120d53d53a2h
                    DQ 3e3a527f6189f256h
                    DQ 3e3856fcffd49c0fh
                    DQ 3e300c2e8228d7dah
                    DQ 3df113d09444dfe0h
                    DQ 3e2510630eea59a6h
                    DQ 3e262e780f32d711h
                    DQ 3ded3ed91a10f8cfh
                    DQ 3e23654a7e4bcd85h
                    DQ 3e055b784980ad21h
                    DQ 3e212f2dd4b16e64h
                    DQ 3e37c4add939f50ch
                    DQ 3e281784627180fch
                    DQ 3dea5162c7e14961h
                    DQ 3e310c9019365163h
                    DQ 3e373c4d2ba17688h
                    DQ 3e2ae8a5e0e93d81h
                    DQ 3e2ab0c6f01621afh
                    DQ 3e301e8b74dd5b66h
                    DQ 3e2d206fecbb5494h
                    DQ 3df0b48b724fcc00h
                    DQ 3e3f831f0b61e229h
                    DQ 3df81a97c407bcafh
                    DQ 3e3e286c1ccbb7aah
                    DQ 3e28630b49220a93h
                    DQ 3dff0b15c1a22c5ch
                    DQ 3e355445e71c0946h
                    DQ 3e3be630f8066d85h
                    DQ 3e2599dff0d96c39h
                    DQ 3e36cc85b18fb081h
                    DQ 3e34476d001ea8c8h
                    DQ 3e373f889e16d31fh
                    DQ 3e3357100d792a87h
                    DQ 3e3bd179ae6101f6h
                    DQ 3e0ca31056c3f6e2h
                    DQ 3e3d2870629c08fbh
                    DQ 3e3aba3880d2673fh
                    DQ 3e2c3633cb297da6h
                    DQ 3e21843899efea02h
                    DQ 3e3bccc99d2008e6h
                    DQ 3e38000544bdd350h
                    DQ 3e2b91c226606ae1h
                    DQ 3e2a7adf26b62bdfh
                    DQ 3e18764fc8826ec9h
                    DQ 3e1f4f3de50f68f0h
                    DQ 3df760ca757995e3h
                    DQ 3dfc667ed3805147h
                    DQ 3e3733f6196adf6fh
                    DQ 3e2fb710f33e836bh
                    DQ 3e39886eba641013h
                    DQ 3dfb5368d0af8c1ah
                    DQ 3e358c691b8d2971h
                    DQ 3dfe9465226d08fbh
                    DQ 3e33587e063f0097h
                    DQ 3e3618e702129f18h
                    DQ 3e361c33fc7aac54h
                    DQ 3e3f07a68408604ah
                    DQ 3e3c34bfe4945421h
                    DQ 3e38b1f00e41300bh
                    DQ 3e3f434284d61b63h
                    DQ 3e3a63095e397436h
                    DQ 3e34428656b919deh
                    DQ 3e36ca9201b2d9a6h
                    DQ 3e2738823a2a931ch
                    DQ 3e3c11880e179230h
                    DQ 3e313ddc8d6d52feh
                    DQ 3e33eed58922e917h
                    DQ 3e295992846bdd50h
                    DQ 3e0ddb4d5f2e278bh
                    DQ 3df1a5f12a0635c4h
                    DQ 3e4642f0882c3c34h
                    DQ 3e2aee9ba7f6475eh
                    DQ 3e264b7f834a60e4h
                    DQ 3e290d42e243792eh
                    DQ 3e4c272008134f01h
                    DQ 3e4a782e16d6cf5bh
                    DQ 3e44505c79da6648h
                    DQ 3e4ca9d4ea4dcd21h
                    DQ 3e297d3d627cd5bch
                    DQ 3e20b15cf9bcaa13h
                    DQ 3e315b2063cf76ddh
                    DQ 3e2983e6f3aa2748h
                    DQ 3e3f4c64f4ffe994h
                    DQ 3e46beba7ce85a0fh
                    DQ 3e3b9c69fd4ea6b8h
                    DQ 3e2b6aa5835fa4abh
                    DQ 3e43ccc3790fedd1h
                    DQ 3e29c04cc4404fe0h
                    DQ 3e40734b7a75d89dh
                    DQ 3e1b4404c4e01612h
                    DQ 3e40c565c2ce4894h
                    DQ 3e33c71441d935cdh
                    DQ 3d72a492556b3b4eh
                    DQ 3e20fa090341dc43h
                    DQ 3e2e8f7009e3d9f4h
                    DQ 3e4b1bf68b048a45h
                    DQ 3e3eee52dffaa956h
                    DQ 3e456b0900e465bdh
                    DQ 3e4d929ef5777666h
                    DQ 3e486ea28637e260h
                    DQ 3e4665aff10ca2f0h
                    DQ 3e2f11fdaf48ec74h
                    DQ 3e4cbe1b86a4d1c7h
                    DQ 3e25b05bfea87665h
                    DQ 3e41cec20a1a4a1dh
                    DQ 3e41cd5f0a409b9fh
                    DQ 3e453656c8265070h
                    DQ 3e377ed835282260h
                    DQ 3e2417bc3040b9d2h
                    DQ 3e408eef7b79eff2h
                    DQ 3e4dc76f39dc57e9h
                    DQ 3e4c0493a70cf457h
                    DQ 3e4a83d6cea5a60ch
                    DQ 3e30d6700dc557bah
                    DQ 3e44c96c12e8bd0ah
                    DQ 3e3d2c1993e32315h
                    DQ 3e22c721135f8242h
                    DQ 3e279a3e4dda747dh
                    DQ 3dfcf89f6941a72bh
                    DQ 3e2149a702f10831h
                    DQ 3e4ead4b7c8175dbh
                    DQ 3e4e6930fe63e70ah
                    DQ 3e41e106bed9ee2fh
                    DQ 3e2d682b82f11c92h
                    DQ 3e3a07f188dba47ch
                    DQ 3e40f9342dc172f6h
                    DQ 3e03ef3fde623e25h

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
