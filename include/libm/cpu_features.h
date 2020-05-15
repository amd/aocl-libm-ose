/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 *
 */
#ifndef __LIBM_CPU_FEATURES_H__
#define __LIBM_CPU_FEATURES_H__

#include <assert.h>
#include <stdint.h>
#include <libm/compiler.h>

enum {
    CPUID_EAX_1 = 0,
    CPUID_EAX_7,
    CPUID_EAX_8_01,           /* 8000.0001 */
    CPUID_EAX_8_07,           /* 8000.0007 */
    CPUID_EAX_8_08,           /* 8000.0008 */

    /* Last entry */
    CPUID_MAX,
};

/*
 * CPUID
 * EAX = 1, ECX = 0 CPUID
 */
enum {
    /* ECX Values */
    CPUID_BIT_SSE3   = (1u << 0),
    CPUID_BIT_SSSE3  = (1u << 9),
    CPUID_BIT_FMA    = (1u << 12),
    CPUID_BIT_SSE4_1 = (1u << 19),
    CPUID_BIT_SSE4_2 = (1u << 20),
    CPUID_BIT_AES    = (1u << 25),
    CPUID_BIT_AVX    = (1u << 28),

    /* EDX Values */
    CPUID_BIT_FPU  = (1u << 0),
    CPUID_BIT_MMX  = (1u << 23),
    CPUID_BIT_FXSR = (1u << 24),
    CPUID_BIT_SSE  = (1u << 25),
    CPUID_BIT_SSE2 = (1u << 26),
    CPUID_BIT_SS   = (1u << 27),
};

/*
 * CPUID
 * EAX = 0x7, ECX = 0
 */
enum {
    /* EBX values */
    CPUID_BIT_FSGSBASE        = (1u << 0),
    CPUID_BIT_TSC_ADJUST      = (1u << 1),
    CPUID_BIT_SGX             = (1u << 2),
    CPUID_BIT_BMI1            = (1u << 3),
    CPUID_BIT_HLE             = (1u << 4),
    CPUID_BIT_AVX2            = (1u << 5),
    CPUID_BIT_SMEP            = (1u << 7),
    CPUID_BIT_BMI2            = (1u << 8),
    CPUID_BIT_ERMS            = (1u << 9),
    CPUID_BIT_INVPCID         = (1u << 10),
    CPUID_BIT_RTM             = (1u << 11),
    CPUID_BIT_TSX             = CPUID_BIT_RTM,
    CPUID_BIT_PQM             = (1u << 12),
    CPUID_BIT_MPX             = (1u << 14),
    CPUID_BIT_PQE             = (1u << 15),
    CPUID_BIT_AVX512F         = (1u << 16),
    CPUID_BIT_AVX512DQ        = (1u << 17),
    CPUID_BIT_RDSEED          = (1u << 18),
    CPUID_BIT_ADX             = (1u << 19),
    CPUID_BIT_SMAP            = (1u << 20),
    CPUID_BIT_AVX512_IFMA     = (1u << 21),
    CPUID_BIT_CLFLUSHOPT      = (1u << 22),
    CPUID_BIT_CLWB            = (1u << 24),
    CPUID_BIT_TRACE           = (1u << 25),
    CPUID_BIT_AVX512PF        = (1u << 26),
    CPUID_BIT_AVX512ER        = (1u << 27),
    CPUID_BIT_AVX512CD        = (1u << 28),
    CPUID_BIT_SHA             = (1u << 29),
    CPUID_BIT_AVX512BW        = (1u << 30),
    CPUID_BIT_AVX512VL        = (1u << 31),

    /* ECX Values*/
    CPUID_BIT_PREFETCHWT1     = (1u << 0),
    CPUID_BIT_AVX512_VBMI     = (1u << 1),
    CPUID_BIT_UMIP            = (1u << 2),
    CPUID_BIT_PKU             = (1u << 3),
    CPUID_BIT_OSPKE           = (1u << 4),
    CPUID_BIT_WAITPKG         = (1u << 5),
    CPUID_BIT_AVX512_VBMI2    = (1u << 6),
    CPUID_BIT_SHSTK           = (1u << 7),
    CPUID_BIT_GFNI            = (1u << 8),
    CPUID_BIT_VAES            = (1u << 9),
    CPUID_BIT_VPCLMULQDQ      = (1u << 10),
    CPUID_BIT_AVX512_VNNI     = (1u << 11),
    CPUID_BIT_AVX512_BITALG   = (1u << 12),
    CPUID_BIT_AVX512_VPOPCNTDQ = (1u << 14),
    CPUID_BIT_RDPID           = (1u << 22),
    CPUID_BIT_CLDEMOTE        = (1u << 25),
    CPUID_BIT_MOVDIRI         = (1u << 27),
    CPUID_BIT_MOVDIR64B       = (1u << 28),
    CPUID_BIT_SGX_LC          = (1u << 30),

    /* EDX Values */
    CPUID_BIT_AVX512_4VNNIW   = (1u << 2),
    CPUID_BIT_AVX512_4FMAPS   = (1u << 3),
    CPUID_BIT_FSRM            = (1u << 4),
    CPUID_BIT_PCONFIG         = (1u << 18),
    CPUID_BIT_IBT             = (1u << 20),
    CPUID_BIT_IBRS_IBPB       = (1u << 26),
    CPUID_BIT_STIBP           = (1u << 27),
    CPUID_BIT_CAPABILITIES    = (1u << 29),
    CPUID_BIT_SSBD            = (1u << 31),
};

#define CPU_FEATURE_REG(ftr, idx, reg) ({       \
            uint32_t val;                       \
            struct cpuid_regs *r;               \
            r = &ftr->available[0];             \
            val = r[(idx)].reg;                 \
            val;                                \
        })

#define CPU_FEATURE(ptr, idx, reg, bit) ({      \
            uint32_t __reg =                    \
                CPU_FEATURE_REG(ptr, idx, reg); \
            (__reg & bit);                      \
        })

#define CPU_HAS_SSSE3(f) CPU_FEATURE(f, CPUID_EAX_1, ecx, CPUID_BIT_SSSE3)
#define CPU_HAS_AVX(f)   CPU_FEATURE(f, CPUID_EAX_1, ecx, CPUID_BIT_AVX)
#define CPU_HAS_AVX2(f)  CPU_FEATURE(f, CPUID_EAX_7, ebx, CPUID_BIT_AVX2)
#define CPU_HAS_TSX(f)   CPU_FEATURE(f, CPUID_EAX_7, ebx, CPUID_BIT_TSX)

#define CPU_FEATURE_USABLE_REG(ftr, idx, reg) ({        \
            uint32_t val;                               \
            struct cpuid_regs *r;                       \
            r = &(ftr)->usable[0];			\
            val = r[(idx)].reg;                         \
            val;                                        \
        })

#define CPU_FEATURE_USABLE(ptr, idx, reg, bit) ({       \
            uint32_t __reg =                            \
                CPU_FEATURE_USABLE_REG(ptr, idx, reg);  \
            (__reg &  bit);                             \
        })

#define CPU_FEATURE_AVX_USABLE(f)                               \
    CPU_FEATURE_USABLE(f, CPUID_EAX_1, eax, CPUID_BIT_AVX)
#define CPU_FEATURE_SSSE3_USABLE(f)                             \
    CPU_FEATURE_USABLE(f, CPUID_EAX_1, ecx, CPUID_BIT_SSSE3)
#define CPU_FEATURE_AVX2_USABLE(f)                              \
    CPU_FEATURE_USABLE(f, CPUID_EAX_7, ebx, CPUID_BIT_AVX2)

/* ID return values */
struct cpuid_regs {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

enum cpu_mfg_type {
    CPU_MFG_INTEL,
    CPU_MFG_AMD,
    CPU_MFG_OTHER,
};

struct cpu_mfg_info {
    enum cpu_mfg_type mfg_type;
    uint16_t     family;
    uint16_t     model;
    uint16_t     stepping;
};

struct cpu_features {
    struct cpu_mfg_info cpu_mfg_info;
    struct cpuid_regs available[CPUID_MAX];
    struct cpuid_regs usable[CPUID_MAX];
};

static inline void __cpuid(struct cpuid_regs *out)
{
    asm volatile
        (
         "cpuid"
         :"=a"(out->eax), "=b"(out->ebx), "=c"(out->ecx), "=d"(out->edx)
         );
}

static inline void __cpuid_1(uint32_t eax, struct cpuid_regs *out)
{
    asm volatile
        (
         "cpuid"
         :"=a"(out->eax), "=b"(out->ebx), "=c"(out->ecx), "=d"(out->edx)
         :"0"(eax)
         );
}

static inline void __cpuid_2(uint32_t eax, uint32_t ecx, struct cpuid_regs *out)
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

struct cpu_features *libm_cpu_get_features(void);
void libm_cpu_set_features(struct cpu_features *f,
                           uint32_t reg_offset,
                           uint32_t bit);

int libm_cpu_feature_is_avx_usable(void);
int libm_cpu_feature_is_avx2_usable(void);

#endif  /* __LIBM_CPU_FEATURES_H__ */
