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

#include <fenv.h>

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

/*
 * Use directly the FMA3 version or glibc version,
 * For proper execution following values needs to be generated by
 * build system
 */

#define PROTOTYPE_GLIBC    0xf1
#define PROTOTYPE_AOCL     0xf2
#define PROTOTYPE_TEST_V1  0xf8
#define PROTOTYPE_TEST_V2  0xf9
#define PROTOTYPE_SVML     0xf4

#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
#pragma message "compiling for AOCL"
#define LIBM_FUNC(x) FN_PROTOTYPE(x)
#define LIBM_FUNC_VEC(prec, elem, fn) FN_PROTOTYPE(vr##prec##elem##_##fn)

#elif (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#pragma message "compiling for GLIBC"
#define LIBM_FUNC(x)    x
#define LIBM_FUNC_VEC(prec, elem, fn) _ZGV##prec##N##elem##v_##fn

#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
#pragma message "compiling for Intel SVML"
#define LIBM_FUNC(x) x
#define LIBM_FUNC_VEC(prec, elem, fn) __svml_##fn##elem

#else
#error "unknown LIBM_PROTOTYPE"
#endif

#endif  /* __LIBM_TESTS_H__ */

