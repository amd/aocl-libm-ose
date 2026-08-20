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
#include "libm/compiler.h"
#include "libm_macros.h"
#include "libm/amd_funcs_internal.h"

#define LIBM_ASM_SYNTAX_ATT
#include "libm/libm_asm_macros.h"

static const uint64_t B1[1] ALIGN32_USED = {
    0x3fc5555555555549ULL,
};

static const uint64_t B2[1] ALIGN32_USED = {
    0x3fa55555555554b6ULL,
};

static const uint64_t B3[1] ALIGN32_USED = {
    0x3f8111111111a9f3ULL,
};

static const uint64_t B4[1] ALIGN32_USED = {
    0x3f56c16c16ce14c6ULL,
};

static const uint64_t B5[1] ALIGN32_USED = {
    0x3f2a01a01159dd2dULL,
};

static const uint64_t B6[1] ALIGN32_USED = {
    0x3efa019f635825c4ULL,
};

static const uint64_t B7[1] ALIGN32_USED = {
    0x3ec71e14bfe3db59ULL,
};

static const uint64_t B8[1] ALIGN32_USED = {
    0x3e928295484734eaULL,
};

static const uint64_t B9[1] ALIGN32_USED = {
    0x3e5a2836aa646b96ULL,
};

static const uint64_t One[1] ALIGN32_USED = {
    0x3ff0000000000000ULL,
};

static const uint64_t TwopM7[1] ALIGN32_USED = {
    0x3f80000000000000ULL,
};

static const uint64_t Log2By64Mhead[2] ALIGN32_USED = {
    0xbf862e42fefa0000ULL, 0xbf862e42fefa0000ULL,
};

static const uint64_t Log2By64Mtail[2] ALIGN32_USED = {
    0xbd1cf79abc9e3b39ULL, 0xbd1cf79abc9e3b39ULL,
};

static const uint64_t LogOneMinusOneByFour[1] ALIGN32_USED = {
    0xbfd269621134db93ULL,
};

static const uint64_t LogOnePlusOneByFour[1] ALIGN32_USED = {
    0x3fcc8ff7c79a9a22ULL,
};

static const uint64_t MaxExpm1Arg[1] ALIGN32_USED = {
    0x40862e6666666666ULL,
};

static const uint64_t MinExpm1Arg[1] ALIGN32_USED = {
    0xc042b708872320e1ULL,
};

static const uint64_t MinusZero[1] ALIGN32_USED = {
    0x8000000000000000ULL,
};

static const uint64_t Real1By120[4] ALIGN32_USED = {
    0x3f81111111111111ULL, 0x3f81111111111111ULL,
    0x3f81111111111111ULL, 0x3f81111111111111ULL,
};

static const uint64_t Real1By2[4] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
};

static const uint64_t Real1By24[4] ALIGN32_USED = {
    0x3fa5555555555555ULL, 0x3fa5555555555555ULL,
    0x3fa5555555555555ULL, 0x3fa5555555555555ULL,
};

static const uint64_t Real1By6[4] ALIGN32_USED = {
    0x3fc5555555555555ULL, 0x3fc5555555555555ULL,
    0x3fc5555555555555ULL, 0x3fc5555555555555ULL,
};

static const uint64_t Real1By720[4] ALIGN32_USED = {
    0x3f56c16c16c16c17ULL, 0x3f56c16c16c16c17ULL,
    0x3f56c16c16c16c17ULL, 0x3f56c16c16c16c17ULL,
};

static const uint64_t Real64ByLog2[2] ALIGN32_USED = {
    0x40571547652b82feULL, 0x40571547652b82feULL,
};

static const uint64_t TwoToJby64HeadTable[64] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff02c9a30000000ULL,
    0x3ff059b0d0000000ULL, 0x3ff0874510000000ULL,
    0x3ff0b55860000000ULL, 0x3ff0e3ec30000000ULL,
    0x3ff11301d0000000ULL, 0x3ff1429aa0000000ULL,
    0x3ff172b830000000ULL, 0x3ff1a35be0000000ULL,
    0x3ff1d48730000000ULL, 0x3ff2063b80000000ULL,
    0x3ff2387a60000000ULL, 0x3ff26b4560000000ULL,
    0x3ff29e9df0000000ULL, 0x3ff2d285a0000000ULL,
    0x3ff306fe00000000ULL, 0x3ff33c08b0000000ULL,
    0x3ff371a730000000ULL, 0x3ff3a7db30000000ULL,
    0x3ff3dea640000000ULL, 0x3ff4160a20000000ULL,
    0x3ff44e0860000000ULL, 0x3ff486a2b0000000ULL,
    0x3ff4bfdad0000000ULL, 0x3ff4f9b270000000ULL,
    0x3ff5342b50000000ULL, 0x3ff56f4730000000ULL,
    0x3ff5ab07d0000000ULL, 0x3ff5e76f10000000ULL,
    0x3ff6247eb0000000ULL, 0x3ff6623880000000ULL,
    0x3ff6a09e60000000ULL, 0x3ff6dfb230000000ULL,
    0x3ff71f75e0000000ULL, 0x3ff75feb50000000ULL,
    0x3ff7a11470000000ULL, 0x3ff7e2f330000000ULL,
    0x3ff8258990000000ULL, 0x3ff868d990000000ULL,
    0x3ff8ace540000000ULL, 0x3ff8f1ae90000000ULL,
    0x3ff93737b0000000ULL, 0x3ff97d8290000000ULL,
    0x3ff9c49180000000ULL, 0x3ffa0c6670000000ULL,
    0x3ffa5503b0000000ULL, 0x3ffa9e6b50000000ULL,
    0x3ffae89f90000000ULL, 0x3ffb33a2b0000000ULL,
    0x3ffb7f76f0000000ULL, 0x3ffbcc1e90000000ULL,
    0x3ffc199bd0000000ULL, 0x3ffc67f120000000ULL,
    0x3ffcb720d0000000ULL, 0x3ffd072d40000000ULL,
    0x3ffd5818d0000000ULL, 0x3ffda9e600000000ULL,
    0x3ffdfc9730000000ULL, 0x3ffe502ee0000000ULL,
    0x3ffea4afa0000000ULL, 0x3ffefa1be0000000ULL,
    0x3fff507650000000ULL, 0x3fffa7c180000000ULL,
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

static const uint64_t TwoToJby64TailTable[64] ALIGN32_USED = {
    0x0000000000000000ULL, 0x3e6cef00c1dcdef9ULL,
    0x3e48ac2ba1d73e2aULL, 0x3e60eb37901186beULL,
    0x3e69f3121ec53172ULL, 0x3e469e8d10103a17ULL,
    0x3df25b50a4ebbf1aULL, 0x3e6d525bbf668203ULL,
    0x3e68faa2f5b9bef9ULL, 0x3e66df96ea796d31ULL,
    0x3e368b9aa7805b80ULL, 0x3e60c519ac771dd6ULL,
    0x3e6ceac470cd83f5ULL, 0x3e5789f37495e99cULL,
    0x3e547f7b84b09745ULL, 0x3e5b900c2d002475ULL,
    0x3e64636e2a5bd1abULL, 0x3e4320b7fa64e430ULL,
    0x3e5ceaa72a9c5154ULL, 0x3e53967fdba86f24ULL,
    0x3e682468446b6824ULL, 0x3e3f72e29f84325bULL,
    0x3e18624b40c4dbd0ULL, 0x3e5704f3404f068eULL,
    0x3e54d8a89c750e5eULL, 0x3e5a74b29ab4cf62ULL,
    0x3e5a753e077c2a0fULL, 0x3e5ad49f699bb2c0ULL,
    0x3e6a90a852b19260ULL, 0x3e56b48521ba6f93ULL,
    0x3e0d2ac258f87d03ULL, 0x3e42a91124893ecfULL,
    0x3e59fcef32422cbeULL, 0x3e68ca345de441c5ULL,
    0x3e61d8bee7ba46e1ULL, 0x3e59099f22fdba6aULL,
    0x3e4f580c36bea881ULL, 0x3e5b3d398841740aULL,
    0x3e62999c25159f11ULL, 0x3e668925d901c83bULL,
    0x3e415506dadd3e2aULL, 0x3e622aee6c57304eULL,
    0x3e29b8bc9e8a0387ULL, 0x3e6fbc9c9f173d24ULL,
    0x3e451f8480e3e235ULL, 0x3e66bbcac96535b5ULL,
    0x3e41f12ae45a1224ULL, 0x3e55e7f6fd0fac90ULL,
    0x3e62b5a75abd0e69ULL, 0x3e609e2bf5ed7fa1ULL,
    0x3e47daf237553d84ULL, 0x3e12f074891ee83dULL,
    0x3e6b0aa538444196ULL, 0x3e6cafa29694426fULL,
    0x3e69df20d22a0797ULL, 0x3e640f12f71a1e45ULL,
    0x3e69f7490e4bb40bULL, 0x3e4ed9942b84600dULL,
    0x3e4bdcdaf5cb4656ULL, 0x3e5e2cffd89cf44cULL,
    0x3e452486cc2c7b9dULL, 0x3e6cc2b44eee3fa4ULL,
    0x3e66dc8a80ce9f09ULL, 0x3e39e90d82e90a7eULL,
};

static const uint64_t ZeroOne[2] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x0000000000000000ULL,
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked, aligned(32)))
#endif
void ALM_PROTO_FMA3(vrda_expm1)(int len, const double *src, double *dst)
{
    begin_asm()
#ifdef _WIN64
        sub(imm(72), rsp)
        movdqa(xmm10, mem(rsp, 0))
        movdqa(xmm11, mem(rsp, 16))
        movdqa(xmm12, mem(rsp, 32))
        mov(rdi, mem(rsp, 48))
        mov(rsi, mem(rsp, 56))
        mov(rcx, rdi)
        mov(rdx, rsi)
        mov(r8, rdx)
#endif
        cmp(imm(0x0), edi)
        jle(__return)
        cmp(imm(0x0), rsi)
        je(__return)
        cmp(imm(0x0), rdx)
        je(__return)
        label_align(__process_next2, 4)
        sub(imm(0x2), rdi)
        cmp(imm(-1), rdi)
        jle(__process_first1)
        movdqu(mem(rsi, rdi, 8), xmm0)
        jmp(__process)
        label_align(__process_first1, 4)
        jl(__return)
        mov(imm(0x0), rcx)
        movsd(mem(rsi, rcx, 8), xmm0)
        label_align(__process, 4)
        vmovdqa(xmm0, xmm10)
        vmulpd(mem_rip(Real64ByLog2), xmm0, xmm3)
        vcvttpd2dq(xmm3, xmm4)
        vcvtdq2pd(xmm4, xmm3)
        vmovd(xmm4, rax)
        vfmadd231pd(mem_rip(Log2By64Mhead), xmm3, xmm0)
        vfmadd231pd(mem_rip(Log2By64Mtail), xmm3, xmm0)
        vmovdqa(xmm0, xmm2)
        mov(imm(0x0000003f0000003f), r8)
        andq(rax, r8)
        mov(r8d, eax)
        shr(imm(32), r8)
        vpsrad(imm(6), xmm4, xmm12)
        vmovapd(mem_rip(Real1By720), xmm3)
        vfmadd213pd(mem_rip(Real1By120), xmm2, xmm3)
        vfmadd213pd(mem_rip(Real1By24), xmm2, xmm3)
        vfmadd213pd(mem_rip(Real1By6), xmm2, xmm3)
        vfmadd213pd(mem_rip(Real1By2), xmm2, xmm3)
        vmulpd(xmm2, xmm2, xmm0)
        vfmadd213pd(xmm2, xmm3, xmm0)
        vmovhlps(xmm0, xmm11, xmm11)
        vmovd(xmm12, rcx)
        mov(imm(2), r9d)
        jmp(__next)
        label_align(__check_n_prepare_next, 4)
        dec(r9d)
        jz(__store)
        vmovlhps(xmm0, xmm11, xmm11)
        vmovdqa(xmm11, xmm0)
        vpsrldq(imm(8), xmm10, xmm10)
        mov(r8, rax)
        vmovq(xmm12, rcx)
        shr(imm(32), rcx)
        label_align(__next, 4)
        vucomisd(mem_rip(MaxExpm1Arg), xmm10)
        ja(__y_is_inf)
        jp(__y_is_nan)
        vucomisd(mem_rip(MinExpm1Arg), xmm10)
        jb(__y_is_minusOne)
        vucomisd(mem_rip(LogOneMinusOneByFour), xmm10)
        jbe(__Normal_Flow)
        vucomisd(mem_rip(LogOnePlusOneByFour), xmm10)
        jb(__Small_Arg)
        label_align(__Normal_Flow, 4)
        lea(mem_rip(TwoToJby64Table), r11)
        vmovsd(mem(r11, rax, 8), xmm1)
        lea(mem_rip(TwoToJby64HeadTable), r10)
        lea(mem_rip(TwoToJby64TailTable), r11)
        vmovsd(mem(r10, rax, 8), xmm2)
        vmovsd(mem(r11, rax, 8), xmm3)
        mov(imm(1023), eax)
        sub(ecx, eax)
        shl(imm(52), rax)
        cmp(imm(52), ecx)
        jg(__M_Above_52)
        cmp(imm(-7), ecx)
        jl(__M_Below_Minus7)
        shl(imm(52), rcx)
        vmovd(rcx, xmm4)
        vaddsd(mem_rip(One), xmm0, xmm1)
        vmulsd(xmm3, xmm1, xmm1)
        vmovd(rax, xmm3)
        vfmadd213sd(xmm1, xmm2, xmm0)
        vsubsd(xmm3, xmm2, xmm2)
        vaddsd(xmm2, xmm0, xmm0)
        vpaddq(xmm4, xmm0, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__M_Above_52, 4)
        cmp(imm(1024), ecx)
        je(__M_Equals_1024)
        vmovq(rax, xmm4)
        vsubsd(xmm4, xmm3, xmm3)
        vfmadd213pd(xmm3, xmm1, xmm0)
        vmovdqa(xmm0, xmm4)
        vaddpd(xmm2, xmm4, xmm0)
        shl(imm(52), rcx)
        vmovq(rcx, xmm1)
        vpaddq(xmm1, xmm0, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__M_Below_Minus7, 4)
        vfmadd213pd(xmm3, xmm1, xmm0)
        vmovdqa(xmm0, xmm4)
        vaddpd(xmm2, xmm4, xmm0)
        shl(imm(52), rcx)
        vmovq(rcx, xmm1)
        vpaddq(xmm1, xmm0, xmm0)
        vsubpd(mem_rip(ZeroOne), xmm0, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__M_Equals_1024, 4)
        mov(imm(0x4000000000000000), rax)
        vfmadd213pd(xmm3, xmm1, xmm0)
        vmovdqa(xmm0, xmm4)
        vaddpd(xmm2, xmm4, xmm0)
        vmovq(rax, xmm1)
        vpaddq(xmm1, xmm0, xmm0)
        vmovq(xmm0, rax)
        mov(imm(0x7FF0000000000000), rcx)
        andq(rcx, rax)
        cmp(rcx, rax)
        je(__y_is_inf)
        jmp(__check_n_prepare_next)
        label_align(__Small_Arg, 4)
        vmovapd(xmm10, xmm0)
        vucomisd(mem_rip(MinusZero), xmm0)
        je(__check_n_prepare_next)
        mov(imm(0x01E0000000000000), rax)
        vmovq(rax, xmm1)
        vpaddq(xmm0, xmm1, xmm1)
        vaddsd(xmm0, xmm1, xmm2)
        vsubsd(xmm1, xmm2, xmm2)
        vsubsd(xmm2, xmm0, xmm1)
        vmulsd(xmm2, xmm2, xmm3)
        vmulsd(mem_rip(Real1By2), xmm3, xmm3)
        vmulsd(mem_rip(B9), xmm0, xmm5)
        vaddsd(mem_rip(B8), xmm5, xmm5)
        vfmadd213sd(mem_rip(B7), xmm0, xmm5)
        vfmadd213sd(mem_rip(B6), xmm0, xmm5)
        vfmadd213sd(mem_rip(B5), xmm0, xmm5)
        vfmadd213sd(mem_rip(B4), xmm0, xmm5)
        vfmadd213sd(mem_rip(B3), xmm0, xmm5)
        vfmadd213sd(mem_rip(B2), xmm0, xmm5)
        vfmadd213sd(mem_rip(B1), xmm0, xmm5)
        vmovdqa(xmm5, xmm4)
        vmulsd(xmm0, xmm4, xmm5)
        vmulsd(xmm0, xmm5, xmm4)
        vmulsd(xmm0, xmm4, xmm5)
        vaddsd(xmm0, xmm2, xmm4)
        vmulsd(xmm1, xmm4, xmm4)
        vmulsd(mem_rip(Real1By2), xmm4, xmm4)
        vucomisd(mem_rip(TwopM7), xmm3)
        jb(__returnNext)
        vaddsd(xmm4, xmm1, xmm1)
        vaddsd(xmm5, xmm1, xmm1)
        vaddsd(xmm3, xmm2, xmm2)
        vaddsd(xmm2, xmm1, xmm1)
        vmovapd(xmm1, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__returnNext, 4)
        vaddsd(xmm5, xmm4, xmm4)
        vaddsd(xmm4, xmm3, xmm3)
        vaddsd(xmm3, xmm0, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__y_is_inf, 4)
        mov(imm(0x7ff0000000000000), rax)
        vmovq(rax, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__y_is_nan, 4)
        vaddsd(xmm0, xmm0, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__y_is_minusOne, 4)
        mov(imm(0xBFF0000000000000), rax)
        vmovq(rax, xmm0)
        jmp(__check_n_prepare_next)
        label_align(__store, 4)
        vmovdqa(xmm0, xmm1)
        vmovhlps(xmm11, xmm0, xmm0)
        vmovlhps(xmm1, xmm0, xmm0)
        cmp(imm(-1), rdi)
        je(__store1)
        vmovdqu(xmm0, mem(rdx, rdi, 8))
        jmp(__process_next2)
        label_align(__store1, 4)
        inc(rdi)
        vmovsd(xmm0, mem(rdx, rdi, 8))
        label_align(__return, 4)
#ifdef _WIN64
        movdqa(mem(rsp, 0), xmm10)
        movdqa(mem(rsp, 16), xmm11)
        movdqa(mem(rsp, 32), xmm12)
        mov(mem(rsp, 48), rdi)
        mov(mem(rsp, 56), rsi)
        add(imm(72), rsp)
#endif
        ret
    end_asm( : : :)
}
