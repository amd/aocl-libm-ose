/*
 *
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#ifndef __LIBM_OPTIMIZED_SINGLE_EXPM1F_H__
#define __LIBM_OPTIMIZED_SINGLE_EXPM1F_H__

#include <stdint.h>

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

#define EXPM1F_N 6

#if EXPM1F_N == 6
const double __two_to_jby64[64];
#elif EXPM1F_N == 7
const double __two_to_jby128[128];
#endif


static const struct {
    double _64_by_ln2, ln2_by_64;
    double Huge;
#if 1
    struct {                            /* Min/Max values that can be passed */
        float min, max;
    } x;
    struct {                            /* Around 1 threshold to take different path */
        double hi, lo;
    } threshold;
#endif
    float poly[5];
    /* The pre-computed double-precision table */
    //double tab[1 << EXPM1F_N];
    const double *tab;
} expm1f_v2_data =  {
    .Huge       = 0x1.8p+52,	/* 2^52 * (1.0 + 0.5) */
#if 1
    .x          = {
        .min = -0x1.154244p+4f,	/* ~= -17.32867 */
        .max =  0x1.633332p+6f,	/* ~=  88.79999 */
    },
    .threshold  = {
        .hi = 0x3E647FBF,	/* log(1 + 1/4) =  0.223144 */
        .lo = 0xBE934B11	/* log(1 - 1/4) = -0.287682 */
    },
#endif
    ._64_by_ln2 = 0x1.71547652b82fep+6,
    .ln2_by_64  = 0x1.62e42fefa39efp-7,
    .poly       = {
        0x1.555554p-3f,			/* 0x3E2AAAAA */
        0x1.55554p-5f,			/* 0x3D2AAAA0 */
        0x1.1113fep-7f,			/* 0x3C0889FF */
        0x1.6c9bcap-10f,		/* 0x3AB64DE5 */
        0x1.95664ep-13f,		/* 0x394AB327 */
    },

    .tab = __two_to_jby64,
};

#endif 					/* LIBM_OPTIMIZED_SINGLE_EXPM1F_H */
