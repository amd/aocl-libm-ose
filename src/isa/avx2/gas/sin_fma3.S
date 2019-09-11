#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(sin)
#define fname_special   _sin_special@PLT
#define fname_special_underflow _sin_special_underflow@PLT

#include "trig_func.h"


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:

    VMOVQ  %xmm0,%r9
    MOV    %r9,%rax
    AND    .L__sign_mask(%rip),%r9            #clear sign
 
sin_early_exit_s_1:

    CMP   .L_mask_3fe(%rip),%r9
    JG    sin_early_exit_s

    CMP  .L_mask_3f2(%rip),%r9
    JGE   compute_sin_pyby_4

    CMP   .L_mask_3e4(%rip),%r9
    JGE   compute_x_xxx_0_1666

##  Additional code to raise underflow exception
   xor      %rax,%rax
   cmp      %rax,%r9    
   je      return_sin_c
    call     fname_special_underflow  
    JMP return_sin_c

compute_x_xxx_0_1666:

    VMULSD    %xmm0,%xmm0,%xmm1                         #
    VMULSD    %xmm0,%xmm1,%xmm1                         # %xmm1 x3

    VFNMADD231SD .L_point_166(%rip),%xmm1,%xmm0         #  x - x*x*x*0.166666666666666666#
    JMP return_sin_c

compute_sin_pyby_4:
     sin_piby4_s_xx_zero_fma3
    JMP    return_sin_c

sin_early_exit_s:

    MOV   .L__inf_mask_64(%rip),%r8
    AND   %r8,%rax
    CMP   %r8,%rax
    JZ    sin_naninf    

range_reduce:

    VMOVAPD %xmm0,%xmm6
    VMOVQ   %r9,%xmm0                                # r9 x with the sign cleared

    cmp  .L_e_5(%rip),%r9   
    JGE  sin_remainder_piby2

    range_e_5_s_fma3
    JMP sin_exit_s

sin_remainder_piby2:
    call_remainder_piby2_fma3

sin_exit_s:

    VMOVQ %xmm7,%rax
    and   $0x1,%rax
    cmp   $0x1,%rax
    JZ   cos_piby4_compute
 

sin_piby4_compute:
    sin_piby4_s_fma3
    JMP sin_exit_s_1

cos_piby4_compute:
    cos_piby4_s_fma3

sin_exit_s_1:

    VPCMPEQQ .L_int_two(%rip),%xmm7,%xmm2
    VPCMPEQQ .L_int_three(%rip),%xmm7,%xmm3

    VORPD     %xmm3,%xmm2,%xmm3
    VANDNPD   .L_signbit(%rip),%xmm3,%xmm3
    VXORPD    %xmm3,%xmm0,%xmm0
    VANDNPD   .L_signbit(%rip),%xmm6,%xmm6
    VXORPD    %xmm0,%xmm6,%xmm0
   
return_sin_s:
return_sin_c:

    ret

sin_naninf:
    call    fname_special
    ret

