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
# An implementation of the sin function.
#
# Prototype:
#
#     double sin(double x);
#
#   Computes sin(x).
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.
#   Based on the NAG C implementation.
#
#
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.section .rodata
.align 32
.L__real_3fe0000000000000: .quad 0x03fe0000000000000  # 0.5
                  .quad 0                             # for alignment
.L__real_3ff0000000000000: .quad 0x03ff0000000000000  # 1.0
                  .quad 0
.L__real_3fc5555555555555: .quad 0x03fc5555555555555  # 0.166666666666
                  .quad 0
.L__real_3fe45f306dc9c883: .quad 0x03fe45f306dc9c883  # twobypi
                  .quad 0
.L__real_411E848000000000: .quad 0x415312d000000000   # 5e6 0x0411E848000000000  # 5e5
                  .quad 0
.L__real_7fffffffffffffff: .quad 0x07fffffffffffffff  # Sign bit zero
                  .quad 0
.L__real_3ff921fb54400000: .quad 0x03ff921fb54400000  # piby2_1
                  .quad 0
.L__real_3dd0b4611a626331: .quad 0x03dd0b4611a626331  # piby2_1tail
                  .quad 0
.L__real_3dd0b4611a600000: .quad 0x03dd0b4611a600000  # piby2_2
                  .quad 0
.L__real_3ba3198a2e037073: .quad 0x03ba3198a2e037073  # piby2_2tail
                  .quad 0

.align 32
.Lcosarray:
   .quad   0x03fa5555555555555                        # 0.0416667         c1
   .quad   0
   .quad   0x0bf56c16c16c16967                        # -0.00138889       c2
   .quad   0
   .quad   0x03EFA01A019F4EC91                        # 2.48016e-005      c3
   .quad   0
   .quad   0x0bE927E4FA17F667B                        # -2.75573e-007     c4
   .quad   0
   .quad   0x03E21EEB690382EEC                        # 2.08761e-009      c5
   .quad   0
   .quad   0x0bDA907DB47258AA7                        # -1.13826e-011     c6
   .quad   0

.align 32
.Lsinarray:
   .quad   0x0bfc5555555555555                        # -0.166667         s1
   .quad   0
   .quad   0x03f81111111110bb3                        # 0.00833333        s2
   .quad   0
   .quad   0x0bf2a01a019e83e5c                        # -0.000198413      s3
   .quad   0
   .quad   0x03ec71de3796cde01                        # 2.75573e-006      s4
   .quad   0
   .quad   0x0be5ae600b42fdfa7                        # -2.50511e-008     s5
   .quad   0
   .quad   0x03de5e0b2f9a43bb8                        # 1.59181e-010      s6
   .quad   0

.text
.align 32
.p2align 4,,15

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(sin)
#define fname_special _sin_special@PLT
#define fname_special_underflow _sin_special_underflow@PLT


# define local variable storage offsets
.equ   p_temp,     0x30                               # temporary for get/put bits operation
.equ   p_temp1,    0x40                               # temporary for get/put bits operation
.equ   r,          0x50                               # pointer to r for amd_remainder_piby2
.equ   rr,         0x60                               # pointer to rr for amd_remainder_piby2
.equ   region,     0x70                               # pointer to region for amd_remainder_piby2
.equ   stack_size, 0x98

.globl fname
.type  fname,@function

fname:
   sub      $stack_size, %rsp
   xorpd    %xmm2, %xmm2                              # zeroed out for later use

# GET_BITS_DP64(x, ux);
# get the input value to an integer register.
   movsd    %xmm0, p_temp(%rsp)
   mov      p_temp(%rsp), %rdx                        # rdx is ux

##  if NaN or inf
   mov      $0x07ff0000000000000, %rax
   mov      %rax, %r10
   and      %rdx, %r10
   cmp      %rax, %r10
   jz       .Lsin_naninf

#  ax = (ux & ~SIGNBIT_DP64);
   mov      $0x07fffffffffffffff, %r10
   and      %rdx, %r10                                # r10 is ax
   mov      $1, %r8d                                  # for determining region later on

##  if (ax <= 0x3fe921fb54442d18) /* abs(x) <= pi/4 */
   mov      $0x03fe921fb54442d18, %rax
   cmp      %rax, %r10
   jg       .Lsin_reduce

##      if (ax < 0x3f20000000000000) /* abs(x) < 2.0^(-13) */
   mov      $0x03f20000000000000, %rax
   cmp      %rax, %r10
   jge      .Lsin_small

##          if (ax < 0x3e40000000000000) /* abs(x) < 2.0^(-27) */
   mov      $0x03e40000000000000, %rax
   cmp      %rax, %r10
   jge      .Lsin_smaller

##  Additional code to raise underflow exception
   xor      %rax,%rax
   cmp      %rax,%r10
   je      .Lsin_cleanup
   call     fname_special_underflow
#                  sin = 1.0;
   jmp      .Lsin_cleanup

.align 32
.Lsin_smaller:
#              sin = x - x^3 * 0.1666666666666666666;
   movsd    %xmm0, %xmm2
   movsd    .L__real_3fc5555555555555(%rip), %xmm4    # 0.1666666666666666666
   mulsd    %xmm2, %xmm2                              # x^2
   mulsd    %xmm0, %xmm2                              # x^3
   mulsd    %xmm4, %xmm2                              # x^3 * 0.1666666666666666666
   subsd    %xmm2, %xmm0                              # x - x^3 * 0.1666666666666666666
   jmp      .Lsin_cleanup

.align 32
.Lsin_small:
#          sin = sin_piby4(x, 0.0);
   movsd    .L__real_3fe0000000000000(%rip), %xmm5   # .5

.Lsin_piby4_noreduce:

  #x2 = x * x;
  #x3 = x2 * x;
  #r = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));
  #x + x3 * (c1 + x2 * r);

    movapd %xmm0,%xmm3                  #x
    movapd %xmm0,%xmm8                  #x
    mulsd  %xmm0,%xmm3                  #x*x = x2
    movapd %xmm3,%xmm6                  #x2

    movapd .Lsinarray+0x40(%rip),%xmm5  #c5
    mulsd .Lsinarray+0x50(%rip),%xmm3   #c6*x2
    addsd %xmm3,%xmm5                   #c5+c6*x2
    mulsd %xmm6,%xmm5                   #x2*(c5+c6*x2)
    addsd .Lsinarray+0x30(%rip),%xmm5   #c4+(x2*(c5+c6*x2))
    mulsd %xmm6,%xmm5                   #x2*(c4+(x2*(c5+c6*x2)))
    addsd .Lsinarray+0x20(%rip),%xmm5   #c3 +(x2*(c4+(x2*(c5+c6*x2))))
    mulsd %xmm6,%xmm5                   #x2*(c3 +(x2*(c4+(x2*(c5+c6*x2)))))
    addsd .Lsinarray+0x10(%rip),%xmm5   #c2+(x2*(c3 +(x2*(c4+(x2*(c5+c6*x2))))))
    mulsd %xmm6,%xmm5
    addsd .Lsinarray(%rip),%xmm5   # (c1 + x2 *  (c2+(x2*(c3 +(x2*(c4+(x2*(c5+c6*x2))))))
    mulsd %xmm6,%xmm5
    mulsd %xmm0,%xmm5
    addsd %xmm5,%xmm0

   jmp      .Lsin_cleanup

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsin_reduce:
#  xneg = (ax != ux);
   cmp      %r10, %rdx
   mov      $0, %r11d

##  if (xneg) x = -x;
   jz       .Lpositive
   mov      $1, %r11d
   subsd    %xmm0, %xmm2
   movsd    %xmm2, %xmm0

.align 16
.Lpositive:
##  if (x < 5.0e5)
   cmp      .L__real_411E848000000000(%rip), %r10
   jae      .Lsin_reduce_precise

# reduce  the argument to be in a range from -pi/4 to +pi/4
# by subtracting multiples of pi/2
   movsd    %xmm0, %xmm2
   movsd    .L__real_3fe45f306dc9c883(%rip), %xmm3    # twobypi
   movsd    %xmm0, %xmm4
   movsd    .L__real_3fe0000000000000(%rip), %xmm5    # .5
   mulsd    %xmm3, %xmm2

#/* How many pi/2 is x a multiple of? */
#      xexp  = ax >> EXPSHIFTBITS_DP64;
   mov      %r10, %r9
   shr      $52, %r9                                  # >>EXPSHIFTBITS_DP64

#        npi2  = (int)(x * twobypi + 0.5);
   addsd    %xmm5, %xmm2                              # npi2

   movsd    .L__real_3ff921fb54400000(%rip), %xmm3    # piby2_1
   cvttpd2dq   %xmm2, %xmm0                           # convert to integer
   movsd    .L__real_3dd0b4611a626331(%rip), %xmm1    # piby2_1tail
   cvtdq2pd   %xmm0, %xmm2                            # and back to float.

#      /* Subtract the multiple from x to get an extra-precision remainder */
#      rhead  = x - npi2 * piby2_1;
   mulsd    %xmm2, %xmm3
   subsd    %xmm3, %xmm4                              # rhead

#      rtail  = npi2 * piby2_1tail;
   mulsd    %xmm2, %xmm1
   movd     %xmm0, %eax

#      GET_BITS_DP64(rhead-rtail, uy);
   movsd    %xmm4, %xmm0
   subsd    %xmm1, %xmm0

   movsd    .L__real_3dd0b4611a600000(%rip), %xmm3    # piby2_2
   movsd    %xmm0,p_temp(%rsp)
   movsd    .L__real_3ba3198a2e037073(%rip), %xmm5    # piby2_2tail
   mov      p_temp(%rsp), %rcx                        # rcx is rhead-rtail

#   xmm0=r, xmm4=rhead, xmm1=rtail, xmm2=npi2, xmm3=temp for calc, xmm5= temp for calc
#      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
   shl      $1, %rcx                                  # strip any sign bit
   shr      $53, %rcx                                 # >> EXPSHIFTBITS_DP64 +1
   sub      %rcx, %r9                                 # expdiff

##      if (expdiff > 15)
   cmp      $15, %r9
   jle      .Lexplediff15

#          /* The remainder is pretty small compared with x, which
#             implies that x is a near multiple of pi/2
#             (x matches the multiple to at least 15 bits) */

#          t  = rhead;
   movsd    %xmm4, %xmm1

#          rtail  = npi2 * piby2_2;
   mulsd    %xmm2, %xmm3

#          rhead  = t - rtail;
   mulsd    %xmm2, %xmm5                              # npi2 * piby2_2tail
   subsd    %xmm3, %xmm4                              # rhead

#          rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
   subsd    %xmm4, %xmm1                              # t - rhead
   subsd    %xmm3, %xmm1                              # -rtail
   subsd    %xmm1, %xmm5                              # rtail

#      r = rhead - rtail;
   movsd    %xmm4, %xmm0


#xmm1=rtail
   movsd    %xmm5, %xmm1
   subsd    %xmm5, %xmm0

#   xmm0=r, xmm4=rhead, xmm1=rtail
.Lexplediff15:
#      region = npi2 & 3;

   subsd    %xmm0, %xmm4                              # rhead-r
   subsd    %xmm1, %xmm4                              # rr = (rhead-r) - rtail

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
## if the input was close to a pi/2 multiple
# The original NAG code missed this trick.  If the input is very close to n*pi/2 after
# reduction,
# then the sin is ~ 1.0 , to within 53 bits, when r is < 2^-27.  We already
# have x at this point, so we can skip the sin polynomials.

#   cmp      $0x03f2, %rcx                             # if r  small.
#   jge      .Lsin_piby4                               # use taylor series if not
    jmp      .Lsin_piby4
#; THIS IS COMMENTED OUT for details check input value 631483.6852007523
#if 0

   cmp      $0x03de, %rcx                             # if r really small.
   jle      .Lr_small                                 # then sin(r) = 0

   movsd    %xmm0, %xmm2
   mulsd    %xmm2, %xmm2                              # x^2

##      if region is 0 or 2 do a sin calc.
   and      %eax, %r8d
   jnz      .Lcossmall

# region 0 or 2 do a sin calculation
# use simply polynomial
#              x - x*x*x*0.166666666666666666;
   movsd    .L__real_3fc5555555555555(%rip), %xmm3
   mulsd    %xmm0, %xmm3                              # * x
   mulsd    %xmm2, %xmm3                              # * x^2
   subsd    %xmm3, %xmm0                              # xs
   jmp      .Ladjust_region

.align 16
.Lcossmall:
# region 1 or 3 do a cos calculation
# use simply polynomial
#              1.0 - x*x*0.5;
   movsd    .L__real_3ff0000000000000(%rip), %xmm0    # 1.0
   mulsd    .L__real_3fe0000000000000(%rip), %xmm2    # 0.5 *x^2
   subsd    %xmm2, %xmm0                              # xc
   jmp      .Ladjust_region

.align 16
.Lr_small:
##      if region is 1 or 3   do a cos calc.
   and      %eax, %r8d
   jz       .Ladjust_region

# odd
   movsd    .L__real_3ff0000000000000(%rip), %xmm0    # cos(r) is a 1
   jmp      .Ladjust_region
#endif
#;END OF ABOVE COMMENTED CODE

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 32
.Lsin_reduce_precise:
#      // Reduce x into range [-pi/4,pi/4]
#      __amd_remainder_piby2(x, &r, &rr, &region);

   mov      %r11,p_temp(%rsp)
   lea      region(%rsp), %rdx
   lea      rr(%rsp), %rsi
   lea      r(%rsp), %rdi

   call     __amd_remainder_piby2@PLT

   mov      p_temp(%rsp), %r11
   mov      $1, %r8d                                  # for determining region later on
   movsd    r(%rsp), %xmm0                            # x
   movsd    rr(%rsp), %xmm4                           # xx
   mov      region(%rsp), %eax                        # region

# xmm0 = x, xmm4 = xx, r8d = 1, eax= region
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
# perform taylor series to calc sinx, sinx
.Lsin_piby4:
#  x2 = r * r;

#xmm4 = a part of rr for the sin path, xmm4 is overwritten in the sin path
#instead use xmm3 because that was freed up in the sin path, xmm3 is overwritten in sin path

##      if region is 0 or 2   do a sin calc.
   and      %eax, %r8d
   jnz      .Lcosregion

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# region 0 or 2 do a sin calculation

    movapd %xmm0,%xmm3                  #x
    movapd %xmm0,%xmm8                  #x
    mulsd  %xmm0,%xmm3                  #x*x = x2
    movapd %xmm3,%xmm6                  #x2

    movapd .Lsinarray+0x40(%rip),%xmm5  #c5
    mulsd .Lsinarray+0x50(%rip),%xmm3   #c6*x2
    addsd %xmm3,%xmm5                   #c5+c6*x2
    mulsd %xmm6,%xmm5                   #x2*(c5+c6*x2)
    addsd .Lsinarray+0x30(%rip),%xmm5   #c4+(x2*(c5+c6*x2))
    mulsd %xmm6,%xmm5                   #x2*(c4+(x2*(c5+c6*x2)))
    addsd .Lsinarray+0x20(%rip),%xmm5   #c3 +(x2*(c4+(x2*(c5+c6*x2))))
    mulsd %xmm6,%xmm5                   #x2*(c3 +(x2*(c4+(x2*(c5+c6*x2)))))
    addsd .Lsinarray+0x10(%rip),%xmm5   #c2+(x2*(c3 +(x2*(c4+(x2*(c5+c6*x2))))))
    mulsd %xmm6,%xmm8                   # x3
    movapd %xmm8,%xmm2                  # x3
    # xmm5 = r = poly
    mulsd %xmm5,%xmm2                   #x3 * r
    movapd %xmm4,%xmm5                  #copy xx
    mulsd  .L__real_3fe0000000000000(%rip),%xmm5 # 0.5*xx
    subsd   %xmm2,%xmm5                 #(0.5*xx - x3*r)
    mulsd  %xmm6,%xmm5                  # x2 * (0.5 * xx - x3 * r)
    subsd  %xmm4,%xmm5                  #((x2 * (0.5 * xx - x3 * r) - xx)
    mulsd .Lsinarray(%rip),%xmm8        #c1*x3
    subsd %xmm8,%xmm5                   #((x2 * (0.5 * xx - x3 * r) - xx) - c1*x3
    subsd  %xmm5,%xmm0                  # x - ((x2 * (0.5 * xx - x3 * r) - xx) - c1*x3
   jmp      .Ladjust_region

.align 16
.Lcosregion:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# region 1 or 3    - do a cos calculation
#  zc = (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))));
    movsd   %xmm0, %xmm2        #x
    mulsd   %xmm0, %xmm2        #x2
    mulsd   %xmm0, %xmm4        # x*xx

    movsd   .L__real_3fe0000000000000(%rip), %xmm5
    movsd   .Lcosarray+0x50(%rip), %xmm1       # c6
    mulsd   %xmm2,%xmm1                        # x2*c6
    addsd   .Lcosarray+0x40(%rip),%xmm1        # c5 + x2*c6
    mulsd   %xmm2,%xmm1                        # x2*(c5 + x2*c6)
    addsd   .Lcosarray+0x30(%rip), %xmm1       # c4 + x2(c5+x2c6)
    mulsd   %xmm2,%xmm1                        # x2(c4 + x2(c5+x2c6))
    addsd   .Lcosarray+0x20(%rip), %xmm1       # c3+x2(c4 + x2(c5+x2c6))
    mulsd   %xmm2,%xmm1                        # x2(c3+x2(c4 + x2(c5+x2c6)))
    addsd   .Lcosarray+0x10(%rip), %xmm1       # c2+x2(c3+x2(c4 + x2(c5+x2c6)))
    mulsd   %xmm2,%xmm1                        # x2*(c2+x2(c3+x2(c4 + x2(c5+x2c6))))
    addsd   .Lcosarray(%rip), %xmm1            # c1 + x2*(c2+x2(c3+x2(c4 + x2(c5+x2c6))))
    mulsd   %xmm2,%xmm1
    mulsd   %xmm2,%xmm1                        # poly = x2*x2*(c1 + x2*(c2+x2(c3+x2(c4 + x2(c5+x2c6)))))

    mulsd   %xmm2, %xmm5                             # r = 0.5 *x2
    movapd  %xmm5,%xmm0                              # r
    subsd   .L__real_3ff0000000000000(%rip),%xmm0    # -t=r-1.0    ;trash r
    movapd  %xmm0,%xmm9                              # -t
    addsd   .L__real_3ff0000000000000(%rip),%xmm0    # 1.0 + (-t)
    subsd   %xmm5,%xmm0                   # (( 1.0 - t) - r)
    subsd   %xmm4,%xmm0                   # (((1.0 - t) - r) - x * xx)
    addsd   %xmm1,%xmm0                   # (((1.0 - t) - r) - x * xx) + poly
    subsd   %xmm9,%xmm0
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.align 16
.Ladjust_region:      # positive or negative
#      switch (region)
   shr      $1, %eax
   mov      %eax, %ecx
   and      %r11d, %eax
   not      %ecx
   not      %r11d
   and      %r11d, %ecx
   or       %ecx, %eax
   and      $1, %eax
   jnz      .Lsin_cleanup

## if the original region 0, 1 and arg is negative, then we negate the result.
## if the original region 2, 3 and arg is positive, then we negate the result.
   movsd    %xmm0, %xmm2
   xorpd    %xmm0, %xmm0
   subsd    %xmm2, %xmm0

.align 16
.Lsin_cleanup:
   add      $stack_size, %rsp
   ret

.align 16
.Lsin_naninf:
   call     fname_special
   add      $stack_size, %rsp
   ret



