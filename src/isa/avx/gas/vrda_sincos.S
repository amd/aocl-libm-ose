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
# vrdasincos.s
#
# An array implementation of the sincos libm function.
#
# Prototype:
#
#    void vrda_sincos(int n, double *x, double *ys, double *yc);
#
#Computes Sine of x for an array of input values.
#Places the results into the supplied ys array.
#Computes Cosine of x for an array of input values.
#Places the results into the supplied yc array.
#Does not perform error checking.
#Denormal inputs may produce unexpected results

#define INTEGER64
#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(vrda_sincos)


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.section .rodata
.align 16
.L__real_7fffffffffffffff: 	.quad 0x07fffffffffffffff	#Sign bit zero
				.quad 0x07fffffffffffffff
.L__real_3ff0000000000000: 	.quad 0x03ff0000000000000	# 1.0
				.quad 0x03ff0000000000000
.L__real_v2p__27:		.quad 0x03e40000000000000	# 2p-27
				.quad 0x03e40000000000000
.L__real_3fe0000000000000: 	.quad 0x03fe0000000000000	# 0.5
				.quad 0x03fe0000000000000
.L__real_3fc5555555555555: 	.quad 0x03fc5555555555555	# 0.166666666666
				.quad 0x03fc5555555555555
.L__real_3fe45f306dc9c883: 	.quad 0x03fe45f306dc9c883	# twobypi
				.quad 0x03fe45f306dc9c883
.L__real_3ff921fb54400000: 	.quad 0x03ff921fb54400000	# piby2_1
				.quad 0x03ff921fb54400000
.L__real_3dd0b4611a626331: 	.quad 0x03dd0b4611a626331	# piby2_1tail
				.quad 0x03dd0b4611a626331
.L__real_3dd0b4611a600000: 	.quad 0x03dd0b4611a600000	# piby2_2
				.quad 0x03dd0b4611a600000
.L__real_3ba3198a2e037073: 	.quad 0x03ba3198a2e037073	# piby2_2tail
				.quad 0x03ba3198a2e037073
.L__real_fffffffff8000000: 	.quad 0x0fffffffff8000000	# mask for stripping head and tail
				.quad 0x0fffffffff8000000
.L__real_8000000000000000:	.quad 0x08000000000000000	# -0  or signbit
				.quad 0x08000000000000000
.L__reald_one_one:		.quad 0x00000000100000001	#
				.quad 0
.L__reald_two_two:		.quad 0x00000000200000002	#
				.quad 0
.L__reald_one_zero:		.quad 0x00000000100000000	# sin_cos_filter
				.quad 0
.L__reald_zero_one:		.quad 0x00000000000000001	#
				.quad 0
.L__reald_two_zero:		.quad 0x00000000200000000	#
				.quad 0
.L__realq_one_one:		.quad 0x00000000000000001	#
				.quad 0x00000000000000001	#
.L__realq_two_two:		.quad 0x00000000000000002	#
				.quad 0x00000000000000002	#
.L__real_1_x_mask:		.quad 0x0ffffffffffffffff	#
				.quad 0x03ff0000000000000	#
.L__real_zero:			.quad 0x00000000000000000	#
				.quad 0x00000000000000000	#
.L__real_one:			.quad 0x00000000000000001	#
				.quad 0x00000000000000001	#
.L__real_jt_mask:		.quad 0x0000000000000000F	#
				.quad 0x00000000000000000	#
.L__real_naninf_upper_sign_mask:	.quad 0x000000000ffffffff	#
					.quad 0x000000000ffffffff	#
.L__real_naninf_lower_sign_mask:	.quad 0x0ffffffff00000000	#
					.quad 0x0ffffffff00000000	#

.Lcosarray:
	.quad	0x03fa5555555555555		# 0.0416667		   	c1
	.quad	0x03fa5555555555555
	.quad	0x0bf56c16c16c16967		# -0.00138889	   		c2
	.quad	0x0bf56c16c16c16967
	.quad	0x03efa01a019f4ec90		# 2.48016e-005			c3
	.quad	0x03efa01a019f4ec90
	.quad	0x0be927e4fa17f65f6		# -2.75573e-007			c4
	.quad	0x0be927e4fa17f65f6
	.quad	0x03e21eeb69037ab78		# 2.08761e-009			c5
	.quad	0x03e21eeb69037ab78
	.quad	0x0bda907db46cc5e42		# -1.13826e-011	   		c6
	.quad	0x0bda907db46cc5e42
.Lsinarray:
	.quad	0x0bfc5555555555555		# -0.166667	   		s1
	.quad	0x0bfc5555555555555
	.quad	0x03f81111111110bb3		# 0.00833333	   		s2
	.quad	0x03f81111111110bb3
	.quad	0x0bf2a01a019e83e5c		# -0.000198413			s3
	.quad	0x0bf2a01a019e83e5c
	.quad	0x03ec71de3796cde01		# 2.75573e-006			s4
	.quad	0x03ec71de3796cde01
	.quad	0x0be5ae600b42fdfa7		# -2.50511e-008			s5
	.quad	0x0be5ae600b42fdfa7
	.quad	0x03de5e0b2f9a43bb8		# 1.59181e-010	   		s6
	.quad	0x03de5e0b2f9a43bb8
.Lsincosarray:
	.quad	0x0bfc5555555555555		# -0.166667	   		s1
	.quad	0x03fa5555555555555		# 0.0416667		   	c1
	.quad	0x03f81111111110bb3		# 0.00833333	   		s2
	.quad	0x0bf56c16c16c16967
	.quad	0x0bf2a01a019e83e5c		# -0.000198413			s3
	.quad	0x03efa01a019f4ec90
	.quad	0x03ec71de3796cde01		# 2.75573e-006			s4
	.quad	0x0be927e4fa17f65f6
	.quad	0x0be5ae600b42fdfa7		# -2.50511e-008			s5
	.quad	0x03e21eeb69037ab78
	.quad	0x03de5e0b2f9a43bb8		# 1.59181e-010	   		s6
	.quad	0x0bda907db46cc5e42


.Lcossinarray:
	.quad	0x03fa5555555555555		# 0.0416667		   	c1
	.quad	0x0bfc5555555555555		# -0.166667	   		s1
	.quad	0x0bf56c16c16c16967
	.quad	0x03f81111111110bb3		# 0.00833333	   		s2
	.quad	0x03efa01a019f4ec90
	.quad	0x0bf2a01a019e83e5c		# -0.000198413			s3
	.quad	0x0be927e4fa17f65f6
	.quad	0x03ec71de3796cde01		# 2.75573e-006			s4
	.quad	0x03e21eeb69037ab78
	.quad	0x0be5ae600b42fdfa7		# -2.50511e-008			s5
	.quad	0x0bda907db46cc5e42
	.quad	0x03de5e0b2f9a43bb8		# 1.59181e-010	   		s6


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.text
    .align 16
    .p2align 4,,15


.globl fname
    .type   fname,@function
fname:
#    mov         (%rdi),%edi
.align 16
.p2align 4,,15

# define local variable storage offsets
.equ	save_xmm6,	0x00		# temporary for get/put bits operation
.equ	save_xmm7,	0x10		# temporary for get/put bits operation
.equ	save_xmm8,	0x20		# temporary for get/put bits operation
.equ	save_xmm9,	0x30		# temporary for get/put bits operation
.equ	save_xmm10,	0x40		# temporary for get/put bits operation
.equ	save_xmm11,	0x50		# temporary for get/put bits operation
.equ	save_xmm12,	0x60		# temporary for get/put bits operation
.equ	save_xmm13,	0x70		# temporary for get/put bits operation
.equ	save_xmm14,	0x80		# temporary for get/put bits operation
.equ	save_xmm15,	0x90		# temporary for get/put bits operation

.equ	save_rdi,	0x0A0
.equ	save_rsi,	0x0B0
.equ	save_rbx,	0x0C0

.equ	r,		0x0D0		# pointer to r for remainder_piby2
.equ	rr,		0x0E0		# pointer to r for remainder_piby2
.equ	rsq,		0x0F0
.equ	region,		0x0100		# pointer to r for remainder_piby2

.equ	r1,		0x0110		# pointer to r for remainder_piby2
.equ	rr1,		0x0120		# pointer to r for remainder_piby2
.equ	rsq1,		0x0130
.equ	region1,	0x0140		# pointer to r for remainder_piby2

.equ	p_temp,		0x0150		# temporary for get/put bits operation
.equ	p_temp1,	0x0160		# temporary for get/put bits operation

.equ	p_temp2,	0x0170		# temporary for get/put bits operation
.equ	p_temp3,	0x0180		# temporary for get/put bits operation

.equ	p_temp4,	0x0190		# temporary for get/put bits operation
.equ	p_temp5,	0x01A0		# temporary for get/put bits operation

.equ	p_temp6,	0x01B0		# temporary for get/put bits operation
.equ	p_temp7,	0x01C0		# temporary for get/put bits operation

.equ	p_original,	0x01D0		# original x
.equ	p_mask,		0x01E0		# original x
.equ	p_signs,	0x01F0		# original x
.equ	p_signc,	0x0200		# original x
.equ	p_region,	0x0210

.equ	p_original1,	0x0220		# original x
.equ	p_mask1,	0x0230		# original x
.equ	p_signs1,	0x0240		# original x
.equ	p_signc1,	0x0250		# original x
.equ	p_region1,	0x0260

.equ	save_r12,	0x0270		# temporary for get/put bits operation
.equ	save_r13,	0x0280		# temporary for get/put bits operation

.equ	save_r14,	0x0290		# temporary for get/put bits operation
.equ	save_r15,	0x02A0		# temporary for get/put bits operation

.equ	save_xa,	0x02B0		# qword ; leave space for 4 args*****
.equ	save_ysa,	0x02C0		# qword ; leave space for 4 args*****
.equ	save_yca,	0x02D0		# qword ; leave space for 4 args*****

.equ	save_nv,	0x02E0		# qword
.equ	p_iter,		0x02F0		# qword	storage for number of loop iterations


.globl vrda_sincos
    .type   vrda_sincos,@function
vrda_sincos:

	sub		$0x0308,%rsp

	mov		%r12,save_r12(%rsp)		# save r12
	mov		%r13,save_r13(%rsp)		# save r13
	mov		%rbx,save_rbx(%rsp)		# save rbx

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#START PROCESS INPUT
# save the arguments
	mov		%rsi,save_xa(%rsp)	# save x_array pointer
	mov		%rdx,save_ysa(%rsp)	# save ysin_array pointer
	mov		%rcx,save_yca(%rsp)	# save ycos_array pointer
#ifdef INTEGER64
        mov             %rdi,%rax
#else
        mov             %edi,%eax
        mov             %rax,%rdi
#endif

	mov		%rdi,save_nv(%rsp)	# save number of values
						# see if too few values to call the main loop
	shr		$2,%rax			# get number of iterations
	jz		.L__vrda_cleanup	# jump if only single calls
						# prepare the iteration counts
	mov		%rax,p_iter(%rsp)	# save number of iterations
	shl		$2,%rax
	sub		%rax,%rdi		# compute number of extra single calls
	mov		%rdi,save_nv(%rsp)	# save number of left over values

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#START LOOP
.align 16
.L__vrda_top:
# build the input _m128d
	movapd	.L__real_7fffffffffffffff(%rip),%xmm2
	mov	.L__real_7fffffffffffffff(%rip),%rdx

	mov	save_xa(%rsp),%rsi	# get x_array pointer
	movlpd	(%rsi),%xmm0
	movhpd	8(%rsi),%xmm0
	mov	(%rsi),%rax
	mov	8(%rsi),%rcx
	movdqa	%xmm0,%xmm6
	movdqa  %xmm0,p_original(%rsp)

	prefetch	64(%rsi)
	add		$32,%rsi
	mov		%rsi,save_xa(%rsp)	# save x_array pointer

	movlpd	-16(%rsi), %xmm1
	movhpd	-8(%rsi), %xmm1
	mov	-16(%rsi), %r8
	mov	-8(%rsi),  %r9
	movdqa	 %xmm1,%xmm7
	movdqa   %xmm1,p_original1(%rsp)

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#STARTMAIN

andpd 	%xmm2,%xmm0	#Unsign
andpd 	%xmm2,%xmm1	#Unsign

and	%rdx,%rax
and	%rdx,%rcx
and	%rdx,%r8
and	%rdx,%r9

movdqa	%xmm0,%xmm12
movdqa	%xmm1,%xmm13

pcmpgtd		%xmm6,%xmm12
pcmpgtd		%xmm7,%xmm13
movdqa		%xmm12,%xmm6
movdqa		%xmm13,%xmm7
psrldq		$4,%xmm12
psrldq		$4,%xmm13
psrldq		$8,%xmm6
psrldq		$8,%xmm7

mov 	$0x3FE921FB54442D18,%rdx			#piby4	+
mov	$0x411E848000000000,%r10			#5e5	+
movapd	.L__real_3fe0000000000000(%rip),%xmm4		#0.5 for later use	+

por	%xmm6,%xmm12
por	%xmm7,%xmm13
movd	%xmm12,%r12				#Move Sign to gpr **
movd	%xmm13,%r13				#Move Sign to gpr **

movapd	%xmm0,%xmm2				#x0
movapd	%xmm1,%xmm3				#x1
movapd	%xmm0,%xmm6				#x0
movapd	%xmm1,%xmm7				#x1

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm2 = x, xmm4 =0.5, xmm6 =x
# xmm3 = x, xmm5 =0.5, xmm7 =x
.align 16
.Leither_or_both_arg_gt_than_piby4:
	cmp	%r10,%rax
	jae	.Lfirst_or_next3_arg_gt_5e5

	cmp	%r10,%rcx
	jae	.Lsecond_or_next2_arg_gt_5e5

	cmp	%r10,%r8
	jae	.Lthird_or_fourth_arg_gt_5e5

	cmp	%r10,%r9
	jae	.Lfourth_arg_gt_5e5


#      /* Find out what multiple of piby2 */
#        npi2  = (int)(x * twobypi + 0.5);
	movapd	.L__real_3fe45f306dc9c883(%rip),%xmm0
	mulpd	%xmm0,%xmm2						# * twobypi
	mulpd	%xmm0,%xmm3						# * twobypi

	addpd	%xmm4,%xmm2						# +0.5, npi2
	addpd	%xmm4,%xmm3						# +0.5, npi2

	movapd	.L__real_3ff921fb54400000(%rip),%xmm0		# piby2_1
	movapd	.L__real_3ff921fb54400000(%rip),%xmm1		# piby2_1

	cvttpd2dq	%xmm2,%xmm4					# convert packed double to packed integers

	xorpd	%xmm12,%xmm12

	cvttpd2dq	%xmm3,%xmm5					# convert packed double to packed integers

	movapd	.L__real_3dd0b4611a600000(%rip),%xmm8		# piby2_2
	movapd	.L__real_3dd0b4611a600000(%rip),%xmm9		# piby2_2

	cvtdq2pd	%xmm4,%xmm2					# and back to double.
	cvtdq2pd	%xmm5,%xmm3					# and back to double.


#      /* Subtract the multiple from x to get an extra-precision remainder */

	movd	%xmm4,%r8						# Region
	movd	%xmm5,%r9						# Region

	mov 	.L__reald_one_zero(%rip),%rdx			# compare value for cossin path
	mov	%r8,%r10						# For Sign of Sin
	mov	%r9,%r11

#      rhead  = x - npi2 * piby2_1;
       mulpd	%xmm2,%xmm0						# npi2 * piby2_1;
       mulpd	%xmm3,%xmm1						# npi2 * piby2_1;

#      rtail  = npi2 * piby2_2;
       mulpd	%xmm2,%xmm8						# rtail
       mulpd	%xmm3,%xmm9						# rtail

#      rhead  = x - npi2 * piby2_1;
       subpd	%xmm0,%xmm6						# rhead  = x - npi2 * piby2_1;
       subpd	%xmm1,%xmm7						# rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd	%xmm6,%xmm0						# t
       movapd	%xmm7,%xmm1						# t

#      rhead  = t - rtail;
       subpd	%xmm8,%xmm0						# rhead
       subpd	%xmm9,%xmm1						# rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	.L__real_3ba3198a2e037073(%rip),%xmm2		# npi2 * piby2_2tail
       mulpd	.L__real_3ba3198a2e037073(%rip),%xmm3		# npi2 * piby2_2tail

       subpd	%xmm0,%xmm6						# t-rhead
       subpd	%xmm1,%xmm7						# t-rhead

       subpd	%xmm6,%xmm8						# - ((t - rhead) - rtail)
       subpd	%xmm7,%xmm9						# - ((t - rhead) - rtail)

       addpd	%xmm2,%xmm8						# rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);
       addpd	%xmm3,%xmm9						# rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm4  = npi2 (int), xmm0 =rhead, xmm8 =rtail
# xmm5  = npi2 (int), xmm1 =rhead, xmm9 =rtail

	pand	.L__reald_one_one(%rip),%xmm4			#odd/even region for cos/sin
	pand	.L__reald_one_one(%rip),%xmm5			#odd/even region for cos/sin

	pcmpeqd		%xmm12,%xmm4
	pcmpeqd		%xmm12,%xmm5

	punpckldq	%xmm4,%xmm4
	punpckldq	%xmm5,%xmm5

	movapd		 %xmm4,p_region(%rsp)
	movapd		 %xmm5,p_region1(%rsp)

	shr	$1,%r10						#~AB+A~B, A is sign and B is upper bit of region
	shr	$1,%r11						#~AB+A~B, A is sign and B is upper bit of region

	mov	%r10,%rax
	mov	%r11,%rcx

	not 	%r12						#ADDED TO CHANGE THE LOGIC
	not 	%r13						#ADDED TO CHANGE THE LOGIC
	and	%r12,%r10
	and	%r13,%r11

	not	%rax
	not	%rcx
	not	%r12
	not	%r13
	and	%r12,%rax
	and	%r13,%rcx

	or	%rax,%r10
	or	%rcx,%r11
	and	.L__reald_one_one(%rip),%r10				#(~AB+A~B)&1
	and	.L__reald_one_one(%rip),%r11				#(~AB+A~B)&1

	mov	%r10,%r12
	mov	%r11,%r13

	and	%rdx,%r12				#mask out the lower sign bit leaving the upper sign bit
	and	%rdx,%r13				#mask out the lower sign bit leaving the upper sign bit

	shl	$63,%r10				#shift lower sign bit left by 63 bits
	shl	$63,%r11				#shift lower sign bit left by 63 bits
	shl	$31,%r12				#shift upper sign bit left by 31 bits
	shl	$31,%r13				#shift upper sign bit left by 31 bits

	mov 	 %r10,p_signs(%rsp)		#write out lower sign bit
	mov 	 %r12,p_signs+8(%rsp)		#write out upper sign bit
	mov 	 %r11,p_signs1(%rsp)		#write out lower sign bit
	mov 	 %r13,p_signs1+8(%rsp)		#write out upper sign bit

# GET_BITS_DP64(rhead-rtail, uy);			   		; originally only rhead
# xmm4  = Sign, xmm0 =rhead, xmm8 =rtail
# xmm5  = Sign, xmm1 =rhead, xmm9 =rtail
	movapd	%xmm0,%xmm6						# rhead
	movapd	%xmm1,%xmm7						# rhead

	subpd	%xmm8,%xmm0						# r = rhead - rtail
	subpd	%xmm9,%xmm1						# r = rhead - rtail

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm4  = Sign, xmm0 = r, xmm6 =rhead, xmm8 =rtail
# xmm5  = Sign, xmm1 = r, xmm7 =rhead, xmm9 =rtail

	subpd	%xmm0,%xmm6				#rr=rhead-r
	subpd	%xmm1,%xmm7				#rr=rhead-r

	movapd	%xmm0,%xmm2				#move r for r2
	movapd	%xmm1,%xmm3				#move r for r2

	mulpd	%xmm0,%xmm2				#r2
	mulpd	%xmm1,%xmm3				#r2

	subpd	%xmm8,%xmm6				#rr=(rhead-r) -rtail
	subpd	%xmm9,%xmm7				#rr=(rhead-r) -rtail


	add	.L__reald_one_one(%rip),%r8
	add	.L__reald_one_one(%rip),%r9

	and	.L__reald_two_two(%rip),%r8
	and	.L__reald_two_two(%rip),%r9

	shr	$1,%r8
	shr	$1,%r9

	mov	%r8,%r12
	mov	%r9,%r13

	and	.L__reald_one_zero(%rip),%r12	#mask out the lower sign bit leaving the upper sign bit
	and	.L__reald_one_zero(%rip),%r13	#mask out the lower sign bit leaving the upper sign bit

	shl	$63,%r8				#shift lower sign bit left by 63 bits
	shl	$63,%r9				#shift lower sign bit left by 63 bits

	shl	$31,%r12				#shift upper sign bit left by 31 bits
	shl	$31,%r13				#shift upper sign bit left by 31 bits

	mov 	 %r8,p_signc(%rsp)		#write out lower sign bit
	mov 	 %r12,p_signc+8(%rsp)		#write out upper sign bit
	mov 	 %r9,p_signc1(%rsp)		#write out lower sign bit
	mov 	 %r13,p_signc1+8(%rsp)		#write out upper sign bit

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm0 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsinsin_sinsin_piby4:

	movapd	 %xmm0,p_temp(%rsp)			# copy of x
	movapd	 %xmm1,p_temp1(%rsp)			# copy of x

	movapd	%xmm2,%xmm10					# x2
	movapd	%xmm3,%xmm11					# x2

	movdqa	.Lsinarray+0x50(%rip),%xmm4			# s6
	movdqa	.Lsinarray+0x50(%rip),%xmm5			# s6
	movapd	.Lsinarray+0x20(%rip),%xmm8			# s3
	movapd	.Lsinarray+0x20(%rip),%xmm9			# s3

	movdqa	.Lcosarray+0x50(%rip),%xmm12			# c6
	movdqa	.Lcosarray+0x50(%rip),%xmm13			# c6
	movapd	.Lcosarray+0x20(%rip),%xmm14			# c3
	movapd	.Lcosarray+0x20(%rip),%xmm15			# c3

	movapd	 %xmm2,p_temp2(%rsp)			# copy of x2
	movapd	 %xmm3,p_temp3(%rsp)			# copy of x2

	mulpd	%xmm2,%xmm4					# s6*x2
	mulpd	%xmm3,%xmm5					# s6*x2
	mulpd	%xmm2,%xmm8					# s3*x2
	mulpd	%xmm3,%xmm9					# s3*x2

	mulpd	%xmm2,%xmm12					# s6*x2
	mulpd	%xmm3,%xmm13					# s6*x2
	mulpd	%xmm2,%xmm14					# s3*x2
	mulpd	%xmm3,%xmm15					# s3*x2

	mulpd	%xmm2,%xmm10					# x4
	mulpd	%xmm3,%xmm11					# x4

	addpd	.Lsinarray+0x40(%rip),%xmm4			# s5+x2s6
	addpd	.Lsinarray+0x40(%rip),%xmm5			# s5+x2s6
	addpd	.Lsinarray+0x10(%rip),%xmm8			# s2+x2C3
	addpd	.Lsinarray+0x10(%rip),%xmm9			# s2+x2C3

	addpd	.Lcosarray+0x40(%rip),%xmm12			# c5+x2c6
	addpd	.Lcosarray+0x40(%rip),%xmm13			# c5+x2c6
	addpd	.Lcosarray+0x10(%rip),%xmm14			# c2+x2C3
	addpd	.Lcosarray+0x10(%rip),%xmm15			# c2+x2C3

	mulpd	%xmm2,%xmm10					# x6
	mulpd	%xmm3,%xmm11					# x6

	mulpd	%xmm2,%xmm4					# x2(s5+x2s6)
	mulpd	%xmm3,%xmm5					# x2(s5+x2s6)
	mulpd	%xmm2,%xmm8					# x2(s2+x2C3)
	mulpd	%xmm3,%xmm9					# x2(s2+x2C3)

	mulpd	%xmm2,%xmm12					# x2(s5+x2s6)
	mulpd	%xmm3,%xmm13					# x2(s5+x2s6)
	mulpd	%xmm2,%xmm14					# x2(s2+x2C3)
	mulpd	%xmm3,%xmm15					# x2(s2+x2C3)

	mulpd	.L__real_3fe0000000000000(%rip),%xmm2		# 0.5 *x2
	mulpd	.L__real_3fe0000000000000(%rip),%xmm3		# 0.5 *x2

	addpd	.Lsinarray+0x30(%rip),%xmm4			# s4 + x2(s5+x2s6)
	addpd	.Lsinarray+0x30(%rip),%xmm5			# s4 + x2(s5+x2s6)
	addpd	.Lsinarray(%rip),%xmm8			# s1 + x2(s2+x2C3)
	addpd	.Lsinarray(%rip),%xmm9			# s1 + x2(s2+x2C3)

	movapd	 %xmm2,p_temp4(%rsp)			# copy of r
	movapd	 %xmm3,p_temp5(%rsp)			# copy of r

	movapd	%xmm2,%xmm0					# r
	movapd	%xmm3,%xmm1					# r

	addpd	.Lcosarray+0x30(%rip),%xmm12			# c4 + x2(c5+x2c6)
	addpd	.Lcosarray+0x30(%rip),%xmm13			# c4 + x2(c5+x2c6)
	addpd	.Lcosarray(%rip),%xmm14			# c1 + x2(c2+x2C3)
	addpd	.Lcosarray(%rip),%xmm15			# c1 + x2(c2+x2C3)

	mulpd	%xmm6,%xmm2					# 0.5 * x2 *xx
	mulpd	%xmm7,%xmm3					# 0.5 * x2 *xx

	subpd	.L__real_3ff0000000000000(%rip),%xmm0		# -t=r-1.0
	subpd	.L__real_3ff0000000000000(%rip),%xmm1		# -t=r-1.0

	mulpd	%xmm10,%xmm4					# x6(s4 + x2(s5+x2s6))
	mulpd	%xmm11,%xmm5					# x6(s4 + x2(s5+x2s6))

	mulpd	%xmm10,%xmm12					# x6(c4 + x2(c5+x2c6))
	mulpd	%xmm11,%xmm13					# x6(c4 + x2(c5+x2c6))

	addpd   .L__real_3ff0000000000000(%rip),%xmm0		# 1+(-t)
	addpd   .L__real_3ff0000000000000(%rip),%xmm1		# 1+(-t)

	addpd	%xmm8,%xmm4					# zs
	addpd	%xmm9,%xmm5					# zs

	addpd	%xmm14,%xmm12					# zc
	addpd	%xmm15,%xmm13					# zc

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm0 = r, xmm2 = 0.5 * x2 *xx, xmm4 = zs, xmm12 = zc, xmm6 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = 0.5 * x2 *xx, xmm5 = zs, xmm13 = zc, xmm7 =rr

# Free
# %xmm8,,%xmm10 xmm14
# %xmm9,,%xmm11 xmm15
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	movapd	p_temp2(%rsp),%xmm10			# x2 for x3
	movapd	p_temp3(%rsp),%xmm11			# x2 for x3

	movapd	%xmm10,%xmm8					# x2 for x4
	movapd	%xmm11,%xmm9					# x2 for x4

	movapd	p_temp(%rsp),%xmm14			# x for x*xx
	movapd	p_temp1(%rsp),%xmm15			# x for x*xx

	subpd   p_temp4(%rsp),%xmm0			# (1 + (-t)) - r
	subpd   p_temp5(%rsp),%xmm1			# (1 + (-t)) - r

	mulpd	%xmm14,%xmm10					# x3
	mulpd	%xmm15,%xmm11					# x3

	mulpd	%xmm8,%xmm8					# x4
	mulpd	%xmm9,%xmm9					# x4

	mulpd	%xmm6,%xmm14					# x*xx
	mulpd	%xmm7,%xmm15					# x*xx

	mulpd	%xmm10,%xmm4					# x3 * zs
	mulpd	%xmm11,%xmm5					# x3 * zs

	mulpd	%xmm8,%xmm12					# x4 * zc
	mulpd	%xmm9,%xmm13					# x4 * zc

	subpd	%xmm2,%xmm4					# x3*zs-0.5 * x2 *xx
	subpd	%xmm3,%xmm5					# x3*zs-0.5 * x2 *xx

	subpd   %xmm14,%xmm0					# ((1 + (-t)) - r) -x*xx
	subpd   %xmm15,%xmm1					# ((1 + (-t)) - r) -x*xx


	movapd	p_temp4(%rsp),%xmm10			# r for t
	movapd	p_temp5(%rsp),%xmm11			# r for t

	addpd	%xmm6,%xmm4					# sin+xx
	addpd	%xmm7,%xmm5					# sin+xx

	addpd   %xmm0,%xmm12					# x4*zc + (((1 + (-t)) - r) - x*xx)
	addpd   %xmm1,%xmm13					# x4*zc + (((1 + (-t)) - r) - x*xx)

	subpd	.L__real_3ff0000000000000(%rip),%xmm10	# -t=r-1.0
	subpd	.L__real_3ff0000000000000(%rip),%xmm11	# -t=r-1.0

	movapd	p_region(%rsp),%xmm2
	movapd	p_region1(%rsp),%xmm3

	movapd	%xmm2,%xmm8
	movapd	%xmm3,%xmm9

	addpd	p_temp(%rsp),%xmm4			# sin+xx+x
	addpd	p_temp1(%rsp),%xmm5			# sin+xx+x

	subpd	%xmm10,%xmm12					# cos + (-t)
	subpd	%xmm11,%xmm13					# cos + (-t)

# xmm4	= sin, xmm5  = sin
# xmm12	= cos, xmm13 = cos

	andnpd	%xmm4,%xmm8
	andnpd	%xmm5,%xmm9

	andpd	%xmm2,%xmm4
	andpd	%xmm3,%xmm5

	andnpd	%xmm12,%xmm2
	andnpd	%xmm13,%xmm3

	andpd	p_region(%rsp),%xmm12
	andpd	p_region1(%rsp),%xmm13

	orpd	%xmm2,%xmm4
	orpd	%xmm3,%xmm5

	orpd	%xmm8,%xmm12
	orpd	%xmm9,%xmm13

	jmp 	.L__vrd4_sin_cleanup

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lfirst_or_next3_arg_gt_5e5:
# %rcx,,%rax r8, r9

	cmp	%r10,%rcx				#is upper arg >= 5e5
	jae	.Lboth_arg_gt_5e5

.Llower_arg_gt_5e5:
# Upper Arg is < 5e5, Lower arg is >= 5e5
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5
# Be sure not to use %xmm3,%xmm1 and xmm7
# Use %xmm8,,%xmm5 xmm10, xmm12
#	    %xmm11,,%xmm9 xmm13


	movlpd	 %xmm0,r(%rsp)		#Save lower fp arg for remainder_piby2 call
	movhlps	%xmm0,%xmm0			#Needed since we want to work on upper arg
	movhlps	%xmm2,%xmm2
	movhlps	%xmm6,%xmm6

# Work on Upper arg
# Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
	mulsd	.L__real_3fe45f306dc9c883(%rip),%xmm2		# x*twobypi
	addsd	%xmm4,%xmm2					# xmm2 = npi2=(x*twobypi+0.5)
	movsd	.L__real_3ff921fb54400000(%rip),%xmm8		# xmm8 = piby2_1
	cvttsd2si	%xmm2,%ecx				# ecx = npi2 trunc to ints
	movsd	.L__real_3dd0b4611a600000(%rip),%xmm10		# xmm10 = piby2_2
	cvtsi2sd	%ecx,%xmm2				# xmm2 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
	mulsd	%xmm2,%xmm8					# npi2 * piby2_1
	subsd	%xmm8,%xmm6					# xmm6 = rhead =(x-npi2*piby2_1)
	movsd	.L__real_3ba3198a2e037073(%rip),%xmm12		# xmm12 =piby2_2tail

#t  = rhead;
       movsd	%xmm6,%xmm5					# xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd	%xmm2,%xmm10					# xmm1 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd	%xmm10,%xmm6					# xmm6 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	%xmm2,%xmm12     					# npi2 * piby2_2tail
       subsd	%xmm6,%xmm5					# t-rhead
       subsd	%xmm5,%xmm10					# (rtail-(t-rhead))
       addsd	%xmm12,%xmm10					# rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov	 %ecx,region+4(%rsp)			# store upper region
       movsd	 %xmm6,%xmm0
       subsd	 %xmm10,%xmm0					# xmm0 = r=(rhead-rtail)
       subsd	 %xmm0,%xmm6					# rr=rhead-r
       subsd	 %xmm10,%xmm6					# xmm6 = rr=((rhead-r) -rtail)
       movlpd	 %xmm0,r+8(%rsp)			# store upper r
       movlpd	 %xmm6,rr+8(%rsp)			# store upper rr

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#Note that volatiles will be trashed by the call
#We will construct r, rr, region and sign

# Work on Lower arg
	mov		$0x07ff0000000000000,%r11			# is lower arg nan/inf
	mov		%r11,%r10
	and		%rax,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_lower_naninf

	mov	  %r8,p_temp(%rsp)
	mov	  %r9,p_temp2(%rsp)
	movapd	  %xmm1,p_temp1(%rsp)
	movapd	  %xmm3,p_temp3(%rsp)
	movapd	  %xmm7,p_temp5(%rsp)

	lea	 region(%rsp),%rdx			# lower arg is **NOT** nan/inf
	lea	 rr(%rsp),%rsi
	lea	 r(%rsp),%rdi
	movlpd	 r(%rsp),%xmm0	#Restore lower fp arg for remainder_piby2 call
        call    __amd_remainder_piby2@PLT

	mov	 p_temp(%rsp),%r8
	mov	 p_temp2(%rsp),%r9
	movapd	 p_temp1(%rsp),%xmm1
	movapd	 p_temp3(%rsp),%xmm3
	movapd	 p_temp5(%rsp),%xmm7
	jmp 	0f

.L__vrd4_sin_lower_naninf:
	mov	p_original(%rsp),%rax			# upper arg is nan/inf
	mov	$0x00008000000000000,%r11
	or	%r11,%rax
	mov	 %rax,r(%rsp)				# r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr(%rsp)				# rr = 0
	mov	 %r10d,region(%rsp)			# region =0
	and 	.L__real_naninf_lower_sign_mask(%rip),%r12	# Sign
.align 16
0:
	jmp 	.Lcheck_next2_args


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lboth_arg_gt_5e5:
#Upper Arg is >= 5e5, Lower arg is >= 5e5
# %rcx,,%rax r8, r9
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5

	movhpd 	%xmm0,r+8(%rsp)				#Save upper fp arg for remainder_piby2 call

	mov		$0x07ff0000000000000,%r11			#is lower arg nan/inf
	mov		%r11,%r10
	and		%rax,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_lower_naninf_of_both_gt_5e5

	mov	  %rcx,p_temp(%rsp)			#Save upper arg
	mov	  %r8,p_temp2(%rsp)
	mov	  %r9,p_temp4(%rsp)
	movapd	  %xmm1,p_temp1(%rsp)
	movapd	  %xmm3,p_temp3(%rsp)
	movapd	  %xmm7,p_temp5(%rsp)

	lea	 region(%rsp),%rdx			#lower arg is **NOT** nan/inf
	lea	 rr(%rsp),%rsi
	lea	 r(%rsp),%rdi
        call    __amd_remainder_piby2@PLT

	mov	 p_temp(%rsp),%rcx			#Restore upper arg
	mov	 p_temp2(%rsp),%r8
	mov	 p_temp4(%rsp),%r9
	movapd	 p_temp1(%rsp),%xmm1
	movapd	 p_temp3(%rsp),%xmm3
	movapd	 p_temp5(%rsp),%xmm7

	jmp 	0f

.L__vrd4_sin_lower_naninf_of_both_gt_5e5:				#lower arg is nan/inf
	mov	p_original(%rsp),%rax
	mov	$0x00008000000000000,%r11
	or	%r11,%rax
	mov	 %rax,r(%rsp)				#r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr(%rsp)				#rr = 0
	mov	 %r10d,region(%rsp)			#region = 0
	and 	.L__real_naninf_lower_sign_mask(%rip),%r12	# Sign

.align 16
0:
	mov		$0x07ff0000000000000,%r11			#is upper arg nan/inf
	mov		%r11,%r10
	and		%rcx,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_upper_naninf_of_both_gt_5e5


	mov	  %r8,p_temp(%rsp)
	mov	  %r9,p_temp2(%rsp)
	movapd	  %xmm1,p_temp1(%rsp)
	movapd	  %xmm3,p_temp3(%rsp)
	movapd	  %xmm7,p_temp5(%rsp)

	lea	 region+4(%rsp),%rdx			#upper arg is **NOT** nan/inf
	lea	 rr+8(%rsp),%rsi
	lea	 r+8(%rsp),%rdi
	movlpd	 r+8(%rsp),%xmm0			#Restore upper fp arg for remainder_piby2 call
        call    __amd_remainder_piby2@PLT

	mov	 p_temp(%rsp),%r8
	mov	 p_temp2(%rsp),%r9
	movapd	 p_temp1(%rsp),%xmm1
	movapd	 p_temp3(%rsp),%xmm3
	movapd	 p_temp5(%rsp),%xmm7

	jmp 	0f

.L__vrd4_sin_upper_naninf_of_both_gt_5e5:
	mov	p_original+8(%rsp),%rcx				#upper arg is nan/inf
	mov	$0x00008000000000000,%r11
	or	%r11,%rcx
	mov	%rcx,r+8(%rsp)					#r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	%r10,rr+8(%rsp)					#rr = 0
	mov	%r10d,region+4(%rsp)				#region = 0
	and 	.L__real_naninf_upper_sign_mask(%rip),%r12	# Sign
.align 16
0:
	jmp 	.Lcheck_next2_args

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lsecond_or_next2_arg_gt_5e5:
# Upper Arg is >= 5e5, Lower arg is < 5e5
# %rcx,,%rax r8, r9
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5
# Do not use %xmm3,,%xmm1 xmm7
# Restore xmm4 and %xmm3,,%xmm1 xmm7
# Can use %xmm10,,%xmm8 xmm12
#   %xmm9,,%xmm5 xmm11, xmm13

	movhpd	 %xmm0,r+8(%rsp)		#Save upper fp arg for remainder_piby2 call
#	movlhps	%xmm0,%xmm0			#Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
#	movlhps	%xmm2,%xmm2
#	movlhps	%xmm6,%xmm6

# Work on Lower arg
# Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

	mulsd	.L__real_3fe45f306dc9c883(%rip),%xmm2		# x*twobypi
	addsd	%xmm4,%xmm2					# xmm2 = npi2=(x*twobypi+0.5)
	movsd	.L__real_3ff921fb54400000(%rip),%xmm8		# xmm3 = piby2_1
	cvttsd2si	%xmm2,%eax				# ecx = npi2 trunc to ints
	movsd	.L__real_3dd0b4611a600000(%rip),%xmm10		# xmm1 = piby2_2
	cvtsi2sd	%eax,%xmm2				# xmm2 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
	mulsd	%xmm2,%xmm8					# npi2 * piby2_1
	subsd	%xmm8,%xmm6					# xmm6 = rhead =(x-npi2*piby2_1)
	movsd	.L__real_3ba3198a2e037073(%rip),%xmm12		# xmm7 =piby2_2tail

#t  = rhead;
       movsd	%xmm6,%xmm5					# xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd	%xmm2,%xmm10					# xmm1 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd	%xmm10,%xmm6					# xmm6 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	%xmm2,%xmm12     					# npi2 * piby2_2tail
       subsd	%xmm6,%xmm5					# t-rhead
       subsd	%xmm5,%xmm10					# (rtail-(t-rhead))
       addsd	%xmm12,%xmm10					# rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov	 %eax,region(%rsp)			# store upper region
       movsd	%xmm6,%xmm0
       subsd	%xmm10,%xmm0					# xmm0 = r=(rhead-rtail)
       subsd	%xmm0,%xmm6					# rr=rhead-r
       subsd	%xmm10,%xmm6					# xmm6 = rr=((rhead-r) -rtail)
       movlpd	 %xmm0,r(%rsp)				# store upper r
       movlpd	 %xmm6,rr(%rsp)				# store upper rr

#Work on Upper arg
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
	mov		$0x07ff0000000000000,%r11			# is upper arg nan/inf
	mov		%r11,%r10
	and		%rcx,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_upper_naninf


	mov	  %r8,p_temp(%rsp)
	mov	  %r9,p_temp2(%rsp)
	movapd	  %xmm1,p_temp1(%rsp)
	movapd	  %xmm3,p_temp3(%rsp)
	movapd	  %xmm7,p_temp5(%rsp)

	lea	 region+4(%rsp),%rdx			# upper arg is **NOT** nan/inf
	lea	 rr+8(%rsp),%rsi
	lea	 r+8(%rsp),%rdi
	movlpd	 r+8(%rsp),%xmm0	#Restore upper fp arg for remainder_piby2 call
        call    __amd_remainder_piby2@PLT

	mov	p_temp(%rsp),%r8
	mov	p_temp2(%rsp),%r9
	movapd	p_temp1(%rsp),%xmm1
	movapd	p_temp3(%rsp),%xmm3
	movapd	p_temp5(%rsp),%xmm7
	jmp 	0f

.L__vrd4_sin_upper_naninf:
	mov	p_original+8(%rsp),%rcx		# upper arg is nan/inf
#	mov	r+8(%rsp),%rcx				; upper arg is nan/inf
	mov	$0x00008000000000000,%r11
	or	%r11,%rcx
	mov	 %rcx,r+8(%rsp)				# r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr+8(%rsp)			# rr = 0
	mov	 %r10d,region+4(%rsp)			# region =0
	and 	.L__real_naninf_upper_sign_mask(%rip),%r12	# Sign

.align 16
0:
	jmp 	.Lcheck_next2_args


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lcheck_next2_args:

	mov	$0x411E848000000000,%r10			#5e5	+

	cmp	%r10,%r8
	jae	.Lfirst_second_done_third_or_fourth_arg_gt_5e5

	cmp	%r10,%r9
	jae	.Lfirst_second_done_fourth_arg_gt_5e5

# Work on next two args, both < 5e5
# %xmm3,,%xmm1 xmm5 = x, xmm4 = 0.5
	movapd	.L__real_3fe0000000000000(%rip),%xmm4			# Restore 0.5

	mulpd	.L__real_3fe45f306dc9c883(%rip),%xmm3						# * twobypi
	addpd	%xmm4,%xmm3						# +0.5, npi2
	movapd	.L__real_3ff921fb54400000(%rip),%xmm1		# piby2_1
	cvttpd2dq	%xmm3,%xmm5					# convert packed double to packed integers
	movapd	.L__real_3dd0b4611a600000(%rip),%xmm9		# piby2_2
	cvtdq2pd	%xmm5,%xmm3					# and back to double.

#      /* Subtract the multiple from x to get an extra-precision remainder */
	movq	 %xmm5,region1(%rsp)						# Region

#      rhead  = x - npi2 * piby2_1;
       mulpd	%xmm3,%xmm1						# npi2 * piby2_1;

#      rtail  = npi2 * piby2_2;
       mulpd	%xmm3,%xmm9						# rtail

#      rhead  = x - npi2 * piby2_1;
       subpd	%xmm1,%xmm7						# rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd	%xmm7,%xmm1						# t

#      rhead  = t - rtail;
       subpd	%xmm9,%xmm1						# rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	.L__real_3ba3198a2e037073(%rip),%xmm3		# npi2 * piby2_2tail

       subpd	%xmm1,%xmm7						# t-rhead
       subpd	%xmm7,%xmm9						# - ((t - rhead) - rtail)
       addpd	%xmm3,%xmm9						# rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

       movapd	%xmm1,%xmm7						# rhead
       subpd	%xmm9,%xmm1						# r = rhead - rtail
       movapd	%xmm1,r1(%rsp)

       subpd	%xmm1,%xmm7						# rr=rhead-r
       subpd	%xmm9,%xmm7						# rr=(rhead-r) -rtail
       movapd	%xmm7,rr1(%rsp)

	jmp	.L__vrd4_sin_reconstruct


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lthird_or_fourth_arg_gt_5e5:
#first two args are < 5e5, third arg >= 5e5, fourth arg >= 5e5 or < 5e5
# %rcx,,%rax r8, r9
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5
# Do not use %xmm3,,%xmm1 xmm7
# Can use 	%xmm11,,%xmm9 xmm13
# 	%xmm8,,%xmm5 xmm10, xmm12
# Restore xmm4

# Work on first two args, both < 5e5

#DEBUG
#	movapd	%xmm2, %xmm4
#	movapd	%xmm1, %xmm5
#	movapd	%xmm2, %xmm12
#	movapd	%xmm1, %xmm13
#	jmp 	.L__vrd4_sin_cleanup
#DEBUG

	mulpd		.L__real_3fe45f306dc9c883(%rip),%xmm2						# * twobypi
	addpd		%xmm4,%xmm2					# +0.5, npi2
	movapd		.L__real_3ff921fb54400000(%rip),%xmm0		# piby2_1
	cvttpd2dq	%xmm2,%xmm4				# convert packed double to packed integers
	movapd		.L__real_3dd0b4611a600000(%rip),%xmm8		# piby2_2
	cvtdq2pd	%xmm4,%xmm2				# and back to double.

#      /* Subtract the multiple from x to get an extra-precision remainder */
	movq	 %xmm4,region(%rsp)						# Region

#DEBUG
#	movapd	region(%rsp), %xmm4
#	movapd	%xmm1, %xmm5
#	movapd	region(%rsp), %xmm12
#	movapd	%xmm1, %xmm13
#	jmp 	.L__vrd4_sin_cleanup
#DEBUG

#      rhead  = x - npi2 * piby2_1;
       mulpd	%xmm2,%xmm0						# npi2 * piby2_1;
#      rtail  = npi2 * piby2_2;
       mulpd	%xmm2,%xmm8						# rtail

#      rhead  = x - npi2 * piby2_1;
       subpd	%xmm0,%xmm6						# rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd	%xmm6,%xmm0						# t

#      rhead  = t - rtail;
       subpd	%xmm8,%xmm0						# rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	.L__real_3ba3198a2e037073(%rip),%xmm2		# npi2 * piby2_2tail

       subpd	%xmm0,%xmm6						# t-rhead
       subpd	%xmm6,%xmm8						# - ((t - rhead) - rtail)
       addpd	%xmm2,%xmm8						# rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

       movapd	%xmm0,%xmm6						# rhead
       subpd	%xmm8,%xmm0						# r = rhead - rtail
       movapd	 %xmm0,r(%rsp)

       subpd	%xmm0,%xmm6						# rr=rhead-r
       subpd	%xmm8,%xmm6						# rr=(rhead-r) -rtail
       movapd	 %xmm6,rr(%rsp)


# Work on next two args, third arg >= 5e5, fourth arg >= 5e5 or < 5e5
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lfirst_second_done_third_or_fourth_arg_gt_5e5:
# %rcx,,%rax r8, r9
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5

#DEBUG
#	movapd	region(%rsp), %xmm4
#	movapd	%xmm1, %xmm5
#	movapd	region(%rsp), %xmm12
#	movapd	%xmm1, %xmm13
#	jmp 	.L__vrd4_sin_cleanup
#DEBUG

	mov	$0x411E848000000000,%r10			#5e5	+

	cmp	%r10,%r9
	jae	.Lboth_arg_gt_5e5_higher


# Upper Arg is <5e5, Lower arg is >= 5e5
# %r9,%r8
# %xmm3,,%xmm1 xmm7 = x, xmm4 = 0.5

	movlpd	 %xmm1,r1(%rsp)				#Save lower fp arg for remainder_piby2 call
	movhlps	%xmm1,%xmm1				#Needed since we want to work on upper arg
	movhlps	%xmm3,%xmm3
	movhlps	%xmm7,%xmm7
	movapd	.L__real_3fe0000000000000(%rip),%xmm4	#0.5 for later use

# Work on Upper arg
# Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs
	mulsd	.L__real_3fe45f306dc9c883(%rip),%xmm3		# x*twobypi
	addsd	%xmm4,%xmm3					# xmm3 = npi2=(x*twobypi+0.5)
	movsd	.L__real_3ff921fb54400000(%rip),%xmm2		# xmm2 = piby2_1
	cvttsd2si	%xmm3,%r9d				# r9d = npi2 trunc to ints
	movsd	.L__real_3dd0b4611a600000(%rip),%xmm0		# xmm0 = piby2_2
	cvtsi2sd	%r9d,%xmm3				# xmm3 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
	mulsd	%xmm3,%xmm2					# npi2 * piby2_1
	subsd	%xmm2,%xmm7					# xmm7 = rhead =(x-npi2*piby2_1)
	movsd	.L__real_3ba3198a2e037073(%rip),%xmm6		# xmm6 =piby2_2tail

#t  = rhead;
       movsd	%xmm7,%xmm5					# xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd	%xmm3,%xmm0					# xmm0 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd	%xmm0,%xmm7					# xmm7 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	%xmm3,%xmm6     					# npi2 * piby2_2tail
       subsd	%xmm7,%xmm5					# t-rhead
       subsd	%xmm5,%xmm0					# (rtail-(t-rhead))
       addsd	%xmm6,%xmm0					# rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov	 %r9d,region1+4(%rsp)			# store upper region
       movsd	 %xmm7,%xmm1
       subsd	 %xmm0,%xmm1					# xmm1 = r=(rhead-rtail)
       subsd	 %xmm1,%xmm7					# rr=rhead-r
       subsd	 %xmm0,%xmm7					# xmm7 = rr=((rhead-r) -rtail)
       movlpd	 %xmm1,r1+8(%rsp)			# store upper r
       movlpd	 %xmm7,rr1+8(%rsp)			# store upper rr

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign

# Work on Lower arg
	mov		$0x07ff0000000000000,%r11			# is lower arg nan/inf
	mov		%r11,%r10
	and		%r8,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_lower_naninf_higher

	lea	 region1(%rsp),%rdx			# lower arg is **NOT** nan/inf
	lea	 rr1(%rsp),%rsi
	lea	 r1(%rsp),%rdi
	movlpd	 r1(%rsp),%xmm0				#Restore lower fp arg for remainder_piby2 call
        call    __amd_remainder_piby2@PLT
	jmp 	0f

.L__vrd4_sin_lower_naninf_higher:
	mov	p_original1(%rsp),%r8			# upper arg is nan/inf
	mov	$0x00008000000000000,%r11
	or	%r11,%r8
	mov	 %r8,r1(%rsp)				# r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr1(%rsp)				# rr = 0
	mov	 %r10d,region1(%rsp)			# region =0
	and 	.L__real_naninf_lower_sign_mask(%rip),%r13	# Sign

.align 16
0:


#DEBUG
#	movapd	r(%rsp), %xmm4
#	movapd	r1(%rsp), %xmm5
#	movapd	r(%rsp), %xmm12
#	movapd	r1(%rsp), %xmm13
#	jmp 	.L__vrd4_sin_cleanup
#DEBUG


	jmp 	.L__vrd4_sin_reconstruct

.align 16
.Lboth_arg_gt_5e5_higher:
# Upper Arg is >= 5e5, Lower arg is >= 5e5
# %r9,%r8
# %xmm3,,%xmm1 xmm7 = x, xmm4 = 0.5

	movhpd 	%xmm1,r1+8(%rsp)		#Save upper fp arg for remainder_piby2 call

	mov		$0x07ff0000000000000,%r11			#is lower arg nan/inf
	mov		%r11,%r10
	and		%r8,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_lower_naninf_of_both_gt_5e5_higher

	mov	 %r9,p_temp1(%rsp)			#Save upper arg
	lea	 region1(%rsp),%rdx			#lower arg is **NOT** nan/inf
	lea	 rr1(%rsp),%rsi
	lea	 r1(%rsp),%rdi
	movsd	 %xmm1,%xmm0
        call    __amd_remainder_piby2@PLT
	mov	 p_temp1(%rsp),%r9			#Restore upper arg

	jmp 	0f

.L__vrd4_sin_lower_naninf_of_both_gt_5e5_higher:				#lower arg is nan/inf
	mov	p_original1(%rsp),%r8
	mov	$0x00008000000000000,%r11
	or	%r11,%r8
	mov	 %r8,r1(%rsp)				#r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr1(%rsp)				#rr = 0
	mov	 %r10d,region1(%rsp)			#region = 0
	and 	.L__real_naninf_lower_sign_mask(%rip),%r13	# Sign

.align 16
0:
	mov		$0x07ff0000000000000,%r11			#is upper arg nan/inf
	mov		%r11,%r10
	and		%r9,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_upper_naninf_of_both_gt_5e5_higher

	lea	 region1+4(%rsp),%rdx			#upper arg is **NOT** nan/inf
	lea	 rr1+8(%rsp),%rsi
	lea	 r1+8(%rsp),%rdi
	movlpd	 r1+8(%rsp),%xmm0			#Restore upper fp arg for remainder_piby2 call
        call    __amd_remainder_piby2@PLT
	jmp 	0f

.L__vrd4_sin_upper_naninf_of_both_gt_5e5_higher:
	mov	p_original1+8(%rsp),%r9		#upper arg is nan/inf
#	movd	%xmm6,%r9					;upper arg is nan/inf
	mov	$0x00008000000000000,%r11
	or	%r11,%r9
	mov	 %r9,r1+8(%rsp)				#r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr1+8(%rsp)			#rr = 0
	mov	 %r10d,region1+4(%rsp)			#region = 0
	and 	.L__real_naninf_upper_sign_mask(%rip),%r13	# Sign

.align 16
0:

	jmp 	.L__vrd4_sin_reconstruct

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lfourth_arg_gt_5e5:
#first two args are < 5e5, third arg < 5e5, fourth arg >= 5e5
#%rcx,,%rax r8, r9
#%xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5

# Work on first two args, both < 5e5

	mulpd	.L__real_3fe45f306dc9c883(%rip),%xmm2						# * twobypi
	addpd	%xmm4,%xmm2						# +0.5, npi2
	movapd	.L__real_3ff921fb54400000(%rip),%xmm0		# piby2_1
	cvttpd2dq	%xmm2,%xmm4					# convert packed double to packed integers
	movapd	.L__real_3dd0b4611a600000(%rip),%xmm8		# piby2_2
	cvtdq2pd	%xmm4,%xmm2					# and back to double.

#      /* Subtract the multiple from x to get an extra-precision remainder */
	movq	 %xmm4,region(%rsp)						# Region

#      rhead  = x - npi2 * piby2_1;
       mulpd	%xmm2,%xmm0						# npi2 * piby2_1;
#      rtail  = npi2 * piby2_2;
       mulpd	%xmm2,%xmm8						# rtail

#      rhead  = x - npi2 * piby2_1;
       subpd	%xmm0,%xmm6						# rhead  = x - npi2 * piby2_1;

#      t  = rhead;
       movapd	%xmm6,%xmm0						# t

#      rhead  = t - rtail;
       subpd	%xmm8,%xmm0						# rhead

#      rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulpd	.L__real_3ba3198a2e037073(%rip),%xmm2		# npi2 * piby2_2tail

       subpd	%xmm0,%xmm6						# t-rhead
       subpd	%xmm6,%xmm8						# - ((t - rhead) - rtail)
       addpd	%xmm2,%xmm8						# rtail = npi2 * piby2_2tail - ((t - rhead) - rtail);

       movapd	%xmm0,%xmm6						# rhead
       subpd	%xmm8,%xmm0						# r = rhead - rtail
       movapd	 %xmm0,r(%rsp)

       subpd	%xmm0,%xmm6						# rr=rhead-r
       subpd	%xmm8,%xmm6						# rr=(rhead-r) -rtail
       movapd	 %xmm6,rr(%rsp)


# Work on next two args, third arg < 5e5, fourth arg >= 5e5
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lfirst_second_done_fourth_arg_gt_5e5:

# Upper Arg is >= 5e5, Lower arg is < 5e5
# %r9,%r8
# %xmm3,,%xmm1 xmm7 = x, xmm4 = 0.5

	movhpd	 %xmm1,r1+8(%rsp)		#Save upper fp arg for remainder_piby2 call
#	movlhps	%xmm1,%xmm1			#Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
#	movlhps	%xmm3,%xmm3
#	movlhps	%xmm7,%xmm7
	movapd	.L__real_3fe0000000000000(%rip),%xmm4	#0.5 for later use

# Work on Lower arg
# Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

	mulsd	.L__real_3fe45f306dc9c883(%rip),%xmm3		# x*twobypi
	addsd	%xmm4,%xmm3					# xmm3 = npi2=(x*twobypi+0.5)
	movsd	.L__real_3ff921fb54400000(%rip),%xmm2		# xmm2 = piby2_1
	cvttsd2si	%xmm3,%r8d				# r8d = npi2 trunc to ints
	movsd	.L__real_3dd0b4611a600000(%rip),%xmm0		# xmm0 = piby2_2
	cvtsi2sd	%r8d,%xmm3				# xmm3 = npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
	mulsd	%xmm3,%xmm2					# npi2 * piby2_1
	subsd	%xmm2,%xmm7					# xmm7 = rhead =(x-npi2*piby2_1)
	movsd	.L__real_3ba3198a2e037073(%rip),%xmm6		# xmm6 =piby2_2tail

#t  = rhead;
       movsd	%xmm7,%xmm5					# xmm5 = t = rhead

#rtail  = npi2 * piby2_2;
       mulsd	%xmm3,%xmm0					# xmm0 =rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd	%xmm0,%xmm7					# xmm7 =rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	%xmm3,%xmm6     					# npi2 * piby2_2tail
       subsd	%xmm7,%xmm5					# t-rhead
       subsd	%xmm5,%xmm0					# (rtail-(t-rhead))
       addsd	%xmm6,%xmm0					# rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov	 %r8d,region1(%rsp)			# store lower region
       movsd	%xmm7,%xmm1
       subsd	%xmm0,%xmm1					# xmm0 = r=(rhead-rtail)
       subsd	%xmm1,%xmm7					# rr=rhead-r
       subsd	%xmm0,%xmm7					# xmm6 = rr=((rhead-r) -rtail)

       movlpd	 %xmm1,r1(%rsp)				# store upper r
       movlpd	 %xmm7,rr1(%rsp)				# store upper rr

#Work on Upper arg
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
	mov		$0x07ff0000000000000,%r11			# is upper arg nan/inf
	mov		%r11,%r10
	and		%r9,%r10
	cmp		%r11,%r10
	jz		.L__vrd4_sin_upper_naninf_higher

	lea	 region1+4(%rsp),%rdx			# upper arg is **NOT** nan/inf
	lea	 rr1+8(%rsp),%rsi
	lea	 r1+8(%rsp),%rdi
	movlpd	 r1+8(%rsp),%xmm0	#Restore upper fp arg for remainder_piby2 call
        call    __amd_remainder_piby2@PLT
	jmp 	0f

.L__vrd4_sin_upper_naninf_higher:
	mov	p_original1+8(%rsp),%r9		# upper arg is nan/inf
#	mov	r1+8(%rsp),%r9				; upper arg is nan/inf
	mov	$0x00008000000000000,%r11
	or	%r11,%r9
	mov	%r9,r1+8(%rsp)				# r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	%r10,rr1+8(%rsp)			# rr = 0
	mov	%r10d,region1+4(%rsp)			# region =0
	and 	.L__real_naninf_upper_sign_mask(%rip),%r13	# Sign

.align 16
0:
	jmp	.L__vrd4_sin_reconstruct


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.L__vrd4_sin_reconstruct:
#Results
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign0  = Sign, xmm0 = r, xmm2 = %xmm6,%r2 =rr
# p_sign1  = Sign, xmm1 = r, xmm3 = %xmm7,%r2 =rr
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#DEBUG
#	movapd	region(%rsp), %xmm4
#	movapd	region1(%rsp), %xmm5
#	movapd	region(%rsp), %xmm12
#	movapd	region1(%rsp), %xmm13
#	jmp 	.L__vrd4_sin_cleanup
#DEBUG


	movapd	r(%rsp),%xmm0
	movapd	r1(%rsp),%xmm1

	movapd	rr(%rsp),%xmm6
	movapd	rr1(%rsp),%xmm7

	mov	region(%rsp),%r8
	mov	region1(%rsp),%r9

	movlpd	region(%rsp),%xmm4
	movlpd	region1(%rsp),%xmm5

	pand	.L__reald_one_one(%rip),%xmm4			#odd/even region for cos/sin
	pand	.L__reald_one_one(%rip),%xmm5			#odd/even region for cos/sin

	xorpd	%xmm12,%xmm12
	pcmpeqd		%xmm12,%xmm4
	pcmpeqd		%xmm12,%xmm5

	punpckldq	%xmm4,%xmm4
	punpckldq	%xmm5,%xmm5

	movapd		 %xmm4,p_region(%rsp)
	movapd		 %xmm5,p_region1(%rsp)

	mov 	.L__reald_one_zero(%rip),%rdx		#compare value for cossin path
	mov 	%r8,%r10
	mov 	%r9,%r11

	shr	$1,%r10						#~AB+A~B, A is sign and B is upper bit of region
	shr	$1,%r11						#~AB+A~B, A is sign and B is upper bit of region

	mov	%r10,%rax
	mov	%r11,%rcx

	not 	%r12						#ADDED TO CHANGE THE LOGIC
	not 	%r13						#ADDED TO CHANGE THE LOGIC
	and	%r12,%r10
	and	%r13,%r11

	not	%rax
	not	%rcx
	not	%r12
	not	%r13
	and	%r12,%rax
	and	%r13,%rcx

	or	%rax,%r10
	or	%rcx,%r11
	and	.L__reald_one_one(%rip),%r10				#(~AB+A~B)&1
	and	.L__reald_one_one(%rip),%r11				#(~AB+A~B)&1

	mov	%r10,%r12
	mov	%r11,%r13

	and	%rdx,%r12				#mask out the lower sign bit leaving the upper sign bit
	and	%rdx,%r13				#mask out the lower sign bit leaving the upper sign bit

	shl	$63,%r10				#shift lower sign bit left by 63 bits
	shl	$63,%r11				#shift lower sign bit left by 63 bits
	shl	$31,%r12				#shift upper sign bit left by 31 bits
	shl	$31,%r13				#shift upper sign bit left by 31 bits

	mov 	 %r10,p_signs(%rsp)		#write out lower sign bit
	mov 	 %r12,p_signs+8(%rsp)		#write out upper sign bit
	mov 	 %r11,p_signs1(%rsp)		#write out lower sign bit
	mov 	 %r13,p_signs1+8(%rsp)		#write out upper sign bit

	movapd	%xmm0,%xmm2				# r
	movapd	%xmm1,%xmm3				# r

	mulpd	%xmm0,%xmm2				# r2
	mulpd	%xmm1,%xmm3				# r2

	add	.L__reald_one_one(%rip),%r8
	add	.L__reald_one_one(%rip),%r9

	and	.L__reald_two_two(%rip),%r8
	and	.L__reald_two_two(%rip),%r9

	shr	$1,%r8
	shr	$1,%r9

	mov	%r8,%rax
	mov	%r9,%rcx

	and	.L__reald_one_zero(%rip),%rax	#mask out the lower sign bit leaving the upper sign bit
	and	.L__reald_one_zero(%rip),%rcx	#mask out the lower sign bit leaving the upper sign bit

	shl	$63,%r8				#shift lower sign bit left by 63 bits
	shl	$63,%r9				#shift lower sign bit left by 63 bits

	shl	$31,%rax				#shift upper sign bit left by 31 bits
	shl	$31,%rcx				#shift upper sign bit left by 31 bits

	mov 	 %r8,p_signc(%rsp)		#write out lower sign bit
	mov 	 %rax,p_signc+8(%rsp)		#write out upper sign bit
	mov 	 %r9,p_signc1(%rsp)		#write out lower sign bit
	mov 	 %rcx,p_signc1+8(%rsp)		#write out upper sign bit

	jmp 	.Lsinsin_sinsin_piby4

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.L__vrd4_sin_cleanup:

	xorpd	 p_signs(%rsp),%xmm4		# (+) Sign
	xorpd	 p_signs1(%rsp),%xmm5		# (+) Sign

	xorpd	 p_signc(%rsp),%xmm12		# (+) Sign
	xorpd	 p_signc1(%rsp),%xmm13		# (+) Sign

.L__vrda_bottom1:
# store the result _m128d
	mov	save_ysa(%rsp),%rdi	# get ysin_array pointer
	mov	save_yca(%rsp),%rbx	# get ycos_array pointer

	movlpd	 %xmm4,(%rdi)
	movhpd	 %xmm4,8(%rdi)

	movlpd	 %xmm12,(%rbx)
	movhpd	 %xmm12,8(%rbx)

.L__vrda_bottom2:

	prefetch	64(%rdi)
	prefetch	64(%rbx)

	add		$32,%rdi
	add		$32,%rbx

	mov		%rdi,save_ysa(%rsp)	# save ysin_array pointer
	mov		%rbx,save_yca(%rsp)	# save ycos_array pointer

# store the result _m128d
	movlpd	%xmm5, -16(%rdi)
	movhpd	%xmm5, -8(%rdi)

	movlpd	%xmm13, -16(%rbx)
	movhpd	%xmm13, -8(%rbx)

	mov	 p_iter(%rsp),%rax	# get number of iterations
	sub	 $1,%rax
	mov	 %rax,p_iter(%rsp)	# save number of iterations
	jnz	.L__vrda_top

# see if we need to do any extras
	mov	save_nv(%rsp),%rax	# get number of values
	test	%rax,%rax
	jnz	.L__vrda_cleanup

.L__final_check:

	mov	save_r12(%rsp),%r12	# restore r12
	mov	save_r13(%rsp),%r13	# restore r13
	mov	save_rbx(%rsp),%rbx	# restore rbx

	add	$0x0308,%rsp
	ret


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# we jump here when we have an odd number of cos calls to make at the end
# we assume that rdx is pointing at the next x array element, r8 at the next y array element.
# The number of values left is in save_nv

.align	16
.L__vrda_cleanup:
        mov             save_nv(%rsp),%rax      # get number of values
        test            %rax,%rax               # are there any values
        jz              .L__final_check         # exit if not

	mov		 save_xa(%rsp),%rsi

# fill in a m128d with zeroes and the extra values and then make a recursive call.
	xorpd		 %xmm0,%xmm0
	movlpd		 %xmm0,p_temp+8(%rsp)
	movapd		 %xmm0,p_temp+16(%rsp)

	mov		 (%rsi),%rcx			# we know there's at least one
	mov	 	 %rcx,p_temp(%rsp)
	cmp		 $2,%rax
	jl		.L__vrdacg

	mov		 8(%rsi),%rcx			# do the second value
	mov	 	 %rcx,p_temp+8(%rsp)
	cmp		 $3,%rax
	jl		.L__vrdacg

	mov		 16(%rsi),%rcx			# do the third value
	mov	 	 %rcx,p_temp+16(%rsp)

.L__vrdacg:
	mov		$4,%rdi				# parameter for N
	lea		p_temp(%rsp),%rsi		# &x parameter
	lea		p_temp2(%rsp),%rdx 		# &ys parameter
	lea		p_temp4(%rsp),%rcx 		# &yc parameter

        call    vrda_sincos@PLT # call recursively to compute four values

# now copy the results to the destination array
	mov		 save_ysa(%rsp),%rdi
	mov		 save_yca(%rsp),%rbx
	mov		 save_nv(%rsp),%rax		# get number of values

	mov	 	 p_temp2(%rsp),%rcx
	mov		 %rcx,(%rdi)			# we know there's at least one
	mov	 	 p_temp4(%rsp),%rdx
	mov		 %rdx,(%rbx)			# we know there's at least one
	cmp		 $2,%rax
	jl		.L__vrdacgf

	mov	 	 p_temp2+8(%rsp),%rcx
	mov		 %rcx,8(%rdi)			# do the second value
	mov	 	 p_temp4+8(%rsp),%rdx
	mov		 %rdx,8(%rbx)			# do the second value
	cmp		 $3,%rax
	jl		.L__vrdacgf

	mov	 	 p_temp2+16(%rsp),%rcx
	mov		 %rcx,16(%rdi)			# do the third value
	mov	 	 p_temp4+16(%rsp),%rdx
	mov		 %rdx,16(%rbx)			# do the third value

.L__vrdacgf:
	jmp		.L__final_check
