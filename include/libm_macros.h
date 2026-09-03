/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __LIBM_MACROS_H__
#define __LIBM_MACROS_H__

/*
 * Spare the ugliness to non-coders
 */
#if defined(WINDOWS)
#include <libm_win_macros.h>
#else
#include <libm_nix_macros.h>
#endif

#ifndef ALM_OVERRIDE

#define ALM_PREFIX                      amd

#define FN_PROTOTYPE(fn_name)           amd_##fn_name

/*
 * ALM_PROTO_OPT to be removed, once all the FN_PROTOTYPE_OPT is converted to ALM_PROTO
 */
#define ALM_PROTO_OPT(fn_name)  amd_opt_##fn_name
#else
#ifndef ALM_ARCH
#error "ALM_ARCH needs to be defined or ALM_OVERRIDE needs to be removed"
#endif

#endif /* ALM_OVERRIDE */

#if defined(ALM_SUFFIX)
#define ALM_PROTO(x)      ALM_MAKE_PROTO_SFX(ALM_PREFIX, x, ALM_SUFFIX)
#else
#define ALM_PROTO(x)      ALM_MAKE_PROTO(ALM_PREFIX, x)
#endif

#define ALM_MAKE_PROTO_SFX(pfx, fn, sfx)        __ALM_MAKE_PROTO_SFX(pfx, fn, sfx)
#define ALM_MAKE_PROTO(pfx, fn)                 __ALM_MAKE_PROTO(pfx, fn)

#define __ALM_MAKE_PROTO_SFX(pfx, fn, sfx)        pfx##_##fn##_##sfx
#define __ALM_MAKE_PROTO(pfx, fn)                 pfx##_##fn

#define ALM_PROTO_REF(fn)    		amd_ref_##fn
#define ALM_PROTO_FAST(fn)   		amd_fast##fn
#define ALM_PROTO_KERN(fn)    		alm_kern_##fn

/* 64-bit variants */
#define ALM_PROTO_BAS64(fn)	__amd_bas64_##fn
/* Generic FMA3 */
#define ALM_PROTO_FMA3(fn)	__amd_fma3_##fn
/* AVX2 versions */
#define ALM_PROTO_AVX2(fn)	__amd_avx2_##fn
/* AVX512 versions */
#define ALM_PROTO_AVX512(fn)	__amd_avx512_##fn

/* Buldozer specific, FMA3 and FMA4 */
#define ALM_PROTO_BDOZR(fn)	__amd_bdozr_##fn

/*
 * Generic arch specific name creators
 */
#define __ALM_PROTO_ARCH(a, x, y)       ALM_MAKE_PROTO_SFX(a, x, y)
#define  ALM_PROTO_ARCH(a, x, y)        __ALM_PROTO_ARCH(a, x, y)

// enable or disable exceptions in linux
#define __enable_IEEE_exceptions 1

// Prevent contraction of FMAs for precise control of FP operations
#if defined(__clang__) || defined(_MSC_VER)
#define ALM_FP_CONTRACT_OFF _Pragma("float_control(precise, on)")
#elif defined(__GNUC__)
#define ALM_FP_CONTRACT_OFF _Pragma("GCC optimize(\"fp-contract=off\")")
#else
#define ALM_FP_CONTRACT_OFF _Pragma("STDC FP_CONTRACT OFF")
#endif

// Passing a value to an empty inline assembly which uses it as both an input
// and output, or storing it in a volatile variable whose address is taken,
// prevents the compiler from optimizing away the computation of the value as a
// dead value, preserving FP exception generation. The inline asm form avoids
// allocating and storing the value on the stack.
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))

// GCC/Clang with XMM registers
#define ALM_KEEP_ALIVE_SP32(x) do { float  tX = (x); __asm__ volatile("" : "+x"(tX)); } while(0)
#define ALM_KEEP_ALIVE_DP64(x) do { double tX = (x); __asm__ volatile("" : "+x"(tX)); } while(0)

#else

// C/C++
#define ALM_KEEP_ALIVE_SP32(x) do { volatile float  tX = (x); (void)&tX; } while(0)
#define ALM_KEEP_ALIVE_DP64(x) do { volatile double tX = (x); (void)&tX; } while(0)

#endif

// Constructs to generate FP exceptions without feraiseexcept()
#define ALM_RAISE_FE_OVERFLOW() do {                    \
        static const volatile float x = 0x1p96f;        \
        ALM_KEEP_ALIVE_SP32(x * x);                     \
    } while(0)

#define ALM_RAISE_FE_UNDERFLOW() do {                   \
        static const volatile float x = 0x1p-96f;       \
        ALM_KEEP_ALIVE_SP32(x * x);                     \
    } while(0)

#define ALM_RAISE_FE_INEXACT() do {                     \
        static const volatile float x = 0x1p96f;        \
        ALM_KEEP_ALIVE_SP32(x + 1.0f);                  \
    } while(0)

#define ALM_RAISE_FE_DIVBYZERO() do {           \
        static const volatile float x = 0.0f;   \
        ALM_KEEP_ALIVE_SP32(1.0f / x);          \
    } while(0)

// The ((0x7f800000u << 16) << 16) | 0x7f800000u is a portable
// endian-independent way of generating 32-bit IEEE float
// Infinity, regardless of whether unsigned is 32- or 64-bit
#define ALM_RAISE_FE_INVALID() do {                             \
        static const volatile union {                           \
            unsigned u; float f;                                \
        } inf = { ((0x7f800000u << 16) << 16) | 0x7f800000u };  \
        ALM_KEEP_ALIVE_SP32(0.0f * inf.f);                      \
    } while(0)

#endif  /* __LIBM_MACROS_H__ */
