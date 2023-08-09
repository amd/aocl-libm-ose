/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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


static double
ALM_PROTO_KERN(log1p)(double x, double head, double tail)
{
    double  u, q, r;
    double  A1, A2, B1, B2, R1;
    double  u3, u7, u2;

    static const double coeffs[5] = {
        0x1.55555555554e6p-4,               // 1/2^2 * 3
        0x1.9999999bac6d4p-7,               // 1/2^4 * 5
        0x1.2492307f1519fp-9,               // 1/2^6 * 7
        0x1.c8034c85dfff0p-12               // 1/2^8 * 9
    };

#define CA1 coeffs[0]
#define CA2 coeffs[1]
#define CA3 coeffs[2]
#define CA4 coeffs[3]

    r = x - 1.0;

    flt64_t fx = {.d = r};

    double u_by_2 = r / (2.0 + r);

    q = u_by_2 * r;  /* correction */

    u = u_by_2 + u_by_2;

    u2 = u * u;

    A1 = CA2 * u2 + CA1;
    A2 = CA4 * u2 + CA3;

    u3 = u2 * u;
    B1 =  u3 * A1;

    u7 = u * (u3 * u3);
    B2 = u7 * A2;

    R1 = B1 + B2;
    double poly = R1 - q;

    fx.u &= 0xffffffff00000000;
    double u_high = fx.d;
    double u_low  = r - u_high;

    double s = poly + u_low;

    double ans  = (s * tail) + (u_high * tail);
    ans        += (s * head) + (u_high * head);

    return ans;
}

