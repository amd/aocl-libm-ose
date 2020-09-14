/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include <libm_special.h>
#include <libm/compiler.h>
#include <libm/typehelper.h>
#include <libm/amd_funcs_internal.h>

float
tanf_oddcase(float result)
{
    return -1.0/result;
}

float
tanf_specialcase(float x)
{
    return ALM_PROTO(tanf)(x);
}
