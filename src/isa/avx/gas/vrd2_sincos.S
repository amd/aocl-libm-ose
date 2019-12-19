#
# A vector implementation of the libm sincos function.
#
# Prototype:
#
#      vrd2_sincos(__m128d x, __m128d* ys, __m128d* yc);
#
#   Computes Sine and Cosine of x.
#   It will provide proper C99 return values,
#   but may not raise floating point status bits properly.
#   Based on the NAG C implementation.


#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(vrd2_sincos)

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
.L__real_ffffffffffffffff: 	.quad 0x0ffffffffffffffff	#Sign bit one
				.quad 0x0ffffffffffffffff
.L__real_naninf_upper_sign_mask:	.quad 0x000000000ffffffff	#
					.quad 0x000000000ffffffff	#
.L__real_naninf_lower_sign_mask:	.quad 0x0ffffffff00000000	#
					.quad 0x0ffffffff00000000	#

.Lcosarray:
	.quad	0x03fa5555555555555		#  0.0416667		   	c1
	.quad	0x03fa5555555555555
	.quad	0x0bf56c16c16c16967		# -0.00138889	   		c2
	.quad	0x0bf56c16c16c16967
	.quad	0x03efa01a019f4ec90		#  2.48016e-005			c3
	.quad	0x03efa01a019f4ec90
	.quad	0x0be927e4fa17f65f6		# -2.75573e-007			c4
	.quad	0x0be927e4fa17f65f6
	.quad	0x03e21eeb69037ab78		#  2.08761e-009			c5
	.quad	0x03e21eeb69037ab78
	.quad	0x0bda907db46cc5e42		# -1.13826e-011	   		c6
	.quad	0x0bda907db46cc5e42
.Lsinarray:
	.quad	0x0bfc5555555555555		# -0.166667	   		s1
	.quad	0x0bfc5555555555555
	.quad	0x03f81111111110bb3		#  0.00833333	   		s2
	.quad	0x03f81111111110bb3
	.quad	0x0bf2a01a019e83e5c		# -0.000198413			s3
	.quad	0x0bf2a01a019e83e5c
	.quad	0x03ec71de3796cde01		#  2.75573e-006			s4
	.quad	0x03ec71de3796cde01
	.quad	0x0be5ae600b42fdfa7		# -2.50511e-008			s5
	.quad	0x0be5ae600b42fdfa7
	.quad	0x03de5e0b2f9a43bb8		#  1.59181e-010	   		s6
	.quad	0x03de5e0b2f9a43bb8
.Lsincosarray:
	.quad	0x0bfc5555555555555		# -0.166667	   		s1
	.quad	0x03fa5555555555555		#  0.0416667		   	c1
	.quad	0x03f81111111110bb3		#  0.00833333	   		s2
	.quad	0x0bf56c16c16c16967		#				c2
	.quad	0x0bf2a01a019e83e5c		# -0.000198413			s3
	.quad	0x03efa01a019f4ec90
	.quad	0x03ec71de3796cde01		#  2.75573e-006			s4
	.quad	0x0be927e4fa17f65f6
	.quad	0x0be5ae600b42fdfa7		# -2.50511e-008			s5
	.quad	0x03e21eeb69037ab78
	.quad	0x03de5e0b2f9a43bb8		#  1.59181e-010	   		s6
	.quad	0x0bda907db46cc5e42
.Lcossinarray:
	.quad	0x03fa5555555555555		#  0.0416667		   	c1
	.quad	0x0bfc5555555555555		# -0.166667	   		s1
	.quad	0x0bf56c16c16c16967		#				c2
	.quad	0x03f81111111110bb3		#  0.00833333	   		s2
	.quad	0x03efa01a019f4ec90
	.quad	0x0bf2a01a019e83e5c		# -0.000198413			s3
	.quad	0x0be927e4fa17f65f6
	.quad	0x03ec71de3796cde01		#  2.75573e-006			s4
	.quad	0x03e21eeb69037ab78
	.quad	0x0be5ae600b42fdfa7		# -2.50511e-008			s5
	.quad	0x0bda907db46cc5e42
	.quad	0x03de5e0b2f9a43bb8		#  1.59181e-010	   		s6

#include "weak_macros.h"    

.text
.align 16
.p2align 4,,15

.equ	p_temp,		0x00		# temporary for get/put bits operation
.equ	p_temp1,	0x10		# temporary for get/put bits operation
.equ	p_temp2,	0x20		# temporary for get/put bits operation

.equ	save_xmm6,	0x30		# temporary for get/put bits operation
.equ	save_xmm7,	0x40		# temporary for get/put bits operation
.equ	save_xmm8,	0x50		# temporary for get/put bits operation
.equ	save_xmm9,	0x60		# temporary for get/put bits operation
.equ	save_xmm10,	0x70		# temporary for get/put bits operation
.equ	save_xmm11,	0x80		# temporary for get/put bits operation
.equ	save_xmm12,	0x90		# temporary for get/put bits operation
.equ	save_xmm13,	0x0A0		# temporary for get/put bits operation
.equ	save_xmm14,	0x0B0		# temporary for get/put bits operation
.equ	save_xmm15,	0x0C0		# temporary for get/put bits operation

.equ	save_rdi,	0x0D0
.equ	save_rsi,	0x0E0

.equ	r,		0x0F0		# pointer to r for remainder_piby2
.equ	rr,		0x0100		# pointer to r for remainder_piby2
.equ	region,		0x0110		# pointer to r for remainder_piby2

.equ	p_original,	0x0120		# original x
.equ	p_mask,		0x0130		# original x
.equ	p_sign,		0x0140		# original x
.equ	p_sign1,	0x0150		# original x
.equ	p_x,		0x0160		#x
.equ	p_xx,		0x0170		#xx
.equ	p_x2,		0x0180		#x2
.equ	p_sin,		0x0190		#sin
.equ	p_cos,		0x01A0		#cos
.equ	p_temp2,	0x01B0		# temporary for get/put bits operation

.globl fname
    .type   fname,@function
fname:
	sub		$0x1C8,%rsp

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#STARTMAIN

movdqa 	%xmm0,%xmm6				#move to mem to get into integer regs **
movdqa  %xmm0, p_original(%rsp)			#move to mem to get into integer regs -

andpd 	.L__real_7fffffffffffffff(%rip),%xmm0		#Unsign			-

mov	%rdi, p_sin(%rsp)		# save address for sin return
mov	%rsi, p_cos(%rsp)		# save address for cos return

movd	%xmm0,%rax				#rax is lower arg
movhpd	%xmm0, p_temp+8(%rsp)			#
mov    	p_temp+8(%rsp),%rcx			#rcx = upper arg
movdqa	%xmm0,%xmm8

pcmpgtd		%xmm6,%xmm8
movdqa		%xmm8,%xmm6
psrldq		$4,%xmm8
psrldq		$8,%xmm6

mov 	$0x3FE921FB54442D18,%rdx			#piby4
mov	$0x411E848000000000,%r10			#5e5
movapd	.L__real_3fe0000000000000(%rip),%xmm4			#0.5 for later use

por	%xmm6,%xmm8
movd	%xmm8,%r11				#Move Sign to gpr **

movapd	%xmm0,%xmm2				#x
movapd	%xmm0,%xmm6				#x

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Leither_or_both_arg_gt_than_piby4:

	cmp	%r10,%rax				#is lower arg >= 5e5
	jae	.Llower_or_both_arg_gt_5e5
	cmp	%r10,%rcx				#is upper arg >= 5e5
	jae	.Lupper_arg_gt_5e5

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Lboth_arg_lt_than_5e5:
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5

	mulpd	.L__real_3fe45f306dc9c883(%rip),%xmm2		# * twobypi
	addpd	%xmm4,%xmm2					# +0.5, npi2
	movapd	.L__real_3ff921fb54400000(%rip),%xmm0		# piby2_1
	cvttpd2dq	%xmm2,%xmm4				# convert packed double to packed integers
	movapd	.L__real_3dd0b4611a600000(%rip),%xmm8		# piby2_2
	cvtdq2pd	%xmm4,%xmm2				# and back to double.


#      /* Subtract the multiple from x to get an extra-precision remainder */

	movd	%xmm4,%r8						# Region

	mov 	.L__reald_one_zero(%rip),%rdx			#compare value for cossin path
	mov	%r8,%r10
	mov	%r8,%rcx

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


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm4  = npi2 (int), xmm0 =rhead, xmm8 =rtail

	and	.L__reald_one_one(%rip),%r8			#odd/even region for cos/sin

	shr	$1,%r10						#~AB+A~B, A is sign and B is upper bit of region
	mov	%r10,%rax
	not 	%r11						#ADDED TO CHANGE THE LOGIC
	and	%r11,%r10
	not	%rax
	not	%r11
	and	%r11,%rax
	or	%rax,%r10
	and	.L__reald_one_one(%rip),%r10				#(~AB+A~B)&1
	mov	%r10,%r11
	and	%rdx,%r11				#mask out the lower sign bit leaving the upper sign bit
	shl	$63,%r10				#shift lower sign bit left by 63 bits
	shl	$31,%r11				#shift upper sign bit left by 31 bits
	mov 	 %r10,p_sign(%rsp)		#write out lower sign bit
	mov 	 %r11,p_sign+8(%rsp)		#write out upper sign bit

# xmm4  = Sign, xmm0 =rhead, xmm8 =rtail

	movapd	%xmm0,%xmm6						# rhead
	subpd	%xmm8,%xmm0						# r = rhead - rtail

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# xmm4  = Sign, xmm0 = r, xmm6 =rhead, xmm8 =rtail

	subpd	%xmm0,%xmm6				#rr=rhead-r
	movapd	%xmm0,%xmm2				#move r for r2
	mulpd	%xmm0,%xmm2				#r2
	subpd	%xmm8,%xmm6				#rr=(rhead-r) -rtail

	mov 	.L__reald_one_zero(%rip),%r9		# Compare value for cossin	+


	add	.L__reald_one_one(%rip),%rcx
	and	.L__reald_two_two(%rip),%rcx
	shr	$1,%rcx

	mov	%rcx,%rdx
	and	%r9,%rdx				#mask out the lower sign bit leaving the upper sign bit
	shl	$63,%rcx				#shift lower sign bit left by 63 bits
	shl	$31,%rdx				#shift upper sign bit left by 31 bits
	mov 	 %rcx,p_sign1(%rsp)		#write out lower sign bit
	mov 	 %rdx,p_sign1+8(%rsp)		#write out upper sign bit

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.align 16
.L__vrd2_sincos_approximate:
	cmp 	$0,%r8
	jnz	.Lvrd2_not_sin_piby4

.Lvrd2_sin_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign  = Sign, xmm0 = r, xmm2 = %xmm6,%r2 =rr

	movdqa	.Lcosarray+0x50(%rip),%xmm4			# c6
	movdqa	.Lsinarray+0x50(%rip),%xmm5			# c6
	movapd	.Lcosarray+0x20(%rip),%xmm8			# c3
	movapd	.Lsinarray+0x20(%rip),%xmm9			# c3

	movapd	%xmm2,%xmm10					# x2
	movapd	%xmm2,%xmm11					# x2

	mulpd	%xmm2,%xmm4					# c6*x2
	mulpd	%xmm2,%xmm5					# c6*x2
	mulpd	%xmm2,%xmm8					# c3*x2
	mulpd	%xmm2,%xmm9					# c3*x2

	mulpd	.L__real_3fe0000000000000(%rip),%xmm10	# r = 0.5 *x2
	movapd	 %xmm2,p_temp(%rsp)			# store x2

	addpd	.Lcosarray+0x40(%rip),%xmm4			# c5+x2c6
	addpd	.Lsinarray+0x40(%rip),%xmm5			# c5+x2c6
	movapd	 %xmm10,p_temp2(%rsp)			# store r
	addpd	.Lcosarray+0x10(%rip),%xmm8			# c2+x2C3
	addpd	.Lsinarray+0x10(%rip),%xmm9			# c2+x2C3

	subpd	.L__real_3ff0000000000000(%rip),%xmm10	# -t=r-1.0
	mulpd	%xmm2,%xmm11					# x4

	mulpd	%xmm2,%xmm4					# x2(c5+x2c6)
	mulpd	%xmm2,%xmm5					# x2(c5+x2c6)
	movapd	 %xmm10,p_temp1(%rsp) 			# store t
	movapd	%xmm11,%xmm3					# Keep x4
	mulpd	%xmm2,%xmm8					# x2(c2+x2C3)
	mulpd	%xmm2,%xmm9					# x2(c2+x2C3)

	addpd   .L__real_3ff0000000000000(%rip),%xmm10	# 1 + (-t)
	mulpd	%xmm2,%xmm11					# x6

	addpd	.Lcosarray+0x30(%rip),%xmm4			# c4 + x2(c5+x2c6)
	addpd	.Lsinarray+0x30(%rip),%xmm5			# c4 + x2(c5+x2c6)
	addpd	.Lcosarray(%rip),%xmm8			# c1 + x2(c2+x2C3)
	addpd	.Lsinarray(%rip),%xmm9			# c1 + x2(c2+x2C3)

	subpd   p_temp2(%rsp),%xmm10			# (1 + (-t)) - r
	mulpd	%xmm0,%xmm2					# x3 recalculate

	mulpd	%xmm11,%xmm4					# x6(c4 + x2(c5+x2c6))
	mulpd	%xmm11,%xmm5					# x6(c4 + x2(c5+x2c6))

	movapd	%xmm0,%xmm1
	movapd	%xmm6,%xmm7
	mulpd	%xmm6,%xmm1					# x*xx
	mulpd	p_temp2(%rsp),%xmm7			# xx * 0.5x2

	addpd	%xmm8,%xmm4					# zc
	addpd	%xmm9,%xmm5					# zs

	subpd   %xmm1,%xmm10					# ((1 + (-t)) - r) -x*xx

	mulpd	%xmm3,%xmm4					# x4 * zc
	mulpd	%xmm2,%xmm5					# x3 * zs

	addpd	%xmm10,%xmm4					# x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	%xmm7,%xmm5					# x3*zs - 0.5 * x2 *xx

	addpd	%xmm6,%xmm5					# sin + xx
	subpd	p_temp1(%rsp),%xmm4			# cos - (-t)
	addpd	%xmm0,%xmm5					# sin + x

	jmp 	.L__vrd2_sincos_cleanup

.align 16
.Lvrd2_not_sin_piby4:
	cmp 	.L__reald_one_one(%rip),%r8
	jnz	.Lvrd2_not_cos_piby4

.Lvrd2_cos_piby4:
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# p_sign  = Sign, xmm0 = r, xmm2 = %xmm6,%r2 =rr

	movdqa	.Lcosarray+0x50(%rip),%xmm5			# c6
	movdqa	.Lsinarray+0x50(%rip),%xmm4			# c6
	movapd	.Lcosarray+0x20(%rip),%xmm9			# c3
	movapd	.Lsinarray+0x20(%rip),%xmm8			# c3

	movapd	%xmm2,%xmm10					# x2
	movapd	%xmm2,%xmm11					# x2

	mulpd	%xmm2,%xmm5					# c6*x2
	mulpd	%xmm2,%xmm4					# c6*x2
	mulpd	%xmm2,%xmm9					# c3*x2
	mulpd	%xmm2,%xmm8					# c3*x2

	mulpd	.L__real_3fe0000000000000(%rip),%xmm10	# r = 0.5 *x2
	movapd	 %xmm2,p_temp(%rsp)			# store x2

	addpd	.Lcosarray+0x40(%rip),%xmm5			# c5+x2c6
	addpd	.Lsinarray+0x40(%rip),%xmm4			# c5+x2c6
	movapd	 %xmm10,p_temp2(%rsp)			# store r
	addpd	.Lcosarray+0x10(%rip),%xmm9			# c2+x2C3
	addpd	.Lsinarray+0x10(%rip),%xmm8			# c2+x2C3

	subpd	.L__real_3ff0000000000000(%rip),%xmm10	# -t=r-1.0
	mulpd	%xmm2,%xmm11					# x4

	mulpd	%xmm2,%xmm5					# x2(c5+x2c6)
	mulpd	%xmm2,%xmm4					# x2(c5+x2c6)
	movapd	 %xmm10,p_temp1(%rsp) 			# store t
	movapd	%xmm11,%xmm3					# Keep x4
	mulpd	%xmm2,%xmm9					# x2(c2+x2C3)
	mulpd	%xmm2,%xmm8					# x2(c2+x2C3)

	addpd   .L__real_3ff0000000000000(%rip),%xmm10	# 1 + (-t)
	mulpd	%xmm2,%xmm11					# x6

	addpd	.Lcosarray+0x30(%rip),%xmm5			# c4 + x2(c5+x2c6)
	addpd	.Lsinarray+0x30(%rip),%xmm4			# c4 + x2(c5+x2c6)
	addpd	.Lcosarray(%rip),%xmm9			# c1 + x2(c2+x2C3)
	addpd	.Lsinarray(%rip),%xmm8			# c1 + x2(c2+x2C3)

	subpd   p_temp2(%rsp),%xmm10			# (1 + (-t)) - r
	mulpd	%xmm0,%xmm2					# x3 recalculate

	mulpd	%xmm11,%xmm5					# x6(c4 + x2(c5+x2c6))
	mulpd	%xmm11,%xmm4					# x6(c4 + x2(c5+x2c6))

	movapd	%xmm0,%xmm1
	movapd	%xmm6,%xmm7
	mulpd	%xmm6,%xmm1					# x*xx
	mulpd	p_temp2(%rsp),%xmm7			# xx * 0.5x2

	addpd	%xmm9,%xmm5					# zc
	addpd	%xmm8,%xmm4					# zs

	subpd   %xmm1,%xmm10					# ((1 + (-t)) - r) -x*xx

	mulpd	%xmm3,%xmm5					# x4 * zc
	mulpd	%xmm2,%xmm4					# x3 * zs

	addpd	%xmm10,%xmm5					# x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	%xmm7,%xmm4					# x3*zs - 0.5 * x2 *xx

	addpd	%xmm6,%xmm4					# sin + xx
	subpd	p_temp1(%rsp),%xmm5			# cos - (-t)
	addpd	%xmm0,%xmm4					# sin + x

	jmp 	.L__vrd2_sincos_cleanup

.align 16
.Lvrd2_not_cos_piby4:
	cmp 	$1,%r8
	jnz	.Lvrd2_cossin_piby4

.Lvrd2_sincos_piby4:
	movdqa	.Lcosarray+0x50(%rip),%xmm4			# c6
	movdqa	.Lsinarray+0x50(%rip),%xmm5			# c6
	movapd	.Lcosarray+0x20(%rip),%xmm8			# c3
	movapd	.Lsinarray+0x20(%rip),%xmm9			# c3

	movapd	%xmm2,%xmm10					# x2
	movapd	%xmm2,%xmm11					# x2

	mulpd	%xmm2,%xmm4					# c6*x2
	mulpd	%xmm2,%xmm5					# c6*x2
	mulpd	%xmm2,%xmm8					# c3*x2
	mulpd	%xmm2,%xmm9					# c3*x2

	mulpd	.L__real_3fe0000000000000(%rip),%xmm10	# r = 0.5 *x2
	movapd	 %xmm2,p_temp(%rsp)			# store x2

	addpd	.Lcosarray+0x40(%rip),%xmm4			# c5+x2c6
	addpd	.Lsinarray+0x40(%rip),%xmm5			# c5+x2c6
	movapd	 %xmm10,p_temp2(%rsp)			# store r
	addpd	.Lcosarray+0x10(%rip),%xmm8			# c2+x2C3
	addpd	.Lsinarray+0x10(%rip),%xmm9			# c2+x2C3

	subpd	.L__real_3ff0000000000000(%rip),%xmm10	# -t=r-1.0
	mulpd	%xmm2,%xmm11					# x4

	mulpd	%xmm2,%xmm4					# x2(c5+x2c6)
	mulpd	%xmm2,%xmm5					# x2(c5+x2c6)
	movapd	 %xmm10,p_temp1(%rsp) 			# store t
	movapd	%xmm11,%xmm3					# Keep x4
	mulpd	%xmm2,%xmm8					# x2(c2+x2C3)
	mulpd	%xmm2,%xmm9					# x2(c2+x2C3)

	addpd   .L__real_3ff0000000000000(%rip),%xmm10	# 1 + (-t)
	mulpd	%xmm2,%xmm11					# x6

	addpd	.Lcosarray+0x30(%rip),%xmm4			# c4 + x2(c5+x2c6)
	addpd	.Lsinarray+0x30(%rip),%xmm5			# c4 + x2(c5+x2c6)
	addpd	.Lcosarray(%rip),%xmm8			# c1 + x2(c2+x2C3)
	addpd	.Lsinarray(%rip),%xmm9			# c1 + x2(c2+x2C3)

	subpd   p_temp2(%rsp),%xmm10			# (1 + (-t)) - r
	mulpd	%xmm0,%xmm2					# x3 recalculate

	mulpd	%xmm11,%xmm4					# x6(c4 + x2(c5+x2c6))
	mulpd	%xmm11,%xmm5					# x6(c4 + x2(c5+x2c6))

	movapd	%xmm0,%xmm1
	movapd	%xmm6,%xmm7
	mulpd	%xmm6,%xmm1					# x*xx
	mulpd	p_temp2(%rsp),%xmm7			# xx * 0.5x2

	addpd	%xmm8,%xmm4					# zc
	addpd	%xmm9,%xmm5					# zs

	subpd   %xmm1,%xmm10					# ((1 + (-t)) - r) -x*xx

	mulpd	%xmm3,%xmm4					# x4 * zc
	mulpd	%xmm2,%xmm5					# x3 * zs

	addpd	%xmm10,%xmm4					# x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	%xmm7,%xmm5					# x3*zs - 0.5 * x2 *xx

	addpd	%xmm6,%xmm5					# sin + xx
	subpd	p_temp1(%rsp),%xmm4			# cos - (-t)
	addpd	%xmm0,%xmm5					# sin + x

	movsd	%xmm4,%xmm1
	movsd	%xmm5,%xmm4
	movsd	%xmm1,%xmm5

	jmp 	.L__vrd2_sincos_cleanup

.align 16
.Lvrd2_cossin_piby4:
	movdqa	.Lcosarray+0x50(%rip),%xmm5			# c6
	movdqa	.Lsinarray+0x50(%rip),%xmm4			# c6
	movapd	.Lcosarray+0x20(%rip),%xmm9			# c3
	movapd	.Lsinarray+0x20(%rip),%xmm8			# c3

	movapd	%xmm2,%xmm10					# x2
	movapd	%xmm2,%xmm11					# x2

	mulpd	%xmm2,%xmm5					# c6*x2
	mulpd	%xmm2,%xmm4					# c6*x2
	mulpd	%xmm2,%xmm9					# c3*x2
	mulpd	%xmm2,%xmm8					# c3*x2

	mulpd	.L__real_3fe0000000000000(%rip),%xmm10	# r = 0.5 *x2
	movapd	 %xmm2,p_temp(%rsp)			# store x2

	addpd	.Lcosarray+0x40(%rip),%xmm5			# c5+x2c6
	addpd	.Lsinarray+0x40(%rip),%xmm4			# c5+x2c6
	movapd	 %xmm10,p_temp2(%rsp)			# store r
	addpd	.Lcosarray+0x10(%rip),%xmm9			# c2+x2C3
	addpd	.Lsinarray+0x10(%rip),%xmm8			# c2+x2C3

	subpd	.L__real_3ff0000000000000(%rip),%xmm10	# -t=r-1.0
	mulpd	%xmm2,%xmm11					# x4

	mulpd	%xmm2,%xmm5					# x2(c5+x2c6)
	mulpd	%xmm2,%xmm4					# x2(c5+x2c6)
	movapd	 %xmm10,p_temp1(%rsp) 			# store t
	movapd	%xmm11,%xmm3					# Keep x4
	mulpd	%xmm2,%xmm9					# x2(c2+x2C3)
	mulpd	%xmm2,%xmm8					# x2(c2+x2C3)

	addpd   .L__real_3ff0000000000000(%rip),%xmm10	# 1 + (-t)
	mulpd	%xmm2,%xmm11					# x6

	addpd	.Lcosarray+0x30(%rip),%xmm5			# c4 + x2(c5+x2c6)
	addpd	.Lsinarray+0x30(%rip),%xmm4			# c4 + x2(c5+x2c6)
	addpd	.Lcosarray(%rip),%xmm9			# c1 + x2(c2+x2C3)
	addpd	.Lsinarray(%rip),%xmm8			# c1 + x2(c2+x2C3)

	subpd   p_temp2(%rsp),%xmm10			# (1 + (-t)) - r
	mulpd	%xmm0,%xmm2					# x3 recalculate

	mulpd	%xmm11,%xmm5					# x6(c4 + x2(c5+x2c6))
	mulpd	%xmm11,%xmm4					# x6(c4 + x2(c5+x2c6))

	movapd	%xmm0,%xmm1
	movapd	%xmm6,%xmm7
	mulpd	%xmm6,%xmm1					# x*xx
	mulpd	p_temp2(%rsp),%xmm7			# xx * 0.5x2

	addpd	%xmm9,%xmm5					# zc
	addpd	%xmm8,%xmm4					# zs

	subpd   %xmm1,%xmm10					# ((1 + (-t)) - r) -x*xx

	mulpd	%xmm3,%xmm5					# x4 * zc
	mulpd	%xmm2,%xmm4					# x3 * zs

	addpd	%xmm10,%xmm5					# x4*zc + (((1 + (-t)) - r) - x*xx)
	subpd	%xmm7,%xmm4					# x3*zs - 0.5 * x2 *xx

	addpd	%xmm6,%xmm4					# sin + xx
	subpd	p_temp1(%rsp),%xmm5			# cos - (-t)
	addpd	%xmm0,%xmm4					# sin + x

	movsd	%xmm5,%xmm1
	movsd	%xmm4,%xmm5
	movsd	%xmm1,%xmm4

	jmp 	.L__vrd2_sincos_cleanup

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Llower_or_both_arg_gt_5e5:
	cmp	%r10,%rcx				#is upper arg >= 5e5
	jae	.Lboth_arg_gt_5e5

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.Llower_arg_gt_5e5:
# Upper Arg is < 5e5, Lower arg is >= 5e5

	movlpd	 %xmm0,r(%rsp)		#Save lower fp arg for remainder_piby2 call

	movhlps	%xmm0,%xmm0			#Needed since we want to work on upper arg
	movhlps	%xmm2,%xmm2
	movhlps	%xmm6,%xmm6

# Work on Upper arg
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5
# Lower arg might contain nan/inf, to avoid exception use only scalar instructions on upper arg which has been moved to lower portions of fp regs

#If upper Arg is <=piby4
       cmp	%rdx,%rcx					# is upper arg > piby4
       ja	0f

       mov 	$0,%ecx						# region = 0
       mov	 %ecx,region+4(%rsp)			# store upper region
       movlpd	 %xmm0,r+8(%rsp)			# store upper r (unsigned - sign is adjusted later based on sign)
       xorpd	%xmm4,%xmm4					# rr = 0
       movlpd	 %xmm4,rr+8(%rsp)			# store upper rr
       jmp	.Lcheck_lower_arg

#If upper Arg is > piby4
.align 16
0:
	mulsd	.L__real_3fe45f306dc9c883(%rip),%xmm2		# x*twobypi
	addsd	%xmm4,%xmm2					# npi2=(x*twobypi+0.5)
	movsd	.L__real_3ff921fb54400000(%rip),%xmm3		# piby2_1
	cvttsd2si	%xmm2,%ecx				# npi2 trunc to ints
	movsd	.L__real_3dd0b4611a600000(%rip),%xmm1		# piby2_2
	cvtsi2sd	%ecx,%xmm2				# npi2 trunc to doubles

	#/* Subtract the multiple from x to get an extra-precision remainder */
	#rhead  = x - npi2 * piby2_1;
	mulsd	%xmm2,%xmm3					# npi2 * piby2_1
	subsd	%xmm3,%xmm6					# rhead =(x-npi2*piby2_1)
	movsd	.L__real_3ba3198a2e037073(%rip),%xmm8		# piby2_2tail

	#t  = rhead;
       movsd	%xmm6,%xmm5					# t = rhead

	#rtail  = npi2 * piby2_2;
       mulsd	%xmm2,%xmm1					# rtail=(npi2*piby2_2)

	#rhead  = t - rtail
       subsd	%xmm1,%xmm6					# rhead=(t-rtail)

	#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	%xmm2,%xmm8     					# npi2 * piby2_2tail
       subsd	%xmm6,%xmm5					# t-rhead
       subsd	%xmm5,%xmm1					# (rtail-(t-rhead))
       addsd	%xmm8,%xmm1					# rtail=npi2*piby2_2tail+(rtail-(t-rhead));

	#r =  rhead - rtail
	#rr = (rhead-r) -rtail
       mov	 %ecx,region+4(%rsp)			# store upper region
       movsd	%xmm6,%xmm0
       subsd	%xmm1,%xmm0					# r=(rhead-rtail)

       subsd	%xmm0,%xmm6					# rr=rhead-r
       subsd	%xmm1,%xmm6					# xmm4 = rr=((rhead-r) -rtail)

       movlpd	 %xmm0,r+8(%rsp)			# store upper r
       movlpd	 %xmm6,rr+8(%rsp)			# store upper rr

#If lower Arg is > 5e5
#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
.align 16
.Lcheck_lower_arg:
	mov		$0x07ff0000000000000,%r9			# is lower arg nan/inf
	mov		%r9,%r10
	and		%rax,%r10
	cmp		%r9,%r10
	jz		.L__vrd2_cos_lower_naninf

	lea	 region(%rsp),%rdx			# lower arg is **NOT** nan/inf
	lea	 rr(%rsp),%rsi
	lea	 r(%rsp),%rdi
	movlpd	 r(%rsp),%xmm0				#Restore lower fp arg for remainder_piby2 call
	mov	 %r11,p_temp(%rsp)			#Save Sign
        call    __amd_remainder_piby2@PLT
	mov	p_temp(%rsp),%r11			#Restore Sign

	jmp 	.L__vrd2_cos_reconstruct

.L__vrd2_cos_lower_naninf:
	mov	p_original(%rsp),%rax			# upper arg is nan/inf

	mov	$0x00008000000000000,%r9
	or	%r9,%rax
	mov	 %rax,r(%rsp)				# r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr(%rsp)				# rr = 0
	mov	 %r10d,region(%rsp)			# region =0
	and 	.L__real_naninf_lower_sign_mask(%rip),%r11	# Sign

	jmp 	.L__vrd2_cos_reconstruct

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lupper_arg_gt_5e5:
# Upper Arg is >= 5e5, Lower arg is < 5e5
	movhpd	 %xmm0,r+8(%rsp)		#Save upper fp arg for remainder_piby2 call
#	movlhps	%xmm0,%xmm0				;Not needed since we want to work on lower arg, but done just to be safe and avoide exceptions due to nan/inf and to mirror the lower_arg_gt_5e5 case
#	movlhps	%xmm2,%xmm2
#	movlhps	%xmm6,%xmm6

# Work on Lower arg
# %xmm2,,%xmm0 xmm6 = x, xmm4 = 0.5
# Upper arg might contain nan/inf, to avoid exception use only scalar instructions on lower arg

#If lower Arg is <=piby4
       cmp	%rdx,%rax					# is upper arg > piby4
       ja	0f

       mov 	$0,%eax						# region = 0
       mov	 %eax,region(%rsp)			# store upper region
       movlpd	 %xmm0,r(%rsp)				# store upper r
       xorpd	%xmm4,%xmm4					# rr = 0
       movlpd	 %xmm4,rr(%rsp)				# store upper rr
       jmp 	.Lcheck_upper_arg

.align 16
0:
#If upper Arg is > piby4
	mulsd	.L__real_3fe45f306dc9c883(%rip),%xmm2		# x*twobypi
	addsd	%xmm4,%xmm2					# npi2=(x*twobypi+0.5)
	movsd	.L__real_3ff921fb54400000(%rip),%xmm3		# piby2_1
	cvttsd2si	%xmm2,%eax				# npi2 trunc to ints
	movsd	.L__real_3dd0b4611a600000(%rip),%xmm1		# piby2_2
	cvtsi2sd	%eax,%xmm2				# npi2 trunc to doubles

#/* Subtract the multiple from x to get an extra-precision remainder */
#rhead  = x - npi2 * piby2_1;
	mulsd	%xmm2,%xmm3					# npi2 * piby2_1;
	subsd	%xmm3,%xmm6					# rhead =(x-npi2*piby2_1)
	movsd	.L__real_3ba3198a2e037073(%rip),%xmm8		# piby2_2tail

#t  = rhead;
       movsd	%xmm6,%xmm5					# t = rhead

#rtail  = npi2 * piby2_2;
       mulsd	%xmm2,%xmm1					# rtail=(npi2*piby2_2)

#rhead  = t - rtail
       subsd	%xmm1,%xmm6					# rhead=(t-rtail)

#rtail  = npi2 * piby2_2tail - ((t - rhead) - rtail);
       mulsd	%xmm2,%xmm8     					# npi2 * piby2_2tail
       subsd	%xmm6,%xmm5					# t-rhead
       subsd	%xmm5,%xmm1					# (rtail-(t-rhead))
       addsd	%xmm8,%xmm1					# rtail=npi2*piby2_2tail+(rtail-(t-rhead));

#r =  rhead - rtail
#rr = (rhead-r) -rtail
       mov	 %eax,region(%rsp)			# store lower region
       movsd	%xmm6,%xmm0
       subsd	%xmm1,%xmm0					# r=(rhead-rtail)
       subsd	%xmm0,%xmm6					# rr=rhead-r
       subsd	%xmm1,%xmm6					# rr=((rhead-r) -rtail)
       movlpd	 %xmm0,r(%rsp)				# store lower r
       movlpd	 %xmm6,rr(%rsp)				# store lower rr

#Note that volatiles will be trashed by the call
#We do not care since this is the last check
#We will construct r, rr, region and sign
.align 16
.Lcheck_upper_arg:
	mov		$0x07ff0000000000000,%r9			# is upper arg nan/inf
	mov		%r9,%r10
	and		%rcx,%r10
	cmp		%r9,%r10
	jz		.L__vrd2_cos_upper_naninf

	lea	 region+4(%rsp),%rdx			# upper arg is **NOT** nan/inf
	lea	 rr+8(%rsp),%rsi
	lea	 r+8(%rsp),%rdi
	movlpd	 r+8(%rsp),%xmm0	#Restore upper fp arg for remainder_piby2 call
	mov	 %r11,p_temp(%rsp)	#Save Sign
        call    __amd_remainder_piby2@PLT
	mov	p_temp(%rsp),%r11	#Restore Sign

	jmp 	.L__vrd2_cos_reconstruct

.L__vrd2_cos_upper_naninf:
	mov	p_original+8(%rsp),%rcx		# upper arg is nan/inf
	mov	$0x00008000000000000,%r9
	or	%r9,%rcx
	mov	 %rcx,r+8(%rsp)					# r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr+8(%rsp)					# rr = 0
	mov	 %r10d,region+4(%rsp)				# region =0
	and 	.L__real_naninf_upper_sign_mask(%rip),%r11	# Sign
	jmp 	.L__vrd2_cos_reconstruct


#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.Lboth_arg_gt_5e5:
#Upper Arg is >= 5e5, Lower arg is >= 5e5

	movhpd	%xmm0,  p_temp2(%rsp) 				#Save upper fp arg for remainder_piby2 call

	mov		$0x07ff0000000000000,%r9			#is lower arg nan/inf
	mov		%r9,%r10
	and		%rax,%r10
	cmp		%r9,%r10
	jz		.L__vrd2_cos_lower_naninf_of_both_gt_5e5

	lea	 region(%rsp),%rdx			#lower arg is **NOT** nan/inf
	lea	 rr(%rsp),%rsi
	lea	 r(%rsp),%rdi
	mov	  %rcx,p_temp(%rsp)			#Save upper arg
	mov	  %r11,p_temp1(%rsp)	#Save Sign
        call    __amd_remainder_piby2@PLT
	mov	 p_temp1(%rsp),%r11	#Restore Sign
	mov	 p_temp(%rsp),%rcx			#Restore upper arg
	jmp 	0f

.L__vrd2_cos_lower_naninf_of_both_gt_5e5:				#lower arg is nan/inf
	mov	p_original(%rsp),%rax
	mov	$0x00008000000000000,%r9
	or	%r9,%rax
	mov	 %rax,r(%rsp)				#r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	 %r10,rr(%rsp)				#rr = 0
	mov	 %r10d,region(%rsp)			#region = 0
	and 	.L__real_naninf_lower_sign_mask(%rip),%r11	# Sign

.align 16
0:
	mov		$0x07ff0000000000000,%r9			#is upper arg nan/inf
	mov		%r9,%r10
	and		%rcx,%r10
	cmp		%r9,%r10
	jz		.L__vrd2_cos_upper_naninf_of_both_gt_5e5

	lea	 region+4(%rsp),%rdx			#upper arg is **NOT** nan/inf
	lea	 rr+8(%rsp),%rsi
	lea	 r+8(%rsp),%rdi
	movlpd	 p_temp2(%rsp), %xmm0			#Restore upper fp arg for remainder_piby2 call
	mov	 %r11,p_temp(%rsp)	#Save Sign
        call    __amd_remainder_piby2@PLT
	mov	 p_temp(%rsp),%r11	#Restore Sign

	jmp 	0f

.L__vrd2_cos_upper_naninf_of_both_gt_5e5:
	mov	p_original+8(%rsp),%rcx		#upper arg is nan/inf
	mov	$0x00008000000000000,%r9
	or	%r9,%rcx
	mov	%rcx,r+8(%rsp)					#r = x | 0x0008000000000000
	xor	%r10,%r10
	mov	%r10,rr+8(%rsp)					#rr = 0
	mov	%r10d,region+4(%rsp)				#region = 0
	and 	.L__real_naninf_upper_sign_mask(%rip),%r11	# Sign

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
0:
.L__vrd2_cos_reconstruct:
#Construct p_sign=Sign for Sin term, p_sign1=Sign for Cos term, xmm0 = r, xmm2 = %xmm6,%r2 =rr, r8=region
	movapd	r(%rsp),%xmm0				#x
	movapd	%xmm0,%xmm2					#move for x2
	mulpd	%xmm2,%xmm2					#x2
	movapd	rr(%rsp),%xmm6				#xx

	mov	region(%rsp),%r8
	mov 	.L__reald_one_zero(%rip),%r9		#compare value for cossin path
	mov 	%r8,%r10
	mov	%r8,%rax
	and	.L__reald_one_one(%rip),%r8		#odd/even region for cos/sin

	shr	$1,%r10						#~AB+A~B, A is sign and B is upper bit of region
	mov	%r10,%rcx
	not 	%r11						#ADDED TO CHANGE THE LOGIC
	and	%r11,%r10
	not	%rcx
	not	%r11
	and	%r11,%rcx
	or	%rcx,%r10
	and	.L__reald_one_one(%rip),%r10				#(~AB+A~B)&1

	mov	%r10,%r11
	and	%r9,%r11				#mask out the lower sign bit leaving the upper sign bit
	shl	$63,%r10				#shift lower sign bit left by 63 bits
	shl	$31,%r11				#shift upper sign bit left by 31 bits
	mov 	 %r10,p_sign(%rsp)		#write out lower sign bit
	mov 	 %r11,p_sign+8(%rsp)		#write out upper sign bit

	add	.L__reald_one_one(%rip),%rax
	and	.L__reald_two_two(%rip),%rax
	shr	$1,%rax

	mov	%rax,%rdx
	and	%r9,%rdx				#mask out the lower sign bit leaving the upper sign bit
	shl	$63,%rax				#shift lower sign bit left by 63 bits
	shl	$31,%rdx				#shift upper sign bit left by 31 bits
	mov 	 %rax,p_sign1(%rsp)		#write out lower sign bit
	mov 	 %rdx,p_sign1+8(%rsp)		#write out upper sign bit


	jmp .L__vrd2_sincos_approximate


#ENDMAIN

#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.align 16
.L__vrd2_sincos_cleanup:

	xorpd	p_sign(%rsp),%xmm5		# SIN sign
	xorpd	p_sign1(%rsp),%xmm4		# COS sign

	mov	p_sin(%rsp),%rdi
	mov	p_cos(%rsp),%rsi

	movapd	 %xmm5,(%rdi)			# save the sin
	movapd	 %xmm4,(%rsi)			# save the cos

.Lfinal_check:
	add	$0x1C8,%rsp
	ret

