/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <immintrin.h>
#include "libm/compiler.h"
#include "libm_macros.h"
#include "libm/amd_funcs_internal.h"
#include "libm/types.h"

#define LIBM_ASM_SYNTAX_ATT
#include "libm/libm_asm_macros.h"

static const uint32_t A1F[1] ALIGN32_USED = {
    0x3e2aaaaaU,
};

static const uint32_t A2F[1] ALIGN32_USED = {
    0x3d2aaaa0U,
};

static const uint32_t A3F[1] ALIGN32_USED = {
    0x3c0889ffU,
};

static const uint32_t A4F[1] ALIGN32_USED = {
    0x3ab64de5U,
};

static const uint32_t A5F[1] ALIGN32_USED = {
    0x394ab327U,
};

static const uint32_t LogOneMinusOneByFour[1] ALIGN32_USED = {
    0xbe934b11U,
};

static const uint32_t LogOnePlusOneByFour[1] ALIGN32_USED = {
    0x3e647fbfU,
};

static const uint32_t MaxExpm1Arg[1] ALIGN32_USED = {
    0x42b19999U,
};

static const uint32_t MinExpm1Arg[1] ALIGN32_USED = {
    0xc18aa122U,
};

static const uint32_t MinusZero[1] ALIGN32_USED = {
    0x80000000U,
};

static const uint64_t Real1By2[4] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
};

static const uint64_t Real41By2s[4] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
};

static const uint64_t Real41By6s[4] ALIGN32_USED = {
    0x3fc5555555555555ULL, 0x3fc5555555555555ULL,
    0x3fc5555555555555ULL, 0x3fc5555555555555ULL,
};

static const uint64_t Real64ByLog2[4] ALIGN32_USED = {
    0x40571547652b82feULL, 0x40571547652b82feULL,
    0x40571547652b82feULL, 0x40571547652b82feULL,
};

static const uint64_t RealMlog2By64[4] ALIGN32_USED = {
    0xbf862e42fefa39efULL, 0xbf862e42fefa39efULL,
    0xbf862e42fefa39efULL, 0xbf862e42fefa39efULL,
};

static const uint64_t TwoToJby64Table[64] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff02c9a3e778061ULL,
    0x3ff059b0d3158574ULL, 0x3ff0874518759bc8ULL,
    0x3ff0b5586cf9890fULL, 0x3ff0e3ec32d3d1a2ULL,
    0x3ff11301d0125b51ULL, 0x3ff1429aaea92de0ULL,
    0x3ff172b83c7d517bULL, 0x3ff1a35beb6fcb75ULL,
    0x3ff1d4873168b9aaULL, 0x3ff2063b88628cd6ULL,
    0x3ff2387a6e756238ULL, 0x3ff26b4565e27cddULL,
    0x3ff29e9df51fdee1ULL, 0x3ff2d285a6e4030bULL,
    0x3ff306fe0a31b715ULL, 0x3ff33c08b26416ffULL,
    0x3ff371a7373aa9cbULL, 0x3ff3a7db34e59ff7ULL,
    0x3ff3dea64c123422ULL, 0x3ff4160a21f72e2aULL,
    0x3ff44e086061892dULL, 0x3ff486a2b5c13cd0ULL,
    0x3ff4bfdad5362a27ULL, 0x3ff4f9b2769d2ca7ULL,
    0x3ff5342b569d4f82ULL, 0x3ff56f4736b527daULL,
    0x3ff5ab07dd485429ULL, 0x3ff5e76f15ad2148ULL,
    0x3ff6247eb03a5585ULL, 0x3ff6623882552225ULL,
    0x3ff6a09e667f3bcdULL, 0x3ff6dfb23c651a2fULL,
    0x3ff71f75e8ec5f74ULL, 0x3ff75feb564267c9ULL,
    0x3ff7a11473eb0187ULL, 0x3ff7e2f336cf4e62ULL,
    0x3ff82589994cce13ULL, 0x3ff868d99b4492edULL,
    0x3ff8ace5422aa0dbULL, 0x3ff8f1ae99157736ULL,
    0x3ff93737b0cdc5e5ULL, 0x3ff97d829fde4e50ULL,
    0x3ff9c49182a3f090ULL, 0x3ffa0c667b5de565ULL,
    0x3ffa5503b23e255dULL, 0x3ffa9e6b5579fdbfULL,
    0x3ffae89f995ad3adULL, 0x3ffb33a2b84f15fbULL,
    0x3ffb7f76f2fb5e47ULL, 0x3ffbcc1e904bc1d2ULL,
    0x3ffc199bdd85529cULL, 0x3ffc67f12e57d14bULL,
    0x3ffcb720dcef9069ULL, 0x3ffd072d4a07897cULL,
    0x3ffd5818dcfba487ULL, 0x3ffda9e603db3285ULL,
    0x3ffdfc97337b9b5fULL, 0x3ffe502ee78b3ff6ULL,
    0x3ffea4afa2a490daULL, 0x3ffefa1bee615a27ULL,
    0x3fff50765b6e4540ULL, 0x3fffa7c1819e90d8ULL,
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked, aligned(32)))
#endif
v_f32x4_t ALM_PROTO_FMA3(vrs4_expm1f)(v_f32x4_t x)
{
    begin_asm()
#ifdef _WIN64
        sub(imm(88), rsp)
        movdqa(xmm6, mem(rsp, 0))
        movdqa(xmm7, mem(rsp, 16))
        movdqa(xmm8, mem(rsp, 32))
        movdqa(xmm9, mem(rsp, 48))
        movdqa(xmm10, mem(rsp, 64))
#endif
        vmovdqa(xmm0, xmm10)
        vcvtps2pd(xmm0, ymm2)
        vmulpd(mem_rip(Real64ByLog2), ymm2, ymm3)
        vcvtpd2dq(ymm3, xmm4)
        vcvtdq2pd(xmm4, ymm0)
        vfmadd132pd(mem_rip(RealMlog2By64), ymm2, ymm0)
        vmovdqa(mem_rip(Real41By6s), ymm3)
        vfmadd213pd(mem_rip(Real41By2s), ymm0, ymm3)
        vmovdqa(ymm3, ymm2)
        vmulpd(ymm0, ymm0, ymm3)
        vfmadd231pd(ymm3, ymm2, ymm0)
        vpsrad(imm(6), xmm4, xmm5)
        vpslld(imm(26), xmm4, xmm3)
        vpsrld(imm(26), xmm3, xmm6)
        lea(mem_rip(TwoToJby64Table), r10)
        vmovd(xmm6, eax)
        vmovd(xmm5, ecx)
        vextractf128(imm(1), ymm0, xmm7)
        vmovhlps(xmm0, xmm8, xmm8)
        mov(imm(4), r11d)
        jmp(__start)
        label_align(__check_n_prepare_next, 4)
        dec(r11d)
        jz(__ret)
        vpsrldq(imm(4), xmm6, xmm6)
        vmovd(xmm6, eax)
        vpsrldq(imm(4), xmm5, xmm5)
        vmovd(xmm5, ecx)
        vpsrldq(imm(4), xmm10, xmm10)
        cmp(imm(3), r11d)
        je(__output1_inputq2)
        cmp(imm(2), r11d)
        je(__output2_inputq3)
        cmp(imm(1), r11d)
        je(__output3_inputq4)
        label_align(__output1_inputq2, 4)
        vpxor(xmm9, xmm9, xmm9)
        vpslldq(imm(12), xmm0, xmm0)
        vpsrldq(imm(12), xmm0, xmm0)
        vpor(xmm0, xmm9, xmm9)
        movdqa(xmm8, xmm0)
        jmp(__start)
        label_align(__output2_inputq3, 4)
        vpslldq(imm(12), xmm0, xmm0)
        vpsrldq(imm(8), xmm0, xmm0)
        vpor(xmm0, xmm9, xmm9)
        vmovdqa(xmm7, xmm0)
        jmp(__start)
        label_align(__output3_inputq4, 4)
        vpslldq(imm(12), xmm0, xmm0)
        vpsrldq(imm(4), xmm0, xmm0)
        vpor(xmm0, xmm9, xmm9)
        vmovhlps(xmm7, xmm0, xmm0)
        label_align(__start, 4)
        ucomiss(mem_rip(MaxExpm1Arg), xmm10)
        ja(__y_is_inf)
        jp(__y_is_nan)
        ucomiss(mem_rip(LogOnePlusOneByFour), xmm10)
        jae(__Normal_Flow)
        ucomiss(mem_rip(LogOneMinusOneByFour), xmm10)
        ja(__Small_Arg)
        ucomiss(mem_rip(MinExpm1Arg), xmm10)
        jb(__y_is_minusOne)
        label_align(__Normal_Flow, 4)
        vmovsd(mem(r10, rax, 8), xmm3)
        mov(imm(1023), eax)
        sub(ecx, eax)
        shl(imm(52), rax)
        vmovq(rax, xmm1)
        vsubsd(xmm1, xmm3, xmm2)
        vfmadd213sd(xmm2, xmm0, xmm3)
        shl(imm(52), rcx)
        movd(rcx, xmm2)
        vpaddq(xmm3, xmm2, xmm3)
        vcvtpd2ps(xmm3, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__Small_Arg, 4)
        movapd(xmm10, xmm0)
        vucomiss(mem_rip(MinusZero), xmm0)
        je(__check_n_prepare_next)
        vmulss(mem_rip(A5F), xmm0, xmm3)
        vaddss(mem_rip(A4F), xmm3, xmm1)
        vfmadd213ss(mem_rip(A3F), xmm0, xmm1)
        vfmadd213ss(mem_rip(A2F), xmm0, xmm1)
        vfmadd213ss(mem_rip(A1F), xmm0, xmm1)
        vmulss(xmm0, xmm1, xmm1)
        vmulss(xmm0, xmm1, xmm3)
        vmulss(xmm0, xmm3, xmm1)
        vcvtps2pd(xmm0, xmm2)
        vmulsd(xmm2, xmm2, xmm0)
        vfmadd231sd(mem_rip(Real1By2), xmm0, xmm2)
        vcvtps2pd(xmm1, xmm0)
        vaddsd(xmm2, xmm0, xmm2)
        vcvtpd2ps(xmm2, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__y_is_minusOne, 4)
        mov(imm(0xBF800000), eax)
        vmovd(eax, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__y_is_inf, 4)
        mov(imm(0x7f800000), eax)
        vmovd(eax, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__y_is_nan, 4)
        vaddss(xmm10, xmm10, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__ret, 4)
        vpslldq(imm(12), xmm0, xmm0)
        vpor(xmm0, xmm9, xmm9)
        vmovdqa(xmm9, xmm0)
#ifdef _WIN64
        movdqa(mem(rsp, 0), xmm6)
        movdqa(mem(rsp, 16), xmm7)
        movdqa(mem(rsp, 32), xmm8)
        movdqa(mem(rsp, 48), xmm9)
        movdqa(mem(rsp, 64), xmm10)
        add(imm(88), rsp)
#endif
        ret
    end_asm( : : :)
}
