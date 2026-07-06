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

static const uint64_t Zero64[2] ALIGN32_USED = {
    0x0000000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Constant1[2] ALIGN32_USED = {
    0x03d0000000000000ULL, 0x03d0000000000000ULL,
};

static const uint64_t ExpMask[2] ALIGN32_USED = {
    0x7ff0000000000000ULL, 0x7ff0000000000000ULL,
};

static const uint64_t Expadjust[2] ALIGN32_USED = {
    0x000000000000003cULL, 0x000000000000003cULL,
};

static const uint64_t ImpbitDp64[2] ALIGN32_USED = {
    0x0010000000000000ULL, 0x0010000000000000ULL,
};

static const uint64_t IndexConstant1[2] ALIGN32_USED = {
    0x3f80000000000000ULL, 0x3f80000000000000ULL,
};

static const uint64_t IndexConstant2[2] ALIGN32_USED = {
    0x0000000000000040ULL, 0x0000000000000040ULL,
};

static const uint64_t IndexMask1[2] ALIGN32_USED = {
    0x000fc00000000000ULL, 0x000fc00000000000ULL,
};

static const uint64_t IndexMask2[2] ALIGN32_USED = {
    0x0000200000000000ULL, 0x0000200000000000ULL,
};

static const uint64_t IndexMask3[2] ALIGN32_USED = {
    0x0010000000000000ULL, 0x0010000000000000ULL,
};

static const uint64_t LnLead64Table[65] ALIGN32_USED = {
    0x0000000000000000ULL, 0x3f8fc0a800000000ULL,
    0x3f9f829800000000ULL, 0x3fa7745800000000ULL,
    0x3faf0a3000000000ULL, 0x3fb341d700000000ULL,
    0x3fb6f0d200000000ULL, 0x3fba926d00000000ULL,
    0x3fbe270700000000ULL, 0x3fc0d77e00000000ULL,
    0x3fc2955280000000ULL, 0x3fc44d2b00000000ULL,
    0x3fc5ff3000000000ULL, 0x3fc7ab8900000000ULL,
    0x3fc9525a80000000ULL, 0x3fcaf3c900000000ULL,
    0x3fcc8ff780000000ULL, 0x3fce270700000000ULL,
    0x3fcfb91800000000ULL, 0x3fd0a324c0000000ULL,
    0x3fd1675c80000000ULL, 0x3fd22941c0000000ULL,
    0x3fd2e8e280000000ULL, 0x3fd3a64c40000000ULL,
    0x3fd4618bc0000000ULL, 0x3fd51aad80000000ULL,
    0x3fd5d1bd80000000ULL, 0x3fd686c800000000ULL,
    0x3fd739d7c0000000ULL, 0x3fd7eaf800000000ULL,
    0x3fd89a3380000000ULL, 0x3fd9479400000000ULL,
    0x3fd9f323c0000000ULL, 0x3fda9cec80000000ULL,
    0x3fdb44f740000000ULL, 0x3fdbeb4d80000000ULL,
    0x3fdc8ff7c0000000ULL, 0x3fdd32fe40000000ULL,
    0x3fddd46a00000000ULL, 0x3fde744240000000ULL,
    0x3fdf128f40000000ULL, 0x3fdfaf5880000000ULL,
    0x3fe02552a0000000ULL, 0x3fe0723e40000000ULL,
    0x3fe0be72e0000000ULL, 0x3fe109f380000000ULL,
    0x3fe154c3c0000000ULL, 0x3fe19ee6a0000000ULL,
    0x3fe1e85f40000000ULL, 0x3fe23130c0000000ULL,
    0x3fe2795e00000000ULL, 0x3fe2c0e9e0000000ULL,
    0x3fe307d720000000ULL, 0x3fe34e2880000000ULL,
    0x3fe393e0c0000000ULL, 0x3fe3d90260000000ULL,
    0x3fe41d8fe0000000ULL, 0x3fe4618bc0000000ULL,
    0x3fe4a4f840000000ULL, 0x3fe4e7d800000000ULL,
    0x3fe52a2d20000000ULL, 0x3fe56bf9c0000000ULL,
    0x3fe5ad4040000000ULL, 0x3fe5ee02a0000000ULL,
    0x3fe62e42e0000000ULL,
};

static const uint64_t LnTail64Table[65] ALIGN32_USED = {
    0x0000000000000000ULL, 0x3e361f807c79f3dbULL,
    0x3e6873c1980267c8ULL, 0x3e5ec65b9f88c69eULL,
    0x3e58022c54cc2f99ULL, 0x3e62c37a3a125330ULL,
    0x3e615cad69737c93ULL, 0x3e4d256ab1b285e9ULL,
    0x3e5b8abcb97a7aa2ULL, 0x3e6f34239659a5dcULL,
    0x3e6e07fd48d30177ULL, 0x3e6b32df4799f4f6ULL,
    0x3e6c29e4f4f21cf8ULL, 0x3e1086c848df1b59ULL,
    0x3e4cf456b4764130ULL, 0x3e63a02ffcb63398ULL,
    0x3e61e6a6886b0976ULL, 0x3e6b8abcb97a7aa2ULL,
    0x3e6b578f8aa35552ULL, 0x3e6139c871afb9fcULL,
    0x3e65d5d30701ce64ULL, 0x3e6de7bcb2d12142ULL,
    0x3e6d708e984e1664ULL, 0x3e556945e9c72f36ULL,
    0x3e20e2f613e85bdaULL, 0x3e3cb7e0b42724f6ULL,
    0x3e6fac04e52846c7ULL, 0x3e5e9b14aec442beULL,
    0x3e6b5de8034e7126ULL, 0x3e6dc157e1b259d3ULL,
    0x3e3b05096ad69c62ULL, 0x3e5c2116faba4cddULL,
    0x3e665fcc25f95b47ULL, 0x3e5a9a08498d4850ULL,
    0x3e6de647b1465f77ULL, 0x3e5da71b7bf7861dULL,
    0x3e3e6a6886b09760ULL, 0x3e6f0075eab0ef64ULL,
    0x3e33071282fb989bULL, 0x3e60eb43c3f1bed2ULL,
    0x3e5faf06ecb35c84ULL, 0x3e4ef1e63db35f68ULL,
    0x3e469743fb1a71a5ULL, 0x3e6c1cdf404e5796ULL,
    0x3e4094aa0ada625eULL, 0x3e6e2d4c96fde3ecULL,
    0x3e62f4d5e9a98f34ULL, 0x3e6467c96ecc5cbeULL,
    0x3e6e7040d03dec5aULL, 0x3e67bebf4282de36ULL,
    0x3e6289b11aeb783fULL, 0x3e5a891d1772f538ULL,
    0x3e634f10be1fb591ULL, 0x3e6d9ce1d316eb93ULL,
    0x3e63562a19a9c442ULL, 0x3e54e2adf548084cULL,
    0x3e508ce55cc8c97aULL, 0x3e30e2f613e85bdaULL,
    0x3e6db03ebb0227bfULL, 0x3e61b75bb09cb098ULL,
    0x3e496f16abb9df22ULL, 0x3e65b3f399411c62ULL,
    0x3e586b3e59f65355ULL, 0x3e52482ceae1ac12ULL,
    0x3e6efa39ef35793cULL,
};

static const uint64_t MantissaMask[2] ALIGN32_USED = {
    0x000fffffffffffffULL, 0x000fffffffffffffULL,
};

static const uint64_t Mask32bitsDP64[2] ALIGN32_USED = {
    0xffffffff00000000ULL, 0xffffffff00000000ULL,
};

static const uint64_t Mask1023[2] ALIGN32_USED = {
    0x00000000000003ffULL, 0x00000000000003ffULL,
};

static const uint64_t OneMask64[2] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff0000000000000ULL,
};

static const uint64_t PlusTwo[2] ALIGN32_USED = {
    0x4000000000000000ULL, 0x4000000000000000ULL,
};

static const uint64_t RealCa1[2] ALIGN32_USED = {
    0x3fb55555555554e6ULL, 0x3fb55555555554e6ULL,
};

static const uint64_t RealCa2[2] ALIGN32_USED = {
    0x3f89999999bac6d4ULL, 0x3f89999999bac6d4ULL,
};

static const uint64_t RealCa3[2] ALIGN32_USED = {
    0x3f62492307f1519fULL, 0x3f62492307f1519fULL,
};

static const uint64_t RealCa4[2] ALIGN32_USED = {
    0x3f3c8034c85dfff0ULL, 0x3f3c8034c85dfff0ULL,
};

static const uint64_t RealCb1[2] ALIGN32_USED = {
    0x3fb5555555555557ULL, 0x3fb5555555555557ULL,
};

static const uint64_t RealCb2[2] ALIGN32_USED = {
    0x3f89999999865edeULL, 0x3f89999999865edeULL,
};

static const uint64_t RealCb3[2] ALIGN32_USED = {
    0x3f6249423bd94741ULL, 0x3f6249423bd94741ULL,
};

static const uint64_t RealHalf[2] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
};

static const uint64_t RealLog10eLead[2] ALIGN32_USED = {
    0x3fdbcb7800000000ULL, 0x3fdbcb7800000000ULL,
};

static const uint64_t RealLog10eTail[2] ALIGN32_USED = {
    0x3ea8a93728719535ULL, 0x3ea8a93728719535ULL,
};

static const uint64_t RealLog2Lead[2] ALIGN32_USED = {
    0x3fe62e42e0000000ULL, 0x3fe62e42e0000000ULL,
};

static const uint64_t RealLog2Tail[2] ALIGN32_USED = {
    0x3e6efa39ef35793cULL, 0x3e6efa39ef35793cULL,
};

static const uint64_t RealLogThresh1[2] ALIGN32_USED = {
    0x3fee0faa00000000ULL, 0x3fee0faa00000000ULL,
};

static const uint64_t RealLogThresh2[2] ALIGN32_USED = {
    0x3ff1082c00000000ULL, 0x3ff1082c00000000ULL,
};

static const uint64_t RealNinf[2] ALIGN32_USED = {
    0xfff0000000000000ULL, 0xfff0000000000000ULL,
};

static const uint64_t RealQnan[2] ALIGN32_USED = {
    0x7ff8000000000000ULL, 0x7ff8000000000000ULL,
};

static const uint64_t SignBit64[4] ALIGN32_USED = {
    0x8000000000000000ULL, 0x8000000000000000ULL,
    0x8000000000000000ULL, 0x8000000000000000ULL,
};

static const uint64_t SignMask64[2] ALIGN32_USED = {
    0x7fffffffffffffffULL, 0x7fffffffffffffffULL,
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked, aligned(32)))
#endif
v_f64x2_t ALM_PROTO_FMA3(vrd2_log10)(v_f64x2_t x)
{
    begin_asm()
#ifdef _WIN64
        sub(imm(152), rsp)
        movdqa(xmm6, mem(rsp, 0))
        movdqa(xmm7, mem(rsp, 16))
        movdqa(xmm8, mem(rsp, 32))
        movdqa(xmm9, mem(rsp, 48))
        movdqa(xmm10, mem(rsp, 64))
        movdqa(xmm11, mem(rsp, 80))
        movdqa(xmm13, mem(rsp, 96))
        movdqa(xmm14, mem(rsp, 112))
        movdqa(xmm15, mem(rsp, 128))
#endif
        vpand(mem_rip(SignBit64), xmm0, xmm1)
        vpcmpeqq(mem_rip(SignBit64), xmm1, xmm1)
        vpand(mem_rip(ExpMask), xmm0, xmm14)
        vpcmpeqq(mem_rip(ExpMask), xmm14, xmm14)
        vpand(mem_rip(SignMask64), xmm0, xmm2)
        vpcmpeqq(mem_rip(Zero64), xmm2, xmm2)
        vpcmpgtq(mem_rip(RealLogThresh1), xmm0, xmm4)
        vcmplepd(mem_rip(RealLogThresh2), xmm0, xmm3)
        vpand(xmm3, xmm4, xmm3)
        vcmpltpd(mem_rip(ImpbitDp64), xmm0, xmm5)
        vpor(mem_rip(Constant1), xmm0, xmm4)
        vsubpd(mem_rip(Constant1), xmm4, xmm4)
        vpand(mem_rip(Expadjust), xmm5, xmm8)
        vandnpd(xmm0, xmm5, xmm6)
        vandpd(xmm5, xmm4, xmm5)
        vorpd(xmm6, xmm5, xmm5)
        vpand(mem_rip(ExpMask), xmm5, xmm4)
        vpsrlq(imm(52), xmm4, xmm4)
        vpsubq(mem_rip(Mask1023), xmm4, xmm4)
        vpsubq(xmm8, xmm4, xmm4)
        vpand(mem_rip(IndexMask1), xmm5, xmm6)
        vpand(mem_rip(IndexMask2), xmm5, xmm7)
        vpor(mem_rip(IndexMask3), xmm6, xmm6)
        vpsrlq(imm(45), xmm7, xmm7)
        vpsrlq(imm(46), xmm6, xmm6)
        vpaddq(xmm7, xmm6, xmm6)
        vpshufd(imm(0xF8), xmm6, xmm7)
        vcvtdq2pd(xmm7, xmm7)
        vmulpd(mem_rip(IndexConstant1), xmm7, xmm7)
        vpsubq(mem_rip(IndexConstant2), xmm6, xmm6)
        vpand(mem_rip(MantissaMask), xmm5, xmm5)
        vpor(mem_rip(RealHalf), xmm5, xmm5)
        vsubpd(xmm7, xmm5, xmm5)
        vmovd(xmm6, r10)
        vpsrldq(imm(8), xmm6, xmm6)
        vmovd(xmm6, r11)
        lea(mem_rip(LnLead64Table), r9)
        lea(mem_rip(LnTail64Table), r8)
        vmovlpd(mem(r9, r10, 8), xmm6, xmm6)
        vmovhpd(mem(r9, r11, 8), xmm6, xmm6)
        vmovlpd(mem(r8, r10, 8), xmm8, xmm8)
        vmovhpd(mem(r8, r11, 8), xmm8, xmm8)
        vfmadd231pd(mem_rip(RealHalf), xmm5, xmm7)
        vdivpd(xmm7, xmm5, xmm7)
        vmulpd(xmm7, xmm7, xmm5)
        vmovapd(mem_rip(RealCb3), xmm13)
        vmovapd(xmm5, xmm11)
        vfmadd213pd(mem_rip(RealCb2), xmm13, xmm5)
        vfmadd213pd(mem_rip(RealCb1), xmm11, xmm5)
        vmulpd(xmm5, xmm11, xmm5)
        vfmadd132pd(xmm7, xmm7, xmm5)
        vaddpd(xmm5, xmm8, xmm5)
        vpshufd(imm(0xF8), xmm4, xmm4)
        vcvtdq2pd(xmm4, xmm4)
        vmovapd(xmm4, xmm8)
        vfmadd132pd(mem_rip(RealLog2Lead), xmm6, xmm4)
        vfmadd132pd(mem_rip(RealLog2Tail), xmm5, xmm8)
        vmulpd(mem_rip(RealLog10eLead), xmm4, xmm9)
        vmulpd(mem_rip(RealLog10eLead), xmm8, xmm10)
        vmulpd(mem_rip(RealLog10eTail), xmm4, xmm4)
        vfmadd132pd(mem_rip(RealLog10eTail), xmm4, xmm8)
        vaddpd(xmm10, xmm8, xmm8)
        vaddpd(xmm9, xmm8, xmm8)
        vsubpd(mem_rip(OneMask64), xmm0, xmm4)
        vaddpd(mem_rip(PlusTwo), xmm4, xmm5)
        vdivpd(xmm5, xmm4, xmm6)
        vmulpd(xmm6, xmm4, xmm7)
        vaddpd(xmm6, xmm6, xmm6)
        vmulpd(xmm6, xmm6, xmm9)
        vmovapd(mem_rip(RealCa4), xmm10)
        vfmadd213pd(mem_rip(RealCa3), xmm9, xmm10)
        vfmadd213pd(mem_rip(RealCa2), xmm9, xmm10)
        vfmadd213pd(mem_rip(RealCa1), xmm9, xmm10)
        vmulpd(xmm9, xmm10, xmm10)
        vmulpd(xmm6, xmm10, xmm10)
        vsubpd(xmm7, xmm10, xmm10)
        movapd(xmm4, xmm7)
        pand(mem_rip(Mask32bitsDP64), xmm4)
        subpd(xmm4, xmm7)
        addpd(xmm7, xmm10)
        vmulpd(mem_rip(RealLog10eLead), xmm4, xmm9)
        vmulpd(mem_rip(RealLog10eLead), xmm10, xmm11)
        vmulpd(mem_rip(RealLog10eTail), xmm4, xmm4)
        vfmadd132pd(mem_rip(RealLog10eTail), xmm4, xmm10)
        vaddpd(xmm11, xmm10, xmm10)
        vaddpd(xmm9, xmm10, xmm10)
        vandnpd(xmm8, xmm3, xmm15)
        vandpd(xmm3, xmm10, xmm3)
        vorpd(xmm15, xmm3, xmm3)
        vmovapd(mem_rip(RealQnan), xmm15)
        vmovapd(mem_rip(RealNinf), xmm13)
        vandnpd(xmm3, xmm1, xmm8)
        vandpd(xmm1, xmm15, xmm1)
        vorpd(xmm8, xmm1, xmm1)
        vandpd(xmm2, xmm13, xmm8)
        vandnpd(xmm1, xmm2, xmm3)
        vorpd(xmm3, xmm8, xmm8)
        vpand(mem_rip(SignBit64), xmm0, xmm3)
        vaddpd(xmm0, xmm0, xmm0)
        vpsrlq(imm(12), xmm3, xmm3)
        vpand(xmm14, xmm3, xmm3)
        vpand(xmm14, xmm0, xmm0)
        vpor(xmm3, xmm0, xmm0)
        vpandn(xmm8, xmm14, xmm14)
        vpor(xmm14, xmm0, xmm0)
#ifdef _WIN64
        movdqa(mem(rsp, 0), xmm6)
        movdqa(mem(rsp, 16), xmm7)
        movdqa(mem(rsp, 32), xmm8)
        movdqa(mem(rsp, 48), xmm9)
        movdqa(mem(rsp, 64), xmm10)
        movdqa(mem(rsp, 80), xmm11)
        movdqa(mem(rsp, 96), xmm13)
        movdqa(mem(rsp, 112), xmm14)
        movdqa(mem(rsp, 128), xmm15)
        add(imm(152), rsp)
#endif
        ret
    end_asm( : : :)
}
