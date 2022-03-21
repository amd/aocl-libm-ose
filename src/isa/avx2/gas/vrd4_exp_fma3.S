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

/*
  vrd4_exp_fma3
     Returns the value of e^x for 4-double precision numbers packed in an ymm0

  Special Cases:
     exp2(INF) => INF and exp2(-INF) => 0
     exp2(Nan) => Nan
     exp2(0)   => 1 (exact only for finite arg)

  Method:
   1. Argument reduction
       We use the identity,   /              \
       e^x = 2^(x/ln(2))   =  | (x*(64/ln(2))|
                              | ------------ |
                              \      64      /         --- (1)
                             2

       Choose 'n' and 'f' such that
       x*(64/ln(2)) = n + f        | n is an integer    --- (2)
                                   | |f| <= 0.5

       Choose 'm' such that
       n = 64 * m + j              | 0 <= j < 64         --- (3)

       From (1), (2) and (3),
       e^x = 2^((64*m + j + f)/64)
           = (2^m) * (2^(j/64)) * 2^(f/64)
           = (2^m) * (2^(j/64)) * e^(f*(ln(2)/64))

   2. 2^(j/64) is from look-up table

   3. Approximate 2^(f*(ln(2)/64))
      From (2),
      f = x*(64/ln(2)) - n

      Let r  = f*(ln(2)/64) = x - n*(ln(2)/64)

      Thus,
      e^x = (2^m) * (2^(j/64)) * e^r

      Values of (2^(j/64)) are precomputed

      e^r is calculated using the Taylor series expansion,
      e^r = 1 + r + (r^2)/2! + (r^3)/3! + (r^4)/4! + (r^5)/5! + (r^6)/6! + (r^7)/7!

   4. Scale back

      e^x = (2^m) * (2^(j/64)) * e^(f*(ln(2)/64))

   Range:  [-1/64, 1/64] i.e [-0.015625, 0.015625]

   Accuracy:
     Has around 0.9% of inputs, bits[10:8] not matching with GNU glibc's exp().
*/

#include "fn_macros.h"
#include "exp_tables.h"

#define fname ALM_PROTO_FMA3(vrd4_exp)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.align 32
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:

    vmovdqa      %ymm0, %ymm10   # save for later use

    # x * (64/ln(2))
    vmulpd       .L__real_64_by_log2(%rip), %ymm0, %ymm3

    # n = int( x * (64/ln(2)) )
    vcvttpd2dq  %ymm3, %xmm4   # ymm4 = 0,0,(int)n1,(int)n0
    vcvtdq2pd   %xmm4, %ymm3   # ymm3 = (double)n1,n0

    # r1 = x - n * ln(2)/64 head
    # r2 = - n * ln(2)/64 tail
    # r  = r1+r2
    vmovdqa     %ymm3, %ymm2
    vfmadd132pd .L__log2_by_64_mhead(%rip), %ymm0, %ymm2
    vfmadd231pd .L__log2_by_64_mtail(%rip), %ymm3, %ymm2  # ymm2 = r_3,r_2,r_1,r_0

    #j = n & 0x3f
    vandpd      .L__mask_64(%rip), %xmm4, %xmm8 # xmm8 = xmm4 & 0x3f
    vpmovsxdq   %xmm8, %ymm8                    # ymm8 = int64(j & 0x3f), needed for vgatherqpd

    # m = (n - j) / 64
    vpsrad      $6, %xmm4, %xmm5                # xmm5 = m3,m2,m1,m0

// HORNER
#ifdef HORNER
//    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
//    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
//    vmovapd       .L__real_1_by_720(%rip), %ymm3
//    vfmadd213pd   .L__real_1_by_120(%rip), %ymm2, %ymm3
//    vfmadd213pd   .L__real_1_by_24(%rip), %ymm2, %ymm3
//    vfmadd213pd   .L__real_1_by_6(%rip), %ymm2, %ymm3
//    vfmadd213pd   .L__real_1_by_2(%rip), %ymm2, %ymm3
//    vmulpd        %ymm2, %ymm2, %ymm0
//    vfmadd213pd   %ymm2, %ymm3, %ymm0
#else
 # ESTRINs approach
 # P3 = (c0 + c1 * r) + (c2 + c3 * r) * r2
 #  c0 = 0              c1 = 1/1 = 1
 #  c2 = 1/2            c3 = 1/6
 #
 # P7 = P3 + [(c4 + c5*r) + (c6 + c7*r) * r^2] * r^4
 #  c4 = 1/24           c6 = 1/720
 #  c5 = 1/120          c7 = 1/5040
 #
 # P8 = P7 + c8 * r^8

    vmovapd     .L__real_1_by_6(%rip),   %ymm0          # ymm0=1/6
    vfmadd213pd .L__real_1_by_2(%rip),   %ymm2, %ymm0   # ymm0= A0 = 1/2+(1/6)*r
    vmovapd     .L__real_1_by_120(%rip), %ymm6          # ymm6=1/120
    vfmadd213pd .L__real_1_by_24(%rip),  %ymm2, %ymm6   # ymm6= A1 = 1/24+(1/120)*r
    vmovapd     .L__real_1_by_5040(%rip),%ymm7          # ymm7=1/5040
    vfmadd213pd .L__real_1_by_720(%rip), %ymm2, %ymm7   # ymm7= A2 = 1/720+(1/5040)*r
    vmulpd      %ymm2, %ymm2, %ymm9                     # ymm9=r^2
    vfmadd213pd %ymm2, %ymm9, %ymm0                     # ymm0= B0 = r + A0 * r^2
    vfmadd213pd %ymm6, %ymm9, %ymm7                     # ymm7= B1 = A1 + A2 * r^2
    vmulpd      %ymm9, %ymm9, %ymm9                     # ymm9=r^4
    vfmadd231pd %ymm9, %ymm7, %ymm0                     # ymm0= B0 + B1 * r^4
#endif

    # (f)*(q) + f2 + f1
    lea         .L__two_to_jby64_table(%rip), %rdx
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    lea         .L__two_to_jby64_head_table(%rip), %r10
    vpcmpeqd     %ymm11, %ymm11, %ymm11
    vgatherqpd   %ymm11, (%rdx, %ymm8, 8), %ymm1
    vpcmpeqd     %ymm12, %ymm12, %ymm12            # build mask, all fff's
    vgatherqpd   %ymm12, (%r11, %ymm8, 8), %ymm3
    vpcmpeqd     %ymm13, %ymm13, %ymm13            # build mask, all fff's
    vgatherqpd   %ymm13, (%r10, %ymm8, 8), %ymm2
    vfmadd231pd  %ymm1, %ymm0, %ymm3               # ymm3 = f*q + f2
    vaddpd       %ymm3, %ymm2, %ymm0

    # normal results
    vpmovsxdq   %xmm5, %ymm4           # ymm4 = m3,m2 m1,m0
    vpsllq      $52, %ymm4, %ymm5      # ymm5 = 2^m3,2^m2,2^m1,2^m0
    vpaddq      %ymm5, %ymm0, %ymm1    # ymm1 = normal results

    #check and process denormals
    vcmppd      $1,.L__real_one(%rip), %ymm0, %ymm3  # check e^r < 1
    vpcmpeqq    .L__m1022(%rip), %ymm5, %ymm2        # check m == -1022
    vpand       %ymm2, %ymm3, %ymm3                  # ymm3 = ( e^r < 1 and m = -1022 )

    vmovdqa     .L__m1022(%rip), %ymm2
    vpcmpgtq    %ymm5, %ymm2, %ymm2        # check m < -1022

    vpor        %ymm3, %ymm2, %ymm2        # ymm2 = mask for inputs requiring denormal processing
    vpmovmskb   %ymm2, %eax
    cmp         $0,%eax
    je          .L__check_min          # jmp to avoid any accidental intermediate denormal results
    vpaddq      .L__ulong_1074(%rip), %ymm4, %ymm3 # ymm3 = 1074 + m

    vunpckhpd   %ymm3, %ymm3, %ymm9        #store high to xmm9
    vmovdqa     .L__ulong_1(%rip), %ymm5
    vpsllq      %xmm3, %ymm5, %ymm4
    vpsllq      %xmm9, %ymm5, %ymm9
    vunpcklpd   %ymm9, %ymm4, %ymm4
    vmulpd      %ymm4, %ymm0, %ymm3      # ymm3 = results for denormal numbers
    vandnpd     %ymm1, %ymm2, %ymm5
    vandpd      %ymm2, %ymm3, %ymm1
    vorpd       %ymm5, %ymm1, %ymm1


.L__check_min:
    ##special case for any x < min_exp_arg,remove this code if the above code takes care of this
    vcmppd      $2,.L__min_exp_arg(%rip), %ymm10, %ymm2 # cmp x<= min_exp_arg
    vpxor       %ymm3, %ymm3, %ymm4       		# make zeros to put in place of x<= min_exp_arg
    vandnpd     %ymm1, %ymm2, %ymm9
    vandpd      %ymm2, %ymm4, %ymm1
    vorpd       %ymm9, %ymm1, %ymm1

    vcmppd      $2,.L__denormal_tiny_threshold(%rip),%ymm10,%ymm3
    vpandn      %ymm3, %ymm2, %ymm4
    vmovdqa     .L__real_smallest_denormal(%rip), %ymm2

    vandnpd     %ymm1, %ymm4, %ymm9
    vandpd      %ymm4, %ymm2, %ymm1
    vorpd       %ymm9, %ymm1, %ymm1

    ##special case for any x > max_exp_arg,remove this code if the above code takes care of this
    vcmppd      $1,.L__max_exp_arg(%rip), %ymm10, %ymm2 #cmp x < max_exp_arg
    vmovdqa     .L__real_inf(%rip), %ymm3  		#inf to put in place of any x >= max_exp_arg
    vandnpd     %ymm3, %ymm2, %ymm9
    vandpd      %ymm2, %ymm1, %ymm1
    vorpd       %ymm9, %ymm1, %ymm1

    ##special case for any x = nan,remove this code if the above code takes care of this
    vcmppd      $0x0,%ymm10, %ymm10, %ymm2
    vaddpd      %ymm10, %ymm10, %ymm3  			#make qnan to put in place of any x =nan
    vandnpd     %ymm3, %ymm2, %ymm9
    vandpd      %ymm2, %ymm1, %ymm0
    vorpd       %ymm9, %ymm0, %ymm0

    ret

.section .rodata
.align 32
.L__max_exp_arg:              .octa 0x40862e42fefa39f040862e42fefa39f0  #40862e42fefa39ef
                              .octa 0x40862e42fefa39f040862e42fefa39f0
.L__denormal_tiny_threshold:  .octa 0xc0874046dfefd9d0c0874046dfefd9d0
                              .octa 0xc0874046dfefd9d0c0874046dfefd9d0
.L__min_exp_arg:              .octa 0xc0874910d52d3051c0874910d52d3051
                              .octa 0xc0874910d52d3051c0874910d52d3051
.L__real_64_by_log2:          .octa 0x40571547652b82fe40571547652b82fe    # 64/ln(2)
                              .octa 0x40571547652b82fe40571547652b82fe
.L__log2_by_64_mhead:         .octa 0xbf862e42fefa0000bf862e42fefa0000
                              .octa 0xbf862e42fefa0000bf862e42fefa0000
.L__log2_by_64_mtail:         .octa 0xbd1cf79abc9e3b39bd1cf79abc9e3b39
                              .octa 0xbd1cf79abc9e3b39bd1cf79abc9e3b39
.L__real_one:                 .octa 0x3ff00000000000003ff0000000000000
                              .octa 0x3ff00000000000003ff0000000000000
.L__real_smallest_denormal:   .octa 0x00000000000000010000000000000001
                              .octa 0x00000000000000010000000000000001
.L__real_inf:                 .octa 0x7ff00000000000007ff0000000000000
                              .octa 0x7ff00000000000007ff0000000000000
.L__m1022:                    .octa 0xc020000000000000c020000000000000
                              .octa 0xc020000000000000c020000000000000
.L__ulong_1074:               .octa 0x00000000000004320000000000000432
                              .octa 0x00000000000004320000000000000432
.L__real_1_by_5040:           .octa 0x3f2a01a01a01a01a3f2a01a01a01a01a    # 1/5040 (1/7!)
                              .octa 0x3f2a01a01a01a01a3f2a01a01a01a01a
.L__ulong_1:                  .octa 0x00000000000000010000000000000001
                              .octa 0x00000000000000010000000000000001
.L__mask_64:                  .octa 0x0000003f0000003f0000003f0000003f
			      .octa 0x0000003f0000003f0000003f0000003f

