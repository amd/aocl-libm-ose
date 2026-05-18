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
 */

#include "alci/arch.h"
#include <string.h>

/* Compiler-specific CPUID intrinsics */
#if defined(_MSC_VER)
#include <intrin.h>
/*
 * MSVC __cpuid/__cpuidex take int* but we use unsigned int regs[4].
 * Use intermediate buffer to avoid undefined behavior from pointer aliasing.
 */
#define CPUID_QUERY(regs, leaf) do {                                          \
    int _tmp[4];                                                              \
    __cpuid(_tmp, (leaf));                                                    \
    (regs)[0] = (unsigned int)_tmp[0];                                        \
    (regs)[1] = (unsigned int)_tmp[1];                                        \
    (regs)[2] = (unsigned int)_tmp[2];                                        \
    (regs)[3] = (unsigned int)_tmp[3];                                        \
} while (0)
#define CPUID_QUERY_EX(regs, leaf, sub) do {                                  \
    int _tmp[4];                                                              \
    __cpuidex(_tmp, (leaf), (sub));                                           \
    (regs)[0] = (unsigned int)_tmp[0];                                        \
    (regs)[1] = (unsigned int)_tmp[1];                                        \
    (regs)[2] = (unsigned int)_tmp[2];                                        \
    (regs)[3] = (unsigned int)_tmp[3];                                        \
} while (0)
#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>
#define CPUID_QUERY(regs, leaf) \
    __cpuid((leaf), (regs)[0], (regs)[1], (regs)[2], (regs)[3])
#define CPUID_QUERY_EX(regs, leaf, sub) \
    __cpuid_count((leaf), (sub), (regs)[0], (regs)[1], (regs)[2], (regs)[3])
#else
#error "Unsupported compiler for CPUID intrinsics"
#endif


/*
 * AMD CPU Family/Model definitions for Zen generations.
 * Reference: AMD64 Architecture Programmer's Manual
 *
 * Note: Zen6 is detected via MODEL RANGES within Family 0x1A (same as Zen5),
 * not a separate family. See detect_amd_uarch() for model-based logic.
 */
#define AMD_FAMILY_ZEN_ZEN2     0x17    /* Zen, Zen+, Zen2 */
#define AMD_FAMILY_ZEN3_ZEN4    0x19    /* Zen3, Zen4 */
#define AMD_FAMILY_ZEN5_ZEN6    0x1A    /* Zen5, Zen6 (model-based detection) */

/*
 * AMD Zen Model IDs
 * MAKE_MODEL(base, ext) = ((ext) << 4) | (base)
 */
/* Family 0x17 - Zen/Zen+/Zen2 */
#define MODEL_NAPLES        0x01    /* Zen */
#define MODEL_RAVENRIDGE    0x11    /* Zen */
#define MODEL_PINNACLERIDGE 0x08    /* Zen+ */
#define MODEL_PICASSO       0x18    /* Zen/Zen+ (stepping dependent) */
#define MODEL_ROME          0x31    /* Zen2 */
#define MODEL_CASTLEPEAKPRO 0x47    /* Zen2 */
#define MODEL_RENOIR        0x60    /* Zen2 */
#define MODEL_MATISSE       0x71    /* Zen2 */
#define MODEL_VANGOGH       0x90    /* Zen2 */
#define MODEL_MENDOCINO     0xA0    /* Zen2 */

/* Family 0x19 - Zen3/Zen4 */
#define MODEL_MILAN         0x01    /* Zen3 */
#define MODEL_CHAGALL       0x08    /* Zen3 */
#define MODEL_VERMEER       0x21    /* Zen3 (or Zen4 Warhol if stepping==2) */
#define MODEL_REMBRANDT     0x44    /* Zen3 */
#define MODEL_CEZANNE       0x50    /* Zen3 */
#define MODEL_GENOA         0x11    /* Zen4 */
#define MODEL_STORMPEAK     0x18    /* Zen4 */
#define MODEL_RAPHAEL       0x61    /* Zen4 */
#define MODEL_PHOENIX       0x75    /* Zen4 */
#define MODEL_PHOENIXPOINT  0x78    /* Zen4 */

/* Family 0x1A - Zen5/Zen6 models (from aocl-utils MockTest.hh)
 * Zen5 models: <= 0x4f, 0x60-0x77, 0xd0-0xd7
 * Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
 */
/* Zen5 Server */
#define MODEL_TURIN         0x02    /* Zen5 server */
#define MODEL_TURIND        0x10    /* Zen5 dense (Turin-Dense-v2) */
#define MODEL_TURIND_V1     0x11    /* Zen5 dense (Turin-Dense-v1) */
/* Zen5 Desktop */
#define MODEL_GRANITERIDGE  0x44    /* Zen5 desktop */
#define MODEL_FIRERANGE     0x48    /* Zen5 desktop/mobile */
/* Zen5 Mobile */
#define MODEL_STRIXPOINT    0x24    /* Zen5 mobile (Strix-Point-v1) */
#define MODEL_STRIXPOINT_V2 0x30    /* Zen5 mobile (Strix-Point-v2) */
#define MODEL_STRIXHALO     0x38    /* Zen5 mobile/desktop (Strix-Halo-v1) */
#define MODEL_STRIXHALO_V2  0x70    /* Zen5 mobile/desktop (Strix-Halo-v2) */
#define MODEL_KRACKANPOINT  0x60    /* Zen5 mobile */
/* Zen6 Server (model range 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7) */
#define MODEL_VENICE        0x50    /* Zen6 server (Venice-v1) */
#define MODEL_VENICEDENSE   0x51    /* Zen6 server dense (Venice-Dense-v1) */


/*
 * CPUID feature bit positions
 *   - AVX512F, AVX512DQ: used by LibM for ISA selection
 *   - CLWB: used for Zen/Zen2 fallback detection
 */
#define CPUID_LEAF7_EBX_AVX512F_BIT  16
#define CPUID_LEAF7_EBX_AVX512DQ_BIT 17
#define CPUID_LEAF7_EBX_CLWB_BIT     24

/*
 * Microarchitecture enumeration (internal use)
 * Note: ZENPLUS is not in aocl-utils but we track it for completeness.
 * For API purposes, ZENPLUS maps to ZEN behavior.
 */
typedef enum {
    UARCH_UNKNOWN = 0,
    UARCH_ZEN,
    UARCH_ZENPLUS,
    UARCH_ZEN2,
    UARCH_ZEN3,
    UARCH_ZEN4,
    UARCH_ZEN5,
    UARCH_ZEN6
} uarch_t;

/*
 * Feature flags bitmask (internal use)
 *
 *   - AVX512F, AVX512DQ: checked by iface.c for ISA selection
 *   - CLWB: used for unknown model fallback detection
 */
typedef enum {
    FEATURE_NONE      = 0,
    FEATURE_AVX512F   = (1 << 0),
    FEATURE_AVX512DQ  = (1 << 1),
    FEATURE_CLWB      = (1 << 2)
} feature_t;

/*
 * Global CPU information structure
 * Initialized before main() via constructor attribute.
 * After initialization, all access is read-only.
 */
typedef struct {
    int             is_amd;
    uarch_t         uarch;
    unsigned int    family;
    unsigned int    model;
    unsigned int    stepping;
    unsigned int    features;
} cpuid_info_t;

static cpuid_info_t g_cpuid = {0};

/*
 * Detect AMD Zen microarchitecture from family/model/stepping/features.
 * Structure:
 *   - switch on family (outer level)
 *   - if-else with model ranges (inner level)
 *   - feature flag fallback for unknown models
 */
static uarch_t
detect_amd_uarch(unsigned int family, unsigned int model, unsigned int stepping,
                 unsigned int features)
{
    switch (family) {
        case AMD_FAMILY_ZEN_ZEN2: /* Family 0x17: Zen, Zen+, Zen2 */
            if (model <= 0x1f) {
                /* Zen / Zen+ range (model <= 0x1f) */
                if (model == MODEL_PINNACLERIDGE)
                    return UARCH_ZENPLUS;
                if (model == MODEL_PICASSO && stepping == 1)
                    return UARCH_ZENPLUS;
                return UARCH_ZEN;
            } else if (model >= 0x30) {
                /* Zen2 - Rome, Castle Peak, Renoir, Matisse, Vangogh, Mendocino */
                return UARCH_ZEN2;
            } else {
                /* Models 0x20-0x2f: use feature flag fallback
                 * CLWB is present on Zen2+, absent on Zen/Zen+ */
                if (features & FEATURE_CLWB)
                    return UARCH_ZEN2;
                return UARCH_ZEN;
            }

        case AMD_FAMILY_ZEN3_ZEN4: /* Family 0x19: Zen3, Zen4 */
            if (model <= 0x0f) {
                /* Zen3 - Milan, Chagall */
                return UARCH_ZEN3;
            } else if ((model >= 0x10 && model <= 0x1f)
                       || (model >= 0x60 && model <= 0xaf)) {
                /* Zen4 - Genoa, Stormpeak, Raphael, Phoenix, Phoenixpoint */
                return UARCH_ZEN4;
            } else if (model == MODEL_VERMEER) {
                /* Vermeer/Warhol: stepping == 2 -> Zen4, else -> Zen3 */
                return (stepping == 2) ? UARCH_ZEN4 : UARCH_ZEN3;
            } else if (model == MODEL_REMBRANDT || model == MODEL_CEZANNE) {
                /* Other Zen3 models outside ranges */
                return UARCH_ZEN3;
            } else {
                /* Unknown 0x19 models: use feature flag fallback
                 * AVX512F is present on Zen4, absent on Zen3 */
                if (features & FEATURE_AVX512F)
                    return UARCH_ZEN4;
                return UARCH_ZEN3;
            }

        case AMD_FAMILY_ZEN5_ZEN6: /* Family 0x1A: Zen5/Zen6 */
            /*
             * Zen5/Zen6 model-based detection per external aocl-utils:
             *   Zen5 models: <= 0x4f, 0x60-0x77, 0xd0-0xd7
             *   Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
             */
            if (model <= 0x4f || (model >= 0x60 && model <= 0x77) ||
                (model >= 0xd0 && model <= 0xd7)) {
                return UARCH_ZEN5;
            } else if ((model >= 0x50 && model <= 0x5f) ||
                       (model >= 0x80 && model <= 0xcf) ||
                       (model >= 0xd8 && model <= 0xe7)) {
                return UARCH_ZEN6;
            }
            /* Unknown 0x1A models - default to Zen5 */
            return UARCH_ZEN5;

        default:
            break;
    }

    /* Future AMD families beyond 0x1A - default to Zen6 */
    if (family > AMD_FAMILY_ZEN5_ZEN6)
        return UARCH_ZEN6;

    return UARCH_UNKNOWN;
}

/*
 * Build feature bitmask from CPUID leaf 7.
 */
static unsigned int
detect_features(unsigned int leaf7_ebx)
{
    unsigned int features = FEATURE_NONE;

    if ((leaf7_ebx >> CPUID_LEAF7_EBX_AVX512F_BIT) & 1)
        features |= FEATURE_AVX512F;
    if ((leaf7_ebx >> CPUID_LEAF7_EBX_AVX512DQ_BIT) & 1)
        features |= FEATURE_AVX512DQ;
    if ((leaf7_ebx >> CPUID_LEAF7_EBX_CLWB_BIT) & 1)
        features |= FEATURE_CLWB;

    return features;
}

/*
 * Initialize the global CPU info structure.
 * Runs before main() via constructor attribute.
 */
INITIALIZER(cpuid_init)
{
    unsigned int regs[4];
    unsigned int family;
    unsigned int model;
    unsigned int stepping;
    unsigned int ext_family;
    unsigned int ext_model;
    unsigned int leaf7_ebx = 0;
    unsigned int max_leaf;

    /* CPUID leaf 0: Max leaf and Vendor ID */
    CPUID_QUERY(regs, 0);
    max_leaf = regs[0];

    /* Check for "AuthenticAMD" */
    if (regs[1] == 0x68747541 &&    /* "htuA" */
        regs[3] == 0x69746E65 &&    /* "itne" */
        regs[2] == 0x444D4163) {    /* "DMAc" */
        g_cpuid.is_amd = 1;
    }

    /* CPUID leaf 1: Family/Model/Stepping */
    CPUID_QUERY(regs, 1);

    stepping = regs[0] & 0xF;
    family = (regs[0] >> 8) & 0xF;
    model = (regs[0] >> 4) & 0xF;
    ext_family = (regs[0] >> 20) & 0xFF;
    ext_model = (regs[0] >> 16) & 0xF;

    if (family == 0xF)
        family += ext_family;
    if (family >= 0xF)
        model += (ext_model << 4);

    g_cpuid.family = family;
    g_cpuid.model = model;
    g_cpuid.stepping = stepping;

    /* CPUID leaf 7: Structured extended features (AVX512F, AVX512DQ, CLWB) */
    if (max_leaf >= 7) {
        CPUID_QUERY_EX(regs, 7, 0);
        leaf7_ebx = regs[1];
    }

    g_cpuid.features = detect_features(leaf7_ebx);

    if (g_cpuid.is_amd)
        g_cpuid.uarch = detect_amd_uarch(family, model, stepping, g_cpuid.features);
    else
        g_cpuid.uarch = UARCH_UNKNOWN;

}

/*
 * Architecture detection functions
 *
 * These use "at-least" semantics:
 *   au_cpuid_arch_is_zen()  -> TRUE if Zen or later (Zen, Zen2, Zen3, Zen4, Zen5, Zen6, ...)
 *   au_cpuid_arch_is_zen2() -> TRUE if Zen2 or later
 *   au_cpuid_arch_is_zen3() -> TRUE if Zen3 or later
 *   au_cpuid_arch_is_zen4() -> TRUE if Zen4 or later
 *   au_cpuid_arch_is_zen5() -> TRUE if Zen5 or later
 *   au_cpuid_arch_is_zen6() -> TRUE if Zen6 or later
 *
 * All functions have hidden visibility to prevent symbol conflicts with
 * external aocl-utils (see arch.h header comment for details).
 */

ALM_UTILS_HIDDEN bool
au_cpuid_arch_is_zen(au_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return g_cpuid.uarch >= UARCH_ZEN;
}

ALM_UTILS_HIDDEN bool
au_cpuid_arch_is_zen2(au_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return g_cpuid.uarch >= UARCH_ZEN2;
}

ALM_UTILS_HIDDEN bool
au_cpuid_arch_is_zen3(au_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return g_cpuid.uarch >= UARCH_ZEN3;
}

ALM_UTILS_HIDDEN bool
au_cpuid_arch_is_zen4(au_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return g_cpuid.uarch >= UARCH_ZEN4;
}

ALM_UTILS_HIDDEN bool
au_cpuid_arch_is_zen5(au_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return g_cpuid.uarch >= UARCH_ZEN5;
}

ALM_UTILS_HIDDEN bool
au_cpuid_arch_is_zen6(au_cpu_num_t cpu_num)
{
    (void)cpu_num;
    return g_cpuid.uarch >= UARCH_ZEN6;
}

/*
 * Convert string flag name to feature bitmask.
 * Only flags actually used by LibM are supported.
 */
static unsigned int
flag_to_feature(const char* flag)
{
    if (strcmp(flag, "avx512f") == 0)
        return FEATURE_AVX512F;
    if (strcmp(flag, "avx512dq") == 0)
        return FEATURE_AVX512DQ;

    return FEATURE_NONE;
}

ALM_UTILS_HIDDEN bool
au_cpuid_has_flags(au_cpu_num_t cpu_num, const char* const flags[], int count)
{
    unsigned int required = FEATURE_NONE;
    unsigned int feature;
    int i;

    (void)cpu_num;

    if (count <= 0)
        return false;

    for (i = 0; i < count; i++) {
        feature = flag_to_feature(flags[i]);
        if (feature == FEATURE_NONE) {
            /* Unknown flag: return false to avoid false positives */
            return false;
        }
        required |= feature;
    }

    return (g_cpuid.features & required) == required;
}
