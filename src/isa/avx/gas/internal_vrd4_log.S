#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
# vrd4log.asm
#
# A vector implementation of the log libm function.
#
# Prototype:
#
#    __m128d,__m128d __vrd4_log(__m128d x1, __m128d x2);
#
#   Computes the natural log of x.
#   Returns proper C99 values, but may not raise status flags properly.
#   Less than 1 ulp of error.  This version can compute 4 logs in
#   192 cycles, or 48 per value
#
# This routine computes 4 double precision log values at a time.
# The four values are passed as packed doubles in xmm0 and xmm1.
# The four results are returned as packed doubles in xmm0 and xmm1.
# Note that this represents a non-standard ABI usage, as no ABI
# ( and indeed C) currently allows returning 2 values for a function.
# It is expected that some compilers may be able to take advantage of this
# interface when implementing vectorized loops.  Using the array implementation
# of the routine requires putting the inputs into memory, and retrieving
# the results from memory.  This routine eliminates the need for this
# overhead if the data does not already reside in memory.
# This routine is derived directly from the array version.
#


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

# define local variable storage offsets
.equ	p_x,0			# temporary for error checking operation
.equ	p_idx,0x010		# index storage
.equ	p_xexp,0x020		# index storage

.equ	p_x2,0x030		# temporary for error checking operation
.equ	p_idx2,0x040		# index storage
.equ	p_xexp2,0x050		# index storage

.equ	save_xa,0x060		#qword
.equ	save_ya,0x068		#qword
.equ	save_nv,0x070		#qword
.equ	p_iter,0x078		# qword	storage for number of loop iterations

.equ	save_rbx,0x080		#qword


.equ	p2_temp,0x090		# second temporary for get/put bits operation
.equ	p2_temp1,0x0b0		# second temporary for exponent multiply

.equ	p_n1,0x0c0		# temporary for near one check
.equ	p_n12,0x0d0		# temporary for near one check


.equ	stack_size,0x0e8


# parameters are expected as:
# xmm0 - __m128d x1
# xmm1 - __m128d x2

    .text
    .align 16
    .p2align 4,,15
.globl __amd_internal_vrd4_log
    .type   __amd_internal_vrd4_log,@function
__amd_internal_vrd4_log:
	sub		$stack_size,%rsp
	mov		%rbx,save_rbx(%rsp)	# save rdi

# process 4 values at a time.

		movdqa	%xmm1,p_x2(%rsp)	# save the input values
	movdqa	%xmm0,p_x(%rsp)	# save the input values
# compute the logs

##  if NaN or inf

#      /* Store the exponent of x in xexp and put
#         f into the range [0.5,1) */

	pxor	%xmm1,%xmm1
	movdqa	%xmm0,%xmm3
	psrlq	$52,%xmm3
	psubq	.L__mask_1023(%rip),%xmm3
	packssdw	%xmm1,%xmm3
	cvtdq2pd	%xmm3,%xmm6			# xexp
	movdqa	%xmm0,%xmm2
	subpd	.L__real_one(%rip),%xmm2

	movapd	%xmm6,p_xexp(%rsp)
	andpd	.L__real_notsign(%rip),%xmm2
	xor		%rax,%rax

	movdqa	%xmm0,%xmm3
	pand	.L__real_mant(%rip),%xmm3

	cmppd	$1,.L__real_threshold(%rip),%xmm2
	movmskpd	%xmm2,%ecx
	movdqa	%xmm3,%xmm4
	mov			%ecx,p_n1(%rsp)

#/* Now  x = 2**xexp  * f,  1/2 <= f < 1. */
	psrlq	$45,%xmm3
	movdqa	%xmm3,%xmm2
	psrlq	$1,%xmm3
	paddq	.L__mask_040(%rip),%xmm3
	pand	.L__mask_001(%rip),%xmm2
	paddq	%xmm2,%xmm3

	packssdw	%xmm1,%xmm3
	cvtdq2pd	%xmm3,%xmm1
		pxor	%xmm7,%xmm7
		movdqa	p_x2(%rsp),%xmm2
		movapd	p_x2(%rsp),%xmm5
		psrlq	$52,%xmm2
		psubq	.L__mask_1023(%rip),%xmm2
		packssdw	%xmm7,%xmm2
		subpd	.L__real_one(%rip),%xmm5
		andpd	.L__real_notsign(%rip),%xmm5
		cvtdq2pd	%xmm2,%xmm6			# xexp
	xor		%rcx,%rcx
		cmppd	$1,.L__real_threshold(%rip),%xmm5
	movq	 %xmm3,p_idx(%rsp)

# reduce and get u
	por		.L__real_half(%rip),%xmm4
	movdqa	%xmm4,%xmm2
		movapd	%xmm6,p_xexp2(%rsp)

	# do near one check
		movmskpd	%xmm5,%edx
		mov			%edx,p_n12(%rsp)

	mulpd	.L__real_3f80000000000000(%rip),%xmm1				# f1 = index/128


	lea		.L__np_ln_lead_table(%rip),%rdx
	mov		p_idx(%rsp),%eax
		movdqa	p_x2(%rsp),%xmm6

	movapd	.L__real_half(%rip),%xmm5							# .5
	subpd	%xmm1,%xmm2											# f2 = f - f1
		pand	.L__real_mant(%rip),%xmm6
	mulpd	%xmm2,%xmm5
	addpd	%xmm5,%xmm1

		movdqa	%xmm6,%xmm8
		psrlq	$45,%xmm6
		movdqa	%xmm6,%xmm4

		psrlq	$1,%xmm6
		paddq	.L__mask_040(%rip),%xmm6
		pand	.L__mask_001(%rip),%xmm4
		paddq	%xmm4,%xmm6
# do error checking here for scheduling.  Saves a bunch of cycles as
# compared to doing this at the start of the routine.
##  if NaN or inf
	movapd	%xmm0,%xmm3
	andpd	.L__real_inf(%rip),%xmm3
	cmppd	$0,.L__real_inf(%rip),%xmm3
	movmskpd	%xmm3,%r8d
		packssdw	%xmm7,%xmm6
		por		.L__real_half(%rip),%xmm8
		movq	 %xmm6,p_idx2(%rsp)
		cvtdq2pd	%xmm6,%xmm9

	cmppd	$2,.L__real_zero(%rip),%xmm0
		mulpd	.L__real_3f80000000000000(%rip),%xmm9				# f1 = index/128
	movmskpd	%xmm0,%r9d
# delaying this divide helps, but moving the other one does not.
# it was after the paddq
	divpd	%xmm1,%xmm2				# u

# compute the index into the log tables
#

        movlpd   -512(%rdx,%rax,8),%xmm0                # z1
        mov             p_idx+4(%rsp),%ecx
        movhpd   -512(%rdx,%rcx,8),%xmm0                # z1
# solve for ln(1+u)
	movapd	%xmm2,%xmm1				# u
	mulpd	%xmm2,%xmm2				# u^2
	movapd	%xmm2,%xmm5
	movapd	.L__real_cb3(%rip),%xmm3
	mulpd	%xmm2,%xmm3				#Cu2
	mulpd	%xmm1,%xmm5				# u^3
	addpd	.L__real_cb2(%rip),%xmm3 #B+Cu2

	mulpd	%xmm5,%xmm2				# u^5
	movapd	.L__real_log2_lead(%rip),%xmm4

	mulpd	.L__real_cb1(%rip),%xmm5 #Au3
	addpd	%xmm5,%xmm1				# u+Au3
	mulpd	%xmm3,%xmm2				# u5(B+Cu2)

	movapd	p_xexp(%rsp),%xmm5		# xexp
	addpd	%xmm2,%xmm1				# poly
# recombine
	mulpd	%xmm5,%xmm4				# xexp * log2_lead
	addpd	%xmm4,%xmm0				#r1
	lea		.L__np_ln_tail_table(%rip),%rdx
        movlpd   -512(%rdx,%rax,8),%xmm4                #z2     +=q
        movhpd   -512(%rdx,%rcx,8),%xmm4                #z2     +=q
		lea		.L__np_ln_lead_table(%rip),%rdx
		mov		p_idx2(%rsp),%eax
		mov		p_idx2+4(%rsp),%ecx
	addpd	%xmm4,%xmm1

	mulpd	.L__real_log2_tail(%rip),%xmm5

		movapd	.L__real_half(%rip),%xmm4							# .5
		subpd	%xmm9,%xmm8											# f2 = f - f1
		mulpd	%xmm8,%xmm4
		addpd	%xmm4,%xmm9

	addpd	%xmm5,%xmm1				#r2
		divpd	%xmm9,%xmm8				# u
		movapd	p_x2(%rsp),%xmm3
		andpd	.L__real_inf(%rip),%xmm3
		cmppd	$0,.L__real_inf(%rip),%xmm3
		movmskpd	%xmm3,%r10d
		movapd	p_x2(%rsp),%xmm6
		cmppd	$2,.L__real_zero(%rip),%xmm6
		movmskpd	%xmm6,%r11d

# check for nans/infs
	test		$3,%r8d
	addpd	%xmm1,%xmm0
	jnz		.L__log_naninf
.L__vlog1:
# check for negative numbers or zero
	test		$3,%r9d
	jnz		.L__z_or_n


.L__vlog2:


	# It seems like a good idea to try and interleave
	# even more of the following code sooner into the
	# program.  But there were conflicts with the table
	# index registers, making the problem difficult.
	# After a lot of work in a branch of this file,
	# I was not able to match the speed of this version.
	# CodeAnalyst shows that there is lots of unused add
	# pipe time around the divides, but the processor
	# doesn't seem to be able to schedule in those slots.

	        movlpd   -512(%rdx,%rax,8),%xmm7                #z2     +=q
        	movhpd   -512(%rdx,%rcx,8),%xmm7                #z2     +=q

# check for near one
	mov			p_n1(%rsp),%r9d
	test			$3,%r9d
	jnz			.L__near_one1
.L__vlog2n:


	# solve for ln(1+u)
		movapd	%xmm8,%xmm9				# u
		mulpd	%xmm8,%xmm8				# u^2
		movapd	%xmm8,%xmm5
		movapd	.L__real_cb3(%rip),%xmm3
		mulpd	%xmm8,%xmm3				#Cu2
		mulpd	%xmm9,%xmm5				# u^3
		addpd	.L__real_cb2(%rip),%xmm3 		#B+Cu2

		mulpd	%xmm5,%xmm8				# u^5
		movapd	.L__real_log2_lead(%rip),%xmm4

		mulpd	.L__real_cb1(%rip),%xmm5 		#Au3
		addpd	%xmm5,%xmm9				# u+Au3
		mulpd	%xmm3,%xmm8				# u5(B+Cu2)

		movapd	p_xexp2(%rsp),%xmm5			# xexp
		addpd	%xmm8,%xmm9				# poly
	# recombine
		mulpd	%xmm5,%xmm4
		addpd	%xmm4,%xmm7				#r1
		lea		.L__np_ln_tail_table(%rip),%rdx
	        movlpd   -512(%rdx,%rax,8),%xmm2                #z2     +=q
        	movhpd   -512(%rdx,%rcx,8),%xmm2                #z2     +=q
		addpd	%xmm2,%xmm9

		mulpd	.L__real_log2_tail(%rip),%xmm5

		addpd	%xmm5,%xmm9				#r2

	# check for nans/infs
		test		$3,%r10d
		addpd	%xmm9,%xmm7
		jnz		.L__log_naninf2
.L__vlog3:
# check for negative numbers or zero
		test		$3,%r11d
		jnz		.L__z_or_n2

.L__vlog4:


	mov			p_n12(%rsp),%r9d
	test			$3,%r9d
	jnz			.L__near_one2

.L__vlog4n:

# store the result _m128d
		movapd	%xmm7,%xmm1


.L__finish:
	mov		save_rbx(%rsp),%rbx		# restore rbx
	add		$stack_size,%rsp
	ret

	.align	16
.Lboth_nearone:
# saves 10 cycles
#      r = x - 1.0;
	movapd	.L__real_two(%rip),%xmm2
	subpd	.L__real_one(%rip),%xmm0	   # r
#      u          = r / (2.0 + r);
	addpd	%xmm0,%xmm2
	movapd	%xmm0,%xmm1
	divpd	%xmm2,%xmm1		# u
	movapd	.L__real_ca4(%rip),%xmm4	  #D
	movapd	.L__real_ca3(%rip),%xmm5	  #C
#      correction = r * u;
	movapd	%xmm0,%xmm6
	mulpd	%xmm1,%xmm6		# correction
#      u          = u + u;
	addpd	%xmm1,%xmm1		#u
	movapd	%xmm1,%xmm2
	mulpd	%xmm2,%xmm2		#v =u^2
#      r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction);
	mulpd	%xmm1,%xmm5		# Cu
	movapd	%xmm1,%xmm3
	mulpd	%xmm2,%xmm3		# u^3
	mulpd	.L__real_ca2(%rip),%xmm2	#Bu^2
	mulpd	%xmm3,%xmm4		#Du^3

	addpd	.L__real_ca1(%rip),%xmm2	# +A
	movapd	%xmm3,%xmm1
	mulpd	%xmm1,%xmm1		# u^6
	addpd	%xmm4,%xmm5		#Cu+Du3

	mulpd	%xmm3,%xmm2		#u3(A+Bu2)
	mulpd	%xmm5,%xmm1		#u6(Cu+Du3)
	addpd	%xmm1,%xmm2
	subpd	%xmm6,%xmm2		# -correction

#      return r + r2;
	addpd	%xmm2,%xmm0
	ret

	.align	16
.L__near_one1:
	cmp	$3,%r9d
	jnz		.L__n1nb1

	movapd	p_x(%rsp),%xmm0
	call	.Lboth_nearone
	jmp		.L__vlog2n

	.align	16
.L__n1nb1:
	test	$1,%r9d
	jz		.L__lnn12

	movlpd	p_x(%rsp),%xmm0
	call	.L__ln1

.L__lnn12:
	test	$2,%r9d		# second number?
	jz		.L__lnn1e
	movlpd	%xmm0,p_x(%rsp)
	movlpd	p_x+8(%rsp),%xmm0
	call	.L__ln1
	movlpd	%xmm0,p_x+8(%rsp)
	movapd	p_x(%rsp),%xmm0

.L__lnn1e:
	jmp		.L__vlog2n


	.align	16
.L__near_one2:
	cmp	$3,%r9d
	jnz		.L__n1nb2

	movapd	%xmm0,%xmm8
	movapd	p_x2(%rsp),%xmm0
	call	.Lboth_nearone
	movapd	%xmm0,%xmm7
	movapd	%xmm8,%xmm0
	jmp		.L__vlog4n

	.align	16
.L__n1nb2:
	movapd	%xmm0,%xmm8
	test	$1,%r9d
	jz		.L__lnn22

	movapd	%xmm7,%xmm0
	movlpd	p_x2(%rsp),%xmm0
	call	.L__ln1
	movapd	%xmm0,%xmm7

.L__lnn22:
	test	$2,%r9d		# second number?
	jz		.L__lnn2e
	movlpd	%xmm7,p_x2(%rsp)
	movlpd	p_x2+8(%rsp),%xmm0
	call	.L__ln1
	movlpd	%xmm0,p_x2+8(%rsp)
	movapd	p_x2(%rsp),%xmm7

.L__lnn2e:
	movapd	%xmm8,%xmm0
	jmp		.L__vlog4n

	.align	16

.L__ln1:
# saves 10 cycles
#      r = x - 1.0;
	movlpd	.L__real_two(%rip),%xmm2
	subsd	.L__real_one(%rip),%xmm0	   # r
#      u          = r / (2.0 + r);
	addsd	%xmm0,%xmm2
	movsd	%xmm0,%xmm1
	divsd	%xmm2,%xmm1		# u
	movlpd	.L__real_ca4(%rip),%xmm4	  #D
	movlpd	.L__real_ca3(%rip),%xmm5	  #C
#      correction = r * u;
	movsd	%xmm0,%xmm6
	mulsd	%xmm1,%xmm6		# correction
#      u          = u + u;
	addsd	%xmm1,%xmm1		#u
	movsd	%xmm1,%xmm2
	mulsd	%xmm2,%xmm2		#v =u^2
#      r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4))) - correction);
	mulsd	%xmm1,%xmm5		# Cu
	movsd	%xmm1,%xmm3
	mulsd	%xmm2,%xmm3		# u^3
	mulsd	.L__real_ca2(%rip),%xmm2	#Bu^2
	mulsd	%xmm3,%xmm4		#Du^3

	addsd	.L__real_ca1(%rip),%xmm2	# +A
	movsd	%xmm3,%xmm1
	mulsd	%xmm1,%xmm1		# u^6
	addsd	%xmm4,%xmm5		#Cu+Du3

	mulsd	%xmm3,%xmm2		#u3(A+Bu2)
	mulsd	%xmm5,%xmm1		#u6(Cu+Du3)
	addsd	%xmm1,%xmm2
	subsd	%xmm6,%xmm2		# -correction

#      return r + r2;
	addsd	%xmm2,%xmm0
	ret

	.align	16

# at least one of the numbers was a nan or infinity
.L__log_naninf:
	test		$1,%r8d		# first number?
	jz		.L__lninf2

	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	movapd	%xmm0,%xmm1		# save the inputs
	mov		p_x(%rsp),%rdx
	movlpd	p_x(%rsp),%xmm0
	call	.L__lni
	shufpd	$2,%xmm1,%xmm0
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__lninf2:
	test		$2,%r8d		# second number?
	jz		.L__lninfe
	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	movapd	%xmm0,%xmm1		# save the inputs
	mov		p_x+8(%rsp),%rdx
	movlpd	p_x+8(%rsp),%xmm0
	call	.L__lni
	shufpd	$0,%xmm0,%xmm1
	movapd	%xmm1,%xmm0
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__lninfe:
	jmp		.L__vlog1		# continue processing if not

# at least one of the numbers was a nan or infinity
.L__log_naninf2:
	movapd	%xmm0,%xmm2
	test		$1,%r10d		# first number?
	jz		.L__lninf22

	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	movapd	%xmm7,%xmm1		# save the inputs
	mov		p_x2(%rsp),%rdx
	movlpd	p_x2(%rsp),%xmm0
	call	.L__lni
	shufpd	$2,%xmm7,%xmm0
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx
	movapd	%xmm0,%xmm7

.L__lninf22:
	test		$2,%r10d		# second number?
	jz		.L__lninfe2

	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	mov		p_x2+8(%rsp),%rdx
	movlpd	p_x2+8(%rsp),%xmm0
	call	.L__lni
	shufpd	$0,%xmm0,%xmm7
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__lninfe2:
	movapd	%xmm2,%xmm0
	jmp		.L__vlog3		# continue processing if not

# a subroutine to treat one number for nan/infinity
# the number is expected in rdx and returned in the low
# half of xmm0
.L__lni:
	mov		$0x0000FFFFFFFFFFFFF,%rax
	test	%rax,%rdx
	jnz		.L__lnan					# jump if mantissa not zero, so it's a NaN
# inf
	rcl		$1,%rdx
	jnc		.L__lne2					# log(+inf) = inf
# negative x
	movlpd	.L__real_nan(%rip),%xmm0
	ret

#NaN
.L__lnan:
	mov		$0x00008000000000000,%rax	# convert to quiet
	or		%rax,%rdx
.L__lne:
	movd	%rdx,%xmm0
.L__lne2:
	ret

	.align	16

# at least one of the numbers was a zero, a negative number, or both.
.L__z_or_n:
	test		$1,%r9d		# first number?
	jz		.L__zn2

	mov		%rax,p2_temp(%rsp)
 	mov		%rdx,p2_temp+8(%rsp)
	movapd	%xmm0,%xmm1		# save the inputs
	mov		p_x(%rsp),%rax
	call	.L__zni
	shufpd	$2,%xmm1,%xmm0
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__zn2:
	test		$2,%r9d		# second number?
	jz		.L__zne
	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	movapd	%xmm0,%xmm1		# save the inputs
	mov		p_x+8(%rsp),%rax
	call	.L__zni
	shufpd	$0,%xmm0,%xmm1
	movapd	%xmm1,%xmm0
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__zne:
	jmp		.L__vlog2

.L__z_or_n2:
	movapd	%xmm0,%xmm2
	test		$1,%r11d		# first number?
	jz		.L__zn22

	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	mov		p_x2(%rsp),%rax
	call	.L__zni
	shufpd	$2,%xmm7,%xmm0
	movapd	%xmm0,%xmm7
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__zn22:
	test		$2,%r11d		# second number?
	jz		.L__zne2

	mov		%rax,p2_temp(%rsp)
	mov		%rdx,p2_temp+8(%rsp)
	mov		p_x2+8(%rsp),%rax
	call	.L__zni
	shufpd	$0,%xmm0,%xmm7
	mov		p2_temp(%rsp),%rax
	mov		p2_temp+8(%rsp),%rdx

.L__zne2:
	movapd	%xmm2,%xmm0
	jmp		.L__vlog4
# a subroutine to treat one number for zero or negative values
# the number is expected in rax and returned in the low
# half of xmm0
.L__zni:
	shl		$1,%rax
	jnz		.L__zn_x		 ## if just a carry, then must be negative
	movlpd	.L__real_ninf(%rip),%xmm0  # C99 specs -inf for +-0
	ret
.L__zn_x:
	movlpd	.L__real_nan(%rip),%xmm0
	ret



	.section .rodata
	.align	16

.L__real_one:				.quad 0x03ff0000000000000	# 1.0
						.quad 0x03ff0000000000000					# for alignment
.L__real_two:				.quad 0x04000000000000000	# 1.0
						.quad 0x04000000000000000
.L__real_ninf:				.quad 0x0fff0000000000000	# -inf
						.quad 0x0fff0000000000000
.L__real_inf:				.quad 0x07ff0000000000000	# +inf
						.quad 0x07ff0000000000000
.L__real_nan:				.quad 0x07ff8000000000000	# NaN
						.quad 0x07ff8000000000000

.L__real_zero:				.quad 0x00000000000000000	# 0.0
						.quad 0x00000000000000000

.L__real_sign:				.quad 0x08000000000000000	# sign bit
						.quad 0x08000000000000000
.L__real_notsign:			.quad 0x07ffFFFFFFFFFFFFF	# ^sign bit
						.quad 0x07ffFFFFFFFFFFFFF
.L__real_threshold:		.quad 0x03F9EB85000000000	# .03
						.quad 0x03F9EB85000000000
.L__real_qnanbit:			.quad 0x00008000000000000	# quiet nan bit
						.quad 0x00008000000000000
.L__real_mant:				.quad 0x0000FFFFFFFFFFFFF	# mantissa bits
						.quad 0x0000FFFFFFFFFFFFF
.L__real_3f80000000000000:	.quad 0x03f80000000000000	# /* 0.0078125 = 1/128 */
						.quad 0x03f80000000000000
.L__mask_1023:				.quad 0x000000000000003ff	#
						.quad 0x000000000000003ff
.L__mask_040:				.quad 0x00000000000000040	#
						.quad 0x00000000000000040
.L__mask_001:				.quad 0x00000000000000001	#
						.quad 0x00000000000000001

.L__real_ca1:				.quad 0x03fb55555555554e6	# 8.33333333333317923934e-02
						.quad 0x03fb55555555554e6
.L__real_ca2:				.quad 0x03f89999999bac6d4	# 1.25000000037717509602e-02
						.quad 0x03f89999999bac6d4
.L__real_ca3:				.quad 0x03f62492307f1519f	# 2.23213998791944806202e-03
						.quad 0x03f62492307f1519f
.L__real_ca4:				.quad 0x03f3c8034c85dfff0	# 4.34887777707614552256e-04
						.quad 0x03f3c8034c85dfff0


.L__real_cb1:				.quad 0x03fb5555555555557	# 8.33333333333333593622e-02
						.quad 0x03fb5555555555557
.L__real_cb2:				.quad 0x03f89999999865ede	# 1.24999999978138668903e-02
						.quad 0x03f89999999865ede
.L__real_cb3:				.quad 0x03f6249423bd94741	# 2.23219810758559851206e-03
						.quad 0x03f6249423bd94741
.L__real_log2_lead:  		.quad 0x03fe62e42e0000000	# log2_lead	  6.93147122859954833984e-01
						.quad 0x03fe62e42e0000000
.L__real_log2_tail: 		.quad 0x03e6efa39ef35793c	# log2_tail	  5.76999904754328540596e-08
						.quad 0x03e6efa39ef35793c

.L__real_half:				.quad 0x03fe0000000000000	# 1/2
						.quad 0x03fe0000000000000

	.align	16

.L__np_ln_lead_table:
	.quad	0x0000000000000000 		# 0.00000000000000000000e+00
	.quad	0x3f8fc0a800000000		# 1.55041813850402832031e-02
	.quad	0x3f9f829800000000		# 3.07716131210327148438e-02
	.quad	0x3fa7745800000000		# 4.58095073699951171875e-02
	.quad	0x3faf0a3000000000		# 6.06245994567871093750e-02
	.quad	0x3fb341d700000000		# 7.52233862876892089844e-02
	.quad	0x3fb6f0d200000000		# 8.96121263504028320312e-02
	.quad	0x3fba926d00000000		# 1.03796780109405517578e-01
	.quad	0x3fbe270700000000		# 1.17783010005950927734e-01
	.quad	0x3fc0d77e00000000		# 1.31576299667358398438e-01
	.quad	0x3fc2955280000000		# 1.45181953907012939453e-01
	.quad	0x3fc44d2b00000000		# 1.58604979515075683594e-01
	.quad	0x3fc5ff3000000000		# 1.71850204467773437500e-01
	.quad	0x3fc7ab8900000000		# 1.84922337532043457031e-01
	.quad	0x3fc9525a80000000		# 1.97825729846954345703e-01
	.quad	0x3fcaf3c900000000		# 2.10564732551574707031e-01
	.quad	0x3fcc8ff780000000		# 2.23143517971038818359e-01
	.quad	0x3fce270700000000		# 2.35566020011901855469e-01
	.quad	0x3fcfb91800000000		# 2.47836112976074218750e-01
	.quad	0x3fd0a324c0000000		# 2.59957492351531982422e-01
	.quad	0x3fd1675c80000000		# 2.71933674812316894531e-01
	.quad	0x3fd22941c0000000		# 2.83768117427825927734e-01
	.quad	0x3fd2e8e280000000		# 2.95464158058166503906e-01
	.quad	0x3fd3a64c40000000		# 3.07025015354156494141e-01
	.quad	0x3fd4618bc0000000		# 3.18453729152679443359e-01
	.quad	0x3fd51aad80000000		# 3.29753279685974121094e-01
	.quad	0x3fd5d1bd80000000		# 3.40926527976989746094e-01
	.quad	0x3fd686c800000000		# 3.51976394653320312500e-01
	.quad	0x3fd739d7c0000000		# 3.62905442714691162109e-01
	.quad	0x3fd7eaf800000000		# 3.73716354370117187500e-01
	.quad	0x3fd89a3380000000		# 3.84411692619323730469e-01
	.quad	0x3fd9479400000000		# 3.94993782043457031250e-01
	.quad	0x3fd9f323c0000000		# 4.05465066432952880859e-01
	.quad	0x3fda9cec80000000		# 4.15827870368957519531e-01
	.quad	0x3fdb44f740000000		# 4.26084339618682861328e-01
	.quad	0x3fdbeb4d80000000		# 4.36236739158630371094e-01
	.quad	0x3fdc8ff7c0000000		# 4.46287095546722412109e-01
	.quad	0x3fdd32fe40000000		# 4.56237375736236572266e-01
	.quad	0x3fddd46a00000000		# 4.66089725494384765625e-01
	.quad	0x3fde744240000000		# 4.75845873355865478516e-01
	.quad	0x3fdf128f40000000		# 4.85507786273956298828e-01
	.quad	0x3fdfaf5880000000		# 4.95077252388000488281e-01
	.quad	0x3fe02552a0000000		# 5.04556000232696533203e-01
	.quad	0x3fe0723e40000000		# 5.13945698738098144531e-01
	.quad	0x3fe0be72e0000000		# 5.23248136043548583984e-01
	.quad	0x3fe109f380000000		# 5.32464742660522460938e-01
	.quad	0x3fe154c3c0000000		# 5.41597247123718261719e-01
	.quad	0x3fe19ee6a0000000		# 5.50647079944610595703e-01
	.quad	0x3fe1e85f40000000		# 5.59615731239318847656e-01
	.quad	0x3fe23130c0000000		# 5.68504691123962402344e-01
	.quad	0x3fe2795e00000000		# 5.77315330505371093750e-01
	.quad	0x3fe2c0e9e0000000		# 5.86049020290374755859e-01
	.quad	0x3fe307d720000000		# 5.94707071781158447266e-01
	.quad	0x3fe34e2880000000		# 6.03290796279907226562e-01
	.quad	0x3fe393e0c0000000		# 6.11801505088806152344e-01
	.quad	0x3fe3d90260000000		# 6.20240390300750732422e-01
	.quad	0x3fe41d8fe0000000		# 6.28608644008636474609e-01
	.quad	0x3fe4618bc0000000		# 6.36907458305358886719e-01
	.quad	0x3fe4a4f840000000		# 6.45137906074523925781e-01
	.quad	0x3fe4e7d800000000		# 6.53301239013671875000e-01
	.quad	0x3fe52a2d20000000		# 6.61398470401763916016e-01
	.quad	0x3fe56bf9c0000000		# 6.69430613517761230469e-01
	.quad	0x3fe5ad4040000000		# 6.77398800849914550781e-01
	.quad	0x3fe5ee02a0000000		# 6.85303986072540283203e-01
	.quad	0x3fe62e42e0000000		# 6.93147122859954833984e-01
	.quad 0					# for alignment

.L__np_ln_tail_table:
	.quad	0x00000000000000000 # 0	; 0.00000000000000000000e+00
	.quad	0x03e361f807c79f3db		# 5.15092497094772879206e-09
	.quad	0x03e6873c1980267c8		# 4.55457209735272790188e-08
	.quad	0x03e5ec65b9f88c69e		# 2.86612990859791781788e-08
	.quad	0x03e58022c54cc2f99		# 2.23596477332056055352e-08
	.quad	0x03e62c37a3a125330		# 3.49498983167142274770e-08
	.quad	0x03e615cad69737c93		# 3.23392843005887000414e-08
	.quad	0x03e4d256ab1b285e9		# 1.35722380472479366661e-08
	.quad	0x03e5b8abcb97a7aa2		# 2.56504325268044191098e-08
	.quad	0x03e6f34239659a5dc		# 5.81213608741512136843e-08
	.quad	0x03e6e07fd48d30177		# 5.59374849578288093334e-08
	.quad	0x03e6b32df4799f4f6		# 5.06615629004996189970e-08
	.quad	0x03e6c29e4f4f21cf8		# 5.24588857848400955725e-08
	.quad	0x03e1086c848df1b59		# 9.61968535632653505972e-10
	.quad	0x03e4cf456b4764130		# 1.34829655346594463137e-08
	.quad	0x03e63a02ffcb63398		# 3.65557749306383026498e-08
	.quad	0x03e61e6a6886b0976		# 3.33431709374069198903e-08
	.quad	0x03e6b8abcb97a7aa2		# 5.13008650536088382197e-08
	.quad	0x03e6b578f8aa35552		# 5.09285070380306053751e-08
	.quad	0x03e6139c871afb9fc		# 3.20853940845502057341e-08
	.quad	0x03e65d5d30701ce64		# 4.06713248643004200446e-08
	.quad	0x03e6de7bcb2d12142		# 5.57028186706125221168e-08
	.quad	0x03e6d708e984e1664		# 5.48356693724804282546e-08
	.quad	0x03e556945e9c72f36		# 1.99407553679345001938e-08
	.quad	0x03e20e2f613e85bda		# 1.96585517245087232086e-09
	.quad	0x03e3cb7e0b42724f6		# 6.68649386072067321503e-09
	.quad	0x03e6fac04e52846c7		# 5.89936034642113390002e-08
	.quad	0x03e5e9b14aec442be		# 2.85038578721554472484e-08
	.quad	0x03e6b5de8034e7126		# 5.09746772910284482606e-08
	.quad	0x03e6dc157e1b259d3		# 5.54234668933210171467e-08
	.quad	0x03e3b05096ad69c62		# 6.29100830926604004874e-09
	.quad	0x03e5c2116faba4cdd		# 2.61974119468563937716e-08
	.quad	0x03e665fcc25f95b47		# 4.16752115011186398935e-08
	.quad	0x03e5a9a08498d4850		# 2.47747534460820790327e-08
	.quad	0x03e6de647b1465f77		# 5.56922172017964209793e-08
	.quad	0x03e5da71b7bf7861d		# 2.76162876992552906035e-08
	.quad	0x03e3e6a6886b09760		# 7.08169709942321478061e-09
	.quad	0x03e6f0075eab0ef64		# 5.77453510221151779025e-08
	.quad	0x03e33071282fb989b		# 4.43021445893361960146e-09
	.quad	0x03e60eb43c3f1bed2		# 3.15140984357495864573e-08
	.quad	0x03e5faf06ecb35c84		# 2.95077445089736670973e-08
	.quad	0x03e4ef1e63db35f68		# 1.44098510263167149349e-08
	.quad	0x03e469743fb1a71a5		# 1.05196987538551827693e-08
	.quad	0x03e6c1cdf404e5796		# 5.23641361722697546261e-08
	.quad	0x03e4094aa0ada625e		# 7.72099925253243069458e-09
	.quad	0x03e6e2d4c96fde3ec		# 5.62089493829364197156e-08
	.quad	0x03e62f4d5e9a98f34		# 3.53090261098577946927e-08
	.quad	0x03e6467c96ecc5cbe		# 3.80080516835568242269e-08
	.quad	0x03e6e7040d03dec5a		# 5.66961038386146408282e-08
	.quad	0x03e67bebf4282de36		# 4.42287063097349852717e-08
	.quad	0x03e6289b11aeb783f		# 3.45294525105681104660e-08
	.quad	0x03e5a891d1772f538		# 2.47132034530447431509e-08
	.quad	0x03e634f10be1fb591		# 3.59655343422487209774e-08
	.quad	0x03e6d9ce1d316eb93		# 5.51581770357780862071e-08
	.quad	0x03e63562a19a9c442		# 3.60171867511861372793e-08
	.quad	0x03e54e2adf548084c		# 1.94511067964296180547e-08
	.quad	0x03e508ce55cc8c97a		# 1.54137376631349347838e-08
	.quad	0x03e30e2f613e85bda		# 3.93171034490174464173e-09
	.quad	0x03e6db03ebb0227bf		# 5.52990607758839766440e-08
	.quad	0x03e61b75bb09cb098		# 3.29990737637586136511e-08
	.quad	0x03e496f16abb9df22		# 1.18436010922446096216e-08
	.quad	0x03e65b3f399411c62		# 4.04248680368301346709e-08
	.quad	0x03e586b3e59f65355		# 2.27418915900284316293e-08
	.quad	0x03e52482ceae1ac12		# 1.70263791333409206020e-08
	.quad	0x03e6efa39ef35793c		# 5.76999904754328540596e-08
	.quad 0					# for alignment

