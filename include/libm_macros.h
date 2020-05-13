/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*/

#ifndef __LIBM_MACROS_H__
#define __LIBM_MACROS_H__

#define INC_REF(x)    <c/x>
#define INC_AVX(x)    <avx/x>
#define INC_AVX2(x)   <avx2/x>
#define INC_AVX512(x) <avx512/x>

/*
 * Spare the ugliness to non-coders
 */
#if defined(WINDOWS)
#include <libm_win_macros.h>
#else
#include <libm_nix_macros.h>
#endif

#define FN_PROTOTYPE(fn_name) amd_##fn_name

#define FN_PROTOTYPE_REF(fn_name) amd_ref_##fn_name

#define FN_PROTOTYPE_FAST(f) amd_fast##f

/*
 * Function with optimum algorithm
 */
#define FN_PROTOTYPE_OPT(fn_name) amd_opt_##fn_name

#define FN_PROTOTYPE_BAS64(fn_name) __amd_bas64_##fn_name

#define FN_PROTOTYPE_BDOZR(fn_name) __amd_bdozr_##fn_name

#define FN_PROTOTYPE_FMA3(fn_name) __amd_fma3_##fn_name

#define FN_PROTOTYPE_AVX2(fn_name) __amd_avx2_##fn_name

#define FN_PROTOTYPE_AVX512(fn_name) __amd_avx512_##fn_name


// enable or disable exceptions in linux
#define __enable_IEEE_exceptions 1


#endif  /* __LIBM_MACROS_H__ */
