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
; vrsa_cbrtf.s
;
; A vector implementation of the cbrtf libm function.
;  This routine implemented in single precision.  It is slightly
;  less accurate than the double precision version, but it will
;  be better for vectorizing.
;
; Prototype:
;
;     void vrsa_cbrtf(int len, float *src,float *dst);
;
;   Computes the floating point single precision cube root of x.
;   Returns proper C99 values, but may not raise status flags properly.
;
;

include fm.inc
include log_tables.inc

; local variable storage offsets if any

save_xmm6       EQU     00h
save_xmm7       EQU     010h
save_xmm8       EQU     020h
save_xmm9       EQU     030h
save_xmm10      EQU     040h
save_xmm11      EQU     050h
save_xmm12      EQU     060h
save_xmm13      EQU     070h
save_xmm14      EQU     080h
save_xmm15      EQU     090h
save_rdi        EQU     0A0h
save_rsi        EQU     0B0h
save_rbx        EQU     0C0h
save_rbp        EQU     0D0h
indexes_0       EQU     0E0h    ; save the array indexs
indexes_1       EQU     0E4h    ; save the array indexs
indexes_2       EQU     0E8h    ; save the array indexs
indexes_3       EQU     0ECh    ; save the array indexs

stack_size      EQU     0F8h ; We take 8 as the last nibble to allow for 
                             ; alligned data movement.


FN_PROTOTYPE_BAS64 vrsa_cbrtf




StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

StackDeallocate   MACRO size
    add         rsp, size
ENDM

SaveReg         MACRO reg64, offset
	mov	QWORD PTR [rsp+offset], reg64	; save rsi
	.SAVEREG reg64, offset
ENDM

RestoreReg         MACRO reg64, offset
	mov	reg64, QWORD PTR [rsp+offset]; save rsi
ENDM

SaveXmm         MACRO xmmreg, offset
                    movdqa      XMMWORD PTR [offset+rsp], xmmreg
                    .SAVEXMM128 xmmreg, offset
ENDM

RestoreXmm      MACRO xmmreg, offset
                    movdqa      xmmreg, XMMWORD PTR [offset+rsp]
ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveXmm xmm6, save_xmm6
    SaveXmm xmm7, save_xmm7
    SaveXmm xmm8, save_xmm8
    SaveXmm xmm9, save_xmm9
    SaveXmm xmm10, save_xmm10
    SaveXmm xmm11, save_xmm11
    SaveXmm xmm12, save_xmm12
    SaveXmm xmm13, save_xmm13
    SaveXmm xmm14, save_xmm14
    SaveXmm xmm15, save_xmm15    
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi
    
    .ENDPROLOG  


    mov	        rdi,rcx
    mov	        rsi, rdx
    mov	        rdx, r8
    and rsi,QWORD PTR L__all_bits_set
    jz  L__exit
    and rdx,QWORD PTR L__all_bits_set
    jz  L__exit
    cmp edi, 0 
    jle L__exit
    
    mov         r10,rdx 
    xor         r9 ,r9
    xor         r8 ,r8
ALIGN 16        
L__vrsa_top:
    ;fill data in xmm0
    cmp   rdi, 0 
    jz    L__exit_vrsa_cbrtf
    pxor  xmm0,xmm0
    cmp rdi, 1
    jz  L__fill_single
    cmp rdi, 2
    jz  L__fill_two
    cmp rdi, 3
    jz  L__fill_three
    movups xmm0, XMMWORD PTR [rsi + r9 *4 ] ;is data pointer always 16 byte aligned?
    sub rdi,4 
    add r9, 4 
    jmp L__start_process_vrs4_cbrtf
L__fill_single:
    movss xmm0, DWORD PTR [rsi + r9 *4 ]
    dec rdi
    inc r9
    jmp L__start_process_vrs4_cbrtf
L__fill_two:
    movlps xmm0, QWORD PTR [rsi + r9 *4 ]
    sub rdi, 2
    add r9, 2
    jmp L__start_process_vrs4_cbrtf
L__fill_three:
    pxor xmm1,xmm1
    movss xmm0, DWORD PTR[rsi + r9 * 4] 
    inc r9
    movlps xmm1, QWORD PTR [rsi + r9 * 4]
    pslldq xmm1, 4
    por    xmm0,xmm1
    sub rdi, 3
    add r9, 2    
L__start_process_vrs4_cbrtf:
;set the signmask. xmm2 stores the sign mask
    movaps   xmm2,XMMWORD PTR L__real_sign
    andps    xmm2 ,xmm0;xmm2 stores the sign mask
    andps    xmm0 ,XMMWORD PTR L__sign_mask;xmm0 = all positive values
    movaps   xmm1,xmm0
    movaps   xmm8 ,xmm0;copy the positive inputs to xmm8
    movaps   xmm13,xmm8
    punpckhqdq xmm1 ,xmm1; unpack the inputs to convert to double
    cvtps2pd xmm0   ,xmm0
    cvtps2pd xmm1   ,xmm1
    movapd   xmm5 ,XMMWORD PTR L__double_exp_mask
    movapd   xmm6,xmm5
    andpd    xmm5   ,xmm0; get the exponent part
    andpd    xmm6,xmm1
    movapd   xmm3 ,XMMWORD PTR L__double_mant_mask
    movapd   xmm4,xmm3
    andpd    xmm3   ,xmm0; get the mantissa part
    andpd    xmm4   ,xmm1
    movapd   xmm0    ,xmm3
    movapd   xmm1   ,xmm4
    pshufd   xmm5, xmm5,00Dh ; 00 00 11 01 {0,0,1,3}  ; move the exponent components to lower 2 32 bits
    pshufd   xmm6, xmm6,00Dh ; 00 00 11 01 {0,0,1,3} 
    punpcklqdq xmm5     ,xmm6; get all the exponents into one register
    psrld    xmm5         ,20; get all the exact exponents
    movaps   xmm7     ,XMMWORD PTR L__double_exp_bias
    psubd    xmm5     ,xmm7; xmm5 contains only the exp values
    cvtdq2ps xmm6 ,xmm5
    pcmpgtd  xmm5     ,xmm7; xmm5 contains the mask for Nans
    pcmpeqd  xmm8 ,XMMWORD PTR L__real_zero; xmm8 contains the mask for zeros
    por      xmm5     ,xmm8; xmm5 contains the mask for zeros 
                             ; and Nans
    ;instaed of division operation we do mult operation
    ;divps    xmm6,XMMWORD PTR L__float_real_three
    mulps     xmm6,XMMWORD PTR L__float_oneby3
    cvttps2dq xmm8              ,xmm6;xmm8 is the scaling factor
    cvtdq2ps  xmm9,xmm8
    subps    xmm6,xmm9
    mulps    xmm6,XMMWORD PTR L__float_real_three
    paddd    xmm8,XMMWORD PTR L__float_exp_bias
    pslld    xmm8,23
    
    movapd   xmm7,XMMWORD PTR L__double_real_one
    por      xmm0  ,xmm7;x01 
    por      xmm1  ,xmm7;x02 
    psrlq    xmm3,44
    psrlq    xmm4,44
    pshufd   xmm3,xmm3,0F8h ; 11 11 10 00 
    pshufd   xmm4,xmm4,0F8h
    punpcklqdq xmm3,xmm4
    movdqu   XMMWORD PTR [indexes_0 + rsp] ,xmm3 
    lea rax,XMMWORD PTR L__DoubleReciprocalTable_
    lea rcx,XMMWORD PTR L__DoubleCubeRootTable_
    xor rdx,rdx
    mov edx, DWORD PTR [indexes_0 + rsp] 
    movlpd  xmm3, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    movlpd  xmm9, QWORD PTR [ rcx + rdx  * 8 ]  ; load from cuberoot table
    mov edx, DWORD PTR [indexes_1 + rsp] 
    movhpd  xmm3, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    movhpd  xmm9, QWORD PTR [ rcx + rdx  * 8 ]  ; load from cuberoot table
    mov edx, DWORD PTR [indexes_2 + rsp] 
    movlpd  xmm4, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    movlpd  xmm10, QWORD PTR [ rcx + rdx  * 8 ] ; load from cuberoot table
    mov edx, DWORD PTR [indexes_3 + rsp] 
    movhpd  xmm4, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    movhpd  xmm10, QWORD PTR [ rcx + rdx  * 8 ] ; load from cuberoot table
    movapd   xmm11,XMMWORD PTR L__coefficient_1
    movapd   xmm12,XMMWORD PTR L__coefficient_2
    mulpd   xmm3 ,xmm0; 
    mulpd   xmm4 ,xmm1;
    subpd   xmm3 ,xmm7; r 
    subpd   xmm4 ,xmm7; r 
    movapd  xmm0  ,xmm3
    movapd  xmm1  ,xmm4
    mulpd   xmm0 ,xmm0; r^2
    mulpd   xmm1 ,xmm1; r^2
    mulpd   xmm3 ,xmm11; (1/3)*r
    mulpd   xmm4 ,xmm11; (1/3)*r
    mulpd   xmm0 ,xmm12; -(1/9)* r^2
    mulpd   xmm1 ,xmm12; -(1/9)* r^2
    addpd   xmm0  ,xmm3; (1/3)*r -(1/9)* r^2
    addpd   xmm1  ,xmm4; (1/3)*r -(1/9)* r^2
    addpd   xmm0  ,xmm7; 1+ (1/3)*r -(1/9)* r^2
    addpd   xmm1  ,xmm7; 1+ (1/3)*r -(1/9)* r^2
    mulpd   xmm0  ,xmm9;
    mulpd   xmm1 ,xmm10;
    movapd  xmm3,xmm8
    punpckhqdq xmm8,xmm8
    cvtps2pd  xmm3,xmm3
    cvtps2pd  xmm8,xmm8
    mulpd     xmm0,xmm3
    mulpd     xmm1,xmm8
    cvtps2dq  xmm6,xmm6
    movdqu    XMMWORD PTR[indexes_0 + rsp] ,xmm6

    lea rax,XMMWORD PTR L__definedCubeRootTable_
    xor rdx,rdx
    mov edx, DWORD PTR [indexes_0 + rsp]
    add edx,2
    movlpd  xmm3, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    mov edx, DWORD PTR [indexes_1 + rsp]
    add edx,2
    movhpd  xmm3, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    mulpd xmm0            ,xmm3; 
    mov edx, DWORD PTR [indexes_2 + rsp]
    add edx,2
    movlpd  xmm4, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    mov edx, DWORD PTR [indexes_3 + rsp]
    add edx,2
    movhpd  xmm4, QWORD PTR [ rax + rdx  * 8 ]  ; load from reciprocal table
    mulpd xmm1,xmm4

    addps xmm13,xmm13
    andps xmm13,xmm5
    cvtpd2ps xmm0,xmm0
    cvtpd2ps xmm1,xmm1
    movlhps  xmm0,xmm1
    andnps   xmm5 ,xmm0; xmm5 is inverted and anded with xmm0 
    orps     xmm5,xmm13
    orps     xmm5,xmm2
    
L__end_process_vrs4_cbrt:
    ;fill data back into memory buffer from the result xmm5
    movss DWORD PTR [r10 + r8 * 4],  xmm5
    inc r8
    cmp r9,r8
    jz L__exit_vrsa_cbrtf
    psrldq xmm5,4
    movss DWORD PTR [r10 + r8 * 4],  xmm5
    inc r8
    cmp r8,r9
    jz L__exit_vrsa_cbrtf
    psrldq xmm5, 4
    movss DWORD PTR [r10 + r8 * 4],  xmm5
    inc r8
    cmp r9,r8
    jz L__exit_vrsa_cbrtf
    psrldq xmm5, 4
    movss DWORD PTR [r10 + r8 * 4],  xmm5
    inc r8
    cmp r9,r8
    jmp L__vrsa_top
L__exit:    
L__exit_vrsa_cbrtf:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    RestoreXmm xmm6, save_xmm6
    RestoreXmm xmm7, save_xmm7
    RestoreXmm xmm8, save_xmm8
    RestoreXmm xmm9, save_xmm9
    RestoreXmm xmm10, save_xmm10
    RestoreXmm xmm11, save_xmm11
    RestoreXmm xmm12, save_xmm12
    RestoreXmm xmm13, save_xmm13
    RestoreXmm xmm14, save_xmm14
    RestoreXmm xmm15, save_xmm15    
    RestoreReg rdi,save_rdi
    RestoreReg rsi,save_rsi
    StackDeallocate stack_size    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    ret
fname        endp
TEXT    ENDS    
    
    
data SEGMENT READ
CONST    SEGMENT

ALIGN 16
L__all_bits_set         DQ 0FFFFFFFFFFFFFFFFh
                        DQ 0FFFFFFFFFFFFFFFFh  
L__double_mant_mask     DQ 0000FFFFFFFFFFFFFh    ; mantisa bits double
                        DQ 0000FFFFFFFFFFFFFh
L__double_exp_mask      DQ 07FF0000000000000h                        
                        DQ 07FF0000000000000h
L__float_exp_bias       DQ 00000007F0000007Fh
                        DQ 00000007F0000007Fh
L__double_exp_bias      DQ 0000003FF000003FFh
                        DQ 0000003FF000003FFh
L__float_real_three     DQ 04040000040400000h ; 3.0
                        DQ 04040000040400000h ; 3.0
L__double_real_one      DQ 03ff0000000000000h ; 1.0
                        DQ 03ff0000000000000h
L__float_oneby3         DQ 03EAAAAAB3EAAAAABh; (1/3)
                        DQ 03EAAAAAB3EAAAAABh
L__coefficient_1        DQ 03fd5555555555555h ; (1/3)
                        DQ 03fd5555555555555h
L__coefficient_2        DQ 0bFBC71C71C71C71Ch ; - (1/9)
                        DQ 0bFBC71C71C71C71Ch
L__real_zero            DQ 00000000000000000h    ; 0.0
                        DQ 00000000000000000h
L__real_sign            DQ 08000000080000000h    ; sign bit
                        DQ 08000000080000000h
L__sign_mask            DQ 07FFFFFFF7FFFFFFFh
                        DQ 07FFFFFFF7FFFFFFFh

ALIGN 16                        
L__DoubleReciprocalTable_          DQ 03ff0000000000000h   ; 1
              DQ 03fefe00000000000h   ; 0.996094
              DQ 03fefc00000000000h   ; 0.992188
              DQ 03fefa00000000000h   ; 0.988281
              DQ 03fef800000000000h   ; 0.984375
              DQ 03fef600000000000h   ; 0.980469
              DQ 03fef400000000000h   ; 0.976562
              DQ 03fef200000000000h   ; 0.972656
              DQ 03fef000000000000h   ; 0.96875
              DQ 03feee00000000000h   ; 0.964844
              DQ 03feec00000000000h   ; 0.960938
              DQ 03feea00000000000h   ; 0.957031
              DQ 03fee900000000000h   ; 0.955078
              DQ 03fee700000000000h   ; 0.951172
              DQ 03fee500000000000h   ; 0.947266
              DQ 03fee300000000000h   ; 0.943359
              DQ 03fee100000000000h   ; 0.939453
              DQ 03fee000000000000h   ; 0.9375
              DQ 03fede00000000000h   ; 0.933594
              DQ 03fedc00000000000h   ; 0.929688
              DQ 03feda00000000000h   ; 0.925781
              DQ 03fed900000000000h   ; 0.923828
              DQ 03fed700000000000h   ; 0.919922
              DQ 03fed500000000000h   ; 0.916016
              DQ 03fed400000000000h   ; 0.914062
              DQ 03fed200000000000h   ; 0.910156
              DQ 03fed000000000000h   ; 0.90625
              DQ 03fecf00000000000h   ; 0.904297
              DQ 03fecd00000000000h   ; 0.900391
              DQ 03fecb00000000000h   ; 0.896484
              DQ 03feca00000000000h   ; 0.894531
              DQ 03fec800000000000h   ; 0.890625
              DQ 03fec700000000000h   ; 0.888672
              DQ 03fec500000000000h   ; 0.884766
              DQ 03fec300000000000h   ; 0.880859
              DQ 03fec200000000000h   ; 0.878906
              DQ 03fec000000000000h   ; 0.875
              DQ 03febf00000000000h   ; 0.873047
              DQ 03febd00000000000h   ; 0.869141
              DQ 03febc00000000000h   ; 0.867188
              DQ 03feba00000000000h   ; 0.863281
              DQ 03feb900000000000h   ; 0.861328
              DQ 03feb700000000000h   ; 0.857422
              DQ 03feb600000000000h   ; 0.855469
              DQ 03feb400000000000h   ; 0.851562
              DQ 03feb300000000000h   ; 0.849609
              DQ 03feb200000000000h   ; 0.847656
              DQ 03feb000000000000h   ; 0.84375
              DQ 03feaf00000000000h   ; 0.841797
              DQ 03fead00000000000h   ; 0.837891
              DQ 03feac00000000000h   ; 0.835938
              DQ 03feaa00000000000h   ; 0.832031
              DQ 03fea900000000000h   ; 0.830078
              DQ 03fea800000000000h   ; 0.828125
              DQ 03fea600000000000h   ; 0.824219
              DQ 03fea500000000000h   ; 0.822266
              DQ 03fea400000000000h   ; 0.820312
              DQ 03fea200000000000h   ; 0.816406
              DQ 03fea100000000000h   ; 0.814453
              DQ 03fea000000000000h   ; 0.8125
              DQ 03fe9e00000000000h   ; 0.808594
              DQ 03fe9d00000000000h   ; 0.806641
              DQ 03fe9c00000000000h   ; 0.804688
              DQ 03fe9a00000000000h   ; 0.800781
              DQ 03fe9900000000000h   ; 0.798828
              DQ 03fe9800000000000h   ; 0.796875
              DQ 03fe9700000000000h   ; 0.794922
              DQ 03fe9500000000000h   ; 0.791016
              DQ 03fe9400000000000h   ; 0.789062
              DQ 03fe9300000000000h   ; 0.787109
              DQ 03fe9200000000000h   ; 0.785156
              DQ 03fe9000000000000h   ; 0.78125
              DQ 03fe8f00000000000h   ; 0.779297
              DQ 03fe8e00000000000h   ; 0.777344
              DQ 03fe8d00000000000h   ; 0.775391
              DQ 03fe8b00000000000h   ; 0.771484
              DQ 03fe8a00000000000h   ; 0.769531
              DQ 03fe8900000000000h   ; 0.767578
              DQ 03fe8800000000000h   ; 0.765625
              DQ 03fe8700000000000h   ; 0.763672
              DQ 03fe8600000000000h   ; 0.761719
              DQ 03fe8400000000000h   ; 0.757812
              DQ 03fe8300000000000h   ; 0.755859
              DQ 03fe8200000000000h   ; 0.753906
              DQ 03fe8100000000000h   ; 0.751953
              DQ 03fe8000000000000h   ; 0.75
              DQ 03fe7f00000000000h   ; 0.748047
              DQ 03fe7e00000000000h   ; 0.746094
              DQ 03fe7d00000000000h   ; 0.744141
              DQ 03fe7b00000000000h   ; 0.740234
              DQ 03fe7a00000000000h   ; 0.738281
              DQ 03fe7900000000000h   ; 0.736328
              DQ 03fe7800000000000h   ; 0.734375
              DQ 03fe7700000000000h   ; 0.732422
              DQ 03fe7600000000000h   ; 0.730469
              DQ 03fe7500000000000h   ; 0.728516
              DQ 03fe7400000000000h   ; 0.726562
              DQ 03fe7300000000000h   ; 0.724609
              DQ 03fe7200000000000h   ; 0.722656
              DQ 03fe7100000000000h   ; 0.720703
              DQ 03fe7000000000000h   ; 0.71875
              DQ 03fe6f00000000000h   ; 0.716797
              DQ 03fe6e00000000000h   ; 0.714844
              DQ 03fe6d00000000000h   ; 0.712891
              DQ 03fe6c00000000000h   ; 0.710938
              DQ 03fe6b00000000000h   ; 0.708984
              DQ 03fe6a00000000000h   ; 0.707031
              DQ 03fe6900000000000h   ; 0.705078
              DQ 03fe6800000000000h   ; 0.703125
              DQ 03fe6700000000000h   ; 0.701172
              DQ 03fe6600000000000h   ; 0.699219
              DQ 03fe6500000000000h   ; 0.697266
              DQ 03fe6400000000000h   ; 0.695312
              DQ 03fe6300000000000h   ; 0.693359
              DQ 03fe6200000000000h   ; 0.691406
              DQ 03fe6100000000000h   ; 0.689453
              DQ 03fe6000000000000h   ; 0.6875
              DQ 03fe5f00000000000h   ; 0.685547
              DQ 03fe5e00000000000h   ; 0.683594
              DQ 03fe5d00000000000h   ; 0.681641
              DQ 03fe5c00000000000h   ; 0.679688
              DQ 03fe5b00000000000h   ; 0.677734
              DQ 03fe5a00000000000h   ; 0.675781
              DQ 03fe5900000000000h   ; 0.673828
              DQ 03fe5800000000000h   ; 0.671875
              DQ 03fe5800000000000h   ; 0.671875
              DQ 03fe5700000000000h   ; 0.669922
              DQ 03fe5600000000000h   ; 0.667969
              DQ 03fe5500000000000h   ; 0.666016
              DQ 03fe5400000000000h   ; 0.664062
              DQ 03fe5300000000000h   ; 0.662109
              DQ 03fe5200000000000h   ; 0.660156
              DQ 03fe5100000000000h   ; 0.658203
              DQ 03fe5000000000000h   ; 0.65625
              DQ 03fe5000000000000h   ; 0.65625
              DQ 03fe4f00000000000h   ; 0.654297
              DQ 03fe4e00000000000h   ; 0.652344
              DQ 03fe4d00000000000h   ; 0.650391
              DQ 03fe4c00000000000h   ; 0.648438
              DQ 03fe4b00000000000h   ; 0.646484
              DQ 03fe4a00000000000h   ; 0.644531
              DQ 03fe4a00000000000h   ; 0.644531
              DQ 03fe4900000000000h   ; 0.642578
              DQ 03fe4800000000000h   ; 0.640625
              DQ 03fe4700000000000h   ; 0.638672
              DQ 03fe4600000000000h   ; 0.636719
              DQ 03fe4600000000000h   ; 0.636719
              DQ 03fe4500000000000h   ; 0.634766
              DQ 03fe4400000000000h   ; 0.632812
              DQ 03fe4300000000000h   ; 0.630859
              DQ 03fe4200000000000h   ; 0.628906
              DQ 03fe4200000000000h   ; 0.628906
              DQ 03fe4100000000000h   ; 0.626953
              DQ 03fe4000000000000h   ; 0.625
              DQ 03fe3f00000000000h   ; 0.623047
              DQ 03fe3e00000000000h   ; 0.621094
              DQ 03fe3e00000000000h   ; 0.621094
              DQ 03fe3d00000000000h   ; 0.619141
              DQ 03fe3c00000000000h   ; 0.617188
              DQ 03fe3b00000000000h   ; 0.615234
              DQ 03fe3b00000000000h   ; 0.615234
              DQ 03fe3a00000000000h   ; 0.613281
              DQ 03fe3900000000000h   ; 0.611328
              DQ 03fe3800000000000h   ; 0.609375
              DQ 03fe3800000000000h   ; 0.609375
              DQ 03fe3700000000000h   ; 0.607422
              DQ 03fe3600000000000h   ; 0.605469
              DQ 03fe3500000000000h   ; 0.603516
              DQ 03fe3500000000000h   ; 0.603516
              DQ 03fe3400000000000h   ; 0.601562
              DQ 03fe3300000000000h   ; 0.599609
              DQ 03fe3200000000000h   ; 0.597656
              DQ 03fe3200000000000h   ; 0.597656
              DQ 03fe3100000000000h   ; 0.595703
              DQ 03fe3000000000000h   ; 0.59375
              DQ 03fe3000000000000h   ; 0.59375
              DQ 03fe2f00000000000h   ; 0.591797
              DQ 03fe2e00000000000h   ; 0.589844
              DQ 03fe2e00000000000h   ; 0.589844
              DQ 03fe2d00000000000h   ; 0.587891
              DQ 03fe2c00000000000h   ; 0.585938
              DQ 03fe2b00000000000h   ; 0.583984
              DQ 03fe2b00000000000h   ; 0.583984
              DQ 03fe2a00000000000h   ; 0.582031
              DQ 03fe2900000000000h   ; 0.580078
              DQ 03fe2900000000000h   ; 0.580078
              DQ 03fe2800000000000h   ; 0.578125
              DQ 03fe2700000000000h   ; 0.576172
              DQ 03fe2700000000000h   ; 0.576172
              DQ 03fe2600000000000h   ; 0.574219
              DQ 03fe2500000000000h   ; 0.572266
              DQ 03fe2500000000000h   ; 0.572266
              DQ 03fe2400000000000h   ; 0.570312
              DQ 03fe2300000000000h   ; 0.568359
              DQ 03fe2300000000000h   ; 0.568359
              DQ 03fe2200000000000h   ; 0.566406
              DQ 03fe2100000000000h   ; 0.564453
              DQ 03fe2100000000000h   ; 0.564453
              DQ 03fe2000000000000h   ; 0.5625
              DQ 03fe2000000000000h   ; 0.5625
              DQ 03fe1f00000000000h   ; 0.560547
              DQ 03fe1e00000000000h   ; 0.558594
              DQ 03fe1e00000000000h   ; 0.558594
              DQ 03fe1d00000000000h   ; 0.556641
              DQ 03fe1c00000000000h   ; 0.554688
              DQ 03fe1c00000000000h   ; 0.554688
              DQ 03fe1b00000000000h   ; 0.552734
              DQ 03fe1b00000000000h   ; 0.552734
              DQ 03fe1a00000000000h   ; 0.550781
              DQ 03fe1900000000000h   ; 0.548828
              DQ 03fe1900000000000h   ; 0.548828
              DQ 03fe1800000000000h   ; 0.546875
              DQ 03fe1800000000000h   ; 0.546875
              DQ 03fe1700000000000h   ; 0.544922
              DQ 03fe1600000000000h   ; 0.542969
              DQ 03fe1600000000000h   ; 0.542969
              DQ 03fe1500000000000h   ; 0.541016
              DQ 03fe1500000000000h   ; 0.541016
              DQ 03fe1400000000000h   ; 0.539062
              DQ 03fe1300000000000h   ; 0.537109
              DQ 03fe1300000000000h   ; 0.537109
              DQ 03fe1200000000000h   ; 0.535156
              DQ 03fe1200000000000h   ; 0.535156
              DQ 03fe1100000000000h   ; 0.533203
              DQ 03fe1100000000000h   ; 0.533203
              DQ 03fe1000000000000h   ; 0.53125
              DQ 03fe0f00000000000h   ; 0.529297
              DQ 03fe0f00000000000h   ; 0.529297
              DQ 03fe0e00000000000h   ; 0.527344
              DQ 03fe0e00000000000h   ; 0.527344
              DQ 03fe0d00000000000h   ; 0.525391
              DQ 03fe0d00000000000h   ; 0.525391
              DQ 03fe0c00000000000h   ; 0.523438
              DQ 03fe0c00000000000h   ; 0.523438
              DQ 03fe0b00000000000h   ; 0.521484
              DQ 03fe0a00000000000h   ; 0.519531
              DQ 03fe0a00000000000h   ; 0.519531
              DQ 03fe0900000000000h   ; 0.517578
              DQ 03fe0900000000000h   ; 0.517578
              DQ 03fe0800000000000h   ; 0.515625
              DQ 03fe0800000000000h   ; 0.515625
              DQ 03fe0700000000000h   ; 0.513672
              DQ 03fe0700000000000h   ; 0.513672
              DQ 03fe0600000000000h   ; 0.511719
              DQ 03fe0600000000000h   ; 0.511719
              DQ 03fe0500000000000h   ; 0.509766
              DQ 03fe0500000000000h   ; 0.509766
              DQ 03fe0400000000000h   ; 0.507812
              DQ 03fe0400000000000h   ; 0.507812
              DQ 03fe0300000000000h   ; 0.505859
              DQ 03fe0300000000000h   ; 0.505859
              DQ 03fe0200000000000h   ; 0.503906
              DQ 03fe0200000000000h   ; 0.503906
              DQ 03fe0100000000000h   ; 0.501953
              DQ 03fe0100000000000h   ; 0.501953
              DQ 03fe0000000000000h   ; 0.5


ALIGN 16
L__DoubleCubeRootTable_   DQ 03ff0000000000000h   ; 1
              DQ 03ff00558e6547c36h   ; 1.00131
              DQ 03ff00ab8f9d2f374h   ; 1.00262
              DQ 03ff010204b673fc7h   ; 1.00394
              DQ 03ff0158eec36749bh   ; 1.00526
              DQ 03ff01b04ed9fdb53h   ; 1.0066
              DQ 03ff02082613df53ch   ; 1.00794
              DQ 03ff0260758e78308h   ; 1.00928
              DQ 03ff02b93e6b091f0h   ; 1.01064
              DQ 03ff031281ceb8ea2h   ; 1.012
              DQ 03ff036c40e2a5e2ah   ; 1.01337
              DQ 03ff03c67cd3f7ceah   ; 1.01475
              DQ 03ff03f3c9fee224ch   ; 1.01544
              DQ 03ff044ec379f7f79h   ; 1.01683
              DQ 03ff04aa3cd578d67h   ; 1.01822
              DQ 03ff0506374d40a3dh   ; 1.01963
              DQ 03ff0562b4218a6e3h   ; 1.02104
              DQ 03ff059123d3a9848h   ; 1.02175
              DQ 03ff05ee6694e7166h   ; 1.02317
              DQ 03ff064c2ee6e07c6h   ; 1.0246
              DQ 03ff06aa7e19c01c5h   ; 1.02604
              DQ 03ff06d9d8b1deccah   ; 1.02676
              DQ 03ff0738f4b6cc8e2h   ; 1.02821
              DQ 03ff07989af9f9f59h   ; 1.02967
              DQ 03ff07c8a2611201ch   ; 1.03041
              DQ 03ff08291a9958f03h   ; 1.03188
              DQ 03ff088a208c3fe28h   ; 1.03336
              DQ 03ff08bad91dd7d8bh   ; 1.0341
              DQ 03ff091cb6588465eh   ; 1.03559
              DQ 03ff097f24eab04a1h   ; 1.0371
              DQ 03ff09b0932aee3f2h   ; 1.03785
              DQ 03ff0a13de8970de4h   ; 1.03937
              DQ 03ff0a45bc08a5ac7h   ; 1.04013
              DQ 03ff0aa9e79bfa986h   ; 1.04166
              DQ 03ff0b0eaa961ca5bh   ; 1.04319
              DQ 03ff0b4145573271ch   ; 1.04396
              DQ 03ff0ba6ee5f9aad4h   ; 1.04552
              DQ 03ff0bd9fd0dbe02dh   ; 1.04629
              DQ 03ff0c408fc1cfd4bh   ; 1.04786
              DQ 03ff0c741430e2059h   ; 1.04865
              DQ 03ff0cdb9442ea813h   ; 1.05023
              DQ 03ff0d0f905168e6ch   ; 1.05102
              DQ 03ff0d7801893d261h   ; 1.05261
              DQ 03ff0dac772091bdeh   ; 1.05341
              DQ 03ff0e15dd5c330abh   ; 1.05502
              DQ 03ff0e4ace71080a4h   ; 1.05583
              DQ 03ff0e7fe920f3037h   ; 1.05664
              DQ 03ff0eea9c37e497eh   ; 1.05827
              DQ 03ff0f203512f4314h   ; 1.05909
              DQ 03ff0f8be68db7f32h   ; 1.06073
              DQ 03ff0fc1ffa42d902h   ; 1.06155
              DQ 03ff102eb3af9ed89h   ; 1.06321
              DQ 03ff10654f1e29cfbh   ; 1.06405
              DQ 03ff109c1679c189fh   ; 1.06488
              DQ 03ff110a29f080b3dh   ; 1.06656
              DQ 03ff114176891738ah   ; 1.06741
              DQ 03ff1178f0099b429h   ; 1.06825
              DQ 03ff11e86ac2cd7abh   ; 1.06995
              DQ 03ff12206c7cf4046h   ; 1.07081
              DQ 03ff12589c21fb842h   ; 1.07166
              DQ 03ff12c986355d0d2h   ; 1.07339
              DQ 03ff13024129645cfh   ; 1.07425
              DQ 03ff133b2b13aa0ebh   ; 1.07512
              DQ 03ff13ad8cdc48ba3h   ; 1.07687
              DQ 03ff13e70544b1d4fh   ; 1.07774
              DQ 03ff1420adb77c99ah   ; 1.07862
              DQ 03ff145a867b1bfeah   ; 1.07951
              DQ 03ff14ceca1189d6dh   ; 1.08128
              DQ 03ff15093574284e9h   ; 1.08217
              DQ 03ff1543d2473ea9bh   ; 1.08307
              DQ 03ff157ea0d433a46h   ; 1.08396
              DQ 03ff15f4d44462724h   ; 1.08577
              DQ 03ff163039bd7cde6h   ; 1.08667
              DQ 03ff166bd21c3a8e2h   ; 1.08758
              DQ 03ff16a79dad1fb59h   ; 1.0885
              DQ 03ff171fcf9aaac3dh   ; 1.09033
              DQ 03ff175c3693980c3h   ; 1.09125
              DQ 03ff1798d1f73f3efh   ; 1.09218
              DQ 03ff17d5a2156e97fh   ; 1.0931
              DQ 03ff1812a73ea2593h   ; 1.09403
              DQ 03ff184fe1c406b8fh   ; 1.09497
              DQ 03ff18caf82b8dba4h   ; 1.09685
              DQ 03ff1908d4b38a510h   ; 1.09779
              DQ 03ff1946e7e36f7e5h   ; 1.09874
              DQ 03ff1985320ff72a2h   ; 1.09969
              DQ 03ff19c3b38e975a8h   ; 1.10064
              DQ 03ff1a026cb58453dh   ; 1.1016
              DQ 03ff1a415ddbb2c10h   ; 1.10256
              DQ 03ff1a808758d9e32h   ; 1.10352
              DQ 03ff1aff84bac98eah   ; 1.10546
              DQ 03ff1b3f5952e1a50h   ; 1.10644
              DQ 03ff1b7f67a896220h   ; 1.10741
              DQ 03ff1bbfb0178d186h   ; 1.10839
              DQ 03ff1c0032fc3cf91h   ; 1.10938
              DQ 03ff1c40f0b3eefc4h   ; 1.11037
              DQ 03ff1c81e99cc193fh   ; 1.11136
              DQ 03ff1cc31e15aae72h   ; 1.11235
              DQ 03ff1d048e7e7b565h   ; 1.11335
              DQ 03ff1d463b37e0090h   ; 1.11435
              DQ 03ff1d8824a365852h   ; 1.11536
              DQ 03ff1dca4b237a4f7h   ; 1.11637
              DQ 03ff1e0caf1b71965h   ; 1.11738
              DQ 03ff1e4f50ef85e61h   ; 1.1184
              DQ 03ff1e923104dbe76h   ; 1.11942
              DQ 03ff1ed54fc185286h   ; 1.12044
              DQ 03ff1f18ad8c82efch   ; 1.12147
              DQ 03ff1f5c4acdc91aah   ; 1.1225
              DQ 03ff1fa027ee4105bh   ; 1.12354
              DQ 03ff1fe44557cc808h   ; 1.12458
              DQ 03ff2028a37548ccfh   ; 1.12562
              DQ 03ff206d42b291a95h   ; 1.12667
              DQ 03ff20b2237c8466ah   ; 1.12772
              DQ 03ff20f74641030a6h   ; 1.12877
              DQ 03ff213cab6ef77c7h   ; 1.12983
              DQ 03ff2182537656c13h   ; 1.13089
              DQ 03ff21c83ec824406h   ; 1.13196
              DQ 03ff220e6dd675180h   ; 1.13303
              DQ 03ff2254e114737d2h   ; 1.13411
              DQ 03ff229b98f66228ch   ; 1.13519
              DQ 03ff22e295f19fd31h   ; 1.13627
              DQ 03ff2329d87caabb6h   ; 1.13736
              DQ 03ff2371610f243f2h   ; 1.13845
              DQ 03ff23b93021d47dah   ; 1.13954
              DQ 03ff2401462eae0b8h   ; 1.14064
              DQ 03ff2449a3b0d1b3fh   ; 1.14175
              DQ 03ff2449a3b0d1b3fh   ; 1.14175
              DQ 03ff2492492492492h   ; 1.14286
              DQ 03ff24db370778844h   ; 1.14397
              DQ 03ff25246dd846f45h   ; 1.14509
              DQ 03ff256dee16fdfd4h   ; 1.14621
              DQ 03ff25b7b844dfe71h   ; 1.14733
              DQ 03ff2601cce474fd2h   ; 1.14846
              DQ 03ff264c2c798fbe5h   ; 1.1496
              DQ 03ff2696d789511e2h   ; 1.15074
              DQ 03ff2696d789511e2h   ; 1.15074
              DQ 03ff26e1ce9a2cd73h   ; 1.15188
              DQ 03ff272d1233edcf3h   ; 1.15303
              DQ 03ff2778a2dfba8d0h   ; 1.15418
              DQ 03ff27c4812819c13h   ; 1.15534
              DQ 03ff2810ad98f6e10h   ; 1.1565
              DQ 03ff285d28bfa6d45h   ; 1.15767
              DQ 03ff285d28bfa6d45h   ; 1.15767
              DQ 03ff28a9f32aecb79h   ; 1.15884
              DQ 03ff28f70d6afeb08h   ; 1.16002
              DQ 03ff294478118ad83h   ; 1.1612
              DQ 03ff299233b1bc38ah   ; 1.16239
              DQ 03ff299233b1bc38ah   ; 1.16239
              DQ 03ff29e040e03fdfbh   ; 1.16358
              DQ 03ff2a2ea0334a07bh   ; 1.16477
              DQ 03ff2a7d52429b556h   ; 1.16597
              DQ 03ff2acc57a7862c2h   ; 1.16718
              DQ 03ff2acc57a7862c2h   ; 1.16718
              DQ 03ff2b1bb0fcf4190h   ; 1.16839
              DQ 03ff2b6b5edf6b54ah   ; 1.16961
              DQ 03ff2bbb61ed145cfh   ; 1.17083
              DQ 03ff2c0bbac5bfa6eh   ; 1.17205
              DQ 03ff2c0bbac5bfa6eh   ; 1.17205
              DQ 03ff2c5c6a0aeb681h   ; 1.17329
              DQ 03ff2cad705fc97a6h   ; 1.17452
              DQ 03ff2cfece6945583h   ; 1.17576
              DQ 03ff2cfece6945583h   ; 1.17576
              DQ 03ff2d5084ce0a331h   ; 1.17701
              DQ 03ff2da294368924fh   ; 1.17826
              DQ 03ff2df4fd4cff7c3h   ; 1.17952
              DQ 03ff2df4fd4cff7c3h   ; 1.17952
              DQ 03ff2e47c0bd7d237h   ; 1.18078
              DQ 03ff2e9adf35eb25ah   ; 1.18205
              DQ 03ff2eee5966124e8h   ; 1.18332
              DQ 03ff2eee5966124e8h   ; 1.18332
              DQ 03ff2f422fffa1e92h   ; 1.1846
              DQ 03ff2f9663b6369b6h   ; 1.18589
              DQ 03ff2feaf53f61612h   ; 1.18718
              DQ 03ff2feaf53f61612h   ; 1.18718
              DQ 03ff303fe552aea57h   ; 1.18847
              DQ 03ff309534a9ad7ceh   ; 1.18978
              DQ 03ff309534a9ad7ceh   ; 1.18978
              DQ 03ff30eae3fff6ff3h   ; 1.19108
              DQ 03ff3140f41335c2fh   ; 1.1924
              DQ 03ff3140f41335c2fh   ; 1.1924
              DQ 03ff319765a32d7aeh   ; 1.19372
              DQ 03ff31ee3971c2b5bh   ; 1.19504
              DQ 03ff3245704302c13h   ; 1.19637
              DQ 03ff3245704302c13h   ; 1.19637
              DQ 03ff329d0add2bb20h   ; 1.19771
              DQ 03ff32f50a08b48f9h   ; 1.19905
              DQ 03ff32f50a08b48f9h   ; 1.19905
              DQ 03ff334d6e9055a5fh   ; 1.2004
              DQ 03ff33a6394110fe6h   ; 1.20176
              DQ 03ff33a6394110fe6h   ; 1.20176
              DQ 03ff33ff6aea3afedh   ; 1.20312
              DQ 03ff3459045d8331bh   ; 1.20448
              DQ 03ff3459045d8331bh   ; 1.20448
              DQ 03ff34b3066efd36bh   ; 1.20586
              DQ 03ff350d71f529dd8h   ; 1.20724
              DQ 03ff350d71f529dd8h   ; 1.20724
              DQ 03ff356847c9006b4h   ; 1.20862
              DQ 03ff35c388c5f80bfh   ; 1.21001
              DQ 03ff35c388c5f80bfh   ; 1.21001
              DQ 03ff361f35ca116ffh   ; 1.21141
              DQ 03ff361f35ca116ffh   ; 1.21141
              DQ 03ff367b4fb5e0985h   ; 1.21282
              DQ 03ff36d7d76c96d0ah   ; 1.21423
              DQ 03ff36d7d76c96d0ah   ; 1.21423
              DQ 03ff3734cdd40cd95h   ; 1.21565
              DQ 03ff379233d4cd42ah   ; 1.21707
              DQ 03ff379233d4cd42ah   ; 1.21707
              DQ 03ff37f00a5a1ef96h   ; 1.21851
              DQ 03ff37f00a5a1ef96h   ; 1.21851
              DQ 03ff384e52521006ch   ; 1.21995
              DQ 03ff38ad0cad80848h   ; 1.22139
              DQ 03ff38ad0cad80848h   ; 1.22139
              DQ 03ff390c3a602dc60h   ; 1.22284
              DQ 03ff390c3a602dc60h   ; 1.22284
              DQ 03ff396bdc60bdb88h   ; 1.2243
              DQ 03ff39cbf3a8ca7a9h   ; 1.22577
              DQ 03ff39cbf3a8ca7a9h   ; 1.22577
              DQ 03ff3a2c8134ee2d1h   ; 1.22724
              DQ 03ff3a2c8134ee2d1h   ; 1.22724
              DQ 03ff3a8d8604cefe3h   ; 1.22872
              DQ 03ff3aef031b2b706h   ; 1.23021
              DQ 03ff3aef031b2b706h   ; 1.23021
              DQ 03ff3b50f97de6de5h   ; 1.2317
              DQ 03ff3b50f97de6de5h   ; 1.2317
              DQ 03ff3bb36a36163d8h   ; 1.23321
              DQ 03ff3bb36a36163d8h   ; 1.23321
              DQ 03ff3c1656500d20ah   ; 1.23472
              DQ 03ff3c79bedb6afb8h   ; 1.23623
              DQ 03ff3c79bedb6afb8h   ; 1.23623
              DQ 03ff3cdda4eb28aa2h   ; 1.23776
              DQ 03ff3cdda4eb28aa2h   ; 1.23776
              DQ 03ff3d420995a63c0h   ; 1.23929
              DQ 03ff3d420995a63c0h   ; 1.23929
              DQ 03ff3da6edf4b9061h   ; 1.24083
              DQ 03ff3da6edf4b9061h   ; 1.24083
              DQ 03ff3e0c5325b9fc2h   ; 1.24238
              DQ 03ff3e723a499453fh   ; 1.24393
              DQ 03ff3e723a499453fh   ; 1.24393
              DQ 03ff3ed8a484d473ah   ; 1.24549
              DQ 03ff3ed8a484d473ah   ; 1.24549
              DQ 03ff3f3f92ffb72d8h   ; 1.24706
              DQ 03ff3f3f92ffb72d8h   ; 1.24706
              DQ 03ff3fa706e6394a4h   ; 1.24864
              DQ 03ff3fa706e6394a4h   ; 1.24864
              DQ 03ff400f01682764ah   ; 1.25023
              DQ 03ff400f01682764ah   ; 1.25023
              DQ 03ff407783b92e17ah   ; 1.25182
              DQ 03ff407783b92e17ah   ; 1.25182
              DQ 03ff40e08f10ea81ah   ; 1.25343
              DQ 03ff40e08f10ea81ah   ; 1.25343
              DQ 03ff414a24aafb1e6h   ; 1.25504
              DQ 03ff414a24aafb1e6h   ; 1.25504
              DQ 03ff41b445c710fa7h   ; 1.25666
              DQ 03ff41b445c710fa7h   ; 1.25666
              DQ 03ff421ef3a901411h   ; 1.25828
              DQ 03ff421ef3a901411h   ; 1.25828
              DQ 03ff428a2f98d728bh   ; 1.25992

ALIGN 16
L__definedCubeRootTable_      DQ 03fe428a2f98d728bh   ; 0.629961
              DQ 03fe965fea53d6e3dh   ; 0.793701
              DQ 03ff0000000000000h   ; 1.0
              DQ 03ff428a2f98d728bh   ; 1.25992
              DQ 03ff965fea53d6e3dh   ; 1.5874

CONST    ENDS
data ENDS

END

