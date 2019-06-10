/*
 * Copyright (C) 2019, AMD Inc.
 *
 *
 * Author: Prem Mallappa<pmallapp@amd.com>
 */

#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */
#include <string.h>                             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>
#include <libm/compiler.h>

/****************************
 * PERF tests
 ****************************/
int
libm_test_s1s_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
    double *restrict o = data->output;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        ops->callbacks.s1s(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        for (uint32_t j = 0; j < sz; j++) {
            o[j] = ops->callbacks.s1s(test, j);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

int
libm_test_s1d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
    double *restrict o = data->output;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        ops->callbacks.s1d(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        for (uint32_t j = 0; j < sz; j++) {
            o[j] = ops->callbacks.s1d(test, j);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

int
libm_test_v4s_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;
    int ret = 0;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1s(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/4; j ++) {
            ret = ops->callbacks.v4s(test, j);
            if (ret) { // something went wrong
                goto out;
            }
        }

        j = j * 4;
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        switch (sz - j) {
        case 3:
            ret = ops->callbacks.s1s(test, j++); if (ret) goto out; FALLTHROUGH;
        case 2:
            ret = ops->callbacks.s1s(test, j++); if (ret) goto out; FALLTHROUGH;
        case 1:
            ret = ops->callbacks.s1s(test, j++);
        default:
            break;
        }
    }

 out:
    if (ret) {
        ///something went wrong what????
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return 0;
}

int
libm_test_v2d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;
    int ret = 0;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1s(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/4; j ++) {
            ret = ops->callbacks.v2d(test, j);
            if (ret) { // something went wrong
                goto out;
            }
        }

        j = j * 4;
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        if (sz - j) {
            ret = ops->callbacks.s1d(test, j++); if (ret) goto out;
        }
    }

 out:
    if (ret) {
        ///something went wrong what????
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

int
libm_test_v4d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;
    int ret = 0;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1d(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/4; j ++) {
            ret = ops->callbacks.v4d(test, j);
            if (ret) { // something went wrong
                goto out;
            }
        }

        j = j * 4;
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        switch (sz - j) {
        case 3:
            ret = ops->callbacks.s1d(test, j++); if (ret) goto out; FALLTHROUGH;
        case 2:
            ret = ops->callbacks.s1d(test, j++); if (ret) goto out; FALLTHROUGH;
        case 1:
            ret = ops->callbacks.s1d(test, j++);
        default:
            break;
        }
    }

 out:
    if (ret) {
        ///something went wrong what????
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}
