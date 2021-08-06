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

#ifndef __LIBM_MACROS_H__
#define __LIBM_MACROS_H__

/*
 * Spare the ugliness to non-coders
 */
#if defined(WINDOWS)
#include <libm_win_macros.h>
#else
#include <libm_nix_macros.h>
#endif

#ifndef ALM_OVERRIDE

#define ALM_PREFIX                      amd

#define FN_PROTOTYPE(fn_name)           amd_##fn_name

/*
 * ALM_PROTO_OPT to be removed, once all the FN_PROTOTYPE_OPT is converted to ALM_PROTO
 */
#define ALM_PROTO_OPT(fn_name)  amd_opt_##fn_name
#else
#ifndef ALM_ARCH
#error "ALM_ARCH needs to be defined or ALM_OVERRIDE needs to be removed"
#endif

#endif /* ALM_OVERRIDE */

#if defined(ALM_SUFFIX)
#define ALM_PROTO(x)      ALM_MAKE_PROTO_SFX(ALM_PREFIX, x, ALM_SUFFIX)
#else
#define ALM_PROTO(x)      ALM_MAKE_PROTO(ALM_PREFIX, x)
#endif

#define ALM_MAKE_PROTO_SFX(pfx, fn, sfx)        __ALM_MAKE_PROTO_SFX(pfx, fn, sfx)
#define ALM_MAKE_PROTO(pfx, fn)                 __ALM_MAKE_PROTO(pfx, fn)

#define __ALM_MAKE_PROTO_SFX(pfx, fn, sfx)        pfx##_##fn##_##sfx
#define __ALM_MAKE_PROTO(pfx, fn)                 pfx##_##fn

#define ALM_PROTO_REF(fn)    		amd_ref_##fn
#define ALM_PROTO_FAST(fn)   		amd_fast##fn
#define ALM_PROTO_KERN(fn)    		alm_kern_##fn

/* 64-bit variants */
#define ALM_PROTO_BAS64(fn)	__amd_bas64_##fn
/* Generic FMA3 */
#define ALM_PROTO_FMA3(fn)	__amd_fma3_##fn
/* AVX2 versions */
#define ALM_PROTO_AVX2(fn)	__amd_avx2_##fn
/* AVX512 versions */
#define ALM_PROTO_AVX512(fn)	__amd_avx512_##fn

/* Buldozer specific, FMA3 and FMA4 */
#define ALM_PROTO_BDOZR(fn)	__amd_bdozr_##fn

/*
 * Generic arch specific name creators
 */
#define __ALM_PROTO_ARCH(a, x, y)       ALM_MAKE_PROTO_SFX(a, x, y)
#define  ALM_PROTO_ARCH(a, x, y)        __ALM_PROTO_ARCH(a, x, y)

// enable or disable exceptions in linux
#define __enable_IEEE_exceptions 1


#endif  /* __LIBM_MACROS_H__ */
