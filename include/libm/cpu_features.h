/*
 * Copyright (C) 2018-2020, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __LIBM_CPU_FEATURES_H__
#define __LIBM_CPU_FEATURES_H__

#include <assert.h>
#include <stdint.h>
#include <libm/compiler.h>

enum {
    ALM_CPUID_EAX_1 = 0,
    ALM_CPUID_EAX_7,
    ALM_CPUID_EAX_8_01,           /* 8000.0001 */
    ALM_CPUID_EAX_8_07,           /* 8000.0007 */
    ALM_CPUID_EAX_8_08,           /* 8000.0008 */

    /* Last entry */
    ALM_CPUID_MAX,
};

/*
 * ALM_CPUID
 * EAX = 1, ECX = 0 ALM_CPUID
 */
enum {
    /* ECX Values */
    ALM_CPUID_BIT_SSE3   = (1u << 0),
    ALM_CPUID_BIT_SSSE3  = (1u << 9),
    ALM_CPUID_BIT_FMA    = (1u << 12),
    ALM_CPUID_BIT_SSE4_1 = (1u << 19),
    ALM_CPUID_BIT_SSE4_2 = (1u << 20),
    ALM_CPUID_BIT_AES    = (1u << 25),
    ALM_CPUID_BIT_AVX    = (1u << 28),

    /* EDX Values */
    ALM_CPUID_BIT_FPU  = (1u << 0),
    ALM_CPUID_BIT_MMX  = (1u << 23),
    ALM_CPUID_BIT_FXSR = (1u << 24),
    ALM_CPUID_BIT_SSE  = (1u << 25),
    ALM_CPUID_BIT_SSE2 = (1u << 26),
    ALM_CPUID_BIT_SS   = (1u << 27),
};

/*
 * ALM_CPUID
 * EAX = 0x7, ECX = 0
 */
enum {
    /* EBX values */
    ALM_CPUID_BIT_FSGSBASE        = (1u << 0),
    ALM_CPUID_BIT_TSC_ADJUST      = (1u << 1),
    ALM_CPUID_BIT_SGX             = (1u << 2),
    ALM_CPUID_BIT_BMI1            = (1u << 3),
    ALM_CPUID_BIT_HLE             = (1u << 4),
    ALM_CPUID_BIT_AVX2            = (1u << 5),
    ALM_CPUID_BIT_SMEP            = (1u << 7),
    ALM_CPUID_BIT_BMI2            = (1u << 8),
    ALM_CPUID_BIT_ERMS            = (1u << 9),
    ALM_CPUID_BIT_INVPCID         = (1u << 10),
    ALM_CPUID_BIT_RTM             = (1u << 11),
    ALM_CPUID_BIT_TSX             = ALM_CPUID_BIT_RTM,
    ALM_CPUID_BIT_PQM             = (1u << 12),
    ALM_CPUID_BIT_MPX             = (1u << 14),
    ALM_CPUID_BIT_PQE             = (1u << 15),
    ALM_CPUID_BIT_AVX512F         = (1u << 16),
    ALM_CPUID_BIT_AVX512DQ        = (1u << 17),
    ALM_CPUID_BIT_RDSEED          = (1u << 18),
    ALM_CPUID_BIT_ADX             = (1u << 19),
    ALM_CPUID_BIT_SMAP            = (1u << 20),
    ALM_CPUID_BIT_AVX512_IFMA     = (1u << 21),
    ALM_CPUID_BIT_CLFLUSHOPT      = (1u << 22),
    ALM_CPUID_BIT_CLWB            = (1u << 24),
    ALM_CPUID_BIT_TRACE           = (1u << 25),
    ALM_CPUID_BIT_AVX512PF        = (1u << 26),
    ALM_CPUID_BIT_AVX512ER        = (1u << 27),
    ALM_CPUID_BIT_AVX512CD        = (1u << 28),
    ALM_CPUID_BIT_SHA             = (1u << 29),
    ALM_CPUID_BIT_AVX512BW        = (1u << 30),
    ALM_CPUID_BIT_AVX512VL        = (1u << 31),

    /* ECX Values*/
    ALM_CPUID_BIT_PREFETCHWT1     = (1u << 0),
    ALM_CPUID_BIT_AVX512_VBMI     = (1u << 1),
    ALM_CPUID_BIT_UMIP            = (1u << 2),
    ALM_CPUID_BIT_PKU             = (1u << 3),
    ALM_CPUID_BIT_OSPKE           = (1u << 4),
    ALM_CPUID_BIT_WAITPKG         = (1u << 5),
    ALM_CPUID_BIT_AVX512_VBMI2    = (1u << 6),
    ALM_CPUID_BIT_SHSTK           = (1u << 7),
    ALM_CPUID_BIT_GFNI            = (1u << 8),
    ALM_CPUID_BIT_VAES            = (1u << 9),
    ALM_CPUID_BIT_VPCLMULQDQ      = (1u << 10),
    ALM_CPUID_BIT_AVX512_VNNI     = (1u << 11),
    ALM_CPUID_BIT_AVX512_BITALG   = (1u << 12),
    ALM_CPUID_BIT_AVX512_VPOPCNTDQ = (1u << 14),
    ALM_CPUID_BIT_RDPID           = (1u << 22),
    ALM_CPUID_BIT_CLDEMOTE        = (1u << 25),
    ALM_CPUID_BIT_MOVDIRI         = (1u << 27),
    ALM_CPUID_BIT_MOVDIR64B       = (1u << 28),
    ALM_CPUID_BIT_SGX_LC          = (1u << 30),

    /* EDX Values */
    ALM_CPUID_BIT_AVX512_4VNNIW   = (1u << 2),
    ALM_CPUID_BIT_AVX512_4FMAPS   = (1u << 3),
    ALM_CPUID_BIT_FSRM            = (1u << 4),
    ALM_CPUID_BIT_PCONFIG         = (1u << 18),
    ALM_CPUID_BIT_IBT             = (1u << 20),
    ALM_CPUID_BIT_IBRS_IBPB       = (1u << 26),
    ALM_CPUID_BIT_STIBP           = (1u << 27),
    ALM_CPUID_BIT_CAPABILITIES    = (1u << 29),
    ALM_CPUID_BIT_SSBD            = (1u << 31),
};

#define ALM_CPU_FEATURE_REG(ftr, idx, reg) ({   \
            uint32_t val;                       \
            struct cpuid_regs *r;               \
            r = &ftr->available[0];             \
            val = r[(idx)].reg;                 \
            val;                                \
        })

#define ALM_CPU_FEATURE(ptr, idx, reg, bit) ({      \
            uint32_t __reg =                        \
                ALM_CPU_FEATURE_REG(ptr, idx, reg); \
            (__reg & bit);                          \
        })

#define ALM_CPU_HAS_SSSE3(f) ALM_CPU_FEATURE(f, ALM_CPUID_EAX_1, ecx, ALM_CPUID_BIT_SSSE3)
#define ALM_CPU_HAS_AVX(f)   ALM_CPU_FEATURE(f, ALM_CPUID_EAX_1, ecx, ALM_CPUID_BIT_AVX)
#define ALM_CPU_HAS_AVX2(f)  ALM_CPU_FEATURE(f, ALM_CPUID_EAX_7, ebx, ALM_CPUID_BIT_AVX2)
#define ALM_CPU_HAS_TSX(f)   ALM_CPU_FEATURE(f, ALM_CPUID_EAX_7, ebx, ALM_CPUID_BIT_TSX)

#define ALM_CPU_FEATURE_USABLE_REG(ftr, idx, reg) ({        \
            uint32_t val;                                   \
            struct alm_cpuid_regs *r;                       \
            r = &(ftr)->usable[0];                          \
            val = r[(idx)].reg;                             \
            val;                                            \
        })

#define ALM_CPU_FEATURE_USABLE(ptr, idx, reg, bit) ({       \
            uint32_t __reg =                                \
                ALM_CPU_FEATURE_USABLE_REG(ptr, idx, reg);  \
            (__reg &  bit);                                 \
        })

#define ALM_CPU_FEATURE_AVX_USABLE(f)                                   \
    ALM_CPU_FEATURE_USABLE(f, ALM_CPUID_EAX_1, eax, ALM_CPUID_BIT_AVX)
#define ALM_CPU_FEATURE_SSSE3_USABLE(f)                                 \
    ALM_CPU_FEATURE_USABLE(f, ALM_CPUID_EAX_1, ecx, ALM_CPUID_BIT_SSSE3)
#define ALM_CPU_FEATURE_AVX2_USABLE(f)                                  \
    ALM_CPU_FEATURE_USABLE(f, ALM_CPUID_EAX_7, ebx, ALM_CPUID_BIT_AVX2)


#define ALM_CPU_FAMILY_NAPLES      0x15
#define ALM_CPU_FAMILY_ROME        0x17
#define ALM_CPU_FAMILY_MILAN       0x19


/* ID return values */
struct alm_cpuid_regs {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

typedef enum {
    ALM_CPU_MFG_INTEL,
    ALM_CPU_MFG_AMD,
    ALM_CPU_MFG_OTHER,
} alm_cpu_mfg_t;

struct alm_cpu_mfg_info {
    alm_cpu_mfg_t     mfg_type;
    uint16_t          family;
    uint16_t          model;
    uint16_t          stepping;
};

struct alm_cpu_features {
    struct alm_cpu_mfg_info cpu_mfg_info;
    struct alm_cpuid_regs   available[ALM_CPUID_MAX];
    struct alm_cpuid_regs   usable[ALM_CPUID_MAX];
};

static inline void __cpuid(struct alm_cpuid_regs *out)
{
    asm volatile
        (
         "cpuid"
         :"=a"(out->eax), "=b"(out->ebx), "=c"(out->ecx), "=d"(out->edx)
         );
}

static inline void __cpuid_1(uint32_t eax, struct alm_cpuid_regs *out)
{
    asm volatile
        (
         "cpuid"
         :"=a"(out->eax), "=b"(out->ebx), "=c"(out->ecx), "=d"(out->edx)
         :"0"(eax)
         );
}

static inline void __cpuid_2(uint32_t eax, uint32_t ecx, struct alm_cpuid_regs *out)
{
    asm volatile
        (
         "cpuid"
         :"=a"(out->eax), "=b"(out->ebx), "=c"(out->ecx), "=d"(out->edx)
         :"0"(eax), "2"(ecx)
         );
}

static inline uint32_t
__extract32(uint32_t value, int start, int length)
{
    assert(start >= 0 && length > 0 && length <= 32 - start);
    return (value >> start) & (~0U >> (32 - length));
}

static inline uint32_t
__cpuid_bit(uint32_t value, int bit)
{
    return __extract32(value, bit, 1);
}

struct alm_cpu_features *alm_cpu_get_features(void);
void alm_cpu_set_features(struct alm_cpu_features *f,
                           uint32_t reg_offset,
                           uint32_t bit);

int alm_cpu_feature_is_avx_usable(void);
int alm_cpu_feature_is_avx2_usable(void);

#endif  /* __LIBM_CPU_FEATURES_H__ */
