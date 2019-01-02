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

#include <libm_tests.h>
#include <bench_timer.h>

#define __TEST_EXP2_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp2_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);

static int test_exp2_vrd4_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = GCC_ALIGN(data->output, 256);
    double *restrict ip1 = GCC_ALIGN(data->input1, 256);
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = exp2(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        //IVDEP //;
        for (uint32_t j = 0; j < (sz - 3); j += 4) {
            __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m256d op4 = FN_PROTOTYPE_FMA3(vrd4_exp2)(ip4);
            _mm256_store_pd(&o[j], op4);
        }
        /*
         * Any left over process with scalar
         */
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static int test_exp2_vrd4_other(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip = &data->input1[0];
    double *op = &data->output[0];
    int sz = data->nelem;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < (sz - 3); j += 4) {
        __m256d ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
        __m256d op4 = FN_PROTOTYPE_FMA3(vrd4_exp2)(ip4);
        _mm256_store_pd(&op[j], op4);
    }

    return 0;
}

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

/* There is no exp2q in recent versions of gcc */
__float128
libm_test_exp2q(struct libm_test *test, double in)
{
    /* logq(2.0) */
    static __float128 ln2 = 6.9314718055994530941723212145817657508364e-01;
    return expq(ln2 * in);
}

#include <quadmath.h>

/* vector single precision */
struct libm_test exp2_test_template = {
    .name       = "exp2_vec",
    .nargs      = 1,
    .ulp_threshold = 4.0,
    .ops        = {
                   .ulp    = {.func1 = libm_test_exp2q},
                   .verify = libm_test_exp2_verify,
                   },
    .libm_func  = { .func_64 = { .func1 = exp2, }, }, /* WOHOOO */
};

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

static int test_exp2_alloc_init_perf_data(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;

    libm_test_alloc_test_data(test, conf->nelem);

    test_exp2_populate_inputs(test, 0);

    if (!&test->test_data)
        goto out;

    return 0;

 out:
    return -1;
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


/**************************
 * SPECIAL CASES TESTS
 **************************/

static int test_exp2_alloc_special_data(struct libm_test *test, size_t size)
{
    struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data;
    int ret = 0;

    libm_test_alloc_test_data(test, size);

    if (ret) {
        printf("unable to allocate\n");
        goto out;
    }

    test_data = &test->test_data;
    test_data->nelem = size;

    /* fixup conf */
    conf->nelem = size;

    return 0;

 out:
    return -1;
}


static int test_exp2_vrd4_special_setup(struct libm_test *test)
{
    /*
     * structure contains both in and out values,
     * input only array size is half of original
     */
    int test_data_size = ARRAY_SIZE(libm_test_exp2_special_data)/2;

    struct libm_test_data *data;

    // Just trying to get rid of warning/errors
    test_exp2_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    for (int i = 0; i < test_data_size; i++) {
        data->input1[i] = libm_test_exp2_special_data[i].in;
        data->expected[i] =libm_test_exp2_special_data[i].out;
    }

    return 0;
}


/**************************
 * ACCURACY TESTS
 **************************/
#include "exp2_accu_data.h"

static int test_exp2_vrd4_accu(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip = &data->input1[0];
    double *op = &data->output[0];
    int sz = data->nelem;
    int arr_sz = ARRAY_SIZE(accu_ranges);

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int i = 0; i < arr_sz ||
             (accu_ranges[i].start = 0.0 && accu_ranges[i].stop == 0.0)
             ; i++) {
        test->conf->inp_range[0] = accu_ranges[i];
        test_exp2_populate_inputs(test, 1);

        LIBM_TEST_DPRINTF(VERBOSE2,
                          "Testing for accuracy %d items in range [%Lf, %Lf]\n",
                          data->nelem,
                          accu_ranges[i].start, accu_ranges[i].stop);
        for (int j = 0; j < (sz - 3); j += 4) {
            __m256d ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            __m256d op4 = FN_PROTOTYPE_FMA3(vrd4_exp2)(ip4);
            _mm256_store_pd(&op[j], op4);
        }

        libm_test_exp2_verify(test, &test->result);
    }

    return 0;
}

int test_exp2_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}

static int test_exp2_vrd4_accu_setup(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    return test_exp2_alloc_init(conf,  test);
}

static int test_exp2_init_v2d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_types = conf->test_types;

    while(test_types) {
        struct libm_test *exp2_v2d = libm_test_alloc_init(conf,
                                                          &exp2_test_template);
        uint32_t bit = 1 << (ffs(test_types) - 1);

        exp2_v2d->variant |= LIBM_FUNC_V2D;

        switch(bit) {
        case TEST_TYPE_PERF:
            exp2_v2d->type_name = "perf";
            exp2_v2d->ops.run = test_exp2_vrd4_perf;
            test_exp2_alloc_init_perf_data(exp2_v2d);
            break;
        case TEST_TYPE_SPECIAL:
            exp2_v2d->type_name = "special";
            exp2_v2d->ops.run = test_exp2_vrd4_other;
            exp2_v2d->ops.setup = test_exp2_vrd4_special_setup;
            break;
        case TEST_TYPE_ACCU:
            exp2_v2d->type_name = "accuracy";
            //exp2_v2d->test_data = libm_test_exp2_accu_data;
            exp2_v2d->ops.setup = test_exp2_vrd4_accu_setup;
            exp2_v2d->ops.run = test_exp2_vrd4_accu;
            exp2_v2d->ops.verify = NULL; // No verify after, will verify inside.
            break;
        case TEST_TYPE_CORNER:
        default:
            goto skip_this;
        }

        ret = test_exp2_register_one(exp2_v2d);
        if (ret)
            return -1;

skip_this:
        test_types = test_types & (test_types -  1);
    }

    return 0;
}

static int test_exp2_init_v4d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_types = conf->test_types;

    while(test_types) {
        struct libm_test *exp2_v4d = libm_test_alloc_init(conf,
                                                          &exp2_test_template);
        uint32_t bit = 1 << (ffs(test_types) - 1);

        exp2_v4d->variant |= LIBM_FUNC_V4D;

        switch(bit) {
        case TEST_TYPE_PERF:
            exp2_v4d->type_name = "perf";
            exp2_v4d->ops.run = test_exp2_vrd4_perf;
            test_exp2_alloc_init_perf_data(exp2_v4d);
            break;
        case TEST_TYPE_SPECIAL:
            exp2_v4d->type_name = "special";
            exp2_v4d->ops.run = test_exp2_vrd4_other;
            exp2_v4d->ops.setup = test_exp2_vrd4_special_setup;
            break;
        case TEST_TYPE_ACCU:
            exp2_v4d->type_name = "accuracy";
            exp2_v4d->ops.setup = test_exp2_vrd4_accu_setup;
            exp2_v4d->ops.run = test_exp2_vrd4_accu;
            exp2_v4d->ops.verify = NULL; // No verify after, will verify inside.
            break;
        case TEST_TYPE_CORNER:
            exp2_v4d->type_name = "corner";
            //exp2_v4d->test_data  = libm_test_exp2_corner_data;
            exp2_v4d->ops.run = test_exp2_vrd4_other;
            break;
        }

        if (ret)
            goto out;

        test_types = test_types & (test_types -  1);
        ret = test_exp2_register_one(exp2_v4d);

        if (ret)
            goto out;
    }

    return 0;

 out:
    return -1;
}

#define EXP2_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

int test_exp2_init_scalar(struct libm_test_conf *conf);


int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;

    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;

    if (!conf->test_types)
        conf->test_types = EXP2_TEST_TYPES_ALL;

    if ((conf->test_types & TEST_TYPE_ACCU) &&
        !conf->inp_range[0].start) {
        /* We dont have ranges */
        conf->inp_range[0].start = DBL_MIN;
        conf->inp_range[0].stop  = DBL_MAX;
    }

    if (conf->variants & LIBM_FUNC_V2D) {
        ret = test_exp2_init_v2d(conf);
        if (ret) {
            printf("registering test failed\n");
            goto out;
        }
    }

    if (conf->variants & LIBM_FUNC_V4D) {
        ret = test_exp2_init_v4d(conf);
        if (ret)
            goto out;
    }

    ret = test_exp2_init_scalar(conf);          /* in other file */

out:
    return ret;
}
