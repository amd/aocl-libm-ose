#
# Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

#define fname ALM_PROTO_FMA3(expm1)
#define fname_special alm_exp_special@PLT
#ifdef __ELF__
    .section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    vucomisd .L__max_expm1_arg(%rip),%xmm0  #check if(x > 709.8)
    ja .L__y_is_inf
    jp .L__y_is_nan
    vucomisd .L__min_expm1_arg(%rip),%xmm0  #if(x < -37.42994775023704)
    jb .L__y_is_minusOne
    vucomisd .L__log_OneMinus_OneByFour(%rip),%xmm0
    jbe .L__Normal_Flow
    vucomisd .L__log_OnePlus_OneByFour(%rip),%xmm0
    jb .L__Small_Arg

    .p2align 4
.L__Normal_Flow:
    # x * (64/ln(2))
    vmulsd       .L__real_64_by_log2(%rip),%xmm0,%xmm1

    # n = int( x * (64/ln(2)) )
    vcvttpd2dq  %xmm1, %xmm2   #xmm2 = (int)n
    vcvtdq2pd   %xmm2, %xmm1   #xmm1 = (double)n
    vmovd       %xmm2, %ecx

    # r1 = x - n * ln(2)/64 head
    # r2 = - n * ln(2)/64 tail
    # r1+r2
    vmovlhps    %xmm1,%xmm1,%xmm1     #xmm1 = (double)n,(double)n
    vmovq       %xmm0,%xmm0           #xmm0 = 0,x   #zero out the upper part
    vfmadd231pd    .L__log2_by_64_mtail_mhead(%rip),%xmm1,%xmm0
    vhaddpd     %xmm0,%xmm0,%xmm2     #xmm2 = r,r

    #j = n & 0x3f
    mov         $0x3f, %rax
    and         %ecx, %eax     #eax = j
    # m = (n - j) / 64
    sar         $6, %ecx       #ecx = m

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    vmovapd    .L__real_1_by_720(%rip),%xmm3
    vfmadd213sd   .L__real_1_by_120(%rip),%xmm2,%xmm3
    vfmadd213sd   .L__real_1_by_24(%rip),%xmm2,%xmm3
    vfmadd213sd   .L__real_1_by_6(%rip),%xmm2,%xmm3
    vfmadd213sd   .L__real_1_by_2(%rip),%xmm2,%xmm3
    vmulsd     %xmm2,%xmm2,%xmm0
    vfmadd213sd   %xmm2,%xmm3,%xmm0

    # load f, f1,f2
    lea         .L__two_to_jby64_table(%rip), %rdx
    lea         .L__two_to_jby64_head_table(%rip), %r10
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    vmovsd      (%rdx,%rax,8), %xmm1   #xmm1 = f
    vmovsd      (%r10,%rax,8), %xmm2   #xmm2 = f1
    vmovsd      (%r11,%rax,8), %xmm3   #xmm3 = f2

    #twopmm.u64 = (1023 - (long)m) << 52;
    mov $1023,%eax
    sub %ecx,%eax
    shl $52, %rax #rax = twopmm

    cmp $52,%ecx        #check m > 52
    jg .L__M_Above_52
    cmp $-7,%ecx        #check if m < -7
    jl .L__M_Below_Minus7
    #(-8 < m) && (m < 53)
    shl   $52,%rcx             #rcx = 2^m,m at exponent
    vmovd  %rcx, %xmm4
    vaddsd .L__One(%rip),%xmm0,%xmm1 #xmm1 = 1+q
    vmulsd %xmm3,%xmm1,%xmm1   #f2*(1+q)
    vmovd  %rax, %xmm3          #xmm3 = twopmm
    vfmadd213sd %xmm1,%xmm2,%xmm0 #f1*q + f2*(1+q))
    vsubsd %xmm3,%xmm2,%xmm2          #xmm2 = f1 - twopmm
    vaddsd %xmm2,%xmm0,%xmm0
    vpaddq %xmm4,%xmm0,%xmm0
    ret

    .p2align 4
.L__M_Above_52:
    cmp $1024,%ecx #check if m = 1024
    je .L__M_Equals_1024
    #twopm.f64 * (f1.f64 + (f*q+(f2.f64 - twopmm.f64)));// 2^-m should not be calculated if m>105
    vmovq %rax,%xmm4            #xmm4 = twopmm
    vsubsd %xmm4,%xmm3,%xmm3    #xmm3 = f2 - twopmm
    vfmadd213pd %xmm3,%xmm1,%xmm0 #xmm0 = (f*q+(f2 - twopmm)))
    vaddpd %xmm2,%xmm0,%xmm0  #xmm0 = (f1 + (f*q+(f2 - twopmm)))
    shl $52,%rcx
    vmovq %rcx,%xmm1    #xmm1 = twopm
    vpaddq %xmm1,%xmm0,%xmm0
    ret

    .p2align 4
.L__M_Below_Minus7:
    #twopm.f64 * (f1.f64 + (f*q + f2.f64)) - 1;
    vfmadd213pd %xmm3,%xmm1,%xmm0 #xmm0 = (f*q + f2.f64)
    vaddpd %xmm2,%xmm0,%xmm0   #xmm0 = f1 + (f*q + f2.f64)
    shl $52,%rcx
    vmovq %rcx,%xmm1           #xmm1 = twopm
    vpaddq %xmm1,%xmm0,%xmm0   #xmm0 = twopm *(xmm0)
    vsubpd .L__zero_One(%rip),%xmm0,%xmm0
    ret

    .p2align 4
.L__M_Equals_1024:
    mov $0x4000000000000000,%rax #1024 at exponent
    vfmadd213pd %xmm3,%xmm1,%xmm0  #xmm0 = (f*q) + f2
    vaddpd %xmm2,%xmm0,%xmm0 #xmm0 = f1 + (f*q) + f2
    vmovq %rax,%xmm1   #xmm1 = twopm
    vpaddq %xmm1,%xmm0,%xmm0
    vmovq %xmm0,%rax
    mov $0x7FF0000000000000,%rcx
    and %rcx,%rax
    cmp %rcx,%rax #check if we reached inf
    je .L__y_is_inf
    ret

    .p2align 4
.L__Small_Arg:
.if __enable_IEEE_exceptions
    vpand         .L__signbit_off(%rip),%xmm0,%xmm1
    vucomisd      .L__zero(%rip),%xmm1
    je           .L__ret
    vucomisd      .L__real_x_near0_threshold(%rip),%xmm1
    jb           .L__y_is_x
.else
    vucomisd .L__minus_zero(%rip),%xmm0
    je .L__ret
.endif

    mov $0x01E0000000000000,%rax #30 in exponents place
    #u = (twop30.f64 * x + x) - twop30.f64 * x;
    vmovq %rax,%xmm1
    vpaddq %xmm0,%xmm1,%xmm1  #xmm1 = twop30.f64 * x
    vaddsd %xmm0,%xmm1,%xmm2  #xmm2 = (twop30.f64 * x + x)
    vsubsd %xmm1,%xmm2,%xmm2  #xmm2 = u
    vsubsd %xmm2,%xmm0,%xmm1 #xmm1 = v = x-u
    vmulsd %xmm2,%xmm2,%xmm3 #xmm3 = u*u
    vmulsd .L__real_1_by_2(%rip),%xmm3,%xmm3 #xmm3 = y = u*u*0.5

    #q = x*x*x*(A1.f64 + x*(A2.f64 + x*(A3.f64 + x*(A4.f64 + x*(A5.f64 + x*(A6.f64 + x*(A7.f64 + x*(A8.f64 + x*(A9.f64)))))))));
    vmulsd .L__B9(%rip),%xmm0,%xmm5
    vaddsd .L__B8(%rip),%xmm5,%xmm5
    vfmadd213sd .L__B7(%rip),%xmm0,%xmm5
    vfmadd213sd .L__B6(%rip),%xmm0,%xmm5
    vfmadd213sd .L__B5(%rip),%xmm0,%xmm5
    vfmadd213sd .L__B4(%rip),%xmm0,%xmm5
    vfmadd213sd .L__B3(%rip),%xmm0,%xmm5
    vfmadd213sd .L__B2(%rip),%xmm0,%xmm5
    vmovdqa %xmm5, %xmm4
    vfmadd213sd .L__B1(%rip),%xmm0,%xmm5
    vmulsd %xmm0,%xmm5,%xmm4
    vmulsd %xmm0,%xmm4,%xmm5
    vmulsd %xmm0,%xmm5,%xmm5   #xmm5 = q

    #z = v * (x + u) * 0.5;
    vaddsd %xmm0,%xmm2,%xmm4
    vmulsd %xmm1,%xmm4,%xmm4
    vmulsd .L__real_1_by_2(%rip),%xmm4,%xmm4 #xmm4 = z

    vucomisd .L__TwopM7(%rip),%xmm3
    jb .L__returnNext
    vaddsd %xmm4,%xmm1,%xmm1  #xmm1 = v+z
    vaddsd %xmm5,%xmm1,%xmm1  #xmm1 = q+(v+z)
    vaddsd %xmm3,%xmm2,%xmm2  #xmm2 = u+y
    vaddsd %xmm2,%xmm1,%xmm1
    vmovapd %xmm1,%xmm0
    ret
.p2align 4
.L__returnNext:
    vaddsd %xmm5,%xmm4,%xmm4  #xmm4 = q +z
    vaddsd %xmm4,%xmm3,%xmm3  #xmm3 = y+(q+z)
    vaddsd %xmm3,%xmm0,%xmm0
.p2align 4
.L__ret:
    ret

.p2align 4
.L__y_is_inf:
    mov         $0x7ff0000000000000,%rax
    vmovq       %rax, %xmm0
    ret

.if __enable_IEEE_exceptions

.p2align 4
.L__y_is_nan:
    vaddsd      %xmm0,%xmm0,%xmm1
    mov         $1, %edi
    call        fname_special
    ret

.p2align 4
.L__y_is_x:
    vmovdqa      %xmm0,%xmm1
    mov         $2,%edi
    call 	fname_special
    ret

.else

.p2align 4
.L__y_is_nan:
    vaddsd      %xmm0,%xmm0,%xmm0
    ret

.endif

.p2align 4
.L__y_is_minusOne:
    mov $0xBFF0000000000000,%rax   #return -1
    vmovq %rax, %xmm0
    ret

.section .rodata
.align 16
.L__max_expm1_arg:           .quad 0x40862E6666666666
.L__min_expm1_arg:           .quad 0xC042B708872320E1
.L__log_OneMinus_OneByFour:  .quad 0xBFD269621134DB93
.L__log_OnePlus_OneByFour:   .quad 0x3FCC8FF7C79A9A22
.L__real_64_by_log2:         .quad 0x40571547652b82fe    # 64/ln(2)
.L__log2_by_64_mtail_mhead:  .octa 0xbd1cf79abc9e3b39bf862e42fefa0000
.L__TwopM7:                  .quad 0x3F80000000000000
.L__zero_One:                .octa 0x00000000000000003FF0000000000000
     .if __enable_IEEE_exceptions
     .L__real_x_near0_threshold: .quad 0x3c00000000000000
     .L__signbit_off:            .quad 0x7fffffffffffffff
     .L__zero:                   .quad 0x0000000000000000
     .else
     .L__minus_zero:              .quad 0x8000000000000000
     .endif

.L__B9:                      .quad 0x3E5A2836AA646B96
.L__B8:                      .quad 0x3E928295484734EA
.L__B7:                      .quad 0x3EC71E14BFE3DB59
.L__B6:                      .quad 0x3EFA019F635825C4
.L__B5:                      .quad 0x3F2A01A01159DD2D
.L__B4:                      .quad 0x3F56C16C16CE14C6
.L__B3:                      .quad 0x3F8111111111A9F3
.L__B2:                      .quad 0x3FA55555555554B6
.L__B1:                      .quad 0x3FC5555555555549
.L__One:                     .quad 0x3FF0000000000000


