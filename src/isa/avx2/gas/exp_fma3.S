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
#include "exp_tables.h"
#define fname ALM_PROTO_FMA3(exp)
#define fname_special alm_exp_special@PLT
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    vpand       .L__signbit_off(%rip), %xmm0, %xmm4
    vmovq       %xmm4, %rax
    #check if x is greater than 709.78271 or less than -709.78271
    cmp         .L__max_exp_arg(%rip), %rax
    jl         .L__exp_continue

    vucomisd    .L__max_exp_arg(%rip), %xmm0
    ja          .L__y_is_inf
    jp          .L__y_is_nan
    vucomisd    .L__denormal_tiny_threshold(%rip), %xmm0
    jbe         .L__y_is_zero

.if __enable_IEEE_exceptions
#    vmovdqa        %xmm0,%xmm1
#    vpand         .L__signbit_off(%rip),%xmm0,%xmm1
#    vucomisd      .L__real_x_near0_threshold(%rip),%xmm1
#    jb            .L__y_is_one
.endif

.L__exp_continue:
    # x * (64/ln(2))
    vmulsd       .L__real_64_by_log2(%rip), %xmm0,%xmm1

    # n = int( x * (64/ln(2)) )
    #This is done by fast double to int conversion.
    #Adding 2^52 + 2^51 and subtracting the same to remove fractional bits

    vaddsd       .L__big_number(%rip), %xmm1, %xmm2
    vsubsd       .L__big_number(%rip), %xmm2, %xmm1

    vmovd        %xmm2, %ecx

    # r1 = x - n * ln(2)/64 head
    # r2 = - n * ln(2)/64 tail
    # r  = r1+r2
     # r1 = x - n * ln(2)/64 head
    # r2 = - n * ln(2)/64 tail
    # r  = r1+r2
    vmovsd       %xmm1, %xmm3, %xmm3
    vfmadd132sd  .L__log2_by_64_mhead(%rip),%xmm0,%xmm1
    vfmadd132sd  .L__log2_by_64_mtail(%rip),%xmm1,%xmm3
    vmovsd       %xmm3, %xmm2, %xmm2

    #j = n & 0x3f
    mov         $0x3f, %rax
    and         %ecx, %eax     #eax = j
    # m = (n - j) / 64
    sar         $6, %ecx       #ecx = m

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # Horner's approach: q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))

    # Estrins method of polynomial computation
    # Let q = a0*x + x^2*a1 + x^3*a2 + x^4 *a3 + x^5*a4 + x^6*a5
    # A0 = a0 + a1*x
    # A1 = a2 + a3*x
    # A2 = a4 + a5*x

    # B0 = A0 + A1*x^2
    # B1 = A2 + A3*x^2

    # C0 = B0 + B1*x^2

   vmovsd    .L__real_1_by_720(%rip), %xmm6  #1/6!
   vmovsd    .L__real_1_by_120(%rip), %xmm5
   vmovsd    .L__real_1_by_6(%rip), %xmm4

   vfmadd213sd   .L__real_1_by_2(%rip), %xmm2, %xmm4 # 1/2 + 1/6*r
   vfmadd213sd   .L__real_1_by_24(%rip), %xmm2, %xmm5 # 1/24 + 1/120*r
   vmulsd     %xmm2, %xmm2, %xmm0 #xmm0 = r*r, xmm2=r
   vfmadd213sd  %xmm2, %xmm0, %xmm4 #xmm4 =  r + r^2*(1/2) + r^3*(1/6)
   vfmadd213sd  %xmm5, %xmm0, %xmm6 # 1/24 +  1/120*r + r^2*(1/720)
   vmulsd       %xmm0, %xmm0, %xmm7 # xmm7 = r^4
   vfmadd213sd  %xmm4, %xmm6, %xmm7 # r + r^2*(1/2) + r^3*(1/6) + r^4*(1/24) + r^5*(1/120) + r^6*(1/720)
   vmovsd       %xmm7,%xmm0,%xmm0


    # (f)*(q) + f2 + f1
    cmp         $0xfffffc02, %ecx # -1022
    lea         .L__two_to_jby64_table(%rip), %rdx
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    lea         .L__two_to_jby64_head_table(%rip), %r10
    vmulsd      (%rdx,%rax,8), %xmm0,%xmm2
    vaddsd      (%r11,%rax,8), %xmm2,%xmm1
    vaddsd      (%r10,%rax,8), %xmm1,%xmm0

    jle         .L__process_denormal

.L__process_normal:
    shl         $52, %rcx
    vmovq       %rcx, %xmm2
    vpaddq       %xmm2, %xmm0, %xmm0
    ret

.p2align 4
.L__process_denormal:
    jl          .L__process_true_denormal
    vucomisd    .L__real_one(%rip), %xmm0
    jae         .L__process_normal
.L__process_true_denormal:
    # here ( e^r < 1 and m = -1022 ) or m <= -1023
    add         $1074, %ecx
    mov         $1, %rax
    shl         %cl, %rax
    vmovq       %rax, %xmm2
    vmulsd      %xmm2,%xmm0,%xmm0
    ret

.L__compute_for_small_x:
    vmovsd         %xmm0, %xmm0, %xmm10
    vmovsd         .L__real_1_by_6(%rip), %xmm3
    vaddsd         .L__real_one(%rip), %xmm10, %xmm2  #A0 = a0 + x = 1 + x
    vfmadd213sd    .L__real_1_by_2(%rip), %xmm10, %xmm3 #A1 = a2 + x*a3 = 0.5 + (1/6)*x
    vmulsd         %xmm10, %xmm10, %xmm12    # xsquare
    vfmadd213sd   %xmm2, %xmm12, %xmm3 # 1 + x + x^2*(1/2) + x^3*(1/6)
    vmovsd        %xmm3,%xmm0,%xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_inf:
    vmovq        %xmm0,%rax
    mov         $0x7ff0000000000000,%rdx
    cmp         %rdx,%rax
    je          .L__x_is_pos_inf
    vmovq       %rdx, %xmm1
    mov         $3, %edi
    call        fname_special
.L__x_is_pos_inf:
    ret

.p2align 4
.L__y_is_nan:
    vaddsd       %xmm0,%xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret

.p2align 4
.L__y_is_zero:
    vucomisd    .L__min_exp_arg(%rip),%xmm0
    jbe         .L__return_zero
    vmovapd     .L__real_smallest_denormal(%rip), %xmm1
    jmp         .L__return_smallest_denormal

.p2align 4
.L__return_zero:
    vmovq       %xmm0,%rax
    mov         $0xfff0000000000000,%rdx
    cmp         %rdx,%rax
    je          .L__x_is_neg_inf
    vpxor       %xmm1,%xmm1, %xmm1    #return value in xmm1,input in xmm0 before calling
.L__return_smallest_denormal:
    mov         $2, %edi        #code in edi
    call        fname_special
    ret
.L__x_is_neg_inf:
    vpxor        %xmm0, %xmm0,%xmm0
    ret

.p2align 4
.L__y_is_one:
    vmovsd       .L__real_one(%rip),%xmm0
    ret

.else

.p2align 4
.L__y_is_inf:
    mov         $0x7ff0000000000000,%rax
    vmovq       %rax, %xmm0
    ret

.p2align 4
.L__y_is_nan:
    vaddsd      %xmm0,%xmm0,%xmm0
    ret

.p2align 4
.L__y_is_zero:
    vucomisd     .L__min_exp_arg(%rip),%xmm0
    jbe          .L__return_zero
    vmovapd      .L__real_smallest_denormal(%rip), %xmm0
    ret

.p2align 4
.L__return_zero:
    vpxor       %xmm2,%xmm2,%xmm0
    ret

.endif

.section .rodata
.align 16
.L__max_exp_arg:            .quad 0x40862e42fefa39ef
.L__denormal_tiny_threshold:.quad 0xc0874046dfefd9d0

	.if __enable_IEEE_exceptions
#	.L__signbit_off:            .quad 0x7fffffffffffffff
	.L__real_x_near0_threshold: .quad 0x3c00000000000000
	.endif
.L__signbit_off:            .quad 0x7fffffffffffffff
.L__big_number:             .quad 0x4338000000000000
.L__min_exp_arg:            .quad 0xc0874910d52d3051
.L__real_64_by_log2:        .quad 0x40571547652b82fe    # 64/ln(2)
.L__log2_by_64_mtail_mhead: .octa 0xbd1cf79abc9e3b39bf862e42fefa0000
.L__log2_by_64_mtail: 	    .octa 0xbd1cf79abc9e3b39
.L__log2_by_64_mhead:	    .quad 0xbf862e42fefa0000
.L__real_smallest_denormal: .quad 0x0000000000000001
.L__real_one:               .quad 0x3ff0000000000000
.L__min_exp:                .quad 0x3EB0C6F7A0B5ED8D #0.000001
.L__pos_zero:               .quad 0x0000000000000000
