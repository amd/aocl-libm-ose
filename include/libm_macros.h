/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#define ALM_PREFIX      		amd

#define FN_PROTOTYPE(fn_name)           amd_##fn_name

/*
 * ALM_PROTO_OPT to be removed, once all the FN_PROTOTYPE_OPT is converted to ALM_PROTO
 */
#define ALM_PROTO_OPT(fn_name)	amd_opt_##fn_name


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

#define FN_PROTOTYPE_OPT(fn)            amd_opt_##fn
#define FN_PROTOTYPE_REF(fn)            amd_ref_##fn
#define FN_PROTOTYPE_FAST(fn)           amd_fast##fn

#define ALM_PROTO_REF(fn)    		amd_ref_##fn
#define ALM_PROTO_FAST(fn)   		amd_fast##fn

/* 64-bit variants */
#define ALM_PROTO_BAS64(fn)	__amd_bas64_##fn
/* Buldozer specific, FMA3 and FMA4 */
#define ALM_PROTO_BDOZR(fn)	__amd_bdozr_##fn
/* Generic FMA3 */
#define ALM_PROTO_FMA3(fn)	__amd_fma3_##fn
/* AVX2 versions */
#define ALM_PROTO_AVX2(fn)	__amd_avx2_##fn
/* AVX512 versions */
#define ALM_PROTO_AVX512(fn)	__amd_avx512_##fn


#define FN_PROTOTYPE_BAS64(fn_name) __amd_bas64_##fn_name

#define FN_PROTOTYPE_BDOZR(fn_name) __amd_bdozr_##fn_name

#define FN_PROTOTYPE_FMA3(fn_name) __amd_fma3_##fn_name

#define FN_PROTOTYPE_AVX2(fn_name) __amd_avx2_##fn_name

#define FN_PROTOTYPE_AVX512(fn_name) __amd_avx512_##fn_name


// enable or disable exceptions in linux
#define __enable_IEEE_exceptions 1


#endif  /* __LIBM_MACROS_H__ */
