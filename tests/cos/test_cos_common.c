/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 */

/*
 *
 * Tests for cos()
 *
 */

#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */

#include <immintrin.h>

#include <libm_amd.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include "../libs/mparith/am_mp_funcs.h"

int test_cos_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;

    /*
     * Call the MPARITH cos() to populate expected IEEE754 compliant values for accu tests
     */
    if (test->test_type == TEST_TYPE_ACCU) {
        if (test_is_single_precision(test)) {
            float *expected = (float*)data->expected;
            float *input1   = (float*)data->input1;

            for (uint32_t j = 0; j < data->nelem; j++) {
                expected[j] = alm_mp_cosf(input1[j]);
            }
        }
        else {
            double *expected = data->expected;
            double *input1 = data->input1;

            for (uint32_t j = 0; j < data->nelem; j++) {
               expected[j] = alm_mp_cos(input1[j]);
            }
        }
    }

    return libm_test_verify(test, result);
}


int test_cos_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}


