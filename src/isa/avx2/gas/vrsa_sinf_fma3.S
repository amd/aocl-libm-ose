#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
#define fname ALM_PROTO_FMA3(vrsa_sinf)

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
.equ    input_val, 0x160
.equ   stack_size, 0x188



#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
mov %rsi, %r14
mov %rdx, %r15


cmp         $0x0, %edi
    jle         .L__return
    cmp         $0x0, %r14
    je         .L__return
    cmp         $0x0, %r15
    je         .L__return

.p2align 4
.L__process_next4:
    sub         $0x4, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first123
    movdqu      (%r14, %rdi, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first123:
    je          .L__process_first3
    cmp         $-3, %rdi
    jl          .L__return #multiple of 4elements, all are processed %rdi == -4
    je          .L__process_first1
    #process first 2
    mov         $0x0, %rcx
    movsd       (%r14, %rcx, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first1:
    mov         $0x0, %rcx
    movss       (%r14, %rcx, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first3:
    mov         $0x1, %rcx
    movsd       (%r14, %rcx, 4), %xmm0
    dec         %rcx  # zero
    movss       (%r14, %rcx, 4), %xmm1
    pslldq      $4,%xmm0
    por         %xmm1, %xmm0

.p2align 4
.L__start:

        sub     $stack_size, %rsp

        VCVTPS2PD   %xmm0,%ymm0
sin_early_exit_s:

    VMOVUPD  %ymm0,input_val(%rsp)

    VCMPNEQPD %ymm0,%ymm0,%ymm1                     #nan mask ymm1
    VANDPD .L__sign_mask(%rip),%ymm0,%ymm8          # ymm8 clear sign
    VEXTRACTF128 $1,%ymm8,%xmm3
    VPCMPEQQ .L__inf_mask_64(%rip),%xmm8,%xmm2      #
    VPCMPEQQ .L__inf_mask_64(%rip),%xmm3,%xmm3      #
    VINSERTF128 $1,%xmm3,%ymm2,%ymm2                #ymm2 has inf mask
    VADDPD   %ymm0,%ymm0,%ymm3                      #nan value
    VANDNPD %ymm1, %ymm1, %ymm11
    VANDPD %ymm1, %ymm3, %ymm4
    VORPD %ymm11, %ymm4, %ymm4

    VMOVUPD .L_nan(%rip),%ymm3
    VANDNPD %ymm4, %ymm2, %ymm11
    VANDPD %ymm2, %ymm3, %ymm12
    VORPD %ymm11, %ymm12, %ymm12

    VORPD   %ymm1,%ymm2,%ymm11                      # ymm11 nan and inf mask



sin_early_exit_s_1:

    VEXTRACTF128 $1,%ymm8,%xmm7

    VMOVDQA   .L_mask_3f_9(%rip),%xmm13
    VMOVDQA   .L_mask_3f_9(%rip),%xmm5

    VPCMPGTQ    %xmm8,%xmm13,%xmm13
    VPCMPGTQ    %xmm7,%xmm5,%xmm5



    VINSERTF128 $1,%xmm5,%ymm13,%ymm13

    VORPD   %ymm11,%ymm13,%ymm14
    VPTEST %ymm14,%ymm14
    JZ range_reduce


    VMOVAPD %ymm0,%ymm10
    VMULPD  %ymm0,%ymm0,%ymm1                       #
    VMULPD  %ymm0,%ymm1,%ymm1                       # ymm1 x3
    VFNMADD132PD .L_point_166(%rip),%ymm0,%ymm1         #  x - x*x*x*0.166666666666666666;
    VMOVDQA %ymm1, %ymm9
    VXORPD  %ymm1,%ymm1,%ymm1
    sin_piby4_s4f_fma3

    VMOVDQA  .L_mask_3e4(%rip),%xmm3
    VMOVDQA  .L_mask_3e4(%rip),%xmm6
    VPCMPGTQ    %xmm8,%xmm3,%xmm3
    VPCMPGTQ    %xmm7,%xmm6,%xmm6

    VINSERTF128 $1,%xmm6,%ymm3,%ymm3

    VMOVDQA  .L_mask_3f2(%rip), %xmm4
    VMOVDQA  .L_mask_3f2(%rip), %xmm5
    VPCMPGTQ    %xmm8,%xmm4,%xmm4
    VPCMPGTQ    %xmm7,%xmm5,%xmm7


    VINSERTF128 $1,%xmm7,%ymm4,%ymm4

    VANDNPD %ymm13,%ymm4,%ymm1
    VANDPD  %ymm0,%ymm1,%ymm1               # res2
    VANDNPD %ymm13,%ymm3,%ymm5
    VANDPD  %ymm5,%ymm4,%ymm5
    VANDPD  %ymm9,%ymm5,%ymm5
    VANDPD  %ymm13,%ymm3,%ymm3
    VANDPD  %ymm10,%ymm3,%ymm3
    VORPD   %ymm3,%ymm5,%ymm5               # res1 amm5
    VORPD   %ymm1,%ymm5,%ymm0               # res_pi_4
    VANDNPD %ymm0, %ymm11, %ymm14
    VANDPD %ymm11, %ymm12, %ymm0
    VORPD %ymm14, %ymm0, %ymm0

    VPTEST .L__allone_mask(%rip),%ymm14
    JC return_sin_s

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

    VANDNPD r1(%rsp), %ymm3, %ymm8
    VANDPD %ymm3, %ymm0, %ymm7
    VORPD %ymm8, %ymm7, %ymm7

    VANDNPD region1(%rsp), %ymm3, %ymm8
    VANDPD %ymm3, %ymm11, %ymm11
    VORPD %ymm8, %ymm11, %ymm11


    VMOVAPD %ymm7,%ymm0
    cos_piby4_s4f_fma3

ret_cos_2fpiby4_s:


    VMOVUPD %ymm0,%ymm8     # store sin piby4

    VMOVAPD %ymm7,%ymm0
    sin_piby4_s4f_fma3

ret_sin_2fpiby4_s:

    VPCMPEQQ .L_int_one(%rip),%xmm11,%xmm1
    VPCMPEQQ .L_int_two(%rip),%xmm11,%xmm2
    VPCMPEQQ .L_int_three(%rip),%xmm11,%xmm3

    VEXTRACTF128 $1,%ymm11,%xmm6

    VPCMPEQQ  .L_int_one(%rip),%xmm6,%xmm4
    VPCMPEQQ  .L_int_two(%rip),%xmm6,%xmm5
    VPCMPEQQ  .L_int_three(%rip),%xmm6,%xmm6

    VINSERTF128 $1,%xmm4,%ymm1,%ymm1
    VINSERTF128 $1,%xmm5,%ymm2,%ymm2
    VINSERTF128 $1,%xmm6,%ymm3,%ymm3

    VORPD   %ymm1,%ymm3,%ymm4
    VANDNPD %ymm0, %ymm4, %ymm11
    VANDPD %ymm4, %ymm8, %ymm5
    VORPD %ymm11, %ymm5, %ymm5


    VORPD   %ymm3,%ymm2,%ymm4
    VANDNPD .L_signbit(%rip),%ymm4,%ymm4
    VXORPD  %ymm4,%ymm5,%ymm4

    VMOVUPD input_val(%rsp),%ymm0
    VANDNPD .L_signbit(%rip),%ymm0,%ymm0
    VXORPD  %ymm4,%ymm0,%ymm4


    VMOVUPD mas_res_pi_4(%rsp),%ymm11
    VMOVUPD res_pi_4(%rsp),%ymm0

    VANDNPD %ymm4, %ymm11, %ymm5
    VANDPD %ymm11, %ymm0, %ymm0
    VORPD %ymm5, %ymm0, %ymm0


return_sin_s:
    VCVTPD2PS   %ymm0,%xmm0
    add      $stack_size, %rsp
    cmp         $-1,  %rdi
        jle          .L__store123
        movdqu       %xmm0, (%r15, %rdi, 4)
        jmp         .L__process_next4

    .p2align 4
    .L__store123:
        je         .L__store3
        cmp         $-3, %rdi
        je          .L__store1
        #store 2
        add        $0x2, %rdi
        movsd      %xmm0, (%r15,%rdi,4)
        jmp         .L__return

    .p2align 4
    .L__store3:
        movdqa      %xmm0, %xmm1
        psrldq      $4, %xmm0
        inc         %rdi
        movss       %xmm1, (%r15,%rdi,4)
        inc         %rdi
        movsd      %xmm0, (%r15,%rdi,4)
        jmp         .L__return

    .p2align 4
    .L__store1:
        mov        $0x0, %rdi
        movss      %xmm0, (%r15,%rdi,4)

    .L__return:
    ret

