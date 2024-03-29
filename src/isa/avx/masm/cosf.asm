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

;
; An implementation of the cosf function.
;
; Prototype:
;
;     float cosf(float x);
;
;   Computes cosf(x).  
;   Based on the NAG C implementation.
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Original Author: Harsha Jagasia

CONST    SEGMENT
ALIGN 16
L__real_3ff0000000000000   DQ 03ff0000000000000h      ; 1.0
                        DQ 0                          ; for alignment
L__real_3fe0000000000000   DQ 03fe0000000000000h      ; 0.5
                        DQ 0
L__real_3fc5555555555555   DQ 03fc5555555555555h      ; 0.166666666666
                        DQ 0
L__real_3fe45f306dc9c883   DQ 03fe45f306dc9c883h      ; twobypi
                        DQ 0
L__real_3FF921FB54442D18   DQ 03FF921FB54442D18h      ; piby2
                        DQ 0
L__real_3ff921fb54400000   DQ 03ff921fb54400000h      ; piby2_1
                        DQ 0
L__real_3dd0b4611a626331   DQ 03dd0b4611a626331h      ; piby2_1tail
                        DQ 0
L__real_3dd0b4611a600000   DQ 03dd0b4611a600000h      ; piby2_2
                        DQ 0
L__real_3ba3198a2e037073   DQ 03ba3198a2e037073h      ; piby2_2tail
                        DQ 0                                         
L__real_fffffffff8000000   DQ 0fffffffff8000000h      ; mask for stripping head and tail
                        DQ 0                  
L__real_411E848000000000    DQ 0415312d000000000h     ; 5e6 0411E848000000000h     ; 5e5
                        DQ 0
                        
ALIGN 16
Lcsarray:
    DQ    0bfc5555555555555h                          ; -0.166667        s1
    DQ    03fa5555555555555h                          ; 0.0416667        c1
    DQ    03f81111111110bb3h                          ; 0.00833333       s2
    DQ    0bf56c16c16c16967h                          ; -0.00138889      c2
    DQ    0bf2a01a019e83e5ch                          ; -0.000198413     s3
    DQ    03efa01a019f4ec90h                          ; 2.48016e-005     c3
    DQ    03ec71de3796cde01h                          ; 2.75573e-006     s4
    DQ    0be927e4fa17f65f6h                          ; -2.75573e-007    c4
CONST    ENDS

; define local variable storage offsets
p_temp        equ        030h                         ; temporary for get/put bits operation
p_temp1       equ        040h                         ; temporary for get/put bits operation
region        equ        050h                         ; pointer to region for amd_remainder_piby2
r             equ        060h                         ; pointer to r for amd_remainder_piby2
stack_size    equ        088h

include fm.inc

ALM_PROTO_BAS64 cosf


fname_special   TEXTEQU <_cosf_special>

;Define name and any external functions being called
EXTRN         __amd_remainder_piby2d2f : PROC    ; NEAR
EXTERN        fname_special        : PROC

StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
StackAllocate stack_size
    .ENDPROLOG   

;;  if NaN or inf
	movd    edx, xmm0
    mov     eax, 07f800000h
    mov     r10d, eax
    and     r10d, edx
    cmp     r10d, eax
    jz      L__cos_naninf   
     
    xorpd   xmm2, xmm2
    mov     r11, rdx                                  ; save 1st return value pointer

;  GET_BITS_DP64(x, ux);
; convert input to double.
    cvtss2sd    xmm0, xmm0

; get the input value to an integer register.
    movsd   QWORD PTR p_temp[rsp], xmm0
    mov     rdx, QWORD PTR p_temp[rsp]                ; rdx is ux


;  ax = (ux & ~SIGNBIT_DP64);
    mov     r10, 07fffffffffffffffh
    and     r10, rdx            ; r10 is ax

    mov     r8d, 1                                    ; for determining region later on
    movsd   xmm1, xmm0                                ; copy x to xmm1


;;  if (ax <= 3fe921fb54442d18h) /* abs(x) <= pi/4 */
    mov     rax, 03fe921fb54442d18h
    cmp     r10, rax
    jg      L__sc_reducec

;          *c = cos_piby4(x, 0.0);
    movsd   xmm2, xmm0 
    mulsd   xmm2, xmm2        ;x^2
    xor     eax, eax
    mov     rdx, r10
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5
    jmp     L__sc_piby4c        ; done


ALIGN 16
L__sc_reducec:    
; reduce  the argument to be in a range from -pi/4 to +pi/4
; by subtracting multiples of pi/2
;  xneg = (ax != ux);
    cmp     rdx, r10
;;  if (xneg) x = -x;
    jz      Lpositive
    subsd   xmm2, xmm0
    movsd   xmm0, xmm2

Lpositive:
;;  if (x < 5.0e5)
    cmp     r10, QWORD PTR L__real_411E848000000000
    jae     Lcosf_reduce_precise

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; perform taylor series to calc cosx, cosx
; xmm0=abs(x), xmm1=x
;/* How many pi/2 is x a multiple of? */
;        npi2  = (int)(x * twobypi + 0.5);
ALIGN 16
Lcosf_piby4:
    movsd   xmm2, xmm0
    movsd   xmm4, xmm0

    mulsd   xmm2, QWORD PTR L__real_3fe45f306dc9c883  ; twobypi
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5 

; How many pi/2 is x a multiple of?
;      xexp  = ax >> EXPSHIFTBITS_DP64;
    mov     r9, r10
    shr     r9, 52                                    ; >> EXPSHIFTBITS_DP64

;        npi2  = (int)(x * twobypi + 0.5);
    addsd   xmm2, xmm5                                ; npi2
    
    movsd   xmm3, QWORD PTR L__real_3ff921fb54400000  ; piby2_1
    cvttpd2dq    xmm0, xmm2                           ; convert to integer 
    movsd   xmm1, QWORD PTR L__real_3dd0b4611a626331  ; piby2_1tail    
    cvtdq2pd    xmm2, xmm0                            ; and back to double    

;    Subtract the multiple from x to get an extra-precision remainder 
;      rhead  = x - npi2 * piby2_1;

    mulsd   xmm3, xmm2                                ; use piby2_1
    subsd   xmm4, xmm3                                ; rhead

;      rtail  = npi2 * piby2_1tail;
    mulsd   xmm1, xmm2                                ; rtail
    movd    eax, xmm0

; GET_BITS_DP64(rhead-rtail, uy);               
; originally only rhead
    movsd   xmm0, xmm4
    subsd   xmm0, xmm1

    movsd   xmm3, QWORD PTR L__real_3dd0b4611a600000  ; piby2_2
    movsd   xmm5, QWORD PTR L__real_3ba3198a2e037073  ; piby2_2tail
    movd    rcx, xmm0                                 ; rcx is rhead-rtail

;      region = npi2 & 3;
;    and        eax, 3
;      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     rcx, 1                                    ; strip any sign bit
    shr     rcx, 53                                   ; >> EXPSHIFTBITS_DP64 +1
    sub     r9, rcx                                   ; expdiff

;;      if (expdiff > 15)
    cmp     r9, 15
    jle     Lexpdiffless15

; The remainder is pretty small compared with x, which
; implies that x is a near multiple of pi/2
; (x matches the multiple to at least 15 bits)
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
; region = npi2 & 3;
; rhead-r
; rr = (rhead-r) - rtail

    movsd   xmm2, xmm0 
    mulsd   xmm2, xmm0                                ; x^2
    movsd   xmm1, xmm0
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5
    
    cmp     rcx, 03f2h                                ; if r  small.
    jge     L__sc_piby4c                              ; use taylor series if not
    cmp     rcx, 03deh                                ; if r really small.
    jle     L__rc_small                               ; then cos(r) = 1

;;      if region is 1 or 3    do a sin calc.
    and     r8d, eax
    jz      Lsinsmall
; region 1 or 3
; use simply polynomial
;              *s = x - x*x*x*0.166666666666666666;
    movsd   xmm3, QWORD PTR L__real_3fc5555555555555  
    mulsd   xmm3, xmm1                                ; * x
    mulsd   xmm3, xmm2                                ; * x^2
    subsd   xmm1, xmm3                                ; xs
    jmp     L__adjust_region_cos
    
ALIGN 16
Lsinsmall:
; region 0 or 2
;              cos = 1.0 - x*x*0.5;
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000  ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000  ; 0.5 *x^2
    subsd   xmm1, xmm2
    jmp     L__adjust_region_cos

ALIGN 16
L__rc_small:                                          
; then sin(r) = r
; if region is 1 or 3    do a sin calc.
    and     r8d, eax
    jnz     L__adjust_region_cos
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000  ; cos(r) is a 1 
    jmp     L__adjust_region_cos


; done with reducing the argument.  Now perform the sin/cos calculations.
; if region is 1 or 3    do a sin calc.
ALIGN 16
L__sc_piby4c:
    and     r8d, eax
    jz      Lcospiby4
    
    movsd   xmm1, QWORD PTR Lcsarray+30h              ; c4
    movsd   xmm4, xmm2
    mulsd   xmm1, xmm2                                ; x2c4
    movsd   xmm3, QWORD PTR Lcsarray+10h              ; c2
    mulsd   xmm4, xmm4                                ; x4    
    mulsd   xmm3, xmm2                                ; x2c2
    mulsd   xmm2, xmm0                                ; x3
    addsd   xmm1, QWORD PTR Lcsarray+20h              ; c3 + x2c4    
    mulsd   xmm1, xmm4                                ; x4(c3 + x2c4)
    addsd   xmm3, QWORD PTR Lcsarray                  ; c1 + x2c2
    addsd   xmm1, xmm3                                ; c1 + c2x2 + c3x4 + c4x6
    mulsd   xmm1, xmm2                                ; c1x3 + c2x5 + c3x7 + c4x9
    addsd   xmm1, xmm0                                ; x + c1x3 + c2x5 + c3x7 + c4x9

    jmp     L__adjust_region_cos
    
ALIGN 16
Lcospiby4:    
; region 0 or 2     - do a cos calculation
    movsd   xmm1, QWORD PTR Lcsarray+38h              ; c4
    movsd   xmm4, xmm2
    mulsd   xmm1, xmm2                                ; x2c4
    movsd   xmm3, QWORD PTR Lcsarray+18h              ; c2
    mulsd   xmm4, xmm4                                ; x4
    mulsd   xmm3, xmm2                                ; x2c2
    mulsd   xmm5, xmm2                                ; 0.5 * x2
    addsd   xmm1, QWORD PTR Lcsarray+28h              ; c3 + x2c4
    mulsd   xmm1, xmm4                                ; x4(c3 + x2c4)    
    addsd   xmm3, QWORD PTR Lcsarray+8                ; c1 + x2c2
    addsd   xmm1, xmm3                                ; c1 + x2c2 + c3x4 + c4x6
    mulsd   xmm1, xmm4                                ; x4(c1 + c2x2 + c3x4 + c4x6)
    subsd   xmm5, QWORD PTR L__real_3ff0000000000000  ; 0.5x2 - 1
    subsd   xmm1, xmm5                                ; cos = 1 - 0.5x2 + c1x4 + c2x6 + c3x8 + c4x10

L__adjust_region_cos:                                 
; xmm1 is cos or sin, relies on previous sections to 
;      switch (region)         
    add     eax, 1
    and     eax, 2
    jz      L__cos_cleanup
;; if region 1 or 2 then we negate the result.
    xorpd   xmm2, xmm2
    subsd   xmm2, xmm1
    movsd   xmm1, xmm2

ALIGN 16    
L__cos_cleanup:
    cvtsd2ss   xmm0, xmm1
    add     rsp, stack_size
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;      /* Reduce abs(x) into range [-pi/4, pi/4] */
;      __amd_remainder_piby2(ax, &r, &region);
ALIGN 16
Lcosf_reduce_precise:
    mov     QWORD PTR p_temp[rsp], rdx                ; save ux for use later
    mov     QWORD PTR p_temp1[rsp], r10               ; save ax for use later
    movd    rcx, xmm0
	lea	    rdx, QWORD PTR r[rsp]
	lea	    r8,	 QWORD PTR region[rsp]
    sub     rsp, 020h    

    call    __amd_remainder_piby2d2f

    add     rsp, 020h
    mov     rdx, QWORD PTR p_temp[rsp]                ; restore ux for use later
    mov     r10, QWORD PTR p_temp1[rsp]               ; restore ax for use later    
    mov     r8d, 1                                    ; for determining region later on
	movsd	xmm0, QWORD PTR r[rsp]		              ; r
	mov	    eax,  DWORD PTR region[rsp]	              ; region

    movsd   xmm2, xmm0 
    mulsd   xmm2, xmm0                                ;x^2
    movsd   xmm1, xmm0
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5

    jmp     L__sc_piby4c


L__cos_naninf:
    call    fname_special
    add     rsp, stack_size
    ret

fname       endp
TEXT    ENDS
END
