#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(tanf)
#define fname_special   _tanf_special@PLT
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

tan_early_exit_s:

    VMOVD  %xmm0,%eax
    MOV   .L__inf_mask_32(%rip),%r8d
    AND   %r8d, %eax
    CMP   %r8d,%eax
    JZ    tanf_naninf   

    VCVTSS2SD    %xmm0,%xmm0,%xmm5
    VMOVQ    %xmm5,%r9
    AND    .L__sign_mask(%rip),%r9                          #clear sign


tan_early_exit_s_1:

    CMP   .L_mask_3fe(%rip),%r9
    JG    range_reduce
    CMP   .L_mask_3f8(%rip),%r9
    JGE    compute_tanf_pyby_4
    CMP   .L_mask_3f2(%rip),%r9
    JGE   compute_x_xxx_0_333


##  Additional code to raise underflow exception
    xor   %rax,%rax
    cmp   %rax,%r9
    je    return_tanf_c    
    call  fname_special_underflow
    JMP   return_tanf_c

compute_x_xxx_0_333:
    VMULSD    %xmm5,%xmm5,%xmm2                         #
    VMULSD    %xmm5,%xmm2,%xmm0                         # %xmm1 x3
    VFMADD132SD  .L_point_333(%rip),%xmm5,%xmm0         #  x + x*x*x*0.3333333333333333;
    JMP return_tanf_s

compute_tanf_pyby_4:
    VMOVSD %xmm5,%xmm5,%xmm0
   tan_piby4_sf_zeor_fma3
    JMP    return_tanf_s

range_reduce:

    VMOVQ   %r9,%xmm0                                # %r9 x with the sign cleared
    cmp  .L_e_5(%rip),%r9
    JGE  tanf_remainder_piby2

#tan_range_e_5_s:

    range_e_5_sf_fma3
    JMP tanf_exit_s

tanf_remainder_piby2:
    call_remainder_piby2_f
    

tanf_exit_s:
    VANDPD  .L_int_one(%rip),%xmm4,%xmm2
    movd  %xmm2,%eax
    tan_piby4_sf_fma3

tanf_exit_s_1:

    VANDPD  .L_signbit(%rip),%xmm5,%xmm5
    VXORPD  %xmm5,%xmm0,%xmm0

return_tanf_s:

    VCVTSD2SS   %xmm0,%xmm0,%xmm0  


return_tanf_c:
    ret

tanf_naninf:
    call    fname_special
    ret

