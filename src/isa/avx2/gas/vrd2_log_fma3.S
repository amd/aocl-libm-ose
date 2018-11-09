/*

   vrd2_log_fma3.S
   ===================
   Returns the natural log of two 64-bit floating point values packed in an xmm register

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

   Case 1:- x lies within the threshold ([e^(-1/16),e^(1/16)])
   ==========================================================

   Choose 'u' and 'r' such that
   x = 1 + r                          --- (1)

   u = 2r / (2 + r)                   --- (2)

   log( 1 + r ) = log (1 + u/2 ) - log (1 - u/2 )

   From (1); r = x - 1
   From (2); u = r - (r^2 / (r+2))        | i.e, r - correction
                                          | 'u' can't be calculated directly from 'r'

   log x = log ( 1 + r ) = u + [A1 * (u^3) + A2 * (u^5) + .... + An * (u^( 2n+1 ))]
   Since u = (r - correction),
   log ( 1 + r ) = r + [ A1 * ( u^3) + A2 * (u^5) + .... + An * (u^( 2n+1 ))] - correction

   Case 2:- x lies outside the threshold ([e^(-1/16),e^(1/16)])
   ===========================================================

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
*/



#include "fn_macros.h"
#include "log_tables.h"
#define fname FN_PROTOTYPE_FMA3(vrd2_log)


# local variable storage offsets
.equ    p_temp, 0x0
.equ    stack_size, 0x18


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:


    # compute exponent part
    #get the mask for negative inputs
    vpand        .L__sign_bit_64(%rip),%xmm0,%xmm1
    vpcmpeqq     .L__sign_bit_64(%rip),%xmm1,%xmm1

    #get the mask for Nans and Infs
    vpand        .L__exp_mask(%rip),%xmm0,%xmm14  # remove the mantissa and sign
    vpcmpeqq     .L__exp_mask(%rip),%xmm14,%xmm14
    
    #get the mask for +/- zero
    vpand        .L__sign_mask_64(%rip), %xmm0, %xmm2
    vpcmpeqq     .L__Zero_64(%rip),%xmm2,%xmm2 # xmm2 stores the mask for +/- zeros 
#################clear till here#####################
    
    #calculate log
    vpcmpgtq      .L__real_log_thresh1(%rip), %xmm0, %xmm4 #TBD probably we can use vpcmpeqq
    vcmplepd      .L__real_log_thresh2(%rip), %xmm0, %xmm3
    vpand         %xmm3, %xmm4, %xmm3 # xmm3 stores mask for (ux >= log_thresh1 && ux <= log_thresh2)

    #calculate outside the threshold range first
    
    #check for IMPBIT_DP_64 and calulate f
    vcmpltpd     .L__impbit_dp64(%rip),%xmm0,%xmm5 # mask for (ux < IMPBIT_DP64) #TBD probably we can use vpcmpeqq
    vpor        .L__constant1(%rip), %xmm0, %xmm4 # PUT_BITS_DP64(ux | 0x03d0000000000000, f)
    vsubpd      .L__constant1(%rip), %xmm4, %xmm4 # f -= corr
    vpand       .L__expadjust(%rip), %xmm5, %xmm8 # xmm8 = expadjust
    # f = xmm4/xmm7 and the mask is xmm5
    #vpcmov      %xmm5,%xmm0,%xmm4,%xmm5
    VANDNPD %xmm0, %xmm5, %xmm13
    VANDPD %xmm5, %xmm4, %xmm5
    VORPD %xmm13, %xmm5, %xmm5 
    
    # Now f = ux = xmm5 expadjust = xmm8
    vpand       .L__exp_mask(%rip),%xmm5,%xmm4
    vpsrlq       $52,%xmm4,%xmm4
    #xexp = (int)((ux & EXPBITS_DP64) >> EXPSHIFTBITS_DP64) - EXPBIAS_DP64 - expadjust;
    vpsubq       .L__mask_1023(%rip),%xmm4,%xmm4
    vpsubq       %xmm8,%xmm4,%xmm4 # subtract expadjust

     
    #index = (int)((((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46) + ((ux & 0x0000200000000000) >> 45));
    vpand        .L__index_mask1(%rip), %xmm5, %xmm6
    vpand        .L__index_mask2(%rip), %xmm5, %xmm7
    vpor         .L__index_mask3(%rip), %xmm6, %xmm6
    
    vpsrlq       $45,%xmm7, %xmm7 # ((ux & 0x0000200000000000) >> 45)
    vpsrlq       $46,%xmm6, %xmm6 # (((ux & 0x000fc00000000000) | 0x0010000000000000) >> 46)
    vpaddq       %xmm7,%xmm6,%xmm6 # index = xmm6
    vpshufd      $0xF8,%xmm6,%xmm7 # [11 11 10 00] = $0xF8
    vcvtdq2pd    %xmm7,%xmm7
    
    vmulpd       .L__index_constant1(%rip),%xmm7,%xmm7 # f1 =index*0.0078125
    vpsubq       .L__index_constant2(%rip),%xmm6,%xmm6 # index -= 64

    vpand        .L__mantissa_mask(%rip),%xmm5,%xmm5
    #PUT_BITS_DP64((ux & MANTBITS_DP64) | HALFEXPBITS_DP64, f);
    vpor         .L__real_half(%rip),%xmm5,%xmm5 # xmm5 = f
    vsubpd       %xmm7,%xmm5,%xmm5 # xmm5 = f2 = f - f1;
    
    #  z1 = ln_lead_table[index];
    #  q = ln_tail_table[index];
    vmovd        %xmm6,%r10 # move lower order index
    vpsrldq      $8,%xmm6,%xmm6
    vmovd        %xmm6,%r11 # move higher order index
    lea         .L__ln_lead_64_table(%rip), %r9
    lea         .L__ln_tail_64_table(%rip), %r8
    vmovlpd      (%r9,%r10,8), %xmm6,%xmm6
    vmovhpd      (%r9,%r11,8), %xmm6,%xmm6 # xmm6 = z1 = ln_lead_table[index]
    vmovlpd      (%r8,%r10,8), %xmm8, %xmm8
    vmovhpd      (%r8,%r11,8), %xmm8, %xmm8 # xmm8 = q = ln_tail_table[index]
    # z1 = xmm6 q = xmm8 f2 = xmm5 f1 = xmm7 

    vfmadd231pd     .L__real_half(%rip),%xmm5,%xmm7 # (f1 + 0.5 * f2);
    vdivpd       %xmm7,%xmm5,%xmm7  # u = f2 / (f1 + 0.5 * f2);
    vmulpd       %xmm7,%xmm7,%xmm5  # v = u*u

    #z1 = xmm6 q = xmm8 u = xmm7 v = %xmm5
    vmovapd      .L__real_cb_3(%rip), %xmm13 # cb_3
    vmovapd      %xmm5,%xmm11 #  
    vfmadd213pd  .L__real_cb_2(%rip),%xmm13,%xmm5 # cb_2 + v*cb_3
     
    vfmadd213pd  .L__real_cb_1(%rip),%xmm11,%xmm5 # (cb_1 + v * (cb_2 + v * cb_3))
    vmulpd      %xmm5,%xmm11,%xmm5 # poly = (v * (cb_1 + v * (cb_2 + v * cb_3)));
    #poly = xmm5 u = xmm7 q = xmm8
    vfmadd132pd    %xmm7,%xmm7,%xmm5
    vaddpd      %xmm5,%xmm8,%xmm5   # z2 = q + (u + u * poly)

    #######till here common to log2,log10,log
    vpshufd      $0xF8,%xmm4,%xmm4 # [11 11 10 00] = 0xF8
    vcvtdq2pd    %xmm4,%xmm4 # xexp (float) 
    vmovapd      %xmm4,%xmm8 # xmm8 = xexp
    vfmadd132pd     .L__real_log2_lead(%rip), %xmm6, %xmm4 # r1 = (xexp * log2_lead + z1)
    vfmadd132pd    .L__real_log2_tail(%rip), %xmm5, %xmm8 # r2 = (xexp * log2_tail + z2)
    vaddpd       %xmm4,%xmm8, %xmm8 # return r1 + r2
    ####END of calculating outside the threshold
    
    ####Now calculate when the value lies within the threshold
    #xmm0 still contains the input x
    vsubpd     .L__one_mask_64(%rip), %xmm0, %xmm4 # r = x - 1.0
    vaddpd      .L__plus_two(%rip),%xmm4, %xmm5 # (2.0 + r)
    vdivpd      %xmm5,%xmm4,%xmm6 # xmm6 = u = r / (2.0 + r); 
    vmulpd       %xmm6,%xmm4,%xmm7 # correction = r * u # TBD
    vaddpd      %xmm6,%xmm6,%xmm6 # u = u + u
    vmulpd      %xmm6,%xmm6,%xmm9 # v = u * u
    
    #r1 = xmm4 v = xmm9, u = xmm6, correction = xmm7
#TBD I think this needs some reordering to avoid the huge number of stalls.
    vmovapd     .L__real_ca_4(%rip),%xmm10 # xmm10 = v
    vfmadd213pd    .L__real_ca_3(%rip),%xmm9,%xmm10 # (ca_3 + (v * ca_4))
    vfmadd213pd    .L__real_ca_2(%rip),%xmm9,%xmm10 # (ca_2 + v * (ca_3 + v * ca_4))
    vfmadd213pd    .L__real_ca_1(%rip),%xmm9,%xmm10 # (ca_1 + v *  (ca_2 + v * (ca_3 + v * ca_4)))
    
    vmulpd      %xmm9,%xmm10,%xmm10 # v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))
    vmulpd      %xmm6,%xmm10,%xmm10 # u * v * (ca_1 + v * (ca_2 + v * (ca_3 + v * ca_4)))

    vsubpd      %xmm7,%xmm10,%xmm10 # r2 = (u * v * (ca_1 + v * (ca_2 + v * (ca_3 +  v * ca_4))) - correction) 
    
    vaddpd     %xmm4,%xmm10,%xmm10
    #END calculating within the threshold

    #now restore the proper results
    #vpcmov     %xmm3,%xmm8,%xmm10,%xmm3
    VANDNPD %xmm8, %xmm3, %xmm15
    VANDPD %xmm3, %xmm10, %xmm3
    VORPD %xmm15, %xmm3, %xmm3

    vmovapd .L__real_qnan(%rip),%xmm15
    vmovapd .L__real_ninf(%rip),%xmm13

    # now restore the result if input is less than 0.0
    #vpcmov  %xmm1,%xmm3,%xmm15,%xmm1
    VANDNPD %xmm3, %xmm1, %xmm8
    VANDPD %xmm1, %xmm15, %xmm1
    VORPD %xmm8, %xmm1, %xmm1 

    #now restore if there are some inputs with -1.0
    #vpcmov  %xmm2,%xmm1,%xmm13,%xmm8
    VANDNPD %xmm1, %xmm2, %xmm3
    VANDPD %xmm2, %xmm13, %xmm8
    VORPD %xmm3, %xmm8, %xmm8
    
    # now restore the Nans and Infs
    vpand      .L__sign_bit_64(%rip),%xmm0,%xmm3 # for negative infities we need to set the result as Qnan
    vaddpd     %xmm0,%xmm0,%xmm0
    #so we get the sign bit and move it to the qnan Bit.
    # then OR it with Qnan/inf result
    vpsrlq     $12,%xmm3,%xmm3
    vpand      %xmm14,%xmm3,%xmm3
    vpand      %xmm14,%xmm0,%xmm0
    vpor       %xmm3,%xmm0,%xmm0

    vpandn     %xmm8,%xmm14,%xmm14
    vpor       %xmm14,%xmm0,%xmm0
    
##########################################
    ret
    
.section .rodata

.align 16
.L__sign_bit_64:      .quad 0x8000000000000000 
                      .quad 0x8000000000000000
                      .quad 0x8000000000000000
                      .quad 0x8000000000000000
.L__sign_bit_32:      .quad 0x8000000080000000 
                      .quad 0x8000000080000000
.L__exp_mask:         .quad 0x7ff0000000000000   #
                      .quad 0x7ff0000000000000
.L__exponent_compare: .quad 0x7FF000007FF00000  
                      .quad 0x7FF000007FF00000
.L__sign_mask_64:     .quad 0x7FFFFFFFFFFFFFFF
                      .quad 0x7FFFFFFFFFFFFFFF
.L__Zero_64:          .quad 0x0000000000000000
                      .quad 0x0000000000000000

# The values exp(-1/16)-1 and exp(1/16)-1 */
.L__real_log_thresh1: .quad 0x3fee0faa00000000 # 0.9394121170043945, 
                      .quad 0x3fee0faa00000000
.L__real_log_thresh2: .quad 0x3ff1082c00000000#  1.0644950866699219;  
                      .quad 0x3ff1082c00000000
.L__impbit_dp64:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
.L__constant1:        .quad 0x03d0000000000000
                      .quad 0x03d0000000000000
.L__expadjust:        .quad 0x000000000000003C
                      .quad 0x000000000000003C
.L__mask_1023:        .quad 0x00000000000003ff
                      .quad 0x00000000000003ff
.L__mantissa_mask:    .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                      .quad 0x000FFFFFFFFFFFFF
.L__real_half:        .quad 0x3fe0000000000000 # 1/2
                      .quad 0x3fe0000000000000
.L__index_mask1:      .quad 0x000fc00000000000
                      .quad 0x000fc00000000000
.L__index_mask2:      .quad 0x0000200000000000
                      .quad 0x0000200000000000
.L__index_mask3:      .quad 0x0010000000000000
                      .quad 0x0010000000000000
.L__index_constant1:  .quad 0x3F80000000000000
                      .quad 0x3F80000000000000
.L__index_constant2:  .quad 0x0000000000000040
                      .quad 0x0000000000000040

# Approximating polynomial coefficients for other x 
.L__real_cb_3:       .quad 0x3f6249423bd94741 # 2.23219810758559851206e-03;  
                     .quad 0x3f6249423bd94741
.L__real_cb_2:       .quad 0x3f89999999865ede # 1.24999999978138668903e-02,  
                     .quad 0x3f89999999865ede
.L__real_cb_1:       .quad 0x3fb5555555555557 # 8.33333333333333593622e-02, 
                     .quad 0x3fb5555555555557
.L__real_log2_lead:  .quad 0x3fe62e42e0000000 # log2_lead  6.93147122859954833984e-01
                     .quad 0x3fe62e42e0000000
.L__real_log2_tail:  .quad 0x3e6efa39ef35793c # log2_tail  5.76999904754328540596e-08
                     .quad 0x3e6efa39ef35793c
.L__one_mask_64:     .quad 0x3ff0000000000000 # 1
                     .quad 0x3ff0000000000000
.L__plus_two:        .quad 0x4000000000000000
                     .quad 0x4000000000000000
# Approximating polynomial coefficients for x near 0.0 
.L__real_ca_4:       .quad 0x3f3c8034c85dfff0 # 4.34887777707614552256e-04,  
                     .quad 0x3f3c8034c85dfff0
.L__real_ca_3:       .quad 0x3f62492307f1519f # 2.23213998791944806202e-03,  
                     .quad 0x3f62492307f1519f
.L__real_ca_2:       .quad 0x3f89999999bac6d4 # 1.25000000037717509602e-02,  
                     .quad 0x3f89999999bac6d4
.L__real_ca_1:       .quad 0x3fb55555555554e6 # 8.33333333333317923934e-02,  
                     .quad 0x3fb55555555554e6

.L__mask32bits_DP64: .quad 0xffffffff00000000
                     .quad 0xffffffff00000000
.L__real_qnan:       .quad 0x7ff8000000000000   # qNaN
                     .quad 0x7ff8000000000000
.L__real_ninf:       .quad 0xfff0000000000000   # -inf
                     .quad 0xfff0000000000000


