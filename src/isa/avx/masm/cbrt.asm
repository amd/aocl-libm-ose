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

; cbrt.S
;
; An implementation of the cbrt libm function.
;
; Prototype:
;
;     double cbrt(double x);
;

;
;   Algorithm:
;


; local variable storage offsets

store_input     EQU     20h
save_xmm6       EQU     30h
save_xmm7       EQU     40h
stack_size      EQU     98h 


include fm.inc

ALM_PROTO_BAS64 cbrt
fname_specail TEXTEQU <_cbrt_special>
EXTERN       fname_specail      : PROC


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
    SaveXmm xmm7, save_xmm7
    .ENDPROLOG   

    xor   rdx, rdx
    ;for the time being the stack pointer is not changed at all 
    ;Assuming that this is a leaf procedure we can avoid the decrementing and incrementing
    ;of the stack pointer. This will save some assembly operations and give us good performance
    ;results. If there is a procedure call then we need to look at the changes in the stack pointer. 

    movd  rax, xmm0
    movsd xmm6, xmm0
    mov   r10, QWORD PTR __exp_mask_64
    mov   r11, QWORD PTR __mantissa_mask_64
    mov   r9, rax
    and   rax, r10 ; rax = stores the exponent
    and   r9, r11 ; r9 = stores the mantissa
    shr   rax, 52
    cmp   rax, 7FFh
    jz    __cbrt_is_Nan_Infinite
    cmp   rax, 0h
    jz    __cbrt_is_denormal
    
ALIGN 16
__cbrt_is_normal:   
    mov   rcx, 3   ; cx is set to 3 to perform division and get the scale and remainder
    pand  xmm6, XMMWORD PTR __sign_bit_64  ; xmm6 contains the sign
    sub   ax, 3FFh
    ;we don't need the compare as sub instruction will raise the flags. But there was no performance improvement
    cmp   ax, 0 
    jge   __donot_change_dx
    not   dx
__donot_change_dx:
    idiv  cx ;Accumulator is divided by bl=3
              ;ax contains the quotient
              ;dx contains the remainder
    mov   cx, dx
    add   ax, 3FFh
    shl   rax, 52
    add   cx, 2
    shl   cx, 1
                 ;ax = Contains the quotient, Scale factor
    mov   QWORD PTR [store_input+rsp], rax
    movsd xmm7, QWORD PTR [store_input+rsp] ;xmm7 is the scaling factor = mf
    ;xmm0 is the modified input value from the denaormal cases 
    pand  xmm0, XMMWORD PTR __mantissa_mask_64
    por   xmm0, XMMWORD PTR __zero_point_five ;xmm0 = Y
    mov   r10, r9
    shr   r10, 43 
    shr   r9, 44 
    and   r10, 01h
    or    r9, 0100h
    add   r10, r9 ;r10 =  index_u64
    cvtsi2sd xmm4, r10 ;xmm4 = index_f64
    sub r10, 256
    lea rax, QWORD PTR __INV_TAB_256
    mulsd xmm4, QWORD PTR __one_by_512  ;xmm4 = F
    subsd xmm0, xmm4 ; xmm0 = f
    movsd xmm4, QWORD PTR [rax+r10*8]
    mulsd xmm0, xmm4  ; xmm0 = r 
   
    ;Now perform polynomial computation
    
    ; movddup xmm0,xmm0 ; xmm0 = r  ,r
    shufpd  xmm0, xmm0, 0 ; replacing movddup

    mulsd   xmm0, xmm0 ; xmm0 = r  ,r^2

    movapd   xmm4, xmm0 ; xmm4 = r  ,r^2
    movapd   xmm3, xmm0 ; xmm3 = r  ,r^2
    mulpd   xmm0, xmm0 ; xmm0 = r^2,r^4   ;;;;;;;;;
    mulpd   xmm3, xmm0 ; xmm3 = r^3,r^6   ;;;;;;;;;
    movapd  xmm2, xmm3
    mulpd   xmm2, XMMWORD PTR __coefficients_3_6 ; xmm2 = [coeff3 * r^3, coeff6 * r^6]
    mulpd   xmm3, xmm0 ; xmm3 = r^5,r^10 We don't need r^10
    unpckhpd xmm4, xmm3 ;xmm4 = r^5,r
    mulpd   xmm0, XMMWORD PTR __coefficients_2_4 ; xmm0 = [coeff2 * r^2, coeff4 * r^4]
    mulpd   xmm4, XMMWORD PTR __coefficients_5_1 ; xmm4 = [coeff5 * r^5, coeff1 * r  ]
    movapd xmm3, xmm4
    unpckhpd xmm3, xmm3          ;xmm3 = [~Don't Care ,coeff5 * r^5]
    addsd xmm2, xmm3 ; xmm2 = [coeff3 * r^3, coeff5 * r^5 + coeff6 * r^6]
    addpd xmm0, xmm2 ; xmm0 = [coeff2 * r^2 + coeff3 * r^3,coeff4 * r^4 + coeff5 * r^5 + coeff6 * r^6]  
    movapd xmm2, xmm0 
    unpckhpd xmm2, xmm2          ;xmm3 = [~Don't Care ,coeff2 * r^2 + coeff3 * r^3]
    addsd  xmm0, xmm2 ; xmm0 = [~Don't Care, coeff2 * r^2 + coeff3 * r^3 + coeff4 * r^4 + coeff5 * r^5 + coeff6 * r^6]
    addsd  xmm0, xmm4 ; xmm0 = [~Don't Care, coeff1 * r   + coeff2 * r^2 + coeff3 * r^3 + coeff4 * r^4 + coeff5 * r^5 + coeff6 * r^6]
    
    ; movddup xmm0,xmm0
    shufpd  xmm0, xmm0, 0 ; replacing movddup

    
    ;Polynomial computation completes here
    ;Now compute the following
    ;switch(rem)
    ;{
    ;    case -2:    cbrtRem_h.u64 = 3fe428a2f0000000h; cbrtRem_t.u64 = 3e531ae515c447bbh; break;
    ;    case -1:    cbrtRem_h.u64 = 3fe965fea0000000h; cbrtRem_t.u64 = 3e44f5b8f20ac166h; break;
    ;    case 0:     cbrtRem_h.u64 = 3ff0000000000000h; cbrtRem_t.u64 = 0000000000000000h; break;
    ;    case 1:     cbrtRem_h.u64 = 3ff428a2f0000000h; cbrtRem_t.u64 = 3e631ae515c447bbh; break;
    ;    case 2:     cbrtRem_h.u64 = 3ff965fea0000000h; cbrtRem_t.u64 = 3e54f5b8f20ac166h; break;
    ;    default:    break;
    ;}
    ;cbrtF_h.u64 = CBRT_F_H[index_u64-256];
    ;cbrtF_t.u64 = CBRT_F_T[index_u64-256];
    ;
    ;bH = (cbrtF_h.f64 * cbrtRem_h.f64);
    ;bT = ((((cbrtF_t.f64 * cbrtRem_t.f64)) + (cbrtF_t.f64 * cbrtRem_h.f64)) + (cbrtRem_t.f64 * cbrtF_h.f64));
    lea r8, QWORD PTR __cuberoot_remainder_h_l  ; load both head and tail of the remainders cuberoot at once
    movapd xmm1, [r8+rcx*8] ; xmm1 = [cbrtRem_h.f64,cbrtRem_t.f64]
    shl r10, 1
    lea rax, QWORD PTR __CBRT_F_H_L_256
    movapd xmm2, XMMWORD PTR [rax+r10*8] ; xmm2 = [cbrtF_h.f64,cbrtF_t.f64]
    movapd xmm3, xmm2     
    psrldq xmm3, 8           ; xmm3 = [~Dont Care,cbrtF_h.f64]
    unpcklpd xmm3, xmm2      ; xmm3 = [cbrtF_t.f64,cbrtF_h.f64]

    mulpd  xmm2, xmm1        ; xmm2 = [(cbrtF_h.f64*cbrtRem_h.f64),(cbrtRem_t.f64*cbrtF_t.f64)]
    mulpd  xmm3, xmm1        ; xmm3 = [(cbrtRem_h.f64*cbrtF_t.f64),(cbrtRem_t.f64*cbrtF_h.f64)]
    movapd xmm4, xmm3        
    unpckhpd xmm4, xmm4      ; xmm4 = [(cbrtRem_h.f64*cbrtF_t.f64),(cbrtRem_h.f64*cbrtF_t.f64)]
    addsd    xmm3, xmm4      ; xmm3 = [~Dont Care, ((cbrtRem_h.f64*cbrtF_t.f64) + (cbrtRem_t.f64*cbrtF_h.f64))]
    addsd    xmm2, xmm3      ; xmm2 = [(cbrtF_h.f64*cbrtRem_h.f64),(((cbrtRem_t.f64*cbrtF_t.f64)+(cbrtRem_h.f64*cbrtF_t.f64) + (cbrtRem_t.f64*cbrtF_h.f64))]
                              ; xmm2 = [bH,bT]
    ; Now calculate
    ;ans.f64 = (((((z * bT)) + (bT)) + (z * bH)) + (bH));
    ;ans.f64 = ans.f64 * mf;
    ;ans.u64 = ans.u64 | sign.u64;

    movapd   xmm3, xmm2
    unpckhpd xmm3, xmm3      ; xmm3 = [Dont Care,bH]
                              ; also xmm0 = [z,z] = the polynomial which was computed earlier
    mulpd    xmm0, xmm2      ; xmm0 = [(bH*z),(bT*z)]
    movapd   xmm4, xmm0      
    unpckhpd xmm4, xmm4      ; xmm4 = [(bH*z),(bH*z)]
    addsd    xmm0, xmm2      ; xmm0 = [~DontCare, ((bT*z) + bT)]
    unpckhpd xmm2, xmm2      ; xmm2 = [(bH),(bH)]
    addsd    xmm0, xmm4      ; xmm0 = [~DontCare, (((bT*z) + bT) + ( z*bH))]
    addsd    xmm0, xmm2      ; xmm0 = [~DontCare, ((((bT*z) + bT) + (z*bH)) + bH)] = [~Dont Care,ans.f64]
    mulsd    xmm0, xmm7      ; xmm0 = ans.f64 * mf; mf is the scaling factor
    por      xmm0, xmm6      ; restore the sign

    movdqa      xmm7, XMMWORD PTR [save_xmm7+rsp]
    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
    add   rsp, stack_size
    ret
    
         
ALIGN 16
__cbrt_is_denormal:
    movsd  xmm4, QWORD PTR __one_mask_64
    cmp    r9, 0
    jz     __cbrt_is_zero
    pand  xmm0, XMMWORD PTR __sign_mask_64   
    por    xmm0, xmm4
    subsd  xmm0, xmm4
    movd   rax, xmm0
    mov    r9, rax
    and    rax, r10 ; rax = stores the exponent
    and    r9, r11 ; r9 = stores the mantissa
    shr    rax, 52
    sub    rax, 1022
    jmp    __cbrt_is_normal

ALIGN 16
__cbrt_is_zero:
    movdqa      xmm7, XMMWORD PTR [save_xmm7+rsp]
    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
    add   rsp, stack_size
    ret

ALIGN 16
__cbrt_is_Nan_Infinite:
    cmp r9, 0
    jz __cbrt_is_Infinite
	call fname_specail
    ;mulsd xmm0, xmm0 ;this multiplication will raise an invalid exception
    ;por  xmm0, XMMWORD PTR __qnan_mask_64

__cbrt_is_Infinite: 
    movdqa      xmm7, XMMWORD PTR [save_xmm7+rsp]
    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]    
    add   rsp, stack_size
    ret

fname        endp
TEXT    ENDS   

data SEGMENT READ

CONST    SEGMENT         

ALIGN 16 

__mantissa_mask_64         DQ 000FFFFFFFFFFFFFh
                           DQ 0          ;this zero is necessary
__qnan_mask_64             DQ 0008000000000000h
__exp_mask_64              DQ 7FF0000000000000h
                           DQ 0
__zero                     DQ 0000000000000000h
                           DQ 0
ALIGN 16                           
__zero_point_five          DQ 3FE0000000000000h
                           DQ 0
ALIGN 16
__sign_mask_64             DQ 7FFFFFFFFFFFFFFFh 
                           DQ 0
__sign_bit_64              DQ 8000000000000000h 
                           DQ 0
__one_mask_64              DQ 3FF0000000000000h 
                           DQ 0
__one_by_512               DQ 3f60000000000000h
                           DQ 0


ALIGN 16
__denormal_factor          DQ 3F7428A2F98D728Bh 
                           DQ 0
; The coeeficients are arranged in a specific order to aid parrallel multiplication
; The numbers corresponding to each coeff corresponds to the rth order to which it is to 
; be multiplied 

ALIGN 16 
__coefficients:           

__coefficients_5_1          DQ 3fd5555555555555h ; 1
                            DQ 3f9ee7113506ac13h ; 5
__coefficients_2_4          DQ 0bfa511e8d2b3183bh ; 4
                            DQ 0bfbc71c71c71c71ch ; 2
__coefficients_3_6          DQ 0bf98090d6221a247h ; 6
                            DQ 3faf9add3c0ca458h ; 3
                            DQ 3f93750ad588f114h ; 7



ALIGN 16
__cuberoot_remainder_h_l: 
                            DQ 3e531ae515c447bbh  ; cbrt(2^-2) Low
                            DQ 3FE428A2F0000000h  ; cbrt(2^-2) High
                            DQ 3e44f5b8f20ac166h  ; cbrt(2^-1) Low
                            DQ 3FE965FEA0000000h  ; cbrt(2^-1) High
                            DQ 0000000000000000h  ; cbrt(2^0) Low
                            DQ 3FF0000000000000h  ; cbrt(2^0) High
                            DQ 3e631ae515c447bbh  ; cbrt(2^1) Low
                            DQ 3FF428A2F0000000h  ; cbrt(2^1) High
                            DQ 3e54f5b8f20ac166h  ; cbrt(2^2) Low
                            DQ 3FF965FEA0000000h  ; cbrt(2^2) High



;interleaved high and low values
ALIGN 16
__CBRT_F_H_L_256:
    DQ 0000000000000000h
    DQ 3ff0000000000000h
    DQ 3e6e6a24c81e4294h
    DQ 3ff0055380000000h
    DQ 3e58548511e3a785h
    DQ 3ff00aa390000000h
    DQ 3e64eb9336ec07f6h
    DQ 3ff00ff010000000h
    DQ 3e40ea64b8b750e1h
    DQ 3ff0153920000000h
    DQ 3e461637cff8a53ch
    DQ 3ff01a7eb0000000h
    DQ 3e40733bf7bd1943h
    DQ 3ff01fc0d0000000h
    DQ 3e5666911345ccedh
    DQ 3ff024ff80000000h
    DQ 3e477b7a3f592f14h
    DQ 3ff02a3ad0000000h
    DQ 3e6f18d3dd1a5402h
    DQ 3ff02f72b0000000h
    DQ 3e2be2f5a58ee9a4h
    DQ 3ff034a750000000h
    DQ 3e68901f8f085fa7h
    DQ 3ff039d880000000h
    DQ 3e5c68b8cd5b5d69h
    DQ 3ff03f0670000000h
    DQ 3e5a6b0e8624be42h
    DQ 3ff0443110000000h
    DQ 3dbc4b22b06f68e7h
    DQ 3ff0495870000000h
    DQ 3e60f3f0afcabe9bh
    DQ 3ff04e7c80000000h
    DQ 3e548495bca4e1b7h
    DQ 3ff0539d60000000h
    DQ 3e66107f1abdfdc3h
    DQ 3ff058bb00000000h
    DQ 3e6e67261878288ah
    DQ 3ff05dd570000000h
    DQ 3e5a6bc155286f1eh
    DQ 3ff062ecc0000000h
    DQ 3e58a759c64a85f2h
    DQ 3ff06800e0000000h
    DQ 3e45fce70a4a8d09h
    DQ 3ff06d11e0000000h
    DQ 3e32f9cbf373fe1dh
    DQ 3ff0721fc0000000h
    DQ 3e590564ce4ac359h
    DQ 3ff0772a80000000h
    DQ 3e5ac29ce761b02fh
    DQ 3ff07c3230000000h
    DQ 3e5cb752f497381ch
    DQ 3ff08136d0000000h
    DQ 3e68bb9e1cfb35e0h
    DQ 3ff0863860000000h
    DQ 3e65b4917099de90h
    DQ 3ff08b36f0000000h
    DQ 3e5cc77ac9c65ef2h
    DQ 3ff0903280000000h
    DQ 3e57a0f3e7be3dbah
    DQ 3ff0952b10000000h
    DQ 3e66ec851ee0c16fh
    DQ 3ff09a20a0000000h
    DQ 3e689449bf2946dah
    DQ 3ff09f1340000000h
    DQ 3e698f25301ba223h
    DQ 3ff0a402f0000000h
    DQ 3e347d5ec651f549h
    DQ 3ff0a8efc0000000h
    DQ 3e6c33ec9a86007ah
    DQ 3ff0add990000000h
    DQ 3e5e0b6653e92649h
    DQ 3ff0b2c090000000h
    DQ 3e3bd64ac09d755fh
    DQ 3ff0b7a4b0000000h
    DQ 3e2f537506f78167h
    DQ 3ff0bc85f0000000h
    DQ 3e62c382d1b3735eh
    DQ 3ff0c16450000000h
    DQ 3e6e20ed659f99e1h
    DQ 3ff0c63fe0000000h
    DQ 3e586b633a9c182ah
    DQ 3ff0cb18b0000000h
    DQ 3e445cfd5a65e777h
    DQ 3ff0cfeeb0000000h
    DQ 3e60c8770f58bca4h
    DQ 3ff0d4c1e0000000h
    DQ 3e6739e44b0933c5h
    DQ 3ff0d99250000000h
    DQ 3e027dc3d9ce7bd8h
    DQ 3ff0de6010000000h
    DQ 3e63c53c7c5a7b64h
    DQ 3ff0e32b00000000h
    DQ 3e69669683830cech
    DQ 3ff0e7f340000000h
    DQ 3e68d772c39bdcc4h
    DQ 3ff0ecb8d0000000h
    DQ 3e69b0008bcf6d7bh
    DQ 3ff0f17bb0000000h
    DQ 3e3bbb305825ce4fh
    DQ 3ff0f63bf0000000h
    DQ 3e6da3f4af13a406h
    DQ 3ff0faf970000000h
    DQ 3e5f36b96f74ce86h
    DQ 3ff0ffb460000000h
    DQ 3e165c002303f790h
    DQ 3ff1046cb0000000h
    DQ 3e682f84095ba7d5h
    DQ 3ff1092250000000h
    DQ 3e6d46433541b2c6h
    DQ 3ff10dd560000000h
    DQ 3e671c3d56e93a89h
    DQ 3ff11285e0000000h
    DQ 3e598dcef4e40012h
    DQ 3ff11733d0000000h
    DQ 3e4530ebef17fe03h
    DQ 3ff11bdf30000000h
    DQ 3e4e8b8fa3715066h
    DQ 3ff1208800000000h
    DQ 3e6ab26eb3b211dch
    DQ 3ff1252e40000000h
    DQ 3e454dd4dc906307h
    DQ 3ff129d210000000h
    DQ 3e5c9f962387984eh
    DQ 3ff12e7350000000h
    DQ 3e6c62a959afec09h
    DQ 3ff1331210000000h
    DQ 3e6638d9ac6a866ah
    DQ 3ff137ae60000000h
    DQ 3e338704eca8a22dh
    DQ 3ff13c4840000000h
    DQ 3e4e6c9e1db14f8fh
    DQ 3ff140dfa0000000h
    DQ 3e58744b7f9c9eaah
    DQ 3ff1457490000000h
    DQ 3e66c2893486373bh
    DQ 3ff14a0710000000h
    DQ 3e5b36bce31699b7h
    DQ 3ff14e9730000000h
    DQ 3e671e3813d200c7h
    DQ 3ff15324e0000000h
    DQ 3e699755ab40aa88h
    DQ 3ff157b030000000h
    DQ 3e6b45ca0e4bcfc0h
    DQ 3ff15c3920000000h
    DQ 3e32dd090d869c5dh
    DQ 3ff160bfc0000000h
    DQ 3e64fe0516b917dah
    DQ 3ff16543f0000000h
    DQ 3e694563226317a2h
    DQ 3ff169c5d0000000h
    DQ 3e653d8fafc2c851h
    DQ 3ff16e4560000000h
    DQ 3e5dcbd41fbd41a3h
    DQ 3ff172c2a0000000h
    DQ 3e5862ff5285f59ch
    DQ 3ff1773d90000000h
    DQ 3e63072ea97a1e1ch
    DQ 3ff17bb630000000h
    DQ 3e52839075184805h
    DQ 3ff1802c90000000h
    DQ 3e64b0323e9eff42h
    DQ 3ff184a0a0000000h
    DQ 3e6b158893c45484h
    DQ 3ff1891270000000h
    DQ 3e3149ef0fc35826h
    DQ 3ff18d8210000000h
    DQ 3e5f2e77ea96acaah
    DQ 3ff191ef60000000h
    DQ 3e5200074c471a95h
    DQ 3ff1965a80000000h
    DQ 3e63f8cc517f6f04h
    DQ 3ff19ac360000000h
    DQ 3e660ba2e311bb55h
    DQ 3ff19f2a10000000h
    DQ 3e64b788730bbec3h
    DQ 3ff1a38e90000000h
    DQ 3e657090795ee20ch
    DQ 3ff1a7f0e0000000h
    DQ 3e6d9ffe983670b1h
    DQ 3ff1ac5100000000h
    DQ 3e62a463ff61bfdah
    DQ 3ff1b0af00000000h
    DQ 3e69d1bc6a5e65cfh
    DQ 3ff1b50ad0000000h
    DQ 3e68718abaa9e922h
    DQ 3ff1b96480000000h
    DQ 3e63c2f52ffa342eh
    DQ 3ff1bdbc10000000h
    DQ 3e60fae13ff42c80h
    DQ 3ff1c21180000000h
    DQ 3e65440f0ef00d57h
    DQ 3ff1c664d0000000h
    DQ 3e46fcd22d4e3c1eh
    DQ 3ff1cab610000000h
    DQ 3e4e0c60b409e863h
    DQ 3ff1cf0530000000h
    DQ 3e6f9cab5a5f0333h
    DQ 3ff1d35230000000h
    DQ 3e630f24744c333dh
    DQ 3ff1d79d30000000h
    DQ 3e4b50622a76b2feh
    DQ 3ff1dbe620000000h
    DQ 3e6fdb94ba595375h
    DQ 3ff1e02cf0000000h
    DQ 3e3861b9b945a171h
    DQ 3ff1e471d0000000h
    DQ 3e654348015188c4h
    DQ 3ff1e8b490000000h
    DQ 3e6b54d149865523h
    DQ 3ff1ecf550000000h
    DQ 3e6a0bb783d9de33h
    DQ 3ff1f13410000000h
    DQ 3e6629d12b1a2157h
    DQ 3ff1f570d0000000h
    DQ 3e6467fe35d179dfh
    DQ 3ff1f9ab90000000h
    DQ 3e69763f3e26c8f7h
    DQ 3ff1fde450000000h
    DQ 3e53f798bb9f7679h
    DQ 3ff2021b20000000h
    DQ 3e552e577e855898h
    DQ 3ff2064ff0000000h
    DQ 3e6fde47e5502c3ah
    DQ 3ff20a82c0000000h
    DQ 3e5cbd0b548d96a0h
    DQ 3ff20eb3b0000000h
    DQ 3e6a9cd9f7be8de8h
    DQ 3ff212e2a0000000h
    DQ 3e522bbe704886deh
    DQ 3ff2170fb0000000h
    DQ 3e6e3dea8317f020h
    DQ 3ff21b3ac0000000h
    DQ 3e6e812085ac8855h
    DQ 3ff21f63f0000000h
    DQ 3e5c87144f24cb07h
    DQ 3ff2238b40000000h
    DQ 3e61e128ee311fa2h
    DQ 3ff227b0a0000000h
    DQ 3e5b5c163d61a2d3h
    DQ 3ff22bd420000000h
    DQ 3e47d97e7fb90633h
    DQ 3ff22ff5c0000000h
    DQ 3e6efe899d50f6a7h
    DQ 3ff2341570000000h
    DQ 3e6d0333eb75de5ah
    DQ 3ff2383350000000h
    DQ 3e40e590be73a573h
    DQ 3ff23c4f60000000h
    DQ 3e68ce8dcac3cdd2h
    DQ 3ff2406980000000h
    DQ 3e6ee8a48954064bh
    DQ 3ff24481d0000000h
    DQ 3e6aa62f18461e09h
    DQ 3ff2489850000000h
    DQ 3e601e5940986a15h
    DQ 3ff24cad00000000h
    DQ 3e3b082f4f9b8d4ch
    DQ 3ff250bfe0000000h
    DQ 3e6876e0e5527f5ah
    DQ 3ff254d0e0000000h
    DQ 3e63617080831e6bh
    DQ 3ff258e020000000h
    DQ 3e681b26e34aa4a2h
    DQ 3ff25ced90000000h
    DQ 3e552ee66dfab0c1h
    DQ 3ff260f940000000h
    DQ 3e5d85a5329e8819h
    DQ 3ff2650320000000h
    DQ 3e5105c1b646b5d1h
    DQ 3ff2690b40000000h
    DQ 3e6bb6690c1a379ch
    DQ 3ff26d1190000000h
    DQ 3e586aeba73ce3a9h
    DQ 3ff2711630000000h
    DQ 3e6dd16198294dd4h
    DQ 3ff2751900000000h
    DQ 3e6454e675775e83h
    DQ 3ff2791a20000000h
    DQ 3e63842e026197eah
    DQ 3ff27d1980000000h
    DQ 3e6f1ce0e70c44d2h
    DQ 3ff2811720000000h
    DQ 3e6ad636441a5627h
    DQ 3ff2851310000000h
    DQ 3e54c205d7212abbh
    DQ 3ff2890d50000000h
    DQ 3e6167c86c116419h
    DQ 3ff28d05d0000000h
    DQ 3e638ec3ef16e294h
    DQ 3ff290fca0000000h
    DQ 3e6473fceace9321h
    DQ 3ff294f1c0000000h
    DQ 3e67af53a836dba7h
    DQ 3ff298e530000000h
    DQ 3e1a51f3c383b652h
    DQ 3ff29cd700000000h
    DQ 3e63696da190822dh
    DQ 3ff2a0c710000000h
    DQ 3e62f9adec77074bh
    DQ 3ff2a4b580000000h
    DQ 3e38190fd5bee55fh
    DQ 3ff2a8a250000000h
    DQ 3e4bfee8fac68e55h
    DQ 3ff2ac8d70000000h
    DQ 3e331c9d6bc5f68ah
    DQ 3ff2b076f0000000h
    DQ 3e689d0523737edfh
    DQ 3ff2b45ec0000000h
    DQ 3e5a295943bf47bbh
    DQ 3ff2b84500000000h
    DQ 3e396be32e5b3207h
    DQ 3ff2bc29a0000000h
    DQ 3e6e44c7d909fa0eh
    DQ 3ff2c00c90000000h
    DQ 3e2b2505da94d9eah
    DQ 3ff2c3ee00000000h
    DQ 3e60c851f46c9c98h
    DQ 3ff2c7cdc0000000h
    DQ 3e5da71f7d9aa3b7h
    DQ 3ff2cbabf0000000h
    DQ 3e6f1b605d019ef1h
    DQ 3ff2cf8880000000h
    DQ 3e4386e8a2189563h
    DQ 3ff2d36390000000h
    DQ 3e3b19fa5d306ba7h
    DQ 3ff2d73d00000000h
    DQ 3e6dd749b67aef76h
    DQ 3ff2db14d0000000h
    DQ 3e676ff6f1dc04b0h
    DQ 3ff2deeb20000000h
    DQ 3e635a33d0b232a6h
    DQ 3ff2e2bfe0000000h
    DQ 3e64bdc80024a4e1h
    DQ 3ff2e69310000000h
    DQ 3e6ebd61770fd723h
    DQ 3ff2ea64b0000000h
    DQ 3e64769fc537264dh
    DQ 3ff2ee34d0000000h
    DQ 3e69021f429f3b98h
    DQ 3ff2f20360000000h
    DQ 3e5ee7083efbd606h
    DQ 3ff2f5d070000000h
    DQ 3e6ad985552a6b1ah
    DQ 3ff2f99bf0000000h
    DQ 3e6e3df778772160h
    DQ 3ff2fd65f0000000h
    DQ 3e6ca5d76ddc9b34h
    DQ 3ff3012e70000000h
    DQ 3e691154ffdbaf74h
    DQ 3ff304f570000000h
    DQ 3e667bdd57fb306ah
    DQ 3ff308baf0000000h
    DQ 3e67dc255ac40886h
    DQ 3ff30c7ef0000000h
    DQ 3df219f38e8afafeh
    DQ 3ff3104180000000h
    DQ 3e62416bf9669a04h
    DQ 3ff3140280000000h
    DQ 3e611c96b2b3987fh
    DQ 3ff317c210000000h
    DQ 3e6f99ed447e1177h
    DQ 3ff31b8020000000h
    DQ 3e13245826328a11h
    DQ 3ff31f3cd0000000h
    DQ 3e66f56dd1e645f8h
    DQ 3ff322f7f0000000h
    DQ 3e46164946945535h
    DQ 3ff326b1b0000000h
    DQ 3e5e37d59d190028h
    DQ 3ff32a69f0000000h
    DQ 3e668671f12bf828h
    DQ 3ff32e20c0000000h
    DQ 3e6e8ecbca6aabbdh
    DQ 3ff331d620000000h
    DQ 3e53f49e109a5912h
    DQ 3ff3358a20000000h
    DQ 3e6b8a0e11ec3043h
    DQ 3ff3393ca0000000h
    DQ 3e65fae00aed691ah
    DQ 3ff33cedc0000000h
    DQ 3e6c0569bece3e4ah
    DQ 3ff3409d70000000h
    DQ 3e605e26744efbfeh
    DQ 3ff3444bc0000000h
    DQ 3e65b570a94be5c5h
    DQ 3ff347f8a0000000h
    DQ 3e5d6f156ea0e063h
    DQ 3ff34ba420000000h
    DQ 3e6e0ca7612fc484h
    DQ 3ff34f4e30000000h
    DQ 3e4963c927b25258h
    DQ 3ff352f6f0000000h
    DQ 3e547930aa725a5ch
    DQ 3ff3569e40000000h
    DQ 3e58a79fe3af43b3h
    DQ 3ff35a4430000000h
    DQ 3e5e6dc29c41bdafh
    DQ 3ff35de8c0000000h
    DQ 3e657a2e76f863a5h
    DQ 3ff3618bf0000000h
    DQ 3e2ae3b61716354dh
    DQ 3ff3652dd0000000h
    DQ 3e665fb5df6906b1h
    DQ 3ff368ce40000000h
    DQ 3e66177d7f588f7bh
    DQ 3ff36c6d60000000h
    DQ 3e3ad55abd091b67h
    DQ 3ff3700b30000000h
    DQ 3e155337b2422d76h
    DQ 3ff373a7a0000000h
    DQ 3e6084ebe86972d5h
    DQ 3ff37742b0000000h
    DQ 3e656395808e1ea3h
    DQ 3ff37adc70000000h
    DQ 3e61bce21b40fba7h
    DQ 3ff37e74e0000000h
    DQ 3e5006f94605b515h
    DQ 3ff3820c00000000h
    DQ 3e6aa676aceb1f7dh
    DQ 3ff385a1c0000000h
    DQ 3e58229f76554ce6h
    DQ 3ff3893640000000h
    DQ 3e6eabfc6cf57330h
    DQ 3ff38cc960000000h
    DQ 3e64daed9c0ce8bch
    DQ 3ff3905b40000000h
    DQ 3e60ff1768237141h
    DQ 3ff393ebd0000000h
    DQ 3e6575f83051b085h
    DQ 3ff3977b10000000h
    DQ 3e42667deb523e29h
    DQ 3ff39b0910000000h
    DQ 3e1816996954f4fdh
    DQ 3ff39e95c0000000h
    DQ 3e587cfccf4d9cd4h
    DQ 3ff3a22120000000h
    DQ 3e52c5d018198353h
    DQ 3ff3a5ab40000000h
    DQ 3e6a7a898dcc34aah
    DQ 3ff3a93410000000h
    DQ 3e2cead6dadc36d1h
    DQ 3ff3acbbb0000000h
    DQ 3e2a55759c498bdfh
    DQ 3ff3b04200000000h
    DQ 3e6c414a9ef6de04h
    DQ 3ff3b3c700000000h
    DQ 3e63e2108a6e58fah
    DQ 3ff3b74ad0000000h
    DQ 3e5587fd7643d77ch
    DQ 3ff3bacd60000000h
    DQ 3e3901eb1d3ff3dfh
    DQ 3ff3be4eb0000000h
    DQ 3e6f2ccd7c812fc6h
    DQ 3ff3c1ceb0000000h
    DQ 3e21c8ee70a01049h
    DQ 3ff3c54d90000000h
    DQ 3e563e8d02831eech
    DQ 3ff3c8cb20000000h
    DQ 3e6f61a42a92c7ffh
    DQ 3ff3cc4770000000h
    DQ 3dda917399c84d24h
    DQ 3ff3cfc2a0000000h
    DQ 3e5e9197c8eec2f0h
    DQ 3ff3d33c80000000h
    DQ 3e5e6f842f5a1378h
    DQ 3ff3d6b530000000h
    DQ 3e2fac242a90a0fch
    DQ 3ff3da2cb0000000h
    DQ 3e535ed726610227h
    DQ 3ff3dda2f0000000h
    DQ 3e50e0d64804b15bh
    DQ 3ff3e11800000000h
    DQ 3e0560675daba814h
    DQ 3ff3e48be0000000h
    DQ 3e637388c8768032h
    DQ 3ff3e7fe80000000h
    DQ 3e3ee3c89f9e01f5h
    DQ 3ff3eb7000000000h
    DQ 3e639f6f0d09747ch
    DQ 3ff3eee040000000h
    DQ 3e4322c327abb8f0h
    DQ 3ff3f24f60000000h
    DQ 3e6961b347c8ac80h
    DQ 3ff3f5bd40000000h
    DQ 3e63711fbbd0f118h
    DQ 3ff3f92a00000000h
    DQ 3e64fad8d7718ffbh
    DQ 3ff3fc9590000000h
    DQ 3e6fffffffffffffh
    DQ 3ff3fffff0000000h
    DQ 3e667efa79ec35b4h
    DQ 3ff4036930000000h
    DQ 3e6a737687a254a8h
    DQ 3ff406d140000000h
    DQ 3e5bace0f87d924dh
    DQ 3ff40a3830000000h
    DQ 3e629e37c237e392h
    DQ 3ff40d9df0000000h
    DQ 3e557ce7ac3f3012h
    DQ 3ff4110290000000h
    DQ 3e682829359f8fbdh
    DQ 3ff4146600000000h
    DQ 3e6cc9be42d14676h
    DQ 3ff417c850000000h
    DQ 3e6a8f001c137d0bh
    DQ 3ff41b2980000000h
    DQ 3e636127687dda05h
    DQ 3ff41e8990000000h
    DQ 3e524dba322646f0h
    DQ 3ff421e880000000h
    DQ 3e6dc43f1ed210b4h
    DQ 3ff4254640000000h
    DQ 3e631ae515c447bbh
    DQ 3ff428a2f0000000h
                         

ALIGN 16
__CBRT_F_H_256:   
                    DQ 3ff0000000000000h
                    DQ 3ff0055380000000h
                    DQ 3ff00aa390000000h
                    DQ 3ff00ff010000000h
                    DQ 3ff0153920000000h
                    DQ 3ff01a7eb0000000h
                    DQ 3ff01fc0d0000000h
                    DQ 3ff024ff80000000h
                    DQ 3ff02a3ad0000000h
                    DQ 3ff02f72b0000000h
                    DQ 3ff034a750000000h
                    DQ 3ff039d880000000h
                    DQ 3ff03f0670000000h
                    DQ 3ff0443110000000h
                    DQ 3ff0495870000000h
                    DQ 3ff04e7c80000000h
                    DQ 3ff0539d60000000h
                    DQ 3ff058bb00000000h
                    DQ 3ff05dd570000000h
                    DQ 3ff062ecc0000000h
                    DQ 3ff06800e0000000h
                    DQ 3ff06d11e0000000h
                    DQ 3ff0721fc0000000h
                    DQ 3ff0772a80000000h
                    DQ 3ff07c3230000000h
                    DQ 3ff08136d0000000h
                    DQ 3ff0863860000000h
                    DQ 3ff08b36f0000000h
                    DQ 3ff0903280000000h
                    DQ 3ff0952b10000000h
                    DQ 3ff09a20a0000000h
                    DQ 3ff09f1340000000h
                    DQ 3ff0a402f0000000h
                    DQ 3ff0a8efc0000000h
                    DQ 3ff0add990000000h
                    DQ 3ff0b2c090000000h
                    DQ 3ff0b7a4b0000000h
                    DQ 3ff0bc85f0000000h
                    DQ 3ff0c16450000000h
                    DQ 3ff0c63fe0000000h
                    DQ 3ff0cb18b0000000h
                    DQ 3ff0cfeeb0000000h
                    DQ 3ff0d4c1e0000000h
                    DQ 3ff0d99250000000h
                    DQ 3ff0de6010000000h
                    DQ 3ff0e32b00000000h
                    DQ 3ff0e7f340000000h
                    DQ 3ff0ecb8d0000000h
                    DQ 3ff0f17bb0000000h
                    DQ 3ff0f63bf0000000h
                    DQ 3ff0faf970000000h
                    DQ 3ff0ffb460000000h
                    DQ 3ff1046cb0000000h
                    DQ 3ff1092250000000h
                    DQ 3ff10dd560000000h
                    DQ 3ff11285e0000000h
                    DQ 3ff11733d0000000h
                    DQ 3ff11bdf30000000h
                    DQ 3ff1208800000000h
                    DQ 3ff1252e40000000h
                    DQ 3ff129d210000000h
                    DQ 3ff12e7350000000h
                    DQ 3ff1331210000000h
                    DQ 3ff137ae60000000h
                    DQ 3ff13c4840000000h
                    DQ 3ff140dfa0000000h
                    DQ 3ff1457490000000h
                    DQ 3ff14a0710000000h
                    DQ 3ff14e9730000000h
                    DQ 3ff15324e0000000h
                    DQ 3ff157b030000000h
                    DQ 3ff15c3920000000h
                    DQ 3ff160bfc0000000h
                    DQ 3ff16543f0000000h
                    DQ 3ff169c5d0000000h
                    DQ 3ff16e4560000000h
                    DQ 3ff172c2a0000000h
                    DQ 3ff1773d90000000h
                    DQ 3ff17bb630000000h
                    DQ 3ff1802c90000000h
                    DQ 3ff184a0a0000000h
                    DQ 3ff1891270000000h
                    DQ 3ff18d8210000000h
                    DQ 3ff191ef60000000h
                    DQ 3ff1965a80000000h
                    DQ 3ff19ac360000000h
                    DQ 3ff19f2a10000000h
                    DQ 3ff1a38e90000000h
                    DQ 3ff1a7f0e0000000h
                    DQ 3ff1ac5100000000h
                    DQ 3ff1b0af00000000h
                    DQ 3ff1b50ad0000000h
                    DQ 3ff1b96480000000h
                    DQ 3ff1bdbc10000000h
                    DQ 3ff1c21180000000h
                    DQ 3ff1c664d0000000h
                    DQ 3ff1cab610000000h
                    DQ 3ff1cf0530000000h
                    DQ 3ff1d35230000000h
                    DQ 3ff1d79d30000000h
                    DQ 3ff1dbe620000000h
                    DQ 3ff1e02cf0000000h
                    DQ 3ff1e471d0000000h
                    DQ 3ff1e8b490000000h
                    DQ 3ff1ecf550000000h
                    DQ 3ff1f13410000000h
                    DQ 3ff1f570d0000000h
                    DQ 3ff1f9ab90000000h
                    DQ 3ff1fde450000000h
                    DQ 3ff2021b20000000h
                    DQ 3ff2064ff0000000h
                    DQ 3ff20a82c0000000h
                    DQ 3ff20eb3b0000000h
                    DQ 3ff212e2a0000000h
                    DQ 3ff2170fb0000000h
                    DQ 3ff21b3ac0000000h
                    DQ 3ff21f63f0000000h
                    DQ 3ff2238b40000000h
                    DQ 3ff227b0a0000000h
                    DQ 3ff22bd420000000h
                    DQ 3ff22ff5c0000000h
                    DQ 3ff2341570000000h
                    DQ 3ff2383350000000h
                    DQ 3ff23c4f60000000h
                    DQ 3ff2406980000000h
                    DQ 3ff24481d0000000h
                    DQ 3ff2489850000000h
                    DQ 3ff24cad00000000h
                    DQ 3ff250bfe0000000h
                    DQ 3ff254d0e0000000h
                    DQ 3ff258e020000000h
                    DQ 3ff25ced90000000h
                    DQ 3ff260f940000000h
                    DQ 3ff2650320000000h
                    DQ 3ff2690b40000000h
                    DQ 3ff26d1190000000h
                    DQ 3ff2711630000000h
                    DQ 3ff2751900000000h
                    DQ 3ff2791a20000000h
                    DQ 3ff27d1980000000h
                    DQ 3ff2811720000000h
                    DQ 3ff2851310000000h
                    DQ 3ff2890d50000000h
                    DQ 3ff28d05d0000000h
                    DQ 3ff290fca0000000h
                    DQ 3ff294f1c0000000h
                    DQ 3ff298e530000000h
                    DQ 3ff29cd700000000h
                    DQ 3ff2a0c710000000h
                    DQ 3ff2a4b580000000h
                    DQ 3ff2a8a250000000h
                    DQ 3ff2ac8d70000000h
                    DQ 3ff2b076f0000000h
                    DQ 3ff2b45ec0000000h
                    DQ 3ff2b84500000000h
                    DQ 3ff2bc29a0000000h
                    DQ 3ff2c00c90000000h
                    DQ 3ff2c3ee00000000h
                    DQ 3ff2c7cdc0000000h
                    DQ 3ff2cbabf0000000h
                    DQ 3ff2cf8880000000h
                    DQ 3ff2d36390000000h
                    DQ 3ff2d73d00000000h
                    DQ 3ff2db14d0000000h
                    DQ 3ff2deeb20000000h
                    DQ 3ff2e2bfe0000000h
                    DQ 3ff2e69310000000h
                    DQ 3ff2ea64b0000000h
                    DQ 3ff2ee34d0000000h
                    DQ 3ff2f20360000000h
                    DQ 3ff2f5d070000000h
                    DQ 3ff2f99bf0000000h
                    DQ 3ff2fd65f0000000h
                    DQ 3ff3012e70000000h
                    DQ 3ff304f570000000h
                    DQ 3ff308baf0000000h
                    DQ 3ff30c7ef0000000h
                    DQ 3ff3104180000000h
                    DQ 3ff3140280000000h
                    DQ 3ff317c210000000h
                    DQ 3ff31b8020000000h
                    DQ 3ff31f3cd0000000h
                    DQ 3ff322f7f0000000h
                    DQ 3ff326b1b0000000h
                    DQ 3ff32a69f0000000h
                    DQ 3ff32e20c0000000h
                    DQ 3ff331d620000000h
                    DQ 3ff3358a20000000h
                    DQ 3ff3393ca0000000h
                    DQ 3ff33cedc0000000h
                    DQ 3ff3409d70000000h
                    DQ 3ff3444bc0000000h
                    DQ 3ff347f8a0000000h
                    DQ 3ff34ba420000000h
                    DQ 3ff34f4e30000000h
                    DQ 3ff352f6f0000000h
                    DQ 3ff3569e40000000h
                    DQ 3ff35a4430000000h
                    DQ 3ff35de8c0000000h
                    DQ 3ff3618bf0000000h
                    DQ 3ff3652dd0000000h
                    DQ 3ff368ce40000000h
                    DQ 3ff36c6d60000000h
                    DQ 3ff3700b30000000h
                    DQ 3ff373a7a0000000h
                    DQ 3ff37742b0000000h
                    DQ 3ff37adc70000000h
                    DQ 3ff37e74e0000000h
                    DQ 3ff3820c00000000h
                    DQ 3ff385a1c0000000h
                    DQ 3ff3893640000000h
                    DQ 3ff38cc960000000h
                    DQ 3ff3905b40000000h
                    DQ 3ff393ebd0000000h
                    DQ 3ff3977b10000000h
                    DQ 3ff39b0910000000h
                    DQ 3ff39e95c0000000h
                    DQ 3ff3a22120000000h
                    DQ 3ff3a5ab40000000h
                    DQ 3ff3a93410000000h
                    DQ 3ff3acbbb0000000h
                    DQ 3ff3b04200000000h
                    DQ 3ff3b3c700000000h
                    DQ 3ff3b74ad0000000h
                    DQ 3ff3bacd60000000h
                    DQ 3ff3be4eb0000000h
                    DQ 3ff3c1ceb0000000h
                    DQ 3ff3c54d90000000h
                    DQ 3ff3c8cb20000000h
                    DQ 3ff3cc4770000000h
                    DQ 3ff3cfc2a0000000h
                    DQ 3ff3d33c80000000h
                    DQ 3ff3d6b530000000h
                    DQ 3ff3da2cb0000000h
                    DQ 3ff3dda2f0000000h
                    DQ 3ff3e11800000000h
                    DQ 3ff3e48be0000000h
                    DQ 3ff3e7fe80000000h
                    DQ 3ff3eb7000000000h
                    DQ 3ff3eee040000000h
                    DQ 3ff3f24f60000000h
                    DQ 3ff3f5bd40000000h
                    DQ 3ff3f92a00000000h
                    DQ 3ff3fc9590000000h
                    DQ 3ff3fffff0000000h
                    DQ 3ff4036930000000h
                    DQ 3ff406d140000000h
                    DQ 3ff40a3830000000h
                    DQ 3ff40d9df0000000h
                    DQ 3ff4110290000000h
                    DQ 3ff4146600000000h
                    DQ 3ff417c850000000h
                    DQ 3ff41b2980000000h
                    DQ 3ff41e8990000000h
                    DQ 3ff421e880000000h
                    DQ 3ff4254640000000h

ALIGN 16
__CBRT_F_T_256:    
                    DQ 0000000000000000h
                    DQ 3e6e6a24c81e4294h
                    DQ 3e58548511e3a785h
                    DQ 3e64eb9336ec07f6h
                    DQ 3e40ea64b8b750e1h
                    DQ 3e461637cff8a53ch
                    DQ 3e40733bf7bd1943h
                    DQ 3e5666911345ccedh
                    DQ 3e477b7a3f592f14h
                    DQ 3e6f18d3dd1a5402h
                    DQ 3e2be2f5a58ee9a4h
                    DQ 3e68901f8f085fa7h
                    DQ 3e5c68b8cd5b5d69h
                    DQ 3e5a6b0e8624be42h
                    DQ 3dbc4b22b06f68e7h
                    DQ 3e60f3f0afcabe9bh
                    DQ 3e548495bca4e1b7h
                    DQ 3e66107f1abdfdc3h
                    DQ 3e6e67261878288ah
                    DQ 3e5a6bc155286f1eh
                    DQ 3e58a759c64a85f2h
                    DQ 3e45fce70a4a8d09h
                    DQ 3e32f9cbf373fe1dh
                    DQ 3e590564ce4ac359h
                    DQ 3e5ac29ce761b02fh
                    DQ 3e5cb752f497381ch
                    DQ 3e68bb9e1cfb35e0h
                    DQ 3e65b4917099de90h
                    DQ 3e5cc77ac9c65ef2h
                    DQ 3e57a0f3e7be3dbah
                    DQ 3e66ec851ee0c16fh
                    DQ 3e689449bf2946dah
                    DQ 3e698f25301ba223h
                    DQ 3e347d5ec651f549h
                    DQ 3e6c33ec9a86007ah
                    DQ 3e5e0b6653e92649h
                    DQ 3e3bd64ac09d755fh
                    DQ 3e2f537506f78167h
                    DQ 3e62c382d1b3735eh
                    DQ 3e6e20ed659f99e1h
                    DQ 3e586b633a9c182ah
                    DQ 3e445cfd5a65e777h
                    DQ 3e60c8770f58bca4h
                    DQ 3e6739e44b0933c5h
                    DQ 3e027dc3d9ce7bd8h
                    DQ 3e63c53c7c5a7b64h
                    DQ 3e69669683830cech
                    DQ 3e68d772c39bdcc4h
                    DQ 3e69b0008bcf6d7bh
                    DQ 3e3bbb305825ce4fh
                    DQ 3e6da3f4af13a406h
                    DQ 3e5f36b96f74ce86h
                    DQ 3e165c002303f790h
                    DQ 3e682f84095ba7d5h
                    DQ 3e6d46433541b2c6h
                    DQ 3e671c3d56e93a89h
                    DQ 3e598dcef4e40012h
                    DQ 3e4530ebef17fe03h
                    DQ 3e4e8b8fa3715066h
                    DQ 3e6ab26eb3b211dch
                    DQ 3e454dd4dc906307h
                    DQ 3e5c9f962387984eh
                    DQ 3e6c62a959afec09h
                    DQ 3e6638d9ac6a866ah
                    DQ 3e338704eca8a22dh
                    DQ 3e4e6c9e1db14f8fh
                    DQ 3e58744b7f9c9eaah
                    DQ 3e66c2893486373bh
                    DQ 3e5b36bce31699b7h
                    DQ 3e671e3813d200c7h
                    DQ 3e699755ab40aa88h
                    DQ 3e6b45ca0e4bcfc0h
                    DQ 3e32dd090d869c5dh
                    DQ 3e64fe0516b917dah
                    DQ 3e694563226317a2h
                    DQ 3e653d8fafc2c851h
                    DQ 3e5dcbd41fbd41a3h
                    DQ 3e5862ff5285f59ch
                    DQ 3e63072ea97a1e1ch
                    DQ 3e52839075184805h
                    DQ 3e64b0323e9eff42h
                    DQ 3e6b158893c45484h
                    DQ 3e3149ef0fc35826h
                    DQ 3e5f2e77ea96acaah
                    DQ 3e5200074c471a95h
                    DQ 3e63f8cc517f6f04h
                    DQ 3e660ba2e311bb55h
                    DQ 3e64b788730bbec3h
                    DQ 3e657090795ee20ch
                    DQ 3e6d9ffe983670b1h
                    DQ 3e62a463ff61bfdah
                    DQ 3e69d1bc6a5e65cfh
                    DQ 3e68718abaa9e922h
                    DQ 3e63c2f52ffa342eh
                    DQ 3e60fae13ff42c80h
                    DQ 3e65440f0ef00d57h
                    DQ 3e46fcd22d4e3c1eh
                    DQ 3e4e0c60b409e863h
                    DQ 3e6f9cab5a5f0333h
                    DQ 3e630f24744c333dh
                    DQ 3e4b50622a76b2feh
                    DQ 3e6fdb94ba595375h
                    DQ 3e3861b9b945a171h
                    DQ 3e654348015188c4h
                    DQ 3e6b54d149865523h
                    DQ 3e6a0bb783d9de33h
                    DQ 3e6629d12b1a2157h
                    DQ 3e6467fe35d179dfh
                    DQ 3e69763f3e26c8f7h
                    DQ 3e53f798bb9f7679h
                    DQ 3e552e577e855898h
                    DQ 3e6fde47e5502c3ah
                    DQ 3e5cbd0b548d96a0h
                    DQ 3e6a9cd9f7be8de8h
                    DQ 3e522bbe704886deh
                    DQ 3e6e3dea8317f020h
                    DQ 3e6e812085ac8855h
                    DQ 3e5c87144f24cb07h
                    DQ 3e61e128ee311fa2h
                    DQ 3e5b5c163d61a2d3h
                    DQ 3e47d97e7fb90633h
                    DQ 3e6efe899d50f6a7h
                    DQ 3e6d0333eb75de5ah
                    DQ 3e40e590be73a573h
                    DQ 3e68ce8dcac3cdd2h
                    DQ 3e6ee8a48954064bh
                    DQ 3e6aa62f18461e09h
                    DQ 3e601e5940986a15h
                    DQ 3e3b082f4f9b8d4ch
                    DQ 3e6876e0e5527f5ah
                    DQ 3e63617080831e6bh
                    DQ 3e681b26e34aa4a2h
                    DQ 3e552ee66dfab0c1h
                    DQ 3e5d85a5329e8819h
                    DQ 3e5105c1b646b5d1h
                    DQ 3e6bb6690c1a379ch
                    DQ 3e586aeba73ce3a9h
                    DQ 3e6dd16198294dd4h
                    DQ 3e6454e675775e83h
                    DQ 3e63842e026197eah
                    DQ 3e6f1ce0e70c44d2h
                    DQ 3e6ad636441a5627h
                    DQ 3e54c205d7212abbh
                    DQ 3e6167c86c116419h
                    DQ 3e638ec3ef16e294h
                    DQ 3e6473fceace9321h
                    DQ 3e67af53a836dba7h
                    DQ 3e1a51f3c383b652h
                    DQ 3e63696da190822dh
                    DQ 3e62f9adec77074bh
                    DQ 3e38190fd5bee55fh
                    DQ 3e4bfee8fac68e55h
                    DQ 3e331c9d6bc5f68ah
                    DQ 3e689d0523737edfh
                    DQ 3e5a295943bf47bbh
                    DQ 3e396be32e5b3207h
                    DQ 3e6e44c7d909fa0eh
                    DQ 3e2b2505da94d9eah
                    DQ 3e60c851f46c9c98h
                    DQ 3e5da71f7d9aa3b7h
                    DQ 3e6f1b605d019ef1h
                    DQ 3e4386e8a2189563h
                    DQ 3e3b19fa5d306ba7h
                    DQ 3e6dd749b67aef76h
                    DQ 3e676ff6f1dc04b0h
                    DQ 3e635a33d0b232a6h
                    DQ 3e64bdc80024a4e1h
                    DQ 3e6ebd61770fd723h
                    DQ 3e64769fc537264dh
                    DQ 3e69021f429f3b98h
                    DQ 3e5ee7083efbd606h
                    DQ 3e6ad985552a6b1ah
                    DQ 3e6e3df778772160h
                    DQ 3e6ca5d76ddc9b34h
                    DQ 3e691154ffdbaf74h
                    DQ 3e667bdd57fb306ah
                    DQ 3e67dc255ac40886h
                    DQ 3df219f38e8afafeh
                    DQ 3e62416bf9669a04h
                    DQ 3e611c96b2b3987fh
                    DQ 3e6f99ed447e1177h
                    DQ 3e13245826328a11h
                    DQ 3e66f56dd1e645f8h
                    DQ 3e46164946945535h
                    DQ 3e5e37d59d190028h
                    DQ 3e668671f12bf828h
                    DQ 3e6e8ecbca6aabbdh
                    DQ 3e53f49e109a5912h
                    DQ 3e6b8a0e11ec3043h
                    DQ 3e65fae00aed691ah
                    DQ 3e6c0569bece3e4ah
                    DQ 3e605e26744efbfeh
                    DQ 3e65b570a94be5c5h
                    DQ 3e5d6f156ea0e063h
                    DQ 3e6e0ca7612fc484h
                    DQ 3e4963c927b25258h
                    DQ 3e547930aa725a5ch
                    DQ 3e58a79fe3af43b3h
                    DQ 3e5e6dc29c41bdafh
                    DQ 3e657a2e76f863a5h
                    DQ 3e2ae3b61716354dh
                    DQ 3e665fb5df6906b1h
                    DQ 3e66177d7f588f7bh
                    DQ 3e3ad55abd091b67h
                    DQ 3e155337b2422d76h
                    DQ 3e6084ebe86972d5h
                    DQ 3e656395808e1ea3h
                    DQ 3e61bce21b40fba7h
                    DQ 3e5006f94605b515h
                    DQ 3e6aa676aceb1f7dh
                    DQ 3e58229f76554ce6h
                    DQ 3e6eabfc6cf57330h
                    DQ 3e64daed9c0ce8bch
                    DQ 3e60ff1768237141h
                    DQ 3e6575f83051b085h
                    DQ 3e42667deb523e29h
                    DQ 3e1816996954f4fdh
                    DQ 3e587cfccf4d9cd4h
                    DQ 3e52c5d018198353h
                    DQ 3e6a7a898dcc34aah
                    DQ 3e2cead6dadc36d1h
                    DQ 3e2a55759c498bdfh
                    DQ 3e6c414a9ef6de04h
                    DQ 3e63e2108a6e58fah
                    DQ 3e5587fd7643d77ch
                    DQ 3e3901eb1d3ff3dfh
                    DQ 3e6f2ccd7c812fc6h
                    DQ 3e21c8ee70a01049h
                    DQ 3e563e8d02831eech
                    DQ 3e6f61a42a92c7ffh
                    DQ 3dda917399c84d24h
                    DQ 3e5e9197c8eec2f0h
                    DQ 3e5e6f842f5a1378h
                    DQ 3e2fac242a90a0fch
                    DQ 3e535ed726610227h
                    DQ 3e50e0d64804b15bh
                    DQ 3e0560675daba814h
                    DQ 3e637388c8768032h
                    DQ 3e3ee3c89f9e01f5h
                    DQ 3e639f6f0d09747ch
                    DQ 3e4322c327abb8f0h
                    DQ 3e6961b347c8ac80h
                    DQ 3e63711fbbd0f118h
                    DQ 3e64fad8d7718ffbh
                    DQ 3e6fffffffffffffh
                    DQ 3e667efa79ec35b4h
                    DQ 3e6a737687a254a8h
                    DQ 3e5bace0f87d924dh
                    DQ 3e629e37c237e392h
                    DQ 3e557ce7ac3f3012h
                    DQ 3e682829359f8fbdh
                    DQ 3e6cc9be42d14676h
                    DQ 3e6a8f001c137d0bh
                    DQ 3e636127687dda05h
                    DQ 3e524dba322646f0h
                    DQ 3e6dc43f1ed210b4h

ALIGN 16
__INV_TAB_256:    
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

CONST    ENDS
data ENDS

END
