
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
#include "amdlibm_vec.h"

int use_sub()
{
    int lenf=10;
    float ipf1[10] = {0.5f, 1.0f, 1.5f, 2.0f, 0.5f, 1.0f, 1.5f, 2.0f, 0.5f, 1.0f};
    float ipf2[10] = {0.5f, 1.0f, 1.5f, 2.0f, 0.5f, 1.0f, 1.5f, 2.0f, 0.5f, 1.0f};
    float opf[10];
    amd_vrsa_subf(lenf, ipf1, ipf2, opf);
    printf("Printing output of SINGLE PRECISION VECTOR - SUB function()\n");
    for(int i=0; i<lenf; ++i)
    {
        printf("%f ", opf[i]);
    }
    printf("\n----------\n");
    int len=10;
    double ip1[10] = {0.5, 1.0, 1.5, 2.0, 0.5, 1.0, 1.5, 2.0, 0.5, 1.0};
    double ip2[10] = {0.5, 1.0, 1.5, 2.0, 0.5, 1.0, 1.5, 2.0, 0.5, 1.0};
    double op[10];
    amd_vrda_sub(len, ip1, ip2, op);
    printf("Printing output of DOUBLE PRECISION VECTOR - SUB function()\n");
    for(int i=0; i<len; ++i)
    {
        printf("%lf ", opf[i]);
    }
    printf("\n----------\n");
    return 0;
}