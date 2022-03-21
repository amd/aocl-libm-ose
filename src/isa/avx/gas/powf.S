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

# powf.S
#
# An implementation of the powf libm function.
#
# Prototype:
#
#     float powf(float x, float y);
#

#
#   Algorithm:
#       x^y = e^(y*ln(x))
#
#       Look in exp, log for the respective algorithms
#

#include "fn_macros.h"
#include "weak_macros.h"
#include "exp_tables.h"
#define fname ALM_PROTO_BAS64(powf)
#define fname_special alm_powf_special@PLT

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:

    punpckldq  %xmm0,%xmm1   #xmm1 = XX,XX,x,y
    pxor       %xmm5,%xmm5   #xmm5 = will contain sign of final result
    movd       %xmm1,%eax    #eax  = y
    movd       %xmm0,%ecx    #ecx  = x
    mov        %ecx,%r9d
    
    and        $0x7fffffff,%eax #sign of y removed
    and        $0x7fffffff,%ecx #sign of x removed    
    cmp        $0x7f800000,%eax
    jae        .L__y_is_inf_or_nan
    cmp        $0x3F800000,%eax
    jle        .L__check_if_y_is_zero_or_pos_one
.L__y_is_not_zero_or_pos_one:   
    cmp        $0x7f800000,%ecx
    jae        .L__x_is_inf_or_nan

    #calculate mx and my to check if result goes above inf
    #mx *2 power my 
    
    # -----------------------------
    # compute log(x) here
    # -----------------------------


    #dy = (double)y  ,  dx = (double)x
    cvtps2pd %xmm1,%xmm0  #xmm0 = x,y
    cmp        $0x3F880000,%r9d
    jl         .L__x_is_neg_or_zero_or_near_one
    
.L__compute_logx:
    #calculate F and Y
    #eight_mant_bits = dx.u64 & 0x000ff00000000000;
    #ninth_mant_bit = dx.u64 &  0x0000080000000000;
    #index = eight_mant_bits + (ninth_mant_bit << 1);
    #F.u64 = index | 0x3fe0000000000000; // 1/2
    #index = index >> 44;
    #uxexp = ((dx.u64 & 0x7ff0000000000000) >> 52) - 1023;
    #xmantissa_bits = dx.u64 & 0x000FFFFFFFFFFFFF;
    #dxexp = (double)uxexp;
    #Y.u64 = xmantissa_bits | 0x3fe0000000000000;//1/2

    pshufd      $238 ,%xmm0,%xmm2 #xmm2 = x,x 
    pand        .L__xexp_bits_xmantissa_bits(%rip),%xmm2 #xmm2 = xexp_bits, xmantissa_bits  
    movd        %xmm2,%rcx
    shr         $44,%rcx
    adc         $0,%ecx
    mov         %ecx,%eax
    or          $0x0003fe00,%ecx
    shl         $44,%rcx
    movd        %rcx,%xmm1  #F
    por         .L__1_by_2(%rip),%xmm2  #xexp_bits, Y
    pshufd      $238 ,%xmm2,%xmm3 #xmm3 = xexp_bits,xexp_bits
    psrlq       $52,%xmm3
    psubq       .L__1023(%rip),%xmm3 
    cvtdq2pd    %xmm3,%xmm3
    lea         .L__log_F_inv(%rip), %rcx
    
    #xmm2 = Y, xmm1 = F, xmm3 = double m 
    
    #f = F - Y;
    subsd     %xmm2,%xmm1     #xmm1 = f
    
    #r = f * L__log_F_inv[index]
    mulsd    (%rcx,%rax,8),%xmm1     #xmm1 = XX,r
    movapd    %xmm1,%xmm2     #xmm2 = r
    
    #q = poly = r + r*r*(1/2 + 1/3 * r);
    movsd       .L__1_by_3(%rip), %xmm4 
    mulsd       %xmm1, %xmm4 #xmm4 = 1/3 * r
    mulsd       %xmm2, %xmm1 #xmm1 =  r  * r
    addsd       .L__1_by_2(%rip), %xmm4 #xmm4 = 1/2 + (1/3 * r)
    mulsd       %xmm4, %xmm1  #xmm1 = r*r*(1/2 + (1/3 * r))
    addsd       %xmm2, %xmm1  #xmm1 = XX,q=poly = r+r*r*(1/2 + (1/3 * r))   
    
    #logx = L__log2 * m + L__log_256_table[index] - poly;
    mulsd    .L__log2(%rip),%xmm3   #xmm3 = L__log2 * m
    lea      .L__log_256_table(%rip),%rcx
    addsd     (%rcx,%rax,8),%xmm3     #xmm3 = logx 
    subsd     %xmm1,%xmm3

.L__return_from_near_one:    
    #ylogx = ((double)y) * logx;
    mulsd    %xmm3,%xmm0     #xmm0 = ylogx

    # -----------------------------
    # compute exp( y * ln(x) ) here
    # xmm0 = XX,ylogx
    # -----------------------------
    
    ucomisd      .L__max_exp_arg(%rip), %xmm0
    ja           .L__y_is_inf
    ucomisd      .L__min_exp_arg(%rip), %xmm0
    jbe          .L__y_is_zero
    
    # x * (64/ln(2))
    movapd      %xmm0,%xmm3      #xmm3 = (xouble)x
    mulpd       .L__real_64_by_log2(%rip), %xmm3  #xmm3 = x * (64/ln(2)

    # n = int( x * (64/ln(2)) )
    cvtpd2dq    %xmm3, %xmm4  #xmm4 = (int)n
    cvtdq2pd    %xmm4, %xmm2  #xmm2 = (double)n

    # r = x - n * ln(2)/64
    mulsd       .L__real_log2_by_64(%rip),%xmm2 #xmm2 = n * ln(2)/64
    movd        %xmm4, %ecx     #ecx = n
    subsd       %xmm2, %xmm0    #xmm0 = r
    movapd      %xmm0, %xmm1    #xmm1 = r

    # q = r+r*r*(1/2 + (1/6 * r))
    movsd       .L__1_by_6(%rip), %xmm3 
    mulsd       %xmm0, %xmm3 #xmm3 = 1/6 * r
    mulsd       %xmm1, %xmm0 #xmm0 =  r  * r
    addsd       .L__1_by_2(%rip), %xmm3 #xmm3 = 1/2 + (1/6 * r)
    mulsd       %xmm3, %xmm0  #xmm0 = r*r*(1/2 + (1/6 * r))
    addsd       %xmm1, %xmm0  #xmm0 = r+r*r*(1/2 + (1/6 * r))
    
    #j = n & 0x3f
    mov         $0x3f, %rax     #rax = 0x3f
    and         %ecx, %eax      #eax = j = n & 0x3f

    # (f)*(1+q)
    lea         .L__two_to_jby64_table(%rip), %rcx    
    mulsd       (%rcx,%rax,8), %xmm0
    addsd       (%rcx,%rax,8), %xmm0
    
    .p2align 4    
    # m = (n - j) / 64        
    psrad       $6,%xmm4
    psllq       $52,%xmm4    
    paddq       %xmm0, %xmm4
    cvtsd2ss    %xmm4, %xmm0
    orps        %xmm5, %xmm0  #negate the result if x negative 
    ret
    
.p2align 4
.L__x_is_neg_or_zero_or_near_one:
    cmp $0x0,%r9d
    jg  .L__x_near_one
    cmp $0x0,%ecx
    je .L__x_is_zero

    ##else x is negative
    #check if y is int or not
    and $0x7f800000,%eax
    cmp $0x4b000000,%eax    
    jg .L__x_near_one #=>y is even int > 2p23       
    cvtss2si %xmm1,%edx    
    cvtsi2ss %edx,%xmm2
    ucomiss  %xmm1,%xmm2
    jne .L__return_qnan #pow(-,y not an integer)=qnan 
    ##here => y is int, check odd or even
    rcr $1,%edx #rotate through carry to check last bit
    jnc .L__x_near_one ##here => y is even int
    ##else y is odd int, just negate x
    movss .L__neg_zero(%rip),%xmm5  #set the sign bit

.L__x_near_one:
    # special case divert
    #cmp        $0x3F800000,%r9d #if x=1
    #je         .L__return_pos_one   
    #check for x=+1 or -1
    #pow(+1,any y) = +1, pow(-1,odd int y)=-1,pow(-1,even int y)=+1
         
    pshufd   $238,%xmm0,%xmm3 #xmm3 = double x,x
    
    # r = x - 1.0
    subsd       .L__real_one(%rip), %xmm3 # r
    
    #check if x is within the threshold
    movq        %xmm3,%rdx
    btr         $63,%rdx
    cmp         .L__real_threshold(%rip),%rdx
    jae         .L__compute_logx #proceed in normal path.. x not near one
    
    movapd      .L__real_ca42(%rip),%xmm7
    movapd      .L__real_ca31(%rip),%xmm8    
    movapd      %xmm3,%xmm1 # r
    movapd      %xmm3,%xmm6 # r
    addsd       .L__real_two(%rip), %xmm1 # 2+r
    divsd       %xmm1, %xmm6 # r/(2+r) = u/2    
    movapd      %xmm6, %xmm2 # u/2
    mulsd       %xmm3, %xmm6 # r*u/2 = correction
    addsd       %xmm2, %xmm2 # u
    movapd      %xmm2, %xmm1 # u
    mulsd       %xmm2, %xmm1 # u^2
    pshufd      $68, %xmm1, %xmm4 # u^2,u^2    
    mulpd       %xmm7,%xmm4 # u^2 * ca4,        u^2 * ca2
    mulsd       %xmm1, %xmm2 # u^3    
    addpd       %xmm8,%xmm4 # u^2 * ca4 + ca3,  u^2 * ca2 + ca1
    mulsd       %xmm1, %xmm1 # u^4
    mulsd       %xmm2, %xmm1 # u^7
    unpcklpd    %xmm1, %xmm2 # u^7, u^3
    mulpd       %xmm4, %xmm2 # u^7 * (u^2 * ca4 + ca3),  u^3 * (u^2 * ca2 + ca1)
    pshufd      $238,%xmm2,%xmm1  # XX,u^7 * (u^2 * ca4 + ca3)    
    addsd       %xmm2,%xmm1  # u^7 * (u^2 * ca4 + ca3) + u^3 * (u^2 * ca2 + ca1)
    subsd       %xmm6,%xmm2  # u^7 * (u^2 * ca4 + ca3) + u^3 * (u^2 * ca2 + ca1) - r*u/2    
    addsd       %xmm2,%xmm3  # r + u^7 * (u^2 * ca4 + ca3) + u^3 * (u^2 * ca2 + ca1) - r*u/2    
    jmp         .L__return_from_near_one

.p2align 4
.L__x_is_zero:
    mov %eax,%r8d    
    movd %xmm1,%edx
    mov $0x0,%eax
    mov $0x0,%edi
    mov $0x7f800000,%ecx
    #check if y < 0 or y>0
    cmp $0x0,%edx
    cmovl %ecx,%edx
    cmovl %ecx,%edi    
    cmovg %eax,%edx
    mov $0x80000000,%ecx
    #check if y is int or not
    and $0x7f800000,%r8d
    cmp $0x4b000000,%r8d    
    cmovg %eax,%ecx
    jg .L__or_sign_and_retvalue_and_ret1    
    cvtss2si %xmm1,%r8d
    cvtsi2ss %r8d,%xmm2
    ucomiss  %xmm1,%xmm2
    cmovne %eax,%ecx
    jne .L__or_sign_and_retvalue_and_ret1    
    rcr $1,%r8d #rotate through carry to check last bit    
    cmovnc %eax,%ecx
    jnc .L__or_sign_and_retvalue_and_ret1
    mov %r9d,%ecx
    and  $0x80000000,%ecx
.L__or_sign_and_retvalue_and_ret1:    
    or %edx,%ecx    
    movd %ecx,%xmm0
    cmp $0x0,%edi
    je .L__return
    ##else raise div by zero exception
    mov $2,%edi
    movdqa %xmm0,%xmm2
    call fname_special    
.L__return:    
    ret
  
.p2align 4   
.L__x_is_inf_or_nan:    
    cmp $0x0,%eax ##pow(+/-inf,+/-0)=1,pow(+/-nan,+/-0)=1
    je  .L__return_pos_one
    movd %xmm0,%ecx
    movd %xmm1,%edx    
    cmp $0x7f800000,%ecx
    je .L__x_is_pos_inf
    cmp $0xff800000,%ecx 
    je .L__x_is_neg_inf
    ##else x is nan return nan
    jmp .L__return_nan
    
.p2align 4
.L__x_is_pos_inf:
    #check if y < 0 or y>0
    cmp $0x0,%edx 
    jl .L__return_pos_zero ##pow(+inf,y<0)= +0
    ##else
    jmp .L__return_pos_inf ##pow(+inf,y>0)= +inf
    
.p2align 4
.L__x_is_neg_inf:
    mov %eax,%r8d    
    mov $0x0,%eax
    mov $0x7f800000,%ecx
    #check if y < 0 or y>0
    cmp $0x0,%edx
    cmovl %eax,%edx
    cmovg %ecx,%edx
    mov $0x80000000,%ecx
    #check if y is int or not
    and $0x7f800000,%r8d
    cmp $0x4b000000,%r8d    
    cmovg %eax,%ecx
    jg .L__or_sign_and_retvalue_and_ret    
    cvtss2si %xmm1,%r8d
    cvtsi2ss %r8d,%xmm2
    ucomiss  %xmm1,%xmm2
    cmovne %eax,%ecx
    jne .L__or_sign_and_retvalue_and_ret
    rcr $1,%r8d #rotate through carry to check last bit    
    cmovnc %eax,%ecx
.L__or_sign_and_retvalue_and_ret:    
    or %edx,%ecx
    movd %ecx,%xmm0
    ret
  
.p2align 4
.L__y_is_inf_or_nan:
    ja  .L__y_is_nan     

##else y is +/-inf
    #check if x is nan
    cmp        $0x7f800000,%ecx
    ##x is nan return nan
    ja .L__return_nan    
    and $0x7fffffff,%ecx ##sign of x removed    
    cmp $0x3F800000,%ecx    ##pow(+/-1,any y, even nan or inf) = +1
    je  .L__return_pos_one  
    movd %xmm1,%eax    
    bt $31,%eax ##check the sign of y
    jc  .L__y_is_neg_inf
    
##else y_is_pos_inf
    cmp $0x3f800000,%ecx
    jb  .L__return_pos_zero ##pow(|x| < 1, +inf) = +0
    ##else return +inf
    jmp .L__return_pos_inf  ##pow(|x| > 1, +inf) = +inf

.p2align 4
.L__y_is_neg_inf:
    mov $0x1,%edx
    mov $0x0,%eax           ##pow(|x| > 1, -inf) = +0
    mov $0x7f800000,%r8d    ##pow(|x| < 1, -inf) = +inf
    cmp $0x0,%ecx
    cmovne %eax,%edx        #edx indicates if exception needs to be raised
    cmp $0x3f800000,%ecx
    cmovb %r8d,%eax         #eax contains result
    movd %eax,%xmm0
    cmp $0x0,%edx
    je  .L__return1
    ##else raise exception
    movdqa %xmm0,%xmm2
    mov $2,%edi
    call fname_special
.L__return1:    
    ret

.p2align 4
.L__y_is_nan:
    cmp $0x3F800000,%r9d 
    je  .L__return_pos_one  ##pow(+1,any y, even nan or inf) = +1
    ##else return nan, check exception
    movdqa %xmm1,%xmm0      ##pow(any x other than 1, nan) = qnan
    jmp .L__return_nan

.p2align 4
.L__check_if_y_is_zero_or_pos_one:
    cmp  $0x0,%eax
    je .L__return_pos_one ##pow(any x, even nan or inf,+/-0) = +1
    movd %xmm1,%edx
    cmp  $0x3f800000,%edx
    jne  .L__y_is_not_zero_or_pos_one
    ##else    
    ret  #y=+1 so return x ##pow(x,1) = x
        
.p2align 4
.L__return_pos_one:
    mov   $0x3f800000,%edx
    movd %edx,%xmm0    
    ret

.p2align 4    
.L__return_pos_zero:
    pxor        %xmm0, %xmm0
    ret
    
   
.p2align 4
.L__return_pos_inf:
    mov         $0x7f800000,%edx
    movd        %edx, %xmm0
    ret   

.p2align 4    
.L__return_nan:
    addss        %xmm0, %xmm0
    ret

.p2align 4    
.L__y_is_zero:
    pxor        %xmm2, %xmm2
    orps        %xmm5,%xmm2
    mov         $7,%edi
    call fname_special            
    ret  

.p2align 4    
.L__y_is_inf:
    mov         $0x7f800000,%edx
    movd        %edx, %xmm2
    orps        %xmm5,%xmm2
    mov         $9,%edi
    call fname_special        
    ret  

.p2align 4
.L__return_qnan:
    mov $0x7fc00000,%edx
    movd %edx,%xmm2
    mov $3,%edi
    call fname_special    
    ret
    
.section .rodata
.align 16
.L__xexp_bits_xmantissa_bits: .octa 0x7ff0000000000000000fffffffffffff
.L__1_by_2:             .quad 0x3fe0000000000000
                        .quad 0x0000000000000000 #zeros required here
.L__1023:               .quad 0x00000000000003ff
.L__1_by_3:             .quad 0x3fd5555555555555
.L__log2:               .quad 0x3fe62e42fefa39ef
.L__max_exp_arg:        .quad 0x40562E4300000000
.L__min_exp_arg:        .quad 0xC059D1DA00000000
.L__real_log2_by_64:    .quad 0x3f862e42fefa39ef
.L__real_64_by_log2:    .quad 0x40571547652b82fe 
                        .quad 0x0000000000000000 #zeros required here
.L__1_by_6:             .quad 0x3fc5555555555555
.L__real_threshold: .quad 0x3fb0000000000000 # .0625
.L__real_one:       .quad 0x3ff0000000000000 # 1
.L__real_two:       .quad 0x4000000000000000 # 2
.L__neg_zero:       .quad 0x8000000080000000 # 2 
.align 16
.L__real_ca42:      .quad 0x3f89999999bac6d4 # 1.25000000037717509602e-02
.L__real_ca4:       .quad 0x3f3c8034c85dfff0 # 4.34887777707614552256e-04
.L__real_ca31:      .quad 0x3fb55555555554e6 # 8.33333333333317923934e-02
.L__real_ca3:       .quad 0x3f62492307f1519f # 2.23213998791944806202e-03
                    
.align 16
.L__log_F_inv:
        .quad 0x4000000000000000                    
        .quad 0x3fffe01fe01fe020
        .quad 0x3fffc07f01fc07f0
        .quad 0x3fffa11caa01fa12
        .quad 0x3fff81f81f81f820
        .quad 0x3fff6310aca0dbb5
        .quad 0x3fff44659e4a4271
        .quad 0x3fff25f644230ab5
        .quad 0x3fff07c1f07c1f08
        .quad 0x3ffee9c7f8458e02
        .quad 0x3ffecc07b301ecc0
        .quad 0x3ffeae807aba01eb
        .quad 0x3ffe9131abf0b767
        .quad 0x3ffe741aa59750e4
        .quad 0x3ffe573ac901e574
        .quad 0x3ffe3a9179dc1a73
        .quad 0x3ffe1e1e1e1e1e1e
        .quad 0x3ffe01e01e01e01e
        .quad 0x3ffde5d6e3f8868a
        .quad 0x3ffdca01dca01dca
        .quad 0x3ffdae6076b981db
        .quad 0x3ffd92f2231e7f8a
        .quad 0x3ffd77b654b82c34
        .quad 0x3ffd5cac807572b2
        .quad 0x3ffd41d41d41d41d
        .quad 0x3ffd272ca3fc5b1a
        .quad 0x3ffd0cb58f6ec074
        .quad 0x3ffcf26e5c44bfc6
        .quad 0x3ffcd85689039b0b
        .quad 0x3ffcbe6d9601cbe7
        .quad 0x3ffca4b3055ee191
        .quad 0x3ffc8b265afb8a42
        .quad 0x3ffc71c71c71c71c
        .quad 0x3ffc5894d10d4986
        .quad 0x3ffc3f8f01c3f8f0
        .quad 0x3ffc26b5392ea01c
        .quad 0x3ffc0e070381c0e0
        .quad 0x3ffbf583ee868d8b
        .quad 0x3ffbdd2b899406f7
        .quad 0x3ffbc4fd65883e7b
        .quad 0x3ffbacf914c1bad0
        .quad 0x3ffb951e2b18ff23
        .quad 0x3ffb7d6c3dda338b
        .quad 0x3ffb65e2e3beee05
        .quad 0x3ffb4e81b4e81b4f
        .quad 0x3ffb37484ad806ce
        .quad 0x3ffb2036406c80d9
        .quad 0x3ffb094b31d922a4
        .quad 0x3ffaf286bca1af28
        .quad 0x3ffadbe87f94905e
        .quad 0x3ffac5701ac5701b
        .quad 0x3ffaaf1d2f87ebfd
        .quad 0x3ffa98ef606a63be
        .quad 0x3ffa82e65130e159
        .quad 0x3ffa6d01a6d01a6d
        .quad 0x3ffa574107688a4a
        .quad 0x3ffa41a41a41a41a
        .quad 0x3ffa2c2a87c51ca0
        .quad 0x3ffa16d3f97a4b02
        .quad 0x3ffa01a01a01a01a
        .quad 0x3ff9ec8e951033d9
        .quad 0x3ff9d79f176b682d
        .quad 0x3ff9c2d14ee4a102
        .quad 0x3ff9ae24ea5510da
        .quad 0x3ff999999999999a
        .quad 0x3ff9852f0d8ec0ff
        .quad 0x3ff970e4f80cb872
        .quad 0x3ff95cbb0be377ae
        .quad 0x3ff948b0fcd6e9e0
        .quad 0x3ff934c67f9b2ce6
        .quad 0x3ff920fb49d0e229
        .quad 0x3ff90d4f120190d5
        .quad 0x3ff8f9c18f9c18fa
        .quad 0x3ff8e6527af1373f
        .quad 0x3ff8d3018d3018d3
        .quad 0x3ff8bfce8062ff3a
        .quad 0x3ff8acb90f6bf3aa
        .quad 0x3ff899c0f601899c
        .quad 0x3ff886e5f0abb04a
        .quad 0x3ff87427bcc092b9
        .quad 0x3ff8618618618618
        .quad 0x3ff84f00c2780614
        .quad 0x3ff83c977ab2bedd
        .quad 0x3ff82a4a0182a4a0
        .quad 0x3ff8181818181818
        .quad 0x3ff8060180601806
        .quad 0x3ff7f405fd017f40
        .quad 0x3ff7e225515a4f1d
        .quad 0x3ff7d05f417d05f4
        .quad 0x3ff7beb3922e017c
        .quad 0x3ff7ad2208e0ecc3
        .quad 0x3ff79baa6bb6398b
        .quad 0x3ff78a4c8178a4c8
        .quad 0x3ff77908119ac60d
        .quad 0x3ff767dce434a9b1
        .quad 0x3ff756cac201756d
        .quad 0x3ff745d1745d1746
        .quad 0x3ff734f0c541fe8d
        .quad 0x3ff724287f46debc
        .quad 0x3ff713786d9c7c09
        .quad 0x3ff702e05c0b8170
        .quad 0x3ff6f26016f26017
        .quad 0x3ff6e1f76b4337c7
        .quad 0x3ff6d1a62681c861
        .quad 0x3ff6c16c16c16c17
        .quad 0x3ff6b1490aa31a3d
        .quad 0x3ff6a13cd1537290
        .quad 0x3ff691473a88d0c0
        .quad 0x3ff6816816816817
        .quad 0x3ff6719f3601671a
        .quad 0x3ff661ec6a5122f9
        .quad 0x3ff6524f853b4aa3
        .quad 0x3ff642c8590b2164
        .quad 0x3ff63356b88ac0de
        .quad 0x3ff623fa77016240
        .quad 0x3ff614b36831ae94
        .quad 0x3ff6058160581606
        .quad 0x3ff5f66434292dfc
        .quad 0x3ff5e75bb8d015e7
        .quad 0x3ff5d867c3ece2a5
        .quad 0x3ff5c9882b931057
        .quad 0x3ff5babcc647fa91
        .quad 0x3ff5ac056b015ac0
        .quad 0x3ff59d61f123ccaa
        .quad 0x3ff58ed2308158ed
        .quad 0x3ff5805601580560
        .quad 0x3ff571ed3c506b3a
        .quad 0x3ff56397ba7c52e2
        .quad 0x3ff5555555555555
        .quad 0x3ff54725e6bb82fe
        .quad 0x3ff5390948f40feb
        .quad 0x3ff52aff56a8054b
        .quad 0x3ff51d07eae2f815
        .quad 0x3ff50f22e111c4c5
        .quad 0x3ff5015015015015
        .quad 0x3ff4f38f62dd4c9b
        .quad 0x3ff4e5e0a72f0539
        .quad 0x3ff4d843bedc2c4c
        .quad 0x3ff4cab88725af6e
        .quad 0x3ff4bd3edda68fe1
        .quad 0x3ff4afd6a052bf5b
        .quad 0x3ff4a27fad76014a
        .quad 0x3ff49539e3b2d067
        .quad 0x3ff4880522014880
        .quad 0x3ff47ae147ae147b
        .quad 0x3ff46dce34596066
        .quad 0x3ff460cbc7f5cf9a
        .quad 0x3ff453d9e2c776ca
        .quad 0x3ff446f86562d9fb
        .quad 0x3ff43a2730abee4d
        .quad 0x3ff42d6625d51f87
        .quad 0x3ff420b5265e5951
        .quad 0x3ff4141414141414
        .quad 0x3ff40782d10e6566
        .quad 0x3ff3fb013fb013fb
        .quad 0x3ff3ee8f42a5af07
        .quad 0x3ff3e22cbce4a902
        .quad 0x3ff3d5d991aa75c6
        .quad 0x3ff3c995a47babe7
        .quad 0x3ff3bd60d9232955
        .quad 0x3ff3b13b13b13b14
        .quad 0x3ff3a524387ac822
        .quad 0x3ff3991c2c187f63
        .quad 0x3ff38d22d366088e
        .quad 0x3ff3813813813814
        .quad 0x3ff3755bd1c945ee
        .quad 0x3ff3698df3de0748
        .quad 0x3ff35dce5f9f2af8
        .quad 0x3ff3521cfb2b78c1
        .quad 0x3ff34679ace01346
        .quad 0x3ff33ae45b57bcb2
        .quad 0x3ff32f5ced6a1dfa
        .quad 0x3ff323e34a2b10bf
        .quad 0x3ff3187758e9ebb6
        .quad 0x3ff30d190130d190
        .quad 0x3ff301c82ac40260
        .quad 0x3ff2f684bda12f68
        .quad 0x3ff2eb4ea1fed14b
        .quad 0x3ff2e025c04b8097
        .quad 0x3ff2d50a012d50a0
        .quad 0x3ff2c9fb4d812ca0
        .quad 0x3ff2bef98e5a3711
        .quad 0x3ff2b404ad012b40
        .quad 0x3ff2a91c92f3c105
        .quad 0x3ff29e4129e4129e
        .quad 0x3ff293725bb804a5
        .quad 0x3ff288b01288b013
        .quad 0x3ff27dfa38a1ce4d
        .quad 0x3ff27350b8812735
        .quad 0x3ff268b37cd60127
        .quad 0x3ff25e22708092f1
        .quad 0x3ff2539d7e9177b2
        .quad 0x3ff2492492492492
        .quad 0x3ff23eb79717605b
        .quad 0x3ff23456789abcdf
        .quad 0x3ff22a0122a0122a
        .quad 0x3ff21fb78121fb78
        .quad 0x3ff21579804855e6
        .quad 0x3ff20b470c67c0d9
        .quad 0x3ff2012012012012
        .quad 0x3ff1f7047dc11f70
        .quad 0x3ff1ecf43c7fb84c
        .quad 0x3ff1e2ef3b3fb874
        .quad 0x3ff1d8f5672e4abd
        .quad 0x3ff1cf06ada2811d
        .quad 0x3ff1c522fc1ce059
        .quad 0x3ff1bb4a4046ed29
        .quad 0x3ff1b17c67f2bae3
        .quad 0x3ff1a7b9611a7b96
        .quad 0x3ff19e0119e0119e
        .quad 0x3ff19453808ca29c
        .quad 0x3ff18ab083902bdb
        .quad 0x3ff1811811811812
        .quad 0x3ff1778a191bd684
        .quad 0x3ff16e0689427379
        .quad 0x3ff1648d50fc3201
        .quad 0x3ff15b1e5f75270d
        .quad 0x3ff151b9a3fdd5c9
        .quad 0x3ff1485f0e0acd3b
        .quad 0x3ff13f0e8d344724
        .quad 0x3ff135c81135c811
        .quad 0x3ff12c8b89edc0ac
        .quad 0x3ff12358e75d3033
        .quad 0x3ff11a3019a74826
        .quad 0x3ff1111111111111
        .quad 0x3ff107fbbe011080
        .quad 0x3ff0fef010fef011
        .quad 0x3ff0f5edfab325a2
        .quad 0x3ff0ecf56be69c90
        .quad 0x3ff0e40655826011
        .quad 0x3ff0db20a88f4696
        .quad 0x3ff0d24456359e3a
        .quad 0x3ff0c9714fbcda3b
        .quad 0x3ff0c0a7868b4171
        .quad 0x3ff0b7e6ec259dc8
        .quad 0x3ff0af2f722eecb5
        .quad 0x3ff0a6810a6810a7
        .quad 0x3ff09ddba6af8360
        .quad 0x3ff0953f39010954
        .quad 0x3ff08cabb37565e2
        .quad 0x3ff0842108421084
        .quad 0x3ff07b9f29b8eae2
        .quad 0x3ff073260a47f7c6
        .quad 0x3ff06ab59c7912fb
        .quad 0x3ff0624dd2f1a9fc
        .quad 0x3ff059eea0727586
        .quad 0x3ff05197f7d73404
        .quad 0x3ff04949cc1664c5
        .quad 0x3ff0410410410410
        .quad 0x3ff038c6b78247fc
        .quad 0x3ff03091b51f5e1a
        .quad 0x3ff02864fc7729e9
        .quad 0x3ff0204081020408
        .quad 0x3ff0182436517a37
        .quad 0x3ff0101010101010
        .quad 0x3ff0080402010080
        .quad 0x3ff0000000000000

.align 16
.L__log_256_table:
        .quad 0x0000000000000000
        .quad 0x3f6ff00aa2b10bc0
        .quad 0x3f7fe02a6b106789
        .quad 0x3f87dc475f810a77
        .quad 0x3f8fc0a8b0fc03e4
        .quad 0x3f93cea44346a575
        .quad 0x3f97b91b07d5b11b
        .quad 0x3f9b9fc027af9198
        .quad 0x3f9f829b0e783300
        .quad 0x3fa1b0d98923d980
        .quad 0x3fa39e87b9febd60
        .quad 0x3fa58a5bafc8e4d5
        .quad 0x3fa77458f632dcfc
        .quad 0x3fa95c830ec8e3eb
        .quad 0x3fab42dd711971bf
        .quad 0x3fad276b8adb0b52
        .quad 0x3faf0a30c01162a6
        .quad 0x3fb075983598e471
        .quad 0x3fb16536eea37ae1
        .quad 0x3fb253f62f0a1417
        .quad 0x3fb341d7961bd1d1
        .quad 0x3fb42edcbea646f0
        .quad 0x3fb51b073f06183f
        .quad 0x3fb60658a93750c4
        .quad 0x3fb6f0d28ae56b4c
        .quad 0x3fb7da766d7b12cd
        .quad 0x3fb8c345d6319b21
        .quad 0x3fb9ab42462033ad
        .quad 0x3fba926d3a4ad563
        .quad 0x3fbb78c82bb0eda1
        .quad 0x3fbc5e548f5bc743
        .quad 0x3fbd4313d66cb35d
        .quad 0x3fbe27076e2af2e6
        .quad 0x3fbf0a30c01162a6
        .quad 0x3fbfec9131dbeabb
        .quad 0x3fc0671512ca596e
        .quad 0x3fc0d77e7cd08e59
        .quad 0x3fc14785846742ac
        .quad 0x3fc1b72ad52f67a0
        .quad 0x3fc2266f190a5acb
        .quad 0x3fc29552f81ff523
        .quad 0x3fc303d718e47fd3
        .quad 0x3fc371fc201e8f74
        .quad 0x3fc3dfc2b0ecc62a
        .quad 0x3fc44d2b6ccb7d1e
        .quad 0x3fc4ba36f39a55e5
        .quad 0x3fc526e5e3a1b438
        .quad 0x3fc59338d9982086
        .quad 0x3fc5ff3070a793d4
        .quad 0x3fc66acd4272ad51
        .quad 0x3fc6d60fe719d21d
        .quad 0x3fc740f8f54037a5
        .quad 0x3fc7ab890210d909
        .quad 0x3fc815c0a14357eb
        .quad 0x3fc87fa06520c911
        .quad 0x3fc8e928de886d41
        .quad 0x3fc9525a9cf456b4
        .quad 0x3fc9bb362e7dfb83
        .quad 0x3fca23bc1fe2b563
        .quad 0x3fca8becfc882f19
        .quad 0x3fcaf3c94e80bff3
        .quad 0x3fcb5b519e8fb5a4
        .quad 0x3fcbc286742d8cd6
        .quad 0x3fcc2968558c18c1
        .quad 0x3fcc8ff7c79a9a22
        .quad 0x3fccf6354e09c5dc
        .quad 0x3fcd5c216b4fbb91
        .quad 0x3fcdc1bca0abec7d
        .quad 0x3fce27076e2af2e6
        .quad 0x3fce8c0252aa5a60
        .quad 0x3fcef0adcbdc5936
        .quad 0x3fcf550a564b7b37
        .quad 0x3fcfb9186d5e3e2b
        .quad 0x3fd00e6c45ad501d
        .quad 0x3fd0402594b4d041
        .quad 0x3fd071b85fcd590d
        .quad 0x3fd0a324e27390e3
        .quad 0x3fd0d46b579ab74b
        .quad 0x3fd1058bf9ae4ad5
        .quad 0x3fd136870293a8b0
        .quad 0x3fd1675cababa60e
        .quad 0x3fd1980d2dd4236f
        .quad 0x3fd1c898c16999fb
        .quad 0x3fd1f8ff9e48a2f3
        .quad 0x3fd22941fbcf7966
        .quad 0x3fd2596010df763a
        .quad 0x3fd2895a13de86a3
        .quad 0x3fd2b9303ab89d25
        .quad 0x3fd2e8e2bae11d31
        .quad 0x3fd31871c9544185
        .quad 0x3fd347dd9a987d55
        .quad 0x3fd3772662bfd85b
        .quad 0x3fd3a64c556945ea
        .quad 0x3fd3d54fa5c1f710
        .quad 0x3fd404308686a7e4
        .quad 0x3fd432ef2a04e814
        .quad 0x3fd4618bc21c5ec2
        .quad 0x3fd49006804009d1
        .quad 0x3fd4be5f957778a1
        .quad 0x3fd4ec973260026a
        .quad 0x3fd51aad872df82d
        .quad 0x3fd548a2c3add263
        .quad 0x3fd5767717455a6c
        .quad 0x3fd5a42ab0f4cfe2
        .quad 0x3fd5d1bdbf5809ca
        .quad 0x3fd5ff3070a793d4
        .quad 0x3fd62c82f2b9c795
        .quad 0x3fd659b57303e1f3
        .quad 0x3fd686c81e9b14af
        .quad 0x3fd6b3bb2235943e
        .quad 0x3fd6e08eaa2ba1e4
        .quad 0x3fd70d42e2789236
        .quad 0x3fd739d7f6bbd007
        .quad 0x3fd7664e1239dbcf
        .quad 0x3fd792a55fdd47a2
        .quad 0x3fd7bede0a37afc0
        .quad 0x3fd7eaf83b82afc3
        .quad 0x3fd816f41da0d496
        .quad 0x3fd842d1da1e8b17
        .quad 0x3fd86e919a330ba0
        .quad 0x3fd89a3386c1425b
        .quad 0x3fd8c5b7c858b48b
        .quad 0x3fd8f11e873662c7
        .quad 0x3fd91c67eb45a83e
        .quad 0x3fd947941c2116fb
        .quad 0x3fd972a341135158
        .quad 0x3fd99d958117e08b
        .quad 0x3fd9c86b02dc0863
        .quad 0x3fd9f323ecbf984c
        .quad 0x3fda1dc064d5b995
        .quad 0x3fda484090e5bb0a
        .quad 0x3fda72a4966bd9ea
        .quad 0x3fda9cec9a9a084a
        .quad 0x3fdac718c258b0e4
        .quad 0x3fdaf1293247786b
        .quad 0x3fdb1b1e0ebdfc5b
        .quad 0x3fdb44f77bcc8f63
        .quad 0x3fdb6eb59d3cf35e
        .quad 0x3fdb9858969310fb
        .quad 0x3fdbc1e08b0dad0a
        .quad 0x3fdbeb4d9da71b7c
        .quad 0x3fdc149ff115f027
        .quad 0x3fdc3dd7a7cdad4d
        .quad 0x3fdc66f4e3ff6ff8
        .quad 0x3fdc8ff7c79a9a22
        .quad 0x3fdcb8e0744d7aca
        .quad 0x3fdce1af0b85f3eb
        .quad 0x3fdd0a63ae721e64
        .quad 0x3fdd32fe7e00ebd5
        .quad 0x3fdd5b7f9ae2c684
        .quad 0x3fdd83e7258a2f3e
        .quad 0x3fddac353e2c5954
        .quad 0x3fddd46a04c1c4a1
        .quad 0x3fddfc859906d5b5
        .quad 0x3fde24881a7c6c26
        .quad 0x3fde4c71a8687704
        .quad 0x3fde744261d68788
        .quad 0x3fde9bfa659861f5
        .quad 0x3fdec399d2468cc0
        .quad 0x3fdeeb20c640ddf4
        .quad 0x3fdf128f5faf06ed
        .quad 0x3fdf39e5bc811e5c
        .quad 0x3fdf6123fa7028ac
        .quad 0x3fdf884a36fe9ec2
        .quad 0x3fdfaf588f78f31f
        .quad 0x3fdfd64f20f61572
        .quad 0x3fdffd2e0857f498
        .quad 0x3fe011fab125ff8a
        .quad 0x3fe02552a5a5d0ff
        .quad 0x3fe0389eefce633b
        .quad 0x3fe04bdf9da926d2
        .quad 0x3fe05f14bd26459c
        .quad 0x3fe0723e5c1cdf40
        .quad 0x3fe0855c884b450e
        .quad 0x3fe0986f4f573521
        .quad 0x3fe0ab76bece14d2
        .quad 0x3fe0be72e4252a83
        .quad 0x3fe0d163ccb9d6b8
        .quad 0x3fe0e44985d1cc8c
        .quad 0x3fe0f7241c9b497d
        .quad 0x3fe109f39e2d4c97
        .quad 0x3fe11cb81787ccf8
        .quad 0x3fe12f719593efbc
        .quad 0x3fe1422025243d45
        .quad 0x3fe154c3d2f4d5ea
        .quad 0x3fe1675cababa60e
        .quad 0x3fe179eabbd899a1
        .quad 0x3fe18c6e0ff5cf06
        .quad 0x3fe19ee6b467c96f
        .quad 0x3fe1b154b57da29f
        .quad 0x3fe1c3b81f713c25
        .quad 0x3fe1d610fe677003
        .quad 0x3fe1e85f5e7040d0
        .quad 0x3fe1faa34b87094c
        .quad 0x3fe20cdcd192ab6e
        .quad 0x3fe21f0bfc65beec
        .quad 0x3fe23130d7bebf43
        .quad 0x3fe2434b6f483934
        .quad 0x3fe2555bce98f7cb
        .quad 0x3fe26762013430e0
        .quad 0x3fe2795e1289b11b
        .quad 0x3fe28b500df60783
        .quad 0x3fe29d37fec2b08b
        .quad 0x3fe2af15f02640ad
        .quad 0x3fe2c0e9ed448e8c
        .quad 0x3fe2d2b4012edc9e
        .quad 0x3fe2e47436e40268
        .quad 0x3fe2f62a99509546
        .quad 0x3fe307d7334f10be
        .quad 0x3fe3197a0fa7fe6a
        .quad 0x3fe32b1339121d71
        .quad 0x3fe33ca2ba328995
        .quad 0x3fe34e289d9ce1d3
        .quad 0x3fe35fa4edd36ea0
        .quad 0x3fe37117b54747b6
        .quad 0x3fe38280fe58797f
        .quad 0x3fe393e0d3562a1a
        .quad 0x3fe3a5373e7ebdfa
        .quad 0x3fe3b68449fffc23
        .quad 0x3fe3c7c7fff73206
        .quad 0x3fe3d9026a7156fb
        .quad 0x3fe3ea33936b2f5c
        .quad 0x3fe3fb5b84d16f42
        .quad 0x3fe40c7a4880dce9
        .quad 0x3fe41d8fe84672ae
        .quad 0x3fe42e9c6ddf80bf
        .quad 0x3fe43f9fe2f9ce67
        .quad 0x3fe4509a5133bb0a
        .quad 0x3fe4618bc21c5ec2
        .quad 0x3fe472743f33aaad
        .quad 0x3fe48353d1ea88df
        .quad 0x3fe4942a83a2fc07
        .quad 0x3fe4a4f85db03ebb
        .quad 0x3fe4b5bd6956e274
        .quad 0x3fe4c679afccee3a
        .quad 0x3fe4d72d3a39fd00
        .quad 0x3fe4e7d811b75bb1
        .quad 0x3fe4f87a3f5026e9
        .quad 0x3fe50913cc01686b
        .quad 0x3fe519a4c0ba3446
        .quad 0x3fe52a2d265bc5ab
        .quad 0x3fe53aad05b99b7d
        .quad 0x3fe54b2467999498
        .quad 0x3fe55b9354b40bcd
        .quad 0x3fe56bf9d5b3f399
        .quad 0x3fe57c57f336f191
        .quad 0x3fe58cadb5cd7989
        .quad 0x3fe59cfb25fae87e
        .quad 0x3fe5ad404c359f2d
        .quad 0x3fe5bd7d30e71c73
        .quad 0x3fe5cdb1dc6c1765
        .quad 0x3fe5ddde57149923
        .quad 0x3fe5ee02a9241675
        .quad 0x3fe5fe1edad18919
        .quad 0x3fe60e32f44788d9
        .quad 0x3fe61e3efda46467
        .quad 0x3fe62e42fefa39ef
                    
