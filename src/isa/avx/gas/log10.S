#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
#

#
# log10.S
#
# An implementation of the log10 libm function.
#
# Prototype:
#
#     double log10(double x);
#

#
#   Algorithm:
#       Similar to one presnted in log.S
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(log10)
#define fname_special _log10_special@PLT



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

    sub         $stack_size, %rsp

    # compute exponent part
    xor         %rax, %rax
    movdqa      %xmm0, %xmm3
    movsd       %xmm0, %xmm4
    psrlq       $52, %xmm3
    movd        %xmm0, %rax
    psubq       .L__mask_1023(%rip), %xmm3
    movdqa      %xmm0, %xmm2
    cvtdq2pd    %xmm3, %xmm6 # xexp

    #  NaN or inf
    movdqa      %xmm0, %xmm5
    andpd       .L__real_inf(%rip), %xmm5
    comisd      .L__real_inf(%rip), %xmm5
    je          .L__x_is_inf_or_nan

    # check for negative numbers or zero
    xorpd       %xmm5, %xmm5
    comisd      %xmm5, %xmm0
    jbe         .L__x_is_zero_or_neg

    pand        .L__real_mant(%rip), %xmm2
    subsd       .L__real_one(%rip), %xmm4

    comisd      .L__mask_1023_f(%rip), %xmm6
    je          .L__denormal_adjust

.L__continue_common:    

    # compute index into the log tables
    mov         %rax, %r9
    and         .L__mask_mant_all8(%rip), %rax
    and         .L__mask_mant9(%rip), %r9
    shl         $1, %r9
    add         %r9, %rax
    mov         %rax, p_temp(%rsp)

    # near one codepath
    andpd       .L__real_notsign(%rip), %xmm4
    comisd      .L__real_threshold(%rip), %xmm4
    jb          .L__near_one

    # F, Y
    movsd       p_temp(%rsp), %xmm1
    shr         $44, %rax
    por         .L__real_half(%rip), %xmm2
    por         .L__real_half(%rip), %xmm1
    lea         .L__log_F_inv(%rip), %r9

    # f = F - Y, r = f * inv
    subsd       %xmm2, %xmm1
    mulsd       (%r9,%rax,8), %xmm1

    movsd       %xmm1, %xmm2
    movsd       %xmm1, %xmm0
    lea         .L__log_256_lead(%rip), %r9

    # poly
    movsd       .L__real_1_over_6(%rip), %xmm3
    movsd       .L__real_1_over_3(%rip), %xmm1
    mulsd       %xmm2, %xmm3                         
    mulsd       %xmm2, %xmm1                         
    mulsd       %xmm2, %xmm0                         
    movsd       %xmm0, %xmm4
    addsd       .L__real_1_over_5(%rip), %xmm3
    addsd       .L__real_1_over_2(%rip), %xmm1
    mulsd       %xmm0, %xmm4                         
    mulsd       %xmm2, %xmm3                         
    mulsd       %xmm0, %xmm1                         
    addsd       .L__real_1_over_4(%rip), %xmm3
    addsd       %xmm2, %xmm1                         
    mulsd       %xmm4, %xmm3                         
    addsd       %xmm3, %xmm1                         

    mulsd       .L__real_log10_e(%rip), %xmm1

    # m*log(10) + log10(G) - poly
    movsd       .L__real_log10_2_tail(%rip), %xmm5
    mulsd       %xmm6, %xmm5
    subsd       %xmm1, %xmm5

    movsd       (%r9,%rax,8), %xmm0
    lea         .L__log_256_tail(%rip), %rdx
    movsd       (%rdx,%rax,8), %xmm2
    addsd       %xmm5, %xmm2

    movsd       .L__real_log10_2_lead(%rip), %xmm4
    mulsd       %xmm6, %xmm4
    addsd       %xmm4, %xmm0

    addsd       %xmm2, %xmm0

    add         $stack_size, %rsp
    ret

.p2align 4,,15
.L__near_one:

    # r = x - 1.0
    movsd       .L__real_two(%rip), %xmm2
    subsd       .L__real_one(%rip), %xmm0 # r

    addsd       %xmm0, %xmm2
    movsd       %xmm0, %xmm1
    divsd       %xmm2, %xmm1 # r/(2+r) = u/2

    movsd       .L__real_ca2(%rip), %xmm4
    movsd       .L__real_ca4(%rip), %xmm5

    movsd       %xmm0, %xmm6
    mulsd       %xmm1, %xmm6 # correction

    addsd       %xmm1, %xmm1 # u
    movsd       %xmm1, %xmm2

    mulsd       %xmm1, %xmm2 # u^2

    mulsd       %xmm2, %xmm4
    mulsd       %xmm2, %xmm5

    addsd       .L__real_ca1(%rip), %xmm4
    addsd       .L__real_ca3(%rip), %xmm5

    mulsd       %xmm1, %xmm2 # u^3
    mulsd       %xmm2, %xmm4

    mulsd       %xmm2, %xmm2
    mulsd       %xmm1, %xmm2 # u^7
    mulsd       %xmm2, %xmm5

    addsd       %xmm5, %xmm4
    subsd       %xmm6, %xmm4

    movdqa      %xmm0, %xmm3
    pand        .L__mask_lower(%rip), %xmm3
    subsd       %xmm3, %xmm0
    addsd       %xmm0, %xmm4

    movsd       %xmm3, %xmm0
    movsd       %xmm4, %xmm1

    mulsd       .L__real_log10_e_tail(%rip), %xmm4
    mulsd       .L__real_log10_e_tail(%rip), %xmm0
    mulsd       .L__real_log10_e_lead(%rip), %xmm1
    mulsd       .L__real_log10_e_lead(%rip), %xmm3

    addsd       %xmm4, %xmm0
    addsd       %xmm1, %xmm0
    addsd       %xmm3, %xmm0

    add         $stack_size, %rsp
    ret

.L__denormal_adjust:
    por         .L__real_one(%rip), %xmm2
    subsd       .L__real_one(%rip), %xmm2
    movsd       %xmm2, %xmm5
    pand        .L__real_mant(%rip), %xmm2
    movd        %xmm2, %rax
    psrlq       $52, %xmm5
    psubd       .L__mask_2045(%rip), %xmm5
    cvtdq2pd    %xmm5, %xmm6
    jmp         .L__continue_common

.p2align 4,,15
.L__x_is_zero_or_neg:
    jne         .L__x_is_neg

    movsd       .L__real_ninf(%rip), %xmm1
    mov         .L__flag_x_zero(%rip), %edi
    call        fname_special
    jmp         .L__finish

.p2align 4,,15
.L__x_is_neg:

    movsd       .L__real_qnan(%rip), %xmm1
    mov         .L__flag_x_neg(%rip), %edi
    call        fname_special
    jmp         .L__finish

.p2align 4,,15
.L__x_is_inf_or_nan:

    cmp         .L__real_inf(%rip), %rax
    je          .L__finish

    cmp         .L__real_ninf(%rip), %rax
    je          .L__x_is_neg

    mov         .L__real_qnanbit(%rip), %r9
    and         %rax, %r9
    jnz         .L__finish

    or          .L__real_qnanbit(%rip), %rax
    movd        %rax, %xmm1
    mov         .L__flag_x_nan(%rip), %edi
    call        fname_special
    jmp         .L__finish    

.p2align 4,,15
.L__finish:
    add         $stack_size, %rsp
    ret


.section .rodata

.align 16

# these codes and the ones in the corresponding .c file have to match
.L__flag_x_zero:        .long 00000001
.L__flag_x_neg:         .long 00000002
.L__flag_x_nan:         .long 00000003

.align 16

.L__real_ninf:      .quad 0x0fff0000000000000   # -inf
                    .quad 0x0000000000000000
.L__real_inf:       .quad 0x7ff0000000000000    # +inf
                    .quad 0x0000000000000000
.L__real_qnan:      .quad 0x7ff8000000000000   # qNaN
                    .quad 0x0000000000000000
.L__real_qnanbit:   .quad 0x0008000000000000
                    .quad 0x0000000000000000
.L__real_mant:      .quad 0x000FFFFFFFFFFFFF    # mantissa bits
                    .quad 0x0000000000000000
.L__mask_1023:      .quad 0x00000000000003ff
                    .quad 0x0000000000000000
.L__mask_001:       .quad 0x0000000000000001
                    .quad 0x0000000000000000

.L__mask_mant_all8:     .quad 0x000ff00000000000
                        .quad 0x0000000000000000
.L__mask_mant9:         .quad 0x0000080000000000
                        .quad 0x0000000000000000

.L__real_log10_e:       .quad 0x3fdbcb7b1526e50e
                        .quad 0x0000000000000000

.L__real_log10_e_lead:  .quad 0x3fdbcb7800000000 # log10e_lead 4.34293746948242187500e-01
                        .quad 0x0000000000000000
.L__real_log10_e_tail:  .quad 0x3ea8a93728719535 # log10e_tail 7.3495500964015109100644e-7
                        .quad 0x0000000000000000

.L__real_log10_2_lead:  .quad 0x3fd3441350000000
                        .quad 0x0000000000000000
.L__real_log10_2_tail:  .quad 0x3e03ef3fde623e25
                        .quad 0x0000000000000000




.L__real_two:       .quad 0x4000000000000000 # 2
                    .quad 0x0000000000000000

.L__real_one:       .quad 0x3ff0000000000000 # 1
                    .quad 0x0000000000000000

.L__real_half:      .quad 0x3fe0000000000000 # 1/2
                    .quad 0x0000000000000000

.L__mask_100:       .quad 0x0000000000000100
                    .quad 0x0000000000000000

.L__real_1_over_512:    .quad 0x3f60000000000000
                        .quad 0x0000000000000000

.L__real_1_over_2:  .quad 0x3fe0000000000000
                    .quad 0x0000000000000000
.L__real_1_over_3:  .quad 0x3fd5555555555555
                    .quad 0x0000000000000000
.L__real_1_over_4:  .quad 0x3fd0000000000000
                    .quad 0x0000000000000000
.L__real_1_over_5:  .quad 0x3fc999999999999a
                    .quad 0x0000000000000000
.L__real_1_over_6:  .quad 0x3fc5555555555555
                    .quad 0x0000000000000000

.L__mask_1023_f:    .quad 0x0c08ff80000000000
                    .quad 0x0000000000000000

.L__mask_2045:      .quad 0x00000000000007fd
                    .quad 0x0000000000000000

.L__real_threshold: .quad 0x3fb0000000000000 # .0625
                    .quad 0x0000000000000000

.L__real_notsign:   .quad 0x7ffFFFFFFFFFFFFF # ^sign bit
                    .quad 0x0000000000000000

.L__real_ca1:       .quad 0x3fb55555555554e6 # 8.33333333333317923934e-02
                    .quad 0x0000000000000000
.L__real_ca2:       .quad 0x3f89999999bac6d4 # 1.25000000037717509602e-02
                    .quad 0x0000000000000000
.L__real_ca3:       .quad 0x3f62492307f1519f # 2.23213998791944806202e-03
                    .quad 0x0000000000000000
.L__real_ca4:       .quad 0x3f3c8034c85dfff0 # 4.34887777707614552256e-04
                    .quad 0x0000000000000000

.L__mask_lower:     .quad 0x0ffffffff00000000
                    .quad 0x0000000000000000

.align 16
.L__log_256_lead:
                    .quad 0x0000000000000000
                    .quad 0x3f5bbd9e90000000
                    .quad 0x3f6bafd470000000
                    .quad 0x3f74b99560000000
                    .quad 0x3f7b9476a0000000
                    .quad 0x3f81344da0000000
                    .quad 0x3f849b0850000000
                    .quad 0x3f87fe71c0000000
                    .quad 0x3f8b5e9080000000
                    .quad 0x3f8ebb6af0000000
                    .quad 0x3f910a83a0000000
                    .quad 0x3f92b5b5e0000000
                    .quad 0x3f945f4f50000000
                    .quad 0x3f96075300000000
                    .quad 0x3f97adc3d0000000
                    .quad 0x3f9952a4f0000000
                    .quad 0x3f9af5f920000000
                    .quad 0x3f9c97c370000000
                    .quad 0x3f9e3806a0000000
                    .quad 0x3f9fd6c5b0000000
                    .quad 0x3fa0ba01a0000000
                    .quad 0x3fa187e120000000
                    .quad 0x3fa25502c0000000
                    .quad 0x3fa32167c0000000
                    .quad 0x3fa3ed1190000000
                    .quad 0x3fa4b80180000000
                    .quad 0x3fa58238e0000000
                    .quad 0x3fa64bb910000000
                    .quad 0x3fa7148340000000
                    .quad 0x3fa7dc98c0000000
                    .quad 0x3fa8a3fad0000000
                    .quad 0x3fa96aaac0000000
                    .quad 0x3faa30a9d0000000
                    .quad 0x3faaf5f920000000
                    .quad 0x3fabba9a00000000
                    .quad 0x3fac7e8d90000000
                    .quad 0x3fad41d510000000
                    .quad 0x3fae0471a0000000
                    .quad 0x3faec66470000000
                    .quad 0x3faf87aeb0000000
                    .quad 0x3fb02428c0000000
                    .quad 0x3fb08426f0000000
                    .quad 0x3fb0e3d290000000
                    .quad 0x3fb1432c30000000
                    .quad 0x3fb1a23440000000
                    .quad 0x3fb200eb60000000
                    .quad 0x3fb25f5210000000
                    .quad 0x3fb2bd68e0000000
                    .quad 0x3fb31b3050000000
                    .quad 0x3fb378a8e0000000
                    .quad 0x3fb3d5d330000000
                    .quad 0x3fb432afa0000000
                    .quad 0x3fb48f3ed0000000
                    .quad 0x3fb4eb8120000000
                    .quad 0x3fb5477730000000
                    .quad 0x3fb5a32160000000
                    .quad 0x3fb5fe8040000000
                    .quad 0x3fb6599440000000
                    .quad 0x3fb6b45df0000000
                    .quad 0x3fb70eddb0000000
                    .quad 0x3fb7691400000000
                    .quad 0x3fb7c30160000000
                    .quad 0x3fb81ca630000000
                    .quad 0x3fb8760300000000
                    .quad 0x3fb8cf1830000000
                    .quad 0x3fb927e640000000
                    .quad 0x3fb9806d90000000
                    .quad 0x3fb9d8aea0000000
                    .quad 0x3fba30a9d0000000
                    .quad 0x3fba885fa0000000
                    .quad 0x3fbadfd070000000
                    .quad 0x3fbb36fcb0000000
                    .quad 0x3fbb8de4d0000000
                    .quad 0x3fbbe48930000000
                    .quad 0x3fbc3aea40000000
                    .quad 0x3fbc910870000000
                    .quad 0x3fbce6e410000000
                    .quad 0x3fbd3c7da0000000
                    .quad 0x3fbd91d580000000
                    .quad 0x3fbde6ec00000000
                    .quad 0x3fbe3bc1a0000000
                    .quad 0x3fbe9056b0000000
                    .quad 0x3fbee4aba0000000
                    .quad 0x3fbf38c0c0000000
                    .quad 0x3fbf8c9680000000
                    .quad 0x3fbfe02d30000000
                    .quad 0x3fc019c2a0000000
                    .quad 0x3fc0434f70000000
                    .quad 0x3fc06cbd60000000
                    .quad 0x3fc0960c80000000
                    .quad 0x3fc0bf3d00000000
                    .quad 0x3fc0e84f10000000
                    .quad 0x3fc11142f0000000
                    .quad 0x3fc13a18a0000000
                    .quad 0x3fc162d080000000
                    .quad 0x3fc18b6a90000000
                    .quad 0x3fc1b3e710000000
                    .quad 0x3fc1dc4630000000
                    .quad 0x3fc2048810000000
                    .quad 0x3fc22cace0000000
                    .quad 0x3fc254b4d0000000
                    .quad 0x3fc27c9ff0000000
                    .quad 0x3fc2a46e80000000
                    .quad 0x3fc2cc20b0000000
                    .quad 0x3fc2f3b690000000
                    .quad 0x3fc31b3050000000
                    .quad 0x3fc3428e20000000
                    .quad 0x3fc369d020000000
                    .quad 0x3fc390f680000000
                    .quad 0x3fc3b80160000000
                    .quad 0x3fc3def0e0000000
                    .quad 0x3fc405c530000000
                    .quad 0x3fc42c7e70000000
                    .quad 0x3fc4531cd0000000
                    .quad 0x3fc479a070000000
                    .quad 0x3fc4a00970000000
                    .quad 0x3fc4c65800000000
                    .quad 0x3fc4ec8c30000000
                    .quad 0x3fc512a640000000
                    .quad 0x3fc538a630000000
                    .quad 0x3fc55e8c50000000
                    .quad 0x3fc5845890000000
                    .quad 0x3fc5aa0b40000000
                    .quad 0x3fc5cfa470000000
                    .quad 0x3fc5f52440000000
                    .quad 0x3fc61a8ad0000000
                    .quad 0x3fc63fd850000000
                    .quad 0x3fc6650cd0000000
                    .quad 0x3fc68a2880000000
                    .quad 0x3fc6af2b80000000
                    .quad 0x3fc6d415e0000000
                    .quad 0x3fc6f8e7d0000000
                    .quad 0x3fc71da170000000
                    .quad 0x3fc74242e0000000
                    .quad 0x3fc766cc40000000
                    .quad 0x3fc78b3da0000000
                    .quad 0x3fc7af9730000000
                    .quad 0x3fc7d3d910000000
                    .quad 0x3fc7f80350000000
                    .quad 0x3fc81c1620000000
                    .quad 0x3fc8401190000000
                    .quad 0x3fc863f5c0000000
                    .quad 0x3fc887c2e0000000
                    .quad 0x3fc8ab7900000000
                    .quad 0x3fc8cf1830000000
                    .quad 0x3fc8f2a0a0000000
                    .quad 0x3fc9161270000000
                    .quad 0x3fc9396db0000000
                    .quad 0x3fc95cb280000000
                    .quad 0x3fc97fe100000000
                    .quad 0x3fc9a2f950000000
                    .quad 0x3fc9c5fb70000000
                    .quad 0x3fc9e8e7b0000000
                    .quad 0x3fca0bbdf0000000
                    .quad 0x3fca2e7e80000000
                    .quad 0x3fca512960000000
                    .quad 0x3fca73bea0000000
                    .quad 0x3fca963e70000000
                    .quad 0x3fcab8a8f0000000
                    .quad 0x3fcadafe20000000
                    .quad 0x3fcafd3e30000000
                    .quad 0x3fcb1f6930000000
                    .quad 0x3fcb417f40000000
                    .quad 0x3fcb638070000000
                    .quad 0x3fcb856cf0000000
                    .quad 0x3fcba744b0000000
                    .quad 0x3fcbc907f0000000
                    .quad 0x3fcbeab6c0000000
                    .quad 0x3fcc0c5130000000
                    .quad 0x3fcc2dd750000000
                    .quad 0x3fcc4f4950000000
                    .quad 0x3fcc70a740000000
                    .quad 0x3fcc91f130000000
                    .quad 0x3fccb32740000000
                    .quad 0x3fccd44980000000
                    .quad 0x3fccf55810000000
                    .quad 0x3fcd165300000000
                    .quad 0x3fcd373a60000000
                    .quad 0x3fcd580e60000000
                    .quad 0x3fcd78cf00000000
                    .quad 0x3fcd997c70000000
                    .quad 0x3fcdba16a0000000
                    .quad 0x3fcdda9dd0000000
                    .quad 0x3fcdfb11f0000000
                    .quad 0x3fce1b7330000000
                    .quad 0x3fce3bc1a0000000
                    .quad 0x3fce5bfd50000000
                    .quad 0x3fce7c2660000000
                    .quad 0x3fce9c3ce0000000
                    .quad 0x3fcebc40e0000000
                    .quad 0x3fcedc3280000000
                    .quad 0x3fcefc11d0000000
                    .quad 0x3fcf1bdee0000000
                    .quad 0x3fcf3b99d0000000
                    .quad 0x3fcf5b42a0000000
                    .quad 0x3fcf7ad980000000
                    .quad 0x3fcf9a5e70000000
                    .quad 0x3fcfb9d190000000
                    .quad 0x3fcfd932f0000000
                    .quad 0x3fcff882a0000000
                    .quad 0x3fd00be050000000
                    .quad 0x3fd01b76a0000000
                    .quad 0x3fd02b0430000000
                    .quad 0x3fd03a8910000000
                    .quad 0x3fd04a0540000000
                    .quad 0x3fd05978e0000000
                    .quad 0x3fd068e3f0000000
                    .quad 0x3fd0784670000000
                    .quad 0x3fd087a080000000
                    .quad 0x3fd096f210000000
                    .quad 0x3fd0a63b30000000
                    .quad 0x3fd0b57bf0000000
                    .quad 0x3fd0c4b450000000
                    .quad 0x3fd0d3e460000000
                    .quad 0x3fd0e30c30000000
                    .quad 0x3fd0f22bc0000000
                    .quad 0x3fd1014310000000
                    .quad 0x3fd1105240000000
                    .quad 0x3fd11f5940000000
                    .quad 0x3fd12e5830000000
                    .quad 0x3fd13d4f00000000
                    .quad 0x3fd14c3dd0000000
                    .quad 0x3fd15b24a0000000
                    .quad 0x3fd16a0370000000
                    .quad 0x3fd178da50000000
                    .quad 0x3fd187a940000000
                    .quad 0x3fd1967060000000
                    .quad 0x3fd1a52fa0000000
                    .quad 0x3fd1b3e710000000
                    .quad 0x3fd1c296c0000000
                    .quad 0x3fd1d13eb0000000
                    .quad 0x3fd1dfdef0000000
                    .quad 0x3fd1ee7770000000
                    .quad 0x3fd1fd0860000000
                    .quad 0x3fd20b91a0000000
                    .quad 0x3fd21a1350000000
                    .quad 0x3fd2288d70000000
                    .quad 0x3fd2370010000000
                    .quad 0x3fd2456b30000000
                    .quad 0x3fd253ced0000000
                    .quad 0x3fd2622b00000000
                    .quad 0x3fd2707fd0000000
                    .quad 0x3fd27ecd40000000
                    .quad 0x3fd28d1360000000
                    .quad 0x3fd29b5220000000
                    .quad 0x3fd2a989a0000000
                    .quad 0x3fd2b7b9e0000000
                    .quad 0x3fd2c5e2e0000000
                    .quad 0x3fd2d404b0000000
                    .quad 0x3fd2e21f50000000
                    .quad 0x3fd2f032c0000000
                    .quad 0x3fd2fe3f20000000
                    .quad 0x3fd30c4470000000
                    .quad 0x3fd31a42b0000000
                    .quad 0x3fd32839e0000000
                    .quad 0x3fd3362a10000000
                    .quad 0x3fd3441350000000

.align 16
.L__log_256_tail:
                    .quad 0x0000000000000000
                    .quad 0x3db20abc22b2208f
                    .quad 0x3db10f69332e0dd4
                    .quad 0x3dce950de87ed257
                    .quad 0x3dd3f3443b626d69
                    .quad 0x3df45aeaa5363e57
                    .quad 0x3dc443683ce1bf0b
                    .quad 0x3df989cd60c6a511
                    .quad 0x3dfd626f201f2e9f
                    .quad 0x3de94f8bb8dabdcd
                    .quad 0x3e0088d8ef423015
                    .quad 0x3e080413a62b79ad
                    .quad 0x3e059717c0eed3c4
                    .quad 0x3dad4a77add44902
                    .quad 0x3e0e763ff037300e
                    .quad 0x3de162d74706f6c3
                    .quad 0x3e0601cc1f4dbc14
                    .quad 0x3deaf3e051f6e5bf
                    .quad 0x3e097a0b1e1af3eb
                    .quad 0x3dc0a38970c002c7
                    .quad 0x3e102e000057c751
                    .quad 0x3e155b00eecd6e0e
                    .quad 0x3ddf86297003b5af
                    .quad 0x3e1057b9b336a36d
                    .quad 0x3e134bc84a06ea4f
                    .quad 0x3e1643da9ea1bcad
                    .quad 0x3e1d66a7b4f7ea2a
                    .quad 0x3df6b2e038f7fcef
                    .quad 0x3df3e954c670f088
                    .quad 0x3e047209093acab3
                    .quad 0x3e1d708fe7275da7
                    .quad 0x3e1fdf9e7771b9e7
                    .quad 0x3e0827bfa70a0660
                    .quad 0x3e1601cc1f4dbc14
                    .quad 0x3e0637f6106a5e5b
                    .quad 0x3e126a13f17c624b
                    .quad 0x3e093eb2ce80623a
                    .quad 0x3e1430d1e91594de
                    .quad 0x3e1d6b10108fa031
                    .quad 0x3e16879c0bbaf241
                    .quad 0x3dff08015ea6bc2b
                    .quad 0x3e29b63dcdc6676c
                    .quad 0x3e2b022cbcc4ab2c
                    .quad 0x3df917d07ddd6544
                    .quad 0x3e1540605703379e
                    .quad 0x3e0cd18b947a1b60
                    .quad 0x3e17ad65277ca97e
                    .quad 0x3e11884dc59f5fa9
                    .quad 0x3e1711c46006d082
                    .quad 0x3e2f092e3c3108f8
                    .quad 0x3e1714c5e32be13a
                    .quad 0x3e26bba7fd734f9a
                    .quad 0x3dfdf48fb5e08483
                    .quad 0x3e232f9bc74d0b95
                    .quad 0x3df973e848790c13
                    .quad 0x3e1eccbc08c6586e
                    .quad 0x3e2115e9f9524a98
                    .quad 0x3e2f1740593131b8
                    .quad 0x3e1bcf8b25643835
                    .quad 0x3e1f5fa81d8bed80
                    .quad 0x3e244a4df929d9e4
                    .quad 0x3e129820d8220c94
                    .quad 0x3e2a0b489304e309
                    .quad 0x3e1f4d56aba665fe
                    .quad 0x3e210c9019365163
                    .quad 0x3df80f78fe592736
                    .quad 0x3e10528825c81cca
                    .quad 0x3de095537d6d746a
                    .quad 0x3e1827bfa70a0660
                    .quad 0x3e06b0a8ec45933c
                    .quad 0x3e105af81bf5dba9
                    .quad 0x3e17e2fa2655d515
                    .quad 0x3e0d59ecbfaee4bf
                    .quad 0x3e1d8b2fda683fa3
                    .quad 0x3e24b8ddfd3a3737
                    .quad 0x3e13827e61ae1204
                    .quad 0x3e2c8c7b49e90f9f
                    .quad 0x3e29eaf01597591d
                    .quad 0x3e19aaa66e317b36
                    .quad 0x3e2e725609720655
                    .quad 0x3e261c33fc7aac54
                    .quad 0x3e29662bcf61a252
                    .quad 0x3e1843c811c42730
                    .quad 0x3e2064bb0b5acb36
                    .quad 0x3e0a340c842701a4
                    .quad 0x3e1a8e55b58f79d6
                    .quad 0x3de92d219c5e9d9a
                    .quad 0x3e3f63e60d7ffd6a
                    .quad 0x3e2e9b0ed9516314
                    .quad 0x3e2923901962350c
                    .quad 0x3e326f8838785e81
                    .quad 0x3e3b5b6a4caba6af
                    .quad 0x3df0226adc8e761c
                    .quad 0x3e3c4ad7313a1aed
                    .quad 0x3e1564e87c738d17
                    .quad 0x3e338fecf18a6618
                    .quad 0x3e3d929ef5777666
                    .quad 0x3e39483bf08da0b8
                    .quad 0x3e3bdd0eeeaa5826
                    .quad 0x3e39c4dd590237ba
                    .quad 0x3e1af3e9e0ebcac7
                    .quad 0x3e35ce5382270dac
                    .quad 0x3e394f74532ab9ba
                    .quad 0x3e07342795888654
                    .quad 0x3e0c5a000be34bf0
                    .quad 0x3e2711c46006d082
                    .quad 0x3e250025b4ed8cf8
                    .quad 0x3e2ed18bcef2d2a0
                    .quad 0x3e21282e0c0a7554
                    .quad 0x3e0d70f33359a7ca
                    .quad 0x3e2b7f7e13a84025
                    .quad 0x3e33306ec321891e
                    .quad 0x3e3fc7f8038b7550
                    .quad 0x3e3eb0358cd71d64
                    .quad 0x3e3a76c822859474
                    .quad 0x3e3d0ec652de86e3
                    .quad 0x3e2fa4cce08658af
                    .quad 0x3e3b84a2d2c00a9e
                    .quad 0x3e20a5b0f2c25bd1
                    .quad 0x3e3dd660225bf699
                    .quad 0x3e08b10f859bf037
                    .quad 0x3e3e8823b590cbe1
                    .quad 0x3e361311f31e96f6
                    .quad 0x3e2e1f875ca20f9a
                    .quad 0x3e2c95724939b9a5
                    .quad 0x3e3805957a3e58e2
                    .quad 0x3e2ff126ea9f0334
                    .quad 0x3e3953f5598e5609
                    .quad 0x3e36c16ff856c448
                    .quad 0x3e24cb220ff261f4
                    .quad 0x3e35e120d53d53a2
                    .quad 0x3e3a527f6189f256
                    .quad 0x3e3856fcffd49c0f
                    .quad 0x3e300c2e8228d7da
                    .quad 0x3df113d09444dfe0
                    .quad 0x3e2510630eea59a6
                    .quad 0x3e262e780f32d711
                    .quad 0x3ded3ed91a10f8cf
                    .quad 0x3e23654a7e4bcd85
                    .quad 0x3e055b784980ad21
                    .quad 0x3e212f2dd4b16e64
                    .quad 0x3e37c4add939f50c
                    .quad 0x3e281784627180fc
                    .quad 0x3dea5162c7e14961
                    .quad 0x3e310c9019365163
                    .quad 0x3e373c4d2ba17688
                    .quad 0x3e2ae8a5e0e93d81
                    .quad 0x3e2ab0c6f01621af
                    .quad 0x3e301e8b74dd5b66
                    .quad 0x3e2d206fecbb5494
                    .quad 0x3df0b48b724fcc00
                    .quad 0x3e3f831f0b61e229
                    .quad 0x3df81a97c407bcaf
                    .quad 0x3e3e286c1ccbb7aa
                    .quad 0x3e28630b49220a93
                    .quad 0x3dff0b15c1a22c5c
                    .quad 0x3e355445e71c0946
                    .quad 0x3e3be630f8066d85
                    .quad 0x3e2599dff0d96c39
                    .quad 0x3e36cc85b18fb081
                    .quad 0x3e34476d001ea8c8
                    .quad 0x3e373f889e16d31f
                    .quad 0x3e3357100d792a87
                    .quad 0x3e3bd179ae6101f6
                    .quad 0x3e0ca31056c3f6e2
                    .quad 0x3e3d2870629c08fb
                    .quad 0x3e3aba3880d2673f
                    .quad 0x3e2c3633cb297da6
                    .quad 0x3e21843899efea02
                    .quad 0x3e3bccc99d2008e6
                    .quad 0x3e38000544bdd350
                    .quad 0x3e2b91c226606ae1
                    .quad 0x3e2a7adf26b62bdf
                    .quad 0x3e18764fc8826ec9
                    .quad 0x3e1f4f3de50f68f0
                    .quad 0x3df760ca757995e3
                    .quad 0x3dfc667ed3805147
                    .quad 0x3e3733f6196adf6f
                    .quad 0x3e2fb710f33e836b
                    .quad 0x3e39886eba641013
                    .quad 0x3dfb5368d0af8c1a
                    .quad 0x3e358c691b8d2971
                    .quad 0x3dfe9465226d08fb
                    .quad 0x3e33587e063f0097
                    .quad 0x3e3618e702129f18
                    .quad 0x3e361c33fc7aac54
                    .quad 0x3e3f07a68408604a
                    .quad 0x3e3c34bfe4945421
                    .quad 0x3e38b1f00e41300b
                    .quad 0x3e3f434284d61b63
                    .quad 0x3e3a63095e397436
                    .quad 0x3e34428656b919de
                    .quad 0x3e36ca9201b2d9a6
                    .quad 0x3e2738823a2a931c
                    .quad 0x3e3c11880e179230
                    .quad 0x3e313ddc8d6d52fe
                    .quad 0x3e33eed58922e917
                    .quad 0x3e295992846bdd50
                    .quad 0x3e0ddb4d5f2e278b
                    .quad 0x3df1a5f12a0635c4
                    .quad 0x3e4642f0882c3c34
                    .quad 0x3e2aee9ba7f6475e
                    .quad 0x3e264b7f834a60e4
                    .quad 0x3e290d42e243792e
                    .quad 0x3e4c272008134f01
                    .quad 0x3e4a782e16d6cf5b
                    .quad 0x3e44505c79da6648
                    .quad 0x3e4ca9d4ea4dcd21
                    .quad 0x3e297d3d627cd5bc
                    .quad 0x3e20b15cf9bcaa13
                    .quad 0x3e315b2063cf76dd
                    .quad 0x3e2983e6f3aa2748
                    .quad 0x3e3f4c64f4ffe994
                    .quad 0x3e46beba7ce85a0f
                    .quad 0x3e3b9c69fd4ea6b8
                    .quad 0x3e2b6aa5835fa4ab
                    .quad 0x3e43ccc3790fedd1
                    .quad 0x3e29c04cc4404fe0
                    .quad 0x3e40734b7a75d89d
                    .quad 0x3e1b4404c4e01612
                    .quad 0x3e40c565c2ce4894
                    .quad 0x3e33c71441d935cd
                    .quad 0x3d72a492556b3b4e
                    .quad 0x3e20fa090341dc43
                    .quad 0x3e2e8f7009e3d9f4
                    .quad 0x3e4b1bf68b048a45
                    .quad 0x3e3eee52dffaa956
                    .quad 0x3e456b0900e465bd
                    .quad 0x3e4d929ef5777666
                    .quad 0x3e486ea28637e260
                    .quad 0x3e4665aff10ca2f0
                    .quad 0x3e2f11fdaf48ec74
                    .quad 0x3e4cbe1b86a4d1c7
                    .quad 0x3e25b05bfea87665
                    .quad 0x3e41cec20a1a4a1d
                    .quad 0x3e41cd5f0a409b9f
                    .quad 0x3e453656c8265070
                    .quad 0x3e377ed835282260
                    .quad 0x3e2417bc3040b9d2
                    .quad 0x3e408eef7b79eff2
                    .quad 0x3e4dc76f39dc57e9
                    .quad 0x3e4c0493a70cf457
                    .quad 0x3e4a83d6cea5a60c
                    .quad 0x3e30d6700dc557ba
                    .quad 0x3e44c96c12e8bd0a
                    .quad 0x3e3d2c1993e32315
                    .quad 0x3e22c721135f8242
                    .quad 0x3e279a3e4dda747d
                    .quad 0x3dfcf89f6941a72b
                    .quad 0x3e2149a702f10831
                    .quad 0x3e4ead4b7c8175db
                    .quad 0x3e4e6930fe63e70a
                    .quad 0x3e41e106bed9ee2f
                    .quad 0x3e2d682b82f11c92
                    .quad 0x3e3a07f188dba47c
                    .quad 0x3e40f9342dc172f6
                    .quad 0x3e03ef3fde623e25

.align 16
.L__log_F_inv:
                    .quad 0x4000000000000000
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
                    .quad 0x0000000000000000


