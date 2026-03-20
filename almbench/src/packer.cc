/*
 * Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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

#include "packer.h"

/*
 * align_to:
 * Aligns value `c` to the next multiple of `e`.
 */
uint64_t align_to(uint64_t c, uint64_t e)
{
    return (c + (e - 1)) & ~(e - 1);
}

/*
 * store_val:
 * Stores a value of type T into an array of type S.
 * Supports scalar and SIMD types.
 */
template <typename T, typename S>
void store_val(const T val, S arr[])
{
    if constexpr (std::is_same<T, S>::value) {
        arr[0] = val;
    } else if constexpr (std::is_same<T, libm::AlignedM128>::value) {
        _mm_storeu_ps(arr, val.data);
    } else if constexpr (std::is_same<T, libm::AlignedM128d>::value) {
        _mm_storeu_pd(arr, val.data);
    } else if constexpr (std::is_same<T, libm::AlignedM256>::value) {
        _mm256_storeu_ps(arr, val.data);
    } else if constexpr (std::is_same<T, libm::AlignedM256d>::value) {
        _mm256_storeu_pd(arr, val.data);
#ifdef __AVX512F__
    } else if constexpr (std::is_same<T, libm::AlignedM512>::value) {
        _mm512_storeu_ps(arr, val.data);
    } else if constexpr (std::is_same<T, libm::AlignedM512d>::value) {
        _mm512_storeu_pd(arr, val.data);
#endif
    } else {
        std::cerr << "Unsupported data type" << std::endl;
    }
}

/* Explicit template instantiations for supported types */
template void store_val<float, float>(const float val, float arr[]);
template void store_val<double, double>(const double val, double arr[]);
template void store_val<libm::AlignedM128, float>(const libm::AlignedM128 val, float arr[]);
template void store_val<libm::AlignedM128d, double>(const libm::AlignedM128d val, double arr[]);
template void store_val<libm::AlignedM256, float>(const libm::AlignedM256 val, float arr[]);
template void store_val<libm::AlignedM256d, double>(const libm::AlignedM256d val, double arr[]);
#ifdef __AVX512F__
template void store_val<libm::AlignedM512, float>(const libm::AlignedM512 val, float arr[]);
template void store_val<libm::AlignedM512d, double>(const libm::AlignedM512d val, double arr[]);
#endif

/*
 * FloatPacker::pack:
 * Packs an array of scalar values into a SIMD type or returns the scalar directly.
 */
template <typename T, typename S>
T FloatPacker<T, S>::pack(const S arr[])
{
    if constexpr (std::is_same<T, S>::value) {
        return arr[0];
    } else if constexpr (std::is_same<T, libm::AlignedM128>::value) {
        return { _mm_loadu_ps(&arr[0]) };
    } else if constexpr (std::is_same<T, libm::AlignedM128d>::value) {
        return { _mm_loadu_pd(&arr[0]) };
    } else if constexpr (std::is_same<T, libm::AlignedM256>::value) {
        return { _mm256_loadu_ps(&arr[0]) };
    } else if constexpr (std::is_same<T, libm::AlignedM256d>::value) {
        return { _mm256_loadu_pd(&arr[0]) };
#ifdef __AVX512F__
    } else if constexpr (std::is_same<T, libm::AlignedM512>::value) {
        return { _mm512_loadu_ps(&arr[0]) };
    } else if constexpr (std::is_same<T, libm::AlignedM512d>::value) {
        return { _mm512_loadu_pd(&arr[0]) };
#endif
    } else {
        std::cerr << "Unsupported data type" << std::endl;
        return T{-1};
    }
}

/* Explicit template instantiations for supported types */
template class FloatPacker<float>;
template class FloatPacker<double>;
template class FloatPacker<libm::AlignedM128>;
template class FloatPacker<libm::AlignedM128d>;
template class FloatPacker<libm::AlignedM256>;
template class FloatPacker<libm::AlignedM256d>;
#ifdef __AVX512F__
template class FloatPacker<libm::AlignedM512>;
template class FloatPacker<libm::AlignedM512d>;
#endif