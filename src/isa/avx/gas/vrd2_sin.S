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
# A vector implementation of the libm sin function.
#
# Prototype:
#
#     __m128d vrd2_sin(__m128d x);
#
#   Computes Sine of x
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.
#   Based on the NAG C implementation.


#include "fn_macros.h"
#define fname ALM_PROTO_BAS64(vrd2_sin)

#define fname_remainder_piby2 __amd_remainder_piby2

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif


.section .rodata
.align 16
.L__real_7fffffffffffffff:  .quad 0x07fffffffffffffff   #Sign bit zero
                .quad 0x07fffffffffffffff
.L__real_3ff0000000000000:  .quad 0x03ff0000000000000   # 1.0
                .quad 0x03ff0000000000000
.L__real_v2p__27:       .quad 0x03e40000000000000   # 2p-27
                .quad 0x03e40000000000000
.L__real_3fe0000000000000:  .quad 0x03fe0000000000000   # 0.5
                .quad 0x03fe0000000000000
.L__real_3fc5555555555555:  .quad 0x03fc5555555555555   # 0.166666666666
                .quad 0x03fc5555555555555
.L__real_3fe45f306dc9c883:  .quad 0x03fe45f306dc9c883   # twobypi
                .quad 0x03fe45f306dc9c883
.L__real_3ff921fb54400000:  .quad 0x03ff921fb54400000   # piby2_1
                .quad 0x03ff921fb54400000
.L__real_3dd0b4611a626331:  .quad 0x03dd0b4611a626331   # piby2_1tail
                .quad 0x03dd0b4611a626331
.L__real_3dd0b4611a600000:  .quad 0x03dd0b4611a600000   # piby2_2
                .quad 0x03dd0b4611a600000
.L__real_3ba3198a2e037073:  .quad 0x03ba3198a2e037073   # piby2_2tail
                .quad 0x03ba3198a2e037073
.L__real_fffffffff8000000:  .quad 0x0fffffffff8000000   # mask for stripping head and tail
                .quad 0x0fffffffff8000000
.L__real_8000000000000000:  .quad 0x08000000000000000   # -0  or signbit
                .quad 0x08000000000000000
.L__reald_one_one:      .quad 0x00000000100000001   #
                .quad 0
.L__reald_two_two:      .quad 0x00000000200000002   #
                .quad 0
.L__reald_one_zero:     .quad 0x00000000100000000   # sin_cos_filter
                .quad 0
.L__reald_zero_one:     .quad 0x00000000000000001   #
                .quad 0
.L__reald_two_zero:     .quad 0x00000000200000000   #
                .quad 0
.L__realq_one_one:      .quad 0x00000000000000001   #
                .quad 0x00000000000000001   #
.L__realq_two_two:      .quad 0x00000000000000002   #
                .quad 0x00000000000000002   #
.L__real_1_x_mask:      .quad 0x0ffffffffffffffff   #
                .quad 0x03ff0000000000000   #
.L__real_zero:          .quad 0x00000000000000000   #
                .quad 0x00000000000000000   #
.L__real_one:           .quad 0x00000000000000001   #
                .quad 0x00000000000000001   #
.L__real_ffffffffffffffff:  .quad 0x0ffffffffffffffff   #Sign bit one
                .quad 0x0ffffffffffffffff

.Lcosarray:
    .quad   0x03fa5555555555555     #  0.0416667            c1
    .quad   0x03fa5555555555555
    .quad   0x0bf56c16c16c16967     # -0.00138889           c2
    .quad   0x0bf56c16c16c16967
    .quad   0x03efa01a019f4ec90     #  2.48016e-005         c3
    .quad   0x03efa01a019f4ec90
    .quad   0x0be927e4fa17f65f6     # -2.75573e-007         c4
    .quad   0x0be927e4fa17f65f6
    .quad   0x03e21eeb69037ab78     #  2.08761e-009         c5
    .quad   0x03e21eeb69037ab78
    .quad   0x0bda907db46cc5e42     # -1.13826e-011         c6
    .quad   0x0bda907db46cc5e42
.Lsinarray:
    .quad   0x0bfc5555555555555     # -0.166667         s1
    .quad   0x0bfc5555555555555
    .quad   0x03f81111111110bb3     #  0.00833333           s2
    .quad   0x03f81111111110bb3
    .quad   0x0bf2a01a019e83e5c     # -0.000198413          s3
    .quad   0x0bf2a01a019e83e5c
    .quad   0x03ec71de3796cde01     #  2.75573e-006         s4
    .quad   0x03ec71de3796cde01
    .quad   0x0be5ae600b42fdfa7     # -2.50511e-008         s5
    .quad   0x0be5ae600b42fdfa7
    .quad   0x03de5e0b2f9a43bb8     #  1.59181e-010         s6
    .quad   0x03de5e0b2f9a43bb8
.Lsincosarray:
    .quad   0x0bfc5555555555555     # -0.166667         s1
    .quad   0x03fa5555555555555     #  0.0416667            c1
    .quad   0x03f81111111110bb3     #  0.00833333           s2
    .quad   0x0bf56c16c16c16967     #               c2
    .quad   0x0bf2a01a019e83e5c     # -0.000198413          s3
    .quad   0x03efa01a019f4ec90
    .quad   0x03ec71de3796cde01     #  2.75573e-006         s4
    .quad   0x0be927e4fa17f65f6
    .quad   0x0be5ae600b42fdfa7     # -2.50511e-008         s5
    .quad   0x03e21eeb69037ab78
    .quad   0x03de5e0b2f9a43bb8     #  1.59181e-010         s6
    .quad   0x0bda907db46cc5e42
.Lcossinarray:
    .quad   0x03fa5555555555555     #  0.0416667            c1
    .quad   0x0bfc5555555555555     # -0.166667         s1
    .quad   0x0bf56c16c16c16967     #               c2
    .quad   0x03f81111111110bb3     #  0.00833333           s2
    .quad   0x03efa01a019f4ec90
    .quad   0x0bf2a01a019e83e5c     # -0.000198413          s3
    .quad   0x0be927e4fa17f65f6
    .quad   0x03ec71de3796cde01     #  2.75573e-006         s4
    .quad   0x03e21eeb69037ab78
    .quad   0x0be5ae600b42fdfa7     # -2.50511e-008         s5
    .quad   0x0bda907db46cc5e42
    .quad   0x03de5e0b2f9a43bb8     #  1.59181e-010         s6

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .text
    .align 16
    .p2align 4,,15

.equ    p_temp, 0x00        # temporary for get/put bits operation
.equ    p_temp1,0x10        # temporary for get/put bits operation
.equ    p_temp2,0x20        # temporary for get/put bits operation
.equ    p_xmm6, 0x30        # temporary for get/put bits operation
.equ    p_xmm7, 0x40        # temporary for get/put bits operation
.equ    p_xmm8, 0x50        # temporary for get/put bits operation
.equ    p_xmm9, 0x60        # temporary for get/put bits operation
.equ    p_xmm10,0x70        # temporary for get/put bits operation
.equ    p_xmm11,0x80        # temporary for get/put bits operation
.equ    p_xmm12,0x90        # temporary for get/put bits operation
.equ    p_xmm13,0x0A0       # temporary for get/put bits operation
.equ    p_xmm14,0x0B0       # temporary for get/put bits operation
.equ    p_xmm15,0x0C0       # temporary for get/put bits operation
.equ    r,  0x0D0       # pointer to r for remainder_piby2
.equ    rr, 0x0E0       # pointer to r for remainder_piby2
.equ    region, 0x0F0       # pointer to r for remainder_piby2
.equ    p_original,0x100    # original x
.equ    p_mask, 0x110       # original x
.equ    p_sign, 0x120       # original x

.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:

    sub     $0x138,%rsp

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#STARTMAIN
movdqa  %xmm0,%xmm6                 #move to mem to get into integer regs **
andpd   .L__real_7fffffffffffffff(%rip), %xmm0      #Unsign         -

movd    %xmm0,%rax                  #rax is lower arg   +
movhpd  %xmm0, p_temp+8(%rsp)               #           +
mov     p_temp+8(%rsp),%rcx             #rcx = upper arg    +
movdqa  %xmm0,%xmm1

                            #This will mask all nan/infs also
pcmpgtd     %xmm6,%xmm1
movdqa      %xmm1,%xmm6
psrldq      $4, %xmm1
psrldq      $8, %xmm6

mov     $0x3FE921FB54442D18,%rdx            #piby4  +
mov $0x411E848000000000,%r10            #5e5    +

movapd  .L__real_3fe0000000000000(%rip), %xmm5      #0.5 for later use  +

por %xmm1,%xmm6
movd    %xmm6,%r11                  #Move Sign to gpr **

movapd  %xmm0,%xmm2                 #x  +
movapd  %xmm0,%xmm4                 #x  +

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Leither_or_both_arg_gt_than_piby4:

    cmp %r10,%rax               #is lower arg >= 5e5
    jae .Llower_or_both_arg_gt_5e5
    cmp %r10,%rcx               #is upper arg >= 5e5
    jae .Lupper_arg_gt_5e5

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lboth_arg_lt_than_5e5:
# %xmm2,,%xmm0 xmm4 = x, xmm5 = 0.5

    mulpd   .L__real_3fe45f306dc9c883(%rip),%xmm2   # x*twobypi
    movapd  .L__real_3ff921fb54400000(%rip),%xmm3   # xmm3=piby2_1
    addpd   %xmm5,%xmm2             # xmm2 = npi2 = x*twobypi+0.5
    movapd  .L__real_3dd0b4611a600000(%rip),%xmm1   # xmm1=piby2_2
    movapd  .L__real_3ba3198a2e037073(%rip),%xmm6   # xmm6=piby2_2tail
    cvttpd2dq   %xmm2,%xmm0         # xmm0=convert npi2 to ints
    cvtdq2pd    %xmm0,%xmm2         # xmm2=and back to double.


#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
    mulpd   %xmm2,%xmm3             # npi2 * piby2_1
    subpd   %xmm3,%xmm4             # xmm4 = rhead=x-npi2*piby2_1

#t  = rhead;
       movapd   %xmm4,%xmm5             # xmm5=t=rhead

#rtail  = npi2 * piby2_2;
       mulpd    %xmm2,%xmm1             # xmm1= npi2*piby2_2

#rhead  = t - rtail;
       subpd    %xmm1,%xmm4             # xmm4= rhead = t-rtail

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd    %xmm2,%xmm6                     # npi2 * piby2_2tail
       subpd    %xmm4,%xmm5             # t-rhead
       subpd    %xmm5,%xmm1             # rtail-(t - rhead)
       addpd    %xmm6,%xmm1             # rtail=npi2*piby2_2+(rtail-(t-rhead))

#r = rhead - rtail
#rr=(rhead-r) -rtail
#Sign
#Region
    movdqa      %xmm0,%xmm5         # Region            +
    movd        %xmm0,%r10          # Sign
    movdqa      %xmm4,%xmm0         # rhead (handle xmm0 retype)    +

    subpd   %xmm1,%xmm0             # rhead - rtail         +
    pand    .L__reald_one_one(%rip),%xmm5       # Odd/Even region for Cos/Sin   +
    mov     .L__reald_one_zero(%rip),%r9        # Compare value for cossin  +
    subpd   %xmm0,%xmm4             # rr=rhead-r            +
    movd    %xmm5,%r8               # Region            +
    movapd  %xmm0,%xmm2             # Move for x2           +
    mulpd   %xmm0,%xmm2             # x2                +
    subpd   %xmm1,%xmm4             # rr=(rhead-r) -rtail       +

    shr $1,%r10                 #~AB+A~B, A is sign and B is upper bit of region
    mov %r10,%rcx
    not     %r11                    #ADDED TO CHANGE THE LOGIC
    and %r11,%r10
    not %rcx
    not %r11
    and %r11,%rcx
    or  %rcx,%r10
    and .L__reald_one_one(%rip),%r10            #(~AB+A~B)&1

    mov %r10,%r11
    and %r9,%r11                #mask out the lower sign bit leaving the upper sign bit
    shl $63,%r10                #shift lower sign bit left by 63 bits
    shl $31,%r11                #shift upper sign bit left by 31 bits
    mov      %r10,p_sign(%rsp)          #write out lower sign bit
    mov      %r11,p_sign+8(%rsp)            #write out upper sign bit

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#xmm0= x, xmm2 = x2, xmm4 = xx, r8 = region, r9 = compare value for sincos path, xmm6 = Sign

.align 16
.L__vrd2_sin_approximate:
    cmp     $0,%r8
    jnz .Lvrd2_not_sin_piby4

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lvrd2_sin_piby4:
    movapd  .Lsinarray+0x50(%rip),%xmm3 # s6
    movapd  .Lsinarray+0x20(%rip),%xmm5 # s3
    movapd  %xmm2,%xmm1         # move for x4

    mulpd   %xmm2,%xmm3         # x2s6
    mulpd   %xmm2,%xmm5         # x2s3
    mulpd   %xmm2,%xmm1         # x4

    addpd   .Lsinarray+0x40(%rip),%xmm3 # s5+x2s6
    movapd  %xmm2,%xmm6         # move for x3
    addpd   .Lsinarray+0x10(%rip),%xmm5 # s2+x2s3

    mulpd   %xmm2,%xmm3         # x2(s5+x2s6)
    mulpd   %xmm2,%xmm5         # x2(s2+x2s3)
    mulpd   %xmm2,%xmm1         # x6

    addpd   .Lsinarray+0x30(%rip),%xmm3 # s4 + x2(s5+x2s6)
    addpd   .Lsinarray(%rip),%xmm5      # s1+x2(s2+x2s3)
    mulpd   .L__real_3fe0000000000000(%rip),%xmm2 # 0.5 *x2

    mulpd   %xmm1,%xmm3         # x6(s4 + x2(s5+x2s6))
    mulpd   %xmm0,%xmm6         # x3
    addpd   %xmm5,%xmm3         # zs
    mulpd   %xmm4,%xmm2         # 0.5 * x2 *xx

    mulpd   %xmm3,%xmm6         # x3*zs
    subpd   %xmm2,%xmm6         # x3*zs - 0.5 * x2 *xx
    addpd   %xmm4,%xmm6         # +xx
    addpd   %xmm6,%xmm0         # +x
    xorpd   p_sign(%rsp),%xmm0  # xor sign
    jmp .L__vrd2_sin_cleanup

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#xmm0= x, xmm2 = x2, xmm4 = xx, r8 = region, r9 = compare value for sincos path, xmm6 = Sign
.align 16
.Lvrd2_not_sin_piby4:
    cmp     $1,%r8
    jnz .Lvrd2_not_sin_cos_piby4

.Lvrd2_sin_cos_piby4:

    movapd   %xmm4,p_temp(%rsp)     # rr move to to memory
    movapd   %xmm0,p_temp1(%rsp)        # r move to to memory

    movapd  .Lcossinarray+0x50(%rip),%xmm3      # s6
    mulpd   %xmm2,%xmm3             # x2s6
    movdqa  .Lcossinarray+0x20(%rip),%xmm5      # s3
    movapd  %xmm2,%xmm1             # move x2 for x4
    mulpd   %xmm2,%xmm1             # x4
    mulpd   %xmm2,%xmm5             # x2s3

    addpd   .Lcossinarray+0x40(%rip),%xmm3      # s5+x2s6
    movapd  %xmm2,%xmm4             # move for x6
    mulpd   %xmm2,%xmm3             # x2(s5+x2s6)
    mulpd   %xmm1,%xmm4             # x6
    addpd   .Lcossinarray+0x10(%rip),%xmm5      # s2+x2s3
    mulpd   %xmm2,%xmm5             # x2(s2+x2s3)
    addpd   .Lcossinarray+0x30(%rip),%xmm3      # s4 + x2(s5+x2s6)

    movhlps %xmm0,%xmm0             # high of x for x3
    mulpd   %xmm4,%xmm3             # x6(s4 + x2(s5+x2s6))
    addpd   .Lcossinarray(%rip),%xmm5       # s1+x2(s2+x2s3)

    movhlps %xmm2,%xmm4             # high of x2 for x3
    addpd   %xmm5,%xmm3             # z

    mulpd   .L__real_3fe0000000000000(%rip),%xmm2   # 0.5*x2
    mulsd   %xmm0,%xmm4             # x3 #
    movhlps %xmm3,%xmm5             # xmm5 = sin
                            # xmm3 = cos

    mulsd   %xmm4,%xmm5             # sin*x3 #
    movsd   .L__real_3ff0000000000000(%rip),%xmm4   # 1.0 #
    mulsd   %xmm1,%xmm3             # cos*x4 #

    subsd   %xmm2,%xmm4                 # t=1.0-r #

    movhlps %xmm2,%xmm6             # move 0.5 * x2 for 0.5 * x2 * xx #
    mulsd   p_temp+8(%rsp),%xmm6            # 0.5 * x2 * xx #
    subsd   %xmm6,%xmm5             # sin - 0.5 * x2 *xx #
    addsd   p_temp+8(%rsp),%xmm5            # sin+xx #

    movlpd  p_temp1(%rsp),%xmm6         # x
    mulsd   p_temp(%rsp),%xmm6          # x *xx #

    movsd   .L__real_3ff0000000000000(%rip),%xmm1   # 1 #
    subsd   %xmm4,%xmm1             # 1 -t #
    addsd   %xmm5,%xmm0             # sin+x #
    subsd   %xmm2,%xmm1             # (1-t) - r #
    subsd   %xmm6,%xmm1             # ((1 + (-t)) - r) - x*xx #
    addsd   %xmm1,%xmm3             # cos+((1 + (-t)) - r) - x*xx #
    addsd   %xmm4,%xmm3             # cos+t #

    movapd  p_sign(%rsp),%xmm2          # load sign
    movlhps %xmm0,%xmm3
    movapd  %xmm3,%xmm0
    xorpd   %xmm2,%xmm0
    jmp .L__vrd2_sin_cleanup

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#xmm0= x, xmm2 = x2, xmm4 = xx, r8 = region, r9 = compare value for sincos path, xmm6 = Sign
.align 16
.Lvrd2_not_sin_cos_piby4:
    cmp     %r9,%r8
    jnz .Lvrd2_cos_piby4

.Lvrd2_cos_sin_piby4:

    movapd   %xmm4,p_temp(%rsp)     # Store rr
    movapd  .Lsincosarray+0x50(%rip),%xmm3      # s6
    mulpd   %xmm2,%xmm3             # x2s6
    movdqa  .Lsincosarray+0x20(%rip),%xmm5      # s3 (handle xmm5 retype)
    movapd  %xmm2,%xmm1             # move x2 for x4
    mulpd   %xmm2,%xmm1             # x4
    mulpd   %xmm2,%xmm5             # x2s3
    addpd   .Lsincosarray+0x40(%rip),%xmm3      # s5+x2s6
    movapd  %xmm2,%xmm4             # move x2 for x6
    mulpd   %xmm2,%xmm3             # x2(s5+x2s6)
    mulpd   %xmm1,%xmm4             # x6
    addpd   .Lsincosarray+0x10(%rip),%xmm5      # s2+x2s3
    mulpd   %xmm2,%xmm5             # x2(s2+x2s3)
    addpd   .Lsincosarray+0x30(%rip),%xmm3      # s4+x2(s5+x2s6)

    movhlps %xmm1,%xmm1             # move high x4 for cos
    mulpd   %xmm4,%xmm3             # x6(s4+x2(s5+x2s6))
    addpd   .Lsincosarray(%rip),%xmm5       # s1+x2(s2+x2s3)
    movapd  %xmm2,%xmm4             # move low x2 for x3
    mulsd   %xmm0,%xmm4             # get low x3 for sin term
    mulpd   .L__real_3fe0000000000000(%rip),%xmm2   # 0.5*x2

    addpd   %xmm3,%xmm5             # z
    movhlps %xmm2,%xmm6             # move high r for cos
    movhlps %xmm5,%xmm3             # xmm5 = sin
                            # xmm3 = cos
    mulsd   p_temp(%rsp),%xmm2      # 0.5 * x2 * xx

    mulsd   %xmm4,%xmm5             # sin *x3
    movsd   .L__real_3ff0000000000000(%rip),%xmm4   # 1.0
    mulsd   %xmm1,%xmm3             # cos *x4
    subsd   %xmm6,%xmm4                 # t=1.0-r

    movhlps %xmm0,%xmm1
    subsd   %xmm2,%xmm5             # sin - 0.5 * x2 *xx

    mulsd   p_temp+8(%rsp),%xmm1            # x * xx
    movsd   .L__real_3ff0000000000000(%rip),%xmm2   # 1
    subsd   %xmm4,%xmm2             # 1 - t
    addsd   p_temp(%rsp),%xmm5          # sin+xx

    subsd   %xmm6,%xmm2             # (1-t) - r
    subsd   %xmm1,%xmm2             # ((1 + (-t)) - r) - x*xx
    addsd   %xmm5,%xmm0             # sin + x
    addsd   %xmm2,%xmm3             # cos+((1-t)-r - x*xx)
    addsd   %xmm4,%xmm3             # cos+t

    movapd  p_sign(%rsp),%xmm5      # load sign
    movlhps %xmm3,%xmm0
    xorpd   %xmm5,%xmm0
    jmp .L__vrd2_sin_cleanup


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16

.Lvrd2_cos_piby4:
    mulpd   %xmm0,%xmm4             # x*xx
    movdqa  .L__real_3fe0000000000000(%rip),%xmm5   # 0.5 (handle xmm5 retype)
    movapd  .Lcosarray+0x50(%rip),%xmm1     # c6
    movapd  .Lcosarray+0x20(%rip),%xmm0     # c3
    mulpd   %xmm2,%xmm5             # r = 0.5 *x2
    movapd  %xmm2,%xmm3             # copy of x2 for x4
    movapd   %xmm4,p_temp(%rsp)     # store x*xx
    mulpd   %xmm2,%xmm1             # c6*x2
    mulpd   %xmm2,%xmm0             # c3*x2
    subpd   .L__real_3ff0000000000000(%rip),%xmm5   # -t=r-1.0
    mulpd   %xmm2,%xmm3             # x4
    addpd   .Lcosarray+0x40(%rip),%xmm1     # c5+x2c6
    addpd   .Lcosarray+0x10(%rip),%xmm0     # c2+x2C3
    addpd   .L__real_3ff0000000000000(%rip),%xmm5   # 1 + (-t)
    mulpd   %xmm2,%xmm3             # x6
    mulpd   %xmm2,%xmm1             # x2(c5+x2c6)
    mulpd   %xmm2,%xmm0             # x2(c2+x2C3)
    movapd  %xmm2,%xmm4             # copy of x2
    mulpd   .L__real_3fe0000000000000(%rip),%xmm4   # r = 0.5 *x2
    addpd   .Lcosarray+0x30(%rip),%xmm1     # c4 + x2(c5+x2c6)
    addpd   .Lcosarray(%rip),%xmm0          # c1+x2(c2+x2C3)
    mulpd   %xmm2,%xmm2             # x4
    subpd   %xmm4,%xmm5             # (1 + (-t)) - r
    mulpd   %xmm3,%xmm1             # x6(c4 + x2(c5+x2c6))
    addpd   %xmm1,%xmm0             # zc
    subpd   .L__real_3ff0000000000000(%rip),%xmm4   # -t=r-1.0
    subpd   p_temp(%rsp),%xmm5      # ((1 + (-t)) - r) - x*xx
    mulpd   %xmm2,%xmm0             # x4 * zc
    addpd   %xmm5,%xmm0             # x4 * zc + ((1 + (-t)) - r -x*xx)
    subpd   %xmm4,%xmm0             # result - (-t)
    xorpd   p_sign(%rsp),%xmm0      # xor with sign
    jmp     .L__vrd2_sin_cleanup




#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Llower_or_both_arg_gt_5e5:
    cmp %r10,%rcx               #is upper arg >= 5e5
    jae .Lboth_arg_gt_5e5

.Llower_arg_gt_5e5:
# Upper Arg is < 5e5, Lower arg is >= 5e5

    movlpd   %xmm0,r(%rsp)          #Save lower fp arg for remainder_piby2 call

    movhlps %xmm0,%xmm0         #Needed since we want to work on upper arg
    movhlps %xmm2,%xmm2
    movhlps %xmm4,%xmm4

# Work on Upper arg
# %xmm2,,%xmm0 xmm4 = x, xmm5 = 0.5
# Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs

#If upper Arg is <=piby4
       cmp  %rdx,%rcx                   # is upper arg > piby4
       ja   0f

       mov   $0,%ecx                        # region = 0
       mov   %ecx,region+4(%rsp)                # store upper region
       movlpd    %xmm0,r+8(%rsp)                    # store upper r (unsigned - sign is adjusted later based on sign)
       xorpd     %xmm4,%xmm4                    # rr = 0
       movlpd    %xmm4,rr+8(%rsp)               # store upper rr
       jmp  .Lcheck_lower_arg

#If upper Arg is > piby4
.align 16
0:
    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm2       # x*twobypi
    addsd   %xmm5,%xmm2                 # xmm2 = npi2=(x*twobypi+0.5)
    movsd   .L__real_3ff921fb54400000(%rip),%xmm3       # xmm3 = piby2_1
    cvttsd2si   %xmm2,%ecx              # xmm0 = npi2 trunc to ints
    movsd   .L__real_3dd0b4611a600000(%rip),%xmm1       # xmm1 = piby2_2
    cvtsi2sd    %ecx,%xmm2              # xmm2 = npi2 trunc to doubles

    #/* Subtract the multiple from x to get an extra-precision remainder */
    #rhead  = x - npi2 * piby2_1;
    mulsd   %xmm2,%xmm3                 # npi2 * piby2_1
    subsd   %xmm3,%xmm4                 # xmm4 = rhead =(x-npi2*piby2_1)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm6       # xmm6 =piby2_2tail

    #t  = rhead;
       movsd    %xmm4,%xmm5                 # xmm5 = t = rhead

    #rtail  = npi2 * piby2_2;
       mulsd    %xmm2,%xmm1                 # xmm1 =rtail=(npi2*piby2_2)

    #rhead  = t - rtail
       subsd    %xmm1,%xmm4                 # xmm4 =rhead=(t-rtail)

    #rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd    %xmm2,%xmm6                         # npi2 * piby2_2tail
       subsd    %xmm4,%xmm5                 # t-rhead
       subsd    %xmm5,%xmm1                 # (rtail-(t-rhead))
       addsd    %xmm6,%xmm1                 # rtail=npi2*piby2_2tail+(rtail-(t-rhead));

    #r =  rhead - rtail
    #rr = (rhead-r) -rtail
       mov   %ecx,region+4(%rsp)            # store upper region
       movsd    %xmm4,%xmm0
       subsd    %xmm1,%xmm0                 # xmm0 = r=(rhead-rtail)
       subsd    %xmm0,%xmm4                 # rr=rhead-r
       subsd    %xmm1,%xmm4                 # xmm4 = rr=((rhead-r) -rtail)
       movlpd    %xmm0,r+8(%rsp)            # store upper r
       movlpd    %xmm4,rr+8(%rsp)           # store upper rr

#If lower Arg is > 5e5
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
.align 16
.Lcheck_lower_arg:
    mov     $0x07ff0000000000000,%r9            # is lower arg nan/inf
    mov     %r9,%r10
    and     %rax,%r10
    cmp     %r9,%r10
    jz      .L__vrd2_cos_lower_naninf

    mov  %r11,p_temp(%rsp)  #Save Sign


    lea  region(%rsp),%rdx  # lower arg is **NOT** nan/inf
    lea  rr(%rsp),%rsi
    lea  r(%rsp),%rdi
    movlpd   r(%rsp),%xmm0      #Restore lower fp arg for remainder_piby2 call

        call    fname_remainder_piby2@PLT

    mov p_temp(%rsp),%r11   #Restore Sign

    jmp     .L__vrd2_sin_reconstruct

.L__vrd2_cos_lower_naninf:
    mov r(%rsp),%rax
    mov $0x00008000000000000,%r9
    or  %r9,%rax
    mov  %rax,r(%rsp)               # r = x | 0x0008000000000000
    xor %r10,%r10
    mov  %r10,rr(%rsp)              # rr = 0
    mov  %r10d,region(%rsp)         # region =0

    jmp     .L__vrd2_sin_reconstruct

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lupper_arg_gt_5e5:
# Upper Arg is >= 5e5, Lower arg is < 5e5
    movhpd  %xmm0,r+8(%rsp)     #Save upper fp arg for remainder_piby2 call
    movlhps %xmm0,%xmm0     #Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
    movlhps %xmm2,%xmm2
    movlhps %xmm4,%xmm4

# Work on Lower arg
# %xmm2,,%xmm0 xmm4 = x, xmm5 = 0.5
# Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

#If lower Arg is <=piby4
       cmp  %rdx,%rax                   # is upper arg > piby4
       ja   0f

       mov  $0,%eax                     # region = 0
       mov   %eax,region(%rsp)          # store upper region
       movlpd    %xmm0,r(%rsp)              # store upper r
       xorpd    %xmm4,%xmm4                 # rr = 0
       movlpd    %xmm4,rr(%rsp)             # store upper rr
       jmp .Lcheck_upper_arg

.align 16
0:
#If upper Arg is > piby4
    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm2       # x*twobypi
    addsd   %xmm5,%xmm2                 # xmm2 = npi2=(x*twobypi+0.5)
    movsd   .L__real_3ff921fb54400000(%rip),%xmm3       # xmm3 = piby2_1
    cvttsd2si   %xmm2,%eax              # xmm0 = npi2 trunc to ints
    movsd   .L__real_3dd0b4611a600000(%rip),%xmm1       # xmm1 = piby2_2
    cvtsi2sd    %eax,%xmm2              # xmm2 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
    mulsd   %xmm2,%xmm3                 # npi2 * piby2_1
    subsd   %xmm3,%xmm4                 # xmm4 = rhead =(x-npi2*piby2_1)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm6       # xmm6 =piby2_2tail

#t  = rhead;
       movsd    %xmm4,%xmm5                 # xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd    %xmm2,%xmm1                 # xmm1 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd    %xmm1,%xmm4                 # xmm4 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd    %xmm2,%xmm6                         # npi2 * piby2_2tail
       subsd    %xmm4,%xmm5                 # t-rhead
       subsd    %xmm5,%xmm1                 # (rtail-(t-rhead))
       addsd    %xmm6,%xmm1                 # rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov   %eax,region(%rsp)          # store lower region
       movsd    %xmm4,%xmm0
       subsd    %xmm1,%xmm0                 # xmm0 = r=(rhead-rtail)
       subsd    %xmm0,%xmm4                 # rr=rhead-r
       subsd    %xmm1,%xmm4                 # xmm4 = rr=((rhead-r) -rtail)
       movlpd    %xmm0,r(%rsp)              # store lower r
       movlpd    %xmm4,rr(%rsp)             # store lower rr

#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
.align 16
.Lcheck_upper_arg:
    mov     $0x07ff0000000000000,%r9            # is upper arg nan/inf
    mov     %r9,%r10
    and     %rcx,%r10
    cmp     %r9,%r10
    jz      .L__vrd2_cos_upper_naninf

    mov  %r11,p_temp(%rsp)  #Save Sign


    lea  region+4(%rsp),%rdx            # upper arg is **NOT** nan/inf
    lea  rr+8(%rsp),%rsi
    lea  r+8(%rsp),%rdi
    movlpd   r+8(%rsp),%xmm0    #Restore upper fp arg for remainder_piby2 call
        call    fname_remainder_piby2@PLT

    mov p_temp(%rsp),%r11   #Restore Sign

    jmp .L__vrd2_sin_reconstruct

.L__vrd2_cos_upper_naninf:
    mov r+8(%rsp),%rcx              # upper arg is nan/inf
    mov $0x00008000000000000,%r9
    or  %r9,%rcx
    mov %rcx,r+8(%rsp)              # r = x | 0x0008000000000000
    xor %r10,%r10
    mov %r10,rr+8(%rsp)             # rr = 0
    mov %r10d,region+4(%rsp)            # region =0
    jmp     .L__vrd2_sin_reconstruct

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lboth_arg_gt_5e5:
#Upper Arg is >= 5e5, Lower arg is >= 5e5

    movhpd   %xmm0,p_temp2(%rsp)            #Save upper fp arg for remainder_piby2 call

    mov     $0x07ff0000000000000,%r9    #is lower arg nan/inf
    mov     %r9,%r10
    and     %rax,%r10
    cmp     %r9,%r10
    jz      .L__vrd2_cos_lower_naninf_of_both_gt_5e5

    mov   %rcx,p_temp(%rsp)         #Save upper arg
    mov   %r11,p_temp1(%rsp)            #Save Sign

    lea  region(%rsp),%rdx          #lower arg is **NOT** nan/inf
    lea  rr(%rsp),%rsi
    lea  r(%rsp),%rdi
        call    fname_remainder_piby2@PLT

    mov  p_temp1(%rsp),%r11         #Restore Sign
    mov  p_temp(%rsp),%rcx          #Restore upper arg
    jmp     0f

.L__vrd2_cos_lower_naninf_of_both_gt_5e5:               #lower arg is nan/inf
    movd    %xmm0,%rax
    mov $0x00008000000000000,%r9
    or  %r9,%rax
    mov %rax,r(%rsp)                #r = x | 0x0008000000000000
    xor %r10,%r10
    mov %r10,rr(%rsp)               #rr = 0
    mov %r10d,region(%rsp)          #region = 0

.align 16
0:
    mov     $0x07ff0000000000000,%r9            #is upper arg nan/inf
    mov     %r9,%r10
    and     %rcx,%r10
    cmp     %r9,%r10
    jz      .L__vrd2_cos_upper_naninf_of_both_gt_5e5


    mov  %r11,p_temp(%rsp)  #Save Sign

    lea  region+4(%rsp),%rdx        #upper arg is **NOT** nan/inf
    lea  rr+8(%rsp),%rsi
    lea  r+8(%rsp),%rdi
    movlpd   p_temp2(%rsp),%xmm0        #Restore upper fp arg for remainder_piby2 call
        call    fname_remainder_piby2@PLT

    mov p_temp(%rsp),%r11   #Restore Sign

    jmp     0f

.L__vrd2_cos_upper_naninf_of_both_gt_5e5:
    mov  p_temp2(%rsp),%rcx         #upper arg is nan/inf
    mov  $0x00008000000000000,%r9
    or   %r9,%rcx
    mov  %rcx,r+8(%rsp)             #r = x | 0x0008000000000000
    xor  %r10,%r10
    mov  %r10,rr+8(%rsp)            #rr = 0
    mov  %r10d,region+4(%rsp)           #region = 0

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
0:
.L__vrd2_sin_reconstruct:
#Construct xmm0=x, xmm2 =x2, xmm4=xx, r8=region, xmm6=sign
    movapd  r(%rsp),%xmm0               #x
    movapd  %xmm0,%xmm2             #move for x2
    mulpd   %xmm2,%xmm2             #x2
    movapd  rr(%rsp),%xmm4              #xx

    mov region(%rsp),%r8
    mov     .L__reald_one_zero(%rip),%r9        #compare value for cossin path
    mov     %r8,%r10
    and .L__reald_one_one(%rip),%r8     #odd/even region for cos/sin

    shr $1,%r10                 #~AB+A~B, A is sign and B is upper bit of region
    mov %r10,%rcx
    not     %r11                    #ADDED TO CHANGE THE LOGIC
    and %r11,%r10
    not %rcx
    not %r11
    and %r11,%rcx
    or  %rcx,%r10
    and .L__reald_one_one(%rip),%r10        #(~AB+A~B)&1

    mov %r10,%r11
    and %r9,%r11                #mask out the lower sign bit leaving the upper sign bit
    shl $63,%r10                #shift lower sign bit left by 63 bits
    shl $31,%r11                #shift upper sign bit left by 31 bits
    mov      %r10,p_sign(%rsp)          #write out lower sign bit
    mov      %r11,p_sign+8(%rsp)            #write out upper sign bit

    jmp .L__vrd2_sin_approximate
#ENDMAIN

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.L__vrd2_sin_cleanup:
    add $0x138,%rsp
    ret

