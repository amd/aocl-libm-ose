/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#include <libm_macros.h>
#include <libm/cpu_features.h>
#include <libm/entry_pt.h>
#include <libm/iface.h>
#include <libm/amd_funcs_internal.h>    /* Contains all implementations */

#include <libm/arch/zen2.h>
#include <libm/arch/zen3.h>

typedef float (*amd_tanhf_t)(float);

void
LIBM_IFACE_PROTO(tanh)(void *arg)
{
    amd_tanhf_t fn_s = NULL;

    static struct cpu_features *features = NULL;

    if(!features)   {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    fn_s = &FN_PROTOTYPE_REF(tanhf);

    if (CPU_HAS_AVX2(features) &&
        CPU_FEATURE_AVX2_USABLE(features)) {
        fn_s = &FN_PROTOTYPE_OPT(tanhf);
    }

    if (mfg_info->mfg_type == CPU_MFG_AMD) {
        switch(mfg_info->family) {
            case 0x15:                      /* Naples */
                        break;
            case 0x17:                      /* Rome */
                        fn_s = &ALM_PROTO_ARCH_ZN2(tanhf);
                        break;
            case 0x19:                      /* Milan */
                        fn_s = &ALM_PROTO_ARCH_ZN3(tanhf);
                        break;
        }
    }

	/* Double */
	G_ENTRY_PT_PTR(tanh) = &FN_PROTOTYPE_REF(tanh);

	/* Single */
	G_ENTRY_PT_PTR(tanhf) = fn_s;

	/* Vector Double */
	/* Vector Single */
}

