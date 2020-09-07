/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*
*/

#include <stdio.h>
#include "amdlibm.h"
#include <immintrin.h>

int use_atan()
{
    printf ("Using Scalar single precision atanf()\n");
    float ipf = 0.5, opf;
    int i;
    opf = amd_atanf (ipf);
    printf("Input: %f\tOutput: %f\n", ipf, opf);
    printf ("Using Scalar double precision atan()\n");
    double ipd = 0.45, opd;
    opd = amd_atan(ipd);
    printf("Input: %f\tOutput: %f\n", ipd, opd);
    return 0;
}
