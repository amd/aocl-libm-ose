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

#include "libm_util_amd.h"

#define LIBM_ASM_SYNTAX_ATT
#include "libm/libm_asm_macros.h"


static const unsigned char L__2_by_pi_bits[] ALIGN16_USED = {
    224,241,27,193,12,88,33,116,53,126,196,126,237,175,169,75,74,41,222,231,
    28,244,236,197,151,175,31,235,158,212,181,168,127,121,154,253,24,61,221,38,
    44,159,60,251,217,180,125,180,41,104,45,70,188,188,63,96,22,120,255,95,
    226,127,236,160,228,247,46,126,17,114,210,231,76,13,230,88,71,230,4,249,
    125,209,154,192,113,166,19,18,237,186,212,215,8,162,251,156,166,196,114,172,
    119,248,115,72,70,39,168,187,36,25,128,75,55,9,233,184,145,220,134,21,
    239,122,175,142,69,249,7,65,14,241,100,86,138,109,3,119,211,212,71,95,
    157,240,167,84,16,57,185,13,230,139,2,0,0,0,0,0,0,0
};

static const uint64_t L__piby2 ALIGN16_USED = 0x3ff921fb54442d18ULL;

/*
 * Portable C-macro inline-asm implementation of remainder_piby2f,
 * structurally identical to gas/remainder_piby2f_new.S.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked))
#endif
void __amd_remainder_piby2d2f(uint64_t x, double *r, int *region);
void __amd_remainder_piby2d2f(uint64_t x, double *r, int *region)
{
    begin_asm()
#ifdef _WIN64
        push(rdi)
        push(rsi)
        mov(rcx, rdi)   /* x (uint64_t) was in rcx */
        mov(rdx, rsi)   /* r  (double*) was in rdx */
        mov(r8,  rdx)   /* region (int*) was in r8  */
#endif
        movq(rdi, xmm0)
        lea_rip(L__2_by_pi_bits, r9)
        movq(xmm0, r11)
        mov(r11, rcx)
        shr(imm(52), r11)
        sub(imm(1023), r11)

        mov(r11, r10)
        shr(imm(3), r10)
        sub(imm(134), r10)
        neg(r10)

        mov(mem(r9, r10, 1), rax)
        mov(rdx, rdi)

        shl(imm(12), rcx)
        shr(imm(12), rcx)
        bts(imm(52), rcx)

        add(imm(8), r10)
        movdqu(mem(r9, r10, 1), xmm0)

        mulq(rcx)
        mov(rax, r8)
        mov(rdx, r10)
        movq(xmm0, rax)
        mulq(rcx)
        andq(imm(7), r11)
        psrldq(imm(8), xmm0)
        add(r10, rax)
        adc(imm(0), rdx)
        mov(rax, r9)
        mov(rdx, r10)
        movq(xmm0, rax)
        mulq(rcx)
        add(rax, r10)

        mov(imm(54), rcx)
        mov(r10, rax)
        sub(r11, rcx)
        xorq(rdx, rdx)
        shr(cl, rax)
        jnc(no_point_five)
        notq(r10)
        notq(r9)
        notq(r8)
        movabs(imm(0x8000000000000000), rdx)

        label(no_point_five)
        adc(imm(0), rax)
        andq(imm(3), rax)
        mov(eax, mem(rdi))

        mov(r11, rcx)
        add(imm(10), rcx)
        shl(cl, r10)
        shr(cl, r10)
        sub(imm(64), rcx)
        mov(rcx, r11)

        bsr(r10, rcx)
        jnz(form_mantissa)
        mov(r9, r10)
        mov(r8, r9)
        bsr(r10, rcx)
        sub(imm(64), r11)

        label(form_mantissa)
        add(rcx, r11)
        sub(imm(52), rcx)
        cmp(imm(0), rcx)
        jl(hsb_below_52)
        je(form_numbers)
        mov(r10, r8)
        shr(cl, r10)
        jmp(form_numbers)

        label(hsb_below_52)
        neg(rcx)
        mov(r9, rax)
        shl(cl, r10)
        shl(cl, r9)
        sub(imm(64), rcx)
        neg(rcx)
        shr(cl, rax)
        orq(rax, r10)

        label(form_numbers)
        add(imm(1023), r11)
        btr(imm(52), r10)
        mov(r11, rcx)
        orq(rdx, r10)
        shl(imm(52), rcx)
        orq(rcx, r10)

        movq(r10, xmm0)
        mulsd(mem_rip(L__piby2), xmm0)
        movsd(xmm0, mem(rsi))
#ifdef _WIN64
        pop(rsi)
        pop(rdi)
#endif
        ret
    end_asm( : : :)
}
