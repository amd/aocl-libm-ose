/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include "amdlibm.h"

/*
 * Stable ascending sort of a double-precision array.
 *
 * The API is index-based: the input is never modified; instead it returns a
 * permutation (dst_index) that lists the elements in stable ascending order.
 * "Stable" means elements that compare equal keep their original relative
 * order (see the two 3.5s and two 2.0s below).
 *
 * 2 APIs are provided and they need to be used in the following manner :
 *   1. amd_stablesort_getsize_64f() - how big a scratch workspace is needed.
 *   2. amd_stablesort_ascend_64f()  - do the sort into a caller-owned index array using the workspace.
 */
int use_stablesort()
{
    printf("\n\n***** stablesort() *****\n");

    double src[] = { 3.5, -1.0, 2.0, 3.5, 0.0, 2.0 };
    int len = (int)(sizeof(src) / sizeof(src[0]));

    /* 1) Ask how many bytes of scratch workspace this length needs. */
    int workspace_size = 0;
    if (amd_stablesort_getsize_64f(len, &workspace_size) != 0) {
        printf("stablesort: getsize failed\n");
        return 1;
    }

    void *workspace  = malloc((size_t)workspace_size);
    int  *dst_index  = malloc((size_t)len * sizeof(int));
    if (workspace == NULL || dst_index == NULL) {
        printf("stablesort: allocation failed\n");
        free(workspace);
        free(dst_index);
        return 1;
    }

    /* 2) Produce the stable ascending permutation into dst_index.
     *    src_stride_bytes is the byte gap between consecutive elements;
     *    for a plain contiguous array that is simply sizeof(double). */
    if (amd_stablesort_ascend_64f(src, (int)sizeof(double),
                                  dst_index, len, workspace) != 0) {
        printf("stablesort: ascend failed\n");
        free(workspace);
        free(dst_index);
        return 1;
    }

    printf("Using Scalar double precision stable ascending sort amd_stablesort_ascend_64f()\n");
    printf("Input: {");
    for (int i = 0; i < len; ++i)
    {
        printf("%f, ", src[i]);
    }
    printf("}, Output: sorted: {");
    for (int i = 0; i < len; ++i)
    {
        printf("%f, ", src[dst_index[i]]);
    }
    printf("}, order: {");
    for (int i = 0; i < len; ++i)
    {
        printf("%d, ", dst_index[i]);
    }
    printf("}\n");
    printf("----------\n");

    free(workspace);
    free(dst_index);
    return 0;
}
