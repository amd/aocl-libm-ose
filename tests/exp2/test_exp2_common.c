/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 *
 * Tests for exp2()
 *
 */

#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <quadmath.h>
#include <strings.h>                            /* for ffs() */

#include <immintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>

char doc[] = BUILD_TEST_DOC(TEST_NAME);

int libm_test_exp2_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    double *expected = data->expected;
    double *input1 = data->input1;

    /*
     * Call the glibc's exp2() to get IEEE754 compliant values
     */
    for (uint32_t j = 0; j < data->nelem; j++) {
        expected[j] = exp2(input1[j]);
    }

    return libm_test_verify(test, result);
}


int test_exp2_populate_inputs(struct libm_test *test, int use_uniform)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_conf *conf = test->conf;
    int ret = 0;
    int (*func)(void *, size_t, uint32_t, double, double);

    if(use_uniform)
        func = libm_test_populate_range_uniform;
    else
        func = libm_test_populate_range_simple;

    ret = func(data->input1, data->nelem,
               test->variant,
               conf->inp_range[0].start,
               conf->inp_range[0].stop);

    /* Fill the same if more inputs are needed */
    if (!ret && test->nargs > 1) {
        ret = func(data->input2, data->nelem,
                   test->variant,
                   conf->inp_range[1].start,
                   conf->inp_range[1].stop);
    }

    if (!ret && test->nargs > 2) {
        ret = func(data->input3, data->nelem,
                   test->variant,
                   conf->inp_range[2].start,
                   conf->inp_range[2].stop);
    }

    return ret;
}

int test_exp2_register_one(struct libm_test *test)
{
    int ret = 0;

    ret = libm_test_register(test);

    if (ret)
        goto out;
 out:
    return ret;
}

int test_exp2_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}


