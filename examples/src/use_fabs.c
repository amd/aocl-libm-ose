/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*
*/

#include <stdio.h>
#include "amdlibm.h"
#include <immintrin.h>

int use_fabs()
{
    printf ("Using Scalar single precision fabsf()\n");
    float ipf = 0.5, opf;
    int i;
    opf = amd_fabsf (ipf);
    printf("Input: %f\tOutput: %f\n", ipf, opf);
    printf ("Using Scalar double precision fabs()\n");
    double ipd = 0.45, opd;
    opd = amd_fabs(ipd);
    printf("Input: %f\tOutput: %f\n", ipd, opd);
    return 0;
}
