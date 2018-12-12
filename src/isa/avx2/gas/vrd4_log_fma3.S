/*

   vrd4_log_fma3.S
   ===================
   Returns the log of four 64-bit floating point values packed in a ymm register

   Method:
   ============
   Reference:
   Ping-Tak Peter Tang
   "Table-driven implementation of the logarithm function in IEEE
   floating-point arithmetic"
   ACM Transactions on Mathematical Software (TOMS)
   Volume 16, Issue 4 (December 1990)

   Special cases:-
   1. Negative argument
   2. NaN or Infinity
   3. Zero

   Assume x to be normalized and not a special case

   Choose 'm', 'F1' and 'F2' as
   x = 2^m * (F1 + F2)                     | m is an integer
                                           | F1 = 1 + ( j / 64), j = 0,1,2,..64 ;
                                           | |F2| < 1/128
   Hence,
   log(x) = m * log(2) + log(F1) + log ( 1 + F2/F1)

   Choose U such that
   U = 2*F2 / (2*F1 + F2 )

   Then,
   log (1 + F2 / F1) = log (1 + U/2) - log (1 - U/2)
   The values of log(2) and log(F1) are precomputed and stored.

   **IMPORTANT** - The following implementation does not handle values between e^(1/16) and e^(-1/16)
                   as special cases. Maximum ULP error of 1.36 could be incurred with this implementation.
*/

#include "fn_macros.h"
#include "log_tables.h"
#define fname FN_PROTOTYPE_FMA3(vrd4_log)


# local variable storage offsets
.equ    p_temp, 0x0
.equ    stack_size, 0x18


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:


    # compute exponent part
    #get the mask for negative inputs

    vmovapd       %ymm0,%ymm1
    vpand         .L__sign_bit_64(%rip),%ymm0,%ymm1
    vpcmpeqq     .L__sign_bit_64(%rip),%ymm1,%ymm1

    #get the mask for Nans and Infs
    vpand        .L__exp_mask(%rip),%ymm0,%ymm14  # remove the mantissa and sign
    vpcmpeqq     .L__exp_mask(%rip),%ymm14,%ymm14

    #get the mask for +/- zero
    vpand        .L__sign_mask_64(%rip), %ymm0, %ymm2
    vpcmpeqq     .L__Zero_64(%rip),%ymm2,%ymm2 # ymm2 stores the mask for +/- zeros

    #check for IMPBIT_DP_64 and calulate f
    vcmpltpd    .L__impbit_dp64(%rip),%ymm0,%ymm5 # mask for (ux < IMPBIT_DP64)
    vpor        .L__constant1(%rip), %ymm0, %ymm4 # PUT_BITS_DP64(ux | 0x03d0000000000000, f)
    vsubpd      .L__constant1(%rip), %ymm4, %ymm4 # f -= corr
    vpand       .L__expadjust(%rip), %ymm5, %ymm8 # ymm8 = expadjust
    # f = ymm4/ymm7 and the mask is ymm5
    VANDNPD %ymm0, %ymm5, %ymm13
    VANDPD %ymm5, %ymm4, %ymm5
    VORPD %ymm13, %ymm5, %ymm5

    # Now f = ux = ymm5 expadjust = ymm8
    vpand       .L__exp_mask(%rip),%ymm5,%ymm4   # Get exponent part of input
    vpsrlq       $52,%ymm4,%ymm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    vpsubq       .L__mask_1023(%rip),%ymm4,%ymm4
    vpsubq       %ymm8,%ymm4,%ymm4 # subtract expadjust


    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    vpand        .L__index_mask1(%rip), %ymm5, %ymm6
    vpand        .L__index_mask2(%rip), %ymm5, %ymm7
    vpor         .L__index_mask3(%rip), %ymm6, %ymm6

    vpsrlq       $45,%ymm7, %ymm7 # ((ux & 0x0000200000000000) >> 45)
    vpsrlq       $46,%ymm6, %ymm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    vpaddq       %ymm7,%ymm6,%ymm6 # index = ymm6
    vpshufd      $0xF8,%ymm6,%ymm7 # [11 11 10 00] = $0xF8
    vextractf128 $1,%ymm7,%xmm8    # Get upper double words into xmm8
    vpslldq      $8,%xmm8,%xmm8
    vpaddq       %xmm7,%xmm8,%xmm7 # Add upper and lower double words
    vcvtdq2pd    %xmm7,%ymm7       # ymm7 has index in floating point

    vmulpd       .L__index_constant1(%rip),%ymm7,%ymm7 # f1 =index*0.0078125
    vpsubq       .L__index_constant2(%rip),%ymm6,%ymm6 # index -= 64

    vpand        .L__mantissa_mask(%rip),%ymm5,%ymm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | HALFEXPBITS_DP64, f);
    vpor         .L__real_half(%rip),%ymm5,%ymm5 # ymm5 = f
    vsubpd       %ymm7,%ymm5,%ymm5 # ymm5 = f2 = f - f1;

    #  z1 = ln_lead_table[index];
    #  q = ln_tail_table[index];
    lea         .L__ln_lead_64_table(%rip), %r9  # r9 has base address of ln_lead_table
    lea         .L__ln_tail_64_table(%rip), %r8  # r8 has base address of ln_tail_table
    vpcmpeqw    %ymm9,%ymm9,%ymm9                # set mask
    vgatherqpd  %ymm9, (%r8, %ymm6, 8), %ymm8    # ymm8 = q = ln_tail_table[index]
    vpcmpeqw    %ymm9,%ymm9,%ymm9		 # Set mask again
    vgatherqpd  %ymm9, (%r9, %ymm6, 8), %ymm10   # ymm10 = z1 = ln_lead_table[index]
    # z1 = ymm10 q = ymm8 f2 = ymm5 f1 = ymm7

    vfmadd231pd  .L__real_half(%rip),%ymm5,%ymm7 # (f1 + 0.5 * f2);
    vdivpd       %ymm7,%ymm5,%ymm7  # u = f2 / (f1 + 0.5 * f2);
    vmulpd       %ymm7,%ymm7,%ymm5  # v = u*u

    #z1 = ymm10 q = ymm8 u = ymm7 v = %ymm5
    vmovapd      .L__real_cb_3(%rip), %ymm13 # cb_3
    vmovapd      %ymm5,%ymm11
    vfmadd213pd  .L__real_cb_2(%rip),%ymm13,%ymm5 # cb_2 + v*cb_3

    vfmadd213pd  .L__real_cb_1(%rip),%ymm11,%ymm5 # (cb_1 + v * (cb_2 + v * cb_3))
    vmulpd      %ymm5,%ymm11,%ymm5 # poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    #poly = ymm5 u = ymm7 q = ymm8
    vfmadd132pd    %ymm7,%ymm7,%ymm5
    vaddpd      %ymm5,%ymm8,%ymm5   # z2 = q + (u + u * poly)

    #######till here common to log2,log10,log
    vpshufd      $0xF8,%ymm4,%ymm4 # [11 11 10 00] = 0xF8
    vextractf128 $1,%ymm4,%xmm8    # Get upper double words
    vcvtdq2pd    %xmm8,%ymm8
    vcvtdq2pd    %xmm4,%ymm4
    vinsertf128  $1,%xmm8,%ymm4,%ymm4 # Combine both pairs of double words
    vmovapd      %ymm4,%ymm8
    vfmadd132pd  .L__real_log2_lead(%rip), %ymm10, %ymm4 # r1 = (xexp * log2_lead + z1)
    vfmadd132pd  .L__real_log2_tail(%rip), %ymm5, %ymm8 # r2 = (xexp * log2_tail + z2)
    vaddpd       %ymm4,%ymm8, %ymm8 # return r1 + r2

    vmovapd .L__real_qnan(%rip),%ymm15
    vmovapd .L__real_ninf(%rip),%ymm13

    # now restore the result if input is less than 0.0
    VANDNPD %ymm8, %ymm1, %ymm8
    VANDPD %ymm1, %ymm15, %ymm1
    VORPD %ymm8, %ymm1, %ymm1

    #now restore if there are some inputs with -1.0
    VANDNPD %ymm1, %ymm2, %ymm3
    VANDPD %ymm2, %ymm13, %ymm8
    VORPD %ymm3, %ymm8, %ymm8

    # now restore the Nans and Infs
    vpand      .L__sign_bit_64(%rip),%ymm0,%ymm3 # for negative infities we need to set the result as Qnan
    vaddpd     %ymm0,%ymm0,%ymm0
    #so we get the sign bit and move it to the qnan Bit.
    # then OR it with Qnan/inf result
    vpsrlq     $12,%ymm3,%ymm3
    vpand      %ymm14,%ymm3,%ymm3
    vpand      %ymm14,%ymm0,%ymm0
    vpor       %ymm3,%ymm0,%ymm0

    vpandn     %ymm8,%ymm14,%ymm14
    vpor       %ymm14,%ymm0,%ymm0

##########################################
    ret

.section .rodata

.align 32

.L__sign_bit_64:      .quad 0x8000000000000000
                      .quad 0x8000000000000000
                      .quad 0x8000000000000000
                      .quad 0x8000000000000000
.L__sign_bit_32:      .quad 0x8000000080000000
                      .quad 0x8000000080000000
		      .quad 0x8000000080000000
		      .quad 0x8000000080000000
.L__exp_mask:         .quad 0x7ff0000000000000   #
                      .quad 0x7ff0000000000000
		      .quad 0x7ff0000000000000
		      .quad 0x7ff0000000000000
.L__exponent_compare: .quad 0x7FF000007FF00000
                      .quad 0x7FF000007FF00000
		      .quad 0x7FF000007FF00000
		      .quad 0x7FF000007FF00000
.L__sign_mask_64:     .quad 0x7FFFFFFFFFFFFFFF
                      .quad 0x7FFFFFFFFFFFFFFF
		      .quad 0x7FFFFFFFFFFFFFFF
		      .quad 0x7FFFFFFFFFFFFFFF
.L__Zero_64:          .quad 0x0000000000000000
                      .quad 0x0000000000000000
		      .quad 0x0000000000000000
		      .quad 0x0000000000000000

.L__impbit_dp64:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
		      .quad 0x0010000000000000
		      .quad 0x0010000000000000
.L__constant1:        .quad 0x03d0000000000000
                      .quad 0x03d0000000000000
		      .quad 0x03d0000000000000
		      .quad 0x03d0000000000000
.L__expadjust:        .quad 0x000000000000003C
                      .quad 0x000000000000003C
		      .quad 0x000000000000003C
		      .quad 0x000000000000003C
.L__mask_1023:        .quad 0x00000000000003ff
                      .quad 0x00000000000003ff
		      .quad 0x00000000000003ff
		      .quad 0x00000000000003ff
.L__mantissa_mask:    .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                      .quad 0x000FFFFFFFFFFFFF
		      .quad 0x000FFFFFFFFFFFFF
		      .quad 0x000FFFFFFFFFFFFF
.L__real_half:        .quad 0x3fe0000000000000 # 1/2
                      .quad 0x3fe0000000000000
		      .quad 0x3fe0000000000000
		      .quad 0x3fe0000000000000
.L__index_mask1:      .quad 0x000fc00000000000
                      .quad 0x000fc00000000000
		      .quad 0x000fc00000000000
		      .quad 0x000fc00000000000
.L__index_mask2:      .quad 0x0000200000000000
                      .quad 0x0000200000000000
		      .quad 0x0000200000000000
		      .quad 0x0000200000000000
.L__index_mask3:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
		      .quad 0x0010000000000000
		      .quad 0x0010000000000000
.L__index_constant1:  .quad 0x3F80000000000000
                      .quad 0x3F80000000000000
		      .quad 0x3F80000000000000
		      .quad 0x3F80000000000000
.L__index_constant2:  .quad 0x0000000000000040
                      .quad 0x0000000000000040
		      .quad 0x0000000000000040
		      .quad 0x0000000000000040

# Approximating polynomial coefficients
.L__real_cb_3:       .quad 0x3f6249423bd94741 # 2.23219810758559851206e-03;
                     .quad 0x3f6249423bd94741
		     .quad 0x3f6249423bd94741
		     .quad 0x3f6249423bd94741
.L__real_cb_2:       .quad 0x3f89999999865ede # 1.24999999978138668903e-02,
                     .quad 0x3f89999999865ede
		     .quad 0x3f89999999865ede
		     .quad 0x3f89999999865ede
.L__real_cb_1:       .quad 0x3fb5555555555557 # 8.33333333333333593622e-02,
                     .quad 0x3fb5555555555557
		     .quad 0x3fb5555555555557
		     .quad 0x3fb5555555555557
.L__real_log2_lead:  .quad 0x3fe62e42e0000000 # log2_lead  6.93147122859954833984e-01
                     .quad 0x3fe62e42e0000000
		     .quad 0x3fe62e42e0000000
		     .quad 0x3fe62e42e0000000
.L__real_log2_tail:  .quad 0x3e6efa39ef35793c # log2_tail  5.76999904754328540596e-08
                     .quad 0x3e6efa39ef35793c
		     .quad 0x3e6efa39ef35793c
		     .quad 0x3e6efa39ef35793c

.L__real_qnan:       .quad 0x7ff8000000000000   # qNaN
                     .quad 0x7ff8000000000000
		     .quad 0x7ff8000000000000
		     .quad 0x7ff8000000000000
.L__real_ninf:       .quad 0xfff0000000000000   # -inf
                     .quad 0xfff0000000000000
		     .quad 0xfff0000000000000
		     .quad 0xfff0000000000000
