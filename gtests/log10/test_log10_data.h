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

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the log10f() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_log10f_conformance_data[] = {
    /*Constant inputs from the Constants.h*/
    {POS_SNAN_F32,POS_QNAN_F32,FE_INVALID}, // invalid
    {POS_SNAN_F32,POS_QNAN_F32,0}, //
    {POS_QNAN_F32,POS_QNAN_F32,0}, //
    {POS_INF_F32,POS_INF_F32,0}, //
    {NEG_INF_F32,NEG_QNAN_F32,FE_INVALID}, //
    {POS_ONE_F32,0x00000000,0}, //
    {NEG_ONE_F32,0xff800000,0}, //10
    {NEG_ZERO_F32,NEG_ZERO_F32,0}, //
    {POS_ZERO_F32,POS_ZERO_F32,0}, //
    {POS_PI_F32,0x3efe8a6b,0}, //
    {NEG_PI_F32,NEG_QNAN_F32,FE_INVALID}, //

    /*Other Input */
    { 0xbf800000, 0xff800000, 0},//25
    { 0xbf7e0000, 0xc09b43d5, 0},
    { 0xbf7c0000, 0xc0851592, 0},
    { 0xbefdb976, 0xbf2f2e21, 0},
    { 0x3f800000, 0x00000000, 0},
    { 0xbf7ffffe, 0xc17f1402, 0},
    { 0x40090fdb, 0x3ea95658, 0},
    { 0x40a90fdb, 0x3f390fb4, 0},
    { 0x41100000, 0x3f74493d, 0},
    { 0x4479c000, 0x403ff8e2, 0},
    { 0x427c0000, 0x3fe650cb, 0},
    { 0x42ad0000, 0x3ff7f025, 0},
    { 0x42ae0000, 0x3ff8422b, 0},
    { 0x42be0000, 0x3ffd260c, 0},
    { 0xc2b10000, 0xffc00000, FE_INVALID},//40
    { 0xc2e20000, 0xffc00000, FE_INVALID},
    { 0xc3010000, 0xffc00000, FE_INVALID},
    { 0xc2b0ac4f, 0xffc00000, FE_INVALID},
    { 0xc2b0ac50, 0xffc00000, FE_INVALID},
    { 0xc2d00000, 0xffc00000, FE_INVALID},
    { 0x42af7216, 0x3ff8b7ea, 0},
    { 0x42af7217, 0x3ff8b7ea, 0},
    { 0x42af7218, 0x3ff8b7eb, 0},
    { 0x50000000, 0x411ef19f, 0},
    { 0xc20e1eb8, 0xffc00000, FE_INVALID},
    { 0xc6de68b6, 0xffc00000, FE_INVALID},
    { 0xbfa66666, 0xffc00000, FE_INVALID},
    { 0xbfd23904, 0xffc00000, FE_INVALID},
    { 0xbfc00000, 0xffc00000, FE_INVALID},
    { 0xbf333333, 0xbf9a1bc8, 0},
    { 0xbf000000, 0xbf317218, 0},
    { 0x42061eb8, 0x3fc34168, 0},
    { 0x46de64b6, 0x408e89e3, 0},
    { 0xc2ca0000, 0xffc00000, FE_INVALID},
    { 0x42c60000, 0x3fff70f9, 0},

    { 0x7f800001, 0x7fc00001, 0},
    { 0xff800001, 0xffc00001, 0},
    { 0x7fc00000, 0x7fc00000, 0},
    { 0xffc00000, 0xffc00000, 0},
    { 0x7f800000, 0x7f800000, 0},
    { 0x7fc00000, 0x7fc00000, 0},
    { 0x3f800000, 0x00000000, 0},
    { 0xbf800000, 0x7fc00000, 0},
    { 0x00000000, 0x00000000, 0},
    { 0x80000000, 0x80000000, 0},

    { 0x007fffff, 0xc217b818, 0},
    { 0x807fffff, 0x807fffff, 0},
    { 0x00000001, 0xc23369f4, 0},
    { 0x80000001, 0x80000001, 0},
    { 0x7f7fffff, 0x421a209b, 0},
    { 0xff7fffff, 0x7fc00000, 0},
    { 0x00800000, 0xc217b818, 0},
    { 0x80800000, 0x80800000, 0},

    { 0x3d800000, 0xbf9a209b, 0}, // 0.0625
    { 0x3cf5c28f, 0xbfc2edb1, 0}, // 0.03
    { 0x3f880000, 0x3cd7afc9, 0}, // 1.0625
    { 0xbf800001, 0x7fc00000, 0}, // -1.00000012
    { 0xbf7fffff, 0xc1851592, 0}, // -0.99999994
    { 0xbf7fff58, 0xc1382f62, 0}, // -0.99999
    { 0xbf7ae148, 0xc07a5e9a, 0}, // -0.98

    { 0x3fc90fdb, 0x3e48d3a7, 0}, //pi/2
    { 0x3c000000, 0xc006dc87, 0}, //0.0078125
    { 0x3f012345, 0xbe9828da, 0}, //0.504444
    { 0x40000000, 0x3e9a209b, 0}, //2
    { 0x40490fdb, 0x3efe8a6e, 0}, //pi
    { 0x40c90fdb, 0x3f4c5584, 0}, //2pi
    { 0x41200000, 0x3f800000, 0}, //10
    { 0x447a0000, 0x40400000, 0}, //1000
    { 0x71200000, 0x41ef30e8, 0}, // 1.25*2^99 

    { 0xc2af0000, 0x7fc00000, 0}, //-87.5
    { 0xc2e00000, 0x7fc00000, 0}, //-112
    { 0xc3000000, 0x7fc00000, 0}, //-128
    { 0xc2800000, 0x7fc00000, 0}, //-64

    { 0x3fdbf0a8, 0x3e70bc95, 0}, // (e-1)--
    { 0x3fdbf0a9, 0x3e70bc97, 0}, // (e-1)
    { 0x3fdbf0aa, 0x3e70bc99, 0}, // (e-1)++
    { 0x3fdbf0ab, 0x3e70bc9b, 0},

};

static libm_test_special_data_f64
test_log10_conformance_data[] = {
    /*Constant inputs from the Constants.h*/
    {POS_SNAN_F64,POS_QNAN_F64,FE_INVALID}, // invalid
    {POS_SNAN_F64,POS_QNAN_F64,0}, //
    {POS_QNAN_F64,POS_QNAN_F64,0}, //
    {POS_INF_F64,POS_INF_F64,0}, //
    {NEG_INF_F64,NEG_QNAN_F64,FE_INVALID}, //
    {POS_ONE_F64,0x0000000000000000LL,0}, //
    {NEG_ONE_F64,NEG_INF_F64,0}, //
    {NEG_ZERO_F64,NEG_ZERO_F64,0}, //
    {POS_ZERO_F64,POS_ZERO_F64,0}, //
    {POS_PI_F64,0x3fdfd14db31ba3bbLL,0}, //
    {NEG_PI_F64,0xfff8000000000000LL,FE_INVALID}, //

    /*Other Input */
    { 0x0LL, 0x0LL, 0},//25
    { 0x3ee0624dd2f1a9fcLL, 0xc0146dc87345c560LL, 0},
    { 0x3ef0624dc31c6ca1LL, 0xc01339873fe9907aLL, 0},
    { 0x3fe02467be553ac5LL, 0xbfd3051cc98fdeceLL, 0},
    { 0x3ff0000000000000LL, 0x0000000000000000LL, 0},
    { 0x4000000000000000LL, 0x3fd34413509f79ffLL, 0},
    { 0x3d3c25c268497682LL, 0xc02a000000000000LL, 0},
    { 0x400921fb60000000LL, 0x3fdfd14dc015a244LL, 0},
    { 0x401921fb54442d18LL, 0x3fe98ab081dd8eddLL, 0},
    { 0x4024000000000000LL, 0x3ff0000000000000LL, 0},
    { 0x408f400000000000LL, 0x4008000000000000LL, 0},
    { 0x4060000000000000LL, 0x4000db90e68b8abfLL, 0},
    { 0x4086240000000000LL, 0x4006cd7efb4f5f1bLL, 0},
    { 0x4086280000000000LL, 0x4006ce1f9d049d47LL, 0},
    { 0x408c000000000000LL, 0x40079e53a969bdcfLL, 0},
    { 0xc086240000000000LL, 0xfff8000000000000LL, FE_INVALID},
    { 0xc089600000000000LL, 0xfff8000000000000LL, FE_INVALID},
    { 0xc070000000000000LL, 0xfff8000000000000LL, FE_INVALID},
    { 0xc086232bdd7abcd2LL, 0xfff8000000000000LL, FE_INVALID},
    { 0xc086232bdd7abcd3LL, 0xfff8000000000000LL, FE_INVALID},
    { 0xc087480000000000LL, 0xfff8000000000000LL, FE_INVALID},
    { 0x40862e42fefa39eeLL, 0x4006cf1ad7ce0276LL, 0},
    { 0x40862e42fefa39efLL, 0x4006cf1ad7ce0276LL, 0},
    { 0x40862e42fefa39ffLL, 0x4006cf1ad7ce0279LL, 0},
    { 0x4200000000000000LL, 0x4023de33eb2475cfLL, 0},
    { 0xc05021eb851eb852LL, 0xfff8000000000000LL, FE_INVALID},
    { 0xc0ff5d35b020c49cLL, 0xfff8000000000000LL, FE_INVALID},
    { 0xbfd3333333333333LL, 0xbfd6d3c324e13f4eLL, 0},
    { 0xbfe48e410b630a91LL, 0xbff073ae445440c6LL, 0},
    { 0xbfe0000000000000LL, 0xbfe62e42fefa39efLL, 0},
    { 0xbfe0000000000000LL, 0xbfe62e42fefa39efLL, 0},
    { 0x3fd3333333333333LL, 0xbfe0bb6c34d81502LL, 0},
    { 0x3fe48e410b630a91LL, 0xbfc89a9a8b4932cfLL, 0},
    { 0x3fe0000000000000LL, 0xbfd34413509f79ffLL, 0},
    { 0x405021eb851eb852LL, 0x3ffcf4c8a5d308a1LL, 0},
    { 0x40ff5d35b020c49cLL, 0x40146f676029d0f9LL, 0},
    { 0xc08f400000000000LL, 0xfff8000000000000LL, FE_INVALID},
    { 0x408f400000000000LL, 0x4008000000000000LL, 0},
};

