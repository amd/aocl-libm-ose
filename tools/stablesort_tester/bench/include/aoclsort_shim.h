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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * stablesort benchmark shim ABI.
 *
 * The benchmark always measures the built-in aocl-libm stablesort
 * implementation (side A). To A/B-compare against another sort (side B) - for
 * example Intel IPP's ippsSortRadixIndexAscend_64f - you build a small shared
 * library OUTSIDE this project that implements the two functions below, and
 * pass it to the benchmark with `--shim /path/to/libyour_shim.so`. The
 * benchmark dlopen()s it at runtime, so no third-party sort ever enters the
 * aocl-libm build.
 *
 * Required exported symbols (C linkage):
 *   aoclsort_shim_get_size_64f
 *   aoclsort_shim_sort_indexed_ascend_64f
 *
 * Optional exported symbol:
 *   aoclsort_shim_name - returns a short label used in the benchmark output.
 *                        If absent, the shim file's basename is used.
 *
 * Contract (identical to stablesort): return 0 on success, negative on invalid
 * input. `ascend` must write a stable ascending permutation of [0, len) into
 * dst_index, reading element i of src at byte offset i * src_stride_bytes.
 */

int aoclsort_shim_get_size_64f(int len, int *workspace_size);

int aoclsort_shim_sort_indexed_ascend_64f(const double *src,
                                          int src_stride_bytes, int *dst_index,
                                          int len, void *workspace);

const char *aoclsort_shim_name(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
