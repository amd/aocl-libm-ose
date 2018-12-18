/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

#include <string.h>                     /* for memcpy() */
#include <stdlib.h>
#include <math.h>                       /* for exp2() poisining */

#include <libm_amd.h>
#include <libm_tests.h>
#include <bench_timer.h>

#include "test_exp2.h"

double FN_PROTOTYPE(exp2_prems)(double);

static int test_exp2_perf(struct libm_test *test)
{
    struct libm_test_data *data = test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = data->output;
    double *restrict ip1 = data->input1;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;
    double (*fn1)(double) = test->libm_func.func_64.func1;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = exp2(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        for (uint32_t j = 0; j < sz; j ++) {
            o[j] = fn1(ip1[j]);
            //o[j] = FN_PROTOTYPE(exp2_prems)(ip1[j]);
            //o[j] = exp2(ip1[j]);
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

static int test_exp2_setup_s_d(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    return test_exp2_alloc_init(conf,  test);
}

static int test_exp2_init_s_d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_type = conf->test_types;

    while (test_type) {
        struct libm_test *test = libm_test_alloc_init(conf, NULL);

        if (!test)
            return -1;

        test->variant = LIBM_FUNC_S_D;
        test->name = "exp2_scalar";
        test->nargs = 1;
        test->libm_func.func_64.func1 = FN_PROTOTYPE(exp2_prems);

        uint32_t bit = 1 << (ffs(test_type) - 1);

        switch (bit) {
        case TEST_TYPE_PERF:
            test->type_name = "perf";
            test->ops.setup = test_exp2_setup_s_d;
            test->ops.run = test_exp2_perf;
            test->ops.verify = libm_test_exp2_verify;
            break;
        default:
            LIBM_TEST_DPRINTF(INFO, "Unknown test type for scalar\n");
            goto skip_this;
        }

        ret = test_exp2_register_one(test);

        if (ret)
            goto out;
skip_this:
        test_type &= (test_type - 1);
    }

out:
    return ret;

}


static int test_exp2_init_s_s(struct libm_test_conf *conf)
{
    return 0;
}

int test_exp2_init_scalar(struct libm_test_conf *conf)
{
    int ret =0;

    if (conf->variants & LIBM_FUNC_S_S) {
        ret = test_exp2_init_s_s(conf);
        if (ret)
            goto out;
    }

    if (conf->variants & LIBM_FUNC_S_D) {
        ret = test_exp2_init_s_d(conf);
        if (ret)
            goto out;
    }

 out:
    return ret;
}
