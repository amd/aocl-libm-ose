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

typedef float  (*amd_coshf_t)(float);
typedef double  (*amd_cosh_t)(double);
typedef __m128  (*amd_cosh_v4s_t)(__m128);
typedef __m256  (*amd_cosh_v8s_t)(__m256);

void
LIBM_IFACE_PROTO(cosh)(void *arg)
{

    amd_coshf_t fn_s = NULL;
    amd_cosh_t  fn_d = NULL;
    amd_cosh_v4s_t fn_v4s = NULL;
    amd_cosh_v8s_t fn_v8s = NULL;

    static struct cpu_features *features = NULL;

    if (!features) {
        features = libm_cpu_get_features();
    }

    struct cpu_mfg_info *mfg_info = &features->cpu_mfg_info;

    /*single precision scalar*/
    fn_s = &FN_PROTOTYPE_OPT(coshf);
    /*double precision scalar*/
    fn_d = &FN_PROTOTYPE_REF(cosh);
    /* Vector Single */
    fn_v4s = &FN_PROTOTYPE_OPT(vrs4_coshf);
    fn_v8s = &FN_PROTOTYPE_OPT(vrs8_coshf);

    /*
     * Template:
     *     override with any micro-architecture-specific
     *     implementations
     */
    if (mfg_info->mfg_type == CPU_MFG_AMD) {
        switch(mfg_info->family) {
            case 0x15:                      /* Naples */
                        break;
            case 0x17:                      /* Rome */
                        fn_s   = &ALM_PROTO_ARCH_ZN2(coshf);
                        fn_v4s = &ALM_PROTO_ARCH_ZN2(vrs4_coshf);
                        fn_v8s = &ALM_PROTO_ARCH_ZN2(vrs8_coshf);
                        break;
            case 0x19:                      /* Milan */
                        fn_s   = &ALM_PROTO_ARCH_ZN3(coshf);
                        fn_v4s = &ALM_PROTO_ARCH_ZN3(vrs4_coshf);
                        fn_v8s = &ALM_PROTO_ARCH_ZN3(vrs8_coshf);
                        break;
        }
    }

     /* Single */
    G_ENTRY_PT_PTR(coshf)      = fn_s;
    G_ENTRY_PT_PTR(cosh)       = fn_d;
    G_ENTRY_PT_PTR(vrs4_coshf) = fn_v4s;
    G_ENTRY_PT_PTR(vrs8_coshf) = fn_v8s;
}

