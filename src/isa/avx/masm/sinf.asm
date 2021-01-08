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
; An implementation of the sinf function.
;
; Prototype   
;
;     double sinf(double x);
;
;   Computes sinf(x).
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the NAG C implementation.
;
;

CONST	SEGMENT
ALIGN 16
L__real_3ff0000000000000    DQ 03ff0000000000000h     ; 1.0
                        DQ 0                          ; for alignment
L__real_3fe0000000000000    DQ 03fe0000000000000h     ; 0.5
                        DQ 0
L__real_3fc5555555555555    DQ 03fc5555555555555h     ; 0.166666666666
                        DQ 0
L__real_3fe45f306dc9c883    DQ 03fe45f306dc9c883h     ; twobypi
                        DQ 0
L__real_3ff921fb54400000    DQ 03ff921fb54400000h     ; piby2_1
                        DQ 0
L__real_3dd0b4611a626331    DQ 03dd0b4611a626331h     ; piby2_1tail
                        DQ 0
L__real_3dd0b4611a600000    DQ 03dd0b4611a600000h     ; piby2_2
                        DQ 0
L__real_3ba3198a2e037073    DQ 03ba3198a2e037073h     ; piby2_2tail
                        DQ 0                    
L__real_411E848000000000    DQ 0415312d000000000h     ; 5e6 0411E848000000000h     ; 5e5
                        DQ 0

ALIGN 16
Lcosfarray:  
    DQ    0bfe0000000000000h                          ; -0.5              c0
    DQ    0
    DQ    03fa5555555555555h                          ; 0.0416667         c1
    DQ    0
    DQ    0bf56c16c16c16c16h                          ; -0.00138889       c2
    DQ    0
    DQ    03EFA01A01A01A019h                          ; 2.48016e-005      c3
    DQ    0
    DQ    0be927e4fb7789f5ch                          ; -2.75573e-007     c4
    DQ    0

ALIGN 16    
Lsinfarray: 
    DQ    0bfc5555555555555h                          ; -0.166667         s1
    DQ    0
    DQ    03f81111111111111h                          ; 0.00833333        s2
    DQ    0
    DQ    0bf2a01a01a01a01ah                          ; -0.000198413      s3
    DQ    0
    DQ    03ec71de3a556c734h                          ; 2.75573e-006      s4
    DQ    0
CONST	ENDS

; define local variable storage offsets
p_temp		equ		030h		                      ; temporary for get/put bits operation
p_temp1		equ		040h		                      ; temporary for get/put bits operation
region		equ		050h		                      ; pointer to region for amd_remainder_piby2
r		    equ		060h		                      ; pointer to r for amd_remainder_piby2
stack_size  equ     088h

include fm.inc

FN_PROTOTYPE_BAS64 sinf

fname_special   TEXTEQU <_sinf_special>

;Define name and any external functions being called
EXTRN 		__amd_remainder_piby2d2f : PROC    ; NEAR
EXTERN      fname_special        : PROC


StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
StackAllocate stack_size
    .ENDPROLOG   
    xorpd   xmm2, xmm2                                ; zeroed out for later use

;;  if NaN or inf
	movd    edx, xmm0
    mov     eax, 07f800000h
    mov     r10d, eax
    and     r10d, edx
    cmp     r10d, eax 
    jz      Lsinf_naninf

; GET_BITS_DP64(x, ux);
; get the input value to an integer register.
    cvtss2sd   xmm0, xmm0                             ; convert input to double.
    movsd   QWORD PTR p_temp[rsp], xmm0               ; get the input value to an integer register.              
    mov     rdx, QWORD PTR p_temp[rsp]                ; rdx is ux

;  ax = (ux & ~SIGNBIT_DP64);
    mov     r10, 07fffffffffffffffh
    and     r10, rdx                                  ; r10 is ax
    mov     r8d, 1                                    ; for determining region later on

;;  if (ax <= 0x3fe921fb54442d18)  abs(x) <= pi/4 
    mov     rax, 03fe921fb54442d18h
    cmp     r10, rax
    jg      Lsinf_reduce

;;      if (ax < 0x3f80000000000000)  abs(x) < 2.0^(-7) 
    mov     rax, 3f80000000000000h
    cmp     r10, rax
    jge     Lsinf_small

;;          if (ax < 0x3f20000000000000) abs(x) < 2.0^(-13)
    mov     rax, 3f20000000000000h
    cmp     r10, rax
    jge     Lsinf_smaller

;                  sinf = x;
    jmp     Lsinf_cleanup 

;;          else
Lsinf_smaller:   
;              sinf = x - x^3 * 0.1666666666666666666;
    movsd   xmm2, xmm0
    movsd   xmm4, QWORD PTR L__real_3fc5555555555555  ; 0.1666666666666666666
    mulsd   xmm2, xmm2                                ; x^2
    mulsd   xmm2, xmm0                                ; x^3
    mulsd   xmm2, xmm4                                ; x^3 * 0.1666666666666666666
    subsd   xmm0, xmm2                                ; x - x^3 * 0.1666666666666666666
    jmp     Lsinf_cleanup  

;;      else
Lsinf_small:   
    movsd   xmm2, xmm0                                ; x2 = r * r;
    mulsd   xmm2, xmm0                                ; x2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 0 or 2     - do a sinf calculation
;  zs = x + x3((s1 + x2 * s2) + x4(s3 + x2 * s4));
    movsd   xmm1, QWORD PTR Lsinfarray+30h            ; s4
    mulsd   xmm1, xmm2                                ; s4x2
    movsd   xmm4, xmm2                                ; move for x4
    movsd   xmm5, QWORD PTR Lsinfarray+10h            ; s2
    mulsd   xmm4, xmm2                                ; x4
    movsd   xmm3, xmm0                                ; move for x3
    mulsd   xmm5, xmm2                                ; s2x2
    mulsd   xmm3, xmm2                                ; x3        
    addsd   xmm1, QWORD PTR Lsinfarray+20h            ; s3+s4x2
    mulsd   xmm1, xmm4                                ; s3x4+s4x6
    addsd   xmm5, QWORD PTR Lsinfarray                ; s1+s2x2
    addsd   xmm1, xmm5                                ; s1+s2x2+s3x4+s4x6
    mulsd   xmm1, xmm3                                ; x3(s1+s2x2+s3x4+s4x6)
    addsd   xmm0, xmm1                                ; x + x3(s1+s2x2+s3x4+s4x6)

    jmp     Lsinf_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsinf_reduce:   
;  xneg = (ax != ux);
    cmp     rdx, r10
    mov     r11d, 0
;;  if (xneg) x = -x;
    jz      L50e5
    
    mov     r11d, 1
    subsd   xmm2, xmm0
    movsd   xmm0, xmm2

L50e5:
;;  if (x < 5.0e5)
    cmp     r10, QWORD PTR L__real_411E848000000000
    jae     Lsinf_reduce_precise

; reduce  the argument to be in a range from -pi/4 to +pi/4
; by subtracting multiples of pi/2
    movsd   xmm2, xmm0
    movsd   xmm3, QWORD PTR L__real_3fe45f306dc9c883  ; twobypi
    movsd   xmm4, xmm0
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5
    mulsd   xmm2, xmm3

;/* How many pi/2 is x a multiple of? */
;      xexp  = ax >> EXPSHIFTBITS_DP64;
    mov     r9, r10
    shr     r9, 52                                    ; >>EXPSHIFTBITS_DP64

;        npi2  = (int)(x * twobypi + 0.5);
    addsd   xmm2, xmm5                        ; npi2

    movsd   xmm3, QWORD PTR L__real_3ff921fb54400000  ; piby2_1
    cvttpd2dq  xmm0, xmm2                             ; convert to integer
    movsd   xmm1, QWORD PTR L__real_3dd0b4611a626331  ; piby2_1tail
    cvtdq2pd   xmm2, xmm0                             ; and back to double.

;      /* Subtract the multiple from x to get an extra-precision remainder */
;      rhead  = x - npi2 * piby2_1;
    mulsd   xmm3, xmm2
    subsd   xmm4, xmm3                                ; rhead

;      rtail  = npi2 * piby2_1tail;
    mulsd   xmm1, xmm2
    movd    eax, xmm0

;      GET_BITS_DP64(rhead-rtail, uy);                   
; originally only rhead
    movsd   xmm0, xmm4
    subsd   xmm0, xmm1

    movsd   xmm3, QWORD PTR L__real_3dd0b4611a600000  ; piby2_2
    movsd   QWORD PTR p_temp[rsp], xmm0
    movsd   xmm5, QWORD PTR L__real_3ba3198a2e037073  ; piby2_2tail
    mov     rcx, [p_temp+rsp]                         ; rcx is rhead-rtail

;    xmm0=r, xmm4=rhead, xmm1=rtail, xmm2=npi2, xmm3=temp for calc, xmm5= temp for calc
;      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     rcx, 1                                    ; strip any sign bit
    shr     rcx, 53                                   ; >> EXPSHIFTBITS_DP64 +1
    sub     r9, rcx                                   ; expdiff

;;      if (expdiff > 15)
    cmp     r9, 15
    jle     Lexpdiff15

;       The remainder is pretty small compared with x, which
;       implies that x is a near multiple of pi/2
;       (x matches the multiple to at least 15 bits)
;          t  = rhead;
    movsd   xmm1, xmm4

;          rtail  = npi2 * piby2_2;
    mulsd   xmm3, xmm2

;          rhead  = t - rtail;
    mulsd   xmm5, xmm2                                ; npi2 * piby2_2tail
    subsd   xmm4, xmm3                                ; rhead

;          rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
    subsd   xmm1, xmm4                                ; t - rhead
    subsd   xmm1, xmm3                                ; -rtail
    subsd   xmm5, xmm1                                ; rtail

;      r = rhead - rtail;
    movsd   xmm0, xmm4

;HARSHA
;xmm1=rtail
    movsd   xmm1, xmm5
    subsd   xmm0, xmm5

;    xmm0=r, xmm4=rhead, xmm1=rtail
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; if the input was close to a pi/2 multiple
; The original NAG code missed this trick.  If the input is very close to n*pi/2 after
; reduction,
; then the sinf is ~ 1.0 , to within 15 bits, when r is < 2^-13.  We already
; have x at this point, so we can skip the sinf polynomials.
Lexpdiff15:  
    cmp     rcx, 03f2h                                ; if r  small.
    jge     Lsinf_piby4                               ; use taylor series if not
    cmp     rcx, 03deh                                ; if r really small.
    jle     Lr_small                                  ; then sinf(r) = 0

    movsd   xmm2, xmm0
    mulsd   xmm2, xmm2                                ; x^2

;;      if region is 0 or 2 do a sinf calc.
    and     r8d, eax
    jnz     Lcosfregion

; region 0 or 2 do a sinf calculation
; use simply polynomial
;              x - x*x*x*0.166666666666666666;
    movsd   xmm3, QWORD PTR L__real_3fc5555555555555   
    mulsd   xmm3, xmm0                                ; * x
    mulsd   xmm3, xmm2                                ; * x^2
    subsd   xmm0, xmm3                                ; xs
    jmp     Ladjust_region

ALIGN 16
Lcosfregion:   
; region 1 or 3 do a cosf calculation
; use simply polynomial
;              1.0 - x*x*0.5;
    movsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000  ; 0.5 *x^2
    subsd   xmm0, xmm2                                ; xc
    jmp     Ladjust_region

ALIGN 16
Lr_small:
;;      if region is 1 or 3    do a cosf calc.
    and     r8d, eax
    jz      Ladjust_region

; odd
    movsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; cosf(r) is a 1
    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Lsinf_reduce_precise:   
;      Reduce x into range [-pi/4, pi/4]
;      __amd_remainder_piby2d2f(x, &r, &region);

	mov	    QWORD PTR p_temp[rsp], r11
	lea	    rdx,	QWORD PTR r[rsp]
	lea	    r8, 	QWORD PTR region[rsp]
	movd	rcx, xmm0
    sub     rsp, 20h
    
    call    __amd_remainder_piby2d2f

    add     rsp, 20h
	mov	    r11, QWORD PTR p_temp[rsp]
    mov     r8d, 1                                    ; for determining region later on
	movsd	xmm1, QWORD PTR r[rsp]                    ; x
	mov	    eax,  DWORD PTR region[rsp]               ; region

; xmm0 = x, xmm4 = xx, r8d = 1, eax= region
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ALIGN 32
; perform taylor series to calc sinfx, cosfx
Lsinf_piby4:
;  x2 = r * r;
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                                ; x2

;;      if region is 0 or 2    do a sinf calc.
    and     r8d, eax
    jnz     Lcosfregion2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 0 or 2 do a sinf calculation
;  zs = x + x3((s1 + x2 * s2) + x4(s3 + x2 * s4));
    movsd   xmm1, QWORD PTR Lsinfarray+30h            ; s4
    mulsd   xmm1, xmm2                                ; s4x2
    movsd   xmm4, xmm2                                ; move for x4    
    mulsd   xmm4, xmm2                                ; x4
    movsd   xmm5, QWORD PTR Lsinfarray+10h            ; s2
    mulsd   xmm5, xmm2                                ; s2x2
    movsd   xmm3, xmm0                                ; move for x3
    mulsd   xmm3, xmm2                                ; x3        
    addsd   xmm1, QWORD PTR Lsinfarray+20h            ; s3+s4x2
    mulsd   xmm1, xmm4                                ; s3x4+s4x6     
    addsd   xmm5, QWORD PTR Lsinfarray                ; s1+s2x2
    addsd   xmm1, xmm5                                ; s1+s2x2+s3x4+s4x6
    mulsd   xmm1, xmm3                                ; x3(s1+s2x2+s3x4+s4x6)
    addsd   xmm0, xmm1                                ; x + x3(s1+s2x2+s3x4+s4x6)
        
    jmp     Ladjust_region

ALIGN 16
Lcosfregion2:   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 1 or 3     - do a cosf calculation
;  zc = 1-0.5*x2+ c1*x4 +c2*x6 +c3*x8;
;     zc = 1-0.5*x2+ c1*x4 +c2*x6 +c3*x8 + c4*x10 for a higher precision
    movsd   xmm1, QWORD PTR Lcosfarray+40h            ; c4
    movsd   xmm4, xmm2                                ; move for x4
    mulsd   xmm1, xmm2                                ; c4x2
    movsd   xmm3, QWORD PTR Lcosfarray+20h            ; c2
    mulsd   xmm4, xmm2                                ; x4
    movsd   xmm0, QWORD PTR Lcosfarray                ; c0
    mulsd   xmm3, xmm2                                ; c2x2
    mulsd   xmm0, xmm2                                ; c0x2 (=-0.5x2)
    addsd   xmm1, QWORD PTR Lcosfarray+30h            ; c3+c4x2
    mulsd   xmm1, xmm4                                ; c3x4 + c4x6
    addsd   xmm3, QWORD PTR Lcosfarray+10h            ; c1+c2x2
    addsd   xmm1, xmm3                                ; c1 + c2x2 + c3x4 + c4x6
    mulsd   xmm1, xmm4                                ; c1x4 + c2x6 + c3x8 + c4x10
    addsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; 1 - 0.5x2
    addsd   xmm0, xmm1                                ; 1 - 0.5x2 + c1x4 + c2x6 + c3x8 + c4x10

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ALIGN 16
Ladjust_region:           
; positive or negative
;      switch (region)
    shr     eax, 1
    mov     ecx, eax
    and     eax, r11d

    not     ecx
    not     r11d
    and     ecx, r11d

    or      eax, ecx
    and     eax, 1
    jnz     Lsinf_cleanup

;; if the original region 0, 1 and arg is negative, then we negate the result.
;; if the original region 2, 3 and arg is positive, then we negate the result.
    movsd   xmm2, xmm0
    xorpd   xmm0, xmm0
    subsd   xmm0, xmm2

ALIGN 16
Lsinf_cleanup:   
    cvtsd2ss xmm0, xmm0
    add     rsp, stack_size
    ret

ALIGN 16
Lsinf_naninf:   
    call    fname_special
    add     rsp, stack_size
    ret

fname		endp
TEXT	ENDS
END

