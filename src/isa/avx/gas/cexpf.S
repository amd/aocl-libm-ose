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
# An implementation of the complex expf function.
#
# Prototype:
#
#   float complex cexpf(float complex x)
#
#   Computes cexpf
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.
#   Based on the sincosf and expf implementation.
#
#

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15

#include "fn_macros.h"
#define fname ALM_PROTO_BAS64(cexpf)
#define fname_special1 _sincosf_special@PLT
#define fname_special2 alm_expf_special@PLT

# define local variable storage offsets
.equ   p_temp0,    0x30                               # temporary for get/put bits operation
.equ   p_temp1,    0x40                               # temporary for get/put bits operation
.equ   p_temp2,    0x50                               # temporary for get/put bits operation
.equ   region,     0x60                               # pointer to region for amd_remainder_piby2
.equ   r,          0x70                               # pointer to r for amd_remainder_piby2
.equ   stack_size, 0x98

.globl fname
ALM_FUNC_TYPE_ASM(f_name)

#include "weak_macros.h"

fname:
    sub     $stack_size, %rsp
    
#start the expf calculation
.L_expf:
	cvtps2pd    %xmm0, %xmm1
    movhpd      %xmm1, p_temp0(%rsp)                  # save image part for sincosf usage	
    
    movd        %xmm0, %edx
    mov         .L__real_inf(%rip), %eax
    and         %edx, %eax
    cmp         .L__real_inf(%rip), %eax
    je          .L__x_is_inf_or_nan

    cvtss2sd    %xmm0, %xmm0
    
    # x * (64/ln(2))
    movsd       .L__real_64_by_log2(%rip), %xmm3
    mulsd       %xmm0, %xmm3

    # x <= 128*ln(2), ( x * (64/ln(2)) ) <= 64*128
    # x > -150*ln(2), ( x * (64/ln(2)) ) > 64*(-150)
    comisd      .L__real_p8192(%rip), %xmm3
    jae         .L__y_is_inf

    comisd      .L__real_m9600(%rip), %xmm3
    jb          .L__y_is_zero

    # n = int( x * (64/ln(2)) )
    cvtpd2dq    %xmm3, %xmm4
    lea         .L__two_to_jby64_table(%rip), %r10
    cvtdq2pd    %xmm4, %xmm1

    # r = x - n * ln(2)/64
    movsd       .L__real_log2_by_64(%rip), %xmm2
    mulsd       %xmm1, %xmm2
    movd        %xmm4, %ecx
    mov         $0x3f, %rax
    and         %ecx, %eax
    subsd       %xmm2, %xmm0
    movsd       %xmm0, %xmm1

    # m = (n - j) / 64
    sub         %eax, %ecx
    sar         $6, %ecx

    # q
    movsd       .L__real_1_by_6(%rip), %xmm3
    mulsd       %xmm0, %xmm3
    mulsd       %xmm0, %xmm0
    addsd       .L__real_1_by_2(%rip), %xmm3
    mulsd       %xmm3, %xmm0
    addsd       %xmm1, %xmm0

    add         $1023, %rcx
    shl         $52, %rcx

    # (f)*(1+q)
    movsd       (%r10,%rax,8), %xmm2
    mulsd       %xmm2, %xmm0
    addsd       %xmm2, %xmm0

    mov         %rcx, p_temp2(%rsp)
    mulsd       p_temp2(%rsp), %xmm0
#   cvtsd2ss    %xmm0, %xmm0          #calculation not done yet
#	movsd       %xmm0, p_temp2(%rsp)                  # save xmm0(exp(x)) to p_temp2
	jmp         .L__SinCosf

.p2align 4,,15
.L__y_is_zero:

    movss       .L__real_zero(%rip), %xmm1
    movd        %edx, %xmm0
    mov         .L__flag_y_zero(%rip), %edi
	
    call        fname_special2
    movsd       .L__real_zero(%rip), %xmm0
    jmp         .L__SinCosf                    # multiplication is needed even 0, for preserving sign          

.p2align 4,,15
.L__y_is_inf:

    movss       .L__real_inf(%rip), %xmm1
    movd        %edx, %xmm0
    mov         .L__flag_y_inf(%rip), %edi

    call        fname_special2
    movsd       .L__real_7ff0000000000000(%rip), %xmm0
    jmp         .L__SinCosf      

.p2align 4,,15
.L__x_is_inf_or_nan:

    cmp         .L__real_inf(%rip), %edx
    movsd       .L__real_7ff0000000000000(%rip), %xmm0
    je          .L__SinCosf

    cmp         .L__real_ninf(%rip), %edx
    movsd       .L__real_zero(%rip), %xmm0
	je          .L__SinCosf

    mov         .L__real_qnanbit(%rip), %r9d
    and         %edx, %r9d
    movsd       .L__real_7ff8000000000000(%rip), %xmm0
    jnz         .L__SinCosf

	movd        %edx, %xmm0
    or          .L__real_qnanbit(%rip), %edx
    movd        %edx, %xmm1
    mov         .L__flag_x_nan(%rip), %edi
    call        fname_special2
	movsd       .L__real_7ff8000000000000(%rip), %xmm0
#    jmp         .L__SinCosf    

.L__SinCosf:
    movsd       %xmm0, p_temp2(%rsp)          # save xmm0(exp(x)) to p_temp2
##############################################################
# start the sincosf calculation    
    xorpd   %xmm2,%xmm2
    
#  GET_BITS_DP64(x, ux);
# convert input to double.
    movsd   p_temp0(%rsp), %xmm0
# get the input value to an integer register.
#   movsd   %xmm0,p_temp1(%rsp)
    mov     p_temp0(%rsp),%rdx                         # rdx is ux

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
   jmp      .L__sc_cleanup
   
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

    cmp     $0x03f2,%rcx                              # if r  small.
    jge     .L__sc_piby4                              # use taylor series if not
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
	movsd   p_temp2(%rsp), %xmm3 					 # recover expf(x)
	mulsd   %xmm3, %xmm1
	mulsd   %xmm3, %xmm0
	
# we need exp(x)cos(y) for xmm0, exp(x)sin(y) for xmm1, swap
# and convert back to floats
	movlhps  %xmm0, %xmm0
	movsd    %xmm1, %xmm0
	cvtpd2ps %xmm0, %xmm0 

    add     $stack_size,%rsp
    ret

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsincosf_reduce_precise:
#      /* Reduce abs(x) into range [-pi/4,pi/4] */
#      __amd_remainder_piby2(ax, &r, &region);

    mov     %rdx,p_temp0(%rsp)                        # save ux for use later
    mov     %r10,p_temp1(%rsp)                        # save ax for use later
    movd    %xmm0,%rdi
    lea     r(%rsp),%rsi
    lea     region(%rsp),%rdx
    sub     $0x040,%rsp    

    call    __amd_remainder_piby2d2f@PLT

    add     $0x040,%rsp
    mov     p_temp0(%rsp),%rdx                        # restore ux for use later
    mov     p_temp1(%rsp),%r10                        # restore ax for use later    

    mov     $1,%r8d                                   # for determining region later on
    movsd   r(%rsp),%xmm0                             # r
    mov     region(%rsp),%eax                         # region
    jmp     .L__sc_piby4

# exception handle for sincosf  
.align 16   
.L__sc_naninf:
    cvtsd2ss %xmm0,%xmm0                              # convert back to floats
    lea      region(%rsp), %rdi
    lea      r(%rsp), %rsi 
    call     fname_special1

    movsd    p_temp2(%rsp), %xmm3         		      # recover exp(x)
    mov      p_temp2(%rsp), %rax 
    mov      $0x07fffffffffffffff, %rcx 
    and      %rcx, %rax
    jz       .LZero_Nan
    
    cvtss2sd %xmm0, %xmm0
	mulsd    %xmm3, %xmm0
	cvtsd2ss %xmm0, %xmm0
	 
	movss    %xmm0, p_temp1(%rsp)
	movss    %xmm0, p_temp1+4(%rsp)
	movsd    p_temp1(%rsp), %xmm0      
    add      $stack_size, %rsp
    ret

.LZero_Nan:
	xorpd    %xmm0, %xmm0
    add      $stack_size, %rsp
    ret
    
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
.L__real_411E848000000000: .quad 0x0411E848000000000  # 5e5
                        .quad 0
.L__real_7ff0000000000000: .quad 0x07ff0000000000000  # inf
                        .quad 0
.L__real_7ff8000000000000: .quad 0x07ff8000000000000  # qnan
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

.align 16
# these codes and the ones in the corresponding .c file have to match
.L__flag_x_nan:         .long 00000001
.L__flag_y_zero:        .long 00000002
.L__flag_y_inf:         .long 00000003

.align 16
.L__real_inf:                   .long 0x7f800000
                                .long 0
                                .quad 0
.L__real_ninf:                  .long 0x0ff800000
                                .long 0
                                .quad 0
.L__real_qnanbit:               .long 0x00400000
                                .long 0
                                .quad 0
.L__real_zero:                  .quad 0x0000000000000000
								.quad 0
.L__real_p8192:                 .quad 0x40c0000000000000
                                .quad 0
.L__real_m9600:                 .quad 0x0c0c2c00000000000
                                .quad 0
.L__real_64_by_log2:            .quad 0x40571547652b82fe # 64/ln(2)
                                .quad 0
.L__real_log2_by_64:            .quad 0x3f862e42fefa39ef # log2_by_64
                                .quad 0
.L__real_1_by_6:                .quad 0x3fc5555555555555 # 1/6
                                .quad 0
.L__real_1_by_2:                .quad 0x3fe0000000000000 # 1/2
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
