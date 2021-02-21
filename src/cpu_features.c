/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>                     /* for memcpy */

#include <libm/cpu_features.h>
#include <libm/compiler.h>
#include <libm/iface.h>

struct alm_cpu_features cpu_features HIDDEN;

struct {
    uint32_t eax;
    uint32_t ecx;
} __cpuid_values[ALM_CPUID_MAX] = {
    [ALM_CPUID_EAX_1] = {0x1, 0x0},                         /* eax = 0, ecx=0 */
    [ALM_CPUID_EAX_7] = {0x7, 0x0},                         /* eax = 7,  -"- */
    [ALM_CPUID_EAX_8_01] = {0x80000001, 0x0},               /* eax = 0x80000001 */
    [ALM_CPUID_EAX_8_07] = {0x80000007, 0x0},               /* eax = 0x80000007 */
    [ALM_CPUID_EAX_8_08] = {0x80000008, 0x0},               /* eax = 0x80000008 */
};

static void
__get_mfg_info(struct alm_cpuid_regs *cpuid_regs, struct alm_cpu_mfg_info *mfg_info)
{
    uint32_t ext_model;
    uint32_t model;
    uint32_t family;

    if (mfg_info) {
        struct alm_cpuid_regs regs;
        uint32_t eax;

        __cpuid_1(1, &regs);
        eax = regs.eax;

        family = (eax >> 8) & 0x0f;
        model = (eax >> 4) & 0x0f;
        ext_model = (eax >> 12) & 0xf0;
        if (family == 0x0f) {
            family += (eax >> 20) & 0xff;
            mfg_info->family = (uint16_t)family;
            model += ext_model;
            mfg_info->model = (uint16_t)model;
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

    struct alm_cpu_mfg_info *mfg_info = &cpu_features.cpu_mfg_info;
    int arr_size = ARRAY_SIZE(__cpuid_values);
    //assert(arr_size <= ALM_CPUID_MAX);

    if (initialized == INITIALIZED_MAGIC)
        return;

    struct alm_cpuid_regs regs;
    __cpuid_1(0, &regs);

    /* "AuthenticAMD" */
    if (regs.ebx == 0x68747541 && regs.ecx == 0x444d4163 &&
        regs.edx == 0x69746e65) {
        cpu_features.cpu_mfg_info.mfg_type = ALM_CPU_MFG_AMD;
    }

    for (int i = 0; i < arr_size; i++) {
        struct alm_cpuid_regs ft;

        __cpuid_2(__cpuid_values[i].eax, __cpuid_values[i].ecx, &ft);

        cpu_features.available[i].eax = ft.eax;
        cpu_features.available[i].ebx = ft.ebx;
        cpu_features.available[i].ecx = ft.ecx;
        cpu_features.available[i].edx = ft.edx;
    }

    __get_mfg_info(&cpu_features.available[ALM_CPUID_EAX_1], mfg_info);

    /*
     * Globally disable some *_USEABLE flags, so that all ifunc's
     * sees them
     */
    if (mfg_info->mfg_type == ALM_CPU_MFG_AMD) {
        memcpy(&cpu_features.usable[0], &cpu_features.available[0],
               sizeof(cpu_features.usable));

        switch(mfg_info->family) {
        case ALM_CPU_FAMILY_NAPLES:     /* Naples */
            break;
        case ALM_CPU_FAMILY_ROME:       /* Rome */
            break;
        case ALM_CPU_FAMILY_MILAN:      /* Milan */
            break;
        default:
            break;
        }
    }

    initialized = INITIALIZED_MAGIC;

#if defined(DEVELOPER)
		/* We override any from  */
		const char *env_cpuid_str = ALM_ENV_ALM_CPUID;
		alm_env_get(env_cpuid_str, sizeof(env_cpuid_str), mfg_info);

#endif
}

static void CONSTRUCTOR
alm_init_cpu(void)
{
    __init_cpu_features();
}

struct alm_cpu_features *
alm_cpu_get_features(void)
{
    __init_cpu_features();

    return &cpu_features;
}

uint32_t
alm_cpu_is_amd(void)
{
    __init_cpu_features();
    struct alm_cpu_mfg_info *mfg_info = &cpu_features.cpu_mfg_info;

    return mfg_info->mfg_type == ALM_CPU_MFG_AMD;
}

static uint32_t
alm_cpu_arch_is(uint16_t family)
{
    __init_cpu_features();

    struct alm_cpu_mfg_info *mfg_info = &cpu_features.cpu_mfg_info;

    return mfg_info->family == family;
}

uint32_t
alm_cpu_arch_is_zen(void)
{
    return alm_cpu_arch_is(ALM_CPU_FAMILY_NAPLES);
}

uint32_t
alm_cpu_arch_is_zen2(void)
{
    return alm_cpu_arch_is(ALM_CPU_FAMILY_ROME);
}

uint32_t
alm_cpu_arch_is_zen3(void)
{
    return alm_cpu_arch_is(ALM_CPU_FAMILY_MILAN);
}

uint32_t
alm_cpu_feature_is_avx_usable(void)
{
    __init_cpu_features();

    return ALM_CPU_FEATURE_AVX_USABLE(&cpu_features);
}

uint32_t
alm_cpu_feature_is_avx2_usable(void)
{
    __init_cpu_features();

    return ALM_CPU_FEATURE_AVX2_USABLE(&cpu_features);
}
