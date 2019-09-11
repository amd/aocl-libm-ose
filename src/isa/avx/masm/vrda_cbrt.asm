;
; vrda_cbrt.S
;
; An implementation of the cbrt libm function.
;
; Prototype:
;
;     void vrda_cbrt(int len,double *src,double *dst);
;
;   Computes the floating point double precision cube root of x.
;   Returns proper C99 values, but may not raise status flags properly.
;
;   Algorithm:
;

include fm.inc
include log_tables.inc

; local variable storage offsets if any
save_xmm6       EQU      00h
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
save_r12        EQU     0E0h
save_r13        EQU     0F0h
stack_size      EQU     108h ; We take 8 as the last nibble to allow for 
                             ; alligned data movement.
FN_PROTOTYPE_BAS64 vrda_cbrt



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
    SaveReg r12,save_r12
    SaveReg r13,save_r13
    .ENDPROLOG  
 
    xor r13,r13
    mov	rdi,rcx
    xor rcx,rcx    
    mov	rsi, rdx
    mov	rdx, r8    
    and rsi,QWORD PTR L__all_bits_set
    jz  L__exit_vrda_cbrt
    and rdx,QWORD PTR L__all_bits_set
    jz  L__exit_vrda_cbrt
    cmp edi, 0
    jle L__exit_vrda_cbrt        
ALIGN 16    
L__vrda_top:
   ;fill data in xmm0
    cmp rdi, 0
    jz L__exit_vrda_cbrt
    pxor xmm0,xmm0
    cmp rdi, 1
    jz  L__fill_single
    movupd xmm0, XMMWORD PTR [rsi + r13 *8] ;is data pointer always 16 byte aligned?
    sub rdi,2
    add r13,2
    jmp L__start_process_vrd2_cbrt
L__fill_single:
    movlpd xmm0, QWORD PTR [rsi + r13 *8]
    dec rdi
    inc r13
L__start_process_vrd2_cbrt:
    movapd xmm1,xmm0
    movapd xmm2,xmm0
    movapd xmm13 ,xmm0; for handling Nans
    movapd xmm14 ,xmm0; for handling denormals
    movapd xmm3,XMMWORD PTR L__exp_mask_64
    movapd xmm4,XMMWORD PTR L__mantissa_mask_64
    pand   xmm1 ,xmm3; xmm1 contains all the exponents
    pand   xmm2 ,xmm4; xmm2 contains all the mantissa

    psrlq    xmm1,52
    pshufd   xmm3, xmm1,0A0h ; [10 10 00 00 ] = 0A0h
    movapd   xmm12,xmm3
    pcmpeqd  xmm3 ,XMMWORD PTR L__exponent_compare; xmm3 contains the mask for Nans or Infinities
    pcmpeqd  xmm12 ,XMMWORD PTR L__zero; xmm12 contains the mask for denormals
    ;get the sign bits
    movapd   xmm5,xmm0
    pand     xmm5 ,XMMWORD PTR L__sign_bit_64;xmm5 stores the sign bit
    ;get the mask for zeroes
    lea      rax,XMMWORD PTR L__mask_for_zero
    movapd   xmm6,xmm0
    pand     xmm6,XMMWORD PTR L__sign_mask_64
    pcmpeqd  xmm6 ,XMMWORD PTR L__zero; xmm6 contains the mask for zeros
    movmskps r8,xmm6
    shl      r8,1
    movapd   xmm6, XMMWORD PTR [ rax + r8  * 8 ] ; xmm6 stores mask of zeroes
    por      xmm3 ,xmm6; xmm3 now stores the mask for  nans ,zerosand infinities

    ;;;;;;;;;;;;;;;;;;;;
    ; this block is for special denormal processing
;if 1
    pand   xmm14 ,xmm12; xmm14 stores only the denormal inputs
    pand   xmm14,XMMWORD PTR L__sign_mask_64
    por    xmm14,XMMWORD PTR L__one_mask_64
    subpd  xmm14,XMMWORD PTR L__one_mask_64
    ;have to move the modified input to xmm0
    movapd xmm15,xmm12
    pandn  xmm15,xmm0
    movapd xmm0,xmm15
    pand   xmm14,xmm12
    por    xmm0,xmm14
    
    movapd xmm15 ,xmm14
    pand   xmm15   ,xmm4; xmm15 stores the mantissa
                          ; xmm4 still stores the mantissa mask
    pand   xmm14 ,XMMWORD PTR L__exp_mask_64; xmm14 stors the exp mask
    psrlq  xmm14,52
    psubq  xmm14 ,XMMWORD PTR L__bias_constant_1022
    pand   xmm14,xmm12
    pand   xmm15,xmm12
    movapd xmm4,xmm12
    pandn  xmm12,xmm1
    pandn  xmm4,xmm2
    movapd xmm1,xmm12
    movapd xmm2,xmm4
    por    xmm1,xmm14
    por    xmm2,xmm15
;endif    
    ;end of denormal processing
    ;;;;;;;;;;;;;;;;;;;;;;;;
    
    psubq     xmm1,XMMWORD PTR L__bias_constant
    pshufd    xmm6,xmm1,0F8h      ; [11 11 10 00] = 0F8h
    cvtdq2pd  xmm6,xmm6
    mulpd     xmm6,XMMWORD PTR L__double_oneby3
    cvttpd2dq xmm7,xmm6
    cvtdq2pd xmm8,xmm7
    pshufd   xmm7,xmm7,0DCh           ;11 01 11 00 = DC
    subpd    xmm6,xmm8
    mulpd    xmm6,XMMWORD PTR L__double_three
    cvtpd2dq xmm6,xmm6
    paddd    xmm6,XMMWORD PTR L__integer_two
    pslld    xmm6  ,1; equivalent to multiply by 2 since the array 
                       ; to which we will be indexing contains both 
                       ; the head and tail values.
    pshufd    xmm6, xmm6,0DCh     ; 11 01 11 00 = DC
    paddq     xmm7 ,XMMWORD PTR L__bias_constant
    psllq     xmm7 ,52; xmm7 is the scaling factor
    movd      r9 ,xmm6; lower order index to remainder
    psrldq    xmm6,8
    movd      r10 ,xmm6; higher order index to remainder

    ;xmm0 is the modified input value from the denaormal cases
    pand      xmm0,XMMWORD PTR L__mantissa_mask_64
    por       xmm0 ,XMMWORD PTR L__zero_point_five;xmm0 = Y
    movapd    xmm6 ,xmm2
    psrlq     xmm6,43
    psrlq     xmm2,44
    por       xmm2,XMMWORD PTR L__9thbit_set
    pand      xmm6,XMMWORD PTR L__1stbit_set
    paddq     xmm2,xmm6
    pshufd    xmm6,xmm2,0F8h      ; [11 11 10 00] = 0F8h
    cvtdq2pd  xmm6,xmm6; index_f64
    psubq     xmm2,XMMWORD PTR L__integer_256
    mulpd     xmm6,XMMWORD PTR L__one_by_512
    subpd     xmm0 ,xmm6; xmm0 = f
    lea       rax,XMMWORD PTR L__INV_TAB_256
    movd      r11,xmm2
    psrldq    xmm2,8
    movlpd    xmm6, QWORD PTR [ rax + r11  * 8 ]
    movd      r12,xmm2
    movhpd    xmm6, QWORD PTR [ rax + r12  * 8 ]
    mulpd     xmm0 ,xmm6; xmm0 = r
    movapd    xmm6 ,xmm0
    mulpd     xmm6 ,xmm0; xmm6 = r^2
    movapd    xmm8 ,xmm6
    mulpd     xmm8 ,xmm8; xmm8=  r^4
    movapd    xmm9 ,xmm8
    mulpd     xmm9 ,xmm6; xmm9 = r^6
    movapd    xmm10 ,xmm6
    mulpd     xmm10 ,xmm0; xmm10 = r^3
    movapd    xmm11 ,xmm10
    mulpd     xmm11 ,xmm6; xmm11 = r^5
    mulpd     xmm9,XMMWORD PTR L__coefficients_6
    mulpd     xmm11,XMMWORD PTR L__coefficients_5
    addpd     xmm9,xmm11
    mulpd     xmm8,XMMWORD PTR L__coefficients_4
    addpd     xmm9,xmm8
    mulpd     xmm10,XMMWORD PTR L__coefficients_3
    addpd     xmm9,xmm10
    mulpd     xmm6,XMMWORD PTR L__coefficients_2
    addpd     xmm9,xmm6
    mulpd     xmm0,XMMWORD PTR L__coefficients_1
    addpd     xmm0 ,xmm9; xmm0 = coeff1 * r + coeff2 * r^2 + coeff3 * r^3 + coeff4 * r^4 + coeff5 * r^5 + coeff6 * r^6
    movapd    xmm6 ,xmm0
    shufpd    xmm0,xmm0,0 ;xmm0 contains the lower order 64 bits duplicated
    shufpd    xmm6,xmm6,3 ;xmm6 contains the higher order 64 bits duplicated
    lea       r8  ,XMMWORD PTR L__cuberoot_remainder_h_l; load both head and tail of the remainders cuberoot at once
    ;1=3,8=9
    movapd    xmm8, XMMWORD PTR[ r8 + r9  * 8 ] ; xmm8 = [cbrtRem_h.cbrtRem_t,f64.f64]
    shl       r11 ,1; multiply by 2 to index both head and tail 
                   ; correctly 
    lea       rax,XMMWORD PTR L__CBRT_F_H_L_256
    movapd    xmm2     ,XMMWORD PTR [ rax + r11  * 8 ] ; xmm2 = [cbrtF_h.cbrtF_t,f64.f64]
    movapd    xmm9     ,xmm2
    psrldq    xmm9     ,8; xmm9 = [~Dont cbrtF_h,Care.f64]
    unpcklpd  xmm9     ,xmm2; xmm9 = [cbrtF_t.cbrtF_h,f64.f64]
    mulpd     xmm2     ,xmm8; xmm2 = [(cbrtF_h.f64*cbrtRem_h.f64),(cbrtRem_t.f64*cbrtF_t.f64)]
    mulpd     xmm9     ,xmm8; xmm9 = [(cbrtRem_h.f64*cbrtF_t.f64),(cbrtRem_t.f64*cbrtF_h.f64)]
    movapd    xmm4     ,xmm9
    unpckhpd  xmm4     ,xmm4; xmm4 = [(cbrtRem_h.f64*cbrtF_t.f64),(cbrtRem_h.f64*cbrtF_t.f64)]
    addsd     xmm9     ,xmm4; xmm9 = [~Dont  ,Care((cbrtRem_h.f64*cbrtF_t.f64) + (cbrtRem_t.f64*cbrtF_h.f64))]
    addsd     xmm2     ,xmm9; xmm2 = [(cbrtF_h.f64*cbrtRem_h.f64),(((cbrtRem_t.f64*cbrtF_t.f64)+(cbrtRem_h.f64*cbrtF_t.f64) + (cbrtRem_t.f64*cbrtF_h.f64))]
                            ; xmm2 = [bT,bH]
    ; Now calculate
    ;ans.f64 = (((((z * bT)) + (bT)) + (z * bH)) + (bH));
    ;ans.f64 = ans.f64 * mf;
    ;ans.u64 = ans.u64 | sign.u64;
    
                              ; also xmm0 = [z,z] = the polynomial which was computed earlier
    mulpd    xmm0      ,xmm2; xmm0 = [(bH*z),(bT*z)]
    movapd   xmm4      ,xmm0
    unpckhpd xmm4      ,xmm4; xmm4 = [(bH*z),(bH*z)]
    addsd    xmm0      ,xmm2; xmm0 = [~ ,DontCare((bT*z) + bT)]
    unpckhpd xmm2      ,xmm2; xmm2 = [(bH),(bH)]
    addsd    xmm0      ,xmm4; xmm0 = [~ ,DontCare(((bT*z) + bT) + ( z*bH))]
    addsd    xmm0      ,xmm2; xmm0 = [~ ,DontCare((((bT*z) + bT) + (z*bH)) + bH)] = [~Dont ans,Care.f64]
    ; the above dekker multiplication has to be done for the second vector input
    ; r8 is already loaded above
    movapd   xmm8      ,XMMWORD PTR [ r8 + r10  * 8 ] ; xmm8 = [cbrtRem_h.cbrtRem_t,f64.f64]
    shl      r12,1
    ;rax is already loaded above 
    movapd   xmm2     ,XMMWORD PTR [ rax + r12  * 8 ] ; xmm2 = [cbrtF_h.cbrtF_t,f64.f64]
    movapd   xmm9     ,xmm2
    psrldq   xmm9     ,8; xmm9 = [~Dont cbrtF_h,Care.f64]
    unpcklpd xmm9     ,xmm2; xmm9 = [cbrtF_t.cbrtF_h,f64.f64]
    mulpd    xmm2     ,xmm8; xmm2 = [(cbrtF_h.f64*cbrtRem_h.f64),(cbrtRem_t.f64*cbrtF_t.f64)]
    mulpd    xmm9     ,xmm8; xmm9 = [(cbrtRem_h.f64*cbrtF_t.f64),(cbrtRem_t.f64*cbrtF_h.f64)]
    movapd   xmm4     ,xmm9
    unpckhpd xmm4     ,xmm4; xmm4 = [(cbrtRem_h.f64*cbrtF_t.f64),(cbrtRem_h.f64*cbrtF_t.f64)]
    addsd    xmm9     ,xmm4; xmm9 = [~Dont  ,Care((cbrtRem_h.f64*cbrtF_t.f64) + (cbrtRem_t.f64*cbrtF_h.f64))]
    addsd    xmm2     ,xmm9; xmm2 = [(cbrtF_h.f64*cbrtRem_h.f64),(((cbrtRem_t.f64*cbrtF_t.f64)+(cbrtRem_h.f64*cbrtF_t.f64) + (cbrtRem_t.f64*cbrtF_h.f64))]
                           ; xmm2 = [bT,bH]
    ; Now calculate
    ;ans.f64 = (((((z * bT)) + (bT)) + (z * bH)) + (bH));
    ;ans.f64 = ans.f64 * mf;
    ;ans.u64 = ans.u64 | sign.u64;
    
    ; also xmm6 = [z,z] = the polynomial which was computed earlier
    mulpd    xmm6      ,xmm2; xmm6 = [(bH*z),(bT*z)]
    movapd   xmm4      ,xmm6
    unpckhpd xmm4      ,xmm4; xmm4 = [(bH*z),(bH*z)]
    addsd    xmm6      ,xmm2; xmm6 = [~ ,DontCare((bT*z) + bT)]
    unpckhpd xmm2      ,xmm2; xmm2 = [(bH),(bH)]
    addsd    xmm6      ,xmm4; xmm6 = [~ ,DontCare(((bT*z) + bT) + ( z*bH))]
    addsd    xmm6      ,xmm2; xmm6 = [~ ,DontCare((((bT*z) + bT) + (z*bH)) + bH)] = [~Dont ans,Care.f64]
   
    shufpd    xmm0     ,xmm6,0
    mulpd     xmm0     ,xmm7; xmm0 = ans.f64 * mf; mf is the scaling factor
    ;handle Nans and infinities
    addpd     xmm13    ,xmm13
    andpd     xmm13    ,xmm3
    andnpd    xmm3     ,xmm0
    orpd      xmm13    ,xmm3
    por       xmm13    ,xmm5; restore the sign
    
L__end_process_vrd2_cbrt  :  
    ;fill data back into memory buffer from the result xmm13
    movlpd QWORD PTR [rdx + rcx * 8], xmm13
    inc rcx
    cmp r13, rcx
    jz L__exit_vrda_cbrt
    movhpd QWORD PTR [rdx + rcx * 8], xmm13
    inc rcx
    jmp L__vrda_top
L__exit_vrda_cbrt:
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
    RestoreReg r12,save_r12
    RestoreReg r13,save_r13
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
      
L__all_bits_set            DQ 0FFFFFFFFFFFFFFFFh
                           DQ 0FFFFFFFFFFFFFFFFh      
L__mantissa_mask_64        DQ 0000FFFFFFFFFFFFFh
                           DQ 0000FFFFFFFFFFFFFh
L__qnan_mask_64            DQ 00008000000000000h
                           DQ 00008000000000000h
L__exp_mask_64             DQ 07FF0000000000000h
                           DQ 07FF0000000000000h
L__zero                    DQ 00000000000000000h
                           DQ 00000000000000000h
L__exponent_compare        DQ 0000007FF000007FFh  
                           DQ 0000007FF000007FFh
L__double_oneby3           DQ 03fd5555555555555h
                           DQ 03fd5555555555555h
L__double_three            DQ 04008000000000000h
                           DQ 04008000000000000h
L__integer_two             DQ 00000000200000002h
                           DQ 00000000000000000h
L__integer_256             DQ 00000000000000100h
                           DQ 00000000000000100h
L__zero_point_five         DQ 03FE0000000000000h
                           DQ 03FE0000000000000h
L__9thbit_set              DQ 00000000000000100h
                           DQ 00000000000000100h 
L__1stbit_set              DQ 00000000000000001h
                           DQ 00000000000000001h 
                            
ALIGN 16
L__sign_mask_64            DQ 07FFFFFFFFFFFFFFFh 
                           DQ 07FFFFFFFFFFFFFFFh
L__sign_bit_64             DQ 08000000000000000h 
                           DQ 08000000000000000h
L__one_mask_64             DQ 03FF0000000000000h 
                           DQ 03FF0000000000000h
L__one_by_512              DQ 03f60000000000000h
                           DQ 03f60000000000000h
L__bias_constant           DQ 000000000000003FFh
                           DQ 000000000000003FFh
L__bias_constant_1022      DQ 000000000000003FEh
                           DQ 000000000000003FEh

ALIGN 16
L__denormal_factor         DQ 03F7428A2F98D728Bh 
                           DQ 03F7428A2F98D728Bh
ALIGN 16
            
L__coefficients_1          DQ 03fd5555555555555h
                           DQ 03fd5555555555555h
L__coefficients_2          DQ 0bfbc71c71c71c71ch
                           DQ 0bfbc71c71c71c71ch
L__coefficients_3          DQ 03faf9add3c0ca458h
                           DQ 03faf9add3c0ca458h
L__coefficients_4          DQ 0bfa511e8d2b3183bh
                           DQ 0bfa511e8d2b3183bh
L__coefficients_5          DQ 03f9ee7113506ac13h
                           DQ 03f9ee7113506ac13h
L__coefficients_6          DQ 0bf98090d6221a247h
                           DQ 0bf98090d6221a247h
L__coefficients_7          DQ 03f93750ad588f114h
                           DQ 03f93750ad588f114h


ALIGN 16
L__cuberoot_remainder_h_l   DQ 03e531ae515c447bbh  ; cbrt(2^-2) Low
                  DQ 03FE428A2F0000000h  ; cbrt(2^-2) High
                  DQ 03e44f5b8f20ac166h  ; cbrt(2^-1) Low
                  DQ 03FE965FEA0000000h  ; cbrt(2^-1) High
                  DQ 00000000000000000h  ; cbrt(2^0) Low
                  DQ 03FF0000000000000h  ; cbrt(2^0) High
                  DQ 03e631ae515c447bbh  ; cbrt(2^1) Low
                  DQ 03FF428A2F0000000h  ; cbrt(2^1) High
                  DQ 03e54f5b8f20ac166h  ; cbrt(2^2) Low
                  DQ 03FF965FEA0000000h  ; cbrt(2^2) High


ALIGN 16
; this was created to avoid exception being raised if we use a comisd instruction
L__mask_for_zero  DQ 00000000000000000h ;mask value = 0000
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 0001
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 0010
                  DQ 00000000000000000h
                  DQ 0FFFFFFFFFFFFFFFFh ;mask value = 0011
                  DQ 00000000000000000h 
                  DQ 00000000000000000h ;mask value = 0100
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 0101
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 0110
                  DQ 00000000000000000h
                  DQ 0FFFFFFFFFFFFFFFFh ;mask value = 0111
                  DQ 00000000000000000h 
                  DQ 00000000000000000h ;mask value = 1000
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 1001
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 1010
                  DQ 00000000000000000h
                  DQ 0FFFFFFFFFFFFFFFFh ;mask value = 1011
                  DQ 00000000000000000h
                  DQ 00000000000000000h ;mask value = 1100
                  DQ 0FFFFFFFFFFFFFFFFh
                  DQ 00000000000000000h ;mask value = 1101
                  DQ 0FFFFFFFFFFFFFFFFh
                  DQ 00000000000000000h ;mask value = 1110
                  DQ 0FFFFFFFFFFFFFFFFh
                  DQ 0FFFFFFFFFFFFFFFFh ;mask value = 1111
                  DQ 0FFFFFFFFFFFFFFFFh
                            

;interleaved high and low values
ALIGN 16
L__CBRT_F_H_L_256 DQ 00000000000000000h							
                  DQ 03ff0000000000000h
                  DQ 03e6e6a24c81e4294h
                  DQ 03ff0055380000000h
	              DQ 03e58548511e3a785h
                  DQ 03ff00aa390000000h
	              DQ 03e64eb9336ec07f6h
                  DQ 03ff00ff010000000h
                  DQ 03e40ea64b8b750e1h
                  DQ 03ff0153920000000h
                  DQ 03e461637cff8a53ch
                  DQ 03ff01a7eb0000000h
                  DQ 03e40733bf7bd1943h
                  DQ 03ff01fc0d0000000h
                  DQ 03e5666911345ccedh
                  DQ 03ff024ff80000000h
                  DQ 03e477b7a3f592f14h                            
                  DQ 03ff02a3ad0000000h
                  DQ 03e6f18d3dd1a5402h                            
                  DQ 03ff02f72b0000000h
                  DQ 03e2be2f5a58ee9a4h                            
                  DQ 03ff034a750000000h
                  DQ 03e68901f8f085fa7h                            
                  DQ 03ff039d880000000h
                  DQ 03e5c68b8cd5b5d69h                            
                  DQ 03ff03f0670000000h
                  DQ 03e5a6b0e8624be42h                            
                  DQ 03ff0443110000000h
                  DQ 03dbc4b22b06f68e7h                            
                  DQ 03ff0495870000000h
                  DQ 03e60f3f0afcabe9bh                            
                  DQ 03ff04e7c80000000h
                  DQ 03e548495bca4e1b7h                            
                  DQ 03ff0539d60000000h
                  DQ 03e66107f1abdfdc3h                            
                  DQ 03ff058bb00000000h
                  DQ 03e6e67261878288ah                            
                  DQ 03ff05dd570000000h
                  DQ 03e5a6bc155286f1eh                            
                  DQ 03ff062ecc0000000h
                  DQ 03e58a759c64a85f2h                            
                  DQ 03ff06800e0000000h
                  DQ 03e45fce70a4a8d09h                            
                  DQ 03ff06d11e0000000h
                  DQ 03e32f9cbf373fe1dh                            
                  DQ 03ff0721fc0000000h
                  DQ 03e590564ce4ac359h                            
                  DQ 03ff0772a80000000h
                  DQ 03e5ac29ce761b02fh                            
                  DQ 03ff07c3230000000h
                  DQ 03e5cb752f497381ch                            
                  DQ 03ff08136d0000000h
                  DQ 03e68bb9e1cfb35e0h                            
                  DQ 03ff0863860000000h
                  DQ 03e65b4917099de90h                            
                  DQ 03ff08b36f0000000h
                  DQ 03e5cc77ac9c65ef2h                            
                  DQ 03ff0903280000000h
                  DQ 03e57a0f3e7be3dbah                            
                  DQ 03ff0952b10000000h
                  DQ 03e66ec851ee0c16fh                            
                  DQ 03ff09a20a0000000h
                  DQ 03e689449bf2946dah                            
                  DQ 03ff09f1340000000h
                  DQ 03e698f25301ba223h                            
                  DQ 03ff0a402f0000000h
                  DQ 03e347d5ec651f549h                            
                  DQ 03ff0a8efc0000000h
                  DQ 03e6c33ec9a86007ah                            
                  DQ 03ff0add990000000h
                  DQ 03e5e0b6653e92649h                            
                  DQ 03ff0b2c090000000h
                  DQ 03e3bd64ac09d755fh                            
                  DQ 03ff0b7a4b0000000h
                  DQ 03e2f537506f78167h                            
                  DQ 03ff0bc85f0000000h
                  DQ 03e62c382d1b3735eh                            
                  DQ 03ff0c16450000000h
                  DQ 03e6e20ed659f99e1h                            
                  DQ 03ff0c63fe0000000h
                  DQ 03e586b633a9c182ah                            
                  DQ 03ff0cb18b0000000h
                  DQ 03e445cfd5a65e777h                            
                  DQ 03ff0cfeeb0000000h
                  DQ 03e60c8770f58bca4h                            
                  DQ 03ff0d4c1e0000000h
                  DQ 03e6739e44b0933c5h                            
                  DQ 03ff0d99250000000h
                  DQ 03e027dc3d9ce7bd8h                            
                  DQ 03ff0de6010000000h
                  DQ 03e63c53c7c5a7b64h                            
                  DQ 03ff0e32b00000000h
                  DQ 03e69669683830cech                            
                  DQ 03ff0e7f340000000h
                  DQ 03e68d772c39bdcc4h                            
                  DQ 03ff0ecb8d0000000h
                  DQ 03e69b0008bcf6d7bh                            
                  DQ 03ff0f17bb0000000h
                  DQ 03e3bbb305825ce4fh                            
                  DQ 03ff0f63bf0000000h
                  DQ 03e6da3f4af13a406h                            
                  DQ 03ff0faf970000000h
                  DQ 03e5f36b96f74ce86h                            
                  DQ 03ff0ffb460000000h
                  DQ 03e165c002303f790h                            
                  DQ 03ff1046cb0000000h
                  DQ 03e682f84095ba7d5h                            
                  DQ 03ff1092250000000h
                  DQ 03e6d46433541b2c6h                            
                  DQ 03ff10dd560000000h
                  DQ 03e671c3d56e93a89h                            
                  DQ 03ff11285e0000000h
                  DQ 03e598dcef4e40012h                            
                  DQ 03ff11733d0000000h
                  DQ 03e4530ebef17fe03h                            
                  DQ 03ff11bdf30000000h
                  DQ 03e4e8b8fa3715066h                            
                  DQ 03ff1208800000000h
                  DQ 03e6ab26eb3b211dch                            
                  DQ 03ff1252e40000000h
                  DQ 03e454dd4dc906307h                            
                  DQ 03ff129d210000000h
                  DQ 03e5c9f962387984eh                            
                  DQ 03ff12e7350000000h
                  DQ 03e6c62a959afec09h                            
                  DQ 03ff1331210000000h
                  DQ 03e6638d9ac6a866ah
                  DQ 03ff137ae60000000h
                  DQ 03e338704eca8a22dh                            
                  DQ 03ff13c4840000000h
                  DQ 03e4e6c9e1db14f8fh                            
                  DQ 03ff140dfa0000000h
                  DQ 03e58744b7f9c9eaah                            
                  DQ 03ff1457490000000h
                  DQ 03e66c2893486373bh                            
                  DQ 03ff14a0710000000h
                  DQ 03e5b36bce31699b7h                            
                  DQ 03ff14e9730000000h
                  DQ 03e671e3813d200c7h                            
                  DQ 03ff15324e0000000h
                  DQ 03e699755ab40aa88h                            
                  DQ 03ff157b030000000h
                  DQ 03e6b45ca0e4bcfc0h                            
                  DQ 03ff15c3920000000h
                  DQ 03e32dd090d869c5dh                            
                  DQ 03ff160bfc0000000h
                  DQ 03e64fe0516b917dah
                  DQ 03ff16543f0000000h
                  DQ 03e694563226317a2h                            
                  DQ 03ff169c5d0000000h
                  DQ 03e653d8fafc2c851h                            
                  DQ 03ff16e4560000000h
                  DQ 03e5dcbd41fbd41a3h                            
                  DQ 03ff172c2a0000000h
                  DQ 03e5862ff5285f59ch                            
                  DQ 03ff1773d90000000h
                  DQ 03e63072ea97a1e1ch                            
                  DQ 03ff17bb630000000h
                  DQ 03e52839075184805h                            
                  DQ 03ff1802c90000000h
                  DQ 03e64b0323e9eff42h                            
                  DQ 03ff184a0a0000000h
                  DQ 03e6b158893c45484h                            
                  DQ 03ff1891270000000h
                  DQ 03e3149ef0fc35826h                            
                  DQ 03ff18d8210000000h
                  DQ 03e5f2e77ea96acaah                            
                  DQ 03ff191ef60000000h
                  DQ 03e5200074c471a95h                            
                  DQ 03ff1965a80000000h
                  DQ 03e63f8cc517f6f04h                            
                  DQ 03ff19ac360000000h
                  DQ 03e660ba2e311bb55h                            
                  DQ 03ff19f2a10000000h
                  DQ 03e64b788730bbec3h                            
                  DQ 03ff1a38e90000000h
                  DQ 03e657090795ee20ch                            
                  DQ 03ff1a7f0e0000000h
                  DQ 03e6d9ffe983670b1h                            
                  DQ 03ff1ac5100000000h
                  DQ 03e62a463ff61bfdah                            
                  DQ 03ff1b0af00000000h
                  DQ 03e69d1bc6a5e65cfh                            
                  DQ 03ff1b50ad0000000h
                  DQ 03e68718abaa9e922h                            
                  DQ 03ff1b96480000000h
                  DQ 03e63c2f52ffa342eh                            
                  DQ 03ff1bdbc10000000h
                  DQ 03e60fae13ff42c80h                            
                  DQ 03ff1c21180000000h
                  DQ 03e65440f0ef00d57h                            
                  DQ 03ff1c664d0000000h
                  DQ 03e46fcd22d4e3c1eh                            
                  DQ 03ff1cab610000000h
                  DQ 03e4e0c60b409e863h                            
                  DQ 03ff1cf0530000000h
                  DQ 03e6f9cab5a5f0333h                            
                  DQ 03ff1d35230000000h
                  DQ 03e630f24744c333dh                            
                  DQ 03ff1d79d30000000h
                  DQ 03e4b50622a76b2feh                            
                  DQ 03ff1dbe620000000h
                  DQ 03e6fdb94ba595375h                            
                  DQ 03ff1e02cf0000000h
                  DQ 03e3861b9b945a171h                            
                  DQ 03ff1e471d0000000h
                  DQ 03e654348015188c4h                            
                  DQ 03ff1e8b490000000h
                  DQ 03e6b54d149865523h                            
                  DQ 03ff1ecf550000000h
                  DQ 03e6a0bb783d9de33h                            
                  DQ 03ff1f13410000000h
                  DQ 03e6629d12b1a2157h                            
                  DQ 03ff1f570d0000000h
                  DQ 03e6467fe35d179dfh                            
                  DQ 03ff1f9ab90000000h
                  DQ 03e69763f3e26c8f7h                            
                  DQ 03ff1fde450000000h
                  DQ 03e53f798bb9f7679h                            
                  DQ 03ff2021b20000000h
                  DQ 03e552e577e855898h                            
                  DQ 03ff2064ff0000000h
                  DQ 03e6fde47e5502c3ah                            
                  DQ 03ff20a82c0000000h
                  DQ 03e5cbd0b548d96a0h                            
                  DQ 03ff20eb3b0000000h
                  DQ 03e6a9cd9f7be8de8h                            
                  DQ 03ff212e2a0000000h
                  DQ 03e522bbe704886deh                            
                  DQ 03ff2170fb0000000h
                  DQ 03e6e3dea8317f020h                            
                  DQ 03ff21b3ac0000000h
                  DQ 03e6e812085ac8855h                            
                  DQ 03ff21f63f0000000h
                  DQ 03e5c87144f24cb07h                            
                  DQ 03ff2238b40000000h
                  DQ 03e61e128ee311fa2h                            
                  DQ 03ff227b0a0000000h
                  DQ 03e5b5c163d61a2d3h                            
                  DQ 03ff22bd420000000h
                  DQ 03e47d97e7fb90633h                            
                  DQ 03ff22ff5c0000000h
                  DQ 03e6efe899d50f6a7h                            
                  DQ 03ff2341570000000h
                  DQ 03e6d0333eb75de5ah                            
                  DQ 03ff2383350000000h
                  DQ 03e40e590be73a573h                            
                  DQ 03ff23c4f60000000h
                  DQ 03e68ce8dcac3cdd2h                            
                  DQ 03ff2406980000000h
                  DQ 03e6ee8a48954064bh                            
                  DQ 03ff24481d0000000h
                  DQ 03e6aa62f18461e09h                            
                  DQ 03ff2489850000000h
                  DQ 03e601e5940986a15h                            
                  DQ 03ff24cad00000000h
                  DQ 03e3b082f4f9b8d4ch                            
                  DQ 03ff250bfe0000000h
                  DQ 03e6876e0e5527f5ah                            
                  DQ 03ff254d0e0000000h
                  DQ 03e63617080831e6bh                            
                  DQ 03ff258e020000000h
                  DQ 03e681b26e34aa4a2h                            
                  DQ 03ff25ced90000000h
                  DQ 03e552ee66dfab0c1h                            
                  DQ 03ff260f940000000h
                  DQ 03e5d85a5329e8819h                            
                  DQ 03ff2650320000000h
                  DQ 03e5105c1b646b5d1h                            
                  DQ 03ff2690b40000000h
                  DQ 03e6bb6690c1a379ch                            
                  DQ 03ff26d1190000000h
                  DQ 03e586aeba73ce3a9h                            
                  DQ 03ff2711630000000h
                  DQ 03e6dd16198294dd4h                            
                  DQ 03ff2751900000000h
                  DQ 03e6454e675775e83h                            
                  DQ 03ff2791a20000000h
                  DQ 03e63842e026197eah                            
                  DQ 03ff27d1980000000h
                  DQ 03e6f1ce0e70c44d2h                            
                  DQ 03ff2811720000000h
                  DQ 03e6ad636441a5627h                            
                  DQ 03ff2851310000000h
                  DQ 03e54c205d7212abbh                            
                  DQ 03ff2890d50000000h
                  DQ 03e6167c86c116419h                            
                  DQ 03ff28d05d0000000h
                  DQ 03e638ec3ef16e294h                            
                  DQ 03ff290fca0000000h
                  DQ 03e6473fceace9321h                            
                  DQ 03ff294f1c0000000h
                  DQ 03e67af53a836dba7h                            
                  DQ 03ff298e530000000h
                  DQ 03e1a51f3c383b652h                            
                  DQ 03ff29cd700000000h
                  DQ 03e63696da190822dh                            
                  DQ 03ff2a0c710000000h
                  DQ 03e62f9adec77074bh                            
                  DQ 03ff2a4b580000000h
                  DQ 03e38190fd5bee55fh                            
                  DQ 03ff2a8a250000000h
                  DQ 03e4bfee8fac68e55h                            
                  DQ 03ff2ac8d70000000h
                  DQ 03e331c9d6bc5f68ah                            
                  DQ 03ff2b076f0000000h
                  DQ 03e689d0523737edfh                            
                  DQ 03ff2b45ec0000000h
                  DQ 03e5a295943bf47bbh                            
                  DQ 03ff2b84500000000h
                  DQ 03e396be32e5b3207h                            
                  DQ 03ff2bc29a0000000h
                  DQ 03e6e44c7d909fa0eh                            
                  DQ 03ff2c00c90000000h
                  DQ 03e2b2505da94d9eah                            
                  DQ 03ff2c3ee00000000h
                  DQ 03e60c851f46c9c98h                            
                  DQ 03ff2c7cdc0000000h
                  DQ 03e5da71f7d9aa3b7h                            
                  DQ 03ff2cbabf0000000h
                  DQ 03e6f1b605d019ef1h                            
                  DQ 03ff2cf8880000000h
                  DQ 03e4386e8a2189563h                            
                  DQ 03ff2d36390000000h
                  DQ 03e3b19fa5d306ba7h                            
                  DQ 03ff2d73d00000000h
                  DQ 03e6dd749b67aef76h                            
                  DQ 03ff2db14d0000000h
                  DQ 03e676ff6f1dc04b0h                            
                  DQ 03ff2deeb20000000h
                  DQ 03e635a33d0b232a6h                            
                  DQ 03ff2e2bfe0000000h
                  DQ 03e64bdc80024a4e1h                            
                  DQ 03ff2e69310000000h
                  DQ 03e6ebd61770fd723h                            
                  DQ 03ff2ea64b0000000h
                  DQ 03e64769fc537264dh                            
                  DQ 03ff2ee34d0000000h
                  DQ 03e69021f429f3b98h                            
                  DQ 03ff2f20360000000h
                  DQ 03e5ee7083efbd606h                            
                  DQ 03ff2f5d070000000h
                  DQ 03e6ad985552a6b1ah                            
                  DQ 03ff2f99bf0000000h
                  DQ 03e6e3df778772160h                            
                  DQ 03ff2fd65f0000000h
                  DQ 03e6ca5d76ddc9b34h                            
                  DQ 03ff3012e70000000h
                  DQ 03e691154ffdbaf74h                            
                  DQ 03ff304f570000000h
                  DQ 03e667bdd57fb306ah                            
                  DQ 03ff308baf0000000h
                  DQ 03e67dc255ac40886h                            
                  DQ 03ff30c7ef0000000h
                  DQ 03df219f38e8afafeh                            
                  DQ 03ff3104180000000h
                  DQ 03e62416bf9669a04h                            
                  DQ 03ff3140280000000h
                  DQ 03e611c96b2b3987fh                            
                  DQ 03ff317c210000000h
                  DQ 03e6f99ed447e1177h                            
                  DQ 03ff31b8020000000h
                  DQ 03e13245826328a11h                            
                  DQ 03ff31f3cd0000000h
                  DQ 03e66f56dd1e645f8h                            
                  DQ 03ff322f7f0000000h
                  DQ 03e46164946945535h                            
                  DQ 03ff326b1b0000000h
                  DQ 03e5e37d59d190028h                            
                  DQ 03ff32a69f0000000h
                  DQ 03e668671f12bf828h                            
                  DQ 03ff32e20c0000000h
                  DQ 03e6e8ecbca6aabbdh                            
                  DQ 03ff331d620000000h
                  DQ 03e53f49e109a5912h                            
                  DQ 03ff3358a20000000h
                  DQ 03e6b8a0e11ec3043h                            
                  DQ 03ff3393ca0000000h
                  DQ 03e65fae00aed691ah                            
                  DQ 03ff33cedc0000000h
                  DQ 03e6c0569bece3e4ah                            
                  DQ 03ff3409d70000000h
                  DQ 03e605e26744efbfeh                            
                  DQ 03ff3444bc0000000h
                  DQ 03e65b570a94be5c5h                            
                  DQ 03ff347f8a0000000h
                  DQ 03e5d6f156ea0e063h                            
                  DQ 03ff34ba420000000h
                  DQ 03e6e0ca7612fc484h                            
                  DQ 03ff34f4e30000000h
                  DQ 03e4963c927b25258h                            
                  DQ 03ff352f6f0000000h
                  DQ 03e547930aa725a5ch                            
                  DQ 03ff3569e40000000h
                  DQ 03e58a79fe3af43b3h                            
                  DQ 03ff35a4430000000h
                  DQ 03e5e6dc29c41bdafh                            
                  DQ 03ff35de8c0000000h
                  DQ 03e657a2e76f863a5h                            
                  DQ 03ff3618bf0000000h
                  DQ 03e2ae3b61716354dh                            
                  DQ 03ff3652dd0000000h
                  DQ 03e665fb5df6906b1h                            
                  DQ 03ff368ce40000000h
                  DQ 03e66177d7f588f7bh                            
                  DQ 03ff36c6d60000000h
                  DQ 03e3ad55abd091b67h                            
                  DQ 03ff3700b30000000h
                  DQ 03e155337b2422d76h                            
                  DQ 03ff373a7a0000000h
                  DQ 03e6084ebe86972d5h                            
                  DQ 03ff37742b0000000h
                  DQ 03e656395808e1ea3h                            
                  DQ 03ff37adc70000000h
                  DQ 03e61bce21b40fba7h                            
                  DQ 03ff37e74e0000000h
                  DQ 03e5006f94605b515h                            
                  DQ 03ff3820c00000000h
                  DQ 03e6aa676aceb1f7dh                            
                  DQ 03ff385a1c0000000h
                  DQ 03e58229f76554ce6h                            
                  DQ 03ff3893640000000h
                  DQ 03e6eabfc6cf57330h                            
                  DQ 03ff38cc960000000h
                  DQ 03e64daed9c0ce8bch                            
                  DQ 03ff3905b40000000h
                  DQ 03e60ff1768237141h                            
                  DQ 03ff393ebd0000000h
                  DQ 03e6575f83051b085h                            
                  DQ 03ff3977b10000000h
                  DQ 03e42667deb523e29h                            
                  DQ 03ff39b0910000000h
                  DQ 03e1816996954f4fdh                            
                  DQ 03ff39e95c0000000h
                  DQ 03e587cfccf4d9cd4h                            
                  DQ 03ff3a22120000000h
                  DQ 03e52c5d018198353h                            
                  DQ 03ff3a5ab40000000h
                  DQ 03e6a7a898dcc34aah                            
                  DQ 03ff3a93410000000h
                  DQ 03e2cead6dadc36d1h                            
                  DQ 03ff3acbbb0000000h
                  DQ 03e2a55759c498bdfh                            
                  DQ 03ff3b04200000000h
                  DQ 03e6c414a9ef6de04h                            
                  DQ 03ff3b3c700000000h
                  DQ 03e63e2108a6e58fah                            
                  DQ 03ff3b74ad0000000h
                  DQ 03e5587fd7643d77ch                            
                  DQ 03ff3bacd60000000h
                  DQ 03e3901eb1d3ff3dfh                            
                  DQ 03ff3be4eb0000000h
                  DQ 03e6f2ccd7c812fc6h                            
                  DQ 03ff3c1ceb0000000h
                  DQ 03e21c8ee70a01049h                            
                  DQ 03ff3c54d90000000h
                  DQ 03e563e8d02831eech                            
                  DQ 03ff3c8cb20000000h
                  DQ 03e6f61a42a92c7ffh
                  DQ 03ff3cc4770000000h
                  DQ 03dda917399c84d24h
                  DQ 03ff3cfc2a0000000h
                  DQ 03e5e9197c8eec2f0h
                  DQ 03ff3d33c80000000h
                  DQ 03e5e6f842f5a1378h
                  DQ 03ff3d6b530000000h
                  DQ 03e2fac242a90a0fch    
                  DQ 03ff3da2cb0000000h
                  DQ 03e535ed726610227h
                  DQ 03ff3dda2f0000000h
                  DQ 03e50e0d64804b15bh                            
                  DQ 03ff3e11800000000h
                  DQ 03e0560675daba814h
                  DQ 03ff3e48be0000000h
                  DQ 03e637388c8768032h
                  DQ 03ff3e7fe80000000h
                  DQ 03e3ee3c89f9e01f5h
                  DQ 03ff3eb7000000000h
                  DQ 03e639f6f0d09747ch
                  DQ 03ff3eee040000000h
                  DQ 03e4322c327abb8f0h
                  DQ 03ff3f24f60000000h
                  DQ 03e6961b347c8ac80h
                  DQ 03ff3f5bd40000000h
                  DQ 03e63711fbbd0f118h
                  DQ 03ff3f92a00000000h
                  DQ 03e64fad8d7718ffbh
                  DQ 03ff3fc9590000000h
                  DQ 03e6fffffffffffffh    
                  DQ 03ff3fffff0000000h
                  DQ 03e667efa79ec35b4h
                  DQ 03ff4036930000000h
                  DQ 03e6a737687a254a8h
                  DQ 03ff406d140000000h
                  DQ 03e5bace0f87d924dh
                  DQ 03ff40a3830000000h
                  DQ 03e629e37c237e392h
                  DQ 03ff40d9df0000000h
                  DQ 03e557ce7ac3f3012h
                  DQ 03ff4110290000000h
                  DQ 03e682829359f8fbdh    
                  DQ 03ff4146600000000h
                  DQ 03e6cc9be42d14676h    
                  DQ 03ff417c850000000h
                  DQ 03e6a8f001c137d0bh    
                  DQ 03ff41b2980000000h
                  DQ 03e636127687dda05h    
                  DQ 03ff41e8990000000h
                  DQ 03e524dba322646f0h
                  DQ 03ff421e880000000h
                  DQ 03e6dc43f1ed210b4h    
                  DQ 03ff4254640000000h
                  DQ 03e631ae515c447bbh
                  DQ 03ff428a2f0000000h
                         

ALIGN 16
L__CBRT_F_H_256    DQ 03ff0000000000000h
					DQ 03ff0055380000000h
					DQ 03ff00aa390000000h
					DQ 03ff00ff010000000h
					DQ 03ff0153920000000h
					DQ 03ff01a7eb0000000h
					DQ 03ff01fc0d0000000h
					DQ 03ff024ff80000000h
					DQ 03ff02a3ad0000000h
					DQ 03ff02f72b0000000h
					DQ 03ff034a750000000h
					DQ 03ff039d880000000h
					DQ 03ff03f0670000000h
					DQ 03ff0443110000000h
					DQ 03ff0495870000000h
					DQ 03ff04e7c80000000h
					DQ 03ff0539d60000000h
					DQ 03ff058bb00000000h
					DQ 03ff05dd570000000h
					DQ 03ff062ecc0000000h
					DQ 03ff06800e0000000h
					DQ 03ff06d11e0000000h
					DQ 03ff0721fc0000000h
					DQ 03ff0772a80000000h
					DQ 03ff07c3230000000h
					DQ 03ff08136d0000000h
					DQ 03ff0863860000000h
					DQ 03ff08b36f0000000h
					DQ 03ff0903280000000h
					DQ 03ff0952b10000000h
					DQ 03ff09a20a0000000h
					DQ 03ff09f1340000000h
					DQ 03ff0a402f0000000h
					DQ 03ff0a8efc0000000h
					DQ 03ff0add990000000h
					DQ 03ff0b2c090000000h
					DQ 03ff0b7a4b0000000h
					DQ 03ff0bc85f0000000h
					DQ 03ff0c16450000000h
					DQ 03ff0c63fe0000000h
					DQ 03ff0cb18b0000000h
					DQ 03ff0cfeeb0000000h
					DQ 03ff0d4c1e0000000h
					DQ 03ff0d99250000000h
					DQ 03ff0de6010000000h
					DQ 03ff0e32b00000000h
					DQ 03ff0e7f340000000h
					DQ 03ff0ecb8d0000000h
					DQ 03ff0f17bb0000000h
					DQ 03ff0f63bf0000000h
					DQ 03ff0faf970000000h
					DQ 03ff0ffb460000000h
					DQ 03ff1046cb0000000h
					DQ 03ff1092250000000h
					DQ 03ff10dd560000000h
					DQ 03ff11285e0000000h
					DQ 03ff11733d0000000h
					DQ 03ff11bdf30000000h
					DQ 03ff1208800000000h
					DQ 03ff1252e40000000h
					DQ 03ff129d210000000h
					DQ 03ff12e7350000000h
					DQ 03ff1331210000000h
					DQ 03ff137ae60000000h
					DQ 03ff13c4840000000h
					DQ 03ff140dfa0000000h
					DQ 03ff1457490000000h
					DQ 03ff14a0710000000h
					DQ 03ff14e9730000000h
					DQ 03ff15324e0000000h
					DQ 03ff157b030000000h
					DQ 03ff15c3920000000h
					DQ 03ff160bfc0000000h
					DQ 03ff16543f0000000h
					DQ 03ff169c5d0000000h
					DQ 03ff16e4560000000h
					DQ 03ff172c2a0000000h
					DQ 03ff1773d90000000h
					DQ 03ff17bb630000000h
					DQ 03ff1802c90000000h
					DQ 03ff184a0a0000000h
					DQ 03ff1891270000000h
					DQ 03ff18d8210000000h
					DQ 03ff191ef60000000h
					DQ 03ff1965a80000000h
					DQ 03ff19ac360000000h
					DQ 03ff19f2a10000000h
					DQ 03ff1a38e90000000h
					DQ 03ff1a7f0e0000000h
					DQ 03ff1ac5100000000h
					DQ 03ff1b0af00000000h
					DQ 03ff1b50ad0000000h
					DQ 03ff1b96480000000h
					DQ 03ff1bdbc10000000h
					DQ 03ff1c21180000000h
					DQ 03ff1c664d0000000h
					DQ 03ff1cab610000000h
					DQ 03ff1cf0530000000h
					DQ 03ff1d35230000000h
					DQ 03ff1d79d30000000h
					DQ 03ff1dbe620000000h
					DQ 03ff1e02cf0000000h
					DQ 03ff1e471d0000000h
					DQ 03ff1e8b490000000h
					DQ 03ff1ecf550000000h
					DQ 03ff1f13410000000h
					DQ 03ff1f570d0000000h
					DQ 03ff1f9ab90000000h
					DQ 03ff1fde450000000h
					DQ 03ff2021b20000000h
					DQ 03ff2064ff0000000h
					DQ 03ff20a82c0000000h
					DQ 03ff20eb3b0000000h
					DQ 03ff212e2a0000000h
					DQ 03ff2170fb0000000h
					DQ 03ff21b3ac0000000h
					DQ 03ff21f63f0000000h
					DQ 03ff2238b40000000h
					DQ 03ff227b0a0000000h
					DQ 03ff22bd420000000h
					DQ 03ff22ff5c0000000h
					DQ 03ff2341570000000h
					DQ 03ff2383350000000h
					DQ 03ff23c4f60000000h
					DQ 03ff2406980000000h
					DQ 03ff24481d0000000h
					DQ 03ff2489850000000h
					DQ 03ff24cad00000000h
					DQ 03ff250bfe0000000h
					DQ 03ff254d0e0000000h
					DQ 03ff258e020000000h
					DQ 03ff25ced90000000h
					DQ 03ff260f940000000h
					DQ 03ff2650320000000h
					DQ 03ff2690b40000000h
					DQ 03ff26d1190000000h
					DQ 03ff2711630000000h
					DQ 03ff2751900000000h
					DQ 03ff2791a20000000h
					DQ 03ff27d1980000000h
					DQ 03ff2811720000000h
					DQ 03ff2851310000000h
					DQ 03ff2890d50000000h
					DQ 03ff28d05d0000000h
					DQ 03ff290fca0000000h
					DQ 03ff294f1c0000000h
					DQ 03ff298e530000000h
					DQ 03ff29cd700000000h
					DQ 03ff2a0c710000000h
					DQ 03ff2a4b580000000h
					DQ 03ff2a8a250000000h
					DQ 03ff2ac8d70000000h
					DQ 03ff2b076f0000000h
					DQ 03ff2b45ec0000000h
					DQ 03ff2b84500000000h
					DQ 03ff2bc29a0000000h
					DQ 03ff2c00c90000000h
					DQ 03ff2c3ee00000000h
					DQ 03ff2c7cdc0000000h
					DQ 03ff2cbabf0000000h
					DQ 03ff2cf8880000000h
					DQ 03ff2d36390000000h
					DQ 03ff2d73d00000000h
					DQ 03ff2db14d0000000h
					DQ 03ff2deeb20000000h
					DQ 03ff2e2bfe0000000h
					DQ 03ff2e69310000000h
					DQ 03ff2ea64b0000000h
					DQ 03ff2ee34d0000000h
					DQ 03ff2f20360000000h
					DQ 03ff2f5d070000000h
					DQ 03ff2f99bf0000000h
					DQ 03ff2fd65f0000000h
					DQ 03ff3012e70000000h
					DQ 03ff304f570000000h
					DQ 03ff308baf0000000h
					DQ 03ff30c7ef0000000h
					DQ 03ff3104180000000h
					DQ 03ff3140280000000h
					DQ 03ff317c210000000h
					DQ 03ff31b8020000000h
					DQ 03ff31f3cd0000000h
					DQ 03ff322f7f0000000h
					DQ 03ff326b1b0000000h
					DQ 03ff32a69f0000000h
					DQ 03ff32e20c0000000h
					DQ 03ff331d620000000h
					DQ 03ff3358a20000000h
					DQ 03ff3393ca0000000h
					DQ 03ff33cedc0000000h
					DQ 03ff3409d70000000h
					DQ 03ff3444bc0000000h
					DQ 03ff347f8a0000000h
					DQ 03ff34ba420000000h
					DQ 03ff34f4e30000000h
					DQ 03ff352f6f0000000h
					DQ 03ff3569e40000000h
					DQ 03ff35a4430000000h
					DQ 03ff35de8c0000000h
					DQ 03ff3618bf0000000h
					DQ 03ff3652dd0000000h
					DQ 03ff368ce40000000h
					DQ 03ff36c6d60000000h
					DQ 03ff3700b30000000h
					DQ 03ff373a7a0000000h
					DQ 03ff37742b0000000h
					DQ 03ff37adc70000000h
					DQ 03ff37e74e0000000h
					DQ 03ff3820c00000000h
					DQ 03ff385a1c0000000h
					DQ 03ff3893640000000h
					DQ 03ff38cc960000000h
					DQ 03ff3905b40000000h
					DQ 03ff393ebd0000000h
					DQ 03ff3977b10000000h
					DQ 03ff39b0910000000h
					DQ 03ff39e95c0000000h
					DQ 03ff3a22120000000h
					DQ 03ff3a5ab40000000h
					DQ 03ff3a93410000000h
					DQ 03ff3acbbb0000000h
					DQ 03ff3b04200000000h
					DQ 03ff3b3c700000000h
					DQ 03ff3b74ad0000000h
					DQ 03ff3bacd60000000h
					DQ 03ff3be4eb0000000h
					DQ 03ff3c1ceb0000000h
					DQ 03ff3c54d90000000h
					DQ 03ff3c8cb20000000h
					DQ 03ff3cc4770000000h
					DQ 03ff3cfc2a0000000h
					DQ 03ff3d33c80000000h
					DQ 03ff3d6b530000000h
					DQ 03ff3da2cb0000000h
					DQ 03ff3dda2f0000000h
					DQ 03ff3e11800000000h
					DQ 03ff3e48be0000000h
					DQ 03ff3e7fe80000000h
					DQ 03ff3eb7000000000h
					DQ 03ff3eee040000000h
					DQ 03ff3f24f60000000h
					DQ 03ff3f5bd40000000h
					DQ 03ff3f92a00000000h
					DQ 03ff3fc9590000000h
					DQ 03ff3fffff0000000h
					DQ 03ff4036930000000h
					DQ 03ff406d140000000h
					DQ 03ff40a3830000000h
					DQ 03ff40d9df0000000h
					DQ 03ff4110290000000h
					DQ 03ff4146600000000h
					DQ 03ff417c850000000h
					DQ 03ff41b2980000000h
					DQ 03ff41e8990000000h
					DQ 03ff421e880000000h
					DQ 03ff4254640000000h
								
ALIGN 16
L__CBRT_F_T_256 	DQ 00000000000000000h
					DQ 03e6e6a24c81e4294h
					DQ 03e58548511e3a785h
					DQ 03e64eb9336ec07f6h
					DQ 03e40ea64b8b750e1h
					DQ 03e461637cff8a53ch
					DQ 03e40733bf7bd1943h
					DQ 03e5666911345ccedh
					DQ 03e477b7a3f592f14h
					DQ 03e6f18d3dd1a5402h
					DQ 03e2be2f5a58ee9a4h
					DQ 03e68901f8f085fa7h
					DQ 03e5c68b8cd5b5d69h
					DQ 03e5a6b0e8624be42h
					DQ 03dbc4b22b06f68e7h
					DQ 03e60f3f0afcabe9bh
					DQ 03e548495bca4e1b7h
					DQ 03e66107f1abdfdc3h
					DQ 03e6e67261878288ah
					DQ 03e5a6bc155286f1eh
					DQ 03e58a759c64a85f2h
					DQ 03e45fce70a4a8d09h
					DQ 03e32f9cbf373fe1dh
					DQ 03e590564ce4ac359h
					DQ 03e5ac29ce761b02fh
					DQ 03e5cb752f497381ch
					DQ 03e68bb9e1cfb35e0h
					DQ 03e65b4917099de90h
					DQ 03e5cc77ac9c65ef2h
					DQ 03e57a0f3e7be3dbah
					DQ 03e66ec851ee0c16fh
					DQ 03e689449bf2946dah
					DQ 03e698f25301ba223h
					DQ 03e347d5ec651f549h
					DQ 03e6c33ec9a86007ah
					DQ 03e5e0b6653e92649h
					DQ 03e3bd64ac09d755fh
					DQ 03e2f537506f78167h
					DQ 03e62c382d1b3735eh
					DQ 03e6e20ed659f99e1h
					DQ 03e586b633a9c182ah
					DQ 03e445cfd5a65e777h
					DQ 03e60c8770f58bca4h
					DQ 03e6739e44b0933c5h
					DQ 03e027dc3d9ce7bd8h
					DQ 03e63c53c7c5a7b64h
					DQ 03e69669683830cech
					DQ 03e68d772c39bdcc4h
					DQ 03e69b0008bcf6d7bh
					DQ 03e3bbb305825ce4fh
					DQ 03e6da3f4af13a406h
					DQ 03e5f36b96f74ce86h
					DQ 03e165c002303f790h
					DQ 03e682f84095ba7d5h
					DQ 03e6d46433541b2c6h
					DQ 03e671c3d56e93a89h
					DQ 03e598dcef4e40012h
					DQ 03e4530ebef17fe03h
					DQ 03e4e8b8fa3715066h
					DQ 03e6ab26eb3b211dch
					DQ 03e454dd4dc906307h
					DQ 03e5c9f962387984eh
					DQ 03e6c62a959afec09h
					DQ 03e6638d9ac6a866ah
					DQ 03e338704eca8a22dh
					DQ 03e4e6c9e1db14f8fh
					DQ 03e58744b7f9c9eaah
					DQ 03e66c2893486373bh
					DQ 03e5b36bce31699b7h
					DQ 03e671e3813d200c7h
					DQ 03e699755ab40aa88h
					DQ 03e6b45ca0e4bcfc0h
					DQ 03e32dd090d869c5dh
					DQ 03e64fe0516b917dah
					DQ 03e694563226317a2h
					DQ 03e653d8fafc2c851h
					DQ 03e5dcbd41fbd41a3h
					DQ 03e5862ff5285f59ch
					DQ 03e63072ea97a1e1ch
					DQ 03e52839075184805h
					DQ 03e64b0323e9eff42h
					DQ 03e6b158893c45484h
					DQ 03e3149ef0fc35826h
					DQ 03e5f2e77ea96acaah
					DQ 03e5200074c471a95h
					DQ 03e63f8cc517f6f04h
					DQ 03e660ba2e311bb55h
					DQ 03e64b788730bbec3h
					DQ 03e657090795ee20ch
					DQ 03e6d9ffe983670b1h
					DQ 03e62a463ff61bfdah
					DQ 03e69d1bc6a5e65cfh
					DQ 03e68718abaa9e922h
					DQ 03e63c2f52ffa342eh
					DQ 03e60fae13ff42c80h
					DQ 03e65440f0ef00d57h
					DQ 03e46fcd22d4e3c1eh
					DQ 03e4e0c60b409e863h
					DQ 03e6f9cab5a5f0333h
					DQ 03e630f24744c333dh
					DQ 03e4b50622a76b2feh
					DQ 03e6fdb94ba595375h
					DQ 03e3861b9b945a171h
					DQ 03e654348015188c4h
					DQ 03e6b54d149865523h
					DQ 03e6a0bb783d9de33h
					DQ 03e6629d12b1a2157h
					DQ 03e6467fe35d179dfh
					DQ 03e69763f3e26c8f7h
					DQ 03e53f798bb9f7679h
					DQ 03e552e577e855898h
					DQ 03e6fde47e5502c3ah
					DQ 03e5cbd0b548d96a0h
					DQ 03e6a9cd9f7be8de8h
					DQ 03e522bbe704886deh
					DQ 03e6e3dea8317f020h
					DQ 03e6e812085ac8855h
					DQ 03e5c87144f24cb07h
					DQ 03e61e128ee311fa2h
					DQ 03e5b5c163d61a2d3h
					DQ 03e47d97e7fb90633h
					DQ 03e6efe899d50f6a7h
					DQ 03e6d0333eb75de5ah
					DQ 03e40e590be73a573h
					DQ 03e68ce8dcac3cdd2h
					DQ 03e6ee8a48954064bh
					DQ 03e6aa62f18461e09h
					DQ 03e601e5940986a15h
					DQ 03e3b082f4f9b8d4ch
					DQ 03e6876e0e5527f5ah
					DQ 03e63617080831e6bh
					DQ 03e681b26e34aa4a2h
					DQ 03e552ee66dfab0c1h
					DQ 03e5d85a5329e8819h
					DQ 03e5105c1b646b5d1h
					DQ 03e6bb6690c1a379ch
					DQ 03e586aeba73ce3a9h
					DQ 03e6dd16198294dd4h
					DQ 03e6454e675775e83h
					DQ 03e63842e026197eah
					DQ 03e6f1ce0e70c44d2h
					DQ 03e6ad636441a5627h
					DQ 03e54c205d7212abbh
					DQ 03e6167c86c116419h
					DQ 03e638ec3ef16e294h
					DQ 03e6473fceace9321h
					DQ 03e67af53a836dba7h
					DQ 03e1a51f3c383b652h
					DQ 03e63696da190822dh
					DQ 03e62f9adec77074bh
					DQ 03e38190fd5bee55fh
					DQ 03e4bfee8fac68e55h
					DQ 03e331c9d6bc5f68ah
					DQ 03e689d0523737edfh
					DQ 03e5a295943bf47bbh
					DQ 03e396be32e5b3207h
					DQ 03e6e44c7d909fa0eh
					DQ 03e2b2505da94d9eah
					DQ 03e60c851f46c9c98h
					DQ 03e5da71f7d9aa3b7h
					DQ 03e6f1b605d019ef1h
					DQ 03e4386e8a2189563h
					DQ 03e3b19fa5d306ba7h
					DQ 03e6dd749b67aef76h
					DQ 03e676ff6f1dc04b0h
					DQ 03e635a33d0b232a6h
					DQ 03e64bdc80024a4e1h
					DQ 03e6ebd61770fd723h
					DQ 03e64769fc537264dh
					DQ 03e69021f429f3b98h
					DQ 03e5ee7083efbd606h
					DQ 03e6ad985552a6b1ah
					DQ 03e6e3df778772160h
					DQ 03e6ca5d76ddc9b34h
					DQ 03e691154ffdbaf74h
					DQ 03e667bdd57fb306ah
					DQ 03e67dc255ac40886h
					DQ 03df219f38e8afafeh
					DQ 03e62416bf9669a04h
					DQ 03e611c96b2b3987fh
					DQ 03e6f99ed447e1177h
					DQ 03e13245826328a11h
					DQ 03e66f56dd1e645f8h
					DQ 03e46164946945535h
					DQ 03e5e37d59d190028h
					DQ 03e668671f12bf828h
					DQ 03e6e8ecbca6aabbdh
					DQ 03e53f49e109a5912h
					DQ 03e6b8a0e11ec3043h
					DQ 03e65fae00aed691ah
					DQ 03e6c0569bece3e4ah
					DQ 03e605e26744efbfeh
					DQ 03e65b570a94be5c5h
					DQ 03e5d6f156ea0e063h
					DQ 03e6e0ca7612fc484h
					DQ 03e4963c927b25258h
					DQ 03e547930aa725a5ch
					DQ 03e58a79fe3af43b3h
					DQ 03e5e6dc29c41bdafh
					DQ 03e657a2e76f863a5h
					DQ 03e2ae3b61716354dh
					DQ 03e665fb5df6906b1h
					DQ 03e66177d7f588f7bh
					DQ 03e3ad55abd091b67h
					DQ 03e155337b2422d76h
					DQ 03e6084ebe86972d5h
					DQ 03e656395808e1ea3h
					DQ 03e61bce21b40fba7h
					DQ 03e5006f94605b515h
					DQ 03e6aa676aceb1f7dh
					DQ 03e58229f76554ce6h
					DQ 03e6eabfc6cf57330h
					DQ 03e64daed9c0ce8bch
					DQ 03e60ff1768237141h
					DQ 03e6575f83051b085h
					DQ 03e42667deb523e29h
					DQ 03e1816996954f4fdh
					DQ 03e587cfccf4d9cd4h
					DQ 03e52c5d018198353h
					DQ 03e6a7a898dcc34aah
					DQ 03e2cead6dadc36d1h
					DQ 03e2a55759c498bdfh
					DQ 03e6c414a9ef6de04h
					DQ 03e63e2108a6e58fah
					DQ 03e5587fd7643d77ch
					DQ 03e3901eb1d3ff3dfh
					DQ 03e6f2ccd7c812fc6h
					DQ 03e21c8ee70a01049h
					DQ 03e563e8d02831eech
					DQ 03e6f61a42a92c7ffh
					DQ 03dda917399c84d24h
					DQ 03e5e9197c8eec2f0h
					DQ 03e5e6f842f5a1378h
					DQ 03e2fac242a90a0fch
					DQ 03e535ed726610227h
					DQ 03e50e0d64804b15bh
					DQ 03e0560675daba814h
					DQ 03e637388c8768032h
					DQ 03e3ee3c89f9e01f5h
					DQ 03e639f6f0d09747ch
					DQ 03e4322c327abb8f0h
					DQ 03e6961b347c8ac80h
					DQ 03e63711fbbd0f118h
					DQ 03e64fad8d7718ffbh
					DQ 03e6fffffffffffffh
					DQ 03e667efa79ec35b4h
					DQ 03e6a737687a254a8h
					DQ 03e5bace0f87d924dh
					DQ 03e629e37c237e392h
					DQ 03e557ce7ac3f3012h
					DQ 03e682829359f8fbdh
					DQ 03e6cc9be42d14676h
					DQ 03e6a8f001c137d0bh
					DQ 03e636127687dda05h
					DQ 03e524dba322646f0h
					DQ 03e6dc43f1ed210b4h
								
ALIGN 16
L__INV_TAB_256     DQ 04000000000000000h
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

CONST    ENDS
data ENDS

END
