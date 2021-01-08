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
# vrs4powf.s
#
# A vector implementation of the powf libm function.
#
# Prototype:
#
#     __m128 __vrs4_powf(__m128 x,__m128 y);
#
#   Computes x raised to the y power.  Returns proper C99 values.
#   Uses new tuned fastlog/fastexp.
#
#

#include "fn_macros.h"
#include "weak_macros.h"

#define fname FN_PROTOTYPE_BAS64(vrs4_powf)

#define fname_exp __amd_internal_vrd4_exp
#define fname_log __amd_internal_vrd4_log

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif


# define local variable storage offsets
.equ    p_temp,0x00     # xmmword
.equ    p_negateres,0x10        # qword

.equ    p_xexp,0x20     # qword

.equ    p_ux,0x030      # storage for X
.equ    p_uy,0x040      # storage for Y

.equ    p_ax,0x050      # absolute x
.equ    p_sx,0x060      # sign of x's

.equ    p_ay,0x070      # absolute y
.equ    p_yexp,0x080        # unbiased exponent of y

.equ    p_inty,0x090        # integer y indicators
.equ    save_rbx,0x0A0          #

.equ    stack_size,0x0B8    # allocate 40h more than
                # we need to avoid bank conflicts



    .text
    .align 16
    .p2align 4,,15
.globl fname
    .type   fname,@function
fname:
    sub     $stack_size,%rsp
        mov             %rbx,save_rbx(%rsp)     # save rbx

    movaps    %xmm0,p_ux(%rsp)      # save x
    movaps    %xmm1,p_uy(%rsp)      # save y

    movaps  %xmm0,%xmm2
    andps   .L__mask_nsign(%rip),%xmm0      # get abs x
    andps   .L__mask_sign(%rip),%xmm2       # mask for the sign bits
    movaps    %xmm0,p_ax(%rsp)      # save them
    movaps    %xmm2,p_sx(%rsp)      # save them
# convert all four x's to double
    cvtps2pd   p_ax(%rsp),%xmm0
    cvtps2pd   p_ax+8(%rsp),%xmm1
#
# classify y
# vector 32 bit integer method   25 cycles to here
#  /* See whether y is an integer.
#     inty = 0 means not an integer.
#     inty = 1 means odd integer.
#     inty = 2 means even integer.
#  */
    movdqa  p_uy(%rsp),%xmm4
    pxor    %xmm3,%xmm3
    pand    .L__mask_nsign(%rip),%xmm4      # get abs y in integer format
    movdqa    %xmm4,p_ay(%rsp)          # save it

# see if the number is less than 1.0
    psrld   $23,%xmm4           #>> EXPSHIFTBITS_SP32

    psubd   .L__mask_127(%rip),%xmm4            # yexp, unbiased exponent
    movdqa    %xmm4,p_yexp(%rsp)        # save it
    paddd   .L__mask_1(%rip),%xmm4          # yexp+1
    pcmpgtd %xmm3,%xmm4     # 0 if exp less than 126 (2^0) (y < 1.0), else FFs
# xmm4 is ffs if abs(y) >=1.0, else 0

# see if the mantissa has fractional bits
#build mask for mantissa
    movdqa  .L__mask_23(%rip),%xmm2
    psubd   p_yexp(%rsp),%xmm2      # 24-yexp
    pmaxsw  %xmm3,%xmm2                         # no shift counts less than 0
    movdqa    %xmm2,p_temp(%rsp)        # save the shift counts
# create mask for all four values
# SSE can't individual shifts so have to do 0xeac one seperately
    mov     p_temp(%rsp),%rcx
    mov     $1,%rbx
    shl     %cl,%ebx            #1 << (24 - yexp)
    shr     $32,%rcx
    mov     $1,%eax
    shl     %cl,%eax            #1 << (24 - yexp)
    shl     $32,%rax
    add     %rax,%rbx
    mov     %rbx,p_temp(%rsp)
    mov     p_temp+8(%rsp),%rcx
    mov     $1,%rbx
    shl     %cl,%ebx            #1 << (24 - yexp)
    shr     $32,%rcx
    mov     $1,%eax
    shl     %cl,%eax            #1 << (24 - yexp)
    shl     $32,%rax
    add     %rbx,%rax
    mov     %rax,p_temp+8(%rsp)
    movdqa  p_temp(%rsp),%xmm5
    psubd   .L__mask_1(%rip),%xmm5  #= mask = (1 << (24 - yexp)) - 1

# now use the mask to see if there are any fractional bits
    movdqa  p_uy(%rsp),%xmm2 # get uy
    pand    %xmm5,%xmm2     # uy & mask
    pcmpeqd %xmm3,%xmm2     # 0 if not zero (y has fractional mantissa bits), else FFs
    pand    %xmm4,%xmm2     # either 0s or ff
# xmm2 now accounts for y< 1.0 or y>=1.0 and y has fractional mantissa bits,
# it has the value 0 if we know it's non-integer or ff if integer.

# now see if it's even or odd.

## if yexp > 24, then it has to be even
    movdqa  .L__mask_24(%rip),%xmm4
    psubd   p_yexp(%rsp),%xmm4      # 24-yexp
    paddd   .L__mask_1(%rip),%xmm5  # mask+1 = least significant integer bit
    pcmpgtd %xmm3,%xmm4      ## if 0, then must be even, else ff's

    pand    %xmm4,%xmm5     # set the integer bit mask to zero if yexp>24
    paddd   .L__mask_2(%rip),%xmm4
    por     .L__mask_2(%rip),%xmm4
    pand    %xmm2,%xmm4      # result can be 0, 2, or 3

# now for integer numbers, see if odd or even
    pand    .L__mask_mant(%rip),%xmm5   # mask out exponent bits
    movdqa  .L__float_one(%rip),%xmm2
    pand    p_uy(%rsp),%xmm5 #  & uy -> even or odd
    pcmpeqd p_ay(%rsp),%xmm2    # is ay equal to 1, ff's if so, then it's odd
    pand    .L__mask_nsign(%rip),%xmm2 # strip the sign bit so the gt comparison works.
    por     %xmm2,%xmm5
    pcmpgtd %xmm3,%xmm5      ## if odd then ff's, else 0's for even
    paddd   .L__mask_2(%rip),%xmm5 # gives us 2 for even, 1 for odd
    pand    %xmm5,%xmm4

    movdqa        %xmm4,p_inty(%rsp)        # save inty
#
# do more x special case checking
#
    movdqa  %xmm4,%xmm5
    pcmpeqd %xmm3,%xmm5                     # is not an integer? ff's if so
    pand    .L__mask_NaN(%rip),%xmm5        # these values will be NaNs, if x<0
    movdqa  %xmm4,%xmm2
    pcmpeqd .L__mask_1(%rip),%xmm2      # is it odd? ff's if so
    pand    .L__mask_sign(%rip),%xmm2   # these values will get their sign bit set
    por     %xmm2,%xmm5

    pcmpeqd p_sx(%rsp),%xmm3        ## if the signs are set
    pandn   %xmm5,%xmm3                     # then negateres gets the values as shown below
    movdqa    %xmm3,p_negateres(%rsp)   # save negateres

#  /* p_negateres now means the following.
#  ** 7FC00000 means x<0, y not an integer, return NaN.
#  ** 80000000 means x<0, y is odd integer, so set the sign bit.
#  ** 0 means even integer, and/or x>=0.
#  */


# **** Here starts the main calculations  ****
# The algorithm used is x**y = exp(y*log(x))
#  Extra precision is required in intermediate steps to meet the 1ulp requirement
#
# log(x) calculation
    call        fname_log@PLT                   # get the double precision log value
                                            # for all four x's
# y* logx
# convert all four y's to double
    lea p_uy(%rsp),%rdx     # get pointer to y
    cvtps2pd   (%rdx),%xmm2
    cvtps2pd   8(%rdx),%xmm3

#  /* just multiply by y */
    mulpd   %xmm2,%xmm0
    mulpd   %xmm3,%xmm1

#  /* The following code computes r = exp(w) */
    call        fname_exp@PLT                   # get the double exp value
                                            # for all four y*log(x)'s
#
# convert all four results to double
    cvtpd2ps    %xmm0,%xmm0
    cvtpd2ps    %xmm1,%xmm1
    movlhps     %xmm1,%xmm0

# perform special case and error checking on input values

# special case checking is done first in the scalar version since
# it allows for early fast returns.  But for vectors, we consider them
# to be rare, so early returns are not necessary.  So we first compute
# the x**y values, and then check for special cases.

# we do some of the checking in reverse order of the scalar version.
    lea p_uy(%rsp),%rdx     # get pointer to y
# apply the negate result flags
    orps    p_negateres(%rsp),%xmm0 # get negateres

## if y is infinite or so large that the result would overflow or underflow
    movdqa  p_ay(%rsp),%xmm4
    cmpps   $5,.L__mask_ly(%rip),%xmm4  # y not less than large value, ffs if so.
    movmskps %xmm4,%edx
    test    $0x0f,%edx
    jnz     .Ly_large
.Lrnsx3:

## if x is infinite
    movdqa  p_ax(%rsp),%xmm4
    cmpps   $0,.L__mask_inf(%rip),%xmm4 # equal to infinity, ffs if so.
    movmskps %xmm4,%edx
    test    $0x0f,%edx
    jnz     .Lx_infinite
.Lrnsx1:
## if x is zero
    xorps   %xmm4,%xmm4
    cmpps   $0,p_ax(%rsp),%xmm4 # equal to zero, ffs if so.
    movmskps %xmm4,%edx
    test    $0x0f,%edx
    jnz     .Lx_zero
.Lrnsx2:
## if y is NAN
    lea     p_uy(%rsp),%rdx     # get pointer to y
    movdqa  (%rdx),%xmm4            # get y
    cmpps   $4,%xmm4,%xmm4                      # a compare not equal  of y to itself should
                                            # be false, unless y is a NaN. ff's if NaN.
    movmskps %xmm4,%ecx
    test    $0x0f,%ecx
    jnz     .Ly_NaN
.Lrnsx4:
## if x is NAN
    lea     p_ux(%rsp),%rdx     # get pointer to x
    movdqa  (%rdx),%xmm4            # get x
    cmpps   $4,%xmm4,%xmm4                      # a compare not equal  of x to itself should
                                            # be false, unless x is a NaN. ff's if NaN.
    movmskps %xmm4,%ecx
    test    $0x0f,%ecx
    jnz     .Lx_NaN
.Lrnsx5:

## if |y| == 0  then return 1
    movdqa  .L__float_one(%rip),%xmm3   # one
    xorps   %xmm2,%xmm2
    cmpps   $4,p_ay(%rsp),%xmm2 # not equal to 0.0?, ffs if not equal.
    andps   %xmm2,%xmm0                     # keep the others
    andnps  %xmm3,%xmm2                     # mask for ones
    orps    %xmm2,%xmm0
## if x == +1, return +1 for all x
    lea     p_ux(%rsp),%rdx     # get pointer to x
    movdqa  %xmm3,%xmm2
    cmpps   $4,(%rdx),%xmm2     # not equal to +1.0?, ffs if not equal.
    andps   %xmm2,%xmm0                     # keep the others
    andnps  %xmm3,%xmm2                     # mask for ones
    orps    %xmm2,%xmm0

.L__powf_cleanup2:
        mov             save_rbx(%rsp),%rbx             # restore rbx
    add     $stack_size,%rsp
    ret

    .align 16
#       y is a NaN.
.Ly_NaN:
    lea p_uy(%rsp),%rdx     # get pointer to y
    movdqa  (%rdx),%xmm4            # get y
    movdqa  %xmm4,%xmm3
    movdqa  %xmm4,%xmm5
    movdqa  .L__mask_sigbit(%rip),%xmm2 # get the signalling bits
    cmpps   $0,%xmm4,%xmm4                      # a compare equal  of y to itself should
                                            # be true, unless y is a NaN. 0's if NaN.
    cmpps   $4,%xmm3,%xmm3                      # compare not equal, ff's if NaN.
    andps   %xmm4,%xmm0                     # keep the other results
    andps   %xmm3,%xmm2                     # get just the right signalling bits
    andps   %xmm5,%xmm3                     # mask for the NaNs
    orps    %xmm2,%xmm3 # convert to QNaNs
    orps    %xmm3,%xmm0                     # combine
    jmp     .Lrnsx4

#       y is a NaN.
.Lx_NaN:
    lea     p_ux(%rsp),%rcx     # get pointer to x
    movdqa  (%rcx),%xmm4            # get x
    movdqa  %xmm4,%xmm3
    movdqa  %xmm4,%xmm5
    movdqa  .L__mask_sigbit(%rip),%xmm2 # get the signalling bits
    cmpps   $0,%xmm4,%xmm4                      # a compare equal  of x to itself should
                                            # be true, unless x is a NaN. 0's if NaN.
    cmpps   $4,%xmm3,%xmm3                      # compare not equal, ff's if NaN.
    andps   %xmm4,%xmm0                     # keep the other results
    andps   %xmm3,%xmm2                     # get just the right signalling bits
    andps   %xmm5,%xmm3                     # mask for the NaNs
    orps    %xmm2,%xmm3 # convert to QNaNs
    orps    %xmm3,%xmm0                     # combine
    jmp     .Lrnsx5

#      * y is infinite or so large that the result would
#         overflow or underflow.
.Ly_large:
    movdqa    %xmm0,p_temp(%rsp)

    test    $1,%edx
    jz      .Lylrga
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     (%rcx),%eax
    mov     (%rbx),%ebx
    mov     p_inty(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special6                   # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp(%rsp)
.Lylrga:
    test    $2,%edx
    jz      .Lylrgb
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     4(%rcx),%eax
    mov     4(%rbx),%ebx
    mov     p_inty+4(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special6                   # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+4(%rsp)
.Lylrgb:
    test    $4,%edx
    jz      .Lylrgc
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     8(%rcx),%eax
    mov     8(%rbx),%ebx
    mov     p_inty+8(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special6                   # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+8(%rsp)
.Lylrgc:
    test    $8,%edx
    jz      .Lylrgd
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     12(%rcx),%eax
    mov     12(%rbx),%ebx
    mov     p_inty+12(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special6                   # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+12(%rsp)
.Lylrgd:
    movdqa  p_temp(%rsp),%xmm0
    jmp     .Lrnsx3

# a subroutine to treat an individual x,y pair when y is large or infinity
# assumes x in .Ly,%eax in ebx.
# returns result in eax
.Lnp_special6:
# handle |x|==1 cases first
    mov     $0x07FFFFFFF,%r8d
    and     %eax,%r8d
    cmp     $0x03f800000,%r8d                     # jump if |x| !=1
    jnz     .Lnps6
    mov     $0x03f800000,%eax                     # return 1 for all |x|==1
    jmp     .Lnpx64

# cases where  |x| !=1
.Lnps6:
    mov     $0x07f800000,%ecx
    xor     %eax,%eax                             # assume 0 return
    test    $0x080000000,%ebx
    jnz     .Lnps62                           # jump if y negative
# y = +inf
    cmp     $0x03f800000,%r8d
    cmovg   %ecx,%eax                             # return inf if |x| < 1
    jmp     .Lnpx64
.Lnps62:
# y = -inf
    cmp     $0x03f800000,%r8d
    cmovl   %ecx,%eax                             # return inf if |x| < 1
    jmp     .Lnpx64

.Lnpx64:
    ret

# handle cases where x is +/- infinity.  edx is the mask
    .align 16
.Lx_infinite:
    movdqa    %xmm0,p_temp(%rsp)

    test    $1,%edx
    jz      .Lxinfa
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     (%rcx),%eax
    mov     (%rbx),%ebx
    mov     p_inty(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x1                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp(%rsp)
.Lxinfa:
    test    $2,%edx
    jz      .Lxinfb
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     4(%rcx),%eax
    mov     4(%rbx),%ebx
    mov     p_inty+4(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x1                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+4(%rsp)
.Lxinfb:
    test    $4,%edx
    jz      .Lxinfc
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     8(%rcx),%eax
    mov     8(%rbx),%ebx
    mov     p_inty+8(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x1                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+8(%rsp)
.Lxinfc:
    test    $8,%edx
    jz      .Lxinfd
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     12(%rcx),%eax
    mov     12(%rbx),%ebx
    mov     p_inty+12(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x1                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+12(%rsp)
.Lxinfd:
    movdqa  p_temp(%rsp),%xmm0
    jmp     .Lrnsx1

# a subroutine to treat an individual x,y pair when x is +/-infinity
# assumes x in .Ly,%eax in ebx, inty in ecx.
# returns result in eax
.Lnp_special_x1:                                            # x is infinite
    test    $0x080000000,%eax                               # is x positive
    jnz     .Lnsx11                                     # jump if not
    test    $0x080000000,%ebx                               # is y positive
    jz      .Lnsx13                                         # just return if so
    xor     %eax,%eax                                       # else return 0
    jmp     .Lnsx13

.Lnsx11:
    cmp     $1,%ecx                                     ## if inty ==1
    jnz     .Lnsx12                                     # jump if not
    test    $0x080000000,%ebx                               # is y positive
    jz      .Lnsx13                                         # just return if so
    mov     $0x080000000,%eax                               # else return -0
    jmp     .Lnsx13
.Lnsx12:                                                    # inty <>1
    and     $0x07FFFFFFF,%eax                               # return -x (|x|)  if y<0
    test    $0x080000000,%ebx                               # is y positive
    jz      .Lnsx13                                         #
    xor     %eax,%eax                                       # return 0  if y >=0
.Lnsx13:
    ret


# handle cases where x is +/- zero.  edx is the mask of x,y pairs with |x|=0
    .align 16
.Lx_zero:
    movdqa    %xmm0,p_temp(%rsp)

    test    $1,%edx
    jz      .Lxzera
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     (%rcx),%eax
    mov     (%rbx),%ebx
    mov     p_inty(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x2                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp(%rsp)
.Lxzera:
    test    $2,%edx
    jz      .Lxzerb
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     4(%rcx),%eax
    mov     4(%rbx),%ebx
    mov     p_inty+4(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x2                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+4(%rsp)
.Lxzerb:
    test    $4,%edx
    jz      .Lxzerc
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     8(%rcx),%eax
    mov     8(%rbx),%ebx
    mov     p_inty+8(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x2                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+8(%rsp)
.Lxzerc:
    test    $8,%edx
    jz      .Lxzerd
    lea     p_ux(%rsp),%rcx     # get pointer to x
    lea     p_uy(%rsp),%rbx     # get pointer to y
    mov     12(%rcx),%eax
    mov     12(%rbx),%ebx
    mov     p_inty+12(%rsp),%ecx
    sub     $8,%rsp
    call    .Lnp_special_x2                 # call the handler for one value
    add     $8,%rsp
    mov       %eax,p_temp+12(%rsp)
.Lxzerd:
    movdqa  p_temp(%rsp),%xmm0
    jmp     .Lrnsx2

# a subroutine to treat an individual x,y pair when x is +/-0
# assumes x in .Ly,%eax in ebx, inty in ecx.
# returns result in eax
    .align 16
.Lnp_special_x2:
    cmp     $1,%ecx                                     ## if inty ==1
    jz      .Lnsx21                                     # jump if so
# handle cases of x=+/-0, y not integer
    xor     %eax,%eax
    mov     $0x07f800000,%ecx
    test    $0x080000000,%ebx                               # is ypos
    cmovnz  %ecx,%eax
    jmp     .Lnsx23
# y is an integer
.Lnsx21:
    xor     %r8d,%r8d
    mov     $0x07f800000,%ecx
    test    $0x080000000,%ebx                               # is ypos
    cmovnz  %ecx,%r8d                                       # set to infinity if not
    and     $0x080000000,%eax                               # pickup the sign of x
    or      %r8d,%eax                                       # and include it in the result
.Lnsx23:
    ret


        .section .rodata
        .align 64

.L__mask_sign:          .quad 0x08000000080000000   # a sign bit mask
                .quad 0x08000000080000000

.L__mask_nsign:         .quad 0x07FFFFFFF7FFFFFFF   # a not sign bit mask
                .quad 0x07FFFFFFF7FFFFFFF

# used by inty
.L__mask_127:           .quad 0x00000007F0000007F   # EXPBIAS_SP32
                .quad 0x00000007F0000007F

.L__mask_mant:          .quad 0x0007FFFFF007FFFFF   # mantissa bit mask
                .quad 0x0007FFFFF007FFFFF

.L__mask_1:         .quad 0x00000000100000001   # 1
                .quad 0x00000000100000001

.L__mask_2:         .quad 0x00000000200000002   # 2
                .quad 0x00000000200000002

.L__mask_24:            .quad 0x00000001800000018   # 24
                .quad 0x00000001800000018

.L__mask_23:            .quad 0x00000001700000017   # 23
                .quad 0x00000001700000017

# used by special case checking

.L__float_one:          .quad 0x03f8000003f800000   # one
                .quad 0x03f8000003f800000

.L__mask_inf:           .quad 0x07f8000007F800000   # inifinity
                .quad 0x07f8000007F800000

.L__mask_NaN:           .quad 0x07fC000007FC00000   # NaN
                .quad 0x07fC000007FC00000

.L__mask_sigbit:        .quad 0x00040000000400000   # QNaN bit
                .quad 0x00040000000400000

.L__mask_ly:            .quad 0x04f0000004f000000   # large y
                .quad 0x04f0000004f000000
