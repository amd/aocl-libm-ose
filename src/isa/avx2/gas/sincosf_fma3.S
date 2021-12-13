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
#define fname ALM_PROTO_FMA3(sincosf)
#define fname_special   _sincosf_special@PLT
#define fname_special_underflow _sin_special_underflow@PLT

#include "trig_func.h"



.equ    sin_addr,    0x10                              
.equ    cos_addr,   0x20                               
.equ    store_input, 0x30
.equ   stack_size, 0x48



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
	#ifdef _WIN64
        mov %rdx, %rdi #sin pointer
        mov %r8, %rsi #cos pointer
	#endif

sincosf_early_exit_s:

    MOVD  %xmm0,%eax
    MOV   %eax,%r9d
    MOV   .L__inf_mask_32(%rip),%r8d
    AND   %r8d,%eax
    CMP   %r8d,%eax
    JZ    sincosf_naninf

    VCVTSS2SD    %xmm0,%xmm0,%xmm0
    VMOVAPD   %xmm0, store_input(%rsp)

    MOVD  %xmm0,%r9
    AND    .L__sign_mask(%rip),%r9                          #clear sign

sincosf_early_exit_s_1:

    CMP   .L_mask_3fe(%rip),%r9
    JG    range_reduce
    CMP  .L_mask_3f2(%rip),%r9
    JGE   compute_sincosf_pyby_4
    CMP   .L_mask_3e4(%rip),%r9
    JGE   compute_x_xxx_0_1666_0_5          ;


##  Additional code to raise underflow exception
   VMOVQ   .L_one(%rip),%xmm1
   xor      %rax,%rax
   cmp      %rax,%r9
   je      return_sincosf_s
   
   mov     %rdi, sin_addr(%rsp)
   mov     %rsi, cos_addr(%rsp)   
   call     fname_special_underflow
   mov     sin_addr(%rsp), %rdi
   mov     cos_addr(%rsp), %rsi   
   VMOVQ   .L_one(%rip),%xmm1
   
   JMP return_sincosf_s

compute_x_xxx_0_1666_0_5:
    VMOVSD .L_one(%rip),%xmm3

#sin
    VMULSD    %xmm0,%xmm0,%xmm1                        # x2
    VMULSD    %xmm1,%xmm0,%xmm2                        # x3
    VFNMADD231SD .L_point_166(%rip),%xmm2,%xmm0     #  sin   x - x*x*x*0.166666666666666666 #
#cos

    VFNMADD132SD .L_point_five(%rip),%xmm3,%xmm1  #cos  1.0 - x*x*(double2)0.5

    JMP return_sincosf_s

compute_sincosf_pyby_4:
    sincosf_piby4_s_fma3

    JMP    return_sincosf_s

range_reduce:

    VMOVQ   %r9,%xmm0                                # %r9 x with the sign cleared
    cmp  .L_e_5(%rip),%r9
    JGE  sincosf_remainder_piby2

#sin_range_e_5_s:
    range_e_5_sf_fma3
    JMP sincosf_exit_s

sincosf_remainder_piby2:

    mov     %rdi, sin_addr(%rsp)
    mov     %rsi, cos_addr(%rsp)
    
    call_remainder_piby2_f
    
    mov     sin_addr(%rsp), %rdi
    mov     cos_addr(%rsp), %rsi

sincosf_exit_s:
    VMOVAPD %xmm4,%xmm11
    sincosf_piby4_s_fma3
    
    VANDPD     .L_int_one(%rip),%xmm11,%xmm3
    VPCMPEQQ   .L_int_one(%rip),%xmm3,%xmm3
    VMOVDDUP    %xmm3,%xmm3
    VPTEST    %xmm3,%xmm3

    JZ    sin_piby4_compute
    JMP    cos_piby4_compute

sin_piby4_compute:
    JMP sincosf_exit_1

cos_piby4_compute:
    VMOVAPD %xmm0,%xmm2
    VMOVAPD %xmm1,%xmm0
    VMOVAPD %xmm2,%xmm1

sincosf_exit_1:

#sin
    VMOVSD    store_input(%rsp),%xmm5
    VPCMPEQQ    .L_int_two(%rip),%xmm11,%xmm2
    VPCMPEQQ    .L_int_three(%rip),%xmm11,%xmm3
    VORPD    %xmm3,%xmm2,%xmm3
    VANDNPD   .L_signbit(%rip),%xmm3, %xmm3
    VXORPD    %xmm3,%xmm0,%xmm0
    VANDNPD   .L_signbit(%rip),%xmm5,%xmm5
    VXORPD    %xmm0,%xmm5,%xmm0

#cos
    VPCMPEQQ    .L_int_one(%rip),%xmm11,%xmm4
    VORPD    %xmm2,%xmm4,%xmm3

    VANDPD    .L_signbit(%rip),%xmm3,%xmm3
    VXORPD    %xmm1,%xmm3,%xmm1

return_sincosf_s:

    VCVTSD2SS    %xmm0,%xmm0,%xmm0
    VCVTSD2SS    %xmm1,%xmm1,%xmm1

    VMOVSS   %xmm0, (%rdi)                             # save the sin
    VMOVSS   %xmm1, (%rsi)                             # save the cos    
    add      $stack_size, %rsp
    ret

sincosf_naninf:
    call    fname_special
    add      $stack_size, %rsp
    ret

