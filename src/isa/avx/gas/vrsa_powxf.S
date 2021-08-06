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
# vrsapowxf.asm
#
# An array implementation of the powf libm function.
# This routine raises the x array to a constant y power.
#
# Prototype:
#
#     void vrsa_powxf(int n, float *x, float y, float *z);
#
#   Places the results into the supplied z array.
# Does not perform error handling, but does return C99 values for error
# inputs.   Denormal results are truncated to 0.
#
#

#include "fn_macros.h"
#define fname ALM_PROTO_BAS64(vrsa_powxf)
#define fname_exp __amd_internal_vrd4_exp
#define fname_log __amd_internal_vrd4_log 

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif


# define local variable storage offsets
.equ	p_temp,0x00		# xmmword
.equ	p_negateres,0x10		# qword

.equ	p_xexp,0x20		# qword

.equ	save_rbx,0x030		#qword

.equ	p_y,0x048		# y value

.equ	p_ax,0x050		# absolute x
.equ	p_sx,0x060		# sign of x's

.equ	p_ay,0x070		# absolute y
.equ	p_yexp,0x080		# unbiased exponent of y

.equ	p_inty,0x090		# integer y indicator

.equ	p_xptr,0x0a0		# ptr to x values
.equ	p_zptr,0x0b0		# ptr to z values

.equ	p_nv,0x0b8		#qword
.equ	p_iter,0x0c0		# qword	storage for number of loop iterations

.equ	p2_temp,0x0d0		#qword
.equ	p2_temp1,0x0f0		#qword

.equ	stack_size,0x0118	# allocate 40h more than
								# we need to avoid bank conflicts


#include "weak_macros.h"

    .text
    .align 16
    .p2align 4,,15

# parameters are passed in by Linux C as:
# edi  - int    n
# rsi  - float *x
# xmm0 - float y
# rdx  - float *z

.globl fname
    .type   fname,@function
fname:

	sub		$stack_size,%rsp
	mov		%rbx,save_rbx(%rsp)	# save rbx

	movss	  %xmm0,p_y(%rsp)		# save y
	mov		  %rsi,p_xptr(%rsp)		# save pointer to x
	mov		  %rdx,p_zptr(%rsp)		# save pointer to z
    mov       %rdi,%rax
	test		%rax,%rax		# just return if count is zero
    jz        .L__final_check         # exit if not

	mov     %rax,%rcx
	mov		%rcx,p_nv(%rsp)	# save number of values

#
# classify y
# vector 32 bit integer method
#  /* See whether y is an integer.
#     inty = 0 means not an integer.
#     inty = 1 means odd integer.
#     inty = 2 means even integer.
#  */
#	movdqa  .LXMMWORD(%rip),%xmm4 PTR [rdx]
# get yexp
	mov		p_y(%rsp),%r8d						# r8 is uy
	mov		$0x07fffffff,%r9d
	and		%r8d,%r9d						# r9 is ay

## if |y| == 0	then return 1
	cmp		$0,%r9d			# is y a zero?
	jz		.Ly_zero

	mov		$0x07f800000,%eax				# EXPBITS_SP32
	and		%r9d,%eax						# y exp

	xor		%edi,%edi
	shr		$23,%eax			#>> EXPSHIFTBITS_SP32
	sub		$126,%eax		# - EXPBIAS_SP32 + 1   - eax is now the unbiased exponent
	mov		$1,%ebx
	cmp		%ebx,%eax			# if (yexp < 1)
	cmovl	%edi,%ebx
	jl		.Lsave_inty

	mov		$24,%ecx
	cmp		%ecx,%eax			# if (yexp >24)
	jle		.Lcly1
	mov		$2,%ebx
	jmp		.Lsave_inty
.Lcly1:							# else 1<=yexp<=24
	sub		%eax,%ecx			# build mask for mantissa
	shl		%cl,%ebx
	dec		%ebx				# rbx = mask = (1 << (24 - yexp)) - 1

	mov		%r8d,%eax
	and		%ebx,%eax			# if ((uy & mask) != 0)
	cmovnz	%edi,%ebx			#   inty = 0;
	jnz		.Lsave_inty

	not		%ebx				# else if (((uy & ~mask) >> (24 - yexp)) & 0x00000001)
	mov		%r8d,%eax
	and		%ebx,%eax
	shr		%cl,%eax
	inc		%edi
	and		%edi,%eax
	mov		%edi,%ebx			#  inty = 1
	jnz		.Lsave_inty
	inc		%ebx				# else	inty = 2


.Lsave_inty:
	mov		 %r8d,p_y+4(%rsp)		# save an extra copy of y
	mov		 %ebx,p_inty(%rsp)		# save inty

	mov		p_nv(%rsp),%rax	# get number of values
	mov     %rax,%rcx
# see if too few values to call the main loop
	shr		$2,%rax						# get number of iterations
	jz		.L__vsa_cleanup				# jump if only single calls
# prepare the iteration counts
	mov		%rax,p_iter(%rsp)	# save number of iterations
	shl		$2,%rax
	sub		%rax,%rcx						# compute number of extra single calls
	mov		%rcx,p_nv(%rsp)	# save number of left over values

# process the array 4 values at a time.

.L__vsa_top:
# build the input _m128
# first get x
	mov		p_xptr(%rsp),%rsi	# get x_array pointer
	movups	(%rsi),%xmm0
	prefetch	64(%rsi)


	movaps	%xmm0,%xmm2
	andps	.L__mask_nsign(%rip),%xmm0		# get abs x
	andps	.L__mask_sign(%rip),%xmm2			# mask for the sign bits
	movaps	  %xmm0,p_ax(%rsp)		# save them
	movaps	  %xmm2,p_sx(%rsp)		# save them
# convert all four x's to double
	cvtps2pd   p_ax(%rsp),%xmm0
	cvtps2pd   p_ax+8(%rsp),%xmm1
#
# do x special case checking
#
#	movdqa	%xmm4,%xmm5
#	pcmpeqd	%xmm3,%xmm5						; is y not an integer? ff's if so
#	pand	.LXMMWORD(%rip),%xmm5 PTR __mask_NaN		; these values will be NaNs, if x<0
	pxor	%xmm3,%xmm3
	xor		%eax,%eax
	mov		$0x07FC00000,%ecx
	cmp		$0,%ebx							# is y not an integer?
	cmovz	%ecx,%eax							# then set to return a NaN.  else 0.
	mov		$0x080000000,%ecx
	cmp		$1,%ebx							# is y an odd integer?
	cmovz	%ecx,%eax							# maybe set sign bit if so
	movd	%eax,%xmm5
	pshufd	$0,%xmm5,%xmm5
#	shufps	xmm5,%xmm5
#	movdqa	%xmm4,%xmm2
#	pcmpeqd	.LXMMWORD(%rip),%xmm2 PTR __mask_1		; is it odd? ff's if so
#	pand	.LXMMWORD(%rip),%xmm2 PTR __mask_sign	; these values might get their sign bit set
#	por		%xmm2,%xmm5

#	cmpps	xmm3,XMMWORD PTR p_sx[rsp],0	; if the signs are set
	pcmpeqd	p_sx(%rsp),%xmm3		# if the signs are set
	pandn	%xmm5,%xmm3						# then negateres gets the values as shown below
	movdqa	  %xmm3,p_negateres(%rsp)	# save negateres

#  /* p_negateres now means the following.
#     7FC00000 means x<0, y not an integer, return NaN.
#     80000000 means x<0, y is odd integer, so set the sign bit.
##     0 means even integer, and/or x>=0.
#  */

# **** Here starts the main calculations  ****
# The algorithm used is x**y = exp(y*log(x))
#  Extra precision is required in intermediate steps to meet the 1ulp requirement
#
# log(x) calculation
	call		fname_log@PLT		# get the double precision log value
						# for all four x's
# y* logx
	cvtps2pd   p_y(%rsp),%xmm2	#convert the two packed single y's to double

#  /* just multiply by y */
	mulpd	%xmm2,%xmm0
	mulpd	%xmm2,%xmm1

#  /* The following code computes r = exp(w) */
	call		fname_exp@PLT		# get the double exp value
						# for all four y*log(x)'s
        mov             p_xptr(%rsp),%rsi       # get x_array pointer

#
# convert all four results to double
	cvtpd2ps	%xmm0,%xmm0
	cvtpd2ps	%xmm1,%xmm1
	movlhps		%xmm1,%xmm0

# perform special case and error checking on input values

# special case checking is done first in the scalar version since
# it allows for early fast returns.  But for vectors, we consider them
# to be rare, so early returns are not necessary.  So we first compute
# the x**y values, and then check for special cases.

# we do some of the checking in reverse order of the scalar version.
# apply the negate result flags
	orps	p_negateres(%rsp),%xmm0	# get negateres

## if y is infinite or so large that the result would overflow or underflow
	mov		p_y(%rsp),%edx			# get y
	and 	$0x07fffffff,%edx					# develop ay
#	mov		$0x04f000000,%eax
	cmp		$0x04f000000,%edx
	ja		.Ly_large
.Lrnsx3:

## if x is infinite
	movdqa	p_ax(%rsp),%xmm4
	cmpps	$0,.L__mask_inf(%rip),%xmm4	# equal to infinity, ffs if so.
	movmskps %xmm4,%edx
	test	$0x0f,%edx
	jnz		.Lx_infinite
.Lrnsx1:
## if x is zero
	xorps	%xmm4,%xmm4
	cmpps	$0,p_ax(%rsp),%xmm4	# equal to zero, ffs if so.
	movmskps %xmm4,%edx
	test	$0x0f,%edx
	jnz		.Lx_zero
.Lrnsx2:
## if y is NAN
	movss	p_y(%rsp),%xmm4			# get y
	ucomiss	%xmm4,%xmm4						# comparing y to itself should
											# be true, unless y is a NaN. parity flag if NaN.
	jp		.Ly_NaN
.Lrnsx4:
## if x is NAN
	movdqa	p_ax(%rsp),%xmm4			# get x
	cmpps	$4,%xmm4,%xmm4						# a compare not equal  of x to itself should
											# be false, unless x is a NaN. ff's if NaN.
	movmskps %xmm4,%ecx
	test	$0x0f,%ecx
	jnz		.Lx_NaN
.Lrnsx5:

## if x == +1, return +1 for all x
	movdqa	.L__float_one(%rip),%xmm3	# one
	mov		p_xptr(%rsp),%rdx		# get pointer to x
	movdqa	%xmm3,%xmm2
	movdqu	(%rdx), %xmm5
	cmpps	$4,%xmm5,%xmm2		# not equal to +1.0?, ffs if not equal.
	andps	%xmm2,%xmm0						# keep the others
	andnps	%xmm3,%xmm2						# mask for ones
	orps	%xmm2,%xmm0

.L__vsa_bottom:

# update the x and y pointers
	add		$16,%rsi
	mov		%rsi,p_xptr(%rsp)	# save x_array pointer
# store the result _m128d
	mov		p_zptr(%rsp),%rdi	# get z_array pointer
	movups	%xmm0,(%rdi)
#	prefetchw	QWORD PTR [rdi+64]
	prefetch	64(%rdi)
	add		$16,%rdi
	mov		%rdi,p_zptr(%rsp)	# save z_array pointer


	mov		p_iter(%rsp),%rax	# get number of iterations
	sub		$1,%rax
	mov		%rax,p_iter(%rsp)	# save number of iterations
	jnz		.L__vsa_top


# see if we need to do any extras
	mov		p_nv(%rsp),%rax	# get number of values
	test	%rax,%rax
	jnz		.L__vsa_cleanup

.L__final_check:

	mov		save_rbx(%rsp),%rbx		# restore rbx
	add		$stack_size,%rsp
	ret

	.align 16
# we jump here when we have an odd number of calls to make at the
# end
.L__vsa_cleanup:
        mov             p_nv(%rsp),%rax      # get number of values

	mov		p_xptr(%rsp),%rsi
	mov		p_y(%rsp),%r8d						# r8 is uy

# fill in a m128 with zeroes and the extra values and then make a recursive call.
	xorps		%xmm0,%xmm0
	movaps	  %xmm0,p2_temp(%rsp)
	movaps	  %xmm0,p2_temp+16(%rsp)

	mov		(%rsi),%ecx			# we know there's at least one
	mov	 	%ecx,p2_temp(%rsp)
	mov	 	%r8d,p2_temp+16(%rsp)
	cmp		$2,%rax
	jl		.L__vsacg

	mov		4(%rsi),%ecx			# do the second value
	mov	 	%ecx,p2_temp+4(%rsp)
	mov	 	%r8d,p2_temp+20(%rsp)
	cmp		$3,%rax
	jl		.L__vsacg

	mov		8(%rsi),%ecx			# do the third value
	mov	 	%ecx,p2_temp+8(%rsp)
	mov	 	%r8d,p2_temp+24(%rsp)

.L__vsacg:
	mov		$4,%rdi			# parameter for N
	lea		p2_temp(%rsp),%rsi	# &x parameter
	movaps	p2_temp+16(%rsp),%xmm0		# y parameter
	lea		p2_temp1(%rsp),%rdx	# &z parameter
	call	fname@PLT			# call recursively to compute four values

# now copy the results to the destination array
	mov		p_zptr(%rsp),%rdi
	mov		p_nv(%rsp),%rax		# get number of values
	mov	 	p2_temp1(%rsp),%ecx
	mov		%ecx,(%rdi)			# we know there's at least one
	cmp		$2,%rax
	jl		.L__vsacgf

	mov	 	p2_temp1+4(%rsp),%ecx
	mov		%ecx,4(%rdi)			# do the second value
	cmp		$3,%rax
	jl		.L__vsacgf

	mov	 	p2_temp1+8(%rsp),%ecx
	mov		%ecx,8(%rdi)			# do the third value

.L__vsacgf:
	jmp		.L__final_check


	.align 16
.Ly_zero:
## if |y| == 0	then return 1
	mov		$0x03f800000,%ecx	# one
# fill all results with a one
	mov		p_zptr(%rsp),%r9	# &z parameter
	mov		p_nv(%rsp),%rax	# get number of values
.L__yzt:
	mov		%ecx,(%r9)			# store a 1
	add		$4,%r9
	sub		$1,%rax
	test	%rax,%rax
	jnz		.L__yzt
	jmp		.L__final_check
#       y is a NaN.
.Ly_NaN:
	mov		p_y(%rsp),%r8d
	or		$0x000400000,%r8d	# convert to QNaNs
	movd	%r8d,%xmm0			# propagate to all results
	shufps	$0,%xmm0,%xmm0
	jmp	   	.Lrnsx4

#       x is a NaN.
.Lx_NaN:
	mov		p_xptr(%rsp),%rcx	# get pointer to x
	movdqu	(%rcx),%xmm4			# get x
	movdqa	%xmm4,%xmm3
	movdqa	%xmm4,%xmm5
	movdqa	.L__mask_sigbit(%rip),%xmm2	# get the signalling bits
	cmpps	$0,%xmm4,%xmm4		# a compare equal  of x to itself should
											# be true, unless x is a NaN. 0's if NaN.
	cmpps	$4,%xmm3,%xmm3		# compare not equal, ff's if NaN.
	andps	%xmm4,%xmm0		# keep the other results
	andps	%xmm3,%xmm2		# get just the right signalling bits
	andps	%xmm5,%xmm3		# mask for the NaNs
	orps	%xmm2,%xmm3		# convert to QNaNs
	orps	%xmm3,%xmm0		# combine
	jmp	   	.Lrnsx5

#       y is infinite or so large that the result would
#         overflow or underflow.
.Ly_large:
	movdqa	  %xmm0,p_temp(%rsp)

	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		(%rcx),%eax
	mov		p_y(%rsp),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6				# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp(%rsp)

	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		4(%rcx),%eax
	mov		p_y(%rsp),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6				# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+4(%rsp)

	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		8(%rcx),%eax
	mov		p_y(%rsp),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6				# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+8(%rsp)

	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		12(%rcx),%eax
	mov		p_y(%rsp),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special6				# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+12(%rsp)

	movdqa	p_temp(%rsp),%xmm0
	jmp 	.Lrnsx3

# a subroutine to treat an individual x,y pair when y is large or infinity
# assumes x in .Ly(%rip),%eax in ebx.
# returns result in eax
.Lnp_special6:
# handle |x|==1 cases first
	mov		$0x07FFFFFFF,%r8d
	and		%eax,%r8d
	cmp		$0x03f800000,%r8d	  # jump if |x| !=1
	jnz		.Lnps6
	mov		$0x03f800000,%eax	  # return 1 for all |x|==1
	jmp 	.Lnpx64

# cases where  |x| !=1
.Lnps6:
	mov		$0x07f800000,%ecx
	xor		%eax,%eax	  # assume 0 return
	test	$0x080000000,%ebx
	jnz		.Lnps62		  # jump if y negative
# y = +inf
	cmp		$0x03f800000,%r8d
	cmovg	%ecx,%eax		  # return inf if |x| < 1
	jmp 	.Lnpx64
.Lnps62:
# y = -inf
	cmp		$0x03f800000,%r8d
	cmovl	%ecx,%eax		  # return inf if |x| < 1
	jmp 	.Lnpx64

.Lnpx64:
	ret

# handle cases where x is +/- infinity.  edx is the mask
	.align 16
.Lx_infinite:
	movdqa	  %xmm0,p_temp(%rsp)

	test	$1,%edx
	jz		.Lxinfa
	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		(%rcx),%eax
	mov		p_y(%rsp),%ebx
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp(%rsp)
.Lxinfa:
	test	$2,%edx
	jz		.Lxinfb
	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		4(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+4(%rsp)
.Lxinfb:
	test	$4,%edx
	jz		.Lxinfc
	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		8(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+8(%rsp)
.Lxinfc:
	test	$8,%edx
	jz		.Lxinfd
	mov		p_xptr(%rsp),%rcx		# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		12(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x1			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+12(%rsp)
.Lxinfd:
	movdqa	p_temp(%rsp),%xmm0
	jmp 	.Lrnsx1

# a subroutine to treat an individual x,y pair when x is +/-infinity
# assumes x in .Ly(%rip),%eax in ebx, inty in ecx.
# returns result in eax
.Lnp_special_x1:			# x is infinite
	test	$0x080000000,%eax	# is x positive
	jnz		.Lnsx11		# jump if not
	test	$0x080000000,%ebx	# is y positive
	jz		.Lnsx13		# just return if so
	xor		%eax,%eax	# else return 0
	jmp 	.Lnsx13

.Lnsx11:
	cmp		$1,%ecx		# if inty ==1
	jnz		.Lnsx12		# jump if not
	test	$0x080000000,%ebx	# is y positive
	jz		.Lnsx13		# just return if so
	mov		$0x080000000,%eax	# else return -0
	jmp 	.Lnsx13
.Lnsx12:				# inty <>1
	and		$0x07FFFFFFF,%eax	# return -x (|x|)  if y<0
	test	$0x080000000,%ebx	# is y positive
	jz		.Lnsx13		#
	xor		%eax,%eax	# return 0  if y >=0
.Lnsx13:
	ret


# handle cases where x is +/- zero.  edx is the mask of x,y pairs with |x|=0
	.align 16
.Lx_zero:
	movdqa	  %xmm0,p_temp(%rsp)

	test	$1,%edx
	jz		.Lxzera
	mov		p_xptr(%rsp),%rcx	# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp(%rsp)
.Lxzera:
	test	$2,%edx
	jz		.Lxzerb
	mov		p_xptr(%rsp),%rcx	# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		4(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+4(%rsp)
.Lxzerb:
	test	$4,%edx
	jz		.Lxzerc
	mov		p_xptr(%rsp),%rcx	# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		8(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+8(%rsp)
.Lxzerc:
	test	$8,%edx
	jz		.Lxzerd
	mov		p_xptr(%rsp),%rcx	# get pointer to x
	mov		p_y(%rsp),%ebx
	mov		12(%rcx),%eax
	mov		p_inty(%rsp),%ecx
	sub		$8,%rsp
	call	.Lnp_special_x2			# call the handler for one value
	add		$8,%rsp
	mov		  %eax,p_temp+12(%rsp)
.Lxzerd:
	movdqa	p_temp(%rsp),%xmm0
	jmp 	.Lrnsx2

# a subroutine to treat an individual x,y pair when x is +/-0
# assumes x in .Ly(%rip),%eax in ebx, inty in ecx.
# returns result in eax
	.align 16
.Lnp_special_x2:
	cmp		$1,%ecx			# if inty ==1
	jz		.Lnsx21			# jump if so
# handle cases of x=+/-0, y not integer
	xor		%eax,%eax
	mov		$0x07f800000,%ecx
	test	$0x080000000,%ebx		# is ypos
	cmovnz	%ecx,%eax
	jmp		.Lnsx23
# y is an integer
.Lnsx21:
	xor		%r8d,%r8d
	mov		$0x07f800000,%ecx
	test	$0x080000000,%ebx		# is ypos
	cmovnz	%ecx,%r8d			# set to infinity if not
	and		$0x080000000,%eax	# pickup the sign of x
	or		%r8d,%eax		# and include it in the result
.Lnsx23:
	ret

        .section .rodata
        .align 64

.L__mask_sign:		.quad 0x08000000080000000	# a sign bit mask
			.quad 0x08000000080000000

.L__mask_nsign:		.quad 0x07FFFFFFF7FFFFFFF	# a not sign bit mask
			.quad 0x07FFFFFFF7FFFFFFF

# used by inty
.L__mask_127:		.quad 0x00000007F0000007F	# EXPBIAS_SP32
			.quad 0x00000007F0000007F

.L__mask_mant:		.quad 0x0007FFFFF007FFFFF	# mantissa bit mask
			.quad 0x0007FFFFF007FFFFF

.L__mask_1:		.quad 0x00000000100000001	# 1
			.quad 0x00000000100000001

.L__mask_2:		.quad 0x00000000200000002	# 2
			.quad 0x00000000200000002

.L__mask_24:		.quad 0x00000001800000018	# 24
			.quad 0x00000001800000018

.L__mask_23:		.quad 0x00000001700000017	# 23
			.quad 0x00000001700000017

# used by special case checking

.L__float_one:		.quad 0x03f8000003f800000	# one
			.quad 0x03f8000003f800000

.L__mask_inf:		.quad 0x07f8000007F800000	# inifinity
			.quad 0x07f8000007F800000

.L__mask_ninf:		.quad 0x0ff800000fF800000	# -inifinity
			.quad 0x0ff800000fF800000

.L__mask_NaN:		.quad 0x07fC000007FC00000	# NaN
			.quad 0x07fC000007FC00000

.L__mask_sigbit:	.quad 0x00040000000400000	# QNaN bit
			.quad 0x00040000000400000

.L__mask_impbit:	.quad 0x00080000000800000	# implicit bit
			.quad 0x00080000000800000

.L__mask_ly:		.quad 0x04f0000004f000000	# large y
			.quad 0x04f0000004f000000



