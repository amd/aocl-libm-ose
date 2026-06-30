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
 */

#ifndef LIBM_SINCOS_TBL_H
#define LIBM_SINCOS_TBL_H

/* N=64 double-double cos/sin table at A = m*pi/32, m in [0, 64). */

#include <stdint.h>
#include <libm/compiler.h>

typedef struct ALIGN(32) {
    double c;
    double cl;
    double s;
    double sl;
} sincos_entry_t;

#ifdef LIBM_SINCOS_TBL_DEFINE
const sincos_entry_t sincos_tbl[64] HIDDEN ALIGN(64) = {
    /*  0 */ { 0x1.0000000000000p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0 },
    /*  1 */ { 0x1.fd88da3d12526p-1, -0x1.87df6378811c7p-55, 0x1.917a6bc29b42cp-4, -0x1.e2718d26ed688p-60 },
    /*  2 */ { 0x1.f6297cff75cb0p-1, 0x1.562172a361fd3p-56, 0x1.8f8b83c69a60bp-3, -0x1.26d19b9ff8d82p-57 },
    /*  3 */ { 0x1.e9f4156c62ddap-1, 0x1.760b1e2e3f81ep-55, 0x1.294062ed59f06p-2, -0x1.5d28da2c4612dp-56 },
    /*  4 */ { 0x1.d906bcf328d46p-1, 0x1.457e610231ac2p-56, 0x1.87de2a6aea963p-2, -0x1.72cedd3d5a610p-57 },
    /*  5 */ { 0x1.c38b2f180bdb1p-1, -0x1.6e0b1757c8d07p-56, 0x1.e2b5d3806f63bp-2, 0x1.e0d891d3c6841p-58 },
    /*  6 */ { 0x1.a9b66290ea1a3p-1, 0x1.9f630e8b6dac8p-60, 0x1.1c73b39ae68c8p-1, 0x1.b25dd267f6600p-55 },
    /*  7 */ { 0x1.8bc806b151741p-1, -0x1.2c5e12ed1336dp-55, 0x1.44cf325091dd6p-1, 0x1.8076a2cfdc6b3p-57 },
    /*  8 */ { 0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26456p-55, 0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26456p-55 },
    /*  9 */ { 0x1.44cf325091dd6p-1, 0x1.8076a2cfdc6b3p-57, 0x1.8bc806b151741p-1, -0x1.2c5e12ed1336dp-55 },
    /* 10 */ { 0x1.1c73b39ae68c8p-1, 0x1.b25dd267f6600p-55, 0x1.a9b66290ea1a3p-1, 0x1.9f630e8b6dac8p-60 },
    /* 11 */ { 0x1.e2b5d3806f63bp-2, 0x1.e0d891d3c6841p-58, 0x1.c38b2f180bdb1p-1, -0x1.6e0b1757c8d07p-56 },
    /* 12 */ { 0x1.87de2a6aea963p-2, -0x1.72cedd3d5a610p-57, 0x1.d906bcf328d46p-1, 0x1.457e610231ac2p-56 },
    /* 13 */ { 0x1.294062ed59f06p-2, -0x1.5d28da2c4612dp-56, 0x1.e9f4156c62ddap-1, 0x1.760b1e2e3f81ep-55 },
    /* 14 */ { 0x1.8f8b83c69a60bp-3, -0x1.26d19b9ff8d82p-57, 0x1.f6297cff75cb0p-1, 0x1.562172a361fd3p-56 },
    /* 15 */ { 0x1.917a6bc29b42cp-4, -0x1.e2718d26ed688p-60, 0x1.fd88da3d12526p-1, -0x1.87df6378811c7p-55 },
    /* 16 */ { 0x0.0p+0, 0x0.0p+0, 0x1.0000000000000p+0, 0x0.0p+0 },
    /* 17 */ { -0x1.917a6bc29b42cp-4, 0x1.e2718d26ed688p-60, 0x1.fd88da3d12526p-1, -0x1.87df6378811c7p-55 },
    /* 18 */ { -0x1.8f8b83c69a60bp-3, 0x1.26d19b9ff8d82p-57, 0x1.f6297cff75cb0p-1, 0x1.562172a361fd3p-56 },
    /* 19 */ { -0x1.294062ed59f06p-2, 0x1.5d28da2c4612dp-56, 0x1.e9f4156c62ddap-1, 0x1.760b1e2e3f81ep-55 },
    /* 20 */ { -0x1.87de2a6aea963p-2, 0x1.72cedd3d5a610p-57, 0x1.d906bcf328d46p-1, 0x1.457e610231ac2p-56 },
    /* 21 */ { -0x1.e2b5d3806f63bp-2, -0x1.e0d891d3c6841p-58, 0x1.c38b2f180bdb1p-1, -0x1.6e0b1757c8d07p-56 },
    /* 22 */ { -0x1.1c73b39ae68c8p-1, -0x1.b25dd267f6600p-55, 0x1.a9b66290ea1a3p-1, 0x1.9f630e8b6dac8p-60 },
    /* 23 */ { -0x1.44cf325091dd6p-1, -0x1.8076a2cfdc6b3p-57, 0x1.8bc806b151741p-1, -0x1.2c5e12ed1336dp-55 },
    /* 24 */ { -0x1.6a09e667f3bcdp-1, 0x1.bdd3413b26456p-55, 0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26456p-55 },
    /* 25 */ { -0x1.8bc806b151741p-1, 0x1.2c5e12ed1336dp-55, 0x1.44cf325091dd6p-1, 0x1.8076a2cfdc6b3p-57 },
    /* 26 */ { -0x1.a9b66290ea1a3p-1, -0x1.9f630e8b6dac8p-60, 0x1.1c73b39ae68c8p-1, 0x1.b25dd267f6600p-55 },
    /* 27 */ { -0x1.c38b2f180bdb1p-1, 0x1.6e0b1757c8d07p-56, 0x1.e2b5d3806f63bp-2, 0x1.e0d891d3c6841p-58 },
    /* 28 */ { -0x1.d906bcf328d46p-1, -0x1.457e610231ac2p-56, 0x1.87de2a6aea963p-2, -0x1.72cedd3d5a610p-57 },
    /* 29 */ { -0x1.e9f4156c62ddap-1, -0x1.760b1e2e3f81ep-55, 0x1.294062ed59f06p-2, -0x1.5d28da2c4612dp-56 },
    /* 30 */ { -0x1.f6297cff75cb0p-1, -0x1.562172a361fd3p-56, 0x1.8f8b83c69a60bp-3, -0x1.26d19b9ff8d82p-57 },
    /* 31 */ { -0x1.fd88da3d12526p-1, 0x1.87df6378811c7p-55, 0x1.917a6bc29b42cp-4, -0x1.e2718d26ed688p-60 },
    /* 32 */ { -0x1.0000000000000p+0, 0x0.0p+0, 0x0.0p+0, 0x0.0p+0 },
    /* 33 */ { -0x1.fd88da3d12526p-1, 0x1.87df6378811c7p-55, -0x1.917a6bc29b42cp-4, 0x1.e2718d26ed688p-60 },
    /* 34 */ { -0x1.f6297cff75cb0p-1, -0x1.562172a361fd3p-56, -0x1.8f8b83c69a60bp-3, 0x1.26d19b9ff8d82p-57 },
    /* 35 */ { -0x1.e9f4156c62ddap-1, -0x1.760b1e2e3f81ep-55, -0x1.294062ed59f06p-2, 0x1.5d28da2c4612dp-56 },
    /* 36 */ { -0x1.d906bcf328d46p-1, -0x1.457e610231ac2p-56, -0x1.87de2a6aea963p-2, 0x1.72cedd3d5a610p-57 },
    /* 37 */ { -0x1.c38b2f180bdb1p-1, 0x1.6e0b1757c8d07p-56, -0x1.e2b5d3806f63bp-2, -0x1.e0d891d3c6841p-58 },
    /* 38 */ { -0x1.a9b66290ea1a3p-1, -0x1.9f630e8b6dac8p-60, -0x1.1c73b39ae68c8p-1, -0x1.b25dd267f6600p-55 },
    /* 39 */ { -0x1.8bc806b151741p-1, 0x1.2c5e12ed1336dp-55, -0x1.44cf325091dd6p-1, -0x1.8076a2cfdc6b3p-57 },
    /* 40 */ { -0x1.6a09e667f3bcdp-1, 0x1.bdd3413b26456p-55, -0x1.6a09e667f3bcdp-1, 0x1.bdd3413b26456p-55 },
    /* 41 */ { -0x1.44cf325091dd6p-1, -0x1.8076a2cfdc6b3p-57, -0x1.8bc806b151741p-1, 0x1.2c5e12ed1336dp-55 },
    /* 42 */ { -0x1.1c73b39ae68c8p-1, -0x1.b25dd267f6600p-55, -0x1.a9b66290ea1a3p-1, -0x1.9f630e8b6dac8p-60 },
    /* 43 */ { -0x1.e2b5d3806f63bp-2, -0x1.e0d891d3c6841p-58, -0x1.c38b2f180bdb1p-1, 0x1.6e0b1757c8d07p-56 },
    /* 44 */ { -0x1.87de2a6aea963p-2, 0x1.72cedd3d5a610p-57, -0x1.d906bcf328d46p-1, -0x1.457e610231ac2p-56 },
    /* 45 */ { -0x1.294062ed59f06p-2, 0x1.5d28da2c4612dp-56, -0x1.e9f4156c62ddap-1, -0x1.760b1e2e3f81ep-55 },
    /* 46 */ { -0x1.8f8b83c69a60bp-3, 0x1.26d19b9ff8d82p-57, -0x1.f6297cff75cb0p-1, -0x1.562172a361fd3p-56 },
    /* 47 */ { -0x1.917a6bc29b42cp-4, 0x1.e2718d26ed688p-60, -0x1.fd88da3d12526p-1, 0x1.87df6378811c7p-55 },
    /* 48 */ { 0x0.0p+0, 0x0.0p+0, -0x1.0000000000000p+0, 0x0.0p+0 },
    /* 49 */ { 0x1.917a6bc29b42cp-4, -0x1.e2718d26ed688p-60, -0x1.fd88da3d12526p-1, 0x1.87df6378811c7p-55 },
    /* 50 */ { 0x1.8f8b83c69a60bp-3, -0x1.26d19b9ff8d82p-57, -0x1.f6297cff75cb0p-1, -0x1.562172a361fd3p-56 },
    /* 51 */ { 0x1.294062ed59f06p-2, -0x1.5d28da2c4612dp-56, -0x1.e9f4156c62ddap-1, -0x1.760b1e2e3f81ep-55 },
    /* 52 */ { 0x1.87de2a6aea963p-2, -0x1.72cedd3d5a610p-57, -0x1.d906bcf328d46p-1, -0x1.457e610231ac2p-56 },
    /* 53 */ { 0x1.e2b5d3806f63bp-2, 0x1.e0d891d3c6841p-58, -0x1.c38b2f180bdb1p-1, 0x1.6e0b1757c8d07p-56 },
    /* 54 */ { 0x1.1c73b39ae68c8p-1, 0x1.b25dd267f6600p-55, -0x1.a9b66290ea1a3p-1, -0x1.9f630e8b6dac8p-60 },
    /* 55 */ { 0x1.44cf325091dd6p-1, 0x1.8076a2cfdc6b3p-57, -0x1.8bc806b151741p-1, 0x1.2c5e12ed1336dp-55 },
    /* 56 */ { 0x1.6a09e667f3bcdp-1, -0x1.bdd3413b26456p-55, -0x1.6a09e667f3bcdp-1, 0x1.bdd3413b26456p-55 },
    /* 57 */ { 0x1.8bc806b151741p-1, -0x1.2c5e12ed1336dp-55, -0x1.44cf325091dd6p-1, -0x1.8076a2cfdc6b3p-57 },
    /* 58 */ { 0x1.a9b66290ea1a3p-1, 0x1.9f630e8b6dac8p-60, -0x1.1c73b39ae68c8p-1, -0x1.b25dd267f6600p-55 },
    /* 59 */ { 0x1.c38b2f180bdb1p-1, -0x1.6e0b1757c8d07p-56, -0x1.e2b5d3806f63bp-2, -0x1.e0d891d3c6841p-58 },
    /* 60 */ { 0x1.d906bcf328d46p-1, 0x1.457e610231ac2p-56, -0x1.87de2a6aea963p-2, 0x1.72cedd3d5a610p-57 },
    /* 61 */ { 0x1.e9f4156c62ddap-1, 0x1.760b1e2e3f81ep-55, -0x1.294062ed59f06p-2, 0x1.5d28da2c4612dp-56 },
    /* 62 */ { 0x1.f6297cff75cb0p-1, 0x1.562172a361fd3p-56, -0x1.8f8b83c69a60bp-3, 0x1.26d19b9ff8d82p-57 },
    /* 63 */ { 0x1.fd88da3d12526p-1, -0x1.87df6378811c7p-55, -0x1.917a6bc29b42cp-4, 0x1.e2718d26ed688p-60 },
};
#else
extern const sincos_entry_t sincos_tbl[64] HIDDEN;
#endif

#endif /* LIBM_SINCOS_TBL_H */
