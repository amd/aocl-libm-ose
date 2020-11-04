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
; An implementation of the sincosf function.
;
; Prototype:
;
;     void sincosf(float x, float * sinfx, float * cosfx);
;
;   Computes sinf(x) and cosf(x).  
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the NAG C implementation.

CONST    SEGMENT
ALIGN 16
__real_3ff0000000000000 DQ 03ff0000000000000h  ; 1.0
                        DQ 0                       ; for alignment
__real_3fe0000000000000 DQ 03fe0000000000000h  ; 0.5
                        DQ 0
__real_3fc5555555555555 DQ 03fc5555555555555h  ; 0.166666666666
                        DQ 0
__real_3fe45f306dc9c883 DQ 03fe45f306dc9c883h  ; twobypi
                        DQ 0
__real_3FF921FB54442D18 DQ 03FF921FB54442D18h  ; piby2
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
__real_8000000000000000 DQ 08000000000000000h  ; -0  or signbit
                        DQ 0
__real_411E848000000000 DQ 0415312d000000000h   ; 5e6 0411E848000000000h  ; 5e5
                        DQ 0

ALIGN 16
Lcsarray:
    DQ    0bfc5555555555555h                      ; -0.166667           s1
    DQ    03fa5555555555555h                      ; 0.0416667           c1
    DQ    03f81111111110bb3h                      ; 0.00833333          s2
    DQ    0bf56c16c16c16967h                      ; -0.00138889         c2
    DQ    0bf2a01a019e83e5ch                      ; -0.000198413        s3
    DQ    03efa01a019f4ec90h                      ; 2.48016e-005        c3
    DQ    03ec71de3796cde01h                      ; 2.75573e-006        s4
    DQ    0be927e4fa17f65f6h                      ; -2.75573e-007       c4
    DQ    0be5ae600b42fdfa7h                      ; -2.50511e-008       s5
    DQ    03e21eeb69037ab78h                      ; 2.08761e-009        c5
    DQ    03de5e0b2f9a43bb8h                      ; 1.59181e-010        s6
    DQ    0bda907db46cc5e42h                      ; -1.13826e-011       c6
CONST    ENDS

; define local variable storage offsets
p_temp      equ    030h                              ; temporary for get/put bits operation
p_temp1     equ    040h                              ; temporary for get/put bits operation
p_temp2     equ    050h                              ; temporary for get/put bits operation
p_temp3     equ    060h                              ; temporary for get/put bits operation
region      equ    070h                              ; pointer to region for amd_remainder_piby2
r           equ    080h                              ; pointer to r for amd_remainder_piby2
save_rdx    equ    090h
save_r8     equ    0a0h
stack_size  equ    0c8h

include fm.inc
FN_PROTOTYPE sincosf

fname_special   TEXTEQU <_sincosf_special>

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

    xorpd   xmm2, xmm2
	mov 	QWORD PTR save_rdx[rsp], rdx	         ; save the return pointer
	mov 	QWORD PTR save_r8[rsp], r8
	    
                      ; rdx is ux

	movd	rdx,xmm0
;;  if NaN or inf
    mov     rax, 07f800000h
    mov     r10, rax
    and     r10, rdx
    cmp     r10d, eax
    jz      __sc_naninf

;  GET_BITS_DP64(x, ux);
; convert input to double.
    cvtss2sd    xmm0, xmm0
	
; get the input value to an integer register.
	movd	rdx,xmm0
    


;  ax = (ux & ~SIGNBIT_DP64);
    mov     r10, 07fffffffffffffffh
    and     r10, rdx                                 ; r10 is ax

;;  if (ax <= 3fe921fb54442d18h) /* abs(x) <= pi/4 */
    mov     rax, 03fe921fb54442d18h
    cmp     r10, rax
    jg      __sc_reduce

;;          if (ax < 0x3f20000000000000) abs(x) < 2.0^(-13)
    mov     rax, 3f20000000000000h
    cmp     r10, rax
    jge     __sc_notsmallest

; sin = x, cos = 1.0;
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     
    jmp     __sc_cleanup   
    
; *s = sin_piby4(x, 0.0);
; *c = cos_piby4(x, 0.0);
__sc_notsmallest:
    xor     eax, eax                                 ; region 0
    mov     rdx, r10
    movsd   xmm5, QWORD PTR __real_3fe0000000000000     ; .5
    jmp     __sc_piby4      

__sc_reduce:    

; reduce  the argument to be in a range from -pi/4 to +pi/4
; by subtracting multiples of pi/2

;  xneg = (ax != ux);
    cmp     rdx, r10
;;  if (xneg) x = -x;
    jz      Lpositive
    subsd   xmm2, xmm0
    movsd   xmm0, xmm2

ALIGN 16
Lpositive:
;;  if (x < 5.0e5)
    cmp     r10, QWORD PTR __real_411E848000000000
    jae     Lsincosf_reduce_precise

    movsd   xmm2, xmm0
    movsd   xmm4, xmm0

    mulsd   xmm2, QWORD PTR __real_3fe45f306dc9c883     ; twobypi
    movsd   xmm5, QWORD PTR __real_3fe0000000000000     ; .5 

;/* How many pi/2 is x a multiple of? */
;      xexp  = ax >> EXPSHIFTBITS_DP64;
    mov     r9, r10
    shr     r9, 52                                   ; >> EXPSHIFTBITS_DP64

;        npi2  = (int)(x * twobypi + 0.5);
    addsd   xmm2, xmm5                               ; npi2

    movsd   xmm3, QWORD PTR __real_3ff921fb54400000     ; piby2_1
    cvttpd2dq    xmm0, xmm2                          ; convert to integer 
    movsd   xmm1, QWORD PTR __real_3dd0b4611a626331     ; piby2_1tail    
    cvtdq2pd    xmm2, xmm0                           ; and back to float.    

;      /* Subtract the multiple from x to get an extra-precision remainder */
;      rhead  = x - npi2 * piby2_1;

    mulsd   xmm3, xmm2                               ; use piby2_1
    subsd   xmm4, xmm3                               ; rhead

;      rtail  = npi2 * piby2_1tail;
    mulsd   xmm1, xmm2                               ; rtail

    movd    eax, xmm0

; GET_BITS_DP64(rhead-rtail, uy);               ; originally only rhead
    movsd   xmm0, xmm4
    subsd   xmm0, xmm1

    movsd   xmm3, QWORD PTR __real_3dd0b4611a600000     ; piby2_2
    movsd   xmm5, QWORD PTR __real_3ba3198a2e037073     ; piby2_2tail
    movd    rcx, xmm0

;      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     rcx, 1                                   ; strip any sign bit
    shr     rcx, 53                                  ; >> EXPSHIFTBITS_DP64 +1
    sub     r9, rcx                                  ; expdiff

;;      if (expdiff > 15)
    cmp     r9, 15
    jle     Lexpdiff15

; /* The remainder is pretty small compared with x, which
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

;
;xmm1=rtail
    movsd   xmm1, xmm5
    subsd   xmm0, xmm5

;      region = npi2 & 3;
;    and    eax, 3
;xmm0=r, xmm4=rhead, xmm1=rtail
Lexpdiff15:

;; if the input was close to a pi/2 multiple
;

    cmp     rcx, 03f2h                              ; if r  small.
    jge     __sc_piby4                              ; use taylor series if not
    cmp     rcx, 03deh                              ; if r really small.
    jle     Lsinsmall                                ; then sin(r) = r

    movsd   xmm2, xmm0 
    mulsd   xmm2, xmm2                               ; x^2
; use simply polynomial
;              *s = x - x*x*x*0.166666666666666666;
    movsd   xmm3, QWORD PTR __real_3fc5555555555555     ; 
    mulsd   xmm3, xmm0                               ; * x
    mulsd   xmm3, xmm2                               ; * x^2
    subsd   xmm0, xmm3                               ; xs

;              *c = 1.0 - x*x*0.5;
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; 1.0
    mulsd   xmm2, QWORD PTR __real_3fe0000000000000     ; 0.5 *x^2
    subsd   xmm1, xmm2
    jmp     __adjust_region

Lsinsmall:                                           ; then sin(r) = r
    movsd   xmm1, QWORD PTR __real_3ff0000000000000     ; cos(r) is a 1 
    jmp     __adjust_region

; perform taylor series to calc sinx, cosx
;  COS
;  x2 = x * x;
;  return (1.0 - 0.5 * x2 + (x2 * x2 *
;                            (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))));
;  x2 = x * x;
;  return (1.0 - 0.5 * x2 + (x2 * x2 *
;                            (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))));                                                    
;  SIN
;  zc,zs = (c2 + x2 * (c3 + x2 * c4 ));
;  xs =  r + x3 * (sc1 + x2 * zs);
;  x2 = x * x;
;  return (x + x * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));
; done with reducing the argument.  Now perform the sin/cos calculations.
ALIGN 16
__sc_piby4:
;  x2 = r * r;
    movsd   xmm5, QWORD PTR __real_3fe0000000000000     ; .5 
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                               ; x2
    shufpd  xmm2, xmm2, 0                            ; x2,x2
    movsd   xmm4, xmm2
    mulsd   xmm4, xmm4                               ; x4
    shufpd  xmm4, xmm4, 0                            ; x4,x4

;  x2m =    _mm_set1_pd (x2);
;  zc,zs = (c2 + x2 * (c3 + x2 * c4 ));
;    xs =  r + x3 * (sc1 + x2 * zs);
;    xc =  t + ( x2 * x2 * (cc1 + x2 * zc));
    movapd  xmm1, QWORD PTR Lcsarray+30h                ; c4
    movapd  xmm3, QWORD PTR Lcsarray+10h                ; c2
    mulpd   xmm1, xmm2                               ; x2c4
    mulpd   xmm3, xmm2                               ; x2c2

;  rc = 0.5 * x2;
    mulsd   xmm5, xmm2                               ;rc
    mulsd   xmm2, xmm0                               ;x3

    addpd   xmm1, QWORD PTR Lcsarray+20h             ; c3 + x2c4
    addpd   xmm3, QWORD PTR Lcsarray                 ; c1 + x2c2
    mulpd   xmm1, xmm4                               ;    x4(c3 + x2c4)
    addpd   xmm1, xmm3                               ; c1 + x2c2 + x4(c3 + x2c4)

;  -t = rc-1;
    subsd   xmm5, QWORD PTR __real_3ff0000000000000  ; 1.0  
; now we have the poly for sin in the low half, and cos in upper half
    mulsd   xmm2, xmm1                               ; x3(sin poly)
    shufpd  xmm1, xmm1, 3                            ; get cos poly to low half of register
    mulsd   xmm1, xmm4                               ; x4(cos poly)

    addsd   xmm0, xmm2                               ; sin = r+...
    subsd   xmm1, xmm5                               ; cos = poly-(-t)

__adjust_region:                                     ; xmm0 is sin, xmm1 is cos
;      switch (region)
    mov     ecx, eax
    and     eax, 1
    jz      Lregion02
; region 1 or 3
    movsd   xmm2, xmm0                               ; swap sin,cos
    movsd   xmm0, xmm1                               ; sin = cos
    xorpd   xmm1, xmm1
    subsd   xmm1, xmm2                               ; cos = -sin
   
Lregion02:
    and     ecx, 2
    jz      Lregion23
; region 2 or 3
    movsd   xmm2, xmm0
    movsd   xmm3, xmm1
    xorpd   xmm0, xmm0
    xorpd   xmm1, xmm1
    subsd   xmm0, xmm2                               ; sin = -sin
    subsd   xmm1, xmm3                               ; cos = -cos
  
Lregion23:        
;;  if (xneg) *s = -*s ;
    cmp     rdx, r10
    jz      __sc_cleanup
    movsd   xmm2, xmm0
    xorpd   xmm0, xmm0
    subsd   xmm0, xmm2                               ; sin = -sin

ALIGN 16
__sc_cleanup:
	mov 	rdx, QWORD PTR save_rdx[rsp]	         ; restore the return pointer
	mov 	r8, QWORD PTR save_r8[rsp]
	 
    cvtsd2ss xmm0, xmm0                              ; convert back to floats
    cvtsd2ss xmm1, xmm1

    movss   DWORD PTR [rdx], xmm0                    ; save the sin
    movss   DWORD PTR [r8], xmm1                    ; save the cos
    
    add     rsp, stack_size
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincosf_reduce_precise:
;      /* Reduce abs(x) into range [-pi/4,pi/4] */
;      __amd_remainder_piby2(ax, &r, &region);

    mov     QWORD PTR p_temp[rsp], rdx               ; save ux for use later
    mov     QWORD PTR p_temp1[rsp], r10              ; save ax for use later
	movd	rcx, xmm0
	lea	    rdx, QWORD PTR r[rsp]
	lea	    r8,  QWORD PTR region[rsp]
	sub	    rsp,020h	

    call    __amd_remainder_piby2d2f

    add     rsp, 020h
    mov     rdx, QWORD PTR p_temp[rsp]               ; restore ux for use later
    mov     r10, QWORD PTR p_temp1[rsp]              ; restore ax for use later    

    mov     r8d, 1                                   ; for determining region later on
    movsd   xmm0, QWORD PTR r[rsp]                   ; r
    mov     eax, DWORD PTR region[rsp]               ; region
    jmp     __sc_piby4
 
ALIGN 16    
__sc_naninf:
    ;cvtsd2ss xmm0, xmm0                              ; convert back to floats
    mov     rdx, QWORD PTR save_rdx[rsp]             ; r8 did not change
    call    fname_special
    add     rsp, stack_size
    ret

fname       endp
TEXT    ENDS
END
