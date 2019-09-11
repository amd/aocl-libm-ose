#
# log2f.S
#
# An implementation of the log2f libm function.
#
# Prototype:
#
#     float log2f(float x);
#

#
#   Algorithm:
#       Similar to one presnted in log.S
#

#include "fn_macros.h"
#define fname FN_PROTOTYPE_BAS64(log2f)
#define fname_special _log2f_special@PLT



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
    xor         %eax, %eax
    movdqa      %xmm0, %xmm3
    movss       %xmm0, %xmm4
    psrld       $23, %xmm3
    movd        %xmm0, %eax
    psubd       .L__mask_127(%rip), %xmm3
    movdqa      %xmm0, %xmm2
    cvtdq2ps    %xmm3, %xmm5 # xexp

    #  NaN or inf
    movdqa      %xmm0, %xmm1
    andps       .L__real_inf(%rip), %xmm1
    comiss      .L__real_inf(%rip), %xmm1
    je          .L__x_is_inf_or_nan

    # check for negative numbers or zero
    xorps       %xmm1, %xmm1
    comiss      %xmm1, %xmm0
    jbe         .L__x_is_zero_or_neg

    pand        .L__real_mant(%rip), %xmm2
    subss       .L__real_one(%rip), %xmm4

    comiss      .L__real_neg127(%rip), %xmm5
    je          .L__denormal_adjust

.L__continue_common:

    # compute index into the log tables
    mov         %eax, %r9d
    and         .L__mask_mant_all7(%rip), %eax
    and         .L__mask_mant8(%rip), %r9d
    shl         $1, %r9d
    add         %r9d, %eax
    mov         %eax, p_temp(%rsp)

    # near one codepath
    andps       .L__real_notsign(%rip), %xmm4
    comiss      .L__real_threshold(%rip), %xmm4
    jb          .L__near_one

    # F, Y
    movss       p_temp(%rsp), %xmm1
    shr         $16, %eax
    por         .L__real_half(%rip), %xmm2
    por         .L__real_half(%rip), %xmm1
    lea         .L__log_F_inv(%rip), %r9

    # f = F - Y, r = f * inv
    subss       %xmm2, %xmm1
    mulss       (%r9,%rax,4), %xmm1

    movss       %xmm1, %xmm2
    movss       %xmm1, %xmm0

    # poly
    mulss       .L__real_1_over_3(%rip), %xmm2
    mulss       %xmm1, %xmm0
    addss       .L__real_1_over_2(%rip), %xmm2

    lea         .L__log_128_tail(%rip), %r9
    lea         .L__log_128_lead(%rip), %r10

    mulss       %xmm0, %xmm2
    movss       (%r9,%rax,4), %xmm3
    addss       %xmm2, %xmm1

    mulss       .L__real_log2_e(%rip), %xmm1

    # m + log2(G) - poly*log2_e
    subss       %xmm1, %xmm3 
    movss       %xmm3, %xmm0
    addss       (%r10,%rax,4), %xmm5
    addss       %xmm5, %xmm0

    add         $stack_size, %rsp
    ret

.p2align 4,,15
.L__near_one:
    # r = x - 1.0#
    movss       .L__real_two(%rip), %xmm2
    subss       .L__real_one(%rip), %xmm0

    # u = r / (2.0 + r)
    addss       %xmm0, %xmm2
    movss       %xmm0, %xmm1
    divss       %xmm2, %xmm1 # u

    # correction = r * u
    movss       %xmm0, %xmm4
    mulss       %xmm1, %xmm4

    # u = u + u#
    addss       %xmm1, %xmm1
    movss       %xmm1, %xmm2
    mulss       %xmm2, %xmm2 # v = u^2

    # r2 = (u * v * (ca_1 + v * ca_2) - correction)
    movss       %xmm1, %xmm3
    mulss       %xmm2, %xmm3 # u^3
    mulss       .L__real_ca2(%rip), %xmm2 # Bu^2
    addss       .L__real_ca1(%rip), %xmm2 # +A
    mulss       %xmm3, %xmm2
    subss       %xmm4, %xmm2 # -correction

    movdqa      %xmm0, %xmm5
    pand        .L__mask_lower(%rip), %xmm5
    subss       %xmm5, %xmm0
    addss       %xmm0, %xmm2

    movss       %xmm5, %xmm0
    movss       %xmm2, %xmm1

    mulss       .L__real_log2_e_tail(%rip), %xmm2
    mulss       .L__real_log2_e_tail(%rip), %xmm0
    mulss       .L__real_log2_e_lead(%rip), %xmm1
    mulss       .L__real_log2_e_lead(%rip), %xmm5

    addss       %xmm2, %xmm0
    addss       %xmm1, %xmm0
    addss       %xmm5, %xmm0

    add         $stack_size, %rsp
    ret

.p2align 4,,15
.L__denormal_adjust:
    por         .L__real_one(%rip), %xmm2
    subss       .L__real_one(%rip), %xmm2
    movdqa      %xmm2, %xmm5
    pand        .L__real_mant(%rip), %xmm2
    movd        %xmm2, %eax
    psrld       $23, %xmm5
    psubd       .L__mask_253(%rip), %xmm5
    cvtdq2ps    %xmm5, %xmm5
    jmp         .L__continue_common

.p2align 4,,15
.L__x_is_zero_or_neg:
    jne         .L__x_is_neg

    movss       .L__real_ninf(%rip), %xmm1
    mov         .L__flag_x_zero(%rip), %edi
    call        fname_special
    jmp         .L__finish

.p2align 4,,15
.L__x_is_neg:

    movss       .L__real_nan(%rip), %xmm1
    mov         .L__flag_x_neg(%rip), %edi
    call        fname_special
    jmp         .L__finish

.p2align 4,,15
.L__x_is_inf_or_nan:

    cmp         .L__real_inf(%rip), %eax
    je          .L__finish

    cmp         .L__real_ninf(%rip), %eax
    je          .L__x_is_neg

    mov         .L__real_qnanbit(%rip), %r9d
    and         %eax, %r9d
    jnz         .L__finish

    or          .L__real_qnanbit(%rip), %eax
    movd        %eax, %xmm1
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

.L__real_one:           .quad 0x03f8000003f800000   # 1.0
                        .quad 0x03f8000003f800000
.L__real_two:           .quad 0x04000000040000000   # 1.0
                        .quad 0x04000000040000000
.L__real_ninf:          .quad 0x0ff800000ff800000   # -inf
                        .quad 0x0ff800000ff800000
.L__real_inf:           .quad 0x07f8000007f800000   # +inf
                        .quad 0x07f8000007f800000
.L__real_nan:           .quad 0x07fc000007fc00000   # NaN
                        .quad 0x07fc000007fc00000
.L__real_ef:            .quad 0x0402DF854402DF854   # float e
                        .quad 0x0402DF854402DF854
.L__real_neg_qnan:      .quad 0x0ffc00000ffc00000
                        .quad 0x0ffc00000ffc00000

.L__real_sign:          .quad 0x08000000080000000   # sign bit
                        .quad 0x08000000080000000
.L__real_notsign:       .quad 0x07ffFFFFF7ffFFFFF   # ^sign bit
                        .quad 0x07ffFFFFF7ffFFFFF
.L__real_qnanbit:       .quad 0x00040000000400000   # quiet nan bit
                        .quad 0x00040000000400000
.L__real_mant:          .quad 0x0007FFFFF007FFFFF   # mantissa bits
                        .quad 0x0007FFFFF007FFFFF
.L__mask_127:           .quad 0x00000007f0000007f   # 
                        .quad 0x00000007f0000007f

.L__mask_mant_all7:     .quad 0x00000000007f0000
                        .quad 0x00000000007f0000
.L__mask_mant8:         .quad 0x0000000000008000
                        .quad 0x0000000000008000

.L__real_ca1:           .quad 0x03DAAAAAB3DAAAAAB   # 8.33333333333317923934e-02
                        .quad 0x03DAAAAAB3DAAAAAB
.L__real_ca2:           .quad 0x03C4CCCCD3C4CCCCD   # 1.25000000037717509602e-02
                        .quad 0x03C4CCCCD3C4CCCCD

.L__real_log2_lead:     .quad 0x03F3170003F317000   # 0.693115234375
                        .quad 0x03F3170003F317000
.L__real_log2_tail:     .quad 0x03805FDF43805FDF4   # 0.000031946183
                        .quad 0x03805FDF43805FDF4
.L__real_half:          .quad 0x03f0000003f000000   # 1/2
                        .quad 0x03f0000003f000000

.L__real_log2_e_lead:   .quad 0x03FB800003FB80000   # 1.4375000000
                        .quad 0x03FB800003FB80000
.L__real_log2_e_tail:   .quad 0x03BAA3B293BAA3B29   # 0.0051950408889633
                        .quad 0x03BAA3B293BAA3B29

.L__real_log2_e:        .quad 0x3fb8aa3b3fb8aa3b
                        .quad 0x0000000000000000

.L__mask_lower:         .quad 0x0ffff0000ffff0000
                        .quad 0x0ffff0000ffff0000

.align 16

.L__real_neg127:    .long 0x0c2fe0000
                    .long 0
                    .quad 0

.L__mask_253:       .long 0x000000fd
                    .long 0
                    .quad 0

.L__real_threshold: .long 0x3d800000
                    .long 0
                    .quad 0

.L__mask_01:        .long 0x00000001
                    .long 0
                    .quad 0

.L__mask_80:        .long 0x00000080
                    .long 0
                    .quad 0

.L__real_3b800000:  .long 0x3b800000
                    .long 0
                    .quad 0

.L__real_1_over_3:  .long 0x3eaaaaab
                    .long 0
                    .quad 0

.L__real_1_over_2:  .long 0x3f000000
                    .long 0
                    .quad 0

.align 16
.L__log_128_lead:
                    .long 0x00000000
                    .long 0x3c37c000
                    .long 0x3cb70000
                    .long 0x3d08c000
                    .long 0x3d35c000
                    .long 0x3d624000
                    .long 0x3d874000
                    .long 0x3d9d4000
                    .long 0x3db30000
                    .long 0x3dc8c000
                    .long 0x3dde4000
                    .long 0x3df38000
                    .long 0x3e044000
                    .long 0x3e0ec000
                    .long 0x3e194000
                    .long 0x3e238000
                    .long 0x3e2e0000
                    .long 0x3e380000
                    .long 0x3e424000
                    .long 0x3e4c4000
                    .long 0x3e564000
                    .long 0x3e604000
                    .long 0x3e6a4000
                    .long 0x3e740000
                    .long 0x3e7dc000
                    .long 0x3e83c000
                    .long 0x3e888000
                    .long 0x3e8d4000
                    .long 0x3e920000
                    .long 0x3e96c000
                    .long 0x3e9b8000
                    .long 0x3ea00000
                    .long 0x3ea4c000
                    .long 0x3ea94000
                    .long 0x3eae0000
                    .long 0x3eb28000
                    .long 0x3eb70000
                    .long 0x3ebb8000
                    .long 0x3ec00000
                    .long 0x3ec44000
                    .long 0x3ec8c000
                    .long 0x3ecd4000
                    .long 0x3ed18000
                    .long 0x3ed5c000
                    .long 0x3eda0000
                    .long 0x3ede8000
                    .long 0x3ee2c000
                    .long 0x3ee70000
                    .long 0x3eeb0000
                    .long 0x3eef4000
                    .long 0x3ef38000
                    .long 0x3ef78000
                    .long 0x3efbc000
                    .long 0x3effc000
                    .long 0x3f01c000
                    .long 0x3f040000
                    .long 0x3f060000
                    .long 0x3f080000
                    .long 0x3f0a0000
                    .long 0x3f0c0000
                    .long 0x3f0dc000
                    .long 0x3f0fc000
                    .long 0x3f11c000
                    .long 0x3f13c000
                    .long 0x3f15c000
                    .long 0x3f178000
                    .long 0x3f198000
                    .long 0x3f1b4000
                    .long 0x3f1d4000
                    .long 0x3f1f0000
                    .long 0x3f210000
                    .long 0x3f22c000
                    .long 0x3f24c000
                    .long 0x3f268000
                    .long 0x3f288000
                    .long 0x3f2a4000
                    .long 0x3f2c0000
                    .long 0x3f2dc000
                    .long 0x3f2f8000
                    .long 0x3f318000
                    .long 0x3f334000
                    .long 0x3f350000
                    .long 0x3f36c000
                    .long 0x3f388000
                    .long 0x3f3a4000
                    .long 0x3f3c0000
                    .long 0x3f3dc000
                    .long 0x3f3f8000
                    .long 0x3f414000
                    .long 0x3f42c000
                    .long 0x3f448000
                    .long 0x3f464000
                    .long 0x3f480000
                    .long 0x3f498000
                    .long 0x3f4b4000
                    .long 0x3f4d0000
                    .long 0x3f4e8000
                    .long 0x3f504000
                    .long 0x3f51c000
                    .long 0x3f538000
                    .long 0x3f550000
                    .long 0x3f56c000
                    .long 0x3f584000
                    .long 0x3f5a0000
                    .long 0x3f5b8000
                    .long 0x3f5d0000
                    .long 0x3f5ec000
                    .long 0x3f604000
                    .long 0x3f61c000
                    .long 0x3f638000
                    .long 0x3f650000
                    .long 0x3f668000
                    .long 0x3f680000
                    .long 0x3f698000
                    .long 0x3f6b0000
                    .long 0x3f6cc000
                    .long 0x3f6e4000
                    .long 0x3f6fc000
                    .long 0x3f714000
                    .long 0x3f72c000
                    .long 0x3f744000
                    .long 0x3f75c000
                    .long 0x3f770000
                    .long 0x3f788000
                    .long 0x3f7a0000
                    .long 0x3f7b8000
                    .long 0x3f7d0000
                    .long 0x3f7e8000
                    .long 0x3f800000

.align 16
.L__log_128_tail:
                    .long 0x00000000
                    .long 0x374a16dd
                    .long 0x37f2d0b8
                    .long 0x381a3aa2
                    .long 0x37b4dd63
                    .long 0x383f5721
                    .long 0x384e27e8
                    .long 0x380bf749
                    .long 0x387dbeb2
                    .long 0x37216e46
                    .long 0x3684815b
                    .long 0x383b045f
                    .long 0x390b119b
                    .long 0x391a32ea
                    .long 0x38ba789e
                    .long 0x39553f30
                    .long 0x3651cfde
                    .long 0x39685a9d
                    .long 0x39057a05
                    .long 0x395ba0ef
                    .long 0x396bc5b6
                    .long 0x3936d9bb
                    .long 0x38772619
                    .long 0x39017ce9
                    .long 0x3902d720
                    .long 0x38856dd8
                    .long 0x3941f6b4
                    .long 0x3980b652
                    .long 0x3980f561
                    .long 0x39443f13
                    .long 0x38926752
                    .long 0x39c8c763
                    .long 0x391e12f3
                    .long 0x39b7bf89
                    .long 0x36d1cfde
                    .long 0x38c7f233
                    .long 0x39087367
                    .long 0x38e95d3f
                    .long 0x38256316
                    .long 0x39d38e5c
                    .long 0x396ea247
                    .long 0x350e4788
                    .long 0x395d829f
                    .long 0x39c30f2f
                    .long 0x39fd7ee7
                    .long 0x3872e9e7
                    .long 0x3897d694
                    .long 0x3824923a
                    .long 0x39ea7c06
                    .long 0x39a7fa88
                    .long 0x391aa879
                    .long 0x39dace65
                    .long 0x39215a32
                    .long 0x39af3350
                    .long 0x3a7b5172
                    .long 0x389cf27f
                    .long 0x3902806b
                    .long 0x3909d8a9
                    .long 0x38c9faa1
                    .long 0x37a33dca
                    .long 0x3a6623d2
                    .long 0x3a3c7a61
                    .long 0x3a083a84
                    .long 0x39930161
                    .long 0x35d1cfde
                    .long 0x3a2d0ebd
                    .long 0x399f1aad
                    .long 0x3a67ff6d
                    .long 0x39ecfea8
                    .long 0x3a7b26f3
                    .long 0x39ec1fa6
                    .long 0x3a675314
                    .long 0x399e12f3
                    .long 0x3a2d4b66
                    .long 0x370c3845
                    .long 0x399ba329
                    .long 0x3a1044d3
                    .long 0x3a49a196
                    .long 0x3a79fe83
                    .long 0x3905c7aa
                    .long 0x39802391
                    .long 0x39abe796
                    .long 0x39c65a9d
                    .long 0x39cfa6c5
                    .long 0x39c7f593
                    .long 0x39af6ff7
                    .long 0x39863e4d
                    .long 0x391910c1
                    .long 0x369d5be7
                    .long 0x3a541616
                    .long 0x3a1ee960
                    .long 0x39c38ed2
                    .long 0x38e61600
                    .long 0x3a4fedb4
                    .long 0x39f6b4ab
                    .long 0x38f8d3b0
                    .long 0x3a3b3faa
                    .long 0x399fb693
                    .long 0x3a5cfe71
                    .long 0x39c5740b
                    .long 0x3a611eb0
                    .long 0x39b079c4
                    .long 0x3a4824d7
                    .long 0x39439a54
                    .long 0x3a1291ea
                    .long 0x3a6d3673
                    .long 0x3981c731
                    .long 0x3a0da88f
                    .long 0x3a53945c
                    .long 0x3895ae91
                    .long 0x3996372a
                    .long 0x39f9a832
                    .long 0x3a27eda4
                    .long 0x3a4c764f
                    .long 0x3a6a7c06
                    .long 0x370321eb
                    .long 0x3899ab3f
                    .long 0x38f02086
                    .long 0x390a1707
                    .long 0x39031e44
                    .long 0x38c6b362
                    .long 0x382bf195
                    .long 0x3a768e36
                    .long 0x3a5c503b
                    .long 0x3a3c1179
                    .long 0x3a15de1d
                    .long 0x39d3845d
                    .long 0x395f263f
                    .long 0x00000000

.align 16
.L__log_F_inv:
                    .long 0x40000000
                    .long 0x3ffe03f8
                    .long 0x3ffc0fc1
                    .long 0x3ffa232d
                    .long 0x3ff83e10
                    .long 0x3ff6603e
                    .long 0x3ff4898d
                    .long 0x3ff2b9d6
                    .long 0x3ff0f0f1
                    .long 0x3fef2eb7
                    .long 0x3fed7304
                    .long 0x3febbdb3
                    .long 0x3fea0ea1
                    .long 0x3fe865ac
                    .long 0x3fe6c2b4
                    .long 0x3fe52598
                    .long 0x3fe38e39
                    .long 0x3fe1fc78
                    .long 0x3fe07038
                    .long 0x3fdee95c
                    .long 0x3fdd67c9
                    .long 0x3fdbeb62
                    .long 0x3fda740e
                    .long 0x3fd901b2
                    .long 0x3fd79436
                    .long 0x3fd62b81
                    .long 0x3fd4c77b
                    .long 0x3fd3680d
                    .long 0x3fd20d21
                    .long 0x3fd0b6a0
                    .long 0x3fcf6475
                    .long 0x3fce168a
                    .long 0x3fcccccd
                    .long 0x3fcb8728
                    .long 0x3fca4588
                    .long 0x3fc907da
                    .long 0x3fc7ce0c
                    .long 0x3fc6980c
                    .long 0x3fc565c8
                    .long 0x3fc43730
                    .long 0x3fc30c31
                    .long 0x3fc1e4bc
                    .long 0x3fc0c0c1
                    .long 0x3fbfa030
                    .long 0x3fbe82fa
                    .long 0x3fbd6910
                    .long 0x3fbc5264
                    .long 0x3fbb3ee7
                    .long 0x3fba2e8c
                    .long 0x3fb92144
                    .long 0x3fb81703
                    .long 0x3fb70fbb
                    .long 0x3fb60b61
                    .long 0x3fb509e7
                    .long 0x3fb40b41
                    .long 0x3fb30f63
                    .long 0x3fb21643
                    .long 0x3fb11fd4
                    .long 0x3fb02c0b
                    .long 0x3faf3ade
                    .long 0x3fae4c41
                    .long 0x3fad602b
                    .long 0x3fac7692
                    .long 0x3fab8f6a
                    .long 0x3faaaaab
                    .long 0x3fa9c84a
                    .long 0x3fa8e83f
                    .long 0x3fa80a81
                    .long 0x3fa72f05
                    .long 0x3fa655c4
                    .long 0x3fa57eb5
                    .long 0x3fa4a9cf
                    .long 0x3fa3d70a
                    .long 0x3fa3065e
                    .long 0x3fa237c3
                    .long 0x3fa16b31
                    .long 0x3fa0a0a1
                    .long 0x3f9fd80a
                    .long 0x3f9f1166
                    .long 0x3f9e4cad
                    .long 0x3f9d89d9
                    .long 0x3f9cc8e1
                    .long 0x3f9c09c1
                    .long 0x3f9b4c70
                    .long 0x3f9a90e8
                    .long 0x3f99d723
                    .long 0x3f991f1a
                    .long 0x3f9868c8
                    .long 0x3f97b426
                    .long 0x3f97012e
                    .long 0x3f964fda
                    .long 0x3f95a025
                    .long 0x3f94f209
                    .long 0x3f944581
                    .long 0x3f939a86
                    .long 0x3f92f114
                    .long 0x3f924925
                    .long 0x3f91a2b4
                    .long 0x3f90fdbc
                    .long 0x3f905a38
                    .long 0x3f8fb824
                    .long 0x3f8f177a
                    .long 0x3f8e7835
                    .long 0x3f8dda52
                    .long 0x3f8d3dcb
                    .long 0x3f8ca29c
                    .long 0x3f8c08c1
                    .long 0x3f8b7034
                    .long 0x3f8ad8f3
                    .long 0x3f8a42f8
                    .long 0x3f89ae41
                    .long 0x3f891ac7
                    .long 0x3f888889
                    .long 0x3f87f781
                    .long 0x3f8767ab
                    .long 0x3f86d905
                    .long 0x3f864b8a
                    .long 0x3f85bf37
                    .long 0x3f853408
                    .long 0x3f84a9fa
                    .long 0x3f842108
                    .long 0x3f839930
                    .long 0x3f83126f
                    .long 0x3f828cc0
                    .long 0x3f820821
                    .long 0x3f81848e
                    .long 0x3f810204
                    .long 0x3f808081
                    .long 0x3f800000


