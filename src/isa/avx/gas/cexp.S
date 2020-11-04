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

#
# An implementation of the complex exp function.
#
# Prototype:
#
#   double complex cexp(double complex x);
#
#   Computes cexp
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.
#   Based on the sincos and exp implementation.
#
#
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(cexp)
#define fname_special1 _sincos_special@PLT
#define fname_special2 _exp_special@PLT
    
# define local variable storage offsets
.equ    p_temp0,    0x30                              # temporary for get/put bits operation
.equ    p_temp1,    0x40                              # temporary for get/put bits operation
.equ    p_temp2,    0x50							  # temporary for get/put bits operation 
.equ    r,          0x60                              # pointer to r for amd_remainder_piby2
.equ    rr,         0x70                              # pointer to rr for amd_remainder_piby2
.equ    region,     0x80                              # pointer to region for amd_remainder_piby2
.equ    stack_size, 0xa8

.globl fname
.type  fname,@function

#include "weak_macros.h"

fname:
    sub     $stack_size, %rsp

#start the exp calculation
.Lexp:
    movsd       %xmm0, p_temp2(%rsp)				  # xmm0 for real part
    movsd       %xmm1, p_temp0(%rsp)                  # save image part for sincos usage
    mov         .L__real_inf(%rip), %rax
    mov         p_temp2(%rsp), %rdx
    and         %rdx, %rax
    cmp         .L__real_inf(%rip), %rax
    je          .L__x_is_inf_or_nan
    movsd       .L__real_64_by_log2(%rip), %xmm3

    # x <= 1024*ln(2)
    # x > -1075*ln(2)
    comisd      .L__real_pthreshold(%rip), %xmm0
    ja          .L__y_is_inf

    comisd      .L__real_mthreshold(%rip), %xmm0
    jbe         .L__y_is_zero

    # x * (64/ln(2))
    mulsd       %xmm0, %xmm3

    # n = int( x * (64/ln(2)) )
    cvtpd2dq    %xmm3, %xmm4
    lea         .L__two_to_jby64_head_table(%rip), %r10
    lea         .L__two_to_jby64_tail_table(%rip), %r11
    cvtdq2pd    %xmm4, %xmm1

    # r1 = x - n * ln(2)/64 head
    movsd       .L__real_log2_by_64_head(%rip), %xmm2
    mulsd       %xmm1, %xmm2
    movd        %xmm4, %ecx
    mov         $0x3f, %rax
    and         %ecx, %eax
    subsd       %xmm2, %xmm0

    # r2 = - n * ln(2)/64 tail
    mulsd       .L__real_log2_by_64_tail(%rip), %xmm1
    movsd       %xmm0, %xmm2

    # m = (n - j) / 64
    sub         %eax, %ecx
    sar         $6, %ecx

    # r1+r2
    addsd       %xmm1, %xmm2
    movsd       %xmm2, %xmm1

    # q
    movsd       .L__real_1_by_720(%rip), %xmm3
    movsd       .L__real_1_by_6(%rip), %xmm0
    mulsd       %xmm2, %xmm3
    mulsd       %xmm2, %xmm0
    mulsd       %xmm2, %xmm1 
    movsd       %xmm1, %xmm4
    addsd       .L__real_1_by_120(%rip), %xmm3
    addsd       .L__real_1_by_2(%rip), %xmm0
    mulsd       %xmm1, %xmm4
    mulsd       %xmm2, %xmm3
    mulsd       %xmm1, %xmm0
    addsd       .L__real_1_by_24(%rip), %xmm3
    addsd       %xmm2, %xmm0
    mulsd       %xmm4, %xmm3
    addsd       %xmm3, %xmm0

    # deal with denormal results
    xor         %r9d, %r9d
    cmp         .L__denormal_threshold(%rip), %ecx
    cmovle      %ecx, %r9d

    add         $1023, %rcx
    shl         $52, %rcx

    lea         .L__two_to_jby64_table(%rip), %r8
    mulsd       (%r8,%rax,8), %xmm0

    cmp         .L__real_inf(%rip), %rcx

    # (f1+f2)*(1+q)
    addsd       (%r11,%rax,8), %xmm0
    addsd       (%r10,%rax,8), %xmm0

    je          .L__process_almost_inf

    test        %r9d, %r9d
    mov         %rcx, p_temp2(%rsp)
    jnz         .L__process_denormal
    mulsd       p_temp2(%rsp), %xmm0
    jmp         .L__SinCos

.p2align 4,,15
.L__process_almost_inf:
    # here e^r < 1 and m = 128, we cannot use mulsd
    orpd        .L__enable_almost_inf(%rip), %xmm0
    jmp         .L__SinCos

.p2align 4,,15
.L__process_denormal:
    mov         %r9d, %ecx
    xor         %r11d, %r11d
    comisd      .L__real_one(%rip), %xmm0
    cmovae      %ecx, %r11d
    cmp         .L__denormal_threshold(%rip), %r11d
    jne         .L__process_true_denormal  

    # here e^r >= 1 m = -1022, result is normal
    mulsd       p_temp2(%rsp), %xmm0
    jmp         .L__SinCos

.p2align 4,,15
.L__process_true_denormal:
    # here ( e^r < 1 and m = -1022 ) or m <= -1023
    xor         %r8, %r8
    cmp         .L__denormal_tiny_threshold(%rip), %rdx
    mov         $1, %r9
    jg          .L__process_denormal_tiny
    add         $1074, %ecx
    cmovs       %r8, %rcx
    shl         %cl, %r9
    mov         %r9, %rcx

    mov         %rcx, p_temp2(%rsp)
    mulsd       p_temp2(%rsp), %xmm0
    jmp         .L__SinCos        

.p2align 4,,15
.L__process_denormal_tiny:
    movsd       .L__real_smallest_denormal(%rip), %xmm0
    jmp         .L__SinCos

.p2align 4,,15
.L__y_is_zero:

    movsd       .L__real_zero(%rip), %xmm1
    movd        %rdx, %xmm0
    mov         .L__flag_y_zero(%rip), %edi

    call        fname_special2
    movsd       .L__real_zero(%rip), %xmm0    
    jmp         .L__SinCos                    # multiplication is needed even 0, for preserving sign          

.p2align 4,,15
.L__y_is_inf:

    movsd       .L__real_inf(%rip), %xmm1
    movd        %rdx, %xmm0
    mov         .L__flag_y_inf(%rip), %edi

    call        fname_special2
    jmp         .L__SinCos      

.p2align 4,,15
.L__x_is_inf_or_nan:

    cmp         .L__real_inf(%rip), %rdx
    je          .L__SinCos

    cmp         .L__real_ninf(%rip), %rdx
    movsd       .L__real_zero(%rip), %xmm0
    je          .L__SinCos    

    mov         .L__real_qnanbit(%rip), %r9
    and         %rdx, %r9
    movsd       .L__real_7ff8000000000000(%rip), %xmm0    
    jnz         .L__SinCos

	movd        %rdx, %xmm0
    or          .L__real_qnanbit(%rip), %rdx
    movd        %rdx, %xmm1
    mov         .L__flag_x_nan(%rip), %edi
    call        fname_special2
;    jmp         .L__SinCos    

.L__SinCos:
##############################################################
# start the sincos calculation
# GET_BITS_DP64(x, ux);
# get the input value to an integer register.
	movsd   %xmm0, p_temp2(%rsp)                      # save xmm0(exp(x)) to p_temp2
	movsd   p_temp0(%rsp), %xmm0
    mov     p_temp0(%rsp), %rcx                       # rcx is ux

##  if NaN or inf
    mov     $0x07ff0000000000000,%rax
    mov     %rax,%r10
    and     %rcx,%r10
    cmp     %rax,%r10
    jz      .Lsincos_naninf

#  ax = (ux & ~SIGNBIT_DP64);
    mov     $0x07fffffffffffffff,%r10
    and     %rcx,%r10                # r10 is ax

##  if (ax <= 0x3fe921fb54442d18) /* abs(x) <= pi/4 */
    mov     $0x03fe921fb54442d18,%rax
    cmp     %rax,%r10
    jg      .Lsincos_reduce

##      if (ax < 0x3f20000000000000) /* abs(x) < 2.0^(-13) */
    mov     $0x03f20000000000000,%rax
    cmp     %rax,%r10
    jge     .Lsincos_small

##          if (ax < 0x3e40000000000000) /* abs(x) < 2.0^(-27) */
    mov     $0x03e40000000000000,%rax
    cmp     %rax,%r10
    jge     .Lsincos_smaller

                            # sin = x;
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # cos = 1.0;
    jmp     .Lsincos_cleanup   

##          else
.align 32
.Lsincos_smaller:
#              sin = x - x^3 * 0.1666666666666666666;
#              cos = 1.0 - x*x*0.5;

    movsd   %xmm0,%xmm2
    movsd   .L__real_3fc5555555555555(%rip),%xmm4     # 0.1666666666666666666
    mulsd   %xmm2,%xmm2                               # x^2
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # 1.0
    movsd   %xmm2,%xmm3                               # copy of x^2

    mulsd   %xmm0,%xmm2                               # x^3
    mulsd   .L__real_3fe0000000000000(%rip),%xmm3     # 0.5 * x^2
    mulsd   %xmm4,%xmm2                               # x^3 * 0.1666666666666666666
    subsd   %xmm2,%xmm0                               # x - x^3 * 0.1666666666666666666, sin
    subsd   %xmm3,%xmm1                               # 1 - 0.5 * x^2, cos

    jmp     .Lsincos_cleanup                


##      else

.align 16
.Lsincos_small:
#          sin = sin_piby4(x, 0.0);
    movsd   .L__real_3fe0000000000000(%rip),%xmm5     # .5

#  x2 = r * r;
    movsd   %xmm0,%xmm2
    mulsd   %xmm0,%xmm2                               # x2
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# region 0 or 2     - do a sin calculation
#  zs = (s2 + x2 * (s3 + x2 * (s4 + x2 * (s5 + x2 * s6))));

    movlhps %xmm2,%xmm2
    movapd  .Lsincosarray+0x50(%rip),%xmm3            # s6
    movapd  %xmm2,%xmm1                               # move for x4
    movdqa  .Lsincosarray+0x20(%rip),%xmm5            # s3
    mulpd   %xmm2,%xmm3                               # x2s6
    addpd   .Lsincosarray+0x40(%rip),%xmm3            # s5+x2s6
    mulpd   %xmm2,%xmm5                               # x2s3
    movapd  %xmm4,p_temp0(%rsp)                       # rr move to to memory
    mulpd   %xmm2,%xmm1                               # x4
    mulpd   %xmm2,%xmm3                               # x2(s5+x2s6)
    addpd   .Lsincosarray+0x10(%rip),%xmm5            # s2+x2s3
    movapd  %xmm1,%xmm4                               # move for x6
    addpd   .Lsincosarray+0x30(%rip),%xmm3            # s4 + x2(s5+x2s6)
    mulpd   %xmm2,%xmm5                               # x2(s2+x2s3)
    mulpd   %xmm2,%xmm4                               # x6
    addpd   .Lsincosarray(%rip),%xmm5                 # s1+x2(s2+x2s3)
    mulpd   %xmm4,%xmm3                               # x6(s4 + x2(s5+x2s6))

    movsd   %xmm2,%xmm4                               # xmm4 = x2 for 0.5x2 for cos
                                                      # xmm2 contains x2 for x3 for sin
    addpd   %xmm5,%xmm3                               # zs in lower and zc upper

    mulsd   %xmm0,%xmm2                               # xmm2=x3 for sin

    movhlps %xmm3,%xmm5                               # Copy z, xmm5 = cos , xmm3 = sin

    mulsd   .L__real_3fe0000000000000(%rip),%xmm4     # xmm4=r=0.5*x2 for cos term
    mulsd   %xmm2,%xmm3                               # sin *x3
    mulsd   %xmm1,%xmm5                               # cos *x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm2     # 1.0
    subsd   %xmm4,%xmm2                               # t=1.0-r
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # 1.0
    subsd   %xmm2,%xmm1                               # 1 - t
    subsd   %xmm4,%xmm1                               # (1-t) -r
    addsd   %xmm5,%xmm1                               # ((1-t) -r) + cos
    addsd   %xmm3,%xmm0                               # xmm0= sin+x, final sin term
    addsd   %xmm2,%xmm1                               # xmm1 = t +{ ((1-t) -r) + cos}, final cos term

    jmp     .Lsincos_cleanup
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsincos_reduce:
# change rdx to rcx and r8 to r9
# rcx= ux, r10 = ax
# %r9,%rax are free

#  xneg = (ax != ux);
    xorpd   %xmm2,%xmm2                               # zeroed out for later use
    cmp     %r10,%rcx
    mov     $0,%r11d

##  if (xneg) x = -x;
    jz      .LPositive
    mov     $1,%r11d
    subsd   %xmm0,%xmm2
    movsd   %xmm2,%xmm0

# rcx= ux, r10 = ax, r11= Sign
# %r9,%rax are free
# change rdx to rcx and r8 to r9

.align 16
.LPositive:
##  if (x < 5.0e5)
    cmp     .L__real_411E848000000000(%rip),%r10
    jae     .Lsincos_reduce_precise

# reduce  the argument to be in a range from -pi/4 to +pi/4
# by subtracting multiples of pi/2
    movsd   %xmm0,%xmm2
    movsd   .L__real_3fe45f306dc9c883(%rip),%xmm3     # twobypi
    movsd   %xmm0,%xmm4
    movsd   .L__real_3fe0000000000000(%rip),%xmm5     # .5
    mulsd   %xmm3,%xmm2

#/* How many pi/2 is x a multiple of? */
#       xexp  = ax >> EXPSHIFTBITS_DP64;
    shr     $52,%r10                                  # >>EXPSHIFTBITS_DP64

#       npi2  = (int)(x * twobypi + 0.5);
    addsd   %xmm5,%xmm2                               # npi2

    movsd   .L__real_3ff921fb54400000(%rip),%xmm3     # piby2_1
    cvttpd2dq    %xmm2,%xmm0                          # convert to integer
    movsd   .L__real_3dd0b4611a626331(%rip),%xmm1     # piby2_1tail
    cvtdq2pd    %xmm0,%xmm2                           # and back to float.

#       /* Subtract the multiple from x to get an extra-precision remainder */
#       rhead  = x - npi2 * piby2_1;
    mulsd   %xmm2,%xmm3
    subsd   %xmm3,%xmm4                               # rhead

#       rtail  = npi2 * piby2_1tail;
    mulsd   %xmm2,%xmm1
    movd    %xmm0,%eax


#       GET_BITS_DP64(rhead-rtail, uy);                   ; originally only rhead
    movsd   %xmm4,%xmm0
    subsd   %xmm1,%xmm0

    movsd   .L__real_3dd0b4611a600000(%rip),%xmm3     # piby2_2
    movsd   %xmm0, p_temp0(%rsp)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm5     # piby2_2tail
    mov     %eax, %ecx
    mov     p_temp0(%rsp), %r9                        # rcx is rhead-rtail

#    xmm0=r, xmm4=rhead, xmm1=rtail, xmm2=npi2, xmm3=temp for calc, xmm5= temp for calc
#       expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     $1,%r9                                    # strip any sign bit
    shr     $53,%r9                                   # >> EXPSHIFTBITS_DP64 +1
    sub     %r9,%r10                                  # expdiff

##       if (expdiff > 15)
    cmp     $15,%r10
    jle     .Lexpdiff15

#          /* The remainder is pretty small compared with x, which
#             implies that x is a near multiple of pi/2
#             (x matches the multiple to at least 15 bits) */

#          t  = rhead;
    movsd   %xmm4,%xmm1

#          rtail  = npi2 * piby2_2;
    mulsd   %xmm2,%xmm3

#          rhead  = t - rtail;
    mulsd   %xmm2,%xmm5                               # npi2 * piby2_2tail
    subsd   %xmm3,%xmm4                               # rhead

#          rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
    subsd   %xmm4,%xmm1                               # t - rhead
    subsd   %xmm3,%xmm1                               # -rtail
    subsd   %xmm1,%xmm5                               # rtail

#      r = rhead - rtail;
    movsd   %xmm4,%xmm0

#xmm1=rtail
    movsd   %xmm5,%xmm1
    subsd   %xmm5,%xmm0
    
#    xmm0=r, xmm4=rhead, xmm1=rtail
.Lexpdiff15:
#      region = npi2 & 3;

    subsd   %xmm0,%xmm4                               # rhead-r
    subsd   %xmm1,%xmm4                               # rr = (rhead-r) - rtail

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
## if the input was close to a pi/2 multiple
# The original NAG code missed this trick.  If the input is very close to n*pi/2 after
# reduction,
# then the sin is ~ 1.0 , to within 53 bits, when r is < 2^-27.  We already
# have x at this point, so we can skip the sin polynomials.

    cmp     $0x03f2,%r9                               # if r  small.
    jge     .Lcossin_piby4                            # use taylor series if not
    cmp     $0x03de,%r9                               # if r really small.
    jle     .Lr_small                                 # then sin(r) = 0

    movsd   %xmm0,%xmm2
    mulsd   %xmm2,%xmm2                               # x^2

##      if region is 0 or 2 do a sin calc.
    and     $1,%ecx
    jnz     .Lregion13

# region 0 or 2 do a sincos calculation
# use simply polynomial
#              sin=x - x*x*x*0.166666666666666666;
    movsd   .L__real_3fc5555555555555(%rip),%xmm3     # 0.166666666
    mulsd   %xmm0,%xmm3                               # * x
    mulsd   %xmm2,%xmm3                               # * x^2
    subsd   %xmm3,%xmm0                               # xs
#              cos=1.0 - x*x*0.5;
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # 1.0
    mulsd   .L__real_3fe0000000000000(%rip),%xmm2     # 0.5 *x^2
    subsd   %xmm2,%xmm1                               # xc

    jmp     .Ladjust_region

.align 16
.Lregion13:
# region 1 or 3 do a cossin calculation
# use simply polynomial
#              sin=x - x*x*x*0.166666666666666666;
    movsd   %xmm0,%xmm1

    movsd   .L__real_3fc5555555555555(%rip),%xmm3     # 0.166666666
    mulsd   %xmm0,%xmm3                               # 0.166666666* x
    mulsd   %xmm2,%xmm3                               # 0.166666666* x * x^2
    subsd   %xmm3,%xmm1                               # xs
#              cos=1.0 - x*x*0.5;
    movsd   .L__real_3ff0000000000000(%rip),%xmm0     # 1.0
    mulsd   .L__real_3fe0000000000000(%rip),%xmm2     # 0.5 *x^2
    subsd   %xmm2,%xmm0                               # xc

    jmp     .Ladjust_region

.align 16
.Lr_small:
##      if region is 0 or 2    do a sincos calc.
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # cos(r) is a 1
    and     $1,%ecx
    jz      .Ladjust_region

##      if region is 1 or 3    do a cossin calc.
    movsd   %xmm0,%xmm1                               # sin(r) is r
    movsd   .L__real_3ff0000000000000(%rip),%xmm0     # cos(r) is a 1
    jmp     .Ladjust_region

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsincos_reduce_precise:
#      // Reduce x into range [-pi/4,pi/4]
#      __amd_remainder_piby2(x, &r, &rr, &region);

    mov     %rdi, p_temp1(%rsp)
    mov     %rsi, p_temp1+8(%rsp)
    mov     %r11,p_temp0(%rsp)

    lea     region(%rsp),%rdx
    lea     rr(%rsp),%rsi
    lea     r(%rsp),%rdi
    
    call    __amd_remainder_piby2@PLT

    mov     p_temp1(%rsp), %rdi
    mov     p_temp1+8(%rsp), %rsi
    mov     p_temp0(%rsp),%r11

    movsd   r(%rsp),%xmm0                             # x
    movsd   rr(%rsp),%xmm4                            # xx
    mov     region(%rsp),%eax                         # region to classify for sin/cos calc
    mov     %eax,%ecx                                 # region to get sign

# xmm0 = x, xmm4 = xx, r8d = 1, eax= region
.align 16
.Lcossin_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# perform taylor series to calc sinx, sinx
#  x2 = r * r;
#xmm4 = a part of rr for the sin path, xmm4 is overwritten in the sin path
#instead use xmm3 because that was freed up in the sin path, xmm3 is overwritten in sin path

    movsd   %xmm0,%xmm2
    mulsd   %xmm0,%xmm2                               #x2

##      if region is 0 or 2    do a sincos calc.
    and     $1,%ecx
    jz      .Lsincos02

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# region 1 or 3     - do a cossin calculation
#  zc = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));


    movlhps %xmm2,%xmm2

    movapd  .Lcossinarray+0x50(%rip),%xmm3            # s6
    movapd  %xmm2,%xmm1                               # move for x4
    movdqa  .Lcossinarray+0x20(%rip),%xmm5            # s3
    mulpd   %xmm2,%xmm3                               # x2s6
    addpd   .Lcossinarray+0x40(%rip),%xmm3            # s5+x2s6
    mulpd   %xmm2,%xmm5                               # x2s3
    movsd   %xmm4,p_temp0(%rsp)                       # rr move to to memory
    mulpd   %xmm2,%xmm1                               # x4
    mulpd   %xmm2,%xmm3                               # x2(s5+x2s6)
    addpd   .Lcossinarray+0x10(%rip),%xmm5            # s2+x2s3
    movapd  %xmm1,%xmm4                               # move for x6
    addpd   .Lcossinarray+0x30(%rip),%xmm3            # s4 + x2(s5+x2s6)
    mulpd   %xmm2,%xmm5                               # x2(s2+x2s3)
    mulpd   %xmm2,%xmm4                               # x6
    addpd   .Lcossinarray(%rip),%xmm5                 # s1+x2(s2+x2s3)
    mulpd   %xmm4,%xmm3                               # x6(s4 + x2(s5+x2s6))

    movsd   %xmm2,%xmm4                               # xmm4 = x2 for 0.5x2 cos
                                                      # xmm2 contains x2 for x3 sin

    addpd   %xmm5,%xmm3                               # zc in lower and zs in upper

    mulsd   %xmm0,%xmm2                               # xmm2=x3 for the sin term

    movhlps %xmm3,%xmm5                               # Copy z, xmm5 = sin, xmm3 = cos
    mulsd   .L__real_3fe0000000000000(%rip),%xmm4     # xmm4=r=0.5*x2 for cos term

    mulsd   %xmm2,%xmm5                               # sin *x3
    mulsd   %xmm1,%xmm3                               # cos *x4
    movsd   %xmm0,p_temp1(%rsp)                       # store x
    movsd   %xmm0,%xmm1

    movsd   .L__real_3ff0000000000000(%rip),%xmm2     # 1.0
    subsd   %xmm4,%xmm2                               # t=1.0-r

    movsd   .L__real_3ff0000000000000(%rip),%xmm0     # 1.0
    subsd   %xmm2,%xmm0                               # 1 - t

    mulsd   p_temp0(%rsp),%xmm1                       # x*xx
    subsd   %xmm4,%xmm0                               # (1-t) -r
    subsd   %xmm1,%xmm0                               # ((1-t) -r) - x *xx

    mulsd   p_temp0(%rsp),%xmm4                       # 0.5*x2*xx

    addsd   %xmm3,%xmm0                               # (((1-t) -r) - x *xx) + cos

    subsd   %xmm4,%xmm5                               # sin - 0.5*x2*xx

    addsd   %xmm2,%xmm0                               # xmm0 = t +{ (((1-t) -r) - x *xx) + cos}, final cos term

    addsd   p_temp0(%rsp),%xmm5                       # sin + xx
    movsd   p_temp1(%rsp),%xmm1                       # load x
    addsd   %xmm5,%xmm1                               # xmm1= sin+x, final sin term

    jmp     .Ladjust_region

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsincos02:
# region 0 or 2 do a sincos calculation
    movlhps %xmm2,%xmm2

    movapd  .Lsincosarray+0x50(%rip),%xmm3            # s6
    movapd  %xmm2,%xmm1                               # move for x4
    movdqa  .Lsincosarray+0x20(%rip),%xmm5            # s3
    mulpd   %xmm2,%xmm3                               # x2s6
    addpd   .Lsincosarray+0x40(%rip),%xmm3            # s5+x2s6
    mulpd   %xmm2,%xmm5                               # x2s3
    movsd   %xmm4,p_temp0(%rsp)                       # rr move to to memory
    mulpd   %xmm2,%xmm1                               # x4
    mulpd   %xmm2,%xmm3                               # x2(s5+x2s6)
    addpd   .Lsincosarray+0x10(%rip),%xmm5            # s2+x2s3
    movapd  %xmm1,%xmm4                               # move for x6
    addpd   .Lsincosarray+0x30(%rip),%xmm3            # s4 + x2(s5+x2s6)
    mulpd   %xmm2,%xmm5                               # x2(s2+x2s3)
    mulpd   %xmm2,%xmm4                               # x6
    addpd   .Lsincosarray(%rip),%xmm5                 # s1+x2(s2+x2s3)
    mulpd   %xmm4,%xmm3                               # x6(s4 + x2(s5+x2s6))

    movsd   %xmm2,%xmm4                               # xmm4 = x2 for 0.5x2 for cos
                                                      # xmm2 contains x2 for x3 for sin

    addpd   %xmm5,%xmm3                               # zs in lower and zc in upper

    mulsd   %xmm0,%xmm2                               # xmm2=x3 for sin

    movhlps %xmm3,%xmm5                               # Copy z, xmm5 = cos , xmm3 = sin

    mulsd   .L__real_3fe0000000000000(%rip),%xmm4     # xmm4=r=0.5*x2 for cos term

    mulsd   %xmm2,%xmm3                               # sin *x3
    mulsd   %xmm1,%xmm5                               # cos *x4

    movsd   .L__real_3ff0000000000000(%rip),%xmm2     # 1.0
    subsd   %xmm4,%xmm2                               # t=1.0-r

    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # 1.0
    subsd   %xmm2,%xmm1                               # 1 - t

    movsd   %xmm0,p_temp1(%rsp)                       # store x
    mulsd   p_temp0(%rsp),%xmm0                       # x*xx

    subsd   %xmm4,%xmm1                               # (1-t) -r
    subsd   %xmm0,%xmm1                               # ((1-t) -r) - x *xx

    mulsd   p_temp0(%rsp),%xmm4                       # 0.5*x2*xx

    addsd   %xmm5,%xmm1                               # (((1-t) -r) - x *xx) + cos

    subsd   %xmm4,%xmm3                               # sin - 0.5*x2*xx

    addsd   %xmm2,%xmm1                               # xmm1 = t +{ (((1-t) -r) - x *xx) + cos}, final cos term

    addsd   p_temp0(%rsp),%xmm3                       # sin + xx
    movsd   p_temp1(%rsp),%xmm0                       # load x
    addsd   %xmm3,%xmm0                               # xmm0= sin+x, final sin term

    jmp     .Ladjust_region

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#      switch (region)
.align 16
.Ladjust_region:        # positive or negative for sin return val in xmm0

    mov     %eax,%r9d

    shr     $1,%eax
    mov     %eax,%ecx
    and     %r11d,%eax

    not     %ecx
    not     %r11d
    and     %r11d,%ecx

    or      %ecx,%eax
    and     $1,%eax
    jnz     .Lcos_sign

## if the original region 0, 1 and arg is negative, then we negate the result.
## if the original region 2, 3 and arg is positive, then we negate the result.
    movsd   %xmm0,%xmm2
    xorpd   %xmm0,%xmm0
    subsd   %xmm2,%xmm0

.Lcos_sign:            # positive or negative for cos return val in xmm1
    add     $1,%r9
    and     $2,%r9d
    jz      .Lsincos_cleanup
## if the original region 1 or 2 then we negate the result.
    movsd   %xmm1,%xmm2
    xorpd   %xmm1,%xmm1
    subsd   %xmm2,%xmm1

#.align 16
#normal result return
.Lsincos_cleanup:
#   movsd   %xmm0, p_temp0(%rsp)                     # store the sin
#   movsd   %xmm1, p_temp1(%rsp)                     # store the cos
# we need cos for xmm0, sin for xmm1, swap
	movsd   %xmm0, %xmm4
	movsd   %xmm1, %xmm0
	movsd   %xmm4, %xmm1
	
	movsd   p_temp2(%rsp), %xmm3 					 # recover exp(x)
	mulsd   %xmm3, %xmm1
	mulsd   %xmm3, %xmm0
	    
    add     $stack_size,%rsp
    ret

.align 16
# exception handle for sincos
.Lsincos_naninf:
    lea     region(%rsp), %rdi
    lea     r(%rsp), %rsi 
    call    fname_special1
    
    movsd   p_temp2(%rsp), %xmm3 					 # recover exp(x)  
    mov     p_temp2(%rsp), %rax
    mov     $0x07fffffffffffffff, %rcx 
    and     %rcx, %rax
    jz      .LZero_Nan

	mulsd   %xmm3, %xmm0   
	movsd   %xmm0, %xmm1     
    add     $stack_size, %rsp
    ret

.LZero_Nan:
	xorpd   %xmm0, %xmm0
	xorpd   %xmm1, %xmm1
    add     $stack_size, %rsp
    ret

# move data to the end
.section .rodata
.align 16
.L__real_7fffffffffffffff: .quad 0x07fffffffffffffff  #Sign bit zero
                        .quad 0                       # for alignment
.L__real_3ff0000000000000: .quad 0x03ff0000000000000  # 1.0
                        .quad 0                          
.L__real_3fe0000000000000: .quad 0x03fe0000000000000  # 0.5
                        .quad 0
.L__real_3fc5555555555555: .quad 0x03fc5555555555555  # 0.166666666666
                        .quad 0
.L__real_3fe45f306dc9c883: .quad 0x03fe45f306dc9c883  # twobypi
                        .quad 0
.L__real_3ff921fb54400000: .quad 0x03ff921fb54400000  # piby2_1
                        .quad 0
.L__real_3dd0b4611a626331: .quad 0x03dd0b4611a626331  # piby2_1tail
                        .quad 0
.L__real_3dd0b4611a600000: .quad 0x03dd0b4611a600000  # piby2_2
                        .quad 0
.L__real_3ba3198a2e037073: .quad 0x03ba3198a2e037073  # piby2_2tail
                        .quad 0                
.L__real_fffffffff8000000: .quad 0x0fffffffff8000000  # mask for stripping head and tail
                        .quad 0                
.L__real_411E848000000000: .quad 0x0415312d000000000  # 411E848000000000  # 5e5
                        .quad 0
.L__real_7ff8000000000000: .quad 0x07ff8000000000000  # qnan
                        .quad 0                       
                        
.align 16
.Lsincosarray:
    .quad    0x0bfc5555555555555                      # -0.16666666666666666    s1
    .quad    0x03fa5555555555555                      # 0.041666666666666664    c1   
    .quad    0x03f81111111110bb3                      # 0.00833333333333095     s2
    .quad    0x0bf56c16c16c16967                      # -0.0013888888888887398  c2
    .quad    0x0bf2a01a019e83e5c                      # -0.00019841269836761127 s3
    .quad    0x03efa01a019f4ec90                      # 2.4801587298767041E-05  c3
    .quad    0x03ec71de3796cde01                      # 2.7557316103728802E-06  s4
    .quad    0x0be927e4fa17f65f6                      # -2.7557317272344188E-07 c4
    .quad    0x0be5ae600b42fdfa7                      # -2.5051132068021698E-08 s5
    .quad    0x03e21eeb69037ab78                      # 2.0876146382232963E-09  c6
    .quad    0x03de5e0b2f9a43bb8                      # 1.5918144304485914E-10  s6
    .quad    0x0bda907db46cc5e42                      # -1.1382639806794487E-11 c7

.align 16
.Lcossinarray:
    .quad    0x03fa5555555555555                      # 0.0416667        c1
    .quad    0x0bfc5555555555555                      # -0.166667        s1
    .quad    0x0bf56c16c16c16967
    .quad    0x03f81111111110bb3                      # 0.00833333       s2
    .quad    0x03efa01a019f4ec90
    .quad    0x0bf2a01a019e83e5c                      # -0.000198413     s3
    .quad    0x0be927e4fa17f65f6
    .quad    0x03ec71de3796cde01                      # 2.75573e-006     s4
    .quad    0x03e21eeb69037ab78
    .quad    0x0be5ae600b42fdfa7                      # -2.50511e-008    s5
    .quad    0x0bda907db46cc5e42
    .quad    0x03de5e0b2f9a43bb8                      # 1.59181e-010     s6

.align 16
# these codes and the ones in the corresponding .c file have to match
.L__flag_x_nan:         .long 00000001
.L__flag_y_zero:        .long 00000002
.L__flag_y_inf:         .long 00000003

.align 16
.L__denormal_threshold:         .long 0x0fffffc02 # -1022
                                .long 0
                                .quad 0
.L__enable_almost_inf:          .quad 0x7fe0000000000000
                                .quad 0
.L__real_zero:                  .quad 0x0000000000000000
                                .quad 0
.L__real_one:                   .quad 0x3ff0000000000000
                                .quad 0 
.L__real_smallest_denormal:     .quad 0x0000000000000001
                                .quad 0
.L__denormal_tiny_threshold:    .quad 0x0c0874046dfefd9d0
                                .quad 0
.L__exp_mant_mask:              .quad 0x7fffffffffffffff
                                .quad 0
.L__real_x_near0_threshold:     .quad 0x3c00000000000000
                                .quad 0
.L__real_inf:                   .quad 0x7ff0000000000000    # inf
                                .quad 0
.L__real_ninf:                  .quad 0x0fff0000000000000   # -inf
                                .quad 0
.L__real_qnanbit:               .quad 0x0008000000000000    # qnan set bit
                                .quad 0
.L__real_pthreshold:            .quad 0x40862e42fefa39ef
                                .quad 0
.L__real_mthreshold:            .quad 0x0c0874910d52d3052
                                .quad 0
.L__real_64_by_log2:            .quad 0x40571547652b82fe    # 64/ln(2)
                                .quad 0
.L__real_log2_by_64_head:       .quad 0x3f862e42fefa0000    # log2_by_64_head
                                .quad 0
.L__real_log2_by_64_tail:       .quad 0x0bd1cf79abc9e3b39   # -log2_by_64_tail
                                .quad 0
.L__real_1_by_720:              .quad 0x3f56c16c16c16c17    # 1/720
                                .quad 0
.L__real_1_by_120:              .quad 0x3f81111111111111    # 1/120
                                .quad 0
.L__real_1_by_24:               .quad 0x3fa5555555555555    # 1/24
                                .quad 0
.L__real_1_by_6:                .quad 0x3fc5555555555555    # 1/6
                                .quad 0
.L__real_1_by_2:                .quad 0x3fe0000000000000    # 1/2
                                .quad 0

.align 16
.L__two_to_jby64_table:
    .quad 0x3ff0000000000000
    .quad 0x3ff02c9a3e778061
    .quad 0x3ff059b0d3158574
    .quad 0x3ff0874518759bc8
    .quad 0x3ff0b5586cf9890f
    .quad 0x3ff0e3ec32d3d1a2
    .quad 0x3ff11301d0125b51
    .quad 0x3ff1429aaea92de0
    .quad 0x3ff172b83c7d517b
    .quad 0x3ff1a35beb6fcb75
    .quad 0x3ff1d4873168b9aa
    .quad 0x3ff2063b88628cd6
    .quad 0x3ff2387a6e756238
    .quad 0x3ff26b4565e27cdd
    .quad 0x3ff29e9df51fdee1
    .quad 0x3ff2d285a6e4030b
    .quad 0x3ff306fe0a31b715
    .quad 0x3ff33c08b26416ff
    .quad 0x3ff371a7373aa9cb
    .quad 0x3ff3a7db34e59ff7
    .quad 0x3ff3dea64c123422
    .quad 0x3ff4160a21f72e2a
    .quad 0x3ff44e086061892d
    .quad 0x3ff486a2b5c13cd0
    .quad 0x3ff4bfdad5362a27
    .quad 0x3ff4f9b2769d2ca7
    .quad 0x3ff5342b569d4f82
    .quad 0x3ff56f4736b527da
    .quad 0x3ff5ab07dd485429
    .quad 0x3ff5e76f15ad2148
    .quad 0x3ff6247eb03a5585
    .quad 0x3ff6623882552225
    .quad 0x3ff6a09e667f3bcd
    .quad 0x3ff6dfb23c651a2f
    .quad 0x3ff71f75e8ec5f74
    .quad 0x3ff75feb564267c9
    .quad 0x3ff7a11473eb0187
    .quad 0x3ff7e2f336cf4e62
    .quad 0x3ff82589994cce13
    .quad 0x3ff868d99b4492ed
    .quad 0x3ff8ace5422aa0db
    .quad 0x3ff8f1ae99157736
    .quad 0x3ff93737b0cdc5e5
    .quad 0x3ff97d829fde4e50
    .quad 0x3ff9c49182a3f090
    .quad 0x3ffa0c667b5de565
    .quad 0x3ffa5503b23e255d
    .quad 0x3ffa9e6b5579fdbf
    .quad 0x3ffae89f995ad3ad
    .quad 0x3ffb33a2b84f15fb
    .quad 0x3ffb7f76f2fb5e47
    .quad 0x3ffbcc1e904bc1d2
    .quad 0x3ffc199bdd85529c
    .quad 0x3ffc67f12e57d14b
    .quad 0x3ffcb720dcef9069
    .quad 0x3ffd072d4a07897c
    .quad 0x3ffd5818dcfba487
    .quad 0x3ffda9e603db3285
    .quad 0x3ffdfc97337b9b5f
    .quad 0x3ffe502ee78b3ff6
    .quad 0x3ffea4afa2a490da
    .quad 0x3ffefa1bee615a27
    .quad 0x3fff50765b6e4540
    .quad 0x3fffa7c1819e90d8

.align 16
.L__two_to_jby64_head_table:
    .quad 0x3ff0000000000000
    .quad 0x3ff02c9a30000000
    .quad 0x3ff059b0d0000000
    .quad 0x3ff0874510000000
    .quad 0x3ff0b55860000000
    .quad 0x3ff0e3ec30000000
    .quad 0x3ff11301d0000000
    .quad 0x3ff1429aa0000000
    .quad 0x3ff172b830000000
    .quad 0x3ff1a35be0000000
    .quad 0x3ff1d48730000000
    .quad 0x3ff2063b80000000
    .quad 0x3ff2387a60000000
    .quad 0x3ff26b4560000000
    .quad 0x3ff29e9df0000000
    .quad 0x3ff2d285a0000000
    .quad 0x3ff306fe00000000
    .quad 0x3ff33c08b0000000
    .quad 0x3ff371a730000000
    .quad 0x3ff3a7db30000000
    .quad 0x3ff3dea640000000
    .quad 0x3ff4160a20000000
    .quad 0x3ff44e0860000000
    .quad 0x3ff486a2b0000000
    .quad 0x3ff4bfdad0000000
    .quad 0x3ff4f9b270000000
    .quad 0x3ff5342b50000000
    .quad 0x3ff56f4730000000
    .quad 0x3ff5ab07d0000000
    .quad 0x3ff5e76f10000000
    .quad 0x3ff6247eb0000000
    .quad 0x3ff6623880000000
    .quad 0x3ff6a09e60000000
    .quad 0x3ff6dfb230000000
    .quad 0x3ff71f75e0000000
    .quad 0x3ff75feb50000000
    .quad 0x3ff7a11470000000
    .quad 0x3ff7e2f330000000
    .quad 0x3ff8258990000000
    .quad 0x3ff868d990000000
    .quad 0x3ff8ace540000000
    .quad 0x3ff8f1ae90000000
    .quad 0x3ff93737b0000000
    .quad 0x3ff97d8290000000
    .quad 0x3ff9c49180000000
    .quad 0x3ffa0c6670000000
    .quad 0x3ffa5503b0000000
    .quad 0x3ffa9e6b50000000
    .quad 0x3ffae89f90000000
    .quad 0x3ffb33a2b0000000
    .quad 0x3ffb7f76f0000000
    .quad 0x3ffbcc1e90000000
    .quad 0x3ffc199bd0000000
    .quad 0x3ffc67f120000000
    .quad 0x3ffcb720d0000000
    .quad 0x3ffd072d40000000
    .quad 0x3ffd5818d0000000
    .quad 0x3ffda9e600000000
    .quad 0x3ffdfc9730000000
    .quad 0x3ffe502ee0000000
    .quad 0x3ffea4afa0000000
    .quad 0x3ffefa1be0000000
    .quad 0x3fff507650000000
    .quad 0x3fffa7c180000000

.align 16
.L__two_to_jby64_tail_table:
    .quad 0x0000000000000000
    .quad 0x3e6cef00c1dcdef9
    .quad 0x3e48ac2ba1d73e2a
    .quad 0x3e60eb37901186be
    .quad 0x3e69f3121ec53172
    .quad 0x3e469e8d10103a17
    .quad 0x3df25b50a4ebbf1a
    .quad 0x3e6d525bbf668203
    .quad 0x3e68faa2f5b9bef9
    .quad 0x3e66df96ea796d31
    .quad 0x3e368b9aa7805b80
    .quad 0x3e60c519ac771dd6
    .quad 0x3e6ceac470cd83f5
    .quad 0x3e5789f37495e99c
    .quad 0x3e547f7b84b09745
    .quad 0x3e5b900c2d002475
    .quad 0x3e64636e2a5bd1ab
    .quad 0x3e4320b7fa64e430
    .quad 0x3e5ceaa72a9c5154
    .quad 0x3e53967fdba86f24
    .quad 0x3e682468446b6824
    .quad 0x3e3f72e29f84325b
    .quad 0x3e18624b40c4dbd0
    .quad 0x3e5704f3404f068e
    .quad 0x3e54d8a89c750e5e
    .quad 0x3e5a74b29ab4cf62
    .quad 0x3e5a753e077c2a0f
    .quad 0x3e5ad49f699bb2c0
    .quad 0x3e6a90a852b19260
    .quad 0x3e56b48521ba6f93
    .quad 0x3e0d2ac258f87d03
    .quad 0x3e42a91124893ecf
    .quad 0x3e59fcef32422cbe
    .quad 0x3e68ca345de441c5
    .quad 0x3e61d8bee7ba46e1
    .quad 0x3e59099f22fdba6a
    .quad 0x3e4f580c36bea881
    .quad 0x3e5b3d398841740a
    .quad 0x3e62999c25159f11
    .quad 0x3e668925d901c83b
    .quad 0x3e415506dadd3e2a
    .quad 0x3e622aee6c57304e
    .quad 0x3e29b8bc9e8a0387
    .quad 0x3e6fbc9c9f173d24
    .quad 0x3e451f8480e3e235
    .quad 0x3e66bbcac96535b5
    .quad 0x3e41f12ae45a1224
    .quad 0x3e55e7f6fd0fac90
    .quad 0x3e62b5a75abd0e69
    .quad 0x3e609e2bf5ed7fa1
    .quad 0x3e47daf237553d84
    .quad 0x3e12f074891ee83d
    .quad 0x3e6b0aa538444196
    .quad 0x3e6cafa29694426f
    .quad 0x3e69df20d22a0797
    .quad 0x3e640f12f71a1e45
    .quad 0x3e69f7490e4bb40b
    .quad 0x3e4ed9942b84600d
    .quad 0x3e4bdcdaf5cb4656
    .quad 0x3e5e2cffd89cf44c
    .quad 0x3e452486cc2c7b9d
    .quad 0x3e6cc2b44eee3fa4
    .quad 0x3e66dc8a80ce9f09
    .quad 0x3e39e90d82e90a7e
