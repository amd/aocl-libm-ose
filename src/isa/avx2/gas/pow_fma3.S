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

    cmp         .L__real_1_by_2(%rip), %r8  #y =0.5
    je          .L__y_is_half # computing sqrt(x)
   
    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__ay_max_bound(%rip), %r10
    jg          .L__ay_is_very_large

    mov         .L__exp_mask(%rip), %r10
    and         %r8, %r10
    cmp         .L__ay_min_bound(%rip), %r10
    jl          .L__ay_is_very_small

    and         .L__real_mant(%rip), %rdx
    cmp         .L__pos_zero(%rip), %rdx
    je          .L__x_is_power_of_2
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
    and         .L__mask_mant_all8(%rip), %r8
    and         .L__mask_mant9(%rip), %r9
    vsubsd       .L__real_one(%rip), %xmm0,%xmm7
    shl         %r9
    add         %r9, %r8
    mov         %r8, p_temp_log(%rsp)
    vandpd       .L__real_notsign(%rip), %xmm7, %xmm7

    # F, Y, switch to near-one codepath
    vmovsd       p_temp_log(%rsp), %xmm1
    shr         $44, %r8
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
    
    lea         .L__log_256_lead(%rip), %r9
    
    # poly
    vmovsd       .L__real_1_over_6(%rip), %xmm3
    vmovsd       .L__real_1_over_3(%rip), %xmm1
                      

    vfmadd213sd     .L__real_1_over_5(%rip), %xmm2, %xmm3
    vfmadd213sd    .L__real_1_over_2(%rip), %xmm2, %xmm1
    
    vmulsd       %xmm0, %xmm0, %xmm4     
    vmulsd       %xmm0, %xmm1, %xmm1                        

    vfmadd213sd  .L__real_1_over_4(%rip), %xmm2, %xmm3
    vfmadd231sd     %xmm4, %xmm3 ,%xmm1
    
    vaddsd       %xmm5, %xmm7, %xmm7
    vaddsd       %xmm7, %xmm1, %xmm1

    
    lea         .L__log_256_tail(%rip), %rdx

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
    # xmm4 - (resT_t)
    # xmm2 - (-resT_h)
.L__log_x_continue:  
    
    vmovsd       %xmm0, %xmm7, %xmm7
    vaddsd       %xmm3, %xmm0, %xmm0
    vmovsd       %xmm0, %xmm5, %xmm5
    vandpd       .L__real_fffffffff8000000(%rip), %xmm0, %xmm0
   
    # xmm0 - H
    # xmm7 - resH
    # xmm5 - res

    mov         save_y(%rsp), %rax
    and         .L__real_fffffffff8000000(%rip), %rax

    vaddsd       %xmm3, %xmm2, %xmm2
    vsubsd       %xmm5, %xmm7, %xmm7
    vsubsd       %xmm2, %xmm4, %xmm1
    vaddsd       %xmm3, %xmm7, %xmm7
    vsubsd       %xmm0, %xmm5, %xmm5

    mov         %rax, y_head(%rsp)
    vmovsd       save_y(%rsp), %xmm4
   
    vaddsd       %xmm1, %xmm7, %xmm7
    vaddsd       %xmm5, %xmm7, %xmm7

    # res = H + T
    # H has leading 26 bits of precision
    # T has full precision

    # xmm0 - H
    # xmm7 - T

    
    vsubsd       y_head(%rsp), %xmm4, %xmm4

    # y is split into head and tail
    # for y * ln(x) computation

    # xmm4 - Yt
    # xmm2 - Yh
    # xmm0 - H
    # xmm7 - T

    vmovsd   %xmm4, %xmm3, %xmm3
   
    
   
    vmulsd   %xmm0, %xmm4, %xmm4 # YtRh
 
    vmulsd   y_head(%rsp), %xmm0, %xmm6 # YhRh 

   
   
    vfmadd213sd %xmm4, %xmm7, %xmm3
    
    vfmadd231sd  y_head(%rsp), %xmm7, %xmm3

    vaddsd   %xmm3, %xmm6, %xmm1
    vmovsd   %xmm1, %xmm0, %xmm0
  

    vsubsd   %xmm1, %xmm6, %xmm6
    vaddsd   %xmm3, %xmm6, %xmm6


    # y * ln(x) = v + vt
    # v and vt are in full precision 
 
    # xmm0 - v
    # xmm6 - vt

    # -----------------------------
    # compute exp( y * ln(x) ) here
    # -----------------------------

    # v * (64/ln(2))
.L__compute_exp:   
     
     vmovsd    %xmm0, %xmm10, %xmm10
     mov       .L__sign_mask(%rip), %r9
     vmovq     %xmm0, %r10    
     and       %r9, %r10
     cmp       .L__sign_mask(%rip), %r10  #check if v is negative
     je        .L__exp__continue #optimized code paths to work only for positive values of y*ln(x)
    
     mov       negate_result(%rsp), %r9
     cmp      .L__sign_mask(%rip), %r9 # if result of pow(x,y) is going to be negative, skip optimized exp code paths
     je       .L__exp__continue
     
     vmovq       %xmm0, %r9  #move v to r9
     cmp        .L__min_exp(%rip), %r9  #if x < 10^-6
     jl         .L__x_very_small # e^x = 1 + x + 0.5*x^2 + (1/6)*x^3

     vmovsd     .L__real_one(%rip), %xmm15

     cmp        .L__real_one(%rip), %r9  #if (0 < x < 1) 
     jl         .L__x_between_0_1

     cmp       .L__max_value(%rip), %r9 # if (1 < x < 100)
     jl        .L__x_less_than_100 

.L__exp__continue:
    vmovsd       %xmm0, p_temp_exp(%rsp)
    vmulsd       .L__real_64_by_log2(%rip), %xmm0, %xmm7
    mov         p_temp_exp(%rsp), %rdx

    # v < 1024*ln(2), ( v * (64/ln(2)) ) < 64*1024
    # v >= -1075*ln(2), ( v * (64/ln(2)) ) >= 64*(-1075)
    vcomisd      .L__real_p65536(%rip), %xmm7
    ja          .L__process_result_inf

    vcomisd      .L__real_m68800(%rip), %xmm7
    jb          .L__process_result_zero

    # n = int( v * (64/ln(2)) )
    vcvtpd2dq    %xmm7, %xmm4
    lea         .L__two_to_jby64_head_table(%rip), %r10
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    vcvtdq2pd    %xmm4, %xmm1

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
    vmovsd       .L__real_1_by_2(%rip), %xmm0
    vmovsd       .L__real_1_by_24(%rip), %xmm3
    vmovsd       .L__real_1_by_720(%rip), %xmm4
    vmulsd       %xmm2, %xmm2, %xmm1
    

    vmovsd       %xmm1, %xmm5, %xmm5
    vmulsd       %xmm2, %xmm1, %xmm1
   
    vfmadd213sd    .L__real_one(%rip), %xmm2, %xmm0
   
    vfmadd213sd  .L__real_1_by_6(%rip), %xmm2, %xmm3
    vmulsd       %xmm1, %xmm5, %xmm5
   
    vfmadd213sd     .L__real_1_by_120(%rip), %xmm2, %xmm4
   
 
    vmulsd       %xmm5, %xmm4, %xmm4

    # deal with denormal results
    xor         %r9d, %r9d
    cmp         .L__denormal_threshold(%rip), %ecx

   
    vfmadd213sd   %xmm4, %xmm1, %xmm3
    
    vfmadd213sd    %xmm3, %xmm2, %xmm0

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

.L__x_less_than_100:
#    Split v into integer part 'vint' and and fractional part 'vfrac'.
#    Calculate exp(v) as exp(vint) * exp(vfrac)
    xor          %r8, %r8           #r8 = 0
    vsubsd       .L__real_half(%rip), %xmm10, %xmm11 #subtracting 0.5 from v in order to be rounded to the same value as floor(v).
#   int(v) = floor(v)
    vcvtsd2si    %xmm11, %r8        #r8 = vint is stored in r8 which is the index of look up table
    vcvtsi2sd    %r8,   %xmm2,  %xmm2   #xmm2 = double(%r8)
  
    vsubsd       %xmm2, %xmm10, %xmm10 #vfrac is stored in xmm10
    lea         .L__e_pow_1_to_100_table(%rip), %rdx  # load address of table
    vmovsd       (%rdx,%r8,8), %xmm15  #e^(vint) # xmm15 = exp(vint) = L__e_pow_1_to_100_table[r8]  
    vmovq        %xmm10, %r9  
 
.L__x_between_0_1:
    vfmadd213sd    %xmm15, %xmm6, %xmm15   #xmm15 = exp(vint)*vt + exp(vint)
#    vaddsd         .L__real_one(%rip), %xmm6, %xmm0 # xmm0 = exp(vt) = vt + 1
    vmovsd         %xmm15, %xmm0, %xmm0   #xmm15 = exp(vint)*vt + exp(vint)
#    vmovsd         %xmm15, %xmm0, %xmm0 #xmm0 = exp(vint+vt)     
    vmovsd         .L__real_one(%rip), %xmm14 #intiitlaise xmm14 to one.      
    mov            .L__lower_exponent(%rip), %r8 #r8 = 0.00390625
    cmp            %r8, %r9  
    jl             .L__compute_polynomial #if v < 0.00390625 then exp(vfrac) = Taylor series upto the 7th term

#Extract the exponent and subtract 1022
#v' = 1022 - (exponent of vfrac)
# This is done as exponent will be less than equal to 1022 for values between 0 and 1.
#index = Extract the first 6 bits of mantissa
# The strucutre of look up table is as follows
# For each value of exponent=1022,1021,1020,1019, we have 2^6 values corresponding to the 6 bits of mantissa.
# |exponent  | range of vfrac  |
#   1022        1.0 to 0.5
#   1021        0.5 to 0.25
#   1020        0.25 to 0.125
#   1019        0.125 to 0.0625
#   1018        0.0625 to 0.03125
#   1017        0.03125 to 0.015625
#   1016        0.015625 to 0.0078125
#   1015        0.0078125 to 0.00390625
#Hence looktable is of size 4*2^6
#vfrac_head = exponent of v_frac + first 6 mantissa bits of mantsissa. The remaining 46 bits of the mantissa are set to zero.
#vfrac_tail = vfrac - vfrac_head
#exp(vfrac_head) = .L__exp_0_1[v' * 64 + index}
#exp(vfrac_tail) = Taylor series upto the 7th degree computed using Estrin's method. 
    mov            %r9, %r8
    shr            $52, %r8  #extract exponent
    mov            $1022, %rax
    sub            %r8, %rax   #v' = 1022 - exponent of vfrac 
    mov            $64, %r8
    mul            %r8    #rax = 64*(v')
    
    vandpd         .L__exp_mantissa6bits(%rip), %xmm10, %xmm3 #xmm3 = vfrac_head
    vsubsd         %xmm3, %xmm10, %xmm10     #vfrac_tail = vfrac - vfrac_head
    and            .L__mantissa6bits(%rip), %r9  #take mantissa 6 bits
    shr            $46, %r9                      #shift by (52 - 6) bits in r9 get the actual value of the first 6 bits of mantissa
    add            %rax, %r9                     #index = v'*64 + index
    lea            .L__exp_0_1(%rip), %rdx  #load address of table
    vmovsd         (%rdx,%r9,8), %xmm14  #xmm14 = exp(vfrac_head)

#xmm0 = exp(vint+vt)
#xmm10 = vfrac_tail
#xmm14 = exp(vfrac_head)    
.L__compute_polynomial:
#    vaddsd         .L__real_one(%rip), %xmm10, %xmm2  #A0 = a0 + x = 1 + x
#    vmovsd         .L__real_1_by_7factorial(%rip), %xmm3
    vmovsd         .L__real_1_by_720(%rip), %xmm3 
    vfmadd213sd    .L__real_1_by_120(%rip), %xmm10, %xmm3 #xmm3 = (1/120 + 1/720*x) 
    vfmadd213sd    .L__real_1_by_24(%rip), %xmm10, %xmm3  #xmm3 = (1/24 + x*xmm3) 
    vfmadd213sd    .L__real_1_by_6(%rip), %xmm10, %xmm3 #xmm3 = (1/6 + x*xmm3)
    vfmadd213sd    .L__real_half(%rip), %xmm10, %xmm3 #xmm3 = 1/2 + x*xmm3
    vfmadd213sd    .L__real_one(%rip), %xmm10, %xmm3 #xmm3 = 1 + x*(xmm3)
    vfmadd213sd    .L__real_one(%rip), %xmm10, %xmm3  #xmm3 = 1 + x*xmm3
 #   vmulsd         %xmm10, %xmm10, %xmm12    # xsquare
 #   vmulsd         %xmm12, %xmm12, %xmm13     #xfour
 
#    vfmadd213sd    .L__real_half(%rip), %xmm10, %xmm3 #A1 = a2 + x*a3 = 0.5 + (1/6)*x
#    vfmadd213sd    .L__real_1_by_24(%rip), %xmm10, %xmm4 #A2 = a4 + x*a5 = (1/24) + (1/120)*x
#    vfmadd213sd    .L__real_1_by_720(%rip), %xmm10, %xmm5 #A3 = a6 + x*a7 = (1/720) + (1/7!)*x
    
#    vfmadd213sd   %xmm2, %xmm12, %xmm3   #B0 = A0 + xsquare*A1
#    vfmadd213sd   %xmm4, %xmm12, %xmm5   #B1 = A2 + xsquare*A3   
    
#    vmovsd         %xmm3, %xmm5, %xmm5   #C0 = B0 + xfour*B1
    
    vmulsd       %xmm3, %xmm0, %xmm11 #xmm11 = exp(vint+vt)*exp(vfrac_tail)
    vmulsd       %xmm14, %xmm11, %xmm0 # xmm0 = exp(vint+vt+vfrac_tail)*exp(vfrac_head)   
    vorpd        negate_result(%rsp), %xmm0, %xmm0 #setting the sign bit
    jmp         .L__final_check


.L__x_very_small:
    vaddsd       %xmm6, %xmm0, %xmm10      #add v+vt
    vaddsd       .L__real_one(%rip), %xmm10, %xmm1 #A0 = 1 + x
    vmovsd       .L__real_1_by_6(%rip), %xmm0
    vfmadd213sd  .L__real_half(%rip), %xmm10, %xmm0 #A1 = a2 + x*a3 = 0.5 + (1/6)*x
    vmulsd       %xmm10, %xmm10, %xmm12 
    vfmadd213sd  %xmm1, %xmm12, %xmm0
    vorpd        negate_result(%rsp), %xmm0, %xmm0 #setting the sign bit
    jmp          .L__final_check


.p2align 4,,15
.L__y_is_half:
    vsqrtsd   %xmm0, %xmm0, %xmm0 #x^0.5 = sqrt(x)
    add         $stack_size, %rsp
    ret

.L__x_is_power_of_2:
    movq    %xmm0, %r9 #copy x to r9
    and      .L__exp_mask(%rip), %r9
    shr     $52, %r9
    sub      $1023, %r9 #r9 = exponent of x
    movq    %xmm1, %r10 #copy y to r10
    and      .L__exp_mask(%rip), %r10
    shr      $52, %r10
    sub      $1023, %r10  #r10 = exponent of x
    add      $1, %r10
    cmp      $1, %r10
    jl       .L__log_x   # y is not a integer
    cmp      $53, %r10
    jg       .L__compute_powers_of_2

.L__compute_powers_of_2:
    mov      $1, %rdx #rdx = mask
    mov      $53, %rcx 
    sub      %r10, %rcx
    shl      %cl, %rdx 
    sub      $1, %rdx #(mask << (53 - yexp)) - 1
    movq     %xmm1, %r11
    and      %r11, %rdx
    cmp      .L__pos_zero(%rip), %rdx
    jne       .L__log_x #if ((y & mask) != 0) then y is not an integer
    vcvtsd2si %xmm1, %r8 #int y = (int)y 
    mov       %r9, %rax  #rax = exponent of x
    mul       %r8   # r8 = y*exponent of x
    cmp       $1023, %rax #if y*exponent of x >1023 then return inifinty
    jg        .L__log_x
    add       $1023, %rax
    shl       $52, %rax
    movq      %rax, %xmm0
    jmp       .L__final_check         


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
.L__lower_exponent:         .quad 0x3F70000000000000 #0.00390625
.L__exp_mantissa6bits:      .quad 0x7FFFC00000000000
.L__mantissa6bits:          .quad 0x000FC00000000000 
.L__min_exp:                .quad 0x3EB0C6F7A0B5ED8D #10^-6
.L__max_value:              .quad 0x4059000000000000  #100
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

.L__mask_mant_all8:         .quad 0x000ff00000000000
.L__mask_mant9:             .quad 0x0000080000000000
.L__pos_2:                  .quad 0x4000000000000000



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
                    .quad 0x0000000000000000


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
.L__log_F_inv_head:
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
.L__log_F_inv_tail:
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
.L__real_1_by_18factorial:            .quad 0x3ca6827863b97d97
                                .quad 0
.L__real_1_by_17factorial:            .quad 0x3ce952c77030ad4a
                                .quad 0
.L__real_1_by_16factorial:            .quad 0x3d2ae7f3e733b81f
                                .quad 0
.L__real_1_by_15factorial:            .quad 0x3d6ae7f3e733b81f
                                .quad 0
.L__real_1_by_14factorial:            .quad 0x3da93974a8c07c9d
                                .quad 0
.L__real_1_by_13factorial:            .quad 0x3de6124613a86d09
                                .quad 0
.L__real_1_by_12factorial:            .quad 0x3e21eed8eff8d898
                                .quad 0
.L__real_1_by_11factorial:            .quad 0x3e5ae64567f544e4
                                .quad 0
.L__real_1_by_10factorial:            .quad 0x3e927e4fb7789f5c
                                .quad 0
.L__real_1_by_9factorial:             .quad 0x3ec71de3a556c734
                                .quad 0
.L__real_1_by_8factorial:             .quad 0x3efa01a01a01a01a
                                .quad 0
.L__real_1_by_7factorial:            .quad 0x3f2a01a01a01a01a
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

.align 16
.L__e_pow_1_to_100_table:
    .quad 0x3ff0000000000000
    .quad 0x4005bf0a8b145769
    .quad 0x401d8e64b8d4ddae
    .quad 0x403415e5bf6fb106
    .quad 0x404b4c902e273a58
    .quad 0x40628d389970338f
    .quad 0x407936dc5690c08f
    .quad 0x409122885aaeddaa
    .quad 0x40a749ea7d470c6e
    .quad 0x40bfa7157c470f82
    .quad 0x40d5829dcf950560
    .quad 0x40ed3c4488ee4f7f
    .quad 0x4103de1654d37c9a
    .quad 0x411b00b5916ac955
    .quad 0x413259ac48bf05d7
    .quad 0x4148f0ccafad2a87
    .quad 0x4160f2ebd0a80020
    .quad 0x417709348c0ea4f9
    .quad 0x418f4f22091940bd
    .quad 0x41a546d8f9ed26e1
    .quad 0x41bceb088b68e804
    .quad 0x41d3a6e1fd9eecfd
    .quad 0x41eab5adb9c43600
    .quad 0x420226af33b1fdc1
    .quad 0x4218ab7fb5475fb7
    .quad 0x4230c3d3920962c9
    .quad 0x4246c932696a6b5d
    .quad 0x425ef822f7f6731d
    .quad 0x42750bba3796379a
    .quad 0x428c9aae4631c056
    .quad 0x42a370470aec28ed
    .quad 0x42ba6b765d8cdf6d
    .quad 0x42d1f43fcc4b662c
    .quad 0x42e866f34a725782
    .quad 0x4300953e2f3a1ef7
    .quad 0x431689e221bc8d5b
    .quad 0x432ea215a1d20d76
    .quad 0x4344d13fbb1a001a
    .quad 0x435c4b334617cc67
    .quad 0x43733a43d282a519
    .quad 0x438a220d397972eb
    .quad 0x43a1c25c88df6862
    .quad 0x43b8232558201159
    .quad 0x43d0672a3c9eb871
    .quad 0x43e64b41c6d37832
    .quad 0x43fe4cf766fe49be
    .quad 0x44149767bc0483e3
    .quad 0x442bfc951eb8bb76
    .quad 0x444304d6aeca254b
    .quad 0x4459d97010884251
    .quad 0x44719103e4080b45
    .quad 0x4487e013cd114461
    .quad 0x44a03996528e074c
    .quad 0x44b60d4f6fdac731
    .quad 0x44cdf8c5af17ba3b
    .quad 0x44e45e3076d61699
    .quad 0x44fbaed16a6e0da7
    .quad 0x4512cffdfebde1a1
    .quad 0x4529919cabefcb69
    .quad 0x454160345c9953e3
    .quad 0x45579dbc9dc53c66
    .quad 0x45700c810d464097
    .quad 0x4585d009394c5c27
    .quad 0x459da57de8f107a8
    .quad 0x45b425982cf597cd
    .quad 0x45cb61e5ca3a5e31
    .quad 0x45e29bb825dfcf87
    .quad 0x45f94a90db0d6fe2
    .quad 0x46112fec759586fd
    .quad 0x46275c1dc469e3af
    .quad 0x463fbfd219c43b04
    .quad 0x4655936d44e1a146
    .quad 0x466d531d8a7ee79c
    .quad 0x4683ed9d24a2d51b
    .quad 0x469b15cfe5b6e17b
    .quad 0x46b268038c2c0e00
    .quad 0x46c9044a73545d48
    .quad 0x46e1002ab6218b38
    .quad 0x46f71b3540cbf921
    .quad 0x470f6799ea9c414a
    .quad 0x47255779b984f3eb
    .quad 0x473d01a210c44aa4
    .quad 0x4753b63da8e91210
    .quad 0x476aca8d6b0116b8
    .quad 0x478234de9e0c74e9
    .quad 0x4798bec7503ca477
    .quad 0x47b0d0eda9796b90
    .quad 0x47c6db0118477245
    .quad 0x47df1056dc7bf22d
    .quad 0x47f51c2cc3433801
    .quad 0x480cb108ffbec164
    .quad 0x48237f780991b584
    .quad 0x483a801c0ea8ac4d
    .quad 0x48520247cc4c46c1
    .quad 0x48687a0553328015
    .quad 0x4880a233dee4f9bb
    .quad 0x48969b7f55b808ba
    .quad 0x48aeba064644060a
    .quad 0x48c4e184933d9364
    .quad 0x48dc614fe2531841
    .quad 0x48f3494a9b171bf5

.align 16
.L__exp_0_1:
  .quad 0x3ffa61298e1e069c
  .quad 0x3ffa9620c6cb3374
  .quad 0x3ffacb82581eee54
  .quad 0x3ffb014f179fc3b8
  .quad 0x3ffb3787dc80f95f
  .quad 0x3ffb6e2d7fa5eb18
  .quad 0x3ffba540dba56e56
  .quad 0x3ffbdcc2cccd3c85
  .quad 0x3ffc14b431256446
  .quad 0x3ffc4d15e873c193
  .quad 0x3ffc85e8d43f7cd0
  .quad 0x3ffcbf2dd7d490f2
  .quad 0x3ffcf8e5d84758a9
  .quad 0x3ffd3311bc7822b4
  .quad 0x3ffd6db26d16cd67
  .quad 0x3ffda8c8d4a66969
  .quad 0x3ffde455df80e3c0
  .quad 0x3ffe205a7bdab73e
  .quad 0x3ffe5cd799c6a54e
  .quad 0x3ffe99ce2b397649
  .quad 0x3ffed73f240dc142
  .quad 0x3fff152b7a07bb76
  .quad 0x3fff539424d90f5e
  .quad 0x3fff927a1e24bb76
  .quad 0x3fffd1de6182f8c9
  .quad 0x400008e0f64294ab
  .quad 0x40002912df5ce72a
  .quad 0x400049856cd84339
  .quad 0x40006a39207f0a09
  .quad 0x40008b2e7d2035cf
  .quad 0x4000ac6606916501
  .quad 0x4000cde041b0e9ae
  .quad 0x4000ef9db467dcf8
  .quad 0x4001119ee5ac36b6
  .quad 0x400133e45d82e952
  .quad 0x4001566ea50201d7
  .quad 0x4001793e4652cc50
  .quad 0x40019c53ccb3fc6b
  .quad 0x4001bfafc47bda73
  .quad 0x4001e352bb1a74ad
  .quad 0x4002073d3f1bd518
  .quad 0x40022b6fe02a3b9c
  .quad 0x40024feb2f105cb8
  .quad 0x400274afbdbba4a6
  .quad 0x400299be1f3e7f1c
  .quad 0x4002bf16e7d2a38c
  .quad 0x4002e4baacdb6614
  .quad 0x40030aaa04e80d05
  .quad 0x400330e587b62b28
  .quad 0x4003576dce33fead
  .quad 0x40037e437282d4ee
  .quad 0x4003a5670ff972ed
  .quad 0x4003ccd9432682b4
  .quad 0x4003f49aa9d30590
  .quad 0x40041cabe304cb34
  .quad 0x4004450d8f00edd4
  .quad 0x40046dc04f4e5338
  .quad 0x400496c4c6b832da
  .quad 0x4004c01b9950a111
  .quad 0x4004e9c56c731f5d
  .quad 0x400513c2e6c731d7
  .quad 0x40053e14b042f9ca
  .quad 0x400568bb722dd593
  .quad 0x400593b7d72305bb
  .quad 0x3ff48b5e3c3e8186
  .quad 0x3ff49ff3e397492c
  .quad 0x3ff4b49e2ae5ac67
  .quad 0x3ff4c95d26d3f440
  .quad 0x3ff4de30ec211e60
  .quad 0x3ff4f3198fa0f1d0
  .quad 0x3ff50817263c13cd
  .quad 0x3ff51d29c4f01cb3
  .quad 0x3ff5325180cfacf7
  .quad 0x3ff5478e6f02823d
  .quad 0x3ff55ce0a4c58c7c
  .quad 0x3ff57248376b033d
  .quad 0x3ff587c53c5a7af0
  .quad 0x3ff59d57c910fa4e
  .quad 0x3ff5b2fff3210fd9
  .quad 0x3ff5c8bdd032e771
  .quad 0x3ff5de9176045ff5
  .quad 0x3ff5f47afa69210a
  .quad 0x3ff60a7a734ab0e8
  .quad 0x3ff6208ff6a88a46
  .quad 0x3ff636bb9a983258
  .quad 0x3ff64cfd75454ee8
  .quad 0x3ff663559cf1bc7c
  .quad 0x3ff679c427f5a49f
  .quad 0x3ff690492cbf9433
  .quad 0x3ff6a6e4c1d491e1
  .quad 0x3ff6bd96fdd034a2
  .quad 0x3ff6d45ff764ba56
  .quad 0x3ff6eb3fc55b1e76
  .quad 0x3ff702367e9330e1
  .quad 0x3ff719443a03acb9
  .quad 0x3ff730690eba4f5b
  .quad 0x3ff747a513dbef6a
  .quad 0x3ff75ef860a493f9
  .quad 0x3ff776630c678bc1
  .quad 0x3ff78de52e8f8479
  .quad 0x3ff7a57ede9ea23e
  .quad 0x3ff7bd30342e9717
  .quad 0x3ff7d4f946f0ba8d
  .quad 0x3ff7ecda2eae215f
  .quad 0x3ff804d30347b546
  .quad 0x3ff81ce3dcb64cdc
  .quad 0x3ff8350cd30ac390
  .quad 0x3ff84d4dfe6e11bb
  .quad 0x3ff865a7772164c5
  .quad 0x3ff87e19557e376a
  .quad 0x3ff896a3b1f66a0e
  .quad 0x3ff8af46a5145b37
  .quad 0x3ff8c802477b0010
  .quad 0x3ff8e0d6b1e5fd0e
  .quad 0x3ff8f9c3fd29beaf
  .quad 0x3ff912ca4233924b
  .quad 0x3ff92be99a09bf00
  .quad 0x3ff945221dcb9ebd
  .quad 0x3ff95e73e6b1b75e
  .quad 0x3ff977df0e0dd3e5
  .quad 0x3ff99163ad4b1dcc
  .quad 0x3ff9ab01ddee3672
  .quad 0x3ff9c4b9b995509b
  .quad 0x3ff9de8b59f84a14
  .quad 0x3ff9f876d8e8c566
  .quad 0x3ffa127c505243ab
  .quad 0x3ffa2c9bda3a3e78
  .quad 0x3ffa46d590c041e1
  .quad 0x3ff2216045b6f5cd
  .quad 0x3ff22a733a669818
  .quad 0x3ff2338ab9b32135
  .quad 0x3ff23ca6c5e27103
  .quad 0x3ff245c7613b8a9b
  .quad 0x3ff24eec8e0694df
  .quad 0x3ff258164e8cdb0e
  .quad 0x3ff26144a518cd55
  .quad 0x3ff26a7793f60164
  .quad 0x3ff273af1d7132ff
  .quad 0x3ff27ceb43d84490
  .quad 0x3ff2862c097a3fbe
  .quad 0x3ff28f7170a755fd
  .quad 0x3ff298bb7bb0e126
  .quad 0x3ff2a20a2ce96406
  .quad 0x3ff2ab5d86a48af9
  .quad 0x3ff2b4b58b372c79
  .quad 0x3ff2be123cf749b8
  .quad 0x3ff2c7739e3c0f33
  .quad 0x3ff2d0d9b15dd546
  .quad 0x3ff2da4478b620c7
  .quad 0x3ff2e3b3f69fa399
  .quad 0x3ff2ed282d763d42
  .quad 0x3ff2f6a11f96fb85
  .quad 0x3ff3001ecf601af7
  .quad 0x3ff309a13f310796
  .quad 0x3ff31328716a5d64
  .quad 0x3ff31cb4686de8fb
  .quad 0x3ff32645269ea829
  .quad 0x3ff32fdaae60ca87
  .quad 0x3ff339750219b213
  .quad 0x3ff34314242ff3c7
  .quad 0x3ff34cb8170b5835
  .quad 0x3ff35660dd14dc22
  .quad 0x3ff3600e78b6b11d
  .quad 0x3ff369c0ec5c3e1b
  .quad 0x3ff373783a722012
  .quad 0x3ff37d3465662a95
  .quad 0x3ff386f56fa7686e
  .quad 0x3ff390bb5ba61c3a
  .quad 0x3ff39a862bd3c106
  .quad 0x3ff3a455e2a30aeb
  .quad 0x3ff3ae2a8287e7a8
  .quad 0x3ff3b8040df77f45
  .quad 0x3ff3c1e2876834aa
  .quad 0x3ff3cbc5f151a642
  .quad 0x3ff3d5ae4e2cae93
  .quad 0x3ff3df9ba07364e1
  .quad 0x3ff3e98deaa11dcc
  .quad 0x3ff3f3852f326beb
  .quad 0x3ff3fd8170a52072
  .quad 0x3ff40782b1784bc8
  .quad 0x3ff41188f42c3e32
  .quad 0x3ff41b943b428869
  .quad 0x3ff425a4893dfc40
  .quad 0x3ff42fb9e0a2ad43
  .quad 0x3ff439d443f5f159
  .quad 0x3ff443f3b5be6164
  .quad 0x3ff44e183883d9e5
  .quad 0x3ff45841cecf7b99
  .quad 0x3ff462707b2bac21
  .quad 0x3ff46ca4402416a2
  .quad 0x3ff476dd2045ac68
  .quad 0x3ff4811b1e1ea588
  .quad 0x3ff1082b577d34ed
  .quad 0x3ff10c6dea9fca74
  .quad 0x3ff110b18e894010
  .quad 0x3ff114f6437dd001
  .quad 0x3ff1193c09c1c596
  .quad 0x3ff11d82e1997d34
  .quad 0x3ff121cacb496458
  .quad 0x3ff12613c715f99f
  .quad 0x3ff12a5dd543ccc5
  .quad 0x3ff12ea8f6177ead
  .quad 0x3ff132f529d5c166
  .quad 0x3ff1374270c3582a
  .quad 0x3ff13b90cb25176a
  .quad 0x3ff13fe0393fe4cc
  .quad 0x3ff14430bb58b732
  .quad 0x3ff1488251b496be
  .quad 0x3ff14cd4fc989cd6
  .quad 0x3ff15128bc49f429
  .quad 0x3ff1557d910dd8b1
  .quad 0x3ff159d37b2997bd
  .quad 0x3ff15e2a7ae28fed
  .quad 0x3ff16282907e313d
  .quad 0x3ff166dbbc41fd09
  .quad 0x3ff16b35fe73860c
  .quad 0x3ff16f9157587069
  .quad 0x3ff173edc73671b0
  .quad 0x3ff1784b4e5350df
  .quad 0x3ff17ca9ecf4e668
  .quad 0x3ff18109a3611c35
  .quad 0x3ff1856a71ddedae
  .quad 0x3ff189cc58b167ba
  .quad 0x3ff18e2f5821a8c7
  .quad 0x3ff192937074e0cd
  .quad 0x3ff196f8a1f15152
  .quad 0x3ff19b5eecdd4d6c
  .quad 0x3ff19fc6517f39cc
  .quad 0x3ff1a42ed01d8cbc
  .quad 0x3ff1a89868fece27
  .quad 0x3ff1ad031c699799
  .quad 0x3ff1b16eeaa4944c
  .quad 0x3ff1b5dbd3f68122
  .quad 0x3ff1ba49d8a62cb2
  .quad 0x3ff1beb8f8fa7746
  .quad 0x3ff1c329353a52e4
  .quad 0x3ff1c79a8dacc351
  .quad 0x3ff1cc0d0298de14
  .quad 0x3ff1d0809445ca7c
  .quad 0x3ff1d4f542fac1a5
  .quad 0x3ff1d96b0eff0e79
  .quad 0x3ff1dde1f89a0dbb
  .quad 0x3ff1e25a00132e03
  .quad 0x3ff1e6d325b1efca
  .quad 0x3ff1eb4d69bde56a
  .quad 0x3ff1efc8cc7eb324
  .quad 0x3ff1f4454e3c0f24
  .quad 0x3ff1f8c2ef3dc188
  .quad 0x3ff1fd41afcba45e
  .quad 0x3ff201c1902da3b2
  .quad 0x3ff2064290abbd88
  .quad 0x3ff20ac4b18e01e9
  .quad 0x3ff20f47f31c92e4
  .quad 0x3ff213cc559fa493
  .quad 0x3ff21851d95f7d1d
  .quad 0x3ff21cd87ea474be
  .quad 0x3ff08205601127ed
  .quad 0x3ff08415c1c29508
  .quad 0x3ff0862665845940
  .quad 0x3ff088374b5eb725
  .quad 0x3ff08a487359f24c
  .quad 0x3ff08c59dd7e4f58
  .quad 0x3ff08e6b89d413ef
  .quad 0x3ff0907d786386c4
  .quad 0x3ff0928fa934ef91
  .quad 0x3ff094a21c509718
  .quad 0x3ff096b4d1bec727
  .quad 0x3ff098c7c987ca93
  .quad 0x3ff09adb03b3ed3b
  .quad 0x3ff09cee804b7c08
  .quad 0x3ff09f023f56c4ec
  .quad 0x3ff0a11640de16e4
  .quad 0x3ff0a32a84e9c1f6
  .quad 0x3ff0a53f0b821731
  .quad 0x3ff0a753d4af68b0
  .quad 0x3ff0a968e07a0999
  .quad 0x3ff0ab7e2eea4e1a
  .quad 0x3ff0ad93c0088b6c
  .quad 0x3ff0afa993dd17d5
  .quad 0x3ff0b1bfaa704aa4
  .quad 0x3ff0b3d603ca7c32
  .quad 0x3ff0b5ec9ff405e7
  .quad 0x3ff0b8037ef54231
  .quad 0x3ff0ba1aa0d68c8d
  .quad 0x3ff0bc3205a04183
  .quad 0x3ff0be49ad5abea6
  .quad 0x3ff0c061980e6294
  .quad 0x3ff0c279c5c38cf9
  .quad 0x3ff0c49236829e8c
  .quad 0x3ff0c6aaea53f90f
  .quad 0x3ff0c8c3e13fff51
  .quad 0x3ff0cadd1b4f152f
  .quad 0x3ff0ccf698899f91
  .quad 0x3ff0cf1058f8046b
  .quad 0x3ff0d12a5ca2aabf
  .quad 0x3ff0d344a391fa9d
  .quad 0x3ff0d55f2dce5d1f
  .quad 0x3ff0d779fb603c6e
  .quad 0x3ff0d9950c5003c2
  .quad 0x3ff0dbb060a61f5d
  .quad 0x3ff0ddcbf86afc92
  .quad 0x3ff0dfe7d3a709bf
  .quad 0x3ff0e203f262b650
  .quad 0x3ff0e42054a672c2
  .quad 0x3ff0e63cfa7ab09d
  .quad 0x3ff0e859e3e7e278
  .quad 0x3ff0ea7710f67bfa
  .quad 0x3ff0ec9481aef1d6
  .quad 0x3ff0eeb23619b9cf
  .quad 0x3ff0f0d02e3f4ab7
  .quad 0x3ff0f2ee6a281c6f
  .quad 0x3ff0f50ce9dca7e6
  .quad 0x3ff0f72bad65671b
  .quad 0x3ff0f94ab4cad51d
  .quad 0x3ff0fb6a00156e08
  .quad 0x3ff0fd898f4daf0a
  .quad 0x3ff0ffa9627c1660
  .quad 0x3ff101c979a92356
  .quad 0x3ff103e9d4dd564a
  .quad 0x3ff1060a742130a8
  .quad 0x3ff04080ab55de39
  .quad 0x3ff04184bb80ff44
  .quad 0x3ff04288dbeda50c
  .quad 0x3ff0438d0c9cd3b1
  .quad 0x3ff044914d8f8f64
  .quad 0x3ff045959ec6dc66
  .quad 0x3ff0469a0043bf08
  .quad 0x3ff0479e72073bac
  .quad 0x3ff048a2f41256c3
  .quad 0x3ff049a7866614cf
  .quad 0x3ff04aac29037a63
  .quad 0x3ff04bb0dbeb8c22
  .quad 0x3ff04cb59f1f4ebe
  .quad 0x3ff04dba729fc6fa
  .quad 0x3ff04ebf566df9ab
  .quad 0x3ff04fc44a8aebb3
  .quad 0x3ff050c94ef7a207
  .quad 0x3ff051ce63b521ab
  .quad 0x3ff052d388c46fb5
  .quad 0x3ff053d8be269148
  .quad 0x3ff054de03dc8b9b
  .quad 0x3ff055e359e763f4
  .quad 0x3ff056e8c0481fa7
  .quad 0x3ff057ee36ffc41c
  .quad 0x3ff058f3be0f56ca
  .quad 0x3ff059f95577dd37
  .quad 0x3ff05afefd3a5cfb
  .quad 0x3ff05c04b557dbbd
  .quad 0x3ff05d0a7dd15f36
  .quad 0x3ff05e1056a7ed2f
  .quad 0x3ff05f163fdc8b7f
  .quad 0x3ff0601c39704011
  .quad 0x3ff06122436410dd
  .quad 0x3ff062285db903ee
  .quad 0x3ff0632e88701f5e
  .quad 0x3ff06434c38a6958
  .quad 0x3ff0653b0f08e817
  .quad 0x3ff066416aeca1e7
  .quad 0x3ff06747d7369d22
  .quad 0x3ff0684e53e7e036
  .quad 0x3ff06954e10171a0
  .quad 0x3ff06a5b7e8457ec
  .quad 0x3ff06b622c7199b8
  .quad 0x3ff06c68eaca3db1
  .quad 0x3ff06d6fb98f4a97
  .quad 0x3ff06e7698c1c737
  .quad 0x3ff06f7d8862ba72
  .quad 0x3ff0708488732b37
  .quad 0x3ff0718b98f42085
  .quad 0x3ff07292b9e6a16e
  .quad 0x3ff07399eb4bb512
  .quad 0x3ff074a12d2462a2
  .quad 0x3ff075a87f71b161
  .quad 0x3ff076afe234a8a1
  .quad 0x3ff077b7556e4fc5
  .quad 0x3ff078bed91fae3f
  .quad 0x3ff079c66d49cb94
  .quad 0x3ff07ace11edaf57
  .quad 0x3ff07bd5c70c612e
  .quad 0x3ff07cdd8ca6e8cd
  .quad 0x3ff07de562be4dfa
  .quad 0x3ff07eed4953988c
  .quad 0x3ff07ff54067d068
  .quad 0x3ff080fd47fbfd85
  .quad 0x3ff0202015600446
  .quad 0x3ff020a11864b8a9
  .quad 0x3ff021221f719552
  .quad 0x3ff021a32a86ba83
  .quad 0x3ff0222439a4487e
  .quad 0x3ff022a54cca5f89
  .quad 0x3ff0232663f91fe6
  .quad 0x3ff023a77f30a9dd
  .quad 0x3ff024289e711db3
  .quad 0x3ff024a9c1ba9bb1
  .quad 0x3ff0252ae90d4420
  .quad 0x3ff025ac14693749
  .quad 0x3ff0262d43ce9577
  .quad 0x3ff026ae773d7ef7
  .quad 0x3ff0272faeb61414
  .quad 0x3ff027b0ea38751d
  .quad 0x3ff0283229c4c260
  .quad 0x3ff028b36d5b1c2e
  .quad 0x3ff02934b4fba2d8
  .quad 0x3ff029b600a676ae
  .quad 0x3ff02a37505bb806
  .quad 0x3ff02ab8a41b8731
  .quad 0x3ff02b39fbe60485
  .quad 0x3ff02bbb57bb5059
  .quad 0x3ff02c3cb79b8b02
  .quad 0x3ff02cbe1b86d4d9
  .quad 0x3ff02d3f837d4e38
  .quad 0x3ff02dc0ef7f1777
  .quad 0x3ff02e425f8c50f2
  .quad 0x3ff02ec3d3a51b06
  .quad 0x3ff02f454bc9960e
  .quad 0x3ff02fc6c7f9e269
  .quad 0x3ff0304848362077
  .quad 0x3ff030c9cc7e7096
  .quad 0x3ff0314b54d2f329
  .quad 0x3ff031cce133c891
  .quad 0x3ff0324e71a11131
  .quad 0x3ff032d0061aed6e
  .quad 0x3ff033519ea17dad
  .quad 0x3ff033d33b34e253
  .quad 0x3ff03454dbd53bc8
  .quad 0x3ff034d68082aa74
  .quad 0x3ff03558293d4ec1
  .quad 0x3ff035d9d6054917
  .quad 0x3ff0365b86dab9e4
  .quad 0x3ff036dd3bbdc192
  .quad 0x3ff0375ef4ae808f
  .quad 0x3ff037e0b1ad1749
  .quad 0x3ff0386272b9a630
  .quad 0x3ff038e437d44db4
  .quad 0x3ff0396600fd2e46
  .quad 0x3ff039e7ce346858
  .quad 0x3ff03a699f7a1c5e
  .quad 0x3ff03aeb74ce6acb
  .quad 0x3ff03b6d4e317416
  .quad 0x3ff03bef2ba358b5
  .quad 0x3ff03c710d24391e
  .quad 0x3ff03cf2f2b435cb
  .quad 0x3ff03d74dc536f35
  .quad 0x3ff03df6ca0205d6
  .quad 0x3ff03e78bbc01a29
  .quad 0x3ff03efab18dccac
  .quad 0x3ff03f7cab6b3ddb
  .quad 0x3ff03ffea9588e35
  .quad 0x3ff0100802ab5577
  .quad 0x3ff01048434be110
  .quad 0x3ff0108884ed712d
  .quad 0x3ff010c8c79009d2
  .quad 0x3ff011090b33af04
  .quad 0x3ff011494fd864c7
  .quad 0x3ff01189957e2f1e
  .quad 0x3ff011c9dc25120f
  .quad 0x3ff0120a23cd119e
  .quad 0x3ff0124a6c7631ce
  .quad 0x3ff0128ab62076a6
  .quad 0x3ff012cb00cbe429
  .quad 0x3ff0130b4c787e5c
  .quad 0x3ff0134b99264944
  .quad 0x3ff0138be6d548e5
  .quad 0x3ff013cc35858145
  .quad 0x3ff0140c8536f668
  .quad 0x3ff0144cd5e9ac54
  .quad 0x3ff0148d279da70d
  .quad 0x3ff014cd7a52ea98
  .quad 0x3ff0150dce097afb
  .quad 0x3ff0154e22c15c3b
  .quad 0x3ff0158e787a925d
  .quad 0x3ff015cecf352167
  .quad 0x3ff0160f26f10d5d
  .quad 0x3ff0164f7fae5a46
  .quad 0x3ff0168fd96d0c27
  .quad 0x3ff016d0342d2706
  .quad 0x3ff017108feeaee8
  .quad 0x3ff01750ecb1a7d3
  .quad 0x3ff017914a7615cc
  .quad 0x3ff017d1a93bfcdb
  .quad 0x3ff0181209036103
  .quad 0x3ff0185269cc464d
  .quad 0x3ff01892cb96b0bd
  .quad 0x3ff018d32e62a45a
  .quad 0x3ff019139230252a
  .quad 0x3ff01953f6ff3732
  .quad 0x3ff019945ccfde7b
  .quad 0x3ff019d4c3a21f09
  .quad 0x3ff01a152b75fce3
  .quad 0x3ff01a55944b7c10
  .quad 0x3ff01a95fe22a097
  .quad 0x3ff01ad668fb6e7d
  .quad 0x3ff01b16d4d5e9ca
  .quad 0x3ff01b5741b21684
  .quad 0x3ff01b97af8ff8b2
  .quad 0x3ff01bd81e6f945b
  .quad 0x3ff01c188e50ed86
  .quad 0x3ff01c58ff34083a
  .quad 0x3ff01c997118e87e
  .quad 0x3ff01cd9e3ff9259
  .quad 0x3ff01d1a57e809d2
  .quad 0x3ff01d5accd252f1
  .quad 0x3ff01d9b42be71bd
  .quad 0x3ff01ddbb9ac6a3d
  .quad 0x3ff01e1c319c4078
  .quad 0x3ff01e5caa8df876
  .quad 0x3ff01e9d24819640
  .quad 0x3ff01edd9f771ddb
  .quad 0x3ff01f1e1b6e9350
  .quad 0x3ff01f5e9867faa8
  .quad 0x3ff01f9f166357e8
  .quad 0x3ff01fdf9560af1a
L__exp_0_1:
