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

static const struct {
    double_t ALIGN(16) poly[LOG_POLY_DEGREE];
    double_t ALIGN(16) poly1[LOG_POLY_DEGREE];
    struct {
        double_t head, tail;
    } ln2;
    struct {
        double_t head, tail;
    } table[(1<<LOG_N) + 1];

    double f_inv[(1<<LOG_N) + 1];
} log_data = {
    .ln2 = {0x1.62e42fefa0000p-1, 0x1.cf79abc9e3b3ap-40 },
    //.ln2 = {0x1.62e42e0000000p-1, 0x1.efa39ef35793cp-25}, /* ln(2) split  */

    .poly = {
        -0x1.0000000000000p-1,
        0x1.5555555555555p-2,
        -0x1.fffffffffffffp-3,
        0x1.999999999999ap-3,
        -0x1.5555555555555p-3,
    },
    .poly1 = {
        0x1.55555555554e6p-4, // 1/2^2 * 3
        0x1.9999999bac6d4p-7, // 1/2^4 * 5
        0x1.2492307f1519fp-9, // 1/2^6 * 7
        0x1.c8034c85dfff0p-12 // 1/2^8 * 9
    },
    .table = {
	    /*
	     * For double-precision, N = 39
	     * head := 2^-N * round_to_nearest_int(2^N * log(1 + j * 2^-7)).
	     * tail := log(1 + j * 2^-7) - head)
	     */
#if LOG_N == 7
  #include "data/_log_tbl_128.data"
#elif LOG_N == 8
  #include "data/_log_tbl_256.data"
#else
  #error "LOG_N not defined, not sure which table to use"
#endif
    },

    .f_inv = {
#if LOG_N == 7
#include "data/_log_tbl_128_f_inv.data"
#elif LOG_N == 8
#include "data/_log_tbl_256_f_inv.data"
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
#define LN2_HEAD  log_data.ln2.head
#define LN2_TAIL  log_data.ln2.tail
#define LOG_TAB   log_data.table
#define LOG_TAB_F_INV log_data.f_inv


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
