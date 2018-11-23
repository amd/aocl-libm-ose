/*
 * Test framework for AMD math library
 *
 * Author: Prem Mallappa
 * Email : pmallapp@amd.com
 */

#ifndef __LIBM_TESTS_H__
#define __LIBM_TESTS_H__

#include <stdint.h>

#include <libm_test_macros.h>
#include <list.h>

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

#define MAX_FAILURES 10
struct libm_test_data {
    uint32_t nelem;
    double  *input1;
    double  *input2;
    double  *output;
    double  *expected;
    double   data[0];
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

    int (*accuracy)(struct libm_test *test);
    int (*test_perf)(struct libm_test *test, struct libm_test_result *result);
    int (*special)(struct libm_test *test);
    int (*corner)(struct libm_test *test);

    int (*ulp)(struct libm_test *test);
    int (*verify)(struct libm_test *test, struct libm_test_result *result);
};


struct libm_test {
    char                    *name;
    char                    *type_name;
    uint32_t                 variant;
    double                   ulp_err;           /* ULP error */
    const
    struct libm_test_conf   *conf;
    struct libm_test_ops    ops;
    struct libm_test_data   *test_data;
    struct libm_test_result *result;
    void                    *data;              /* data that the test needs back */
    struct list_head         list;
};


struct libm_test_input_range {
    long double     start;
    long double     stop;
    long double     inc;
};

struct libm_test_conf {
    uint32_t        niter;
    uint32_t        nelem;
    uint32_t        variants;
    uint32_t        test_types;                 /* specifies accu,perf,special.. */

    struct libm_test_input_range inp_range[3];  /* Some tests have 3 inputs */
};


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

#endif  /* __LIBM_TESTS_H__ */
