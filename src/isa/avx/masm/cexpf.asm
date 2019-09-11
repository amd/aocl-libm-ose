;
; An implementation of the complex expf function.
;
; Prototype:
;
;   float complex cexpf(float complex x)
;
;   Computes cexpf
;   It will provide proper C99 return values,
;   but may not raise floating point status bits properly.
;   Based on the sincosf and expf implementation.
;
;

; define local variable storage offsets
p_temp0      equ    030h                             ; temporary for get/put bits operation
p_temp1      equ    040h                             ; temporary for get/put bits operation
p_temp2      equ    050h                             ; temporary for get/put bits operation
region       equ    060h                             ; pointer to region for amd_remainder_piby2
r            equ    070h                             ; pointer to r for amd_remainder_piby2
save_rax     equ    080h                             ; for windows calling convention
stack_size   equ    0a8h

include fm.inc
FN_PROTOTYPE cexpf


fname_special1  TEXTEQU <_sincosf_special>
fname_special2  TEXTEQU <_expf_special>

;Define name and any external functions being called
EXTRN        __amd_remainder_piby2d2f : PROC   ; NEAR
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
	    
;start the expf calculation
L_expf:
	movd        xmm1, rdx
    cvtps2pd    xmm1, xmm1
    movhpd      QWORD PTR [p_temp0+rsp], xmm1         ; save image part for sincosf usage
    
    movd        xmm0, edx
;    movd        edx, xmm0
    mov         eax, DWORD PTR L__real_inf
    and         eax, edx
    cmp         eax, DWORD PTR L__real_inf
    je          L__x_is_inf_or_nan

    cvtss2sd    xmm0, xmm0
       
    ; x * (64/ln(2))
    movsd       xmm3, L__real_64_by_log2
    mulsd       xmm3, xmm0

    ; x <= 128*ln(2), ( x * (64/ln(2)) ) <= 64*128
    ; x > -150*ln(2), ( x * (64/ln(2)) ) > 64*(-150)
    comisd      xmm3, L__real_p8192
    jae         L__y_is_inf

    comisd      xmm3, L__real_m9600
    jb          L__y_is_zero

    ; n = int( x * (64/ln(2)) )
    cvtpd2dq    xmm4, xmm3
    lea         r10, L__two_to_jby64_table
    cvtdq2pd    xmm1, xmm4

    ; r = x - n * ln(2)/64
    movsd       xmm2, L__real_log2_by_64
    mulsd       xmm2, xmm1
    movd        ecx, xmm4
    mov         rax, 3fh
    and         eax, ecx
    subsd       xmm0, xmm2
    movsd       xmm1, xmm0

    ; m = (n - j) / 64
    sub         ecx, eax
    sar         ecx, 6

    ; q
    movsd       xmm3, L__real_1_by_6
    mulsd       xmm3, xmm0
    mulsd       xmm0, xmm0
    addsd       xmm3, L__real_1_by_2
    mulsd       xmm0, xmm3
    addsd       xmm0, xmm1

    add         rcx, 1023
    shl         rcx, 52

    ; (f)*(1+q)
    movsd       xmm2, QWORD PTR [r10+rax*8] 
    mulsd       xmm0, xmm2
    addsd       xmm0, xmm2

    mov         QWORD PTR [p_temp2+rsp], rcx
    mulsd       xmm0, QWORD PTR [p_temp2+rsp]
;   cvtsd2ss    xmm0, xmm0          ;calculation not done yet
;    movsd       QWORD PTR [p_temp2+rsp], xmm0         ; save xmm0(exp(x)) to p_temp2
    jmp         L__SinCosf

ALIGN 16
L__y_is_zero:

    movss       xmm1, DWORD PTR L__real_zero
    movd        xmm0, edx
    mov         r8d, DWORD PTR L__flag_y_zero

    call        fname_special2
	movsd       xmm0, QWORD PTR L__real_zero    
    jmp         L__SinCosf                           

ALIGN 16
L__y_is_inf:

    movss       xmm1, DWORD PTR L__real_inf
    movd        xmm0, edx
    mov         r8d, DWORD PTR L__flag_y_inf

    call        fname_special2
    movsd       xmm0, QWORD PTR L__real_7ff0000000000000
    jmp         L__SinCosf       

ALIGN 16
L__x_is_inf_or_nan:

    cmp         edx, DWORD PTR L__real_inf
    movsd       xmm0, QWORD PTR L__real_7ff0000000000000
    je          L__SinCosf

    cmp         edx, DWORD PTR L__real_ninf
    movsd       xmm0, QWORD PTR L__real_zero
	je          L__SinCosf

    mov         r9d, DWORD PTR L__real_qnanbit
    and         r9d, edx
    movsd       xmm0, QWORD PTR L__real_7ff8000000000000
    jnz         L__SinCosf

	movd        xmm0, edx
    or          edx, DWORD PTR L__real_qnanbit
    movd        xmm1, edx
    mov         r8d, DWORD PTR L__flag_x_nan
    call        fname_special2
	movsd       xmm0, QWORD PTR L__real_7ff8000000000000 

L__SinCosf:
    movsd       QWORD PTR [p_temp2+rsp], xmm0         ; save xmm0(exp(x)) to p_temp2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; start the sincosf calculation    
    xorpd   xmm2, xmm2
    
;  GET_BITS_DP64(x, ux);
; convert input to double.
    movsd   xmm0, QWORD PTR [p_temp0+rsp]
; get the input value to an integer register.
    mov     rdx, QWORD PTR [p_temp0+rsp]              ; rdx is ux

;;  if NaN or inf
    mov     rax, 07ff0000000000000h
    mov     r10, rax
    and     r10, rdx
    cmp     r10, rax
    jz      L__sc_naninf
    
;  ax = (ux & ~SIGNBIT_DP64);
    mov     r10, 07fffffffffffffffh
    and     r10, rdx                                  ; r10 is ax

;;  if (ax <= 3fe921fb54442d18h) /* abs(x) <= pi/4 */
    mov     rax, 03fe921fb54442d18h
    cmp     r10, rax
    jg      L__sc_reduce

;;          if (ax < 0x3f20000000000000) abs(x) < 2.0^(-13)
    mov     rax, 3f20000000000000h
    cmp     r10, rax
    jge     L__sc_notsmallest

; sin = x, cos = 1.0;
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000     
    jmp     L__sc_cleanup   
    
; *s = sin_piby4(x, 0.0);
; *c = cos_piby4(x, 0.0);
L__sc_notsmallest:
    xor     eax, eax                                  ; region 0
    mov     rdx, r10
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5
    jmp     L__sc_piby4      

L__sc_reduce:    

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
    cmp     r10, QWORD PTR L__real_411E848000000000
    jae     Lsincosf_reduce_precise

    movsd   xmm2, xmm0
    movsd   xmm4, xmm0

    mulsd   xmm2, QWORD PTR L__real_3fe45f306dc9c883  ; twobypi
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5 

;/* How many pi/2 is x a multiple of? */
;      xexp  = ax >> EXPSHIFTBITS_DP64;
    mov     r9, r10
    shr     r9, 52                                    ; >> EXPSHIFTBITS_DP64

;        npi2  = (int)(x * twobypi + 0.5);
    addsd   xmm2, xmm5                                ; npi2

    movsd   xmm3, QWORD PTR L__real_3ff921fb54400000  ; piby2_1
    cvttpd2dq    xmm0, xmm2                           ; convert to integer 
    movsd   xmm1, QWORD PTR L__real_3dd0b4611a626331  ; piby2_1tail    
    cvtdq2pd    xmm2, xmm0                            ; and back to float.    

;      /* Subtract the multiple from x to get an extra-precision remainder */
;      rhead  = x - npi2 * piby2_1;
;      /* Subtract the multiple from x to get an extra-precision remainder */
;       rhead  = x - npi2 * piby2_1;

    mulsd   xmm3, xmm2                                ; use piby2_1
    subsd   xmm4, xmm3                                ; rhead

;      rtail  = npi2 * piby2_1tail;
    mulsd   xmm1, xmm2                                ; rtail

    movd    eax, xmm0

;      GET_BITS_DP64(rhead-rtail, uy);                ; originally only rhead
    movsd   xmm0, xmm4
    subsd   xmm0, xmm1

    movsd   xmm3, QWORD PTR L__real_3dd0b4611a600000  ; piby2_2
    movsd   xmm5, QWORD PTR L__real_3ba3198a2e037073  ; piby2_2tail
    movd    rcx, xmm0

;      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     rcx, 1                                    ; strip any sign bit
    shr     rcx, 53                                   ; >> EXPSHIFTBITS_DP64 +1
    sub     r9, rcx                                   ; expdiff

;;      if (expdiff > 15)
    cmp     r9, 15
    jle     Lexpdiff15

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

;xmm1=rtail
    movsd   xmm1, xmm5
    subsd   xmm0, xmm5

;      region = npi2 & 3;
;    and    3, eax
;    xmm0=r, xmm4=rhead, xmm1=rtail
Lexpdiff15:

;; if the input was close to a pi/2 multiple
;

    cmp     rcx, 03f2h                                ; if r  small.
    jge     L__sc_piby4                               ; use taylor series if not
    cmp     rcx, 03deh                                ; if r really small.
    jle     Lsinsmall                                 ; then sin(r) = r

    movsd   xmm2, xmm0 
    mulsd   xmm2, xmm2                                ; x^2
; use simply polynomial
;              *s = x - x*x*x*0.166666666666666666;
    movsd   xmm3, QWORD PTR L__real_3fc5555555555555     ; 
    mulsd   xmm3, xmm0                                ; * x
    mulsd   xmm3, xmm2                                ; * x^2
    subsd   xmm0, xmm3                                ; xs

;              *c = 1.0 - x*x*0.5;
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000  ; 1.0
    mulsd   xmm2, QWORD PTR L__real_3fe0000000000000  ; 0.5 *x^2
    subsd   xmm1, xmm2
    jmp     L__adjust_region

Lsinsmall:                                            ; then sin(r) = r
    movsd   xmm1, QWORD PTR L__real_3ff0000000000000  ; cos(r) is a 1 
    jmp     L__adjust_region

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
L__sc_piby4:
;  x2 = r * r;
    movsd   xmm5, QWORD PTR L__real_3fe0000000000000  ; .5 
    movsd   xmm2, xmm0
    mulsd   xmm2, xmm0                                ; x2
    shufpd  xmm2, xmm2, 0                             ; x2,x2
    movsd   xmm4, xmm2
    mulsd   xmm4, xmm4                                ; x4
    shufpd  xmm4, xmm4, 0                             ; x4,x4

;  x2m =    _mm_set1_pd (x2);
;  zc,zs = (c2 + x2 * (c3 + x2 * c4 ));
;    xs =  r + x3 * (sc1 + x2 * zs);
;    xc =  t + ( x2 * x2 * (cc1 + x2 * zc));
    movapd  xmm1, QWORD PTR Lcsarray+30h              ; c4
    movapd  xmm3, QWORD PTR Lcsarray+10h              ; c2
    mulpd   xmm1, xmm2                                ; x2c4
    mulpd   xmm3, xmm2                                ; x2c2

;  rc = 0.5 * x2;
    mulsd   xmm5, xmm2                                ; rc
    mulsd   xmm2, xmm0                                ; x3

    addpd   xmm1, QWORD PTR Lcsarray+20h              ; c3 + x2c4
    addpd   xmm3, QWORD PTR Lcsarray                  ; c1 + x2c2
    mulpd   xmm1, xmm4                                ; x4(c3 + x2c4)
    addpd   xmm1, xmm3                                ; c1 + x2c2 + x4(c3 + x2c4)

;  -t = rc-1;
    subsd   xmm5, QWORD PTR L__real_3ff0000000000000  ; 1.0  
; now we have the poly for sin in the low half, and cos in upper half
    mulsd   xmm2, xmm1                                ; x3(sin poly)
    shufpd  xmm1, xmm1, 3                             ; get cos poly to low half of register
    mulsd   xmm1, xmm4                                ; x4(cos poly)

    addsd   xmm0, xmm2                                ; sin = r+...
    subsd   xmm1, xmm5                                ; cos = poly-(-t)

L__adjust_region:                                     ; xmm0 is sin, xmm1 is cos
;      switch (region)
    mov     ecx, eax
    and     eax, 1
    jz      Lregion02
; region 1 or 3
    movsd   xmm2, xmm0                                ; swap sin,cos
    movsd   xmm0, xmm1                                ; sin = cos
    xorpd   xmm1, xmm1
    subsd   xmm1, xmm2                                ; cos = -sin
   
Lregion02:
    and     ecx, 2
    jz      Lregion23
; region 2 or 3
    movsd   xmm2, xmm0
    movsd   xmm3, xmm1
    xorpd   xmm0, xmm0
    xorpd   xmm1, xmm1
    subsd   xmm0, xmm2                                ; sin = -sin
    subsd   xmm1, xmm3                                ; cos = -cos
  
Lregion23:        
;;  if (xneg) *s = -*s ;
    cmp     rdx, r10
    jz      L__sc_cleanup
    movsd   xmm2, xmm0
    xorpd   xmm0, xmm0
    subsd   xmm0, xmm2                                ; sin = -sin

ALIGN 16
L__sc_cleanup:
    movsd   xmm3, QWORD PTR [p_temp2+rsp]             ; recover expf(x)
    mulsd   xmm1, xmm3
    mulsd   xmm0, xmm3
    
; we need exp(x)cos(y) for xmm0, exp(x)sin(y) for xmm1, swap
; and convert back to floats
    movlhps  xmm0, xmm0
    movsd    xmm0, xmm1
    cvtpd2ps xmm0, xmm0
 
 ; For Windows calling convention   
    mov     rax, QWORD PTR [save_rax+rsp]
    movsd   QWORD PTR [rax], xmm0     
    add     rsp, stack_size
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ALIGN 16
Lsincosf_reduce_precise:
;      /* Reduce abs(x) into range [-pi/4,pi/4] */
;      __amd_remainder_piby2(ax, &r, &region);

    mov     QWORD PTR [p_temp0+rsp], rdx              ; save ux for use later
    mov     QWORD PTR [p_temp1+rsp], r10              ; save ax for use later
	movd	rcx, xmm0
	lea	    rdx, QWORD PTR r[rsp]
	lea	    r8,  QWORD PTR region[rsp]
	sub	    rsp, 020h	

    call    __amd_remainder_piby2d2f

    add     rsp, 020h
    mov     rdx, QWORD PTR [p_temp0+rsp]              ; restore ux for use later
    mov     r10, QWORD PTR [p_temp1+rsp]              ; restore ax for use later    
    mov     r8d, 1                                    ; for determining region later on
	movsd	xmm0, QWORD PTR r[rsp]		              ; r
	mov	    eax,  DWORD PTR region[rsp]	              ; region
	
    jmp     L__sc_piby4

; exception handle for sincosf  
ALIGN 16   
L__sc_naninf:
    cvtsd2ss  xmm0, xmm0                               ; convert back to floats
    lea       rdx, QWORD PTR region[rsp]               ; set up temp pointer for special call
    lea       r8,  QWORD PTR r[rsp]
    call      fname_special1
    movss	  xmm0, DWORD PTR r[rsp]
    
    movsd     xmm3, QWORD PTR [p_temp2+rsp]            ; recover expf(x)
    mov       rax, QWORD PTR [p_temp2+rsp] 
    mov       rcx, 7fffffffffffffffh               
    and       rax, rcx
    jz        LZero_Nan
	
	cvtss2sd  xmm0, xmm0
	mulsd     xmm0, xmm3
	cvtsd2ss  xmm0, xmm0

	movss     DWORD PTR [p_temp1+rsp], xmm0
	movss     DWORD PTR 4[p_temp1+rsp], xmm0
    movsd     xmm0, QWORD PTR [p_temp1+rsp]  
 
 ; For Windows calling convention   
    mov     rax, QWORD PTR [save_rax+rsp]
    movsd   QWORD PTR [rax], xmm0  
          
    add       rsp, stack_size
    ret

LZero_Nan:
	xorpd     xmm0, xmm0

 ; For Windows calling convention   
    mov     rax, QWORD PTR [save_rax+rsp]
    movsd   QWORD PTR [rax], xmm0  
        
    add       rsp, stack_size
    ret

fname endp

text ENDS

data SEGMENT READ

ALIGN 16
L__real_3ff0000000000000 DQ 03ff0000000000000h        ; 1.0
                         DQ 0                         ; for alignment
L__real_3fe0000000000000 DQ 03fe0000000000000h        ; 0.5
                         DQ 0
L__real_3fc5555555555555 DQ 03fc5555555555555h        ; 0.166666666666
                         DQ 0
L__real_3fe45f306dc9c883 DQ 03fe45f306dc9c883h        ; twobypi
                         DQ 0
L__real_3FF921FB54442D18 DQ 03FF921FB54442D18h        ; piby2
                         DQ 0
L__real_3ff921fb54400000 DQ 03ff921fb54400000h        ; piby2_1
                         DQ 0
L__real_3dd0b4611a626331 DQ 03dd0b4611a626331h        ; piby2_1tail
                         DQ 0
L__real_3dd0b4611a600000 DQ 03dd0b4611a600000h        ; piby2_2
                         DQ 0
L__real_3ba3198a2e037073 DQ 03ba3198a2e037073h        ; piby2_2tail
                         DQ 0                                        
L__real_fffffffff8000000 DQ 0fffffffff8000000h        ; mask for stripping head and tail
                         DQ 0               
L__real_8000000000000000 DQ 08000000000000000h        ; -0  or signbit
                         DQ 0
L__real_411E848000000000 DQ 0411E848000000000h        ; 5e5
                         DQ 0
L__real_7ff0000000000000 DQ 07ff0000000000000h        ; inf
                         DQ 0
L__real_7ff8000000000000 DQ 07ff8000000000000h        ; qnan
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

ALIGN 16
; these codes and the ones in the corresponding .c file have to match
L__flag_x_nan         DD 00000001
L__flag_y_zero        DD 00000002
L__flag_y_inf         DD 00000003

ALIGN 16
L__real_inf                     DD 7f800000h
                                DD 0
                                DQ 0
L__real_ninf                    DD 0ff800000h
                                DD 0
                                DQ 0
L__real_qnanbit                 DD 00400000h
                                DD 0
                                DQ 0
L__real_zero                    DQ 0000000000000000h
                                DQ 0
L__real_p8192                   DQ 40c0000000000000h
                                DQ 0
L__real_m9600                   DQ 0c0c2c00000000000h
                                DQ 0
L__real_64_by_log2              DQ 40571547652b82feh ; 64/ln(2)
                                DQ 0
L__real_log2_by_64              DQ 3f862e42fefa39efh ; log2_by_64
                                DQ 0
L__real_1_by_6                  DQ 3fc5555555555555h ; 1/6
                                DQ 0
L__real_1_by_2                  DQ 3fe0000000000000h ; 1/2
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

data ENDS

END
