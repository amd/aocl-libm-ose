/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#include "libm_amd.h"
#include "libm_util_amd.h"

float FN_PROTOTYPE_REF(remquof) (float x, float y, int *quo)
{
    return (float) FN_PROTOTYPE(remquo)(x,y,quo);
}

