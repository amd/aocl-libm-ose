/*
 * Copyright (C) 2018, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <libm/cpu_features.h>
#include <libm/compiler.h>

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

#define CONSTRUCTOR __attribute__((constructor))
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/ sizeof(x[0]))
#endif

static void CONSTRUCTOR
__init_cpu_features(void)
{
    struct cpu_mfg_info *mfg_info = &cpu_features.cpu_mfg_info;
    int arr_size = ARRAY_SIZE(__cpuid_values);
    assert(arr_size <= CPUID_MAX);

    struct cpuid_regs regs;
    __cpuid_1(0, &regs);

    /* GenuineAMD */
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
        switch(mfg_info->family) {
        case 0x15:                      /* Naples */
            break;
        case 0x17:                      /* Rome */
            break;
        case 0x19:                      /* Milan */
            break;
        }
    }
}

struct cpu_features *
__get_cpu_features(void)
{
    return &cpu_features;
}
