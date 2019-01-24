/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 *
 * Tests for log()
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

/*
 * Call the glibc's log() to get IEEE754 compliant values
 */

int libm_test_log_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;

    /*
     * Call the glibc's log() to get IEEE754 compliant values
     */
    if (test_is_single_precision(test)) {
        float *expected = (float*)data->expected;
        float *input1   = (float*)data->input1;

        for (uint32_t j = 0; j < data->nelem; j++) {
            expected[j] = logf(input1[j]);
        }
    } else {
        double *expected = data->expected;
        double *input1 = data->input1;


        for (uint32_t j = 0; j < data->nelem; j++) {
            expected[j] = log(input1[j]);
        }
    }

    return libm_test_verify(test, result);
}

typedef int (*rand_func_t)(void *, size_t, uint32_t, double, double);

static rand_func_t libm_test_get_rand_func(enum libm_test_range_type type)
{
    switch(type)
    {
    case LIBM_INPUT_RANGE_LINEAR:
        return libm_test_populate_range_uniform;

    case LIBM_INPUT_RANGE_RANDOM:
        return libm_test_populate_range_rand;
    default:
        break;
    }

    //case LIBM_INPUT_RANGE_SIMPLE:
    return libm_test_populate_range_linear;
}

int test_log_populate_inputs(struct libm_test *test, int use_uniform)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_conf *conf = test->conf;
    int ret = 0;
    rand_func_t func = NULL;

    func = libm_test_get_rand_func(conf->inp_range[0].type);
    ret = func(data->input1, data->nelem,
               test->variant,
               conf->inp_range[0].start,
               conf->inp_range[0].stop);

    /* Fill the same if more inputs are needed */
    if (!ret && test->nargs > 1) {
        func = libm_test_get_rand_func(conf->inp_range[1].type);
        ret = func(data->input2, data->nelem,
                   test->variant,
                   conf->inp_range[1].start,
                   conf->inp_range[1].stop);
    }

    if (!ret && test->nargs > 2) {
        func = libm_test_get_rand_func(conf->inp_range[2].type);
        ret = func(data->input3, data->nelem,
                   test->variant,
                   conf->inp_range[2].start,
                   conf->inp_range[2].stop);
    }

    return ret;
}

int test_log_register_one(struct libm_test *test)
{
    int ret = 0;

    ret = libm_test_register(test);

    if (ret)
        goto out;
 out:
    return ret;
}

int test_log_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}


