;
; vrda_log2.S
;
; An implementation of the vrda_log2 libm function.
;
; Prototype:
;
;     void vrda_log2(int len,double *src,double *dst);
;
;
;   Algorithm:
;
;   Based on:
;   Ping-Tak Peter Tang
;   "Table-driven implementation of the logarithm function in IEEE
;   floating-point arithmetic"
;   ACM Transactions on Mathematical Software (TOMS)
;   Volume 16, Issue 4 (December 1990)
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
stack_size      EQU     0E8h ; We take 8 as the last nibble to allow for 
                             ; alligned data movement.

FN_PROTOTYPE_BAS64 vrda_log2


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
    ;SaveXmm xmm12, save_xmm12
    SaveXmm xmm13, save_xmm13
    SaveXmm xmm14, save_xmm14
    ;SaveXmm xmm15, save_xmm15    
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi
    
    .ENDPROLOG   
    
    mov	rdi,rcx
    mov	rsi,rdx
    mov	rdx,r8   
    
    and rsi,QWORD PTR L__all_bits_set
    jz  L__exit_vrda_log2
    and rdx,QWORD PTR L__all_bits_set
    jz  L__exit_vrda_log2
    cmp edi, 0
    jle L__exit_vrda_log2    
    
    xor r8,r8
    xor rax,rax
ALIGN 16    
L__vrda_top:
    ;fill data in xmm0
    cmp rdi, 0
    jz L__exit_vrda_log2
    pxor xmm0,xmm0
    cmp rdi, 1
    jz  L__fill_single
    movupd xmm0, QWORD PTR [rsi + r8 * 8] ;is data pointer always 16 byte aligned
    sub rdi, 2
    add r8, 2
    jmp L__start_process_vrd2_log2
L__fill_single:
    movlpd xmm0, QWORD PTR [rsi + r8 * 8]
    dec rdi
    inc r8
L__start_process_vrd2_log2:
    ; compute exponent part
    movapd       xmm1 ,xmm0
    movapd       xmm2 ,xmm0;
    movapd       xmm3 ,xmm0;
    movapd       xmm4 ,xmm0;
    movapd       xmm5 ,xmm0;
    movapd       xmm14 ,xmm0;
    ;get the mask for negative inputs
    pand        xmm1 ,XMMWORD PTR L__sign_bit_64 
    pshufd      xmm1,xmm1, 0F5h ; [11 11 01 01 ] = 0F5h
    cmpeqpd      xmm1 ,XMMWORD PTR L__sign_bit_32 ; xmm1 stors the mask for negative inputs

    ;get the mask for Nans and Infs
    pand        xmm14  ,XMMWORD PTR L__exp_mask ; remove the mantissa
    pshufd      xmm14,xmm14, 0F5h ; [11 11 01 01 ] = 0Fh
    pcmpeqd     xmm14 ,XMMWORD PTR L__exponent_compare ; xmm14 stores the mask for nans and Infs

    
    ;get the mask for +/- zero
    pand        xmm2 ,XMMWORD PTR L__sign_mask_64 
    cmpeqpd     xmm2 ,XMMWORD PTR L__Zero_64 ; xmm2 stores the mask for +/- zeros 
;;;;;;;;;;;;;;;;;clear till here;;;;;;;;;;;;;;;;;;;;;

    ;calculate log2
    cmpnltpd      xmm4,XMMWORD PTR L__real_log_thresh1 
    cmplepd       xmm3,XMMWORD PTR L__real_log_thresh2 
    pand          xmm3 ,xmm4; xmm3 stores mask for (ux >= log_thresh1 && ux <= log_thresh2)

    ;calculate outside the threshold range first
    
    ;check for IMPBIT_DP_64 and calulate f
    movapd      xmm4 ,xmm5; copy input x
    cmpltpd     xmm5 ,XMMWORD PTR L__impbit_dp64 ; mask for (ux < IMPBIT_DP64)
    movapd      xmm8 ,xmm5; copy mask
    por          xmm4 ,XMMWORD PTR L__constant1 ; PUT_BITS_DP64(ux |  f,003d0000000000000h)
    subpd       xmm4 ,XMMWORD PTR L__constant1 ; f -= corr
    pand        xmm8 ,XMMWORD PTR L__expadjust ; xmm8 = expadjust
    movapd      xmm7 ,xmm0; copy input
    ; f = xmm4/xmm7 and the mask is xmm5
    pand        xmm4,xmm5
    pandn       xmm5,xmm7
    por         xmm5,xmm4
    ; Now f = ux = xmm5 expadjust = xmm8
    
    movapd      xmm6,xmm5
    movapd      xmm4,xmm5
    movapd      xmm7,xmm5
    pand        xmm4,XMMWORD PTR L__exp_mask 
    psrlq       xmm4,52
    ;xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    psubq       xmm4,XMMWORD PTR L__mask_1023 
    psubq       xmm4 ,xmm8; subtract expadjust

    pand        xmm5,XMMWORD PTR L__mantissa_mask 
    ;PUT_BITS_DP64((ux & MANTBITS_DP64) |  f,HALFEXPBITS_DP64);
    por         xmm5 ,XMMWORD PTR L__real_half ; xmm5 = f
     
    ;index = (int)((((ux & 0000fc00000000000h) | 00010000000000000h) >> 46) + ((ux & 00000200000000000h) >> 45));
    pand        xmm6,XMMWORD PTR L__index_mask1 
    pand        xmm7,XMMWORD PTR L__index_mask2 
    por         xmm6,XMMWORD PTR L__index_mask3 
    psrlq       xmm7 ,45; ((ux & 00000200000000000h) >> 45)
    psrlq       xmm6 ,46; (((ux & 0000fc00000000000h) | 00010000000000000h) >> 46)
    paddq       xmm6 ,xmm7; index = xmm6
    
    pshufd      xmm7,xmm6,0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd    xmm7,xmm7
    mulpd       xmm7 ,XMMWORD PTR L__index_constant1 ; f1 =index*0.0078125
    psubq       xmm6 ,XMMWORD PTR L__index_constant2 ; index -= 64
    subpd       xmm5 ,xmm7; xmm5 = f2 = f - f1;
    
    ;  z1 = ln_lead_table[index];
    ;  q = ln_tail_table[index];
    movd        r10 ,xmm6; move lower order index
    psrldq      xmm6,8
    movd        r11 ,xmm6; move higher order index
    lea          r9,XMMWORD PTR L__ln_lead_64_table 
    lea          rcx,XMMWORD PTR L__ln_tail_64_table 
    movlpd      xmm6, QWORD PTR [ r9 + r10  * 8 ] 
    movhpd      xmm6, QWORD PTR [ r9 + r11  * 8 ] ; xmm6 = z1 = ln_lead_table[index]
    movlpd      xmm8, QWORD PTR [ rcx + r10  * 8 ] 
    movhpd      xmm8, QWORD PTR [ rcx + r11  * 8 ] ; xmm8 = q = ln_tail_table[index]
    ; z1 = xmm6 q = xmm8 f2 = xmm5 f1 = xmm7 

    movapd      xmm13,xmm5
    mulpd       xmm13 ,XMMWORD PTR L__real_half ; (0.5 * f2)
    addpd       xmm7 ,xmm13;  (f1 + 0.5 * f2);
    divpd       xmm5  ,xmm7; u = f2 / (f1 + 0.5 * f2);
    movapd      xmm7  ,xmm5; xmm7 = u
    mulpd       xmm5  ,xmm5; v = u*u
    
    ;z1 = xmm6 q = xmm8 u = xmm7 v = xmm5
    movapd      xmm11 ,xmm5; v 
    mulpd       xmm5 ,XMMWORD PTR L__real_cb_3 ; v*cb_3
    addpd       xmm5 ,XMMWORD PTR L__real_cb_2 ; cb_2 + v*cb_3
    mulpd       xmm5  ,xmm11; v * (cb_2 + v * cb_3)
    addpd       xmm5 ,XMMWORD PTR L__real_cb_1 ; (cb_1 + v * (cb_2 + v * cb_3))
    mulpd       xmm5 ,xmm11; poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    ;poly = xmm5 u = xmm7 q = xmm8
    mulpd       xmm5 ,xmm7
    addpd       xmm5,xmm7
    addpd       xmm5   ,xmm8; z2 = q + (u + u * poly)
    ;;;;;;;till here common to log10,log2,log
    pshufd      xmm4, xmm4,0F8h ; [11 11 10 00] = 0F8h
    cvtdq2pd    xmm4 ,xmm4; xexp (float) 
    movapd      xmm8 ,xmm4; xmm8 = xexp
    mulpd        xmm4,XMMWORD PTR L__real_log2_lead 
    addpd       xmm4 ,xmm6; r1 = (xexp * log2_lead + z1)
    mulpd        xmm8,XMMWORD PTR L__real_log2_tail 
    addpd       xmm8 ,xmm5; r2 = (xexp * log2_tail + z2)
    ;xmm8 = r2 xmm4 = r1 xmm6 =z1 xmm5 = z2
    movapd      xmm9 ,xmm4; copy r1
    movapd      xmm10 ,xmm8; copy r2
    mulpd       xmm9 ,XMMWORD PTR L__real_log2e_lead ; (log10e_lead*r1)
    mulpd       xmm10 ,XMMWORD PTR L__real_log2e_lead ;(log10e_lead*r2)
    mulpd       xmm4 ,XMMWORD PTR L__real_log2e_tail ; (log10e_tail*r1)
    mulpd       xmm8 ,XMMWORD PTR L__real_log2e_tail ; (log10e_tail*r2)
    addpd       xmm8  ,xmm4; (log10e_tail*r1) + (log10e_tail*r2)
    addpd       xmm8 ,xmm10; (log10e_lead*r1) + (log10e_lead*r2)
    addpd       xmm8 ,xmm9; result = (((log10e_tail*r2) + log10e_tail*r1) + log10e_lead*r2) + log10e_lead*r1;
    ;;;;END of calculating outside the threshold
    
    ;;;;Now calculate when the value lies within the threshold
    ;xmm0 still contains the input x
    movapd     xmm4 ,xmm0; xmm4 = r1 
    subpd       xmm4 ,XMMWORD PTR L__one_mask_64 ; r = x - 1.0
    movapd     xmm5 ,xmm4; 
    movapd     xmm6 ,xmm4; 
    movapd     xmm7 ,xmm4; 
    addpd      xmm5 ,XMMWORD PTR L__plus_two ; (2.0 + r)
    divpd      xmm6 ,xmm5; xmm6 = u = r / (2.0 + r); 
    mulpd      xmm7 ,xmm6; correction = r * u
    addpd      xmm6 ,xmm6; u = u+ u
    movapd     xmm9 ,xmm6
    mulpd      xmm9 ,xmm9; v = u * u
    
    ;r1 = xmm4 v =  u ,xmm9=  correction ,xmm6= xmm7
    movapd     xmm10 ,xmm9; xmm10 = v
    mulpd      xmm10 ,XMMWORD PTR L__real_ca_4 ; (v * ca_4)
    addpd      xmm10 ,XMMWORD PTR L__real_ca_3 ; (ca_3 + (v * ca_4))
    mulpd      xmm10 ,xmm9; v * (ca_3 + v * ca_4)
    addpd      xmm10 ,XMMWORD PTR L__real_ca_2 ; (ca_2 + v * (ca_3 + v * ca_4))
    mulpd      xmm10 ,xmm9; v * (ca_2 + v * (ca_3 + v * ca_4))
    addpd      xmm10 ,XMMWORD PTR L__real_ca_1 ; (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      xmm10 ,xmm9; v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    mulpd      xmm10 ,xmm6; u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    subpd      xmm10  ,xmm7; r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction)
    ;;;;; till here common to all log10,log,log2
    ; r1 = xmm4 r2 = xmm10;
    movapd    xmm7 ,xmm4; xmm7 = r
    pand      xmm4 ,XMMWORD PTR L__mask32bits_DP64 ; r1 = r1 & 0ffffffff00000000h
    subpd     xmm7 ,xmm4; (r-r1)
    addpd      xmm10 ,xmm7; r2 = r2 + (r - r1)
    ;r1 = xmm4 r2 = xmm10
    ;;;;;;
    movapd      xmm9 ,xmm4; copy r1
    movapd      xmm11 ,xmm10; copy r2
    mulpd       xmm9 ,XMMWORD PTR L__real_log2e_lead ; (log10e_lead*r1)
    mulpd       xmm11 ,XMMWORD PTR L__real_log2e_lead ;(log10e_lead*r2)
    mulpd       xmm4 ,XMMWORD PTR L__real_log2e_tail ; (log10e_tail*r1)
    mulpd       xmm10 ,XMMWORD PTR L__real_log2e_tail ; (log10e_tail*r2)
    addpd       xmm10  ,xmm4; (log10e_tail*r1) + (log10e_tail*r2)
    addpd       xmm10 ,xmm11; (log10e_lead*r1) + (log10e_lead*r2)
    addpd       xmm10 ,xmm9; result = (((log10e_tail*r2) + log10e_tail*r1) + log10e_lead*r2) + log10e_lead*r1;
    
    ;;;;;;
    ;END calculating within the threshold

    ;now restore the proper results
    pand      xmm10,xmm3
    pandn     xmm3,xmm8
    por       xmm3 ,xmm10; xmm3 stores the result


    movapd    xmm8 ,xmm0; copy the input

    ; now restore the result if input is less than 0.0
    movapd    xmm8,xmm1
    pand       xmm8,XMMWORD PTR L__real_qnan 
    pandn     xmm1,xmm3
    por       xmm1,xmm8



    ; now restore if there are some inputs with -1.0
    movapd    xmm8,xmm2
    pandn     xmm8,xmm1
    pand       xmm2,XMMWORD PTR L__real_ninf 
    por       xmm8 ,xmm2; first OR the infinities = -1.0
    
    ; now restore the Nans and Infs
    movapd    xmm3,xmm0
    addpd     xmm0,xmm0
    pand      xmm3 ,XMMWORD PTR L__sign_bit_64 ; for negative infities we need to set the result as Qnan
    ;so we get the sign bit and move it to the qnan Bit.
    ; then OR it with Qnan/inf result
    psrlq     xmm3,12
    pand      xmm3,xmm14
    pand      xmm0,xmm14
    por       xmm0,xmm3
    pandn     xmm14,xmm8
    por       xmm0,xmm14

;##########################################
L__end_process_vrd2_log2:    
    ;fill data back into memory buffer from the result xmm13
    movlpd QWORD PTR[rdx + rax * 8], xmm0
    inc rax
    cmp r8, rax
    jz L__exit_vrda_log2
    movhpd QWORD PTR [rdx + rax * 8], xmm0 
    inc rax
    jmp L__vrda_top   
L__exit_vrda_log2:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    RestoreXmm xmm6, save_xmm6
    RestoreXmm xmm7, save_xmm7
    RestoreXmm xmm8, save_xmm8
    RestoreXmm xmm9, save_xmm9
    RestoreXmm xmm10, save_xmm10
    RestoreXmm xmm11, save_xmm11
    ;RestoreXmm xmm12, save_xmm12
    RestoreXmm xmm13, save_xmm13
    RestoreXmm xmm14, save_xmm14
    ;RestoreXmm xmm15, save_xmm15    
    RestoreReg rdi,save_rdi
    RestoreReg rsi,save_rsi
    StackDeallocate stack_size
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;    
    ret
fname        endp
TEXT    ENDS    


data SEGMENT READ
CONST    SEGMENT
ALIGN 16
L__all_bits_set     DQ 0FFFFFFFFFFFFFFFFh
                    DQ 0FFFFFFFFFFFFFFFFh    
L__sign_bit_64      DQ 08000000000000000h 
                    DQ 08000000000000000h
L__sign_bit_32      DQ 08000000080000000h 
                    DQ 08000000080000000h
L__exp_mask         DQ 07ff0000000000000h   ;
                    DQ 07ff0000000000000h
L__exponent_compare DQ 07FF000007FF00000h  
                    DQ 07FF000007FF00000h
L__sign_mask_64     DQ 07FFFFFFFFFFFFFFFh
                    DQ 07FFFFFFFFFFFFFFFh
L__Zero_64          DQ 00000000000000000h
                    DQ 00000000000000000h

; The values exp(-1/16)-1 and exp(1/16)-1 
L__real_log_thresh1 DQ 03fee0faa00000000h ; 0.9394121170043945, 
                    DQ 03fee0faa00000000h
L__real_log_thresh2 DQ 03ff1082c00000000h;  1.0644950866699219;  
                    DQ 03ff1082c00000000h
L__impbit_dp64      DQ 00010000000000000h
                    DQ 00010000000000000h
L__constant1        DQ 003d0000000000000h
                    DQ 003d0000000000000h
L__expadjust        DQ 0000000000000003Ch
                    DQ 0000000000000003Ch
L__mask_1023        DQ 000000000000003ffh
                    DQ 000000000000003ffh
L__mantissa_mask    DQ 0000FFFFFFFFFFFFFh    ; mantissa bits
                    DQ 0000FFFFFFFFFFFFFh
L__real_half        DQ 03fe0000000000000h ; 1/2
                    DQ 03fe0000000000000h
L__index_mask1      DQ 0000fc00000000000h
                    DQ 0000fc00000000000h
L__index_mask2      DQ 00000200000000000h
                    DQ 00000200000000000h
L__index_mask3      DQ 00010000000000000h
                    DQ 00010000000000000h
L__index_constant1  DQ 03F80000000000000h
                    DQ 03F80000000000000h
L__index_constant2  DQ 00000000000000040h
                    DQ 00000000000000040h

; Approximating polynomial coefficients for other x 
L__real_cb_3       DQ 03f6249423bd94741h ; 2.23219810758559851206e-03;  
                   DQ 03f6249423bd94741h
L__real_cb_2       DQ 03f89999999865edeh ; 1.24999999978138668903e-02,  
                   DQ 03f89999999865edeh
L__real_cb_1       DQ 03fb5555555555557h ; 8.33333333333333593622e-02, 
                   DQ 03fb5555555555557h
L__real_log2_lead  DQ 03fe62e42e0000000h ; log2_lead  6.93147122859954833984e-01
                   DQ 03fe62e42e0000000h
L__real_log2_tail  DQ 03e6efa39ef35793ch ; log2_tail  5.76999904754328540596e-08
                   DQ 03e6efa39ef35793ch
L__real_log2e_lead DQ 03FF7154400000000h ; 1.44269180297851562500E+00  
                   DQ 03FF7154400000000h 
L__real_log2e_tail DQ 03ECB295C17F0BBBEh ; 3.23791044778235969970E-06
                   DQ 03ECB295C17F0BBBEh
L__one_mask_64     DQ 03ff0000000000000h ; 1
                   DQ 03ff0000000000000h
L__plus_two        DQ 04000000000000000h
                   DQ 04000000000000000h

; Approximating polynomial coefficients for x near 0.0 
L__real_ca_4       DQ 03f3c8034c85dfff0h ; 4.34887777707614552256e-04,  
                   DQ 03f3c8034c85dfff0h
L__real_ca_3       DQ 03f62492307f1519fh ; 2.23213998791944806202e-03,  
                   DQ 03f62492307f1519fh
L__real_ca_2       DQ 03f89999999bac6d4h ; 1.25000000037717509602e-02,  
                   DQ 03f89999999bac6d4h
L__real_ca_1       DQ 03fb55555555554e6h ; 8.33333333333317923934e-02,  
                   DQ 03fb55555555554e6h

L__mask32bits_DP64 DQ 0ffffffff00000000h
                   DQ 0ffffffff00000000h
L__real_qnan       DQ 07ff8000000000000h   ; qNaN
                   DQ 07ff8000000000000h
L__real_ninf       DQ 0fff0000000000000h   ; -inf
                   DQ 0fff0000000000000h

CONST    ENDS
data ENDS

END
