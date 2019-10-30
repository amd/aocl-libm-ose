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
#include <strings.h>                            /* for ffs() */

#include <immintrin.h>

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>

#include <libm_tests.h>
#include <bench_timer.h>

/*
 * Call the glibc's pow() to get IEEE754 compliant values
 */

int test_pow_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;

    /*
     * Call the glibc's log() to get IEEE754 compliant values
     */
    if (test_is_single_precision(test)) {
        float *expected = (float*)data->expected;
        float *input1   = (float*)data->input1;
        float *input2 = (float*)data->input2;
        for (uint32_t j = 0; j < data->nelem; j++) {
            expected[j] = powf(input1[j], input2[j]);
        }
    } else {
        double *expected = data->expected;
        double *input1 = data->input1;
        double *input2 = data->input2;
        for (uint32_t j = 0; j < data->nelem; j++) {
            expected[j] = pow(input1[j], input2[j]);
        }
    }

    return libm_test_verify(test, result);
}

int test_pow_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}



