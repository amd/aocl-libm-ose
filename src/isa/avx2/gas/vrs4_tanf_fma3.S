#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(vrs4_tanf)

#include "trig_func.h"

.align 32
.equ    r,    0x20                               # pointer to r for amd_remainder_piby2
.equ    rr,   0x40                               # pointer to rr for amd_remainder_piby2
.equ    region,    0x60                               # pointer to region for amd_remainder_piby2
.equ    res_pi_4, 0x80
.equ    mas_res_pi_4, 0xA0
.equ    input, 0xC0
.equ    r1,    0xE0                               # pointer to r for amd_remainder_piby2
.equ    rr1,   0x100                               # pointer to rr for amd_remainder_piby2
.equ    region1,    0x120                               # pointer to region for amd_remainder_piby2
.equ    ret_addr, 0x140
.equ    store_input, 0x160
.equ   stack_size, 0x188


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:

        sub     $stack_size, %rsp
        VCVTPS2PD   %xmm0,%ymm0
tan_early_exit_s:

    VMOVUPD  %ymm0,store_input(%rsp)


    VCMPNEQPD %ymm0,%ymm0,%ymm1                     #nan mask ymm1
    VANDPD .L__sign_mask(%rip),%ymm0,%ymm8          # ymm8 clear sign

    VEXTRACTF128 $1,%ymm8,%xmm3
    VPCMPEQQ .L__inf_mask_64(%rip),%xmm8,%xmm2      #
    VPCMPEQQ .L__inf_mask_64(%rip),%xmm3,%xmm3      #
    VINSERTF128 $1,%xmm3,%ymm2,%ymm2                #ymm2 has inf mask

    VADDPD   %ymm0,%ymm0,%ymm3                      #nan value
    VANDNPD %ymm1, %ymm1, %ymm7
    VANDPD %ymm1, %ymm3, %ymm4
    VORPD %ymm7, %ymm4, %ymm4   

    VMOVUPD .L_nan(%rip),%ymm3
    VANDNPD %ymm4, %ymm2, %ymm7
    VANDPD %ymm2, %ymm3, %ymm12
    VORPD %ymm7, %ymm12, %ymm12 

    VORPD   %ymm1,%ymm2,%ymm11                      # ymm11 nan and inf mask

tan_early_exit_s_1:

    VEXTRACTF128    $1,%ymm8,%xmm7
    VMOVDQA         .L_mask_3f_9(%rip),%xmm13

    VMOVDQA         .L_mask_3f_9(%rip),%xmm5

    VPCMPGTQ        %xmm8,%xmm13,%xmm13
    VPCMPGTQ        %xmm7,%xmm5,%xmm5

    VINSERTF128 $1,%xmm5,%ymm13,%ymm13

    VORPD   %ymm11,%ymm13,%ymm14
    VPTEST %ymm14,%ymm14
    JZ range_reduce


    VMOVAPD %ymm0,%ymm10
    VMULPD  %ymm0,%ymm0,%ymm1                       #
    VMULPD  %ymm0,%ymm1,%ymm1                       # ymm1 x3
    VFMADD132PD .L_point_333(%rip),%ymm0,%ymm1         #  x + x*x*x*0.3333333333333333;
    VMOVDQA %ymm1, %ymm14
    VXORPD  %ymm2,%ymm2,%ymm2
    tan_piby4_s4f_fma3


    VMOVDQA .L_mask_3e4(%rip),%xmm3
    VMOVDQA .L_mask_3e4(%rip),%xmm6
    VPCMPGTQ    %xmm8,%xmm3,%xmm3
    VPCMPGTQ    %xmm7,%xmm6,%xmm6

    VINSERTF128 $1,%xmm6,%ymm3,%ymm3

    VMOVDQA  .L_mask_3f2(%rip),%xmm4
    VMOVDQA  .L_mask_3f2(%rip),%xmm5

    VPCMPGTQ    %xmm8,%xmm4,%xmm4       
    VPCMPGTQ    %xmm7,%xmm5,%xmm7
    VINSERTF128 $1,%xmm7,%ymm4,%ymm4


    VANDNPD %ymm13,%ymm4,%ymm1
    VANDPD  %ymm0,%ymm1,%ymm1               # res2

    VANDNPD %ymm13,%ymm3,%ymm5
    VANDPD  %ymm5,%ymm4,%ymm5
    VANDPD  %ymm14,%ymm5,%ymm5

    VANDPD  %ymm13,%ymm3,%ymm3
    VANDPD  %ymm10,%ymm3,%ymm3
    VORPD   %ymm3,%ymm5,%ymm5               # res1 amm5
    VORPD   %ymm1,%ymm5,%ymm0               # res_pi_4
    VANDNPD %ymm0, %ymm11, %ymm14
    VANDPD %ymm11, %ymm12, %ymm0
    VORPD %ymm14, %ymm0, %ymm0

    VPTEST .L__allone_mask(%rip),%ymm14
    JC return_tanf_s

    VMOVUPD  %ymm0,res_pi_4(%rsp)

range_reduce:

    VORPD   %ymm11,%ymm13,%ymm13
    VMOVUPD  %ymm13,mas_res_pi_4(%rsp)
    VANDNPD %ymm8,%ymm13,%ymm0                     # ymm0 x with the sign cleared
    VMOVUPD %ymm0,input(%rsp)

    VCMPGEPD .L_e_5(%rip),%ymm0,%ymm3
    VPTEST %ymm3,%ymm3
    JZ call_range_e5

    call_remainder_2fpiby2_4f_4d_fma3

    VMOVUPD %ymm0,r1(%rsp)
    VMOVUPD %ymm11,region1(%rsp)
    VMOVUPD input(%rsp),%ymm0

    VCMPLTPD .L_e_5(%rip),%ymm0,%ymm3
    VPTEST %ymm3,%ymm3
    JZ if_not_remainder

call_range_e5:

    range_e_5_s4f_fma3

if_not_remainder:


    VMOVUPD input(%rsp),%ymm2
    VCMPLTPD .L_e_5(%rip),%ymm2,%ymm3

    VANDNPD r1(%rsp), %ymm3, %ymm2
    VANDPD %ymm3, %ymm0, %ymm0
    VORPD %ymm2, %ymm0, %ymm0

    VANDNPD region1(%rsp), %ymm3, %ymm2
    VANDPD %ymm3, %ymm11, %ymm11
    VORPD %ymm2, %ymm11, %ymm11


    VANDPD  .L_int_one(%rip),%ymm11,%ymm2

    tan_piby4_s4f_fma3

ret_tan_2fpiby4_s:

    VMOVUPD  store_input(%rsp),%ymm1
    VANDPD  .L_signbit(%rip),%ymm1,%ymm1
    VXORPD  %ymm0,%ymm1,%ymm4

    VMOVUPD mas_res_pi_4(%rsp),%ymm11
    VMOVUPD res_pi_4(%rsp),%ymm0

    VANDNPD %ymm4, %ymm11, %ymm2
    VANDPD %ymm11, %ymm0, %ymm0
    VORPD %ymm2, %ymm0, %ymm0

return_tanf_s:
    VCVTPD2PS   %ymm0,%xmm0
    add      $stack_size, %rsp
    ret
