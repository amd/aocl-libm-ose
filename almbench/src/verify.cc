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

#include <iostream>
#include <cstdint>
#include <type_traits>
#include <bitset>
#include <iomanip>

#include "ulp.h"

/*
 * typeinfo::RealType:
 * Maps floating-point types to their corresponding unsigned integer types.
 */
namespace typeinfo {

template <typename T>
struct RealType;

template <>
struct RealType<float> {
    using uint = uint32_t;
};

template <>
struct RealType<double> {
    using uint = uint64_t;
};

template <>
struct RealType<long double> {
    using uint = uint64_t;
};

} /* namespace typeinfo */

/*
 * PrintUlpResults01:
 * Prints ULP comparison results for single-input functions.
 */
template <typename S, typename L>
void PrintUlpResults01(S inp, S ap, L xp, double ulp)
{
    using SUINT = typename std::conditional<std::is_same<S, float>::value,
                                            uint32_t, uint64_t>::type;
    using DUINT = uint64_t;

    std::cout << "Inp      : " << std::dec << inp
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&inp) << ")\n";
    std::cout << "Actual   : " << std::dec << ap
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&ap) << ")\n";
    std::cout << "Expected : " << std::dec << xp
              << " (0x" << std::hex << *reinterpret_cast<DUINT *>(&xp) << ")\n";
    std::cout << "ULP      : " << std::dec << ulp
              << " (0x" << std::hex << *reinterpret_cast<DUINT *>(&ulp) << ")\n";
}

template void PrintUlpResults01<float, double>(float ip, float op, double mpfrop, double ulp);
template void PrintUlpResults01<double, long double>(double ip, double op, long double mpfrop, double ulp);

/*
 * PrintUlpResults02:
 * Prints ULP comparison results for two-input functions.
 */
template <typename S, typename L>
void PrintUlpResults02(S inp1, S inp2, S ap, L xp, double ulp)
{
    using SUINT = typename std::conditional<std::is_same<S, float>::value,
                                            uint32_t, uint64_t>::type;
    using DUINT = uint64_t;

    std::cout << "Inp1     : " << std::dec << inp1
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&inp1) << ")\n";
    std::cout << "Inp2     : " << std::dec << inp2
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&inp2) << ")\n";
    std::cout << "Actual   : " << std::dec << ap
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&ap) << ")\n";
    std::cout << "Expected : " << std::dec << xp
              << " (0x" << std::hex << *reinterpret_cast<DUINT *>(&xp) << ")\n";
    std::cout << "ULP      : " << std::dec << ulp
              << " (0x" << std::hex << *reinterpret_cast<DUINT *>(&ulp) << ")\n";
}

template void PrintUlpResults02<float, double>(float ip1, float ip2, float op, double mpfrop, double ulp);
template void PrintUlpResults02<double, long double>(double ip1, double ip2, double op, long double mpfrop, double ulp);

/*
 * printval:
 * Helper function to print variadic input values with type-specific formatting.
 */
template <typename... Args>
void printval(Args... inp)
{
    std::cout << "Inp : ";

     auto print =  [](auto ip) {
        using T = decltype(ip);
        if constexpr (std::is_integral_v<T>) {
            std::cout << std::dec << ip << " ";
        } else if constexpr (std::is_floating_point_v<T>) {
            using UINT = typename std::conditional<std::is_same<T, float>::value,
                                                   uint32_t, uint64_t>::type;
            std::cout << std::dec << ip
                      << " (0x" << std::hex << *reinterpret_cast<UINT *>(&ip) << ") ";
        } else {
            std::cerr << "Error: Unsupported input type\n";
        }
    };

    (print(inp), ...);
    std::cout << std::endl;
}

/*
 * PrintUlpResults:
 * Prints ULP results for functions with variadic inputs.
 */
template <typename S, typename L, typename... Args>
void PrintUlpResults(S ap, L xp, double ulp, Args... inputs)
{
    using SUINT = typename std::conditional<std::is_same<S, float>::value,
                                            uint32_t, uint64_t>::type;
    using DUINT = uint64_t;

    std::cout << "Inputs: ";
    auto print =  [](auto ip) {
        using T = decltype(ip);
        if constexpr (std::is_integral_v<T>) {
            std::cout << std::dec << ip << " ";
        } else if constexpr (std::is_floating_point_v<T>) {
            using UINT = typename std::conditional<std::is_same<T, float>::value,
                                                   uint32_t, uint64_t>::type;
            std::cout << std::dec << ip
                      << " (0x" << std::hex << *reinterpret_cast<UINT *>(&ip) << ") ";
        } else {
            std::cerr << "Error: Unsupported input type\n";
        }
    };
//    std::cout << std::endl;
    (print(inputs), ...);

    std::cout << "Actual: " << std::dec << ap
              << " (0x" << std::hex << *reinterpret_cast<const SUINT *>(&ap) << ") ";
    std::cout << "Exptd: " << std::dec << xp
              << " (0x" << std::hex << *reinterpret_cast<const DUINT *>(&xp) << ") ";
    std::cout << "ULP: " << std::dec << ulp
              << " (0x" << std::hex << *reinterpret_cast<const DUINT *>(&ulp) << ")\n";
}

/* Explicit template instantiations */
template void PrintUlpResults<float, double>(float ap, double xp, double ulp, float ip1);
template void PrintUlpResults<double, long double>(double ap, long double xp, double ulp, double ip1);

template void PrintUlpResults<float, double>(float ap, double xp, double ulp, float ip1, float ip2);
template void PrintUlpResults<double, long double>(double ap, long double xp, double ulp, double ip1, double ip2);

template void PrintUlpResults<float, double>(float ap, double xp, double ulp, float ip1, int ip2);
template void PrintUlpResults<double, long double>(double ap, long double xp, double ulp, double ip1, int ip2);

template void PrintUlpResults<float, double>(float ap, double xp, double ulp,
                                             float ip1, float ip2, float ip3,
                                             float ip4, float ip5, float ip6);
template void PrintUlpResults<double, long double>(double ap, long double xp, double ulp,
                                                   double ip1, double ip2, double ip3,
                                                   double ip4, double ip5, double ip6);


/*
 * print_00:
 * Prints input, actual, and expected values with their bit representations.
 */
template <typename S, typename L>
void print_00(S inp, S ap, L xp)
{
    using SUINT = typename typeinfo::RealType<S>::uint;
    using LUINT = typename typeinfo::RealType<L>::uint;

    std::cout << "Inp  : " << std::dec << inp
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&inp) << ")  ";
    std::cout << "Amd  : " << std::dec << ap
              << " (0x" << std::hex << *reinterpret_cast<SUINT *>(&ap) << ")  ";
    std::cout << "Mpfr : " << std::dec << xp
              << " (0x" << std::hex << *reinterpret_cast<LUINT *>(&xp) << ")\n";
}

template void print_00<float, double>(float ip, float op, double mpfrop);
template void print_00<double, long double>(double ip, double op, long double mpfrop);

/*
 * FloatIntUnion:
 * Union to reinterpret a float/double as an integer for bit-level inspection.
 */
template <typename S, typename U>
union FloatIntUnion {
    S f;
    U i;
};

/*
 * print_formats:
 * Prints a floating-point value in multiple representations.
 */
template <typename S>
void print_formats(S val)
{
    using U = typename typeinfo::RealType<S>::uint;
    FloatIntUnion<S, U> u_val;
    u_val.f = val;

    std::cout << "Value     : " << val << "\n";
    std::cout << "Binary    : " << std::bitset<sizeof(S) * 8>(u_val.i) << "\n";
    std::cout << "Hex       : 0x" << std::hex << u_val.i << std::dec << "\n";
    std::cout << "Hexfloat  : " << std::hexfloat << val << std::defaultfloat << "\n";
    std::cout << "Decimal   : " << val << "\n";
    std::cout << "Canonical : " << std::scientific << val << std::defaultfloat << "\n";
    std::cout << "Uint      : " << u_val.i << "\n\n";
}

template void print_formats<float>(float val);
template void print_formats<double>(double val);

/*
 * print_val:
 * Prints a floating-point value with bit-level and formatted representations.
 */
template <typename S>
void print_val(S val)
{
    using U = typename std::conditional<std::is_same<S, float>::value,
                                        uint32_t, uint64_t>::type;

    std::cout << "Value       : " << val << "\n";
    std::cout << "Binary      : " << std::bitset<sizeof(U) * 8>(*reinterpret_cast<U *>(&val)) << "\n";
    std::cout << "Hex         : 0x" << std::hex << *reinterpret_cast<U *>(&val) << "\n";
    std::cout << "Hexadecimal : " << std::hexfloat << val << "\n";
    std::cout << "Decimal     : " << std::dec << val << "\n";
    std::cout << "Canonical   : " << std::scientific << val << "\n";
    std::cout << "Uint        : " << *reinterpret_cast<U *>(&val) << "\n\n";
    std::cout << std::defaultfloat;
}

template void print_val<float>(float val);
template void print_val<double>(double val);