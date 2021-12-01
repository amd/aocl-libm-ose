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
; log_bdozr.S
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

ALM_PROTO_FMA3 log

; local variable storage offsets
save_xmm6       EQU     00h
save_xmm7       EQU     020h
save_xmm8       EQU     040h
save_xmm9       EQU     060h
save_xmm10      EQU     080h
save_xmm11      EQU     0A0h
save_xmm12      EQU     0C0h
save_xmm13      EQU     0E0h
save_xmm14      EQU     0100h
save_xmm15      EQU     0120h
save_rdi        EQU     0140h
stack_size      EQU     0168h ; We take 8 as the last nibble to allow for 
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
    vcvtdq2pd xmm6,xmm3 ; xexp 
   
    ;  NaN or inf
    vpand     xmm5,xmm0,QWORD PTR L__real_inf
    vcomisd   xmm5,QWORD PTR L__real_inf
    je        L__x_is_inf_or_nan
    
    ; check for negative numbers or zero
    vpxor     xmm5,xmm5,xmm5
    vcomisd   xmm0,xmm5
    jbe       L__x_is_zero_or_neg

    vpand     xmm2,xmm0,QWORD PTR L__real_mant
    vsubsd    xmm4,xmm0,QWORD PTR L__real_one

    vcomisd   xmm6,QWORD PTR L__mask_1023_f
    je        L__denormal_adjust

L__continue_common:    
    ; compute index into the log tables
    vpand     xmm1,xmm0,QWORD PTR L__mask_mant_all8
    vpand     xmm3,xmm0,QWORD PTR L__mask_mant9
    vpsllq    xmm3,xmm3,1 
    vpaddq    xmm1,xmm3,xmm1
    vmovq     rax,xmm1
	
    ; near one codepath
    vpand     xmm4,xmm4,QWORD PTR L__real_notsign
    vcomisd   xmm4,QWORD PTR L__real_threshold
    jb        L__near_one

    ; F,Y
    shr     rax,44
    vpor    xmm2,xmm2,QWORD PTR L__real_half
    vpor    xmm1,xmm1,QWORD PTR L__real_half
    lea     r9,QWORD PTR L__log_F_inv

    ; f = F - Y,r = f * inv
    vsubsd    xmm1,xmm1,xmm2
    vmulsd    xmm1,xmm1,QWORD PTR[r9 + rax * 8]

    lea    r9,QWORD PTR L__log_256_lead

    ; poly
    vmulsd    xmm0,xmm1,xmm1 ; r*r
    vmovsd    xmm3,QWORD PTR L__real_1_over_6
    vmovsd    xmm5,QWORD PTR L__real_1_over_3
    vfmadd213sd    xmm3,xmm1,QWORD PTR L__real_1_over_5 ; r*1/6 + 1/5
    vfmadd213sd    xmm5,xmm1,QWORD PTR L__real_1_over_2 ; 1/2+r*1/3
    vmovsd    xmm4,xmm0,xmm0 ; r*r
    vfmadd213sd    xmm3,xmm1,QWORD PTR L__real_1_over_4 ;1/4+(1/5*r+r*r*1/6)
    
    vmulsd    xmm4,xmm0,xmm0                   ; r*r*r*r
    vfmadd231sd    xmm1,xmm5,xmm0 ; r*r*(1/2+r*1/3) + r
    vfmadd231sd    xmm1,xmm3,xmm4

    ; m*log(2) + log(G) - poly
    vmovsd    xmm5,QWORD PTR L__real_log2_tail
    vfmsub213sd    xmm5,xmm6,xmm1

    vmovsd    xmm0,QWORD PTR[r9 + rax * 8]
    lea    rdx,QWORD PTR L__log_256_tail
    vmovsd    xmm1,QWORD PTR[rdx + rax * 8]
    vaddsd    xmm1,xmm1,xmm5

    vfmadd231sd    xmm0,xmm6,QWORD PTR L__real_log2_lead

    vaddsd    xmm0,xmm0,xmm1
	RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret


ALIGN  16
L__near_one:

    ; r = x - 1.0
    vmovsd    xmm3,QWORD PTR L__real_two
    vsubsd    xmm0,xmm0,QWORD PTR L__real_one ; r

    vaddsd    xmm3,xmm3,xmm0
    vdivsd    xmm1,xmm0,xmm3 ; r/(2+r) = u/2

    vmovsd    xmm4,QWORD PTR L__real_ca2
    vmovsd    xmm5,QWORD PTR L__real_ca4

    vmulsd    xmm3,xmm0,xmm1 ; correction
    vaddsd    xmm1,xmm1,xmm1 ; u

    vmulsd    xmm2,xmm1,xmm1 ; u^2
    vfmadd213sd xmm4,xmm2,QWORD PTR L__real_ca1
    vfmadd213sd xmm5,xmm2,QWORD PTR L__real_ca3

    vmulsd    xmm2,xmm2,xmm1 ; u^3
    vmulsd    xmm4,xmm4,xmm2

    vmulsd    xmm2,xmm2,xmm2
    vmulsd    xmm2,xmm2,xmm1 ; u^7

    vfmadd231sd  xmm4,xmm5,xmm2
    vsubsd    xmm4,xmm4,xmm3
    vaddsd    xmm0,xmm0,xmm4
 
	RestoreRegsAVX xmm6,save_xmm6
    StackDeallocate stack_size
    ret



L__denormal_adjust:
    vpor       xmm2,xmm2,QWORD PTR L__real_one
    vsubsd     xmm2,xmm2,QWORD PTR L__real_one
    vpsrlq     xmm5,xmm2,52
    vpand      xmm2,xmm2,QWORD PTR L__real_mant
    vmovapd    xmm0,xmm2
    vpsubd     xmm5,xmm5,XMMWORD PTR L__mask_2045
    vcvtdq2pd  xmm6,xmm5
    jmp        L__continue_common

ALIGN  16
L__x_is_zero_or_neg:
    jne        L__x_is_neg
    vmovsd     xmm1,QWORD PTR L__real_ninf
    mov        r8d,DWORD PTR L__flag_x_zero
    call       fname_special

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
L__mask_001         DQ 00000000000000001h
                    DQ 00000000000000000h

L__mask_mant_all8   DQ 0000ff00000000000h
                    DQ 00000000000000000h
L__mask_mant9       DQ 00000080000000000h
                    DQ 00000000000000000h

L__real_log2_lead   DQ 03fe62e42e0000000h ; log2_lead  6.93147122859954833984e-01
                    DQ 00000000000000000h
L__real_log2_tail   DQ 03e6efa39ef35793ch ; log2_tail  5.76999904754328540596e-08
                    DQ 00000000000000000h

L__real_two         DQ 04000000000000000h ; 2
                    DQ 00000000000000000h

L__real_one         DQ 03ff0000000000000h ; 1
                    DQ 00000000000000000h

L__real_half        DQ 03fe0000000000000h ; 1/2
                    DQ 00000000000000000h

L__mask_100         DQ 00000000000000100h
                    DQ 00000000000000000h

L__real_1_over_512      DQ 03f60000000000000h
                        DQ 00000000000000000h

L__real_1_over_2    DQ 03fe0000000000000h
                    DQ 00000000000000000h
L__real_1_over_3    DQ 03fd5555555555555h
                    DQ 00000000000000000h
L__real_1_over_4    DQ 03fd0000000000000h
                    DQ 00000000000000000h
L__real_1_over_5    DQ 03fc999999999999ah
                    DQ 00000000000000000h
L__real_1_over_6    DQ 03fc5555555555555h
                    DQ 00000000000000000h

L__mask_1023_f      DQ 00c08ff80000000000h
                    DQ 00000000000000000h

L__mask_2045        DQ 000000000000007fdh
                    DQ 00000000000000000h

L__real_threshold   DQ 03fb0000000000000h ; .0625
                    DQ 00000000000000000h

L__real_notsign     DQ 07ffFFFFFFFFFFFFFh ; ^sign bit
                    DQ 00000000000000000h

L__real_ca1         DQ 03fb55555555554e6h ; 8.33333333333317923934e-02
                    DQ 00000000000000000h
L__real_ca2         DQ 03f89999999bac6d4h ; 1.25000000037717509602e-02
                    DQ 00000000000000000h
L__real_ca3         DQ 03f62492307f1519fh ; 2.23213998791944806202e-03
                    DQ 00000000000000000h
L__real_ca4         DQ 03f3c8034c85dfff0h ; 4.34887777707614552256e-04
                    DQ 00000000000000000h

ALIGN 16
L__log_256_lead     DQ 00000000000000000h
                    DQ 03f6ff00aa0000000h
                    DQ 03f7fe02a60000000h
                    DQ 03f87dc4750000000h
                    DQ 03f8fc0a8b0000000h
                    DQ 03f93cea440000000h
                    DQ 03f97b91b00000000h
                    DQ 03f9b9fc020000000h
                    DQ 03f9f829b00000000h
                    DQ 03fa1b0d980000000h
                    DQ 03fa39e87b0000000h
                    DQ 03fa58a5ba0000000h
                    DQ 03fa77458f0000000h
                    DQ 03fa95c8300000000h
                    DQ 03fab42dd70000000h
                    DQ 03fad276b80000000h
                    DQ 03faf0a30c0000000h
                    DQ 03fb0759830000000h
                    DQ 03fb16536e0000000h
                    DQ 03fb253f620000000h
                    DQ 03fb341d790000000h
                    DQ 03fb42edcb0000000h
                    DQ 03fb51b0730000000h
                    DQ 03fb60658a0000000h
                    DQ 03fb6f0d280000000h
                    DQ 03fb7da7660000000h
                    DQ 03fb8c345d0000000h
                    DQ 03fb9ab4240000000h
                    DQ 03fba926d30000000h
                    DQ 03fbb78c820000000h
                    DQ 03fbc5e5480000000h
                    DQ 03fbd4313d0000000h
                    DQ 03fbe270760000000h
                    DQ 03fbf0a30c0000000h
                    DQ 03fbfec9130000000h
                    DQ 03fc0671510000000h
                    DQ 03fc0d77e70000000h
                    DQ 03fc1478580000000h
                    DQ 03fc1b72ad0000000h
                    DQ 03fc2266f10000000h
                    DQ 03fc29552f0000000h
                    DQ 03fc303d710000000h
                    DQ 03fc371fc20000000h
                    DQ 03fc3dfc2b0000000h
                    DQ 03fc44d2b60000000h
                    DQ 03fc4ba36f0000000h
                    DQ 03fc526e5e0000000h
                    DQ 03fc59338d0000000h
                    DQ 03fc5ff3070000000h
                    DQ 03fc66acd40000000h
                    DQ 03fc6d60fe0000000h
                    DQ 03fc740f8f0000000h
                    DQ 03fc7ab8900000000h
                    DQ 03fc815c0a0000000h
                    DQ 03fc87fa060000000h
                    DQ 03fc8e928d0000000h
                    DQ 03fc9525a90000000h
                    DQ 03fc9bb3620000000h
                    DQ 03fca23bc10000000h
                    DQ 03fca8becf0000000h
                    DQ 03fcaf3c940000000h
                    DQ 03fcb5b5190000000h
                    DQ 03fcbc28670000000h
                    DQ 03fcc296850000000h
                    DQ 03fcc8ff7c0000000h
                    DQ 03fccf63540000000h
                    DQ 03fcd5c2160000000h
                    DQ 03fcdc1bca0000000h
                    DQ 03fce270760000000h
                    DQ 03fce8c0250000000h
                    DQ 03fcef0adc0000000h
                    DQ 03fcf550a50000000h
                    DQ 03fcfb91860000000h
                    DQ 03fd00e6c40000000h
                    DQ 03fd0402590000000h
                    DQ 03fd071b850000000h
                    DQ 03fd0a324e0000000h
                    DQ 03fd0d46b50000000h
                    DQ 03fd1058bf0000000h
                    DQ 03fd1368700000000h
                    DQ 03fd1675ca0000000h
                    DQ 03fd1980d20000000h
                    DQ 03fd1c898c0000000h
                    DQ 03fd1f8ff90000000h
                    DQ 03fd22941f0000000h
                    DQ 03fd2596010000000h
                    DQ 03fd2895a10000000h
                    DQ 03fd2b93030000000h
                    DQ 03fd2e8e2b0000000h
                    DQ 03fd31871c0000000h
                    DQ 03fd347dd90000000h
                    DQ 03fd3772660000000h
                    DQ 03fd3a64c50000000h
                    DQ 03fd3d54fa0000000h
                    DQ 03fd4043080000000h
                    DQ 03fd432ef20000000h
                    DQ 03fd4618bc0000000h
                    DQ 03fd4900680000000h
                    DQ 03fd4be5f90000000h
                    DQ 03fd4ec9730000000h
                    DQ 03fd51aad80000000h
                    DQ 03fd548a2c0000000h
                    DQ 03fd5767710000000h
                    DQ 03fd5a42ab0000000h
                    DQ 03fd5d1bdb0000000h
                    DQ 03fd5ff3070000000h
                    DQ 03fd62c82f0000000h
                    DQ 03fd659b570000000h
                    DQ 03fd686c810000000h
                    DQ 03fd6b3bb20000000h
                    DQ 03fd6e08ea0000000h
                    DQ 03fd70d42e0000000h
                    DQ 03fd739d7f0000000h
                    DQ 03fd7664e10000000h
                    DQ 03fd792a550000000h
                    DQ 03fd7bede00000000h
                    DQ 03fd7eaf830000000h
                    DQ 03fd816f410000000h
                    DQ 03fd842d1d0000000h
                    DQ 03fd86e9190000000h
                    DQ 03fd89a3380000000h
                    DQ 03fd8c5b7c0000000h
                    DQ 03fd8f11e80000000h
                    DQ 03fd91c67e0000000h
                    DQ 03fd9479410000000h
                    DQ 03fd972a340000000h
                    DQ 03fd99d9580000000h
                    DQ 03fd9c86b00000000h
                    DQ 03fd9f323e0000000h
                    DQ 03fda1dc060000000h
                    DQ 03fda484090000000h
                    DQ 03fda72a490000000h
                    DQ 03fda9cec90000000h
                    DQ 03fdac718c0000000h
                    DQ 03fdaf12930000000h
                    DQ 03fdb1b1e00000000h
                    DQ 03fdb44f770000000h
                    DQ 03fdb6eb590000000h
                    DQ 03fdb985890000000h
                    DQ 03fdbc1e080000000h
                    DQ 03fdbeb4d90000000h
                    DQ 03fdc149ff0000000h
                    DQ 03fdc3dd7a0000000h
                    DQ 03fdc66f4e0000000h
                    DQ 03fdc8ff7c0000000h
                    DQ 03fdcb8e070000000h
                    DQ 03fdce1af00000000h
                    DQ 03fdd0a63a0000000h
                    DQ 03fdd32fe70000000h
                    DQ 03fdd5b7f90000000h
                    DQ 03fdd83e720000000h
                    DQ 03fddac3530000000h
                    DQ 03fddd46a00000000h
                    DQ 03fddfc8590000000h
                    DQ 03fde248810000000h
                    DQ 03fde4c71a0000000h
                    DQ 03fde744260000000h
                    DQ 03fde9bfa60000000h
                    DQ 03fdec399d0000000h
                    DQ 03fdeeb20c0000000h
                    DQ 03fdf128f50000000h
                    DQ 03fdf39e5b0000000h
                    DQ 03fdf6123f0000000h
                    DQ 03fdf884a30000000h
                    DQ 03fdfaf5880000000h
                    DQ 03fdfd64f20000000h
                    DQ 03fdffd2e00000000h
                    DQ 03fe011fab0000000h
                    DQ 03fe02552a0000000h
                    DQ 03fe0389ee0000000h
                    DQ 03fe04bdf90000000h
                    DQ 03fe05f14b0000000h
                    DQ 03fe0723e50000000h
                    DQ 03fe0855c80000000h
                    DQ 03fe0986f40000000h
                    DQ 03fe0ab76b0000000h
                    DQ 03fe0be72e0000000h
                    DQ 03fe0d163c0000000h
                    DQ 03fe0e44980000000h
                    DQ 03fe0f72410000000h
                    DQ 03fe109f390000000h
                    DQ 03fe11cb810000000h
                    DQ 03fe12f7190000000h
                    DQ 03fe1422020000000h
                    DQ 03fe154c3d0000000h
                    DQ 03fe1675ca0000000h
                    DQ 03fe179eab0000000h
                    DQ 03fe18c6e00000000h
                    DQ 03fe19ee6b0000000h
                    DQ 03fe1b154b0000000h
                    DQ 03fe1c3b810000000h
                    DQ 03fe1d610f0000000h
                    DQ 03fe1e85f50000000h
                    DQ 03fe1faa340000000h
                    DQ 03fe20cdcd0000000h
                    DQ 03fe21f0bf0000000h
                    DQ 03fe23130d0000000h
                    DQ 03fe2434b60000000h
                    DQ 03fe2555bc0000000h
                    DQ 03fe2676200000000h
                    DQ 03fe2795e10000000h
                    DQ 03fe28b5000000000h
                    DQ 03fe29d37f0000000h
                    DQ 03fe2af15f0000000h
                    DQ 03fe2c0e9e0000000h
                    DQ 03fe2d2b400000000h
                    DQ 03fe2e47430000000h
                    DQ 03fe2f62a90000000h
                    DQ 03fe307d730000000h
                    DQ 03fe3197a00000000h
                    DQ 03fe32b1330000000h
                    DQ 03fe33ca2b0000000h
                    DQ 03fe34e2890000000h
                    DQ 03fe35fa4e0000000h
                    DQ 03fe37117b0000000h
                    DQ 03fe38280f0000000h
                    DQ 03fe393e0d0000000h
                    DQ 03fe3a53730000000h
                    DQ 03fe3b68440000000h
                    DQ 03fe3c7c7f0000000h
                    DQ 03fe3d90260000000h
                    DQ 03fe3ea3390000000h
                    DQ 03fe3fb5b80000000h
                    DQ 03fe40c7a40000000h
                    DQ 03fe41d8fe0000000h
                    DQ 03fe42e9c60000000h
                    DQ 03fe43f9fe0000000h
                    DQ 03fe4509a50000000h
                    DQ 03fe4618bc0000000h
                    DQ 03fe4727430000000h
                    DQ 03fe48353d0000000h
                    DQ 03fe4942a80000000h
                    DQ 03fe4a4f850000000h
                    DQ 03fe4b5bd60000000h
                    DQ 03fe4c679a0000000h
                    DQ 03fe4d72d30000000h
                    DQ 03fe4e7d810000000h
                    DQ 03fe4f87a30000000h
                    DQ 03fe50913c0000000h
                    DQ 03fe519a4c0000000h
                    DQ 03fe52a2d20000000h
                    DQ 03fe53aad00000000h
                    DQ 03fe54b2460000000h
                    DQ 03fe55b9350000000h
                    DQ 03fe56bf9d0000000h
                    DQ 03fe57c57f0000000h
                    DQ 03fe58cadb0000000h
                    DQ 03fe59cfb20000000h
                    DQ 03fe5ad4040000000h
                    DQ 03fe5bd7d30000000h
                    DQ 03fe5cdb1d0000000h
                    DQ 03fe5ddde50000000h
                    DQ 03fe5ee02a0000000h
                    DQ 03fe5fe1ed0000000h
                    DQ 03fe60e32f0000000h
                    DQ 03fe61e3ef0000000h
                    DQ 03fe62e42e0000000h
                    DQ 00000000000000000h

ALIGN 16
L__log_256_tail     DQ 00000000000000000h
                    DQ 03db5885e0250435ah
                    DQ 03de620cf11f86ed2h
                    DQ 03dff0214edba4a25h
                    DQ 03dbf807c79f3db4eh
                    DQ 03dea352ba779a52bh
                    DQ 03dff56c46aa49fd5h
                    DQ 03dfebe465fef5196h
                    DQ 03e0cf0660099f1f8h
                    DQ 03e1247b2ff85945dh
                    DQ 03e13fd7abf5202b6h
                    DQ 03e1f91c9a918d51eh
                    DQ 03e08cb73f118d3cah
                    DQ 03e1d91c7d6fad074h
                    DQ 03de1971bec28d14ch
                    DQ 03e15b616a423c78ah
                    DQ 03da162a6617cc971h
                    DQ 03e166391c4c06d29h
                    DQ 03e2d46f5c1d0c4b8h
                    DQ 03e2e14282df1f6d3h
                    DQ 03e186f47424a660dh
                    DQ 03e2d4c8de077753eh
                    DQ 03e2e0c307ed24f1ch
                    DQ 03e226ea18763bdd3h
                    DQ 03e25cad69737c933h
                    DQ 03e2af62599088901h
                    DQ 03e18c66c83d6b2d0h
                    DQ 03e1880ceb36fb30fh
                    DQ 03e2495aac6ca17a4h
                    DQ 03e2761db4210878ch
                    DQ 03e2eb78e862bac2fh
                    DQ 03e19b2cd75790dd9h
                    DQ 03e2c55e5cbd3d50fh
                    DQ 03db162a6617cc971h
                    DQ 03dfdbeabaaa2e519h
                    DQ 03e1652cb7150c647h
                    DQ 03e39a11cb2cd2ee2h
                    DQ 03e219d0ab1a28813h
                    DQ 03e24bd9e80a41811h
                    DQ 03e3214b596faa3dfh
                    DQ 03e303fea46980bb8h
                    DQ 03e31c8ffa5fd28c7h
                    DQ 03dce8f743bcd96c5h
                    DQ 03dfd98c5395315c6h
                    DQ 03e3996fa3ccfa7b2h
                    DQ 03e1cd2af2ad13037h
                    DQ 03e1d0da1bd17200eh
                    DQ 03e3330410ba68b75h
                    DQ 03df4f27a790e7c41h
                    DQ 03e13956a86f6ff1bh
                    DQ 03e2c6748723551d9h
                    DQ 03e2500de9326cdfch
                    DQ 03e1086c848df1b59h
                    DQ 03e04357ead6836ffh
                    DQ 03e24832442408024h
                    DQ 03e3d10da8154b13dh
                    DQ 03e39e8ad68ec8260h
                    DQ 03e3cfbf706abaf18h
                    DQ 03e3fc56ac6326e23h
                    DQ 03e39105e3185cf21h
                    DQ 03e3d017fe5b19cc0h
                    DQ 03e3d1f6b48dd13feh
                    DQ 03e20b63358a7e73ah
                    DQ 03e263063028c211ch
                    DQ 03e2e6a6886b09760h
                    DQ 03e3c138bb891cd03h
                    DQ 03e369f7722b7221ah
                    DQ 03df57d8fac1a628ch
                    DQ 03e3c55e5cbd3d50fh
                    DQ 03e1552d2ff48fe2eh
                    DQ 03e37b8b26ca431bch
                    DQ 03e292decdc1c5f6dh
                    DQ 03e3abc7c551aaa8ch
                    DQ 03e36b540731a354bh
                    DQ 03e32d341036b89efh
                    DQ 03e4f9ab21a3a2e0fh
                    DQ 03e239c871afb9fbdh
                    DQ 03e3e6add2c81f640h
                    DQ 03e435c95aa313f41h
                    DQ 03e249d4582f6cc53h
                    DQ 03e47574c1c07398fh
                    DQ 03e4ba846dece9e8dh
                    DQ 03e16999fafbc68e7h
                    DQ 03e4c9145e51b0103h
                    DQ 03e479ef2cb44850ah
                    DQ 03e0beec73de11275h
                    DQ 03e2ef4351af5a498h
                    DQ 03e45713a493b4a50h
                    DQ 03e45c23a61385992h
                    DQ 03e42a88309f57299h
                    DQ 03e4530faa9ac8aceh
                    DQ 03e25fec2d792a758h
                    DQ 03e35a517a71cbcd7h
                    DQ 03e3707dc3e1cd9a3h
                    DQ 03e3a1a9f8ef43049h
                    DQ 03e4409d0276b3674h
                    DQ 03e20e2f613e85bd9h
                    DQ 03df0027433001e5fh
                    DQ 03e35dde2836d3265h
                    DQ 03e2300134d7aaf04h
                    DQ 03e3cb7e0b42724f5h
                    DQ 03e2d6e93167e6308h
                    DQ 03e3d1569b1526adbh
                    DQ 03e0e99fc338a1a41h
                    DQ 03e4eb01394a11b1ch
                    DQ 03e04f27a790e7c41h
                    DQ 03e25ce3ca97b7af9h
                    DQ 03e281f0f940ed857h
                    DQ 03e4d36295d88857ch
                    DQ 03e21aca1ec4af526h
                    DQ 03e445743c7182726h
                    DQ 03e23c491aead337eh
                    DQ 03e3aef401a738931h
                    DQ 03e21cede76092a29h
                    DQ 03e4fba8f44f82bb4h
                    DQ 03e446f5f7f3c3e1ah
                    DQ 03e47055f86c9674bh
                    DQ 03e4b41a92b6b6e1ah
                    DQ 03e443d162e927628h
                    DQ 03e4466174013f9b1h
                    DQ 03e3b05096ad69c62h
                    DQ 03e40b169150faa58h
                    DQ 03e3cd98b1df85da7h
                    DQ 03e468b507b0f8fa8h
                    DQ 03e48422df57499bah
                    DQ 03e11351586970274h
                    DQ 03e117e08acba92eeh
                    DQ 03e26e04314dd0229h
                    DQ 03e497f3097e56d1ah
                    DQ 03e3356e655901286h
                    DQ 03e0cb761457f94d6h
                    DQ 03e39af67a85a9dach
                    DQ 03e453410931a909fh
                    DQ 03e22c587206058f5h
                    DQ 03e223bc358899c22h
                    DQ 03e4d7bf8b6d223cbh
                    DQ 03e47991ec5197ddbh
                    DQ 03e4a79e6bb3a9219h
                    DQ 03e3a4c43ed663ec5h
                    DQ 03e461b5a1484f438h
                    DQ 03e4b4e36f7ef0c3ah
                    DQ 03e115f026acd0d1bh
                    DQ 03e3f36b535cecf05h
                    DQ 03e2ffb7fbf3eb5c6h
                    DQ 03e3e6a6886b09760h
                    DQ 03e3135eb27f5bbc3h
                    DQ 03e470be7d6f6fa57h
                    DQ 03e4ce43cc84ab338h
                    DQ 03e4c01d7aac3bd91h
                    DQ 03e45c58d07961060h
                    DQ 03e3628bcf941456eh
                    DQ 03e4c58b2a8461cd2h
                    DQ 03e33071282fb989ah
                    DQ 03e420dab6a80f09ch
                    DQ 03e44f8d84c397b1eh
                    DQ 03e40d0ee08599e48h
                    DQ 03e1d68787e37da36h
                    DQ 03e366187d591bafch
                    DQ 03e22346600bae772h
                    DQ 03e390377d0d61b8eh
                    DQ 03e4f5e0dd966b907h
                    DQ 03e49023cb79a00e2h
                    DQ 03e44e05158c28ad8h
                    DQ 03e3bfa7b08b18ae4h
                    DQ 03e4ef1e63db35f67h
                    DQ 03e0ec2ae39493d4fh
                    DQ 03e40afe930ab2fa0h
                    DQ 03e225ff8a1810dd4h
                    DQ 03e469743fb1a71a5h
                    DQ 03e5f9cc676785571h
                    DQ 03e5b524da4cbf982h
                    DQ 03e5a4c8b381535b8h
                    DQ 03e5839be809caf2ch
                    DQ 03e50968a1cb82c13h
                    DQ 03e5eae6a41723fb5h
                    DQ 03e5d9c29a380a4dbh
                    DQ 03e4094aa0ada625eh
                    DQ 03e5973ad6fc108cah
                    DQ 03e4747322fdbab97h
                    DQ 03e593692fa9d4221h
                    DQ 03e5c5a992dfbc7d9h
                    DQ 03e4e1f33e102387ah
                    DQ 03e464fbef14c048ch
                    DQ 03e4490f513ca5e3bh
                    DQ 03e37a6af4d4c799dh
                    DQ 03e57574c1c07398fh
                    DQ 03e57b133417f8c1ch
                    DQ 03e5feb9e0c176514h
                    DQ 03e419f25bb3172f7h
                    DQ 03e45f68a7bbfb852h
                    DQ 03e5ee278497929f1h
                    DQ 03e5ccee006109d58h
                    DQ 03e5ce081a07bd8b3h
                    DQ 03e570e12981817b8h
                    DQ 03e292ab6d93503d0h
                    DQ 03e58cb7dd7c3b61eh
                    DQ 03e4efafd0a0b78dah
                    DQ 03e5e907267c4288eh
                    DQ 03e5d31ef96780875h
                    DQ 03e23430dfcd2ad50h
                    DQ 03e344d88d75bc1f9h
                    DQ 03e5bec0f055e04fch
                    DQ 03e5d85611590b9adh
                    DQ 03df320568e583229h
                    DQ 03e5a891d1772f538h
                    DQ 03e22edc9dabba74dh
                    DQ 03e4b9009a1015086h
                    DQ 03e52a12a8c5b1a19h
                    DQ 03e3a7885f0fdac85h
                    DQ 03e5f4ffcd43ac691h
                    DQ 03e52243ae2640aadh
                    DQ 03e546513299035d3h
                    DQ 03e5b39c3a62dd725h
                    DQ 03e5ba6dd40049f51h
                    DQ 03e451d1ed7177409h
                    DQ 03e5cb0f2fd7f5216h
                    DQ 03e3ab150cd4e2213h
                    DQ 03e5cfd7bf3193844h
                    DQ 03e53fff8455f1dbdh
                    DQ 03e5fee640b905fc9h
                    DQ 03e54e2adf548084ch
                    DQ 03e3b597adc1ecdd2h
                    DQ 03e4345bd096d3a75h
                    DQ 03e5101b9d2453c8bh
                    DQ 03e508ce55cc8c979h
                    DQ 03e5bbf017e595f71h
                    DQ 03e37ce733bd393dch
                    DQ 03e233bb0a503f8a1h
                    DQ 03e30e2f613e85bd9h
                    DQ 03e5e67555a635b3ch
                    DQ 03e2ea88df73d5e8bh
                    DQ 03e3d17e03bda18a8h
                    DQ 03e5b607d76044f7eh
                    DQ 03e52adc4e71bc2fch
                    DQ 03e5f99dc7362d1d9h
                    DQ 03e5473fa008e6a6ah
                    DQ 03e2b75bb09cb0985h
                    DQ 03e5ea04dd10b9abah
                    DQ 03e5802d0d6979674h
                    DQ 03e174688ccd99094h
                    DQ 03e496f16abb9df22h
                    DQ 03e46e66df2aa374fh
                    DQ 03e4e66525ea4550ah
                    DQ 03e42d02f34f20cbdh
                    DQ 03e46cfce65047188h
                    DQ 03e39b78c842d58b8h
                    DQ 03e4735e624c24bc9h
                    DQ 03e47eba1f7dd1adfh
                    DQ 03e586b3e59f65355h
                    DQ 03e1ce38e637f1b4dh
                    DQ 03e58d82ec919edc7h
                    DQ 03e4c52648ddcfa37h
                    DQ 03e52482ceae1ac12h
                    DQ 03e55a312311aba4fh
                    DQ 03e411e236329f225h
                    DQ 03e5b48c8cd2f246ch
                    DQ 03e6efa39ef35793ch
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

