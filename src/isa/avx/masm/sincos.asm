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
; An implementation of the sincos function.
;
; Prototype:
;
;   void sincos(double x, double* sinr, double* cosr);
;
;   Computes sincos
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the NAG C implementation.
;
;

CONST    SEGMENT
ALIGN 16
__real_7fffffffffffffff DQ 07fffffffffffffffh  ;Sign bit zero
                        DQ 0                       ; for alignment
__real_3ff0000000000000 DQ 03ff0000000000000h  ; 1.0
                        DQ 0                          
__real_3fe0000000000000 DQ 03fe0000000000000h  ; 0.5
                        DQ 0
__real_3fc5555555555555 DQ 03fc5555555555555h  ; 0.166666666666
                        DQ 0
__real_3fe45f306dc9c883 DQ 03fe45f306dc9c883h  ; twobypi
                        DQ 0
__real_3ff921fb54400000 DQ 03ff921fb54400000h  ; piby2_1
                        DQ 0
__real_3dd0b4611a626331 DQ 03dd0b4611a626331h  ; piby2_1tail
                        DQ 0
__real_3dd0b4611a600000 DQ 03dd0b4611a600000h  ; piby2_2
                        DQ 0
__real_3ba3198a2e037073 DQ 03ba3198a2e037073h  ; piby2_2tail
                        DQ 0                
__real_fffffffff8000000 DQ 0fffffffff8000000h  ; mask for stripping head and tail
                        DQ 0                
__real_411E848000000000 DQ 0415312d000000000h   ; 5e6 0411E848000000000h  ; 5e5
                        DQ 0

ALIGN 16
Lsincosarray:
    DQ    0bfc5555555555555h                      ; -0.16666666666666666    s1
    DQ    03fa5555555555555h                      ; 0.041666666666666664    c1   
    DQ    03f81111111110bb3h                      ; 0.00833333333333095     s2
    DQ    0bf56c16c16c16967h                      ; -0.0013888888888887398  c2
    DQ    0bf2a01a019e83e5ch                      ; -0.00019841269836761127 s3
    DQ    03efa01a019f4ec90h                      ; 2.4801587298767041E-05  c3
    DQ    03ec71de3796cde01h                      ; 2.7557316103728802E-06  s4
    DQ    0be927e4fa17f65f6h                      ; -2.7557317272344188E-07 c4
    DQ    0be5ae600b42fdfa7h                      ; -2.5051132068021698E-08 s5
    DQ    03e21eeb69037ab78h                      ; 2.0876146382232963E-09  c6
    DQ    03de5e0b2f9a43bb8h                      ; 1.5918144304485914E-10  s6
    DQ    0bda907db46cc5e42h                      ; -1.1382639806794487E-11 c7

ALIGN 16
Lcossinarray:
    DQ    03fa5555555555555h                      ; 0.0416667        c1
    DQ    0bfc5555555555555h                      ; -0.166667        s1
    DQ    0bf56c16c16c16967h
    DQ    03f81111111110bb3h                      ; 0.00833333       s2
    DQ    03efa01a019f4ec90h
    DQ    0bf2a01a019e83e5ch                      ; -0.000198413     s3
    DQ    0be927e4fa17f65f6h
    DQ    03ec71de3796cde01h                      ; 2.75573e-006     s4
    DQ    03e21eeb69037ab78h
    DQ    0be5ae600b42fdfa7h                      ; -2.50511e-008    s5
    DQ    0bda907db46cc5e42h
    DQ    03de5e0b2f9a43bb8h                      ; 1.59181e-010     s6
CONST    ENDS
    
; define local variable storage offsets
p_temp       equ   030h                              ; temporary for get/put bits operation
p_temp1      equ   040h                              ; temporary for get/put bits operation
r            equ   050h                              ; pointer to r for amd_remainder_piby2
rr           equ   060h                              ; pointer to rr for amd_remainder_piby2
region       equ   070h                              ; pointer to region for amd_remainder_piby2
stack_size   equ   098h

include fm.inc
FN_PROTOTYPE sincos

fname_special   TEXTEQU <_sincos_special>

;Define name and any external functions being called
EXTRN        __amd_remainder_piby2 : PROC        ; NEAR
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
    mov     rcx, QWORD PTR p_temp[rsp]                         ; rcx is ux

;;  if NaN or inf
    mov     rax, 07ff0000000000000h
    mov     r10, rax
    and     r10, rcx
    cmp     r10, rax
    jz      Lsincos_naninf

;  ax = (ux & ~SIGNBIT_DP64);
    mov     r10, 07fffffffffffffffh
    and     r10, rcx                ; r10 is ax

;;  if (ax <= 3fe921fb54442d18h) /* abs(x) <= pi/4 */
    mov     rax, 03fe921fb54442d18h
    cmp     r10, rax
    jg      Lsincos_reduce

;;      if (ax < 3f20000000000000h) /* abs(x) < 2.0^(-13) */
    mov     rax, 03f20000000000000h
    cmp     r10, rax
    jge     Lsincos_small

;;          if (ax < 3e40000000000000h) /* abs(x) < 2.0^(-27) */
    mov     rax, 03e40000000000000h
    cmp     r10, rax
    jge     Lsincos_smaller

                            ; sin = x;
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; cos = 1.0;
    jmp     Lsincos_cleanup   

;;          else
ALIGN 16
Lsincos_smaller:
;              sin = x - x^3 * 0.1666666666666666666;
;              cos = 1.0 - x*x*0.5;

    movsd   xmm2, xmm0
    movsd   xmm4, QWORD PTR __real_3fc5555555555555     ; 0.1666666666666666666
    mulsd   xmm2, xmm2                               ; x^2
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; 1.0
    movsd   xmm3, xmm2                               ; copy of x^2

    mulsd   xmm2, xmm0                               ; x^3
    mulsd   xmm3, QWORD PTR __real_3fe0000000000000     ; 0.5 * x^2
    mulsd   xmm2, xmm4                               ; x^3 * 0.1666666666666666666
    subsd   xmm0, xmm2                               ; x - x^3 * 0.1666666666666666666, sin
    subsd   xmm1, xmm3                               ; 1 - 0.5 * x^2, cos

    jmp     Lsincos_cleanup                


;;      else

ALIGN 16
Lsincos_small:
;          sin = sin_piby4(x, 0.0);
    movsd   xmm5, QWORD PTR __real_3fe0000000000000     ; .5

;  x2 = r * r;
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                               ; x2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 0 or 2     - do a sin calculation
;  zs = (s2 + x2 * (s3 + x2 * (s4 + x2 * (s5 + x2 * s6))));

    movlhps xmm2, xmm2
    movapd  xmm3, XMMWORD PTR Lsincosarray+50h            ; s6
    movapd  xmm1, xmm2                               ; move for x4
    movdqa  xmm5, XMMWORD PTR Lsincosarray+20h            ; s3
    mulpd   xmm3, xmm2                               ; x2s6
    addpd   xmm3, XMMWORD PTR Lsincosarray+40h            ; s5+x2s6
    mulpd   xmm5, xmm2                               ; x2s3
    movapd  XMMWORD PTR p_temp[rsp], xmm4                        ; rr move to to memory
    mulpd   xmm1, xmm2                               ; x4
    mulpd   xmm3, xmm2                               ; x2(s5+x2s6)
    addpd   xmm5, XMMWORD PTR Lsincosarray+10h            ; s2+x2s3
    movapd  xmm4, xmm1                               ; move for x6
    addpd   xmm3, XMMWORD PTR Lsincosarray+30h            ; s4 + x2(s5+x2s6)
    mulpd   xmm5, xmm2                               ; x2(s2+x2s3)
    mulpd   xmm4, xmm2                               ; x6
    addpd   xmm5, XMMWORD PTR Lsincosarray                 ; s1+x2(s2+x2s3)
    mulpd   xmm3, xmm4                               ; x6(s4 + x2(s5+x2s6))

    movsd   xmm4, xmm2                               ; xmm4 = x2 for 0.5x2 for cos
                                                      ; xmm2 contains x2 for x3 for sin
    addpd   xmm3, xmm5                               ; zs in lower and zc upper

    mulsd   xmm2, xmm0                               ; xmm2=x3 for sin

    movhlps xmm5, xmm3                               ; Copy z, xmm5 = cos , xmm3 = sin

    mulsd   xmm4, QWORD PTR __real_3fe0000000000000     ; xmm4=r=0.5*x2 for cos term
    mulsd   xmm3, xmm2                               ; sin *x3
    mulsd   xmm5, xmm1                               ; cos *x4
    movsd   xmm2, QWORD PTR __real_3ff0000000000000     ; 1.0
    subsd   xmm2, xmm4                               ; t=1.0-r
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; 1.0
    subsd   xmm1, xmm2                               ; 1 - t
    subsd   xmm1, xmm4                               ; (1-t) -r
    addsd   xmm1, xmm5                               ; ((1-t) -r) + cos
    addsd   xmm0, xmm3                               ; xmm0= sin+x, final sin term
    addsd   xmm1, xmm2                               ; xmm1 = t +{ ((1-t) -r) + cos}, final cos term

    jmp     Lsincos_cleanup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincos_reduce:
; change rdx to rcx and r8 to r9
; rcx= ux, r10 = ax
; r9,rax are free

;  xneg = (ax != ux);
    cmp     rcx, r10
    mov     r11d, 0

;;  if (xneg) x = -x;
    jz      LPositive
    mov     r11d, 1
    subsd   xmm2, xmm0
    movsd   xmm0, xmm2

; rcx= ux, r10 = ax, r11= Sign
; r9,rax are free
; change rdx to rcx and r8 to r9

ALIGN 16
LPositive:
;;  if (x < 5.0e5)
    cmp     r10, QWORD PTR __real_411E848000000000
    jae     Lsincos_reduce_precise

; reduce  the argument to be in a range from -pi/4 to +pi/4
; by subtracting multiples of pi/2
    movsd   xmm2, xmm0
    movsd   xmm3, QWORD PTR __real_3fe45f306dc9c883     ; twobypi
    movsd   xmm4, xmm0
    movsd   xmm5, QWORD PTR __real_3fe0000000000000     ; .5
    mulsd   xmm2, xmm3

;/* How many pi/2 is x a multiple of? */
;       xexp  = ax >> EXPSHIFTBITS_DP64;
    shr     r10, 52                                  ; >>EXPSHIFTBITS_DP64

;       npi2  = (int)(x * twobypi + 0.5);
    addsd   xmm2, xmm5                               ; npi2

    movsd   xmm3, QWORD PTR __real_3ff921fb54400000     ; piby2_1
    cvttpd2dq    xmm0, xmm2                          ; convert to integer
    movsd   xmm1, QWORD PTR __real_3dd0b4611a626331     ; piby2_1tail
    cvtdq2pd    xmm2, xmm0                           ; and back to float.

;       /* Subtract the multiple from x to get an extra-precision remainder */
;       rhead  = x - npi2 * piby2_1;
    mulsd   xmm3, xmm2
    subsd   xmm4, xmm3                               ; rhead

;       rtail  = npi2 * piby2_1tail;
    mulsd   xmm1, xmm2
    movd    eax, xmm0


;       GET_BITS_DP64(rhead-rtail, uy);                   ; originally only rhead
    movsd   xmm0, xmm4
    subsd   xmm0, xmm1

    movsd   xmm3, QWORD PTR __real_3dd0b4611a600000     ; piby2_2
    movsd   QWORD PTR p_temp[rsp], xmm0
    movsd   xmm5, QWORD PTR __real_3ba3198a2e037073     ; piby2_2tail
    mov     ecx, eax
    mov     r9, QWORD PTR p_temp[rsp]                          ; rcx is rhead-rtail

;    xmm0=r, xmm4=rhead, xmm1=rtail, xmm2=npi2, xmm3=temp for calc, xmm5= temp for calc
;       expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     r9, 1                                    ; strip any sign bit
    shr     r9, 53                                   ; >> EXPSHIFTBITS_DP64 +1
    sub     r10, r9                                  ; expdiff

;;       if (expdiff > 15)
    cmp     r10, 15
    jle     Lexpdiff15

;          /* The remainder is pretty small compared with x, which
;             implies that x is a near multiple of pi/2
;             (x matches the multiple to at least 15 bits) */

;          t  = rhead;
    movsd   xmm1, xmm4

;          rtail  = npi2 * piby2_2;
    mulsd   xmm3, xmm2

;          rhead  = t - rtail;
    mulsd   xmm5, xmm2                               ; npi2 * piby2_2tail
    subsd   xmm4, xmm3                               ; rhead

;          rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
    subsd   xmm1, xmm4                               ; t - rhead
    subsd   xmm1, xmm3                               ; -rtail
    subsd   xmm5, xmm1                               ; rtail

;      r = rhead - rtail;
    movsd   xmm0, xmm4

;HARSHA
;xmm1=rtail
    movsd   xmm1, xmm5
    subsd   xmm0, xmm5
    
;    xmm0=r, xmm4=rhead, xmm1=rtail
Lexpdiff15:
;      region = npi2 & 3;

    subsd   xmm4, xmm0                               ; rhead-r
    subsd   xmm4, xmm1                               ; rr = (rhead-r) - rtail

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; if the input was close to a pi/2 multiple
; The original NAG code missed this trick.  If the input is very close to n*pi/2 after
; reduction,
; then the sin is ~ 1.0 , to within 53 bits, when r is < 2^-27.  We already
; have x at this point, so we can skip the sin polynomials.

    cmp     r9, 03f2h                               ; if r  small.
    jge     Lcossin_piby4                            ; use taylor series if not
    cmp     r9, 03deh                               ; if r really small.
    jle     Lr_small                                 ; then sin(r) = 0

    movsd   xmm2, xmm0
    mulsd   xmm2, xmm2                               ; x^2

;;      if region is 0 or 2 do a sin calc.
    and     ecx, 1
    jnz     Lregion13

; region 0 or 2 do a sincos calculation
; use simply polynomial
;              sin=x - x*x*x*0.166666666666666666;
    movsd   xmm3, QWORD PTR __real_3fc5555555555555     ; 0.166666666
    mulsd   xmm3, xmm0                               ; * x
    mulsd   xmm3, xmm2                               ; * x^2
    subsd   xmm0, xmm3                               ; xs
;              cos=1.0 - x*x*0.5;
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; 1.0
    mulsd   xmm2, QWORD PTR __real_3fe0000000000000     ; 0.5 *x^2
    subsd   xmm1, xmm2                               ; xc

    jmp     Ladjust_region

ALIGN 16
Lregion13:
; region 1 or 3 do a cossin calculation
; use simply polynomial
;              sin=x - x*x*x*0.166666666666666666;
    movsd   xmm1, xmm0

    movsd   xmm3, QWORD PTR __real_3fc5555555555555     ; 0.166666666
    mulsd   xmm3, xmm0                               ; 0.166666666* x
    mulsd   xmm3, xmm2                               ; 0.166666666* x * x^2
    subsd   xmm1, xmm3                               ; xs
;              cos=1.0 - x*x*0.5;
    movsd   xmm0, QWORD PTR __real_3ff0000000000000     ; 1.0
    mulsd   xmm2, QWORD PTR __real_3fe0000000000000     ; 0.5 *x^2
    subsd   xmm0, xmm2                               ; xc

    jmp     Ladjust_region

ALIGN 16
Lr_small:
;;      if region is 0 or 2    do a sincos calc.
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; cos(r) is a 1
    and     ecx, 1
    jz      Ladjust_region

;;      if region is 1 or 3    do a cossin calc.
    movsd   xmm1, xmm0                               ; sin(r) is r
    movsd   xmm0, QWORD PTR __real_3ff0000000000000     ; cos(r) is a 1
    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincos_reduce_precise:
;      // Reduce x into range [-pi/4,pi/4]
;      __amd_remainder_piby2(x, &r, &rr, &region);
	mov     QWORD PTR p_temp1[rsp], r8
	mov     QWORD PTR p_temp1+8[rsp], rdx

    mov     QWORD PTR [p_temp+rsp], r11
	lea	    r9, QWORD PTR region[rsp]
	lea     r8, QWORD PTR rr[rsp]
	lea     rdx, QWORD PTR r[rsp]

;change to MS ABI - shadow space
    sub     rsp, 020h    
    
    call    __amd_remainder_piby2

;change to MS ABI - shadow space
    add     rsp,020h
	mov     r8, QWORD PTR p_temp1[rsp]
	mov     rdx, QWORD PTR p_temp1+8[rsp]

	mov	    r11, QWORD PTR p_temp[rsp]
	movsd	xmm0, QWORD PTR r[rsp]		             ; x
	movsd	xmm4, QWORD PTR rr[rsp]		             ; xx
	mov	    eax, DWORD PTR region[rsp]	             ; region to classify for sin/cos calc
    mov     ecx, eax                                 ; region to get sign
    
; xmm0 = x, xmm4 = xx, r8d = 1, eax= region
ALIGN 16
Lcossin_piby4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; perform taylor series to calc sinx, sinx
;  x2 = r * r;
;xmm4 = a part of rr for the sin path, xmm4 is overwritten in the sin path
;instead use xmm3 because that was freed up in the sin path, xmm3 is overwritten in sin path

    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                               ;x2

;;      if region is 0 or 2    do a sincos calc.
    and     ecx, 1
    jz      Lsincos02

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; region 1 or 3     - do a cossin calculation
;  zc = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));


    movlhps xmm2, xmm2

    movapd  xmm3, XMMWORD PTR Lcossinarray+50h            ; s6
    movapd  xmm1, xmm2                               ; move for x4
    movdqa  xmm5, XMMWORD PTR Lcossinarray+20h            ; s3
    mulpd   xmm3, xmm2                               ; x2s6
    addpd   xmm3, XMMWORD PTR Lcossinarray+40h            ; s5+x2s6
    mulpd   xmm5, xmm2                               ; x2s3
    movsd   QWORD PTR p_temp[rsp], xmm4                        ; rr move to to memory
    mulpd   xmm1, xmm2                               ; x4
    mulpd   xmm3, xmm2                               ; x2(s5+x2s6)
    addpd   xmm5, XMMWORD PTR Lcossinarray+10h            ; s2+x2s3
    movapd  xmm4, xmm1                               ; move for x6
    addpd   xmm3, XMMWORD PTR Lcossinarray+30h            ; s4 + x2(s5+x2s6)
    mulpd   xmm5, xmm2                               ; x2(s2+x2s3)
    mulpd   xmm4, xmm2                               ; x6
    addpd   xmm5, XMMWORD PTR Lcossinarray                 ; s1+x2(s2+x2s3)
    mulpd   xmm3, xmm4                               ; x6(s4 + x2(s5+x2s6))

    movsd   xmm4, xmm2                               ; xmm4 = x2 for 0.5x2 cos
                                                      ; xmm2 contains x2 for x3 sin

    addpd   xmm3, xmm5                               ; zc in lower and zs in upper

    mulsd   xmm2, xmm0                               ; xmm2=x3 for the sin term

    movhlps xmm5, xmm3                               ; Copy z, xmm5 = sin, xmm3 = cos
    mulsd   xmm4, QWORD PTR __real_3fe0000000000000     ; xmm4=r=0.5*x2 for cos term

    mulsd   xmm5, xmm2                               ; sin *x3
    mulsd   xmm3, xmm1                               ; cos *x4
    movsd   QWORD PTR p_temp1[rsp], xmm0                       ; store x
    movsd   xmm1, xmm0

    movsd   xmm2, QWORD PTR __real_3ff0000000000000     ; 1.0
    subsd   xmm2, xmm4                               ; t=1.0-r

    movsd   xmm0, QWORD PTR __real_3ff0000000000000     ; 1.0
    subsd   xmm0, xmm2                               ; 1 - t

    mulsd   xmm1, QWORD PTR p_temp[rsp]                       ; x*xx
    subsd   xmm0, xmm4                               ; (1-t) -r
    subsd   xmm0, xmm1                               ; ((1-t) -r) - x *xx

    mulsd   xmm4, QWORD PTR p_temp[rsp]                        ; 0.5*x2*xx

    addsd   xmm0, xmm3                               ; (((1-t) -r) - x *xx) + cos

    subsd   xmm5, xmm4                               ; sin - 0.5*x2*xx

    addsd   xmm0, xmm2                               ; xmm0 = t +{ (((1-t) -r) - x *xx) + cos}, final cos term

    addsd   xmm5, QWORD PTR p_temp[rsp]                        ; sin + xx
    movsd   xmm1, QWORD PTR p_temp1[rsp]                       ; load x
    addsd   xmm1, xmm5                               ; xmm1= sin+x, final sin term

    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincos02:
; region 0 or 2 do a sincos calculation
    movlhps xmm2,xmm2

    movapd  xmm3, XMMWORD PTR Lsincosarray+50h            ; s6
    movapd  xmm1, xmm2                               ; move for x4
    movdqa  xmm5, XMMWORD PTR Lsincosarray+20h            ; s3
    mulpd   xmm3, xmm2                               ; x2s6
    addpd   xmm3, XMMWORD PTR Lsincosarray+40h            ; s5+x2s6
    mulpd   xmm5, xmm2                               ; x2s3
    movsd   QWORD PTR p_temp[rsp], xmm4                        ; rr move to to memory
    mulpd   xmm1, xmm2                               ; x4
    mulpd   xmm3, xmm2                               ; x2(s5+x2s6)
    addpd   xmm5, XMMWORD PTR Lsincosarray+10h            ; s2+x2s3
    movapd  xmm4, xmm1                               ; move for x6
    addpd   xmm3, XMMWORD PTR Lsincosarray+30h            ; s4 + x2(s5+x2s6)
    mulpd   xmm5, xmm2                               ; x2(s2+x2s3)
    mulpd   xmm4, xmm2                               ; x6
    addpd   xmm5, XMMWORD PTR Lsincosarray                 ; s1+x2(s2+x2s3)
    mulpd   xmm3, xmm4                               ; x6(s4 + x2(s5+x2s6))

    movsd   xmm4, xmm2                               ; xmm4 = x2 for 0.5x2 for cos
                                                      ; xmm2 contains x2 for x3 for sin

    addpd   xmm3, xmm5                               ; zs in lower and zc in upper

    mulsd   xmm2, xmm0                               ; xmm2=x3 for sin

    movhlps xmm5, xmm3                               ; Copy z, xmm5 = cos , xmm3 = sin

    mulsd   xmm4, QWORD PTR __real_3fe0000000000000     ; xmm4=r=0.5*x2 for cos term

    mulsd   xmm3, xmm2                               ; sin *x3
    mulsd   xmm5, xmm1                               ; cos *x4

    movsd   xmm2, QWORD PTR __real_3ff0000000000000     ; 1.0
    subsd   xmm2, xmm4                               ; t=1.0-r

    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; 1.0
    subsd   xmm1, xmm2                               ; 1 - t

    movsd   QWORD PTR p_temp1[rsp], xmm0                       ; store x
    mulsd   xmm0, QWORD PTR p_temp[rsp]                       ; x*xx

    subsd   xmm1, xmm4                               ; (1-t) -r
    subsd   xmm1, xmm0                               ; ((1-t) -r) - x *xx

    mulsd   xmm4, QWORD PTR p_temp[rsp]                       ; 0.5*x2*xx

    addsd   xmm1, xmm5                               ; (((1-t) -r) - x *xx) + cos

    subsd   xmm3, xmm4                               ; sin - 0.5*x2*xx

    addsd   xmm1, xmm2                               ; xmm1 = t +{ (((1-t) -r) - x *xx) + cos}, final cos term

    addsd   xmm3, QWORD PTR p_temp[rsp]                        ; sin + xx
    movsd   xmm0, QWORD PTR p_temp1[rsp]                       ; load x
    addsd   xmm0, xmm3                               ; xmm0= sin+x, final sin term

    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;      switch (region)
ALIGN 16
Ladjust_region:        ; positive or negative for sin return val in xmm0

    mov     r9d, eax

    shr     eax, 1
    mov     ecx, eax
    and     eax, r11d

    not     ecx
    not     r11d
    and     ecx, r11d

    or      eax, ecx
    and     eax, 1
    jnz     Lcos_sign

;; if the original region 0, 1 and arg is negative, then we negate the result.
;; if the original region 2, 3 and arg is positive, then we negate the result.
    movsd   xmm2, xmm0
    xorpd   xmm0, xmm0
    subsd   xmm0, xmm2

Lcos_sign:            ; positive or negative for cos return val in xmm1
    add     r9, 1
    and     r9d, 2
    jz      Lsincos_cleanup
;; if the original region 1 or 2 then we negate the result.
    movsd   xmm2, xmm1
    xorpd   xmm1, xmm1
    subsd   xmm1, xmm2

;ALIGN 16
Lsincos_cleanup:
    movsd   QWORD PTR [rdx], xmm0                    ; save the sin
    movsd   QWORD PTR [r8], xmm1                     ; save the cos

    add     rsp, stack_size
    ret

ALIGN 16
Lsincos_naninf:
   call     fname_special
   add      rsp, stack_size
   ret

fname       endp
TEXT    ENDS
END

