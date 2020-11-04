#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
#define fname FN_PROTOTYPE_FMA3(vrda_sin)

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
.equ    input_val,    0x110                               # pointer to region for amd_remainder_piby2
.equ   stack_size, 0x128


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:
mov %rsi,%r14
mov %rdx,%r15


 cmp         $0x0, %edi
    jle         .L__return    
    cmp         $0x0, %r14
    je         .L__return
    cmp         $0x0, %r15
    je         .L__return
   
.p2align 4    
.L__process_next2:
    sub         $0x2, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first1
    movdqu      (%r14, %rdi, 8), %xmm0
    jmp         .L__start
    
.p2align 4    
.L__process_first1:
    jl          .L__return #even elements, all are processed %rdi == -2
    mov         $0x0, %rcx
    movsd       (%r14, %rcx, 8), %xmm0    
    
.p2align 4    
.L__start:    
        sub     $stack_size, %rsp

sin_early_exit_s:


    VMOVAPD  %xmm0,input_val(%rsp)
    VCMPNEQPD %xmm0,%xmm0,%xmm1                     #nan mask xmm1

    VANDPD .L__sign_mask(%rip),%xmm0,%xmm8          # xmm8 clear sign
    VPCMPEQQ .L__inf_mask_64(%rip),%xmm8,%xmm2      #xmm2 has inf mask

    VADDPD   %xmm0,%xmm0,%xmm3                      #nan value


    VANDNPD %xmm1, %xmm1, %xmm11
    VANDPD %xmm1, %xmm3, %xmm4

    VORPD %xmm11, %xmm4, %xmm4
    VMOVAPD .L_nan(%rip),%xmm3


    VANDNPD %xmm4, %xmm2, %xmm11
    VANDPD %xmm2, %xmm3, %xmm12

    VORPD %xmm11, %xmm12, %xmm12

    VORPD   %xmm1,%xmm2,%xmm11                      # xmm11 nan and inf mask
sin_early_exit_s_1:


    VMOVDQA     .L_mask_3f_9(%rip),%xmm7
    VPCMPGTQ     %xmm8,%xmm7,%xmm7

    VORPD   %xmm11,%xmm7,%xmm2
    VPTEST %xmm2,%xmm2
    JZ range_reduce

    VMOVAPD %xmm0,%xmm10
    VMULPD  %xmm0,%xmm0,%xmm1                       #
    VMULPD  %xmm0,%xmm1,%xmm1                       # xmm1 x3

    VFNMADD132PD .L_point_166(%rip),%xmm0,%xmm1         #  x - x*x*x*0.166666666666666666;
    VMOVDQA %xmm1, %xmm9
    VXORPD  %xmm1,%xmm1,%xmm1

    sin_2fpiby4_s_xx_zero_fma3
    VMOVDQA   .L_mask_3e4(%rip),%xmm3
    VMOVDQA   .L_mask_3f2(%rip),%xmm4



    VPCMPGTQ    %xmm8,%xmm3,%xmm3
    VPCMPGTQ    %xmm8,%xmm4,%xmm4

    VANDNPD %xmm7,%xmm4,%xmm1
    VANDPD  %xmm0,%xmm1,%xmm1               # res2
    VANDNPD %xmm7,%xmm3,%xmm5
    VANDPD  %xmm5,%xmm4,%xmm5
    VANDPD  %xmm9,%xmm5,%xmm5
    VANDPD  %xmm7,%xmm3,%xmm3
    VANDPD  %xmm10,%xmm3,%xmm3
    VORPD   %xmm3,%xmm5,%xmm5               # res1 amm5
    VORPD   %xmm1,%xmm5,%xmm0               # res_pi_4


    VANDNPD %xmm0, %xmm11, %xmm2
    VANDPD %xmm11, %xmm12, %xmm0
    VORPD %xmm2, %xmm0, %xmm0

    VPTEST .L__allone_mask(%rip),%xmm2
    JC return_sin_s

range_reduce:

    VORPD   %xmm11,%xmm7,%xmm7
    VMOVAPD  %xmm0,res_pi_4(%rsp)
    VMOVAPD  %xmm7,mas_res_pi_4(%rsp)

    VANDNPD %xmm8,%xmm7,%xmm0                     # xmm0 x with the sign cleared
    VMOVAPD %xmm0,input(%rsp)

    VCMPGEPD .L_e_5(%rip),%xmm0,%xmm3
    VPTEST %xmm3,%xmm3

    JZ call_range_e5

#    call_remainder_2fpiby2_fma3
    call_remainder_2d_piby2_fma3    

ret_remainder_2fpiby2:

    VMOVAPD %xmm0,r1(%rsp)
    VMOVAPD %xmm1,rr1(%rsp)
    VMOVAPD %xmm11,region1(%rsp)

call_range_e5:
    VMOVAPD input(%rsp),%xmm0
    range_e_5_2f_s_fma3

if_not_remainder:

    VMOVAPD input(%rsp),%xmm2 
    VCMPLTPD .L_e_5(%rip),%xmm2,%xmm3

   
    VANDNPD r1(%rsp), %xmm3, %xmm2
    VANDPD %xmm3, %xmm0, %xmm7
    VORPD %xmm2, %xmm7, %xmm7   



    VANDNPD rr1(%rsp), %xmm3, %xmm2
    VANDPD %xmm3, %xmm1, %xmm8
    VORPD %xmm2, %xmm8, %xmm8   


    VANDNPD region1(%rsp), %xmm3, %xmm2
    VANDPD %xmm3, %xmm11, %xmm11
    VORPD %xmm2, %xmm11, %xmm11

    VPCMPEQQ .L_int_one(%rip),%xmm11,%xmm1
    VPCMPEQQ .L_int_two(%rip),%xmm11,%xmm2
    VPCMPEQQ .L_int_three(%rip),%xmm11,%xmm3

    VORPD   %xmm1,%xmm3,%xmm10
    VORPD   %xmm3,%xmm2,%xmm9

    VMOVAPD %xmm7,%xmm0
    VMOVAPD %xmm8,%xmm1

    VMOVD %xmm10,%r8
    VPUNPCKHQDQ %xmm10,%xmm10,%xmm10
    VMOVD %xmm10,%r9

    cmp $0xFFFFFFFF,%r8d
    je compute_xxx_cos

    cmp $0xFFFFFFFF,%r9d
    je compute_cos_sin


both_sin_sin:
        sin_2fpiby4_s_fma3

        jmp ret_sin_2fpiby4_s_fma3

compute_cos_sin:

        sin_piby4_s_fma3
        VMOVAPD %xmm0,%xmm10
        VPUNPCKHQDQ %xmm7,%xmm7,%xmm0
        VPUNPCKHQDQ %xmm8,%xmm8,%xmm1
        cos_piby4_s_fma3
        VPUNPCKLQDQ %xmm0,%xmm10,%xmm0

        jmp ret_sin_2fpiby4_s_fma3

compute_xxx_cos:
    cmp $0xFFFFFFFF,%r9d
    je compute_cos_cos

        cos_piby4_s_fma3
        VMOVAPD %xmm0,%xmm10
        VPUNPCKHQDQ %xmm7,%xmm7,%xmm0
        VPUNPCKHQDQ %xmm8,%xmm8,%xmm1
        sin_piby4_s_fma3
        VPUNPCKLQDQ %xmm0,%xmm10,%xmm0
        jmp ret_sin_2fpiby4_s_fma3

compute_cos_cos:

    cos_2fpiby4_s_fma3

ret_sin_2fpiby4_s_fma3:


    VANDNPD .L_signbit(%rip),%xmm9,%xmm9
    VXORPD  %xmm9,%xmm0,%xmm9

    VMOVAPD input_val(%rsp),%xmm0
    VANDNPD .L_signbit(%rip),%xmm0,%xmm0
    VXORPD  %xmm9,%xmm0,%xmm9


    VMOVAPD mas_res_pi_4(%rsp),%xmm8
    VMOVAPD res_pi_4(%rsp),%xmm0

    VANDNPD %xmm9, %xmm8, %xmm12
    VANDPD %xmm8, %xmm0, %xmm0
    VORPD %xmm12, %xmm0, %xmm0

return_sin_s:

    add      $stack_size, %rsp
cmp         $-1,  %rdi
    je          .L__store1
    movdqu       %xmm0, (%r15, %rdi, 8)
    jmp         .L__process_next2    
    
.p2align 4    
.L__store1:
    inc        %rdi
    movsd      %xmm0, (%r15,%rdi,8)
    
.L__return:    
    ret      
