#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# An implementation of the sincosf function.
#
# Prototype:
#
#     void fastsincosf(float x, float * sinfx, float * cosfx);
#
#   Computes sinf(x) and cosf(x).
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.
#   Based on the NAG C implementation.


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.section .rodata
.align 32
.L__real_3ff0000000000000: .quad 0x03ff0000000000000  # 1.0
                        .quad 0                       # for alignment
.L__real_3fe0000000000000: .quad 0x03fe0000000000000  # 0.5
                        .quad 0
.L__real_3fc5555555555555: .quad 0x03fc5555555555555  # 0.166666666666
                        .quad 0
.L__real_3fe45f306dc9c883: .quad 0x03fe45f306dc9c883  # twobypi
                        .quad 0
.L__real_3FF921FB54442D18: .quad 0x03FF921FB54442D18  # piby2
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
.L__real_8000000000000000: .quad 0x08000000000000000  # -0  or signbit
                        .quad 0
.L__real_411E848000000000: .quad 0x415312d000000000   # 5e6 0x0411E848000000000  # 5e5
                        .quad 0

.align 32
.Lcsarray:
    .quad    0x0bfc5555555555555                      # -0.166667           s1
    .quad    0x03fa5555555555555                      # 0.0416667           c1
    .quad    0x03f81111111110bb3                      # 0.00833333          s2
    .quad    0x0bf56c16c16c16967                      # -0.00138889         c2
    .quad    0x0bf2a01a019e83e5c                      # -0.000198413        s3
    .quad    0x03efa01a019f4ec90                      # 2.48016e-005        c3
    .quad    0x03ec71de3796cde01                      # 2.75573e-006        s4
    .quad    0x0be927e4fa17f65f6                      # -2.75573e-007       c4
    .quad    0x0be5ae600b42fdfa7                      # -2.50511e-008       s5
    .quad    0x03e21eeb69037ab78                      # 2.08761e-009        c5
    .quad    0x03de5e0b2f9a43bb8                      # 1.59181e-010        s6
    .quad    0x0bda907db46cc5e42                      # -1.13826e-011       c6

.text
.align 16
.p2align 4,,15

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(sincosf)
#define fname_special _sincosf_special@PLT
#define fname_special_underflow _sin_special_underflow@PLT



# define local variable storage offsets
.equ   p_temp,     0x30                               # temporary for get/put bits operation
.equ   p_temp1,    0x40                               # temporary for get/put bits operation
.equ   p_temp2,    0x50                               # temporary for get/put bits operation
.equ   p_temp3,    0x60                               # temporary for get/put bits operation
.equ   region,     0x70                               # pointer to region for amd_remainder_piby2
.equ   r,          0x80                               # pointer to r for amd_remainder_piby2
.equ    p_rdi,     0x90                               # temporary for get/put bits operation
.equ    p_rsi,      0xa0                               # temporary for get/put bits operation
.equ   stack_size, 0xb8

.globl fname
.type  fname,@function

fname:
    sub     $stack_size, %rsp

    xorpd   %xmm2,%xmm2

#  GET_BITS_DP64(x, ux);
# convert input to double.
    cvtss2sd    %xmm0,%xmm0
# get the input value to an integer register.
    movsd   %xmm0,p_temp(%rsp)
    mov     p_temp(%rsp),%rdx                         # rdx is ux

##  if NaN or inf
    mov     $0x07ff0000000000000,%rax
    mov     %rax,%r10
    and     %rdx,%r10
    cmp     %rax,%r10
    jz      .L__sc_naninf

#  ax = (ux & ~SIGNBIT_DP64);
    mov     $0x07fffffffffffffff,%r10
    and     %rdx,%r10                                 # r10 is ax

##  if (ax <= 0x3fe921fb54442d18) /* abs(x) <= pi/4 */
    mov     $0x03fe921fb54442d18,%rax
    cmp     %rax,%r10
    jg      .L__sc_reduce

##          if (ax < 0x3f20000000000000) /* abs(x) < 2.0^(-13) */
   mov      $0x3f20000000000000, %rax
   cmp      %rax, %r10
   jge      .L__sc_notsmallest



#                  sinf = x, cos=1.0
   movsd    .L__real_3ff0000000000000(%rip),%xmm1

##  Additional code to raise underflow exception
    xor     %rax,%rax
    cmp     %rax,%r10
    je      .L__sc_cleanup
    mov     %rdi,p_rdi(%rsp)
    mov     %rsi,p_rsi(%rsp)
    call    fname_special_underflow
    mov     p_rdi(%rsp),%rdi
    mov     p_rsi(%rsp),%rsi
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # cos = 1.0;
    jmp     .L__sc_cleanup

#          *s = sin_piby4(x, 0.0);
#          *c = cos_piby4(x, 0.0);
.L__sc_notsmallest:
    xor     %eax,%eax                                 # region 0
    mov     %r10,%rdx
    movsd   .L__real_3fe0000000000000(%rip),%xmm5     # .5
    jmp     .L__sc_piby4

.L__sc_reduce:

# reduce  the argument to be in a range from -pi/4 to +pi/4
# by subtracting multiples of pi/2

#  xneg = (ax != ux);
    cmp     %r10,%rdx
##  if (xneg) x = -x;
    jz      .Lpositive
    subsd   %xmm0,%xmm2
    movsd   %xmm2,%xmm0

.align 16
.Lpositive:
##  if (x < 5.0e5)
    cmp     .L__real_411E848000000000(%rip),%r10
    jae     .Lsincosf_reduce_precise

    movsd   %xmm0,%xmm2
    movsd   %xmm0,%xmm4

    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm2     # twobypi
    movsd   .L__real_3fe0000000000000(%rip),%xmm5     # .5

#/* How many pi/2 is x a multiple of? */
#      xexp  = ax >> EXPSHIFTBITS_DP64;
    mov     %r10,%r9
    shr     $52,%r9                                   # >> EXPSHIFTBITS_DP64

#        npi2  = (int)(x * twobypi + 0.5);
    addsd   %xmm5,%xmm2                               # npi2

    movsd   .L__real_3ff921fb54400000(%rip),%xmm3     # piby2_1
    cvttpd2dq    %xmm2,%xmm0                          # convert to integer
    movsd   .L__real_3dd0b4611a626331(%rip),%xmm1     # piby2_1tail
    cvtdq2pd    %xmm0,%xmm2                           # and back to float.

#      /* Subtract the multiple from x to get an extra-precision remainder */
#      rhead  = x - npi2 * piby2_1;
#      /* Subtract the multiple from x to get an extra-precision remainder */
#       rhead  = x - npi2 * piby2_1;

    mulsd   %xmm2,%xmm3                               # use piby2_1
    subsd   %xmm3,%xmm4                               # rhead

#      rtail  = npi2 * piby2_1tail;
    mulsd   %xmm2,%xmm1                               # rtail

    movd    %xmm0,%eax

#      GET_BITS_DP64(rhead-rtail, uy);               ; originally only rhead
    movsd   %xmm4,%xmm0
    subsd   %xmm1,%xmm0

    movsd   .L__real_3dd0b4611a600000(%rip),%xmm3     # piby2_2
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm5     # piby2_2tail
    movd    %xmm0,%rcx

#      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     $1,%rcx                                   # strip any sign bit
    shr     $53,%rcx                                  # >> EXPSHIFTBITS_DP64 +1
    sub     %rcx,%r9                                  # expdiff

##      if (expdiff > 15)
    cmp     $15,%r9
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

#      region = npi2 & 3;
#    and    $3,%eax
#    xmm0=r, xmm4=rhead, xmm1=rtail
.Lexpdiff15:

## if the input was close to a pi/2 multiple
#

#    cmp     $0x03f2,%rcx                              # if r  small.
#    jge     .L__sc_piby4                              # use taylor series if not
    jmp     .L__sc_piby4

#if 0
    cmp     $0x03de,%rcx                              # if r really small.
    jle     .Lsinsmall                                # then sin(r) = r

    movsd   %xmm0,%xmm2
    mulsd   %xmm2,%xmm2                               # x^2
# use simply polynomial
#              *s = x - x*x*x*0.166666666666666666;
    movsd   .L__real_3fc5555555555555(%rip),%xmm3     #
    mulsd   %xmm0,%xmm3                               # * x
    mulsd   %xmm2,%xmm3                               # * x^2
    subsd   %xmm3,%xmm0                               # xs

#              *c = 1.0 - x*x*0.5;
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # 1.0
    mulsd   .L__real_3fe0000000000000(%rip),%xmm2     # 0.5 *x^2
    subsd   %xmm2,%xmm1
    jmp     .L__adjust_region

.Lsinsmall:                                           # then sin(r) = r
    movsd   .L__real_3ff0000000000000(%rip),%xmm1     # cos(r) is a 1
    jmp     .L__adjust_region

#endif

# perform taylor series to calc sinx, cosx
#  COS
#  x2 = x * x;
#  return (1.0 - 0.5 * x2 + (x2 * x2 *
#                            (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))));
#  x2 = x * x;
#  return (1.0 - 0.5 * x2 + (x2 * x2 *
#                            (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))));
#  SIN
#  zc,zs = (c2 + x2 * (c3 + x2 * c4 ));
#  xs =  r + x3 * (sc1 + x2 * zs);
#  x2 = x * x;
#  return (x + x * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));
# done with reducing the argument.  Now perform the sin/cos calculations.
.align 16
.L__sc_piby4:


 #   (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)
#sin (x + x *                x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));
#cos (1.0 - 0.5 * x2 + (x2 * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))));


    movsd   %xmm0,%xmm2
    mulsd   %xmm0,%xmm2                               # x2
    shufpd  $0,%xmm2,%xmm2                            # x2,x2
    movapd   %xmm2,%xmm3

    mulpd .Lcsarray+0x30(%rip),%xmm3
    addpd .Lcsarray+0x20(%rip),%xmm3
    mulpd  %xmm2,%xmm3
    addpd .Lcsarray+0x10(%rip),%xmm3
    mulpd  %xmm2,%xmm3
    addpd .Lcsarray(%rip),%xmm3
    mulpd %xmm2,%xmm3                   # x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4)))

    movhlps   %xmm3,%xmm1               # xmm3 sin xmm1 cos
    mulsd   %xmm0,%xmm3
    addsd   %xmm3,%xmm0                 # xmm0 is sin (x + x * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));

    mulsd   %xmm2,%xmm1
    mulsd .L__real_3fe0000000000000(%rip),%xmm2
    subsd .L__real_3ff0000000000000(%rip),%xmm2             # 0.5*x2 - 1.0

    subsd %xmm2,%xmm1                                       # xmm1 -( -(1 - 0.5 *x2))



#if 0
#  x2 = r * r;
    movsd   .L__real_3fe0000000000000(%rip),%xmm5     # .5
    movsd   %xmm0,%xmm2
    mulsd   %xmm0,%xmm2                               # x2
    shufpd  $0,%xmm2,%xmm2                            # x2,x2
    movsd   %xmm2,%xmm4
    mulsd   %xmm4,%xmm4                               # x4
    shufpd  $0,%xmm4,%xmm4                            # x4,x4

#  x2m =    _mm_set1_pd (x2);
#  zc,zs = (c2 + x2 * (c3 + x2 * c4 ));
#    xs =  r + x3 * (sc1 + x2 * zs);
#    xc =  t + ( x2 * x2 * (cc1 + x2 * zc));
    movapd  .Lcsarray+0x30(%rip),%xmm1                # c4
    movapd  .Lcsarray+0x10(%rip),%xmm3                # c2
    mulpd   %xmm2,%xmm1                               # x2c4
    mulpd   %xmm2,%xmm3                               # x2c2

#  rc = 0.5 * x2;
    mulsd   %xmm2,%xmm5                               #rc
    mulsd   %xmm0,%xmm2                               #x3

    addpd   .Lcsarray+0x20(%rip),%xmm1                # c3 + x2c4
    addpd   .Lcsarray(%rip),%xmm3                     # c1 + x2c2
    mulpd   %xmm4,%xmm1                               #    x4(c3 + x2c4)
    addpd   %xmm3,%xmm1                               # c1 + x2c2 + x4(c3 + x2c4)

#  -t = rc-1;
    subsd   .L__real_3ff0000000000000(%rip),%xmm5     # 1.0
# now we have the poly for sin in the low half, and cos in upper half
    mulsd   %xmm1,%xmm2                               # x3(sin poly)
    shufpd  $3,%xmm1,%xmm1                            # get cos poly to low half of register
    mulsd   %xmm4,%xmm1                               # x4(cos poly)

    addsd   %xmm2,%xmm0                               # sin = r+...
    subsd   %xmm5,%xmm1                               # cos = poly-(-t)

#endif
.L__adjust_region:                                    # xmm0 is sin, xmm1 is cos
#      switch (region)
    mov     %eax,%ecx
    and     $1,%eax
    jz      .Lregion02
# region 1 or 3
    movsd   %xmm0,%xmm2                               # swap sin,cos
    movsd   %xmm1,%xmm0                               # sin = cos
    xorpd   %xmm1,%xmm1
    subsd   %xmm2,%xmm1                               # cos = -sin

.Lregion02:
    and     $2,%ecx
    jz      .Lregion23
# region 2 or 3
    movsd   %xmm0,%xmm2
    movsd   %xmm1,%xmm3
    xorpd   %xmm0,%xmm0
    xorpd   %xmm1,%xmm1
    subsd   %xmm2,%xmm0                               # sin = -sin
    subsd   %xmm3,%xmm1                               # cos = -cos

.Lregion23:
##  if (xneg) *s = -*s ;
    cmp     %r10,%rdx
    jz      .L__sc_cleanup
    movsd   %xmm0,%xmm2
    xorpd   %xmm0,%xmm0
    subsd   %xmm2,%xmm0                               # sin = -sin

.align 16
.L__sc_cleanup:
    cvtsd2ss %xmm0,%xmm0                              # convert back to floats
    cvtsd2ss %xmm1,%xmm1

    movss   %xmm0,(%rdi)                              # save the sin
    movss   %xmm1,(%rsi)                              # save the cos

    add     $stack_size,%rsp
    ret

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsincosf_reduce_precise:
#      /* Reduce abs(x) into range [-pi/4,pi/4] */
#      __amd_remainder_piby2(ax, &r, &region);

    mov     %rdx,p_temp(%rsp)                         # save ux for use later
    mov     %r10,p_temp1(%rsp)                        # save ax for use later
    mov     %rdi,p_temp2(%rsp)                        # save ux for use later
    mov     %rsi,p_temp3(%rsp)                        # save ax for use later
    movd    %xmm0,%rdi
    lea     r(%rsp),%rsi
    lea     region(%rsp),%rdx
    sub     $0x040,%rsp

    call    __amd_remainder_piby2d2f@PLT

    add     $0x040,%rsp
    mov     p_temp(%rsp),%rdx                         # restore ux for use later
    mov     p_temp1(%rsp),%r10                        # restore ax for use later
    mov     p_temp2(%rsp),%rdi                        # restore ux for use later
    mov     p_temp3(%rsp),%rsi                        # restore ax for use later

    mov     $1,%r8d                                   # for determining region later on
    movsd   r(%rsp),%xmm0                             # r
    mov     region(%rsp),%eax                         # region
    jmp     .L__sc_piby4

.align 16
.L__sc_naninf:
    cvtsd2ss %xmm0,%xmm0                              # convert back to floats
    call    fname_special                             # rdi and rsi are ready for the function call
    add     $stack_size, %rsp
    ret
