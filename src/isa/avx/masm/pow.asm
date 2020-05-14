;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

;
; pow.asm
;
; An implementation of the pow libm function.
;
; Prototype:
;
;     double pow(double x, double y);
;

;
;   Algorithm:
;       x^y = e^(y*ln(x))
;
;       Look in exp, log for the respective algorithms
;

include fm.inc
FN_PROTOTYPE_BAS64 pow


fname_special   TEXTEQU <_pow_special>

; define local variable storage offsets

save_x          EQU     40h
save_y          EQU     50h
p_temp_exp      EQU     60h
negate_result   EQU     70h
save_ax         EQU     80h
y_head          EQU     90h
p_temp_log      EQU     0a0h

save_xmm6       EQU     0b0h
save_xmm7       EQU     0c0h

;arg_x           EQU     0h
;arg_y           EQU     10h
;arg_z           EQU     20h
;arg_code        EQU     30h

stack_size      EQU     118h

; external function
EXTERN fname_special:PROC

; macros

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

    movsd       QWORD PTR [save_x+rsp], xmm0
    movsd       QWORD PTR [save_y+rsp], xmm1

    mov         rdx, QWORD PTR [save_x+rsp]
    mov         r8, QWORD PTR [save_y+rsp]

    mov         r10, QWORD PTR __exp_mant_mask
    and         r10, r8
    jz          __y_is_zero

    cmp         r8, QWORD PTR __pos_one
    je          __y_is_one

    mov         r9, QWORD PTR __sign_mask
    and         r9, rdx
    cmp         r9, QWORD PTR __sign_mask
    mov         rax, QWORD PTR __pos_zero
    mov         QWORD PTR [negate_result+rsp], rax    
    je          __x_is_neg

    cmp         rdx, QWORD PTR __pos_one
    je          __x_is_pos_one

    cmp         rdx, QWORD PTR __pos_zero
    je          __x_is_zero

    mov         r9, QWORD PTR __exp_mask
    and         r9, rdx
    cmp         r9, QWORD PTR __exp_mask
    je          __x_is_inf_or_nan
   
    mov         r10, QWORD PTR __exp_mask
    and         r10, r8
    cmp         r10, QWORD PTR __ay_max_bound
    jg          __ay_is_very_large

    mov         r10, QWORD PTR __exp_mask
    and         r10, r8
    cmp         r10, QWORD PTR __ay_min_bound
    jl          __ay_is_very_small

    ; -----------------------------
    ; compute log(x) here
    ; -----------------------------
__log_x:

    ; compute exponent part
    xor         r8, r8
    movdqa      xmm3, xmm0
    psrlq       xmm3, 52
    movd        r8, xmm0
    psubq       xmm3, XMMWORD PTR __mask_1023
    movdqa      xmm2, xmm0
    cvtdq2pd    xmm6, xmm3 ; xexp
    pand        xmm2, XMMWORD PTR __real_mant

    comisd      xmm6, QWORD PTR __mask_1023_f
    je          __denormal_adjust

__continue_common:

    ; compute index into the log tables
    movsd       xmm7, xmm0
    mov         r9, r8
    and         r8, QWORD PTR __mask_mant_all8
    and         r9, QWORD PTR __mask_mant9
    subsd       xmm7, __real_one
    shl         r9, 1
    add         r8, r9
    mov         QWORD PTR [p_temp_log+rsp], r8
    andpd       xmm7, __real_notsign

    ; F, Y, switch to near-one codepath
    movsd       xmm1, QWORD PTR [p_temp_log+rsp]
    shr         r8, 44
    por         xmm2, XMMWORD PTR __real_half
    por         xmm1, XMMWORD PTR __real_half
    comisd      xmm7, __real_threshold
    lea         r9, QWORD PTR __log_F_inv_head
    lea         rdx, QWORD PTR __log_F_inv_tail
    jb          __near_one

    ; f = F - Y, r = f * inv
    subsd       xmm1, xmm2
    movsd       xmm4, xmm1
    mulsd       xmm1, QWORD PTR [r9+r8*8]
    movsd       xmm5, xmm1
    mulsd       xmm4, QWORD PTR [rdx+r8*8]
    movsd       xmm7, xmm4
    addsd       xmm1, xmm4

    movsd       xmm2, xmm1
    movsd       xmm0, xmm1
    lea         r9, QWORD PTR __log_256_lead

    ; poly
    movsd       xmm3, QWORD PTR __real_1_over_6
    movsd       xmm1, QWORD PTR __real_1_over_3
    mulsd       xmm3, xmm2                         
    mulsd       xmm1, xmm2                         
    mulsd       xmm0, xmm2                         
    subsd       xmm5, xmm2
    movsd       xmm4, xmm0
    addsd       xmm3, QWORD PTR __real_1_over_5
    addsd       xmm1, QWORD PTR __real_1_over_2
    mulsd       xmm4, xmm0                         
    mulsd       xmm3, xmm2                         
    mulsd       xmm1, xmm0                         
    addsd       xmm3, QWORD PTR __real_1_over_4
    addsd       xmm7, xmm5
    mulsd       xmm3, xmm4                         
    addsd       xmm1, xmm3                         
    addsd       xmm1, xmm7

    movsd       xmm5, QWORD PTR __real_log2_tail
    lea         rdx, QWORD PTR __log_256_tail
    mulsd       xmm5, xmm6
    movsd       xmm0, QWORD PTR [r9+r8*8]
    subsd       xmm5, xmm1

    movsd       xmm3, QWORD PTR [rdx+r8*8]
    addsd       xmm3, xmm5
    movsd       xmm1, xmm3
    subsd       xmm3, xmm2

    movsd       xmm7, QWORD PTR __real_log2_lead
    mulsd       xmm7, xmm6
    addsd       xmm0, xmm7

    ; result of ln(x) is computed from head and tail parts, resH and resT
    ; res = ln(x) = resH + resT
    ; resH and resT are in full precision 

    ; resT is computed from head and tail parts, resT_h and resT_t
    ; resT = resT_h + resT_t

    ; now
    ; xmm3 - resT
    ; xmm0 - resH
    ; xmm1 - (resT_t)
    ; xmm2 - (-resT_h)

__log_x_continue:

    movsd       xmm7, xmm0
    addsd       xmm0, xmm3
    movsd       xmm5, xmm0
    andpd       xmm0, XMMWORD PTR __real_fffffffff8000000
   
    ; xmm0 - H
    ; xmm7 - resH
    ; xmm5 - res

    mov         rax, QWORD PTR [save_y+rsp]
    and         rax, QWORD PTR __real_fffffffff8000000

    addsd       xmm2, xmm3
    subsd       xmm7, xmm5
    subsd       xmm1, xmm2
    addsd       xmm7, xmm3
    subsd       xmm5, xmm0

    mov         QWORD PTR [y_head+rsp], rax
    movsd       xmm4, QWORD PTR [save_y+rsp]
   
    addsd       xmm7, xmm1 
    addsd       xmm7, xmm5

    ; res = H + T
    ; H has leading 26 bits of precision
    ; T has full precision

    ; xmm0 - H
    ; xmm7 - T

    movsd       xmm2, QWORD PTR [y_head+rsp] 
    subsd       xmm4, xmm2

    ; y is split into head and tail
    ; for y * ln(x) computation

    ; xmm4 - Yt
    ; xmm2 - Yh
    ; xmm0 - H
    ; xmm7 - T

    movsd   xmm3, xmm4
    movsd   xmm5, xmm7
    movsd   xmm6, xmm0
    mulsd   xmm3, xmm7 ; YtRt
    mulsd   xmm4, xmm0 ; YtRh
    mulsd   xmm5, xmm2 ; YhRt
    mulsd   xmm6, xmm2 ; YhRh 

    movsd   xmm1, xmm6
    addsd   xmm3, xmm4
    addsd   xmm3, xmm5

    addsd   xmm1, xmm3
    movsd   xmm0, xmm1

    subsd   xmm6, xmm1
    addsd   xmm6, xmm3 

    ; y * ln(x) = v + vt
    ; v and vt are in full precision 
 
    ; xmm0 - v
    ; xmm6 - vt

    ; -----------------------------
    ; compute exp( y * ln(x) ) here
    ; -----------------------------

    ; v * (64/ln(2))
    movsd       xmm7, QWORD PTR __real_64_by_log2
    movsd       QWORD PTR [p_temp_exp+rsp], xmm0
    mulsd       xmm7, xmm0
    mov         rdx, QWORD PTR [p_temp_exp+rsp]

    ; v < 1024*ln(2), ( v * (64/ln(2)) ) < 64*1024
    ; v >= -1075*ln(2), ( v * (64/ln(2)) ) >= 64*(-1075)
    comisd      xmm7, QWORD PTR __real_p65536
    ja          __process_result_inf

    comisd      xmm7, QWORD PTR __real_m68800
    jb          __process_result_zero

    ; n = int( v * (64/ln(2)) )
    cvtpd2dq    xmm4, xmm7
    lea         r10, QWORD PTR __two_to_jby64_head_table
    lea         r11, QWORD PTR __two_to_jby64_tail_table
    cvtdq2pd    xmm1, xmm4

    ; r1 = x - n * ln(2)/64 head
    movsd       xmm2, QWORD PTR __real_log2_by_64_head
    mulsd       xmm2, xmm1
    movd        ecx, xmm4
    mov         rax, 3fh
    and         eax, ecx
    subsd       xmm0, xmm2

    ; r2 = - n * ln(2)/64 tail
    mulsd       xmm1, QWORD PTR __real_log2_by_64_tail
    movsd       xmm2, xmm0

    ; m = (n - j) / 64
    sub         ecx, eax
    sar         ecx, 6

    ; r1+r2
    addsd       xmm2, xmm1
    addsd       xmm2, xmm6 ; add vt here
    movsd       xmm1, xmm2

    ; q
    movsd       xmm0, QWORD PTR __real_1_by_2
    movsd       xmm3, QWORD PTR __real_1_by_24
    movsd       xmm4, QWORD PTR __real_1_by_720
    mulsd       xmm1, xmm2
    mulsd       xmm0, xmm2
    mulsd       xmm3, xmm2
    mulsd       xmm4, xmm2

    movsd       xmm5, xmm1
    mulsd       xmm1, xmm2
    addsd       xmm0, QWORD PTR __real_one
    addsd       xmm3, QWORD PTR __real_1_by_6
    mulsd       xmm5, xmm1
    addsd       xmm4, QWORD PTR __real_1_by_120
    mulsd       xmm0, xmm2
    mulsd       xmm3, xmm1
 
    mulsd       xmm4, xmm5

    ; deal with denormal results
    xor         r9d, r9d
    cmp         ecx, DWORD PTR __denormal_threshold

    addsd       xmm3, xmm4
    addsd       xmm0, xmm3

    cmovle      r9d, ecx
    add         rcx, 1023
    shl         rcx, 52

    ; f1, f2
    movsd       xmm5, QWORD PTR [r11+rax*8]
    movsd       xmm1, QWORD PTR [r10+rax*8]
    mulsd       xmm5, xmm0
    mulsd       xmm1, xmm0

    cmp         rcx, QWORD PTR __real_inf

    ; (f1+f2)*(1+q)
    addsd       xmm5, QWORD PTR [r11+rax*8]
    addsd       xmm1, xmm5
    addsd       xmm1, QWORD PTR [r10+rax*8]
    movsd       xmm0, xmm1

    je          __process_almost_inf

    test        r9d, r9d
    mov         QWORD PTR [p_temp_exp+rsp], rcx
    jnz         __process_denormal
    mulsd       xmm0, QWORD PTR [p_temp_exp+rsp]
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]

__final_check:
    movdqa      xmm7, XMMWORD PTR [save_xmm7+rsp]
    movdqa      xmm6, XMMWORD PTR [save_xmm6+rsp]
    add         rsp, stack_size
    ret

ALIGN 16
__process_almost_inf:
    comisd      xmm0, QWORD PTR __real_one
    jae         __process_result_inf

    orpd        xmm0, XMMWORD PTR __enable_almost_inf
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]
    jmp         __final_check

ALIGN 16
__process_denormal:
    mov         ecx, r9d
    xor         r11d, r11d
    comisd      xmm0, QWORD PTR __real_one
    cmovae      r11d, ecx
    cmp         r11d, DWORD PTR __denormal_threshold
    jne         __process_true_denormal  

    mulsd       xmm0, QWORD PTR [p_temp_exp+rsp]
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]
    jmp         __final_check

ALIGN 16
__process_true_denormal:
    xor         r8, r8
    cmp         rdx, QWORD PTR __denormal_tiny_threshold
    mov         r9, 1
    jg          __process_denormal_tiny
    add         ecx, 1074
    cmovs       rcx, r8
    shl         r9, cl
    mov         rcx, r9

    mov         QWORD PTR [p_temp_exp+rsp], rcx
    mulsd       xmm0, QWORD PTR [p_temp_exp+rsp]
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]
    jmp         __z_denormal        

ALIGN 16
__process_denormal_tiny:
    movsd       xmm0, QWORD PTR __real_smallest_denormal
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]
    jmp         __z_denormal

ALIGN 16
__process_result_zero:
    mov         r11, QWORD PTR __real_zero
    or          r11, QWORD PTR [negate_result+rsp]
    jmp         __z_is_zero_or_inf
 
ALIGN 16
__process_result_inf:
    mov         r11, QWORD PTR __real_inf
    or          r11, QWORD PTR [negate_result+rsp]
    jmp         __z_is_zero_or_inf

ALIGN 16
__denormal_adjust:
    por         xmm2, XMMWORD PTR __real_one
    subsd       xmm2, QWORD PTR __real_one
    movsd       xmm5, xmm2
    pand        xmm2, XMMWORD PTR __real_mant
    movd        r8, xmm2
    psrlq       xmm5, 52
    psubd       xmm5, XMMWORD PTR __mask_2045
    cvtdq2pd    xmm6, xmm5
    jmp         __continue_common

ALIGN 16
__x_is_neg:

    mov         r10, QWORD PTR __exp_mask
    and         r10, r8
    cmp         r10, QWORD PTR __ay_max_bound
    jg          __ay_is_very_large

    ; determine if y is an integer
    mov         r10, QWORD PTR __exp_mant_mask
    and         r10, r8
    mov         r11, r10
    mov         rcx, QWORD PTR __exp_shift
    shr         r10, cl
    sub         r10, QWORD PTR __exp_bias
    js          __x_is_neg_y_is_not_int
   
    mov         rax, QWORD PTR __exp_mant_mask
    and         rax, rdx
    mov         QWORD PTR [save_ax+rsp], rax

    cmp         r10, QWORD PTR __yexp_53
    mov         rcx, r10
    jg          __continue_after_y_int_check

    mov         r9, QWORD PTR __mant_full
    shr         r9, cl
    and         r9, r11
    jnz         __x_is_neg_y_is_not_int

    mov         r9, QWORD PTR __1_before_mant
    shr         r9, cl
    and         r9, r11
    jz          __continue_after_y_int_check

    mov         rax, QWORD PTR __sign_mask
    mov         QWORD PTR [negate_result+rsp], rax    

__continue_after_y_int_check:

    cmp         rdx, QWORD PTR __neg_zero
    je          __x_is_zero

    cmp         rdx, QWORD PTR __neg_one
    je          __x_is_neg_one

    mov         r9, QWORD PTR __exp_mask
    and         r9, rdx
    cmp         r9, QWORD PTR __exp_mask
    je          __x_is_inf_or_nan
   
    movsd       xmm0, QWORD PTR [save_ax+rsp]
    jmp         __log_x


ALIGN 16
__near_one:

    ; f = F - Y, r = f * inv
    movsd       xmm0, xmm1
    subsd       xmm1, xmm2
    movsd       xmm4, xmm1

    movsd       xmm3, QWORD PTR [r9+r8*8]
    addsd       xmm3, QWORD PTR [rdx+r8*8]
    mulsd       xmm4, xmm3
    andpd       xmm4, QWORD PTR __real_fffffffff8000000
    movsd       xmm5, xmm4 ; r1
    mulsd       xmm4, xmm0
    subsd       xmm1, xmm4
    mulsd       xmm1, xmm3
    movsd       xmm7, xmm1 ; r2
    addsd       xmm1, xmm5

    movsd       xmm2, xmm1
    movsd       xmm0, xmm1

    lea         r9, QWORD PTR __log_256_lead

    ; poly
    movsd       xmm3, QWORD PTR __real_1_over_7
    movsd       xmm1, QWORD PTR __real_1_over_4
    mulsd       xmm3, xmm2
    mulsd       xmm1, xmm2
    mulsd       xmm0, xmm2
    movsd       xmm4, xmm0
    addsd       xmm3, QWORD PTR __real_1_over_6
    addsd       xmm1, QWORD PTR __real_1_over_3
    mulsd       xmm4, xmm0
    mulsd       xmm3, xmm2
    mulsd       xmm1, xmm2
    addsd       xmm3, QWORD PTR __real_1_over_5
    mulsd       xmm3, xmm2
    mulsd       xmm1, xmm0
    mulsd       xmm3, xmm4

    movsd       xmm2, xmm5
    movsd       xmm0, xmm7
    mulsd       xmm0, xmm0
    mulsd       xmm0, QWORD PTR __real_1_over_2
    mulsd       xmm5, xmm7
    addsd       xmm5, xmm0
    addsd       xmm5, xmm7

    movsd       xmm0, xmm2
    movsd       xmm7, xmm2
    mulsd       xmm0, xmm0
    mulsd       xmm0, QWORD PTR __real_1_over_2
    movsd       xmm4, xmm0
    addsd       xmm2, xmm0 ; r1 + r1^2/2
    subsd       xmm7, xmm2
    addsd       xmm7, xmm4

    addsd       xmm3, xmm7
    movsd       xmm4, QWORD PTR __real_log2_tail
    addsd       xmm1, xmm3
    mulsd       xmm4, xmm6
    lea         rdx, QWORD PTR __log_256_tail
    addsd       xmm1, xmm5
    addsd       xmm4, QWORD PTR [rdx+r8*8]
    subsd       xmm4, xmm1

    movsd       xmm3, xmm4
    movsd       xmm1, xmm4
    subsd       xmm3, xmm2

    movsd       xmm0, QWORD PTR [r9+r8*8]
    movsd       xmm7, QWORD PTR __real_log2_lead
    mulsd       xmm7, xmm6
    addsd       xmm0, xmm7

    jmp         __log_x_continue


ALIGN 16
__x_is_pos_one:
    jmp         __final_check

ALIGN 16
__y_is_zero:
    movsd       xmm0, QWORD PTR __real_one
    jmp         __final_check

ALIGN 16
__y_is_one:
    xor         rax, rax
    mov         r11, rdx
    mov         r9, QWORD PTR __exp_mask
    ;or          r11, QWORD PTR __qnan_set
    and         r9, rdx
    cmp         r9, QWORD PTR __exp_mask
    cmove       rax, rdx
    mov         r9, QWORD PTR __mant_mask
    and         r9, rax
    jnz         __x_is_nan

    movd        xmm0, rdx 
    jmp         __final_check

ALIGN 16
__x_is_neg_one:
    mov         rdx, QWORD PTR __pos_one
    or          rdx, QWORD PTR [negate_result+rsp]
    xor         rax, rax
    mov         r11, r8
    mov         r10, QWORD PTR __exp_mask
    ;or          r11, QWORD PTR __qnan_set
    and         r10, r8
    cmp         r10, QWORD PTR __exp_mask
    cmove       rax, r8
    mov         r10, QWORD PTR __mant_mask
    and         r10, rax
    jnz         __y_is_nan

    movd        xmm0, rdx
    jmp         __final_check

ALIGN 16
__x_is_neg_y_is_not_int:
    mov         r9, QWORD PTR __exp_mask
    and         r9, rdx
    cmp         r9, QWORD PTR __exp_mask
    je          __x_is_inf_or_nan

    cmp         rdx, QWORD PTR __neg_zero
    je          __x_is_zero

    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    movsd       xmm2, QWORD PTR __neg_qnan
    mov         r9d, DWORD PTR __flag_x_neg_y_notint

    call        fname_special
    jmp         __final_check

ALIGN 16
__ay_is_very_large:
    mov         r9, QWORD PTR __exp_mask
    and         r9, rdx
    cmp         r9, QWORD PTR __exp_mask
    je          __x_is_inf_or_nan

    mov         r9, QWORD PTR __exp_mant_mask
    and         r9, rdx
    jz          __x_is_zero 

    cmp         rdx, QWORD PTR __neg_one
    je          __x_is_neg_one

    mov         r9, rdx
    and         r9, QWORD PTR __exp_mant_mask
    cmp         r9, QWORD PTR __pos_one
    jl          __ax_lt1_y_is_large_or_inf_or_nan
  
    jmp         __ax_gt1_y_is_large_or_inf_or_nan

ALIGN 16
__x_is_zero:
    mov         r10, QWORD PTR __exp_mask
    xor         rax, rax
    and         r10, r8
    cmp         r10, QWORD PTR __exp_mask
    je          __x_is_zero_y_is_inf_or_nan

    mov         r10, QWORD PTR __sign_mask
    and         r10, r8
    cmovnz      rax, QWORD PTR __pos_inf
    jnz         __x_is_zero_z_is_inf

    movd        xmm0, rax
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]
    jmp         __final_check

ALIGN 16
__x_is_zero_z_is_inf:

    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    movd        xmm2, rax
    orpd        xmm2, XMMWORD PTR [negate_result+rsp]
    mov         r9d, DWORD PTR __flag_x_zero_z_inf

    call        fname_special
    jmp         __final_check

ALIGN 16
__x_is_zero_y_is_inf_or_nan:
    mov         r11, r8
    cmp         r8, QWORD PTR __neg_inf
    cmove       rax, QWORD PTR __pos_inf
    je          __x_is_zero_z_is_inf

    ;or          r11, QWORD PTR __qnan_set
    mov         r10, QWORD PTR __mant_mask
    and         r10, r8
    jnz         __y_is_nan

    movd        xmm0, rax
    jmp         __final_check

ALIGN 16
__x_is_inf_or_nan:
    xor         r11, r11
    mov         r10, QWORD PTR __sign_mask
    and         r10, r8
    cmovz       r11, QWORD PTR __pos_inf
    mov         rax, rdx
    mov         r9, QWORD PTR __mant_mask
    ;or          rax, QWORD PTR __qnan_set
    and         r9, rdx
    cmovnz      r11, rax
    jnz         __x_is_nan

    xor         rax, rax
    mov         r9, r8
    mov         r10, QWORD PTR __exp_mask
    ;or          r9, QWORD PTR __qnan_set
    and         r10, r8
    cmp         r10, QWORD PTR __exp_mask
    cmove       rax, r8
    mov         r10, QWORD PTR __mant_mask
    and         r10, rax
    cmovnz      r11, r9
    jnz         __y_is_nan

    movd        xmm0, r11
    orpd        xmm0, XMMWORD PTR [negate_result+rsp]
    jmp         __final_check

ALIGN 16
__ay_is_very_small:
    movsd       xmm0, QWORD PTR __pos_one
    addsd       xmm0, xmm1
    jmp         __final_check


ALIGN 16
__ax_lt1_y_is_large_or_inf_or_nan:
    xor         r11, r11
    mov         r10, QWORD PTR __sign_mask
    and         r10, r8
    cmovnz      r11, QWORD PTR __pos_inf
    jmp         __adjust_for_nan

ALIGN 16
__ax_gt1_y_is_large_or_inf_or_nan:
    xor         r11, r11
    mov         r10, QWORD PTR __sign_mask
    and         r10, r8
    cmovz       r11, QWORD PTR __pos_inf

ALIGN 16
__adjust_for_nan:

    xor         rax, rax
    mov         r9, r8
    mov         r10, QWORD PTR __exp_mask
    ;or          r9, QWORD PTR __qnan_set
    and         r10, r8
    cmp         r10, QWORD PTR __exp_mask
    cmove       rax, r8
    mov         r10, QWORD PTR __mant_mask
    and         r10, rax
    cmovnz      r11, r9
    jnz         __y_is_nan

    test        rax, rax
    jnz         __y_is_inf

ALIGN 16
__z_is_zero_or_inf:

    mov         r9d, DWORD PTR __flag_z_zero
    test        r11, QWORD PTR __exp_mant_mask
    cmovnz      r9d, DWORD PTR __flag_z_inf
    
    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    movd        xmm2, r11

    call        fname_special
    jmp         __final_check

ALIGN 16
__y_is_inf:

    movd        xmm0, r11
    jmp         __final_check

ALIGN 16
__x_is_nan:

    xor         rax, rax
    mov         r10, QWORD PTR __exp_mask
    and         r10, r8
    cmp         r10, QWORD PTR __exp_mask
    cmove       rax, r8
    mov         r10, QWORD PTR __mant_mask
    and         r10, rax
    jnz         __x_is_nan_y_is_nan

    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    movd        xmm2, r11
    mov         r9d, DWORD PTR __flag_x_nan

    call        fname_special
    jmp         __final_check

ALIGN 16
__y_is_nan:

    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    movd        xmm2, r11
    mov         r9d, DWORD PTR __flag_y_nan

    call        fname_special
    jmp         __final_check

ALIGN 16
__x_is_nan_y_is_nan:

    mov         r9, r8
    
    cmp         r11, QWORD PTR __ind_pattern
    cmove       r11, r9
    je          __continue_xy_nan

    cmp         r9, QWORD PTR __ind_pattern
    cmove       r9, r11

    mov         r10, r9
    and         r10, QWORD PTR __sign_mask
    cmovnz      r9, r11

    mov         r10, r11
    and         r10, QWORD PTR __sign_mask
    cmovnz      r11, r9
    
__continue_xy_nan:    
    ;or          r11, QWORD PTR __qnan_set
    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    movd        xmm2, r11
    mov         r9d, DWORD PTR __flag_x_nan_y_nan

    call        fname_special
    jmp         __final_check  
    
ALIGN 16
__z_denormal:
    
    movsd       xmm2, xmm0
    movsd       xmm0, QWORD PTR [save_x+rsp]
    movsd       xmm1, QWORD PTR [save_y+rsp]
    mov         r9d, DWORD PTR __flag_z_denormal

    call        fname_special
    jmp         __final_check  

fname endp

text ENDS

data SEGMENT READ

ALIGN 16

; these codes and the ones in the corresponding .c file have to match
__flag_x_one_y_snan             DD 00000001
__flag_x_zero_z_inf             DD 00000002
__flag_x_nan                    DD 00000003
__flag_y_nan                    DD 00000004
__flag_x_nan_y_nan              DD 00000005
__flag_x_neg_y_notint           DD 00000006
__flag_z_zero                   DD 00000007
__flag_z_denormal               DD 00000008
__flag_z_inf                    DD 00000009

ALIGN 16
   
__ay_max_bound              DQ 43e0000000000000h
__ay_min_bound              DQ 3c00000000000000h
__sign_mask                 DQ 8000000000000000h
__sign_and_exp_mask         DQ 0fff0000000000000h
__exp_mask                  DQ 7ff0000000000000h
__neg_inf                   DQ 0fff0000000000000h
__pos_inf                   DQ 7ff0000000000000h
__pos_one                   DQ 3ff0000000000000h
__pos_zero                  DQ 0000000000000000h
__exp_mant_mask             DQ 7fffffffffffffffh
__mant_mask                 DQ 000fffffffffffffh
__ind_pattern               DQ 0fff8000000000000h

__neg_qnan                  DQ 0fff8000000000000h
__qnan                      DQ 7ff8000000000000h
__qnan_set                  DQ 0008000000000000h

__neg_one                   DQ 0bff0000000000000h
__neg_zero                  DQ 8000000000000000h

__exp_shift                 DQ 0000000000000034h ; 52
__exp_bias                  DQ 00000000000003ffh ; 1023
__exp_bias_m1               DQ 00000000000003feh ; 1022

__yexp_53                   DQ 0000000000000035h ; 53
__mant_full                 DQ 000fffffffffffffh
__1_before_mant             DQ 0010000000000000h

__mask_mant_all8            DQ 000ff00000000000h
__mask_mant9                DQ 0000080000000000h

ALIGN 16
__real_fffffffff8000000     DQ 0fffffffff8000000h
                            DQ 0fffffffff8000000h

__mask_8000000000000000     DQ 8000000000000000h
                            DQ 8000000000000000h

__real_4090040000000000     DQ 4090040000000000h
                            DQ 4090040000000000h

__real_C090C80000000000     DQ 0C090C80000000000h
                            DQ 0C090C80000000000h

;---------------------
; log data
;---------------------

ALIGN 16

__real_ninf     DQ 0fff0000000000000h   ; -inf
                DQ 0000000000000000h
__real_inf      DQ 7ff0000000000000h    ; +inf
                DQ 0000000000000000h
__real_nan      DQ 7ff8000000000000h    ; NaN
                DQ 0000000000000000h
__real_mant     DQ 000FFFFFFFFFFFFFh    ; mantissa bits
                DQ 0000000000000000h
__mask_1023     DQ 00000000000003ffh
                DQ 0000000000000000h
__mask_001      DQ 0000000000000001h
                DQ 0000000000000000h


__real_log2_lead    DQ 3fe62e42e0000000h ; log2_lead  6.93147122859954833984e-01
                    DQ 0000000000000000h
__real_log2_tail    DQ 3e6efa39ef35793ch ; log2_tail  5.76999904754328540596e-08
                    DQ 0000000000000000h

__real_two          DQ 4000000000000000h ; 2
                    DQ 0000000000000000h

__real_one          DQ 3ff0000000000000h ; 1
                    DQ 0000000000000000h

__real_half         DQ 3fe0000000000000h ; 1/2
                    DQ 0000000000000000h

__mask_100          DQ 0000000000000100h
                    DQ 0000000000000000h

__real_1_over_2     DQ 3fe0000000000000h
                    DQ 0000000000000000h
__real_1_over_3     DQ 3fd5555555555555h
                    DQ 0000000000000000h
__real_1_over_4     DQ 3fd0000000000000h
                    DQ 0000000000000000h
__real_1_over_5     DQ 3fc999999999999ah
                    DQ 0000000000000000h
__real_1_over_6     DQ 3fc5555555555555h
                    DQ 0000000000000000h
__real_1_over_7     DQ 3fc2492492492494h
                    DQ 0000000000000000h

__mask_1023_f       DQ 0c08ff80000000000h
                    DQ 0000000000000000h

__mask_2045         DQ 00000000000007fdh
                    DQ 0000000000000000h

__real_threshold    DQ 3fc0000000000000h ; 0.125
                    DQ 3fc0000000000000h

__real_notsign      DQ 7ffFFFFFFFFFFFFFh ; ^sign bit
                    DQ 0000000000000000h


ALIGN 16
__log_256_lead:
                    DQ 0000000000000000h
                    DQ 3f6ff00aa0000000h
                    DQ 3f7fe02a60000000h
                    DQ 3f87dc4750000000h
                    DQ 3f8fc0a8b0000000h
                    DQ 3f93cea440000000h
                    DQ 3f97b91b00000000h
                    DQ 3f9b9fc020000000h
                    DQ 3f9f829b00000000h
                    DQ 3fa1b0d980000000h
                    DQ 3fa39e87b0000000h
                    DQ 3fa58a5ba0000000h
                    DQ 3fa77458f0000000h
                    DQ 3fa95c8300000000h
                    DQ 3fab42dd70000000h
                    DQ 3fad276b80000000h
                    DQ 3faf0a30c0000000h
                    DQ 3fb0759830000000h
                    DQ 3fb16536e0000000h
                    DQ 3fb253f620000000h
                    DQ 3fb341d790000000h
                    DQ 3fb42edcb0000000h
                    DQ 3fb51b0730000000h
                    DQ 3fb60658a0000000h
                    DQ 3fb6f0d280000000h
                    DQ 3fb7da7660000000h
                    DQ 3fb8c345d0000000h
                    DQ 3fb9ab4240000000h
                    DQ 3fba926d30000000h
                    DQ 3fbb78c820000000h
                    DQ 3fbc5e5480000000h
                    DQ 3fbd4313d0000000h
                    DQ 3fbe270760000000h
                    DQ 3fbf0a30c0000000h
                    DQ 3fbfec9130000000h
                    DQ 3fc0671510000000h
                    DQ 3fc0d77e70000000h
                    DQ 3fc1478580000000h
                    DQ 3fc1b72ad0000000h
                    DQ 3fc2266f10000000h
                    DQ 3fc29552f0000000h
                    DQ 3fc303d710000000h
                    DQ 3fc371fc20000000h
                    DQ 3fc3dfc2b0000000h
                    DQ 3fc44d2b60000000h
                    DQ 3fc4ba36f0000000h
                    DQ 3fc526e5e0000000h
                    DQ 3fc59338d0000000h
                    DQ 3fc5ff3070000000h
                    DQ 3fc66acd40000000h
                    DQ 3fc6d60fe0000000h
                    DQ 3fc740f8f0000000h
                    DQ 3fc7ab8900000000h
                    DQ 3fc815c0a0000000h
                    DQ 3fc87fa060000000h
                    DQ 3fc8e928d0000000h
                    DQ 3fc9525a90000000h
                    DQ 3fc9bb3620000000h
                    DQ 3fca23bc10000000h
                    DQ 3fca8becf0000000h
                    DQ 3fcaf3c940000000h
                    DQ 3fcb5b5190000000h
                    DQ 3fcbc28670000000h
                    DQ 3fcc296850000000h
                    DQ 3fcc8ff7c0000000h
                    DQ 3fccf63540000000h
                    DQ 3fcd5c2160000000h
                    DQ 3fcdc1bca0000000h
                    DQ 3fce270760000000h
                    DQ 3fce8c0250000000h
                    DQ 3fcef0adc0000000h
                    DQ 3fcf550a50000000h
                    DQ 3fcfb91860000000h
                    DQ 3fd00e6c40000000h
                    DQ 3fd0402590000000h
                    DQ 3fd071b850000000h
                    DQ 3fd0a324e0000000h
                    DQ 3fd0d46b50000000h
                    DQ 3fd1058bf0000000h
                    DQ 3fd1368700000000h
                    DQ 3fd1675ca0000000h
                    DQ 3fd1980d20000000h
                    DQ 3fd1c898c0000000h
                    DQ 3fd1f8ff90000000h
                    DQ 3fd22941f0000000h
                    DQ 3fd2596010000000h
                    DQ 3fd2895a10000000h
                    DQ 3fd2b93030000000h
                    DQ 3fd2e8e2b0000000h
                    DQ 3fd31871c0000000h
                    DQ 3fd347dd90000000h
                    DQ 3fd3772660000000h
                    DQ 3fd3a64c50000000h
                    DQ 3fd3d54fa0000000h
                    DQ 3fd4043080000000h
                    DQ 3fd432ef20000000h
                    DQ 3fd4618bc0000000h
                    DQ 3fd4900680000000h
                    DQ 3fd4be5f90000000h
                    DQ 3fd4ec9730000000h
                    DQ 3fd51aad80000000h
                    DQ 3fd548a2c0000000h
                    DQ 3fd5767710000000h
                    DQ 3fd5a42ab0000000h
                    DQ 3fd5d1bdb0000000h
                    DQ 3fd5ff3070000000h
                    DQ 3fd62c82f0000000h
                    DQ 3fd659b570000000h
                    DQ 3fd686c810000000h
                    DQ 3fd6b3bb20000000h
                    DQ 3fd6e08ea0000000h
                    DQ 3fd70d42e0000000h
                    DQ 3fd739d7f0000000h
                    DQ 3fd7664e10000000h
                    DQ 3fd792a550000000h
                    DQ 3fd7bede00000000h
                    DQ 3fd7eaf830000000h
                    DQ 3fd816f410000000h
                    DQ 3fd842d1d0000000h
                    DQ 3fd86e9190000000h
                    DQ 3fd89a3380000000h
                    DQ 3fd8c5b7c0000000h
                    DQ 3fd8f11e80000000h
                    DQ 3fd91c67e0000000h
                    DQ 3fd9479410000000h
                    DQ 3fd972a340000000h
                    DQ 3fd99d9580000000h
                    DQ 3fd9c86b00000000h
                    DQ 3fd9f323e0000000h
                    DQ 3fda1dc060000000h
                    DQ 3fda484090000000h
                    DQ 3fda72a490000000h
                    DQ 3fda9cec90000000h
                    DQ 3fdac718c0000000h
                    DQ 3fdaf12930000000h
                    DQ 3fdb1b1e00000000h
                    DQ 3fdb44f770000000h
                    DQ 3fdb6eb590000000h
                    DQ 3fdb985890000000h
                    DQ 3fdbc1e080000000h
                    DQ 3fdbeb4d90000000h
                    DQ 3fdc149ff0000000h
                    DQ 3fdc3dd7a0000000h
                    DQ 3fdc66f4e0000000h
                    DQ 3fdc8ff7c0000000h
                    DQ 3fdcb8e070000000h
                    DQ 3fdce1af00000000h
                    DQ 3fdd0a63a0000000h
                    DQ 3fdd32fe70000000h
                    DQ 3fdd5b7f90000000h
                    DQ 3fdd83e720000000h
                    DQ 3fddac3530000000h
                    DQ 3fddd46a00000000h
                    DQ 3fddfc8590000000h
                    DQ 3fde248810000000h
                    DQ 3fde4c71a0000000h
                    DQ 3fde744260000000h
                    DQ 3fde9bfa60000000h
                    DQ 3fdec399d0000000h
                    DQ 3fdeeb20c0000000h
                    DQ 3fdf128f50000000h
                    DQ 3fdf39e5b0000000h
                    DQ 3fdf6123f0000000h
                    DQ 3fdf884a30000000h
                    DQ 3fdfaf5880000000h
                    DQ 3fdfd64f20000000h
                    DQ 3fdffd2e00000000h
                    DQ 3fe011fab0000000h
                    DQ 3fe02552a0000000h
                    DQ 3fe0389ee0000000h
                    DQ 3fe04bdf90000000h
                    DQ 3fe05f14b0000000h
                    DQ 3fe0723e50000000h
                    DQ 3fe0855c80000000h
                    DQ 3fe0986f40000000h
                    DQ 3fe0ab76b0000000h
                    DQ 3fe0be72e0000000h
                    DQ 3fe0d163c0000000h
                    DQ 3fe0e44980000000h
                    DQ 3fe0f72410000000h
                    DQ 3fe109f390000000h
                    DQ 3fe11cb810000000h
                    DQ 3fe12f7190000000h
                    DQ 3fe1422020000000h
                    DQ 3fe154c3d0000000h
                    DQ 3fe1675ca0000000h
                    DQ 3fe179eab0000000h
                    DQ 3fe18c6e00000000h
                    DQ 3fe19ee6b0000000h
                    DQ 3fe1b154b0000000h
                    DQ 3fe1c3b810000000h
                    DQ 3fe1d610f0000000h
                    DQ 3fe1e85f50000000h
                    DQ 3fe1faa340000000h
                    DQ 3fe20cdcd0000000h
                    DQ 3fe21f0bf0000000h
                    DQ 3fe23130d0000000h
                    DQ 3fe2434b60000000h
                    DQ 3fe2555bc0000000h
                    DQ 3fe2676200000000h
                    DQ 3fe2795e10000000h
                    DQ 3fe28b5000000000h
                    DQ 3fe29d37f0000000h
                    DQ 3fe2af15f0000000h
                    DQ 3fe2c0e9e0000000h
                    DQ 3fe2d2b400000000h
                    DQ 3fe2e47430000000h
                    DQ 3fe2f62a90000000h
                    DQ 3fe307d730000000h
                    DQ 3fe3197a00000000h
                    DQ 3fe32b1330000000h
                    DQ 3fe33ca2b0000000h
                    DQ 3fe34e2890000000h
                    DQ 3fe35fa4e0000000h
                    DQ 3fe37117b0000000h
                    DQ 3fe38280f0000000h
                    DQ 3fe393e0d0000000h
                    DQ 3fe3a53730000000h
                    DQ 3fe3b68440000000h
                    DQ 3fe3c7c7f0000000h
                    DQ 3fe3d90260000000h
                    DQ 3fe3ea3390000000h
                    DQ 3fe3fb5b80000000h
                    DQ 3fe40c7a40000000h
                    DQ 3fe41d8fe0000000h
                    DQ 3fe42e9c60000000h
                    DQ 3fe43f9fe0000000h
                    DQ 3fe4509a50000000h
                    DQ 3fe4618bc0000000h
                    DQ 3fe4727430000000h
                    DQ 3fe48353d0000000h
                    DQ 3fe4942a80000000h
                    DQ 3fe4a4f850000000h
                    DQ 3fe4b5bd60000000h
                    DQ 3fe4c679a0000000h
                    DQ 3fe4d72d30000000h
                    DQ 3fe4e7d810000000h
                    DQ 3fe4f87a30000000h
                    DQ 3fe50913c0000000h
                    DQ 3fe519a4c0000000h
                    DQ 3fe52a2d20000000h
                    DQ 3fe53aad00000000h
                    DQ 3fe54b2460000000h
                    DQ 3fe55b9350000000h
                    DQ 3fe56bf9d0000000h
                    DQ 3fe57c57f0000000h
                    DQ 3fe58cadb0000000h
                    DQ 3fe59cfb20000000h
                    DQ 3fe5ad4040000000h
                    DQ 3fe5bd7d30000000h
                    DQ 3fe5cdb1d0000000h
                    DQ 3fe5ddde50000000h
                    DQ 3fe5ee02a0000000h
                    DQ 3fe5fe1ed0000000h
                    DQ 3fe60e32f0000000h
                    DQ 3fe61e3ef0000000h
                    DQ 3fe62e42e0000000h
                    DQ 0000000000000000h

ALIGN 16
__log_256_tail:
                    DQ 0000000000000000h
                    DQ 3db5885e0250435ah
                    DQ 3de620cf11f86ed2h
                    DQ 3dff0214edba4a25h
                    DQ 3dbf807c79f3db4eh
                    DQ 3dea352ba779a52bh
                    DQ 3dff56c46aa49fd5h
                    DQ 3dfebe465fef5196h
                    DQ 3e0cf0660099f1f8h
                    DQ 3e1247b2ff85945dh
                    DQ 3e13fd7abf5202b6h
                    DQ 3e1f91c9a918d51eh
                    DQ 3e08cb73f118d3cah
                    DQ 3e1d91c7d6fad074h
                    DQ 3de1971bec28d14ch
                    DQ 3e15b616a423c78ah
                    DQ 3da162a6617cc971h
                    DQ 3e166391c4c06d29h
                    DQ 3e2d46f5c1d0c4b8h
                    DQ 3e2e14282df1f6d3h
                    DQ 3e186f47424a660dh
                    DQ 3e2d4c8de077753eh
                    DQ 3e2e0c307ed24f1ch
                    DQ 3e226ea18763bdd3h
                    DQ 3e25cad69737c933h
                    DQ 3e2af62599088901h
                    DQ 3e18c66c83d6b2d0h
                    DQ 3e1880ceb36fb30fh
                    DQ 3e2495aac6ca17a4h
                    DQ 3e2761db4210878ch
                    DQ 3e2eb78e862bac2fh
                    DQ 3e19b2cd75790dd9h
                    DQ 3e2c55e5cbd3d50fh
                    DQ 3db162a6617cc971h
                    DQ 3dfdbeabaaa2e519h
                    DQ 3e1652cb7150c647h
                    DQ 3e39a11cb2cd2ee2h
                    DQ 3e219d0ab1a28813h
                    DQ 3e24bd9e80a41811h
                    DQ 3e3214b596faa3dfh
                    DQ 3e303fea46980bb8h
                    DQ 3e31c8ffa5fd28c7h
                    DQ 3dce8f743bcd96c5h
                    DQ 3dfd98c5395315c6h
                    DQ 3e3996fa3ccfa7b2h
                    DQ 3e1cd2af2ad13037h
                    DQ 3e1d0da1bd17200eh
                    DQ 3e3330410ba68b75h
                    DQ 3df4f27a790e7c41h
                    DQ 3e13956a86f6ff1bh
                    DQ 3e2c6748723551d9h
                    DQ 3e2500de9326cdfch
                    DQ 3e1086c848df1b59h
                    DQ 3e04357ead6836ffh
                    DQ 3e24832442408024h
                    DQ 3e3d10da8154b13dh
                    DQ 3e39e8ad68ec8260h
                    DQ 3e3cfbf706abaf18h
                    DQ 3e3fc56ac6326e23h
                    DQ 3e39105e3185cf21h
                    DQ 3e3d017fe5b19cc0h
                    DQ 3e3d1f6b48dd13feh
                    DQ 3e20b63358a7e73ah
                    DQ 3e263063028c211ch
                    DQ 3e2e6a6886b09760h
                    DQ 3e3c138bb891cd03h
                    DQ 3e369f7722b7221ah
                    DQ 3df57d8fac1a628ch
                    DQ 3e3c55e5cbd3d50fh
                    DQ 3e1552d2ff48fe2eh
                    DQ 3e37b8b26ca431bch
                    DQ 3e292decdc1c5f6dh
                    DQ 3e3abc7c551aaa8ch
                    DQ 3e36b540731a354bh
                    DQ 3e32d341036b89efh
                    DQ 3e4f9ab21a3a2e0fh
                    DQ 3e239c871afb9fbdh
                    DQ 3e3e6add2c81f640h
                    DQ 3e435c95aa313f41h
                    DQ 3e249d4582f6cc53h
                    DQ 3e47574c1c07398fh
                    DQ 3e4ba846dece9e8dh
                    DQ 3e16999fafbc68e7h
                    DQ 3e4c9145e51b0103h
                    DQ 3e479ef2cb44850ah
                    DQ 3e0beec73de11275h
                    DQ 3e2ef4351af5a498h
                    DQ 3e45713a493b4a50h
                    DQ 3e45c23a61385992h
                    DQ 3e42a88309f57299h
                    DQ 3e4530faa9ac8aceh
                    DQ 3e25fec2d792a758h
                    DQ 3e35a517a71cbcd7h
                    DQ 3e3707dc3e1cd9a3h
                    DQ 3e3a1a9f8ef43049h
                    DQ 3e4409d0276b3674h
                    DQ 3e20e2f613e85bd9h
                    DQ 3df0027433001e5fh
                    DQ 3e35dde2836d3265h
                    DQ 3e2300134d7aaf04h
                    DQ 3e3cb7e0b42724f5h
                    DQ 3e2d6e93167e6308h
                    DQ 3e3d1569b1526adbh
                    DQ 3e0e99fc338a1a41h
                    DQ 3e4eb01394a11b1ch
                    DQ 3e04f27a790e7c41h
                    DQ 3e25ce3ca97b7af9h
                    DQ 3e281f0f940ed857h
                    DQ 3e4d36295d88857ch
                    DQ 3e21aca1ec4af526h
                    DQ 3e445743c7182726h
                    DQ 3e23c491aead337eh
                    DQ 3e3aef401a738931h
                    DQ 3e21cede76092a29h
                    DQ 3e4fba8f44f82bb4h
                    DQ 3e446f5f7f3c3e1ah
                    DQ 3e47055f86c9674bh
                    DQ 3e4b41a92b6b6e1ah
                    DQ 3e443d162e927628h
                    DQ 3e4466174013f9b1h
                    DQ 3e3b05096ad69c62h
                    DQ 3e40b169150faa58h
                    DQ 3e3cd98b1df85da7h
                    DQ 3e468b507b0f8fa8h
                    DQ 3e48422df57499bah
                    DQ 3e11351586970274h
                    DQ 3e117e08acba92eeh
                    DQ 3e26e04314dd0229h
                    DQ 3e497f3097e56d1ah
                    DQ 3e3356e655901286h
                    DQ 3e0cb761457f94d6h
                    DQ 3e39af67a85a9dach
                    DQ 3e453410931a909fh
                    DQ 3e22c587206058f5h
                    DQ 3e223bc358899c22h
                    DQ 3e4d7bf8b6d223cbh
                    DQ 3e47991ec5197ddbh
                    DQ 3e4a79e6bb3a9219h
                    DQ 3e3a4c43ed663ec5h
                    DQ 3e461b5a1484f438h
                    DQ 3e4b4e36f7ef0c3ah
                    DQ 3e115f026acd0d1bh
                    DQ 3e3f36b535cecf05h
                    DQ 3e2ffb7fbf3eb5c6h
                    DQ 3e3e6a6886b09760h
                    DQ 3e3135eb27f5bbc3h
                    DQ 3e470be7d6f6fa57h
                    DQ 3e4ce43cc84ab338h
                    DQ 3e4c01d7aac3bd91h
                    DQ 3e45c58d07961060h
                    DQ 3e3628bcf941456eh
                    DQ 3e4c58b2a8461cd2h
                    DQ 3e33071282fb989ah
                    DQ 3e420dab6a80f09ch
                    DQ 3e44f8d84c397b1eh
                    DQ 3e40d0ee08599e48h
                    DQ 3e1d68787e37da36h
                    DQ 3e366187d591bafch
                    DQ 3e22346600bae772h
                    DQ 3e390377d0d61b8eh
                    DQ 3e4f5e0dd966b907h
                    DQ 3e49023cb79a00e2h
                    DQ 3e44e05158c28ad8h
                    DQ 3e3bfa7b08b18ae4h
                    DQ 3e4ef1e63db35f67h
                    DQ 3e0ec2ae39493d4fh
                    DQ 3e40afe930ab2fa0h
                    DQ 3e225ff8a1810dd4h
                    DQ 3e469743fb1a71a5h
                    DQ 3e5f9cc676785571h
                    DQ 3e5b524da4cbf982h
                    DQ 3e5a4c8b381535b8h
                    DQ 3e5839be809caf2ch
                    DQ 3e50968a1cb82c13h
                    DQ 3e5eae6a41723fb5h
                    DQ 3e5d9c29a380a4dbh
                    DQ 3e4094aa0ada625eh
                    DQ 3e5973ad6fc108cah
                    DQ 3e4747322fdbab97h
                    DQ 3e593692fa9d4221h
                    DQ 3e5c5a992dfbc7d9h
                    DQ 3e4e1f33e102387ah
                    DQ 3e464fbef14c048ch
                    DQ 3e4490f513ca5e3bh
                    DQ 3e37a6af4d4c799dh
                    DQ 3e57574c1c07398fh
                    DQ 3e57b133417f8c1ch
                    DQ 3e5feb9e0c176514h
                    DQ 3e419f25bb3172f7h
                    DQ 3e45f68a7bbfb852h
                    DQ 3e5ee278497929f1h
                    DQ 3e5ccee006109d58h
                    DQ 3e5ce081a07bd8b3h
                    DQ 3e570e12981817b8h
                    DQ 3e292ab6d93503d0h
                    DQ 3e58cb7dd7c3b61eh
                    DQ 3e4efafd0a0b78dah
                    DQ 3e5e907267c4288eh
                    DQ 3e5d31ef96780875h
                    DQ 3e23430dfcd2ad50h
                    DQ 3e344d88d75bc1f9h
                    DQ 3e5bec0f055e04fch
                    DQ 3e5d85611590b9adh
                    DQ 3df320568e583229h
                    DQ 3e5a891d1772f538h
                    DQ 3e22edc9dabba74dh
                    DQ 3e4b9009a1015086h
                    DQ 3e52a12a8c5b1a19h
                    DQ 3e3a7885f0fdac85h
                    DQ 3e5f4ffcd43ac691h
                    DQ 3e52243ae2640aadh
                    DQ 3e546513299035d3h
                    DQ 3e5b39c3a62dd725h
                    DQ 3e5ba6dd40049f51h
                    DQ 3e451d1ed7177409h
                    DQ 3e5cb0f2fd7f5216h
                    DQ 3e3ab150cd4e2213h
                    DQ 3e5cfd7bf3193844h
                    DQ 3e53fff8455f1dbdh
                    DQ 3e5fee640b905fc9h
                    DQ 3e54e2adf548084ch
                    DQ 3e3b597adc1ecdd2h
                    DQ 3e4345bd096d3a75h
                    DQ 3e5101b9d2453c8bh
                    DQ 3e508ce55cc8c979h
                    DQ 3e5bbf017e595f71h
                    DQ 3e37ce733bd393dch
                    DQ 3e233bb0a503f8a1h
                    DQ 3e30e2f613e85bd9h
                    DQ 3e5e67555a635b3ch
                    DQ 3e2ea88df73d5e8bh
                    DQ 3e3d17e03bda18a8h
                    DQ 3e5b607d76044f7eh
                    DQ 3e52adc4e71bc2fch
                    DQ 3e5f99dc7362d1d9h
                    DQ 3e5473fa008e6a6ah
                    DQ 3e2b75bb09cb0985h
                    DQ 3e5ea04dd10b9abah
                    DQ 3e5802d0d6979674h
                    DQ 3e174688ccd99094h
                    DQ 3e496f16abb9df22h
                    DQ 3e46e66df2aa374fh
                    DQ 3e4e66525ea4550ah
                    DQ 3e42d02f34f20cbdh
                    DQ 3e46cfce65047188h
                    DQ 3e39b78c842d58b8h
                    DQ 3e4735e624c24bc9h
                    DQ 3e47eba1f7dd1adfh
                    DQ 3e586b3e59f65355h
                    DQ 3e1ce38e637f1b4dh
                    DQ 3e58d82ec919edc7h
                    DQ 3e4c52648ddcfa37h
                    DQ 3e52482ceae1ac12h
                    DQ 3e55a312311aba4fh
                    DQ 3e411e236329f225h
                    DQ 3e5b48c8cd2f246ch
                    DQ 3e6efa39ef35793ch
                    DQ 0000000000000000h

ALIGN 16
__log_F_inv_head:
                    DQ 4000000000000000h
                    DQ 3fffe00000000000h
                    DQ 3fffc00000000000h
                    DQ 3fffa00000000000h
                    DQ 3fff800000000000h
                    DQ 3fff600000000000h
                    DQ 3fff400000000000h
                    DQ 3fff200000000000h
                    DQ 3fff000000000000h
                    DQ 3ffee00000000000h
                    DQ 3ffec00000000000h
                    DQ 3ffea00000000000h
                    DQ 3ffe900000000000h
                    DQ 3ffe700000000000h
                    DQ 3ffe500000000000h
                    DQ 3ffe300000000000h
                    DQ 3ffe100000000000h
                    DQ 3ffe000000000000h
                    DQ 3ffde00000000000h
                    DQ 3ffdc00000000000h
                    DQ 3ffda00000000000h
                    DQ 3ffd900000000000h
                    DQ 3ffd700000000000h
                    DQ 3ffd500000000000h
                    DQ 3ffd400000000000h
                    DQ 3ffd200000000000h
                    DQ 3ffd000000000000h
                    DQ 3ffcf00000000000h
                    DQ 3ffcd00000000000h
                    DQ 3ffcb00000000000h
                    DQ 3ffca00000000000h
                    DQ 3ffc800000000000h
                    DQ 3ffc700000000000h
                    DQ 3ffc500000000000h
                    DQ 3ffc300000000000h
                    DQ 3ffc200000000000h
                    DQ 3ffc000000000000h
                    DQ 3ffbf00000000000h
                    DQ 3ffbd00000000000h
                    DQ 3ffbc00000000000h
                    DQ 3ffba00000000000h
                    DQ 3ffb900000000000h
                    DQ 3ffb700000000000h
                    DQ 3ffb600000000000h
                    DQ 3ffb400000000000h
                    DQ 3ffb300000000000h
                    DQ 3ffb200000000000h
                    DQ 3ffb000000000000h
                    DQ 3ffaf00000000000h
                    DQ 3ffad00000000000h
                    DQ 3ffac00000000000h
                    DQ 3ffaa00000000000h
                    DQ 3ffa900000000000h
                    DQ 3ffa800000000000h
                    DQ 3ffa600000000000h
                    DQ 3ffa500000000000h
                    DQ 3ffa400000000000h
                    DQ 3ffa200000000000h
                    DQ 3ffa100000000000h
                    DQ 3ffa000000000000h
                    DQ 3ff9e00000000000h
                    DQ 3ff9d00000000000h
                    DQ 3ff9c00000000000h
                    DQ 3ff9a00000000000h
                    DQ 3ff9900000000000h
                    DQ 3ff9800000000000h
                    DQ 3ff9700000000000h
                    DQ 3ff9500000000000h
                    DQ 3ff9400000000000h
                    DQ 3ff9300000000000h
                    DQ 3ff9200000000000h
                    DQ 3ff9000000000000h
                    DQ 3ff8f00000000000h
                    DQ 3ff8e00000000000h
                    DQ 3ff8d00000000000h
                    DQ 3ff8b00000000000h
                    DQ 3ff8a00000000000h
                    DQ 3ff8900000000000h
                    DQ 3ff8800000000000h
                    DQ 3ff8700000000000h
                    DQ 3ff8600000000000h
                    DQ 3ff8400000000000h
                    DQ 3ff8300000000000h
                    DQ 3ff8200000000000h
                    DQ 3ff8100000000000h
                    DQ 3ff8000000000000h
                    DQ 3ff7f00000000000h
                    DQ 3ff7e00000000000h
                    DQ 3ff7d00000000000h
                    DQ 3ff7b00000000000h
                    DQ 3ff7a00000000000h
                    DQ 3ff7900000000000h
                    DQ 3ff7800000000000h
                    DQ 3ff7700000000000h
                    DQ 3ff7600000000000h
                    DQ 3ff7500000000000h
                    DQ 3ff7400000000000h
                    DQ 3ff7300000000000h
                    DQ 3ff7200000000000h
                    DQ 3ff7100000000000h
                    DQ 3ff7000000000000h
                    DQ 3ff6f00000000000h
                    DQ 3ff6e00000000000h
                    DQ 3ff6d00000000000h
                    DQ 3ff6c00000000000h
                    DQ 3ff6b00000000000h
                    DQ 3ff6a00000000000h
                    DQ 3ff6900000000000h
                    DQ 3ff6800000000000h
                    DQ 3ff6700000000000h
                    DQ 3ff6600000000000h
                    DQ 3ff6500000000000h
                    DQ 3ff6400000000000h
                    DQ 3ff6300000000000h
                    DQ 3ff6200000000000h
                    DQ 3ff6100000000000h
                    DQ 3ff6000000000000h
                    DQ 3ff5f00000000000h
                    DQ 3ff5e00000000000h
                    DQ 3ff5d00000000000h
                    DQ 3ff5c00000000000h
                    DQ 3ff5b00000000000h
                    DQ 3ff5a00000000000h
                    DQ 3ff5900000000000h
                    DQ 3ff5800000000000h
                    DQ 3ff5800000000000h
                    DQ 3ff5700000000000h
                    DQ 3ff5600000000000h
                    DQ 3ff5500000000000h
                    DQ 3ff5400000000000h
                    DQ 3ff5300000000000h
                    DQ 3ff5200000000000h
                    DQ 3ff5100000000000h
                    DQ 3ff5000000000000h
                    DQ 3ff5000000000000h
                    DQ 3ff4f00000000000h
                    DQ 3ff4e00000000000h
                    DQ 3ff4d00000000000h
                    DQ 3ff4c00000000000h
                    DQ 3ff4b00000000000h
                    DQ 3ff4a00000000000h
                    DQ 3ff4a00000000000h
                    DQ 3ff4900000000000h
                    DQ 3ff4800000000000h
                    DQ 3ff4700000000000h
                    DQ 3ff4600000000000h
                    DQ 3ff4600000000000h
                    DQ 3ff4500000000000h
                    DQ 3ff4400000000000h
                    DQ 3ff4300000000000h
                    DQ 3ff4200000000000h
                    DQ 3ff4200000000000h
                    DQ 3ff4100000000000h
                    DQ 3ff4000000000000h
                    DQ 3ff3f00000000000h
                    DQ 3ff3e00000000000h
                    DQ 3ff3e00000000000h
                    DQ 3ff3d00000000000h
                    DQ 3ff3c00000000000h
                    DQ 3ff3b00000000000h
                    DQ 3ff3b00000000000h
                    DQ 3ff3a00000000000h
                    DQ 3ff3900000000000h
                    DQ 3ff3800000000000h
                    DQ 3ff3800000000000h
                    DQ 3ff3700000000000h
                    DQ 3ff3600000000000h
                    DQ 3ff3500000000000h
                    DQ 3ff3500000000000h
                    DQ 3ff3400000000000h
                    DQ 3ff3300000000000h
                    DQ 3ff3200000000000h
                    DQ 3ff3200000000000h
                    DQ 3ff3100000000000h
                    DQ 3ff3000000000000h
                    DQ 3ff3000000000000h
                    DQ 3ff2f00000000000h
                    DQ 3ff2e00000000000h
                    DQ 3ff2e00000000000h
                    DQ 3ff2d00000000000h
                    DQ 3ff2c00000000000h
                    DQ 3ff2b00000000000h
                    DQ 3ff2b00000000000h
                    DQ 3ff2a00000000000h
                    DQ 3ff2900000000000h
                    DQ 3ff2900000000000h
                    DQ 3ff2800000000000h
                    DQ 3ff2700000000000h
                    DQ 3ff2700000000000h
                    DQ 3ff2600000000000h
                    DQ 3ff2500000000000h
                    DQ 3ff2500000000000h
                    DQ 3ff2400000000000h
                    DQ 3ff2300000000000h
                    DQ 3ff2300000000000h
                    DQ 3ff2200000000000h
                    DQ 3ff2100000000000h
                    DQ 3ff2100000000000h
                    DQ 3ff2000000000000h
                    DQ 3ff2000000000000h
                    DQ 3ff1f00000000000h
                    DQ 3ff1e00000000000h
                    DQ 3ff1e00000000000h
                    DQ 3ff1d00000000000h
                    DQ 3ff1c00000000000h
                    DQ 3ff1c00000000000h
                    DQ 3ff1b00000000000h
                    DQ 3ff1b00000000000h
                    DQ 3ff1a00000000000h
                    DQ 3ff1900000000000h
                    DQ 3ff1900000000000h
                    DQ 3ff1800000000000h
                    DQ 3ff1800000000000h
                    DQ 3ff1700000000000h
                    DQ 3ff1600000000000h
                    DQ 3ff1600000000000h
                    DQ 3ff1500000000000h
                    DQ 3ff1500000000000h
                    DQ 3ff1400000000000h
                    DQ 3ff1300000000000h
                    DQ 3ff1300000000000h
                    DQ 3ff1200000000000h
                    DQ 3ff1200000000000h
                    DQ 3ff1100000000000h
                    DQ 3ff1100000000000h
                    DQ 3ff1000000000000h
                    DQ 3ff0f00000000000h
                    DQ 3ff0f00000000000h
                    DQ 3ff0e00000000000h
                    DQ 3ff0e00000000000h
                    DQ 3ff0d00000000000h
                    DQ 3ff0d00000000000h
                    DQ 3ff0c00000000000h
                    DQ 3ff0c00000000000h
                    DQ 3ff0b00000000000h
                    DQ 3ff0a00000000000h
                    DQ 3ff0a00000000000h
                    DQ 3ff0900000000000h
                    DQ 3ff0900000000000h
                    DQ 3ff0800000000000h
                    DQ 3ff0800000000000h
                    DQ 3ff0700000000000h
                    DQ 3ff0700000000000h
                    DQ 3ff0600000000000h
                    DQ 3ff0600000000000h
                    DQ 3ff0500000000000h
                    DQ 3ff0500000000000h
                    DQ 3ff0400000000000h
                    DQ 3ff0400000000000h
                    DQ 3ff0300000000000h
                    DQ 3ff0300000000000h
                    DQ 3ff0200000000000h
                    DQ 3ff0200000000000h
                    DQ 3ff0100000000000h
                    DQ 3ff0100000000000h
                    DQ 3ff0000000000000h
                    DQ 3ff0000000000000h

ALIGN 16
__log_F_inv_tail:
                    DQ 0000000000000000h
                    DQ 3effe01fe01fe020h
                    DQ 3f1fc07f01fc07f0h
                    DQ 3f31caa01fa11caah
                    DQ 3f3f81f81f81f820h
                    DQ 3f48856506ddaba6h
                    DQ 3f5196792909c560h
                    DQ 3f57d9108c2ad433h
                    DQ 3f5f07c1f07c1f08h
                    DQ 3f638ff08b1c03ddh
                    DQ 3f680f6603d980f6h
                    DQ 3f6d00f57403d5d0h
                    DQ 3f331abf0b7672a0h
                    DQ 3f506a965d43919bh
                    DQ 3f5ceb240795ceb2h
                    DQ 3f6522f3b834e67fh
                    DQ 3f6c3c3c3c3c3c3ch
                    DQ 3f3e01e01e01e01eh
                    DQ 3f575b8fe21a291ch
                    DQ 3f6403b9403b9404h
                    DQ 3f6cc0ed7303b5cch
                    DQ 3f479118f3fc4da2h
                    DQ 3f5ed952e0b0ce46h
                    DQ 3f695900eae56404h
                    DQ 3f3d41d41d41d41dh
                    DQ 3f5cb28ff16c69aeh
                    DQ 3f696b1edd80e866h
                    DQ 3f4372e225fe30d9h
                    DQ 3f60ad12073615a2h
                    DQ 3f6cdb2c0397cdb3h
                    DQ 3f52cc157b864407h
                    DQ 3f664cb5f7148404h
                    DQ 3f3c71c71c71c71ch
                    DQ 3f6129a21a930b84h
                    DQ 3f6f1e0387f1e038h
                    DQ 3f5ad4e4ba80709bh
                    DQ 3f6c0e070381c0e0h
                    DQ 3f560fba1a362bb0h
                    DQ 3f6a5713280dee96h
                    DQ 3f53f59620f9ece9h
                    DQ 3f69f22983759f23h
                    DQ 3f5478ac63fc8d5ch
                    DQ 3f6ad87bb4671656h
                    DQ 3f578b8efbb8148ch
                    DQ 3f6d0369d0369d03h
                    DQ 3f5d212b601b3748h
                    DQ 3f0b2036406c80d9h
                    DQ 3f629663b24547d1h
                    DQ 3f4435e50d79435eh
                    DQ 3f67d0ff2920bc03h
                    DQ 3f55c06b15c06b16h
                    DQ 3f6e3a5f0fd7f954h
                    DQ 3f61dec0d4c77b03h
                    DQ 3f473289870ac52eh
                    DQ 3f6a034da034da03h
                    DQ 3f5d041da2292856h
                    DQ 3f3a41a41a41a41ah
                    DQ 3f68550f8a39409dh
                    DQ 3f5b4fe5e92c0686h
                    DQ 3f3a01a01a01a01ah
                    DQ 3f691d2a2067b23ah
                    DQ 3f5e7c5dada0b4e5h
                    DQ 3f468a7725080ce1h
                    DQ 3f6c49d4aa21b490h
                    DQ 3f63333333333333h
                    DQ 3f54bc363b03fccfh
                    DQ 3f2c9f01970e4f81h
                    DQ 3f697617c6ef5b25h
                    DQ 3f6161f9add3c0cah
                    DQ 3f5319fe6cb39806h
                    DQ 3f2f693a1c451ab3h
                    DQ 3f6a9e240321a9e2h
                    DQ 3f63831f3831f383h
                    DQ 3f5949ebc4dcfc1ch
                    DQ 3f480c6980c6980ch
                    DQ 3f6f9d00c5fe7403h
                    DQ 3f69721ed7e75347h
                    DQ 3f6381ec0313381fh
                    DQ 3f5b97c2aec12653h
                    DQ 3f509ef3024ae3bah
                    DQ 3f38618618618618h
                    DQ 3f6e0184f00c2780h
                    DQ 3f692ef5657dba52h
                    DQ 3f64940305494030h
                    DQ 3f60303030303030h
                    DQ 3f58060180601806h
                    DQ 3f5017f405fd017fh
                    DQ 3f412a8ad278e8ddh
                    DQ 3f17d05f417d05f4h
                    DQ 3f6d67245c02f7d6h
                    DQ 3f6a4411c1d986a9h
                    DQ 3f6754d76c7316dfh
                    DQ 3f649902f149902fh
                    DQ 3f621023358c1a68h
                    DQ 3f5f7390d2a6c406h
                    DQ 3f5b2b0805d5b2b1h
                    DQ 3f5745d1745d1746h
                    DQ 3f53c31507fa32c4h
                    DQ 3f50a1fd1b7af017h
                    DQ 3f4bc36ce3e0453ah
                    DQ 3f4702e05c0b8170h
                    DQ 3f4300b79300b793h
                    DQ 3f3f76b4337c6cb1h
                    DQ 3f3a62681c860fb0h
                    DQ 3f36c16c16c16c17h
                    DQ 3f3490aa31a3cfc7h
                    DQ 3f33cd153729043eh
                    DQ 3f3473a88d0bfd2eh
                    DQ 3f36816816816817h
                    DQ 3f39f36016719f36h
                    DQ 3f3ec6a5122f9016h
                    DQ 3f427c29da5519cfh
                    DQ 3f4642c8590b2164h
                    DQ 3f4ab5c45606f00bh
                    DQ 3f4fd3b80b11fd3ch
                    DQ 3f52cda0c6ba4eaah
                    DQ 3f56058160581606h
                    DQ 3f5990d0a4b7ef87h
                    DQ 3f5d6ee340579d6fh
                    DQ 3f60cf87d9c54a69h
                    DQ 3f6310572620ae4ch
                    DQ 3f65798c8ff522a2h
                    DQ 3f680ad602b580adh
                    DQ 3f6ac3e24799546fh
                    DQ 3f6da46102b1da46h
                    DQ 3f15805601580560h
                    DQ 3f3ed3c506b39a23h
                    DQ 3f4cbdd3e2970f60h
                    DQ 3f55555555555555h
                    DQ 3f5c979aee0bf805h
                    DQ 3f621291e81fd58eh
                    DQ 3f65fead500a9580h
                    DQ 3f6a0fd5c5f02a3ah
                    DQ 3f6e45c223898adch
                    DQ 3f35015015015015h
                    DQ 3f4c7b16ea64d422h
                    DQ 3f57829cbc14e5e1h
                    DQ 3f60877db8589720h
                    DQ 3f65710e4b5edceah
                    DQ 3f6a7dbb4d1fc1c8h
                    DQ 3f6fad40a57eb503h
                    DQ 3f43fd6bb00a5140h
                    DQ 3f54e78ecb419ba9h
                    DQ 3f600a44029100a4h
                    DQ 3f65c28f5c28f5c3h
                    DQ 3f6b9c68b2c0cc4ah
                    DQ 3f2978feb9f34381h
                    DQ 3f4ecf163bb6500ah
                    DQ 3f5be1958b67ebb9h
                    DQ 3f644e6157dc9a3bh
                    DQ 3f6acc4baa3f0ddfh
                    DQ 3f26a4cbcb2a247bh
                    DQ 3f50505050505050h
                    DQ 3f5e0b4439959819h
                    DQ 3f66027f6027f602h
                    DQ 3f6d1e854b5e0db4h
                    DQ 3f4165e7254813e2h
                    DQ 3f576646a9d716efh
                    DQ 3f632b48f757ce88h
                    DQ 3f6ac1b24652a906h
                    DQ 3f33b13b13b13b14h
                    DQ 3f5490e1eb208984h
                    DQ 3f62385830fec66eh
                    DQ 3f6a45a6cc111b7eh
                    DQ 3f33813813813814h
                    DQ 3f556f472517b708h
                    DQ 3f631be7bc0e8f2ah
                    DQ 3f6b9cbf3e55f044h
                    DQ 3f40e7d95bc609a9h
                    DQ 3f59e6b3804d19e7h
                    DQ 3f65c8b6af7963c2h
                    DQ 3f6eb9dad43bf402h
                    DQ 3f4f1a515885fb37h
                    DQ 3f60eeb1d3d76c02h
                    DQ 3f6a320261a32026h
                    DQ 3f3c82ac40260390h
                    DQ 3f5a12f684bda12fh
                    DQ 3f669d43fda2962ch
                    DQ 3f02e025c04b8097h
                    DQ 3f542804b542804bh
                    DQ 3f63f69b02593f6ah
                    DQ 3f6df31cb46e21fah
                    DQ 3f5012b404ad012bh
                    DQ 3f623925e7820a7fh
                    DQ 3f6c8253c8253c82h
                    DQ 3f4b92ddc02526e5h
                    DQ 3f61602511602511h
                    DQ 3f6bf471439c9adfh
                    DQ 3f4a85c40939a85ch
                    DQ 3f6166f9ac024d16h
                    DQ 3f6c44e10125e227h
                    DQ 3f4cebf48bbd90e5h
                    DQ 3f62492492492492h
                    DQ 3f6d6f2e2ec0b673h
                    DQ 3f5159e26af37c05h
                    DQ 3f64024540245402h
                    DQ 3f6f6f0243f6f024h
                    DQ 3f55e60121579805h
                    DQ 3f668e18cf81b10fh
                    DQ 3f32012012012012h
                    DQ 3f5c11f7047dc11fh
                    DQ 3f69e878ff70985eh
                    DQ 3f4779d9fdc3a219h
                    DQ 3f61eace5c957907h
                    DQ 3f6e0d5b450239e1h
                    DQ 3f548bf073816367h
                    DQ 3f6694808dda5202h
                    DQ 3f37c67f2bae2b21h
                    DQ 3f5ee58469ee5847h
                    DQ 3f6c0233c0233c02h
                    DQ 3f514e02328a7012h
                    DQ 3f6561072057b573h
                    DQ 3f31811811811812h
                    DQ 3f5e28646f5a1060h
                    DQ 3f6c0d1284e6f1d7h
                    DQ 3f523543f0c80459h
                    DQ 3f663cbeea4e1a09h
                    DQ 3f3b9a3fdd5c8cb8h
                    DQ 3f60be1c159a76d2h
                    DQ 3f6e1d1a688e4838h
                    DQ 3f572044d72044d7h
                    DQ 3f691713db81577bh
                    DQ 3f4ac73ae9819b50h
                    DQ 3f6460334e904cf6h
                    DQ 3f31111111111111h
                    DQ 3f5feef80441fef0h
                    DQ 3f6de021fde021feh
                    DQ 3f57b7eacc9686a0h
                    DQ 3f69ead7cd391fbch
                    DQ 3f50195609804390h
                    DQ 3f6641511e8d2b32h
                    DQ 3f4222b1acf1ce96h
                    DQ 3f62e29f79b47582h
                    DQ 3f24f0d1682e11cdh
                    DQ 3f5f9bb096771e4dh
                    DQ 3f6e5ee45dd96ae2h
                    DQ 3f5a0429a0429a04h
                    DQ 3f6bb74d5f06c021h
                    DQ 3f54fce404254fceh
                    DQ 3f695766eacbc402h
                    DQ 3f50842108421084h
                    DQ 3f673e5371d5c338h
                    DQ 3f4930523fbe3368h
                    DQ 3f656b38f225f6c4h
                    DQ 3f426e978d4fdf3bh
                    DQ 3f63dd40e4eb0cc6h
                    DQ 3f397f7d73404146h
                    DQ 3f6293982cc98af1h
                    DQ 3f30410410410410h
                    DQ 3f618d6f048ff7e4h
                    DQ 3f2236a3ebc349deh
                    DQ 3f60c9f8ee53d18ch
                    DQ 3f10204081020408h
                    DQ 3f60486ca2f46ea6h
                    DQ 3ef0101010101010h
                    DQ 3f60080402010080h
                    DQ 0000000000000000h

;---------------------
; exp data
;---------------------

ALIGN 16

__denormal_threshold            DD 0fffffc02h ; -1022
                                DD 0
                                DQ 0

__enable_almost_inf             DQ 7fe0000000000000h
                                DQ 0

__real_zero                     DQ 0000000000000000h
                                DQ 0

__real_smallest_denormal        DQ 0000000000000001h
                                DQ 0
__denormal_tiny_threshold       DQ 0c0874046dfefd9d0h
                                DQ 0

__real_p65536                   DQ 40f0000000000000h    ; 65536
                                DQ 0
__real_m68800                   DQ 0c0f0cc0000000000h   ; -68800
                                DQ 0
__real_64_by_log2               DQ 40571547652b82feh    ; 64/ln(2)
                                DQ 0
__real_log2_by_64_head          DQ 3f862e42f0000000h    ; log2_by_64_head
                                DQ 0
__real_log2_by_64_tail          DQ 0bdfdf473de6af278h   ; -log2_by_64_tail
                                DQ 0
__real_1_by_720                 DQ 3f56c16c16c16c17h    ; 1/720
                                DQ 0
__real_1_by_120                 DQ 3f81111111111111h    ; 1/120
                                DQ 0
__real_1_by_24                  DQ 3fa5555555555555h    ; 1/24
                                DQ 0
__real_1_by_6                   DQ 3fc5555555555555h    ; 1/6
                                DQ 0
__real_1_by_2                   DQ 3fe0000000000000h    ; 1/2
                                DQ 0

ALIGN 16
__two_to_jby64_head_table:
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
__two_to_jby64_tail_table:
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
