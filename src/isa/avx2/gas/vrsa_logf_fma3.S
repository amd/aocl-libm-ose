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

# vrs4_logf_bdozr.S
#
# An implementation of the logf libm function.
#
# Prototype:
#
#     float logf(float x);
#

#
#   Algorithm:
#       Similar to one presented in log.S
#

#include "fn_macros.h"
#define fname ALM_PROTO_FMA3(vrsa_logf)
#define fname_special alm_logf_special@PLT


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
    vmovaps      .L__real_one(%rip),%xmm15
    vmovaps      .L__real_inf(%rip),%xmm13
    vmovaps      .L__real_nan(%rip),%xmm10
    # compute exponent part
    xor          %eax, %eax   #eax=0
    vpsrld       $23, %xmm0, %xmm3   #xmm3 = (ux>>23)
    vpsubd      .L__mask_127(%rip), %xmm3,%xmm3 # xmm3 = (ux>>23) - 127
    vandps       %xmm0,%xmm13,%xmm12
    #special case processing Nans Infs Negatives, and e's 
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
    #vpcmov       %xmm7,%xmm11,%xmm10,%xmm11
    VANDNPD %xmm11, %xmm7, %xmm8
    VANDPD %xmm7, %xmm10, %xmm11
    VORPD %xmm8, %xmm11, %xmm11

    vpcmpeqd     .L__real_ef(%rip), %xmm0, %xmm8    #if (x == e)  return 1.0
    vpor         %xmm6, %xmm8, %xmm6
#xmm8 stores the mask for e
    #vpcmov       %xmm8,%xmm11,%xmm15,%xmm7
    VANDNPD %xmm11, %xmm8, %xmm2
    VANDPD %xmm8, %xmm15, %xmm7
    VORPD %xmm2, %xmm7, %xmm7


    # xmm6 = Mask for Nan Infs e's and negatives
    # xmm7 = result of Nan Infs e's and negatives

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
    VANDNPD %xmm2, %xmm8, %xmm5
    VANDPD %xmm8, %xmm11, %xmm2
    VORPD %xmm5, %xmm2, %xmm2

    vpsrld       $23,%xmm10,%xmm10
    vpsubd       .L__mask_253(%rip),%xmm10,%xmm10 
    #vpcmov       %xmm8,%xmm3,%xmm10,%xmm3
    VANDNPD %xmm3, %xmm8, %xmm5
    VANDPD %xmm8, %xmm10, %xmm3
    VORPD %xmm5, %xmm3, %xmm3

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
    lea         .L__log_128_lead_tail(%rip), %r10
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
    vmovdqa %xmm3, %xmm2
    vmulps       %xmm1, %xmm1, %xmm14                       # r*r
    vmovaps      .L__real_log2_tail(%rip), %xmm3  #xmm3  = log2_tail
    vmovaps      .L__real_log2_lead(%rip), %xmm10 #xmm10 = log2_lead
    vfmadd231ps     %xmm2,%xmm14, %xmm1  #poly = r + 1/2*r*r + 1/3*r*r*r
    vfmsub213ps     %xmm1,%xmm5, %xmm3 # z2 = (xexp * log2_tail) - poly
    # m*log(2) + log(G) - poly
    vmulps       %xmm5, %xmm10, %xmm10         # z1 = xexp * log2_lead
#   z1=  4321
#   z2=  4321
# z2,z1 = 44332211 
# t,h   = thththth 
# add the two 
# z2,z1 
#   xmm3 = z2, xmm10 = z1, %xmm11=lead,tail xmm12=lead/tail
    vpxor        %xmm13,%xmm13,%xmm13
    vunpcklps    %xmm3,%xmm10,%xmm9  # xmm9 = 2211 in z2,z1 order
    vunpckhps    %xmm3,%xmm10,%xmm10   # xmm10  = 4433 in z2,z1 order
    vinsertf128  $1, %xmm10,%ymm9,%ymm9   # ymm9 = z2,z1 - 44332211 in z2,z1 order
    vinsertf128  $1, %xmm12,%ymm11,%ymm10 # thththth in z2,z1 order
    vaddps       %ymm9,%ymm10,%ymm11      # z2,z1 = (xexp * log2_tail) - poly + log_128_tail[index], (xexp * log2_lead) + log_128_lead[index]
    vhaddps      %ymm13,%ymm11,%ymm13
    vextractf128 $1,%ymm13,%xmm14  #
    vmovlhps     %xmm14,%xmm13,%xmm9      #xmm9 stores the result

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
    vaddps       %xmm2, %xmm0,%xmm0 # return (r + r2)
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
    #vpcmov    %xmm6,%xmm0,%xmm7,%xmm0
    #vpcmov    %xmm12,%xmm0,%xmm5,%xmm0

    VANDNPD %xmm9, %xmm4, %xmm14
    VANDPD %xmm4, %xmm0, %xmm0
    VORPD %xmm14, %xmm0, %xmm0
 
    VANDNPD %xmm0, %xmm6, %xmm14
    VANDPD %xmm6, %xmm7, %xmm0
    VORPD %xmm14, %xmm0, %xmm0

    VANDNPD %xmm0, %xmm12, %xmm14
    VANDPD %xmm12, %xmm5, %xmm0
    VORPD %xmm14, %xmm0, %xmm0


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
.L__log_128_lead_tail:
                    .long 0x00000000      #lead      
                    .long 0x00000000      #tail
                    .long 0x3bff0000      #lead
                    .long 0x3429ac41      #tail
                    .long 0x3c7e0000      #.
                    .long 0x35a8b0fc      #and so on
                    .long 0x3cbdc000
                    .long 0x368d83ea
                    .long 0x3cfc1000
                    .long 0x361b0e78
                    .long 0x3d1cf000
                    .long 0x3687b9fe
                    .long 0x3d3ba000
                    .long 0x3631ec65
                    .long 0x3d5a1000
                    .long 0x36dd7119
                    .long 0x3d785000
                    .long 0x35c30045
                    .long 0x3d8b2000
                    .long 0x379b7751
                    .long 0x3d9a0000
                    .long 0x37ebcb0d
                    .long 0x3da8d000
                    .long 0x37839f83
                    .long 0x3db78000
                    .long 0x37528ae5
                    .long 0x3dc61000
                    .long 0x37a2eb18
                    .long 0x3dd49000
                    .long 0x36da7495
                    .long 0x3de2f000
                    .long 0x36a91eb7
                    .long 0x3df13000
                    .long 0x3783b715
                    .long 0x3dff6000
                    .long 0x371131db
                    .long 0x3e06b000
                    .long 0x383f3e68
                    .long 0x3e0db000
                    .long 0x38156a97
                    .long 0x3e14a000
                    .long 0x38297c0f
                    .long 0x3e1b8000
                    .long 0x387e100f
                    .long 0x3e226000
                    .long 0x3815b665
                    .long 0x3e293000
                    .long 0x37e5e3a1
                    .long 0x3e2ff000
                    .long 0x38183853
                    .long 0x3e36b000
                    .long 0x35fe719d
                    .long 0x3e3d5000
                    .long 0x38448108
                    .long 0x3e43f000
                    .long 0x38503290
                    .long 0x3e4a9000
                    .long 0x373539e8
                    .long 0x3e511000
                    .long 0x385e0ff1
                    .long 0x3e579000
                    .long 0x3864a740
                    .long 0x3e5e1000
                    .long 0x3786742d
                    .long 0x3e647000
                    .long 0x387be3cd
                    .long 0x3e6ae000
                    .long 0x3685ad3e
                    .long 0x3e713000
                    .long 0x3803b715
                    .long 0x3e778000
                    .long 0x37adcbdc
                    .long 0x3e7dc000
                    .long 0x380c36af
                    .long 0x3e820000
                    .long 0x371652d3
                    .long 0x3e851000
                    .long 0x38927139
                    .long 0x3e882000
                    .long 0x38c5fcd7
                    .long 0x3e8b3000
                    .long 0x38ae55d5
                    .long 0x3e8e4000
                    .long 0x3818c169
                    .long 0x3e914000
                    .long 0x38a0fde7
                    .long 0x3e944000
                    .long 0x38ad09ef
                    .long 0x3e974000
                    .long 0x3862bae1
                    .long 0x3e9a3000
                    .long 0x38eecd4c
                    .long 0x3e9d3000
                    .long 0x3798aad2
                    .long 0x3ea02000
                    .long 0x37421a1a
                    .long 0x3ea30000
                    .long 0x38c5e10e
                    .long 0x3ea5f000
                    .long 0x37bf2aee
                    .long 0x3ea8d000
                    .long 0x382d872d
                    .long 0x3eabb000
                    .long 0x37ee2e8a
                    .long 0x3eae8000
                    .long 0x38dedfac
                    .long 0x3eb16000
                    .long 0x3802f2b9
                    .long 0x3eb43000
                    .long 0x38481e9b
                    .long 0x3eb70000
                    .long 0x380eaa2b
                    .long 0x3eb9c000
                    .long 0x38ebfb5d
                    .long 0x3ebc9000
                    .long 0x38255fdd
                    .long 0x3ebf5000
                    .long 0x38783b82
                    .long 0x3ec21000
                    .long 0x3851da1e
                    .long 0x3ec4d000
                    .long 0x374e1b05
                    .long 0x3ec78000
                    .long 0x388f439b
                    .long 0x3eca3000
                    .long 0x38ca0e10
                    .long 0x3ecce000
                    .long 0x38cac08b
                    .long 0x3ecf9000
                    .long 0x3891f65f
                    .long 0x3ed24000
                    .long 0x378121cb
                    .long 0x3ed4e000
                    .long 0x386c9a9a
                    .long 0x3ed78000
                    .long 0x38949923
                    .long 0x3eda2000
                    .long 0x38777bcc
                    .long 0x3edcc000
                    .long 0x37b12d26
                    .long 0x3edf5000
                    .long 0x38a6ced3
                    .long 0x3ee1e000
                    .long 0x38ebd3e6
                    .long 0x3ee47000
                    .long 0x38fbe3cd
                    .long 0x3ee70000
                    .long 0x38d785c2
                    .long 0x3ee99000
                    .long 0x387e7e00
                    .long 0x3eec1000
                    .long 0x38f392c5
                    .long 0x3eeea000
                    .long 0x37d40983
                    .long 0x3ef12000
                    .long 0x38081a7c
                    .long 0x3ef3a000
                    .long 0x3784c3ad
                    .long 0x3ef61000
                    .long 0x38cce923
                    .long 0x3ef89000
                    .long 0x380f5faf
                    .long 0x3efb0000
                    .long 0x3891fd38
                    .long 0x3efd7000
                    .long 0x38ac47bc
                    .long 0x3effe000
                    .long 0x3897042b
                    .long 0x3f012000
                    .long 0x392952d2
                    .long 0x3f025000
                    .long 0x396fced4
                    .long 0x3f039000
                    .long 0x37f97073
                    .long 0x3f04c000
                    .long 0x385e9eae
                    .long 0x3f05f000
                    .long 0x3865c84a
                    .long 0x3f072000
                    .long 0x38130ba3
                    .long 0x3f084000
                    .long 0x3979cf16
                    .long 0x3f097000
                    .long 0x3938cac9
                    .long 0x3f0aa000
                    .long 0x38c3d2f4
                    .long 0x3f0bc000
                    .long 0x39755dec
                    .long 0x3f0cf000
                    .long 0x38e6b467
                    .long 0x3f0e1000
                    .long 0x395c0fb8
                    .long 0x3f0f4000
                    .long 0x383ebce0
                    .long 0x3f106000
                    .long 0x38dcd192
                    .long 0x3f118000
                    .long 0x39186bdf
                    .long 0x3f12a000
                    .long 0x392de74c
                    .long 0x3f13c000
                    .long 0x392f0944
                    .long 0x3f14e000
                    .long 0x391bff61
                    .long 0x3f160000
                    .long 0x38e9ed44
                    .long 0x3f172000
                    .long 0x38686dc8
                    .long 0x3f183000
                    .long 0x396b99a7
                    .long 0x3f195000
                    .long 0x39099c89
                    .long 0x3f1a7000
                    .long 0x37a27673
                    .long 0x3f1b8000
                    .long 0x390bdaa3
                    .long 0x3f1c9000
                    .long 0x397069ab
                    .long 0x3f1db000
                    .long 0x388449ff
                    .long 0x3f1ec000
                    .long 0x39013538
                    .long 0x3f1fd000
                    .long 0x392dc268
                    .long 0x3f20e000
                    .long 0x3947f423
                    .long 0x3f21f000
                    .long 0x394ff17c
                    .long 0x3f230000
                    .long 0x3945e10e
                    .long 0x3f241000
                    .long 0x3929e8f5
                    .long 0x3f252000
                    .long 0x38f85db0
                    .long 0x3f263000
                    .long 0x38735f99
                    .long 0x3f273000
                    .long 0x396c08db
                    .long 0x3f284000
                    .long 0x3909e600
                    .long 0x3f295000
                    .long 0x37b4996f
                    .long 0x3f2a5000
                    .long 0x391233cc
                    .long 0x3f2b5000
                    .long 0x397cead9
                    .long 0x3f2c6000
                    .long 0x38adb5cd
                    .long 0x3f2d6000
                    .long 0x3920261a
                    .long 0x3f2e6000
                    .long 0x3958ee36
                    .long 0x3f2f7000
                    .long 0x35aa4905
                    .long 0x3f307000
                    .long 0x37cbd11e
                    .long 0x3f317000
                    .long 0x3805fdf4

                    
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


