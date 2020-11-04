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

# vrs4_log2f_bdozr.S
#
# An implementation of the log2f libm function.
#
# Prototype:
#
#     float log2f(float x);
#

#
#   Algorithm:
#       Similar to one presnted in log.S
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(vrs4_log2f)
#define fname_special _log2f_special@PLT


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

    vmovaps      .L__real_one(%rip),%xmm15
    vmovaps      .L__real_inf(%rip),%xmm13
    vmovaps      .L__real_nan(%rip),%xmm10
    # compute exponent part
    xor          %eax, %eax   #eax=0
    vpsrld       $23, %xmm0, %xmm3   #xmm3 = (ux>>23)
    vpsubd      .L__mask_127(%rip), %xmm3,%xmm3 # xmm3 = (ux>>23) - 127
    vandps       %xmm0,%xmm13,%xmm12
    #speacial case processing Nans Infs Negatives, and e's 
    # NaN or inf
    vpcmpeqd     %xmm12,%xmm13, %xmm6 # if((ux & 0x7f800000) == 0x7f800000) go to .L__x_is_inf_or_nan
#xmm6 stores the mask for Nans and Infs
    vaddps       %xmm0,%xmm0,%xmm11
    vpand        %xmm11,%xmm6,%xmm11 #xmm11 stores the result for Nan's and INfs
#check for negative numbers
    vxorps       %xmm1, %xmm1, %xmm1
    vpcmpgtd     %xmm0, %xmm1, %xmm7 # if( x <= 0.0 )
#xmm7 stores the negative mask for negative numbers
    vpor         %xmm6, %xmm7, %xmm6
    #vpcmov       %xmm7,%xmm11,%xmm10,%xmm7
    vandnpd %xmm11, %xmm7, %xmm2
    vandpd %xmm7, %xmm10, %xmm7
    vorpd %xmm2, %xmm7, %xmm7

    # xmm6 = Mask for Nan Infs and negatives
    # xmm7 = result of Nan Infs and negatives

    vpand        .L__real_mant(%rip),%xmm0,%xmm2  # xmm2 = ux & 0x007FFFFF
    vsubps       %xmm15,%xmm0,%xmm4   # xmm4 = x - 1.0
    vpcmpeqd     .L__mask_neg_127(%rip), %xmm3, %xmm8 # if (xexp == 127) 
#xmm8 stores the mask for denormal numbers 
#####################################
# commenting this part of the code will not process the denormal inputs                
#if 1
    vpor         %xmm2,%xmm15,%xmm11
    vsubps       %xmm15,%xmm11,%xmm10
    vpand        .L__real_mant(%rip),%xmm10,%xmm11
    #vpcmov       %xmm8,%xmm2,%xmm11,%xmm2
    vandnpd %xmm2, %xmm8, %xmm1
    vandpd  %xmm8, %xmm11, %xmm2
    vorpd %xmm1, %xmm2, %xmm2

    vpsrld       $23,%xmm10,%xmm10
    vpsubd       .L__mask_253(%rip),%xmm10,%xmm10 
    #vpcmov       %xmm8,%xmm3,%xmm10,%xmm3
    vandnpd %xmm3, %xmm8, %xmm1
    vandpd %xmm8, %xmm10, %xmm3
    vorpd %xmm1, %xmm3, %xmm3    

#endif    
#####################################
    vcvtdq2ps    %xmm3, %xmm5 # xmm5 = xexp
    # compute the index into the log tables
    vpand        .L__mask_mant_all7(%rip),%xmm2,%xmm11 #xmm11 = ux & 0x007f0000 
    vpand        .L__mask_mant8(%rip), %xmm2,%xmm8 # xmm8 = ux & 0x007f0000 
    vpslld       $1, %xmm8,%xmm8                   #xmm8  = (ux & 0x00008000) << 1
    vpaddd       %xmm8,%xmm11,%xmm1   #xmm1 = (ux & 0x007f0000) + ((ux & 0x00008000) << 1)

    # near one codepath
    vpand       .L__real_notsign(%rip), %xmm4, %xmm4   # xmm4 = fabs (x - 1.0)
    vcmpltps    .L__real_threshold(%rip), %xmm4, %xmm4 # if (xmm4 < real_threshold) go to near_one
#xmm4 stores the mask for nearone values
    # F, Y

    vpsrld      $16,%xmm1,%xmm9
    vpor        .L__real_half(%rip), %xmm2, %xmm2 # xmm2 = Y = ((ux & 0x007FFFFF) | 0x3f000000)
    vpor        .L__real_half(%rip), %xmm1, %xmm1 # xmm1 = F = ((ux & 0x007f0000) + ((ux & 0x00008000) << 1) | 0x3f000000)
    lea         .L__log_F_inv(%rip), %r9   #
    lea         .L__log2_128_lead_tail(%rip), %r10
    xor         %r8,%r8 # can be removed

#1st
    vmovd       %xmm9,%r8d
    vpsrldq     $4, %xmm9, %xmm9 
    vmovss      (%r9,%r8,4),%xmm10
    vmovsd      (%r10,%r8,8),%xmm11
    vpslldq     $4, %xmm10,%xmm10
#2nd    
    vmovd       %xmm9,%r8d
    vpsrldq     $4, %xmm9, %xmm9 
    vmovss      (%r9,%r8,4),%xmm13
    vmovss      %xmm13,%xmm10,%xmm10
    vmovsd      (%r10,%r8,8),%xmm14
    vpslldq     $8, %xmm14,%xmm14
    vmovsd      %xmm11,%xmm14,%xmm11
    vpslldq     $4, %xmm10,%xmm10
#3rd
    vmovd       %xmm9,%r8d
    vpsrldq     $4, %xmm9, %xmm9
    vmovss      (%r9,%r8,4),%xmm13
    vmovss      %xmm13,%xmm10,%xmm10
    vmovsd      (%r10,%r8,8),%xmm12
    vpslldq     $4, %xmm10,%xmm10
#4th
    vmovd       %xmm9,%r8d
    vmovss      (%r9,%r8,4),%xmm13
    vmovss      %xmm13,%xmm10,%xmm10
    vmovsd      (%r10,%r8,8),%xmm14
    vpslldq     $8, %xmm14,%xmm14
    vmovsd      %xmm12,%xmm14,%xmm12
    
    vpermilps   $0x1B,%xmm10,%xmm10 # 1B = 00011011
    # f = F - Y, r = f * inv
    vsubps      %xmm2, %xmm1,%xmm1     # f = F - Y
    vmulps      %xmm10,%xmm1,%xmm1     # r = f * log_F_inv[index]
    
    #poly
    vmovaps      .L__real_1_over_3(%rip),%xmm3
    vfmadd213ps     .L__real_1_over_2(%rip),%xmm1,%xmm3 # 1/3*r + 1/2
    vmulps       %xmm1, %xmm1, %xmm14                       # r*r
    #vmovaps      .L__real_log2_tail(%rip), %xmm3  #xmm3  = log2_tail
    #vmovaps      .L__real_log2_lead(%rip), %xmm10 #xmm10 = log2_lead
    vfmadd231ps     %xmm3,%xmm14, %xmm1  #poly = r + 1/2*r*r + 1/3*r*r*r
    vmulps        .L__real_log2_e(%rip),%xmm1,%xmm1 # poly = -log2_e*poly
    #vfmsubps     %xmm1,%xmm3,%xmm5, %xmm3 # z2 = (xexp * log2_tail) - poly
    # m*log(2) + log(G) - poly
    #vmulps       %xmm5, %xmm10, %xmm10         # z1 = xexp * log2_lead
#   xexp =  4321
#   poly =  4321
# xexp,poly = 44332211
# t,h   = thththth 
# add the two 
#   xmm1 = poly,  xmm5 = xexp, %xmm11=lead,tail xmm12=lead/tail
    
    vpxor        %xmm13,%xmm13,%xmm13
    vunpcklps    %xmm1,%xmm5,%xmm9  # xmm9 = 2211 in poly,xexp order
    vunpckhps    %xmm1,%xmm5,%xmm10   # xmm10  = 4433 in poly,xexp order
    vinsertf128  $1, %xmm10,%ymm9,%ymm9   # ymm9 = poly,xexp - 44332211 in poly,xexp order
    
    vinsertf128  $1, %xmm12,%ymm11,%ymm10 # thththth in z2,z1 order
    vaddps       %ymm9,%ymm10,%ymm11      # z2,z1 = (- poly + log_128_tail[index]), (xexp  + log_128_lead[index])
    vhaddps      %ymm13,%ymm11,%ymm13
    vextractf128 $1,%ymm13,%xmm14  #
    vmovlhps     %xmm14,%xmm13,%xmm9      #xmm9 stores the result
    #Now calculate for values near one
    # r = x - 1.0#
    vmovaps     .L__real_two(%rip), %xmm2        # xmm2 = 2.0
    vpand       .L__real_notsign(%rip),%xmm0,%xmm11
    #get the zero input masks
    vpxor       %xmm12,%xmm12,%xmm12
    vsubps      %xmm15, %xmm0, %xmm0  # xmm0 = r = = x - 1.0
    vpcmpeqd    %xmm12,%xmm11,%xmm12  # xmm12 stores the mask for zero
    
    # u = r / (2.0 + r)
    vaddps      %xmm0, %xmm2, %xmm2 # (r+2.0)
    vdivps      %xmm2, %xmm0,%xmm1 # u = r / (2.0 + r)

    # correction = r * u
    vmulps      %xmm1, %xmm0, %xmm10 # correction = u*r

    # u = u + u#
    vaddps      %xmm1, %xmm1,%xmm1 # u = u+u 
    vmulps      %xmm1, %xmm1,%xmm2 # v = u^2

    # r2 = (u * v * (ca_1 + v * ca_2) - correction)
    vmulps       %xmm2, %xmm1, %xmm3         # u^3
    vmovaps     .L__real_ca2(%rip),%xmm5    # xmm5 = ca_2 
    vfmadd213ps    .L__real_ca1(%rip),%xmm5, %xmm2 # ca1 + ca2 * v
    vfmsub213ps    %xmm10,%xmm3,%xmm2 # r2 = (ca1 + ca2 * v) * u^3 - correction 
    
    # r + r2
    vpand       .L__mask_lower(%rip),%xmm0,%xmm5
    vsubps      %xmm5,%xmm0,%xmm0     
    vaddps      %xmm0,%xmm2,%xmm2
    
    vmulps      .L__real_log2_e_lead(%rip), %xmm2,%xmm1
    vmulps      .L__real_log2_e_tail(%rip), %xmm5,%xmm0
    vmulps      .L__real_log2_e_lead(%rip), %xmm5, %xmm5

    vfmadd231ps    .L__real_log2_e_tail(%rip), %xmm2, %xmm0
    vaddps       %xmm1, %xmm0,%xmm0
    vaddps        %xmm5, %xmm0,%xmm0

# till here I have 
# xmm0 = .L__near_one results 
# xmm9 = away from 1.0 results
# Masks 
#xmm4 stores the mask for nearone values
#xmm12 stores the mask for 0.0
#xmm6 = Mask for Nan Infs e's and negatives
#xmm7 = result of Nan Infs e's and negatives

    vmovaps   .L__real_ninf(%rip),%xmm5
#vmovaps   .L__real_inf(%rip),%xmm14
    #vpcmov    %xmm4,%xmm9,%xmm0, %xmm0 # xmm0 stores nearone and away from one results
    vandnpd %xmm9, %xmm4, %xmm13
    vandpd %xmm4, %xmm0, %xmm0
    vorpd %xmm13, %xmm0, %xmm0
   
	
    #vpcmov    %xmm6,%xmm0,%xmm7,%xmm0
    vandnpd %xmm0, %xmm6, %xmm13
    vandpd %xmm6, %xmm7, %xmm0
    vorpd %xmm13, %xmm0, %xmm0

    #vpcmov    %xmm12,%xmm0,%xmm5,%xmm0
    vandnpd %xmm0, %xmm12, %xmm13
    vandpd %xmm12, %xmm5, %xmm0
    vorpd %xmm13, %xmm0, %xmm0	


    ret
    



.section .rodata

.align 16
.L__real_one:           .quad 0x3f8000003f800000   # 1.0
                        .quad 0x3f8000003f800000
.L__real_two:           .quad 0x4000000040000000   # 1.0
                        .quad 0x4000000040000000
.L__real_ninf:          .quad 0xff800000ff800000   # -inf
                        .quad 0xff800000ff800000
.L__real_inf:           .quad 0x7f8000007f800000   # +inf
                        .quad 0x7f8000007f800000
.L__real_nan:           .quad 0x7fc000007fc00000   # NaN
                        .quad 0x7fc000007fc00000
.L__real_ef:            .quad 0x402DF854402DF854   # float e
                        .quad 0x402DF854402DF854
.L__real_neg_qnan:      .quad 0xffc00000ffc00000
                        .quad 0xffc00000ffc00000

.L__real_sign:          .quad 0x8000000080000000  # sign bit
                        .quad 0x8000000080000000
.L__real_notsign:       .quad 0x7ffFFFFF7ffFFFFF  # ^sign bit
                        .quad 0x7ffFFFFF7ffFFFFF
.L__real_qnanbit:       .quad 0x0040000000400000   # quiet nan bit
                        .quad 0x0040000000400000
.L__real_mant:          .quad 0x007FFFFF007FFFFF   # mantissa bits
                        .quad 0x007FFFFF007FFFFF
.L__mask_127:           .quad 0x0000007f0000007f   # 
                        .quad 0x0000007f0000007f
.L__mask_neg_127:       .quad 0xFFFFFF81FFFFFF81   # 
                        .quad 0xFFFFFF81FFFFFF81

.L__mask_mant_all7:     .quad 0x007f0000007f0000
                        .quad 0x007f0000007f0000
.L__mask_mant8:         .quad 0x0000800000008000
                        .quad 0x0000800000008000

.L__real_ca1:           .quad 0x3DAAAAAB3DAAAAAB   # 8.33333333333317923934e-02
                        .quad 0x3DAAAAAB3DAAAAAB
.L__real_ca2:           .quad 0x3C4CCCCD3C4CCCCD   # 1.25000000037717509602e-02
                        .quad 0x3C4CCCCD3C4CCCCD

.L__real_log2_lead:     .quad 0x3F3170003F317000   # 0.693115234375
                        .quad 0x3F3170003F317000
.L__real_log2_tail:     .quad 0x3805FDF43805FDF4   # 0.000031946183
                        .quad 0x3805FDF43805FDF4
.L__real_half:          .quad 0x3f0000003f000000   # 1/2
                        .quad 0x3f0000003f000000
.L__real_log2_e:        .quad 0xbfb8aa3bbfb8aa3b
                        .quad 0xbfb8aa3bbfb8aa3b
.L__real_log2_e_lead:   .quad 0x3FB800003FB80000   # 1.4375000000
                        .quad 0x3FB800003FB80000
.L__real_log2_e_tail:   .quad 0x3BAA3B293BAA3B29   # 0.0051950408889633
                        .quad 0x3BAA3B293BAA3B29
.L__mask_lower:         .quad 0xffff0000ffff0000
                        .quad 0xffff0000ffff0000
.align 16
.L__mask_253:       .quad 0x000000fd000000fd
                    .quad 0x000000fd000000fd

.L__real_threshold: .quad 0x3d8000003d800000
                    .quad 0x3d8000003d800000

.L__real_1_over_3:  .quad 0x3eaaaaab3eaaaaab
                    .quad 0x3eaaaaab3eaaaaab

.L__real_1_over_2:  .quad 0x3f0000003f000000
                    .quad 0x3f0000003f000000



.align 16
#.L__log_128_lead:                                  .L__log_128_tail:
.L__log2_128_lead_tail:
                    .long 0x00000000  #lead
                    .long 0x00000000  #tail
                    .long 0x3c37c000  #lead
                    .long 0x374a16dd  #tail
                    .long 0x3cb70000
                    .long 0x37f2d0b8
                    .long 0x3d08c000
                    .long 0x381a3aa2
                    .long 0x3d35c000
                    .long 0x37b4dd63
                    .long 0x3d624000
                    .long 0x383f5721
                    .long 0x3d874000
                    .long 0x384e27e8
                    .long 0x3d9d4000
                    .long 0x380bf749
                    .long 0x3db30000
                    .long 0x387dbeb2
                    .long 0x3dc8c000
                    .long 0x37216e46
                    .long 0x3dde4000
                    .long 0x3684815b
                    .long 0x3df38000
                    .long 0x383b045f
                    .long 0x3e044000
                    .long 0x390b119b
                    .long 0x3e0ec000
                    .long 0x391a32ea
                    .long 0x3e194000
                    .long 0x38ba789e
                    .long 0x3e238000
                    .long 0x39553f30
                    .long 0x3e2e0000
                    .long 0x3651cfde
                    .long 0x3e380000
                    .long 0x39685a9d
                    .long 0x3e424000
                    .long 0x39057a05
                    .long 0x3e4c4000
                    .long 0x395ba0ef
                    .long 0x3e564000
                    .long 0x396bc5b6
                    .long 0x3e604000
                    .long 0x3936d9bb
                    .long 0x3e6a4000
                    .long 0x38772619
                    .long 0x3e740000
                    .long 0x39017ce9
                    .long 0x3e7dc000
                    .long 0x3902d720
                    .long 0x3e83c000
                    .long 0x38856dd8
                    .long 0x3e888000
                    .long 0x3941f6b4
                    .long 0x3e8d4000
                    .long 0x3980b652
                    .long 0x3e920000
                    .long 0x3980f561
                    .long 0x3e96c000
                    .long 0x39443f13
                    .long 0x3e9b8000
                    .long 0x38926752
                    .long 0x3ea00000
                    .long 0x39c8c763
                    .long 0x3ea4c000
                    .long 0x391e12f3
                    .long 0x3ea94000
                    .long 0x39b7bf89
                    .long 0x3eae0000
                    .long 0x36d1cfde
                    .long 0x3eb28000
                    .long 0x38c7f233
                    .long 0x3eb70000
                    .long 0x39087367
                    .long 0x3ebb8000
                    .long 0x38e95d3f
                    .long 0x3ec00000
                    .long 0x38256316
                    .long 0x3ec44000
                    .long 0x39d38e5c
                    .long 0x3ec8c000
                    .long 0x396ea247
                    .long 0x3ecd4000
                    .long 0x350e4788
                    .long 0x3ed18000
                    .long 0x395d829f
                    .long 0x3ed5c000
                    .long 0x39c30f2f
                    .long 0x3eda0000
                    .long 0x39fd7ee7
                    .long 0x3ede8000
                    .long 0x3872e9e7
                    .long 0x3ee2c000
                    .long 0x3897d694
                    .long 0x3ee70000
                    .long 0x3824923a
                    .long 0x3eeb0000
                    .long 0x39ea7c06
                    .long 0x3eef4000
                    .long 0x39a7fa88
                    .long 0x3ef38000
                    .long 0x391aa879
                    .long 0x3ef78000
                    .long 0x39dace65
                    .long 0x3efbc000
                    .long 0x39215a32
                    .long 0x3effc000
                    .long 0x39af3350
                    .long 0x3f01c000
                    .long 0x3a7b5172
                    .long 0x3f040000
                    .long 0x389cf27f
                    .long 0x3f060000
                    .long 0x3902806b
                    .long 0x3f080000
                    .long 0x3909d8a9
                    .long 0x3f0a0000
                    .long 0x38c9faa1
                    .long 0x3f0c0000
                    .long 0x37a33dca
                    .long 0x3f0dc000
                    .long 0x3a6623d2
                    .long 0x3f0fc000
                    .long 0x3a3c7a61
                    .long 0x3f11c000
                    .long 0x3a083a84
                    .long 0x3f13c000
                    .long 0x39930161
                    .long 0x3f15c000
                    .long 0x35d1cfde
                    .long 0x3f178000
                    .long 0x3a2d0ebd
                    .long 0x3f198000
                    .long 0x399f1aad
                    .long 0x3f1b4000
                    .long 0x3a67ff6d
                    .long 0x3f1d4000
                    .long 0x39ecfea8
                    .long 0x3f1f0000
                    .long 0x3a7b26f3
                    .long 0x3f210000
                    .long 0x39ec1fa6
                    .long 0x3f22c000
                    .long 0x3a675314
                    .long 0x3f24c000
                    .long 0x399e12f3
                    .long 0x3f268000
                    .long 0x3a2d4b66
                    .long 0x3f288000
                    .long 0x370c3845
                    .long 0x3f2a4000
                    .long 0x399ba329
                    .long 0x3f2c0000
                    .long 0x3a1044d3
                    .long 0x3f2dc000
                    .long 0x3a49a196
                    .long 0x3f2f8000
                    .long 0x3a79fe83
                    .long 0x3f318000
                    .long 0x3905c7aa
                    .long 0x3f334000
                    .long 0x39802391
                    .long 0x3f350000
                    .long 0x39abe796
                    .long 0x3f36c000
                    .long 0x39c65a9d
                    .long 0x3f388000
                    .long 0x39cfa6c5
                    .long 0x3f3a4000
                    .long 0x39c7f593
                    .long 0x3f3c0000
                    .long 0x39af6ff7
                    .long 0x3f3dc000
                    .long 0x39863e4d
                    .long 0x3f3f8000
                    .long 0x391910c1
                    .long 0x3f414000
                    .long 0x369d5be7
                    .long 0x3f42c000
                    .long 0x3a541616
                    .long 0x3f448000
                    .long 0x3a1ee960
                    .long 0x3f464000
                    .long 0x39c38ed2
                    .long 0x3f480000
                    .long 0x38e61600
                    .long 0x3f498000
                    .long 0x3a4fedb4
                    .long 0x3f4b4000
                    .long 0x39f6b4ab
                    .long 0x3f4d0000
                    .long 0x38f8d3b0
                    .long 0x3f4e8000
                    .long 0x3a3b3faa
                    .long 0x3f504000
                    .long 0x399fb693
                    .long 0x3f51c000
                    .long 0x3a5cfe71
                    .long 0x3f538000
                    .long 0x39c5740b
                    .long 0x3f550000
                    .long 0x3a611eb0
                    .long 0x3f56c000
                    .long 0x39b079c4
                    .long 0x3f584000
                    .long 0x3a4824d7
                    .long 0x3f5a0000
                    .long 0x39439a54
                    .long 0x3f5b8000
                    .long 0x3a1291ea
                    .long 0x3f5d0000
                    .long 0x3a6d3673
                    .long 0x3f5ec000
                    .long 0x3981c731
                    .long 0x3f604000
                    .long 0x3a0da88f
                    .long 0x3f61c000
                    .long 0x3a53945c
                    .long 0x3f638000
                    .long 0x3895ae91
                    .long 0x3f650000
                    .long 0x3996372a
                    .long 0x3f668000
                    .long 0x39f9a832
                    .long 0x3f680000
                    .long 0x3a27eda4
                    .long 0x3f698000
                    .long 0x3a4c764f
                    .long 0x3f6b0000
                    .long 0x3a6a7c06
                    .long 0x3f6cc000
                    .long 0x370321eb
                    .long 0x3f6e4000
                    .long 0x3899ab3f
                    .long 0x3f6fc000
                    .long 0x38f02086
                    .long 0x3f714000
                    .long 0x390a1707
                    .long 0x3f72c000
                    .long 0x39031e44
                    .long 0x3f744000
                    .long 0x38c6b362
                    .long 0x3f75c000
                    .long 0x382bf195
                    .long 0x3f770000
                    .long 0x3a768e36
                    .long 0x3f788000
                    .long 0x3a5c503b
                    .long 0x3f7a0000
                    .long 0x3a3c1179
                    .long 0x3f7b8000
                    .long 0x3a15de1d
                    .long 0x3f7d0000
                    .long 0x39d3845d
                    .long 0x3f7e8000
                    .long 0x395f263f
                    .long 0x3f800000
                    .long 0x00000000                    

.align 16
.L__log_F_inv:
                    .long 0x40000000
                    .long 0x3ffe03f8
                    .long 0x3ffc0fc1
                    .long 0x3ffa232d
                    .long 0x3ff83e10
                    .long 0x3ff6603e
                    .long 0x3ff4898d
                    .long 0x3ff2b9d6
                    .long 0x3ff0f0f1
                    .long 0x3fef2eb7
                    .long 0x3fed7304
                    .long 0x3febbdb3
                    .long 0x3fea0ea1
                    .long 0x3fe865ac
                    .long 0x3fe6c2b4
                    .long 0x3fe52598
                    .long 0x3fe38e39
                    .long 0x3fe1fc78
                    .long 0x3fe07038
                    .long 0x3fdee95c
                    .long 0x3fdd67c9
                    .long 0x3fdbeb62
                    .long 0x3fda740e
                    .long 0x3fd901b2
                    .long 0x3fd79436
                    .long 0x3fd62b81
                    .long 0x3fd4c77b
                    .long 0x3fd3680d
                    .long 0x3fd20d21
                    .long 0x3fd0b6a0
                    .long 0x3fcf6475
                    .long 0x3fce168a
                    .long 0x3fcccccd
                    .long 0x3fcb8728
                    .long 0x3fca4588
                    .long 0x3fc907da
                    .long 0x3fc7ce0c
                    .long 0x3fc6980c
                    .long 0x3fc565c8
                    .long 0x3fc43730
                    .long 0x3fc30c31
                    .long 0x3fc1e4bc
                    .long 0x3fc0c0c1
                    .long 0x3fbfa030
                    .long 0x3fbe82fa
                    .long 0x3fbd6910
                    .long 0x3fbc5264
                    .long 0x3fbb3ee7
                    .long 0x3fba2e8c
                    .long 0x3fb92144
                    .long 0x3fb81703
                    .long 0x3fb70fbb
                    .long 0x3fb60b61
                    .long 0x3fb509e7
                    .long 0x3fb40b41
                    .long 0x3fb30f63
                    .long 0x3fb21643
                    .long 0x3fb11fd4
                    .long 0x3fb02c0b
                    .long 0x3faf3ade
                    .long 0x3fae4c41
                    .long 0x3fad602b
                    .long 0x3fac7692
                    .long 0x3fab8f6a
                    .long 0x3faaaaab
                    .long 0x3fa9c84a
                    .long 0x3fa8e83f
                    .long 0x3fa80a81
                    .long 0x3fa72f05
                    .long 0x3fa655c4
                    .long 0x3fa57eb5
                    .long 0x3fa4a9cf
                    .long 0x3fa3d70a
                    .long 0x3fa3065e
                    .long 0x3fa237c3
                    .long 0x3fa16b31
                    .long 0x3fa0a0a1
                    .long 0x3f9fd80a
                    .long 0x3f9f1166
                    .long 0x3f9e4cad
                    .long 0x3f9d89d9
                    .long 0x3f9cc8e1
                    .long 0x3f9c09c1
                    .long 0x3f9b4c70
                    .long 0x3f9a90e8
                    .long 0x3f99d723
                    .long 0x3f991f1a
                    .long 0x3f9868c8
                    .long 0x3f97b426
                    .long 0x3f97012e
                    .long 0x3f964fda
                    .long 0x3f95a025
                    .long 0x3f94f209
                    .long 0x3f944581
                    .long 0x3f939a86
                    .long 0x3f92f114
                    .long 0x3f924925
                    .long 0x3f91a2b4
                    .long 0x3f90fdbc
                    .long 0x3f905a38
                    .long 0x3f8fb824
                    .long 0x3f8f177a
                    .long 0x3f8e7835
                    .long 0x3f8dda52
                    .long 0x3f8d3dcb
                    .long 0x3f8ca29c
                    .long 0x3f8c08c1
                    .long 0x3f8b7034
                    .long 0x3f8ad8f3
                    .long 0x3f8a42f8
                    .long 0x3f89ae41
                    .long 0x3f891ac7
                    .long 0x3f888889
                    .long 0x3f87f781
                    .long 0x3f8767ab
                    .long 0x3f86d905
                    .long 0x3f864b8a
                    .long 0x3f85bf37
                    .long 0x3f853408
                    .long 0x3f84a9fa
                    .long 0x3f842108
                    .long 0x3f839930
                    .long 0x3f83126f
                    .long 0x3f828cc0
                    .long 0x3f820821
                    .long 0x3f81848e
                    .long 0x3f810204
                    .long 0x3f808081
                    .long 0x3f800000


