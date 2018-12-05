/*
 * Test framework for AMD math library
 *
 * Author: Prem Mallappa
 * Email : pmallapp@amd.com
 */

#ifndef __LIBM_TESTS_H__
#define __LIBM_TESTS_H__

#include <stdio.h>
#include <stdint.h>

#include <libm_test_macros.h>
#include <list.h>

#define MAX_FAILURES 10

enum LIBM_FUNC_VARIANT {
    LIBM_FUNC_S_S = (1 << 0),                   /* scalar single precision */
    LIBM_FUNC_S_D = (1 << 1),                   /* scalar double precision */
    LIBM_FUNC_V2S = (1 << 2),                   /* vector 2 elem single preci */
    LIBM_FUNC_V4S = (1 << 3),                   /*  "     4   " */
    LIBM_FUNC_V8S = (1 << 4),                   /*  "     8   " */
    LIBM_FUNC_V2D = (1 << 5),                   /* vector 2 element double preci */
    LIBM_FUNC_V4D = (1 << 6),                   /*   "    4   " */
    LIBM_FUNC_MAX,
};

struct libm_test_result{
    int    ntests;                              /* total number of tests */
    int    npass;                               /* passed tests */
    int    nignored;                            /* ignored tests */
    int    nfail;                               /* failed tests */
    double mops;                                /* Million Ops per second */
    double cpi;                                 /* Clocks per instruction */
    double input1[MAX_FAILURES];
    double input2[MAX_FAILURES];
    double input3[MAX_FAILURES];
    double output[MAX_FAILURES];
    double expected[MAX_FAILURES];
};

/*
 * This is an array of data,
 * Compiler needs to know the various input and outputs dont
 * overlap,
 *     data[0] - ensures that is one continous chunck of array
 *     input1, input2, output and expected all point to inside data[],
 *      but at various offsets.
 * eg:
 *      test->test_data = malloc(sizeof(*test->test_data) +
 *              sizeof(test->data) * conf->nelem * 3);
 *
 *      test_data->input1 = &test_data->data[0];
 *      test_data->output = &test_data->data[nelem];
 */

struct libm_test_data {
    uint32_t nelem;
  //uint32_t reserved1;
    double  *input1;
    double  *input2;
    double  *input3;
    double  *output;
    double  *expected;
  //double  *reserved[27];
    double  data[0];
};

enum {
    TEST_TYPE_ACCU = (1<<0),
    TEST_TYPE_PERF = (1<<1),
    TEST_TYPE_SPECIAL = (1<<2),
    TEST_TYPE_CORNER = (1<<3),
};

struct libm_test;

struct libm_test_ops {
    int (*setup)(struct libm_test *test);
    int (*run)(struct libm_test *test);
    int (*cleanup)(struct libm_test *test);

    int (*ulp)(struct libm_test *test);
    int (*verify)(struct libm_test *test, struct libm_test_result *result);
};

struct libm_test {
    char                    *name;
    char                    *type_name;
    uint32_t                 variant;
    double                   ulp_err;           /* ULP error */

    uint8_t                  nargs;      /* number of arguments for this func */

    struct libm_test_conf   *conf;             /* will be allocated elsewhere */
    struct libm_test_ops     ops;
    struct libm_test_data   *test_data;         /* will be allocated by test */
    struct libm_test_result  result;

    void                    *private;        /* data that the test needs back */

    struct list_head         list;
};


struct libm_test_input_range {
    long double     start;
    long double     stop;
    long double     inc;
};

struct libm_test_conf {
    uint32_t niter;
    uint32_t nelem;
    uint32_t variants;
    uint32_t test_types;                        /* specifies accu,perf,special.. */
    uint64_t coremask;
    /* Some tests have 3 inputs */
    struct libm_test_input_range inp_range[3];
};


#define LIBM_TEST_DEBUG
#ifdef LIBM_TEST_DEBUG

extern uint32_t  dbg_bits;

#define HERE()  printf("%s:%d\n", __func__, __LINE__)

enum {
    LIBM_TEST_DBG_PANIC, LIBM_TEST_DBG_CRIT, LIBM_TEST_DBG_WARN, /* error level */
    LIBM_TEST_DBG_DBG1, LIBM_TEST_DBG_DBG2, LIBM_TEST_DBG_INFO, /* info level */
};

#define DBG_BIT(bit)    (1 << LIBM_TEST_DBG_##bit)

#define IS_DBG_ENABLED(bit) (dbg_bits & (1 << LIBM_TEST_DBG_##bit))

#define DBG_DEFAULT  (DBG_BIT(PANIC) | DBG_BIT(CRIT))
#define DBG_VERBOSE1 (DBG_DEFAULT | DBG_BIT(DBG1))
#define DBG_VERBOSE2 (DBG_VERBOSE1 | DBG_BIT(DBG1))
#define DBG_VERBOSE3 (DBG_VERBOSE2 | DBG_BIT(DBG2))
#define DBG_VERBOSE4 (DBG_VERBOSE3 | DBG_BIT(INFO))

#define LIBM_TEST_DPRINTF(lvl, fmt, ...)                \
    do {                                                \
        if (dbg_bits & DBG_BIT(lvl)) {                  \
            printf("%s:%d: " fmt ,                      \
                   __FILE__, __LINE__,                  \
                   ## __VA_ARGS__);                     \
        }                                               \
    } while (0)

#else
#define IS_DBG_ENABLED(bit) false
#define LIBM_TEST_DPRINTF(lvl, fmt, ...)

#endif  /* LIBM_TEST_DEBUG */

int libm_test_init(struct libm_test_conf *conf);
int libm_test_register(struct libm_test *test);

/*
 * returns -1 if success,
 * returns offset in array where the mismatch occurs
 */
int libm_test_verify_dbl(struct libm_test *test,
                                struct libm_test_result *result);

int libm_test_verify_flt(struct libm_test *test,
				struct libm_test_data *data,
				struct libm_test_result *result);

int libm_test_populate_range_simple(void *data,
                                    size_t nelem, uint32_t variant,
                                    double min, double max);

int libm_test_populate_range_uniform(void *data,
                                     size_t nelem, uint32_t variant,
                                     double min, double max);

#endif  /* __LIBM_TESTS_H__ */
