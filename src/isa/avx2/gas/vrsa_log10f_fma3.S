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

# vrs4_log10f_bdozr.S
#
# An implementation of the log10f libm function.
#
# Prototype:
#
#     float log10f(float x);
#

#
#   Algorithm:
#       Similar to one presnted in log.S
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(vrsa_log10f)
#define fname_special _log10f_special@PLT


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
    VANDNPD %xmm11, %xmm7, %xmm2
    VANDPD %xmm7, %xmm10, %xmm7 
    VORPD %xmm2, %xmm7, %xmm7
     
#vpcmpeqd     .L__real_ef(%rip), %xmm0, %xmm8    #if (x == e)  return 1.0
#vpor         %xmm6, %xmm8, %xmm6
#xmm8 stores the mask for e
#vpcmov       %xmm8,%xmm11,%xmm15,%xmm7
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
    vmovaps      .L__real_log10_2_tail(%rip), %xmm3  #xmm3  = log2_tail
    vmovaps      .L__real_log10_2_lead(%rip), %xmm10 #xmm10 = log2_lead
    vfmadd231ps     %xmm2,%xmm14, %xmm1  #poly = r + 1/2*r*r + 1/3*r*r*r
    vmulps       .L__real_log10_e(%rip), %xmm1,%xmm1 # poly = log10_e* poly
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

    vpand       .L__mask_lower(%rip), %xmm0, %xmm5
    vsubps      %xmm5, %xmm0, %xmm0
    vaddps       %xmm0, %xmm2, %xmm2
    vmulps      .L__real_log10_e_lead(%rip), %xmm2,%xmm1
    vmulps      .L__real_log10_e_tail(%rip), %xmm5,%xmm0
    vmulps      .L__real_log10_e_lead(%rip), %xmm5,%xmm5

    # r + r2
    vfmadd231ps      .L__real_log10_e_tail(%rip),%xmm2,%xmm0 #
    vaddps       %xmm1, %xmm0,%xmm0 #
    vaddps       %xmm5, %xmm0,%xmm0 #
 
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
    VANDPD %xmm4, %xmm0, %xmm0
    VANDNPD %xmm9, %xmm4, %xmm10
    VORPD %xmm10, %xmm0, %xmm0
    
    #vpcmov    %xmm6,%xmm0,%xmm7,%xmm0
    VANDNPD %xmm0, %xmm6, %xmm10
    VANDPD %xmm6, %xmm7, %xmm0
    VORPD %xmm10, %xmm0, %xmm0
        

    #vpcmov    %xmm12,%xmm0,%xmm5,%xmm0
    VANDNPD %xmm0, %xmm12, %xmm10
    VANDPD %xmm12, %xmm5, %xmm0
    VORPD %xmm10, %xmm0, %xmm0

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

.L__real_log10_e_lead:  .quad 0x3EDE00003EDE0000    # log10e_lead  0.4335937500
                        .quad 0x3EDE00003EDE0000
.L__real_log10_e_tail:  .quad 0x3A37B1523A37B152    # log10e_tail  0.0007007319
                        .quad 0x3A37B1523A37B152

.L__real_log10_2_lead:  .quad 0x3e9a00003e9a0000
                        .quad 0x3e9a00003e9a0000
.L__real_log10_2_tail:  .quad 0x39826a1339826a13
                        .quad 0x39826a1339826a13
.L__real_log10_e:       .quad 0x3ede5bd93ede5bd9
                        .quad 0x3ede5bd93ede5bd9

.L__mask_lower:         .quad 0x0ffff0000ffff0000
                        .quad 0x0ffff0000ffff0000


.align 16
.L__log_128_lead_tail:
                    .long 0x00000000
                    .long 0x00000000
                    .long 0x3b5d4000
                    .long 0x367a8e44
                    .long 0x3bdc8000
                    .long 0x368ed49f
                    .long 0x3c24c000
                    .long 0x36c21451
                    .long 0x3c5ac000
                    .long 0x375211d6
                    .long 0x3c884000
                    .long 0x3720ea11
                    .long 0x3ca2c000
                    .long 0x37e9eb59
                    .long 0x3cbd4000
                    .long 0x37b87be7
                    .long 0x3cd78000
                    .long 0x37bf2560
                    .long 0x3cf1c000
                    .long 0x33d597a0
                    .long 0x3d05c000
                    .long 0x37806a05
                    .long 0x3d128000
                    .long 0x3820581f
                    .long 0x3d1f4000
                    .long 0x38223334
                    .long 0x3d2c0000
                    .long 0x378e3bac
                    .long 0x3d388000
                    .long 0x3810684f
                    .long 0x3d450000
                    .long 0x37feb7ae
                    .long 0x3d518000
                    .long 0x36a9d609
                    .long 0x3d5dc000
                    .long 0x37a68163
                    .long 0x3d6a0000
                    .long 0x376a8b27
                    .long 0x3d760000
                    .long 0x384c8fd6
                    .long 0x3d810000
                    .long 0x3885183e
                    .long 0x3d870000
                    .long 0x3874a760
                    .long 0x3d8d0000
                    .long 0x380d1154
                    .long 0x3d92c000
                    .long 0x38ea42bd
                    .long 0x3d98c000
                    .long 0x384c1571
                    .long 0x3d9e8000
                    .long 0x38ba66b8
                    .long 0x3da44000
                    .long 0x38e7da3b
                    .long 0x3daa0000
                    .long 0x38eee632
                    .long 0x3dafc000
                    .long 0x38d00911
                    .long 0x3db58000
                    .long 0x388bbede
                    .long 0x3dbb4000
                    .long 0x378a0512
                    .long 0x3dc0c000
                    .long 0x3894c7a0
                    .long 0x3dc64000
                    .long 0x38e30710
                    .long 0x3dcc0000
                    .long 0x36db2829
                    .long 0x3dd18000
                    .long 0x3729d609
                    .long 0x3dd6c000
                    .long 0x38fa0e82
                    .long 0x3ddc4000
                    .long 0x38bc9a75
                    .long 0x3de1c000
                    .long 0x383a9297
                    .long 0x3de70000
                    .long 0x38dc83c8
                    .long 0x3dec8000
                    .long 0x37eac335
                    .long 0x3df1c000
                    .long 0x38706ac3
                    .long 0x3df70000
                    .long 0x389574c2
                    .long 0x3dfc4000
                    .long 0x3892d068
                    .long 0x3e00c000
                    .long 0x38615032
                    .long 0x3e034000
                    .long 0x3917acf4
                    .long 0x3e05c000
                    .long 0x3967a126
                    .long 0x3e088000
                    .long 0x38217840
                    .long 0x3e0b0000
                    .long 0x38b420ab
                    .long 0x3e0d8000
                    .long 0x38f9c7b2
                    .long 0x3e100000
                    .long 0x391103bd
                    .long 0x3e128000
                    .long 0x39169a6b
                    .long 0x3e150000
                    .long 0x390dd194
                    .long 0x3e178000
                    .long 0x38eda471
                    .long 0x3e1a0000
                    .long 0x38a38950
                    .long 0x3e1c8000
                    .long 0x37f6844a
                    .long 0x3e1ec000
                    .long 0x395e1cdb
                    .long 0x3e214000
                    .long 0x390fcffc
                    .long 0x3e23c000
                    .long 0x38503e9d
                    .long 0x3e260000
                    .long 0x394b00fd
                    .long 0x3e288000
                    .long 0x38a9910a
                    .long 0x3e2ac000
                    .long 0x39518a31
                    .long 0x3e2d4000
                    .long 0x3882d2c2
                    .long 0x3e2f8000
                    .long 0x392488e4
                    .long 0x3e31c000
                    .long 0x397b0aff
                    .long 0x3e344000
                    .long 0x388a22d8
                    .long 0x3e368000
                    .long 0x3902bd5e
                    .long 0x3e38c000
                    .long 0x39342f85
                    .long 0x3e3b0000
                    .long 0x39598811
                    .long 0x3e3d4000
                    .long 0x3972e6b1
                    .long 0x3e3fc000
                    .long 0x34d53654
                    .long 0x3e420000
                    .long 0x360ca25e
                    .long 0x3e440000
                    .long 0x39785cc0
                    .long 0x3e464000
                    .long 0x39630710
                    .long 0x3e488000
                    .long 0x39424ed7
                    .long 0x3e4ac000
                    .long 0x39165101
                    .long 0x3e4d0000
                    .long 0x38be5421
                    .long 0x3e4f4000
                    .long 0x37e7b0c0
                    .long 0x3e514000
                    .long 0x394fd0c3
                    .long 0x3e538000
                    .long 0x38efaaaa
                    .long 0x3e55c000
                    .long 0x37a8f566
                    .long 0x3e57c000
                    .long 0x3927c744
                    .long 0x3e5a0000
                    .long 0x383fa4d5
                    .long 0x3e5c0000
                    .long 0x392d9e39
                    .long 0x3e5e4000
                    .long 0x3803feae
                    .long 0x3e604000
                    .long 0x390a268c
                    .long 0x3e624000
                    .long 0x39692b80
                    .long 0x3e648000
                    .long 0x38789b4f
                    .long 0x3e668000
                    .long 0x3909307d
                    .long 0x3e688000
                    .long 0x394a601c
                    .long 0x3e6ac000
                    .long 0x35e67edc
                    .long 0x3e6cc000
                    .long 0x383e386d
                    .long 0x3e6ec000
                    .long 0x38a7743d
                    .long 0x3e70c000
                    .long 0x38dccec3
                    .long 0x3e72c000
                    .long 0x38ff57e0
                    .long 0x3e74c000
                    .long 0x39079d8b
                    .long 0x3e76c000
                    .long 0x390651a6
                    .long 0x3e78c000
                    .long 0x38f7bad9
                    .long 0x3e7ac000
                    .long 0x38d0ab82
                    .long 0x3e7cc000
                    .long 0x38979e7d
                    .long 0x3e7ec000
                    .long 0x381978ee
                    .long 0x3e804000
                    .long 0x397816c8
                    .long 0x3e814000
                    .long 0x39410cb2
                    .long 0x3e824000
                    .long 0x39015384
                    .long 0x3e834000
                    .long 0x3863fa28
                    .long 0x3e840000
                    .long 0x39f41065
                    .long 0x3e850000
                    .long 0x39c7668a
                    .long 0x3e860000
                    .long 0x39968afa
                    .long 0x3e870000
                    .long 0x39430db9
                    .long 0x3e880000
                    .long 0x38a18cf3
                    .long 0x3e88c000
                    .long 0x39eb2907
                    .long 0x3e89c000
                    .long 0x39a9e10c
                    .long 0x3e8ac000
                    .long 0x39492800
                    .long 0x3e8bc000
                    .long 0x385a53d1
                    .long 0x3e8c8000
                    .long 0x39ce0cf7
                    .long 0x3e8d8000
                    .long 0x3979c7b2
                    .long 0x3e8e8000
                    .long 0x389f5d99
                    .long 0x3e8f4000
                    .long 0x39ceefcb
                    .long 0x3e904000
                    .long 0x39646a39
                    .long 0x3e914000
                    .long 0x380d7a9b
                    .long 0x3e920000
                    .long 0x39ad6650
                    .long 0x3e930000
                    .long 0x390ac3b8
                    .long 0x3e93c000
                    .long 0x39d9a9a8
                    .long 0x3e94c000
                    .long 0x39548a99
                    .long 0x3e958000
                    .long 0x39f73c4b
                    .long 0x3e968000
                    .long 0x3980960e
                    .long 0x3e978000
                    .long 0x374b3d5a
                    .long 0x3e984000
                    .long 0x39888f1e
                    .long 0x3e994000
                    .long 0x37679a07
                    .long 0x3e9a0000
                    .long 0x39826a13
                    
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


