/*
 * Copyright (C) 2008-2023 Advanced Micro Devices, Inc. All rights reserved.
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

#define AMD_LIBM_VEC_EXPERIMENTAL

#include <stdio.h>
#include "amdlibm.h"
#include "amdlibm_vec.h"

void fmin_single_precision()
{
    printf ("Using Scalar single precision fminf()\n");
    float ipf1 = 0.01, ipf2 = 0.5, opf;
    opf = amd_fminf (ipf1, ipf2);
    printf("Input: %f,%f\tOutput: %f\n", ipf1,ipf2, opf);
}

void fmin_double_precision()
{
    printf ("Using Scalar double precision fmin()\n");
    double ipd1 = 1.89, ipd2= 0.45, opd;
    opd = amd_fmin(ipd1, ipd2);
    printf("Input: %lf, %lf\tOutput: %f\n", ipd1, ipd2, opd);
}


void fmin_single_precision_array()
{
    int n=5;
    float ipf1[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
    float ipf2[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
    float opf[10] = {0};

    amd_vrsa_fminf(n, ipf1, ipf2, opf);

    printf("Printing output of SINGLE PRECISION VECTOR - fmin function()\n");
    for(int i=0; i<10; ++i)
    {
        printf("%f ", opf[i]);
    }
    printf("\n----------\n");
}

void fmin_double_precision_array()
{
    int n=8;
    double ip1[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    double ip2[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    double op[10] = {0};

    amd_vrda_fmin(n, ip1, ip2, op);

    printf("Printing output of DOUBLE PRECISION VECTOR - fmin function()\n");
    for(int i=0; i<10; ++i)
    {
        printf("%lf ", op[i]);
    }
    printf("\n----------\n");
}

/**********************************************
 *    fmin function with increment
 * *******************************************/
void fmin_single_precision_array_with_inc()
{
    int n=5, inc1=2, inc2=0, inco=2;
    float ipf1[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
    float ipf2[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
    float opf[10] = {0};

    amd_vrsa_fminfi(n, ipf1, inc1, ipf2, inc2, opf, inco);

    printf("Printing output of SINGLE PRECISION VECTOR - fmin i function()\n");
    for(int i=0; i<10; ++i)
    {
        printf("%f ", opf[i]);
    }
    printf("\n----------\n");
}

void fmin_double_precision_array_with_inc()
{
    int n=10, inc1=1, inc2=-2, inco=2;
    double ip1[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    double ip2[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    double op[10] = {0};

    amd_vrda_fmini(n, ip1, inc1, ip2+9, inc2, op, inco);

    printf("Printing output of DOUBLE PRECISION VECTOR - fmin i function()\n");
    for(int i=0; i<10; ++i)
    {
        printf("%lf ", op[i]);
    }
    printf("\n----------\n");
}

int use_fmin()
{
    fmin_single_precision();
    fmin_double_precision();
    fmin_single_precision_array();
    fmin_double_precision_array();
    fmin_single_precision_array_with_inc();
    fmin_double_precision_array_with_inc();
    return 0;
}
