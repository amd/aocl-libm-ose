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
 * This header provides CPU detection functionality for AOCL-LibM.
 * All functions are static inline, eliminating exported symbols entirely.
 *
 * Symbol Conflict Prevention:
 *   - All types/enums use ALM_ prefix to avoid conflicts with external aocl-utils
 *   - Static inline functions are not exported - no symbol table entries
 *   - Each translation unit gets its own copy of the inline functions
 *   - Global state is defined once in alm_cpuid.c, accessed via extern
 *
 * Design:
 *   - CPUID is queried exactly once at program startup via an explicit
 *     alm_cpuid_init() call (see below), not an auto-run constructor
 *   - All CPU information is cached in global structures (defined in alm_cpuid.c)
 *   - Feature flags use efficient bitmask operations internally
 */

#ifndef ALM_ARCH_H
#define ALM_ARCH_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type definitions for libm internal utils
 */
typedef uint32_t alm_cpu_num_t;

/*
 * CPU number constant for current CPU.
 * The cpu parameter is reserved for future use (e.g., per-core detection).
 * Currently ignored; detection always runs on the current CPU.
 */
#define ALM_CURRENT_CPU_NUM  0

/*
 * Microarchitecture enumeration
 * Used for "at-least" comparisons in architecture detection.
 */
typedef enum {
    ALM_UARCH_UNKNOWN = 0,
    ALM_UARCH_ZEN,
    ALM_UARCH_ZENPLUS,
    ALM_UARCH_ZEN2,
    ALM_UARCH_ZEN3,
    ALM_UARCH_ZEN4,
    ALM_UARCH_ZEN5,
    ALM_UARCH_ZEN6
} alm_uarch_t;

/*
 * Feature flags bitmask
 *   - AVX512F, AVX512DQ: used by LibM for ISA selection
 *   - CLWB: used for Zen/Zen2 fallback detection (internal)
 */
typedef enum {
    ALM_FEATURE_NONE      = 0,
    ALM_FEATURE_AVX512F   = (1 << 0),
    ALM_FEATURE_AVX512DQ  = (1 << 1),
    ALM_FEATURE_CLWB      = (1 << 2)
} alm_feature_t;

/*
 * Global CPU detection state (defined in alm_cpuid.c, populated by
 * alm_cpuid_init()). These are the only data symbols exported from the utils
 * object file.
 */
extern alm_uarch_t   alm_g_detected_uarch;
extern unsigned int  alm_g_detected_features;

/*
 * Run CPU detection and populate the global detection state above.
 *
 * This is a normal function, not an auto-run constructor: the caller must
 * invoke it once before using the detection API below. libm calls it from its
 * dispatch fixup constructor (src/entry_pt.c) so detection is guaranteed to
 * run before dispatch selection, independent of static-constructor link order.
 * Callers that link libalm statically and only use the detection API (without
 * pulling in the libm entry points) must call it themselves. Calling it more
 * than once is safe.
 */
void alm_cpuid_init(void);

/*
 * Architecture detection functions - static inline.
 * Returns non-zero if the current CPU is the specified Zen generation OR LATER.
 * Uses "at-least" semantics.
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
 *   On Zen6: alm_cpuid_arch_is_zen() -> TRUE, alm_cpuid_arch_is_zen6() -> TRUE
 *   On Zen5: alm_cpuid_arch_is_zen5() -> TRUE, alm_cpuid_arch_is_zen6() -> FALSE
 *   On Zen4: alm_cpuid_arch_is_zen4() -> TRUE, alm_cpuid_arch_is_zen5() -> FALSE
 *
 * @param cpu_num  CPU number (currently ignored, use ALM_CURRENT_CPU_NUM)
 * @return         true if CPU is at least the specified architecture, false otherwise
 */
static inline bool alm_cpuid_arch_is_zen(alm_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return alm_g_detected_uarch >= ALM_UARCH_ZEN;
}

static inline bool alm_cpuid_arch_is_zen2(alm_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return alm_g_detected_uarch >= ALM_UARCH_ZEN2;
}

static inline bool alm_cpuid_arch_is_zen3(alm_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return alm_g_detected_uarch >= ALM_UARCH_ZEN3;
}

static inline bool alm_cpuid_arch_is_zen4(alm_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return alm_g_detected_uarch >= ALM_UARCH_ZEN4;
}

static inline bool alm_cpuid_arch_is_zen5(alm_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return alm_g_detected_uarch >= ALM_UARCH_ZEN5;
}

static inline bool alm_cpuid_arch_is_zen6(alm_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return alm_g_detected_uarch >= ALM_UARCH_ZEN6;
}

/*
 * Feature flag detection - static inline.
 * Check if the CPU supports all specified feature flags.
 *
 * @param cpu_num  CPU number (currently ignored, use ALM_CURRENT_CPU_NUM)
 * @param flags    Array of flag name strings (e.g., "avx512f", "avx512dq")
 * @param count    Number of flags in the array
 * @return         true if ALL specified flags are supported, false otherwise
 *
 * Supported flag names (only flags used by LibM):
 *   "avx512f"   - AVX-512 Foundation
 *   "avx512dq"  - AVX-512 Doubleword and Quadword Instructions
 */
static inline bool alm_cpuid_has_flags(alm_cpu_num_t cpu_num, 
                                       const char* const flags[], int count)
{
    unsigned int required = ALM_FEATURE_NONE;
    int i;

    (void)cpu_num;

    if (count <= 0)
        return false;

    for (i = 0; i < count; i++) {
        if (strcmp(flags[i], "avx512f") == 0)
            required |= ALM_FEATURE_AVX512F;
        else if (strcmp(flags[i], "avx512dq") == 0)
            required |= ALM_FEATURE_AVX512DQ;
        else
            return false;  /* Unknown flag */
    }

    return (alm_g_detected_features & required) == required;
}

#ifdef __cplusplus
}
#endif

#endif  /* ALM_ARCH_H */
