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

#include <quadmath.h>                   /* for __float128 */

/* LIBM test specific includes */
#include <libm_test_macros.h>
#include <list.h>
#include <debug.h>

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

#define TEST_SINGLE_PRECISION_MASK              \
    (LIBM_FUNC_S_S | LIBM_FUNC_V2S | LIBM_FUNC_V4S | LIBM_FUNC_V8S)


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
    TEST_TYPE_MAX,
};

typedef union {
    float (*func1)(float);
    float (*func2)(float, float);
    float (*func3)(float, float, float);
} libm_func_32;

typedef union {
    double (*func1)(double);
    double (*func2)(double, double);
    double (*func3)(double, double, double);
} libm_func_64;

struct libm_test;

struct libm_test_ops {
    int (*setup)(struct libm_test *test);
    int (*run)(struct libm_test *test);
    int (*cleanup)(struct libm_test *test);

    union {
        double     (*func)(struct libm_test *test, int idx);
        __float128 (*funcq)(struct libm_test *test, int idx);
    } ulp;

    int (*libm_func_callback)(struct libm_test *test, int idx);

    int (*verify)(struct libm_test *test, struct libm_test_result *result);
};


#if 0
#define CAT(A, B) CAT2(A, B)
#define CAT2(A, B) A ## B

#define call_func(func, ...)                                    \
    {                                                           \
        CAT(func, COUNT_PARMS(__VA_ARGS__))(__VA_ARGS__);       \
    }

#define call_func(func, t, ...)                 \
    {                                           \
        if (sizeof(t) == sizeof(float))         \
            call_func_float(func, __VA_ARGS__); \
        else if (sizeof(t) == sizeof(double))
    }
#endif

struct libm_test {
    char                    *name;
    char                    *type_name;
    uint32_t                 variant;

    double                   max_ulp_err;           /* ULP error */

    uint8_t                  nargs;      /* number of arguments for this func */

    struct libm_test_conf   *conf;             /* will be allocated elsewhere */
    struct libm_test_ops     ops;
    struct libm_test_data    test_data;         /* will be allocated by test */
    struct libm_test_result  result;

    double                   ulp_threshold;
    void                    *private;        /* data that the test needs back */

    void                    *private;        /* data that the test needs back */

    struct list_head         list;
};

enum libm_test_range_type {
      LIBM_INPUT_RANGE_SIMPLE,
      LIBM_INPUT_RANGE_RANDOM,
      LIBM_INPUT_RANGE_LINEAR,
};

struct libm_test_input_range {
    long double     start;
    long double     stop;
    enum libm_test_range_type type;
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

static inline int test_is_single_precision(struct libm_test *test)
{
    return test->variant & TEST_SINGLE_PRECISION_MASK;
}

int libm_test_init(struct libm_test_conf *conf);
int libm_test_register(struct libm_test *test);

/*
 * returns -1 if success,
 * returns offset in array where the mismatch occurs
 */
int libm_test_verify(struct libm_test *test,
		     struct libm_test_result *result);

int libm_test_populate_range_linear(void *data,
                                    size_t nelem, uint32_t variant,
                                    double min, double max);

int libm_test_populate_range_uniform(void *data,
                                     size_t nelem, uint32_t variant,
                                     double min, double max);

int libm_test_populate_range_rand(void *data,
                                     size_t nelem, uint32_t variant,
                                     double min, double max);

int libm_test_get_data_size(uint32_t variant);
const char *libm_test_variant_str(uint32_t variant);

struct libm_test *
libm_test_alloc_init(struct libm_test_conf *conf, struct libm_test *template);
int libm_test_free(struct libm_test *test);
int libm_test_alloc_test_data(struct libm_test *test, uint32_t nelem);

/**********************************
 * ULP error calculations
 **********************************/
double libm_test_ulp_errorf(float computed, double expected);
double libm_test_ulp_error(double computed, __float128 expected);

#endif  /* __LIBM_TESTS_H__ */
