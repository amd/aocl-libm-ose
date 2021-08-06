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

/*
  vrd4_exp2_fma3.S
     Returns the value of 2^x for 4 double-precision numbers packed in an ymm register

  Special Cases:
     exp2(INF) => INF and exp2(-INF) => 0
     exp2(Nan) => Nan
     exp2(0)   => 1 (exact only for finite arg)

  Method:
   1. Argument reduction
        Choose 'n' and 'f' such that
        x*64 = n + f                                  --- (1)
        (n is integer part, f is the fraction part)
        &
        n = m*64 + j          | j=0,1,2...63          --- (2)

        2^x = 2^(x*64/64) = 2^((n+f)/64)

        From (1) and (2)
            = 2^m . 2^(j/64) . 2^(f/64)

   2. 2^(j/64) from look-up table

   3. Approximate 2^(f/64) using polynomial
        2^x = e^(ln2 * x)

        f = x*64 - n

        2^(f/64) = e(ln2 * ((x*64 - n)/64))
        Let r = (x*64 - n)/64
              = x - n/64

        Calculate e^r using Taylor series (upto 7th degree)
        e^r = 1 + r + (r^2)/2! + (r^3)/3! + (r^4)/4! + (r^5)/5! + (r^6)/6! + (r^7)/7!

   4. Scale back
        2^x = (2^m) * (2^(j/64)) * e^r

  Range:  [-1/64, 1/64] i.e [-0.015625, 0.015625]

  Accuracy:
          Has 0.9% (9 out of 1000) of inputs, has a descrepency in BIT[10:8]
	  against GNU libc's exp2

*/

#include "fn_macros.h"
#include "exp_tables.h"

#define fname ALM_PROTO_FMA3(vrd4_exp2)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif


.text
.p2align 4
.globl fname
.type fname,@function
fname:
    vmovdqa      %ymm0, %ymm10   # save for later use

    # m = x * (64)
    vmulpd       .L__real_64(%rip), %ymm0, %ymm3

    # n = int(m)
    vcvttpd2dq    %ymm3, %xmm4   # xmm4 = (int)n3,(int)n4,(int)n1,(int)n0
    vcvtdq2pd     %xmm4, %ymm3   # ymm3 = (double)n3,n2,n1,n0
    #vmovdqa      %xmm4, %xmm7   # xmm7 has backup

    # r = x - n * 1/64
    # r *= ln2;
    vfmadd132pd   .L__mone_by_64(%rip), %ymm0, %ymm3
    vmulpd        .L__ln_2(%rip), %ymm3, %ymm2  # ymm2 = r_3,r_2,r_1,r_0

    # j = n & 0x3f
    vandpd         .L__mask_64(%rip), %xmm4, %xmm8 # xmm8 = xmm4 & 0x3f
    vpmovsxdq      %xmm8, %ymm8                    # ymm8 = int64(j & 0x3f), needed for vgatherqpd

    # m = (n - j) / 64
    vpsrad         $6, %xmm4, %xmm5      # xmm5 = m3,m2,m1,m0

#define USE_ESTRIN_COMPUTE
#ifdef USE_ESTRINS_COMPUTE
 # ESTRINs approach
 # P3 = (c0 + c1 * r) + (c2 + c3 * r) * r2
 #  c0 = 0		c1 = 1/1 = 1
 #  c2 = 1/2		c3 = 1/6
 #
 # P7 = P3 + [(c4 + c5*r) + (c6 + c7*r) * r^2] * r^4
 #  c4 = 1/24		c6 = 1/720
 #  c5 = 1/120		c7 = 1/5040
 #
 # P8 = P7 + c8 * r^8

    vmovapd 	.L__real_1_by_6(%rip),   %ymm0		# ymm0=1/6
    vfmadd213pd .L__real_1_by_2(%rip),   %ymm2, %ymm0   # ymm0= A0 = 1/2+(1/6)*r
    vmovapd	.L__real_1_by_120(%rip), %ymm6          # ymm6=1/120
    vfmadd213pd	.L__real_1_by_24(%rip),  %ymm2, %ymm6   # ymm6= A1 = 1/24+(1/120)*r
    vmovapd     .L__real_1_by_5040(%rip),%ymm7          # ymm7=1/5040
    vfmadd213pd .L__real_1_by_720(%rip), %ymm2, %ymm7   # ymm7= A2 = 1/720+(1/5040)*r
    vmulpd 	%ymm2, %ymm2, %ymm9                     # ymm9=r^2
    vfmadd213pd %ymm2, %ymm9, %ymm0                     # ymm0= B0 = r + A0 * r^2
    vfmadd213pd %ymm6, %ymm9, %ymm7                     # ymm7= B1 = A1 + A2 * r^2
    vmulpd 	%ymm9, %ymm9, %ymm9			# ymm9=r^4
    vfmadd231pd %ymm9, %ymm7, %ymm0		        # ymm0= B0 + B1 * r^4
#else
 # using Hornor's method
     # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
     # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
     vmovapd       .L__real_1_by_720(%rip), %ymm3
     vfmadd213pd   .L__real_1_by_120(%rip), %ymm2, %ymm3

#define USE_ALL_FMADD
#ifdef USE_ALL_FMADD
     vfmadd213pd   .L__real_1_by_24(%rip), %ymm2, %ymm3
     vfmadd213pd   .L__real_1_by_6(%rip), %ymm2, %ymm3
     vfmadd213pd   .L__real_1_by_2(%rip), %ymm2, %ymm3
     vmulpd        %ymm2, %ymm2, %ymm0
     vfmadd213pd   %ymm2, %ymm3, %ymm0
#else
     vmulpd        %ymm3, %ymm2, %ymm3
     vaddpd        .L__real_1_by_24(%rip), %ymm3, %ymm3
     vmulpd        %ymm3, %ymm2, %ymm3
     vaddpd        .L__real_1_by_6(%rip), %ymm3, %ymm3
     vmulpd        %ymm3, %ymm2, %ymm3
     vaddpd        .L__real_1_by_2(%rip), %ymm3, %ymm3
     vmulpd        %ymm2, %ymm2, %ymm0
     vmulpd        %ymm0, %ymm3, %ymm3
     vaddpd        %ymm2, %ymm3, %ymm0
#endif /* USE_FMADD*/

#endif /* USE_ESTRINS_COMPUTE */


    # (f)*(q) + f2 + f1
    lea         .L__two_to_jby64_table(%rip), %rdx
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    lea         .L__two_to_jby64_head_table(%rip), %r10
    vpcmpeqd     %ymm11, %ymm11, %ymm11            # build mask, all fff's
    vgatherqpd   %ymm11, (%rdx, %ymm8, 8), %ymm1
    vpcmpeqd     %ymm11, %ymm11, %ymm11            # build mask, all fff's
    vgatherqpd   %ymm11, (%r11, %ymm8, 8), %ymm3
    vpcmpeqd     %ymm11, %ymm11, %ymm11            # build mask, all fff's
    vgatherqpd   %ymm11, (%r10, %ymm8, 8), %ymm2
    vfmadd213pd  %ymm3, %ymm1, %ymm0               # ymm0 = f*q + f2
    vaddpd       %ymm0, %ymm2, %ymm0

    # normal results
    vpmovsxdq   %xmm5, %ymm4               # ymm4 = m3, m2, m1,m0
    vpsllq      $52, %ymm4, %ymm5          # ymm5 = 2^m3,2^m2,2^m1,2^m0
    vpaddq      %ymm5, %ymm0, %ymm1        # ymm1 = normal results


    #check and process denormals
    vcmppd      $1,.L__real_one(%rip), %ymm0, %ymm3	# check e^r < 1
    vpcmpeqq    .L__m1022(%rip),%ymm5,%ymm2		# check m == -1022
    vpand       %ymm2, %ymm3, %ymm3  			# ymm3 = ( e^r < 1 and m = -1022 )

    vmovapd     .L__m1022(%rip), %ymm2
    vpcmpgtq    %ymm5, %ymm2, %ymm2  			# check m < -1022

    # ymm2 = mask for inputs requiring denormal processing
    vpor        %ymm3, %ymm2, %ymm2
    vpmovmskb   %ymm2, %eax
    cmp         $0,%eax
    #jmp to avoid any accidental intermediate denormal results
    je          .L__check_min
    vpaddq      .L__ulong_1074(%rip), %ymm4, %ymm3 	# ymm3 = 1074 + m

    vunpckhpd   %ymm3, %ymm3, %ymm9 #store high to ymm9
    vmovdqa     .L__ulong_1(%rip),%ymm5
    vpsllq      %xmm3, %ymm5, %ymm4
    vpsllq      %xmm9, %ymm5, %ymm9
    vunpcklpd   %ymm9, %ymm4, %ymm4
    vmulpd      %ymm4, %ymm0, %ymm3 #ymm3 = results for denormal numbers
    vandnpd     %ymm1, %ymm2, %ymm5
    vandpd      %ymm2, %ymm3, %ymm1
    vorpd       %ymm5, %ymm1, %ymm1

.L__check_min:
    ##special case for any x < min_exp_arg,remove this code if the above code takes care of this
    vcmppd      $2,.L__min_exp2_arg(%rip),%ymm10,%ymm2 #cmp x<= min_exp_arg
    vpxor       %ymm3,%ymm3,%ymm4  #make zeros to put in place of x<= min_exp_arg
    vandnpd     %ymm1, %ymm2, %ymm9
    vandpd      %ymm2, %ymm4, %ymm1
    vorpd       %ymm9, %ymm1, %ymm1

    ##special case for any x > max_exp_arg,remove this code if the above code takes care of this
    vcmppd       $1,.L__max_exp2_arg(%rip),%ymm10,%ymm2 #cmp x < max_exp_arg
    vmovdqa     .L__real_inf(%rip), %ymm3  #inf to put in place of any x >= max_exp_arg
    vandnpd     %ymm3, %ymm2, %ymm9
    vandpd      %ymm2, %ymm1, %ymm1
    vorpd       %ymm9, %ymm1, %ymm1

    ##special case for any x = nan,remove this code if the above code takes care of this
    vcmppd       $0x0, %ymm10, %ymm10, %ymm2
    vaddpd       %ymm10, %ymm10,%ymm3  #make qnan to put in place of any x =nan
    vandnpd      %ymm3, %ymm2, %ymm9
    vandpd       %ymm2, %ymm1, %ymm0
    vorpd        %ymm9, %ymm0, %ymm0

    ret

.section .rodata
.align 32
.L__max_exp2_arg:	.octa 0x40900000000000004090000000000000
			.octa 0x40900000000000004090000000000000
.L__min_exp2_arg:	.octa 0xc090c80000000000c090c80000000000
			.octa 0xc090c80000000000c090c80000000000
.L__real_64:		.octa 0x40500000000000004050000000000000    # 64
			.octa 0x40500000000000004050000000000000    # 64
.L__ln_2:		.octa 0x3FE62E42FEFA39EF3FE62E42FEFA39EF
			.octa 0x3FE62E42FEFA39EF3FE62E42FEFA39EF
.L__mone_by_64:		.octa 0xbF90000000000000bF90000000000000
			.octa 0xbF90000000000000bF90000000000000
.L__real_one:		.octa 0x3ff00000000000003ff0000000000000    # 1.0
			.octa 0x3ff00000000000003ff0000000000000
.L__real_inf:		.octa 0x7ff00000000000007ff0000000000000    # INF
			.octa 0x7ff00000000000007ff0000000000000
.L__m1022:		.octa 0xc020000000000000c020000000000000
			.octa 0xc020000000000000c020000000000000
.L__ulong_1074:		.octa 0x00000000000004320000000000000432
			.octa 0x00000000000004320000000000000432
.L__ulong_1:		.octa 0x00000000000000010000000000000001
			.octa 0x00000000000000010000000000000001
.L__real_1_by_5040:	.octa 0x3f2a01a01a01a01a3f2a01a01a01a01a    # 1/5040 (1/7!)
			.octa 0x3f2a01a01a01a01a3f2a01a01a01a01a
.L__real_1_by_40320:	.octa 0x3efa01a01a01a01a3efa01a01a01a01a    # 1/40320 (1/8!)
			.octa 0x3efa01a01a01a01a3efa01a01a01a01a
.L__mask_64:		.octa 0x0000003f0000003f0000003f0000003f
                        .octa 0x0000003f0000003f0000003f0000003f

