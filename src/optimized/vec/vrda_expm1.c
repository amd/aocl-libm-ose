
/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
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
/********************************************
 * ---------------------
 * Signature
 * ---------------------
 * void vrda_expm1(int length, double *input, double *result)
 *
 * vrda_expm1() computes the expm1 values for 'length' number of elements
 * present in the 'input' array.
 * The corresponding ouput is stored in the 'result' array.
 *
 * ---------------------
 * Implementation Notes
 * ---------------------
 *
 * For any given length,
 *     For each element in the input array,
 *         call expm1f()
 *     Store the output into result array.
 * Return
 * 
 * TODO: The above mentioned logic can be enhanced with vrd4_expm1() API, once it is implemented. 
 */
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
void ALM_PROTO_OPT(vrda_expm1)(int length, double *input, double *result)
{
    for (int i = 0; i < length ; i++)
    {
        result[i] = ALM_PROTO(expm1)(input[i]);
    }
}