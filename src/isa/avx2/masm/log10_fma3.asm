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
; log10_fma3.S
;
; An implementation of the log10 libm function.
;
; Prototype:
;
;     double log10(double x);
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
;   x very close to 1.0 is handled    for,differently x everywhere else
;   a brief explanation is given below
;
;   x = (2^m)*A
;   x = (2^m)*(G+g) with (1 <= G < 2) and (g <= 2^(-9))
;   x = (2^m)*2*(G/2+g/2)
;   x = (2^m)*2*(F+f) with (0.5 <= F < 1) and (f <= 2^(-10))
;   
;   Y = (2^(-1))*(2^(-m))*(2^m)*A
;   No    range,w of Y is: 0.5 <= Y < 1
;
;   F = 0100h + (first 8 mantissa bits) + (9th mantissa bit)
;   No    range,w of F is: 256 <= F <= 512
;   F = F / 512
;   No    range,w of F is: 0.5 <= F <= 1
;
;   f = -(Y-F    with,) (f <= 2^(-10))
;
;   log(x) = m*log(2) + log(2) + log(F-f)
;   log(x) = m*log(2) + log(2) + log(F) + log(1-(f/F))
;   log(x) = m*log(2) + log(2*F) + log(1-r)
;
;   r = (f/F    with,) (r <= 2^(-9))
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

ALM_PROTO_FMA3 log10
 
; local variable storage offsets
save_xmm6       EQU     00h
stack_size      EQU     028h ; We take 8 as the last nibble to allow for 
                              ; alligned data movement.
                              
fname_special   TEXTEQU <alm_log_special>
EXTERN fname_special:PROC
text SEGMENT EXECUTE
ALIGN 16
PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveRegsAVX xmm6,save_xmm6
    .ENDPROLOG 


    ; compute exponent part
    xor        rax,rax
    vpsrlq     xmm3,xmm0,52
    vmovq      rax,xmm0
    vpsubq     xmm3,xmm3,QWORD PTR L__mask_1023
    vcvtdq2pd  xmm6,xmm3 ; xexp replaced VCVTDQ2PD
    
    ;  NaN or inf
    vpand    xmm5,xmm0,L__real_inf
    vcomisd    xmm5,QWORD PTR L__real_inf
    je         L__x_is_inf_or_nan
    
    ; check for negative numbers or zero
    vpxor      xmm5,xmm5,xmm5
    vcomisd    xmm0,xmm5
    jbe        L__x_is_zero_or_neg

    vpand      xmm2,xmm0,L__real_mant
    vsubsd     xmm4,xmm0,QWORD PTR L__real_one

    vcomisd    xmm6,QWORD PTR L__mask_1023_f
    je         L__denormal_adjust

L__continue_common:    
    ; compute index into the log tables
    vpand    xmm1,xmm0,DWORD PTR L__mask_mant_all8
    vpand    xmm3,xmm0,DWORD PTR L__mask_mant9
    vpsllq   xmm3,xmm3,1 
    vpaddq   xmm1,xmm3,xmm1
    vmovq    rax,xmm1

    ; near one codepath
    vpand    xmm4,xmm4,DWORD PTR L__real_notsign
    vcomisd  xmm4,QWORD PTR L__real_threshold
    jb       L__near_one

    ; F,Y
    shr    rax,44
    vpor    xmm2,xmm2,DWORD PTR L__real_half
    vpor    xmm1,xmm1,DWORD PTR L__real_half
    lea    r9,DWORD PTR L__log_F_inv

    ; f = F - Y,r = f * inv
    vsubsd    xmm1,xmm1,xmm2
    vmulsd    xmm1,xmm1,QWORD PTR [r9 + rax * 8]

    lea    r9,DWORD PTR L__log_256_lead

    ; poly
    vmulsd    xmm0,xmm1,xmm1 ; r*r
    vmovsd    xmm3,QWORD PTR L__real_1_over_6
    vmovsd    xmm5,QWORD PTR L__real_1_over_3
    vfmadd213sd  xmm3,xmm1,QWORD PTR L__real_1_over_5 ; r*1/6 + 1/5
    vfmadd213sd  xmm5,xmm1,QWORD PTR L__real_1_over_2 ; 1/2+r*1/3
    movsd     xmm4,xmm0 ; r*r
    vfmadd213sd  xmm3 ,xmm1,QWORD PTR L__real_1_over_4 ;1/4+(1/5*r+r*r*1/6)
    
    vmulsd    xmm4,xmm0,xmm0                   ; r*r*r*r
    vfmadd231sd  xmm1,xmm5,xmm0 ; r*r*(1/2+r*1/3) + r
    vfmadd231sd  xmm1,xmm3,xmm4

    vmulsd    xmm1,xmm1,QWORD PTR L__real_log10_e
    ; m*log(2) + log(G) - poly*log10_e
    vmovsd    xmm5,QWORD PTR L__real_log10_2_tail
    vfmsub213sd  xmm5,xmm6,xmm1

    movsd     xmm0,QWORD PTR [r9 + rax * 8]
    lea       rdx,DWORD PTR L__log_256_tail
    movsd     xmm2,QWORD PTR [rdx + rax * 8]
    vaddsd    xmm2,xmm2,xmm5

    vfmadd231sd  xmm0,xmm6,QWORD PTR L__real_log10_2_lead 

    vaddsd    xmm0,xmm0,xmm2
	RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN  16
L__near_one:
    ; r = x - 1.0
    vmovsd    xmm2,QWORD PTR L__real_two
    vsubsd    xmm0,xmm0,QWORD PTR L__real_one ; r

    vaddsd    xmm2,xmm2,xmm0
    vdivsd    xmm1,xmm0,xmm2 ; r/(2+r) = u/2

    vmovsd    xmm4,QWORD PTR L__real_ca2
    vmovsd    xmm5,QWORD PTR L__real_ca4

    vmulsd    xmm6,xmm0,xmm1 ; correction
    vaddsd    xmm1,xmm1,xmm1 ; u

    vmulsd    xmm2,xmm1,xmm1 ; u^2
    vfmadd213sd  xmm4,xmm2,QWORD PTR L__real_ca1
    vfmadd213sd  xmm5,xmm2,QWORD PTR L__real_ca3

    vmulsd    xmm2,xmm2,xmm1 ; u^3
    vmulsd    xmm4,xmm4,xmm2

    vmulsd    xmm2,xmm2,xmm2
    vmulsd    xmm2,xmm2,xmm1 ; u^7

    vmulsd    xmm5,xmm5,xmm2
    vaddsd    xmm4,xmm4,xmm5
    vsubsd    xmm4,xmm4,xmm6
    vpand     xmm3,xmm0,QWORD PTR L__mask_lower
    vsubsd    xmm0,xmm0,xmm3
    vaddsd    xmm4,xmm4,xmm0

    vmulsd    xmm1,xmm4,QWORD PTR L__real_log10_e_lead
    vmulsd    xmm4,xmm4,QWORD PTR L__real_log10_e_tail
    vmulsd    xmm0,xmm3,QWORD PTR L__real_log10_e_tail
    vmulsd    xmm3,xmm3,QWORD PTR L__real_log10_e_lead

    vaddsd    xmm0,xmm0,xmm4
    vaddsd    xmm0,xmm0,xmm1
    vaddsd    xmm0,xmm0,xmm3

	RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


L__denormal_adjust:
    vpor      xmm2,xmm2,QWORD PTR L__real_one
    vsubsd    xmm2,xmm2,QWORD PTR L__real_one
    vpsrlq    xmm5,xmm2,52
    vpand     xmm2,xmm2,QWORD PTR L__real_mant
    vmovapd   xmm0,xmm2
    vpsubd    xmm5,xmm5,DWORD PTR L__mask_2045
    vcvtdq2pd xmm6,xmm5
    jmp       L__continue_common

ALIGN  16 
L__x_is_zero_or_neg:
    jne         L__x_is_neg
    vmovsd    xmm1,QWORD PTR L__real_ninf
    mov       r8d,DWORD PTR L__flag_x_zero
    call      fname_special

	RestoreRegsAVX xmm6,save_xmm6
     StackDeallocate stack_size
    ret
   

ALIGN  16 
L__x_is_neg:

    vmovsd    xmm1,QWORD PTR L__real_neg_qnan
    mov       r8d,DWORD PTR L__flag_x_neg
    call      fname_special

	RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN  16 
L__x_is_inf_or_nan:

    cmp    rax,QWORD PTR L__real_inf
    je     L__finish

    cmp    rax,QWORD PTR L__real_ninf
    je     L__x_is_neg

    or     rax,QWORD PTR L__real_qnanbit
    movd   xmm1,rax
    mov    r8d,DWORD PTR L__flag_x_nan
    call   fname_special
    jmp    L__finish    

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
L__flag_x_zero         DD 00000001
L__flag_x_neg          DD 00000002
L__flag_x_nan          DD 00000003

ALIGN 16

L__real_ninf        DQ 00fff0000000000000h   ; -inf
                    DQ 00000000000000000h
L__real_inf         DQ 07ff0000000000000h    ; +inf
                    DQ 00000000000000000h
L__real_neg_qnan    DQ 0fff8000000000000h   ; neg qNaN
                    DQ 0000000000000000h
L__real_qnan        DQ 07ff8000000000000h   ; qNaN
                    DQ 00000000000000000h
L__real_qnanbit     DQ 00008000000000000h
                    DQ 00000000000000000h
L__real_mant        DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                    DQ 00000000000000000h
L__mask_1023        DQ 000000000000003ffh
                    DQ 00000000000000000h

L__mask_mant_all8      DQ 0000ff00000000000h
                       DQ 00000000000000000h
L__mask_mant9          DQ 00000080000000000h
                       DQ 00000000000000000h
L__real_log10_e        DQ 03fdbcb7b1526e50eh
                       DQ 00000000000000000h
L__real_log10_e_lead   DQ 03fdbcb7800000000h ; log10e_lead 4.34293746948242187500e-01
                       DQ 00000000000000000h
L__real_log10_e_tail   DQ 03ea8a93728719535h ; log10e_tail 7.3495500964015109100644e-7
                       DQ 00000000000000000h
L__real_log10_2_lead   DQ 03fd3441350000000h
                       DQ 00000000000000000h
L__real_log10_2_tail   DQ 03e03ef3fde623e25h
                       DQ 00000000000000000h






L__real_two        DQ 04000000000000000h ; 2
                   DQ 00000000000000000h

L__real_one        DQ 03ff0000000000000h ; 1
                   DQ 00000000000000000h

L__real_half       DQ 03fe0000000000000h ; 1/2
                   DQ 00000000000000000h

L__mask_100        DQ 00000000000000100h
                   DQ 00000000000000000h

L__real_1_over_512 DQ 03f60000000000000h
                   DQ 00000000000000000h

L__real_1_over_2   DQ 03fe0000000000000h
                   DQ 00000000000000000h
L__real_1_over_3   DQ 03fd5555555555555h
                   DQ 00000000000000000h
L__real_1_over_4   DQ 03fd0000000000000h
                   DQ 00000000000000000h
L__real_1_over_5   DQ 03fc999999999999ah
                   DQ 00000000000000000h
L__real_1_over_6   DQ 03fc5555555555555h
                   DQ 00000000000000000h

L__mask_1023_f     DQ 00c08ff80000000000h
                   DQ 00000000000000000h

L__mask_2045       DQ 000000000000007fdh
                   DQ 00000000000000000h

L__real_threshold  DQ 03fb0000000000000h ; .0625
                   DQ 00000000000000000h

L__real_notsign    DQ 07ffFFFFFFFFFFFFFh ; ^sign bit
                   DQ 00000000000000000h

L__real_ca1        DQ 03fb55555555554e6h ; 8.33333333333317923934e-02
                   DQ 00000000000000000h
L__real_ca2        DQ 03f89999999bac6d4h ; 1.25000000037717509602e-02
                   DQ 00000000000000000h
L__real_ca3        DQ 03f62492307f1519fh ; 2.23213998791944806202e-03
                    DQ 00000000000000000h
L__real_ca4         DQ 03f3c8034c85dfff0h ; 4.34887777707614552256e-04
                    DQ 00000000000000000h
L__mask_lower       DQ 0ffffffff00000000h
                    DQ 00000000000000000h

ALIGN 16
L__log_256_lead     DQ 00000000000000000h
                    DQ 03f5bbd9e90000000h
                    DQ 03f6bafd470000000h
                    DQ 03f74b99560000000h
                    DQ 03f7b9476a0000000h
                    DQ 03f81344da0000000h
                    DQ 03f849b0850000000h
                    DQ 03f87fe71c0000000h
                    DQ 03f8b5e9080000000h
                    DQ 03f8ebb6af0000000h
                    DQ 03f910a83a0000000h
                    DQ 03f92b5b5e0000000h
                    DQ 03f945f4f50000000h
                    DQ 03f96075300000000h
                    DQ 03f97adc3d0000000h
                    DQ 03f9952a4f0000000h
                    DQ 03f9af5f920000000h
                    DQ 03f9c97c370000000h
                    DQ 03f9e3806a0000000h
                    DQ 03f9fd6c5b0000000h
                    DQ 03fa0ba01a0000000h
                    DQ 03fa187e120000000h
                    DQ 03fa25502c0000000h
                    DQ 03fa32167c0000000h
                    DQ 03fa3ed1190000000h
                    DQ 03fa4b80180000000h
                    DQ 03fa58238e0000000h
                    DQ 03fa64bb910000000h
                    DQ 03fa7148340000000h
                    DQ 03fa7dc98c0000000h
                    DQ 03fa8a3fad0000000h
                    DQ 03fa96aaac0000000h
                    DQ 03faa30a9d0000000h
                    DQ 03faaf5f920000000h
                    DQ 03fabba9a00000000h
                    DQ 03fac7e8d90000000h
                    DQ 03fad41d510000000h
                    DQ 03fae0471a0000000h
                    DQ 03faec66470000000h
                    DQ 03faf87aeb0000000h
                    DQ 03fb02428c0000000h
                    DQ 03fb08426f0000000h
                    DQ 03fb0e3d290000000h
                    DQ 03fb1432c30000000h
                    DQ 03fb1a23440000000h
                    DQ 03fb200eb60000000h
                    DQ 03fb25f5210000000h
                    DQ 03fb2bd68e0000000h
                    DQ 03fb31b3050000000h
                    DQ 03fb378a8e0000000h
                    DQ 03fb3d5d330000000h
                    DQ 03fb432afa0000000h
                    DQ 03fb48f3ed0000000h
                    DQ 03fb4eb8120000000h
                    DQ 03fb5477730000000h
                    DQ 03fb5a32160000000h
                    DQ 03fb5fe8040000000h
                    DQ 03fb6599440000000h
                    DQ 03fb6b45df0000000h
                    DQ 03fb70eddb0000000h
                    DQ 03fb7691400000000h
                    DQ 03fb7c30160000000h
                    DQ 03fb81ca630000000h
                    DQ 03fb8760300000000h
                    DQ 03fb8cf1830000000h
                    DQ 03fb927e640000000h
                    DQ 03fb9806d90000000h
                    DQ 03fb9d8aea0000000h
                    DQ 03fba30a9d0000000h
                    DQ 03fba885fa0000000h
                    DQ 03fbadfd070000000h
                    DQ 03fbb36fcb0000000h
                    DQ 03fbb8de4d0000000h
                    DQ 03fbbe48930000000h
                    DQ 03fbc3aea40000000h
                    DQ 03fbc910870000000h
                    DQ 03fbce6e410000000h
                    DQ 03fbd3c7da0000000h
                    DQ 03fbd91d580000000h
                    DQ 03fbde6ec00000000h
                    DQ 03fbe3bc1a0000000h
                    DQ 03fbe9056b0000000h
                    DQ 03fbee4aba0000000h
                    DQ 03fbf38c0c0000000h
                    DQ 03fbf8c9680000000h
                    DQ 03fbfe02d30000000h
                    DQ 03fc019c2a0000000h
                    DQ 03fc0434f70000000h
                    DQ 03fc06cbd60000000h
                    DQ 03fc0960c80000000h
                    DQ 03fc0bf3d00000000h
                    DQ 03fc0e84f10000000h
                    DQ 03fc11142f0000000h
                    DQ 03fc13a18a0000000h
                    DQ 03fc162d080000000h
                    DQ 03fc18b6a90000000h
                    DQ 03fc1b3e710000000h
                    DQ 03fc1dc4630000000h
                    DQ 03fc2048810000000h
                    DQ 03fc22cace0000000h
                    DQ 03fc254b4d0000000h
                    DQ 03fc27c9ff0000000h
                    DQ 03fc2a46e80000000h
                    DQ 03fc2cc20b0000000h
                    DQ 03fc2f3b690000000h
                    DQ 03fc31b3050000000h
                    DQ 03fc3428e20000000h
                    DQ 03fc369d020000000h
                    DQ 03fc390f680000000h
                    DQ 03fc3b80160000000h
                    DQ 03fc3def0e0000000h
                    DQ 03fc405c530000000h
                    DQ 03fc42c7e70000000h
                    DQ 03fc4531cd0000000h
                    DQ 03fc479a070000000h
                    DQ 03fc4a00970000000h
                    DQ 03fc4c65800000000h
                    DQ 03fc4ec8c30000000h
                    DQ 03fc512a640000000h
                    DQ 03fc538a630000000h
                    DQ 03fc55e8c50000000h
                    DQ 03fc5845890000000h
                    DQ 03fc5aa0b40000000h
                    DQ 03fc5cfa470000000h
                    DQ 03fc5f52440000000h
                    DQ 03fc61a8ad0000000h
                    DQ 03fc63fd850000000h
                    DQ 03fc6650cd0000000h
                    DQ 03fc68a2880000000h
                    DQ 03fc6af2b80000000h
                    DQ 03fc6d415e0000000h
                    DQ 03fc6f8e7d0000000h
                    DQ 03fc71da170000000h
                    DQ 03fc74242e0000000h
                    DQ 03fc766cc40000000h
                    DQ 03fc78b3da0000000h
                    DQ 03fc7af9730000000h
                    DQ 03fc7d3d910000000h
                    DQ 03fc7f80350000000h
                    DQ 03fc81c1620000000h
                    DQ 03fc8401190000000h
                    DQ 03fc863f5c0000000h
                    DQ 03fc887c2e0000000h
                    DQ 03fc8ab7900000000h
                    DQ 03fc8cf1830000000h
                    DQ 03fc8f2a0a0000000h
                    DQ 03fc9161270000000h
                    DQ 03fc9396db0000000h
                    DQ 03fc95cb280000000h
                    DQ 03fc97fe100000000h
                    DQ 03fc9a2f950000000h
                    DQ 03fc9c5fb70000000h
                    DQ 03fc9e8e7b0000000h
                    DQ 03fca0bbdf0000000h
                    DQ 03fca2e7e80000000h
                    DQ 03fca512960000000h
                    DQ 03fca73bea0000000h
                    DQ 03fca963e70000000h
                    DQ 03fcab8a8f0000000h
                    DQ 03fcadafe20000000h
                    DQ 03fcafd3e30000000h
                    DQ 03fcb1f6930000000h
                    DQ 03fcb417f40000000h
                    DQ 03fcb638070000000h
                    DQ 03fcb856cf0000000h
                    DQ 03fcba744b0000000h
                    DQ 03fcbc907f0000000h
                    DQ 03fcbeab6c0000000h
                    DQ 03fcc0c5130000000h
                    DQ 03fcc2dd750000000h
                    DQ 03fcc4f4950000000h
                    DQ 03fcc70a740000000h
                    DQ 03fcc91f130000000h
                    DQ 03fccb32740000000h
                    DQ 03fccd44980000000h
                    DQ 03fccf55810000000h
                    DQ 03fcd165300000000h
                    DQ 03fcd373a60000000h
                    DQ 03fcd580e60000000h
                    DQ 03fcd78cf00000000h
                    DQ 03fcd997c70000000h
                    DQ 03fcdba16a0000000h
                    DQ 03fcdda9dd0000000h
                    DQ 03fcdfb11f0000000h
                    DQ 03fce1b7330000000h
                    DQ 03fce3bc1a0000000h
                    DQ 03fce5bfd50000000h
                    DQ 03fce7c2660000000h
                    DQ 03fce9c3ce0000000h
                    DQ 03fcebc40e0000000h
                    DQ 03fcedc3280000000h
                    DQ 03fcefc11d0000000h
                    DQ 03fcf1bdee0000000h
                    DQ 03fcf3b99d0000000h
                    DQ 03fcf5b42a0000000h
                    DQ 03fcf7ad980000000h
                    DQ 03fcf9a5e70000000h
                    DQ 03fcfb9d190000000h
                    DQ 03fcfd932f0000000h
                    DQ 03fcff882a0000000h
                    DQ 03fd00be050000000h
                    DQ 03fd01b76a0000000h
                    DQ 03fd02b0430000000h
                    DQ 03fd03a8910000000h
                    DQ 03fd04a0540000000h
                    DQ 03fd05978e0000000h
                    DQ 03fd068e3f0000000h
                    DQ 03fd0784670000000h
                    DQ 03fd087a080000000h
                    DQ 03fd096f210000000h
                    DQ 03fd0a63b30000000h
                    DQ 03fd0b57bf0000000h
                    DQ 03fd0c4b450000000h
                    DQ 03fd0d3e460000000h
                    DQ 03fd0e30c30000000h
                    DQ 03fd0f22bc0000000h
                    DQ 03fd1014310000000h
                    DQ 03fd1105240000000h
                    DQ 03fd11f5940000000h
                    DQ 03fd12e5830000000h
                    DQ 03fd13d4f00000000h
                    DQ 03fd14c3dd0000000h
                    DQ 03fd15b24a0000000h
                    DQ 03fd16a0370000000h
                    DQ 03fd178da50000000h
                    DQ 03fd187a940000000h
                    DQ 03fd1967060000000h
                    DQ 03fd1a52fa0000000h
                    DQ 03fd1b3e710000000h
                    DQ 03fd1c296c0000000h
                    DQ 03fd1d13eb0000000h
                    DQ 03fd1dfdef0000000h
                    DQ 03fd1ee7770000000h
                    DQ 03fd1fd0860000000h
                    DQ 03fd20b91a0000000h
                    DQ 03fd21a1350000000h
                    DQ 03fd2288d70000000h
                    DQ 03fd2370010000000h
                    DQ 03fd2456b30000000h
                    DQ 03fd253ced0000000h
                    DQ 03fd2622b00000000h
                    DQ 03fd2707fd0000000h
                    DQ 03fd27ecd40000000h
                    DQ 03fd28d1360000000h
                    DQ 03fd29b5220000000h
                    DQ 03fd2a989a0000000h
                    DQ 03fd2b7b9e0000000h
                    DQ 03fd2c5e2e0000000h
                    DQ 03fd2d404b0000000h
                    DQ 03fd2e21f50000000h
                    DQ 03fd2f032c0000000h
                    DQ 03fd2fe3f20000000h
                    DQ 03fd30c4470000000h
                    DQ 03fd31a42b0000000h
                    DQ 03fd32839e0000000h
                    DQ 03fd3362a10000000h
                    DQ 03fd3441350000000h

ALIGN 16
L__log_256_tail     DQ 00000000000000000h
                    DQ 03db20abc22b2208fh
                    DQ 03db10f69332e0dd4h
                    DQ 03dce950de87ed257h
                    DQ 03dd3f3443b626d69h
                    DQ 03df45aeaa5363e57h
                    DQ 03dc443683ce1bf0bh
                    DQ 03df989cd60c6a511h
                    DQ 03dfd626f201f2e9fh
                    DQ 03de94f8bb8dabdcdh
                    DQ 03e0088d8ef423015h
                    DQ 03e080413a62b79adh
                    DQ 03e059717c0eed3c4h
                    DQ 03dad4a77add44902h
                    DQ 03e0e763ff037300eh
                    DQ 03de162d74706f6c3h
                    DQ 03e0601cc1f4dbc14h
                    DQ 03deaf3e051f6e5bfh
                    DQ 03e097a0b1e1af3ebh
                    DQ 03dc0a38970c002c7h
                    DQ 03e102e000057c751h
                    DQ 03e155b00eecd6e0eh
                    DQ 03ddf86297003b5afh
                    DQ 03e1057b9b336a36dh
                    DQ 03e134bc84a06ea4fh
                    DQ 03e1643da9ea1bcadh
                    DQ 03e1d66a7b4f7ea2ah
                    DQ 03df6b2e038f7fcefh
                    DQ 03df3e954c670f088h
                    DQ 03e047209093acab3h
                    DQ 03e1d708fe7275da7h
                    DQ 03e1fdf9e7771b9e7h
                    DQ 03e0827bfa70a0660h
                    DQ 03e1601cc1f4dbc14h
                    DQ 03e0637f6106a5e5bh
                    DQ 03e126a13f17c624bh
                    DQ 03e093eb2ce80623ah
                    DQ 03e1430d1e91594deh
                    DQ 03e1d6b10108fa031h
                    DQ 03e16879c0bbaf241h
                    DQ 03dff08015ea6bc2bh
                    DQ 03e29b63dcdc6676ch
                    DQ 03e2b022cbcc4ab2ch
                    DQ 03df917d07ddd6544h
                    DQ 03e1540605703379eh
                    DQ 03e0cd18b947a1b60h
                    DQ 03e17ad65277ca97eh
                    DQ 03e11884dc59f5fa9h
                    DQ 03e1711c46006d082h
                    DQ 03e2f092e3c3108f8h
                    DQ 03e1714c5e32be13ah
                    DQ 03e26bba7fd734f9ah
                    DQ 03dfdf48fb5e08483h
                    DQ 03e232f9bc74d0b95h
                    DQ 03df973e848790c13h
                    DQ 03e1eccbc08c6586eh
                    DQ 03e2115e9f9524a98h
                    DQ 03e2f1740593131b8h
                    DQ 03e1bcf8b25643835h
                    DQ 03e1f5fa81d8bed80h
                    DQ 03e244a4df929d9e4h
                    DQ 03e129820d8220c94h
                    DQ 03e2a0b489304e309h
                    DQ 03e1f4d56aba665feh
                    DQ 03e210c9019365163h
                    DQ 03df80f78fe592736h
                    DQ 03e10528825c81ccah
                    DQ 03de095537d6d746ah
                    DQ 03e1827bfa70a0660h
                    DQ 03e06b0a8ec45933ch
                    DQ 03e105af81bf5dba9h
                    DQ 03e17e2fa2655d515h
                    DQ 03e0d59ecbfaee4bfh
                    DQ 03e1d8b2fda683fa3h
                    DQ 03e24b8ddfd3a3737h
                    DQ 03e13827e61ae1204h
                    DQ 03e2c8c7b49e90f9fh
                    DQ 03e29eaf01597591dh
                    DQ 03e19aaa66e317b36h
                    DQ 03e2e725609720655h
                    DQ 03e261c33fc7aac54h
                    DQ 03e29662bcf61a252h
                    DQ 03e1843c811c42730h
                    DQ 03e2064bb0b5acb36h
                    DQ 03e0a340c842701a4h
                    DQ 03e1a8e55b58f79d6h
                    DQ 03de92d219c5e9d9ah
                    DQ 03e3f63e60d7ffd6ah
                    DQ 03e2e9b0ed9516314h
                    DQ 03e2923901962350ch
                    DQ 03e326f8838785e81h
                    DQ 03e3b5b6a4caba6afh
                    DQ 03df0226adc8e761ch
                    DQ 03e3c4ad7313a1aedh
                    DQ 03e1564e87c738d17h
                    DQ 03e338fecf18a6618h
                    DQ 03e3d929ef5777666h
                    DQ 03e39483bf08da0b8h
                    DQ 03e3bdd0eeeaa5826h
                    DQ 03e39c4dd590237bah
                    DQ 03e1af3e9e0ebcac7h
                    DQ 03e35ce5382270dach
                    DQ 03e394f74532ab9bah
                    DQ 03e07342795888654h
                    DQ 03e0c5a000be34bf0h
                    DQ 03e2711c46006d082h
                    DQ 03e250025b4ed8cf8h
                    DQ 03e2ed18bcef2d2a0h
                    DQ 03e21282e0c0a7554h
                    DQ 03e0d70f33359a7cah
                    DQ 03e2b7f7e13a84025h
                    DQ 03e33306ec321891eh
                    DQ 03e3fc7f8038b7550h
                    DQ 03e3eb0358cd71d64h
                    DQ 03e3a76c822859474h
                    DQ 03e3d0ec652de86e3h
                    DQ 03e2fa4cce08658afh
                    DQ 03e3b84a2d2c00a9eh
                    DQ 03e20a5b0f2c25bd1h
                    DQ 03e3dd660225bf699h
                    DQ 03e08b10f859bf037h
                    DQ 03e3e8823b590cbe1h
                    DQ 03e361311f31e96f6h
                    DQ 03e2e1f875ca20f9ah
                    DQ 03e2c95724939b9a5h
                    DQ 03e3805957a3e58e2h
                    DQ 03e2ff126ea9f0334h
                    DQ 03e3953f5598e5609h
                    DQ 03e36c16ff856c448h
                    DQ 03e24cb220ff261f4h
                    DQ 03e35e120d53d53a2h
                    DQ 03e3a527f6189f256h
                    DQ 03e3856fcffd49c0fh
                    DQ 03e300c2e8228d7dah
                    DQ 03df113d09444dfe0h
                    DQ 03e2510630eea59a6h
                    DQ 03e262e780f32d711h
                    DQ 03ded3ed91a10f8cfh
                    DQ 03e23654a7e4bcd85h
                    DQ 03e055b784980ad21h
                    DQ 03e212f2dd4b16e64h
                    DQ 03e37c4add939f50ch
                    DQ 03e281784627180fch
                    DQ 03dea5162c7e14961h
                    DQ 03e310c9019365163h
                    DQ 03e373c4d2ba17688h
                    DQ 03e2ae8a5e0e93d81h
                    DQ 03e2ab0c6f01621afh
                    DQ 03e301e8b74dd5b66h
                    DQ 03e2d206fecbb5494h
                    DQ 03df0b48b724fcc00h
                    DQ 03e3f831f0b61e229h
                    DQ 03df81a97c407bcafh
                    DQ 03e3e286c1ccbb7aah
                    DQ 03e28630b49220a93h
                    DQ 03dff0b15c1a22c5ch
                    DQ 03e355445e71c0946h
                    DQ 03e3be630f8066d85h
                    DQ 03e2599dff0d96c39h
                    DQ 03e36cc85b18fb081h
                    DQ 03e34476d001ea8c8h
                    DQ 03e373f889e16d31fh
                    DQ 03e3357100d792a87h
                    DQ 03e3bd179ae6101f6h
                    DQ 03e0ca31056c3f6e2h
                    DQ 03e3d2870629c08fbh
                    DQ 03e3aba3880d2673fh
                    DQ 03e2c3633cb297da6h
                    DQ 03e21843899efea02h
                    DQ 03e3bccc99d2008e6h
                    DQ 03e38000544bdd350h
                    DQ 03e2b91c226606ae1h
                    DQ 03e2a7adf26b62bdfh
                    DQ 03e18764fc8826ec9h
                    DQ 03e1f4f3de50f68f0h
                    DQ 03df760ca757995e3h
                    DQ 03dfc667ed3805147h
                    DQ 03e3733f6196adf6fh
                    DQ 03e2fb710f33e836bh
                    DQ 03e39886eba641013h
                    DQ 03dfb5368d0af8c1ah
                    DQ 03e358c691b8d2971h
                    DQ 03dfe9465226d08fbh
                    DQ 03e33587e063f0097h
                    DQ 03e3618e702129f18h
                    DQ 03e361c33fc7aac54h
                    DQ 03e3f07a68408604ah
                    DQ 03e3c34bfe4945421h
                    DQ 03e38b1f00e41300bh
                    DQ 03e3f434284d61b63h
                    DQ 03e3a63095e397436h
                    DQ 03e34428656b919deh
                    DQ 03e36ca9201b2d9a6h
                    DQ 03e2738823a2a931ch
                    DQ 03e3c11880e179230h
                    DQ 03e313ddc8d6d52feh
                    DQ 03e33eed58922e917h
                    DQ 03e295992846bdd50h
                    DQ 03e0ddb4d5f2e278bh
                    DQ 03df1a5f12a0635c4h
                    DQ 03e4642f0882c3c34h
                    DQ 03e2aee9ba7f6475eh
                    DQ 03e264b7f834a60e4h
                    DQ 03e290d42e243792eh
                    DQ 03e4c272008134f01h
                    DQ 03e4a782e16d6cf5bh
                    DQ 03e44505c79da6648h
                    DQ 03e4ca9d4ea4dcd21h
                    DQ 03e297d3d627cd5bch
                    DQ 03e20b15cf9bcaa13h
                    DQ 03e315b2063cf76ddh
                    DQ 03e2983e6f3aa2748h
                    DQ 03e3f4c64f4ffe994h
                    DQ 03e46beba7ce85a0fh
                    DQ 03e3b9c69fd4ea6b8h
                    DQ 03e2b6aa5835fa4abh
                    DQ 03e43ccc3790fedd1h
                    DQ 03e29c04cc4404fe0h
                    DQ 03e40734b7a75d89dh
                    DQ 03e1b4404c4e01612h
                    DQ 03e40c565c2ce4894h
                    DQ 03e33c71441d935cdh
                    DQ 03d72a492556b3b4eh
                    DQ 03e20fa090341dc43h
                    DQ 03e2e8f7009e3d9f4h
                    DQ 03e4b1bf68b048a45h
                    DQ 03e3eee52dffaa956h
                    DQ 03e456b0900e465bdh
                    DQ 03e4d929ef5777666h
                    DQ 03e486ea28637e260h
                    DQ 03e4665aff10ca2f0h
                    DQ 03e2f11fdaf48ec74h
                    DQ 03e4cbe1b86a4d1c7h
                    DQ 03e25b05bfea87665h
                    DQ 03e41cec20a1a4a1dh
                    DQ 03e41cd5f0a409b9fh
                    DQ 03e453656c8265070h
                    DQ 03e377ed835282260h
                    DQ 03e2417bc3040b9d2h
                    DQ 03e408eef7b79eff2h
                    DQ 03e4dc76f39dc57e9h
                    DQ 03e4c0493a70cf457h
                    DQ 03e4a83d6cea5a60ch
                    DQ 03e30d6700dc557bah
                    DQ 03e44c96c12e8bd0ah
                    DQ 03e3d2c1993e32315h
                    DQ 03e22c721135f8242h
                    DQ 03e279a3e4dda747dh
                    DQ 03dfcf89f6941a72bh
                    DQ 03e2149a702f10831h
                    DQ 03e4ead4b7c8175dbh
                    DQ 03e4e6930fe63e70ah
                    DQ 03e41e106bed9ee2fh
                    DQ 03e2d682b82f11c92h
                    DQ 03e3a07f188dba47ch
                    DQ 03e40f9342dc172f6h
                    DQ 03e03ef3fde623e25h


ALIGN 16
L__log_F_inv        DQ 04000000000000000h
                    DQ 03fffe01fe01fe020h
                    DQ 03fffc07f01fc07f0h
                    DQ 03fffa11caa01fa12h
                    DQ 03fff81f81f81f820h
                    DQ 03fff6310aca0dbb5h
                    DQ 03fff44659e4a4271h
                    DQ 03fff25f644230ab5h
                    DQ 03fff07c1f07c1f08h
                    DQ 03ffee9c7f8458e02h
                    DQ 03ffecc07b301ecc0h
                    DQ 03ffeae807aba01ebh
                    DQ 03ffe9131abf0b767h
                    DQ 03ffe741aa59750e4h
                    DQ 03ffe573ac901e574h
                    DQ 03ffe3a9179dc1a73h
                    DQ 03ffe1e1e1e1e1e1eh
                    DQ 03ffe01e01e01e01eh
                    DQ 03ffde5d6e3f8868ah
                    DQ 03ffdca01dca01dcah
                    DQ 03ffdae6076b981dbh
                    DQ 03ffd92f2231e7f8ah
                    DQ 03ffd77b654b82c34h
                    DQ 03ffd5cac807572b2h
                    DQ 03ffd41d41d41d41dh
                    DQ 03ffd272ca3fc5b1ah
                    DQ 03ffd0cb58f6ec074h
                    DQ 03ffcf26e5c44bfc6h
                    DQ 03ffcd85689039b0bh
                    DQ 03ffcbe6d9601cbe7h
                    DQ 03ffca4b3055ee191h
                    DQ 03ffc8b265afb8a42h
                    DQ 03ffc71c71c71c71ch
                    DQ 03ffc5894d10d4986h
                    DQ 03ffc3f8f01c3f8f0h
                    DQ 03ffc26b5392ea01ch
                    DQ 03ffc0e070381c0e0h
                    DQ 03ffbf583ee868d8bh
                    DQ 03ffbdd2b899406f7h
                    DQ 03ffbc4fd65883e7bh
                    DQ 03ffbacf914c1bad0h
                    DQ 03ffb951e2b18ff23h
                    DQ 03ffb7d6c3dda338bh
                    DQ 03ffb65e2e3beee05h
                    DQ 03ffb4e81b4e81b4fh
                    DQ 03ffb37484ad806ceh
                    DQ 03ffb2036406c80d9h
                    DQ 03ffb094b31d922a4h
                    DQ 03ffaf286bca1af28h
                    DQ 03ffadbe87f94905eh
                    DQ 03ffac5701ac5701bh
                    DQ 03ffaaf1d2f87ebfdh
                    DQ 03ffa98ef606a63beh
                    DQ 03ffa82e65130e159h
                    DQ 03ffa6d01a6d01a6dh
                    DQ 03ffa574107688a4ah
                    DQ 03ffa41a41a41a41ah
                    DQ 03ffa2c2a87c51ca0h
                    DQ 03ffa16d3f97a4b02h
                    DQ 03ffa01a01a01a01ah
                    DQ 03ff9ec8e951033d9h
                    DQ 03ff9d79f176b682dh
                    DQ 03ff9c2d14ee4a102h
                    DQ 03ff9ae24ea5510dah
                    DQ 03ff999999999999ah
                    DQ 03ff9852f0d8ec0ffh
                    DQ 03ff970e4f80cb872h
                    DQ 03ff95cbb0be377aeh
                    DQ 03ff948b0fcd6e9e0h
                    DQ 03ff934c67f9b2ce6h
                    DQ 03ff920fb49d0e229h
                    DQ 03ff90d4f120190d5h
                    DQ 03ff8f9c18f9c18fah
                    DQ 03ff8e6527af1373fh
                    DQ 03ff8d3018d3018d3h
                    DQ 03ff8bfce8062ff3ah
                    DQ 03ff8acb90f6bf3aah
                    DQ 03ff899c0f601899ch
                    DQ 03ff886e5f0abb04ah
                    DQ 03ff87427bcc092b9h
                    DQ 03ff8618618618618h
                    DQ 03ff84f00c2780614h
                    DQ 03ff83c977ab2beddh
                    DQ 03ff82a4a0182a4a0h
                    DQ 03ff8181818181818h
                    DQ 03ff8060180601806h
                    DQ 03ff7f405fd017f40h
                    DQ 03ff7e225515a4f1dh
                    DQ 03ff7d05f417d05f4h
                    DQ 03ff7beb3922e017ch
                    DQ 03ff7ad2208e0ecc3h
                    DQ 03ff79baa6bb6398bh
                    DQ 03ff78a4c8178a4c8h
                    DQ 03ff77908119ac60dh
                    DQ 03ff767dce434a9b1h
                    DQ 03ff756cac201756dh
                    DQ 03ff745d1745d1746h
                    DQ 03ff734f0c541fe8dh
                    DQ 03ff724287f46debch
                    DQ 03ff713786d9c7c09h
                    DQ 03ff702e05c0b8170h
                    DQ 03ff6f26016f26017h
                    DQ 03ff6e1f76b4337c7h
                    DQ 03ff6d1a62681c861h
                    DQ 03ff6c16c16c16c17h
                    DQ 03ff6b1490aa31a3dh
                    DQ 03ff6a13cd1537290h
                    DQ 03ff691473a88d0c0h
                    DQ 03ff6816816816817h
                    DQ 03ff6719f3601671ah
                    DQ 03ff661ec6a5122f9h
                    DQ 03ff6524f853b4aa3h
                    DQ 03ff642c8590b2164h
                    DQ 03ff63356b88ac0deh
                    DQ 03ff623fa77016240h
                    DQ 03ff614b36831ae94h
                    DQ 03ff6058160581606h
                    DQ 03ff5f66434292dfch
                    DQ 03ff5e75bb8d015e7h
                    DQ 03ff5d867c3ece2a5h
                    DQ 03ff5c9882b931057h
                    DQ 03ff5babcc647fa91h
                    DQ 03ff5ac056b015ac0h
                    DQ 03ff59d61f123ccaah
                    DQ 03ff58ed2308158edh
                    DQ 03ff5805601580560h
                    DQ 03ff571ed3c506b3ah
                    DQ 03ff56397ba7c52e2h
                    DQ 03ff5555555555555h
                    DQ 03ff54725e6bb82feh
                    DQ 03ff5390948f40febh
                    DQ 03ff52aff56a8054bh
                    DQ 03ff51d07eae2f815h
                    DQ 03ff50f22e111c4c5h
                    DQ 03ff5015015015015h
                    DQ 03ff4f38f62dd4c9bh
                    DQ 03ff4e5e0a72f0539h
                    DQ 03ff4d843bedc2c4ch
                    DQ 03ff4cab88725af6eh
                    DQ 03ff4bd3edda68fe1h
                    DQ 03ff4afd6a052bf5bh
                    DQ 03ff4a27fad76014ah
                    DQ 03ff49539e3b2d067h
                    DQ 03ff4880522014880h
                    DQ 03ff47ae147ae147bh
                    DQ 03ff46dce34596066h
                    DQ 03ff460cbc7f5cf9ah
                    DQ 03ff453d9e2c776cah
                    DQ 03ff446f86562d9fbh
                    DQ 03ff43a2730abee4dh
                    DQ 03ff42d6625d51f87h
                    DQ 03ff420b5265e5951h
                    DQ 03ff4141414141414h
                    DQ 03ff40782d10e6566h
                    DQ 03ff3fb013fb013fbh
                    DQ 03ff3ee8f42a5af07h
                    DQ 03ff3e22cbce4a902h
                    DQ 03ff3d5d991aa75c6h
                    DQ 03ff3c995a47babe7h
                    DQ 03ff3bd60d9232955h
                    DQ 03ff3b13b13b13b14h
                    DQ 03ff3a524387ac822h
                    DQ 03ff3991c2c187f63h
                    DQ 03ff38d22d366088eh
                    DQ 03ff3813813813814h
                    DQ 03ff3755bd1c945eeh
                    DQ 03ff3698df3de0748h
                    DQ 03ff35dce5f9f2af8h
                    DQ 03ff3521cfb2b78c1h
                    DQ 03ff34679ace01346h
                    DQ 03ff33ae45b57bcb2h
                    DQ 03ff32f5ced6a1dfah
                    DQ 03ff323e34a2b10bfh
                    DQ 03ff3187758e9ebb6h
                    DQ 03ff30d190130d190h
                    DQ 03ff301c82ac40260h
                    DQ 03ff2f684bda12f68h
                    DQ 03ff2eb4ea1fed14bh
                    DQ 03ff2e025c04b8097h
                    DQ 03ff2d50a012d50a0h
                    DQ 03ff2c9fb4d812ca0h
                    DQ 03ff2bef98e5a3711h
                    DQ 03ff2b404ad012b40h
                    DQ 03ff2a91c92f3c105h
                    DQ 03ff29e4129e4129eh
                    DQ 03ff293725bb804a5h
                    DQ 03ff288b01288b013h
                    DQ 03ff27dfa38a1ce4dh
                    DQ 03ff27350b8812735h
                    DQ 03ff268b37cd60127h
                    DQ 03ff25e22708092f1h
                    DQ 03ff2539d7e9177b2h
                    DQ 03ff2492492492492h
                    DQ 03ff23eb79717605bh
                    DQ 03ff23456789abcdfh
                    DQ 03ff22a0122a0122ah
                    DQ 03ff21fb78121fb78h
                    DQ 03ff21579804855e6h
                    DQ 03ff20b470c67c0d9h
                    DQ 03ff2012012012012h
                    DQ 03ff1f7047dc11f70h
                    DQ 03ff1ecf43c7fb84ch
                    DQ 03ff1e2ef3b3fb874h
                    DQ 03ff1d8f5672e4abdh
                    DQ 03ff1cf06ada2811dh
                    DQ 03ff1c522fc1ce059h
                    DQ 03ff1bb4a4046ed29h
                    DQ 03ff1b17c67f2bae3h
                    DQ 03ff1a7b9611a7b96h
                    DQ 03ff19e0119e0119eh
                    DQ 03ff19453808ca29ch
                    DQ 03ff18ab083902bdbh
                    DQ 03ff1811811811812h
                    DQ 03ff1778a191bd684h
                    DQ 03ff16e0689427379h
                    DQ 03ff1648d50fc3201h
                    DQ 03ff15b1e5f75270dh
                    DQ 03ff151b9a3fdd5c9h
                    DQ 03ff1485f0e0acd3bh
                    DQ 03ff13f0e8d344724h
                    DQ 03ff135c81135c811h
                    DQ 03ff12c8b89edc0ach
                    DQ 03ff12358e75d3033h
                    DQ 03ff11a3019a74826h
                    DQ 03ff1111111111111h
                    DQ 03ff107fbbe011080h
                    DQ 03ff0fef010fef011h
                    DQ 03ff0f5edfab325a2h
                    DQ 03ff0ecf56be69c90h
                    DQ 03ff0e40655826011h
                    DQ 03ff0db20a88f4696h
                    DQ 03ff0d24456359e3ah
                    DQ 03ff0c9714fbcda3bh
                    DQ 03ff0c0a7868b4171h
                    DQ 03ff0b7e6ec259dc8h
                    DQ 03ff0af2f722eecb5h
                    DQ 03ff0a6810a6810a7h
                    DQ 03ff09ddba6af8360h
                    DQ 03ff0953f39010954h
                    DQ 03ff08cabb37565e2h
                    DQ 03ff0842108421084h
                    DQ 03ff07b9f29b8eae2h
                    DQ 03ff073260a47f7c6h
                    DQ 03ff06ab59c7912fbh
                    DQ 03ff0624dd2f1a9fch
                    DQ 03ff059eea0727586h
                    DQ 03ff05197f7d73404h
                    DQ 03ff04949cc1664c5h
                    DQ 03ff0410410410410h
                    DQ 03ff038c6b78247fch
                    DQ 03ff03091b51f5e1ah
                    DQ 03ff02864fc7729e9h
                    DQ 03ff0204081020408h
                    DQ 03ff0182436517a37h
                    DQ 03ff0101010101010h
                    DQ 03ff0080402010080h
                    DQ 03ff0000000000000h
                    DQ 00000000000000000h


CONST    ENDS
data ENDS
END

