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
#include <assert.h>                             /* for assert() */
#include <stdio.h>
#include <math.h>
#include <string.h>		/* for memcpy() */

#include <immintrin.h>

#include <libm_amd.h>

#include <libm_tests.h>
#include <bench_timer.h>

#define __TEST_EXP2_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp2_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);

#if 0
__attribute__((optimize("tree-vectorize")))
void test_exp2_scalar(test_conf_t *conf, test_report_t *report,
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

static int test_exp2_vrd4_perf(struct libm_test *test)
{
    struct libm_test_data *data = test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = GCC_ALIGN(data->output, 256);
    double *restrict ip1 = GCC_ALIGN(data->input1, 256);
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j)
        o[j] = exp(ip1[j]);

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        //IVDEP //;
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

static int test_exp2_vrd4_other(struct libm_test *test)
{
    struct libm_test_data *data = test->test_data;
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

int libm_test_exp2_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = test->test_data;

    for (uint32_t j = 0; j < data->nelem; j++) {
        data->expected[j] = exp(data->input1[j]);
    }

    return libm_test_verify_dbl(test, result);
}

/* vector single precision */
struct libm_test exp2_test_template = {
    .name = "exp2_vec",
    .ops = {
        .ulp = libm_test_generic_ulp,
        .verify = libm_test_exp2_verify,
    },
};

static void *libm_ptr_align_up(void *ptr, uint64_t align)
{
    uint64_t st = (uint64_t)ptr;

    assert(align > 0 && (align & (align - 1)) == 0); /* Power of 2 */
    assert(st != 0);

    if (st % align != 0)
        st += align - st % align;

    return (void *)st;
}

static void *libm_test_ext2_alloc_test_data(uint32_t nelem)
{
#ifdef LIBM_AVX512_SUPPORTED
#define _ALIGN_FACTOR 512
#else
#define _ALIGN_FACTOR 256
#endif

    struct libm_test_data *test_data;
    /*
     * size => 3 of size nelem (input1, output, expected)
     *         + size of the structure itself
     *         + 3 times size of _ALIGN_FACTOR
     */
    uint32_t size = sizeof(*test_data) +
        nelem * 3 * sizeof(double) + 
        (_ALIGN_FACTOR * 3);


    test_data = aligned_alloc(_ALIGN_FACTOR, size);

    if (!test_data)
        goto out;

    bzero (test_data, size);

    test_data->nelem = nelem;
    /* CAUTION */
    test_data->input1 = libm_ptr_align_up(&test_data->data[0], _ALIGN_FACTOR);
    /* CAUTION - dependency on previous */
    test_data->output = libm_ptr_align_up(&test_data->input1[nelem], _ALIGN_FACTOR);
    /* CAUTION - dependency on previous */
    test_data->expected = libm_ptr_align_up(&test_data->output[nelem], _ALIGN_FACTOR);

out:
    return test_data;
}

static int test_exp2_alloc_init_perf_data(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;

    test->test_data = libm_test_ext2_alloc_test_data(conf->nelem);

    if (!test->test_data)
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

static int test_exp2_alloc_special_data(struct libm_test *test, size_t size)
{
    struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data;

    test->test_data = aligned_alloc(64, sizeof(*test->test_data) +
                                    sizeof(*test_data) * size * 2);

    if (!test->test_data)
        goto out;

    test_data = test->test_data;
    test_data->nelem = size;
    test_data->input1 = &test_data->data[0];
    test_data->output = &test_data->data[size];
    test_data->expected = &test_data->output[size];

    /* fixup conf */
    conf->nelem = size;

    return 0;

 out:
    return -1;
}


static int test_exp2_vrd4_special_setup(struct libm_test *test)
{
    int test_data_size = ARRAY_SIZE(libm_test_exp2_special_data)/2;
    struct libm_test_data *data = test->test_data;

    // Just trying to get rid of warning/errors
    test_exp2_alloc_special_data(test, test_data_size);

    for (int i = 0; i < test_data_size; i++) {
        data->input1[i] = libm_test_exp2_special_data[i].in;
        data->expected[i] =libm_test_exp2_special_data[i].out;
    }

    return 0;
}

static int test_exp2_vrd4_other_setup(struct libm_test *test)
{
    return 0;
}

static int test_exp2_init_v4d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_types = conf->test_types;

    while(test_types) {
        struct libm_test *exp2_v4d = malloc(sizeof(struct libm_test));
        if (!exp2_v4d)
            return -1;

        memcpy(exp2_v4d, &exp2_test_template, sizeof(*exp2_v4d));

        exp2_v4d->variant |= LIBM_FUNC_V4D;
        exp2_v4d->conf = conf;

        uint32_t bit = 1 << ffs(test_types);
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
            //exp2_v4d->test_data = libm_test_exp2_accu_data;
            exp2_v4d->ops.setup = test_exp2_vrd4_other_setup;
            exp2_v4d->ops.run = test_exp2_vrd4_other;
            break;
        case TEST_TYPE_CORNER:
            exp2_v4d->type_name = "corner";
            //exp2_v4d->test_data  = libm_test_exp2_corner_data;
            exp2_v4d->ops.run = test_exp2_vrd4_other;
            break;
        }

        if (ret)
            return -1;

        test_types = test_types & (test_types -  1);
        ret = test_exp2_register_one(exp2_v4d);

        if (ret)
            return -1;
    }

    return 0;
}

#define EXP2_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

int libm_test_init(struct libm_test_conf *conf)
{
    int ret = 0;

    if (!conf->test_types)
        conf->test_types = EXP2_TEST_TYPES_ALL;

    if (conf->variants & LIBM_FUNC_V4D) {
        ret = test_exp2_init_v4d(conf);
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
        ret = libm_test_register(exp2_test);
        if (ret)
            exit();
    }
#endif

out:
    return ret;
}

