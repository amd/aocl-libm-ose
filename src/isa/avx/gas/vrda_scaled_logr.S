#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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
# vrda_scaled_logr.s
#
# An array implementation of the log libm function.
#  Adapted to provide a scalingi and shifting factor.  This routine is
#  used by the ACML RNG distribution functions.
#
# Prototype:
#
#    void vrda_scaled_logr(int n, double *x, double *y, double b);
#
#   Computes the natural log of x multiplied by a.
# A reduced precision routine.   Uses the intel novel reduction technique
# with frcpai to compute logs.
#  Also uses only 3 polynomial terms to acheive52-18= 34 significant digits
#
#   This specialized routine does not handle negative numbers, 0, NaNs, or infinity.
#   This routine is not C99 compliant
#   This version can compute logs in 26
#   cycles with n <= 24
#
#


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



.equ	p2_temp,0x090		# second temporary for get/put bits operation

.equ	stack_size,0x0e8

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

	.weak vrda_scaled_logr__
	.set vrda_scaled_logr__,__vrda_scaled_logr__
	.weak vrda_scaled_logr_
	.set vrda_scaled_logr_,__vrda_scaled_logr__

# Fortran interface parameters are passed in by Linux as:
# rdi - int n
# rsi - double *x
# rdx - double *y
# rcx - double *b

    .text
    .align 16
    .p2align 4,,15

#x/* a FORTRAN subroutine implementation of array log
#**     VRDA_SCALED_LOG(N,X,Y,B)
# C equivalent*/
#void vrda_scaled_logr__(int * n, double *x, double *y,double *b)
#{
#       vrda_scaled_logr(*n,x,y,b);
#}
.globl __vrda_scaled_logr__
#ifdef __GNU_ASM__
    .type   __vrda_scaled_logr__,@function
#endif
__vrda_scaled_logr__:
    mov		(%rdi),%edi
    movlpd	(%rcx),%xmm0

# C interface parameters are passed in by Linux as:
# rdi - int n
# rsi - double *x
# rdx - double *y
# xmm0 - double b

    .align 16
    .p2align 4,,15
.globl vrda_scaled_logr
#ifdef __GNU_ASM__
    .type   vrda_scaled_logr,@function
#endif
vrda_scaled_logr:
	sub		$stack_size,%rsp

# save the arguments
	mov		%rsi,save_xa(%rsp)	# save x_array pointer
	mov		%rdx,save_ya(%rsp)	# save y_array pointer
#ifdef INTEGER64
        mov             %rdi,%rax
#else
        mov             %edi,%eax
        mov             %rax,%rdi
#endif

# move the scale and shift factor to another register
	movsd		%xmm0,%xmm10
	unpcklpd	%xmm10,%xmm10

	mov		%rdi,save_nv(%rsp)	# save number of values
# see if too few values to call the main loop
	shr		$2,%rax			# get number of iterations
	jz		.L__vda_cleanup		# jump if only single calls
# prepare the iteration counts
	mov		%rax,p_iter(%rsp)	# save number of iterations
	shl		$2,%rax
	sub		%rax,%rdi		# compute number of extra single calls
	mov		%rdi,save_nv(%rsp)	# save number of left over values

# In this second version, process the array 2 values at a time.

.L__vda_top:
# build the input _m128d
	mov		save_xa(%rsp),%rsi	# get x_array pointer
	movlpd	(%rsi),%xmm0
	movhpd	8(%rsi),%xmm0
	prefetch	64(%rsi)
	add		$32,%rsi
	mov		%rsi,save_xa(%rsp)	# save x_array pointer

                movlpd  -16(%rsi),%xmm1
                movhpd  -8(%rsi),%xmm1

# compute the logs

#	movdqa	%xmm0,p_x(%rsp)	# save the input values

# use the  algorithm referenced in the itanic trancendental paper.

# reduction
#  compute r = x frcpa(x) - 1
        movdqa  %xmm0,%xmm8
        movdqa  %xmm1,%xmm9

        call    __vrd4_frcpa@PLT
        movdqa  %xmm8,%xmm4
                movdqa  %xmm9,%xmm7
# invert the exponent
        psllq   $1,%xmm8
                psllq   $1,%xmm9
        mulpd   %xmm0,%xmm4                             # r
                mulpd   %xmm1,%xmm7                     # r
        movdqa  %xmm8,%xmm5
        paddq   .L__mask_rup(%rip),%xmm8
        psrlq   $53,%xmm8
                movdqa  %xmm9,%xmm6
                paddq   .L__mask_rup(%rip),%xmm6
                psrlq   $53,%xmm6
        psubq   .L__mask_3ff(%rip),%xmm8
                psubq   .L__mask_3ff(%rip),%xmm6
        pshufd  $0x058,%xmm8,%xmm8
                pshufd  $0x058,%xmm6,%xmm6


        subpd   .L__real_one(%rip),%xmm4
                subpd   .L__real_one(%rip),%xmm7

        cvtdq2pd        %xmm8,%xmm0             #N
                cvtdq2pd        %xmm6,%xmm1             #N
#	movdqa	%xmm8,%xmm0
#	movdqa	%xmm6,%xmm1
# compute index for table lookup. if 1/2 bit set, increment the index+exponent
        psrlq   $42,%xmm5
                psrlq   $42,%xmm9
        paddq   .L__int_one(%rip),%xmm5
                paddq   .L__int_one(%rip),%xmm9
        psrlq   $1,%xmm5
                psrlq   $1,%xmm9
        pand    .L__mask_3ff(%rip),%xmm5
                pand    .L__mask_3ff(%rip),%xmm9
        psllq   $1,%xmm5
                psllq   $1,%xmm9

        movdqa  %xmm5,p_x(%rsp) # move the indexes to a memory location
                movdqa  %xmm9,p_x2(%rsp)


        movapd  .L__real_third(%rip),%xmm3
                movdqa  %xmm3,%xmm5
        movapd  %xmm4,%xmm2
                movapd  %xmm7,%xmm8

# approximation
#  compute the polynomial
#   p(r) = p1r^2+p2r^3+p3r^4+p4r^5

        mulpd   %xmm4,%xmm2                     #r^2
                mulpd   %xmm7,%xmm8                     #r^2

        mulpd   %xmm4,%xmm3                     # 1/3r
                mulpd   %xmm7,%xmm5                     # 1/3r
# lookup the f(k) term
        lea             .L__np_lnf_table(%rip),%rdx
        mov             p_x(%rsp),%rcx
        mov             p_x+8(%rsp),%r9
        movlpd          (%rdx,%rcx,8),%xmm6     # lookup
        movhpd          (%rdx,%r9,8),%xmm6      # lookup

        addpd   .L__real_half(%rip),%xmm3  # p2 + p3r
                addpd   .L__real_half(%rip),%xmm5  # p2 + p3r

                mov             p_x2(%rsp),%rcx
                mov             p_x2+8(%rsp),%r9
                movlpd          (%rdx,%rcx,8),%xmm9     # lookup
                movhpd          (%rdx,%r9,8),%xmm9      # lookup

        mulpd   %xmm3,%xmm2                     # r2(p2 + p3r)
                mulpd   %xmm5,%xmm8                     # r2(p2 + p3r)
        addpd   %xmm4,%xmm2                     # +r
                addpd   %xmm7,%xmm8                     # +r


#       reconstruction
#  compute ln(x) = T + r + p(r) where
#   T = N*ln(2)+ln(1/frcpa(x)) via tab of ln(1/frcpa(y)), where y = 1 + k/256, 0<=k<=255

        mulpd   .L__real_log2(%rip),%xmm0        # compute  N*__real_log2
                mulpd   .L__real_log2(%rip),%xmm1        # compute  N*__real_log2
        addpd   %xmm6,%xmm2     # add the new mantissas
                addpd   %xmm9,%xmm8     # add the new mantissas
        addpd   %xmm2,%xmm0
                addpd   %xmm8,%xmm1


# store the result _m128d
	mov		save_ya(%rsp),%rdi	# get y_array pointer
	mulpd	%xmm10,%xmm0
	movlpd	%xmm0,(%rdi)
	movhpd	 %xmm0,8(%rdi)


	prefetch	64(%rdi)
	add		$32,%rdi
	mov		%rdi,save_ya(%rsp)	# save y_array pointer

# store the result _m128d
		mulpd	%xmm10,%xmm1
		movlpd	%xmm1,-16(%rdi)
		movhpd	%xmm1,-8(%rdi)

	mov		p_iter(%rsp),%rax	# get number of iterations
	sub		$1,%rax
	mov		%rax,p_iter(%rsp)	# save number of iterations
	jnz		.L__vda_top


# see if we need to do any extras
	mov		save_nv(%rsp),%rax	# get number of values
	test		%rax,%rax
	jnz		.L__vda_cleanup


.L__finish:
	add		$stack_size,%rsp
	ret

	.align	16



# we jump here when we have an odd number of log calls to make at the
# end
#  we assume that rdx is pointing at the next x array element,
#  r8 at the next y array element.  The number of values left is in
#  save_nv
.L__vda_cleanup:
        mov             save_nv(%rsp),%rax      # get number of values
        test            %rax,%rax               # are there any values
        jz              .L__finish              # exit if not

	mov		save_xa(%rsp),%rsi
	mov		save_ya(%rsp),%rdi

# fill in a m128d with zeroes and the extra values and then make a recursive call.
	xorpd		%xmm0,%xmm0
	movlpd	%xmm0,p_x+8(%rsp)
	movapd	%xmm0,p_x+16(%rsp)

	mov		(%rsi),%rcx			# we know there's at least one
	mov	 	%rcx,p_x(%rsp)
	cmp		$2,%rax
	jl		.L__vdacg

	mov		8(%rsi),%rcx			# do the second value
	mov	 	%rcx,p_x+8(%rsp)
	cmp		$3,%rax
	jl		.L__vdacg

	mov		16(%rsi),%rcx			# do the third value
	mov	 	%rcx,p_x+16(%rsp)

.L__vdacg:
	mov		$4,%rdi				# parameter for N
	lea		p_x(%rsp),%rsi		# &x parameter
	lea		p2_temp(%rsp),%rdx	# &y parameter
	movsd		%xmm10,%xmm0
	call		vrda_scaled_logr@PLT		# call recursively to compute four values

# now copy the results to the destination array
	mov		save_ya(%rsp),%rdi
	mov		save_nv(%rsp),%rax	# get number of values
	mov	 	p2_temp(%rsp),%rcx
	mov		%rcx,(%rdi)			# we know there's at least one
	cmp		$2,%rax
	jl		.L__vdacgf

	mov	 	p2_temp+8(%rsp),%rcx
	mov		%rcx,8(%rdi)			# do the second value
	cmp		$3,%rax
	jl		.L__vdacgf

	mov	 	p2_temp+16(%rsp),%rcx
	mov		%rcx,16(%rdi)			# do the third value

.L__vdacgf:
	jmp		.L__finish

	.section .rodata
	.align	64

.L__real_one:		.quad 0x03ff0000000000000	# 1.0
			.quad 0x03ff0000000000000

.L__real_half:		.quad 0x0bfe0000000000000	# 1/2
			.quad 0x0bfe0000000000000
.L__real_third:		.quad 0x03fd5555555555555	# 1/3
			.quad 0x03fd5555555555555
.L__real_fourth:	.quad 0x0bfd0000000000000	# 1/4
			.quad 0x0bfd0000000000000

.L__real_log2:	        .quad 0x03FE62E42FEFA39EF  # 0.693147182465
		        .quad 0x03FE62E42FEFA39EF

.L__mask_3ff:		.quad 0x000000000000003ff	#
			.quad 0x000000000000003ff

.L__mask_rup:           .quad 0x0000003fffffffffe
                        .quad 0x0000003fffffffffe

.L__int_one:		.quad 0x00000000000000001
			.quad 0x00000000000000001




.L__np_lnf_table:
#log table Program - logtab.c
#Built Jan 18 2006  09:51:57
#Compiler version  1400

    .quad 0x00000000000000000  # 0.000000000000 0
    .quad 0x00000000000000000
    .quad 0x03F50020055655885  # 0.000977039648 1
    .quad 0x03F50020055655885
    .quad 0x03F60040155D5881E  # 0.001955034836 2
    .quad 0x03F60040155D5881E
    .quad 0x03F6809048289860A  # 0.002933987435 3
    .quad 0x03F6809048289860A
    .quad 0x03F70080559588B25  # 0.003913899321 4
    .quad 0x03F70080559588B25
    .quad 0x03F740C8A7478788D  # 0.004894772377 5
    .quad 0x03F740C8A7478788D
    .quad 0x03F78121214586B02  # 0.005876608489 6
    .quad 0x03F78121214586B02
    .quad 0x03F7C189CBB0E283F  # 0.006859409551 7
    .quad 0x03F7C189CBB0E283F
    .quad 0x03F8010157588DE69  # 0.007843177461 8
    .quad 0x03F8010157588DE69
    .quad 0x03F82145E939EF1BC  # 0.008827914124 9
    .quad 0x03F82145E939EF1BC
    .quad 0x03F83D8896A83D7A8  # 0.009690354884 10
    .quad 0x03F83D8896A83D7A8
    .quad 0x03F85DDC705054DFF  # 0.010676913110 11
    .quad 0x03F85DDC705054DFF
    .quad 0x03F87E38762CA0C6D  # 0.011664445593 12
    .quad 0x03F87E38762CA0C6D
    .quad 0x03F89E9CAC6007563  # 0.012652954261 13
    .quad 0x03F89E9CAC6007563
    .quad 0x03F8BF091710935A4  # 0.013642441046 14
    .quad 0x03F8BF091710935A4
    .quad 0x03F8DF7DBA6777895  # 0.014632907884 15
    .quad 0x03F8DF7DBA6777895
    .quad 0x03F8FBEA8B13C03F9  # 0.015500371846 16
    .quad 0x03F8FBEA8B13C03F9
    .quad 0x03F90E3751F24F45C  # 0.016492681528 17
    .quad 0x03F90E3751F24F45C
    .quad 0x03F91E7D80B1FBF4C  # 0.017485976867 18
    .quad 0x03F91E7D80B1FBF4C
    .quad 0x03F92CBE4F6CC56C3  # 0.018355920375 19
    .quad 0x03F92CBE4F6CC56C3
    .quad 0x03F93D0C443D7258C  # 0.019351069108 20
    .quad 0x03F93D0C443D7258C
    .quad 0x03F94D5E6176ACC89  # 0.020347209148 21
    .quad 0x03F94D5E6176ACC89
    .quad 0x03F95DB4A937DEF10  # 0.021344342472 22
    .quad 0x03F95DB4A937DEF10
    .quad 0x03F96C039490E37F4  # 0.022217650494 23
    .quad 0x03F96C039490E37F4
    .quad 0x03F97C61B1CF5DED7  # 0.023216651576 24
    .quad 0x03F97C61B1CF5DED7
    .quad 0x03F98AB77B3FD6EAD  # 0.024091596947 25
    .quad 0x03F98AB77B3FD6EAD
    .quad 0x03F99B1D75828E780  # 0.025092472797 26
    .quad 0x03F99B1D75828E780
    .quad 0x03F9AB87A478CB7CB  # 0.026094351403 27
    .quad 0x03F9AB87A478CB7CB
    .quad 0x03F9B9E8027E1916F  # 0.026971819338 28
    .quad 0x03F9B9E8027E1916F
    .quad 0x03F9CA5A1A18613E6  # 0.027975583538 29
    .quad 0x03F9CA5A1A18613E6
    .quad 0x03F9D8C1670325921  # 0.028854704473 30
    .quad 0x03F9D8C1670325921
    .quad 0x03F9E93B6EE41F674  # 0.029860361378 31
    .quad 0x03F9E93B6EE41F674
    .quad 0x03F9F7A9B16782855  # 0.030741141554 32
    .quad 0x03F9F7A9B16782855
    .quad 0x03FA0415D89E74440  # 0.031748698315 33
    .quad 0x03FA0415D89E74440
    .quad 0x03FA0C58FA19DFAAB  # 0.032757271269 34
    .quad 0x03FA0C58FA19DFAAB
    .quad 0x03FA139577CC41C1A  # 0.033640607815 35
    .quad 0x03FA139577CC41C1A
    .quad 0x03FA1AD398C6CD57C  # 0.034524725334 36
    .quad 0x03FA1AD398C6CD57C
    .quad 0x03FA231C9C40E204E  # 0.035536103423 37
    .quad 0x03FA231C9C40E204E
    .quad 0x03FA2A5E4231CF7BD  # 0.036421899115 38
    .quad 0x03FA2A5E4231CF7BD
    .quad 0x03FA32AB4D4C59CB0  # 0.037435198758 39
    .quad 0x03FA32AB4D4C59CB0
    .quad 0x03FA39F07BA0EBD5A  # 0.038322679007 40
    .quad 0x03FA39F07BA0EBD5A
    .quad 0x03FA424192495D571  # 0.039337907520 41
    .quad 0x03FA424192495D571
    .quad 0x03FA498A4C73DA65D  # 0.040227078744 42
    .quad 0x03FA498A4C73DA65D
    .quad 0x03FA50D4AF75CA86F  # 0.041117041297 43
    .quad 0x03FA50D4AF75CA86F
    .quad 0x03FA592BBC15215BC  # 0.042135112141 44
    .quad 0x03FA592BBC15215BC
    .quad 0x03FA6079B00423FF6  # 0.043026775152 45
    .quad 0x03FA6079B00423FF6
    .quad 0x03FA67C94F2D4BB65  # 0.043919233935 46
    .quad 0x03FA67C94F2D4BB65
    .quad 0x03FA70265A550E77B  # 0.044940163069 47
    .quad 0x03FA70265A550E77B
    .quad 0x03FA77798F8D6DFDC  # 0.045834331871 48
    .quad 0x03FA77798F8D6DFDC
    .quad 0x03FA7ECE7267CD123  # 0.046729300926 49
    .quad 0x03FA7ECE7267CD123
    .quad 0x03FA873184BC09586  # 0.047753104446 50
    .quad 0x03FA873184BC09586
    .quad 0x03FA8E8A02D2E3175  # 0.048649793163 51
    .quad 0x03FA8E8A02D2E3175
    .quad 0x03FA95E430F8CE456  # 0.049547286652 52
    .quad 0x03FA95E430F8CE456
    .quad 0x03FA9D400FF482586  # 0.050445586359 53
    .quad 0x03FA9D400FF482586
    .quad 0x03FAA5AB21CB34A9E  # 0.051473203662 54
    .quad 0x03FAA5AB21CB34A9E
    .quad 0x03FAAD0AA2E784EF4  # 0.052373235867 55
    .quad 0x03FAAD0AA2E784EF4
    .quad 0x03FAB46BD74DA76A0  # 0.053274078860 56
    .quad 0x03FAB46BD74DA76A0
    .quad 0x03FABBCEBFC68F424  # 0.054175734102 57
    .quad 0x03FABBCEBFC68F424
    .quad 0x03FAC3335D1BBAE4D  # 0.055078203060 58
    .quad 0x03FAC3335D1BBAE4D
    .quad 0x03FACBA87200EB8F1  # 0.056110594428 59
    .quad 0x03FACBA87200EB8F1
    .quad 0x03FAD310BA20455A2  # 0.057014812019 60
    .quad 0x03FAD310BA20455A2
    .quad 0x03FADA7AB998B77ED  # 0.057919847959 61
    .quad 0x03FADA7AB998B77ED
    .quad 0x03FAE1E6713606CFB  # 0.058825703731 62
    .quad 0x03FAE1E6713606CFB
    .quad 0x03FAE953E1C48603A  # 0.059732380822 63
    .quad 0x03FAE953E1C48603A
    .quad 0x03FAF0C30C1116351  # 0.060639880722 64
    .quad 0x03FAF0C30C1116351
    .quad 0x03FAF833F0E927711  # 0.061548204926 65
    .quad 0x03FAF833F0E927711
    .quad 0x03FAFFA6911AB9309  # 0.062457354934 66
    .quad 0x03FAFFA6911AB9309
    .quad 0x03FB038D76BA2D737  # 0.063367332247 67
    .quad 0x03FB038D76BA2D737
    .quad 0x03FB0748836296412  # 0.064278138373 68
    .quad 0x03FB0748836296412
    .quad 0x03FB0B046EEE6F7A4  # 0.065189774824 69
    .quad 0x03FB0B046EEE6F7A4
    .quad 0x03FB0EC139C5DA5FD  # 0.066102243114 70
    .quad 0x03FB0EC139C5DA5FD
    .quad 0x03FB127EE451413A8  # 0.067015544762 71
    .quad 0x03FB127EE451413A8
    .quad 0x03FB163D6EF9579FC  # 0.067929681294 72
    .quad 0x03FB163D6EF9579FC
    .quad 0x03FB19FCDA271ABC0  # 0.068844654235 73
    .quad 0x03FB19FCDA271ABC0
    .quad 0x03FB1DBD2643D1912  # 0.069760465119 74
    .quad 0x03FB1DBD2643D1912
    .quad 0x03FB217E53B90D3CE  # 0.070677115481 75
    .quad 0x03FB217E53B90D3CE
    .quad 0x03FB254062F0A9417  # 0.071594606862 76
    .quad 0x03FB254062F0A9417
    .quad 0x03FB29035454CBCB0  # 0.072512940806 77
    .quad 0x03FB29035454CBCB0
    .quad 0x03FB2CC7284FE5F1A  # 0.073432118863 78
    .quad 0x03FB2CC7284FE5F1A
    .quad 0x03FB308BDF4CB4062  # 0.074352142586 79
    .quad 0x03FB308BDF4CB4062
    .quad 0x03FB345179B63DD3F  # 0.075273013532 80
    .quad 0x03FB345179B63DD3F
    .quad 0x03FB3817F7F7D6EAB  # 0.076194733263 81
    .quad 0x03FB3817F7F7D6EAB
    .quad 0x03FB3BDF5A7D1EE5E  # 0.077117303344 82
    .quad 0x03FB3BDF5A7D1EE5E
    .quad 0x03FB3F1D405CE86D3  # 0.077908755701 83
    .quad 0x03FB3F1D405CE86D3
    .quad 0x03FB42E64BEC266E4  # 0.078832909176 84
    .quad 0x03FB42E64BEC266E4
    .quad 0x03FB46B03CF437BC4  # 0.079757917501 85
    .quad 0x03FB46B03CF437BC4
    .quad 0x03FB4A7B13E1E3E65  # 0.080683782259 86
    .quad 0x03FB4A7B13E1E3E65
    .quad 0x03FB4E46D1223FE84  # 0.081610505036 87
    .quad 0x03FB4E46D1223FE84
    .quad 0x03FB52137522AE732  # 0.082538087426 88
    .quad 0x03FB52137522AE732
    .quad 0x03FB5555DE434F2A0  # 0.083333843436 89
    .quad 0x03FB5555DE434F2A0
    .quad 0x03FB59242FF043D34  # 0.084263026485 90
    .quad 0x03FB59242FF043D34
    .quad 0x03FB5CF36997817B2  # 0.085193073719 91
    .quad 0x03FB5CF36997817B2
    .quad 0x03FB60C38BA799459  # 0.086123986746 92
    .quad 0x03FB60C38BA799459
    .quad 0x03FB6408F471C82A2  # 0.086922602521 93
    .quad 0x03FB6408F471C82A2
    .quad 0x03FB67DAC7466CB96  # 0.087855127734 94
    .quad 0x03FB67DAC7466CB96
    .quad 0x03FB6BAD83C1883BA  # 0.088788523361 95
    .quad 0x03FB6BAD83C1883BA
    .quad 0x03FB6EF528C056A2D  # 0.089589270768 96
    .quad 0x03FB6EF528C056A2D
    .quad 0x03FB72C9985035BB1  # 0.090524287199 97
    .quad 0x03FB72C9985035BB1
    .quad 0x03FB769EF2C6B5688  # 0.091460178704 98
    .quad 0x03FB769EF2C6B5688
    .quad 0x03FB79E8D70A364C6  # 0.092263069152 99
    .quad 0x03FB79E8D70A364C6
    .quad 0x03FB7DBFE6EA733FE  # 0.093200590148 100
    .quad 0x03FB7DBFE6EA733FE
    .quad 0x03FB8197E2F40E3F0  # 0.094138990914 101
    .quad 0x03FB8197E2F40E3F0
    .quad 0x03FB84E40992A4804  # 0.094944035906 102
    .quad 0x03FB84E40992A4804
    .quad 0x03FB88BDBD5FC66D2  # 0.095884074919 103
    .quad 0x03FB88BDBD5FC66D2
    .quad 0x03FB8C985E9B9EC7E  # 0.096824998438 104
    .quad 0x03FB8C985E9B9EC7E
    .quad 0x03FB8FE6CAB20E979  # 0.097632209567 105
    .quad 0x03FB8FE6CAB20E979
    .quad 0x03FB93C3261014C65  # 0.098574780162 106
    .quad 0x03FB93C3261014C65
    .quad 0x03FB97130DC9235DE  # 0.099383405543 107
    .quad 0x03FB97130DC9235DE
    .quad 0x03FB9AF124D64C623  # 0.100327628989 108
    .quad 0x03FB9AF124D64C623
    .quad 0x03FB9E4289871E964  # 0.101137673586 109
    .quad 0x03FB9E4289871E964
    .quad 0x03FBA2225DD276FCB  # 0.102083555691 110
    .quad 0x03FBA2225DD276FCB
    .quad 0x03FBA57540D1FE441  # 0.102895024494 111
    .quad 0x03FBA57540D1FE441
    .quad 0x03FBA956D3ECADE60  # 0.103842571097 112
    .quad 0x03FBA956D3ECADE60
    .quad 0x03FBACAB3693AB9C0  # 0.104655469123 113
    .quad 0x03FBACAB3693AB9C0
    .quad 0x03FBB08E8A10F96F4  # 0.105604686090 114
    .quad 0x03FBB08E8A10F96F4
    .quad 0x03FBB3E46DBA02181  # 0.106419018383 115
    .quad 0x03FBB3E46DBA02181
    .quad 0x03FBB7C9832F58018  # 0.107369911615 116
    .quad 0x03FBB7C9832F58018
    .quad 0x03FBBB20E936D6976  # 0.108185683244 117
    .quad 0x03FBBB20E936D6976
    .quad 0x03FBBF07C23BC54EA  # 0.109138258671 118
    .quad 0x03FBBF07C23BC54EA
    .quad 0x03FBC260ABFFFE972  # 0.109955474734 119
    .quad 0x03FBC260ABFFFE972
    .quad 0x03FBC6494A2E418A0  # 0.110909738320 120
    .quad 0x03FBC6494A2E418A0
    .quad 0x03FBC9A3B90F57748  # 0.111728403941 121
    .quad 0x03FBC9A3B90F57748
    .quad 0x03FBCCFEDBFEE13A8  # 0.112547740324 122
    .quad 0x03FBCCFEDBFEE13A8
    .quad 0x03FBD0EA1362CDBFC  # 0.113504482008 123
    .quad 0x03FBD0EA1362CDBFC
    .quad 0x03FBD446BD753D433  # 0.114325275488 124
    .quad 0x03FBD446BD753D433
    .quad 0x03FBD7A41C8627307  # 0.115146743223 125
    .quad 0x03FBD7A41C8627307
    .quad 0x03FBDB91F09680DF9  # 0.116105975911 126
    .quad 0x03FBDB91F09680DF9
    .quad 0x03FBDEF0D8D466DBB  # 0.116928908339 127
    .quad 0x03FBDEF0D8D466DBB
    .quad 0x03FBE2507702AF03B  # 0.117752518544 128
    .quad 0x03FBE2507702AF03B
    .quad 0x03FBE640EB3D2B411  # 0.118714255240 129
    .quad 0x03FBE640EB3D2B411
    .quad 0x03FBE9A214A69DD58  # 0.119539337795 130
    .quad 0x03FBE9A214A69DD58
    .quad 0x03FBED03F4F440969  # 0.120365101673 131
    .quad 0x03FBED03F4F440969
    .quad 0x03FBF0F70CDD992E4  # 0.121329355484 132
    .quad 0x03FBF0F70CDD992E4
    .quad 0x03FBF45A7A78B7C3B  # 0.122156599431 133
    .quad 0x03FBF45A7A78B7C3B
    .quad 0x03FBF7BE9FEDBFDED  # 0.122984528276 134
    .quad 0x03FBF7BE9FEDBFDED
    .quad 0x03FBFB237D8AB13FB  # 0.123813143156 135
    .quad 0x03FBFB237D8AB13FB
    .quad 0x03FBFF1A13EAC95FD  # 0.124780729104 136
    .quad 0x03FBFF1A13EAC95FD
    .quad 0x03FC014040CAB0229  # 0.125610834299 137
    .quad 0x03FC014040CAB0229
    .quad 0x03FC02F3D4301417B  # 0.126441629140 138
    .quad 0x03FC02F3D4301417B
    .quad 0x03FC04A7C44CF87A4  # 0.127273114776 139
    .quad 0x03FC04A7C44CF87A4
    .quad 0x03FC06A4D1D26C5E9  # 0.128244055971 140
    .quad 0x03FC06A4D1D26C5E9
    .quad 0x03FC08598B59E3A07  # 0.129077042275 141
    .quad 0x03FC08598B59E3A07
    .quad 0x03FC0A0EA2164AF02  # 0.129910723024 142
    .quad 0x03FC0A0EA2164AF02
    .quad 0x03FC0BC4162F73B66  # 0.130745099376 143
    .quad 0x03FC0BC4162F73B66
    .quad 0x03FC0D79E7CD48E58  # 0.131580172493 144
    .quad 0x03FC0D79E7CD48E58
    .quad 0x03FC0F301717CF0FB  # 0.132415943541 145
    .quad 0x03FC0F301717CF0FB
    .quad 0x03FC10E6A437247B7  # 0.133252413686 146
    .quad 0x03FC10E6A437247B7
    .quad 0x03FC12E6BFA8FEAD6  # 0.134229180665 147
    .quad 0x03FC12E6BFA8FEAD6
    .quad 0x03FC149E189F8642E  # 0.135067169541 148
    .quad 0x03FC149E189F8642E
    .quad 0x03FC1655CFEA923A4  # 0.135905861231 149
    .quad 0x03FC1655CFEA923A4
    .quad 0x03FC180DE5B2ACE5C  # 0.136745256915 150
    .quad 0x03FC180DE5B2ACE5C
    .quad 0x03FC19C65A207AC07  # 0.137585357777 151
    .quad 0x03FC19C65A207AC07
    .quad 0x03FC1B7F2D5CBA842  # 0.138426165001 152
    .quad 0x03FC1B7F2D5CBA842
    .quad 0x03FC1D385F90453F2  # 0.139267679777 153
    .quad 0x03FC1D385F90453F2
    .quad 0x03FC1EF1F0E40E6CD  # 0.140109903297 154
    .quad 0x03FC1EF1F0E40E6CD
    .quad 0x03FC20ABE18124098  # 0.140952836755 155
    .quad 0x03FC20ABE18124098
    .quad 0x03FC22663190AEACC  # 0.141796481350 156
    .quad 0x03FC22663190AEACC
    .quad 0x03FC2420E13BF19E3  # 0.142640838281 157
    .quad 0x03FC2420E13BF19E3
    .quad 0x03FC25DBF0AC4AED2  # 0.143485908754 158
    .quad 0x03FC25DBF0AC4AED2
    .quad 0x03FC2797600B3387B  # 0.144331693975 159
    .quad 0x03FC2797600B3387B
    .quad 0x03FC29532F823F525  # 0.145178195155 160
    .quad 0x03FC29532F823F525
    .quad 0x03FC2B0F5F3B1D3EF  # 0.146025413505 161
    .quad 0x03FC2B0F5F3B1D3EF
    .quad 0x03FC2CCBEF5F97653  # 0.146873350243 162
    .quad 0x03FC2CCBEF5F97653
    .quad 0x03FC2E88E01993187  # 0.147722006588 163
    .quad 0x03FC2E88E01993187
    .quad 0x03FC3046319311009  # 0.148571383763 164
    .quad 0x03FC3046319311009
    .quad 0x03FC3203E3F62D328  # 0.149421482992 165
    .quad 0x03FC3203E3F62D328
    .quad 0x03FC33C1F76D1F469  # 0.150272305505 166
    .quad 0x03FC33C1F76D1F469
    .quad 0x03FC35806C223A70F  # 0.151123852534 167
    .quad 0x03FC35806C223A70F
    .quad 0x03FC373F423FED9A1  # 0.151976125313 168
    .quad 0x03FC373F423FED9A1
    .quad 0x03FC38FE79F0C3771  # 0.152829125080 169
    .quad 0x03FC38FE79F0C3771
    .quad 0x03FC3ABE135F62A12  # 0.153682853077 170
    .quad 0x03FC3ABE135F62A12
    .quad 0x03FC3C335E0447D71  # 0.154394850259 171
    .quad 0x03FC3C335E0447D71
    .quad 0x03FC3DF3AB13505F9  # 0.155249916579 172
    .quad 0x03FC3DF3AB13505F9
    .quad 0x03FC3FB45A59928CA  # 0.156105714663 173
    .quad 0x03FC3FB45A59928CA
    .quad 0x03FC41756C0220C81  # 0.156962245765 174
    .quad 0x03FC41756C0220C81
    .quad 0x03FC4336E03829D61  # 0.157819511141 175
    .quad 0x03FC4336E03829D61
    .quad 0x03FC44F8B726F8EFE  # 0.158677512051 176
    .quad 0x03FC44F8B726F8EFE
    .quad 0x03FC46BAF0F9F5DB8  # 0.159536249760 177
    .quad 0x03FC46BAF0F9F5DB8
    .quad 0x03FC48326CD3EC797  # 0.160252428262 178
    .quad 0x03FC48326CD3EC797
    .quad 0x03FC49F55C6502F81  # 0.161112520058 179
    .quad 0x03FC49F55C6502F81
    .quad 0x03FC4BB8AF55DE908  # 0.161973352249 180
    .quad 0x03FC4BB8AF55DE908
    .quad 0x03FC4D7C65D25566D  # 0.162834926111 181
    .quad 0x03FC4D7C65D25566D
    .quad 0x03FC4F4080065AA7F  # 0.163697242922 182
    .quad 0x03FC4F4080065AA7F
    .quad 0x03FC50B98CD30A759  # 0.164416408720 183
    .quad 0x03FC50B98CD30A759
    .quad 0x03FC527E5E4A1B58D  # 0.165280090939 184
    .quad 0x03FC527E5E4A1B58D
    .quad 0x03FC544393F5DF80F  # 0.166144519750 185
    .quad 0x03FC544393F5DF80F
    .quad 0x03FC56092E02BA514  # 0.167009696444 186
    .quad 0x03FC56092E02BA514
    .quad 0x03FC57837B3098F2C  # 0.167731249257 187
    .quad 0x03FC57837B3098F2C
    .quad 0x03FC5949CDB873419  # 0.168597800437 188
    .quad 0x03FC5949CDB873419
    .quad 0x03FC5B10851FC924A  # 0.169465103180 189
    .quad 0x03FC5B10851FC924A
    .quad 0x03FC5C8BC079D8289  # 0.170188430518 190
    .quad 0x03FC5C8BC079D8289
    .quad 0x03FC5E533144C1718  # 0.171057114516 191
    .quad 0x03FC5E533144C1718
    .quad 0x03FC601B076E7A8A8  # 0.171926553783 192
    .quad 0x03FC601B076E7A8A8
    .quad 0x03FC619732215D786  # 0.172651664394 193
    .quad 0x03FC619732215D786
    .quad 0x03FC635FC298F6C77  # 0.173522491735 194
    .quad 0x03FC635FC298F6C77
    .quad 0x03FC6528B8EFA5D16  # 0.174394078077 195
    .quad 0x03FC6528B8EFA5D16
    .quad 0x03FC66A5D42A3AD33  # 0.175120980777 196
    .quad 0x03FC66A5D42A3AD33
    .quad 0x03FC686F85BAD4298  # 0.175993962063 197
    .quad 0x03FC686F85BAD4298
    .quad 0x03FC6A399DABBD383  # 0.176867706111 198
    .quad 0x03FC6A399DABBD383
    .quad 0x03FC6BB7AA9F22C40  # 0.177596409780 199
    .quad 0x03FC6BB7AA9F22C40
    .quad 0x03FC6D827EB7C1E57  # 0.178471555693 200
    .quad 0x03FC6D827EB7C1E57
    .quad 0x03FC6F0128B756AB9  # 0.179201429458 201
    .quad 0x03FC6F0128B756AB9
    .quad 0x03FC70CCB9927BCF6  # 0.180077981742 202
    .quad 0x03FC70CCB9927BCF6
    .quad 0x03FC7298B1A4E32B6  # 0.180955303044 203
    .quad 0x03FC7298B1A4E32B6
    .quad 0x03FC74184F58CC7DC  # 0.181686992547 204
    .quad 0x03FC74184F58CC7DC
    .quad 0x03FC75E5051E74141  # 0.182565727226 205
    .quad 0x03FC75E5051E74141
    .quad 0x03FC77654128F6127  # 0.183298596442 206
    .quad 0x03FC77654128F6127
    .quad 0x03FC7932B53E97639  # 0.184178749058 207
    .quad 0x03FC7932B53E97639
    .quad 0x03FC7AB390229D8FD  # 0.184912801796 208
    .quad 0x03FC7AB390229D8FD
    .quad 0x03FC7C81C325B4A5E  # 0.185794376934 209
    .quad 0x03FC7C81C325B4A5E
    .quad 0x03FC7E033D66CD24A  # 0.186529617023 210
    .quad 0x03FC7E033D66CD24A
    .quad 0x03FC7FD22FF599D4C  # 0.187412619288 211
    .quad 0x03FC7FD22FF599D4C
    .quad 0x03FC81544A17F67C1  # 0.188149050576 212
    .quad 0x03FC81544A17F67C1
    .quad 0x03FC8323FCD17DAC8  # 0.189033484595 213
    .quad 0x03FC8323FCD17DAC8
    .quad 0x03FC84A6B759F512D  # 0.189771110947 214
    .quad 0x03FC84A6B759F512D
    .quad 0x03FC86772ADE0201C  # 0.190656981373 215
    .quad 0x03FC86772ADE0201C
    .quad 0x03FC87FA865210911  # 0.191395806674 216
    .quad 0x03FC87FA865210911
    .quad 0x03FC89CBBB4136201  # 0.192283118179 217
    .quad 0x03FC89CBBB4136201
    .quad 0x03FC8B4FB826FF291  # 0.193023146334 218
    .quad 0x03FC8B4FB826FF291
    .quad 0x03FC8D21AF2299298  # 0.193911903613 219
    .quad 0x03FC8D21AF2299298
    .quad 0x03FC8EA64E00E7FC0  # 0.194653138545 220
    .quad 0x03FC8EA64E00E7FC0
    .quad 0x03FC902B36AB7681D  # 0.195394923313 221
    .quad 0x03FC902B36AB7681D
    .quad 0x03FC91FE49096581E  # 0.196285791969 222
    .quad 0x03FC91FE49096581E
    .quad 0x03FC9383D471B869B  # 0.197028789254 223
    .quad 0x03FC9383D471B869B
    .quad 0x03FC9557AA6B87F65  # 0.197921115309 224
    .quad 0x03FC9557AA6B87F65
    .quad 0x03FC96DDD91A0B959  # 0.198665329082 225
    .quad 0x03FC96DDD91A0B959
    .quad 0x03FC9864522D04491  # 0.199410097121 226
    .quad 0x03FC9864522D04491
    .quad 0x03FC9A3945D1A44B3  # 0.200304551564 227
    .quad 0x03FC9A3945D1A44B3
    .quad 0x03FC9BC062F26FC3B  # 0.201050541900 228
    .quad 0x03FC9BC062F26FC3B
    .quad 0x03FC9D47CAD2C1871  # 0.201797089154 229
    .quad 0x03FC9D47CAD2C1871
    .quad 0x03FC9F1DDD7FE4F8B  # 0.202693682161 230
    .quad 0x03FC9F1DDD7FE4F8B
    .quad 0x03FCA0A5EA371A910  # 0.203441457564 231
    .quad 0x03FCA0A5EA371A910
    .quad 0x03FCA22E42098F498  # 0.204189792554 232
    .quad 0x03FCA22E42098F498
    .quad 0x03FCA405751F6CCE4  # 0.205088534376 233
    .quad 0x03FCA405751F6CCE4
    .quad 0x03FCA58E729348F40  # 0.205838103409 234
    .quad 0x03FCA58E729348F40
    .quad 0x03FCA717BB7EC64A3  # 0.206588234717 235
    .quad 0x03FCA717BB7EC64A3
    .quad 0x03FCA8F010601E5FD  # 0.207489135679 236
    .quad 0x03FCA8F010601E5FD
    .quad 0x03FCAA79FFB8FCD48  # 0.208240506966 237
    .quad 0x03FCAA79FFB8FCD48
    .quad 0x03FCAC043AE68965A  # 0.208992443238 238
    .quad 0x03FCAC043AE68965A
    .quad 0x03FCAD8EC205FB6AD  # 0.209744945343 239
    .quad 0x03FCAD8EC205FB6AD
    .quad 0x03FCAF6895610DBAD  # 0.210648695969 240
    .quad 0x03FCAF6895610DBAD
    .quad 0x03FCB0F3C3FBD65C9  # 0.211402445910 241
    .quad 0x03FCB0F3C3FBD65C9
    .quad 0x03FCB27F3EE674219  # 0.212156764419 242
    .quad 0x03FCB27F3EE674219
    .quad 0x03FCB40B063E65B0F  # 0.212911652354 243
    .quad 0x03FCB40B063E65B0F
    .quad 0x03FCB5E65A8096C88  # 0.213818270730 244
    .quad 0x03FCB5E65A8096C88
    .quad 0x03FCB772CA646760C  # 0.214574414434 245
    .quad 0x03FCB772CA646760C
    .quad 0x03FCB8FF871461198  # 0.215331130323 246
    .quad 0x03FCB8FF871461198
    .quad 0x03FCBA8C90AE4AD19  # 0.216088419265 247
    .quad 0x03FCBA8C90AE4AD19
    .quad 0x03FCBC19E74FFCBDA  # 0.216846282128 248
    .quad 0x03FCBC19E74FFCBDA
    .quad 0x03FCBDF71B83DAE7A  # 0.217756476365 249
    .quad 0x03FCBDF71B83DAE7A
    .quad 0x03FCBF851C067555C  # 0.218515604922 250
    .quad 0x03FCBF851C067555C
    .quad 0x03FCC11369F0CDB3C  # 0.219275310193 251
    .quad 0x03FCC11369F0CDB3C
    .quad 0x03FCC2A205610593E  # 0.220035593055 252
    .quad 0x03FCC2A205610593E
    .quad 0x03FCC430EE755023B  # 0.220796454387 253
    .quad 0x03FCC430EE755023B
    .quad 0x03FCC5C0254BF23A8  # 0.221557895069 254
    .quad 0x03FCC5C0254BF23A8
    .quad 0x03FCC79F9AB632BF1  # 0.222472389875 255
    .quad 0x03FCC79F9AB632BF1
    .quad 0x03FCC92F7D09ABE20  # 0.223235108240 256
    .quad 0x03FCC92F7D09ABE20
    .quad 0x03FCCABFAD80D023D  # 0.223998408788 257
    .quad 0x03FCCABFAD80D023D
    .quad 0x03FCCC502C3A2F1E8  # 0.224762292410 258
    .quad 0x03FCCC502C3A2F1E8
    .quad 0x03FCCDE0F9546A5E7  # 0.225526759995 259
    .quad 0x03FCCDE0F9546A5E7
    .quad 0x03FCCF7214EE356E9  # 0.226291812439 260
    .quad 0x03FCCF7214EE356E9
    .quad 0x03FCD1037F2655E7B  # 0.227057450635 261
    .quad 0x03FCD1037F2655E7B
    .quad 0x03FCD295381BA37E9  # 0.227823675483 262
    .quad 0x03FCD295381BA37E9
    .quad 0x03FCD4273FED08111  # 0.228590487882 263
    .quad 0x03FCD4273FED08111
    .quad 0x03FCD5B996B97FB5F  # 0.229357888733 264
    .quad 0x03FCD5B996B97FB5F
    .quad 0x03FCD74C3CA018C9C  # 0.230125878940 265
    .quad 0x03FCD74C3CA018C9C
    .quad 0x03FCD8DF31BFF3FF2  # 0.230894459410 266
    .quad 0x03FCD8DF31BFF3FF2
    .quad 0x03FCDA727638446A1  # 0.231663631050 267
    .quad 0x03FCDA727638446A1
    .quad 0x03FCDC56CAE452F5B  # 0.232587418645 268
    .quad 0x03FCDC56CAE452F5B
    .quad 0x03FCDDEABE5A3926E  # 0.233357894066 269
    .quad 0x03FCDDEABE5A3926E
    .quad 0x03FCDF7F018CE771F  # 0.234128963578 270
    .quad 0x03FCDF7F018CE771F
    .quad 0x03FCE113949BDEC62  # 0.234900628096 271
    .quad 0x03FCE113949BDEC62
    .quad 0x03FCE2A877A6B2C0F  # 0.235672888541 272
    .quad 0x03FCE2A877A6B2C0F
    .quad 0x03FCE43DAACD09BEC  # 0.236445745833 273
    .quad 0x03FCE43DAACD09BEC
    .quad 0x03FCE5D32E2E9CE87  # 0.237219200895 274
    .quad 0x03FCE5D32E2E9CE87
    .quad 0x03FCE76901EB38427  # 0.237993254653 275
    .quad 0x03FCE76901EB38427
    .quad 0x03FCE8ADE53F76866  # 0.238612929343 276
    .quad 0x03FCE8ADE53F76866
    .quad 0x03FCEA4449F04AAF4  # 0.239388063093 277
    .quad 0x03FCEA4449F04AAF4
    .quad 0x03FCEBDAFF5593E99  # 0.240163798141 278
    .quad 0x03FCEBDAFF5593E99
    .quad 0x03FCED72058F666C5  # 0.240940135421 279
    .quad 0x03FCED72058F666C5
    .quad 0x03FCEF095CBDE9937  # 0.241717075868 280
    .quad 0x03FCEF095CBDE9937
    .quad 0x03FCF0A1050157ED6  # 0.242494620422 281
    .quad 0x03FCF0A1050157ED6
    .quad 0x03FCF238FE79FF4BF  # 0.243272770021 282
    .quad 0x03FCF238FE79FF4BF
    .quad 0x03FCF3D1494840D2F  # 0.244051525609 283
    .quad 0x03FCF3D1494840D2F
    .quad 0x03FCF569E58C91077  # 0.244830888130 284
    .quad 0x03FCF569E58C91077
    .quad 0x03FCF702D36777DF0  # 0.245610858531 285
    .quad 0x03FCF702D36777DF0
    .quad 0x03FCF89C12F990D0C  # 0.246391437760 286
    .quad 0x03FCF89C12F990D0C
    .quad 0x03FCFA35A4638AE2C  # 0.247172626770 287
    .quad 0x03FCFA35A4638AE2C
    .quad 0x03FCFB7D86EEE3B92  # 0.247798017660 288
    .quad 0x03FCFB7D86EEE3B92
    .quad 0x03FCFD17ABFCDB683  # 0.248580306677 289
    .quad 0x03FCFD17ABFCDB683
    .quad 0x03FCFEB2233EA07CB  # 0.249363208150 290
    .quad 0x03FCFEB2233EA07CB
    .quad 0x03FD0026766A9671C  # 0.250146723037 291
    .quad 0x03FD0026766A9671C
    .quad 0x03FD00F40470C7323  # 0.250930852302 292
    .quad 0x03FD00F40470C7323
    .quad 0x03FD01C1BBC2735A3  # 0.251715596908 293
    .quad 0x03FD01C1BBC2735A3
    .quad 0x03FD028F9C7035C1D  # 0.252500957822 294
    .quad 0x03FD028F9C7035C1D
    .quad 0x03FD03346E0106062  # 0.253129690945 295
    .quad 0x03FD03346E0106062
    .quad 0x03FD0402994B4F041  # 0.253916163656 296
    .quad 0x03FD0402994B4F041
    .quad 0x03FD04D0EE20620AF  # 0.254703255393 297
    .quad 0x03FD04D0EE20620AF
    .quad 0x03FD059F6C910034D  # 0.255490967131 298
    .quad 0x03FD059F6C910034D
    .quad 0x03FD066E14ADF4BFD  # 0.256279299848 299
    .quad 0x03FD066E14ADF4BFD
    .quad 0x03FD07138604D5864  # 0.256910413785 300
    .quad 0x03FD07138604D5864
    .quad 0x03FD07E2794F3E8C1  # 0.257699866735 301
    .quad 0x03FD07E2794F3E8C1
    .quad 0x03FD08B196753A125  # 0.258489943414 302
    .quad 0x03FD08B196753A125
    .quad 0x03FD0980DD87BA2DD  # 0.259280644807 303
    .quad 0x03FD0980DD87BA2DD
    .quad 0x03FD0A504E97BB40C  # 0.260071971904 304
    .quad 0x03FD0A504E97BB40C
    .quad 0x03FD0AF660EB9E278  # 0.260705484754 305
    .quad 0x03FD0AF660EB9E278
    .quad 0x03FD0BC61DBBA97CB  # 0.261497940616 306
    .quad 0x03FD0BC61DBBA97CB
    .quad 0x03FD0C9604B8FC51E  # 0.262291024962 307
    .quad 0x03FD0C9604B8FC51E
    .quad 0x03FD0D3C7586CD5E5  # 0.262925945618 308
    .quad 0x03FD0D3C7586CD5E5
    .quad 0x03FD0E0CA89A72D29  # 0.263720163752 309
    .quad 0x03FD0E0CA89A72D29
    .quad 0x03FD0EDD060B78082  # 0.264515013170 310
    .quad 0x03FD0EDD060B78082
    .quad 0x03FD0FAD8DEB1E2C0  # 0.265310494876 311
    .quad 0x03FD0FAD8DEB1E2C0
    .quad 0x03FD10547F9D26ABC  # 0.265947336165 312
    .quad 0x03FD10547F9D26ABC
    .quad 0x03FD1125540925114  # 0.266743958529 313
    .quad 0x03FD1125540925114
    .quad 0x03FD11F653144CB8B  # 0.267541216005 314
    .quad 0x03FD11F653144CB8B
    .quad 0x03FD129DA43F5BE9E  # 0.268179479949 315
    .quad 0x03FD129DA43F5BE9E
    .quad 0x03FD136EF02E8290C  # 0.268977883185 316
    .quad 0x03FD136EF02E8290C
    .quad 0x03FD144066EDAE406  # 0.269776924378 317
    .quad 0x03FD144066EDAE406
    .quad 0x03FD14E817FF359D7  # 0.270416617347 318
    .quad 0x03FD14E817FF359D7
    .quad 0x03FD15B9DBFA9DEC8  # 0.271216809436 319
    .quad 0x03FD15B9DBFA9DEC8
    .quad 0x03FD168BCAF73B3EB  # 0.272017642345 320
    .quad 0x03FD168BCAF73B3EB
    .quad 0x03FD1733DC5D68DE8  # 0.272658770753 321
    .quad 0x03FD1733DC5D68DE8
    .quad 0x03FD180618EF18ADE  # 0.273460759729 322
    .quad 0x03FD180618EF18ADE
    .quad 0x03FD18D880B3826FE  # 0.274263392407 323
    .quad 0x03FD18D880B3826FE
    .quad 0x03FD1980F2DD42B6F  # 0.274905962710 324
    .quad 0x03FD1980F2DD42B6F
    .quad 0x03FD1A53A8902E70B  # 0.275709756661 325
    .quad 0x03FD1A53A8902E70B
    .quad 0x03FD1AFC59297024D  # 0.276353257326 326
    .quad 0x03FD1AFC59297024D
    .quad 0x03FD1BCF5D04AE1EA  # 0.277158215914 327
    .quad 0x03FD1BCF5D04AE1EA
    .quad 0x03FD1CA28C64BAE54  # 0.277963822983 328
    .quad 0x03FD1CA28C64BAE54
    .quad 0x03FD1D4B9E796C245  # 0.278608776246 329
    .quad 0x03FD1D4B9E796C245
    .quad 0x03FD1E1F1C5C3A06C  # 0.279415553216 330
    .quad 0x03FD1E1F1C5C3A06C
    .quad 0x03FD1EC86D5747AAD  # 0.280061443760 331
    .quad 0x03FD1EC86D5747AAD
    .quad 0x03FD1F9C39F74C559  # 0.280869394034 332
    .quad 0x03FD1F9C39F74C559
    .quad 0x03FD2070326F1F789  # 0.281677997620 333
    .quad 0x03FD2070326F1F789
    .quad 0x03FD2119E59F8789C  # 0.282325351583 334
    .quad 0x03FD2119E59F8789C
    .quad 0x03FD21EE2D300381C  # 0.283135133796 335
    .quad 0x03FD21EE2D300381C
    .quad 0x03FD22981FBEF797A  # 0.283783432036 336
    .quad 0x03FD22981FBEF797A
    .quad 0x03FD236CB6A339EED  # 0.284594396317 337
    .quad 0x03FD236CB6A339EED
    .quad 0x03FD2416E8C01F606  # 0.285243641592 338
    .quad 0x03FD2416E8C01F606
    .quad 0x03FD24EBCF3387FF6  # 0.286055791397 339
    .quad 0x03FD24EBCF3387FF6
    .quad 0x03FD2596410DF963A  # 0.286705986479 340
    .quad 0x03FD2596410DF963A
    .quad 0x03FD266B774C2AF55  # 0.287519325279 341
    .quad 0x03FD266B774C2AF55
    .quad 0x03FD27162913F873F  # 0.288170472950 342
    .quad 0x03FD27162913F873F
    .quad 0x03FD27EBAF58D8C9C  # 0.288985004232 343
    .quad 0x03FD27EBAF58D8C9C
    .quad 0x03FD2896A13E086A3  # 0.289637107288 344
    .quad 0x03FD2896A13E086A3
    .quad 0x03FD296C77C5C0E13  # 0.290452834554 345
    .quad 0x03FD296C77C5C0E13
    .quad 0x03FD2A17A9F88EDD2  # 0.291105895801 346
    .quad 0x03FD2A17A9F88EDD2
    .quad 0x03FD2AEDD0FF8CC2C  # 0.291922822568 347
    .quad 0x03FD2AEDD0FF8CC2C
    .quad 0x03FD2B9943B06BD77  # 0.292576844829 348
    .quad 0x03FD2B9943B06BD77
    .quad 0x03FD2C6FBB7360D0E  # 0.293394974630 349
    .quad 0x03FD2C6FBB7360D0E
    .quad 0x03FD2D1B6ED2FA90C  # 0.294049960734 350
    .quad 0x03FD2D1B6ED2FA90C
    .quad 0x03FD2DC73F01B0DD4  # 0.294705376127 351
    .quad 0x03FD2DC73F01B0DD4
    .quad 0x03FD2E9E2BCE12286  # 0.295525249913 352
    .quad 0x03FD2E9E2BCE12286
    .quad 0x03FD2F4A3CF22EDC2  # 0.296181633264 353
    .quad 0x03FD2F4A3CF22EDC2
    .quad 0x03FD30217B1006601  # 0.297002718785 354
    .quad 0x03FD30217B1006601
    .quad 0x03FD30CDCD5ABA762  # 0.297660072959 355
    .quad 0x03FD30CDCD5ABA762
    .quad 0x03FD31A55D07A8590  # 0.298482373803 356
    .quad 0x03FD31A55D07A8590
    .quad 0x03FD3251F0AA5CC1A  # 0.299140701674 357
    .quad 0x03FD3251F0AA5CC1A
    .quad 0x03FD32FEA167A6D70  # 0.299799463226 358
    .quad 0x03FD32FEA167A6D70
    .quad 0x03FD33D6A7509D491  # 0.300623525901 359
    .quad 0x03FD33D6A7509D491
    .quad 0x03FD348399ADA9D94  # 0.301283265328 360
    .quad 0x03FD348399ADA9D94
    .quad 0x03FD3530A9454ADC9  # 0.301943440298 361
    .quad 0x03FD3530A9454ADC9
    .quad 0x03FD360925EC44F5C  # 0.302769272371 362
    .quad 0x03FD360925EC44F5C
    .quad 0x03FD36B6776BE1116  # 0.303430429420 363
    .quad 0x03FD36B6776BE1116
    .quad 0x03FD378F469437FB4  # 0.304257490918 364
    .quad 0x03FD378F469437FB4
    .quad 0x03FD383CDA2E14ECB  # 0.304919632971 365
    .quad 0x03FD383CDA2E14ECB
    .quad 0x03FD38EA8B3924521  # 0.305582213748 366
    .quad 0x03FD38EA8B3924521
    .quad 0x03FD39C3D1FD60E74  # 0.306411057558 367
    .quad 0x03FD39C3D1FD60E74
    .quad 0x03FD3A71C56BB48C7  # 0.307074627589 368
    .quad 0x03FD3A71C56BB48C7
    .quad 0x03FD3B1FD66BC8D10  # 0.307738638238 369
    .quad 0x03FD3B1FD66BC8D10
    .quad 0x03FD3BF995502CB5C  # 0.308569272059 370
    .quad 0x03FD3BF995502CB5C
    .quad 0x03FD3CA7E8FD01DF6  # 0.309234276240 371
    .quad 0x03FD3CA7E8FD01DF6
    .quad 0x03FD3D565A5C5BF11  # 0.309899722945 372
    .quad 0x03FD3D565A5C5BF11
    .quad 0x03FD3E3091E6049FB  # 0.310732154526 373
    .quad 0x03FD3E3091E6049FB
    .quad 0x03FD3EDF463C1683E  # 0.311398599069 374
    .quad 0x03FD3EDF463C1683E
    .quad 0x03FD3F8E1865A82DD  # 0.312065488057 375
    .quad 0x03FD3F8E1865A82DD
    .quad 0x03FD403D086CEA79B  # 0.312732822082 376
    .quad 0x03FD403D086CEA79B
    .quad 0x03FD4117DE854CA15  # 0.313567616354 377
    .quad 0x03FD4117DE854CA15
    .quad 0x03FD41C711E4BA15E  # 0.314235953889 378
    .quad 0x03FD41C711E4BA15E
    .quad 0x03FD427663431B221  # 0.314904738398 379
    .quad 0x03FD427663431B221
    .quad 0x03FD4325D2AAB6F18  # 0.315573970480 380
    .quad 0x03FD4325D2AAB6F18
    .quad 0x03FD44014838E5513  # 0.316411140893 381
    .quad 0x03FD44014838E5513
    .quad 0x03FD44B0FB5AF4F44  # 0.317081382205 382
    .quad 0x03FD44B0FB5AF4F44
    .quad 0x03FD4560CCA7CB3B2  # 0.317752073041 383
    .quad 0x03FD4560CCA7CB3B2
    .quad 0x03FD4610BC29C5E18  # 0.318423214006 384
    .quad 0x03FD4610BC29C5E18
    .quad 0x03FD46ECD216CDCB5  # 0.319262774126 385
    .quad 0x03FD46ECD216CDCB5
    .quad 0x03FD479D05B65CB60  # 0.319934930091 386
    .quad 0x03FD479D05B65CB60
    .quad 0x03FD484D57ACE5A1A  # 0.320607538154 387
    .quad 0x03FD484D57ACE5A1A
    .quad 0x03FD48FDC804DD1CB  # 0.321280598924 388
    .quad 0x03FD48FDC804DD1CB
    .quad 0x03FD49DA7F3BCC420  # 0.322122562432 389
    .quad 0x03FD49DA7F3BCC420
    .quad 0x03FD4A8B341552B09  # 0.322796644021 390
    .quad 0x03FD4A8B341552B09
    .quad 0x03FD4B3C077267E9A  # 0.323471180303 391
    .quad 0x03FD4B3C077267E9A
    .quad 0x03FD4BECF95D97914  # 0.324146171892 392
    .quad 0x03FD4BECF95D97914
    .quad 0x03FD4C9E09E172C3D  # 0.324821619401 393
    .quad 0x03FD4C9E09E172C3D
    .quad 0x03FD4D4F3908901A0  # 0.325497523449 394
    .quad 0x03FD4D4F3908901A0
    .quad 0x03FD4E2CDF1F341C1  # 0.326343046455 395
    .quad 0x03FD4E2CDF1F341C1
    .quad 0x03FD4EDE535C79642  # 0.327019979972 396
    .quad 0x03FD4EDE535C79642
    .quad 0x03FD4F8FE65F90500  # 0.327697372039 397
    .quad 0x03FD4F8FE65F90500
    .quad 0x03FD5041983326F2D  # 0.328375223276 398
    .quad 0x03FD5041983326F2D
    .quad 0x03FD50F368E1F0F02  # 0.329053534308 399
    .quad 0x03FD50F368E1F0F02
    .quad 0x03FD51A55876A77F5  # 0.329732305758 400
    .quad 0x03FD51A55876A77F5
    .quad 0x03FD5283EF743F98B  # 0.330581418486 401
    .quad 0x03FD5283EF743F98B
    .quad 0x03FD533624B59CA35  # 0.331261228165 402
    .quad 0x03FD533624B59CA35
    .quad 0x03FD53E878FFE6EAE  # 0.331941500300 403
    .quad 0x03FD53E878FFE6EAE
    .quad 0x03FD549AEC5DEF880  # 0.332622235521 404
    .quad 0x03FD549AEC5DEF880
    .quad 0x03FD554D7EDA8D3C4  # 0.333303434457 405
    .quad 0x03FD554D7EDA8D3C4
    .quad 0x03FD560030809C759  # 0.333985097742 406
    .quad 0x03FD560030809C759
    .quad 0x03FD56B3015AFF52C  # 0.334667226008 407
    .quad 0x03FD56B3015AFF52C
    .quad 0x03FD5765F1749DA6C  # 0.335349819892 408
    .quad 0x03FD5765F1749DA6C
    .quad 0x03FD581900D864FD7  # 0.336032880027 409
    .quad 0x03FD581900D864FD7
    .quad 0x03FD58CC2F91489F5  # 0.336716407053 410
    .quad 0x03FD58CC2F91489F5
    .quad 0x03FD59AC5618CCE38  # 0.337571473373 411
    .quad 0x03FD59AC5618CCE38
    .quad 0x03FD5A5FCB795780C  # 0.338256053239 412
    .quad 0x03FD5A5FCB795780C
    .quad 0x03FD5B136052BCE39  # 0.338941102075 413
    .quad 0x03FD5B136052BCE39
    .quad 0x03FD5BC714B008E23  # 0.339626620526 414
    .quad 0x03FD5BC714B008E23
    .quad 0x03FD5C7AE89C4D254  # 0.340312609234 415
    .quad 0x03FD5C7AE89C4D254
    .quad 0x03FD5D2EDC22A12BA  # 0.340999068845 416
    .quad 0x03FD5D2EDC22A12BA
    .quad 0x03FD5DE2EF4E224D6  # 0.341686000008 417
    .quad 0x03FD5DE2EF4E224D6
    .quad 0x03FD5E972229F3C15  # 0.342373403369 418
    .quad 0x03FD5E972229F3C15
    .quad 0x03FD5F4B74C13EA04  # 0.343061279578 419
    .quad 0x03FD5F4B74C13EA04
    .quad 0x03FD5FFFE71F31E9A  # 0.343749629287 420
    .quad 0x03FD5FFFE71F31E9A
    .quad 0x03FD60B4794F02875  # 0.344438453147 421
    .quad 0x03FD60B4794F02875
    .quad 0x03FD61692B5BEB520  # 0.345127751813 422
    .quad 0x03FD61692B5BEB520
    .quad 0x03FD621DFD512D14F  # 0.345817525940 423
    .quad 0x03FD621DFD512D14F
    .quad 0x03FD62D2EF3A0E933  # 0.346507776183 424
    .quad 0x03FD62D2EF3A0E933
    .quad 0x03FD63880121DC8AB  # 0.347198503200 425
    .quad 0x03FD63880121DC8AB
    .quad 0x03FD643D3313E9B92  # 0.347889707652 426
    .quad 0x03FD643D3313E9B92
    .quad 0x03FD64F2851B8EE01  # 0.348581390197 427
    .quad 0x03FD64F2851B8EE01
    .quad 0x03FD65A7F7442AC90  # 0.349273551498 428
    .quad 0x03FD65A7F7442AC90
    .quad 0x03FD665D8999224A5  # 0.349966192218 429
    .quad 0x03FD665D8999224A5
    .quad 0x03FD67133C25E04A5  # 0.350659313022 430
    .quad 0x03FD67133C25E04A5
    .quad 0x03FD67C90EF5D5C4C  # 0.351352914576 431
    .quad 0x03FD67C90EF5D5C4C
    .quad 0x03FD687F021479CEE  # 0.352046997547 432
    .quad 0x03FD687F021479CEE
    .quad 0x03FD6935158D499B3  # 0.352741562603 433
    .quad 0x03FD6935158D499B3
    .quad 0x03FD69EB496BC87E5  # 0.353436610416 434
    .quad 0x03FD69EB496BC87E5
    .quad 0x03FD6AA19DBB7FF34  # 0.354132141656 435
    .quad 0x03FD6AA19DBB7FF34
    .quad 0x03FD6B581287FF9FD  # 0.354828156996 436
    .quad 0x03FD6B581287FF9FD
    .quad 0x03FD6C0EA7DCDD591  # 0.355524657112 437
    .quad 0x03FD6C0EA7DCDD591
    .quad 0x03FD6C97AD3CFCFD9  # 0.356047350738 438
    .quad 0x03FD6C97AD3CFCFD9
    .quad 0x03FD6D4E7B9C727EC  # 0.356744700836 439
    .quad 0x03FD6D4E7B9C727EC
    .quad 0x03FD6E056AA4421D6  # 0.357442537571 440
    .quad 0x03FD6E056AA4421D6
    .quad 0x03FD6EBC7A6019066  # 0.358140861621 441
    .quad 0x03FD6EBC7A6019066
    .quad 0x03FD6F73AADBAAAB7  # 0.358839673669 442
    .quad 0x03FD6F73AADBAAAB7
    .quad 0x03FD702AFC22B0C6D  # 0.359538974397 443
    .quad 0x03FD702AFC22B0C6D
    .quad 0x03FD70E26E40EB5FA  # 0.360238764489 444
    .quad 0x03FD70E26E40EB5FA
    .quad 0x03FD719A014220CF5  # 0.360939044629 445
    .quad 0x03FD719A014220CF5
    .quad 0x03FD7251B5321DC54  # 0.361639815506 446
    .quad 0x03FD7251B5321DC54
    .quad 0x03FD73098A1CB54BA  # 0.362341077807 447
    .quad 0x03FD73098A1CB54BA
    .quad 0x03FD73937F783CEBA  # 0.362867347444 448
    .quad 0x03FD73937F783CEBA
    .quad 0x03FD744B8E35E9EDA  # 0.363569471398 449
    .quad 0x03FD744B8E35E9EDA
    .quad 0x03FD7503BE0ED6C66  # 0.364272088676 450
    .quad 0x03FD7503BE0ED6C66
    .quad 0x03FD75BC0F0EEE7DE  # 0.364975199972 451
    .quad 0x03FD75BC0F0EEE7DE
    .quad 0x03FD76748142228C7  # 0.365678805982 452
    .quad 0x03FD76748142228C7
    .quad 0x03FD772D14B46AE00  # 0.366382907402 453
    .quad 0x03FD772D14B46AE00
    .quad 0x03FD77E5C971C5E06  # 0.367087504930 454
    .quad 0x03FD77E5C971C5E06
    .quad 0x03FD787066E04915F  # 0.367616279067 455
    .quad 0x03FD787066E04915F
    .quad 0x03FD792955FDF47A3  # 0.368321746469 456
    .quad 0x03FD792955FDF47A3
    .quad 0x03FD79E26687CFB3D  # 0.369027711906 457
    .quad 0x03FD79E26687CFB3D
    .quad 0x03FD7A9B9889F19E2  # 0.369734176082 458
    .quad 0x03FD7A9B9889F19E2
    .quad 0x03FD7B54EC1077A48  # 0.370441139703 459
    .quad 0x03FD7B54EC1077A48
    .quad 0x03FD7C0E612785C74  # 0.371148603475 460
    .quad 0x03FD7C0E612785C74
    .quad 0x03FD7C998F06FB152  # 0.371679529954 461
    .quad 0x03FD7C998F06FB152
    .quad 0x03FD7D533EF841E8A  # 0.372387870696 462
    .quad 0x03FD7D533EF841E8A
    .quad 0x03FD7E0D109B95F19  # 0.373096713539 463
    .quad 0x03FD7E0D109B95F19
    .quad 0x03FD7EC703FD340AA  # 0.373806059198 464
    .quad 0x03FD7EC703FD340AA
    .quad 0x03FD7F8119295FB9B  # 0.374515908385 465
    .quad 0x03FD7F8119295FB9B
    .quad 0x03FD800CBF3ED1CC2  # 0.375048626146 466
    .quad 0x03FD800CBF3ED1CC2
    .quad 0x03FD80C70FAB0BDF6  # 0.375759358229 467
    .quad 0x03FD80C70FAB0BDF6
    .quad 0x03FD81818203AFC7F  # 0.376470595813 468
    .quad 0x03FD81818203AFC7F
    .quad 0x03FD823C16551A3C3  # 0.377182339615 469
    .quad 0x03FD823C16551A3C3
    .quad 0x03FD82C81BE4DFF4A  # 0.377716480107 470
    .quad 0x03FD82C81BE4DFF4A
    .quad 0x03FD8382EBC7794D1  # 0.378429111528 471
    .quad 0x03FD8382EBC7794D1
    .quad 0x03FD843DDDC4FB137  # 0.379142251156 472
    .quad 0x03FD843DDDC4FB137
    .quad 0x03FD84F8F1E9DB72B  # 0.379855899714 473
    .quad 0x03FD84F8F1E9DB72B
    .quad 0x03FD85855776DCBFB  # 0.380391470556 474
    .quad 0x03FD85855776DCBFB
    .quad 0x03FD8640A77EB3957  # 0.381106011494 475
    .quad 0x03FD8640A77EB3957
    .quad 0x03FD86FC19D05148E  # 0.381821063366 476
    .quad 0x03FD86FC19D05148E
    .quad 0x03FD87B7AE7845C0F  # 0.382536626902 477
    .quad 0x03FD87B7AE7845C0F
    .quad 0x03FD8844748678822  # 0.383073635776 478
    .quad 0x03FD8844748678822
    .quad 0x03FD89004563D3DFD  # 0.383790096491 479
    .quad 0x03FD89004563D3DFD
    .quad 0x03FD89BC38BA356B4  # 0.384507070890 480
    .quad 0x03FD89BC38BA356B4
    .quad 0x03FD8A4945E20894E  # 0.385045139237 481
    .quad 0x03FD8A4945E20894E
    .quad 0x03FD8B0575AAB1FC5  # 0.385763014358 482
    .quad 0x03FD8B0575AAB1FC5
    .quad 0x03FD8BC1C80F45A32  # 0.386481405193 483
    .quad 0x03FD8BC1C80F45A32
    .quad 0x03FD8C7E3D1C80B2F  # 0.387200312485 484
    .quad 0x03FD8C7E3D1C80B2F
    .quad 0x03FD8D0BABACC89EE  # 0.387739832326 485
    .quad 0x03FD8D0BABACC89EE
    .quad 0x03FD8DC85D7FE5013  # 0.388459645206 486
    .quad 0x03FD8DC85D7FE5013
    .quad 0x03FD8E85321ED5598  # 0.389179976589 487
    .quad 0x03FD8E85321ED5598
    .quad 0x03FD8F12E873862C7  # 0.389720565845 488
    .quad 0x03FD8F12E873862C7
    .quad 0x03FD8FCFFA1614AA0  # 0.390441806410 489
    .quad 0x03FD8FCFFA1614AA0
    .quad 0x03FD908D2EA7D9511  # 0.391163567538 490
    .quad 0x03FD908D2EA7D9511
    .quad 0x03FD911B2D09ED9D6  # 0.391705230456 491
    .quad 0x03FD911B2D09ED9D6
    .quad 0x03FD91D89EDD6B7FF  # 0.392427904381 492
    .quad 0x03FD91D89EDD6B7FF
    .quad 0x03FD929633C3B7D3E  # 0.393151100941 493
    .quad 0x03FD929633C3B7D3E
    .quad 0x03FD93247A7C99B52  # 0.393693841796 494
    .quad 0x03FD93247A7C99B52
    .quad 0x03FD93E24CE3195E8  # 0.394417954789 495
    .quad 0x03FD93E24CE3195E8
    .quad 0x03FD9470C1CB1962E  # 0.394961383840 496
    .quad 0x03FD9470C1CB1962E
    .quad 0x03FD952ED1D9C0435  # 0.395686415592 497
    .quad 0x03FD952ED1D9C0435
    .quad 0x03FD95ED0535EA5D9  # 0.396411973396 498
    .quad 0x03FD95ED0535EA5D9
    .quad 0x03FD967BC2EDCCE17  # 0.396956487431 499
    .quad 0x03FD967BC2EDCCE17
    .quad 0x03FD973A3431356AE  # 0.397682967666 500
    .quad 0x03FD973A3431356AE
    .quad 0x03FD97F8C8E64A1C7  # 0.398409976059 501
    .quad 0x03FD97F8C8E64A1C7
    .quad 0x03FD9887CFB8A3932  # 0.398955579419 502
    .quad 0x03FD9887CFB8A3932
    .quad 0x03FD9946A2946EF3C  # 0.399683513937 503
    .quad 0x03FD9946A2946EF3C
    .quad 0x03FD99D5D8130607C  # 0.400229812776 504
    .quad 0x03FD99D5D8130607C
    .quad 0x03FD9A94E93E1EC37  # 0.400958675782 505
    .quad 0x03FD9A94E93E1EC37
    .quad 0x03FD9B244D87735E8  # 0.401505671875 506
    .quad 0x03FD9B244D87735E8
    .quad 0x03FD9BE39D2A97F0B  # 0.402235465741 507
    .quad 0x03FD9BE39D2A97F0B
    .quad 0x03FD9CA3109266E23  # 0.402965792595 508
    .quad 0x03FD9CA3109266E23
    .quad 0x03FD9D32BEA15ED3A  # 0.403513887977 509
    .quad 0x03FD9D32BEA15ED3A
    .quad 0x03FD9DF270C1914A8  # 0.404245149435 510
    .quad 0x03FD9DF270C1914A8
    .quad 0x03FD9E824DEA3E135  # 0.404793946669 511
    .quad 0x03FD9E824DEA3E135
    .quad 0x03FD9F423EEBF9DA1  # 0.405526145127 512
    .quad 0x03FD9F423EEBF9DA1
    .quad 0x03FD9FD24B4D47012  # 0.406075646011 513
    .quad 0x03FD9FD24B4D47012
    .quad 0x03FDA0927B59DA6E2  # 0.406808783874 514
    .quad 0x03FDA0927B59DA6E2
    .quad 0x03FDA152CF7F3B46D  # 0.407542459622 515
    .quad 0x03FDA152CF7F3B46D
    .quad 0x03FDA1E32653B420E  # 0.408093069896 516
    .quad 0x03FDA1E32653B420E
    .quad 0x03FDA2A3B9C527DB1  # 0.408827688845 517
    .quad 0x03FDA2A3B9C527DB1
    .quad 0x03FDA33440224FA79  # 0.409379007429 518
    .quad 0x03FDA33440224FA79
    .quad 0x03FDA3F513098DD09  # 0.410114572008 519
    .quad 0x03FDA3F513098DD09
    .quad 0x03FDA485C90EBDB0C  # 0.410666600728 520
    .quad 0x03FDA485C90EBDB0C
    .quad 0x03FDA546DB95A721A  # 0.411403113374 521
    .quad 0x03FDA546DB95A721A
    .quad 0x03FDA5D7C16257437  # 0.411955854060 522
    .quad 0x03FDA5D7C16257437
    .quad 0x03FDA69913B2F6572  # 0.412693317221 523
    .quad 0x03FDA69913B2F6572
    .quad 0x03FDA72A2966BE1EA  # 0.413246771713 524
    .quad 0x03FDA72A2966BE1EA
    .quad 0x03FDA7EBBBAB46E8B  # 0.413985187844 525
    .quad 0x03FDA7EBBBAB46E8B
    .quad 0x03FDA87D0165DD199  # 0.414539357989 526
    .quad 0x03FDA87D0165DD199
    .quad 0x03FDA93ED3C8AD9E3  # 0.415278729556 527
    .quad 0x03FDA93ED3C8AD9E3
    .quad 0x03FDA9D049A9E884A  # 0.415833617206 528
    .quad 0x03FDA9D049A9E884A
    .quad 0x03FDAA925C5588EFA  # 0.416573946686 529
    .quad 0x03FDAA925C5588EFA
    .quad 0x03FDAB24027D5E8AF  # 0.417129553701 530
    .quad 0x03FDAB24027D5E8AF
    .quad 0x03FDABE6559C8167C  # 0.417870843580 531
    .quad 0x03FDABE6559C8167C
    .quad 0x03FDAC782C2B07944  # 0.418427171828 532
    .quad 0x03FDAC782C2B07944
    .quad 0x03FDAD3ABFE88A06E  # 0.419169424599 533
    .quad 0x03FDAD3ABFE88A06E
    .quad 0x03FDADCCC6FDF6A80  # 0.419726475955 534
    .quad 0x03FDADCCC6FDF6A80
    .quad 0x03FDAE5EE2E961227  # 0.420283837790 535
    .quad 0x03FDAE5EE2E961227
    .quad 0x03FDAF21D34189D0A  # 0.421027470470 536
    .quad 0x03FDAF21D34189D0A
    .quad 0x03FDAFB41FE2167B4  # 0.421585558104 537
    .quad 0x03FDAFB41FE2167B4
    .quad 0x03FDB07751416A7F3  # 0.422330159776 538
    .quad 0x03FDB07751416A7F3
    .quad 0x03FDB109CEB79DB8A  # 0.422888975102 539
    .quad 0x03FDB109CEB79DB8A
    .quad 0x03FDB1CD41498DF12  # 0.423634548296 540
    .quad 0x03FDB1CD41498DF12
    .quad 0x03FDB25FEFB60CB2E  # 0.424194093214 541
    .quad 0x03FDB25FEFB60CB2E
    .quad 0x03FDB323A3A63594A  # 0.424940640468 542
    .quad 0x03FDB323A3A63594A
    .quad 0x03FDB3B68329C59E9  # 0.425500916886 543
    .quad 0x03FDB3B68329C59E9
    .quad 0x03FDB44977C148F1A  # 0.426061507389 544
    .quad 0x03FDB44977C148F1A
    .quad 0x03FDB50D895F7773A  # 0.426809450580 545
    .quad 0x03FDB50D895F7773A
    .quad 0x03FDB5A0AF3D169CD  # 0.427370775322 546
    .quad 0x03FDB5A0AF3D169CD
    .quad 0x03FDB66502A41E541  # 0.428119698779 547
    .quad 0x03FDB66502A41E541
    .quad 0x03FDB6F859E8EF639  # 0.428681759684 548
    .quad 0x03FDB6F859E8EF639
    .quad 0x03FDB78BC664238C0  # 0.429244136679 549
    .quad 0x03FDB78BC664238C0
    .quad 0x03FDB85078123E586  # 0.429994464983 550
    .quad 0x03FDB85078123E586
    .quad 0x03FDB8E41624226C5  # 0.430557580905 551
    .quad 0x03FDB8E41624226C5
    .quad 0x03FDB9A90A06BCB3D  # 0.431308895742 552
    .quad 0x03FDB9A90A06BCB3D
    .quad 0x03FDBA3CD9D0B81BD  # 0.431872752537 553
    .quad 0x03FDBA3CD9D0B81BD
    .quad 0x03FDBAD0BEF3DB164  # 0.432436927446 554
    .quad 0x03FDBAD0BEF3DB164
    .quad 0x03FDBB9611B80E2FC  # 0.433189656123 555
    .quad 0x03FDBB9611B80E2FC
    .quad 0x03FDBC2A28C33B75D  # 0.433754574696 556
    .quad 0x03FDBC2A28C33B75D
    .quad 0x03FDBCBE553C2BDDF  # 0.434319812582 557
    .quad 0x03FDBCBE553C2BDDF
    .quad 0x03FDBD84073D8EC2B  # 0.435073960430 558
    .quad 0x03FDBD84073D8EC2B
    .quad 0x03FDBE1865CEC1EC9  # 0.435639944787 559
    .quad 0x03FDBE1865CEC1EC9
    .quad 0x03FDBEACD9E271AD1  # 0.436206249662 560
    .quad 0x03FDBEACD9E271AD1
    .quad 0x03FDBF72EB7D20355  # 0.436961822044 561
    .quad 0x03FDBF72EB7D20355
    .quad 0x03FDC00791D99132B  # 0.437528876213 562
    .quad 0x03FDC00791D99132B
    .quad 0x03FDC09C4DCD565AB  # 0.438096252115 563
    .quad 0x03FDC09C4DCD565AB
    .quad 0x03FDC162BF5DF23E4  # 0.438853254422 564
    .quad 0x03FDC162BF5DF23E4
    .quad 0x03FDC1F7ADCB3DAB0  # 0.439421382456 565
    .quad 0x03FDC1F7ADCB3DAB0
    .quad 0x03FDC28CB1E4D32FD  # 0.439989833442 566
    .quad 0x03FDC28CB1E4D32FD
    .quad 0x03FDC35383C8850B0  # 0.440748271097 567
    .quad 0x03FDC35383C8850B0
    .quad 0x03FDC3E8BA8CACF27  # 0.441317477070 568
    .quad 0x03FDC3E8BA8CACF27
    .quad 0x03FDC47E071233744  # 0.441887007223 569
    .quad 0x03FDC47E071233744
    .quad 0x03FDC54539A6ABCD2  # 0.442646885679 570
    .quad 0x03FDC54539A6ABCD2
    .quad 0x03FDC5DAB908186FF  # 0.443217173690 571
    .quad 0x03FDC5DAB908186FF
    .quad 0x03FDC6704E4016FF7  # 0.443787787115 572
    .quad 0x03FDC6704E4016FF7
    .quad 0x03FDC737E1E38F4FB  # 0.444549111857 573
    .quad 0x03FDC737E1E38F4FB
    .quad 0x03FDC7CDAA290FEAD  # 0.445120486027 574
    .quad 0x03FDC7CDAA290FEAD
    .quad 0x03FDC863885A74D16  # 0.445692186852 575
    .quad 0x03FDC863885A74D16
    .quad 0x03FDC8F97C7E299DB  # 0.446264214707 576
    .quad 0x03FDC8F97C7E299DB
    .quad 0x03FDC9C18EDC7C26B  # 0.447027427871 577
    .quad 0x03FDC9C18EDC7C26B
    .quad 0x03FDCA57B64E9DB05  # 0.447600220249 578
    .quad 0x03FDCA57B64E9DB05
    .quad 0x03FDCAEDF3C88A364  # 0.448173340907 579
    .quad 0x03FDCAEDF3C88A364
    .quad 0x03FDCB844750B9995  # 0.448746790220 580
    .quad 0x03FDCB844750B9995
    .quad 0x03FDCC4CD90B3ECE5  # 0.449511901199 581
    .quad 0x03FDCC4CD90B3ECE5
    .quad 0x03FDCCE3602341C10  # 0.450086118843 582
    .quad 0x03FDCCE3602341C10
    .quad 0x03FDCD79FD5F2BC77  # 0.450660666403 583
    .quad 0x03FDCD79FD5F2BC77
    .quad 0x03FDCE10B0C581284  # 0.451235544257 584
    .quad 0x03FDCE10B0C581284
    .quad 0x03FDCED9C27EC6607  # 0.452002562511 585
    .quad 0x03FDCED9C27EC6607
    .quad 0x03FDCF70A9B6D3810  # 0.452578212532 586
    .quad 0x03FDCF70A9B6D3810
    .quad 0x03FDD007A72F19BBC  # 0.453154194116 587
    .quad 0x03FDD007A72F19BBC
    .quad 0x03FDD09EBAEE29DD8  # 0.453730507647 588
    .quad 0x03FDD09EBAEE29DD8
    .quad 0x03FDD1684D49F46AE  # 0.454499442710 589
    .quad 0x03FDD1684D49F46AE
    .quad 0x03FDD1FF951D1F1B3  # 0.455076532271 590
    .quad 0x03FDD1FF951D1F1B3
    .quad 0x03FDD296F34D0B65C  # 0.455653955057 591
    .quad 0x03FDD296F34D0B65C
    .quad 0x03FDD32E67E056BD5  # 0.456231711452 592
    .quad 0x03FDD32E67E056BD5
    .quad 0x03FDD3C5F2DDA1840  # 0.456809801843 593
    .quad 0x03FDD3C5F2DDA1840
    .quad 0x03FDD490246DEFA6A  # 0.457581109247 594
    .quad 0x03FDD490246DEFA6A
    .quad 0x03FDD527E3D1B95FC  # 0.458159980465 595
    .quad 0x03FDD527E3D1B95FC
    .quad 0x03FDD5BFB9B5AE71F  # 0.458739186968 596
    .quad 0x03FDD5BFB9B5AE71F
    .quad 0x03FDD657A6207C0DB  # 0.459318729146 597
    .quad 0x03FDD657A6207C0DB
    .quad 0x03FDD6EFA918D25CE  # 0.459898607388 598
    .quad 0x03FDD6EFA918D25CE
    .quad 0x03FDD7BA7AD9E7DA1  # 0.460672301817 599
    .quad 0x03FDD7BA7AD9E7DA1
    .quad 0x03FDD852B28BE5A0F  # 0.461252965726 600
    .quad 0x03FDD852B28BE5A0F
    .quad 0x03FDD8EB00E1CCE14  # 0.461833967001 601
    .quad 0x03FDD8EB00E1CCE14
    .quad 0x03FDD98365E25ABB9  # 0.462415306035 602
    .quad 0x03FDD98365E25ABB9
    .quad 0x03FDDA1BE1944F538  # 0.462996983220 603
    .quad 0x03FDDA1BE1944F538
    .quad 0x03FDDAE75484C9615  # 0.463773079495 604
    .quad 0x03FDDAE75484C9615
    .quad 0x03FDDB8005445488B  # 0.464355547233 605
    .quad 0x03FDDB8005445488B
    .quad 0x03FDDC18CCCBDCB83  # 0.464938354438 606
    .quad 0x03FDDC18CCCBDCB83
    .quad 0x03FDDCB1AB222F33D  # 0.465521501504 607
    .quad 0x03FDDCB1AB222F33D
    .quad 0x03FDDD4AA04E1C4B7  # 0.466104988830 608
    .quad 0x03FDDD4AA04E1C4B7
    .quad 0x03FDDDE3AC56775D2  # 0.466688816812 609
    .quad 0x03FDDDE3AC56775D2
    .quad 0x03FDDE7CCF4216D6E  # 0.467272985848 610
    .quad 0x03FDDE7CCF4216D6E
    .quad 0x03FDDF492177D7BBC  # 0.468052409114 611
    .quad 0x03FDDF492177D7BBC
    .quad 0x03FDDFE279E5BF4EE  # 0.468637375496 612
    .quad 0x03FDDFE279E5BF4EE
    .quad 0x03FDE07BE94DCC439  # 0.469222684263 613
    .quad 0x03FDE07BE94DCC439
    .quad 0x03FDE1156FB6E2626  # 0.469808335817 614
    .quad 0x03FDE1156FB6E2626
    .quad 0x03FDE1AF0D27E88D7  # 0.470394330560 615
    .quad 0x03FDE1AF0D27E88D7
    .quad 0x03FDE248C1A7C8C26  # 0.470980668894 616
    .quad 0x03FDE248C1A7C8C26
    .quad 0x03FDE2E28D3D701CC  # 0.471567351222 617
    .quad 0x03FDE2E28D3D701CC
    .quad 0x03FDE37C6FEFCED73  # 0.472154377948 618
    .quad 0x03FDE37C6FEFCED73
    .quad 0x03FDE449C232C39D8  # 0.472937616681 619
    .quad 0x03FDE449C232C39D8
    .quad 0x03FDE4E3DAEDDB5F6  # 0.473525448578 620
    .quad 0x03FDE4E3DAEDDB5F6
    .quad 0x03FDE57E0ADCE1EA5  # 0.474113626224 621
    .quad 0x03FDE57E0ADCE1EA5
    .quad 0x03FDE6185206D516F  # 0.474702150027 622
    .quad 0x03FDE6185206D516F
    .quad 0x03FDE6B2B072B5E6F  # 0.475291020395 623
    .quad 0x03FDE6B2B072B5E6F
    .quad 0x03FDE74D26278887A  # 0.475880237735 624
    .quad 0x03FDE74D26278887A
    .quad 0x03FDE7E7B32C5453F  # 0.476469802457 625
    .quad 0x03FDE7E7B32C5453F
    .quad 0x03FDE882578823D52  # 0.477059714970 626
    .quad 0x03FDE882578823D52
    .quad 0x03FDE91D134204C67  # 0.477649975686 627
    .quad 0x03FDE91D134204C67
    .quad 0x03FDE9B7E6610815A  # 0.478240585015 628
    .quad 0x03FDE9B7E6610815A
    .quad 0x03FDEA52D0EC41E5E  # 0.478831543369 629
    .quad 0x03FDEA52D0EC41E5E
    .quad 0x03FDEB218376ECFC0  # 0.479620031484 630
    .quad 0x03FDEB218376ECFC0
    .quad 0x03FDEBBCA4C4E9E87  # 0.480211805838 631
    .quad 0x03FDEBBCA4C4E9E87
    .quad 0x03FDEC57DD96CD0CB  # 0.480803930597 632
    .quad 0x03FDEC57DD96CD0CB
    .quad 0x03FDECF32DF3B887D  # 0.481396406174 633
    .quad 0x03FDECF32DF3B887D
    .quad 0x03FDED8E95E2D1B88  # 0.481989232987 634
    .quad 0x03FDED8E95E2D1B88
    .quad 0x03FDEE2A156B413E5  # 0.482582411453 635
    .quad 0x03FDEE2A156B413E5
    .quad 0x03FDEEC5AC9432FCB  # 0.483175941987 636
    .quad 0x03FDEEC5AC9432FCB
    .quad 0x03FDEF615B64D61C7  # 0.483769825010 637
    .quad 0x03FDEF615B64D61C7
    .quad 0x03FDEFFD21E45D0D1  # 0.484364060939 638
    .quad 0x03FDEFFD21E45D0D1
    .quad 0x03FDF0990019FD887  # 0.484958650194 639
    .quad 0x03FDF0990019FD887
    .quad 0x03FDF134F60CF092D  # 0.485553593197 640
    .quad 0x03FDF134F60CF092D
    .quad 0x03FDF1D103C4727E4  # 0.486148890367 641
    .quad 0x03FDF1D103C4727E4
    .quad 0x03FDF26D2947C2EC5  # 0.486744542127 642
    .quad 0x03FDF26D2947C2EC5
    .quad 0x03FDF309669E24CF9  # 0.487340548899 643
    .quad 0x03FDF309669E24CF9
    .quad 0x03FDF3A5BBCEDE6E1  # 0.487936911107 644
    .quad 0x03FDF3A5BBCEDE6E1
    .quad 0x03FDF44228E13963A  # 0.488533629176 645
    .quad 0x03FDF44228E13963A
    .quad 0x03FDF4DEADDC82A35  # 0.489130703529 646
    .quad 0x03FDF4DEADDC82A35
    .quad 0x03FDF57B4AC80A79A  # 0.489728134594 647
    .quad 0x03FDF57B4AC80A79A
    .quad 0x03FDF617FFAB248ED  # 0.490325922795 648
    .quad 0x03FDF617FFAB248ED
    .quad 0x03FDF6B4CC8D27E87  # 0.490924068561 649
    .quad 0x03FDF6B4CC8D27E87
    .quad 0x03FDF751B1756EEC8  # 0.491522572320 650
    .quad 0x03FDF751B1756EEC8
    .quad 0x03FDF7EEAE6B5761C  # 0.492121434499 651
    .quad 0x03FDF7EEAE6B5761C
    .quad 0x03FDF88BC3764273B  # 0.492720655530 652
    .quad 0x03FDF88BC3764273B
    .quad 0x03FDF928F09D94B32  # 0.493320235842 653
    .quad 0x03FDF928F09D94B32
    .quad 0x03FDF9C635E8B6192  # 0.493920175866 654
    .quad 0x03FDF9C635E8B6192
    .quad 0x03FDFA63935F1208C  # 0.494520476034 655
    .quad 0x03FDFA63935F1208C
    .quad 0x03FDFB0109081751A  # 0.495121136779 656
    .quad 0x03FDFB0109081751A
    .quad 0x03FDFB9E96EB38311  # 0.495722158534 657
    .quad 0x03FDFB9E96EB38311
    .quad 0x03FDFC3C3D0FEA555  # 0.496323541733 658
    .quad 0x03FDFC3C3D0FEA555
    .quad 0x03FDFCD9FB7DA6DEF  # 0.496925286812 659
    .quad 0x03FDFCD9FB7DA6DEF
    .quad 0x03FDFD77D23BEA634  # 0.497527394206 660
    .quad 0x03FDFD77D23BEA634
    .quad 0x03FDFE15C15234EE2  # 0.498129864352 661
    .quad 0x03FDFE15C15234EE2
    .quad 0x03FDFEB3C8C80A04E  # 0.498732697687 662
    .quad 0x03FDFEB3C8C80A04E
    .quad 0x03FDFF51E8A4F0A74  # 0.499335894649 663
    .quad 0x03FDFF51E8A4F0A74
    .quad 0x03FDFFF020F07352E  # 0.499939455677 664
    .quad 0x03FDFFF020F07352E
    .quad 0x03FE004738D910023  # 0.500543381211 665
    .quad 0x03FE004738D910023
    .quad 0x03FE00966D78C41CF  # 0.501147671692 666
    .quad 0x03FE00966D78C41CF
    .quad 0x03FE00E5AE5B207AB  # 0.501752327560 667
    .quad 0x03FE00E5AE5B207AB
    .quad 0x03FE011A8B18F0ED6  # 0.502155634684 668
    .quad 0x03FE011A8B18F0ED6
    .quad 0x03FE0169E072D7311  # 0.502760900515 669
    .quad 0x03FE0169E072D7311
    .quad 0x03FE01B942198A5A1  # 0.503366532915 670
    .quad 0x03FE01B942198A5A1
    .quad 0x03FE0208B010DB642  # 0.503972532327 671
    .quad 0x03FE0208B010DB642
    .quad 0x03FE02582A5C9D122  # 0.504578899198 672
    .quad 0x03FE02582A5C9D122
    .quad 0x03FE02A7B100A3EF0  # 0.505185633972 673
    .quad 0x03FE02A7B100A3EF0
    .quad 0x03FE02F74400C64EA  # 0.505792737097 674
    .quad 0x03FE02F74400C64EA
    .quad 0x03FE0346E360DC4F9  # 0.506400209020 675
    .quad 0x03FE0346E360DC4F9
    .quad 0x03FE03968F24BFDB6  # 0.507008050190 676
    .quad 0x03FE03968F24BFDB6
    .quad 0x03FE03E647504CA89  # 0.507616261055 677
    .quad 0x03FE03E647504CA89
    .quad 0x03FE04360BE7603AE  # 0.508224842066 678
    .quad 0x03FE04360BE7603AE
    .quad 0x03FE046B4089BE0FD  # 0.508630768599 679
    .quad 0x03FE046B4089BE0FD
    .quad 0x03FE04BB19DCA36B3  # 0.509239967521 680
    .quad 0x03FE04BB19DCA36B3
    .quad 0x03FE050AFFA5671A5  # 0.509849537793 681
    .quad 0x03FE050AFFA5671A5
    .quad 0x03FE055AF1E7ED47B  # 0.510459479867 682
    .quad 0x03FE055AF1E7ED47B
    .quad 0x03FE05AAF0A81BF04  # 0.511069794198 683
    .quad 0x03FE05AAF0A81BF04
    .quad 0x03FE05FAFBE9DAE58  # 0.511680481240 684
    .quad 0x03FE05FAFBE9DAE58
    .quad 0x03FE064B13B113CDD  # 0.512291541448 685
    .quad 0x03FE064B13B113CDD
    .quad 0x03FE069B3801B2263  # 0.512902975280 686
    .quad 0x03FE069B3801B2263
    .quad 0x03FE06D0AC85B63A2  # 0.513310805628 687
    .quad 0x03FE06D0AC85B63A2
    .quad 0x03FE0720E5C40DF1D  # 0.513922863181 688
    .quad 0x03FE0720E5C40DF1D
    .quad 0x03FE07712B9648153  # 0.514535295577 689
    .quad 0x03FE07712B9648153
    .quad 0x03FE07C17E0056E7C  # 0.515148103277 690
    .quad 0x03FE07C17E0056E7C
    .quad 0x03FE0811DD062E889  # 0.515761286740 691
    .quad 0x03FE0811DD062E889
    .quad 0x03FE086248ABC4F3B  # 0.516374846428 692
    .quad 0x03FE086248ABC4F3B
    .quad 0x03FE08B2C0F512033  # 0.516988782802 693
    .quad 0x03FE08B2C0F512033
    .quad 0x03FE08E86D82DA3EE  # 0.517398283218 694
    .quad 0x03FE08E86D82DA3EE
    .quad 0x03FE0938FAE5D8E9B  # 0.518012848432 695
    .quad 0x03FE0938FAE5D8E9B
    .quad 0x03FE098994F72C539  # 0.518627791569 696
    .quad 0x03FE098994F72C539
    .quad 0x03FE09DA3BBAD339C  # 0.519243113094 697
    .quad 0x03FE09DA3BBAD339C
    .quad 0x03FE0A2AEF34CE3D1  # 0.519858813473 698
    .quad 0x03FE0A2AEF34CE3D1
    .quad 0x03FE0A7BAF691FE34  # 0.520474893172 699
    .quad 0x03FE0A7BAF691FE34
    .quad 0x03FE0AB18BF5823C3  # 0.520885823936 700
    .quad 0x03FE0AB18BF5823C3
    .quad 0x03FE0B02616952989  # 0.521502536876 701
    .quad 0x03FE0B02616952989
    .quad 0x03FE0B5343A234476  # 0.522119630385 702
    .quad 0x03FE0B5343A234476
    .quad 0x03FE0BA432A430CA2  # 0.522737104934 703
    .quad 0x03FE0BA432A430CA2
    .quad 0x03FE0BF52E73538CE  # 0.523354960993 704
    .quad 0x03FE0BF52E73538CE
    .quad 0x03FE0C463713A9E6F  # 0.523973199034 705
    .quad 0x03FE0C463713A9E6F
    .quad 0x03FE0C7C43F4C861E  # 0.524385570174 706
    .quad 0x03FE0C7C43F4C861E
    .quad 0x03FE0CCD61FAD07D2  # 0.525004445903 707
    .quad 0x03FE0CCD61FAD07D2
    .quad 0x03FE0D1E8CDCE3DB6  # 0.525623704876 708
    .quad 0x03FE0D1E8CDCE3DB6
    .quad 0x03FE0D6FC49F16E93  # 0.526243347569 709
    .quad 0x03FE0D6FC49F16E93
    .quad 0x03FE0DC109458004A  # 0.526863374456 710
    .quad 0x03FE0DC109458004A
    .quad 0x03FE0DF73E353F0ED  # 0.527276939392 711
    .quad 0x03FE0DF73E353F0ED
    .quad 0x03FE0E4898611CCE1  # 0.527897607665 712
    .quad 0x03FE0E4898611CCE1
    .quad 0x03FE0E99FF7C20738  # 0.528518661406 713
    .quad 0x03FE0E99FF7C20738
    .quad 0x03FE0EEB738A67874  # 0.529140101094 714
    .quad 0x03FE0EEB738A67874
    .quad 0x03FE0F21C81D1ADC3  # 0.529554608872 715
    .quad 0x03FE0F21C81D1ADC3
    .quad 0x03FE0F7351C9FCD7F  # 0.530176692874 716
    .quad 0x03FE0F7351C9FCD7F
    .quad 0x03FE0FC4E875254C1  # 0.530799164104 717
    .quad 0x03FE0FC4E875254C1
    .quad 0x03FE10168C22B8FB9  # 0.531422023047 718
    .quad 0x03FE10168C22B8FB9
    .quad 0x03FE10683CD6DEA54  # 0.532045270185 719
    .quad 0x03FE10683CD6DEA54
    .quad 0x03FE109EB9E2E4C97  # 0.532460984179 720
    .quad 0x03FE109EB9E2E4C97
    .quad 0x03FE10F08055E7785  # 0.533084879385 721
    .quad 0x03FE10F08055E7785
    .quad 0x03FE114253DA97DA0  # 0.533709164079 722
    .quad 0x03FE114253DA97DA0
    .quad 0x03FE1194347523FDC  # 0.534333838748 723
    .quad 0x03FE1194347523FDC
    .quad 0x03FE11CAD1789B0F8  # 0.534750505421 724
    .quad 0x03FE11CAD1789B0F8
    .quad 0x03FE121CC7EB8F7E6  # 0.535375831132 725
    .quad 0x03FE121CC7EB8F7E6
    .quad 0x03FE126ECB7F8F007  # 0.536001548120 726
    .quad 0x03FE126ECB7F8F007
    .quad 0x03FE12A57FDA37091  # 0.536418910396 727
    .quad 0x03FE12A57FDA37091
    .quad 0x03FE12F799594EFBC  # 0.537045280601 728
    .quad 0x03FE12F799594EFBC
    .quad 0x03FE1349C004AFB00  # 0.537672043392 729
    .quad 0x03FE1349C004AFB00
    .quad 0x03FE139BF3E094003  # 0.538299199261 730
    .quad 0x03FE139BF3E094003
    .quad 0x03FE13D2C873C5E13  # 0.538717521794 731
    .quad 0x03FE13D2C873C5E13
    .quad 0x03FE142512549C16C  # 0.539345333889 732
    .quad 0x03FE142512549C16C
    .quad 0x03FE14776971477F1  # 0.539973540381 733
    .quad 0x03FE14776971477F1
    .quad 0x03FE14C9CDCE0A74D  # 0.540602141763 734
    .quad 0x03FE14C9CDCE0A74D
    .quad 0x03FE1500C2BFD1561  # 0.541021428981 735
    .quad 0x03FE1500C2BFD1561
    .quad 0x03FE15533D3B8D7B3  # 0.541650689621 736
    .quad 0x03FE15533D3B8D7B3
    .quad 0x03FE15A5C502C6DC5  # 0.542280346478 737
    .quad 0x03FE15A5C502C6DC5
    .quad 0x03FE15DCD1973457B  # 0.542700338085 738
    .quad 0x03FE15DCD1973457B
    .quad 0x03FE162F6F9071F76  # 0.543330656416 739
    .quad 0x03FE162F6F9071F76
    .quad 0x03FE16821AE0A13C6  # 0.543961372300 740
    .quad 0x03FE16821AE0A13C6
    .quad 0x03FE16B93F2C12808  # 0.544382070665 741
    .quad 0x03FE16B93F2C12808
    .quad 0x03FE170C00C169B51  # 0.545013450251 742
    .quad 0x03FE170C00C169B51
    .quad 0x03FE175ECFB935CC6  # 0.545645228728 743
    .quad 0x03FE175ECFB935CC6
    .quad 0x03FE17B1AC17CBD5B  # 0.546277406602 744
    .quad 0x03FE17B1AC17CBD5B
    .quad 0x03FE17E8F12052E8A  # 0.546699080654 745
    .quad 0x03FE17E8F12052E8A
    .quad 0x03FE183BE3DE8A7AF  # 0.547331925312 746
    .quad 0x03FE183BE3DE8A7AF
    .quad 0x03FE188EE40F23CA7  # 0.547965170715 747
    .quad 0x03FE188EE40F23CA7
    .quad 0x03FE18C640FF75F06  # 0.548387557205 748
    .quad 0x03FE18C640FF75F06
    .quad 0x03FE191957A30FA51  # 0.549021471648 749
    .quad 0x03FE191957A30FA51
    .quad 0x03FE196C7BC4B1F3A  # 0.549655788193 750
    .quad 0x03FE196C7BC4B1F3A
    .quad 0x03FE19A3F0B1860BD  # 0.550078889532 751
    .quad 0x03FE19A3F0B1860BD
    .quad 0x03FE19F72B59A0CEC  # 0.550713877383 752
    .quad 0x03FE19F72B59A0CEC
    .quad 0x03FE1A4A738B7A33C  # 0.551349268700 753
    .quad 0x03FE1A4A738B7A33C
    .quad 0x03FE1A820089A2156  # 0.551773087312 754
    .quad 0x03FE1A820089A2156
    .quad 0x03FE1AD55F55855C8  # 0.552409152212 755
    .quad 0x03FE1AD55F55855C8
    .quad 0x03FE1B28CBB6EC93E  # 0.553045621948 756
    .quad 0x03FE1B28CBB6EC93E
    .quad 0x03FE1B6070DB553D8  # 0.553470160269 757
    .quad 0x03FE1B6070DB553D8
    .quad 0x03FE1BB3F3EA714F6  # 0.554107305878 758
    .quad 0x03FE1BB3F3EA714F6
    .quad 0x03FE1BEBA8316EF2C  # 0.554532295260 759
    .quad 0x03FE1BEBA8316EF2C
    .quad 0x03FE1C3F41FA97C6B  # 0.555170118179 760
    .quad 0x03FE1C3F41FA97C6B
    .quad 0x03FE1C92E96C86020  # 0.555808348176 761
    .quad 0x03FE1C92E96C86020
    .quad 0x03FE1CCAB5FBFFEE1  # 0.556234061252 762
    .quad 0x03FE1CCAB5FBFFEE1
    .quad 0x03FE1D1E743BCFC47  # 0.556872970868 763
    .quad 0x03FE1D1E743BCFC47
    .quad 0x03FE1D72403052E75  # 0.557512288951 764
    .quad 0x03FE1D72403052E75
    .quad 0x03FE1DAA251D7E433  # 0.557938728190 765
    .quad 0x03FE1DAA251D7E433
    .quad 0x03FE1DFE07F3D1DAB  # 0.558578728212 766
    .quad 0x03FE1DFE07F3D1DAB
    .quad 0x03FE1E35FC265D75E  # 0.559005622562 767
    .quad 0x03FE1E35FC265D75E
    .quad 0x03FE1E89F5EB04126  # 0.559646305979 768
    .quad 0x03FE1E89F5EB04126
    .quad 0x03FE1EDDFD77E1FEF  # 0.560287400135 769
    .quad 0x03FE1EDDFD77E1FEF
    .quad 0x03FE1F160A2AD0DA3  # 0.560715024687 770
    .quad 0x03FE1F160A2AD0DA3
    .quad 0x03FE1F6A28BA1B476  # 0.561356804579 771
    .quad 0x03FE1F6A28BA1B476
    .quad 0x03FE1FBE551DB43C1  # 0.561998996616 772
    .quad 0x03FE1FBE551DB43C1
    .quad 0x03FE1FF67A6684F47  # 0.562427353873 773
    .quad 0x03FE1FF67A6684F47
    .quad 0x03FE204ABDE0BE5DF  # 0.563070233998 774
    .quad 0x03FE204ABDE0BE5DF
    .quad 0x03FE2082F29233211  # 0.563499050471 775
    .quad 0x03FE2082F29233211
    .quad 0x03FE20D74D2FBAFE4  # 0.564142620160 776
    .quad 0x03FE20D74D2FBAFE4
    .quad 0x03FE210F91524B469  # 0.564571896835 777
    .quad 0x03FE210F91524B469
    .quad 0x03FE2164031FDA0B0  # 0.565216157568 778
    .quad 0x03FE2164031FDA0B0
    .quad 0x03FE21B882DD26040  # 0.565860833641 779
    .quad 0x03FE21B882DD26040
    .quad 0x03FE21F0DFC65CEEC  # 0.566290848698 780
    .quad 0x03FE21F0DFC65CEEC
    .quad 0x03FE224576C81FFE0  # 0.566936218194 781
    .quad 0x03FE224576C81FFE0
    .quad 0x03FE227DE33896A44  # 0.567366696031 782
    .quad 0x03FE227DE33896A44
    .quad 0x03FE22D2918BA4A31  # 0.568012760445 783
    .quad 0x03FE22D2918BA4A31
    .quad 0x03FE23274DE272A83  # 0.568659242528 784
    .quad 0x03FE23274DE272A83
    .quad 0x03FE235FD33D232FC  # 0.569090462888 785
    .quad 0x03FE235FD33D232FC
    .quad 0x03FE23B4A6F9D8688  # 0.569737642287 786
    .quad 0x03FE23B4A6F9D8688
    .quad 0x03FE23ED3BF21CA33  # 0.570169328026 787
    .quad 0x03FE23ED3BF21CA33
    .quad 0x03FE24422721A89D7  # 0.570817206248 788
    .quad 0x03FE24422721A89D7
    .quad 0x03FE247ACBC023D2B  # 0.571249358372 789
    .quad 0x03FE247ACBC023D2B
    .quad 0x03FE24CFCE6F80D9B  # 0.571897936927 790
    .quad 0x03FE24CFCE6F80D9B
    .quad 0x03FE250882BCDD7D8  # 0.572330556445 791
    .quad 0x03FE250882BCDD7D8
    .quad 0x03FE255D9CF910A56  # 0.572979836849 792
    .quad 0x03FE255D9CF910A56
    .quad 0x03FE25B2C55CD5762  # 0.573629539091 793
    .quad 0x03FE25B2C55CD5762
    .quad 0x03FE25EB92D41992D  # 0.574062908546 794
    .quad 0x03FE25EB92D41992D
    .quad 0x03FE2640D2D99FFEA  # 0.574713315073 795
    .quad 0x03FE2640D2D99FFEA
    .quad 0x03FE2679B0166F51C  # 0.575147154559 796
    .quad 0x03FE2679B0166F51C
    .quad 0x03FE26CF07CAD8B00  # 0.575798266899 797
    .quad 0x03FE26CF07CAD8B00
    .quad 0x03FE2707F4D5F7C40  # 0.576232577438 798
    .quad 0x03FE2707F4D5F7C40
    .quad 0x03FE275D644670606  # 0.576884397124 799
    .quad 0x03FE275D644670606
    .quad 0x03FE27966128AB11B  # 0.577319179739 800
    .quad 0x03FE27966128AB11B
    .quad 0x03FE27EBE8626A387  # 0.577971708311 801
    .quad 0x03FE27EBE8626A387
    .quad 0x03FE2824F52493BD2  # 0.578406964030 802
    .quad 0x03FE2824F52493BD2
    .quad 0x03FE287A9434DBC7B  # 0.579060203030 803
    .quad 0x03FE287A9434DBC7B
    .quad 0x03FE28B3B0DFCEB80  # 0.579495932884 804
    .quad 0x03FE28B3B0DFCEB80
    .quad 0x03FE290967D3ED18D  # 0.580149883861 805
    .quad 0x03FE290967D3ED18D
    .quad 0x03FE294294708B773  # 0.580586088885 806
    .quad 0x03FE294294708B773
    .quad 0x03FE29986355D8C69  # 0.581240753393 807
    .quad 0x03FE29986355D8C69
    .quad 0x03FE29D19FED0C082  # 0.581677434622 808
    .quad 0x03FE29D19FED0C082
    .quad 0x03FE2A2786D0EC107  # 0.582332814220 809
    .quad 0x03FE2A2786D0EC107
    .quad 0x03FE2A60D36BA5253  # 0.582769972697 810
    .quad 0x03FE2A60D36BA5253
    .quad 0x03FE2AB6D25B86EF7  # 0.583426068948 811
    .quad 0x03FE2AB6D25B86EF7
    .quad 0x03FE2AF02F02BE4AB  # 0.583863705716 812
    .quad 0x03FE2AF02F02BE4AB
    .quad 0x03FE2B46460C1C2B3  # 0.584520520190 813
    .quad 0x03FE2B46460C1C2B3
    .quad 0x03FE2B7FB2C8D1CC1  # 0.584958636297 814
    .quad 0x03FE2B7FB2C8D1CC1
    .quad 0x03FE2BD5E1F9316F2  # 0.585616170568 815
    .quad 0x03FE2BD5E1F9316F2
    .quad 0x03FE2C0F5ED46CE8D  # 0.586054767066 816
    .quad 0x03FE2C0F5ED46CE8D
    .quad 0x03FE2C65A6395F5F5  # 0.586713022712 817
    .quad 0x03FE2C65A6395F5F5
    .quad 0x03FE2C9F333C2FE1E  # 0.587152100656 818
    .quad 0x03FE2C9F333C2FE1E
    .quad 0x03FE2CF592E351AE5  # 0.587811079263 819
    .quad 0x03FE2CF592E351AE5
    .quad 0x03FE2D2F3016CE0EF  # 0.588250639709 820
    .quad 0x03FE2D2F3016CE0EF
    .quad 0x03FE2D85A80DC7324  # 0.588910342867 821
    .quad 0x03FE2D85A80DC7324
    .quad 0x03FE2DBF557B0DF43  # 0.589350386878 822
    .quad 0x03FE2DBF557B0DF43
    .quad 0x03FE2E15E5CF91FA7  # 0.590010816181 823
    .quad 0x03FE2E15E5CF91FA7
    .quad 0x03FE2E4FA37FC9577  # 0.590451344823 824
    .quad 0x03FE2E4FA37FC9577
    .quad 0x03FE2E8967B3BF4E1  # 0.590892067615 825
    .quad 0x03FE2E8967B3BF4E1
    .quad 0x03FE2EE01A3BED567  # 0.591553516212 826
    .quad 0x03FE2EE01A3BED567
    .quad 0x03FE2F19EEBFB00BA  # 0.591994725131 827
    .quad 0x03FE2F19EEBFB00BA
    .quad 0x03FE2F70B9C67A7C2  # 0.592656903723 828
    .quad 0x03FE2F70B9C67A7C2
    .quad 0x03FE2FAA9EA342D04  # 0.593098599843 829
    .quad 0x03FE2FAA9EA342D04
    .quad 0x03FE3001823684D73  # 0.593761510043 830
    .quad 0x03FE3001823684D73
    .quad 0x03FE303B7775937EF  # 0.594203694441 831
    .quad 0x03FE303B7775937EF
    .quad 0x03FE309273A3340FC  # 0.594867337868 832
    .quad 0x03FE309273A3340FC
    .quad 0x03FE30CC794DD19D0  # 0.595310011625 833
    .quad 0x03FE30CC794DD19D0
    .quad 0x03FE3106858C76BB7  # 0.595752881428 834
    .quad 0x03FE3106858C76BB7
    .quad 0x03FE315DA4434068B  # 0.596417554101 835
    .quad 0x03FE315DA4434068B
    .quad 0x03FE3197C0FA80E6A  # 0.596860914783 836
    .quad 0x03FE3197C0FA80E6A
    .quad 0x03FE31EEF86D36EF1  # 0.597526324589 837
    .quad 0x03FE31EEF86D36EF1
    .quad 0x03FE322925A66E62D  # 0.597970177237 838
    .quad 0x03FE322925A66E62D
    .quad 0x03FE328075E32022F  # 0.598636325813 839
    .quad 0x03FE328075E32022F
    .quad 0x03FE32BAB3A7B21E9  # 0.599080671521 840
    .quad 0x03FE32BAB3A7B21E9
    .quad 0x03FE32F4F80D0B1BD  # 0.599525214760 841
    .quad 0x03FE32F4F80D0B1BD
    .quad 0x03FE334C6B15D30DD  # 0.600192400374 842
    .quad 0x03FE334C6B15D30DD
    .quad 0x03FE3386C013B90D6  # 0.600637438209 843
    .quad 0x03FE3386C013B90D6
    .quad 0x03FE33DE4C086C40A  # 0.601305366543 844
    .quad 0x03FE33DE4C086C40A
    .quad 0x03FE3418B1A85622C  # 0.601750900077 845
    .quad 0x03FE3418B1A85622C
    .quad 0x03FE34531DF21CFE3  # 0.602196632199 846
    .quad 0x03FE34531DF21CFE3
    .quad 0x03FE34AACCE299BA5  # 0.602865603124 847
    .quad 0x03FE34AACCE299BA5
    .quad 0x03FE34E549DBB21EF  # 0.603311832493 848
    .quad 0x03FE34E549DBB21EF
    .quad 0x03FE353D11DA4F855  # 0.603981550121 849
    .quad 0x03FE353D11DA4F855
    .quad 0x03FE35779F8C43D6D  # 0.604428277847 850
    .quad 0x03FE35779F8C43D6D
    .quad 0x03FE35B233F13DD4A  # 0.604875205229 851
    .quad 0x03FE35B233F13DD4A
    .quad 0x03FE360A1F1BBA738  # 0.605545971045 852
    .quad 0x03FE360A1F1BBA738
    .quad 0x03FE3644C446F97BC  # 0.605993398346 853
    .quad 0x03FE3644C446F97BC
    .quad 0x03FE367F702A9EA94  # 0.606441025927 854
    .quad 0x03FE367F702A9EA94
    .quad 0x03FE36D77E9D34FD7  # 0.607112843218 855
    .quad 0x03FE36D77E9D34FD7
    .quad 0x03FE37123B54987B7  # 0.607560972287 856
    .quad 0x03FE37123B54987B7
    .quad 0x03FE376A630C0A1D6  # 0.608233542652 857
    .quad 0x03FE376A630C0A1D6
    .quad 0x03FE37A530A0D5A31  # 0.608682174333 858
    .quad 0x03FE37A530A0D5A31
    .quad 0x03FE37E004F74E13B  # 0.609131007374 859
    .quad 0x03FE37E004F74E13B
    .quad 0x03FE383850278CFD9  # 0.609804634884 860
    .quad 0x03FE383850278CFD9
    .quad 0x03FE3873356902AB7  # 0.610253972119 861
    .quad 0x03FE3873356902AB7
    .quad 0x03FE38AE2171976E8  # 0.610703511349 862
    .quad 0x03FE38AE2171976E8
    .quad 0x03FE390690373AFFF  # 0.611378199331 863
    .quad 0x03FE390690373AFFF
    .quad 0x03FE39418D3872A53  # 0.611828244343 864
    .quad 0x03FE39418D3872A53
    .quad 0x03FE397C91064221F  # 0.612278491987 865
    .quad 0x03FE397C91064221F
    .quad 0x03FE39D5237E045A5  # 0.612954243787 866
    .quad 0x03FE39D5237E045A5
    .quad 0x03FE3A1038522CE82  # 0.613404998809 867
    .quad 0x03FE3A1038522CE82
    .quad 0x03FE3A68E45AD354B  # 0.614081512534 868
    .quad 0x03FE3A68E45AD354B
    .quad 0x03FE3AA40A3F2A68B  # 0.614532776080 869
    .quad 0x03FE3AA40A3F2A68B
    .quad 0x03FE3ADF36F98A182  # 0.614984243356 870
    .quad 0x03FE3ADF36F98A182
    .quad 0x03FE3B3806E5DF340  # 0.615661826668 871
    .quad 0x03FE3B3806E5DF340
    .quad 0x03FE3B7344BE40311  # 0.616113804077 872
    .quad 0x03FE3B7344BE40311
    .quad 0x03FE3BAE897234A87  # 0.616565985862 873
    .quad 0x03FE3BAE897234A87
    .quad 0x03FE3C077D5F51881  # 0.617244642149 874
    .quad 0x03FE3C077D5F51881
    .quad 0x03FE3C42D33F2AE7B  # 0.617697335683 875
    .quad 0x03FE3C42D33F2AE7B
    .quad 0x03FE3C7E30002960C  # 0.618150234241 876
    .quad 0x03FE3C7E30002960C
    .quad 0x03FE3CD7480B4A8A3  # 0.618829966906 877
    .quad 0x03FE3CD7480B4A8A3
    .quad 0x03FE3D12B60622748  # 0.619283378838 878
    .quad 0x03FE3D12B60622748
    .quad 0x03FE3D4E2AE7B7E2B  # 0.619736996447 879
    .quad 0x03FE3D4E2AE7B7E2B
    .quad 0x03FE3D89A6B1A558D  # 0.620190819917 880
    .quad 0x03FE3D89A6B1A558D
    .quad 0x03FE3DE2ED57B1F9B  # 0.620871941524 881
    .quad 0x03FE3DE2ED57B1F9B
    .quad 0x03FE3E1E7A6D8330E  # 0.621326280468 882
    .quad 0x03FE3E1E7A6D8330E
    .quad 0x03FE3E5A0E714DA6E  # 0.621780825931 883
    .quad 0x03FE3E5A0E714DA6E
    .quad 0x03FE3EB37978B85B6  # 0.622463031756 884
    .quad 0x03FE3EB37978B85B6
    .quad 0x03FE3EEF1ED68236B  # 0.622918094335 885
    .quad 0x03FE3EEF1ED68236B
    .quad 0x03FE3F2ACB27ED6C7  # 0.623373364090 886
    .quad 0x03FE3F2ACB27ED6C7
    .quad 0x03FE3F845AAE68C81  # 0.624056657591 887
    .quad 0x03FE3F845AAE68C81
    .quad 0x03FE3FC0186800514  # 0.624512446113 888
    .quad 0x03FE3FC0186800514
    .quad 0x03FE3FFBDD1AE8406  # 0.624968442473 889
    .quad 0x03FE3FFBDD1AE8406
    .quad 0x03FE4037A8C8C197A  # 0.625424646860 890
    .quad 0x03FE4037A8C8C197A
    .quad 0x03FE409167679DD99  # 0.626109343909 891
    .quad 0x03FE409167679DD99
    .quad 0x03FE40CD448FF6DD6  # 0.626566069196 892
    .quad 0x03FE40CD448FF6DD6
    .quad 0x03FE410928B8F950F  # 0.627023003177 893
    .quad 0x03FE410928B8F950F
    .quad 0x03FE41630C1B50AFF  # 0.627708795866 894
    .quad 0x03FE41630C1B50AFF
    .quad 0x03FE419F01CD27AD0  # 0.628166252416 895
    .quad 0x03FE419F01CD27AD0
    .quad 0x03FE41DAFE85672B9  # 0.628623918328 896
    .quad 0x03FE41DAFE85672B9
    .quad 0x03FE42170245B4C6A  # 0.629081793794 897
    .quad 0x03FE42170245B4C6A
    .quad 0x03FE42711518DF546  # 0.629769000326 898
    .quad 0x03FE42711518DF546
    .quad 0x03FE42AD2A74888A0  # 0.630227400518 899
    .quad 0x03FE42AD2A74888A0
    .quad 0x03FE42E946DE080C0  # 0.630686010936 900
    .quad 0x03FE42E946DE080C0
    .quad 0x03FE43437EB9D9424  # 0.631374321162 901
    .quad 0x03FE43437EB9D9424
    .quad 0x03FE437FACCD31C10  # 0.631833457993 902
    .quad 0x03FE437FACCD31C10
    .quad 0x03FE43BBE1F42FE09  # 0.632292805727 903
    .quad 0x03FE43BBE1F42FE09
    .quad 0x03FE43F81E307DE5E  # 0.632752364559 904
    .quad 0x03FE43F81E307DE5E
    .quad 0x03FE445285D68EA69  # 0.633442099038 905
    .quad 0x03FE445285D68EA69
    .quad 0x03FE448ED3CF71355  # 0.633902186463 906
    .quad 0x03FE448ED3CF71355
    .quad 0x03FE44CB28E37C3EE  # 0.634362485666 907
    .quad 0x03FE44CB28E37C3EE
    .quad 0x03FE450785145CAFE  # 0.634822996841 908
    .quad 0x03FE450785145CAFE
    .quad 0x03FE45621CB769366  # 0.635514161481 909
    .quad 0x03FE45621CB769366
    .quad 0x03FE459E8AB7B799D  # 0.635975203444 910
    .quad 0x03FE459E8AB7B799D
    .quad 0x03FE45DAFFDABD4DB  # 0.636436458065 911
    .quad 0x03FE45DAFFDABD4DB
    .quad 0x03FE46177C2229EC0  # 0.636897925539 912
    .quad 0x03FE46177C2229EC0
    .quad 0x03FE467243F53F69E  # 0.637590526283 913
    .quad 0x03FE467243F53F69E
    .quad 0x03FE46AED21F117FC  # 0.638052526753 914
    .quad 0x03FE46AED21F117FC
    .quad 0x03FE46EB677335D13  # 0.638514740766 915
    .quad 0x03FE46EB677335D13
    .quad 0x03FE472803F35EAAE  # 0.638977168520 916
    .quad 0x03FE472803F35EAAE
    .quad 0x03FE4764A7A13EF3B  # 0.639439810212 917
    .quad 0x03FE4764A7A13EF3B
    .quad 0x03FE47BFAA9F80271  # 0.640134174319 918
    .quad 0x03FE47BFAA9F80271
    .quad 0x03FE47FC60471DAF8  # 0.640597351724 919
    .quad 0x03FE47FC60471DAF8
    .quad 0x03FE48391D226992D  # 0.641060743762 920
    .quad 0x03FE48391D226992D
    .quad 0x03FE4875E1331971E  # 0.641524350631 921
    .quad 0x03FE4875E1331971E
    .quad 0x03FE48D114D3FB884  # 0.642220164181 922
    .quad 0x03FE48D114D3FB884
    .quad 0x03FE490DEAF1A3FC8  # 0.642684309003 923
    .quad 0x03FE490DEAF1A3FC8
    .quad 0x03FE494AC84AB0ED3  # 0.643148669355 924
    .quad 0x03FE494AC84AB0ED3
    .quad 0x03FE4987ACE0DABB0  # 0.643613245438 925
    .quad 0x03FE4987ACE0DABB0
    .quad 0x03FE49C498B5DA63F  # 0.644078037452 926
    .quad 0x03FE49C498B5DA63F
    .quad 0x03FE4A20080EF10B2  # 0.644775630783 927
    .quad 0x03FE4A20080EF10B2
    .quad 0x03FE4A5D060894B8C  # 0.645240963504 928
    .quad 0x03FE4A5D060894B8C
    .quad 0x03FE4A9A0B471A943  # 0.645706512861 929
    .quad 0x03FE4A9A0B471A943
    .quad 0x03FE4AD717CC3E626  # 0.646172279055 930
    .quad 0x03FE4AD717CC3E626
    .quad 0x03FE4B142B99BC871  # 0.646638262288 931
    .quad 0x03FE4B142B99BC871
    .quad 0x03FE4B6FD6F970C1F  # 0.647337644529 932
    .quad 0x03FE4B6FD6F970C1F
    .quad 0x03FE4BACFD036D080  # 0.647804171246 933
    .quad 0x03FE4BACFD036D080
    .quad 0x03FE4BEA2A5BDBE87  # 0.648270915712 934
    .quad 0x03FE4BEA2A5BDBE87
    .quad 0x03FE4C275F047C956  # 0.648737878130 935
    .quad 0x03FE4C275F047C956
    .quad 0x03FE4C649AFF0EE16  # 0.649205058703 936
    .quad 0x03FE4C649AFF0EE16
    .quad 0x03FE4CC082B46485A  # 0.649906239052 937
    .quad 0x03FE4CC082B46485A
    .quad 0x03FE4CFDD1037E37C  # 0.650373965908 938
    .quad 0x03FE4CFDD1037E37C
    .quad 0x03FE4D3B26AAADDD9  # 0.650841911635 939
    .quad 0x03FE4D3B26AAADDD9
    .quad 0x03FE4D7883ABB61F6  # 0.651310076438 940
    .quad 0x03FE4D7883ABB61F6
    .quad 0x03FE4DB5E8085A477  # 0.651778460521 941
    .quad 0x03FE4DB5E8085A477
    .quad 0x03FE4DF353C25E42B  # 0.652247064091 942
    .quad 0x03FE4DF353C25E42B
    .quad 0x03FE4E4F832C560DD  # 0.652950381434 943
    .quad 0x03FE4E4F832C560DD
    .quad 0x03FE4E8D015786F16  # 0.653419534621 944
    .quad 0x03FE4E8D015786F16
    .quad 0x03FE4ECA86E64A683  # 0.653888908016 945
    .quad 0x03FE4ECA86E64A683
    .quad 0x03FE4F0813DA673DD  # 0.654358501826 946
    .quad 0x03FE4F0813DA673DD
    .quad 0x03FE4F45A835A4E19  # 0.654828316258 947
    .quad 0x03FE4F45A835A4E19
    .quad 0x03FE4F8343F9CB678  # 0.655298351519 948
    .quad 0x03FE4F8343F9CB678
    .quad 0x03FE4FDFBB88A119A  # 0.656003818920 949
    .quad 0x03FE4FDFBB88A119A
    .quad 0x03FE501D69DADD660  # 0.656474407164 950
    .quad 0x03FE501D69DADD660
    .quad 0x03FE505B1F9C43ED7  # 0.656945216966 951
    .quad 0x03FE505B1F9C43ED7
    .quad 0x03FE5098DCCE9FABA  # 0.657416248534 952
    .quad 0x03FE5098DCCE9FABA
    .quad 0x03FE50D6A173BC425  # 0.657887502077 953
    .quad 0x03FE50D6A173BC425
    .quad 0x03FE51146D8D65F98  # 0.658358977805 954
    .quad 0x03FE51146D8D65F98
    .quad 0x03FE5152411D69C03  # 0.658830675927 955
    .quad 0x03FE5152411D69C03
    .quad 0x03FE51AF0C774A2D0  # 0.659538640558 956
    .quad 0x03FE51AF0C774A2D0
    .quad 0x03FE51ECF2B713F8A  # 0.660010895584 957
    .quad 0x03FE51ECF2B713F8A
    .quad 0x03FE522AE0738A3D8  # 0.660483373741 958
    .quad 0x03FE522AE0738A3D8
    .quad 0x03FE5268D5AE7CDCB  # 0.660956075239 959
    .quad 0x03FE5268D5AE7CDCB
    .quad 0x03FE52A6D269BC600  # 0.661429000289 960
    .quad 0x03FE52A6D269BC600
    .quad 0x03FE52E4D6A719F9B  # 0.661902149103 961
    .quad 0x03FE52E4D6A719F9B
    .quad 0x03FE5322E26867857  # 0.662375521893 962
    .quad 0x03FE5322E26867857
    .quad 0x03FE53800225BA6E2  # 0.663086001497 963
    .quad 0x03FE53800225BA6E2
    .quad 0x03FE53BE20B8DA502  # 0.663559935155 964
    .quad 0x03FE53BE20B8DA502
    .quad 0x03FE53FC46D64DDD1  # 0.664034093533 965
    .quad 0x03FE53FC46D64DDD1
    .quad 0x03FE543A747FE9ED6  # 0.664508476843 966
    .quad 0x03FE543A747FE9ED6
    .quad 0x03FE5478A9B78404C  # 0.664983085300 967
    .quad 0x03FE5478A9B78404C
    .quad 0x03FE54B6E67EF251C  # 0.665457919117 968
    .quad 0x03FE54B6E67EF251C
    .quad 0x03FE54F52AD80BAE9  # 0.665932978509 969
    .quad 0x03FE54F52AD80BAE9
    .quad 0x03FE553376C4A7A16  # 0.666408263689 970
    .quad 0x03FE553376C4A7A16
    .quad 0x03FE5571CA469E5C9  # 0.666883774872 971
    .quad 0x03FE5571CA469E5C9
    .quad 0x03FE55CF55C5A5437  # 0.667597465874 972
    .quad 0x03FE55CF55C5A5437
    .quad 0x03FE560DBC45153C7  # 0.668073543008 973
    .quad 0x03FE560DBC45153C7
    .quad 0x03FE564C2A6059FE7  # 0.668549846899 974
    .quad 0x03FE564C2A6059FE7
    .quad 0x03FE568AA0194EC6E  # 0.669026377763 975
    .quad 0x03FE568AA0194EC6E
    .quad 0x03FE56C91D71CF810  # 0.669503135817 976
    .quad 0x03FE56C91D71CF810
    .quad 0x03FE5707A26BB8C66  # 0.669980121278 977
    .quad 0x03FE5707A26BB8C66
    .quad 0x03FE57462F08E7DF5  # 0.670457334363 978
    .quad 0x03FE57462F08E7DF5
    .quad 0x03FE5784C34B3AC30  # 0.670934775289 979
    .quad 0x03FE5784C34B3AC30
    .quad 0x03FE57C35F3490183  # 0.671412444273 980
    .quad 0x03FE57C35F3490183
    .quad 0x03FE580202C6C7353  # 0.671890341535 981
    .quad 0x03FE580202C6C7353
    .quad 0x03FE5840AE03C0204  # 0.672368467291 982
    .quad 0x03FE5840AE03C0204
    .quad 0x03FE589EBD437CA31  # 0.673086084831 983
    .quad 0x03FE589EBD437CA31
    .quad 0x03FE58DD7BB392B30  # 0.673564782782 984
    .quad 0x03FE58DD7BB392B30
    .quad 0x03FE591C41D500163  # 0.674043709994 985
    .quad 0x03FE591C41D500163
    .quad 0x03FE595B0FA9A7EF1  # 0.674522866688 986
    .quad 0x03FE595B0FA9A7EF1
    .quad 0x03FE5999E5336E121  # 0.675002253082 987
    .quad 0x03FE5999E5336E121
    .quad 0x03FE59D8C2743705E  # 0.675481869398 988
    .quad 0x03FE59D8C2743705E
    .quad 0x03FE5A17A76DE803B  # 0.675961715857 989
    .quad 0x03FE5A17A76DE803B
    .quad 0x03FE5A56942266F7B  # 0.676441792678 990
    .quad 0x03FE5A56942266F7B
    .quad 0x03FE5A9588939A810  # 0.676922100084 991
    .quad 0x03FE5A9588939A810
    .quad 0x03FE5AD484C369F2D  # 0.677402638296 992
    .quad 0x03FE5AD484C369F2D
    .quad 0x03FE5B1388B3BD53E  # 0.677883407536 993
    .quad 0x03FE5B1388B3BD53E
    .quad 0x03FE5B5294667D5F7  # 0.678364408027 994
    .quad 0x03FE5B5294667D5F7
    .quad 0x03FE5B91A7DD93852  # 0.678845639990 995
    .quad 0x03FE5B91A7DD93852
    .quad 0x03FE5BD0C31AE9E9D  # 0.679327103649 996
    .quad 0x03FE5BD0C31AE9E9D
    .quad 0x03FE5C2F7A8ED5E5B  # 0.680049734055 997
    .quad 0x03FE5C2F7A8ED5E5B
    .quad 0x03FE5C6EA94431EF9  # 0.680531777930 998
    .quad 0x03FE5C6EA94431EF9
    .quad 0x03FE5CADDFC6874F5  # 0.681014054284 999
    .quad 0x03FE5CADDFC6874F5
    .quad 0x03FE5CED1E17C35C6  # 0.681496563340 1000
    .quad 0x03FE5CED1E17C35C6
    .quad 0x03FE5D2C6439D4252  # 0.681979305324 1001
    .quad 0x03FE5D2C6439D4252
    .quad 0x03FE5D6BB22EA86F6  # 0.682462280460 1002
    .quad 0x03FE5D6BB22EA86F6
    .quad 0x03FE5DAB07F82FB84  # 0.682945488974 1003
    .quad 0x03FE5DAB07F82FB84
    .quad 0x03FE5DEA65985A350  # 0.683428931091 1004
    .quad 0x03FE5DEA65985A350
    .quad 0x03FE5E29CB1118D32  # 0.683912607038 1005
    .quad 0x03FE5E29CB1118D32
    .quad 0x03FE5E6938645D390  # 0.684396517040 1006
    .quad 0x03FE5E6938645D390
    .quad 0x03FE5EA8AD9419C5B  # 0.684880661324 1007
    .quad 0x03FE5EA8AD9419C5B
    .quad 0x03FE5EE82AA241920  # 0.685365040118 1008
    .quad 0x03FE5EE82AA241920
    .quad 0x03FE5F27AF90C8705  # 0.685849653648 1009
    .quad 0x03FE5F27AF90C8705
    .quad 0x03FE5F673C61A2ED2  # 0.686334502142 1010
    .quad 0x03FE5F673C61A2ED2
    .quad 0x03FE5FA6D116C64F7  # 0.686819585829 1011
    .quad 0x03FE5FA6D116C64F7
    .quad 0x03FE5FE66DB228992  # 0.687304904936 1012
    .quad 0x03FE5FE66DB228992
    .quad 0x03FE60261235C0874  # 0.687790459692 1013
    .quad 0x03FE60261235C0874
    .quad 0x03FE6065BEA385926  # 0.688276250325 1014
    .quad 0x03FE6065BEA385926
    .quad 0x03FE60A572FD6FEF1  # 0.688762277066 1015
    .quad 0x03FE60A572FD6FEF1
    .quad 0x03FE60E52F45788E4  # 0.689248540144 1016
    .quad 0x03FE60E52F45788E4
    .quad 0x03FE6124F37D991D4  # 0.689735039789 1017
    .quad 0x03FE6124F37D991D4
    .quad 0x03FE6164BFA7CC06C  # 0.690221776231 1018
    .quad 0x03FE6164BFA7CC06C
    .quad 0x03FE61A493C60C729  # 0.690708749700 1019
    .quad 0x03FE61A493C60C729
    .quad 0x03FE61E46FDA56466  # 0.691195960429 1020
    .quad 0x03FE61E46FDA56466
    .quad 0x03FE622453E6A6263  # 0.691683408647 1021
    .quad 0x03FE622453E6A6263
    .quad 0x03FE62643FECF9743  # 0.692171094587 1022
    .quad 0x03FE62643FECF9743
    .quad 0x03FE62A433EF4E51A  # 0.692659018480 1023
    .quad 0x03FE62A433EF4E51A
