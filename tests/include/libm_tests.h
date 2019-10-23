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
    void  *input1;
    void  *input2;
    void  *input3;
    void  *output;
    void  *expected;
    int   *expected_exception;
    int   *raised_exception;

    //double  *reserved[27];
    //double  data[0];
};

enum LIBM_TEST_TYPE {
    TEST_TYPE_ACCU = (1<<0),
    TEST_TYPE_PERF = (1<<1),
    TEST_TYPE_SPECIAL = (1<<2),
    TEST_TYPE_CORNER = (1<<3),
    TEST_TYPE_CONFORMANCE = (1<<4),
    TEST_TYPE_MAX,
};

struct libm_test;

typedef int (*libm_func_cb_t)(struct libm_test *test, int idx);

struct libm_test_ops {
    int (*setup)(struct libm_test *test);
    int (*run)(struct libm_test *test);
    int (*cleanup)(struct libm_test *test);

    union {
        double      (*func)(struct libm_test *test, int idx);
        long double (*funcl)(struct libm_test *test, int idx);
        __float128  (*funcq)(struct libm_test *test, int idx);
    } ulp;

    struct {
        libm_func_cb_t s1s, s1d, v2s, v4s, v8s, v2d, v4d, verify, accu_ranges;
    } callbacks;

    int (*verify)(struct libm_test *test, struct libm_test_result *result);
};

struct libm_test_funcs {
    struct libm_test_ops performance;
    struct libm_test_ops accuracy;
    struct libm_test_ops special;
    struct libm_test_ops corner;
    struct libm_test_ops conformance;
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
    char                    *name;      /* Scalar/vector */
    char                    *type_name; /* perf/accu/special/corner */
    char                    *input_name; /* s1f,s1d,v2s,v4d etc */

    enum LIBM_TEST_TYPE      test_type; /* derives from conf->test_types */
    enum LIBM_FUNC_VARIANT   variant;

    double                   max_ulp_err; /* ULP error */

    uint8_t                  nargs;     /* number of arguments for this func */

    struct libm_test_conf   *conf;      /* will be allocated elsewhere */
    struct libm_test_ops     ops;
    struct libm_test_data    test_data; /* will be allocated by test */
    struct libm_test_result  result;

    double                   ulp_threshold;

    void                    *private;        /* data that the test needs back */

    struct list_head         list;
};

static inline const char *test_get_name(struct libm_test *test)
{
    return test->name;
}

static inline const char *test_get_test_type(struct libm_test *test)
{
    return test->type_name;
}

static inline const char *test_get_input_type(struct libm_test *test)
{
    return test->input_name;
}

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
int libm_tests_setup(struct libm_test_conf *conf,
                     struct libm_test_funcs test_table[LIBM_FUNC_MAX],
                     struct libm_test *template);
int libm_test_populate_inputs(struct libm_test *test, int use_uniform);

/*
 * returns 0 if success, -1 otherwise
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

struct libm_test *libm_test_alloc_init(struct libm_test_conf *conf,
                                       struct libm_test *template);
int libm_test_free(struct libm_test *test);
int libm_test_alloc_test_data(struct libm_test *test, uint32_t nelem);

/* Generic perf measurement functions */
int libm_test_s1s_perf(struct libm_test *test);
int libm_test_s1d_perf(struct libm_test *test);
int libm_test_v4s_perf(struct libm_test *test);
int libm_test_v2d_perf(struct libm_test *test);
int libm_test_v4d_perf(struct libm_test *test);

/* Generic accuracy measurement functions */
int libm_test_accu(struct libm_test *test);
int libm_test_accu_double(struct libm_test *test, uint32_t type);
int libm_test_accu_single(struct libm_test *test, uint32_t type);

/**********************************
 * ULP error calculations
 **********************************/
double libm_test_ulp_errorf(float computed, double expected);
double libm_test_ulp_error(double computed, long double expected);

/******special functions**********/
int libm_test_s1s_conf(struct libm_test *test);
int libm_test_s1d_conf(struct libm_test *test);

/*
 * Use directly the FMA3 version or glibc version
 */

#define PROTOTYPE_GLIBC    0xf1
#define PROTOTYPE_AOCL     0xf2
#define PROTOTYPE_TEST_V1  0xf8
#define PROTOTYPE_TEST_V2  0xf9

#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
#define LIBM_FUNC(x) FN_PROTOTYPE(x)
#define LIBM_FUNC_VEC(prec, elem, fn) FN_PROTOTYPE(vr##prec##elem##_##fn)
#pragma message "compilig for AOCL"
#elif (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#pragma message "compilig for GLIBC"
#define LIBM_FUNC(x)    x
#define LIBM_FUNC_VEC(prec, elem, fn) _ZGV##prec##N##elem##v_##fn
#else
#error "unknown LIBM_PROTOTYPE"
#endif


#endif  /* __LIBM_TESTS_H__ */
