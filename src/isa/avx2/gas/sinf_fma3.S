#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(sinf)
#define fname_special   _sinf_special@PLT
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

sinf_early_exit_s:

    MOVD  %xmm0,%eax
    MOV   .L__inf_mask_32(%rip),%r8d
   
    AND   %r8d,%eax
    CMP   %r8d,%eax
    JZ    sinf_naninf

    VCVTSS2SD    %xmm0,%xmm0,%xmm5
    VMOVQ    %xmm5,%r9

    AND    .L__sign_mask(%rip),%r9                          #clear sign

sinf_early_exit_s_1:

    CMP   .L_mask_3fe(%rip),%r9
    JG    range_reduce

    CMP   .L_mask_3f8(%rip),%r9
    JGE    compute_sinf_pyby_4

    CMP   .L_mask_3f2(%rip),%r9
    JGE   compute_x_xxx_0_1666

 
##  Additional code to raise underflow exception
    xor   %rax,%rax
    cmp   %rax,%r9  
    je    return_sinf_c
    call  fname_special_underflow  
    JMP   return_sinf_c

compute_x_xxx_0_1666:
    VMULSD    %xmm5,%xmm5,%xmm1                         #
    VMULSD    %xmm5,%xmm1,%xmm0                         # %xmm1 x3

   VFNMADD132SD  .L_point_166(%rip),%xmm5,%xmm0         #  x - x*x*x*0.166666666666666666#
    JMP return_sinf_s

compute_sinf_pyby_4:
   
    VMOVSD %xmm5,%xmm5,%xmm0
    sin_piby4_sf_fma3
    JMP    return_sinf_s

range_reduce:
    VMOVQ   %r9,%xmm0                                # %r9 x with the sign cleared
    cmp  .L_e_5(%rip),%r9
    JGE  sinf_remainder_piby2

#sinf_range_e_5_s:
    range_e_5_sf_fma3
    JMP sinf_exit_s

sinf_remainder_piby2:
    call_remainder_piby2_f

sinf_exit_s:

    VMOVQ  %xmm4,%rax
    and   $0x1,%rax     
    cmp   $0x1,%rax
    JZ    cos_piby4_compute


sin_piby4_compute:
    sin_piby4_sf_fma3
    JMP sinf_exit_s_1

cos_piby4_compute:
    cos_piby4_sf_fma3

sinf_exit_s_1:

     VPCMPEQQ    .L_int_two(%rip),%xmm4,%xmm2
     VPCMPEQQ    .L_int_three(%rip),%xmm4,%xmm3
    VORPD    %xmm3,%xmm2,%xmm3
    VANDNPD    .L_signbit(%rip),%xmm3,%xmm3
    VXORPD    %xmm3,%xmm0,%xmm0

    VANDNPD   .L_signbit(%rip),%xmm5,%xmm1
    VXORPD    %xmm0,%xmm1,%xmm0

return_sinf_s:

    VCVTSD2SS    %xmm0,%xmm0,%xmm0

    ret

return_sinf_c:
    ret

sinf_naninf:
    call    fname_special
    ret

