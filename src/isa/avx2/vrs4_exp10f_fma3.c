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

static const uint64_t MaxExp10fArg[2] ALIGN32_USED = {
    0x421a209b421a209bULL, 0x421a209b421a209bULL,
};

static const uint64_t MinExp10fArg[2] ALIGN32_USED = {
    0xc23369f4c23369f4ULL, 0xc23369f4c23369f4ULL,
};

static const uint64_t Real41By2s[4] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
};

static const uint64_t Real41By6s[4] ALIGN32_USED = {
    0x3fc5555555555555ULL, 0x3fc5555555555555ULL,
    0x3fc5555555555555ULL, 0x3fc5555555555555ULL,
};

static const uint64_t Real64ByLog10of2[4] ALIGN32_USED = {
    0x406a934f0979a371ULL, 0x406a934f0979a371ULL,
    0x406a934f0979a371ULL, 0x406a934f0979a371ULL,
};

static const uint64_t RealLn10[4] ALIGN32_USED = {
    0x40026bb1bbb55516ULL, 0x40026bb1bbb55516ULL,
    0x40026bb1bbb55516ULL, 0x40026bb1bbb55516ULL,
};

static const uint64_t RealMlog10of2By64[4] ALIGN32_USED = {
    0xbf734413509f79ffULL, 0xbf734413509f79ffULL,
    0xbf734413509f79ffULL, 0xbf734413509f79ffULL,
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
v_f32x4_t ALM_PROTO_FMA3(vrs4_exp10f)(v_f32x4_t x)
{
    begin_asm()
        vmovdqa(xmm0, xmm5)
        vminps(mem_rip(MaxExp10fArg), xmm0, xmm0)
        vcvtps2pd(xmm0, ymm2)
        vmulpd(mem_rip(Real64ByLog10of2), ymm2, ymm3)
        vcvtpd2dq(ymm3, xmm4)
        vcvtdq2pd(xmm4, ymm0)
        vfmadd132pd(mem_rip(RealMlog10of2By64), ymm2, ymm0)
        vmulpd(mem_rip(RealLn10), ymm0, ymm1)
        vmovdqa(mem_rip(Real41By6s), ymm3)
        vmovdqa(ymm3, ymm2)
        vfmadd213pd(mem_rip(Real41By2s), ymm1, ymm3)
        vmulpd(ymm1, ymm1, ymm2)
        vfmadd213pd(ymm1, ymm2, ymm3)
        vpsrad(imm(6), xmm4, xmm1)
        vpslld(imm(26), xmm4, xmm0)
        vpsrld(imm(26), xmm0, xmm4)
        lea(mem_rip(TwoToJby64Table), r10)
        vmovd(xmm4, eax)
        vpsrldq(imm(4), xmm4, xmm0)
        vmovd(xmm0, ecx)
        vmovsd(mem(r10, rax, 8), xmm2)
        vmovhpd(mem(r10, rcx, 8), xmm2, xmm2)
        vpsrldq(imm(4), xmm0, xmm4)
        vmovd(xmm4, eax)
        vpsrldq(imm(4), xmm4, xmm0)
        vmovd(xmm0, ecx)
        vmovsd(mem(r10, rax, 8), xmm0)
        vmovhpd(mem(r10, rcx, 8), xmm0, xmm0)
        vinsertf128(imm(1), xmm0, ymm2, ymm2)
        vfmadd213pd(ymm2, ymm2, ymm3)
        vpmovsxdq(xmm1, xmm2)
        vpsllq(imm(52), xmm2, xmm0)
        vpsrldq(imm(8), xmm1, xmm2)
        vpmovsxdq(xmm2, xmm1)
        vpsllq(imm(52), xmm1, xmm2)
        vextractf128(imm(1), ymm3, xmm1)
        vpaddq(xmm3, xmm0, xmm0)
        vpaddq(xmm1, xmm2, xmm1)
        vinsertf128(imm(1), xmm1, ymm0, ymm2)
        vcvtpd2ps(ymm2, xmm0)
        vmovdqa(mem_rip(MinExp10fArg), xmm1)
        vcmpps(imm(1), xmm5, xmm1, xmm2)
        vpand(xmm2, xmm0, xmm1)
        vcmpps(imm(0x0), xmm5, xmm5, xmm2)
        vaddps(xmm5, xmm5, xmm3)
        vandpd(xmm2, xmm1, xmm0)
        vandnpd(xmm3, xmm2, xmm4)
        vorpd(xmm4, xmm0, xmm0)
        ret
    end_asm( : : :)
}
