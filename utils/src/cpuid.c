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
 * AOCL-LibM Internal Utils - CPUID Implementation
 *
 * This module provides runtime CPU detection using CPUID instructions.
 * CPUID is queried exactly once at program startup via constructor initialization.
 *
 * This file defines the global state variables and initialization logic.
 * The API functions are static inline in arch.h to eliminate symbol exports.
 */

#include "alci/arch.h"

/* Constructor attribute for initialization functions */
#if defined(__GNUC__) || defined(__clang__)
#define ALM_UTILS_CONSTRUCTOR __attribute__((constructor))
#define ALM_INITIALIZER(f) static void f(void) ALM_UTILS_CONSTRUCTOR; static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define ALM_INITIALIZER(f) \
    static void __cdecl f(void); \
    static __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
    static void __cdecl f(void)
#else
#error "Unsupported compiler - need constructor attribute"
#endif

/* Compiler-specific CPUID intrinsics */
#if defined(_MSC_VER)
#include <intrin.h>
/*
 * MSVC __cpuid/__cpuidex take int* but we use unsigned int regs[4].
 * Use intermediate buffer to avoid undefined behavior from pointer aliasing.
 */
#define ALM_CPUID_QUERY(regs, leaf) do {                                      \
    int _tmp[4];                                                              \
    __cpuid(_tmp, (leaf));                                                    \
    (regs)[0] = (unsigned int)_tmp[0];                                        \
    (regs)[1] = (unsigned int)_tmp[1];                                        \
    (regs)[2] = (unsigned int)_tmp[2];                                        \
    (regs)[3] = (unsigned int)_tmp[3];                                        \
} while (0)
#define ALM_CPUID_QUERY_EX(regs, leaf, sub) do {                              \
    int _tmp[4];                                                              \
    __cpuidex(_tmp, (leaf), (sub));                                           \
    (regs)[0] = (unsigned int)_tmp[0];                                        \
    (regs)[1] = (unsigned int)_tmp[1];                                        \
    (regs)[2] = (unsigned int)_tmp[2];                                        \
    (regs)[3] = (unsigned int)_tmp[3];                                        \
} while (0)
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#define ALM_CPUID_QUERY(regs, leaf) \
    __cpuid((leaf), (regs)[0], (regs)[1], (regs)[2], (regs)[3])
#define ALM_CPUID_QUERY_EX(regs, leaf, sub) \
    __cpuid_count((leaf), (sub), (regs)[0], (regs)[1], (regs)[2], (regs)[3])
#else
#error "Unsupported compiler for CPUID intrinsics"
#endif


/*
 * AMD CPU Family/Model definitions for Zen generations.
 * Reference: AMD64 Architecture Programmer's Manual
 *
 * Note: Zen6 is detected via MODEL RANGES within Family 0x1A (same as Zen5),
 * not a separate family. See alm_detect_amd_uarch() for model-based logic.
 *
 * All definitions use ALM_ prefix to avoid conflicts with external aocl-utils.
 */
#define ALM_AMD_FAMILY_ZEN_ZEN2     0x17    /* Zen, Zen+, Zen2 */
#define ALM_AMD_FAMILY_ZEN3_ZEN4    0x19    /* Zen3, Zen4 */
#define ALM_AMD_FAMILY_ZEN5_ZEN6    0x1A    /* Zen5, Zen6 (model-based detection) */

/*
 * AMD Zen Model IDs
 * MAKE_MODEL(base, ext) = ((ext) << 4) | (base)
 */
/* Family 0x17 - Zen/Zen2 (Zen+ treated as Zen per aocl-utils) */
#define ALM_MODEL_NAPLES        0x01    /* Zen */
#define ALM_MODEL_RAVENRIDGE    0x11    /* Zen */
#define ALM_MODEL_PINNACLERIDGE 0x08    /* Zen+ */
#define ALM_MODEL_PICASSO       0x18    /* Zen+ (treated as Zen per aocl-utils) */
#define ALM_MODEL_ROME          0x31    /* Zen2 */
#define ALM_MODEL_CASTLEPEAKPRO 0x47    /* Zen2 */
#define ALM_MODEL_RENOIR        0x60    /* Zen2 */
#define ALM_MODEL_MATISSE       0x71    /* Zen2 */
#define ALM_MODEL_VANGOGH       0x90    /* Zen2 */
#define ALM_MODEL_MENDOCINO     0xA0    /* Zen2 */

/* Family 0x19 - Zen3/Zen4 */
#define ALM_MODEL_MILAN         0x01    /* Zen3 */
#define ALM_MODEL_CHAGALL       0x08    /* Zen3 */
#define ALM_MODEL_VERMEER       0x21    /* Zen3 */
#define ALM_MODEL_REMBRANDT     0x44    /* Zen3 */
#define ALM_MODEL_CEZANNE       0x50    /* Zen3 */
#define ALM_MODEL_GENOA         0x11    /* Zen4 */
#define ALM_MODEL_STORMPEAK     0x18    /* Zen4 */
#define ALM_MODEL_RAPHAEL       0x61    /* Zen4 */
#define ALM_MODEL_PHOENIX       0x75    /* Zen4 */
#define ALM_MODEL_PHOENIXPOINT  0x78    /* Zen4 */

/* Family 0x1A - Zen5/Zen6 models (from aocl-utils MockTest.hh)
 * Zen5 models: <= 0x4f, 0x60-0x77, 0xd0-0xd7
 * Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
 */
/* Zen5 Server */
#define ALM_MODEL_TURIN         0x02    /* Zen5 server */
#define ALM_MODEL_TURIND        0x10    /* Zen5 dense (Turin-Dense-v2) */
#define ALM_MODEL_TURIND_V1     0x11    /* Zen5 dense (Turin-Dense-v1) */
/* Zen5 Desktop */
#define ALM_MODEL_GRANITERIDGE  0x44    /* Zen5 desktop */
#define ALM_MODEL_FIRERANGE     0x48    /* Zen5 desktop/mobile */
/* Zen5 Mobile */
#define ALM_MODEL_STRIXPOINT    0x24    /* Zen5 mobile (Strix-Point-v1) */
#define ALM_MODEL_STRIXPOINT_V2 0x30    /* Zen5 mobile (Strix-Point-v2) */
#define ALM_MODEL_STRIXHALO     0x38    /* Zen5 mobile/desktop (Strix-Halo-v1) */
#define ALM_MODEL_STRIXHALO_V2  0x70    /* Zen5 mobile/desktop (Strix-Halo-v2) */
#define ALM_MODEL_KRACKANPOINT  0x60    /* Zen5 mobile */
/* Zen6 Server (model range 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7) */
#define ALM_MODEL_VENICE        0x50    /* Zen6 server (Venice-v1) */
#define ALM_MODEL_VENICEDENSE   0x51    /* Zen6 server dense (Venice-Dense-v1) */

/*
 * CPUID feature bit positions
 *   - AVX512F, AVX512DQ: used by LibM for ISA selection
 *   - CLWB: used for Zen/Zen2 fallback detection
 */
#define ALM_CPUID_LEAF7_EBX_AVX512F_BIT  16
#define ALM_CPUID_LEAF7_EBX_AVX512DQ_BIT 17
#define ALM_CPUID_LEAF7_EBX_CLWB_BIT     24

/*
 * Internal CPU info structure
 * Stores complete CPUID detection results.
 */
typedef struct {
    int             is_amd;
    alm_uarch_t     uarch;
    unsigned int    family;
    unsigned int    model;
    unsigned int    features;
} alm_cpuid_info_t;

static alm_cpuid_info_t g_cpuid = {0};

/*
 * Global CPU detection state (exported for static inline API)
 * Initialized before main() via constructor attribute.
 * After initialization, all access is read-only.
 */
alm_uarch_t   alm_g_detected_uarch    = ALM_UARCH_UNKNOWN;
unsigned int  alm_g_detected_features = ALM_FEATURE_NONE;

/*
 * Detect AMD Zen microarchitecture from family/model/features.
 * Detection is model-based to match aocl-utils behavior.
 * Structure:
 *   - switch on family (outer level)
 *   - if-else with model ranges (inner level)
 *   - feature flag fallback for unknown models
 */
static alm_uarch_t
alm_detect_amd_uarch(unsigned int family, unsigned int model, unsigned int features)
{
    switch (family) {
        case ALM_AMD_FAMILY_ZEN_ZEN2: /* Family 0x17: Zen/Zen2 (Zen+ treated as Zen) */
            if (model <= 0x1f) {
                /* Zen / Zen+ range (model <= 0x1f)
                 * Note: aocl-utils does not differentiate Zen+, all return Zen */
                return ALM_UARCH_ZEN;
            } else if (model >= 0x30) {
                /* Zen2 - Rome, Castle Peak, Renoir, Matisse, Vangogh, Mendocino */
                return ALM_UARCH_ZEN2;
            } else {
                /* Models 0x20-0x2f: use feature flag fallback
                 * CLWB is present on Zen2+, absent on Zen/Zen+ */
                if (features & ALM_FEATURE_CLWB)
                    return ALM_UARCH_ZEN2;
                return ALM_UARCH_ZEN;
            }

        case ALM_AMD_FAMILY_ZEN3_ZEN4: /* Family 0x19: Zen3, Zen4 */
            if (model <= 0x0f) {
                /* Zen3 - Milan, Chagall */
                return ALM_UARCH_ZEN3;
            } else if ((model >= 0x10 && model <= 0x1f)
                       || (model >= 0x60 && model <= 0xaf)) {
                /* Zen4 - Genoa, Stormpeak, Raphael, Phoenix, Phoenixpoint */
                return ALM_UARCH_ZEN4;
            } else if ((model == ALM_MODEL_VERMEER) || (model == ALM_MODEL_REMBRANDT)
                       || (model == ALM_MODEL_CEZANNE)) {
                /* Zen3 models outside the 0x00-0x0f range */
                return ALM_UARCH_ZEN3;
            } else {
                /* Unknown 0x19 models: use feature flag fallback
                 * AVX512F is present on Zen4, absent on Zen3 */
                if (features & ALM_FEATURE_AVX512F)
                    return ALM_UARCH_ZEN4;
                return ALM_UARCH_ZEN3;
            }

        case ALM_AMD_FAMILY_ZEN5_ZEN6: /* Family 0x1A: Zen5/Zen6 */
            /*
             * Zen5/Zen6 model-based detection per external aocl-utils:
             *   Zen5 models: <= 0x4f, 0x60-0x77, 0xd0-0xd7
             *   Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
             */
            if (model <= 0x4f || (model >= 0x60 && model <= 0x77) ||
                (model >= 0xd0 && model <= 0xd7)) {
                return ALM_UARCH_ZEN5;
            } else if ((model >= 0x50 && model <= 0x5f) ||
                       (model >= 0x80 && model <= 0xcf) ||
                       (model >= 0xd8 && model <= 0xe7)) {
                return ALM_UARCH_ZEN6;
            }
            /* Unknown 0x1A models - default to Zen5 */
            return ALM_UARCH_ZEN5;

        default:
            break;
    }

    /* Future AMD families beyond 0x1A - default to Zen6 */
    if (family > ALM_AMD_FAMILY_ZEN5_ZEN6)
        return ALM_UARCH_ZEN6;

    return ALM_UARCH_UNKNOWN;
}

/*
 * Build feature bitmask from CPUID leaf 7.
 */
static unsigned int
alm_detect_features(unsigned int leaf7_ebx)
{
    unsigned int features = ALM_FEATURE_NONE;

    if ((leaf7_ebx >> ALM_CPUID_LEAF7_EBX_AVX512F_BIT) & 1)
        features |= ALM_FEATURE_AVX512F;
    if ((leaf7_ebx >> ALM_CPUID_LEAF7_EBX_AVX512DQ_BIT) & 1)
        features |= ALM_FEATURE_AVX512DQ;
    if ((leaf7_ebx >> ALM_CPUID_LEAF7_EBX_CLWB_BIT) & 1)
        features |= ALM_FEATURE_CLWB;

    return features;
}

/*
 * Initialize the global CPU detection state.
 * Runs before main() via constructor attribute.
 */
ALM_INITIALIZER(alm_cpuid_init)
{
    unsigned int regs[4];
    unsigned int family;
    unsigned int model;
    unsigned int ext_family;
    unsigned int ext_model;
    unsigned int leaf7_ebx = 0;
    unsigned int max_leaf;
    unsigned int features;
    int is_amd = 0;

    /* CPUID leaf 0: Max leaf and Vendor ID */
    ALM_CPUID_QUERY(regs, 0);
    max_leaf = regs[0];

    /* Check for "AuthenticAMD" */
    if (regs[1] == 0x68747541 &&    /* "htuA" */
        regs[3] == 0x69746E65 &&    /* "itne" */
        regs[2] == 0x444D4163) {    /* "DMAc" */
        is_amd = 1;
    }

    /* CPUID leaf 1: Family/Model */
    ALM_CPUID_QUERY(regs, 1);

    family = (regs[0] >> 8) & 0xF;
    model = (regs[0] >> 4) & 0xF;
    ext_family = (regs[0] >> 20) & 0xFF;
    ext_model = (regs[0] >> 16) & 0xF;

    if (family == 0xF)
        family += ext_family;
    if (family >= 0xF)
        model += (ext_model << 4);

    /* CPUID leaf 7: Structured extended features (AVX512F, AVX512DQ, CLWB) */
    if (max_leaf >= 7) {
        ALM_CPUID_QUERY_EX(regs, 7, 0);
        leaf7_ebx = regs[1];
    }

    features = alm_detect_features(leaf7_ebx);

    /* Populate internal struct with all detected info */
    g_cpuid.is_amd   = is_amd;
    g_cpuid.family   = family;
    g_cpuid.model    = model;
    g_cpuid.features = features;

    if (is_amd)
        g_cpuid.uarch = alm_detect_amd_uarch(family, model, features);
    else
        g_cpuid.uarch = ALM_UARCH_UNKNOWN;

    /* Store results in exported global state (for static inline API) */
    alm_g_detected_uarch    = g_cpuid.uarch;
    alm_g_detected_features = features & (ALM_FEATURE_AVX512F | ALM_FEATURE_AVX512DQ);
}
