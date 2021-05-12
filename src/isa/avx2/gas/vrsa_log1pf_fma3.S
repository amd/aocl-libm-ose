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

#
# vrs4_log1pf_bdozr.S
#
# An implementation of the vrs4_log1pf libm function.
#
# Prototype:
#
#     __m128 vrs4_log1pf(__m128 x);
#

#
#   Algorithm:
#
#   Based on:
#   Ping-Tak Peter Tang
#   "Table-driven implementation of the logarithm function in IEEE
#   floating-point arithmetic"
#   ACM Transactions on Mathematical Software (TOMS)
#   Volume 16, Issue 4 (December 1990)
#
#
#

#include "fn_macros.h"
#include "log_tables.h"
#define fname ALM_PROTO_FMA3(vrsa_log1pf)
#define fname_special _vrs4_log1pf_special@PLT


# local variable storage offsets
.equ    p_temp, 0x0
.equ    stack_size, 0x18


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
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

    # compute exponent part
    vmovaps      .L__real_qnan(%rip),%xmm3
    vmovaps      .L__real_ninf_32(%rip),%xmm5
    vmovaps      .L__negative_one(%rip),%xmm6
    vpand        .L__sign_mask_32(%rip),%xmm0,%xmm1

    vcmpleps     .L__real_epsilon(%rip),%xmm1,%xmm1 #mask for epsilons
    vcmpltps     %xmm6,%xmm0,%xmm2 #mask for input less than -1.0
    #get the mask for exactly -1.0
    vcmpeqps     %xmm6,%xmm0,%xmm15
 
    #process inputs less than 0x3380000033800000
    vpand     %xmm1,%xmm0,%xmm4 
    #now restore if there are some inputs less than -1.0
    vpor      %xmm1,%xmm2,%xmm6 # store the mask for final use
    #vpcmov    %xmm2,%xmm4,%xmm3,%xmm1
    vandnpd %xmm4, %xmm2, %xmm11
    vandpd %xmm2, %xmm3, %xmm1
    vorpd %xmm11, %xmm1, %xmm1
    

    # now restore if there are some inputs with -1.0
    vpor      %xmm15,%xmm6,%xmm2 # store the mask for final use
    #vpcmov    %xmm15,%xmm1,%xmm5,%xmm1
    vandnpd %xmm1, %xmm15, %xmm11
    vandpd %xmm15, %xmm5, %xmm1
    vorpd %xmm11, %xmm1, %xmm1

#xmm2 = mask for -1.0, <-1.0, and less than epsilon
#xmm1 = result for -1.0, <-1.0, and less than epsilon
############################################
    #calculate log1pf for lower half
    vbroadcastsd   .L__one_mask_64(%rip),%ymm3
    vbroadcastsd   .L__exp_mask_64(%rip),%ymm4
    vbroadcastsd   .L__mantissa_mask(%rip),%ymm11
    vbroadcastsd   .L__index_mask1(%rip),%ymm6
    vbroadcastsd   .L__index_mask2(%rip),%ymm7
    vbroadcastsd   .L__index_mask3(%rip),%ymm8
    vmovaps        .L__selector_constant(%rip),%xmm13

    vcvtps2pd      %xmm0,%ymm5
    vmovapd        %ymm5,%ymm14
    vaddpd         %ymm3,%ymm5,%ymm5 #ymm5 = 1.0 +x

    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    vandpd         %ymm6,%ymm5,%ymm6
    vandpd         %ymm7,%ymm5,%ymm7
    vorpd          %ymm8,%ymm6,%ymm6
    vextractf128   $1,%ymm7,%xmm8
    #vpermil2ps     $0,%xmm13,%xmm8,%xmm7,%xmm7
    vshufps $221,%xmm8, %xmm7, %xmm7

    vextractf128   $1,%ymm6,%xmm8
    #vpermil2ps     $0,%xmm13,%xmm8,%xmm6,%xmm6
    vshufps $221, %xmm8, %xmm6, %xmm6

    vpsrld         $13,%xmm7,%xmm7
    vpsrld         $14,%xmm6,%xmm6
    vpaddd         %xmm6,%xmm7,%xmm6
    vcvtdq2pd      %xmm6,%ymm7
    vbroadcastsd   .L__index_constant1(%rip),%ymm8 # 
    
    vmovaps        .L__mask_1023_32bit(%rip),%xmm12
    vandpd         %ymm4,%ymm5,%ymm4
    vextractf128   $1,%ymm4,%xmm15
    #vpermil2ps     $0,%xmm13,%xmm15,%xmm4,%xmm4
    vshufps $221, %xmm15, %xmm4, %xmm4	

    vpsrld         $20,%xmm4,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64;
    vpsubd         %xmm12,%xmm4,%xmm4
    vandps         %ymm11,%ymm5,%ymm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | ONEEXPBITS_DP64, f);
    vorps          %ymm3,%ymm5,%ymm5 # f = ymm5
    vmulpd         %ymm8,%ymm7,%ymm7 # f1 =index*0.015625
    vpsubd         .L__index_constant2(%rip),%xmm6,%xmm6 # index -= 64
    
# xmm4=xexp
    vmovaps        .L__plus_sixtyone(%rip),%xmm9
    vmovaps        .L__minus_two(%rip),%xmm10
    vmovaps        .L__mask_127_32bit(%rip),%xmm11
    #     IF( (xexp <= -2 ) || (xexp >= MANTLENGTH_DP64 + 8))
    vpcmpgtd       %xmm4,%xmm9,%xmm9   # (xmm9>xmm4) (61 > xexp)
    vpcmpgtd       %xmm4,%xmm10,%xmm10 #(xmm10>xmm4) (-2 > xexp)
    vpandn         %xmm9,%xmm10,%xmm9 # xmm9 stores the mask for all the numbers which lie between -2 and 61
    vmovaps        %xmm9,%xmm10
    vpsrldq        $8,%xmm10,%xmm10
    vinsertf128    $1,%xmm10,%ymm9,%ymm9
    vpermilps      $0x50 ,%ymm9,%ymm9 # 0x50 = 01 01 00 00
    vsubpd         %ymm7,%ymm5,%ymm5 # f2 = f - f1;
    #ELSE
    vpsubd         %xmm4,%xmm11,%xmm11
    
    #ux = (unsigned long long)(0x3ff - xexp) << EXPSHIFTBITS_DP64;
    #PUT_BITS_DP64(ux,m2);
    vpslld         $23,%xmm11,%xmm11 # xmm11 = m2;
    vcvtps2pd      %xmm11,%ymm11
    vmulpd         %ymm14,%ymm11,%ymm13 # xmm13 = m2*x
    #ymm7=f1, ymm11=m2, ymm13 = m2*x
    vsubpd         %ymm7,%ymm11,%ymm11 # (m2 - f1)
    vaddpd         %ymm13,%ymm11,%ymm11 # xmm11 = f2 = (m2 - f1) + m2*dx
    # z1 = ln_lead_table[index];
    # q = ln_tail_table[index];
    lea            .L__ln_lead_64_table(%rip), %r9
    lea            .L__ln_tail_64_table(%rip), %r8
    
    vmovd          %xmm6,%r10d # move 1st order index
    vpsrldq        $4,%xmm6,%xmm6
    vmovlpd        (%r9,%r10,8),%xmm14,%xmm14
    vmovlpd        (%r8,%r10,8),%xmm12,%xmm12

    vmovd          %xmm6,%r11d # move 2nd order index
    vpsrldq        $4,%xmm6,%xmm6
    vmovhpd        (%r9,%r11,8),%xmm14,%xmm14
    vmovhpd        (%r8,%r11,8),%xmm12,%xmm12

    vmovd          %xmm6,%r10d # move 3rd order index
    vpsrldq        $4,%xmm6,%xmm6
    vmovlpd        (%r9,%r10,8),%xmm15,%xmm15
    vmovlpd        (%r8,%r10,8),%xmm13,%xmm13

    vmovd          %xmm6,%r11d # move 4th order index
    vmovhpd        (%r9,%r11,8),%xmm15,%xmm15
    vmovhpd        (%r8,%r11,8),%xmm13,%xmm13

    vinsertf128    $1,%xmm15,%ymm14,%ymm6 # ymm6 = z1 = ln_lead_table[index]
    vinsertf128    $1,%xmm13,%ymm12,%ymm8 # ymm8 = q = ln_tail_table[index]
    
    vbroadcastsd   .L__real_half(%rip),%ymm12
    vbroadcastsd   .L__real_cb_1(%rip),%ymm13
    vbroadcastsd   .L__real_cb_2(%rip),%ymm15
    #f2 = ymm11/ymm5 f1 = xmm7 
    #vpcmov         %ymm9,%ymm5,%ymm11,%ymm11
    vandnpd %ymm5, %ymm9, %ymm14
    vandpd %ymm9, %ymm11, %ymm11
    vorpd %ymm14, %ymm11, %ymm11


    vmovapd        %ymm11,%ymm5
    
    vfmadd231pd       %ymm12,%ymm11,%ymm7#(f1 + 0.5 * f2);
    vdivpd         %ymm7,%ymm5,%ymm7  # u = f2 / (f1 + 0.5 * f2);
    vmulpd         %ymm7,%ymm7,%ymm5  # v = u*u

    vfmadd231pd       %ymm15,%ymm5,%ymm13
    vmulpd         %ymm5,%ymm13,%ymm5  # poly = v * (cb_1 + v * cb_2)
    vbroadcastsd   .L__real_log2(%rip),%ymm13
    #poly = xmm5 u = xmm7 q = xmm8
    vfmadd132pd       %ymm7,%ymm7,%ymm5 # (u + u * poly)
    vaddpd         %ymm8,%ymm5,%ymm5   # z2 = q + (u + u * poly)
    #xmm5 = z2, xmm8 = q, xmm7 = u, xmm6 = z1 
    vcvtdq2pd      %xmm4,%ymm4 # xexp (float)
    
    vfmadd213pd       %ymm6,%ymm13,%ymm4
    vaddpd         %ymm5,%ymm4,%ymm4 # r = (xexp * log2 + z1 + z2)
    
    vcvtpd2ps      %ymm4,%xmm4
#################################
    # xmm0 is the result of log1p  
#xmm2 = mask for -1.0, <-1.0, and less than epsilon
#xmm1 = result for -1.0, <-1.0, and less than epsilon
    #vpcmov         %xmm2,%xmm4,%xmm1,%xmm2
    vandnpd %xmm4, %xmm2, %xmm14
    vandpd %xmm2, %xmm1, %xmm2
    vorpd %xmm14, %xmm2, %xmm2

    #get the mask for Nans and Infs
    vpand          .L__exp_mask(%rip),%xmm0,%xmm14  # remove the mantissa
    vpcmpeqd       .L__exponent_compare(%rip),%xmm14,%xmm14 # xmm14 stores the mask for nans and Infs

    #now restore the Nans and Infs
    vpand          .L__sign_bit_32(%rip),%xmm0,%xmm3 # for negative infities we need to set the result as Qnan
    vaddps         %xmm0,%xmm0,%xmm0
    #so we get the sign bit and move it to the qnan Bit.
    #then OR it with Qnan/inf result
    vpsrld         $9,%xmm3,%xmm3
    
    vpor           %xmm3,%xmm0,%xmm0
    #vpcmov         %xmm14,%xmm2,%xmm0,%xmm0
    vandnpd %xmm2, %xmm14, %xmm3
    vandpd %xmm14, %xmm0, %xmm0
    vorpd %xmm3, %xmm0, %xmm0
    
##########################################
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
.L__real_qnan:      .quad 0x7FC000007FC00000   # qNaN
                    .quad 0x7FC000007FC00000
.L__real_ninf_32:   .quad 0xff800000ff800000   # -inf
                    .quad 0xff800000ff800000
.L__negative_one:   .quad 0xbf800000bf800000
                    .quad 0xbf800000bf800000
.L__sign_mask_32:   .quad 0x7FFFFFFF7FFFFFFF
                    .quad 0x7FFFFFFF7FFFFFFF
.L__real_epsilon:   .quad 0x3380000033800000
                    .quad 0x3380000033800000
.L__one_mask_64:    .quad 0x3fF0000000000000# 1
                    .quad 0x3fF0000000000000
.L__exp_mask_64:    .quad 0x7FF0000000000000#
                    .quad 0x7FF0000000000000
.L__mantissa_mask:  .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                    .quad 0x000FFFFFFFFFFFFF
.L__index_mask1:    .quad 0x000fc00000000000
                    .quad 0x000fc00000000000
.L__index_mask2:    .quad 0x0000200000000000
                    .quad 0x0000200000000000
.L__index_mask3:    .quad 0x0010000000000000
                    .quad 0x0010000000000000
.L__selector_constant: .quad 0x0000000300000001
                       .quad 0x0000000700000005
.L__index_constant1:.quad 0x3F90000000000000
                    .quad 0x3F90000000000000
.L__mask_1023_32bit: .quad 0x000003ff000003ff
                     .quad 0x000003ff000003ff
.L__index_constant2:.quad 0x0000004000000040
                    .quad 0x0000004000000040
.L__plus_sixtyone:  .quad 0x0000003D0000003D
                    .quad 0x0000003D0000003D
.L__minus_two:      .quad 0xFFFFFFFEFFFFFFFE
                    .quad 0xFFFFFFFEFFFFFFFE
.L__mask_127_32bit:  .quad 0x0000007f0000007f
                     .quad 0x0000007f0000007f
.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x3fe0000000000000
# Approximating polynomial coefficients for other x 
.L__real_cb_1:      .quad 0x3fb5555555555557 # 8.33333333333333593622e-02, 
                    .quad 0x3fb5555555555557
.L__real_cb_2:      .quad 0x3f89999999865ede # 1.24999999978138668903e-02,  
                    .quad 0x3f89999999865ede
.L__real_log2:      .quad 0x3fe62e42fefa39ef # log2 = 6.931471805599453e-01
                    .quad 0x3fe62e42fefa39ef 
.L__exp_mask:       .quad 0x7F8000007F800000   #
                    .quad 0x7F8000007F800000
.L__exponent_compare: .quad 0x7F8000007F800000  
                      .quad 0x7F8000007F800000
.L__sign_bit_32:    .quad 0x8000000080000000 
                    .quad 0x8000000080000000



