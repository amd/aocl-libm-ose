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
; An implementation of the cos function.
;
; Prototype:
;
;     double cos(double x);
;
;   Computes cos(x).
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the NAG C implementation.
;
;

CONST    SEGMENT
ALIGN 16
L__real_7fffffffffffffff    DQ 07fffffffffffffffh     ; Sign bit zero
                        DQ 0
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
L__real_fffffffff8000000    DQ 0fffffffff8000000h     ; mask for stripping head and tail
                        DQ 0                   
L__real_411E848000000000    DQ 0415312d000000000h     ; 5e6 0411E848000000000h     ; 5e5
                        DQ 0
L__real_bfe0000000000000    DQ 0bfe0000000000000h     ; - 0.5
                        DQ 0
                        
ALIGN 16
Lcosarray:
    DQ    03fa5555555555555h                          ; 0.0416667           c1
    DQ    0
    DQ    0bf56c16c16c16967h                          ; -0.00138889         c2
    DQ    0
    DQ    03EFA01A019F4EC91h                          ; 2.48016e-005        c3
    DQ    0
    DQ    0bE927E4FA17F667Bh                          ; -2.75573e-007       c4
    DQ    0
    DQ    03E21EEB690382EECh                          ; 2.08761e-009        c5
    DQ    0
    DQ    0bDA907DB47258AA7h                          ; -1.13826e-011       c6
    DQ    0

ALIGN 16
Lsinarray:
    DQ    0bfc5555555555555h                          ; -0.166667           s1
    DQ    0
    DQ    03f81111111110bb3h                          ; 0.00833333          s2
    DQ    0
    DQ    0bf2a01a019e83e5ch                          ; -0.000198413        s3
    DQ    0
    DQ    03ec71de3796cde01h                          ; 2.75573e-006        s4
    DQ    0
    DQ    0be5ae600b42fdfa7h                          ; -2.50511e-008       s5
    DQ    0
    DQ    03de5e0b2f9a43bb8h                          ; 1.59181e-010        s6
    DQ    0
CONST    ENDS

; define local variable storage offsets
p_temp      equ        030h                           ; temporary for get/put bits operation
p_temp1     equ        040h                           ; temporary for get/put bits operation
r           equ        050h                           ; pointer to r for amd_remainder_piby2
rr          equ        060h                           ; pointer to rr for amd_remainder_piby2
region      equ        070h                           ; pointer to region for amd_remainder_piby2
stack_size  equ        098h

include fm.inc

FN_PROTOTYPE_BAS64 cos

fname_special   TEXTEQU <_cos_special>

;Define name and any external functions being called
EXTRN         __amd_remainder_piby2 : PROC        ; NEAR
EXTERN       fname_special      : PROC

StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
StackAllocate stack_size
    .ENDPROLOG   

    xorpd   xmm2, xmm2                               ; zeroed out for later use

; GET_BITS_DP64(x, ux);
; get the input value to an integer register.
    movsd   QWORD PTR p_temp[rsp], xmm0
    mov     rdx, QWORD PTR p_temp[rsp]                ; rdx is ux

;;  if NaN or inf
    mov     rax, 07ff0000000000000h
    mov     r10, rax
    and     r10, rdx
    cmp     r10, rax
    jz      Lcos_naninf

;  ax = (ux & ~SIGNBIT_DP64);
    mov     r10, 07fffffffffffffffh
    and     r10, rdx                                  ; r10 is ax
    mov     r8d, 1                                    ; for determining region later on


;;  if (ax <= 3fe921fb54442d18h) abs(x) <= pi/4 
    mov     rax, 03fe921fb54442d18h
    cmp     r10, rax
    jg      Lcos_reduce

;;      if (ax < 3f20000000000000h) abs(x) < 2.0^(-13)
    mov     rax, 03f20000000000000h
    cmp     r10, rax
    jge     Lcos_small

;;          if (ax < 3e40000000000000h) abs(x) < 2.0^(-27)
    mov     rax, 03e40000000000000h
    cmp     r10, rax
    jge     Lcos_smaller

;                  cos = 1.0;
    movsd   xmm0, L__real_3ff0000000000000            ; return a 1
    jmp     Lcos_cleanup     

;;          else
ALIGN 16
Lcos_smaller:
;              cos = 1.0 - x*x*0.5;
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm2                                ; x^2

    movsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000  ; 0.5 * x^2
    subsd   xmm0, xmm2
    jmp     Lcos_cleanup
      
;;      else
ALIGN 16
Lcos_small:
;          cos = cos_piby4(x, 0.0);
;    movsd  xmm5, QWORD PTR L__real_3fe0000000000000  ; .5

;  x2 = r * r;
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                                ; x2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 0 or 2     - do a cos calculation
;  zc = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));
    movsd   xmm1, QWORD PTR Lcosarray+10h             ; c2
    movsd   xmm4, xmm2                                ; move for x4
    mulsd   xmm4, xmm2                                ; x4
    movsd   xmm3, QWORD PTR Lcosarray+30h             ; c4
    mulsd   xmm1, xmm2                                ; c2x2
    movsd   xmm5, QWORD PTR Lcosarray+50h             ; c6
    mulsd   xmm3, xmm2                                ; c4x2
    movsd   xmm0, xmm4                                ; move for x8
    mulsd   xmm5, xmm2                                ; c6x2
    mulsd   xmm0, xmm4                                ; x8
    addsd   xmm1, QWORD PTR Lcosarray                 ; c1 + c2x2
    mulsd   xmm1, xmm4                                ; c1x4 + c2x6
    addsd   xmm3, QWORD PTR Lcosarray+20h             ; c3 + c4x2
    mulsd   xmm2, QWORD PTR L__real_bfe0000000000000  ; -0.5x2, destroy xmm2
    addsd   xmm5, QWORD PTR Lcosarray+40h             ; c5 + c6x2
    mulsd   xmm3, xmm0                                ; c3x8 + c4x10    
    mulsd   xmm4, xmm0                                ; x12    
    mulsd   xmm4, xmm5                                ; c5x12 + c6x14

    movsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; 1    
    addsd   xmm1, xmm3                                ; c1x4 + c2x6 + c3x8 + c4x10
    movsd   xmm3, xmm2                                ; preserve -0.5x2
    addsd   xmm2, xmm0                                ; t = 1 - 0.5x2
    subsd   xmm0, xmm2                                ; 1-t
    addsd   xmm0, xmm3                                ; (1-t) - r
    addsd   xmm1, xmm4                                ; c1x4 + c2x6 + c3x8 + c4x10 + c5x12 + c6x14
    addsd   xmm0, xmm1                                ; (1-t) - r + c1x4 + c2x6 + c3x8 + c4x10 + c5x12 + c6x14
    addsd   xmm0, xmm2                                ; 1 - 0.5x2 + above
	
    jmp     Lcos_cleanup

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lcos_reduce:

;  xneg = (ax != ux);
    cmp     rdx, r10

;;  if (xneg) x = -x;
    jz      Lpositive
    subsd   xmm2, xmm0
    movsd   xmm0, xmm2

Lpositive:
;;  if (x < 5.0e5)
    cmp     r10, QWORD PTR L__real_411E848000000000
    jae     Lcos_reduce_precise

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
    addsd   xmm2, xmm5                                ; npi2

    movsd   xmm3, QWORD PTR L__real_3ff921fb54400000  ; piby2_1
    cvttpd2dq    xmm0, xmm2                           ; convert to integer
    movsd   xmm1, QWORD PTR L__real_3dd0b4611a626331  ; piby2_1tail
    cvtdq2pd    xmm2, xmm0                            ; and back to float.

;      /* Subtract the multiple from x to get an extra-precision remainder */
;      rhead  = x - npi2 * piby2_1;
    mulsd   xmm3, xmm2
    subsd   xmm4, xmm3                                ; rhead

;      rtail  = npi2 * piby2_1tail;
    mulsd   xmm1, xmm2
    movd    eax, xmm0

;      GET_BITS_DP64(rhead-rtail, uy);                ; originally only rhead
    movsd   xmm0, xmm4
    subsd   xmm0, xmm1

    movsd   xmm3, QWORD PTR L__real_3dd0b4611a600000  ; piby2_2
    movsd   QWORD PTR p_temp[rsp], xmm0
    movsd   xmm5, QWORD PTR L__real_3ba3198a2e037073  ; piby2_2tail
    mov     rcx, QWORD PTR p_temp[rsp]                ; rcx is rhead-rtail

;    xmm0=r, xmm4=rhead, xmm1=rtail, xmm2=npi2, xmm3=temp for calc, xmm5= temp for calc
;      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     rcx, 1                                    ; strip any sign bit
    shr     rcx, 53                                   ; >> EXPSHIFTBITS_DP64 +1
    sub     r9, rcx                                   ; expdiff

;;      if (expdiff > 15)
    cmp     r9, 15
    jle     Lexpdiffless15

;          /* The remainder is pretty small compared with x, which
;             implies that x is a near multiple of pi/2
;             (x matches the multiple to at least 15 bits) */

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
Lexpdiffless15:
;      region = npi2 & 3;
    subsd   xmm4, xmm0                                ; rhead-r
    subsd   xmm4, xmm1                                ; rr = (rhead-r) - rtail

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; if the input was close to a pi/2 multiple
; The original NAG code missed this trick.  If the input is very close to n*pi/2 after
; reduction,
; then the cos is ~ 1.0 , to within 53 bits, when r is < 2^-27.  We already
; have x at this point, so we can skip the cos polynomials.

    cmp     rcx, 03f2h                                ; if r  small.
    jge     Lcos_piby4                                ; use taylor series if not
    cmp     rcx, 03deh                                ; if r really small.
    jle     Lr_small                                  ; then cos(r) = 1

    movsd   xmm2, xmm0
    mulsd   xmm2, xmm2                                ;x^2

;;      if region is 1 or 3    do a sin calc.
    and     r8d, eax
    jz      Lsinsmall

; region 1 or 3
; use simply polynomial
;              *s = x - x*x*x*0.166666666666666666;
    movsd   xmm3, QWORD PTR L__real_3fc5555555555555  
    mulsd   xmm3, xmm0                                ; * x
    mulsd   xmm3, xmm2                                ; * x^2
    subsd   xmm0, xmm3                                ; xs
    jmp     Ladjust_region

ALIGN 16
Lsinsmall:
; region 0 or 2
;              cos = 1.0 - x*x*0.5;
    movsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000  ; 0.5 *x^2
    subsd   xmm0, xmm2
    jmp     Ladjust_region

ALIGN 16
Lr_small:
;;      if region is 1 or 3    do a sin calc.
    and     r8d, eax
    jnz     Ladjust_region

    movsd   xmm0, QWORD PTR L__real_3ff0000000000000  ; cos(r) is a 1
    jmp     Ladjust_region

ALIGN 16
Lcos_reduce_precise:
;      // Reduce x into range [-pi/4, pi/4]
;      __amd_remainder_piby2(x, &r, &rr, &region);

	lea	    r9, 	QWORD PTR region[rsp]
	lea	    r8, 	QWORD PTR rr[rsp]
	lea	    rdx,	QWORD PTR r[rsp]

;change to MS ABI - shadow space
sub	rsp,020h
	
	call	__amd_remainder_piby2

;change to MS ABI - shadow space
add	rsp,020h
	
	mov	    r8d,1				                      ; for determining region later on
	movsd	xmm0, QWORD PTR r[rsp]		              ; x
	movsd	xmm4, QWORD PTR rr[rsp]		              ; xx
	mov	    eax,  DWORD PTR region[rsp]	              ; region


; xmm0 = x, xmm4 = xx, r8d = 1, eax= region
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
; perform taylor series to calc sinx, cosx
Lcos_piby4:
;  x2 = r * r;

;xmm4 = a part of rr for the sin path, xmm4 is overwritten in the cos path
;instead use xmm3 because that was freed up in the sin path, xmm3 is overwritten in sin path
    movsd   xmm3, xmm0
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                                ;x2

;;      if region is 1 or 3    do a sin calc.
    and     r8d, eax
    jz      Lcospiby4

; region 1 or 3
    movsd   xmm3, QWORD PTR Lsinarray+50h             ; s6
    mulsd   xmm3, xmm2                                ; x2s6
    movsd   xmm5, QWORD PTR Lsinarray+20h             ; s3
    movsd   QWORD PTR p_temp[rsp], xmm4               ; store xx
    movsd   xmm1, xmm2                                ; move for x4
    mulsd   xmm1, xmm2                                ; x4
    movsd   QWORD PTR p_temp1[rsp], xmm0              ; store x
    mulsd   xmm5, xmm2                                ; x2s3
    movsd   xmm4, xmm0                                ; move for x3
    addsd   xmm3, QWORD PTR Lsinarray+40h             ; s5+x2s6
    mulsd   xmm1, xmm2                                ; x6
    mulsd   xmm3, xmm2                                ; x2(s5+x2s6)
    mulsd   xmm4, xmm2                                ; x3
    addsd   xmm5, QWORD PTR Lsinarray+10h             ; s2+x2s3
    mulsd   xmm5, xmm2                                ; x2(s2+x2s3)
    addsd   xmm3, QWORD PTR Lsinarray+30h             ; s4 + x2(s5+x2s6)
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000  ; 0.5 *x2
    movsd   xmm0, QWORD PTR p_temp[rsp]               ; load xx
    mulsd   xmm3, xmm1                                ; x6(s4 + x2(s5+x2s6))
    addsd   xmm5, QWORD PTR Lsinarray                 ; s1+x2(s2+x2s3)
    mulsd   xmm2, xmm0                                ; 0.5 * x2 *xx
    addsd   xmm3, xmm5                                ; zs
    mulsd   xmm4, xmm3                                ; *x3
    subsd   xmm4, xmm2                                ; x3*zs - 0.5 * x2 *xx
    addsd   xmm0, xmm4                                ; +xx
    addsd   xmm0, QWORD PTR p_temp1[rsp]              ; +x
    
    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lcospiby4:
    
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 0 or 2     - do a cos calculation
;  zc = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));
    mulsd   xmm4, xmm0                                ; x*xx
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000
    movsd   xmm1, QWORD PTR Lcosarray+50h             ; c6
    movsd   xmm0, QWORD PTR Lcosarray+20h             ; c3
    mulsd   xmm5, xmm2                                ; r = 0.5 *x2
    movsd   xmm3, xmm2                                ; copy of x2
    movsd   QWORD PTR p_temp[rsp], xmm4               ; store x*xx
    mulsd   xmm1, xmm2                                ; c6*x2
    mulsd   xmm0, xmm2                                ; c3*x2
    subsd   xmm5, QWORD PTR L__real_3ff0000000000000  ; -t=r-1.0    ;trash r
    mulsd   xmm3, xmm2                                ; x4
    addsd   xmm1, QWORD PTR Lcosarray+40h             ; c5+x2c6
    addsd   xmm0, QWORD PTR Lcosarray+10h             ; c2+x2C3
    addsd   xmm5, QWORD PTR L__real_3ff0000000000000  ; 1 + (-t)    ;trash t
    mulsd   xmm3, xmm2                                ; x6
    mulsd   xmm1, xmm2                                ; x2(c5+x2c6)
    mulsd   xmm0, xmm2                                ; x2(c2+x2C3)
    movsd   xmm4, xmm2                                ; copy of x2
    mulsd   xmm4, QWORD PTR L__real_3fe0000000000000  ; r recalculate
    addsd   xmm1, QWORD PTR Lcosarray+30h             ; c4 + x2(c5+x2c6)
    addsd   xmm0, QWORD PTR Lcosarray                 ; c1+x2(c2+x2C3)
    mulsd   xmm2, xmm2                                ; x4 recalculate
    subsd   xmm5, xmm4                                ; (1 + (-t)) - r
    mulsd   xmm1, xmm3                                ; x6(c4 + x2(c5+x2c6))
    addsd   xmm0, xmm1                                ; zc
    subsd   xmm4, QWORD PTR L__real_3ff0000000000000  ; t relaculate
    subsd   xmm5, QWORD PTR p_temp[rsp]               ; ((1 + (-t)) - r) - x*xx
    mulsd   xmm0, xmm2                                ; x4 * zc
    addsd   xmm0, xmm5                                ; x4 * zc + ((1 + (-t)) - r -x*xx)
    subsd   xmm0, xmm4                                ; result - (-t)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ALIGN 16
Ladjust_region:        ; positive or negative (0, 1, 2, 3)=>(1, 2, 3 , 4)=>(0, 2, 2, 0)
;      switch (region)
    add     eax, 1
    and     eax, 2
    jz      Lcos_cleanup
    
;; if the original region 1 or 2 then we negate the result.
    movsd   xmm2, xmm0
    xorpd   xmm0, xmm0
    subsd   xmm0, xmm2

ALIGN 16
Lcos_cleanup:
    add     rsp, stack_size
    ret

ALIGN 16
Lcos_naninf:
    call    fname_special
    add     rsp, stack_size
    ret

fname		endp
TEXT	ENDS
END

