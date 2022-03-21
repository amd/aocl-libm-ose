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

#define fname ALM_PROTO_FMA3(vrsa_exp10f)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    cmp         $0x0, %edi
    jle         .L__return
    cmp         $0x0, %rsi
    je         .L__return
    cmp         $0x0, %rdx
    je         .L__return

.p2align 4
.L__process_next4:
    sub         $0x4, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first123
    movdqu      (%rsi, %rdi, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first123:
    je          .L__process_first3
    cmp         $-3, %rdi
    jl          .L__return #multiple of 4elements, all are processed %rdi == -4
    je          .L__process_first1
    #process first 2
    mov         $0x0, %rcx
    movsd       (%rsi, %rcx, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first1:
    mov         $0x0, %rcx
    movss       (%rsi, %rcx, 4), %xmm0
    jmp         .L__start

.p2align 4
.L__process_first3:
    mov         $0x1, %rcx
    movsd       (%rsi, %rcx, 4), %xmm0
    dec         %rcx  # zero
    movss       (%rsi, %rcx, 4), %xmm1
    pslldq      $4,%xmm0
    por         %xmm1, %xmm0

.p2align 4
.L__start:
    vmovdqa       %xmm0, %xmm10   # save for later use
    vminps        .L__max_exp10f_arg(%rip),%xmm0,%xmm0
    vcvtps2pd     %xmm0, %ymm2    #ymm2 = (double)x3,x2,x1,x0

    # x * (64/log10of(2))
    vmulpd       .L__real_64_by_log10of2(%rip), %ymm2, %ymm3 #ymm3 = x3 * (64/ln(2),x2 * (64/ln(2),x1 * (64/ln(2),x0 * (64/ln(2)

    # n = int( x * (64/log10of(2)) )
    vcvtpd2dq    %ymm3, %xmm4  #xmm4 = (int)n3,n2,n1,n0
    vcvtdq2pd    %xmm4, %ymm0  #ymm0 = (double)n3,n2,n1,n0

    # r = x - n * ln(2)/64
    # r *= ln(10)
    #vfmaddpd    %ymm2,.L__real_mlog10of2_by_64(%rip),%ymm0,%ymm1 #ymm1 = r3,r2,r1,r0
     vfmadd132pd    .L__real_mlog10of2_by_64(%rip),%ymm2,%ymm0 #ymm0 = r3,r2,r1,r0
    vmulpd      .L__real_ln10(%rip),%ymm0,%ymm1 #ymm1 = r = r*ln(2)

    # q = r + r*r(1/2 + r*1/6)
    vmovdqa     .L__real_4_1_by_6s(%rip), %ymm3
    #vfmaddpd    .L__real_4_1_by_2s(%rip),%ymm3,%ymm1,%ymm2 #ymm2 = (1/2 + (1/6 * r))
    #vmulpd       %ymm1,%ymm1,%ymm3   #ymm3 = r3*r3,r2*r2,r1*r1,r0*r0
    #vfmaddpd     %ymm1,%ymm3,%ymm2,%ymm0 #ymm0 = q3,q2,q1,q0

   # #j = n & 0x3f
    #vpsrad      $6,%xmm4,%xmm1    #xmm1 = m3,m2,m1,m0
   # vpslld      $26,%xmm4,%xmm3
    #vpsrld      $26,%xmm3,%xmm4   #xmm4 = j3,j2,j1,j0

   # # f + (f*q)
   # lea         .L__two_to_jby64_table(%rip), %r10
   # vmovd        %xmm4,%eax      #eax = j0
   # vpsrldq      $4,%xmm4,%xmm3  #xmm3 = XX,j3,j2,j1
   # vmovd        %xmm3,%ecx      #ecx = j1
   # vmovsd      (%r10,%rax,8), %xmm2
   # vmovhpd     (%r10,%rcx,8), %xmm2,%xmm2  #xmm2 = f1,f0
   # vpsrldq      $4,%xmm3,%xmm4  #xmm4 = XX,XX,j3,j2
   ## vpsrldq      $4,%xmm4,%xmm3  #xmm3 = XX,XX,XX,j3
    #vmovd        %xmm3,%ecx      #ecx = j3
   # vmovsd      (%r10,%rax,8), %xmm3
   # vmovhpd     (%r10,%rcx,8), %xmm3,%xmm3  #xmm3 = f3,f2
   # vinsertf128  $1,%xmm3,%ymm2,%ymm2  #ymm2 = f3,f2,f1,f0
   # vfmaddpd     %ymm2,%ymm0,%ymm2,%ymm3   #ymm3 = f + (f*q)

    vmovdqa     %ymm3,%ymm2
    vfmadd213pd    .L__real_4_1_by_2s(%rip),%ymm1,%ymm3                 #ymm3 = (1/2 + (1/6 * r))

    vmulpd       %ymm1,%ymm1,%ymm2   #ymm2 = r3*r3,r2*r2,r1*r1,r0*r0

    vfmadd213pd     %ymm1,%ymm2,%ymm3         #ymm3 = q3,q2,q1,q0


    #j = n & 0x3f
    vpsrad      $6,%xmm4,%xmm1    #xmm1 = m3,m2,m1,m0
    vpslld      $26,%xmm4,%xmm0
    vpsrld      $26,%xmm0,%xmm4   #xmm4 = j3,j2,j1,j0

    # f + (f*q)
    lea         .L__two_to_jby64_table(%rip), %r10
    vmovd        %xmm4,%eax      #eax = j0
    vpsrldq      $4,%xmm4,%xmm0  #xmm0 = XX,j3,j2,j1
    vmovd        %xmm0,%ecx      #ecx = j1
    vmovsd      (%r10,%rax,8), %xmm2
    vmovhpd     (%r10,%rcx,8), %xmm2,%xmm2  #xmm2 = f1,f0
    vpsrldq      $4,%xmm0,%xmm4  #xmm4 = XX,XX,j3,j2
    vmovd        %xmm4,%eax      #eax = j2
    vpsrldq      $4,%xmm4,%xmm0  #xmm0 = XX,XX,XX,j3
    vmovd        %xmm0,%ecx      #ecx = j3
    vmovsd      (%r10,%rax,8), %xmm0
    vmovhpd     (%r10,%rcx,8), %xmm0,%xmm0  #xmm0 = f3,f2
    vinsertf128  $1,%xmm0,%ymm2,%ymm2  #ymm2 = f3,f2,f1,f0
    vfmadd213pd     %ymm2,%ymm2,%ymm3    #ymm3 = f + (f*q)







    # m = (n - j) / 64
    vpmovsxdq   %xmm1,%xmm2      #xmm2 = m1,m0
    vpsllq      $52,%xmm2,%xmm0 #xmm0 = 2^m1,2^m0
    vpsrldq     $8,%xmm1,%xmm2   #xmm2 = XX,XX,m3,m2
    vpmovsxdq   %xmm2,%xmm1      #xmm1 = m3,m2
    vpsllq      $52,%xmm1,%xmm2  #xmm2 = 2^m3,2^m2

    #2 ^m * (f + (f*q))
    vextractf128   $1,%ymm3,%xmm1
    vpaddq      %xmm3,%xmm0,%xmm0
    vpaddq      %xmm1,%xmm2,%xmm1
    vinsertf128    $1,%xmm1,%ymm0,%ymm2
    vcvtpd2ps   %ymm2, %xmm0

    ##special case for any x < min_exp_arg
    ##remove this code if the above code takes care of this
    vmovdqa      .L__min_exp10f_arg(%rip), %xmm1
    vcmpps       $1,%xmm10,%xmm1,%xmm2
    vpand        %xmm2, %xmm0,%xmm1   #put zeros in place of any x < min_exp2_arg

    ##special case for any x = nan
    ##remove this code if the above code takes care of this
    vcmpps      $0x0,%xmm10,%xmm10,%xmm2
    vaddps      %xmm10, %xmm10,%xmm3  #make qnan to put in place of any x =nan
    #vpcmov      %xmm2, %xmm3,%xmm1,%xmm0
    VANDPD %xmm2, %xmm1, %xmm0
    VANDNPD %xmm3, %xmm2, %xmm4
    VORPD %xmm4, %xmm0, %xmm0


    cmp         $-1,  %rdi
    jle          .L__store123
    movdqu       %xmm0, (%rdx, %rdi, 4)
    jmp         .L__process_next4

.p2align 4
.L__store123:
    je         .L__store3
    cmp         $-3, %rdi
    je          .L__store1
    #store 2
    add        $0x2, %rdi
    movsd      %xmm0, (%rdx,%rdi,4)
    jmp         .L__return

.p2align 4
.L__store3:
    movdqa      %xmm0, %xmm1
    psrldq      $4, %xmm0
    inc         %rdi
    movss       %xmm1, (%rdx,%rdi,4)
    inc         %rdi
    movsd      %xmm0, (%rdx,%rdi,4)
    jmp         .L__return

.p2align 4
.L__store1:
    mov        $0x0, %rdi
    movss      %xmm0, (%rdx,%rdi,4)

.L__return:
    ret


.section .rodata
.align 16
.L__max_exp10f_arg:             .octa 0x421A209B421A209B421A209B421A209B
.L__min_exp10f_arg:             .octa 0xC23369F4C23369F4C23369F4C23369F4

.align 32
.L__real_64_by_log10of2:        .octa 0x406A934F0979A371406A934F0979A371 # 64/log10(2)
                                .octa 0x406A934F0979A371406A934F0979A371 # 64/log10(2)
.L__real_mlog10of2_by_64:       .octa 0xbF734413509F79FFbF734413509F79FF # log10of2_by_64
                                .octa 0xbF734413509F79FFbF734413509F79FF # log10of2_by_64
.L__real_ln10:                  .octa 0x40026BB1BBB5551640026BB1BBB55516 # ln(10)
                                .octa 0x40026BB1BBB5551640026BB1BBB55516 # ln(10)
.L__real_4_1_by_2s:             .octa 0x3fe00000000000003fe0000000000000
                                .octa 0x3fe00000000000003fe0000000000000
.L__real_4_1_by_6s:             .octa 0x3fc55555555555553fc5555555555555    # 1/6
                                .octa 0x3fc55555555555553fc5555555555555



