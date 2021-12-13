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
; An implementation of the complex exp function.
;
; Prototype:
;
;   double complex cexp(double complex x);
;
;   Computes cexp
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the sincos and exp implementation.
;
;
    
; define local variable storage offsets
p_temp0      equ    30h                              ; temporary for get/put bits operation
p_temp1      equ    40h                              ; temporary for get/put bits operation
p_temp2      equ    50h                              ; temporary for get/put bits operation 
r            equ    60h                              ; pointer to r for amd_remainder_piby2
rr           equ    70h                              ; pointer to rr for amd_remainder_piby2
region       equ    80h                              ; pointer to region for amd_remainder_piby2
save_rax     equ    90h                              ; for windows calling convention
stack_size   equ    0b8h

include fm.inc
FN_PROTOTYPE cexp


fname_special1  TEXTEQU <_sincos_special>
fname_special2  TEXTEQU <_exp_special>

;Define name and any external functions being called
EXTRN        __amd_remainder_piby2 : PROC   ; NEAR
EXTERN       fname_special1      : PROC
EXTERN       fname_special2      : PROC

StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
StackAllocate stack_size
    .ENDPROLOG   

; For Windows calling convention
	mov         QWORD PTR [save_rax+rsp], rcx
    movsd       xmm0, QWORD PTR [rdx]
    movsd       xmm1, QWORD PTR [rdx+8]

;start the exp calculation
Lexp:
    movsd       QWORD PTR [p_temp2+rsp], xmm0                  ; xmm0 for real part
    movsd       QWORD PTR [p_temp0+rsp], xmm1                  ; save image part for sincos usage
    mov         rax, QWORD PTR L__real_inf
    mov         rdx, QWORD PTR [p_temp2+rsp]
    and         rax, rdx
    cmp         rax, QWORD PTR L__real_inf
    je          L__x_is_inf_or_nan
    movsd       xmm3, QWORD PTR L__real_64_by_log2

    ; x <= 1024*ln(2)
    ; x > -1075*ln(2)
    comisd      xmm0, QWORD PTR L__real_pthreshold
    ja          L__y_is_inf

    comisd      xmm0, QWORD PTR L__real_mthreshold
    jbe         L__y_is_zero

    ; x * (64/ln(2))
    mulsd       xmm3, xmm0

    ; n = int( x * (64/ln(2)) )
    cvtpd2dq    xmm4, xmm3
    lea         r10, QWORD PTR L__two_to_jby64_head_table
    lea         r11, QWORD PTR L__two_to_jby64_tail_table
    cvtdq2pd    xmm1, xmm4

    ; r1 = x - n * ln(2)/64 head
    movsd       xmm2, QWORD PTR L__real_log2_by_64_head
    mulsd       xmm2, xmm1
    movd        ecx, xmm4
    mov         rax, 3fh
    and         eax, ecx
    subsd       xmm0, xmm2

    ; r2 = - n * ln(2)/64 tail
    mulsd       xmm1, QWORD PTR L__real_log2_by_64_tail
    movsd       xmm2, xmm0

    ; m = (n - j) / 64
    sub         ecx, eax
    sar         ecx, 6

    ; r1+r2
    addsd       xmm2, xmm1
    movsd       xmm1, xmm2

    ; q
    movsd       xmm3, QWORD PTR L__real_1_by_720
    movsd       xmm0, QWORD PTR L__real_1_by_6
    mulsd       xmm3, xmm2
    mulsd       xmm0, xmm2
    mulsd       xmm1, xmm2 
    movsd       xmm4, xmm1
    addsd       xmm3, QWORD PTR L__real_1_by_120
    addsd       xmm0, QWORD PTR L__real_1_by_2
    mulsd       xmm4, xmm1
    mulsd       xmm3, xmm2
    mulsd       xmm0, xmm1
    addsd       xmm3, QWORD PTR L__real_1_by_24
    addsd       xmm0, xmm2
    mulsd       xmm3, xmm4
    addsd       xmm0, xmm3

    ; deal with denormal results
    xor         r9d, r9d
    cmp         ecx, DWORD PTR L__denormal_threshold
    cmovle      r9d, ecx

    add         rcx, 1023
    shl         rcx, 52

    lea         r8, QWORD PTR L__two_to_jby64_table
    mulsd       xmm0, QWORD PTR [r8+rax*8]

    cmp         rcx, QWORD PTR L__real_inf

    ; (f1+f2)*(1+q)
    addsd       xmm0, QWORD PTR [r11+rax*8]
    addsd       xmm0, QWORD PTR [r10+rax*8]

    je          L__process_almost_inf

    test        r9d, r9d
    mov         QWORD PTR [p_temp2+rsp], rcx
    jnz         L__process_denormal
    mulsd       xmm0, QWORD PTR [p_temp2+rsp]
    jmp         L__SinCos

ALIGN 16
L__process_almost_inf:
    ; here e^r < 1 and m = 128, we cannot use mulsd
    orpd        xmm0, QWORD PTR L__enable_almost_inf
    jmp         L__SinCos

ALIGN 16
L__process_denormal:
    mov         ecx, r9d
    xor         r11d, r11d
    comisd      xmm0, QWORD PTR L__real_one
    cmovae      r11d, ecx
    cmp         r11d, DWORD PTR L__denormal_threshold
    jne         L__process_true_denormal  

    ; here e^r >= 1 m = -1022, result is normal
    mulsd       xmm0, QWORD PTR [p_temp2+rsp]
    jmp         L__SinCos

ALIGN 16
L__process_true_denormal:
    ; here ( e^r < 1 and m = -1022 ) or m <= -1023
    xor         r8, r8
    cmp         rdx, QWORD PTR L__denormal_tiny_threshold
    mov         r9, 1
    jg          L__process_denormal_tiny
    add         ecx, 1074
    cmovs       rcx, r8
    shl         r9, cl
    mov         rcx, r9

    mov         QWORD PTR [p_temp2+rsp], rcx
    mulsd       xmm0, QWORD PTR [p_temp2+rsp]
    jmp         L__SinCos        

ALIGN 16
L__process_denormal_tiny:
    movsd       xmm0, QWORD PTR L__real_smallest_denormal
    jmp         L__SinCos

ALIGN 16
L__y_is_zero:

    movsd       xmm1, QWORD PTR L__real_zero
    movd        xmm0, rdx
    mov         r8d, DWORD PTR L__flag_y_zero

    call        fname_special2
    movsd       xmm0, QWORD PTR L__real_zero    
    jmp         L__SinCos         

ALIGN 16
L__y_is_inf:

    movsd       xmm1, QWORD PTR L__real_inf
    movd        xmm0, rdx
    mov         r8d, DWORD PTR L__flag_y_inf

    call        fname_special2
    jmp         L__SinCos      

ALIGN 16
L__x_is_inf_or_nan:

    cmp         rdx, QWORD PTR L__real_inf
    je          L__SinCos

    cmp         rdx, QWORD PTR L__real_ninf
    movsd       xmm0, QWORD PTR L__real_zero    
    je          L__SinCos

    mov         r9, QWORD PTR L__real_qnanbit
    and         r9, rdx
    movsd       xmm0, QWORD PTR L__real_7ff8000000000000     
    jnz         L__SinCos

	movd        xmm0, rdx
    or          rdx, QWORD PTR L__real_qnanbit
    movd        xmm1, rdx
    mov         r8d, DWORD PTR L__flag_x_nan
    call        fname_special2
;    jmp         L__SinCos    

L__SinCos:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; start the sincos calculation
; GET_BITS_DP64(x, ux);
; get the input value to an integer register.
    movsd   QWORD PTR [p_temp2+rsp], xmm0                      ; save xmm0(exp(x)) to p_temp2
    movsd   xmm0, QWORD PTR [p_temp0+rsp]
    mov     rcx, QWORD PTR [p_temp0+rsp]                       ; rcx is ux

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
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     ; cos = 1.0;
    jmp     Lsincos_cleanup   

;;          else
ALIGN 16
Lsincos_smaller:
;              sin = x - x^3 * 0.1666666666666666666;
;              cos = 1.0 - x*x*0.5;

    movsd   xmm2, xmm0
    movsd   xmm4, QWORD PTR L__real_3fc5555555555555     ; 0.1666666666666666666
    mulsd   xmm2, xmm2                               ; x^2
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     ; 1.0
    movsd   xmm3, xmm2                               ; copy of x^2

    mulsd   xmm2, xmm0                               ; x^3
    mulsd   xmm3, QWORD PTR L__real_3fe0000000000000     ; 0.5 * x^2
    mulsd   xmm2, xmm4                               ; x^3 * 0.1666666666666666666
    subsd   xmm0, xmm2                               ; x - x^3 * 0.1666666666666666666, sin
    subsd   xmm1, xmm3                               ; 1 - 0.5 * x^2, cos

    jmp     Lsincos_cleanup                


;;      else

ALIGN 16
Lsincos_small:
;          sin = sin_piby4(x, 0.0);
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000     ; .5

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
    movapd  QWORD PTR [p_temp0+rsp], xmm4                       ; rr move to to memory
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

    mulsd   xmm4, QWORD PTR L__real_3fe0000000000000     ; xmm4=r=0.5*x2 for cos term
    mulsd   xmm3, xmm2                               ; sin *x3
    mulsd   xmm5, xmm1                               ; cos *x4
    movsd   xmm2, QWORD PTR L__real_3ff0000000000000     ; 1.0
    subsd   xmm2, xmm4                               ; t=1.0-r
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     ; 1.0
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
; r9, rax are free

;  xneg = (ax != ux);
    xorpd   xmm2, xmm2                               ; zeroed out for later use
    cmp     rcx, r10
    mov     r11d, 0

;;  if (xneg) x = -x;
    jz      LPositive
    mov     r11d, 1
    subsd   xmm2, xmm0
    movsd   xmm0, xmm2

; rcx= ux, r10 = ax, r11= Sign
; r9, rax are free
; change rdx to rcx and r8 to r9

ALIGN 16
LPositive:
;;  if (x < 5.0e5)
    cmp     r10, QWORD PTR L__real_411E848000000000
    jae     Lsincos_reduce_precise

; reduce  the argument to be in a range from -pi/4 to +pi/4
; by subtracting multiples of pi/2
    movsd   xmm2, xmm0
    movsd   xmm3, QWORD PTR L__real_3fe45f306dc9c883     ; twobypi
    movsd   xmm4, xmm0
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000     ; .5
    mulsd   xmm2, xmm3

;/* How many pi/2 is x a multiple of? */
;       xexp  = ax >> EXPSHIFTBITS_DP64;
    shr     r10, 52                                  ; >>EXPSHIFTBITS_DP64

;       npi2  = (int)(x * twobypi + 0.5);
    addsd   xmm2, xmm5                               ; npi2

    movsd   xmm3, QWORD PTR L__real_3ff921fb54400000     ; piby2_1
    cvttpd2dq    xmm0, xmm2                          ; convert to integer
    movsd   xmm1, QWORD PTR L__real_3dd0b4611a626331     ; piby2_1tail
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

    movsd   xmm3, QWORD PTR L__real_3dd0b4611a600000     ; piby2_2
    movsd   QWORD PTR [p_temp0+rsp], xmm0
    movsd   xmm5, QWORD PTR L__real_3ba3198a2e037073     ; piby2_2tail
    mov     ecx, eax
    mov     r9, QWORD PTR [p_temp0+rsp]                        ; rcx is rhead-rtail

;    xmm0=r,xmm4=rhead,xmm1=rtail,xmm2=npi2,xmm3=temp for calc, xmm5= temp for calc
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
    movsd   xmm3, QWORD PTR L__real_3fc5555555555555     ; 0.166666666
    mulsd   xmm3, xmm0                               ; * x
    mulsd   xmm3, xmm2                               ; * x^2
    subsd   xmm0, xmm3                               ; xs
;              cos=1.0 - x*x*0.5;
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000     ; 0.5 *x^2
    subsd   xmm1, xmm2                               ; xc

    jmp     Ladjust_region

ALIGN 16
Lregion13:
; region 1 or 3 do a cossin calculation
; use simply polynomial
;              sin=x - x*x*x*0.166666666666666666;
    movsd   xmm1, xmm0

    movsd   xmm3, QWORD PTR L__real_3fc5555555555555     ; 0.166666666
    mulsd   xmm3, xmm0                               ; 0.166666666* x
    mulsd   xmm3, xmm2                               ; 0.166666666* x * x^2
    subsd   xmm1, xmm3                               ; xs
;              cos=1.0 - x*x*0.5;
    movsd   xmm0, QWORD PTR L__real_3ff0000000000000     ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000     ; 0.5 *x^2
    subsd   xmm0, xmm2                               ; xc

    jmp     Ladjust_region

ALIGN 16
Lr_small:
;;      if region is 0 or 2    do a sincos calc.
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     ; cos(r) is a 1
    and     ecx, 1
    jz      Ladjust_region

;;      if region is 1 or 3    do a cossin calc.
    movsd   xmm1, xmm0                               ; sin(r) is r
    movsd   xmm0, L__real_3ff0000000000000     ; cos(r) is a 1
    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincos_reduce_precise:
;      // Reduce x into range [-pi/4,pi/4]
;      __amd_remainder_piby2(x, &r, &rr, &region);

	mov     QWORD PTR p_temp1[rsp], r8
	mov     QWORD PTR p_temp1+8[rsp], rdx

    mov     QWORD PTR [p_temp0+rsp], r11
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

	mov	    r11, QWORD PTR p_temp0[rsp]
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
    movsd   QWORD PTR [p_temp0+rsp], xmm4                       ; rr move to to memory
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
    mulsd   xmm4, QWORD PTR L__real_3fe0000000000000     ; xmm4=r=0.5*x2 for cos term

    mulsd   xmm5, xmm2                               ; sin *x3
    mulsd   xmm3, xmm1                               ; cos *x4
    movsd   QWORD PTR [p_temp1+rsp], xmm0                       ; store x
    movsd   xmm1, xmm0

    movsd   xmm2, QWORD PTR L__real_3ff0000000000000     ; 1.0
    subsd   xmm2, xmm4                               ; t=1.0-r

    movsd   xmm0, QWORD PTR L__real_3ff0000000000000     ; 1.0
    subsd   xmm0, xmm2                               ; 1 - t

    mulsd   xmm1, QWORD PTR [p_temp0+rsp]                       ; x*xx
    subsd   xmm0, xmm4                               ; (1-t) -r
    subsd   xmm0, xmm1                               ; ((1-t) -r) - x *xx

    mulsd   xmm4, QWORD PTR [p_temp0+rsp]                       ; 0.5*x2*xx

    addsd   xmm0, xmm3                               ; (((1-t) -r) - x *xx) + cos

    subsd   xmm5, xmm4                               ; sin - 0.5*x2*xx

    addsd   xmm0, xmm2                               ; xmm0 = t +{ (((1-t) -r) - x *xx) + cos}, final cos term

    addsd   xmm5, QWORD PTR [p_temp0+rsp]                       ; sin + xx
    movsd   xmm1, QWORD PTR [p_temp1+rsp]                       ; load x
    addsd   xmm1, xmm5                               ; xmm1= sin+x, final sin term

    jmp     Ladjust_region

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincos02:
; region 0 or 2 do a sincos calculation
    movlhps xmm2, xmm2

    movapd  xmm3, XMMWORD PTR Lsincosarray+50h            ; s6
    movapd  xmm1, xmm2                               ; move for x4
    movdqa  xmm5, XMMWORD PTR Lsincosarray+20h            ; s3
    mulpd   xmm3, xmm2                               ; x2s6
    addpd   xmm3, XMMWORD PTR Lsincosarray+40h            ; s5+x2s6
    mulpd   xmm5, xmm2                               ; x2s3
    movsd   QWORD PTR [p_temp0+rsp], xmm4                       ; rr move to to memory
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

    mulsd   xmm4, QWORD PTR L__real_3fe0000000000000     ; xmm4=r=0.5*x2 for cos term

    mulsd   xmm3, xmm2                               ; sin *x3
    mulsd   xmm5, xmm1                               ; cos *x4

    movsd   xmm2, QWORD PTR L__real_3ff0000000000000     ; 1.0
    subsd   xmm2, xmm4                               ; t=1.0-r

    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     ; 1.0
    subsd   xmm1, xmm2                               ; 1 - t

    movsd   QWORD PTR [p_temp1+rsp], xmm0                       ; store x
    mulsd   xmm0, QWORD PTR [p_temp0+rsp]                       ; x*xx

    subsd   xmm1, xmm4                               ; (1-t) -r
    subsd   xmm1, xmm0                               ; ((1-t) -r) - x *xx

    mulsd   xmm4, QWORD PTR [p_temp0+rsp]                       ; 0.5*x2*xx

    addsd   xmm1, xmm5                               ; (((1-t) -r) - x *xx) + cos

    subsd   xmm3, xmm4                               ; sin - 0.5*x2*xx

    addsd   xmm1, xmm2                               ; xmm1 = t +{ (((1-t) -r) - x *xx) + cos}, final cos term

    addsd   xmm3, QWORD PTR [p_temp0+rsp]                       ; sin + xx
    movsd   xmm0, QWORD PTR [p_temp1+rsp]                       ; load x
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
;normal result return
Lsincos_cleanup:
;   movsd   xmm0, QWORD PTR [p_temp0+rsp]                     ; store the sin
;   movsd   xmm1, QWORD PTR [p_temp1+rsp]                     ; store the cos
; we need cos for xmm0, sin for xmm1, swap
    movsd   xmm4, xmm0
    movsd   xmm0, xmm1
    movsd   xmm1, xmm4
    
    movsd   xmm3, QWORD PTR [p_temp2+rsp]                      ; recover exp(x)
    mulsd   xmm1, xmm3
    mulsd   xmm0, xmm3

; For Windows calling convention
	mov         rax, QWORD PTR [save_rax+rsp]
    movsd       QWORD PTR [rax], xmm0
    movsd       QWORD PTR [rax+8], xmm1

    add     rsp, stack_size
    ret

ALIGN 16
; exception handle for sincos
Lsincos_naninf:
    lea     rdx, QWORD PTR region[rsp]                ; set up temp pointer for special call
    lea     r8,  QWORD PTR r[rsp]

    call    fname_special1
    movsd	xmm0, QWORD PTR r[rsp]
  
    movsd   xmm3, QWORD PTR [p_temp2+rsp]                      ; recover exp(x)
    mov     rax, QWORD PTR [p_temp2+rsp]                 
    mov     rcx, 7fffffffffffffffh               
    and     rax, rcx
    jz      LZero_Nan
    
    mulsd   xmm0, xmm3    
    mov     rax, QWORD PTR [save_rax+rsp]
    movsd   QWORD PTR [rax], xmm0
    movsd   QWORD PTR [rax+8], xmm0       
    add     rsp, stack_size
    ret
    
LZero_Nan:
	xorpd   xmm0, xmm0
        
    mov     rax, QWORD PTR [save_rax+rsp]
    movsd   QWORD PTR [rax], xmm0
    movsd   QWORD PTR [rax+8], xmm0       
    add     rsp, stack_size
    ret

fname endp

text ENDS

data SEGMENT READ

; move data to the end
ALIGN 16
L__real_7fffffffffffffff DQ 07fffffffffffffffh  ;Sign bit zero
                        DQ 0                       ; for alignment
L__real_3ff0000000000000 DQ 03ff0000000000000h  ; 1.0
                        DQ 0                          
L__real_3fe0000000000000 DQ 03fe0000000000000h  ; 0.5
                        DQ 0
L__real_3fc5555555555555 DQ 03fc5555555555555h  ; 0.166666666666
                        DQ 0
L__real_3fe45f306dc9c883 DQ 03fe45f306dc9c883h  ; twobypi
                        DQ 0
L__real_3ff921fb54400000 DQ 03ff921fb54400000h  ; piby2_1
                        DQ 0
L__real_3dd0b4611a626331 DQ 03dd0b4611a626331h  ; piby2_1tail
                        DQ 0
L__real_3dd0b4611a600000 DQ 03dd0b4611a600000h  ; piby2_2
                        DQ 0
L__real_3ba3198a2e037073 DQ 03ba3198a2e037073h  ; piby2_2tail
                        DQ 0                
L__real_fffffffff8000000 DQ 0fffffffff8000000h  ; mask for stripping head and tail
                        DQ 0                
L__real_411E848000000000 DQ 0415312d000000000h  ; 411E848000000000  ; 5e5
                        DQ 0
L__real_7ff8000000000000 DQ 07ff8000000000000h  ; qnan
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

ALIGN 16
; these codes and the ones in the corresponding .c file have to match
L__flag_x_nan         DD 00000001
L__flag_y_zero        DD 00000002
L__flag_y_inf         DD 00000003

ALIGN 16
L__denormal_threshold           DD 0fffffc02h ; -1022
                                DD 0
                                DQ 0
L__enable_almost_inf            DQ 7fe0000000000000h
                                DQ 0
L__real_zero                    DQ 0000000000000000h
                                DQ 0
L__real_one                     DQ 3ff0000000000000h
                                DQ 0 
L__real_smallest_denormal       DQ 0000000000000001h
                                DQ 0
L__denormal_tiny_threshold      DQ 0c0874046dfefd9d0h
                                DQ 0
L__exp_mant_mask                DQ 7fffffffffffffffh
                                DQ 0
L__real_x_near0_threshold       DQ 3c00000000000000h
                                DQ 0
L__real_inf                     DQ 7ff0000000000000h    ; inf
                                DQ 0
L__real_ninf                    DQ 0fff0000000000000h   ; -inf
                                DQ 0
L__real_qnanbit                 DQ 0008000000000000h    ; qnan set bit
                                DQ 0
L__real_pthreshold              DQ 40862e42fefa39efh
                                DQ 0
L__real_mthreshold              DQ 0c0874910d52d3052h
                                DQ 0
L__real_64_by_log2              DQ 40571547652b82feh    ; 64/ln(2)
                                DQ 0
L__real_log2_by_64_head         DQ 3f862e42fefa0000h    ; log2_by_64_head
                                DQ 0
L__real_log2_by_64_tail         DQ 0bd1cf79abc9e3b39h   ; -log2_by_64_tail
                                DQ 0
L__real_1_by_720                DQ 3f56c16c16c16c17h    ; 1/720
                                DQ 0
L__real_1_by_120                DQ 3f81111111111111h    ; 1/120
                                DQ 0
L__real_1_by_24                 DQ 3fa5555555555555h    ; 1/24
                                DQ 0
L__real_1_by_6                  DQ 3fc5555555555555h    ; 1/6
                                DQ 0
L__real_1_by_2                  DQ 3fe0000000000000h    ; 1/2
                                DQ 0

ALIGN 16
L__two_to_jby64_table:
    DQ 3ff0000000000000h
    DQ 3ff02c9a3e778061h
    DQ 3ff059b0d3158574h
    DQ 3ff0874518759bc8h
    DQ 3ff0b5586cf9890fh
    DQ 3ff0e3ec32d3d1a2h
    DQ 3ff11301d0125b51h
    DQ 3ff1429aaea92de0h
    DQ 3ff172b83c7d517bh
    DQ 3ff1a35beb6fcb75h
    DQ 3ff1d4873168b9aah
    DQ 3ff2063b88628cd6h
    DQ 3ff2387a6e756238h
    DQ 3ff26b4565e27cddh
    DQ 3ff29e9df51fdee1h
    DQ 3ff2d285a6e4030bh
    DQ 3ff306fe0a31b715h
    DQ 3ff33c08b26416ffh
    DQ 3ff371a7373aa9cbh
    DQ 3ff3a7db34e59ff7h
    DQ 3ff3dea64c123422h
    DQ 3ff4160a21f72e2ah
    DQ 3ff44e086061892dh
    DQ 3ff486a2b5c13cd0h
    DQ 3ff4bfdad5362a27h
    DQ 3ff4f9b2769d2ca7h
    DQ 3ff5342b569d4f82h
    DQ 3ff56f4736b527dah
    DQ 3ff5ab07dd485429h
    DQ 3ff5e76f15ad2148h
    DQ 3ff6247eb03a5585h
    DQ 3ff6623882552225h
    DQ 3ff6a09e667f3bcdh
    DQ 3ff6dfb23c651a2fh
    DQ 3ff71f75e8ec5f74h
    DQ 3ff75feb564267c9h
    DQ 3ff7a11473eb0187h
    DQ 3ff7e2f336cf4e62h
    DQ 3ff82589994cce13h
    DQ 3ff868d99b4492edh
    DQ 3ff8ace5422aa0dbh
    DQ 3ff8f1ae99157736h
    DQ 3ff93737b0cdc5e5h
    DQ 3ff97d829fde4e50h
    DQ 3ff9c49182a3f090h
    DQ 3ffa0c667b5de565h
    DQ 3ffa5503b23e255dh
    DQ 3ffa9e6b5579fdbfh
    DQ 3ffae89f995ad3adh
    DQ 3ffb33a2b84f15fbh
    DQ 3ffb7f76f2fb5e47h
    DQ 3ffbcc1e904bc1d2h
    DQ 3ffc199bdd85529ch
    DQ 3ffc67f12e57d14bh
    DQ 3ffcb720dcef9069h
    DQ 3ffd072d4a07897ch
    DQ 3ffd5818dcfba487h
    DQ 3ffda9e603db3285h
    DQ 3ffdfc97337b9b5fh
    DQ 3ffe502ee78b3ff6h
    DQ 3ffea4afa2a490dah
    DQ 3ffefa1bee615a27h
    DQ 3fff50765b6e4540h
    DQ 3fffa7c1819e90d8h

ALIGN 16
L__two_to_jby64_head_table:
    DQ 3ff0000000000000h
    DQ 3ff02c9a30000000h
    DQ 3ff059b0d0000000h
    DQ 3ff0874510000000h
    DQ 3ff0b55860000000h
    DQ 3ff0e3ec30000000h
    DQ 3ff11301d0000000h
    DQ 3ff1429aa0000000h
    DQ 3ff172b830000000h
    DQ 3ff1a35be0000000h
    DQ 3ff1d48730000000h
    DQ 3ff2063b80000000h
    DQ 3ff2387a60000000h
    DQ 3ff26b4560000000h
    DQ 3ff29e9df0000000h
    DQ 3ff2d285a0000000h
    DQ 3ff306fe00000000h
    DQ 3ff33c08b0000000h
    DQ 3ff371a730000000h
    DQ 3ff3a7db30000000h
    DQ 3ff3dea640000000h
    DQ 3ff4160a20000000h
    DQ 3ff44e0860000000h
    DQ 3ff486a2b0000000h
    DQ 3ff4bfdad0000000h
    DQ 3ff4f9b270000000h
    DQ 3ff5342b50000000h
    DQ 3ff56f4730000000h
    DQ 3ff5ab07d0000000h
    DQ 3ff5e76f10000000h
    DQ 3ff6247eb0000000h
    DQ 3ff6623880000000h
    DQ 3ff6a09e60000000h
    DQ 3ff6dfb230000000h
    DQ 3ff71f75e0000000h
    DQ 3ff75feb50000000h
    DQ 3ff7a11470000000h
    DQ 3ff7e2f330000000h
    DQ 3ff8258990000000h
    DQ 3ff868d990000000h
    DQ 3ff8ace540000000h
    DQ 3ff8f1ae90000000h
    DQ 3ff93737b0000000h
    DQ 3ff97d8290000000h
    DQ 3ff9c49180000000h
    DQ 3ffa0c6670000000h
    DQ 3ffa5503b0000000h
    DQ 3ffa9e6b50000000h
    DQ 3ffae89f90000000h
    DQ 3ffb33a2b0000000h
    DQ 3ffb7f76f0000000h
    DQ 3ffbcc1e90000000h
    DQ 3ffc199bd0000000h
    DQ 3ffc67f120000000h
    DQ 3ffcb720d0000000h
    DQ 3ffd072d40000000h
    DQ 3ffd5818d0000000h
    DQ 3ffda9e600000000h
    DQ 3ffdfc9730000000h
    DQ 3ffe502ee0000000h
    DQ 3ffea4afa0000000h
    DQ 3ffefa1be0000000h
    DQ 3fff507650000000h
    DQ 3fffa7c180000000h

ALIGN 16
L__two_to_jby64_tail_table:
    DQ 0000000000000000h
    DQ 3e6cef00c1dcdef9h
    DQ 3e48ac2ba1d73e2ah
    DQ 3e60eb37901186beh
    DQ 3e69f3121ec53172h
    DQ 3e469e8d10103a17h
    DQ 3df25b50a4ebbf1ah
    DQ 3e6d525bbf668203h
    DQ 3e68faa2f5b9bef9h
    DQ 3e66df96ea796d31h
    DQ 3e368b9aa7805b80h
    DQ 3e60c519ac771dd6h
    DQ 3e6ceac470cd83f5h
    DQ 3e5789f37495e99ch
    DQ 3e547f7b84b09745h
    DQ 3e5b900c2d002475h
    DQ 3e64636e2a5bd1abh
    DQ 3e4320b7fa64e430h
    DQ 3e5ceaa72a9c5154h
    DQ 3e53967fdba86f24h
    DQ 3e682468446b6824h
    DQ 3e3f72e29f84325bh
    DQ 3e18624b40c4dbd0h
    DQ 3e5704f3404f068eh
    DQ 3e54d8a89c750e5eh
    DQ 3e5a74b29ab4cf62h
    DQ 3e5a753e077c2a0fh
    DQ 3e5ad49f699bb2c0h
    DQ 3e6a90a852b19260h
    DQ 3e56b48521ba6f93h
    DQ 3e0d2ac258f87d03h
    DQ 3e42a91124893ecfh
    DQ 3e59fcef32422cbeh
    DQ 3e68ca345de441c5h
    DQ 3e61d8bee7ba46e1h
    DQ 3e59099f22fdba6ah
    DQ 3e4f580c36bea881h
    DQ 3e5b3d398841740ah
    DQ 3e62999c25159f11h
    DQ 3e668925d901c83bh
    DQ 3e415506dadd3e2ah
    DQ 3e622aee6c57304eh
    DQ 3e29b8bc9e8a0387h
    DQ 3e6fbc9c9f173d24h
    DQ 3e451f8480e3e235h
    DQ 3e66bbcac96535b5h
    DQ 3e41f12ae45a1224h
    DQ 3e55e7f6fd0fac90h
    DQ 3e62b5a75abd0e69h
    DQ 3e609e2bf5ed7fa1h
    DQ 3e47daf237553d84h
    DQ 3e12f074891ee83dh
    DQ 3e6b0aa538444196h
    DQ 3e6cafa29694426fh
    DQ 3e69df20d22a0797h
    DQ 3e640f12f71a1e45h
    DQ 3e69f7490e4bb40bh
    DQ 3e4ed9942b84600dh
    DQ 3e4bdcdaf5cb4656h
    DQ 3e5e2cffd89cf44ch
    DQ 3e452486cc2c7b9dh
    DQ 3e6cc2b44eee3fa4h
    DQ 3e66dc8a80ce9f09h
    DQ 3e39e90d82e90a7eh

data ENDS

END
