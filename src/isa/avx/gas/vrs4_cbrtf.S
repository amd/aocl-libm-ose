#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# vrs4_cbrtf.s
#
# A vector implementation of the cbrtf libm function.
#  This routine implemented in single precision.  It is slightly
#  less accurate than the double precision version, but it will
#  be better for vectorizing.
#
# Prototype:
#
#     __m128 vrs4_cbrtf(__m128 x);
#
#   Computes the floating point single precision cube root of x.
#   Returns proper C99 values, but may not raise status flags properly.
#
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(vrs4_cbrtf)

#define fname_special vrs4_cbrtf_special

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15

# define local variable storage offsets
.equ    indexes_0,0x010    # save the array indexs
.equ    indexes_1,0x014    # save the array indexs
.equ    indexes_2,0x018    # save the array indexs
.equ    indexes_3,0x01c    # save the array indexs
.equ    stack_size,0x030



.globl fname
.type  fname,@function
fname:
    sub      $stack_size,%rsp
    
    #set the signmask. xmm2 stores the sign mask
    movaps   .L__real_sign(%rip),%xmm2
    andps    %xmm0,%xmm2 #xmm2 stores the sign mask
    andps    .L__sign_mask(%rip),%xmm0 #xmm0 = all positive values
    movaps   %xmm0,%xmm1
    movaps   %xmm0,%xmm8 #copy the positive inputs to xmm8
    movaps   %xmm8,%xmm13
    punpckhqdq %xmm1,%xmm1 # unpack the inputs to convert to double
    cvtps2pd %xmm0,%xmm0   
    cvtps2pd %xmm1,%xmm1   
    movapd   .L__double_exp_mask(%rip),%xmm5 
    movapd   %xmm5,%xmm6
    andpd    %xmm0,%xmm5   # get the exponent part
    andpd    %xmm1,%xmm6
    movapd   .L__double_mant_mask(%rip),%xmm3 
    movapd   %xmm3,%xmm4
    andpd    %xmm0,%xmm3   # get the mantissa part
    andpd    %xmm1,%xmm4   
    movapd   %xmm3,%xmm0    
    movapd   %xmm4,%xmm1   
    pshufd   $0x0D,%xmm5,%xmm5 # 00 00 11 01 {0,0,3,1}  # move the exponent components to lower 2 32 bits
    pshufd   $0x0D,%xmm6,%xmm6 # 00 00 11 01 {0,0,3,1} 
    punpcklqdq %xmm6,%xmm5     # get all the exponents into one register
    psrld    $20,%xmm5         # get all the exact exponents
    movaps   .L__double_exp_bias(%rip),%xmm7     
    psubd    %xmm7,%xmm5     # xmm5 contains only the exp values
    cvtdq2ps %xmm5,%xmm6 
    pcmpgtd  %xmm7,%xmm5     # xmm5 contains the mask for Nans
    pcmpeqd  .L__real_zero(%rip),%xmm8 # xmm8 contains the mask for zeros
    por      %xmm8,%xmm5     # xmm5 contains the mask for zeros 
                             # and Nans
    #instaed of division operation we do mult operation
    #divps    .L__float_real_three(%rip),%xmm6
    mulps     .L__float_oneby3(%rip),%xmm6
    cvttps2dq %xmm6,%xmm8              #xmm8 is the scaling factor
    cvtdq2ps  %xmm8,%xmm9
    subps    %xmm9,%xmm6
    mulps    .L__float_real_three(%rip),%xmm6
    paddd    .L__float_exp_bias(%rip),%xmm8
    pslld    $23,%xmm8
    
    movapd   .L__double_real_one(%rip),%xmm7
    por      %xmm7,%xmm0  #x01 
    por      %xmm7,%xmm1  #x02 
    psrlq    $44,%xmm3
    psrlq    $44,%xmm4
    pshufd   $0xF8, %xmm3,%xmm3 # 11 11 10 00 
    pshufd   $0xF8, %xmm4,%xmm4
    punpcklqdq %xmm4,%xmm3
    movdqu   %xmm3,indexes_0(%rsp) 
    lea .L__DoubleReciprocalTable_(%rip),%rax
    lea .L__DoubleCubeRootTable_(%rip),%rcx
    xor %rdx,%rdx
    mov indexes_0(%rsp),%edx
    movlpd  (%rax,%rdx,8),%xmm3  # load from reciprocal table
    movlpd  (%rcx,%rdx,8),%xmm9  # load from cuberoot table
    mov indexes_1(%rsp),%edx
    movhpd  (%rax,%rdx,8),%xmm3  # load from reciprocal table
    movhpd  (%rcx,%rdx,8),%xmm9  # load from cuberoot table
    mov indexes_2(%rsp),%edx
    movlpd  (%rax,%rdx,8),%xmm4  # load from reciprocal table
    movlpd  (%rcx,%rdx,8),%xmm10 # load from cuberoot table
    mov indexes_3(%rsp),%edx
    movhpd  (%rax,%rdx,8),%xmm4  # load from reciprocal table
    movhpd  (%rcx,%rdx,8),%xmm10 # load from cuberoot table
    movapd  .L__coefficient_1(%rip), %xmm11
    movapd  .L__coefficient_2(%rip), %xmm12
    mulpd   %xmm0,%xmm3 # 
    mulpd   %xmm1,%xmm4 #
    subpd   %xmm7,%xmm3 # r 
    subpd   %xmm7,%xmm4 # r 
    movapd  %xmm3,%xmm0  
    movapd  %xmm4,%xmm1  
    mulpd   %xmm0,%xmm0 # r^2
    mulpd   %xmm1,%xmm1 # r^2
    mulpd   %xmm11,%xmm3 # (1/3)*r
    mulpd   %xmm11,%xmm4 # (1/3)*r
    mulpd   %xmm12,%xmm0 # -(1/9)* r^2
    mulpd   %xmm12,%xmm1 # -(1/9)* r^2
    addpd   %xmm3,%xmm0  # (1/3)*r -(1/9)* r^2
    addpd   %xmm4,%xmm1  # (1/3)*r -(1/9)* r^2
    addpd   %xmm7,%xmm0  # 1+ (1/3)*r -(1/9)* r^2
    addpd   %xmm7,%xmm1  # 1+ (1/3)*r -(1/9)* r^2
    mulpd   %xmm9,%xmm0  #
    mulpd   %xmm10,%xmm1 #
    movapd  %xmm8,%xmm3
    punpckhqdq %xmm8,%xmm8
    cvtps2pd  %xmm3,%xmm3
    cvtps2pd  %xmm8,%xmm8
    mulpd     %xmm3,%xmm0
    mulpd     %xmm8,%xmm1
    cvtps2dq  %xmm6,%xmm6
    movdqu    %xmm6,indexes_0(%rsp)

    lea .L__definedCubeRootTable_(%rip),%rax
    xor %rdx,%rdx
    mov indexes_0(%rsp),%edx
    add $2,%edx
    movlpd  (%rax,%rdx,8),%xmm3  # load from reciprocal table
    mov indexes_1(%rsp),%edx
    add $2,%edx
    movhpd  (%rax,%rdx,8),%xmm3  # load from reciprocal table
    mulpd %xmm3,%xmm0            # 
    mov indexes_2(%rsp),%edx
    add $2,%edx
    movlpd  (%rax,%rdx,8),%xmm4  # load from reciprocal table
    mov indexes_3(%rsp),%edx
    add $2,%edx
    movhpd  (%rax,%rdx,8),%xmm4  # load from reciprocal table
    mulpd %xmm4,%xmm1

    addps %xmm13,%xmm13
    andps %xmm5,%xmm13
    cvtpd2ps %xmm0,%xmm0
    cvtpd2ps %xmm1,%xmm1
    movlhps  %xmm1,%xmm0
    andnps   %xmm0,%xmm5 # xmm5 is inverted and anded with xmm0 
    orps     %xmm13,%xmm5
    orps     %xmm2,%xmm5
    movaps   %xmm5,%xmm0
    add      $stack_size,%rsp
    ret


.section .rodata
.align 64

.L__double_mant_mask:   .quad 0x000FFFFFFFFFFFFF    # mantisa bits double
                        .quad 0x000FFFFFFFFFFFFF
.L__double_exp_mask:    .quad 0x7FF0000000000000                        
                        .quad 0x7FF0000000000000
.L__float_exp_bias:     .quad 0x0000007F0000007F
                        .quad 0x0000007F0000007F
.L__double_exp_bias:    .quad 0x000003FF000003FF
                        .quad 0x000003FF000003FF
.L__float_real_three:   .quad 0x4040000040400000 # 3.0
                        .quad 0x4040000040400000 # 3.0
.L__double_real_one:    .quad 0x3ff0000000000000 # 1.0
                        .quad 0x3ff0000000000000
.L__float_oneby3:       .quad 0x3EAAAAAB3EAAAAAB# (1/3)
                        .quad 0x3EAAAAAB3EAAAAAB
.L__coefficient_1:      .quad 0x3fd5555555555555 # (1/3)
                        .quad 0x3fd5555555555555
.L__coefficient_2:      .quad 0xbFBC71C71C71C71C # - (1/9)
                        .quad 0xbFBC71C71C71C71C
.L__real_zero:          .quad 0x0000000000000000    # 0.0
                        .quad 0x0000000000000000
.L__real_sign:          .quad 0x8000000080000000    # sign bit
                        .quad 0x8000000080000000
.L__sign_mask:          .quad 0x7FFFFFFF7FFFFFFF
                        .quad 0x7FFFFFFF7FFFFFFF
.L__DoubleReciprocalTable_:
              .quad 0x3ff0000000000000   # 1
              .quad 0x3fefe00000000000   # 0.996094
              .quad 0x3fefc00000000000   # 0.992188
              .quad 0x3fefa00000000000   # 0.988281
              .quad 0x3fef800000000000   # 0.984375
              .quad 0x3fef600000000000   # 0.980469
              .quad 0x3fef400000000000   # 0.976562
              .quad 0x3fef200000000000   # 0.972656
              .quad 0x3fef000000000000   # 0.96875
              .quad 0x3feee00000000000   # 0.964844
              .quad 0x3feec00000000000   # 0.960938
              .quad 0x3feea00000000000   # 0.957031
              .quad 0x3fee900000000000   # 0.955078
              .quad 0x3fee700000000000   # 0.951172
              .quad 0x3fee500000000000   # 0.947266
              .quad 0x3fee300000000000   # 0.943359
              .quad 0x3fee100000000000   # 0.939453
              .quad 0x3fee000000000000   # 0.9375
              .quad 0x3fede00000000000   # 0.933594
              .quad 0x3fedc00000000000   # 0.929688
              .quad 0x3feda00000000000   # 0.925781
              .quad 0x3fed900000000000   # 0.923828
              .quad 0x3fed700000000000   # 0.919922
              .quad 0x3fed500000000000   # 0.916016
              .quad 0x3fed400000000000   # 0.914062
              .quad 0x3fed200000000000   # 0.910156
              .quad 0x3fed000000000000   # 0.90625
              .quad 0x3fecf00000000000   # 0.904297
              .quad 0x3fecd00000000000   # 0.900391
              .quad 0x3fecb00000000000   # 0.896484
              .quad 0x3feca00000000000   # 0.894531
              .quad 0x3fec800000000000   # 0.890625
              .quad 0x3fec700000000000   # 0.888672
              .quad 0x3fec500000000000   # 0.884766
              .quad 0x3fec300000000000   # 0.880859
              .quad 0x3fec200000000000   # 0.878906
              .quad 0x3fec000000000000   # 0.875
              .quad 0x3febf00000000000   # 0.873047
              .quad 0x3febd00000000000   # 0.869141
              .quad 0x3febc00000000000   # 0.867188
              .quad 0x3feba00000000000   # 0.863281
              .quad 0x3feb900000000000   # 0.861328
              .quad 0x3feb700000000000   # 0.857422
              .quad 0x3feb600000000000   # 0.855469
              .quad 0x3feb400000000000   # 0.851562
              .quad 0x3feb300000000000   # 0.849609
              .quad 0x3feb200000000000   # 0.847656
              .quad 0x3feb000000000000   # 0.84375
              .quad 0x3feaf00000000000   # 0.841797
              .quad 0x3fead00000000000   # 0.837891
              .quad 0x3feac00000000000   # 0.835938
              .quad 0x3feaa00000000000   # 0.832031
              .quad 0x3fea900000000000   # 0.830078
              .quad 0x3fea800000000000   # 0.828125
              .quad 0x3fea600000000000   # 0.824219
              .quad 0x3fea500000000000   # 0.822266
              .quad 0x3fea400000000000   # 0.820312
              .quad 0x3fea200000000000   # 0.816406
              .quad 0x3fea100000000000   # 0.814453
              .quad 0x3fea000000000000   # 0.8125
              .quad 0x3fe9e00000000000   # 0.808594
              .quad 0x3fe9d00000000000   # 0.806641
              .quad 0x3fe9c00000000000   # 0.804688
              .quad 0x3fe9a00000000000   # 0.800781
              .quad 0x3fe9900000000000   # 0.798828
              .quad 0x3fe9800000000000   # 0.796875
              .quad 0x3fe9700000000000   # 0.794922
              .quad 0x3fe9500000000000   # 0.791016
              .quad 0x3fe9400000000000   # 0.789062
              .quad 0x3fe9300000000000   # 0.787109
              .quad 0x3fe9200000000000   # 0.785156
              .quad 0x3fe9000000000000   # 0.78125
              .quad 0x3fe8f00000000000   # 0.779297
              .quad 0x3fe8e00000000000   # 0.777344
              .quad 0x3fe8d00000000000   # 0.775391
              .quad 0x3fe8b00000000000   # 0.771484
              .quad 0x3fe8a00000000000   # 0.769531
              .quad 0x3fe8900000000000   # 0.767578
              .quad 0x3fe8800000000000   # 0.765625
              .quad 0x3fe8700000000000   # 0.763672
              .quad 0x3fe8600000000000   # 0.761719
              .quad 0x3fe8400000000000   # 0.757812
              .quad 0x3fe8300000000000   # 0.755859
              .quad 0x3fe8200000000000   # 0.753906
              .quad 0x3fe8100000000000   # 0.751953
              .quad 0x3fe8000000000000   # 0.75
              .quad 0x3fe7f00000000000   # 0.748047
              .quad 0x3fe7e00000000000   # 0.746094
              .quad 0x3fe7d00000000000   # 0.744141
              .quad 0x3fe7b00000000000   # 0.740234
              .quad 0x3fe7a00000000000   # 0.738281
              .quad 0x3fe7900000000000   # 0.736328
              .quad 0x3fe7800000000000   # 0.734375
              .quad 0x3fe7700000000000   # 0.732422
              .quad 0x3fe7600000000000   # 0.730469
              .quad 0x3fe7500000000000   # 0.728516
              .quad 0x3fe7400000000000   # 0.726562
              .quad 0x3fe7300000000000   # 0.724609
              .quad 0x3fe7200000000000   # 0.722656
              .quad 0x3fe7100000000000   # 0.720703
              .quad 0x3fe7000000000000   # 0.71875
              .quad 0x3fe6f00000000000   # 0.716797
              .quad 0x3fe6e00000000000   # 0.714844
              .quad 0x3fe6d00000000000   # 0.712891
              .quad 0x3fe6c00000000000   # 0.710938
              .quad 0x3fe6b00000000000   # 0.708984
              .quad 0x3fe6a00000000000   # 0.707031
              .quad 0x3fe6900000000000   # 0.705078
              .quad 0x3fe6800000000000   # 0.703125
              .quad 0x3fe6700000000000   # 0.701172
              .quad 0x3fe6600000000000   # 0.699219
              .quad 0x3fe6500000000000   # 0.697266
              .quad 0x3fe6400000000000   # 0.695312
              .quad 0x3fe6300000000000   # 0.693359
              .quad 0x3fe6200000000000   # 0.691406
              .quad 0x3fe6100000000000   # 0.689453
              .quad 0x3fe6000000000000   # 0.6875
              .quad 0x3fe5f00000000000   # 0.685547
              .quad 0x3fe5e00000000000   # 0.683594
              .quad 0x3fe5d00000000000   # 0.681641
              .quad 0x3fe5c00000000000   # 0.679688
              .quad 0x3fe5b00000000000   # 0.677734
              .quad 0x3fe5a00000000000   # 0.675781
              .quad 0x3fe5900000000000   # 0.673828
              .quad 0x3fe5800000000000   # 0.671875
              .quad 0x3fe5800000000000   # 0.671875
              .quad 0x3fe5700000000000   # 0.669922
              .quad 0x3fe5600000000000   # 0.667969
              .quad 0x3fe5500000000000   # 0.666016
              .quad 0x3fe5400000000000   # 0.664062
              .quad 0x3fe5300000000000   # 0.662109
              .quad 0x3fe5200000000000   # 0.660156
              .quad 0x3fe5100000000000   # 0.658203
              .quad 0x3fe5000000000000   # 0.65625
              .quad 0x3fe5000000000000   # 0.65625
              .quad 0x3fe4f00000000000   # 0.654297
              .quad 0x3fe4e00000000000   # 0.652344
              .quad 0x3fe4d00000000000   # 0.650391
              .quad 0x3fe4c00000000000   # 0.648438
              .quad 0x3fe4b00000000000   # 0.646484
              .quad 0x3fe4a00000000000   # 0.644531
              .quad 0x3fe4a00000000000   # 0.644531
              .quad 0x3fe4900000000000   # 0.642578
              .quad 0x3fe4800000000000   # 0.640625
              .quad 0x3fe4700000000000   # 0.638672
              .quad 0x3fe4600000000000   # 0.636719
              .quad 0x3fe4600000000000   # 0.636719
              .quad 0x3fe4500000000000   # 0.634766
              .quad 0x3fe4400000000000   # 0.632812
              .quad 0x3fe4300000000000   # 0.630859
              .quad 0x3fe4200000000000   # 0.628906
              .quad 0x3fe4200000000000   # 0.628906
              .quad 0x3fe4100000000000   # 0.626953
              .quad 0x3fe4000000000000   # 0.625
              .quad 0x3fe3f00000000000   # 0.623047
              .quad 0x3fe3e00000000000   # 0.621094
              .quad 0x3fe3e00000000000   # 0.621094
              .quad 0x3fe3d00000000000   # 0.619141
              .quad 0x3fe3c00000000000   # 0.617188
              .quad 0x3fe3b00000000000   # 0.615234
              .quad 0x3fe3b00000000000   # 0.615234
              .quad 0x3fe3a00000000000   # 0.613281
              .quad 0x3fe3900000000000   # 0.611328
              .quad 0x3fe3800000000000   # 0.609375
              .quad 0x3fe3800000000000   # 0.609375
              .quad 0x3fe3700000000000   # 0.607422
              .quad 0x3fe3600000000000   # 0.605469
              .quad 0x3fe3500000000000   # 0.603516
              .quad 0x3fe3500000000000   # 0.603516
              .quad 0x3fe3400000000000   # 0.601562
              .quad 0x3fe3300000000000   # 0.599609
              .quad 0x3fe3200000000000   # 0.597656
              .quad 0x3fe3200000000000   # 0.597656
              .quad 0x3fe3100000000000   # 0.595703
              .quad 0x3fe3000000000000   # 0.59375
              .quad 0x3fe3000000000000   # 0.59375
              .quad 0x3fe2f00000000000   # 0.591797
              .quad 0x3fe2e00000000000   # 0.589844
              .quad 0x3fe2e00000000000   # 0.589844
              .quad 0x3fe2d00000000000   # 0.587891
              .quad 0x3fe2c00000000000   # 0.585938
              .quad 0x3fe2b00000000000   # 0.583984
              .quad 0x3fe2b00000000000   # 0.583984
              .quad 0x3fe2a00000000000   # 0.582031
              .quad 0x3fe2900000000000   # 0.580078
              .quad 0x3fe2900000000000   # 0.580078
              .quad 0x3fe2800000000000   # 0.578125
              .quad 0x3fe2700000000000   # 0.576172
              .quad 0x3fe2700000000000   # 0.576172
              .quad 0x3fe2600000000000   # 0.574219
              .quad 0x3fe2500000000000   # 0.572266
              .quad 0x3fe2500000000000   # 0.572266
              .quad 0x3fe2400000000000   # 0.570312
              .quad 0x3fe2300000000000   # 0.568359
              .quad 0x3fe2300000000000   # 0.568359
              .quad 0x3fe2200000000000   # 0.566406
              .quad 0x3fe2100000000000   # 0.564453
              .quad 0x3fe2100000000000   # 0.564453
              .quad 0x3fe2000000000000   # 0.5625
              .quad 0x3fe2000000000000   # 0.5625
              .quad 0x3fe1f00000000000   # 0.560547
              .quad 0x3fe1e00000000000   # 0.558594
              .quad 0x3fe1e00000000000   # 0.558594
              .quad 0x3fe1d00000000000   # 0.556641
              .quad 0x3fe1c00000000000   # 0.554688
              .quad 0x3fe1c00000000000   # 0.554688
              .quad 0x3fe1b00000000000   # 0.552734
              .quad 0x3fe1b00000000000   # 0.552734
              .quad 0x3fe1a00000000000   # 0.550781
              .quad 0x3fe1900000000000   # 0.548828
              .quad 0x3fe1900000000000   # 0.548828
              .quad 0x3fe1800000000000   # 0.546875
              .quad 0x3fe1800000000000   # 0.546875
              .quad 0x3fe1700000000000   # 0.544922
              .quad 0x3fe1600000000000   # 0.542969
              .quad 0x3fe1600000000000   # 0.542969
              .quad 0x3fe1500000000000   # 0.541016
              .quad 0x3fe1500000000000   # 0.541016
              .quad 0x3fe1400000000000   # 0.539062
              .quad 0x3fe1300000000000   # 0.537109
              .quad 0x3fe1300000000000   # 0.537109
              .quad 0x3fe1200000000000   # 0.535156
              .quad 0x3fe1200000000000   # 0.535156
              .quad 0x3fe1100000000000   # 0.533203
              .quad 0x3fe1100000000000   # 0.533203
              .quad 0x3fe1000000000000   # 0.53125
              .quad 0x3fe0f00000000000   # 0.529297
              .quad 0x3fe0f00000000000   # 0.529297
              .quad 0x3fe0e00000000000   # 0.527344
              .quad 0x3fe0e00000000000   # 0.527344
              .quad 0x3fe0d00000000000   # 0.525391
              .quad 0x3fe0d00000000000   # 0.525391
              .quad 0x3fe0c00000000000   # 0.523438
              .quad 0x3fe0c00000000000   # 0.523438
              .quad 0x3fe0b00000000000   # 0.521484
              .quad 0x3fe0a00000000000   # 0.519531
              .quad 0x3fe0a00000000000   # 0.519531
              .quad 0x3fe0900000000000   # 0.517578
              .quad 0x3fe0900000000000   # 0.517578
              .quad 0x3fe0800000000000   # 0.515625
              .quad 0x3fe0800000000000   # 0.515625
              .quad 0x3fe0700000000000   # 0.513672
              .quad 0x3fe0700000000000   # 0.513672
              .quad 0x3fe0600000000000   # 0.511719
              .quad 0x3fe0600000000000   # 0.511719
              .quad 0x3fe0500000000000   # 0.509766
              .quad 0x3fe0500000000000   # 0.509766
              .quad 0x3fe0400000000000   # 0.507812
              .quad 0x3fe0400000000000   # 0.507812
              .quad 0x3fe0300000000000   # 0.505859
              .quad 0x3fe0300000000000   # 0.505859
              .quad 0x3fe0200000000000   # 0.503906
              .quad 0x3fe0200000000000   # 0.503906
              .quad 0x3fe0100000000000   # 0.501953
              .quad 0x3fe0100000000000   # 0.501953
              .quad 0x3fe0000000000000   # 0.5

.L__DoubleCubeRootTable_:
              .quad 0x3ff0000000000000   # 1
              .quad 0x3ff00558e6547c36   # 1.00131
              .quad 0x3ff00ab8f9d2f374   # 1.00262
              .quad 0x3ff010204b673fc7   # 1.00394
              .quad 0x3ff0158eec36749b   # 1.00526
              .quad 0x3ff01b04ed9fdb53   # 1.0066
              .quad 0x3ff02082613df53c   # 1.00794
              .quad 0x3ff0260758e78308   # 1.00928
              .quad 0x3ff02b93e6b091f0   # 1.01064
              .quad 0x3ff031281ceb8ea2   # 1.012
              .quad 0x3ff036c40e2a5e2a   # 1.01337
              .quad 0x3ff03c67cd3f7cea   # 1.01475
              .quad 0x3ff03f3c9fee224c   # 1.01544
              .quad 0x3ff044ec379f7f79   # 1.01683
              .quad 0x3ff04aa3cd578d67   # 1.01822
              .quad 0x3ff0506374d40a3d   # 1.01963
              .quad 0x3ff0562b4218a6e3   # 1.02104
              .quad 0x3ff059123d3a9848   # 1.02175
              .quad 0x3ff05ee6694e7166   # 1.02317
              .quad 0x3ff064c2ee6e07c6   # 1.0246
              .quad 0x3ff06aa7e19c01c5   # 1.02604
              .quad 0x3ff06d9d8b1decca   # 1.02676
              .quad 0x3ff0738f4b6cc8e2   # 1.02821
              .quad 0x3ff07989af9f9f59   # 1.02967
              .quad 0x3ff07c8a2611201c   # 1.03041
              .quad 0x3ff08291a9958f03   # 1.03188
              .quad 0x3ff088a208c3fe28   # 1.03336
              .quad 0x3ff08bad91dd7d8b   # 1.0341
              .quad 0x3ff091cb6588465e   # 1.03559
              .quad 0x3ff097f24eab04a1   # 1.0371
              .quad 0x3ff09b0932aee3f2   # 1.03785
              .quad 0x3ff0a13de8970de4   # 1.03937
              .quad 0x3ff0a45bc08a5ac7   # 1.04013
              .quad 0x3ff0aa9e79bfa986   # 1.04166
              .quad 0x3ff0b0eaa961ca5b   # 1.04319
              .quad 0x3ff0b4145573271c   # 1.04396
              .quad 0x3ff0ba6ee5f9aad4   # 1.04552
              .quad 0x3ff0bd9fd0dbe02d   # 1.04629
              .quad 0x3ff0c408fc1cfd4b   # 1.04786
              .quad 0x3ff0c741430e2059   # 1.04865
              .quad 0x3ff0cdb9442ea813   # 1.05023
              .quad 0x3ff0d0f905168e6c   # 1.05102
              .quad 0x3ff0d7801893d261   # 1.05261
              .quad 0x3ff0dac772091bde   # 1.05341
              .quad 0x3ff0e15dd5c330ab   # 1.05502
              .quad 0x3ff0e4ace71080a4   # 1.05583
              .quad 0x3ff0e7fe920f3037   # 1.05664
              .quad 0x3ff0eea9c37e497e   # 1.05827
              .quad 0x3ff0f203512f4314   # 1.05909
              .quad 0x3ff0f8be68db7f32   # 1.06073
              .quad 0x3ff0fc1ffa42d902   # 1.06155
              .quad 0x3ff102eb3af9ed89   # 1.06321
              .quad 0x3ff10654f1e29cfb   # 1.06405
              .quad 0x3ff109c1679c189f   # 1.06488
              .quad 0x3ff110a29f080b3d   # 1.06656
              .quad 0x3ff114176891738a   # 1.06741
              .quad 0x3ff1178f0099b429   # 1.06825
              .quad 0x3ff11e86ac2cd7ab   # 1.06995
              .quad 0x3ff12206c7cf4046   # 1.07081
              .quad 0x3ff12589c21fb842   # 1.07166
              .quad 0x3ff12c986355d0d2   # 1.07339
              .quad 0x3ff13024129645cf   # 1.07425
              .quad 0x3ff133b2b13aa0eb   # 1.07512
              .quad 0x3ff13ad8cdc48ba3   # 1.07687
              .quad 0x3ff13e70544b1d4f   # 1.07774
              .quad 0x3ff1420adb77c99a   # 1.07862
              .quad 0x3ff145a867b1bfea   # 1.07951
              .quad 0x3ff14ceca1189d6d   # 1.08128
              .quad 0x3ff15093574284e9   # 1.08217
              .quad 0x3ff1543d2473ea9b   # 1.08307
              .quad 0x3ff157ea0d433a46   # 1.08396
              .quad 0x3ff15f4d44462724   # 1.08577
              .quad 0x3ff163039bd7cde6   # 1.08667
              .quad 0x3ff166bd21c3a8e2   # 1.08758
              .quad 0x3ff16a79dad1fb59   # 1.0885
              .quad 0x3ff171fcf9aaac3d   # 1.09033
              .quad 0x3ff175c3693980c3   # 1.09125
              .quad 0x3ff1798d1f73f3ef   # 1.09218
              .quad 0x3ff17d5a2156e97f   # 1.0931
              .quad 0x3ff1812a73ea2593   # 1.09403
              .quad 0x3ff184fe1c406b8f   # 1.09497
              .quad 0x3ff18caf82b8dba4   # 1.09685
              .quad 0x3ff1908d4b38a510   # 1.09779
              .quad 0x3ff1946e7e36f7e5   # 1.09874
              .quad 0x3ff1985320ff72a2   # 1.09969
              .quad 0x3ff19c3b38e975a8   # 1.10064
              .quad 0x3ff1a026cb58453d   # 1.1016
              .quad 0x3ff1a415ddbb2c10   # 1.10256
              .quad 0x3ff1a808758d9e32   # 1.10352
              .quad 0x3ff1aff84bac98ea   # 1.10546
              .quad 0x3ff1b3f5952e1a50   # 1.10644
              .quad 0x3ff1b7f67a896220   # 1.10741
              .quad 0x3ff1bbfb0178d186   # 1.10839
              .quad 0x3ff1c0032fc3cf91   # 1.10938
              .quad 0x3ff1c40f0b3eefc4   # 1.11037
              .quad 0x3ff1c81e99cc193f   # 1.11136
              .quad 0x3ff1cc31e15aae72   # 1.11235
              .quad 0x3ff1d048e7e7b565   # 1.11335
              .quad 0x3ff1d463b37e0090   # 1.11435
              .quad 0x3ff1d8824a365852   # 1.11536
              .quad 0x3ff1dca4b237a4f7   # 1.11637
              .quad 0x3ff1e0caf1b71965   # 1.11738
              .quad 0x3ff1e4f50ef85e61   # 1.1184
              .quad 0x3ff1e923104dbe76   # 1.11942
              .quad 0x3ff1ed54fc185286   # 1.12044
              .quad 0x3ff1f18ad8c82efc   # 1.12147
              .quad 0x3ff1f5c4acdc91aa   # 1.1225
              .quad 0x3ff1fa027ee4105b   # 1.12354
              .quad 0x3ff1fe44557cc808   # 1.12458
              .quad 0x3ff2028a37548ccf   # 1.12562
              .quad 0x3ff206d42b291a95   # 1.12667
              .quad 0x3ff20b2237c8466a   # 1.12772
              .quad 0x3ff20f74641030a6   # 1.12877
              .quad 0x3ff213cab6ef77c7   # 1.12983
              .quad 0x3ff2182537656c13   # 1.13089
              .quad 0x3ff21c83ec824406   # 1.13196
              .quad 0x3ff220e6dd675180   # 1.13303
              .quad 0x3ff2254e114737d2   # 1.13411
              .quad 0x3ff229b98f66228c   # 1.13519
              .quad 0x3ff22e295f19fd31   # 1.13627
              .quad 0x3ff2329d87caabb6   # 1.13736
              .quad 0x3ff2371610f243f2   # 1.13845
              .quad 0x3ff23b93021d47da   # 1.13954
              .quad 0x3ff2401462eae0b8   # 1.14064
              .quad 0x3ff2449a3b0d1b3f   # 1.14175
              .quad 0x3ff2449a3b0d1b3f   # 1.14175
              .quad 0x3ff2492492492492   # 1.14286
              .quad 0x3ff24db370778844   # 1.14397
              .quad 0x3ff25246dd846f45   # 1.14509
              .quad 0x3ff256dee16fdfd4   # 1.14621
              .quad 0x3ff25b7b844dfe71   # 1.14733
              .quad 0x3ff2601cce474fd2   # 1.14846
              .quad 0x3ff264c2c798fbe5   # 1.1496
              .quad 0x3ff2696d789511e2   # 1.15074
              .quad 0x3ff2696d789511e2   # 1.15074
              .quad 0x3ff26e1ce9a2cd73   # 1.15188
              .quad 0x3ff272d1233edcf3   # 1.15303
              .quad 0x3ff2778a2dfba8d0   # 1.15418
              .quad 0x3ff27c4812819c13   # 1.15534
              .quad 0x3ff2810ad98f6e10   # 1.1565
              .quad 0x3ff285d28bfa6d45   # 1.15767
              .quad 0x3ff285d28bfa6d45   # 1.15767
              .quad 0x3ff28a9f32aecb79   # 1.15884
              .quad 0x3ff28f70d6afeb08   # 1.16002
              .quad 0x3ff294478118ad83   # 1.1612
              .quad 0x3ff299233b1bc38a   # 1.16239
              .quad 0x3ff299233b1bc38a   # 1.16239
              .quad 0x3ff29e040e03fdfb   # 1.16358
              .quad 0x3ff2a2ea0334a07b   # 1.16477
              .quad 0x3ff2a7d52429b556   # 1.16597
              .quad 0x3ff2acc57a7862c2   # 1.16718
              .quad 0x3ff2acc57a7862c2   # 1.16718
              .quad 0x3ff2b1bb0fcf4190   # 1.16839
              .quad 0x3ff2b6b5edf6b54a   # 1.16961
              .quad 0x3ff2bbb61ed145cf   # 1.17083
              .quad 0x3ff2c0bbac5bfa6e   # 1.17205
              .quad 0x3ff2c0bbac5bfa6e   # 1.17205
              .quad 0x3ff2c5c6a0aeb681   # 1.17329
              .quad 0x3ff2cad705fc97a6   # 1.17452
              .quad 0x3ff2cfece6945583   # 1.17576
              .quad 0x3ff2cfece6945583   # 1.17576
              .quad 0x3ff2d5084ce0a331   # 1.17701
              .quad 0x3ff2da294368924f   # 1.17826
              .quad 0x3ff2df4fd4cff7c3   # 1.17952
              .quad 0x3ff2df4fd4cff7c3   # 1.17952
              .quad 0x3ff2e47c0bd7d237   # 1.18078
              .quad 0x3ff2e9adf35eb25a   # 1.18205
              .quad 0x3ff2eee5966124e8   # 1.18332
              .quad 0x3ff2eee5966124e8   # 1.18332
              .quad 0x3ff2f422fffa1e92   # 1.1846
              .quad 0x3ff2f9663b6369b6   # 1.18589
              .quad 0x3ff2feaf53f61612   # 1.18718
              .quad 0x3ff2feaf53f61612   # 1.18718
              .quad 0x3ff303fe552aea57   # 1.18847
              .quad 0x3ff309534a9ad7ce   # 1.18978
              .quad 0x3ff309534a9ad7ce   # 1.18978
              .quad 0x3ff30eae3fff6ff3   # 1.19108
              .quad 0x3ff3140f41335c2f   # 1.1924
              .quad 0x3ff3140f41335c2f   # 1.1924
              .quad 0x3ff319765a32d7ae   # 1.19372
              .quad 0x3ff31ee3971c2b5b   # 1.19504
              .quad 0x3ff3245704302c13   # 1.19637
              .quad 0x3ff3245704302c13   # 1.19637
              .quad 0x3ff329d0add2bb20   # 1.19771
              .quad 0x3ff32f50a08b48f9   # 1.19905
              .quad 0x3ff32f50a08b48f9   # 1.19905
              .quad 0x3ff334d6e9055a5f   # 1.2004
              .quad 0x3ff33a6394110fe6   # 1.20176
              .quad 0x3ff33a6394110fe6   # 1.20176
              .quad 0x3ff33ff6aea3afed   # 1.20312
              .quad 0x3ff3459045d8331b   # 1.20448
              .quad 0x3ff3459045d8331b   # 1.20448
              .quad 0x3ff34b3066efd36b   # 1.20586
              .quad 0x3ff350d71f529dd8   # 1.20724
              .quad 0x3ff350d71f529dd8   # 1.20724
              .quad 0x3ff356847c9006b4   # 1.20862
              .quad 0x3ff35c388c5f80bf   # 1.21001
              .quad 0x3ff35c388c5f80bf   # 1.21001
              .quad 0x3ff361f35ca116ff   # 1.21141
              .quad 0x3ff361f35ca116ff   # 1.21141
              .quad 0x3ff367b4fb5e0985   # 1.21282
              .quad 0x3ff36d7d76c96d0a   # 1.21423
              .quad 0x3ff36d7d76c96d0a   # 1.21423
              .quad 0x3ff3734cdd40cd95   # 1.21565
              .quad 0x3ff379233d4cd42a   # 1.21707
              .quad 0x3ff379233d4cd42a   # 1.21707
              .quad 0x3ff37f00a5a1ef96   # 1.21851
              .quad 0x3ff37f00a5a1ef96   # 1.21851
              .quad 0x3ff384e52521006c   # 1.21995
              .quad 0x3ff38ad0cad80848   # 1.22139
              .quad 0x3ff38ad0cad80848   # 1.22139
              .quad 0x3ff390c3a602dc60   # 1.22284
              .quad 0x3ff390c3a602dc60   # 1.22284
              .quad 0x3ff396bdc60bdb88   # 1.2243
              .quad 0x3ff39cbf3a8ca7a9   # 1.22577
              .quad 0x3ff39cbf3a8ca7a9   # 1.22577
              .quad 0x3ff3a2c8134ee2d1   # 1.22724
              .quad 0x3ff3a2c8134ee2d1   # 1.22724
              .quad 0x3ff3a8d8604cefe3   # 1.22872
              .quad 0x3ff3aef031b2b706   # 1.23021
              .quad 0x3ff3aef031b2b706   # 1.23021
              .quad 0x3ff3b50f97de6de5   # 1.2317
              .quad 0x3ff3b50f97de6de5   # 1.2317
              .quad 0x3ff3bb36a36163d8   # 1.23321
              .quad 0x3ff3bb36a36163d8   # 1.23321
              .quad 0x3ff3c1656500d20a   # 1.23472
              .quad 0x3ff3c79bedb6afb8   # 1.23623
              .quad 0x3ff3c79bedb6afb8   # 1.23623
              .quad 0x3ff3cdda4eb28aa2   # 1.23776
              .quad 0x3ff3cdda4eb28aa2   # 1.23776
              .quad 0x3ff3d420995a63c0   # 1.23929
              .quad 0x3ff3d420995a63c0   # 1.23929
              .quad 0x3ff3da6edf4b9061   # 1.24083
              .quad 0x3ff3da6edf4b9061   # 1.24083
              .quad 0x3ff3e0c5325b9fc2   # 1.24238
              .quad 0x3ff3e723a499453f   # 1.24393
              .quad 0x3ff3e723a499453f   # 1.24393
              .quad 0x3ff3ed8a484d473a   # 1.24549
              .quad 0x3ff3ed8a484d473a   # 1.24549
              .quad 0x3ff3f3f92ffb72d8   # 1.24706
              .quad 0x3ff3f3f92ffb72d8   # 1.24706
              .quad 0x3ff3fa706e6394a4   # 1.24864
              .quad 0x3ff3fa706e6394a4   # 1.24864
              .quad 0x3ff400f01682764a   # 1.25023
              .quad 0x3ff400f01682764a   # 1.25023
              .quad 0x3ff407783b92e17a   # 1.25182
              .quad 0x3ff407783b92e17a   # 1.25182
              .quad 0x3ff40e08f10ea81a   # 1.25343
              .quad 0x3ff40e08f10ea81a   # 1.25343
              .quad 0x3ff414a24aafb1e6   # 1.25504
              .quad 0x3ff414a24aafb1e6   # 1.25504
              .quad 0x3ff41b445c710fa7   # 1.25666
              .quad 0x3ff41b445c710fa7   # 1.25666
              .quad 0x3ff421ef3a901411   # 1.25828
              .quad 0x3ff421ef3a901411   # 1.25828
              .quad 0x3ff428a2f98d728b   # 1.25992

.L__definedCubeRootTable_:
              .quad 0x3fe428a2f98d728b   # 0.629961
              .quad 0x3fe965fea53d6e3d   # 0.793701
              .quad 0x3ff0000000000000   # 1.0
              .quad 0x3ff428a2f98d728b   # 1.25992
              .quad 0x3ff965fea53d6e3d   # 1.5874


