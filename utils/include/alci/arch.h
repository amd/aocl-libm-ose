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

/*
 * AOCL-LibM Internal Utils - CPU Architecture Detection
 *
 * This header provides API-compatible replacements for aocl-utils (libau_cpuid).
 * It uses the same function names and signatures to allow seamless switching
 * between internal and external utils.
 *
 * Symbol Visibility:
 *   These functions have HIDDEN visibility to prevent symbol conflicts with
 *   external aocl-utils in BIY (Build-It-Yourself) scenarios. When libm is
 *   linked as a shared library (libalm.so), these symbols are not exported
 *   and will not conflict with any aocl-utils symbols.
 *
 *   Note: Hidden visibility only affects shared library exports. For static
 *   linking (libalm.a), symbol conflicts may still occur if the application
 *   also links external aocl-utils. See utils/README.md for details.
 *
 * Design:
 *   - CPUID is queried exactly once at program startup via constructor initialization
 *   - All CPU information is cached in a global structure
 *   - Feature flags use efficient bitmask operations internally
 */

#ifndef ALCI_ARCH_H
#define ALCI_ARCH_H

#include <stdbool.h>
#include <stdint.h>

/* Visibility attribute for internal symbols (Linux/ELF only, not Windows) */
#if (defined(__GNUC__) || defined(__clang__)) && !defined(_WIN32)
#define ALM_UTILS_HIDDEN __attribute__((__visibility__("hidden")))
#else
#define ALM_UTILS_HIDDEN
#endif

/* Constructor attribute for initialization functions */
#if defined(__GNUC__) || defined(__clang__)
#define ALM_UTILS_CONSTRUCTOR __attribute__((constructor))
#define INITIALIZER(f) static void f(void) ALM_UTILS_CONSTRUCTOR; static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define INITIALIZER(f) \
    static void __cdecl f(void); \
    __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
    static void __cdecl f(void)
#else
#error "Unsupported compiler - need constructor attribute"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type definitions matching aocl-utils API
 */
#ifndef AU_CPU_NUM_T_DEFINED
#define AU_CPU_NUM_T_DEFINED
typedef uint32_t au_cpu_num_t;
#endif

/*
 * CPU number constant for current CPU.
 * The cpu parameter is reserved for future use (e.g., per-core detection).
 * Currently ignored; detection always runs on the current CPU.
 */
#define AU_CURRENT_CPU_NUM  0

/*
 * Architecture detection functions.
 * Returns non-zero if the current CPU is the specified Zen generation OR LATER.
 * This matches aocl-utils "at-least" semantics.
 *
 * Detection is based on CPUID family/model/stepping:
 *   Family 0x17: Zen, Zen+, Zen2
 *   Family 0x19: Zen3, Zen4
 *   Family 0x1A: Zen5 or Zen6 (model-based detection)
 *
 * Unknown model handling:
 *   Family 0x17, model <= 0x1f: Zen
 *   Family 0x17, model 0x20-0x2f: CLWB flag ? Zen2 : Zen (feature fallback)
 *   Family 0x17, model >= 0x30: Zen2
 *   Family 0x19, model <= 0x0f: Zen3
 *   Family 0x19, model 0x10-0x1f or 0x60-0xaf: Zen4
 *   Family 0x19, other models: AVX512F flag ? Zen4 : Zen3 (feature fallback)
 *   Family 0x1A, Zen5 models: <= 0x4f, 0x60-0x77, 0xd0-0xd7
 *   Family 0x1A, Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
 *   Family 0x1A, unknown models: Zen5 (default)
 *   Family > 0x1A: Zen6 (forward compatibility)
 *   Other families: Unknown
 *
 * Examples:
 *   On Zen6: au_cpuid_arch_is_zen() -> TRUE, au_cpuid_arch_is_zen6() -> TRUE
 *   On Zen5: au_cpuid_arch_is_zen5() -> TRUE, au_cpuid_arch_is_zen6() -> FALSE
 *   On Zen4: au_cpuid_arch_is_zen4() -> TRUE, au_cpuid_arch_is_zen5() -> FALSE
 *
 * @param cpu_num  CPU number (currently ignored, use AU_CURRENT_CPU_NUM)
 * @return         true if CPU is at least the specified architecture, false otherwise
 */
ALM_UTILS_HIDDEN bool au_cpuid_arch_is_zen(au_cpu_num_t cpu_num);
ALM_UTILS_HIDDEN bool au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num);
ALM_UTILS_HIDDEN bool au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num);
ALM_UTILS_HIDDEN bool au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num);
ALM_UTILS_HIDDEN bool au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num);
ALM_UTILS_HIDDEN bool au_cpuid_arch_is_zen6(au_cpu_num_t cpu_num);

/*
 * Feature flag detection.
 * Check if the CPU supports all specified feature flags.
 *
 * @param cpu_num  CPU number (currently ignored, use AU_CURRENT_CPU_NUM)
 * @param flags    Array of flag name strings (e.g., "avx512f", "avx512dq")
 * @param count    Number of flags in the array
 * @return         true if ALL specified flags are supported, false otherwise
 *
 * Supported flag names (only flags used by LibM):
 *   "avx512f"   - AVX-512 Foundation
 *   "avx512dq"  - AVX-512 Doubleword and Quadword Instructions
 */
ALM_UTILS_HIDDEN bool au_cpuid_has_flags(au_cpu_num_t cpu_num, const char* const flags[], int count);

#ifdef __cplusplus
}
#endif

#endif  /* ALCI_ARCH_H */
