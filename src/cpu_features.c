/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>                     /* for memcpy */

#include <libm/cpu_features.h>
#include <libm/compiler.h>
#include <libm/iface.h>

struct cpu_features cpu_features HIDDEN;

struct {
    uint32_t eax;
    uint32_t ecx;
} __cpuid_values[CPUID_MAX] = {
    [CPUID_EAX_1] = {0x1, 0x0},                         /* eax = 0, ecx=0 */
    [CPUID_EAX_7] = {0x7, 0x0},                         /* eax = 7,  -"- */
    [CPUID_EAX_8_01] = {0x80000001, 0x0},               /* eax = 0x80000001 */
    [CPUID_EAX_8_07] = {0x80000007, 0x0},               /* eax = 0x80000007 */
    [CPUID_EAX_8_08] = {0x80000008, 0x0},               /* eax = 0x80000008 */
};

static void
__get_mfg_info(struct cpuid_regs *regs, struct cpu_mfg_info *mfg_info)
{
    uint32_t ext_model;

    if (mfg_info) {
        struct cpuid_regs regs;
        uint32_t eax;

        __cpuid_1(1, &regs);
        eax = regs.eax;

        mfg_info->family = (eax >> 8) & 0x0f;
        mfg_info->model = (eax >> 4) & 0x0f;
        ext_model = (eax >> 12) & 0xf0;
        if (mfg_info->family == 0x0f) {
            mfg_info->family += (eax >> 20) & 0xff;
            mfg_info->model += ext_model;
        }

        mfg_info->stepping = eax & 0x0f;
    }
}

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/ sizeof(x[0]))
#endif

#define INITIALIZED_MAGIC 0xdeadbeaf

static void
__init_cpu_features(void)
{
    static unsigned initialized = 0;

    struct cpu_mfg_info *mfg_info = &cpu_features.cpu_mfg_info;
    int arr_size = ARRAY_SIZE(__cpuid_values);
    //assert(arr_size <= CPUID_MAX);

    if (initialized == INITIALIZED_MAGIC)
        return;

    struct cpuid_regs regs;
    __cpuid_1(0, &regs);

    /* "AuthenticAMD" */
    if (regs.ebx == 0x68747541 && regs.ecx == 0x444d4163 &&
        regs.edx == 0x69746e65) {
        cpu_features.cpu_mfg_info.mfg_type = CPU_MFG_AMD;
    }

    for (int i = 0; i < arr_size; i++) {
        struct cpuid_regs ft;

        __cpuid_2(__cpuid_values[i].eax, __cpuid_values[i].ecx, &ft);

        cpu_features.available[i].eax = ft.eax;
        cpu_features.available[i].ebx = ft.ebx;
        cpu_features.available[i].ecx = ft.ecx;
        cpu_features.available[i].edx = ft.edx;
    }

    __get_mfg_info(&cpu_features.available[CPUID_EAX_1], mfg_info);

    /*
     * Globally disable some *_USEABLE flags, so that all ifunc's
     * sees them
     */
    if (mfg_info->mfg_type == CPU_MFG_AMD) {
        memcpy(&cpu_features.usable[0], &cpu_features.available[0],
               sizeof(cpu_features.usable));

        switch(mfg_info->family) {
        case 0x15:                      /* Naples */
            break;
        case 0x17:                      /* Rome */
            break;
        case 0x19:                      /* Milan */
            break;
        }
    }

    initialized = INITIALIZED_MAGIC;
}

static void CONSTRUCTOR
libm_init_cpu(void)
{
    __init_cpu_features();
}

struct cpu_features *
libm_cpu_get_features(void)
{
    __init_cpu_features();

    return &cpu_features;
}

int
libm_cpu_feature_is_avx_usable(void)
{
    __init_cpu_features();

    return CPU_FEATURE_AVX_USABLE(&cpu_features);
}

int
libm_cpu_feature_is_avx2_usable(void)
{
    __init_cpu_features();

    return CPU_FEATURE_AVX2_USABLE(&cpu_features);
}
