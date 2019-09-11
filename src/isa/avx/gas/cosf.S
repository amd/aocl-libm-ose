# An implementation of the cosf function.
#
# Prototype:
#
#     float fastcosf(float x);
#
#   Computes cosf(x).
#   Based on the NAG C implementation.
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.


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
.L__real_411E848000000000: .quad 0x415312d000000000   # 5e6 0x0411E848000000000  # 5e5
                        .quad 0

.align 32
.Lcsarray:
    .quad    0x0bfc5555555555555                      # -0.166667        s1
    .quad    0x03fa5555555555555                      # 0.0416667        c1
    .quad    0x03f81111111110bb3                      # 0.00833333       s2
    .quad    0x0bf56c16c16c16967                      # -0.00138889      c2
    .quad    0x0bf2a01a019e83e5c                      # -0.000198413     s3
    .quad    0x03efa01a019f4ec90                      # 2.48016e-005     c3
    .quad    0x03ec71de3796cde01                      # 2.75573e-006     s4
    .quad    0x0be927e4fa17f65f6                      # -2.75573e-007    c4

.text
.align 32
.p2align 5,,31

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(cosf)
#define fname_special _cosf_special@PLT


# define local variable storage offsets
.equ    p_temp,    0x30                               # temporary for get/put bits operation
.equ    p_temp1,   0x40                               # temporary for get/put bits operation
.equ    region,    0x50                               # pointer to region for amd_remainder_piby2
.equ    r,         0x60                               # pointer to r for amd_remainder_piby2
.equ   stack_size, 0x88

.globl fname
.type  fname,@function

fname:

    sub     $stack_size, %rsp

##  if NaN or inf
    movd    %xmm0, %edx
    mov     $0x07f800000, %eax
    mov     %eax, %r10d
    and     %edx, %r10d
    cmp     %eax, %r10d
    jz      .Lcosf_naninf

    xorpd   %xmm2, %xmm2
    mov     %rdx, %r11                                # save 1st return value pointer

#  GET_BITS_DP64(x, ux);
# convert input to double.
    cvtss2sd    %xmm0, %xmm0

# get the input value to an integer register.
    movsd   %xmm0,p_temp(%rsp)
    mov     p_temp(%rsp), %rdx                        # rdx is ux

#  ax = (ux & ~SIGNBIT_DP64);
    mov     $0x07fffffffffffffff, %r10
    and     %rdx, %r10                                # r10 is ax

    mov     $1, %r8d                                  # for determining region later on
    movsd   %xmm0, %xmm1                              # copy x to xmm1


##  if (ax <= 0x3fe921fb54442d18) /* abs(x) <= pi/4 */
    mov     $0x03fe921fb54442d18, %rax
    cmp     %rax, %r10
    jg      .L__sc_reducec

#          *c = cos_piby4(x, 0.0);
    movsd   %xmm0, %xmm2
    mulsd   %xmm2, %xmm2                              # x^2
    xor     %eax, %eax
    mov     %r10, %rdx
    movsd   .L__real_3fe0000000000000(%rip), %xmm5    # .5
    jmp     .L__sc_piby4c

.align 32
.L__sc_reducec:
# reduce  the argument to be in a range from -pi/4 to +pi/4
# by subtracting multiples of pi/2
#  xneg = (ax != ux);
    cmp     %r10, %rdx
##  if (xneg) x = -x;
    jz      .Lpositive
    subsd   %xmm0, %xmm2
    movsd   %xmm2, %xmm0

.Lpositive:
##  if (x < 5.0e5)
    cmp     .L__real_411E848000000000(%rip), %r10
    jae     .Lcosf_reduce_precise

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

# perform taylor series to calc cosx, cosx
# xmm0=abs(x), xmm1=x
.align 32
.Lcosf_piby4:
#/* How many pi/2 is x a multiple of? */
#        npi2  = (int)(x * twobypi + 0.5);

    movsd   %xmm0, %xmm2
    movsd   %xmm0, %xmm4

    mulsd   .L__real_3fe45f306dc9c883(%rip), %xmm2    # twobypi
    movsd   .L__real_3fe0000000000000(%rip), %xmm5    # .5

#/* How many pi/2 is x a multiple of? */

#      xexp  = ax >> EXPSHIFTBITS_DP64;
    mov     %r10, %r9
    shr     $52, %r9                                  # >> EXPSHIFTBITS_DP64

#        npi2  = (int)(x * twobypi + 0.5);
    addsd   %xmm5, %xmm2                              # npi2

    movsd   .L__real_3ff921fb54400000(%rip), %xmm3    # piby2_1
    cvttpd2dq    %xmm2, %xmm0                         # convert to integer
    movsd   .L__real_3dd0b4611a626331(%rip), %xmm1    # piby2_1tail
    cvtdq2pd    %xmm0, %xmm2                          # and back to double

#      /* Subtract the multiple from x to get an extra-precision remainder */
#      rhead  = x - npi2 * piby2_1;

    mulsd   %xmm2, %xmm3                              # use piby2_1
    subsd   %xmm3, %xmm4                              # rhead

#      rtail  = npi2 * piby2_1tail;
    mulsd   %xmm2, %xmm1                              # rtail
    movd    %xmm0, %eax

#      GET_BITS_DP64(rhead-rtail, uy);
    movsd   %xmm4, %xmm0
    subsd   %xmm1, %xmm0

    movsd   .L__real_3dd0b4611a600000(%rip), %xmm3    # piby2_2
    movsd   .L__real_3ba3198a2e037073(%rip), %xmm5    # piby2_2tail
    movd    %xmm0, %rcx                               # rcx is rhead-rtail

#      expdiff = xexp - ((uy & EXPBITS_DP64) >> EXPSHIFTBITS_DP64);
    shl     $1, %rcx                                  # strip any sign bit
    shr     $53, %rcx                                 # >> EXPSHIFTBITS_DP64 +1
    sub     %rcx, %r9                                 # expdiff

##      if (expdiff > 15)
    cmp     $15, %r9
    jle     .Lexpdiffless15

#          /* The remainder is pretty small compared with x, which
#             implies that x is a near multiple of pi/2
#             (x matches the multiple to at least 15 bits) */

#          t  = rhead;
    movsd   %xmm4, %xmm1

#          rtail  = npi2 * piby2_2;
    mulsd   %xmm2, %xmm3

#          rhead  = t - rtail;
    mulsd   %xmm2, %xmm5                              # npi2 * piby2_2tail
    subsd   %xmm3, %xmm4                              # rhead

#          rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
    subsd   %xmm4, %xmm1                              # t - rhead
    subsd   %xmm3, %xmm1                              # -rtail
    subsd   %xmm1, %xmm5                              # rtail

#      r = rhead - rtail;
    movsd   %xmm4, %xmm0


#xmm1=rtail
    movsd   %xmm5, %xmm1
    subsd   %xmm5, %xmm0

#    xmm0=r, xmm4=rhead, xmm1=rtail
.Lexpdiffless15:
#      region = npi2 & 3;

    movsd   %xmm0, %xmm2
    mulsd   %xmm0, %xmm2        #x^2
    movsd   %xmm0, %xmm1
    movsd   .L__real_3fe0000000000000(%rip), %xmm5    # .5

#    cmp     $0x03f2, %rcx                             # if r  small.
#    jge     .L__sc_piby4c                             # use taylor series if not
     jmp    .L__sc_piby4c

#if 0
    cmp     $0x03de, %rcx                             # if r really small.
    jle     .L__rc_small                              # then cos(r) = 1

##      if region is 1 or 3    do a sin calc.
    and     %eax, %r8d
    jz      .Lsinsmall
# region 1 or 3
# use simply polynomial
#              *s = x - x*x*x*0.166666666666666666;
    movsd   .L__real_3fc5555555555555(%rip), %xmm3
    mulsd   %xmm1, %xmm3                              # * x
    mulsd   %xmm2, %xmm3                              # * x^2
    subsd   %xmm3, %xmm1                              # xs
    jmp     .L__adjust_region_cos

.align 16
.Lsinsmall:
# region 0 or 2
#              cos = 1.0 - x*x*0.5;
    movsd   .L__real_3ff0000000000000(%rip), %xmm1    # 1.0
    mulsd   .L__real_3fe0000000000000(%rip), %xmm2    # 0.5 *x^2
    subsd   %xmm2, %xmm1
    jmp     .L__adjust_region_cos

.align 16
.L__rc_small:                                         # then sin(r) = r
##     if region is 1 or 3   do a sin calc.
   and      %eax, %r8d
   jnz      .L__adjust_region_cos
   movsd    .L__real_3ff0000000000000(%rip), %xmm1    # cos(r) is a 1
   jmp      .L__adjust_region_cos

#endif
# done with reducing the argument.  Now perform the sin/cos calculations.
.align 16
.L__sc_piby4c:
##     if region is 1 or 3   do a sin calc.
   and      %eax, %r8d
   jz       .Lcospiby4
   movsd    .Lcsarray+0x30(%rip), %xmm1   # s4
   mulsd    %xmm2, %xmm1                    # s4*x2
   addsd    .Lcsarray+0x20(%rip),%xmm1    # (s3 +s4*x2)
   mulsd    %xmm2, %xmm1                    # x2*(s3 +s4*x2)
   addsd    .Lcsarray+0x10(%rip),%xmm1    # s2 + (x2*(s3 +s4*x2))
   mulsd    %xmm2, %xmm1                    # x2*(s2 + (x2*(s3 +s4*x2)))
   addsd    .Lcsarray(%rip),%xmm1         # s1+(x2*(s2 + (x2*(s3 +s4*x2))))
   mulsd    %xmm2,%xmm1
   mulsd    %xmm0,%xmm1
   addsd    %xmm0,%xmm1
   jmp      .L__adjust_region_cos

.align 16
.Lcospiby4:
# region 0 or 2    - do a cos calculation
   movsd    .Lcsarray+0x38(%rip), %xmm1    # c4
   mulsd    %xmm2,%xmm1                      # c4*x2
   addsd    .Lcsarray+0x28(%rip), %xmm1    # c3+c4*x2
   mulsd    %xmm2,%xmm1                      # x2*(c3+c4*x2)
   addsd    .Lcsarray+0x18(%rip),%xmm1     # (c2+x2*(c3+c4*x2))
   mulsd    %xmm2,%xmm1                      # x2*(c2+x2(c3+c4*x2))
   addsd    .Lcsarray+0x08(%rip),%xmm1     # c1+(x2*(c2+x2(c3+c4*x2)))
   mulsd    %xmm2,%xmm1
   mulsd    %xmm2,%xmm1                      # x2*x2*c1+(x2*(c2+x2(c3+c4*x2)))
   mulsd    %xmm5,%xmm2          # 0.5 * x2
   subsd    .L__real_3ff0000000000000(%rip),%xmm2 #-(1.0 - 0.5*x2 )
   subsd    %xmm2,%xmm1

.L__adjust_region_cos:                                # xmm1 is cos or sin, relies on previous sections to
#     switch (region)
   add      $1, %eax
   and      $2, %eax
   jz       .L__cos_cleanup
## if region 1 or 2 then we negate the result.
   xorpd    %xmm2, %xmm2
   subsd    %xmm1, %xmm2
   movsd    %xmm2, %xmm1

.align 16
.L__cos_cleanup:
   cvtsd2ss %xmm1, %xmm0
   add      $stack_size, %rsp
   ret

.align 16
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lcosf_reduce_precise:
#     /* Reduce abs(x) into range [-pi/4,pi/4] */
#     __amd_remainder_piby2(ax, &r, &region);

   mov      %rdx,p_temp(%rsp)                         # save ux for use later
   mov      %r10,p_temp1(%rsp)                        # save ax for use later
   movd     %xmm0, %rdi
   lea      r(%rsp), %rsi
   lea      region(%rsp), %rdx
   sub      $0x020, %rsp

   call     __amd_remainder_piby2d2f@PLT

   add      $0x020, %rsp
   mov      p_temp(%rsp), %rdx                        # restore ux for use later
   mov      p_temp1(%rsp), %r10                       # restore ax for use later
   mov      $1, %r8d                                  # for determining region later on
   movsd    r(%rsp), %xmm0                            # r
   mov      region(%rsp), %eax                        # region

   movsd    %xmm0, %xmm2
   mulsd    %xmm0, %xmm2                              # x^2
   movsd    %xmm0, %xmm1
   movsd    .L__real_3fe0000000000000(%rip), %xmm5    # .5

   jmp      .L__sc_piby4c

.align 32
.Lcosf_naninf:
   call     fname_special
   add      $stack_size, %rsp
   ret
