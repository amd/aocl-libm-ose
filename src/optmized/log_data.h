/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __OPTIMIZED_LOG_DATA_H__
#define __OPTIMIZED_LOG_DATA_H__

#ifndef LOG_N
#define LOG_N               7
#endif
#define LOG_TABLE_SIZE      (1ULL << N)
#define LOG_MAX_POLYDEGREE  8

/* We define default poly degree which are having ULP <= 0.5 */
#ifndef LOG_POLY_DEGREE
#if LOG_N == 7
#define LOG_POLY_DEGREE 5
#elif LOG_N == 8
#define LOG_POLY_DEGREE 5
#elif LOG_N == 9
#define LOG_POLY_DEGREE 4
#endif
#endif

struct log_table {
    double f_inv, head, tail;
};

static const struct {
    double_t ALIGN(16) poly[LOG_POLY_DEGREE];
    double_t ALIGN(16) poly1[4];
    struct {
        double head, tail;
    } ln2;
    struct log_table ALIGN(16) table[(1<<LOG_N) + 1];
    double f_inv[1<<LOG_N];
} log_data = {
    .ln2 = {0x1.62e42fefa0000p-1, 0x1.cf79abc9e3b3ap-40 },
    .poly = {
        -0x1.0000000000000p-1,          /* -1/2 */
        0x1.5555555555555p-2,           /* +1/3 */
        -0x1.0000000000000p-2,          /* -1/4 */
        0x1.999999999999ap-3,           /* +1/5 */
#if LOG_POLY_DEGREE >= 5
        -0x1.5555555555555p-3,          /* -1/6 */
#if LOG_POLY_DEGREE >= 6
        -0x1.2492492492492p-3,          /* +1/7 */
#if LOG_POLY_DEGREE >= 7
        -0x1.0000000000000p-3,          /* -1/8 */
#endif
#endif
#endif
    },
    .poly1 = {
        0x1.55555555554e6p-4,           /* 1/2^2 * 3 */
        0x1.9999999bac6d4p-7,           /* 1/2^4 * 5 */
        0x1.2492307f1519fp-9,           /* 1/2^6 * 7 */
        0x1.c8034c85dfff0p-12           /* 1/2^8 * 9 */
    },
    .table = {
	    /*
	     * For double-precision, N = 39
	     * head := 2^-N * round_to_nearest_int(2^N * log(1 + j * 2^-7)).
	     * tail := log(1 + j * 2^-7) - head)
	     */
#if LOG_N == 7
  #include "data/_log_v3_tbl_128_interleaved.data"
#elif LOG_N == 8
  #include "data/_log_v3_tbl_256_interleaved.data"
#else
  #error "LOG_N not defined, not sure which table to use"
#endif
    },
};

#define LOG_C1	  log_data.poly[0]
#define LOG_C2	  log_data.poly[1]
#define LOG_C3	  log_data.poly[2]
#define LOG_C4	  log_data.poly[3]
#define LOG_C5	  log_data.poly[4]
#define LOG_C6	  log_data.poly[5]
#define LOG_C7	  log_data.poly[6]
#define LOG_C8	  log_data.poly[7]
#define LN2_HEAD  log_data.ln2.head
#define LN2_TAIL  log_data.ln2.tail
#define LOG_TAB   log_data.table

typedef union PACKED {
    struct {
        unsigned long mantissa:52;
        unsigned long expo:11;
        unsigned long sign:1;
    } f;
    uint64_t i;
    double_t d;
} flt64_split_t;

#endif
