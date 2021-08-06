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
# vrs4sinf.s
#
# A vector implementation of the sin libm function.
#
# Prototype:
#
#    __m128 vrs4_sinf(__m128 x);
#
# Computes Sine of x for an array of input values.
# Places the results into the supplied y array.
# Does not perform error checking.
# Denormal inputs may produce unexpected results.
# This routine computes 4 single precision Sine values at a time.
# The four values are passed as packed single in xmm10.
# The four results are returned as packed singles in xmm10.
# Note that this represents a non-standard ABI usage, as no ABI
# ( and indeed C) currently allows returning 2 values for a function.
# It is expected that some compilers may be able to take advantage of this
# interface when implementing vectorized loops.  Using the array implementation
# of the routine requires putting the inputs into memory, and retrieving
# the results from memory.  This routine eliminates the need for this
# overhead if the data does not already reside in memory.


#include "fn_macros.h"
#define fname ALM_PROTO_BAS64(vrs4_sinf)

#define fname_remainder_piby2d2f __amd_remainder_piby2d2f


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.section .rodata
.align 64
.L__real_7fffffffffffffff: .quad 0x07fffffffffffffff    #Sign bit zero
            .quad 0x07fffffffffffffff
.L__real_3ff0000000000000: .quad 0x03ff0000000000000    # 1.0
            .quad 0x03ff0000000000000
.L__real_v2p__27:       .quad 0x03e40000000000000   # 2p-27
            .quad 0x03e40000000000000
.L__real_3fe0000000000000: .quad 0x03fe0000000000000    # 0.5
            .quad 0x03fe0000000000000
.L__real_3fc5555555555555: .quad 0x03fc5555555555555    # 0.166666666666
            .quad 0x03fc5555555555555
.L__real_3fe45f306dc9c883: .quad 0x03fe45f306dc9c883    # twobypi
            .quad 0x03fe45f306dc9c883
.L__real_3ff921fb54400000: .quad 0x03ff921fb54400000    # piby2_1
            .quad 0x03ff921fb54400000
.L__real_3dd0b4611a626331: .quad 0x03dd0b4611a626331    # piby2_1tail
            .quad 0x03dd0b4611a626331
.L__real_3dd0b4611a600000: .quad 0x03dd0b4611a600000    # piby2_2
            .quad 0x03dd0b4611a600000
.L__real_3ba3198a2e037073: .quad 0x03ba3198a2e037073    # piby2_2tail
            .quad 0x03ba3198a2e037073
.L__real_fffffffff8000000: .quad 0x0fffffffff8000000    # mask for stripping head and tail
            .quad 0x0fffffffff8000000
.L__real_8000000000000000:  .quad 0x08000000000000000   # -0  or signbit
            .quad 0x08000000000000000
.L__reald_one_one:      .quad 0x00000000100000001   #
            .quad 0
.L__reald_two_two:      .quad 0x00000000200000002   #
            .quad 0
.L__reald_one_zero: .quad 0x00000000100000000   # sin_cos_filter
            .quad 0
.L__reald_zero_one: .quad 0x00000000000000001   #
            .quad 0
.L__reald_two_zero: .quad 0x00000000200000000   #
            .quad 0
.L__realq_one_one:      .quad 0x00000000000000001   #
            .quad 0x00000000000000001       #
.L__realq_two_two:      .quad 0x00000000000000002   #
            .quad 0x00000000000000002       #
.L__real_1_x_mask:      .quad 0x0ffffffffffffffff   #
            .quad 0x03ff0000000000000       #
.L__real_zero:      .quad 0x00000000000000000       #
            .quad 0x00000000000000000       #
.L__real_one:       .quad 0x00000000000000001       #
            .quad 0x00000000000000001       #

.Lcosarray:
    .quad   0x03FA5555555502F31     #  0.0416667            c1
    .quad   0x03FA5555555502F31
    .quad   0x0BF56C16BF55699D7     # -0.00138889           c2
    .quad   0x0BF56C16BF55699D7
    .quad   0x03EFA015C50A93B49     #  2.48016e-005         c3
    .quad   0x03EFA015C50A93B49
    .quad   0x0BE92524743CC46B8     # -2.75573e-007         c4
    .quad   0x0BE92524743CC46B8

.Lsinarray:
    .quad   0x0BFC555555545E87D     # -0.166667         s1
    .quad   0x0BFC555555545E87D
    .quad   0x03F811110DF01232D     # 0.00833333            s2
    .quad   0x03F811110DF01232D
    .quad   0x0BF2A013A88A37196     # -0.000198413          s3
    .quad   0x0BF2A013A88A37196
    .quad   0x03EC6DBE4AD1572D5     # 2.75573e-006          s4
    .quad   0x03EC6DBE4AD1572D5

.Lsincosarray:
    .quad   0x0BFC555555545E87D     # -0.166667         s1
    .quad   0x03FA5555555502F31     # 0.0416667         c1
    .quad   0x03F811110DF01232D     # 0.00833333            s2
    .quad   0x0BF56C16BF55699D7
    .quad   0x0BF2A013A88A37196     # -0.000198413          s3
    .quad   0x03EFA015C50A93B49
    .quad   0x03EC6DBE4AD1572D5     # 2.75573e-006          s4
    .quad   0x0BE92524743CC46B8

.Lcossinarray:
    .quad   0x03FA5555555502F31     # 0.0416667         c1
    .quad   0x0BFC555555545E87D     # -0.166667         s1
    .quad   0x0BF56C16BF55699D7     #               c2
    .quad   0x03F811110DF01232D
    .quad   0x03EFA015C50A93B49     #               c3
    .quad   0x0BF2A013A88A37196
    .quad   0x0BE92524743CC46B8     #               c4
    .quad   0x03EC6DBE4AD1572D5

.align 64
    .Levensin_oddcos_tbl:

        .quad   .Lsinsin_sinsin_piby4       # 0     *   ; Done
        .quad   .Lsinsin_sincos_piby4       # 1     +   ; Done
        .quad   .Lsinsin_cossin_piby4       # 2         ; Done
        .quad   .Lsinsin_coscos_piby4       # 3     +   ; Done

        .quad   .Lsincos_sinsin_piby4       # 4         ; Done
        .quad   .Lsincos_sincos_piby4       # 5     *   ; Done
        .quad   .Lsincos_cossin_piby4       # 6         ; Done
        .quad   .Lsincos_coscos_piby4       # 7         ; Done

        .quad   .Lcossin_sinsin_piby4       # 8         ; Done
        .quad   .Lcossin_sincos_piby4       # 9         ; TBD
        .quad   .Lcossin_cossin_piby4       # 10        *   ; Done
        .quad   .Lcossin_coscos_piby4       # 11            ; Done

        .quad   .Lcoscos_sinsin_piby4       # 12            ; Done
        .quad   .Lcoscos_sincos_piby4       # 13        +   ; Done
        .quad   .Lcoscos_cossin_piby4       # 14            ; Done
        .quad   .Lcoscos_coscos_piby4       # 15        *   ; Done

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    .text
    .align 16
    .p2align 4,,15

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# define local variable storage offsets
.equ    p_temp,0        # temporary for get/put bits operation
.equ    p_temp1,0x10        # temporary for get/put bits operation

.equ    save_xmm6,0x20      # temporary for get/put bits operation
.equ    save_xmm7,0x30      # temporary for get/put bits operation
.equ    save_xmm8,0x40      # temporary for get/put bits operation
.equ    save_xmm9,0x50      # temporary for get/put bits operation
.equ    save_xmm0,0x60      # temporary for get/put bits operation
.equ    save_xmm11,0x70     # temporary for get/put bits operation
.equ    save_xmm12,0x80     # temporary for get/put bits operation
.equ    save_xmm13,0x90     # temporary for get/put bits operation
.equ    save_xmm14,0x0A0    # temporary for get/put bits operation
.equ    save_xmm15,0x0B0    # temporary for get/put bits operation

.equ    r,0x0C0         # pointer to r for remainder_piby2
.equ    rr,0x0D0        # pointer to r for remainder_piby2
.equ    region,0x0E0        # pointer to r for remainder_piby2

.equ    r1,0x0F0        # pointer to r for remainder_piby2
.equ    rr1,0x0100      # pointer to r for remainder_piby2
.equ    region1,0x0110      # pointer to r for remainder_piby2

.equ    p_temp2,0x0120      # temporary for get/put bits operation
.equ    p_temp3,0x0130      # temporary for get/put bits operation

.equ    p_temp4,0x0140      # temporary for get/put bits operation
.equ    p_temp5,0x0150      # temporary for get/put bits operation

.equ    p_original,0x0160       # original x
.equ    p_mask,0x0170       # original x
.equ    p_sign,0x0180       # original x

.equ    p_original1,0x0190      # original x
.equ    p_mask1,0x01A0      # original x
.equ    p_sign1,0x01B0      # original x

.equ    save_r12,0x01C0     # temporary for get/put bits operation
.equ    save_r13,0x01D0     # temporary for get/put bits operation

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.globl fname
    .type   fname,@function
fname:

    sub     $0x01E8,%rsp

    mov %r12,save_r12(%rsp) # save r12

    mov %r13,save_r13(%rsp) # save r13

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#STARTMAIN

    movhlps     %xmm0,%xmm8
    cvtps2pd    %xmm0,%xmm10            # convert input to double.
    cvtps2pd    %xmm8,%xmm1         # convert input to double.

movdqa  %xmm10,%xmm6
movdqa  %xmm1,%xmm7
movapd  .L__real_7fffffffffffffff(%rip),%xmm2

andpd   %xmm2,%xmm10    #Unsign
andpd   %xmm2,%xmm1 #Unsign

movd    %xmm10,%rax             #rax is lower arg
movhpd  %xmm10, p_temp+8(%rsp)          #
mov     p_temp+8(%rsp),%rcx         #rcx = upper arg

movd    %xmm1,%r8               #r8 is lower arg
movhpd  %xmm1, p_temp1+8(%rsp)          #
mov     p_temp1+8(%rsp),%r9         #r9 = upper arg

movdqa  %xmm10,%xmm12
movdqa  %xmm1,%xmm13

pcmpgtd     %xmm6,%xmm12
pcmpgtd     %xmm7,%xmm13
movdqa      %xmm12,%xmm6
movdqa      %xmm13,%xmm7
psrldq      $4,%xmm12
psrldq      $4,%xmm13
psrldq      $8,%xmm6
psrldq      $8,%xmm7

mov     $0x3FE921FB54442D18,%rdx            #piby4  +
mov $0x411E848000000000,%r10            #5e5    +
movapd  .L__real_3fe0000000000000(%rip),%xmm4       #0.5 for later use +

por %xmm6,%xmm12
por %xmm7,%xmm13
movd    %xmm12,%r12             #Move Sign to gpr **
movd    %xmm13,%r13             #Move Sign to gpr **

movapd  %xmm10,%xmm2                #x0
movapd  %xmm1,%xmm3             #x1
movapd  %xmm10,%xmm6                #x0
movapd  %xmm1,%xmm7             #x1

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm2 = x, xmm4 =0.5/t, xmm6 =x
# xmm3 = x, xmm5 =0.5/t, xmm7 =x
.align 16
.Leither_or_both_arg_gt_than_piby4:
    cmp %r10,%rax
    jae .Lfirst_or_next3_arg_gt_5e5

    cmp %r10,%rcx
    jae .Lsecond_or_next2_arg_gt_5e5

    cmp %r10,%r8
    jae .Lthird_or_fourth_arg_gt_5e5

    cmp %r10,%r9
    jae .Lfourth_arg_gt_5e5


#      /* Find out what multiple of piby2 */
#        npi2  = (int)(x * twobypi + 0.5);
    movapd  .L__real_3fe45f306dc9c883(%rip),%xmm10
    mulpd   %xmm10,%xmm2                        # * twobypi
    mulpd   %xmm10,%xmm3                        # * twobypi

    addpd   %xmm4,%xmm2                     # +0.5, npi2
    addpd   %xmm4,%xmm3                     # +0.5, npi2

    movapd  .L__real_3ff921fb54400000(%rip),%xmm10      # piby2_1
    movapd  .L__real_3ff921fb54400000(%rip),%xmm1       # piby2_1

    cvttpd2dq   %xmm2,%xmm4                 # convert packed double to packed integers
    cvttpd2dq   %xmm3,%xmm5                 # convert packed double to packed integers

    movapd  .L__real_3dd0b4611a600000(%rip),%xmm8       # piby2_2
    movapd  .L__real_3dd0b4611a600000(%rip),%xmm9       # piby2_2

    cvtdq2pd    %xmm4,%xmm2                 # and back to double.
    cvtdq2pd    %xmm5,%xmm3                 # and back to double.

#      /* Subtract the multiple from x to get an extra-precision remainder */

    movd    %xmm4,%r8                       # Region
    movd    %xmm5,%r9                       # Region

    mov     .L__reald_one_zero(%rip),%rdx           #compare value for cossin path
    mov %r8,%r10
    mov %r9,%r11

#      rhead  = x - npi2 * piby2_1;
       mulpd    %xmm2,%xmm10                        # npi2 * piby2_1;
       mulpd    %xmm3,%xmm1                     # npi2 * piby2_1;

#      rtail  = npi2 * piby2_2;
       mulpd    %xmm2,%xmm8                     # rtail
       mulpd    %xmm3,%xmm9                     # rtail

#      rhead  = x - npi2 * piby2_1;
       subpd    %xmm10,%xmm6                        # rhead  = x - npi2 * piby2_1;
       subpd    %xmm1,%xmm7                     # rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd   %xmm6,%xmm10                        # t
       movapd   %xmm7,%xmm1                     # t

#      rhead  = t - rtail;
       subpd    %xmm8,%xmm10                        # rhead
       subpd    %xmm9,%xmm1                     # rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd    .L__real_3ba3198a2e037073(%rip),%xmm2       # npi2 * piby2_2tail
       mulpd    .L__real_3ba3198a2e037073(%rip),%xmm3       # npi2 * piby2_2tail

       subpd    %xmm10,%xmm6                        # t-rhead
       subpd    %xmm1,%xmm7                     # t-rhead

       subpd    %xmm6,%xmm8                     # - ((t - rhead) - rtail)
       subpd    %xmm7,%xmm9                     # - ((t - rhead) - rtail)

       addpd    %xmm2,%xmm8                     # rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);
       addpd    %xmm3,%xmm9                     # rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm4  = npi2 (int), xmm10 =rhead, xmm8 =rtail
# xmm5  = npi2 (int), xmm1 =rhead, xmm9 =rtail

    and .L__reald_one_one(%rip),%r8     #odd/even region for cos/sin
    and .L__reald_one_one(%rip),%r9     #odd/even region for cos/sin

    shr $1,%r10                     #~AB+A~B, A is sign and B is upper bit of region
    shr $1,%r11                     #~AB+A~B, A is sign and B is upper bit of region

    mov %r10,%rax
    mov %r11,%rcx

    not     %r12                        #ADDED TO CHANGE THE LOGIC
    not     %r13                        #ADDED TO CHANGE THE LOGIC
    and %r12,%r10
    and %r13,%r11

    not %rax
    not %rcx
    not %r12
    not %r13
    and %r12,%rax
    and %r13,%rcx

    or  %rax,%r10
    or  %rcx,%r11
    and .L__reald_one_one(%rip),%r10                #(~AB+A~B)&1
    and .L__reald_one_one(%rip),%r11                #(~AB+A~B)&1

    mov %r10,%r12
    mov %r11,%r13

    and %rdx,%r12               #mask out the lower sign bit leaving the upper sign bit
    and %rdx,%r13               #mask out the lower sign bit leaving the upper sign bit

    shl $63,%r10                #shift lower sign bit left by 63 bits
    shl $63,%r11                #shift lower sign bit left by 63 bits
    shl $31,%r12                #shift upper sign bit left by 31 bits
    shl $31,%r13                #shift upper sign bit left by 31 bits

    mov      %r10,p_sign(%rsp)      #write out lower sign bit
    mov      %r12,p_sign+8(%rsp)        #write out upper sign bit
    mov      %r11,p_sign1(%rsp)     #write out lower sign bit
    mov      %r13,p_sign1+8(%rsp)       #write out upper sign bit

# GET_BITS_DP64(rhead-rtail, uy);                   ; originally only rhead
# xmm4  = Sign, xmm10 =rhead, xmm8 =rtail
# xmm5  = Sign, xmm1 =rhead, xmm9 =rtail
    movapd  %xmm10,%xmm6                        # rhead
    movapd  %xmm1,%xmm7                     # rhead

    subpd   %xmm8,%xmm10                        # r = rhead - rtail
    subpd   %xmm9,%xmm1                     # r = rhead - rtail

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm4  = Sign, xmm10 = r, xmm6 =rhead, xmm8 =rtail
# xmm5  = Sign, xmm1 = r, xmm7 =rhead, xmm9 =rtail

    mov %r8,%rax
    mov %r9,%rcx

    movapd  %xmm10,%xmm2                # move r for r2
    movapd  %xmm1,%xmm3             # move r for r2

    mulpd   %xmm10,%xmm2                # r2
    mulpd   %xmm1,%xmm3             # r2

    and .L__reald_zero_one(%rip),%rax
    and .L__reald_zero_one(%rip),%rcx
    shr $31,%r8
    shr $31,%r9
    or  %r8,%rax
    or  %r9,%rcx
    shl $2,%rcx
    or  %rcx,%rax

    lea .Levensin_oddcos_tbl(%rip),%rcx
    jmp *(%rcx,%rax,8)              #Jmp table for cos/sin calculation based on even/odd region













#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lfirst_or_next3_arg_gt_5e5:
# %rcx,,%rax r8, r9

    cmp %r10,%rcx               #is upper arg >= 5e5
    jae .Lboth_arg_gt_5e5

.Llower_arg_gt_5e5:
# Upper Arg is < 5e5, Lower arg is >= 5e5
# %xmm2,,%xmm10 xmm6 = x, xmm4 = 0.5
# Be sure not to use %xmm3,%xmm1 and xmm7
# Use %xmm8,,%xmm5 xmm0, xmm12
#       %xmm11,,%xmm9 xmm13


    movlpd   %xmm10,r(%rsp)     #Save lower fp arg for remainder_piby2 call
    movhlps %xmm10,%xmm10           #Needed since we want to work on upper arg
    movhlps %xmm2,%xmm2
    movhlps %xmm6,%xmm6

# Work on Upper arg
# Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm2       # x*twobypi
    addsd   %xmm4,%xmm2                 # xmm2 = npi2=(x*twobypi+0.5)
    movsd   .L__real_3ff921fb54400000(%rip),%xmm8       # xmm8 = piby2_1
    cvttsd2si   %xmm2,%ecx              # ecx = npi2 trunc to ints
    movsd   .L__real_3dd0b4611a600000(%rip),%xmm0       # xmm0 = piby2_2
    cvtsi2sd    %ecx,%xmm2              # xmm2 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
    mulsd   %xmm2,%xmm8                 # npi2 * piby2_1
    subsd   %xmm8,%xmm6                 # xmm6 = rhead =(x-npi2*piby2_1)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm12      # xmm12 =piby2_2tail

#t  = rhead;
       movsd    %xmm6,%xmm5                 # xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd    %xmm2,%xmm0                 # xmm1 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd    %xmm0,%xmm6                 # xmm6 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd    %xmm2,%xmm12                        # npi2 * piby2_2tail
       subsd    %xmm6,%xmm5                 # t-rhead
       subsd    %xmm5,%xmm0                 # (rtail-(t-rhead))
       addsd    %xmm12,%xmm0                    # rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov   %ecx,region+4(%rsp)            # store upper region
       movsd    %xmm6,%xmm10
       subsd    %xmm0,%xmm10                    # xmm10 = r=(rhead-rtail)
       movlpd    %xmm10,r+8(%rsp)           # store upper r

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#Note that volatiles will be trashed by the call
#We will construct r, rr, region and sign

# Work on Lower arg
    mov     $0x07ff0000000000000,%r11           # is lower arg nan/inf
    mov     %r11,%r10
    and     %rax,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_lower_naninf

    mov   %r8,p_temp(%rsp)
    mov   %r9,p_temp2(%rsp)
    movapd    %xmm1,p_temp1(%rsp)
    movapd    %xmm3,p_temp3(%rsp)
    movapd    %xmm7,p_temp5(%rsp)

    lea  region(%rsp),%rdx          # lower arg is **NOT** nan/inf
    lea  r(%rsp),%rsi

# changed input from xmm10 to xmm0
    mov  r(%rsp),%rdi   #Restore lower fp arg for remainder_piby2 call

    call     fname_remainder_piby2d2f@PLT

    mov  p_temp(%rsp),%r8
    mov  p_temp2(%rsp),%r9
    movapd   p_temp1(%rsp),%xmm1
    movapd   p_temp3(%rsp),%xmm3
    movapd   p_temp5(%rsp),%xmm7
    jmp     0f

.L__vrs4_sinf_lower_naninf:
    mov $0x00008000000000000,%r11
    or  %r11,%rax
    mov  %rax,r(%rsp)               # r = x | 0x0008000000000000
    mov  %r10d,region(%rsp)         # region =0

.align 16
0:
    jmp     .Lcheck_next2_args


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lboth_arg_gt_5e5:
#Upper Arg is >= 5e5, Lower arg is >= 5e5
# %rcx,,%rax r8, r9
# %xmm2,,%xmm10 xmm6 = x, xmm4 = 0.5

    movhlps %xmm10,%xmm6        #Save upper fp arg for remainder_piby2 call

    mov     $0x07ff0000000000000,%r11           #is lower arg nan/inf
    mov     %r11,%r10
    and     %rax,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_lower_naninf_of_both_gt_5e5

    mov   %rcx,p_temp(%rsp)         #Save upper arg
    mov   %r8,p_temp2(%rsp)
    mov   %r9,p_temp4(%rsp)
    movapd    %xmm1,p_temp1(%rsp)
    movapd    %xmm3,p_temp3(%rsp)
    movapd    %xmm7,p_temp5(%rsp)

    lea  region(%rsp),%rdx          #lower arg is **NOT** nan/inf
    lea  r(%rsp),%rsi

# added ins- changed input from xmm10 to xmm0
    movd     %xmm10,%rdi

    call     fname_remainder_piby2d2f@PLT

    mov  p_temp2(%rsp),%r8
    mov  p_temp4(%rsp),%r9
    movapd   p_temp1(%rsp),%xmm1
    movapd   p_temp3(%rsp),%xmm3
    movapd   p_temp5(%rsp),%xmm7

    mov  p_temp(%rsp),%rcx          #Restore upper arg
    jmp     0f

.L__vrs4_sinf_lower_naninf_of_both_gt_5e5:              #lower arg is nan/inf
#   mov .LQWORD,%rax PTR p_original[rsp]
    mov $0x00008000000000000,%r11
    or  %r11,%rax
    mov  %rax,r(%rsp)               #r = x | 0x0008000000000000
    mov  %r10d,region(%rsp)         #region = 0

.align 16
0:
    mov     $0x07ff0000000000000,%r11           #is upper arg nan/inf
    mov     %r11,%r10
    and     %rcx,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_upper_naninf_of_both_gt_5e5


    mov   %r8,p_temp2(%rsp)
    mov   %r9,p_temp4(%rsp)
    movapd    %xmm1,p_temp1(%rsp)
    movapd    %xmm3,p_temp3(%rsp)
    movapd    %xmm7,p_temp5(%rsp)

    lea  region+4(%rsp),%rdx            #upper arg is **NOT** nan/inf
    lea  r+8(%rsp),%rsi

# changed input from xmm10 to xmm0
    movd     %xmm6,%rdi

    call     fname_remainder_piby2d2f@PLT

    mov  p_temp2(%rsp),%r8
    mov  p_temp4(%rsp),%r9
    movapd   p_temp1(%rsp),%xmm1
    movapd   p_temp3(%rsp),%xmm3
    movapd   p_temp5(%rsp),%xmm7

    jmp     0f

.L__vrs4_sinf_upper_naninf_of_both_gt_5e5:
    mov $0x00008000000000000,%r11
    or  %r11,%rcx
    mov  %rcx,r+8(%rsp)             #r = x | 0x0008000000000000
    mov  %r10d,region+4(%rsp)           #region = 0

.align 16
0:
    jmp     .Lcheck_next2_args

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsecond_or_next2_arg_gt_5e5:

# Upper Arg is >= 5e5, Lower arg is < 5e5
# %rcx,,%rax r8, r9
# %xmm2,,%xmm10 xmm6 = x, xmm4 = 0.5
# Do not use %xmm3,,%xmm1 xmm7
# Restore xmm4 and %xmm3,,%xmm1 xmm7
# Can use %xmm0,,%xmm8 xmm12
#   %xmm9,,%xmm5 xmm11, xmm13

    movhpd   %xmm10,r+8(%rsp)   #Save upper fp arg for remainder_piby2 call

# Work on Lower arg
# Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm2       # x*twobypi
    addsd   %xmm4,%xmm2                 # xmm2 = npi2=(x*twobypi+0.5)
    movsd   .L__real_3ff921fb54400000(%rip),%xmm8       # xmm3 = piby2_1
    cvttsd2si   %xmm2,%eax              # ecx = npi2 trunc to ints
    movsd   .L__real_3dd0b4611a600000(%rip),%xmm0       # xmm1 = piby2_2
    cvtsi2sd    %eax,%xmm2              # xmm2 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
    mulsd   %xmm2,%xmm8                 # npi2 * piby2_1
    subsd   %xmm8,%xmm6                 # xmm6 = rhead =(x-npi2*piby2_1)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm12      # xmm7 =piby2_2tail

#t  = rhead;
       movsd    %xmm6,%xmm5                 # xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd    %xmm2,%xmm0                 # xmm1 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd    %xmm0,%xmm6                 # xmm6 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd    %xmm2,%xmm12                    # npi2 * piby2_2tail
       subsd    %xmm6,%xmm5                 # t-rhead
       subsd    %xmm5,%xmm0                 # (rtail-(t-rhead))
       addsd    %xmm12,%xmm0                    # rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov   %eax,region(%rsp)          # store upper region

        subsd   %xmm0,%xmm6                 # xmm10 = r=(rhead-rtail)

        movlpd   %xmm6,r(%rsp)              # store upper r


#Work on Upper arg
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
    mov     $0x07ff0000000000000,%r11           # is upper arg nan/inf
    mov     %r11,%r10
    and     %rcx,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_upper_naninf

    mov  %r8,p_temp(%rsp)
    mov  %r9,p_temp2(%rsp)
    movapd   %xmm1,p_temp1(%rsp)
    movapd   %xmm3,p_temp3(%rsp)
    movapd   %xmm7,p_temp5(%rsp)

    lea  region+4(%rsp),%rdx            # upper arg is **NOT** nan/inf
    lea  r+8(%rsp),%rsi

# changed input from xmm10 to xmm0
    mov  r+8(%rsp),%rdi

    call     fname_remainder_piby2d2f@PLT

    mov p_temp(%rsp),%r8
    mov p_temp2(%rsp),%r9
    movapd  p_temp1(%rsp),%xmm1
    movapd  p_temp3(%rsp),%xmm3
    movapd  p_temp5(%rsp),%xmm7
    jmp     0f

.L__vrs4_sinf_upper_naninf:
    mov $0x00008000000000000,%r11
    or  %r11,%rcx
    mov  %rcx,r+8(%rsp)             # r = x | 0x0008000000000000
    mov  %r10d,region+4(%rsp)           # region =0

.align 16
0:
    jmp     .Lcheck_next2_args


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lcheck_next2_args:

    mov $0x411E848000000000,%r10            #5e5    +

    cmp %r10,%r8
    jae .Lfirst_second_done_third_or_fourth_arg_gt_5e5

    cmp %r10,%r9
    jae .Lfirst_second_done_fourth_arg_gt_5e5



# Work on next two args, both < 5e5
# %xmm3,,%xmm1 xmm5 = x, xmm4 = 0.5

    movapd  .L__real_3fe0000000000000(%rip),%xmm4           #Restore 0.5

    mulpd   .L__real_3fe45f306dc9c883(%rip),%xmm3           # * twobypi
    addpd   %xmm4,%xmm3                     # +0.5, npi2
    movapd  .L__real_3ff921fb54400000(%rip),%xmm1       # piby2_1
    cvttpd2dq   %xmm3,%xmm5                 # convert packed double to packed integers
    movapd  .L__real_3dd0b4611a600000(%rip),%xmm9       # piby2_2
    cvtdq2pd    %xmm5,%xmm3                 # and back to double.

###
#      /* Subtract the multiple from x to get an extra-precision remainder */
    movlpd   %xmm5,region1(%rsp)                        # Region
###

#      rhead  = x - npi2 * piby2_1;
       mulpd    %xmm3,%xmm1                     # npi2 * piby2_1;

#      rtail  = npi2 * piby2_2;
       mulpd    %xmm3,%xmm9                     # rtail

#      rhead  = x - npi2 * piby2_1;
       subpd    %xmm1,%xmm7                     # rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd   %xmm7,%xmm1                     # t

#      rhead  = t - rtail;
       subpd    %xmm9,%xmm1                     # rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd    .L__real_3ba3198a2e037073(%rip),%xmm3       # npi2 * piby2_2tail

       subpd    %xmm1,%xmm7                     # t-rhead
       subpd    %xmm7,%xmm9                     # - ((t - rhead) - rtail)
       addpd    %xmm3,%xmm9                     # rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

#       movapd  %xmm1,%xmm7                     ; rhead
       subpd    %xmm9,%xmm1                     # r = rhead - rtail
       movapd    %xmm1,r1(%rsp)

#       subpd   %xmm1,%xmm7                     ; rr=rhead-r
#       subpd   xmm7, xmm9                      ; rr=(rhead-r) -rtail
#       movapd  OWORD PTR rr1[rsp], xmm7

    jmp .L__vrs4_sinf_reconstruct


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lthird_or_fourth_arg_gt_5e5:
#first two args are < 5e5, third arg >= 5e5, fourth arg >= 5e5 or < 5e5
# %rcx,,%rax r8, r9
# %xmm2,,%xmm10 xmm6 = x, xmm4 = 0.5
# Do not use %xmm3,,%xmm1 xmm7
# Can use   %xmm11,,%xmm9 xmm13
#   %xmm8,,%xmm5 xmm0, xmm12
# Restore xmm4

# Work on first two args, both < 5e5



    mulpd   .L__real_3fe45f306dc9c883(%rip),%xmm2       # * twobypi
    addpd   %xmm4,%xmm2                     # +0.5, npi2
    movapd  .L__real_3ff921fb54400000(%rip),%xmm10      # piby2_1
    cvttpd2dq   %xmm2,%xmm4                 # convert packed double to packed integers
    movapd  .L__real_3dd0b4611a600000(%rip),%xmm8       # piby2_2
    cvtdq2pd    %xmm4,%xmm2                 # and back to double.

###
#      /* Subtract the multiple from x to get an extra-precision remainder */
    movlpd   %xmm4,region(%rsp)             # Region
###

#      rhead  = x - npi2 * piby2_1;
       mulpd    %xmm2,%xmm10                        # npi2 * piby2_1;
#      rtail  = npi2 * piby2_2;
       mulpd    %xmm2,%xmm8                     # rtail

#      rhead  = x - npi2 * piby2_1;
       subpd    %xmm10,%xmm6                        # rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd   %xmm6,%xmm10                        # t

#      rhead  = t - rtail;
       subpd    %xmm8,%xmm10                        # rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd    .L__real_3ba3198a2e037073(%rip),%xmm2       # npi2 * piby2_2tail

       subpd    %xmm10,%xmm6                        # t-rhead
       subpd    %xmm6,%xmm8                     # - ((t - rhead) - rtail)
       addpd    %xmm2,%xmm8                     # rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

#       movapd  %xmm10,%xmm6                        ; rhead
       subpd    %xmm8,%xmm10                        # r = rhead - rtail
       movapd    %xmm10,r(%rsp)

#       subpd   %xmm10,%xmm6                        ; rr=rhead-r
#       subpd   xmm6, xmm8                      ; rr=(rhead-r) -rtail
#       movapd  OWORD PTR rr[rsp], xmm6


# Work on next two args, third arg >= 5e5, fourth arg >= 5e5 or < 5e5

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lfirst_second_done_third_or_fourth_arg_gt_5e5:
# %rcx,,%rax r8, r9
# %xmm2,,%xmm10 xmm6 = x, xmm4 = 0.5


    mov $0x411E848000000000,%r10            #5e5    +
    cmp %r10,%r9
    jae .Lboth_arg_gt_5e5_higher


# Upper Arg is <5e5, Lower arg is >= 5e5
# %r9,%r8
# %xmm3,,%xmm1 xmm7 = x, xmm4 = 0.5

    movlpd   %xmm1,r1(%rsp)     #Save lower fp arg for remainder_piby2 call
    movhlps %xmm1,%xmm1         #Needed since we want to work on upper arg
    movhlps %xmm3,%xmm3
    movhlps %xmm7,%xmm7


# Work on Upper arg
# Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
    movapd  .L__real_3fe0000000000000(%rip),%xmm4       # Restore 0.5

    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm3       # x*twobypi
    addsd   %xmm4,%xmm3                 # xmm3 = npi2=(x*twobypi+0.5)
    movsd   .L__real_3ff921fb54400000(%rip),%xmm2       # xmm2 = piby2_1
    cvttsd2si   %xmm3,%r9d              # r9d = npi2 trunc to ints
    movsd   .L__real_3dd0b4611a600000(%rip),%xmm10      # xmm10 = piby2_2
    cvtsi2sd    %r9d,%xmm3              # xmm3 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
    mulsd   %xmm3,%xmm2                 # npi2 * piby2_1
    subsd   %xmm2,%xmm7                 # xmm7 = rhead =(x-npi2*piby2_1)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm6       # xmm6 =piby2_2tail

#t  = rhead;
       movsd    %xmm7,%xmm5                 # xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd    %xmm3,%xmm10                    # xmm10 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd    %xmm10,%xmm7                    # xmm7 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd    %xmm3,%xmm6                         # npi2 * piby2_2tail
       subsd    %xmm7,%xmm5                 # t-rhead
       subsd    %xmm5,%xmm10                    # (rtail-(t-rhead))
       addsd    %xmm6,%xmm10                    # rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov   %r9d,region1+4(%rsp)           # store upper region

       subsd    %xmm10,%xmm7                    # xmm1 = r=(rhead-rtail)

       movlpd    %xmm7,r1+8(%rsp)           # store upper r



#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign

# Work on Lower arg
    mov     $0x07ff0000000000000,%r11           # is lower arg nan/inf
    mov     %r11,%r10
    and     %r8,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_lower_naninf_higher

    lea  region1(%rsp),%rdx         # lower arg is **NOT** nan/inf
    lea  r1(%rsp),%rsi

# changed input from xmm10 to xmm0
    mov  r1(%rsp),%rdi

    call     fname_remainder_piby2d2f@PLT

    jmp     0f

.L__vrs4_sinf_lower_naninf_higher:
    mov $0x00008000000000000,%r11
    or  %r11,%r8
    mov  %r8,r1(%rsp)               # r = x | 0x0008000000000000
    mov  %r10d,region1(%rsp)            # region =0

.align 16
0:
    jmp     .L__vrs4_sinf_reconstruct







.align 16
.Lboth_arg_gt_5e5_higher:
# Upper Arg is >= 5e5, Lower arg is >= 5e5
# %r9,%r8
# %xmm3,,%xmm1 xmm7 = x, xmm4 = 0.5


    movhlps %xmm1,%xmm7     #Save upper fp arg for remainder_piby2 call

    mov     $0x07ff0000000000000,%r11           #is lower arg nan/inf
    mov     %r11,%r10
    and     %r8,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_lower_naninf_of_both_gt_5e5_higher

    mov   %r9,p_temp1(%rsp)         #Save upper arg
    lea  region1(%rsp),%rdx         #lower arg is **NOT** nan/inf
    lea  r1(%rsp),%rsi

# changed input from xmm10 to xmm0
    movd     %xmm1,%rdi

    call     fname_remainder_piby2d2f@PLT

    mov  p_temp1(%rsp),%r9          #Restore upper arg

    jmp     0f

.L__vrs4_sinf_lower_naninf_of_both_gt_5e5_higher:               #lower arg is nan/inf
    mov $0x00008000000000000,%r11
    or  %r11,%r8
    mov  %r8,r1(%rsp)               #r = x | 0x0008000000000000
    mov  %r10d,region1(%rsp)            #region = 0

.align 16
0:
    mov     $0x07ff0000000000000,%r11           #is upper arg nan/inf
    mov     %r11,%r10
    and     %r9,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_upper_naninf_of_both_gt_5e5_higher

    lea  region1+4(%rsp),%rdx           #upper arg is **NOT** nan/inf
    lea  r1+8(%rsp),%rsi

# changed input from xmm10 to xmm0
    movd     %xmm7,%rdi

    call     fname_remainder_piby2d2f@PLT

    jmp     0f

.L__vrs4_sinf_upper_naninf_of_both_gt_5e5_higher:
    mov $0x00008000000000000,%r11
    or  %r11,%r9
    mov  %r9,r1+8(%rsp)             #r = x | 0x0008000000000000
    mov  %r10d,region1+4(%rsp)          #region = 0

.align 16
0:
    jmp     .L__vrs4_sinf_reconstruct

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lfourth_arg_gt_5e5:
#first two args are < 5e5, third arg < 5e5, fourth arg >= 5e5
#%rcx,,%rax r8, r9
#%xmm2,,%xmm10 xmm6 = x, xmm4 = 0.5

# Work on first two args, both < 5e5

    mulpd   .L__real_3fe45f306dc9c883(%rip),%xmm2       # * twobypi
    addpd   %xmm4,%xmm2                     # +0.5, npi2
    movapd  .L__real_3ff921fb54400000(%rip),%xmm10      # piby2_1
    cvttpd2dq   %xmm2,%xmm4                 # convert packed double to packed integers
    movapd  .L__real_3dd0b4611a600000(%rip),%xmm8       # piby2_2
    cvtdq2pd    %xmm4,%xmm2                 # and back to double.

###
#      /* Subtract the multiple from x to get an extra-precision remainder */
    movlpd   %xmm4,region(%rsp)             # Region
###

#      rhead  = x - npi2 * piby2_1;
       mulpd    %xmm2,%xmm10                        # npi2 * piby2_1;
#      rtail  = npi2 * piby2_2;
       mulpd    %xmm2,%xmm8                     # rtail

#      rhead  = x - npi2 * piby2_1;
       subpd    %xmm10,%xmm6                        # rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd   %xmm6,%xmm10                        # t

#      rhead  = t - rtail;
       subpd    %xmm8,%xmm10                        # rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd    .L__real_3ba3198a2e037073(%rip),%xmm2       # npi2 * piby2_2tail

       subpd    %xmm10,%xmm6                        # t-rhead
       subpd    %xmm6,%xmm8                     # - ((t - rhead) - rtail)
       addpd    %xmm2,%xmm8                     # rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

#       movapd  %xmm10,%xmm6                        ; rhead
       subpd    %xmm8,%xmm10                        # r = rhead - rtail
       movapd    %xmm10,r(%rsp)

#       subpd   %xmm10,%xmm6                        ; rr=rhead-r
#       subpd   xmm6, xmm8                      ; rr=(rhead-r) -rtail
#       movapd  OWORD PTR rr[rsp], xmm6


# Work on next two args, third arg < 5e5, fourth arg >= 5e5
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lfirst_second_done_fourth_arg_gt_5e5:

# Upper Arg is >= 5e5, Lower arg is < 5e5
# %r9,%r8
# %xmm3,,%xmm1 xmm7 = x, xmm4 = 0.5

    movhpd   %xmm1,r1+8(%rsp)   #Save upper fp arg for remainder_piby2 call

# Work on Lower arg
# Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg
    movapd  .L__real_3fe0000000000000(%rip),%xmm4       # Restore 0.5
    mulsd   .L__real_3fe45f306dc9c883(%rip),%xmm3       # x*twobypi
    addsd   %xmm4,%xmm3                 # xmm3 = npi2=(x*twobypi+0.5)
    movsd   .L__real_3ff921fb54400000(%rip),%xmm2       # xmm2 = piby2_1
    cvttsd2si   %xmm3,%r8d              # r8d = npi2 trunc to ints
    movsd   .L__real_3dd0b4611a600000(%rip),%xmm10      # xmm10 = piby2_2
    cvtsi2sd    %r8d,%xmm3              # xmm3 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
    mulsd   %xmm3,%xmm2                 # npi2 * piby2_1
    subsd   %xmm2,%xmm7                 # xmm7 = rhead =(x-npi2*piby2_1)
    movsd   .L__real_3ba3198a2e037073(%rip),%xmm6       # xmm6 =piby2_2tail

#t  = rhead;
       movsd    %xmm7,%xmm5                 # xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd    %xmm3,%xmm10                    # xmm10 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd    %xmm10,%xmm7                    # xmm7 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd    %xmm3,%xmm6                         # npi2 * piby2_2tail
       subsd    %xmm7,%xmm5                 # t-rhead
       subsd    %xmm5,%xmm10                    # (rtail-(t-rhead))
       addsd    %xmm6,%xmm10                    # rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov   %r8d,region1(%rsp)         # store lower region

#       movsd   %xmm7,%xmm1
#       subsd   xmm1, xmm10                 ; xmm10 = r=(rhead-rtail)
#       subsd   %xmm1,%xmm7                 ; rr=rhead-r
#       subsd   xmm7, xmm10                 ; xmm6 = rr=((rhead-r) -rtail)

        subsd   %xmm10,%xmm7                    # xmm10 = r=(rhead-rtail)

#       movlpd  QWORD PTR r1[rsp], xmm1             ; store upper r
#       movlpd  QWORD PTR rr1[rsp], xmm7            ; store upper rr

        movlpd   %xmm7,r1(%rsp)             # store upper r

#Work on Upper arg
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
    mov     $0x07ff0000000000000,%r11           # is upper arg nan/inf
    mov     %r11,%r10
    and     %r9,%r10
    cmp     %r11,%r10
    jz      .L__vrs4_sinf_upper_naninf_higher

    lea  region1+4(%rsp),%rdx           # upper arg is **NOT** nan/inf
    lea  r1+8(%rsp),%rsi

# changed input from xmm10 to xmm0
    mov  r1+8(%rsp),%rdi

    call     fname_remainder_piby2d2f@PLT

    jmp     0f

.L__vrs4_sinf_upper_naninf_higher:
    mov $0x00008000000000000,%r11
    or  %r11,%r9
    mov  %r9,r1+8(%rsp)             # r = x | 0x0008000000000000
    mov  %r10d,region1+4(%rsp)          # region =0

.align 16
0:
    jmp .L__vrs4_sinf_reconstruct


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.L__vrs4_sinf_reconstruct:
#Results
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    movapd  r(%rsp),%xmm10
    movapd  r1(%rsp),%xmm1

    mov region(%rsp),%r8
    mov region1(%rsp),%r9
    mov     .L__reald_one_zero(%rip),%rdx       #compare value for cossin path

    mov     %r8,%r10
    mov     %r9,%r11

    and .L__reald_one_one(%rip),%r8     #odd/even region for cos/sin
    and .L__reald_one_one(%rip),%r9     #odd/even region for cos/sin

    shr $1,%r10                     #~AB+A~B, A is sign and B is upper bit of region
    shr $1,%r11                     #~AB+A~B, A is sign and B is upper bit of region

    mov %r10,%rax
    mov %r11,%rcx

    not     %r12                        #ADDED TO CHANGE THE LOGIC
    not     %r13                        #ADDED TO CHANGE THE LOGIC
    and %r12,%r10
    and %r13,%r11

    not %rax
    not %rcx
    not %r12
    not %r13
    and %r12,%rax
    and %r13,%rcx

    or  %rax,%r10
    or  %rcx,%r11
    and .L__reald_one_one(%rip),%r10                #(~AB+A~B)&1
    and .L__reald_one_one(%rip),%r11                #(~AB+A~B)&1

    mov %r10,%r12
    mov %r11,%r13

    and %rdx,%r12               #mask out the lower sign bit leaving the upper sign bit
    and %rdx,%r13               #mask out the lower sign bit leaving the upper sign bit

    shl $63,%r10                #shift lower sign bit left by 63 bits
    shl $63,%r11                #shift lower sign bit left by 63 bits
    shl $31,%r12                #shift upper sign bit left by 31 bits
    shl $31,%r13                #shift upper sign bit left by 31 bits

    mov      %r10,p_sign(%rsp)      #write out lower sign bit
    mov      %r12,p_sign+8(%rsp)        #write out upper sign bit
    mov      %r11,p_sign1(%rsp)     #write out lower sign bit
    mov      %r13,p_sign1+8(%rsp)       #write out upper sign bit

    mov %r8,%rax
    mov %r9,%rcx

    movapd  %xmm10,%xmm2
    movapd  %xmm1,%xmm3

    mulpd   %xmm10,%xmm2                # r2
    mulpd   %xmm1,%xmm3             # r2

    and .L__reald_zero_one(%rip),%rax
    and .L__reald_zero_one(%rip),%rcx
    shr $31,%r8
    shr $31,%r9
    or  %r8,%rax
    or  %r9,%rcx
    shl $2,%rcx
    or  %rcx,%rax


    lea .Levensin_oddcos_tbl(%rip),%rcx
    jmp *(%rcx,%rax,8)              #Jmp table for cos/sin calculation based on even/odd region

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.L__vrs4_sinf_cleanup:

    movapd    p_sign(%rsp),%xmm10
    movapd    p_sign1(%rsp),%xmm1

    xorpd   %xmm4,%xmm10            # (+) Sign
    xorpd   %xmm5,%xmm1         # (+) Sign

    cvtpd2ps %xmm10,%xmm0
    cvtpd2ps %xmm1,%xmm11
    movlhps  %xmm11,%xmm0

    mov save_r12(%rsp),%r12 # restore r12
    mov save_r13(%rsp),%r13 # restore r13

    add $0x01E8,%rsp
    ret

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;JUMP TABLE;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lcoscos_coscos_piby4:
    movapd  %xmm2,%xmm0                 # r
    movapd  %xmm3,%xmm11                    # r

    movdqa  .Lcosarray+0x30(%rip),%xmm4         # c4
    movdqa  .Lcosarray+0x30(%rip),%xmm5         # c4

    movapd  .Lcosarray+0x10(%rip),%xmm8         # c2
    movapd  .Lcosarray+0x10(%rip),%xmm9         # c2

    mulpd   .L__real_3fe0000000000000(%rip),%xmm0   # r = 0.5 *x2
    mulpd   .L__real_3fe0000000000000(%rip),%xmm11  # r = 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2

    mulpd   %xmm2,%xmm8                 # c3*x2
    mulpd   %xmm3,%xmm9                 # c3*x2

    subpd   .L__real_3ff0000000000000(%rip),%xmm0       # -t=r-1.0  ;trash r
    subpd   .L__real_3ff0000000000000(%rip),%xmm11  # -t=r-1.0  ;trash r

    mulpd   %xmm2,%xmm2                 # x4
    mulpd   %xmm3,%xmm3                 # x4

    addpd   .Lcosarray+0x20(%rip),%xmm4         # c3+x2c4
    addpd   .Lcosarray+0x20(%rip),%xmm5         # c3+x2c4

    addpd   .Lcosarray(%rip),%xmm8          # c1+x2c2
    addpd   .Lcosarray(%rip),%xmm9          # c1+x2c2

    mulpd   %xmm2,%xmm4                 # x4(c3+x2c4)
    mulpd   %xmm3,%xmm5                 # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zc
    addpd   %xmm9,%xmm5                 # zc

    mulpd   %xmm2,%xmm4                 # x4 * zc
    mulpd   %xmm3,%xmm5                 # x4 * zc

    subpd   %xmm0,%xmm4                 # + t
    subpd   %xmm11,%xmm5                    # + t

    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lcossin_cossin_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    movdqa  .Lsincosarray+0x30(%rip),%xmm4      # s4
    movdqa  .Lsincosarray+0x30(%rip),%xmm5      # s4
    movapd  .Lsincosarray+0x10(%rip),%xmm8      # s2
    movapd  .Lsincosarray+0x10(%rip),%xmm9      # s2

    movapd  %xmm2,%xmm0             # move x2 for x4
    movapd  %xmm3,%xmm11                # move x2 for x4

    mulpd   %xmm2,%xmm4             # x2s4
    mulpd   %xmm3,%xmm5             # x2s4
    mulpd   %xmm2,%xmm8             # x2s2
    mulpd   %xmm3,%xmm9             # x2s2

    mulpd   %xmm2,%xmm0             # x4
    mulpd   %xmm3,%xmm11                # x4

    addpd   .Lsincosarray+0x20(%rip),%xmm4      # s4+x2s3
    addpd   .Lsincosarray+0x20(%rip),%xmm5      # s4+x2s3
    addpd   .Lsincosarray(%rip),%xmm8       # s2+x2s1
    addpd   .Lsincosarray(%rip),%xmm9       # s2+x2s1

    mulpd   %xmm0,%xmm4             # x4(s3+x2s4)
    mulpd   %xmm11,%xmm5                # x4(s3+x2s4)

    movhlps %xmm0,%xmm0             # move high x4 for cos term
    movhlps %xmm11,%xmm11               # move high x4 for cos term

    movsd   %xmm2,%xmm6             # move low x2 for x3 for sin term
    movsd   %xmm3,%xmm7             # move low x2 for x3 for sin term
    mulsd   %xmm10,%xmm6                # get low x3 for sin term
    mulsd   %xmm1,%xmm7             # get low x3 for sin term

    mulpd   .L__real_3fe0000000000000(%rip),%xmm2   # 0.5*x2 for sin and cos terms
    mulpd   .L__real_3fe0000000000000(%rip),%xmm3   # 0.5*x2 for sin and cos terms

    addpd   %xmm8,%xmm4             # z
    addpd   %xmm9,%xmm5             # z

    movhlps %xmm2,%xmm12                # move high r for cos
    movhlps %xmm3,%xmm13                # move high r for cos

    movhlps %xmm4,%xmm8             # xmm4 = sin , xmm8 = cos
    movhlps %xmm5,%xmm9             # xmm4 = sin , xmm8 = cos

    mulsd   %xmm6,%xmm4             # sin *x3
    mulsd   %xmm7,%xmm5             # sin *x3

    mulsd   %xmm0,%xmm8             # cos *x4
    mulsd   %xmm11,%xmm9                # cos *x4

    subsd   .L__real_3ff0000000000000(%rip),%xmm12  #-t=r-1.0
    subsd   .L__real_3ff0000000000000(%rip),%xmm13  #-t=r-1.0

    addsd   %xmm10,%xmm4                # sin + x
    addsd   %xmm1,%xmm5             # sin + x
    subsd   %xmm12,%xmm8                # cos+t
    subsd   %xmm13,%xmm9                # cos+t

    movlhps %xmm8,%xmm4
    movlhps %xmm9,%xmm5

    jmp     .L__vrs4_sinf_cleanup
.align 16
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lsincos_cossin_piby4:

    movapd  .Lsincosarray+0x30(%rip),%xmm4      # s4
    movapd  .Lcossinarray+0x30(%rip),%xmm5      # s4
    movdqa  .Lsincosarray+0x10(%rip),%xmm8      # s2
    movdqa  .Lcossinarray+0x10(%rip),%xmm9      # s2

    movapd  %xmm2,%xmm0             # move x2 for x4
    movapd  %xmm3,%xmm11                # move x2 for x4
    movapd  %xmm3,%xmm7             # sincos term upper x2 for x3

    mulpd   %xmm2,%xmm4             # x2s4
    mulpd   %xmm3,%xmm5             # x2s4
    mulpd   %xmm2,%xmm8             # x2s2
    mulpd   %xmm3,%xmm9             # x2s2

    mulpd   %xmm2,%xmm0             # x4
    mulpd   %xmm3,%xmm11                # x4

    addpd   .Lsincosarray+0x20(%rip),%xmm4      # s3+x2s4
    addpd   .Lcossinarray+0x20(%rip),%xmm5      # s3+x2s4
    addpd   .Lsincosarray(%rip),%xmm8       # s1+x2s2
    addpd   .Lcossinarray(%rip),%xmm9       # s1+x2s2

    mulpd   %xmm0,%xmm4             # x4(s3+x2s4)
    mulpd   %xmm11,%xmm5                # x4(s3+x2s4)

    movhlps %xmm0,%xmm0             # move high x4 for cos term

    movsd   %xmm2,%xmm6             # move low x2 for x3 for sin term  (cossin)
    mulpd   %xmm1,%xmm7

    mulsd   %xmm10,%xmm6                # get low x3 for sin term (cossin)
    movhlps %xmm7,%xmm7             # get high x3 for sin term (sincos)

    mulpd   .L__real_3fe0000000000000(%rip),%xmm2   # 0.5*x2 for cos term
    mulpd   .L__real_3fe0000000000000(%rip),%xmm3   # 0.5*x2 for cos term

    addpd   %xmm8,%xmm4             # z
    addpd   %xmm9,%xmm5             # z


    movhlps %xmm2,%xmm12                # move high r for cos (cossin)


    movhlps %xmm4,%xmm8             # xmm8 = cos , xmm4 = sin   (cossin)
    movhlps %xmm5,%xmm9             # xmm9 = sin , xmm5 = cos   (sincos)

    mulsd   %xmm6,%xmm4             # sin *x3
    mulsd   %xmm11,%xmm5                # cos *x4
    mulsd   %xmm0,%xmm8             # cos *x4
    mulsd   %xmm7,%xmm9             # sin *x3

    subsd   .L__real_3ff0000000000000(%rip),%xmm12  #-t=r-1.0
    subsd   .L__real_3ff0000000000000(%rip),%xmm3   # -t=r-1.0

    movhlps %xmm1,%xmm11                # move high x for x for sin term    (sincos)

    addsd   %xmm10,%xmm4                # sin + x   +
    addsd   %xmm11,%xmm9                # sin + x   +

    subsd   %xmm12,%xmm8                # cos+t
    subsd   %xmm3,%xmm5             # cos+t

    movlhps %xmm8,%xmm4             # cossin
    movlhps %xmm9,%xmm5             # sincos

    jmp .L__vrs4_sinf_cleanup

.align 16
.Lsincos_sincos_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    movapd  .Lcossinarray+0x30(%rip),%xmm4      # s4
    movapd  .Lcossinarray+0x30(%rip),%xmm5      # s4
    movdqa  .Lcossinarray+0x10(%rip),%xmm8      # s2
    movdqa  .Lcossinarray+0x10(%rip),%xmm9      # s2

    movapd  %xmm2,%xmm0             # move x2 for x4
    movapd  %xmm3,%xmm11                # move x2 for x4
    movapd  %xmm2,%xmm6             # move x2 for x4
    movapd  %xmm3,%xmm7             # move x2 for x4

    mulpd   %xmm2,%xmm4             # x2s6
    mulpd   %xmm3,%xmm5             # x2s6
    mulpd   %xmm2,%xmm8             # x2s3
    mulpd   %xmm3,%xmm9             # x2s3

    mulpd   %xmm2,%xmm0             # x4
    mulpd   %xmm3,%xmm11                # x4

    addpd   .Lcossinarray+0x20(%rip),%xmm4      # s4+x2s3
    addpd   .Lcossinarray+0x20(%rip),%xmm5      # s4+x2s3
    addpd   .Lcossinarray(%rip),%xmm8       # s2+x2s1
    addpd   .Lcossinarray(%rip),%xmm9       # s2+x2s1

    mulpd   %xmm0,%xmm4             # x4(s4+x2s3)
    mulpd   %xmm11,%xmm5                # x4(s4+x2s3)

    mulpd   %xmm10,%xmm6                # get low x3 for sin term
    mulpd   %xmm1,%xmm7             # get low x3 for sin term
    movhlps %xmm6,%xmm6             # move low x2 for x3 for sin term
    movhlps %xmm7,%xmm7             # move low x2 for x3 for sin term

    mulsd   .L__real_3fe0000000000000(%rip),%xmm2   # 0.5*x2 for cos terms
    mulsd   .L__real_3fe0000000000000(%rip),%xmm3   # 0.5*x2 for cos terms

    addpd   %xmm8,%xmm4             # z
    addpd   %xmm9,%xmm5             # z

    movhlps %xmm4,%xmm12                # xmm8 = sin , xmm4 = cos
    movhlps %xmm5,%xmm13                # xmm9 = sin , xmm5 = cos

    mulsd   %xmm6,%xmm12                # sin *x3
    mulsd   %xmm7,%xmm13                # sin *x3
    mulsd   %xmm0,%xmm4             # cos *x4
    mulsd   %xmm11,%xmm5                # cos *x4

    subsd   .L__real_3ff0000000000000(%rip),%xmm2   #-t=r-1.0
    subsd   .L__real_3ff0000000000000(%rip),%xmm3   #-t=r-1.0

    movhlps %xmm10,%xmm0                # move high x for x for sin term
    movhlps %xmm1,%xmm11                # move high x for x for sin term
                            # Reverse 10 and 0

    addsd   %xmm0,%xmm12                # sin + x
    addsd   %xmm11,%xmm13               # sin + x

    subsd   %xmm2,%xmm4             # cos+t
    subsd   %xmm3,%xmm5             # cos+t

    movlhps %xmm12,%xmm4
    movlhps %xmm13,%xmm5
    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lcossin_sincos_piby4:

    movapd  .Lcossinarray+0x30(%rip),%xmm4      # s4
    movapd  .Lsincosarray+0x30(%rip),%xmm5      # s4
    movdqa  .Lcossinarray+0x10(%rip),%xmm8      # s2
    movdqa  .Lsincosarray+0x10(%rip),%xmm9      # s2

    movapd  %xmm2,%xmm0             # move x2 for x4
    movapd  %xmm3,%xmm11                # move x2 for x4
    movapd  %xmm2,%xmm7             # upper x2 for x3 for sin term (sincos)

    mulpd   %xmm2,%xmm4             # x2s4
    mulpd   %xmm3,%xmm5             # x2s4
    mulpd   %xmm2,%xmm8             # x2s2
    mulpd   %xmm3,%xmm9             # x2s2

    mulpd   %xmm2,%xmm0             # x4
    mulpd   %xmm3,%xmm11                # x4

    addpd   .Lcossinarray+0x20(%rip),%xmm4      # s3+x2s4
    addpd   .Lsincosarray+0x20(%rip),%xmm5      # s3+x2s4
    addpd   .Lcossinarray(%rip),%xmm8       # s1+x2s2
    addpd   .Lsincosarray(%rip),%xmm9       # s1+x2s2

    mulpd   %xmm0,%xmm4             # x4(s3+x2s4)
    mulpd   %xmm11,%xmm5                # x4(s3+x2s4)

    movhlps %xmm11,%xmm11               # move high x4 for cos term

    movsd   %xmm3,%xmm6             # move low x2 for x3 for sin term  (cossin)
    mulpd   %xmm10,%xmm7

    mulsd   %xmm1,%xmm6             # get low x3 for sin term (cossin)
    movhlps %xmm7,%xmm7             # get high x3 for sin term (sincos)

    mulsd   .L__real_3fe0000000000000(%rip),%xmm2   # 0.5*x2 for cos term
    mulpd   .L__real_3fe0000000000000(%rip),%xmm3   # 0.5*x2 for cos term

    addpd   %xmm8,%xmm4             # z
    addpd   %xmm9,%xmm5             # z


    movhlps %xmm3,%xmm12                # move high r for cos (cossin)


    movhlps %xmm4,%xmm8             # xmm8 = sin , xmm4 = cos   (sincos)
    movhlps %xmm5,%xmm9             # xmm9 = cos , xmm5 = sin   (cossin)

    mulsd   %xmm0,%xmm4             # cos *x4
    mulsd   %xmm6,%xmm5             # sin *x3
    mulsd   %xmm7,%xmm8             # sin *x3
    mulsd   %xmm11,%xmm9                # cos *x4

    subsd   .L__real_3ff0000000000000(%rip),%xmm2   # -t=r-1.0
    subsd   .L__real_3ff0000000000000(%rip),%xmm12  # -t=r-1.0

    movhlps %xmm10,%xmm11               # move high x for x for sin term    (sincos)

    subsd   %xmm2,%xmm4             # cos-(-t)
    subsd   %xmm12,%xmm9                # cos-(-t)

    addsd   %xmm11,%xmm8                # sin + x
    addsd   %xmm1,%xmm5             # sin + x

    movlhps %xmm8,%xmm4             # cossin
    movlhps %xmm9,%xmm5             # sincos

    jmp .L__vrs4_sinf_cleanup
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lcoscos_sinsin_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr:     SIN
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr:  COS
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    movapd  %xmm2,%xmm0                 # x2    ; SIN
    movapd  %xmm3,%xmm11                    # x2    ; COS
    movapd  %xmm3,%xmm1                 # copy of x2 for x4

    movdqa  .Lsinarray+0x30(%rip),%xmm4         # c4
    movdqa  .Lcosarray+0x30(%rip),%xmm5         # c4
    movapd  .Lsinarray+0x10(%rip),%xmm8         # c2
    movapd  .Lcosarray+0x10(%rip),%xmm9         # c2

    mulpd   .L__real_3fe0000000000000(%rip),%xmm11      # r = 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    mulpd   %xmm2,%xmm0                 # x4
    subpd   .L__real_3ff0000000000000(%rip),%xmm11      # -t=r-1.0
    mulpd   %xmm3,%xmm1                 # x4

    addpd   .Lsinarray+0x20(%rip),%xmm4         # c3+x2c4
    addpd   .Lcosarray+0x20(%rip),%xmm5         # c3+x2c4
    addpd   .Lsinarray(%rip),%xmm8          # c1+x2c2
    addpd   .Lcosarray(%rip),%xmm9          # c1+x2c2

    mulpd   %xmm10,%xmm2                    # x3

    mulpd   %xmm0,%xmm4                 # x4(c3+x2c4)
    mulpd   %xmm1,%xmm5                 # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zs
    addpd   %xmm9,%xmm5                 # zc

    mulpd   %xmm2,%xmm4                 # x3 * zs
    mulpd   %xmm1,%xmm5                 # x4 * zc

    addpd   %xmm10,%xmm4                    # +x
    subpd   %xmm11,%xmm5                    # +t

    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lsinsin_coscos_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr:     COS
# p_sign1  = Sign, xmm1  = r, xmm3 = %xmm7,%r2 =rr: SIN
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    movapd  %xmm2,%xmm0                 # x2    ; COS
    movapd  %xmm3,%xmm11                    # x2    ; SIN
    movapd  %xmm2,%xmm10                    # copy of x2 for x4

    movdqa  .Lcosarray+0x30(%rip),%xmm4         # c4
    movdqa  .Lsinarray+0x30(%rip),%xmm5         # s4
    movapd  .Lcosarray+0x10(%rip),%xmm8         # c2
    movapd  .Lsinarray+0x10(%rip),%xmm9         # s2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # s4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # s2*x2

    mulpd   %xmm2,%xmm10                    # x4
    mulpd   %xmm3,%xmm11                    # x4

    addpd   .Lcosarray+0x20(%rip),%xmm4         # c3+x2c4
    addpd   .Lsinarray+0x20(%rip),%xmm5         # s3+x2c4
    addpd   .Lcosarray(%rip),%xmm8          # c1+x2c2
    addpd   .Lsinarray(%rip),%xmm9          # s1+x2c2

    mulpd   %xmm1,%xmm3                 # x3
    mulpd   .L__real_3fe0000000000000(%rip),%xmm0       # r = 0.5 *x2

    mulpd   %xmm10,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm11,%xmm5                    # x4(s3+x2s4)

    subpd   .L__real_3ff0000000000000(%rip),%xmm0       # -t=r-1.0
    addpd   %xmm8,%xmm4                 # zc
    addpd   %xmm9,%xmm5                 # zs

    mulpd   %xmm10,%xmm4                    # x4 * zc
    mulpd   %xmm3,%xmm5                 # x3 * zc

    subpd   %xmm0,%xmm4                 # +t
    addpd   %xmm1,%xmm5                 # +x

    jmp     .L__vrs4_sinf_cleanup
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lcoscos_cossin_piby4:              #Derive from cossin_coscos
    movhlps %xmm2,%xmm0                 # x2 for 0.5x2 for upper cos
    movsd   %xmm2,%xmm6                 # lower x2 for x3 for lower sin
    movapd  %xmm3,%xmm11                    # x2 for 0.5x2
    movapd  %xmm2,%xmm12                    # x2 for x4
    movapd  %xmm3,%xmm13                    # x2 for x4

    movsd   .L__real_3ff0000000000000(%rip),%xmm7

    movdqa  .Lsincosarray+0x30(%rip),%xmm4          # c4
    movdqa  .Lcosarray+0x30(%rip),%xmm5         # c4
    movapd  .Lsincosarray+0x10(%rip),%xmm8          # c2
    movapd  .Lcosarray+0x10(%rip),%xmm9         # c2


    mulsd   .L__real_3fe0000000000000(%rip),%xmm0   # 0.5 *x2
    mulpd   .L__real_3fe0000000000000(%rip),%xmm11  # 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    subsd   %xmm0,%xmm7                 #  t=1.0-r  for cos
    subpd   .L__real_3ff0000000000000(%rip),%xmm11  # -t=r-1.0
    mulpd   %xmm2,%xmm12                    # x4
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lsincosarray+0x20(%rip),%xmm4          # c4+x2c3
    addpd   .Lcosarray+0x20(%rip),%xmm5         # c4+x2c3
    addpd   .Lsincosarray(%rip),%xmm8           # c2+x2c1
    addpd   .Lcosarray(%rip),%xmm9          # c2+x2c1


    movapd  %xmm12,%xmm2                    # upper=x4
    movsd   %xmm6,%xmm2                 # lower=x2
    mulsd   %xmm10,%xmm2                    # lower=x2*x

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zczs
    addpd   %xmm9,%xmm5                 # zc

    mulpd   %xmm2,%xmm4                 # upper= x4 * zc
                                # lower=x3 * zs
    mulpd   %xmm13,%xmm5                    # x4 * zc


    movlhps %xmm7,%xmm10                    #
    addpd   %xmm10,%xmm4                    # +x for lower sin, +t for upper cos
    subpd   %xmm11,%xmm5                    # -(-t)

    jmp     .L__vrs4_sinf_cleanup
.align 16
.Lcoscos_sincos_piby4:              #Derive from cossin_coscos
    movsd   %xmm2,%xmm0                 # x2 for 0.5x2 for lower cos
    movapd  %xmm3,%xmm11                    # x2 for 0.5x2
    movapd  %xmm2,%xmm12                    # x2 for x4
    movapd  %xmm3,%xmm13                    # x2 for x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm7

    movdqa  .Lcossinarray+0x30(%rip),%xmm4          # cs4
    movdqa  .Lcosarray+0x30(%rip),%xmm5         # c4
    movapd  .Lcossinarray+0x10(%rip),%xmm8          # cs2
    movapd  .Lcosarray+0x10(%rip),%xmm9         # c2

    mulsd   .L__real_3fe0000000000000(%rip),%xmm0       # 0.5 *x2
    mulpd   .L__real_3fe0000000000000(%rip),%xmm11      # 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    subsd   %xmm0,%xmm7                 # t=1.0-r  for cos
    subpd   .L__real_3ff0000000000000(%rip),%xmm11      # -t=r-1.0
    mulpd   %xmm2,%xmm12                    # x4
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lcossinarray+0x20(%rip),%xmm4          # c4+x2c3
    addpd   .Lcosarray+0x20(%rip),%xmm5         # c4+x2c3
    addpd   .Lcossinarray(%rip),%xmm8           # c2+x2c1
    addpd   .Lcosarray(%rip),%xmm9              # c2+x2c1

    mulpd   %xmm10,%xmm2                    # upper=x3 for sin
    mulsd   %xmm10,%xmm2                    # lower=x4 for cos

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zczs
    addpd   %xmm9,%xmm5                 # zc

    mulpd   %xmm2,%xmm4                 # lower= x4 * zc
                                # upper= x3 * zs
    mulpd   %xmm13,%xmm5                    # x4 * zc


    movsd   %xmm7,%xmm10
    addpd   %xmm10,%xmm4                    # +x for upper sin, +t for lower cos
    subpd   %xmm11,%xmm5                    # -(-t)

    jmp     .L__vrs4_sinf_cleanup
.align 16
.Lcossin_coscos_piby4:
    movhlps %xmm3,%xmm0                 # x2 for 0.5x2 for upper cos
    movapd  %xmm2,%xmm11                    # x2 for 0.5x2
    movapd  %xmm2,%xmm12                    # x2 for x4
    movapd  %xmm3,%xmm13                    # x2 for x4
    movsd   %xmm3,%xmm6                 # lower x2 for x3 for sin
    movsd   .L__real_3ff0000000000000(%rip),%xmm7

    movdqa  .Lcosarray+0x30(%rip),%xmm4         # cs4
    movdqa  .Lsincosarray+0x30(%rip),%xmm5          # c4
    movapd  .Lcosarray+0x10(%rip),%xmm8         # cs2
    movapd  .Lsincosarray+0x10(%rip),%xmm9          # c2


    mulsd   .L__real_3fe0000000000000(%rip),%xmm0       # 0.5 *x2
    mulpd   .L__real_3fe0000000000000(%rip),%xmm11      # 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    subsd   %xmm0,%xmm7                 # t=1.0-r  for cos
    subpd   .L__real_3ff0000000000000(%rip),%xmm11      # -t=r-1.0
    mulpd   %xmm2,%xmm12                    # x4
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lcosarray+0x20(%rip),%xmm4         # c4+x2c3
    addpd   .Lsincosarray+0x20(%rip),%xmm5          # c4+x2c3
    addpd   .Lcosarray(%rip),%xmm8          # c2+x2c1
    addpd   .Lsincosarray(%rip),%xmm9           # c2+x2c1

    movapd  %xmm13,%xmm3                    # upper=x4
    movsd   %xmm6,%xmm3                 # lower x2
    mulsd   %xmm1,%xmm3                 # lower x2*x

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zczs
    addpd   %xmm9,%xmm5                 # zc

    mulpd   %xmm12,%xmm4                    # x4 * zc
    mulpd   %xmm3,%xmm5                 # upper= x4 * zc
                                # lower=x3 * zs

    movlhps %xmm7,%xmm1
    addpd   %xmm1,%xmm5                 # +x for lower sin, +t for upper cos
    subpd   %xmm11,%xmm4                    # -(-t)

    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lcossin_sinsin_piby4:      # Derived from sincos_coscos

    movhlps %xmm3,%xmm0                 # x2
    movapd  %xmm3,%xmm7
    movapd  %xmm2,%xmm12                    # copy of x2 for x4
    movapd  %xmm3,%xmm13                    # copy of x2 for x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm11

    movdqa  .Lsinarray+0x30(%rip),%xmm4         # c4
    movdqa  .Lsincosarray+0x30(%rip),%xmm5          # c4
    movapd  .Lsinarray+0x10(%rip),%xmm8         # c2
    movapd  .Lsincosarray+0x10(%rip),%xmm9          # c2

    mulsd   .L__real_3fe0000000000000(%rip),%xmm0       # r = 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    mulpd   %xmm2,%xmm12                    # x4
    subsd   %xmm0,%xmm11                    # t=1.0-r for cos
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lsinarray+0x20(%rip),%xmm4         # c3+x2c4
    addpd   .Lsincosarray+0x20(%rip),%xmm5          # c3+x2c4
    addpd   .Lsinarray(%rip),%xmm8          # c1+x2c2
    addpd   .Lsincosarray(%rip),%xmm9           # c1+x2c2

    mulpd   %xmm10,%xmm2                    # x3
    movapd  %xmm13,%xmm3                    # upper x4 for cos
    movsd   %xmm7,%xmm3                 # lower x2 for sin
    mulsd   %xmm1,%xmm3                 # lower x3=x2*x for sin

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    movlhps %xmm11,%xmm1                    # t for upper cos and x for lower sin

    addpd   %xmm8,%xmm4                 # zczs
    addpd   %xmm9,%xmm5                 # zs

    mulpd   %xmm2,%xmm4                 # x3 * zs
    mulpd   %xmm3,%xmm5                 # upper=x4 * zc
                                # lower=x3 * zs

    addpd   %xmm10,%xmm4                    # +x
    addpd   %xmm1,%xmm5                 # +t upper, +x lower


    jmp     .L__vrs4_sinf_cleanup
.align 16
.Lsincos_coscos_piby4:
    movsd   %xmm3,%xmm0                 # x2 for 0.5x2 for lower cos
    movapd  %xmm2,%xmm11                    # x2 for 0.5x2
    movapd  %xmm2,%xmm12                    # x2 for x4
    movapd  %xmm3,%xmm13                    # x2 for x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm7

    movdqa  .Lcosarray+0x30(%rip),%xmm4         # cs4
    movdqa  .Lcossinarray+0x30(%rip),%xmm5          # c4
    movapd  .Lcosarray+0x10(%rip),%xmm8         # cs2
    movapd  .Lcossinarray+0x10(%rip),%xmm9          # c2

    mulsd   .L__real_3fe0000000000000(%rip),%xmm0   # 0.5 *x2
    mulpd   .L__real_3fe0000000000000(%rip),%xmm11  # 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    subsd   %xmm0,%xmm7                 # t=1.0-r  for cos
    subpd   .L__real_3ff0000000000000(%rip),%xmm11  # -t=r-1.0
    mulpd   %xmm2,%xmm12                    # x4
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lcosarray+0x20(%rip),%xmm4         # c4+x2c3
    addpd   .Lcossinarray+0x20(%rip),%xmm5          # c4+x2c3
    addpd   .Lcosarray(%rip),%xmm8          # c2+x2c1
    addpd   .Lcossinarray(%rip),%xmm9           # c2+x2c1

    mulpd   %xmm1,%xmm3                 # upper=x3 for sin
    mulsd   %xmm1,%xmm3                 # lower=x4 for cos

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zczs
    addpd   %xmm9,%xmm5                 # zc

    mulpd   %xmm12,%xmm4                    # x4 * zc
    mulpd   %xmm3,%xmm5                 # lower= x4 * zc
                                # upper= x3 * zs

    movsd   %xmm7,%xmm1
    subpd   %xmm11,%xmm4                    # -(-t)
    addpd   %xmm1,%xmm5                 # +x for upper sin, +t for lower cos


    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lsincos_sinsin_piby4:      # Derived from sincos_coscos

    movsd   %xmm3,%xmm0                 # x2
    movapd  %xmm2,%xmm12                    # copy of x2 for x4
    movapd  %xmm3,%xmm13                    # copy of x2 for x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm11

    movdqa  .Lsinarray+0x30(%rip),%xmm4         # c4
    movdqa  .Lcossinarray+0x30(%rip),%xmm5          # c4
    movapd  .Lsinarray+0x10(%rip),%xmm8         # c2
    movapd  .Lcossinarray+0x10(%rip),%xmm9          # c2

    mulsd   .L__real_3fe0000000000000(%rip),%xmm0       # r = 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    mulpd   %xmm2,%xmm12                    # x4
    subsd   %xmm0,%xmm11                    # t=1.0-r for cos
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lsinarray+0x20(%rip),%xmm4         # c3+x2c4
    addpd   .Lcossinarray+0x20(%rip),%xmm5          # c3+x2c4
    addpd   .Lsinarray(%rip),%xmm8              # c1+x2c2
    addpd   .Lcossinarray(%rip),%xmm9           # c1+x2c2

    mulpd   %xmm10,%xmm2                    # x3
    mulpd   %xmm1,%xmm3                 # upper x3 for sin
    mulsd   %xmm1,%xmm3                 # lower x4 for cos

    movhlps %xmm1,%xmm6

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    movlhps %xmm6,%xmm11                    # upper =t ; lower =x

    addpd   %xmm8,%xmm4                 # zs
    addpd   %xmm9,%xmm5                 # zszc

    mulpd   %xmm2,%xmm4                 # x3 * zs
    mulpd   %xmm3,%xmm5                 # lower=x4 * zc
                                # upper=x3 * zs

    addpd   %xmm10,%xmm4                    # +x
    addpd   %xmm11,%xmm5                    # +t lower, +x upper

    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lsinsin_cossin_piby4:      # Derived from sincos_coscos

    movhlps %xmm2,%xmm0                 # x2
    movapd  %xmm2,%xmm7
    movapd  %xmm2,%xmm12                    # copy of x2 for x4
    movapd  %xmm3,%xmm13                    # copy of x2 for x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm11

    movdqa  .Lsincosarray+0x30(%rip),%xmm4          # c4
    movdqa  .Lsinarray+0x30(%rip),%xmm5         # c4
    movapd  .Lsincosarray+0x10(%rip),%xmm8          # c2
    movapd  .Lsinarray+0x10(%rip),%xmm9         # c2

    mulsd   .L__real_3fe0000000000000(%rip),%xmm0       # r = 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    mulpd   %xmm2,%xmm12                    # x4
    subsd   %xmm0,%xmm11                    # t=1.0-r for cos
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lsincosarray+0x20(%rip),%xmm4          # c3+x2c4
    addpd   .Lsinarray+0x20(%rip),%xmm5         # c3+x2c4
    addpd   .Lsincosarray(%rip),%xmm8           # c1+x2c2
    addpd   .Lsinarray(%rip),%xmm9          # c1+x2c2

    mulpd   %xmm1,%xmm3                 # x3
    movapd  %xmm12,%xmm2                    # upper x4 for cos
    movsd   %xmm7,%xmm2                 # lower x2 for sin
    mulsd   %xmm10,%xmm2                    # lower x3=x2*x for sin

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    movlhps %xmm11,%xmm10                   # t for upper cos and x for lower sin

    addpd   %xmm8,%xmm4                 # zs
    addpd   %xmm9,%xmm5                 # zszc

    mulpd   %xmm3,%xmm5                 # x3 * zs
    mulpd   %xmm2,%xmm4                 # upper=x4 * zc
                                # lower=x3 * zs

    addpd   %xmm1,%xmm5                 # +x
    addpd   %xmm10,%xmm4                    # +t upper, +x lower

    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lsinsin_sincos_piby4:      # Derived from sincos_coscos

    movsd   %xmm2,%xmm0                 # x2
    movapd  %xmm2,%xmm12                    # copy of x2 for x4
    movapd  %xmm3,%xmm13                    # copy of x2 for x4
    movsd   .L__real_3ff0000000000000(%rip),%xmm11

    movdqa  .Lcossinarray+0x30(%rip),%xmm4          # c4
    movdqa  .Lsinarray+0x30(%rip),%xmm5         # c4
    movapd  .Lcossinarray+0x10(%rip),%xmm8          # c2
    movapd  .Lsinarray+0x10(%rip),%xmm9         # c2

    mulsd   .L__real_3fe0000000000000(%rip),%xmm0       # r = 0.5 *x2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2
    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    mulpd   %xmm2,%xmm12                    # x4
    subsd   %xmm0,%xmm11                    # t=1.0-r for cos
    mulpd   %xmm3,%xmm13                    # x4

    addpd   .Lcossinarray+0x20(%rip),%xmm4          # c3+x2c4
    addpd   .Lsinarray+0x20(%rip),%xmm5         # c3+x2c4
    addpd   .Lcossinarray(%rip),%xmm8           # c1+x2c2
    addpd   .Lsinarray(%rip),%xmm9          # c1+x2c2

    mulpd   %xmm1,%xmm3                 # x3
    mulpd   %xmm10,%xmm2                    # upper x3 for sin
    mulsd   %xmm10,%xmm2                    # lower x4 for cos

    movhlps %xmm10,%xmm6

    mulpd   %xmm12,%xmm4                    # x4(c3+x2c4)
    mulpd   %xmm13,%xmm5                    # x4(c3+x2c4)

    movlhps %xmm6,%xmm11

    addpd   %xmm8,%xmm4                 # zs
    addpd   %xmm9,%xmm5                 # zszc

    mulpd   %xmm3,%xmm5                 # x3 * zs
    mulpd   %xmm2,%xmm4                 # lower=x4 * zc
                                # upper=x3 * zs

    addpd   %xmm1,%xmm5                 # +x
    addpd   %xmm11,%xmm4                    # +t lower, +x upper

    jmp     .L__vrs4_sinf_cleanup

.align 16
.Lsinsin_sinsin_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm10 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  #x2 = x * x;
  #(x + x * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * c4))));

  #x + x3 * ((c1 + x2 *c2) + x4 * (c3 + x2 * c4));


    movapd  %xmm2,%xmm0                 # x2
    movapd  %xmm3,%xmm11                    # x2

    movdqa  .Lsinarray+0x30(%rip),%xmm4         # c4
    movdqa  .Lsinarray+0x30(%rip),%xmm5         # c4

    mulpd   %xmm2,%xmm0                 # x4
    mulpd   %xmm3,%xmm11                    # x4

    movapd  .Lsinarray+0x10(%rip),%xmm8         # c2
    movapd  .Lsinarray+0x10(%rip),%xmm9         # c2

    mulpd   %xmm2,%xmm4                 # c4*x2
    mulpd   %xmm3,%xmm5                 # c4*x2

    mulpd   %xmm2,%xmm8                 # c2*x2
    mulpd   %xmm3,%xmm9                 # c2*x2

    addpd   .Lsinarray+0x20(%rip),%xmm4         # c3+x2c4
    addpd   .Lsinarray+0x20(%rip),%xmm5         # c3+x2c4

    mulpd   %xmm10,%xmm2                    # x3
    mulpd   %xmm1,%xmm3                 # x3

    addpd   .Lsinarray(%rip),%xmm8          # c1+x2c2
    addpd   .Lsinarray(%rip),%xmm9          # c1+x2c2

    mulpd   %xmm0,%xmm4                 # x4(c3+x2c4)
    mulpd   %xmm11,%xmm5                    # x4(c3+x2c4)

    addpd   %xmm8,%xmm4                 # zs
    addpd   %xmm9,%xmm5                 # zs

    mulpd   %xmm2,%xmm4                 # x3 * zs
    mulpd   %xmm3,%xmm5                 # x3 * zs

    addpd   %xmm10,%xmm4                    # +x
    addpd   %xmm1,%xmm5                 # +x

    jmp     .L__vrs4_sinf_cleanup
