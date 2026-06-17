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
#include "libm/alm_special.h"

#define LIBM_ASM_SYNTAX_ATT
#include "libm/libm_asm_macros.h"

#define TEMP_X      0x0
#define TEMP_Y      0x10

static const uint64_t L__sign_mask_64[2]        ALIGN16_USED = { 0x8000000000000000ULL, 0x0ULL };
static const uint64_t L__exp_mask_64[2]         ALIGN16_USED = { 0x7FF0000000000000ULL, 0x0ULL };
static const uint64_t L__27bit_andingmask_64[2] ALIGN16_USED = { 0xfffffffff8000000ULL, 0x0ULL };
static const uint64_t L__Zero_64[2]             ALIGN16_USED = { 0x0ULL, 0x0ULL };
static const uint64_t L__QNaN_mask_64[2]        ALIGN16_USED = { 0x0008000000000000ULL, 0x0ULL };
static const uint64_t L__Nan_64[2]              ALIGN16_USED = { 0x7FFFFFFFFFFFFFFFULL, 0x0ULL };
static const uint64_t L__ZeroPointFive[2]       ALIGN16_USED = { 0x3FE0000000000000ULL, 0x0ULL };
static const uint64_t L__Qnan[2]                ALIGN16_USED = { 0x7FF8000000000000ULL, 0x7FF8000000000000ULL };

/*
 * Portable C-macro inline-asm implementation of remainder,
 * structurally identical to gas/remainder.S.
 */
#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked))
#endif
double __amd_bas64_remainder(double x, double y);
double __amd_bas64_remainder(double x, double y)
{
    begin_asm()
#ifdef _WIN64
        sub(imm(0xa8), rsp)
        movdqa(xmm6, mem(rsp, 0x40))
        movdqa(xmm7, mem(rsp, 0x50))
#endif
        movd(xmm0, r8)
        movd(xmm1, r9)
        andq(mem_rip(L__Nan_64), r8)
        andq(mem_rip(L__Nan_64), r9)
        jz(y_is_zero)
        cmp(mem_rip(L__exp_mask_64), r8)
        jge(input_x_is_nan_inf)
        cmp(mem_rip(L__exp_mask_64), r9)
        jg(input_y_is_nan_inf)
        cmp(r9, r8)
        jz(input_is_equal)

        movd(xmm0, r8)
        movd(xmm1, r9)
        movsd(xmm0, xmm2)
        movsd(xmm1, xmm3)
        movsd(xmm0, xmm4)
        movsd(xmm1, xmm5)
        pand(mem_rip(L__Nan_64), xmm4)
        pand(mem_rip(L__Nan_64), xmm5)
        comisd(xmm5, xmm4)
        jbe(return_immediate)
        mov(mem_rip(L__exp_mask_64), r10)
        andq(r10, r8)
        andq(r10, r9)
        xorq(r10, r10)
        ror(imm(52), r8)
        ror(imm(52), r9)

        cmp(imm(0), r8)
        jz(large_exp_diff_computation)
        cmp(imm(0), r9)
        jz(large_exp_diff_computation)
        sub(r9, r8)
        cmp(imm(52), r8)
        jge(large_exp_diff_computation)
        cmp(imm(0x7FF), r8)
        jz(dividend_is_infinity)

        label(direct_computation)
        movapd(xmm4, xmm2)
        movapd(xmm5, xmm3)
        divsd(xmm3, xmm2)
        cvttsd2siq(xmm2, r8)
        mov(r8, r10)
        andq(imm(0x01), r10)
        cvtsi2sdq(r8, xmm2)

        movapd(mem_rip(L__27bit_andingmask_64), xmm4)
        movapd(xmm5, xmm1)
        movapd(xmm2, xmm6)
        movapd(xmm2, xmm7)
        mulpd(xmm5, xmm7)
        andpd(xmm4, xmm1)
        andpd(xmm4, xmm2)
        subsd(xmm1, xmm5)
        subsd(xmm2, xmm6)

        movapd(xmm1, xmm4)
        mulsd(xmm2, xmm4)
        subsd(xmm7, xmm4)
        mulsd(xmm6, xmm1)
        addsd(xmm1, xmm4)
        mulsd(xmm5, xmm2)
        addsd(xmm2, xmm4)
        mulsd(xmm5, xmm6)
        addsd(xmm4, xmm6)

        movapd(xmm0, xmm1)
        pand(mem_rip(L__Nan_64), xmm1)
        movapd(xmm1, xmm2)
        subsd(xmm7, xmm1)
        subsd(xmm1, xmm2)
        subsd(xmm7, xmm2)
        subsd(xmm6, xmm2)
        addsd(xmm1, xmm2)

        movapd(xmm2, xmm4)
        movapd(xmm3, xmm5)
        addsd(xmm4, xmm4)
        comisd(xmm4, xmm3)
        jb(substract_w)
        mulpd(mem_rip(L__ZeroPointFive), xmm5)
        comisd(xmm2, xmm5)
        jb(substract_w)
        cmp(imm(0x01), r10)
        jnz(finish)
        comisd(xmm4, xmm3)
        jz(substract_w)
        comisd(xmm0, xmm5)
        jnz(finish)

        label(substract_w)
        subsd(xmm3, xmm2)

        label(finish)
        comisd(mem_rip(L__Zero_64), xmm0)
        ja(not_negative_number1)

        label(negative_number1)
        movapd(mem_rip(L__Zero_64), xmm0)
        subsd(xmm2, xmm0)
        jmp(done)

        label(not_negative_number1)
        movapd(xmm2, xmm0)
        jmp(done)

        label(large_exp_diff_computation)
#ifdef _WIN64
        movsd(xmm0, mem(rsp, 0x20))
        movsd(xmm1, mem(rsp, 0x30))
        ffree_st0
        ffree_st1
        fldl_(mem(rsp, 0x30))
        fldl_(mem(rsp, 0x20))
        fnclex_()

        label(repeat)
        fprem1_()
        fnstsw_(ax)
        andw(imm(0x0400), ax)
        cmp(imm(0x0400), ax)
        jz(repeat)
        fstpl_(mem(rsp, 0x20))
        fstpl_(mem(rsp, 0x30))
        movsd(mem(rsp, 0x20), xmm0)
#else
        sub(imm(0x80), rsp)
        movsd(xmm0, mem(rsp, TEMP_X))
        movsd(xmm1, mem(rsp, TEMP_Y))
        ffree_st0
        ffree_st1
        fldl_(mem(rsp, TEMP_Y))
        fldl_(mem(rsp, TEMP_X))
        fnclex_()

        label(repeat)
        fprem1_()
        fnstsw_(ax)
        andw(imm(0x0400), ax)
        cmp(imm(0x0400), ax)
        jz(repeat)
        fstpl_(mem(rsp, TEMP_X))
        fstpl_(mem(rsp, TEMP_Y))
        movsd(mem(rsp, TEMP_X), xmm0)
        add(imm(0x80), rsp)
#endif
        jmp(done)

        label(input_is_equal)
        movsd(xmm0, xmm1)
        pand(mem_rip(L__sign_mask_64), xmm1)
        movsd(mem_rip(L__Zero_64), xmm0)
        por(xmm1, xmm0)
        jmp(done)

        label(input_x_is_nan_inf)
        je(dividend_is_infinity)
        cmp(mem_rip(L__exp_mask_64), r9)
        jle(input_x_is_nan)
        cmp(mem_rip(L__Qnan), r9)
        jl(input_y_is_snan)

        label(input_x_is_nan)
        movapd(xmm0, xmm1)
        por(mem_rip(L__Qnan), xmm1)
#ifdef _WIN64
        mov(imm(1), r8)
#else
        movl(imm(1), edi)
#endif
        call_fn(_remainder_special)
        jmp(done)

        label(input_y_is_nan_inf)
        cmp(mem_rip(L__Qnan), r9)
        jl(input_y_is_snan)
        movapd(xmm1, xmm0)
        por(mem_rip(L__Qnan), xmm1)
#ifdef _WIN64
        mov(imm(1), r8)
#else
        movl(imm(1), edi)
#endif
        call_fn(_remainder_special)
        jmp(done)

        label(input_y_is_snan)
        movapd(xmm1, xmm0)
        por(mem_rip(L__Qnan), xmm1)
#ifdef _WIN64
        mov(imm(1), r8)
#else
        movl(imm(1), edi)
#endif
        call_fn(_remainder_special)
        jmp(done)

        label(dividend_is_infinity)
        cmp(mem_rip(L__exp_mask_64), r9)
        jg(input_y_is_nan_inf)
        movapd(xmm0, xmm1)
        por(mem_rip(L__QNaN_mask_64), xmm1)
#ifdef _WIN64
        mov(imm(3), r8)
#else
        movl(imm(3), edi)
#endif
        call_fn(_remainder_special)
        jmp(done)

        label(return_immediate)
        movapd(xmm5, xmm7)
        mulpd(mem_rip(L__ZeroPointFive), xmm5)
        comisd(xmm4, xmm5)
        jae(found_result1)
        subsd(xmm7, xmm4)
        comisd(mem_rip(L__Zero_64), xmm0)
        ja(not_negative_number)

        label(negative_number)
        movapd(mem_rip(L__Zero_64), xmm0)
        subsd(xmm4, xmm0)
        jmp(done)

        label(not_negative_number)
        movapd(xmm4, xmm0)
        jmp(done)

        label(found_result1)
        jmp(done)

        label(y_is_zero)
        movapd(mem_rip(L__Qnan), xmm1)
#ifdef _WIN64
        mov(imm(2), r8)
#else
        movl(imm(2), edi)
#endif
        call_fn(_remainder_special)
        jmp(done)

        label(done)
#ifdef _WIN64
        movdqa(mem(rsp, 0x50), xmm7)
        movdqa(mem(rsp, 0x40), xmm6)
        add(imm(0xa8), rsp)
#endif
        ret
    end_asm( : : :)
}
