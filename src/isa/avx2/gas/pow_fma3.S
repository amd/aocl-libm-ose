# pow.S
#
# An implementation of the pow libm function.
#
# Prototype:
#
#     double pow(double x, double y);
#

#
#   Algorithm:
#       x^y = e^(y*ln(x))
#
#       Look in exp, log for the respective algorithms
#
#include "log_scalar_tables.h"
#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_FMA3(pow)
#define fname_special _pow_special@PLT


# local variable storage offsets
.equ    save_x, 0x0
.equ    save_y, 0x10
.equ    p_temp_exp, 0x20
.equ    negate_result, 0x30
.equ    save_ax, 0x40
.equ    y_head, 0x50
.equ    p_temp_log, 0x60
.equ    stack_size, 0x78


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:

    sub         $stack_size, %rsp

    vmovsd       %xmm0, save_x(%rsp)
    vmovsd       %xmm1, save_y(%rsp)

    mov         save_x(%rsp), %rdx
    mov         save_y(%rsp), %r8

    mov         .L__exp_mant_mask(%rip), %r10
    and         %r8, %r10
    jz          .L__y_is_zero

    cmp         .L__pos_one(%rip), %r8
    je          .L__y_is_one

    mov         .L__sign_mask(%rip), %r9
    and         %rdx, %r9
    cmp         .L__sign_mask(%rip), %r9
    mov         .L__pos_zero(%rip), %rax
    mov         %rax, negate_result(%rsp)    
    je          .L__x_is_neg

    cmp         .L__pos_one(%rip), %rdx
    je          .L__x_is_pos_one

    cmp         .L__pos_zero(%rip), %rdx
    je          .L__x_is_zero

    mov         .L__exp_mask(%rip), %r9
    and         %rdx, %r9
    cmp         .L__exp_mask(%rip), %r9
    je          .L__x_is_inf_or_nan
   
    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__ay_max_bound(%rip), %r10
    jg          .L__ay_is_very_large

    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__ay_min_bound(%rip), %r10
    jl          .L__ay_is_very_small

    # -----------------------------
    # compute log(x) here
    # -----------------------------
.L__log_x:

    # compute exponent part
    xor         %r8, %r8
    vmovdqa      %xmm0, %xmm3
    vpsrlq       $52, %xmm3, %xmm3
    vmovd        %xmm0, %r8
    vpsubq       .L__mask_1023(%rip), %xmm3, %xmm3
    vmovdqa      %xmm0, %xmm2
    vcvtdq2pd    %xmm3, %xmm6 # xexp
    vpand        .L__real_mant(%rip), %xmm2, %xmm2

    vcomisd      .L__mask_1023_f(%rip), %xmm6
    je          .L__denormal_adjust

.L__continue_common:

    # compute index into the log tables

    mov         %r8, %r9
    and         .L__mask_mant_all10(%rip), %r8 #ux & .L__mask_mant_all8
    vsubsd       .L__real_one(%rip), %xmm0,%xmm7
    and         .L__mask_mant11(%rip), %r9
    shl         %r9
    add         %r9, %r8
    mov         %r8, p_temp_log(%rsp)
    vpand	.L__real_notsign(%rip), %xmm7, %xmm7

 # F, Y, switch to near-one codepath
    vmovsd       p_temp_log(%rsp), %xmm1
    shr         $42, %r8
    vpor         .L__real_half(%rip), %xmm2, %xmm2
    vpor         .L__real_half(%rip), %xmm1, %xmm1
    vcomisd      .L__real_threshold(%rip), %xmm7
    lea         .L__log_F_inv_head(%rip), %r9
    lea         .L__log_F_inv_tail(%rip), %rdx
    jb          .L__near_one

    # f = F - Y, r = f * inv
    vsubsd       %xmm2, %xmm1, %xmm1
    vmulsd       (%rdx,%r8,8), %xmm1, %xmm7    
    vmulsd       (%r9,%r8,8), %xmm1, %xmm1
   
    vaddsd       %xmm7, %xmm1, %xmm2
    vmulsd       %xmm2, %xmm2, %xmm0    
    vsubsd       %xmm2, %xmm1, %xmm5    
    
    lea         .L__log_1024_lead(%rip), %r9
    
    # poly
    vmovsd       .L__real_1_over_5(%rip), %xmm3
    vmovsd       .L__real_1_over_3(%rip), %xmm1
                      
    vfmadd213sd    .L__real_1_over_2(%rip), %xmm2, %xmm1 #r*1/3 + 1/2
    
    vmulsd       %xmm0, %xmm0, %xmm4     #r^4
    vmulsd       %xmm0, %xmm1, %xmm1     #1/2r^2 + 1/3*r^3

    vfmadd213sd  .L__real_1_over_4(%rip), %xmm2, %xmm3 #1/4 + r*1/5
    vfmadd231sd     %xmm4, %xmm3 ,%xmm1
    
    vaddsd       %xmm5, %xmm7, %xmm7
    vaddsd       %xmm7, %xmm1, %xmm1

    
    lea         .L__log_1024_tail(%rip), %rdx

    vmovsd       .L__real_log2_tail(%rip), %xmm5
    vfmsub213sd   %xmm1, %xmm6, %xmm5

    vaddsd       (%rdx,%r8,8),%xmm5,%xmm4
    

    vsubsd       %xmm2, %xmm4, %xmm3

    vmovsd       .L__real_log2_lead(%rip), %xmm7
    vmovsd       (%r9,%r8,8), %xmm0
    vfmadd231sd  %xmm7, %xmm6, %xmm0

    # result of ln(x) is computed from head and tail parts, resH and resT
    # res = ln(x) = resH + resT
    # resH and resT are in full precision 

    # resT is computed from head and tail parts, resT_h and resT_t
    # resT = resT_h + resT_t

    # now
    # xmm3 - resT
    # xmm0 - resH

    # loghi = resT + resH
    # loglo = resH - hi + resT
.L__log_x_continue:
    vmovsd      %xmm0, %xmm7, %xmm7
    vaddsd       %xmm3, %xmm0, %xmm0
    vsubsd       %xmm0, %xmm7, %xmm7
    vaddsd       %xmm7, %xmm3, %xmm6

    # Multiplication of y and (loghi + loglo)
    # v = y * loghi
    # tail = y * loglo
    # vt = tail + fma(y, loghi, -v)

    vmovsd  %xmm0, %xmm7, %xmm7
    vmovsd  save_y(%rsp), %xmm13
    vmulsd  %xmm13, %xmm0, %xmm0
    vmulsd  %xmm13, %xmm6, %xmm3
    vfmsub213sd %xmm0, %xmm13, %xmm7
    vaddsd  %xmm7, %xmm3, %xmm6

    # y * ln(x) = v + vt
    # v and vt are in full precision
 
    # xmm0 - v
    # xmm6 - vt

    # -----------------------------
    # compute exp( y * ln(x) ) here
    # -----------------------------

    # v * (64/ln(2))
    vmovsd       %xmm0, p_temp_exp(%rsp)
    vmulsd       .L__real_64_by_log2(%rip), %xmm0, %xmm7
    mov         p_temp_exp(%rsp), %rdx

    vpand	.L__real_notsign(%rip), %xmm7, %xmm4
    vmovq       %xmm4, %rax
    #check |x*(64/ln(2))| > 65536)
    cmp		.L__real_p65536(%rip), %rax
    jb          .L__exp_continue

    # v < 1024*ln(2), ( v * (64/ln(2)) ) < 64*1024
    # v >= -1075*ln(2), ( v * (64/ln(2)) ) >= 64*(-1075)
    vcomisd      .L__real_p65536(%rip), %xmm7
    ja          .L__process_result_inf

    vcomisd      .L__real_m68800(%rip), %xmm7
    jb          .L__process_result_zero

    # n = int( v * (64/ln(2)) )
    #This is done by fast double to int conversion.
    #Adding 2^52 + 2^51 and subtracting the same to remove fractional bits
.L__exp_continue:
    lea         .L__two_to_jby64_head_table(%rip), %r10
    lea         .L__two_to_jby64_tail_table(%rip), %r11

    vaddsd       .L__big_number(%rip), %xmm7, %xmm4
    vsubsd       .L__big_number(%rip), %xmm4, %xmm1
    # r1 = x - n * ln(2)/64 head
    
    vmovd        %xmm4, %ecx
    mov         $0x3f, %rax
    and         %ecx, %eax
    vfnmadd231sd .L__real_log2_by_64_head(%rip), %xmm1, %xmm0
    vmovsd       %xmm0, %xmm2, %xmm2
    

    # r2 = - n * ln(2)/64 tail
   


    # m = (n - j) / 64
    sub         %eax, %ecx
    sar         $6, %ecx

    # r1+r2
   
    vfmadd231sd  .L__real_log2_by_64_tail(%rip), %xmm1, %xmm2
    vaddsd       %xmm6, %xmm2, %xmm2 # add vt here
    

    # q
    # compute q + q^2/2! + q^3/3! + q^4/4! + q^5/5! + q^6/6!
    # The polynomial is being computed using Estrin's scheme in order to use independent fmas.

    vmovsd       .L__real_1_by_6(%rip), %xmm0
    vmovsd       .L__real_1_by_120(%rip), %xmm3
    vmovsd       .L__real_1_by_720(%rip), %xmm4
    vmulsd       %xmm2, %xmm2, %xmm1 #x^2
    
    vfmadd213sd    .L__real_1_by_2(%rip), %xmm2, %xmm0 #(1/2) + (1/6)*x
   
    vfmadd213sd  .L__real_1_by_24(%rip), %xmm2, %xmm3#1/24 + 1/120*x
    vmulsd       %xmm1, %xmm1, %xmm5 #x^4
   
    vfmadd213sd     %xmm2, %xmm1, %xmm0 #x+x^2*((1/2) + (1/6)*x))
   
    vfmadd213sd     %xmm3, %xmm1, %xmm4 #1/24 + 1/120*x + x^2*(1/720)

    # deal with denormal results
    xor         %r9d, %r9d
    cmp         .L__denormal_threshold(%rip), %ecx

   
    vfmadd213sd   %xmm0, %xmm5, %xmm4 #x+x^2*((1/2) + (1/6)*x)) + x^4*(1/24 + 1/120*x + x^2*(1/720))
    vmovsd        %xmm4, %xmm0, %xmm0

    cmovle      %ecx, %r9d
    add         $1023, %rcx
    shl         $52, %rcx

    # f1, f2
    vmovsd       (%r11,%rax,8), %xmm5
   
   

    cmp         .L__real_inf(%rip), %rcx

    # (f1+f2)*(1+q)
   
    vfmadd213sd  (%r11,%rax,8), %xmm0, %xmm5
  
    vfmadd231sd  (%r10,%rax,8), %xmm0, %xmm5
    vmovsd       %xmm5, %xmm1, %xmm1
    vaddsd       (%r10,%rax,8), %xmm1, %xmm1
    

    je          .L__process_almost_inf

    test        %r9d, %r9d
    mov         %rcx, p_temp_exp(%rsp)
    jnz         .L__process_denormal
    vmulsd       p_temp_exp(%rsp), %xmm1, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0

.L__final_check:
    add         $stack_size, %rsp
    ret

.p2align 4,,15
.L__process_almost_inf:
    vcomisd      .L__real_one(%rip), %xmm1
    jae         .L__process_result_inf

    vorpd        .L__enable_almost_inf(%rip), %xmm1, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__process_denormal:
    mov         %r9d, %ecx
    xor         %r11d, %r11d
    vcomisd      .L__real_one(%rip), %xmm1
    cmovae      %ecx, %r11d
    cmp         .L__denormal_threshold(%rip), %r11d
    jne         .L__process_true_denormal  

    vmulsd       p_temp_exp(%rsp), %xmm1, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__process_true_denormal:
    xor         %r8, %r8
    cmp         .L__denormal_tiny_threshold(%rip), %rdx
    mov         $1, %r9
    jg          .L__process_denormal_tiny
    add         $1074, %ecx
    cmovs       %r8, %rcx
    shl         %cl, %r9
    mov         %r9, %rcx

    mov         %rcx, p_temp_exp(%rsp)
    vmulsd       p_temp_exp(%rsp), %xmm1, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0
    jmp         .L__z_denormal        

.p2align 4,,15
.L__process_denormal_tiny:
    vmovsd       .L__real_smallest_denormal(%rip), %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0
    jmp         .L__z_denormal

.p2align 4,,15
.L__process_result_zero:
    mov         .L__real_zero(%rip), %r11
    or          negate_result(%rsp), %r11
    jmp         .L__z_is_zero_or_inf
 
.p2align 4,,15
.L__process_result_inf:
    mov         .L__real_inf(%rip), %r11
    or          negate_result(%rsp), %r11
    jmp         .L__z_is_zero_or_inf

.p2align 4,,15
.L__denormal_adjust:
    vpor         .L__real_one(%rip), %xmm2, %xmm2
    vsubsd       .L__real_one(%rip), %xmm2, %xmm5
    
    vpand        .L__real_mant(%rip), %xmm5, %xmm2
    vmovd        %xmm2, %r8
    vpsrlq       $52, %xmm5, %xmm5
    vpsubd       .L__mask_2045(%rip), %xmm5, %xmm5
    vcvtdq2pd    %xmm5, %xmm6
    jmp         .L__continue_common

.p2align 4,,15
.L__x_is_neg:

    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__ay_max_bound(%rip), %r10
    jg          .L__ay_is_very_large

    # determine if y is an integer
    mov         .L__exp_mant_mask(%rip), %r10
    and         %r8, %r10
    mov         %r10, %r11
    mov         .L__exp_shift(%rip), %rcx
    shr         %cl, %r10
    sub         .L__exp_bias(%rip), %r10
    js          .L__x_is_neg_y_is_not_int
   
    mov         .L__exp_mant_mask(%rip), %rax
    and         %rdx, %rax
    mov         %rax, save_ax(%rsp)

    cmp         .L__yexp_53(%rip), %r10
    mov         %r10, %rcx
    jg          .L__continue_after_y_int_check

    mov         .L__mant_full(%rip), %r9
    shr         %cl, %r9
    and         %r11, %r9
    jnz         .L__x_is_neg_y_is_not_int

    mov         .L__1_before_mant(%rip), %r9
    shr         %cl, %r9
    and         %r11, %r9
    jz          .L__continue_after_y_int_check

    mov         .L__sign_mask(%rip), %rax
    mov         %rax, negate_result(%rsp)    

.L__continue_after_y_int_check:

    cmp         .L__neg_zero(%rip), %rdx
    je          .L__x_is_zero

    cmp         .L__neg_one(%rip), %rdx
    je          .L__x_is_neg_one

    mov         .L__exp_mask(%rip), %r9
    and         %rdx, %r9
    cmp         .L__exp_mask(%rip), %r9
    je          .L__x_is_inf_or_nan
   
    vmovsd       save_ax(%rsp), %xmm0
    jmp         .L__log_x


.p2align 4,,15
.L__near_one:

    # f = F - Y, r = f * inv
    vmovsd       %xmm1, %xmm0, %xmm0
    vsubsd       %xmm2, %xmm1, %xmm1
    

    vmovsd       (%r9,%r8,8), %xmm3
    vaddsd       (%rdx,%r8,8), %xmm3, %xmm3
    vmulsd       %xmm3, %xmm1, %xmm4
    vandpd       .L__real_fffffffff8000000(%rip), %xmm4, %xmm5
    
   
    vfnmadd231sd %xmm5, %xmm0, %xmm1
    vmulsd       %xmm3, %xmm1, %xmm7
    
    vaddsd       %xmm5, %xmm7, %xmm1


    vmovsd       %xmm1, %xmm2, %xmm2

    
    

    lea         .L__log_256_lead(%rip), %r9

    # poly
    vmovsd       .L__real_1_over_7(%rip), %xmm3
    vmovsd       .L__real_1_over_4(%rip), %xmm1
                            
    vmulsd       %xmm2, %xmm2, %xmm0                      
    
    
    vfmadd213sd  .L__real_1_over_6(%rip), %xmm2, %xmm3
   
    vfmadd213sd  .L__real_1_over_3(%rip), %xmm2, %xmm1
    vmulsd       %xmm0, %xmm0, %xmm4                         
              
    vmulsd       %xmm2, %xmm1, %xmm1                         
  
    vfmadd213sd  .L__real_1_over_5(%rip), %xmm2, %xmm3
    vmulsd       %xmm2, %xmm3, %xmm3
    vmulsd       %xmm0, %xmm1, %xmm1
    vmulsd       %xmm4, %xmm3, %xmm3                        

    
    
    vmovsd       %xmm5, %xmm2, %xmm2
    vmulsd       %xmm7, %xmm7, %xmm0
    vmulsd       .L__real_1_over_2(%rip), %xmm0, %xmm0
   
    vfmadd213sd   %xmm0, %xmm7, %xmm5
    vaddsd       %xmm7, %xmm5, %xmm5

    
    vmovsd       %xmm2, %xmm7, %xmm7
    vmulsd       %xmm2, %xmm2, %xmm0
    vmulsd       .L__real_1_over_2(%rip), %xmm0, %xmm0
    
    vaddsd       %xmm0, %xmm2, %xmm2# r1 + r1^2/2
    vsubsd       %xmm2, %xmm7, %xmm7
    vaddsd       %xmm0, %xmm7, %xmm7

    vaddsd       %xmm7, %xmm3, %xmm3 
    vmovsd       .L__real_log2_tail(%rip), %xmm4
    vaddsd       %xmm3, %xmm1 , %xmm1                       
   
    lea         .L__log_256_tail(%rip), %rdx
    vaddsd       %xmm5, %xmm1, %xmm1
   
    vfmadd213sd   (%rdx,%r8,8), %xmm6, %xmm4
    vsubsd       %xmm1, %xmm4, %xmm4

   
    
    vsubsd       %xmm2, %xmm4, %xmm3

    
    vmovsd       .L__real_log2_lead(%rip), %xmm7
   
    vfmadd213sd     (%r9,%r8,8), %xmm7, %xmm6
    vmovsd          %xmm6, %xmm0, %xmm0

    jmp         .L__log_x_continue


.p2align 4,,15
.L__x_is_pos_one:
    xor         %rax, %rax
    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__exp_mask(%rip), %r10
    cmove       %r8, %rax
    mov         .L__mant_mask(%rip), %r10
    and         %rax, %r10
    jz          .L__final_check

    mov         .L__qnan_set(%rip), %r10
    and         %r8, %r10
    jnz         .L__final_check

    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovsd       .L__pos_one(%rip), %xmm2
    mov         .L__flag_x_one_y_snan(%rip), %edi

    call        fname_special
    jmp         .L__final_check

.p2align 4,,15
.L__y_is_zero:
    xor         %rax, %rax
    mov         .L__exp_mask(%rip), %r9
    mov         .L__real_one(%rip), %r11
    and         %rdx, %r9
    cmp         .L__exp_mask(%rip), %r9
    cmove       %rdx, %rax
    mov         .L__mant_mask(%rip), %r9
    and         %rax, %r9
    jnz         .L__x_is_nan

    vmovsd       .L__real_one(%rip), %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__y_is_one:
    xor         %rax, %rax
    mov         %rdx, %r11
    mov         .L__exp_mask(%rip), %r9
    or          .L__qnan_set(%rip), %r11
    and         %rdx, %r9
    cmp         .L__exp_mask(%rip), %r9
    cmove       %rdx, %rax
    mov         .L__mant_mask(%rip), %r9
    and         %rax, %r9
    jnz         .L__x_is_nan

    vmovd        %rdx, %xmm0 
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_neg_one:
    mov         .L__pos_one(%rip), %rdx
    or          negate_result(%rsp), %rdx
    xor         %rax, %rax
    mov         %r8, %r11
    mov         .L__exp_mask(%rip), %r10
    or          .L__qnan_set(%rip), %r11
    and         %r8, %r10
    cmp         .L__exp_mask(%rip), %r10
    cmove       %r8, %rax
    mov         .L__mant_mask(%rip), %r10
    and         %rax, %r10
    jnz         .L__y_is_nan

    vmovd        %rdx, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_neg_y_is_not_int:
    mov         .L__exp_mask(%rip), %r9
    and         %rdx, %r9
    cmp         .L__exp_mask(%rip), %r9
    je          .L__x_is_inf_or_nan

    cmp         .L__neg_zero(%rip), %rdx
    je          .L__x_is_zero

    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovsd       .L__qnan(%rip), %xmm2
    mov         .L__flag_x_neg_y_notint(%rip), %edi

    call        fname_special
    jmp         .L__final_check

.p2align 4,,15
.L__ay_is_very_large:
    mov         .L__exp_mask(%rip), %r9
    and         %rdx, %r9
    cmp         .L__exp_mask(%rip), %r9
    je          .L__x_is_inf_or_nan

    mov         .L__exp_mant_mask(%rip), %r9
    and         %rdx, %r9
    jz          .L__x_is_zero 

    cmp         .L__neg_one(%rip), %rdx
    je          .L__x_is_neg_one

    mov         %rdx, %r9
    and         .L__exp_mant_mask(%rip), %r9
    cmp         .L__pos_one(%rip), %r9
    jl          .L__ax_lt1_y_is_large_or_inf_or_nan
  
    jmp         .L__ax_gt1_y_is_large_or_inf_or_nan

.p2align 4,,15
.L__x_is_zero:
    mov         .L__exp_mask(%rip), %r10
    xor         %rax, %rax
    and         %r8, %r10
    cmp         .L__exp_mask(%rip), %r10
    je          .L__x_is_zero_y_is_inf_or_nan

    mov         .L__sign_mask(%rip), %r10
    and         %r8, %r10
    cmovnz      .L__pos_inf(%rip), %rax
    jnz         .L__x_is_zero_z_is_inf

    vmovd        %rax, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_zero_z_is_inf:

    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovd        %rax, %xmm2
    vorpd        negate_result(%rsp), %xmm2, %xmm2
    mov         .L__flag_x_zero_z_inf(%rip), %edi

    call        fname_special
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_zero_y_is_inf_or_nan:
    mov         %r8, %r11
    cmp         .L__neg_inf(%rip), %r8
    cmove       .L__pos_inf(%rip), %rax
    je          .L__x_is_zero_z_is_inf

    or          .L__qnan_set(%rip), %r11
    mov         .L__mant_mask(%rip), %r10
    and         %r8, %r10
    jnz         .L__y_is_nan

    vmovd        %rax, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_inf_or_nan:
    xor         %r11, %r11
    mov         .L__sign_mask(%rip), %r10
    and         %r8, %r10
    cmovz       .L__pos_inf(%rip), %r11
    mov         %rdx, %rax
    mov         .L__mant_mask(%rip), %r9
    or          .L__qnan_set(%rip), %rax
    and         %rdx, %r9
    cmovnz      %rax, %r11
    jnz         .L__x_is_nan

    xor         %rax, %rax
    mov         %r8, %r9
    mov         .L__exp_mask(%rip), %r10
    or          .L__qnan_set(%rip), %r9
    and         %r8, %r10
    cmp         .L__exp_mask(%rip), %r10
    cmove       %r8, %rax
    mov         .L__mant_mask(%rip), %r10
    and         %rax, %r10
    cmovnz      %r9, %r11
    jnz         .L__y_is_nan

    vmovd        %r11, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__ay_is_very_small:
    
    vaddsd       .L__pos_one(%rip), %xmm1,%xmm0
    jmp         .L__final_check


.p2align 4,,15
.L__ax_lt1_y_is_large_or_inf_or_nan:
    xor         %r11, %r11
    mov         .L__sign_mask(%rip), %r10
    and         %r8, %r10
    cmovnz      .L__pos_inf(%rip), %r11
    jmp         .L__adjust_for_nan

.p2align 4,,15
.L__ax_gt1_y_is_large_or_inf_or_nan:
    xor         %r11, %r11
    mov         .L__sign_mask(%rip), %r10
    and         %r8, %r10
    cmovz       .L__pos_inf(%rip), %r11

.p2align 4,,15
.L__adjust_for_nan:

    xor         %rax, %rax
    mov         %r8, %r9
    mov         .L__exp_mask(%rip), %r10
    or          .L__qnan_set(%rip), %r9
    and         %r8, %r10
    cmp         .L__exp_mask(%rip), %r10
    cmove       %r8, %rax
    mov         .L__mant_mask(%rip), %r10
    and         %rax, %r10
    cmovnz      %r9, %r11
    jnz         .L__y_is_nan

    test        %rax, %rax
    jnz         .L__y_is_inf

.p2align 4,,15
.L__z_is_zero_or_inf:

    mov         .L__flag_z_zero(%rip), %edi
    test        .L__exp_mant_mask(%rip), %r11
    cmovnz      .L__flag_z_inf(%rip), %edi
    
    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovd        %r11, %xmm2

    call        fname_special
    jmp         .L__final_check

.p2align 4,,15
.L__y_is_inf:

    vmovd        %r11, %xmm0
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_nan:

    xor         %rax, %rax
    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__exp_mask(%rip), %r10
    cmove       %r8, %rax
    mov         .L__mant_mask(%rip), %r10
    and         %rax, %r10
    jnz         .L__x_is_nan_y_is_nan

    mov         .L__qnan_set(%rip), %r9
    and         %rdx, %r9
    vmovd        %r11, %xmm0
    jnz         .L__final_check

    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovd        %r11, %xmm2
    mov         .L__flag_x_nan(%rip), %edi

    call        fname_special
    jmp         .L__final_check

.p2align 4,,15
.L__y_is_nan:

    mov         .L__qnan_set(%rip), %r10
    and         %r8, %r10
    vmovd        %r11, %xmm0
    jnz         .L__final_check

    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovd        %r11, %xmm2
    mov         .L__flag_y_nan(%rip), %edi

    call        fname_special
    jmp         .L__final_check

.p2align 4,,15
.L__x_is_nan_y_is_nan:

    mov         .L__qnan_set(%rip), %r9
    and         %rdx, %r9
    jz          .L__continue_xy_nan

    mov         .L__qnan_set(%rip), %r10
    and         %r8, %r10
    jz          .L__continue_xy_nan

    vmovd        %r11, %xmm0
    jmp         .L__final_check

.L__continue_xy_nan:    
    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    vmovd        %r11, %xmm2
    mov         .L__flag_x_nan_y_nan(%rip), %edi

    call        fname_special
    jmp         .L__final_check  
    
.p2align 4,,15
.L__z_denormal:
    
    vmovsd       %xmm0, %xmm2, %xmm2
    vmovsd       save_x(%rsp), %xmm0
    vmovsd       save_y(%rsp), %xmm1
    mov         .L__flag_z_denormal(%rip), %edi

    call        fname_special
    jmp         .L__final_check  


.section .rodata
.align 16

# these codes and the ones in the corresponding .c file have to match
.L__flag_x_one_y_snan:          .long 1
.L__flag_x_zero_z_inf:          .long 2
.L__flag_x_nan:                 .long 3
.L__flag_y_nan:                 .long 4
.L__flag_x_nan_y_nan:           .long 5
.L__flag_x_neg_y_notint:        .long 6
.L__flag_z_zero:                .long 7
.L__flag_z_denormal:            .long 8
.L__flag_z_inf:                 .long 9

.align 16

.L__ay_max_bound:           .quad 0x43e0000000000000
.L__ay_min_bound:           .quad 0x3c00000000000000
.L__sign_mask:              .quad 0x8000000000000000
.L__sign_and_exp_mask:      .quad 0x0fff0000000000000
.L__exp_mask:               .quad 0x7ff0000000000000
.L__neg_inf:                .quad 0x0fff0000000000000
.L__pos_inf:                .quad 0x7ff0000000000000
.L__pos_one:                .quad 0x3ff0000000000000
.L__pos_zero:               .quad 0x0000000000000000
.L__exp_mant_mask:          .quad 0x7fffffffffffffff
.L__mant_mask:              .quad 0x000fffffffffffff
.L__ind_pattern:            .quad 0x0fff8000000000000

.L__neg_qnan:               .quad 0x0fff8000000000000
.L__qnan:                   .quad 0x7ff8000000000000
.L__qnan_set:               .quad 0x0008000000000000

.L__neg_one:                .quad 0x0bff0000000000000
.L__neg_zero:               .quad 0x8000000000000000

.L__exp_shift:              .quad 0x0000000000000034 # 52
.L__exp_bias:               .quad 0x00000000000003ff # 1023
.L__exp_bias_m1:            .quad 0x00000000000003fe # 1022

.L__yexp_53:                .quad 0x0000000000000035 # 53
.L__mant_full:              .quad 0x000fffffffffffff
.L__1_before_mant:          .quad 0x0010000000000000

.L__mask_mant_all10:        .quad 0x000FFC0000000000
.L__mask_mant11:            .quad 0x0000020000000000

.align 16
.L__real_fffffffff8000000:  .quad 0x0fffffffff8000000
                            .quad 0x0fffffffff8000000

.L__mask_8000000000000000:  .quad 0x8000000000000000
                            .quad 0x8000000000000000

.L__real_4090040000000000:  .quad 0x4090040000000000
                            .quad 0x4090040000000000

.L__real_C090C80000000000:  .quad 0x0C090C80000000000
                            .quad 0x0C090C80000000000

#---------------------
# log data
#---------------------

.align 16

.L__real_ninf:  .quad 0x0fff0000000000000   # -inf
                .quad 0x0000000000000000
.L__real_inf:   .quad 0x7ff0000000000000    # +inf
                .quad 0x0000000000000000
.L__real_nan:   .quad 0x7ff8000000000000    # NaN
                .quad 0x0000000000000000
.L__real_mant:  .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                .quad 0x0000000000000000
.L__mask_1023:  .quad 0x00000000000003ff
                .quad 0x0000000000000000
.L__mask_001:   .quad 0x0000000000000001
                .quad 0x0000000000000000


.L__real_log2_lead: .quad 0x3fe62e42e0000000 # log2_lead  6.93147122859954833984e-01
                    .quad 0x0000000000000000
.L__real_log2_tail: .quad 0x3e6efa39ef35793c # log2_tail  5.76999904754328540596e-08
                    .quad 0x0000000000000000

.L__real_two:       .quad 0x4000000000000000 # 2
                    .quad 0x0000000000000000

.L__real_one:       .quad 0x3ff0000000000000 # 1
                    .quad 0x0000000000000000

.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x0000000000000000

.L__mask_100:       .quad 0x0000000000000100
                    .quad 0x0000000000000000

.L__real_1_over_2:  .quad 0x3fe0000000000000
                    .quad 0x0000000000000000
.L__real_1_over_3:  .quad 0x3fd5555555555555
                    .quad 0x0000000000000000
.L__real_1_over_4:  .quad 0x3fd0000000000000
                    .quad 0x0000000000000000
.L__real_1_over_5:  .quad 0x3fc999999999999a
                    .quad 0x0000000000000000
.L__real_1_over_6:  .quad 0x3fc5555555555555
                    .quad 0x0000000000000000
.L__real_1_over_7:  .quad 0x3fc2492492492494
                    .quad 0x0000000000000000

.L__mask_1023_f:    .quad 0x0c08ff80000000000
                    .quad 0x0000000000000000

.L__mask_2045:      .quad 0x00000000000007fd
                    .quad 0x0000000000000000

.L__real_threshold: .quad 0x3fc0000000000000 # 0.125
                    .quad 0x3fc0000000000000

.L__real_notsign:   .quad 0x7ffFFFFFFFFFFFFF # ^sign bit


.align 16
.L__log_256_lead:
                    .quad 0x0000000000000000
                    .quad 0x3f6ff00aa0000000
                    .quad 0x3f7fe02a60000000
                    .quad 0x3f87dc4750000000
                    .quad 0x3f8fc0a8b0000000
                    .quad 0x3f93cea440000000
                    .quad 0x3f97b91b00000000
                    .quad 0x3f9b9fc020000000
                    .quad 0x3f9f829b00000000
                    .quad 0x3fa1b0d980000000
                    .quad 0x3fa39e87b0000000
                    .quad 0x3fa58a5ba0000000
                    .quad 0x3fa77458f0000000
                    .quad 0x3fa95c8300000000
                    .quad 0x3fab42dd70000000
                    .quad 0x3fad276b80000000
                    .quad 0x3faf0a30c0000000
                    .quad 0x3fb0759830000000
                    .quad 0x3fb16536e0000000
                    .quad 0x3fb253f620000000
                    .quad 0x3fb341d790000000
                    .quad 0x3fb42edcb0000000
                    .quad 0x3fb51b0730000000
                    .quad 0x3fb60658a0000000
                    .quad 0x3fb6f0d280000000
                    .quad 0x3fb7da7660000000
                    .quad 0x3fb8c345d0000000
                    .quad 0x3fb9ab4240000000
                    .quad 0x3fba926d30000000
                    .quad 0x3fbb78c820000000
                    .quad 0x3fbc5e5480000000
                    .quad 0x3fbd4313d0000000
                    .quad 0x3fbe270760000000
                    .quad 0x3fbf0a30c0000000
                    .quad 0x3fbfec9130000000
                    .quad 0x3fc0671510000000
                    .quad 0x3fc0d77e70000000
                    .quad 0x3fc1478580000000
                    .quad 0x3fc1b72ad0000000
                    .quad 0x3fc2266f10000000
                    .quad 0x3fc29552f0000000
                    .quad 0x3fc303d710000000
                    .quad 0x3fc371fc20000000
                    .quad 0x3fc3dfc2b0000000
                    .quad 0x3fc44d2b60000000
                    .quad 0x3fc4ba36f0000000
                    .quad 0x3fc526e5e0000000
                    .quad 0x3fc59338d0000000
                    .quad 0x3fc5ff3070000000
                    .quad 0x3fc66acd40000000
                    .quad 0x3fc6d60fe0000000
                    .quad 0x3fc740f8f0000000
                    .quad 0x3fc7ab8900000000
                    .quad 0x3fc815c0a0000000
                    .quad 0x3fc87fa060000000
                    .quad 0x3fc8e928d0000000
                    .quad 0x3fc9525a90000000
                    .quad 0x3fc9bb3620000000
                    .quad 0x3fca23bc10000000
                    .quad 0x3fca8becf0000000
                    .quad 0x3fcaf3c940000000
                    .quad 0x3fcb5b5190000000
                    .quad 0x3fcbc28670000000
                    .quad 0x3fcc296850000000
                    .quad 0x3fcc8ff7c0000000
                    .quad 0x3fccf63540000000
                    .quad 0x3fcd5c2160000000
                    .quad 0x3fcdc1bca0000000
                    .quad 0x3fce270760000000
                    .quad 0x3fce8c0250000000
                    .quad 0x3fcef0adc0000000
                    .quad 0x3fcf550a50000000
                    .quad 0x3fcfb91860000000
                    .quad 0x3fd00e6c40000000
                    .quad 0x3fd0402590000000
                    .quad 0x3fd071b850000000
                    .quad 0x3fd0a324e0000000
                    .quad 0x3fd0d46b50000000
                    .quad 0x3fd1058bf0000000
                    .quad 0x3fd1368700000000
                    .quad 0x3fd1675ca0000000
                    .quad 0x3fd1980d20000000
                    .quad 0x3fd1c898c0000000
                    .quad 0x3fd1f8ff90000000
                    .quad 0x3fd22941f0000000
                    .quad 0x3fd2596010000000
                    .quad 0x3fd2895a10000000
                    .quad 0x3fd2b93030000000
                    .quad 0x3fd2e8e2b0000000
                    .quad 0x3fd31871c0000000
                    .quad 0x3fd347dd90000000
                    .quad 0x3fd3772660000000
                    .quad 0x3fd3a64c50000000
                    .quad 0x3fd3d54fa0000000
                    .quad 0x3fd4043080000000
                    .quad 0x3fd432ef20000000
                    .quad 0x3fd4618bc0000000
                    .quad 0x3fd4900680000000
                    .quad 0x3fd4be5f90000000
                    .quad 0x3fd4ec9730000000
                    .quad 0x3fd51aad80000000
                    .quad 0x3fd548a2c0000000
                    .quad 0x3fd5767710000000
                    .quad 0x3fd5a42ab0000000
                    .quad 0x3fd5d1bdb0000000
                    .quad 0x3fd5ff3070000000
                    .quad 0x3fd62c82f0000000
                    .quad 0x3fd659b570000000
                    .quad 0x3fd686c810000000
                    .quad 0x3fd6b3bb20000000
                    .quad 0x3fd6e08ea0000000
                    .quad 0x3fd70d42e0000000
                    .quad 0x3fd739d7f0000000
                    .quad 0x3fd7664e10000000
                    .quad 0x3fd792a550000000
                    .quad 0x3fd7bede00000000
                    .quad 0x3fd7eaf830000000
                    .quad 0x3fd816f410000000
                    .quad 0x3fd842d1d0000000
                    .quad 0x3fd86e9190000000
                    .quad 0x3fd89a3380000000
                    .quad 0x3fd8c5b7c0000000
                    .quad 0x3fd8f11e80000000
                    .quad 0x3fd91c67e0000000
                    .quad 0x3fd9479410000000
                    .quad 0x3fd972a340000000
                    .quad 0x3fd99d9580000000
                    .quad 0x3fd9c86b00000000
                    .quad 0x3fd9f323e0000000
                    .quad 0x3fda1dc060000000
                    .quad 0x3fda484090000000
                    .quad 0x3fda72a490000000
                    .quad 0x3fda9cec90000000
                    .quad 0x3fdac718c0000000
                    .quad 0x3fdaf12930000000
                    .quad 0x3fdb1b1e00000000
                    .quad 0x3fdb44f770000000
                    .quad 0x3fdb6eb590000000
                    .quad 0x3fdb985890000000
                    .quad 0x3fdbc1e080000000
                    .quad 0x3fdbeb4d90000000
                    .quad 0x3fdc149ff0000000
                    .quad 0x3fdc3dd7a0000000
                    .quad 0x3fdc66f4e0000000
                    .quad 0x3fdc8ff7c0000000
                    .quad 0x3fdcb8e070000000
                    .quad 0x3fdce1af00000000
                    .quad 0x3fdd0a63a0000000
                    .quad 0x3fdd32fe70000000
                    .quad 0x3fdd5b7f90000000
                    .quad 0x3fdd83e720000000
                    .quad 0x3fddac3530000000
                    .quad 0x3fddd46a00000000
                    .quad 0x3fddfc8590000000
                    .quad 0x3fde248810000000
                    .quad 0x3fde4c71a0000000
                    .quad 0x3fde744260000000
                    .quad 0x3fde9bfa60000000
                    .quad 0x3fdec399d0000000
                    .quad 0x3fdeeb20c0000000
                    .quad 0x3fdf128f50000000
                    .quad 0x3fdf39e5b0000000
                    .quad 0x3fdf6123f0000000
                    .quad 0x3fdf884a30000000
                    .quad 0x3fdfaf5880000000
                    .quad 0x3fdfd64f20000000
                    .quad 0x3fdffd2e00000000
                    .quad 0x3fe011fab0000000
                    .quad 0x3fe02552a0000000
                    .quad 0x3fe0389ee0000000
                    .quad 0x3fe04bdf90000000
                    .quad 0x3fe05f14b0000000
                    .quad 0x3fe0723e50000000
                    .quad 0x3fe0855c80000000
                    .quad 0x3fe0986f40000000
                    .quad 0x3fe0ab76b0000000
                    .quad 0x3fe0be72e0000000
                    .quad 0x3fe0d163c0000000
                    .quad 0x3fe0e44980000000
                    .quad 0x3fe0f72410000000
                    .quad 0x3fe109f390000000
                    .quad 0x3fe11cb810000000
                    .quad 0x3fe12f7190000000
                    .quad 0x3fe1422020000000
                    .quad 0x3fe154c3d0000000
                    .quad 0x3fe1675ca0000000
                    .quad 0x3fe179eab0000000
                    .quad 0x3fe18c6e00000000
                    .quad 0x3fe19ee6b0000000
                    .quad 0x3fe1b154b0000000
                    .quad 0x3fe1c3b810000000
                    .quad 0x3fe1d610f0000000
                    .quad 0x3fe1e85f50000000
                    .quad 0x3fe1faa340000000
                    .quad 0x3fe20cdcd0000000
                    .quad 0x3fe21f0bf0000000
                    .quad 0x3fe23130d0000000
                    .quad 0x3fe2434b60000000
                    .quad 0x3fe2555bc0000000
                    .quad 0x3fe2676200000000
                    .quad 0x3fe2795e10000000
                    .quad 0x3fe28b5000000000
                    .quad 0x3fe29d37f0000000
                    .quad 0x3fe2af15f0000000
                    .quad 0x3fe2c0e9e0000000
                    .quad 0x3fe2d2b400000000
                    .quad 0x3fe2e47430000000
                    .quad 0x3fe2f62a90000000
                    .quad 0x3fe307d730000000
                    .quad 0x3fe3197a00000000
                    .quad 0x3fe32b1330000000
                    .quad 0x3fe33ca2b0000000
                    .quad 0x3fe34e2890000000
                    .quad 0x3fe35fa4e0000000
                    .quad 0x3fe37117b0000000
                    .quad 0x3fe38280f0000000
                    .quad 0x3fe393e0d0000000
                    .quad 0x3fe3a53730000000
                    .quad 0x3fe3b68440000000
                    .quad 0x3fe3c7c7f0000000
                    .quad 0x3fe3d90260000000
                    .quad 0x3fe3ea3390000000
                    .quad 0x3fe3fb5b80000000
                    .quad 0x3fe40c7a40000000
                    .quad 0x3fe41d8fe0000000
                    .quad 0x3fe42e9c60000000
                    .quad 0x3fe43f9fe0000000
                    .quad 0x3fe4509a50000000
                    .quad 0x3fe4618bc0000000
                    .quad 0x3fe4727430000000
                    .quad 0x3fe48353d0000000
                    .quad 0x3fe4942a80000000
                    .quad 0x3fe4a4f850000000
                    .quad 0x3fe4b5bd60000000
                    .quad 0x3fe4c679a0000000
                    .quad 0x3fe4d72d30000000
                    .quad 0x3fe4e7d810000000
                    .quad 0x3fe4f87a30000000
                    .quad 0x3fe50913c0000000
                    .quad 0x3fe519a4c0000000
                    .quad 0x3fe52a2d20000000
                    .quad 0x3fe53aad00000000
                    .quad 0x3fe54b2460000000
                    .quad 0x3fe55b9350000000
                    .quad 0x3fe56bf9d0000000
                    .quad 0x3fe57c57f0000000
                    .quad 0x3fe58cadb0000000
                    .quad 0x3fe59cfb20000000
                    .quad 0x3fe5ad4040000000
                    .quad 0x3fe5bd7d30000000
                    .quad 0x3fe5cdb1d0000000
                    .quad 0x3fe5ddde50000000
                    .quad 0x3fe5ee02a0000000
                    .quad 0x3fe5fe1ed0000000
                    .quad 0x3fe60e32f0000000
                    .quad 0x3fe61e3ef0000000
                    .quad 0x3fe62e42e0000000
                    .quad 0x0000000000000000

.align 16
.L__log_256_tail:
                    .quad 0x0000000000000000
                    .quad 0x3db5885e0250435a
                    .quad 0x3de620cf11f86ed2
                    .quad 0x3dff0214edba4a25
                    .quad 0x3dbf807c79f3db4e
                    .quad 0x3dea352ba779a52b
                    .quad 0x3dff56c46aa49fd5
                    .quad 0x3dfebe465fef5196
                    .quad 0x3e0cf0660099f1f8
                    .quad 0x3e1247b2ff85945d
                    .quad 0x3e13fd7abf5202b6
                    .quad 0x3e1f91c9a918d51e
                    .quad 0x3e08cb73f118d3ca
                    .quad 0x3e1d91c7d6fad074
                    .quad 0x3de1971bec28d14c
                    .quad 0x3e15b616a423c78a
                    .quad 0x3da162a6617cc971
                    .quad 0x3e166391c4c06d29
                    .quad 0x3e2d46f5c1d0c4b8
                    .quad 0x3e2e14282df1f6d3
                    .quad 0x3e186f47424a660d
                    .quad 0x3e2d4c8de077753e
                    .quad 0x3e2e0c307ed24f1c
                    .quad 0x3e226ea18763bdd3
                    .quad 0x3e25cad69737c933
                    .quad 0x3e2af62599088901
                    .quad 0x3e18c66c83d6b2d0
                    .quad 0x3e1880ceb36fb30f
                    .quad 0x3e2495aac6ca17a4
                    .quad 0x3e2761db4210878c
                    .quad 0x3e2eb78e862bac2f
                    .quad 0x3e19b2cd75790dd9
                    .quad 0x3e2c55e5cbd3d50f
                    .quad 0x3db162a6617cc971
                    .quad 0x3dfdbeabaaa2e519
                    .quad 0x3e1652cb7150c647
                    .quad 0x3e39a11cb2cd2ee2
                    .quad 0x3e219d0ab1a28813
                    .quad 0x3e24bd9e80a41811
                    .quad 0x3e3214b596faa3df
                    .quad 0x3e303fea46980bb8
                    .quad 0x3e31c8ffa5fd28c7
                    .quad 0x3dce8f743bcd96c5
                    .quad 0x3dfd98c5395315c6
                    .quad 0x3e3996fa3ccfa7b2
                    .quad 0x3e1cd2af2ad13037
                    .quad 0x3e1d0da1bd17200e
                    .quad 0x3e3330410ba68b75
                    .quad 0x3df4f27a790e7c41
                    .quad 0x3e13956a86f6ff1b
                    .quad 0x3e2c6748723551d9
                    .quad 0x3e2500de9326cdfc
                    .quad 0x3e1086c848df1b59
                    .quad 0x3e04357ead6836ff
                    .quad 0x3e24832442408024
                    .quad 0x3e3d10da8154b13d
                    .quad 0x3e39e8ad68ec8260
                    .quad 0x3e3cfbf706abaf18
                    .quad 0x3e3fc56ac6326e23
                    .quad 0x3e39105e3185cf21
                    .quad 0x3e3d017fe5b19cc0
                    .quad 0x3e3d1f6b48dd13fe
                    .quad 0x3e20b63358a7e73a
                    .quad 0x3e263063028c211c
                    .quad 0x3e2e6a6886b09760
                    .quad 0x3e3c138bb891cd03
                    .quad 0x3e369f7722b7221a
                    .quad 0x3df57d8fac1a628c
                    .quad 0x3e3c55e5cbd3d50f
                    .quad 0x3e1552d2ff48fe2e
                    .quad 0x3e37b8b26ca431bc
                    .quad 0x3e292decdc1c5f6d
                    .quad 0x3e3abc7c551aaa8c
                    .quad 0x3e36b540731a354b
                    .quad 0x3e32d341036b89ef
                    .quad 0x3e4f9ab21a3a2e0f
                    .quad 0x3e239c871afb9fbd
                    .quad 0x3e3e6add2c81f640
                    .quad 0x3e435c95aa313f41
                    .quad 0x3e249d4582f6cc53
                    .quad 0x3e47574c1c07398f
                    .quad 0x3e4ba846dece9e8d
                    .quad 0x3e16999fafbc68e7
                    .quad 0x3e4c9145e51b0103
                    .quad 0x3e479ef2cb44850a
                    .quad 0x3e0beec73de11275
                    .quad 0x3e2ef4351af5a498
                    .quad 0x3e45713a493b4a50
                    .quad 0x3e45c23a61385992
                    .quad 0x3e42a88309f57299
                    .quad 0x3e4530faa9ac8ace
                    .quad 0x3e25fec2d792a758
                    .quad 0x3e35a517a71cbcd7
                    .quad 0x3e3707dc3e1cd9a3
                    .quad 0x3e3a1a9f8ef43049
                    .quad 0x3e4409d0276b3674
                    .quad 0x3e20e2f613e85bd9
                    .quad 0x3df0027433001e5f
                    .quad 0x3e35dde2836d3265
                    .quad 0x3e2300134d7aaf04
                    .quad 0x3e3cb7e0b42724f5
                    .quad 0x3e2d6e93167e6308
                    .quad 0x3e3d1569b1526adb
                    .quad 0x3e0e99fc338a1a41
                    .quad 0x3e4eb01394a11b1c
                    .quad 0x3e04f27a790e7c41
                    .quad 0x3e25ce3ca97b7af9
                    .quad 0x3e281f0f940ed857
                    .quad 0x3e4d36295d88857c
                    .quad 0x3e21aca1ec4af526
                    .quad 0x3e445743c7182726
                    .quad 0x3e23c491aead337e
                    .quad 0x3e3aef401a738931
                    .quad 0x3e21cede76092a29
                    .quad 0x3e4fba8f44f82bb4
                    .quad 0x3e446f5f7f3c3e1a
                    .quad 0x3e47055f86c9674b
                    .quad 0x3e4b41a92b6b6e1a
                    .quad 0x3e443d162e927628
                    .quad 0x3e4466174013f9b1
                    .quad 0x3e3b05096ad69c62
                    .quad 0x3e40b169150faa58
                    .quad 0x3e3cd98b1df85da7
                    .quad 0x3e468b507b0f8fa8
                    .quad 0x3e48422df57499ba
                    .quad 0x3e11351586970274
                    .quad 0x3e117e08acba92ee
                    .quad 0x3e26e04314dd0229
                    .quad 0x3e497f3097e56d1a
                    .quad 0x3e3356e655901286
                    .quad 0x3e0cb761457f94d6
                    .quad 0x3e39af67a85a9dac
                    .quad 0x3e453410931a909f
                    .quad 0x3e22c587206058f5
                    .quad 0x3e223bc358899c22
                    .quad 0x3e4d7bf8b6d223cb
                    .quad 0x3e47991ec5197ddb
                    .quad 0x3e4a79e6bb3a9219
                    .quad 0x3e3a4c43ed663ec5
                    .quad 0x3e461b5a1484f438
                    .quad 0x3e4b4e36f7ef0c3a
                    .quad 0x3e115f026acd0d1b
                    .quad 0x3e3f36b535cecf05
                    .quad 0x3e2ffb7fbf3eb5c6
                    .quad 0x3e3e6a6886b09760
                    .quad 0x3e3135eb27f5bbc3
                    .quad 0x3e470be7d6f6fa57
                    .quad 0x3e4ce43cc84ab338
                    .quad 0x3e4c01d7aac3bd91
                    .quad 0x3e45c58d07961060
                    .quad 0x3e3628bcf941456e
                    .quad 0x3e4c58b2a8461cd2
                    .quad 0x3e33071282fb989a
                    .quad 0x3e420dab6a80f09c
                    .quad 0x3e44f8d84c397b1e
                    .quad 0x3e40d0ee08599e48
                    .quad 0x3e1d68787e37da36
                    .quad 0x3e366187d591bafc
                    .quad 0x3e22346600bae772
                    .quad 0x3e390377d0d61b8e
                    .quad 0x3e4f5e0dd966b907
                    .quad 0x3e49023cb79a00e2
                    .quad 0x3e44e05158c28ad8
                    .quad 0x3e3bfa7b08b18ae4
                    .quad 0x3e4ef1e63db35f67
                    .quad 0x3e0ec2ae39493d4f
                    .quad 0x3e40afe930ab2fa0
                    .quad 0x3e225ff8a1810dd4
                    .quad 0x3e469743fb1a71a5
                    .quad 0x3e5f9cc676785571
                    .quad 0x3e5b524da4cbf982
                    .quad 0x3e5a4c8b381535b8
                    .quad 0x3e5839be809caf2c
                    .quad 0x3e50968a1cb82c13
                    .quad 0x3e5eae6a41723fb5
                    .quad 0x3e5d9c29a380a4db
                    .quad 0x3e4094aa0ada625e
                    .quad 0x3e5973ad6fc108ca
                    .quad 0x3e4747322fdbab97
                    .quad 0x3e593692fa9d4221
                    .quad 0x3e5c5a992dfbc7d9
                    .quad 0x3e4e1f33e102387a
                    .quad 0x3e464fbef14c048c
                    .quad 0x3e4490f513ca5e3b
                    .quad 0x3e37a6af4d4c799d
                    .quad 0x3e57574c1c07398f
                    .quad 0x3e57b133417f8c1c
                    .quad 0x3e5feb9e0c176514
                    .quad 0x3e419f25bb3172f7
                    .quad 0x3e45f68a7bbfb852
                    .quad 0x3e5ee278497929f1
                    .quad 0x3e5ccee006109d58
                    .quad 0x3e5ce081a07bd8b3
                    .quad 0x3e570e12981817b8
                    .quad 0x3e292ab6d93503d0
                    .quad 0x3e58cb7dd7c3b61e
                    .quad 0x3e4efafd0a0b78da
                    .quad 0x3e5e907267c4288e
                    .quad 0x3e5d31ef96780875
                    .quad 0x3e23430dfcd2ad50
                    .quad 0x3e344d88d75bc1f9
                    .quad 0x3e5bec0f055e04fc
                    .quad 0x3e5d85611590b9ad
                    .quad 0x3df320568e583229
                    .quad 0x3e5a891d1772f538
                    .quad 0x3e22edc9dabba74d
                    .quad 0x3e4b9009a1015086
                    .quad 0x3e52a12a8c5b1a19
                    .quad 0x3e3a7885f0fdac85
                    .quad 0x3e5f4ffcd43ac691
                    .quad 0x3e52243ae2640aad
                    .quad 0x3e546513299035d3
                    .quad 0x3e5b39c3a62dd725
                    .quad 0x3e5ba6dd40049f51
                    .quad 0x3e451d1ed7177409
                    .quad 0x3e5cb0f2fd7f5216
                    .quad 0x3e3ab150cd4e2213
                    .quad 0x3e5cfd7bf3193844
                    .quad 0x3e53fff8455f1dbd
                    .quad 0x3e5fee640b905fc9
                    .quad 0x3e54e2adf548084c
                    .quad 0x3e3b597adc1ecdd2
                    .quad 0x3e4345bd096d3a75
                    .quad 0x3e5101b9d2453c8b
                    .quad 0x3e508ce55cc8c979
                    .quad 0x3e5bbf017e595f71
                    .quad 0x3e37ce733bd393dc
                    .quad 0x3e233bb0a503f8a1
                    .quad 0x3e30e2f613e85bd9
                    .quad 0x3e5e67555a635b3c
                    .quad 0x3e2ea88df73d5e8b
                    .quad 0x3e3d17e03bda18a8
                    .quad 0x3e5b607d76044f7e
                    .quad 0x3e52adc4e71bc2fc
                    .quad 0x3e5f99dc7362d1d9
                    .quad 0x3e5473fa008e6a6a
                    .quad 0x3e2b75bb09cb0985
                    .quad 0x3e5ea04dd10b9aba
                    .quad 0x3e5802d0d6979674
                    .quad 0x3e174688ccd99094
                    .quad 0x3e496f16abb9df22
                    .quad 0x3e46e66df2aa374f
                    .quad 0x3e4e66525ea4550a
                    .quad 0x3e42d02f34f20cbd
                    .quad 0x3e46cfce65047188
                    .quad 0x3e39b78c842d58b8
                    .quad 0x3e4735e624c24bc9
                    .quad 0x3e47eba1f7dd1adf
                    .quad 0x3e586b3e59f65355
                    .quad 0x3e1ce38e637f1b4d
                    .quad 0x3e58d82ec919edc7
                    .quad 0x3e4c52648ddcfa37
                    .quad 0x3e52482ceae1ac12
                    .quad 0x3e55a312311aba4f
                    .quad 0x3e411e236329f225
                    .quad 0x3e5b48c8cd2f246c
                    .quad 0x3e6efa39ef35793c
                    .quad 0x0000000000000000

.align 16
.L__log_F_inv_head_old:
                    .quad 0x4000000000000000
                    .quad 0x3fffe00000000000
                    .quad 0x3fffc00000000000
                    .quad 0x3fffa00000000000
                    .quad 0x3fff800000000000
                    .quad 0x3fff600000000000
                    .quad 0x3fff400000000000
                    .quad 0x3fff200000000000
                    .quad 0x3fff000000000000
                    .quad 0x3ffee00000000000
                    .quad 0x3ffec00000000000
                    .quad 0x3ffea00000000000
                    .quad 0x3ffe900000000000
                    .quad 0x3ffe700000000000
                    .quad 0x3ffe500000000000
                    .quad 0x3ffe300000000000
                    .quad 0x3ffe100000000000
                    .quad 0x3ffe000000000000
                    .quad 0x3ffde00000000000
                    .quad 0x3ffdc00000000000
                    .quad 0x3ffda00000000000
                    .quad 0x3ffd900000000000
                    .quad 0x3ffd700000000000
                    .quad 0x3ffd500000000000
                    .quad 0x3ffd400000000000
                    .quad 0x3ffd200000000000
                    .quad 0x3ffd000000000000
                    .quad 0x3ffcf00000000000
                    .quad 0x3ffcd00000000000
                    .quad 0x3ffcb00000000000
                    .quad 0x3ffca00000000000
                    .quad 0x3ffc800000000000
                    .quad 0x3ffc700000000000
                    .quad 0x3ffc500000000000
                    .quad 0x3ffc300000000000
                    .quad 0x3ffc200000000000
                    .quad 0x3ffc000000000000
                    .quad 0x3ffbf00000000000
                    .quad 0x3ffbd00000000000
                    .quad 0x3ffbc00000000000
                    .quad 0x3ffba00000000000
                    .quad 0x3ffb900000000000
                    .quad 0x3ffb700000000000
                    .quad 0x3ffb600000000000
                    .quad 0x3ffb400000000000
                    .quad 0x3ffb300000000000
                    .quad 0x3ffb200000000000
                    .quad 0x3ffb000000000000
                    .quad 0x3ffaf00000000000
                    .quad 0x3ffad00000000000
                    .quad 0x3ffac00000000000
                    .quad 0x3ffaa00000000000
                    .quad 0x3ffa900000000000
                    .quad 0x3ffa800000000000
                    .quad 0x3ffa600000000000
                    .quad 0x3ffa500000000000
                    .quad 0x3ffa400000000000
                    .quad 0x3ffa200000000000
                    .quad 0x3ffa100000000000
                    .quad 0x3ffa000000000000
                    .quad 0x3ff9e00000000000
                    .quad 0x3ff9d00000000000
                    .quad 0x3ff9c00000000000
                    .quad 0x3ff9a00000000000
                    .quad 0x3ff9900000000000
                    .quad 0x3ff9800000000000
                    .quad 0x3ff9700000000000
                    .quad 0x3ff9500000000000
                    .quad 0x3ff9400000000000
                    .quad 0x3ff9300000000000
                    .quad 0x3ff9200000000000
                    .quad 0x3ff9000000000000
                    .quad 0x3ff8f00000000000
                    .quad 0x3ff8e00000000000
                    .quad 0x3ff8d00000000000
                    .quad 0x3ff8b00000000000
                    .quad 0x3ff8a00000000000
                    .quad 0x3ff8900000000000
                    .quad 0x3ff8800000000000
                    .quad 0x3ff8700000000000
                    .quad 0x3ff8600000000000
                    .quad 0x3ff8400000000000
                    .quad 0x3ff8300000000000
                    .quad 0x3ff8200000000000
                    .quad 0x3ff8100000000000
                    .quad 0x3ff8000000000000
                    .quad 0x3ff7f00000000000
                    .quad 0x3ff7e00000000000
                    .quad 0x3ff7d00000000000
                    .quad 0x3ff7b00000000000
                    .quad 0x3ff7a00000000000
                    .quad 0x3ff7900000000000
                    .quad 0x3ff7800000000000
                    .quad 0x3ff7700000000000
                    .quad 0x3ff7600000000000
                    .quad 0x3ff7500000000000
                    .quad 0x3ff7400000000000
                    .quad 0x3ff7300000000000
                    .quad 0x3ff7200000000000
                    .quad 0x3ff7100000000000
                    .quad 0x3ff7000000000000
                    .quad 0x3ff6f00000000000
                    .quad 0x3ff6e00000000000
                    .quad 0x3ff6d00000000000
                    .quad 0x3ff6c00000000000
                    .quad 0x3ff6b00000000000
                    .quad 0x3ff6a00000000000
                    .quad 0x3ff6900000000000
                    .quad 0x3ff6800000000000
                    .quad 0x3ff6700000000000
                    .quad 0x3ff6600000000000
                    .quad 0x3ff6500000000000
                    .quad 0x3ff6400000000000
                    .quad 0x3ff6300000000000
                    .quad 0x3ff6200000000000
                    .quad 0x3ff6100000000000
                    .quad 0x3ff6000000000000
                    .quad 0x3ff5f00000000000
                    .quad 0x3ff5e00000000000
                    .quad 0x3ff5d00000000000
                    .quad 0x3ff5c00000000000
                    .quad 0x3ff5b00000000000
                    .quad 0x3ff5a00000000000
                    .quad 0x3ff5900000000000
                    .quad 0x3ff5800000000000
                    .quad 0x3ff5800000000000
                    .quad 0x3ff5700000000000
                    .quad 0x3ff5600000000000
                    .quad 0x3ff5500000000000
                    .quad 0x3ff5400000000000
                    .quad 0x3ff5300000000000
                    .quad 0x3ff5200000000000
                    .quad 0x3ff5100000000000
                    .quad 0x3ff5000000000000
                    .quad 0x3ff5000000000000
                    .quad 0x3ff4f00000000000
                    .quad 0x3ff4e00000000000
                    .quad 0x3ff4d00000000000
                    .quad 0x3ff4c00000000000
                    .quad 0x3ff4b00000000000
                    .quad 0x3ff4a00000000000
                    .quad 0x3ff4a00000000000
                    .quad 0x3ff4900000000000
                    .quad 0x3ff4800000000000
                    .quad 0x3ff4700000000000
                    .quad 0x3ff4600000000000
                    .quad 0x3ff4600000000000
                    .quad 0x3ff4500000000000
                    .quad 0x3ff4400000000000
                    .quad 0x3ff4300000000000
                    .quad 0x3ff4200000000000
                    .quad 0x3ff4200000000000
                    .quad 0x3ff4100000000000
                    .quad 0x3ff4000000000000
                    .quad 0x3ff3f00000000000
                    .quad 0x3ff3e00000000000
                    .quad 0x3ff3e00000000000
                    .quad 0x3ff3d00000000000
                    .quad 0x3ff3c00000000000
                    .quad 0x3ff3b00000000000
                    .quad 0x3ff3b00000000000
                    .quad 0x3ff3a00000000000
                    .quad 0x3ff3900000000000
                    .quad 0x3ff3800000000000
                    .quad 0x3ff3800000000000
                    .quad 0x3ff3700000000000
                    .quad 0x3ff3600000000000
                    .quad 0x3ff3500000000000
                    .quad 0x3ff3500000000000
                    .quad 0x3ff3400000000000
                    .quad 0x3ff3300000000000
                    .quad 0x3ff3200000000000
                    .quad 0x3ff3200000000000
                    .quad 0x3ff3100000000000
                    .quad 0x3ff3000000000000
                    .quad 0x3ff3000000000000
                    .quad 0x3ff2f00000000000
                    .quad 0x3ff2e00000000000
                    .quad 0x3ff2e00000000000
                    .quad 0x3ff2d00000000000
                    .quad 0x3ff2c00000000000
                    .quad 0x3ff2b00000000000
                    .quad 0x3ff2b00000000000
                    .quad 0x3ff2a00000000000
                    .quad 0x3ff2900000000000
                    .quad 0x3ff2900000000000
                    .quad 0x3ff2800000000000
                    .quad 0x3ff2700000000000
                    .quad 0x3ff2700000000000
                    .quad 0x3ff2600000000000
                    .quad 0x3ff2500000000000
                    .quad 0x3ff2500000000000
                    .quad 0x3ff2400000000000
                    .quad 0x3ff2300000000000
                    .quad 0x3ff2300000000000
                    .quad 0x3ff2200000000000
                    .quad 0x3ff2100000000000
                    .quad 0x3ff2100000000000
                    .quad 0x3ff2000000000000
                    .quad 0x3ff2000000000000
                    .quad 0x3ff1f00000000000
                    .quad 0x3ff1e00000000000
                    .quad 0x3ff1e00000000000
                    .quad 0x3ff1d00000000000
                    .quad 0x3ff1c00000000000
                    .quad 0x3ff1c00000000000
                    .quad 0x3ff1b00000000000
                    .quad 0x3ff1b00000000000
                    .quad 0x3ff1a00000000000
                    .quad 0x3ff1900000000000
                    .quad 0x3ff1900000000000
                    .quad 0x3ff1800000000000
                    .quad 0x3ff1800000000000
                    .quad 0x3ff1700000000000
                    .quad 0x3ff1600000000000
                    .quad 0x3ff1600000000000
                    .quad 0x3ff1500000000000
                    .quad 0x3ff1500000000000
                    .quad 0x3ff1400000000000
                    .quad 0x3ff1300000000000
                    .quad 0x3ff1300000000000
                    .quad 0x3ff1200000000000
                    .quad 0x3ff1200000000000
                    .quad 0x3ff1100000000000
                    .quad 0x3ff1100000000000
                    .quad 0x3ff1000000000000
                    .quad 0x3ff0f00000000000
                    .quad 0x3ff0f00000000000
                    .quad 0x3ff0e00000000000
                    .quad 0x3ff0e00000000000
                    .quad 0x3ff0d00000000000
                    .quad 0x3ff0d00000000000
                    .quad 0x3ff0c00000000000
                    .quad 0x3ff0c00000000000
                    .quad 0x3ff0b00000000000
                    .quad 0x3ff0a00000000000
                    .quad 0x3ff0a00000000000
                    .quad 0x3ff0900000000000
                    .quad 0x3ff0900000000000
                    .quad 0x3ff0800000000000
                    .quad 0x3ff0800000000000
                    .quad 0x3ff0700000000000
                    .quad 0x3ff0700000000000
                    .quad 0x3ff0600000000000
                    .quad 0x3ff0600000000000
                    .quad 0x3ff0500000000000
                    .quad 0x3ff0500000000000
                    .quad 0x3ff0400000000000
                    .quad 0x3ff0400000000000
                    .quad 0x3ff0300000000000
                    .quad 0x3ff0300000000000
                    .quad 0x3ff0200000000000
                    .quad 0x3ff0200000000000
                    .quad 0x3ff0100000000000
                    .quad 0x3ff0100000000000
                    .quad 0x3ff0000000000000
                    .quad 0x3ff0000000000000

.align 16
.L__log_F_inv_tail_old:
                    .quad 0x0000000000000000
                    .quad 0x3effe01fe01fe020
                    .quad 0x3f1fc07f01fc07f0
                    .quad 0x3f31caa01fa11caa
                    .quad 0x3f3f81f81f81f820
                    .quad 0x3f48856506ddaba6
                    .quad 0x3f5196792909c560
                    .quad 0x3f57d9108c2ad433
                    .quad 0x3f5f07c1f07c1f08
                    .quad 0x3f638ff08b1c03dd
                    .quad 0x3f680f6603d980f6
                    .quad 0x3f6d00f57403d5d0
                    .quad 0x3f331abf0b7672a0
                    .quad 0x3f506a965d43919b
                    .quad 0x3f5ceb240795ceb2
                    .quad 0x3f6522f3b834e67f
                    .quad 0x3f6c3c3c3c3c3c3c
                    .quad 0x3f3e01e01e01e01e
                    .quad 0x3f575b8fe21a291c
                    .quad 0x3f6403b9403b9404
                    .quad 0x3f6cc0ed7303b5cc
                    .quad 0x3f479118f3fc4da2
                    .quad 0x3f5ed952e0b0ce46
                    .quad 0x3f695900eae56404
                    .quad 0x3f3d41d41d41d41d
                    .quad 0x3f5cb28ff16c69ae
                    .quad 0x3f696b1edd80e866
                    .quad 0x3f4372e225fe30d9
                    .quad 0x3f60ad12073615a2
                    .quad 0x3f6cdb2c0397cdb3
                    .quad 0x3f52cc157b864407
                    .quad 0x3f664cb5f7148404
                    .quad 0x3f3c71c71c71c71c
                    .quad 0x3f6129a21a930b84
                    .quad 0x3f6f1e0387f1e038
                    .quad 0x3f5ad4e4ba80709b
                    .quad 0x3f6c0e070381c0e0
                    .quad 0x3f560fba1a362bb0
                    .quad 0x3f6a5713280dee96
                    .quad 0x3f53f59620f9ece9
                    .quad 0x3f69f22983759f23
                    .quad 0x3f5478ac63fc8d5c
                    .quad 0x3f6ad87bb4671656
                    .quad 0x3f578b8efbb8148c
                    .quad 0x3f6d0369d0369d03
                    .quad 0x3f5d212b601b3748
                    .quad 0x3f0b2036406c80d9
                    .quad 0x3f629663b24547d1
                    .quad 0x3f4435e50d79435e
                    .quad 0x3f67d0ff2920bc03
                    .quad 0x3f55c06b15c06b16
                    .quad 0x3f6e3a5f0fd7f954
                    .quad 0x3f61dec0d4c77b03
                    .quad 0x3f473289870ac52e
                    .quad 0x3f6a034da034da03
                    .quad 0x3f5d041da2292856
                    .quad 0x3f3a41a41a41a41a
                    .quad 0x3f68550f8a39409d
                    .quad 0x3f5b4fe5e92c0686
                    .quad 0x3f3a01a01a01a01a
                    .quad 0x3f691d2a2067b23a
                    .quad 0x3f5e7c5dada0b4e5
                    .quad 0x3f468a7725080ce1
                    .quad 0x3f6c49d4aa21b490
                    .quad 0x3f63333333333333
                    .quad 0x3f54bc363b03fccf
                    .quad 0x3f2c9f01970e4f81
                    .quad 0x3f697617c6ef5b25
                    .quad 0x3f6161f9add3c0ca
                    .quad 0x3f5319fe6cb39806
                    .quad 0x3f2f693a1c451ab3
                    .quad 0x3f6a9e240321a9e2
                    .quad 0x3f63831f3831f383
                    .quad 0x3f5949ebc4dcfc1c
                    .quad 0x3f480c6980c6980c
                    .quad 0x3f6f9d00c5fe7403
                    .quad 0x3f69721ed7e75347
                    .quad 0x3f6381ec0313381f
                    .quad 0x3f5b97c2aec12653
                    .quad 0x3f509ef3024ae3ba
                    .quad 0x3f38618618618618
                    .quad 0x3f6e0184f00c2780
                    .quad 0x3f692ef5657dba52
                    .quad 0x3f64940305494030
                    .quad 0x3f60303030303030
                    .quad 0x3f58060180601806
                    .quad 0x3f5017f405fd017f
                    .quad 0x3f412a8ad278e8dd
                    .quad 0x3f17d05f417d05f4
                    .quad 0x3f6d67245c02f7d6
                    .quad 0x3f6a4411c1d986a9
                    .quad 0x3f6754d76c7316df
                    .quad 0x3f649902f149902f
                    .quad 0x3f621023358c1a68
                    .quad 0x3f5f7390d2a6c406
                    .quad 0x3f5b2b0805d5b2b1
                    .quad 0x3f5745d1745d1746
                    .quad 0x3f53c31507fa32c4
                    .quad 0x3f50a1fd1b7af017
                    .quad 0x3f4bc36ce3e0453a
                    .quad 0x3f4702e05c0b8170
                    .quad 0x3f4300b79300b793
                    .quad 0x3f3f76b4337c6cb1
                    .quad 0x3f3a62681c860fb0
                    .quad 0x3f36c16c16c16c17
                    .quad 0x3f3490aa31a3cfc7
                    .quad 0x3f33cd153729043e
                    .quad 0x3f3473a88d0bfd2e
                    .quad 0x3f36816816816817
                    .quad 0x3f39f36016719f36
                    .quad 0x3f3ec6a5122f9016
                    .quad 0x3f427c29da5519cf
                    .quad 0x3f4642c8590b2164
                    .quad 0x3f4ab5c45606f00b
                    .quad 0x3f4fd3b80b11fd3c
                    .quad 0x3f52cda0c6ba4eaa
                    .quad 0x3f56058160581606
                    .quad 0x3f5990d0a4b7ef87
                    .quad 0x3f5d6ee340579d6f
                    .quad 0x3f60cf87d9c54a69
                    .quad 0x3f6310572620ae4c
                    .quad 0x3f65798c8ff522a2
                    .quad 0x3f680ad602b580ad
                    .quad 0x3f6ac3e24799546f
                    .quad 0x3f6da46102b1da46
                    .quad 0x3f15805601580560
                    .quad 0x3f3ed3c506b39a23
                    .quad 0x3f4cbdd3e2970f60
                    .quad 0x3f55555555555555
                    .quad 0x3f5c979aee0bf805
                    .quad 0x3f621291e81fd58e
                    .quad 0x3f65fead500a9580
                    .quad 0x3f6a0fd5c5f02a3a
                    .quad 0x3f6e45c223898adc
                    .quad 0x3f35015015015015
                    .quad 0x3f4c7b16ea64d422
                    .quad 0x3f57829cbc14e5e1
                    .quad 0x3f60877db8589720
                    .quad 0x3f65710e4b5edcea
                    .quad 0x3f6a7dbb4d1fc1c8
                    .quad 0x3f6fad40a57eb503
                    .quad 0x3f43fd6bb00a5140
                    .quad 0x3f54e78ecb419ba9
                    .quad 0x3f600a44029100a4
                    .quad 0x3f65c28f5c28f5c3
                    .quad 0x3f6b9c68b2c0cc4a
                    .quad 0x3f2978feb9f34381
                    .quad 0x3f4ecf163bb6500a
                    .quad 0x3f5be1958b67ebb9
                    .quad 0x3f644e6157dc9a3b
                    .quad 0x3f6acc4baa3f0ddf
                    .quad 0x3f26a4cbcb2a247b
                    .quad 0x3f50505050505050
                    .quad 0x3f5e0b4439959819
                    .quad 0x3f66027f6027f602
                    .quad 0x3f6d1e854b5e0db4
                    .quad 0x3f4165e7254813e2
                    .quad 0x3f576646a9d716ef
                    .quad 0x3f632b48f757ce88
                    .quad 0x3f6ac1b24652a906
                    .quad 0x3f33b13b13b13b14
                    .quad 0x3f5490e1eb208984
                    .quad 0x3f62385830fec66e
                    .quad 0x3f6a45a6cc111b7e
                    .quad 0x3f33813813813814
                    .quad 0x3f556f472517b708
                    .quad 0x3f631be7bc0e8f2a
                    .quad 0x3f6b9cbf3e55f044
                    .quad 0x3f40e7d95bc609a9
                    .quad 0x3f59e6b3804d19e7
                    .quad 0x3f65c8b6af7963c2
                    .quad 0x3f6eb9dad43bf402
                    .quad 0x3f4f1a515885fb37
                    .quad 0x3f60eeb1d3d76c02
                    .quad 0x3f6a320261a32026
                    .quad 0x3f3c82ac40260390
                    .quad 0x3f5a12f684bda12f
                    .quad 0x3f669d43fda2962c
                    .quad 0x3f02e025c04b8097
                    .quad 0x3f542804b542804b
                    .quad 0x3f63f69b02593f6a
                    .quad 0x3f6df31cb46e21fa
                    .quad 0x3f5012b404ad012b
                    .quad 0x3f623925e7820a7f
                    .quad 0x3f6c8253c8253c82
                    .quad 0x3f4b92ddc02526e5
                    .quad 0x3f61602511602511
                    .quad 0x3f6bf471439c9adf
                    .quad 0x3f4a85c40939a85c
                    .quad 0x3f6166f9ac024d16
                    .quad 0x3f6c44e10125e227
                    .quad 0x3f4cebf48bbd90e5
                    .quad 0x3f62492492492492
                    .quad 0x3f6d6f2e2ec0b673
                    .quad 0x3f5159e26af37c05
                    .quad 0x3f64024540245402
                    .quad 0x3f6f6f0243f6f024
                    .quad 0x3f55e60121579805
                    .quad 0x3f668e18cf81b10f
                    .quad 0x3f32012012012012
                    .quad 0x3f5c11f7047dc11f
                    .quad 0x3f69e878ff70985e
                    .quad 0x3f4779d9fdc3a219
                    .quad 0x3f61eace5c957907
                    .quad 0x3f6e0d5b450239e1
                    .quad 0x3f548bf073816367
                    .quad 0x3f6694808dda5202
                    .quad 0x3f37c67f2bae2b21
                    .quad 0x3f5ee58469ee5847
                    .quad 0x3f6c0233c0233c02
                    .quad 0x3f514e02328a7012
                    .quad 0x3f6561072057b573
                    .quad 0x3f31811811811812
                    .quad 0x3f5e28646f5a1060
                    .quad 0x3f6c0d1284e6f1d7
                    .quad 0x3f523543f0c80459
                    .quad 0x3f663cbeea4e1a09
                    .quad 0x3f3b9a3fdd5c8cb8
                    .quad 0x3f60be1c159a76d2
                    .quad 0x3f6e1d1a688e4838
                    .quad 0x3f572044d72044d7
                    .quad 0x3f691713db81577b
                    .quad 0x3f4ac73ae9819b50
                    .quad 0x3f6460334e904cf6
                    .quad 0x3f31111111111111
                    .quad 0x3f5feef80441fef0
                    .quad 0x3f6de021fde021fe
                    .quad 0x3f57b7eacc9686a0
                    .quad 0x3f69ead7cd391fbc
                    .quad 0x3f50195609804390
                    .quad 0x3f6641511e8d2b32
                    .quad 0x3f4222b1acf1ce96
                    .quad 0x3f62e29f79b47582
                    .quad 0x3f24f0d1682e11cd
                    .quad 0x3f5f9bb096771e4d
                    .quad 0x3f6e5ee45dd96ae2
                    .quad 0x3f5a0429a0429a04
                    .quad 0x3f6bb74d5f06c021
                    .quad 0x3f54fce404254fce
                    .quad 0x3f695766eacbc402
                    .quad 0x3f50842108421084
                    .quad 0x3f673e5371d5c338
                    .quad 0x3f4930523fbe3368
                    .quad 0x3f656b38f225f6c4
                    .quad 0x3f426e978d4fdf3b
                    .quad 0x3f63dd40e4eb0cc6
                    .quad 0x3f397f7d73404146
                    .quad 0x3f6293982cc98af1
                    .quad 0x3f30410410410410
                    .quad 0x3f618d6f048ff7e4
                    .quad 0x3f2236a3ebc349de
                    .quad 0x3f60c9f8ee53d18c
                    .quad 0x3f10204081020408
                    .quad 0x3f60486ca2f46ea6
                    .quad 0x3ef0101010101010
                    .quad 0x3f60080402010080
                    .quad 0x0000000000000000

#---------------------
# exp data
#---------------------

.align 16

.L__denormal_threshold:         .long 0x0fffffc02 # -1022
                                .long 0
                                .quad 0

.L__enable_almost_inf:          .quad 0x7fe0000000000000
                                .quad 0

.L__real_zero:                  .quad 0x0000000000000000
                                .quad 0

.L__real_smallest_denormal:     .quad 0x0000000000000001
                                .quad 0
.L__denormal_tiny_threshold:    .quad 0x0c0874046dfefd9d0
                                .quad 0

.L__real_p65536:                .quad 0x40f0000000000000    # 65536
                                .quad 0
.L__real_m68800:                .quad 0x0c0f0cc0000000000   # -68800
                                .quad 0
.L__big_number:		        .quad 0x4338000000000000
#.L__const_1024_by_log2:         .quad 0x40971547652b82fe    # 1024/ln(2)
                                .quad 0
#.L__real_log2_by_64_head:       .quad 0x3f862e42f0000000    # log2_by_64_head
#                                .quad 0
#.L__const_log2_by_1024_head:	.quad 0x3F462E42F0000000
#.L__const_log2_by_1024_tail:    .quad 0xbdbdf473de6af279   # -log2_by_1024_tail
.L__real_64_by_log2:            .quad 0x40571547652b82fe    # 64/ln(2)
                                .quad 0
.L__real_log2_by_64_head:       .quad 0x3f862e42f0000000    # log2_by_64_head
                                .quad 0
.L__real_log2_by_64_tail:       .quad 0x0bdfdf473de6af278   # -log2_by_64_tail
                                .quad 0
.L__real_1_by_720:              .quad 0x3f56c16c16c16c17    # 1/720
                                .quad 0
.L__real_1_by_120:              .quad 0x3f81111111111111    # 1/120
                                .quad 0
.L__real_1_by_24:               .quad 0x3fa5555555555555    # 1/24
                                .quad 0
.L__real_1_by_6:                .quad 0x3fc5555555555555    # 1/6
                                .quad 0
.L__real_1_by_2:                .quad 0x3fe0000000000000    # 1/2
                                .quad 0
.align 16
.L__two_to_jby64_head_table:
    .quad 0x3ff0000000000000
    .quad 0x3ff02c9a30000000
    .quad 0x3ff059b0d0000000
    .quad 0x3ff0874510000000
    .quad 0x3ff0b55860000000
    .quad 0x3ff0e3ec30000000
    .quad 0x3ff11301d0000000
    .quad 0x3ff1429aa0000000
    .quad 0x3ff172b830000000
    .quad 0x3ff1a35be0000000
    .quad 0x3ff1d48730000000
    .quad 0x3ff2063b80000000
    .quad 0x3ff2387a60000000
    .quad 0x3ff26b4560000000
    .quad 0x3ff29e9df0000000
    .quad 0x3ff2d285a0000000
    .quad 0x3ff306fe00000000
    .quad 0x3ff33c08b0000000
    .quad 0x3ff371a730000000
    .quad 0x3ff3a7db30000000
    .quad 0x3ff3dea640000000
    .quad 0x3ff4160a20000000
    .quad 0x3ff44e0860000000
    .quad 0x3ff486a2b0000000
    .quad 0x3ff4bfdad0000000
    .quad 0x3ff4f9b270000000
    .quad 0x3ff5342b50000000
    .quad 0x3ff56f4730000000
    .quad 0x3ff5ab07d0000000
    .quad 0x3ff5e76f10000000
    .quad 0x3ff6247eb0000000
    .quad 0x3ff6623880000000
    .quad 0x3ff6a09e60000000
    .quad 0x3ff6dfb230000000
    .quad 0x3ff71f75e0000000
    .quad 0x3ff75feb50000000
    .quad 0x3ff7a11470000000
    .quad 0x3ff7e2f330000000
    .quad 0x3ff8258990000000
    .quad 0x3ff868d990000000
    .quad 0x3ff8ace540000000
    .quad 0x3ff8f1ae90000000
    .quad 0x3ff93737b0000000
    .quad 0x3ff97d8290000000
    .quad 0x3ff9c49180000000
    .quad 0x3ffa0c6670000000
    .quad 0x3ffa5503b0000000
    .quad 0x3ffa9e6b50000000
    .quad 0x3ffae89f90000000
    .quad 0x3ffb33a2b0000000
    .quad 0x3ffb7f76f0000000
    .quad 0x3ffbcc1e90000000
    .quad 0x3ffc199bd0000000
    .quad 0x3ffc67f120000000
    .quad 0x3ffcb720d0000000
    .quad 0x3ffd072d40000000
    .quad 0x3ffd5818d0000000
    .quad 0x3ffda9e600000000
    .quad 0x3ffdfc9730000000
    .quad 0x3ffe502ee0000000
    .quad 0x3ffea4afa0000000
    .quad 0x3ffefa1be0000000
    .quad 0x3fff507650000000
    .quad 0x3fffa7c180000000

.align 16
.L__two_to_jby64_tail_table:
    .quad 0x0000000000000000
    .quad 0x3e6cef00c1dcdef9
    .quad 0x3e48ac2ba1d73e2a
    .quad 0x3e60eb37901186be
    .quad 0x3e69f3121ec53172
    .quad 0x3e469e8d10103a17
    .quad 0x3df25b50a4ebbf1a
    .quad 0x3e6d525bbf668203
    .quad 0x3e68faa2f5b9bef9
    .quad 0x3e66df96ea796d31
    .quad 0x3e368b9aa7805b80
    .quad 0x3e60c519ac771dd6
    .quad 0x3e6ceac470cd83f5
    .quad 0x3e5789f37495e99c
    .quad 0x3e547f7b84b09745
    .quad 0x3e5b900c2d002475
    .quad 0x3e64636e2a5bd1ab
    .quad 0x3e4320b7fa64e430
    .quad 0x3e5ceaa72a9c5154
    .quad 0x3e53967fdba86f24
    .quad 0x3e682468446b6824
    .quad 0x3e3f72e29f84325b
    .quad 0x3e18624b40c4dbd0
    .quad 0x3e5704f3404f068e
    .quad 0x3e54d8a89c750e5e
    .quad 0x3e5a74b29ab4cf62
    .quad 0x3e5a753e077c2a0f
    .quad 0x3e5ad49f699bb2c0
    .quad 0x3e6a90a852b19260
    .quad 0x3e56b48521ba6f93
    .quad 0x3e0d2ac258f87d03
    .quad 0x3e42a91124893ecf
    .quad 0x3e59fcef32422cbe
    .quad 0x3e68ca345de441c5
    .quad 0x3e61d8bee7ba46e1
    .quad 0x3e59099f22fdba6a
    .quad 0x3e4f580c36bea881
    .quad 0x3e5b3d398841740a
    .quad 0x3e62999c25159f11
    .quad 0x3e668925d901c83b
    .quad 0x3e415506dadd3e2a
    .quad 0x3e622aee6c57304e
    .quad 0x3e29b8bc9e8a0387
    .quad 0x3e6fbc9c9f173d24
    .quad 0x3e451f8480e3e235
    .quad 0x3e66bbcac96535b5
    .quad 0x3e41f12ae45a1224
    .quad 0x3e55e7f6fd0fac90
    .quad 0x3e62b5a75abd0e69
    .quad 0x3e609e2bf5ed7fa1
    .quad 0x3e47daf237553d84
    .quad 0x3e12f074891ee83d
    .quad 0x3e6b0aa538444196
    .quad 0x3e6cafa29694426f
    .quad 0x3e69df20d22a0797
    .quad 0x3e640f12f71a1e45
    .quad 0x3e69f7490e4bb40b
    .quad 0x3e4ed9942b84600d
    .quad 0x3e4bdcdaf5cb4656
    .quad 0x3e5e2cffd89cf44c
    .quad 0x3e452486cc2c7b9d
    .quad 0x3e6cc2b44eee3fa4
    .quad 0x3e66dc8a80ce9f09
    .quad 0x3e39e90d82e90a7e

