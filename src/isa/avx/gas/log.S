#
# log.S
#
# An implementation of the log libm function.
#
# Prototype:
#
#     double log(double x);
#

#
#   Algorithm:
#
#   Based on:
#   Ping-Tak Peter Tang
#   "Table-driven implementation of the logarithm function in IEEE
#   floating-point arithmetic"
#   ACM Transactions on Mathematical Software (TOMS)
#   Volume 16, Issue 4 (December 1990)
#
#
#   x very close to 1.0 is handled differently, for x everywhere else
#   a brief explanation is given below
#
#   x = (2^m)*A
#   x = (2^m)*(G+g) with (1 <= G < 2) and (g <= 2^(-9))
#   x = (2^m)*2*(G/2+g/2)
#   x = (2^m)*2*(F+f) with (0.5 <= F < 1) and (f <= 2^(-10))
#
#   Y = (2^(-1))*(2^(-m))*(2^m)*A
#   Now, range of Y is: 0.5 <= Y < 1
#
#   F = 0x100 + (first 8 mantissa bits) + (9th mantissa bit)
#   Now, range of F is: 256 <= F <= 512
#   F = F / 512
#   Now, range of F is: 0.5 <= F <= 1
#
#   f = -(Y-F), with (f <= 2^(-10))
#
#   log(x) = m*log(2) + log(2) + log(F-f)
#   log(x) = m*log(2) + log(2) + log(F) + log(1-(f/F))
#   log(x) = m*log(2) + log(2*F) + log(1-r)
#
#   r = (f/F), with (r <= 2^(-9))
#   r = f*(1/F) with (1/F) precomputed to avoid division
#
#   log(x) = m*log(2) + log(G) - poly
#
#   log(G) is precomputed
#   poly = (r + (r^2)/2 + (r^3)/3 + (r^4)/4) + (r^5)/5) + (r^6)/6))
#
#   log(2) and log(G) need to be maintained in extra precision
#   to avoid losing precision in the calculations
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(log)
#define fname_special _log_special@PLT



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

    # m*log(2) + log(G) - poly
    movsd       .L__real_log2_tail(%rip), %xmm5
    mulsd       %xmm6, %xmm5
    subsd       %xmm1, %xmm5

    movsd       (%r9,%rax,8), %xmm0
    lea         .L__log_256_tail(%rip), %rdx
    movsd       (%rdx,%rax,8), %xmm2
    addsd       %xmm5, %xmm2

    movsd       .L__real_log2_lead(%rip), %xmm4
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
    addsd       %xmm4, %xmm0

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

.L__mask_mant_all8: .quad 0x000ff00000000000
                    .quad 0x0000000000000000
.L__mask_mant9:     .quad 0x0000080000000000
                    .quad 0x0000000000000000

.L__real_log2_lead: .quad 0x3fe62e42e0000000 # log2_lead  6.93147122859954833984e-01
                    .quad 0x0000000000000000
.L__real_log2_tail: .quad 0x3e6efa39ef35793c # log2_tail  5.76999904754328540596e-08
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

.align 16
.L__log_256_lead:
                    .quad 0x0000000000000000
                    .quad 0x3f6ff00aa0000000
                    .quad 0x3f7fe02a60000000
                    .quad 0x3f87dc4750000000
                    .quad 0x3f8fc0a8b0000000
                    .quad 0x3f93cea440000000
                    .quad 0x3f97b91b00000000
                    .quad 0x3f9b9fc020000000
                    .quad 0x3f9f829b00000000
                    .quad 0x3fa1b0d980000000
                    .quad 0x3fa39e87b0000000
                    .quad 0x3fa58a5ba0000000
                    .quad 0x3fa77458f0000000
                    .quad 0x3fa95c8300000000
                    .quad 0x3fab42dd70000000
                    .quad 0x3fad276b80000000
                    .quad 0x3faf0a30c0000000
                    .quad 0x3fb0759830000000
                    .quad 0x3fb16536e0000000
                    .quad 0x3fb253f620000000
                    .quad 0x3fb341d790000000
                    .quad 0x3fb42edcb0000000
                    .quad 0x3fb51b0730000000
                    .quad 0x3fb60658a0000000
                    .quad 0x3fb6f0d280000000
                    .quad 0x3fb7da7660000000
                    .quad 0x3fb8c345d0000000
                    .quad 0x3fb9ab4240000000
                    .quad 0x3fba926d30000000
                    .quad 0x3fbb78c820000000
                    .quad 0x3fbc5e5480000000
                    .quad 0x3fbd4313d0000000
                    .quad 0x3fbe270760000000
                    .quad 0x3fbf0a30c0000000
                    .quad 0x3fbfec9130000000
                    .quad 0x3fc0671510000000
                    .quad 0x3fc0d77e70000000
                    .quad 0x3fc1478580000000
                    .quad 0x3fc1b72ad0000000
                    .quad 0x3fc2266f10000000
                    .quad 0x3fc29552f0000000
                    .quad 0x3fc303d710000000
                    .quad 0x3fc371fc20000000
                    .quad 0x3fc3dfc2b0000000
                    .quad 0x3fc44d2b60000000
                    .quad 0x3fc4ba36f0000000
                    .quad 0x3fc526e5e0000000
                    .quad 0x3fc59338d0000000
                    .quad 0x3fc5ff3070000000
                    .quad 0x3fc66acd40000000
                    .quad 0x3fc6d60fe0000000
                    .quad 0x3fc740f8f0000000
                    .quad 0x3fc7ab8900000000
                    .quad 0x3fc815c0a0000000
                    .quad 0x3fc87fa060000000
                    .quad 0x3fc8e928d0000000
                    .quad 0x3fc9525a90000000
                    .quad 0x3fc9bb3620000000
                    .quad 0x3fca23bc10000000
                    .quad 0x3fca8becf0000000
                    .quad 0x3fcaf3c940000000
                    .quad 0x3fcb5b5190000000
                    .quad 0x3fcbc28670000000
                    .quad 0x3fcc296850000000
                    .quad 0x3fcc8ff7c0000000
                    .quad 0x3fccf63540000000
                    .quad 0x3fcd5c2160000000
                    .quad 0x3fcdc1bca0000000
                    .quad 0x3fce270760000000
                    .quad 0x3fce8c0250000000
                    .quad 0x3fcef0adc0000000
                    .quad 0x3fcf550a50000000
                    .quad 0x3fcfb91860000000
                    .quad 0x3fd00e6c40000000
                    .quad 0x3fd0402590000000
                    .quad 0x3fd071b850000000
                    .quad 0x3fd0a324e0000000
                    .quad 0x3fd0d46b50000000
                    .quad 0x3fd1058bf0000000
                    .quad 0x3fd1368700000000
                    .quad 0x3fd1675ca0000000
                    .quad 0x3fd1980d20000000
                    .quad 0x3fd1c898c0000000
                    .quad 0x3fd1f8ff90000000
                    .quad 0x3fd22941f0000000
                    .quad 0x3fd2596010000000
                    .quad 0x3fd2895a10000000
                    .quad 0x3fd2b93030000000
                    .quad 0x3fd2e8e2b0000000
                    .quad 0x3fd31871c0000000
                    .quad 0x3fd347dd90000000
                    .quad 0x3fd3772660000000
                    .quad 0x3fd3a64c50000000
                    .quad 0x3fd3d54fa0000000
                    .quad 0x3fd4043080000000
                    .quad 0x3fd432ef20000000
                    .quad 0x3fd4618bc0000000
                    .quad 0x3fd4900680000000
                    .quad 0x3fd4be5f90000000
                    .quad 0x3fd4ec9730000000
                    .quad 0x3fd51aad80000000
                    .quad 0x3fd548a2c0000000
                    .quad 0x3fd5767710000000
                    .quad 0x3fd5a42ab0000000
                    .quad 0x3fd5d1bdb0000000
                    .quad 0x3fd5ff3070000000
                    .quad 0x3fd62c82f0000000
                    .quad 0x3fd659b570000000
                    .quad 0x3fd686c810000000
                    .quad 0x3fd6b3bb20000000
                    .quad 0x3fd6e08ea0000000
                    .quad 0x3fd70d42e0000000
                    .quad 0x3fd739d7f0000000
                    .quad 0x3fd7664e10000000
                    .quad 0x3fd792a550000000
                    .quad 0x3fd7bede00000000
                    .quad 0x3fd7eaf830000000
                    .quad 0x3fd816f410000000
                    .quad 0x3fd842d1d0000000
                    .quad 0x3fd86e9190000000
                    .quad 0x3fd89a3380000000
                    .quad 0x3fd8c5b7c0000000
                    .quad 0x3fd8f11e80000000
                    .quad 0x3fd91c67e0000000
                    .quad 0x3fd9479410000000
                    .quad 0x3fd972a340000000
                    .quad 0x3fd99d9580000000
                    .quad 0x3fd9c86b00000000
                    .quad 0x3fd9f323e0000000
                    .quad 0x3fda1dc060000000
                    .quad 0x3fda484090000000
                    .quad 0x3fda72a490000000
                    .quad 0x3fda9cec90000000
                    .quad 0x3fdac718c0000000
                    .quad 0x3fdaf12930000000
                    .quad 0x3fdb1b1e00000000
                    .quad 0x3fdb44f770000000
                    .quad 0x3fdb6eb590000000
                    .quad 0x3fdb985890000000
                    .quad 0x3fdbc1e080000000
                    .quad 0x3fdbeb4d90000000
                    .quad 0x3fdc149ff0000000
                    .quad 0x3fdc3dd7a0000000
                    .quad 0x3fdc66f4e0000000
                    .quad 0x3fdc8ff7c0000000
                    .quad 0x3fdcb8e070000000
                    .quad 0x3fdce1af00000000
                    .quad 0x3fdd0a63a0000000
                    .quad 0x3fdd32fe70000000
                    .quad 0x3fdd5b7f90000000
                    .quad 0x3fdd83e720000000
                    .quad 0x3fddac3530000000
                    .quad 0x3fddd46a00000000
                    .quad 0x3fddfc8590000000
                    .quad 0x3fde248810000000
                    .quad 0x3fde4c71a0000000
                    .quad 0x3fde744260000000
                    .quad 0x3fde9bfa60000000
                    .quad 0x3fdec399d0000000
                    .quad 0x3fdeeb20c0000000
                    .quad 0x3fdf128f50000000
                    .quad 0x3fdf39e5b0000000
                    .quad 0x3fdf6123f0000000
                    .quad 0x3fdf884a30000000
                    .quad 0x3fdfaf5880000000
                    .quad 0x3fdfd64f20000000
                    .quad 0x3fdffd2e00000000
                    .quad 0x3fe011fab0000000
                    .quad 0x3fe02552a0000000
                    .quad 0x3fe0389ee0000000
                    .quad 0x3fe04bdf90000000
                    .quad 0x3fe05f14b0000000
                    .quad 0x3fe0723e50000000
                    .quad 0x3fe0855c80000000
                    .quad 0x3fe0986f40000000
                    .quad 0x3fe0ab76b0000000
                    .quad 0x3fe0be72e0000000
                    .quad 0x3fe0d163c0000000
                    .quad 0x3fe0e44980000000
                    .quad 0x3fe0f72410000000
                    .quad 0x3fe109f390000000
                    .quad 0x3fe11cb810000000
                    .quad 0x3fe12f7190000000
                    .quad 0x3fe1422020000000
                    .quad 0x3fe154c3d0000000
                    .quad 0x3fe1675ca0000000
                    .quad 0x3fe179eab0000000
                    .quad 0x3fe18c6e00000000
                    .quad 0x3fe19ee6b0000000
                    .quad 0x3fe1b154b0000000
                    .quad 0x3fe1c3b810000000
                    .quad 0x3fe1d610f0000000
                    .quad 0x3fe1e85f50000000
                    .quad 0x3fe1faa340000000
                    .quad 0x3fe20cdcd0000000
                    .quad 0x3fe21f0bf0000000
                    .quad 0x3fe23130d0000000
                    .quad 0x3fe2434b60000000
                    .quad 0x3fe2555bc0000000
                    .quad 0x3fe2676200000000
                    .quad 0x3fe2795e10000000
                    .quad 0x3fe28b5000000000
                    .quad 0x3fe29d37f0000000
                    .quad 0x3fe2af15f0000000
                    .quad 0x3fe2c0e9e0000000
                    .quad 0x3fe2d2b400000000
                    .quad 0x3fe2e47430000000
                    .quad 0x3fe2f62a90000000
                    .quad 0x3fe307d730000000
                    .quad 0x3fe3197a00000000
                    .quad 0x3fe32b1330000000
                    .quad 0x3fe33ca2b0000000
                    .quad 0x3fe34e2890000000
                    .quad 0x3fe35fa4e0000000
                    .quad 0x3fe37117b0000000
                    .quad 0x3fe38280f0000000
                    .quad 0x3fe393e0d0000000
                    .quad 0x3fe3a53730000000
                    .quad 0x3fe3b68440000000
                    .quad 0x3fe3c7c7f0000000
                    .quad 0x3fe3d90260000000
                    .quad 0x3fe3ea3390000000
                    .quad 0x3fe3fb5b80000000
                    .quad 0x3fe40c7a40000000
                    .quad 0x3fe41d8fe0000000
                    .quad 0x3fe42e9c60000000
                    .quad 0x3fe43f9fe0000000
                    .quad 0x3fe4509a50000000
                    .quad 0x3fe4618bc0000000
                    .quad 0x3fe4727430000000
                    .quad 0x3fe48353d0000000
                    .quad 0x3fe4942a80000000
                    .quad 0x3fe4a4f850000000
                    .quad 0x3fe4b5bd60000000
                    .quad 0x3fe4c679a0000000
                    .quad 0x3fe4d72d30000000
                    .quad 0x3fe4e7d810000000
                    .quad 0x3fe4f87a30000000
                    .quad 0x3fe50913c0000000
                    .quad 0x3fe519a4c0000000
                    .quad 0x3fe52a2d20000000
                    .quad 0x3fe53aad00000000
                    .quad 0x3fe54b2460000000
                    .quad 0x3fe55b9350000000
                    .quad 0x3fe56bf9d0000000
                    .quad 0x3fe57c57f0000000
                    .quad 0x3fe58cadb0000000
                    .quad 0x3fe59cfb20000000
                    .quad 0x3fe5ad4040000000
                    .quad 0x3fe5bd7d30000000
                    .quad 0x3fe5cdb1d0000000
                    .quad 0x3fe5ddde50000000
                    .quad 0x3fe5ee02a0000000
                    .quad 0x3fe5fe1ed0000000
                    .quad 0x3fe60e32f0000000
                    .quad 0x3fe61e3ef0000000
                    .quad 0x3fe62e42e0000000
                    .quad 0x0000000000000000

.align 16
.L__log_256_tail:
                    .quad 0x0000000000000000
                    .quad 0x3db5885e0250435a
                    .quad 0x3de620cf11f86ed2
                    .quad 0x3dff0214edba4a25
                    .quad 0x3dbf807c79f3db4e
                    .quad 0x3dea352ba779a52b
                    .quad 0x3dff56c46aa49fd5
                    .quad 0x3dfebe465fef5196
                    .quad 0x3e0cf0660099f1f8
                    .quad 0x3e1247b2ff85945d
                    .quad 0x3e13fd7abf5202b6
                    .quad 0x3e1f91c9a918d51e
                    .quad 0x3e08cb73f118d3ca
                    .quad 0x3e1d91c7d6fad074
                    .quad 0x3de1971bec28d14c
                    .quad 0x3e15b616a423c78a
                    .quad 0x3da162a6617cc971
                    .quad 0x3e166391c4c06d29
                    .quad 0x3e2d46f5c1d0c4b8
                    .quad 0x3e2e14282df1f6d3
                    .quad 0x3e186f47424a660d
                    .quad 0x3e2d4c8de077753e
                    .quad 0x3e2e0c307ed24f1c
                    .quad 0x3e226ea18763bdd3
                    .quad 0x3e25cad69737c933
                    .quad 0x3e2af62599088901
                    .quad 0x3e18c66c83d6b2d0
                    .quad 0x3e1880ceb36fb30f
                    .quad 0x3e2495aac6ca17a4
                    .quad 0x3e2761db4210878c
                    .quad 0x3e2eb78e862bac2f
                    .quad 0x3e19b2cd75790dd9
                    .quad 0x3e2c55e5cbd3d50f
                    .quad 0x3db162a6617cc971
                    .quad 0x3dfdbeabaaa2e519
                    .quad 0x3e1652cb7150c647
                    .quad 0x3e39a11cb2cd2ee2
                    .quad 0x3e219d0ab1a28813
                    .quad 0x3e24bd9e80a41811
                    .quad 0x3e3214b596faa3df
                    .quad 0x3e303fea46980bb8
                    .quad 0x3e31c8ffa5fd28c7
                    .quad 0x3dce8f743bcd96c5
                    .quad 0x3dfd98c5395315c6
                    .quad 0x3e3996fa3ccfa7b2
                    .quad 0x3e1cd2af2ad13037
                    .quad 0x3e1d0da1bd17200e
                    .quad 0x3e3330410ba68b75
                    .quad 0x3df4f27a790e7c41
                    .quad 0x3e13956a86f6ff1b
                    .quad 0x3e2c6748723551d9
                    .quad 0x3e2500de9326cdfc
                    .quad 0x3e1086c848df1b59
                    .quad 0x3e04357ead6836ff
                    .quad 0x3e24832442408024
                    .quad 0x3e3d10da8154b13d
                    .quad 0x3e39e8ad68ec8260
                    .quad 0x3e3cfbf706abaf18
                    .quad 0x3e3fc56ac6326e23
                    .quad 0x3e39105e3185cf21
                    .quad 0x3e3d017fe5b19cc0
                    .quad 0x3e3d1f6b48dd13fe
                    .quad 0x3e20b63358a7e73a
                    .quad 0x3e263063028c211c
                    .quad 0x3e2e6a6886b09760
                    .quad 0x3e3c138bb891cd03
                    .quad 0x3e369f7722b7221a
                    .quad 0x3df57d8fac1a628c
                    .quad 0x3e3c55e5cbd3d50f
                    .quad 0x3e1552d2ff48fe2e
                    .quad 0x3e37b8b26ca431bc
                    .quad 0x3e292decdc1c5f6d
                    .quad 0x3e3abc7c551aaa8c
                    .quad 0x3e36b540731a354b
                    .quad 0x3e32d341036b89ef
                    .quad 0x3e4f9ab21a3a2e0f
                    .quad 0x3e239c871afb9fbd
                    .quad 0x3e3e6add2c81f640
                    .quad 0x3e435c95aa313f41
                    .quad 0x3e249d4582f6cc53
                    .quad 0x3e47574c1c07398f
                    .quad 0x3e4ba846dece9e8d
                    .quad 0x3e16999fafbc68e7
                    .quad 0x3e4c9145e51b0103
                    .quad 0x3e479ef2cb44850a
                    .quad 0x3e0beec73de11275
                    .quad 0x3e2ef4351af5a498
                    .quad 0x3e45713a493b4a50
                    .quad 0x3e45c23a61385992
                    .quad 0x3e42a88309f57299
                    .quad 0x3e4530faa9ac8ace
                    .quad 0x3e25fec2d792a758
                    .quad 0x3e35a517a71cbcd7
                    .quad 0x3e3707dc3e1cd9a3
                    .quad 0x3e3a1a9f8ef43049
                    .quad 0x3e4409d0276b3674
                    .quad 0x3e20e2f613e85bd9
                    .quad 0x3df0027433001e5f
                    .quad 0x3e35dde2836d3265
                    .quad 0x3e2300134d7aaf04
                    .quad 0x3e3cb7e0b42724f5
                    .quad 0x3e2d6e93167e6308
                    .quad 0x3e3d1569b1526adb
                    .quad 0x3e0e99fc338a1a41
                    .quad 0x3e4eb01394a11b1c
                    .quad 0x3e04f27a790e7c41
                    .quad 0x3e25ce3ca97b7af9
                    .quad 0x3e281f0f940ed857
                    .quad 0x3e4d36295d88857c
                    .quad 0x3e21aca1ec4af526
                    .quad 0x3e445743c7182726
                    .quad 0x3e23c491aead337e
                    .quad 0x3e3aef401a738931
                    .quad 0x3e21cede76092a29
                    .quad 0x3e4fba8f44f82bb4
                    .quad 0x3e446f5f7f3c3e1a
                    .quad 0x3e47055f86c9674b
                    .quad 0x3e4b41a92b6b6e1a
                    .quad 0x3e443d162e927628
                    .quad 0x3e4466174013f9b1
                    .quad 0x3e3b05096ad69c62
                    .quad 0x3e40b169150faa58
                    .quad 0x3e3cd98b1df85da7
                    .quad 0x3e468b507b0f8fa8
                    .quad 0x3e48422df57499ba
                    .quad 0x3e11351586970274
                    .quad 0x3e117e08acba92ee
                    .quad 0x3e26e04314dd0229
                    .quad 0x3e497f3097e56d1a
                    .quad 0x3e3356e655901286
                    .quad 0x3e0cb761457f94d6
                    .quad 0x3e39af67a85a9dac
                    .quad 0x3e453410931a909f
                    .quad 0x3e22c587206058f5
                    .quad 0x3e223bc358899c22
                    .quad 0x3e4d7bf8b6d223cb
                    .quad 0x3e47991ec5197ddb
                    .quad 0x3e4a79e6bb3a9219
                    .quad 0x3e3a4c43ed663ec5
                    .quad 0x3e461b5a1484f438
                    .quad 0x3e4b4e36f7ef0c3a
                    .quad 0x3e115f026acd0d1b
                    .quad 0x3e3f36b535cecf05
                    .quad 0x3e2ffb7fbf3eb5c6
                    .quad 0x3e3e6a6886b09760
                    .quad 0x3e3135eb27f5bbc3
                    .quad 0x3e470be7d6f6fa57
                    .quad 0x3e4ce43cc84ab338
                    .quad 0x3e4c01d7aac3bd91
                    .quad 0x3e45c58d07961060
                    .quad 0x3e3628bcf941456e
                    .quad 0x3e4c58b2a8461cd2
                    .quad 0x3e33071282fb989a
                    .quad 0x3e420dab6a80f09c
                    .quad 0x3e44f8d84c397b1e
                    .quad 0x3e40d0ee08599e48
                    .quad 0x3e1d68787e37da36
                    .quad 0x3e366187d591bafc
                    .quad 0x3e22346600bae772
                    .quad 0x3e390377d0d61b8e
                    .quad 0x3e4f5e0dd966b907
                    .quad 0x3e49023cb79a00e2
                    .quad 0x3e44e05158c28ad8
                    .quad 0x3e3bfa7b08b18ae4
                    .quad 0x3e4ef1e63db35f67
                    .quad 0x3e0ec2ae39493d4f
                    .quad 0x3e40afe930ab2fa0
                    .quad 0x3e225ff8a1810dd4
                    .quad 0x3e469743fb1a71a5
                    .quad 0x3e5f9cc676785571
                    .quad 0x3e5b524da4cbf982
                    .quad 0x3e5a4c8b381535b8
                    .quad 0x3e5839be809caf2c
                    .quad 0x3e50968a1cb82c13
                    .quad 0x3e5eae6a41723fb5
                    .quad 0x3e5d9c29a380a4db
                    .quad 0x3e4094aa0ada625e
                    .quad 0x3e5973ad6fc108ca
                    .quad 0x3e4747322fdbab97
                    .quad 0x3e593692fa9d4221
                    .quad 0x3e5c5a992dfbc7d9
                    .quad 0x3e4e1f33e102387a
                    .quad 0x3e464fbef14c048c
                    .quad 0x3e4490f513ca5e3b
                    .quad 0x3e37a6af4d4c799d
                    .quad 0x3e57574c1c07398f
                    .quad 0x3e57b133417f8c1c
                    .quad 0x3e5feb9e0c176514
                    .quad 0x3e419f25bb3172f7
                    .quad 0x3e45f68a7bbfb852
                    .quad 0x3e5ee278497929f1
                    .quad 0x3e5ccee006109d58
                    .quad 0x3e5ce081a07bd8b3
                    .quad 0x3e570e12981817b8
                    .quad 0x3e292ab6d93503d0
                    .quad 0x3e58cb7dd7c3b61e
                    .quad 0x3e4efafd0a0b78da
                    .quad 0x3e5e907267c4288e
                    .quad 0x3e5d31ef96780875
                    .quad 0x3e23430dfcd2ad50
                    .quad 0x3e344d88d75bc1f9
                    .quad 0x3e5bec0f055e04fc
                    .quad 0x3e5d85611590b9ad
                    .quad 0x3df320568e583229
                    .quad 0x3e5a891d1772f538
                    .quad 0x3e22edc9dabba74d
                    .quad 0x3e4b9009a1015086
                    .quad 0x3e52a12a8c5b1a19
                    .quad 0x3e3a7885f0fdac85
                    .quad 0x3e5f4ffcd43ac691
                    .quad 0x3e52243ae2640aad
                    .quad 0x3e546513299035d3
                    .quad 0x3e5b39c3a62dd725
                    .quad 0x3e5ba6dd40049f51
                    .quad 0x3e451d1ed7177409
                    .quad 0x3e5cb0f2fd7f5216
                    .quad 0x3e3ab150cd4e2213
                    .quad 0x3e5cfd7bf3193844
                    .quad 0x3e53fff8455f1dbd
                    .quad 0x3e5fee640b905fc9
                    .quad 0x3e54e2adf548084c
                    .quad 0x3e3b597adc1ecdd2
                    .quad 0x3e4345bd096d3a75
                    .quad 0x3e5101b9d2453c8b
                    .quad 0x3e508ce55cc8c979
                    .quad 0x3e5bbf017e595f71
                    .quad 0x3e37ce733bd393dc
                    .quad 0x3e233bb0a503f8a1
                    .quad 0x3e30e2f613e85bd9
                    .quad 0x3e5e67555a635b3c
                    .quad 0x3e2ea88df73d5e8b
                    .quad 0x3e3d17e03bda18a8
                    .quad 0x3e5b607d76044f7e
                    .quad 0x3e52adc4e71bc2fc
                    .quad 0x3e5f99dc7362d1d9
                    .quad 0x3e5473fa008e6a6a
                    .quad 0x3e2b75bb09cb0985
                    .quad 0x3e5ea04dd10b9aba
                    .quad 0x3e5802d0d6979674
                    .quad 0x3e174688ccd99094
                    .quad 0x3e496f16abb9df22
                    .quad 0x3e46e66df2aa374f
                    .quad 0x3e4e66525ea4550a
                    .quad 0x3e42d02f34f20cbd
                    .quad 0x3e46cfce65047188
                    .quad 0x3e39b78c842d58b8
                    .quad 0x3e4735e624c24bc9
                    .quad 0x3e47eba1f7dd1adf
                    .quad 0x3e586b3e59f65355
                    .quad 0x3e1ce38e637f1b4d
                    .quad 0x3e58d82ec919edc7
                    .quad 0x3e4c52648ddcfa37
                    .quad 0x3e52482ceae1ac12
                    .quad 0x3e55a312311aba4f
                    .quad 0x3e411e236329f225
                    .quad 0x3e5b48c8cd2f246c
                    .quad 0x3e6efa39ef35793c
                    .quad 0x0000000000000000

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


