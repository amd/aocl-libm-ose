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

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>
#include <libm/compiler.h>

#include <fenv.h>

/****************************
 * PERF tests
 ****************************/
int
libm_test_s1s_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
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
            ops->callbacks.s1s(test, j);
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
            ops->callbacks.s1d(test, j);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static inline void
NO_OPTIMIZE
__libm_test_v2d_overhead(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip1 = (double*)data->input1;
    uint64_t sz = data->nelem;
    double o[4] ALIGN(32);

    /* TODO: This works for 1 input load and 1 output store
     * in case of multiple inputs this needs to be fixed
     */

    for (uint64_t i = 0; i < sz; i++) {
        __m128d ip2 = _mm_set_pd(ip1[1], ip1[0]);
        _mm_store_pd(&o[0], ip2);
    }

    /* To shutup compiler for set-but-not used error */
    o[1] = o[0];
}


static inline void
NO_OPTIMIZE
__libm_test_v4d_overhead(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip1 = (double*)data->input1;
    uint64_t sz = data->nelem;
    double o[4] ALIGN(32);

    /* TODO: This works for 1 input load and 1 output store
     * in case of multiple inputs this needs to be fixed
     */

    for (uint64_t i = 0; i < sz; i++) {
        __m256d ip4 = _mm256_set_pd(ip1[3], ip1[2], ip1[1], ip1[0]);
        _mm256_store_pd(&o[0], ip4);
    }

    /* To shutup compiler for set-but-not used error */
    o[1] = o[0];
}

static inline void
NO_OPTIMIZE
__libm_test_v4s_overhead(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    float *ip1 = (float*)data->input1;
    uint64_t sz = data->nelem;
    float o[4] ALIGN(16);

    /* TODO: This works for 1 input load and 1 output store
     * in case of multiple inputs this needs to be fixed
     */

    for (uint64_t i = 0; i < sz; i++) {
        __m128 ip4 = _mm_set_ps(ip1[3], ip1[2], ip1[1], ip1[0]);
        _mm_store_ps(&o[0], ip4);
    }

    /* To shutup compiler for set-but-not used error */
    o[1] = o[0];
}

/*
 * v8s - overhead
 */
static inline void
NO_OPTIMIZE
__libm_test_v8s_overhead(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    float *ip1 = (float*)data->input1;
    uint64_t sz = data->nelem;
    float o[8] ALIGN(32);

    /* TODO: This works for 1 input load and 1 output store
     * in case of multiple inputs this needs to be fixed
     */

    for (uint64_t i = 0; i < sz; i++) {
        __m256 ip8 = _mm256_set_ps(ip1[7], ip1[6], ip1[5], ip1[4], ip1[3], ip1[2], ip1[1], ip1[0]);
        _mm256_store_ps(&o[0], ip8);
    }

    /* To shutup compiler for set-but-not used error */
    o[1] = o[0];
}



/* Measure time taken to load and store */
static double
NO_OPTIMIZE
libm_test_overhead(struct libm_test *test)
{

    static void (*funcptr)(struct libm_test *test) = NULL;

    switch (test->variant) {
    case LIBM_FUNC_V4S: funcptr = __libm_test_v4s_overhead; break;
    case LIBM_FUNC_V8S: funcptr = __libm_test_v8s_overhead; break;
    case LIBM_FUNC_V2D: funcptr = __libm_test_v2d_overhead; break;
    case LIBM_FUNC_V4D: funcptr = __libm_test_v4d_overhead; break;
    default: break;
    }


    bench_timer_t bt;
    timer_start(&bt);
    funcptr(test);
    timer_stop(&bt);

    double s2 = timer_span(&bt);

    return s2;
}

int
libm_test_v8s_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    struct libm_test_ops *ops = &test->ops;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;
    int ret = 0;
    const int scale = 8;

    result->mops = 0;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1s(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/scale; j ++) {
            ret = ops->callbacks.v8s(test, j*scale);
            if (ret) { // something went wrong
                goto out;
            }
        }

        j = j * scale;
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        switch (sz - j) {
        case 4:
            ret = ops->callbacks.v4s(test, j);   break;
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

    s -= libm_test_overhead(test);

    result->mops = sec2mps(s, n * sz);

    return 0;
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
    const int scale = 4 ;

    result->mops = 0;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1s(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/scale; j ++) {
            ret = ops->callbacks.v4s(test, j*scale);
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

    s -= libm_test_overhead(test);

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
    const int scale = 2;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1s(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/4; j ++) {
            ret = ops->callbacks.v2d(test, j*scale);
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

    s -= libm_test_overhead(test);

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
    const int scale = 4;

    /* Poison output */
    for (uint32_t j = 0; j < sz; ++j) {
        ops->callbacks.s1d(test, j);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < test->conf->niter ; ++i) {
        uint32_t j;

        for (j = 0; j < sz/4; j ++) {
            ret = ops->callbacks.v4d(test, j*scale);
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

    s -= libm_test_overhead(test);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}



/* This function calls the corresponding double precision
version of the function(s1d,v2d,v4d) for accuracy tests
*/
int
libm_test_accu_double(struct libm_test *test, uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_ops *ops = &test->ops;
    int sz = data->nelem;
    int scale = 1, ret = 0;

    switch (type) {
        case LIBM_FUNC_V2D: scale = 2; break;
        case LIBM_FUNC_V4D: scale = 4; break;
        default: break;
    }

    for (int j = 0; j < sz/scale; j++) {
        switch (type) {
        case LIBM_FUNC_V2D:
            ret = ops->callbacks.v2d(test, j*scale);
            break;

        case LIBM_FUNC_V4D:
            ret = ops->callbacks.v4d(test, j*scale);
            break;

        case LIBM_FUNC_S_D:
            ret = ops->callbacks.s1d(test, j);
            break;

        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;

            }
     }

    return ret;
}

/* This function calls the corresponding single precision version
of the function(s1s,v4s) for accuracy tests */
int
libm_test_accu_single(struct libm_test *test, uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_ops *ops = &test->ops;
    int sz = data->nelem;
    int scale = 1, ret = 0;

    switch (type) {
        case LIBM_FUNC_V8S: scale = 8; break;
        case LIBM_FUNC_V4S: scale = 4; break;
        default: break;
    }

    for (int j = 0; j < sz / scale; j++) {
        switch (type) {
        case LIBM_FUNC_V8S:
            ret = ops->callbacks.v8s(test, j*scale);
            break;
        case LIBM_FUNC_V4S:
            ret = ops->callbacks.v4s(test, j*scale);
            break;
        case LIBM_FUNC_S_S:
            ret = ops->callbacks.s1s(test, j);
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return ret;
}

/* Function to generate input values within required ranges */

int libm_generate_test_one_range(struct libm_test *test,
                                      const struct libm_test_input_range *range)
{
     int ret = 0;
     LIBM_TEST_DPRINTF(DBG2,
                       "Testing for accuracy %d items in range [%Lf, %Lf]",
                       test->test_data.nelem,
                       range->start, range->stop);
     test->conf->inp_range[0] = range[0];

     ret = libm_test_populate_inputs(test, range->type);
     if (test_is_single_precision(test))
         ret = libm_test_accu_single(test, test->variant);
     else
         ret = libm_test_accu_double(test, test->variant);
     return ret;
}

/****************************
 * ACCU tests
 ****************************/
int
libm_test_accu(struct libm_test *test)
{
    int ret = 0;
    struct libm_test_ops *ops = &test->ops;

    for (int i = 0; i < MAX_INPUT_RANGES; i++) {
        struct libm_test_input_range *range = &test->conf->inp_range[i];
        if ((range->start == 0.0) && (range->stop == 0.0) )
            break;

        ret = libm_generate_test_one_range(test, range);
        if (!ret) break;

        ret = ops->verify(test, &test->result);
        if (!ret) break;
    }

    if(ret)
        return ret;

    ret = ops->verify(test, &test->result);
 
    return ret;
}


/********conformance functions**************/
int libm_test_conf(struct libm_test *test)
{
    int ret=0;
    struct libm_test_data *data = &test->test_data;
    struct libm_test_ops *ops = &test->ops;
    int sz = data->nelem, end=sz;
    int *exception = data->raised_exception;
    for(int j=0; j < end; j++) {
        feclearexcept(FE_ALL_EXCEPT);
        if (test_is_single_precision(test)) {
            ret = ops->callbacks.s1s(test,j);
        }
        else {
            ret = ops->callbacks.s1d(test,j);
        }
        const int flags = fetestexcept(FE_ALL_EXCEPT);
        exception[j] = flags;
    }
    return ret;
}

/******************special tests***************/
int libm_test_special(struct libm_test *test)
{
    int ret=0;
    struct libm_test_data *data = &test->test_data;
    struct libm_test_ops *ops = &test->ops;
    int sz = data->nelem;
    for(int j=0; j<sz; j++) {
        if (test_is_single_precision(test)) {
            ret = ops->callbacks.s1s(test, j);
        }
        else {
            ret = ops->callbacks.s1d(test, j);
        }
    }
    return ret;
}

