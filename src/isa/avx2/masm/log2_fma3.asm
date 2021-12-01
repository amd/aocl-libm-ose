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
; log2_fma3.S
;
; An implementation of the log libm function.
;
; Prototype:
;
;     double log2(double x);
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

ALM_PROTO_FMA3 log2

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
    xor       rax,rax
    vpsrlq    xmm3,xmm0,52
    vmovq     rax,xmm0
    vpsubq    xmm3,xmm3,QWORD PTR L__mask_1023
    vcvtdq2pd xmm6,xmm3 ; xexp replaced VCVTDQ2PD
    
    ;  NaN or inf
    vpand    xmm5,xmm0,QWORD PTR L__real_inf
    vcomisd    xmm5,QWORD PTR L__real_inf
    je          L__x_is_inf_or_nan
    
    ; check for negative numbers or zero
    vpxor    xmm5,xmm5,xmm5
    vcomisd    xmm0,xmm5
    jbe         L__x_is_zero_or_neg

    vpand    xmm2,xmm0,QWORD PTR L__real_mant
    vsubsd    xmm4,xmm0,QWORD PTR L__real_one

    vcomisd    xmm6,QWORD PTR L__mask_1023_f
    je          L__denormal_adjust

L__continue_common:    
    ; compute index into the log tables
    vpand    xmm1,xmm0,QWORD PTR L__mask_mant_all8
    vpand    xmm3,xmm0,QWORD PTR L__mask_mant9
    vpsllq   xmm3,xmm3,1 
    vpaddq   xmm1,xmm3,xmm1
    vmovq    rax,xmm1

    ; near one codepath
    vpand    xmm4,xmm4,QWORD PTR L__real_notsign
    vcomisd    xmm4,QWORD PTR L__real_threshold
    jb           L__near_one

    ; F,Y
    shr    rax,44
    vpor    xmm2,xmm2,QWORD PTR L__real_half
    vpor    xmm1,xmm1,QWORD PTR L__real_half
    lea    r9,L__log_F_inv

    ; f = F - Y,r = f * inv
    vsubsd    xmm1,xmm1,xmm2
    vmulsd    xmm1,xmm1,QWORD PTR [r9 + rax * 8]

    lea    r9,QWORD PTR L__log_256_lead
    lea    rdx,QWORD PTR L__log_256_tail
    vmovsd    xmm2,QWORD PTR [rdx + rax * 8]
    movsd    xmm0,QWORD PTR [r9 + rax * 8]
    vaddsd    xmm0,xmm0,xmm6

    ; poly
    vmulsd    xmm6,xmm1,xmm1 ; r*r
    vmovsd    xmm3,QWORD PTR L__real_1_over_6
    vmovsd    xmm5,QWORD PTR L__real_1_over_3
    vfmadd213sd    xmm3,xmm1,QWORD PTR L__real_1_over_5 ; r*1/6 + 1/5
    vfmadd213sd    xmm5,xmm1,QWORD PTR L__real_1_over_2 ; 1/2+r*1/3
    movsd    xmm4,xmm6 ; r*r
    vfmadd213sd    xmm3 ,xmm1,QWORD PTR L__real_1_over_4 ;1/4+(1/5*r+r*r*1/6)
    
    vmulsd    xmm4,xmm6,xmm6                   ; r*r*r*r
    vfmadd231sd    xmm1 ,xmm5,xmm6 ; r*r*(1/2+r*1/3) + r
    vfmadd231sd    xmm1 ,xmm3,xmm4 ; poly

    ; m*log(2) + log(G) - poly
    vfnmadd231sd    xmm2 ,xmm1,QWORD PTR L__real_log2_e
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


    vpand    xmm3,xmm0,QWORD PTR L__mask_lower
    vsubsd    xmm0,xmm0,xmm3
    vaddsd    xmm4,xmm4,xmm0

    vmulsd    xmm1,xmm4,QWORD PTR L__real_log2_e_lead
    vmulsd    xmm4,xmm4,QWORD PTR L__real_log2_e_tail
    vmulsd    xmm0,xmm3,QWORD PTR L__real_log2_e_tail
    vmulsd    xmm3,xmm3,QWORD PTR L__real_log2_e_lead

    vaddsd    xmm0,xmm0,xmm4
    vaddsd    xmm0,xmm0,xmm1
    vaddsd    xmm0,xmm0,xmm3
 
   
     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


L__denormal_adjust:
    vpor    xmm2,xmm2,QWORD PTR L__real_one
    vsubsd    xmm2,xmm2,QWORD PTR L__real_one
    vpsrlq    xmm5,xmm2,52
    vpand    xmm2,xmm2,QWORD PTR L__real_mant
    vmovapd    xmm0,xmm2
    vpsubd    xmm5,xmm5,DWORD PTR L__mask_2045
    vcvtdq2pd    xmm6,xmm5
    jmp         L__continue_common

ALIGN  16
L__x_is_zero_or_neg:
    jne    L__x_is_neg
    vmovsd xmm1,QWORD PTR L__real_ninf
    mov    r8d,DWORD PTR L__flag_x_zero
    call   fname_special
   
     RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret
   

ALIGN  16
L__x_is_neg:

    vmovsd xmm1,QWORD PTR L__neg_real_qnan
    mov    r8d,DWORD PTR L__flag_x_neg
    call   fname_special
   
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

L__real_ninf       DQ 00fff0000000000000h   ; -inf
                    DQ 00000000000000000h
L__real_inf        DQ 07ff0000000000000h    ; +inf
                    DQ 00000000000000000h
L__neg_real_qnan    DQ 0fff8000000000000h   ; neg qNaN
                    DQ 0000000000000000h
L__real_qnan       DQ 07ff8000000000000h   ; qNaN
                    DQ 00000000000000000h
L__real_qnanbit    DQ 00008000000000000h
                    DQ 00000000000000000h
L__real_mant       DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                    DQ 00000000000000000h
L__mask_1023       DQ 000000000000003ffh
                    DQ 00000000000000000h
L__mask_001        DQ 00000000000000001h
                    DQ 00000000000000000h

L__mask_mant_all8  DQ 0000ff00000000000h
                    DQ 00000000000000000h
L__mask_mant9      DQ 00000080000000000h
                    DQ 00000000000000000h
L__real_log2_e         DQ 03ff71547652b82feh
                        DQ 00000000000000000h

L__real_log2_e_lead    DQ 03ff7154400000000h ; log2e_lead 1.44269180297851562500E+00
                        DQ 00000000000000000h
L__real_log2_e_tail    DQ 03ecb295c17f0bbbeh ; log2e_tail 3.23791044778235969970E-06
                        DQ 00000000000000000h

L__real_two        DQ 04000000000000000h ; 2
                    DQ 00000000000000000h

L__real_one        DQ 03ff0000000000000h ; 1
                    DQ 00000000000000000h

L__real_half       DQ 03fe0000000000000h ; 1/2
                    DQ 00000000000000000h

L__mask_100        DQ 00000000000000100h
                    DQ 00000000000000000h

L__real_1_over_512     DQ 03f60000000000000h
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
L__real_ca4        DQ 03f3c8034c85dfff0h ; 4.34887777707614552256e-04
                    DQ 00000000000000000h
L__mask_lower      DQ 00ffffffff00000000h
                    DQ 00000000000000000h



ALIGN 16
L__log_256_lead     DQ 00000000000000000h
                    DQ 03f7709c460000000h
                    DQ 03f86fe50b0000000h
                    DQ 03f91363110000000h
                    DQ 03f96e79680000000h
                    DQ 03f9c9363b0000000h
                    DQ 03fa11cd1d0000000h
                    DQ 03fa3ed3090000000h
                    DQ 03fa6bad370000000h
                    DQ 03fa985bfc0000000h
                    DQ 03fac4dfab0000000h
                    DQ 03faf138980000000h
                    DQ 03fb0eb3890000000h
                    DQ 03fb24b5b70000000h
                    DQ 03fb3aa2fd0000000h
                    DQ 03fb507b830000000h
                    DQ 03fb663f6f0000000h
                    DQ 03fb7beee90000000h
                    DQ 03fb918a160000000h
                    DQ 03fba7111d0000000h
                    DQ 03fbbc84240000000h
                    DQ 03fbd1e34e0000000h
                    DQ 03fbe72ec10000000h
                    DQ 03fbfc66a00000000h
                    DQ 03fc08c5880000000h
                    DQ 03fc134e1b0000000h
                    DQ 03fc1dcd190000000h
                    DQ 03fc2842940000000h
                    DQ 03fc32ae9e0000000h
                    DQ 03fc3d11460000000h
                    DQ 03fc476a9f0000000h
                    DQ 03fc51bab90000000h
                    DQ 03fc5c01a30000000h
                    DQ 03fc663f6f0000000h
                    DQ 03fc70742d0000000h
                    DQ 03fc7a9fec0000000h
                    DQ 03fc84c2bd0000000h
                    DQ 03fc8edcae0000000h
                    DQ 03fc98edd00000000h
                    DQ 03fca2f6320000000h
                    DQ 03fcacf5e20000000h
                    DQ 03fcb6ecf10000000h
                    DQ 03fcc0db6c0000000h
                    DQ 03fccac1630000000h
                    DQ 03fcd49ee40000000h
                    DQ 03fcde73fe0000000h
                    DQ 03fce840be0000000h
                    DQ 03fcf205330000000h
                    DQ 03fcfbc16b0000000h
                    DQ 03fd02baba0000000h
                    DQ 03fd0790ad0000000h
                    DQ 03fd0c62970000000h
                    DQ 03fd11307d0000000h
                    DQ 03fd15fa670000000h
                    DQ 03fd1ac05b0000000h
                    DQ 03fd1f825f0000000h
                    DQ 03fd24407a0000000h
                    DQ 03fd28fab30000000h
                    DQ 03fd2db10f0000000h
                    DQ 03fd3263960000000h
                    DQ 03fd37124c0000000h
                    DQ 03fd3bbd3a0000000h
                    DQ 03fd4064630000000h
                    DQ 03fd4507cf0000000h
                    DQ 03fd49a7840000000h
                    DQ 03fd4e43880000000h
                    DQ 03fd52dbdf0000000h
                    DQ 03fd5770910000000h
                    DQ 03fd5c01a30000000h
                    DQ 03fd608f1b0000000h
                    DQ 03fd6518fe0000000h
                    DQ 03fd699f520000000h
                    DQ 03fd6e221c0000000h
                    DQ 03fd72a1630000000h
                    DQ 03fd771d2b0000000h
                    DQ 03fd7b957a0000000h
                    DQ 03fd800a560000000h
                    DQ 03fd847bc30000000h
                    DQ 03fd88e9c70000000h
                    DQ 03fd8d54670000000h
                    DQ 03fd91bba80000000h
                    DQ 03fd961f900000000h
                    DQ 03fd9a80230000000h
                    DQ 03fd9edd670000000h
                    DQ 03fda337600000000h
                    DQ 03fda78e140000000h
                    DQ 03fdabe1870000000h
                    DQ 03fdb031be0000000h
                    DQ 03fdb47ebf0000000h
                    DQ 03fdb8c88d0000000h
                    DQ 03fdbd0f2e0000000h
                    DQ 03fdc152a60000000h
                    DQ 03fdc592fa0000000h
                    DQ 03fdc9d02f0000000h
                    DQ 03fdce0a490000000h
                    DQ 03fdd2414c0000000h
                    DQ 03fdd6753e0000000h
                    DQ 03fddaa6220000000h
                    DQ 03fdded3fd0000000h
                    DQ 03fde2fed30000000h
                    DQ 03fde726aa0000000h
                    DQ 03fdeb4b840000000h
                    DQ 03fdef6d670000000h
                    DQ 03fdf38c560000000h
                    DQ 03fdf7a8560000000h
                    DQ 03fdfbc16b0000000h
                    DQ 03fdffd7990000000h
                    DQ 03fe01f5720000000h
                    DQ 03fe03fda80000000h
                    DQ 03fe0604710000000h
                    DQ 03fe0809cf0000000h
                    DQ 03fe0a0dc30000000h
                    DQ 03fe0c10500000000h
                    DQ 03fe0e11770000000h
                    DQ 03fe10113b0000000h
                    DQ 03fe120f9d0000000h
                    DQ 03fe140c9f0000000h
                    DQ 03fe1608440000000h
                    DQ 03fe18028c0000000h
                    DQ 03fe19fb7b0000000h
                    DQ 03fe1bf3110000000h
                    DQ 03fe1de9510000000h
                    DQ 03fe1fde3d0000000h
                    DQ 03fe21d1d50000000h
                    DQ 03fe23c41d0000000h
                    DQ 03fe25b5150000000h
                    DQ 03fe27a4c00000000h
                    DQ 03fe29931f0000000h
                    DQ 03fe2b80340000000h
                    DQ 03fe2d6c010000000h
                    DQ 03fe2f56870000000h
                    DQ 03fe313fc80000000h
                    DQ 03fe3327c60000000h
                    DQ 03fe350e830000000h
                    DQ 03fe36f3ff0000000h
                    DQ 03fe38d83e0000000h
                    DQ 03fe3abb3f0000000h
                    DQ 03fe3c9d060000000h
                    DQ 03fe3e7d930000000h
                    DQ 03fe405ce80000000h
                    DQ 03fe423b070000000h
                    DQ 03fe4417f20000000h
                    DQ 03fe45f3a90000000h
                    DQ 03fe47ce2f0000000h
                    DQ 03fe49a7840000000h
                    DQ 03fe4b7fab0000000h
                    DQ 03fe4d56a50000000h
                    DQ 03fe4f2c740000000h
                    DQ 03fe5101180000000h
                    DQ 03fe52d4940000000h
                    DQ 03fe54a6e80000000h
                    DQ 03fe5678170000000h
                    DQ 03fe5848220000000h
                    DQ 03fe5a170a0000000h
                    DQ 03fe5be4d00000000h
                    DQ 03fe5db1770000000h
                    DQ 03fe5f7cff0000000h
                    DQ 03fe61476a0000000h
                    DQ 03fe6310b80000000h
                    DQ 03fe64d8ed0000000h
                    DQ 03fe66a0080000000h
                    DQ 03fe68660c0000000h
                    DQ 03fe6a2af90000000h
                    DQ 03fe6beed20000000h
                    DQ 03fe6db1960000000h
                    DQ 03fe6f73480000000h
                    DQ 03fe7133e90000000h
                    DQ 03fe72f37a0000000h
                    DQ 03fe74b1fd0000000h
                    DQ 03fe766f720000000h
                    DQ 03fe782bdb0000000h
                    DQ 03fe79e73a0000000h
                    DQ 03fe7ba18f0000000h
                    DQ 03fe7d5adc0000000h
                    DQ 03fe7f13220000000h
                    DQ 03fe80ca620000000h
                    DQ 03fe82809d0000000h
                    DQ 03fe8435d50000000h
                    DQ 03fe85ea0b0000000h
                    DQ 03fe879d3f0000000h
                    DQ 03fe894f740000000h
                    DQ 03fe8b00aa0000000h
                    DQ 03fe8cb0e30000000h
                    DQ 03fe8e60200000000h
                    DQ 03fe900e610000000h
                    DQ 03fe91bba80000000h
                    DQ 03fe9367f60000000h
                    DQ 03fe95134d0000000h
                    DQ 03fe96bdad0000000h
                    DQ 03fe9867170000000h
                    DQ 03fe9a0f8d0000000h
                    DQ 03fe9bb70f0000000h
                    DQ 03fe9d5d9f0000000h
                    DQ 03fe9f033e0000000h
                    DQ 03fea0a7ed0000000h
                    DQ 03fea24bad0000000h
                    DQ 03fea3ee7f0000000h
                    DQ 03fea590640000000h
                    DQ 03fea7315d0000000h
                    DQ 03fea8d16b0000000h
                    DQ 03feaa708f0000000h
                    DQ 03feac0eca0000000h
                    DQ 03feadac1e0000000h
                    DQ 03feaf488b0000000h
                    DQ 03feb0e4120000000h
                    DQ 03feb27eb40000000h
                    DQ 03feb418730000000h
                    DQ 03feb5b14f0000000h
                    DQ 03feb749480000000h
                    DQ 03feb8e0620000000h
                    DQ 03feba769b0000000h
                    DQ 03febc0bf50000000h
                    DQ 03febda0710000000h
                    DQ 03febf34110000000h
                    DQ 03fec0c6d40000000h
                    DQ 03fec258bc0000000h
                    DQ 03fec3e9ca0000000h
                    DQ 03fec579fe0000000h
                    DQ 03fec7095a0000000h
                    DQ 03fec897df0000000h
                    DQ 03feca258d0000000h
                    DQ 03fecbb2660000000h
                    DQ 03fecd3e6a0000000h
                    DQ 03fecec9990000000h
                    DQ 03fed053f60000000h
                    DQ 03fed1dd810000000h
                    DQ 03fed3663b0000000h
                    DQ 03fed4ee240000000h
                    DQ 03fed6753e0000000h
                    DQ 03fed7fb890000000h
                    DQ 03fed981060000000h
                    DQ 03fedb05b60000000h
                    DQ 03fedc899a0000000h
                    DQ 03fede0cb30000000h
                    DQ 03fedf8f020000000h
                    DQ 03fee110860000000h
                    DQ 03fee291420000000h
                    DQ 03fee411360000000h
                    DQ 03fee590630000000h
                    DQ 03fee70eca0000000h
                    DQ 03fee88c6b0000000h
                    DQ 03feea09470000000h
                    DQ 03feeb855f0000000h
                    DQ 03feed00b40000000h
                    DQ 03feee7b470000000h
                    DQ 03feeff5180000000h
                    DQ 03fef16e280000000h
                    DQ 03fef2e6780000000h
                    DQ 03fef45e080000000h
                    DQ 03fef5d4da0000000h
                    DQ 03fef74aef0000000h
                    DQ 03fef8c0460000000h
                    DQ 03fefa34e10000000h
                    DQ 03fefba8c00000000h
                    DQ 03fefd1be40000000h
                    DQ 03fefe8e4f0000000h
                    DQ 03ff0000000000000h

ALIGN 16
L__log_256_tail     DQ 00000000000000000h
                    DQ 03deaf558ee95b37ah
                    DQ 03debbc2145fe38deh
                    DQ 03dfea5ec312ed069h
                    DQ 03df70b48a629b89fh
                    DQ 03e050a1f0cccdd01h
                    DQ 03e044cd04bb60514h
                    DQ 03e01a16898809d2dh
                    DQ 03e063bf61cc4d81bh
                    DQ 03dfa4a8ca305071dh
                    DQ 03e121556bde9f1f0h
                    DQ 03df9929cfd0e6835h
                    DQ 03e2f453f35679ee9h
                    DQ 03e2c26b47913459eh
                    DQ 03e2a4fe385b009a2h
                    DQ 03e180ceedb53cb4dh
                    DQ 03e2592262cf998a7h
                    DQ 03e1ae28a04f106b8h
                    DQ 03e2c8c66b55ce464h
                    DQ 03e2e690927d688b0h
                    DQ 03de5b5774c7658b4h
                    DQ 03e0adc16d26859c7h
                    DQ 03df7fa5b21cbdb5dh
                    DQ 03e2e160149209a68h
                    DQ 03e39b4f3c72c4f78h
                    DQ 03e222418b7fcd690h
                    DQ 03e2d54aded7a9150h
                    DQ 03e360f4c7f1aed15h
                    DQ 03e13c570d0fa8f96h
                    DQ 03e3b3514c7e0166eh
                    DQ 03e3307ee9a6271d2h
                    DQ 03dee9722922c0226h
                    DQ 03e33f7ad0f3f4016h
                    DQ 03e3592262cf998a7h
                    DQ 03e23bc09fca70073h
                    DQ 03e2f41777bc5f936h
                    DQ 03dd781d97ee91247h
                    DQ 03e306a56d76b9a84h
                    DQ 03e2df9c37c0beb3ah
                    DQ 03e1905c35651c429h
                    DQ 03e3b69d927dfc23dh
                    DQ 03e2d7e57a5afb633h
                    DQ 03e3bb29bdc81c4dbh
                    DQ 03e38ee1b912d8994h
                    DQ 03e3864b2df91e96ah
                    DQ 03e1d8a40770df213h
                    DQ 03e2d39a9331f27cfh
                    DQ 03e32411e4e8eea54h
                    DQ 03e3204d0144751b3h
                    DQ 03e2268331dd8bd0bh
                    DQ 03e47606012de0634h
                    DQ 03e3550aa3a93ec7eh
                    DQ 03e45a616eb9612e0h
                    DQ 03e3aec23fd65f8e1h
                    DQ 03e248f838294639ch
                    DQ 03e3b62384cafa1a3h
                    DQ 03e461c0e73048b72h
                    DQ 03e36cc9a0d8c0e85h
                    DQ 03e489b355ede26f4h
                    DQ 03e2b5941acd71f1eh
                    DQ 03e4d499bd9b32266h
                    DQ 03e043b9f52b061bah
                    DQ 03e46360892eb65e6h
                    DQ 03e4dba9f8729ab41h
                    DQ 03e479a3715fc9257h
                    DQ 03e0d1f6d3f77ae38h
                    DQ 03e48992d66fb9ec1h
                    DQ 03e4666f195620f03h
                    DQ 03e43f7ad0f3f4016h
                    DQ 03e30a522b65bc039h
                    DQ 03e319dee9b9489e3h
                    DQ 03e323352e1a31521h
                    DQ 03e4b3a19bcaf1aa4h
                    DQ 03e3f2f060a50d366h
                    DQ 03e44fdf677c8dfd9h
                    DQ 03e48a35588aec6dfh
                    DQ 03e28b0e2a19575b0h
                    DQ 03e2ec30c6e3e04a7h
                    DQ 03e2705912d25b325h
                    DQ 03e2dae1b8d59e849h
                    DQ 03e423e2e1169656ah
                    DQ 03e349d026e33d675h
                    DQ 03e423c465e6976dah
                    DQ 03e366c977e236c73h
                    DQ 03e44fec0a13af881h
                    DQ 03e3bdefbd14a0816h
                    DQ 03e42fe3e91c348e4h
                    DQ 03e4fc0c868ccc02dh
                    DQ 03e3ce20a829051bbh
                    DQ 03e47f10cf32e6bbah
                    DQ 03e43cf2061568859h
                    DQ 03e484995cb804b94h
                    DQ 03e4a52b6acfcfdcah
                    DQ 03e3b291ecf4dff1eh
                    DQ 03e21d2c3e64ae851h
                    DQ 03e4017e4faa42b7dh
                    DQ 03de975077f1f5f0ch
                    DQ 03e20327dc8093a52h
                    DQ 03e3108d9313aec65h
                    DQ 03e4a12e5301be44ah
                    DQ 03e1e754d20c519e1h
                    DQ 03e3f456f394f9727h
                    DQ 03e29471103e8f00dh
                    DQ 03e3ef3150343f8dfh
                    DQ 03e41960d9d9c3263h
                    DQ 03e4204d0144751b3h
                    DQ 03e4507ff357398feh
                    DQ 03e4dc9937fc8cafdh
                    DQ 03e572f32fe672868h
                    DQ 03e53e49d647d323eh
                    DQ 03e33fb81ea92d9e0h
                    DQ 03e43e387ef003635h
                    DQ 03e1ac754cb104aeah
                    DQ 03e4535f0444ebaafh
                    DQ 03e253c8ea7b1cddah
                    DQ 03e3cf0c0396a568bh
                    DQ 03e5543ca873c2b4ah
                    DQ 03e425780181e2b37h
                    DQ 03e5ee52ed49d71d2h
                    DQ 03e51e64842e2c386h
                    DQ 03e5d2ba01bc76a27h
                    DQ 03e5b39774c30f499h
                    DQ 03e38740932120aeah
                    DQ 03e576dab3462a1e8h
                    DQ 03e409c9f20203b31h
                    DQ 03e516e7a08ad0d1ah
                    DQ 03e46172fe015e13bh
                    DQ 03e49e4558147cf67h
                    DQ 03e4cfdeb43cfd005h
                    DQ 03e3a809c03254a71h
                    DQ 03e47acfc98509e33h
                    DQ 03e54366de473e474h
                    DQ 03e5569394d90d724h
                    DQ 03e32b83ec743664ch
                    DQ 03e56db22c4808ee5h
                    DQ 03df7ae84940df0e1h
                    DQ 03e554042cd999564h
                    DQ 03e4242b8488b3056h
                    DQ 03e4e7dc059ab8a9eh
                    DQ 03e5a71e977d7da5fh
                    DQ 03e5d30d552ce0ec3h
                    DQ 03e43208592b6c6b7h
                    DQ 03e51440e7149afffh
                    DQ 03e36812c371a1c87h
                    DQ 03e579a3715fc9257h
                    DQ 03e57c92f2af8b0cah
                    DQ 03e56679d8894dbdfh
                    DQ 03e2a9f33e77507f0h
                    DQ 03e4c22a3e377a524h
                    DQ 03e3723c84a77a4dch
                    DQ 03e594a871b636194h
                    DQ 03e570d6058f62f4dh
                    DQ 03e4a6274cf0e362fh
                    DQ 03e42fe3570af1a0bh
                    DQ 03e596a286955d67eh
                    DQ 03e442104f127091eh
                    DQ 03e407826bae32c6bh
                    DQ 03df8d8844ce77237h
                    DQ 03e5eaa609080d4b4h
                    DQ 03e4dc66fbe61efc4h
                    DQ 03e5c8f11979a5db6h
                    DQ 03e52dedf0e6f1770h
                    DQ 03e5cb41e1410132ah
                    DQ 03e32866d705c553dh
                    DQ 03e54ec3293b2fbe0h
                    DQ 03e578b8c2f4d0fe1h
                    DQ 03e562ad8f7ca2cffh
                    DQ 03e5a298b5f973a2ch
                    DQ 03e49381d4f1b95e0h
                    DQ 03e564c7bdb9bc56ch
                    DQ 03e5fbb4caef790fch
                    DQ 03e51200c3f899927h
                    DQ 03e526a05c813d56eh
                    DQ 03e4681e2910108eeh
                    DQ 03e282cf15d12ecd7h
                    DQ 03e0a537e32446892h
                    DQ 03e46f9c1cb6f7010h
                    DQ 03e4328ddcedf39d8h
                    DQ 03e164f64c210df9dh
                    DQ 03e58f676e17cc811h
                    DQ 03e560ddf1680dd45h
                    DQ 03e5e2da951c2d91bh
                    DQ 03e5696777b66d115h
                    DQ 03e311eb3043f5601h
                    DQ 03e48000b33f90fd4h
                    DQ 03e523e2e1169656ah
                    DQ 03e5b41565d3990cbh
                    DQ 03e46138b8d9d31e6h
                    DQ 03e3565afaf7f6248h
                    DQ 03e4b68e0ba153594h
                    DQ 03e3d87027ef4ab9ah
                    DQ 03e556b9c99085939h
                    DQ 03e5aa02166cccab2h
                    DQ 03e5991d2aca399a1h
                    DQ 03e54982259cc625dh
                    DQ 03e4b9feddaab9820h
                    DQ 03e3c70c0f683cc68h
                    DQ 03e213156425e67e5h
                    DQ 03df79063deab051fh
                    DQ 03e27e2744b2b8ca5h
                    DQ 03e4600534df378dfh
                    DQ 03e59322676507a79h
                    DQ 03e4c4720cb4558b5h
                    DQ 03e445e4b56add63ah
                    DQ 03e4af321af5e9bb5h
                    DQ 03e57f1e1148dad64h
                    DQ 03e42a4022f65e2e6h
                    DQ 03e11f2ccbcd0d3cch
                    DQ 03e5eaa65b49696e2h
                    DQ 03e110e6123a74764h
                    DQ 03e3cf24b2077c3f6h
                    DQ 03e4fc8d8164754dah
                    DQ 03e598cfcdb6a2dbch
                    DQ 03e24464a6bcdf47bh
                    DQ 03e41f1774d8b66a6h
                    DQ 03e459920a2adf6fah
                    DQ 03e370d02a99b4c5ah
                    DQ 03e576b6cafa2532dh
                    DQ 03e5d23c38ec17936h
                    DQ 03e541b6b1b0e66c4h
                    DQ 03e5952662c6bfdc7h
                    DQ 03e4333f3d6bb35ech
                    DQ 03e195120d8486e92h
                    DQ 03e5db8a405fac56eh
                    DQ 03e5a4c112ce6312eh
                    DQ 03e536987e1924e45h
                    DQ 03e33f98ea94bc1bdh
                    DQ 03e459718aacb6ec7h
                    DQ 03df975077f1f5f0ch
                    DQ 03e13654d88f20500h
                    DQ 03e40f598530f101bh
                    DQ 03e5145f6c94f7fd7h
                    DQ 03e567fead8bcce75h
                    DQ 03e52e67148cd0a7bh
                    DQ 03e10d5e5897de907h
                    DQ 03e5b5ee92c53d919h
                    DQ 03e5c1c02803f7554h
                    DQ 03e5d5caa7a35c9f7h
                    DQ 03e5910459cac3223h
                    DQ 03e41fbe1bb98afdfh
                    DQ 03e3b135395510d1eh
                    DQ 03e47b8f0e7b8e757h
                    DQ 03e519511f61a96b8h
                    DQ 03e5117d846ae1f8eh
                    DQ 03e2b3a9507d6dc1fh
                    DQ 03e15fa7c78c9e676h
                    DQ 03e2db76303b21928h
                    DQ 03e27eb8450ac22edh
                    DQ 03e579e0caa9c9ab7h
                    DQ 03e59de6d7cba1bbeh
                    DQ 03e1df5f5baf436cbh
                    DQ 03e3e746344728dbfh
                    DQ 03e277c23362928b9h
                    DQ 03e4715137cfeba9fh
                    DQ 03e58fe55f2856443h
                    DQ 03e25bd1a025d9e24h
                    DQ 00000000000000000h

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

