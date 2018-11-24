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
#include <math.h>
#include <string.h>		/* for memcpy() */

#include <immintrin.h>

#include <libm_amd.h>

#include <libm_tests.h>
#include <bench_timer.h>
#define __TEST_EXP_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp2_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);

#if 0
__attribute__((optimize("tree-vectorize")))
void test_exp_scalar(test_conf_t *conf, test_report_t *report,
                      void *restrict input, void *restrict output)
{
    double *restrict o = output;
    double *restrict ip1 = input;
    int n = conf->niter;
    int sz = conf->sz;

    for (int j = 0; j < sz; ++j)
        o[j] = exp(ip1[j]);

    bench_timer_t bt;
    timer_start(&bt);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < sz; ++j) {
            o[j] = exp(ip1[j]);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);
    report->mops = sec2mps(s, n * sz);
}
#endif

static int test_exp_vrd4_perf(struct libm_test *test)
{
    struct libm_test_data *data = test->data;
    struct libm_test_result *result = test->result;
    double *restrict o = GCC_ALIGN(data->output, 32);
    double *restrict ip1 = GCC_ALIGN(data->input1, 32);
    uint32_t sz = data->nelem;
    uint32_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j)
        o[j] = exp(ip1[j]);

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        IVDEP //;
            for (uint32_t j = 0; j < (sz - 3); j += 4) {
                __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
                __m256d op4 = amd_vrd4_exp2(ip4);
                _mm256_store_pd(&o[j], op4);
            }
        /*
         * For now we are not handling the case of 'j'
         * not being multiple of 2/4/8
         */
    }

    timer_stop(&bt);
    double s = timer_span(&bt);
    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static int test_exp_vrd4_other(struct libm_test *test)
{
    struct libm_test_data *data = test->data;
    double *ip = &data->input1[0];
    double *op = &data->output[0];
    int sz = data->nelem;

    if (sz % 4 != 0)
        printf("%s%s : Size is not a multiple of 4, some entries may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name);

    for (int j = 0; j < (sz - 3); j += 4) {
        __m256d ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
        __m256d op4 = amd_vrd4_exp2(ip4);
        _mm256_store_pd(&op[j], op4);
    }

    return 0;
}

int libm_test_generic_ulp(struct libm_test *test)
{
    return 0;
}

int libm_test_generic_verify(struct libm_test *test, struct libm_test_result *result)
{
    return libm_test_verify_dbl(test, result);
}

/* vector single precision */
struct libm_test exp_test_template = {
    .name = "exp_vec",
    .ops = {
        .ulp = libm_test_generic_ulp,
        .verify = libm_test_generic_verify,
    },
};

static int test_exp_alloc_init_perf_data(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data;

    test->test_data = malloc(sizeof(*test->test_data) +
                             sizeof(test->data) * conf->nelem * 3);

    test_data = test->test_data;
    test_data->nelem = conf->nelem;
    test_data->input1 = &test_data->data[0];
    test_data->output = &test_data->data[conf->nelem];

    if (!test->test_data)
        goto out;

    return 0;

 out:
    return -1;
}

int test_exp_register_one(struct libm_test *test)
{
    int ret = 0;

    ret = libm_test_register(test);

    if (ret)
        goto out;
 out:
    return ret;

}
static int test_exp_vrd4_other_setup(struct libm_test *test)
{
    // Just trying to get rid of warning/errors
    return (int)libm_test_exp_special_data[0].in; 
}


static int test_exp_init_v4d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_types = conf->test_types;

    while(test_types) {
        struct libm_test *exp_v4d = malloc(sizeof(struct libm_test));
        if (!exp_v4d)
            return -1;

        memcpy(exp_v4d, &exp_test_template, sizeof(*exp_v4d));

        exp_v4d->variant |= LIBM_FUNC_V4D;
        exp_v4d->conf = conf;

        uint32_t bit = 1 << ffs(conf->test_types);
        switch(bit) {
        case TEST_TYPE_PERF:
            exp_v4d->ops.run = test_exp_vrd4_perf;
            test_exp_alloc_init_perf_data(exp_v4d);
            break;
        case TEST_TYPE_SPECIAL:
            //exp_v4d->test_data = libm_test_exp_special_data;
            exp_v4d->ops.run = test_exp_vrd4_other;
            exp_v4d->ops.setup = test_exp_vrd4_other_setup;
            break;
        case TEST_TYPE_ACCU:
            //exp_v4d->test_data = libm_test_exp_accu_data;
            exp_v4d->ops.run = test_exp_vrd4_other;
            break;
        case TEST_TYPE_CORNER:
            //exp_v4d->test_data  = libm_test_exp_corner_data;
            exp_v4d->ops.run = test_exp_vrd4_other;
            break;
        }

        test_types = test_types & (test_types -  1);
        ret = test_exp_register_one(exp_v4d);

        if (ret)
            return -1;
    }

    return 0;
}

int libm_test_init(struct libm_test_conf *conf)
{
    int ret = 0;

    printf("%s called\n", __func__);

    if (conf->variants & LIBM_FUNC_V4D) {
        ret = test_exp_init_v4d(conf);
        if (ret)
            goto out;
    }

#if 0
    if (conf->variants & LIBM_FUNC_S_S) {
        ret = test_init_s_s(conf);
        if (ret)
            goto out;
    }

    if (conf->variants & LIBM_FUNC_S_D) {
        ret = libm_test_register(exp_test);
        if (ret)
            exit();
    }
#endif

 out:
    return ret;
}

