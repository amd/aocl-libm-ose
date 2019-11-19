/*
 * Copyright (C) 2018-2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 *
 */

#include <fn_macros.h>
#include <libm/types.h>
#include <libm/entry_pt.h>
#include "entry_pt_macros.h"

WEAK_LIBM_ALIAS(fastcbrtf, FN_PROTOTYPE(cbrtf));
WEAK_LIBM_ALIAS(fastcbrt, FN_PROTOTYPE(cbrt));
WEAK_LIBM_ALIAS(fastcosf, FN_PROTOTYPE(cosf));
WEAK_LIBM_ALIAS(fastcos, FN_PROTOTYPE(cos));
WEAK_LIBM_ALIAS(fastexp10f, FN_PROTOTYPE(exp10f));
WEAK_LIBM_ALIAS(fastexp10, FN_PROTOTYPE(exp10));
WEAK_LIBM_ALIAS(fastexp2f, FN_PROTOTYPE(exp2f));
WEAK_LIBM_ALIAS(fastexp2, FN_PROTOTYPE(exp2));
WEAK_LIBM_ALIAS(fastexpf, FN_PROTOTYPE(expf));
WEAK_LIBM_ALIAS(fastexp, FN_PROTOTYPE(exp));
WEAK_LIBM_ALIAS(fastexpm1f, FN_PROTOTYPE(expm1f));
WEAK_LIBM_ALIAS(fastexpm1, FN_PROTOTYPE(expm1));
WEAK_LIBM_ALIAS(fastfmaf, FN_PROTOTYPE(fmaf));
WEAK_LIBM_ALIAS(fastfma, FN_PROTOTYPE(fma));
WEAK_LIBM_ALIAS(fastlog10f,FN_PROTOTYPE(log10f));
WEAK_LIBM_ALIAS(fastlog10, FN_PROTOTYPE(log10));
WEAK_LIBM_ALIAS(fastlog1pf, FN_PROTOTYPE(log1pf));
WEAK_LIBM_ALIAS(fastlog1p, FN_PROTOTYPE(log1p));
WEAK_LIBM_ALIAS(fastlog2f, FN_PROTOTYPE(log2f));
WEAK_LIBM_ALIAS(fastlog2, FN_PROTOTYPE(log2));
WEAK_LIBM_ALIAS(fastlogf,FN_PROTOTYPE(logf));
WEAK_LIBM_ALIAS(fastlog, FN_PROTOTYPE(log));
WEAK_LIBM_ALIAS(fastpow, FN_PROTOTYPE(pow));
WEAK_LIBM_ALIAS(fastsincos, FN_PROTOTYPE(sincos));
WEAK_LIBM_ALIAS(fastsincos, FN_PROTOTYPE(sincosf));
WEAK_LIBM_ALIAS(fastsinf, FN_PROTOTYPE(sinf));
WEAK_LIBM_ALIAS(fasttanf, FN_PROTOTYPE(tanf));
WEAK_LIBM_ALIAS(fasttan, FN_PROTOTYPE(tan));
