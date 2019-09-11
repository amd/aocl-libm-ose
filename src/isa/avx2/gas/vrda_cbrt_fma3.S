#Algorithm:
#
#x = (2^m)*A
#x = (2^m)*(G+g) with (1 <= G < 2) and (g <= 2^(-9))
#x = (2^m)*2*(G/2+g/2)
#x = (2^m)*2*(F+f) with (0.5 <= F < 1) and (f <= 2^(-10))
#
#Y = (2^(-1))*(2^(-m))*(2^m)*A
#Now, range of Y is: 0.5 <= Y < 1
#
#F = 0x100 + (first 8 mantissa bits) + (9th mantissa bit)
#Now, range of F is: 256 <= F <= 512
#F = F / 512
#Now, range of F is: 0.5 <= F <= 1
#
#f = (Y-F), with (f <= 2^(-10))
#
#cbrt(x) = cbrt(2^m) * cbrt(2) * cbrt(F+f)
#cbrt(x) = cbrt(2^m) * cbrt(2) * cbrt(F) + cbrt(1+(f/F))
#cbrt(x) = cbrt(2^m) * cbrt(2*F) * cbrt(1+r)
#
#r = (f/F), with (r <= 2^(-9))
#r = f*(1/F) with (1/F) precomputed to avoid division
#
#cbrt(x) = cbrt(2^m) * cbrt(G) * (1+poly)
#
#poly = c1*r + c2*(r^2) + c3*(r^3) + c4*(r^4) + c5*(r^5) + c6*(r^6)
#
# vrd2_cbrt.S
#
# An implementation of the cbrt libm function.
#
# Prototype:
#
#     __m128d vrd2_cbrt(__m128d x);
#
#   Computes the floating point double precision cube root of x.
#   Returns proper C99 values, but may not raise status flags properly.
#
#   Algorithm:
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(vrda_cbrt)

# local variable storage offsets


.macro LOAD_INDEXED address, ind_reg, reg_val, scratch_pad
        lea \address, %rax
        VMOVD \ind_reg, %r11
        VMOVLPD (%rax,%r11,8),\reg_val, \reg_val
        VPUNPCKHQDQ \ind_reg, \ind_reg, \scratch_pad
        VMOVD \scratch_pad, %r11
        VMOVHPD (%rax,%r11,8), \reg_val, \reg_val
        .endm


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:
cmp         $0x0, %edi
    jle         .L__return    
    cmp         $0x0, %rsi
    je         .L__return
    cmp         $0x0, %rdx
    je         .L__return
    

.p2align 4    
.L__process_next2:
    sub         $0x2, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first1
    movdqu      (%rsi, %rdi, 8), %xmm0
    jmp         .L__start
    
.p2align 4    
.L__process_first1:
    jl          .L__return #even elements, all are processed %rdi == -2
    mov         $0x0, %rcx
    movsd       (%rsi, %rcx, 8), %xmm0    
    
.p2align 4    
.L__start:    

    VCMPNEQPD %xmm0,%xmm0,%xmm11                        #nan mask xmm11
    VXORPD %xmm7,%xmm7,%xmm7
    VANDPD .L__sign_mask_64(%rip),%xmm0,%xmm2           #clear the sign
    VCMPEQPD %xmm7,%xmm2,%xmm7                          #zero mask xmm7
    VPCMPEQQ .L__inf_mask_64(%rip),%xmm2,%xmm4          #xmm4 has inf mask
    VANDPD .L__qnan_mask_64(%rip),%xmm11,%xmm3          #xmm3 is nan mask value

    VORPD %xmm7,%xmm4,%xmm4                             #
    VORPD %xmm11,%xmm4,%xmm11                           #xmm11 has nan zero and inf mask
    VANDPD %xmm11,%xmm0,%xmm12
    VANDNPD %xmm11,%xmm11, %xmm1
    VORPD %xmm1,%xmm12,%xmm12   



    VANDNPD %xmm12, %xmm3, %xmm1
    VORPD %xmm3, %xmm12, %xmm12
    VORPD %xmm1, %xmm12, %xmm12

    VANDNPD %xmm2,%xmm11,%xmm2                          #clear the input value in case of invalid input

    VANDPD .L__sign_bit_64(%rip),%xmm0,%xmm1            #store the sign...
    VPSRLQ  $52,%xmm2,%xmm3                             #xmm3 has the exponent
    VPSUBQ  .L__bias_constant_1023(%rip),%xmm3,%xmm3    #m - 1023.   m - xmm3    x - xmm2

    VORPD .L__one_mask_64(%rip),%xmm2,%xmm4             #denormal number
    VSUBPD .L__one_mask_64(%rip),%xmm4,%xmm4
    VPSRLQ  $52,%xmm4,%xmm5
    VPSUBQ  .L__bias_constant_1022(%rip),%xmm5,%xmm5
    VPADDQ  %xmm3,%xmm5,%xmm5                           #x -  xmm4  xmm5 - m  for denormal

    VPCMPEQQ .L__denormal_number(%rip),%xmm3,%xmm6      # if the exponent is 0xfffffffffffffc01 then denormal

    VANDNPD %xmm2, %xmm6, %xmm0
    VANDPD %xmm6, %xmm4, %xmm2
    VORPD %xmm0, %xmm2, %xmm2

    VANDNPD %xmm3, %xmm6, %xmm0
    VANDPD %xmm6, %xmm5, %xmm3
    VORPD %xmm0, %xmm3, %xmm3


    VSHUFPS $8,%xmm3,%xmm3,%xmm4
    VCVTDQ2PD %xmm4,%xmm4                               #convert to double
    VMULPD  .L__double_oneby3(%rip),%xmm4,%xmm5         #compute m % 3 and m / 3
    VCVTTPD2DQ %xmm5,%xmm5                              #convert back to integer , xmm5 - m
    VPADDD %xmm5,%xmm5,%xmm6
    VPADDD %xmm6,%xmm5,%xmm6
    VPMOVSXDQ  %xmm5,%xmm5
    VPMOVSXDQ %xmm6,%xmm6
    VPSUBQ %xmm6,%xmm3,%xmm0                            # subtract to calculate remainder xmm0 - remainder


    VPADDQ .L__bias_constant_1023(%rip),%xmm5,%xmm3
    VPSLLQ  $52,%xmm3,%xmm3                             # mf = xmm3
    VANDPD .L__mantissa_mask_64(%rip),%xmm2,%xmm2
    VORPD .L__zero_point_five(%rip),%xmm2,%xmm4         # xmm4 is Y
    VPSRLQ $43,%xmm2,%xmm5
    VPSRLQ $44,%xmm2,%xmm6
    VANDPD .L__1stbit_set(%rip),%xmm5,%xmm5
    VORPD .L__9thbit_set(%rip),%xmm6,%xmm6
    VPADDQ %xmm5,%xmm6,%xmm2
    VSHUFPS $8,%xmm2,%xmm2,%xmm5
    VCVTDQ2PD %xmm5,%xmm5
    VFNMADD231PD .L__one_by_512(%rip),%xmm5,%xmm4   #index_u64  - xmm2    f = xmm4

    VPSUBQ .L__integer_256(%rip),%xmm2,%xmm2
    LOAD_INDEXED .L__INV_TAB_256(%rip),%xmm2,%xmm5,%xmm6
    VMULPD %xmm4,%xmm5,%xmm5                            # r = xmm5  xmm2 = index_u64-256

    VMULPD %xmm5,%xmm5,%xmm6                            # xmm6 = r^2
    VMULPD %xmm6,%xmm5,%xmm7                            # xmm7 = r^3
    VMULPD %xmm7,%xmm5,%xmm8                            # xmm8 = r^4
    VMULPD %xmm8,%xmm5,%xmm9                            # xmm9 = r^5
    VMULPD %xmm9,%xmm5,%xmm10                            # xmm10 = r^6


    VMULPD   .L__coefficients_6(%rip),%xmm10,%xmm10
    VFMADD231PD .L__coefficients_5(%rip),%xmm9,%xmm10
    VFMADD231PD .L__coefficients_4(%rip),%xmm8,%xmm10
    VFMADD231PD .L__coefficients_3(%rip),%xmm7,%xmm10
    VFMADD231PD .L__coefficients_2(%rip),%xmm6,%xmm10
    VFMADD132PD .L__coefficients_1(%rip),%xmm10,%xmm5   # z = xmm5 , xmm2 = index_u64-256,mf = xmm3

    LOAD_INDEXED .L__CBRT_REM_H(%rip),%xmm0,%xmm8,%xmm6
    LOAD_INDEXED .L__CBRT_REM_L(%rip),%xmm0,%xmm9,%xmm6
    LOAD_INDEXED .L__CBRT_F_H_256(%rip),%xmm2,%xmm6,%xmm4
    LOAD_INDEXED .L__CBRT_F_T_256(%rip),%xmm2,%xmm7,%xmm4  # cbrtF_h = xmm6  , cbrtF_t = xmm7 , cbrtRem_h, xmm8 , cbrtRem_t = xmm9

    VMULPD %xmm6,%xmm8,%xmm2
    VMULPD  %xmm7,%xmm9,%xmm0
    VFMADD231PD  %xmm7,%xmm8,%xmm0
    VFMADD231PD %xmm9,%xmm6,%xmm0                           # bH = xmm2 , bT = xmm0

    VFMADD231PD %xmm5,%xmm0,%xmm0
    VFMADD231PD %xmm5,%xmm2,%xmm0
    VADDPD   %xmm2,%xmm0,%xmm0
    VMULPD   %xmm0,%xmm3,%xmm0
    VORPD   %xmm0,%xmm1,%xmm0

    VANDPD %xmm11,%xmm12,%xmm12
    VANDNPD %xmm0,%xmm11,%xmm11
    VORPD %xmm12,%xmm11,%xmm0   
    
 cmp         $-1,  %rdi
    je          .L__store1
    movdqu       %xmm0, (%rdx, %rdi, 8)
    jmp         .L__process_next2    
    
.p2align 4    
.L__store1:
    inc        %rdi
    movsd      %xmm0, (%rdx,%rdi,8)
    
.L__return:    
    ret      

.section .rodata
.align 16

.L__coefficients_1: .quad 0x3fd5555555555555
.quad 0x3fd5555555555555
.L__coefficients_2: .quad 0xbfbc71c71c71c71c
.quad 0xbfbc71c71c71c71c
.L__coefficients_3: .quad 0x3faf9add3c0ca458
.quad 0x3faf9add3c0ca458
.L__coefficients_4: .quad 0xbfa511e8d2b3183b
.quad 0xbfa511e8d2b3183b
.L__coefficients_5: .quad 0x3f9ee7113506ac13
.quad 0x3f9ee7113506ac13
.L__coefficients_6: .quad 0xbf98090d6221a247
.quad 0xbf98090d6221a247
.L__coefficients_7: .quad 0x3f93750ad588f114
.quad 0x3f93750ad588f114


.L__CBRT_REM_H_TEMP: .quad 0x3fe428a2f0000000
.quad 0x3fe965fea0000000
.L__CBRT_REM_H: .quad 0x3ff0000000000000
.quad 0x3ff428a2f0000000
.quad 0x3ff965fea0000000



.L__CBRT_REM_L_TEMP: .quad 0x3e531ae515c447bb
.quad 0x3e44f5b8f20ac166

.L__CBRT_REM_L: .quad 0x0000000000000000
.quad 0x3e631ae515c447bb
.quad 0x3e54f5b8f20ac166


.L__inf_mask_64: .quad 0x7FF0000000000000
.quad 0x7FF0000000000000

.L__qnan_mask_64: .quad 0x0008000000000000
.quad 0x0008000000000000

.L__sign_bit_64: .quad 0x8000000000000000
.quad 0x8000000000000000

.L__sign_mask_64: .quad 0x7FFFFFFFFFFFFFFF
.quad 0x7FFFFFFFFFFFFFFF

.L__bias_constant_1023: .quad 0x00000000000003FF
.quad 0x00000000000003FF

.L__bias_constant_1022: .quad 0x00000000000003FE
.quad 0x00000000000003FE


.L__denormal_number: .quad 0xfffffffffffffc01
.quad 0xfffffffffffffc01

.L__double_oneby3: .quad 0x3fd5555555555555
.quad 0x3fd5555555555555

.L__mantissa_mask_64: .quad 0x000FFFFFFFFFFFFF
.quad 0x000FFFFFFFFFFFFF

.L__zero_point_five: .quad 0x3FE0000000000000
.quad 0x3FE0000000000000

.L__integer_256: .quad 0x0000000000000100
.quad 0x0000000000000100

.L__one_mask_64: .quad 0x3FF0000000000000
.quad 0x3FF0000000000000

.L__1stbit_set: .quad 0x0000000000000001
.quad 0x0000000000000001

.L__9thbit_set: .quad 0x0000000000000100
.quad 0x0000000000000100

.L__one_by_512: .quad 0x3f60000000000000
.quad 0x3f60000000000000


.align 32
.L__CBRT_F_H_256: .quad 0x3ff0000000000000
.quad 0x3ff0055380000000
.quad 0x3ff00aa390000000
.quad 0x3ff00ff010000000
.quad 0x3ff0153920000000
.quad 0x3ff01a7eb0000000
.quad 0x3ff01fc0d0000000
.quad 0x3ff024ff80000000
.quad 0x3ff02a3ad0000000
.quad 0x3ff02f72b0000000
.quad 0x3ff034a750000000
.quad 0x3ff039d880000000
.quad 0x3ff03f0670000000
.quad 0x3ff0443110000000
.quad 0x3ff0495870000000
.quad 0x3ff04e7c80000000
.quad 0x3ff0539d60000000
.quad 0x3ff058bb00000000
.quad 0x3ff05dd570000000
.quad 0x3ff062ecc0000000
.quad 0x3ff06800e0000000
.quad 0x3ff06d11e0000000
.quad 0x3ff0721fc0000000
.quad 0x3ff0772a80000000
.quad 0x3ff07c3230000000
.quad 0x3ff08136d0000000
.quad 0x3ff0863860000000
.quad 0x3ff08b36f0000000
.quad 0x3ff0903280000000
.quad 0x3ff0952b10000000
.quad 0x3ff09a20a0000000
.quad 0x3ff09f1340000000
.quad 0x3ff0a402f0000000
.quad 0x3ff0a8efc0000000
.quad 0x3ff0add990000000
.quad 0x3ff0b2c090000000
.quad 0x3ff0b7a4b0000000
.quad 0x3ff0bc85f0000000
.quad 0x3ff0c16450000000
.quad 0x3ff0c63fe0000000
.quad 0x3ff0cb18b0000000
.quad 0x3ff0cfeeb0000000
.quad 0x3ff0d4c1e0000000
.quad 0x3ff0d99250000000
.quad 0x3ff0de6010000000
.quad 0x3ff0e32b00000000
.quad 0x3ff0e7f340000000
.quad 0x3ff0ecb8d0000000
.quad 0x3ff0f17bb0000000
.quad 0x3ff0f63bf0000000
.quad 0x3ff0faf970000000
.quad 0x3ff0ffb460000000
.quad 0x3ff1046cb0000000
.quad 0x3ff1092250000000
.quad 0x3ff10dd560000000
.quad 0x3ff11285e0000000
.quad 0x3ff11733d0000000
.quad 0x3ff11bdf30000000
.quad 0x3ff1208800000000
.quad 0x3ff1252e40000000
.quad 0x3ff129d210000000
.quad 0x3ff12e7350000000
.quad 0x3ff1331210000000
.quad 0x3ff137ae60000000
.quad 0x3ff13c4840000000
.quad 0x3ff140dfa0000000
.quad 0x3ff1457490000000
.quad 0x3ff14a0710000000
.quad 0x3ff14e9730000000
.quad 0x3ff15324e0000000
.quad 0x3ff157b030000000
.quad 0x3ff15c3920000000
.quad 0x3ff160bfc0000000
.quad 0x3ff16543f0000000
.quad 0x3ff169c5d0000000
.quad 0x3ff16e4560000000
.quad 0x3ff172c2a0000000
.quad 0x3ff1773d90000000
.quad 0x3ff17bb630000000
.quad 0x3ff1802c90000000
.quad 0x3ff184a0a0000000
.quad 0x3ff1891270000000
.quad 0x3ff18d8210000000
.quad 0x3ff191ef60000000
.quad 0x3ff1965a80000000
.quad 0x3ff19ac360000000
.quad 0x3ff19f2a10000000
.quad 0x3ff1a38e90000000
.quad 0x3ff1a7f0e0000000
.quad 0x3ff1ac5100000000
.quad 0x3ff1b0af00000000
.quad 0x3ff1b50ad0000000
.quad 0x3ff1b96480000000
.quad 0x3ff1bdbc10000000
.quad 0x3ff1c21180000000
.quad 0x3ff1c664d0000000
.quad 0x3ff1cab610000000
.quad 0x3ff1cf0530000000
.quad 0x3ff1d35230000000
.quad 0x3ff1d79d30000000
.quad 0x3ff1dbe620000000
.quad 0x3ff1e02cf0000000
.quad 0x3ff1e471d0000000
.quad 0x3ff1e8b490000000
.quad 0x3ff1ecf550000000
.quad 0x3ff1f13410000000
.quad 0x3ff1f570d0000000
.quad 0x3ff1f9ab90000000
.quad 0x3ff1fde450000000
.quad 0x3ff2021b20000000
.quad 0x3ff2064ff0000000
.quad 0x3ff20a82c0000000
.quad 0x3ff20eb3b0000000
.quad 0x3ff212e2a0000000
.quad 0x3ff2170fb0000000
.quad 0x3ff21b3ac0000000
.quad 0x3ff21f63f0000000
.quad 0x3ff2238b40000000
.quad 0x3ff227b0a0000000
.quad 0x3ff22bd420000000
.quad 0x3ff22ff5c0000000
.quad 0x3ff2341570000000
.quad 0x3ff2383350000000
.quad 0x3ff23c4f60000000
.quad 0x3ff2406980000000
.quad 0x3ff24481d0000000
.quad 0x3ff2489850000000
.quad 0x3ff24cad00000000
.quad 0x3ff250bfe0000000
.quad 0x3ff254d0e0000000
.quad 0x3ff258e020000000
.quad 0x3ff25ced90000000
.quad 0x3ff260f940000000
.quad 0x3ff2650320000000
.quad 0x3ff2690b40000000
.quad 0x3ff26d1190000000
.quad 0x3ff2711630000000
.quad 0x3ff2751900000000
.quad 0x3ff2791a20000000
.quad 0x3ff27d1980000000
.quad 0x3ff2811720000000
.quad 0x3ff2851310000000
.quad 0x3ff2890d50000000
.quad 0x3ff28d05d0000000
.quad 0x3ff290fca0000000
.quad 0x3ff294f1c0000000
.quad 0x3ff298e530000000
.quad 0x3ff29cd700000000
.quad 0x3ff2a0c710000000
.quad 0x3ff2a4b580000000
.quad 0x3ff2a8a250000000
.quad 0x3ff2ac8d70000000
.quad 0x3ff2b076f0000000
.quad 0x3ff2b45ec0000000
.quad 0x3ff2b84500000000
.quad 0x3ff2bc29a0000000
.quad 0x3ff2c00c90000000
.quad 0x3ff2c3ee00000000
.quad 0x3ff2c7cdc0000000
.quad 0x3ff2cbabf0000000
.quad 0x3ff2cf8880000000
.quad 0x3ff2d36390000000
.quad 0x3ff2d73d00000000
.quad 0x3ff2db14d0000000
.quad 0x3ff2deeb20000000
.quad 0x3ff2e2bfe0000000
.quad 0x3ff2e69310000000
.quad 0x3ff2ea64b0000000
.quad 0x3ff2ee34d0000000
.quad 0x3ff2f20360000000
.quad 0x3ff2f5d070000000
.quad 0x3ff2f99bf0000000
.quad 0x3ff2fd65f0000000
.quad 0x3ff3012e70000000
.quad 0x3ff304f570000000
.quad 0x3ff308baf0000000
.quad 0x3ff30c7ef0000000
.quad 0x3ff3104180000000
.quad 0x3ff3140280000000
.quad 0x3ff317c210000000
.quad 0x3ff31b8020000000
.quad 0x3ff31f3cd0000000
.quad 0x3ff322f7f0000000
.quad 0x3ff326b1b0000000
.quad 0x3ff32a69f0000000
.quad 0x3ff32e20c0000000
.quad 0x3ff331d620000000
.quad 0x3ff3358a20000000
.quad 0x3ff3393ca0000000
.quad 0x3ff33cedc0000000
.quad 0x3ff3409d70000000
.quad 0x3ff3444bc0000000
.quad 0x3ff347f8a0000000
.quad 0x3ff34ba420000000
.quad 0x3ff34f4e30000000
.quad 0x3ff352f6f0000000
.quad 0x3ff3569e40000000
.quad 0x3ff35a4430000000
.quad 0x3ff35de8c0000000
.quad 0x3ff3618bf0000000
.quad 0x3ff3652dd0000000
.quad 0x3ff368ce40000000
.quad 0x3ff36c6d60000000
.quad 0x3ff3700b30000000
.quad 0x3ff373a7a0000000
.quad 0x3ff37742b0000000
.quad 0x3ff37adc70000000
.quad 0x3ff37e74e0000000
.quad 0x3ff3820c00000000
.quad 0x3ff385a1c0000000
.quad 0x3ff3893640000000
.quad 0x3ff38cc960000000
.quad 0x3ff3905b40000000
.quad 0x3ff393ebd0000000
.quad 0x3ff3977b10000000
.quad 0x3ff39b0910000000
.quad 0x3ff39e95c0000000
.quad 0x3ff3a22120000000
.quad 0x3ff3a5ab40000000
.quad 0x3ff3a93410000000
.quad 0x3ff3acbbb0000000
.quad 0x3ff3b04200000000
.quad 0x3ff3b3c700000000
.quad 0x3ff3b74ad0000000
.quad 0x3ff3bacd60000000
.quad 0x3ff3be4eb0000000
.quad 0x3ff3c1ceb0000000
.quad 0x3ff3c54d90000000
.quad 0x3ff3c8cb20000000
.quad 0x3ff3cc4770000000
.quad 0x3ff3cfc2a0000000
.quad 0x3ff3d33c80000000
.quad 0x3ff3d6b530000000
.quad 0x3ff3da2cb0000000
.quad 0x3ff3dda2f0000000
.quad 0x3ff3e11800000000
.quad 0x3ff3e48be0000000
.quad 0x3ff3e7fe80000000
.quad 0x3ff3eb7000000000
.quad 0x3ff3eee040000000
.quad 0x3ff3f24f60000000
.quad 0x3ff3f5bd40000000
.quad 0x3ff3f92a00000000
.quad 0x3ff3fc9590000000
.quad 0x3ff3fffff0000000
.quad 0x3ff4036930000000
.quad 0x3ff406d140000000
.quad 0x3ff40a3830000000
.quad 0x3ff40d9df0000000
.quad 0x3ff4110290000000
.quad 0x3ff4146600000000
.quad 0x3ff417c850000000
.quad 0x3ff41b2980000000
.quad 0x3ff41e8990000000
.quad 0x3ff421e880000000
.quad 0x3ff4254640000000
.quad 0x3ff428a2f0000000

.align 32
.L__CBRT_F_T_256: .quad 0x0000000000000000
.quad 0x3e6e6a24c81e4294
.quad 0x3e58548511e3a785
.quad 0x3e64eb9336ec07f6
.quad 0x3e40ea64b8b750e1
.quad 0x3e461637cff8a53c
.quad 0x3e40733bf7bd1943
.quad 0x3e5666911345cced
.quad 0x3e477b7a3f592f14
.quad 0x3e6f18d3dd1a5402
.quad 0x3e2be2f5a58ee9a4
.quad 0x3e68901f8f085fa7
.quad 0x3e5c68b8cd5b5d69
.quad 0x3e5a6b0e8624be42
.quad 0x3dbc4b22b06f68e7
.quad 0x3e60f3f0afcabe9b
.quad 0x3e548495bca4e1b7
.quad 0x3e66107f1abdfdc3
.quad 0x3e6e67261878288a
.quad 0x3e5a6bc155286f1e
.quad 0x3e58a759c64a85f2
.quad 0x3e45fce70a4a8d09
.quad 0x3e32f9cbf373fe1d
.quad 0x3e590564ce4ac359
.quad 0x3e5ac29ce761b02f
.quad 0x3e5cb752f497381c
.quad 0x3e68bb9e1cfb35e0
.quad 0x3e65b4917099de90
.quad 0x3e5cc77ac9c65ef2
.quad 0x3e57a0f3e7be3dba
.quad 0x3e66ec851ee0c16f
.quad 0x3e689449bf2946da
.quad 0x3e698f25301ba223
.quad 0x3e347d5ec651f549
.quad 0x3e6c33ec9a86007a
.quad 0x3e5e0b6653e92649
.quad 0x3e3bd64ac09d755f
.quad 0x3e2f537506f78167
.quad 0x3e62c382d1b3735e
.quad 0x3e6e20ed659f99e1
.quad 0x3e586b633a9c182a
.quad 0x3e445cfd5a65e777
.quad 0x3e60c8770f58bca4
.quad 0x3e6739e44b0933c5
.quad 0x3e027dc3d9ce7bd8
.quad 0x3e63c53c7c5a7b64
.quad 0x3e69669683830cec
.quad 0x3e68d772c39bdcc4
.quad 0x3e69b0008bcf6d7b
.quad 0x3e3bbb305825ce4f
.quad 0x3e6da3f4af13a406
.quad 0x3e5f36b96f74ce86
.quad 0x3e165c002303f790
.quad 0x3e682f84095ba7d5
.quad 0x3e6d46433541b2c6
.quad 0x3e671c3d56e93a89
.quad 0x3e598dcef4e40012
.quad 0x3e4530ebef17fe03
.quad 0x3e4e8b8fa3715066
.quad 0x3e6ab26eb3b211dc
.quad 0x3e454dd4dc906307
.quad 0x3e5c9f962387984e
.quad 0x3e6c62a959afec09
.quad 0x3e6638d9ac6a866a
.quad 0x3e338704eca8a22d
.quad 0x3e4e6c9e1db14f8f
.quad 0x3e58744b7f9c9eaa
.quad 0x3e66c2893486373b
.quad 0x3e5b36bce31699b7
.quad 0x3e671e3813d200c7
.quad 0x3e699755ab40aa88
.quad 0x3e6b45ca0e4bcfc0
.quad 0x3e32dd090d869c5d
.quad 0x3e64fe0516b917da
.quad 0x3e694563226317a2
.quad 0x3e653d8fafc2c851
.quad 0x3e5dcbd41fbd41a3
.quad 0x3e5862ff5285f59c
.quad 0x3e63072ea97a1e1c
.quad 0x3e52839075184805
.quad 0x3e64b0323e9eff42
.quad 0x3e6b158893c45484
.quad 0x3e3149ef0fc35826
.quad 0x3e5f2e77ea96acaa
.quad 0x3e5200074c471a95
.quad 0x3e63f8cc517f6f04
.quad 0x3e660ba2e311bb55
.quad 0x3e64b788730bbec3
.quad 0x3e657090795ee20c
.quad 0x3e6d9ffe983670b1
.quad 0x3e62a463ff61bfda
.quad 0x3e69d1bc6a5e65cf
.quad 0x3e68718abaa9e922
.quad 0x3e63c2f52ffa342e
.quad 0x3e60fae13ff42c80
.quad 0x3e65440f0ef00d57
.quad 0x3e46fcd22d4e3c1e
.quad 0x3e4e0c60b409e863
.quad 0x3e6f9cab5a5f0333
.quad 0x3e630f24744c333d
.quad 0x3e4b50622a76b2fe
.quad 0x3e6fdb94ba595375
.quad 0x3e3861b9b945a171
.quad 0x3e654348015188c4
.quad 0x3e6b54d149865523
.quad 0x3e6a0bb783d9de33
.quad 0x3e6629d12b1a2157
.quad 0x3e6467fe35d179df
.quad 0x3e69763f3e26c8f7
.quad 0x3e53f798bb9f7679
.quad 0x3e552e577e855898
.quad 0x3e6fde47e5502c3a
.quad 0x3e5cbd0b548d96a0
.quad 0x3e6a9cd9f7be8de8
.quad 0x3e522bbe704886de
.quad 0x3e6e3dea8317f020
.quad 0x3e6e812085ac8855
.quad 0x3e5c87144f24cb07
.quad 0x3e61e128ee311fa2
.quad 0x3e5b5c163d61a2d3
.quad 0x3e47d97e7fb90633
.quad 0x3e6efe899d50f6a7
.quad 0x3e6d0333eb75de5a
.quad 0x3e40e590be73a573
.quad 0x3e68ce8dcac3cdd2
.quad 0x3e6ee8a48954064b
.quad 0x3e6aa62f18461e09
.quad 0x3e601e5940986a15
.quad 0x3e3b082f4f9b8d4c
.quad 0x3e6876e0e5527f5a
.quad 0x3e63617080831e6b
.quad 0x3e681b26e34aa4a2
.quad 0x3e552ee66dfab0c1
.quad 0x3e5d85a5329e8819
.quad 0x3e5105c1b646b5d1
.quad 0x3e6bb6690c1a379c
.quad 0x3e586aeba73ce3a9
.quad 0x3e6dd16198294dd4
.quad 0x3e6454e675775e83
.quad 0x3e63842e026197ea
.quad 0x3e6f1ce0e70c44d2
.quad 0x3e6ad636441a5627
.quad 0x3e54c205d7212abb
.quad 0x3e6167c86c116419
.quad 0x3e638ec3ef16e294
.quad 0x3e6473fceace9321
.quad 0x3e67af53a836dba7
.quad 0x3e1a51f3c383b652
.quad 0x3e63696da190822d
.quad 0x3e62f9adec77074b
.quad 0x3e38190fd5bee55f
.quad 0x3e4bfee8fac68e55
.quad 0x3e331c9d6bc5f68a
.quad 0x3e689d0523737edf
.quad 0x3e5a295943bf47bb
.quad 0x3e396be32e5b3207
.quad 0x3e6e44c7d909fa0e
.quad 0x3e2b2505da94d9ea
.quad 0x3e60c851f46c9c98
.quad 0x3e5da71f7d9aa3b7
.quad 0x3e6f1b605d019ef1
.quad 0x3e4386e8a2189563
.quad 0x3e3b19fa5d306ba7
.quad 0x3e6dd749b67aef76
.quad 0x3e676ff6f1dc04b0
.quad 0x3e635a33d0b232a6
.quad 0x3e64bdc80024a4e1
.quad 0x3e6ebd61770fd723
.quad 0x3e64769fc537264d
.quad 0x3e69021f429f3b98
.quad 0x3e5ee7083efbd606
.quad 0x3e6ad985552a6b1a
.quad 0x3e6e3df778772160
.quad 0x3e6ca5d76ddc9b34
.quad 0x3e691154ffdbaf74
.quad 0x3e667bdd57fb306a
.quad 0x3e67dc255ac40886
.quad 0x3df219f38e8afafe
.quad 0x3e62416bf9669a04
.quad 0x3e611c96b2b3987f
.quad 0x3e6f99ed447e1177
.quad 0x3e13245826328a11
.quad 0x3e66f56dd1e645f8
.quad 0x3e46164946945535
.quad 0x3e5e37d59d190028
.quad 0x3e668671f12bf828
.quad 0x3e6e8ecbca6aabbd
.quad 0x3e53f49e109a5912
.quad 0x3e6b8a0e11ec3043
.quad 0x3e65fae00aed691a
.quad 0x3e6c0569bece3e4a
.quad 0x3e605e26744efbfe
.quad 0x3e65b570a94be5c5
.quad 0x3e5d6f156ea0e063
.quad 0x3e6e0ca7612fc484
.quad 0x3e4963c927b25258
.quad 0x3e547930aa725a5c
.quad 0x3e58a79fe3af43b3
.quad 0x3e5e6dc29c41bdaf
.quad 0x3e657a2e76f863a5
.quad 0x3e2ae3b61716354d
.quad 0x3e665fb5df6906b1
.quad 0x3e66177d7f588f7b
.quad 0x3e3ad55abd091b67
.quad 0x3e155337b2422d76
.quad 0x3e6084ebe86972d5
.quad 0x3e656395808e1ea3
.quad 0x3e61bce21b40fba7
.quad 0x3e5006f94605b515
.quad 0x3e6aa676aceb1f7d
.quad 0x3e58229f76554ce6
.quad 0x3e6eabfc6cf57330
.quad 0x3e64daed9c0ce8bc
.quad 0x3e60ff1768237141
.quad 0x3e6575f83051b085
.quad 0x3e42667deb523e29
.quad 0x3e1816996954f4fd
.quad 0x3e587cfccf4d9cd4
.quad 0x3e52c5d018198353
.quad 0x3e6a7a898dcc34aa
.quad 0x3e2cead6dadc36d1
.quad 0x3e2a55759c498bdf
.quad 0x3e6c414a9ef6de04
.quad 0x3e63e2108a6e58fa
.quad 0x3e5587fd7643d77c
.quad 0x3e3901eb1d3ff3df
.quad 0x3e6f2ccd7c812fc6
.quad 0x3e21c8ee70a01049
.quad 0x3e563e8d02831eec
.quad 0x3e6f61a42a92c7ff
.quad 0x3dda917399c84d24
.quad 0x3e5e9197c8eec2f0
.quad 0x3e5e6f842f5a1378
.quad 0x3e2fac242a90a0fc
.quad 0x3e535ed726610227
.quad 0x3e50e0d64804b15b
.quad 0x3e0560675daba814
.quad 0x3e637388c8768032
.quad 0x3e3ee3c89f9e01f5
.quad 0x3e639f6f0d09747c
.quad 0x3e4322c327abb8f0
.quad 0x3e6961b347c8ac80
.quad 0x3e63711fbbd0f118
.quad 0x3e64fad8d7718ffb
.quad 0x3e6fffffffffffff
.quad 0x3e667efa79ec35b4
.quad 0x3e6a737687a254a8
.quad 0x3e5bace0f87d924d
.quad 0x3e629e37c237e392
.quad 0x3e557ce7ac3f3012
.quad 0x3e682829359f8fbd
.quad 0x3e6cc9be42d14676
.quad 0x3e6a8f001c137d0b
.quad 0x3e636127687dda05
.quad 0x3e524dba322646f0
.quad 0x3e6dc43f1ed210b4
.quad 0x3e631ae515c447bb

.align 32
.L__INV_TAB_256: .quad 0x4000000000000000
.quad 0x3fffe01fe01fe020
.quad 0x3fffc07f01fc07f0
.quad 0x3fffa11caa01fa12
.quad 0x3fff81f81f81f820
.quad 0x3fff6310aca0dbb5
.quad 0x3fff44659e4a4271
.quad 0x3fff25f644230ab5
.quad 0x3fff07c1f07c1f08
.quad 0x3ffee9c7f8458e02
.quad 0x3ffecc07b301ecc0
.quad 0x3ffeae807aba01eb
.quad 0x3ffe9131abf0b767
.quad 0x3ffe741aa59750e4
.quad 0x3ffe573ac901e574
.quad 0x3ffe3a9179dc1a73
.quad 0x3ffe1e1e1e1e1e1e
.quad 0x3ffe01e01e01e01e
.quad 0x3ffde5d6e3f8868a
.quad 0x3ffdca01dca01dca
.quad 0x3ffdae6076b981db
.quad 0x3ffd92f2231e7f8a
.quad 0x3ffd77b654b82c34
.quad 0x3ffd5cac807572b2
.quad 0x3ffd41d41d41d41d
.quad 0x3ffd272ca3fc5b1a
.quad 0x3ffd0cb58f6ec074
.quad 0x3ffcf26e5c44bfc6
.quad 0x3ffcd85689039b0b
.quad 0x3ffcbe6d9601cbe7
.quad 0x3ffca4b3055ee191
.quad 0x3ffc8b265afb8a42
.quad 0x3ffc71c71c71c71c
.quad 0x3ffc5894d10d4986
.quad 0x3ffc3f8f01c3f8f0
.quad 0x3ffc26b5392ea01c
.quad 0x3ffc0e070381c0e0
.quad 0x3ffbf583ee868d8b
.quad 0x3ffbdd2b899406f7
.quad 0x3ffbc4fd65883e7b
.quad 0x3ffbacf914c1bad0
.quad 0x3ffb951e2b18ff23
.quad 0x3ffb7d6c3dda338b
.quad 0x3ffb65e2e3beee05
.quad 0x3ffb4e81b4e81b4f
.quad 0x3ffb37484ad806ce
.quad 0x3ffb2036406c80d9
.quad 0x3ffb094b31d922a4
.quad 0x3ffaf286bca1af28
.quad 0x3ffadbe87f94905e
.quad 0x3ffac5701ac5701b
.quad 0x3ffaaf1d2f87ebfd
.quad 0x3ffa98ef606a63be
.quad 0x3ffa82e65130e159
.quad 0x3ffa6d01a6d01a6d
.quad 0x3ffa574107688a4a
.quad 0x3ffa41a41a41a41a
.quad 0x3ffa2c2a87c51ca0
.quad 0x3ffa16d3f97a4b02
.quad 0x3ffa01a01a01a01a
.quad 0x3ff9ec8e951033d9
.quad 0x3ff9d79f176b682d
.quad 0x3ff9c2d14ee4a102
.quad 0x3ff9ae24ea5510da
.quad 0x3ff999999999999a
.quad 0x3ff9852f0d8ec0ff
.quad 0x3ff970e4f80cb872
.quad 0x3ff95cbb0be377ae
.quad 0x3ff948b0fcd6e9e0
.quad 0x3ff934c67f9b2ce6
.quad 0x3ff920fb49d0e229
.quad 0x3ff90d4f120190d5
.quad 0x3ff8f9c18f9c18fa
.quad 0x3ff8e6527af1373f
.quad 0x3ff8d3018d3018d3
.quad 0x3ff8bfce8062ff3a
.quad 0x3ff8acb90f6bf3aa
.quad 0x3ff899c0f601899c
.quad 0x3ff886e5f0abb04a
.quad 0x3ff87427bcc092b9
.quad 0x3ff8618618618618
.quad 0x3ff84f00c2780614
.quad 0x3ff83c977ab2bedd
.quad 0x3ff82a4a0182a4a0
.quad 0x3ff8181818181818
.quad 0x3ff8060180601806
.quad 0x3ff7f405fd017f40
.quad 0x3ff7e225515a4f1d
.quad 0x3ff7d05f417d05f4
.quad 0x3ff7beb3922e017c
.quad 0x3ff7ad2208e0ecc3
.quad 0x3ff79baa6bb6398b
.quad 0x3ff78a4c8178a4c8
.quad 0x3ff77908119ac60d
.quad 0x3ff767dce434a9b1
.quad 0x3ff756cac201756d
.quad 0x3ff745d1745d1746
.quad 0x3ff734f0c541fe8d
.quad 0x3ff724287f46debc
.quad 0x3ff713786d9c7c09
.quad 0x3ff702e05c0b8170
.quad 0x3ff6f26016f26017
.quad 0x3ff6e1f76b4337c7
.quad 0x3ff6d1a62681c861
.quad 0x3ff6c16c16c16c17
.quad 0x3ff6b1490aa31a3d
.quad 0x3ff6a13cd1537290
.quad 0x3ff691473a88d0c0
.quad 0x3ff6816816816817
.quad 0x3ff6719f3601671a
.quad 0x3ff661ec6a5122f9
.quad 0x3ff6524f853b4aa3
.quad 0x3ff642c8590b2164
.quad 0x3ff63356b88ac0de
.quad 0x3ff623fa77016240
.quad 0x3ff614b36831ae94
.quad 0x3ff6058160581606
.quad 0x3ff5f66434292dfc
.quad 0x3ff5e75bb8d015e7
.quad 0x3ff5d867c3ece2a5
.quad 0x3ff5c9882b931057
.quad 0x3ff5babcc647fa91
.quad 0x3ff5ac056b015ac0
.quad 0x3ff59d61f123ccaa
.quad 0x3ff58ed2308158ed
.quad 0x3ff5805601580560
.quad 0x3ff571ed3c506b3a
.quad 0x3ff56397ba7c52e2
.quad 0x3ff5555555555555
.quad 0x3ff54725e6bb82fe
.quad 0x3ff5390948f40feb
.quad 0x3ff52aff56a8054b
.quad 0x3ff51d07eae2f815
.quad 0x3ff50f22e111c4c5
.quad 0x3ff5015015015015
.quad 0x3ff4f38f62dd4c9b
.quad 0x3ff4e5e0a72f0539
.quad 0x3ff4d843bedc2c4c
.quad 0x3ff4cab88725af6e
.quad 0x3ff4bd3edda68fe1
.quad 0x3ff4afd6a052bf5b
.quad 0x3ff4a27fad76014a
.quad 0x3ff49539e3b2d067
.quad 0x3ff4880522014880
.quad 0x3ff47ae147ae147b
.quad 0x3ff46dce34596066
.quad 0x3ff460cbc7f5cf9a
.quad 0x3ff453d9e2c776ca
.quad 0x3ff446f86562d9fb
.quad 0x3ff43a2730abee4d
.quad 0x3ff42d6625d51f87
.quad 0x3ff420b5265e5951
.quad 0x3ff4141414141414
.quad 0x3ff40782d10e6566
.quad 0x3ff3fb013fb013fb
.quad 0x3ff3ee8f42a5af07
.quad 0x3ff3e22cbce4a902
.quad 0x3ff3d5d991aa75c6
.quad 0x3ff3c995a47babe7
.quad 0x3ff3bd60d9232955
.quad 0x3ff3b13b13b13b14
.quad 0x3ff3a524387ac822
.quad 0x3ff3991c2c187f63
.quad 0x3ff38d22d366088e
.quad 0x3ff3813813813814
.quad 0x3ff3755bd1c945ee
.quad 0x3ff3698df3de0748
.quad 0x3ff35dce5f9f2af8
.quad 0x3ff3521cfb2b78c1
.quad 0x3ff34679ace01346
.quad 0x3ff33ae45b57bcb2
.quad 0x3ff32f5ced6a1dfa
.quad 0x3ff323e34a2b10bf
.quad 0x3ff3187758e9ebb6
.quad 0x3ff30d190130d190
.quad 0x3ff301c82ac40260
.quad 0x3ff2f684bda12f68
.quad 0x3ff2eb4ea1fed14b
.quad 0x3ff2e025c04b8097
.quad 0x3ff2d50a012d50a0
.quad 0x3ff2c9fb4d812ca0
.quad 0x3ff2bef98e5a3711
.quad 0x3ff2b404ad012b40
.quad 0x3ff2a91c92f3c105
.quad 0x3ff29e4129e4129e
.quad 0x3ff293725bb804a5
.quad 0x3ff288b01288b013
.quad 0x3ff27dfa38a1ce4d
.quad 0x3ff27350b8812735
.quad 0x3ff268b37cd60127
.quad 0x3ff25e22708092f1
.quad 0x3ff2539d7e9177b2
.quad 0x3ff2492492492492
.quad 0x3ff23eb79717605b
.quad 0x3ff23456789abcdf
.quad 0x3ff22a0122a0122a
.quad 0x3ff21fb78121fb78
.quad 0x3ff21579804855e6
.quad 0x3ff20b470c67c0d9
.quad 0x3ff2012012012012
.quad 0x3ff1f7047dc11f70
.quad 0x3ff1ecf43c7fb84c
.quad 0x3ff1e2ef3b3fb874
.quad 0x3ff1d8f5672e4abd
.quad 0x3ff1cf06ada2811d
.quad 0x3ff1c522fc1ce059
.quad 0x3ff1bb4a4046ed29
.quad 0x3ff1b17c67f2bae3
.quad 0x3ff1a7b9611a7b96
.quad 0x3ff19e0119e0119e
.quad 0x3ff19453808ca29c
.quad 0x3ff18ab083902bdb
.quad 0x3ff1811811811812
.quad 0x3ff1778a191bd684
.quad 0x3ff16e0689427379
.quad 0x3ff1648d50fc3201
.quad 0x3ff15b1e5f75270d
.quad 0x3ff151b9a3fdd5c9
.quad 0x3ff1485f0e0acd3b
.quad 0x3ff13f0e8d344724
.quad 0x3ff135c81135c811
.quad 0x3ff12c8b89edc0ac
.quad 0x3ff12358e75d3033
.quad 0x3ff11a3019a74826
.quad 0x3ff1111111111111
.quad 0x3ff107fbbe011080
.quad 0x3ff0fef010fef011
.quad 0x3ff0f5edfab325a2
.quad 0x3ff0ecf56be69c90
.quad 0x3ff0e40655826011
.quad 0x3ff0db20a88f4696
.quad 0x3ff0d24456359e3a
.quad 0x3ff0c9714fbcda3b
.quad 0x3ff0c0a7868b4171
.quad 0x3ff0b7e6ec259dc8
.quad 0x3ff0af2f722eecb5
.quad 0x3ff0a6810a6810a7
.quad 0x3ff09ddba6af8360
.quad 0x3ff0953f39010954
.quad 0x3ff08cabb37565e2
.quad 0x3ff0842108421084
.quad 0x3ff07b9f29b8eae2
.quad 0x3ff073260a47f7c6
.quad 0x3ff06ab59c7912fb
.quad 0x3ff0624dd2f1a9fc
.quad 0x3ff059eea0727586
.quad 0x3ff05197f7d73404
.quad 0x3ff04949cc1664c5
.quad 0x3ff0410410410410
.quad 0x3ff038c6b78247fc
.quad 0x3ff03091b51f5e1a
.quad 0x3ff02864fc7729e9
.quad 0x3ff0204081020408
.quad 0x3ff0182436517a37
.quad 0x3ff0101010101010
.quad 0x3ff0080402010080
.quad 0x3ff0000000000000
