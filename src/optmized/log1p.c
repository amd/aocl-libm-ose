/*
 * Copyright (C) 2019, AMD. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 * 
 */

#include <stdint.h>
#include <libm_util_amd.h>
#include <libm_special.h>

#if !defined(__clang__) && !defined(ENABLE_DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("O2")
#endif  /* !DEBUG */

#include <libm_macros.h>
#include <libm/types.h>

#include <libm/typehelper.h>
#include <libm/compiler.h>

#include <math.h>

double
FN_PROTOTYPE(log1p)(double x)
{
    return (log1p(x));
}

