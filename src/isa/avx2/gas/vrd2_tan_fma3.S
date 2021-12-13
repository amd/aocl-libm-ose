#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#include "fn_macros.h"
#define fname ALM_PROTO_FMA3(vrd2_tan)

#include "trig_func.h"

.equ    r,    0x10                               # pointer to r for amd_remainder_piby2
.equ    rr,   0x20                               # pointer to rr for amd_remainder_piby2
.equ    region,    0x30                               # pointer to region for amd_remainder_piby2
.equ    ret_addr, 0x40
.equ    res_pi_4, 0x50
.equ    mas_res_pi_4, 0x60
.equ    input, 0x70
.equ    r1,    0x80                               # pointer to r for amd_remainder_piby2
.equ    rr1,   0x90                               # pointer to rr for amd_remainder_piby2
.equ    region1,    0x100                               # pointer to region for amd_remainder_piby2
.equ    store_input,    0x110                               # pointer to region for amd_remainder_piby2
.equ   stack_size, 0x128

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:

        sub     $stack_size, %rsp


tan_early_exit_s:

    VMOVAPD  %xmm0,store_input(%rsp)

    VCMPNEQPD %xmm0,%xmm0,%xmm1                     #nan mask xmm1
    VANDPD    .L__sign_mask(%rip),%xmm0,%xmm13         # xmm13 clear sign

    VPCMPEQQ  .L__inf_mask_64(%rip),%xmm13,%xmm2     #xmm2 has inf mask
    VADDPD   %xmm0,%xmm0,%xmm3                      #nan value


    VANDNPD %xmm1, %xmm1, %xmm11
    VANDPD %xmm1, %xmm3, %xmm4
    VORPD %xmm11, %xmm4, %xmm4

    VMOVAPD .L_nan(%rip),%xmm3
 
    VANDNPD %xmm4, %xmm2, %xmm11
    VANDPD %xmm2, %xmm3, %xmm12
    VORPD %xmm11, %xmm12, %xmm12

    VORPD   %xmm1,%xmm2,%xmm11                      # xmm11 nan and inf mask

tan_early_exit_s_1:
    
    VMOVDQA     .L_mask_3f_9(%rip),%xmm7
    VPCMPGTQ     %xmm13,%xmm7,%xmm7

    VORPD        %xmm11,%xmm7,%xmm2
    VPTEST       %xmm2,%xmm2
    JZ range_reduce


    VMOVAPD %xmm0,%xmm10
    VMULPD  %xmm0,%xmm0,%xmm14                       #
    VMULPD  %xmm0,%xmm14,%xmm14                       # xmm1 x3
    VFMADD132PD .L_point_333(%rip),%xmm0,%xmm14         #  x + x*x*x*0.3333333333333333;

    VXORPD    %xmm3,%xmm3,%xmm3
    VXORPD    %xmm1,%xmm1,%xmm1

  tan_2fpiby4_s_fma3


    VMOVDQA    .L_mask_3e4(%rip),%xmm3
    VMOVDQA    .L_mask_3f2(%rip),%xmm4
    VPCMPGTQ    %xmm13,%xmm3,%xmm3
    VPCMPGTQ    %xmm13,%xmm4,%xmm4

    VANDNPD %xmm7,%xmm4,%xmm1
    VANDPD  %xmm0,%xmm1,%xmm1               # res2


    VANDNPD %xmm7,%xmm3,%xmm5
    VANDPD  %xmm5,%xmm4,%xmm5
    VANDPD  %xmm14,%xmm5,%xmm5

    VANDPD  %xmm7,%xmm3,%xmm3
    VANDPD  %xmm10,%xmm3,%xmm3
    VORPD   %xmm3,%xmm5,%xmm5               # res1 amm5
    VORPD   %xmm1,%xmm5,%xmm0               # res_pi_4

    VANDNPD %xmm0, %xmm11, %xmm3
    VANDPD %xmm11, %xmm12, %xmm0
    VORPD %xmm3, %xmm0, %xmm0
    


range_reduce:

    VORPD   %xmm11,%xmm7,%xmm7
    VMOVAPD  %xmm0,res_pi_4(%rsp)
    VMOVAPD  %xmm7,mas_res_pi_4(%rsp)

    VANDNPD %xmm13,%xmm7,%xmm0                     # xmm0 x with the sign cleared
    VMOVAPD %xmm0,input(%rsp)

    VCMPGEPD .L_e_5(%rip),%xmm0,%xmm3
    VPTEST %xmm3,%xmm3

    JZ call_range_e5
    # call_remainder_2fpiby2_fma3   
    call_remainder_2d_piby2_fma3
    
    VMOVAPD %xmm0,r1(%rsp)
    VMOVAPD %xmm1,rr1(%rsp)
    VMOVAPD %xmm11,region1(%rsp)

call_range_e5:

    VMOVAPD input(%rsp),%xmm0
    range_e_5_2f_s_fma3

if_not_remainder:

    VMOVAPD input(%rsp),%xmm2
    VCMPLTPD .L_e_5(%rip),%xmm2,%xmm3
   

    VANDNPD r1(%rsp),%xmm3, %xmm2
    VANDPD %xmm3, %xmm0, %xmm0
    VORPD %xmm2, %xmm0, %xmm0   

  
    VANDNPD rr1(%rsp), %xmm3, %xmm2
    VANDPD %xmm3, %xmm1, %xmm1
    VORPD %xmm1,%xmm2, %xmm1

     
     VANDNPD   region1(%rsp),%xmm3,%xmm2
     VANDPD    %xmm3,%xmm11,%xmm11 
     VORPD     %xmm2,%xmm11,%xmm11 
     VANDPD    .L_int_one(%rip),%xmm11,%xmm3

    tan_2fpiby4_s_fma3


ret_tan_2fpiby4_s:

    VMOVAPD  store_input(%rsp),%xmm1
    VANDPD  .L_signbit(%rip),%xmm1,%xmm1
    VXORPD  %xmm0,%xmm1,%xmm4

    VMOVAPD mas_res_pi_4(%rsp),%xmm8
    VMOVAPD res_pi_4(%rsp),%xmm0

  
    VANDNPD %xmm4, %xmm8, %xmm4
    VANDPD %xmm0, %xmm8, %xmm0
    VORPD %xmm4, %xmm0, %xmm0

return_cos_s:

    add      $stack_size, %rsp
    ret


