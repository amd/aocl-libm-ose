/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Nimmy Krishnan <Nimmy.Krishnan@amd.com>
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
#include <fmaintrin.h>
#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>
#include <libm_types.h>

#define __TEST_LOG_INTERNAL__                   /* needed to include log-test-data.h */
#include "test_log_data.h"


double test_log_ulp(struct libm_test *test, int idx)
{
   float *buf = (float*)test->test_data.input1;
   return log(buf[idx]);

}

static int test_log_v4d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = data->output;
    double *restrict ip1 = data->input1;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = log(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        //IVDEP //;
        for (uint32_t j = 0; j < (sz - 3); j += 4) {
            __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m256d op4 = LIBM_FUNC(vrd4_log)(ip4);
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

static int test_log_v2d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = data->output;
    double *restrict ip1 = data->input1;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = log(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 1); j += 2) {
            __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
            __m128d op2 = LIBM_FUNC(vrd2_log)(ip2);
            _mm_store_pd(&o[j], op2);
        }
        /*
         * Any left over process with scalar
         */
           if (sz - j)
            o[j] = LIBM_FUNC(log)(ip1[j]);
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static int test_log_s1d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = data->output;
    double *restrict ip1 = data->input1;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
            double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
            o[j] = log(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            o[j] = LIBM_FUNC(log)(ip1[j]);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;

}
float LIBM_FUNC(logf)(float);
static int test_log_s1s_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    float *restrict o = (float*)data->output;
    float *restrict ip1 = (float*)data->input1;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        float t = ((j+1)%700) + (float)j/(float)RAND_MAX;
        o[j] = logf(t);
    }

     bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            o[j] = LIBM_FUNC(logf)(ip1[j]);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;

}

static int test_log_v4s_perf(struct libm_test *test)
{
   struct libm_test_data *data = &test->test_data;
   struct libm_test_result *result = &test->result;
   float *restrict o = (float*)data->output;
   float *restrict ip1 = (float*)data->input1;
   uint64_t sz = data->nelem;
   uint64_t n = test->conf->niter;

   /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = ((j+1)%700) + j / RAND_MAX;
        o[j] = logf(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

      for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 3); j += 4) {
            __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m128 op4 = LIBM_FUNC(vrs4_logf)(ip4);
            _mm_store_ps(&o[j], op4);
        }

    switch (sz - j) {
        case 3:
            o[j] = LIBM_FUNC(logf)(ip1[j]);
            j++;                 __attribute__ ((fallthrough));
        case 2:
            o[j] = LIBM_FUNC(logf)(ip1[j]);
            j++;                 __attribute__ ((fallthrough));
        case 1:
            o[j] = LIBM_FUNC(logf)(ip1[j]);
        default:
            break;
        }
    }
     timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;

}


int test_log_populate_inputs(struct libm_test *test, int use_uniform);

static int test_log_perf_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    if (ret)
        goto out;

    ret = test_log_populate_inputs(test, LIBM_INPUT_RANGE_SIMPLE);

    if (ret || !test->test_data.input1)
        goto out;

    return 0;

 out:
    return -1;
}

int libm_test_log_verify(struct libm_test *test, struct libm_test_result *result);

long double
libm_test_logl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return logl(d[idx]);
}


/* vector single precision */
struct libm_test log_template = {
    .nargs      = 1,
    .ulp_threshold = 4.0,
    .ops        = {
                   .ulp    = {.funcl = libm_test_logl},
                   .verify = libm_test_log_verify,
                   },
};

int test_log_register_one(struct libm_test *test);


/**************************
 * SPECIAL CASES TESTS
 **************************/

static int test_log_alloc_special_data(struct libm_test *test, size_t size)
{
    struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data;
    int ret = 0;

    libm_test_alloc_test_data(test, size);

    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
        goto out;
    }

    test_data = &test->test_data;
    test_data->nelem = size;

    /* fixup conf */
    conf->nelem = size;

    return -1;

 out:
    return -1;
}

static int test_log_special_setup(struct libm_test *test)
{
 /*
     * structure contains both in and out values,
     * input only array size is half of original
     */
    int test_data_size = ARRAY_SIZE(test_log_special_data)/2;
    double *in1, *expected;
    struct libm_test_data *data;

     // Just trying to get rid of warning/errors
    test_log_alloc_special_data(test, test_data_size);

    data = &test->test_data;
     in1 = (double*)data->input1;
    expected = (double*)data->expected;

    for (int i = 0; i < test_data_size; i++) {
        in1[i] = test_log_special_data[i].in;
        expected[i] = test_log_special_data[i].out;
    }

    return 0;


}

/**************************
 * ACCURACY TESTS
 **************************/
#include "log_accu_data.h"

static int __test_log_accu(struct libm_test *test, uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    double *ip = (double*)data->input1;
    double *op = (double*)data->output;
    int sz = data->nelem, end = sz;

    switch(type) {
    case LIBM_FUNC_V2D: end = sz - 1; break;
    case LIBM_FUNC_V4D: end = sz - 3; break;
    default: break;
    }

    for (int j = 0; j < end;) {
        __m128d ip2, op2;
        __m256d ip4, op4;
        switch (type) {
        case LIBM_FUNC_V2D:
            ip2 = _mm_set_pd(ip[j+1], ip[j]);
            op2 = LIBM_FUNC(vrd2_log)(ip2);
            _mm_store_pd(&op[j], op2);
            j += 2;
            break;
        case LIBM_FUNC_V4D:
            ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = LIBM_FUNC(vrd4_log)(ip4);
            _mm256_store_pd(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_D:
            op[j] = LIBM_FUNC(log)(ip[j]);
            //op[j] = log(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
}

static int __test_logf_accu(struct libm_test *test,
                            uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    float *ip = (float*)data->input1;
    float *op = (float*)data->output;
    int sz = data->nelem, end = sz;

    switch(type) {
    case LIBM_FUNC_V2D: end = sz - 1; break;
    case LIBM_FUNC_V4D: end = sz - 3; break;
    default: break;
    }

    for (int j = 0; j < end;) {
        __m128 ip4, op4;
        switch (type) {
        case LIBM_FUNC_V4S:
            ip4 = _mm_set_ps(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = LIBM_FUNC(vrs4_logf)(ip4);
            _mm_store_ps(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_S:
            op[j] = LIBM_FUNC(logf)(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
}
static int __generate_test_one_range(struct libm_test *test,
                                     struct libm_test_input_range *range)
{
    int ret = 0;

    LIBM_TEST_DPRINTF(DBG2,
                      "Testing for accuracy %d items in range [%Lf, %Lf]\n",
                      test->test_data.nelem,
                      range->start, range->stop);

    test->conf->inp_range[0] = *range;

    ret = test_log_populate_inputs(test, range->type);

    if (test_is_single_precision(test))
        ret = __test_logf_accu(test, test->variant);
    else
        ret = __test_log_accu(test, test->variant);

    return ret;
}

static int test_log_accu(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    int ret = 0;
    int sz = data->nelem;

    /* we are verifying here, no need to do again */
    test->ops.verify = NULL;

    if (sz % 4 != 0)
        LIBM_TEST_DPRINTF(DBG2,
                          "%s %s : %d is not a multiple of 4, some may be left out\n"
                          " And error reported may not be real for such entries\n",
                          test->name, test->type_name, sz);

    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];

        ret = __generate_test_one_range(test, range);

        ret = libm_test_log_verify(test, &test->result);

        return ret;
    }

    int arr_sz = ARRAY_SIZE(accu_ranges);

    for (int i = 0; i < arr_sz ||
             (accu_ranges[i].start = 0.0 &&
              accu_ranges[i].stop == 0.0) ; i++) {
        ret = __generate_test_one_range(test, &accu_ranges[i]);
        if (ret)
            return ret;

        ret = libm_test_log_verify(test, &test->result);
    }

    return 0;
}


static int test_log_accu_setup(struct libm_test *test)
{
    int ret = 0;

    test_log_perf_setup(test);
    test->ulp_threshold = 0.54;

    return ret;
}

struct libm_test_funcs test_log_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance = { .setup = test_log_perf_setup,
                                          .run   = test_log_s1s_perf,},
                         .accuracy     = {.setup = test_log_accu_setup,
                                          .run   = test_log_accu,
                                          .ulp   = {.func = test_log_ulp},
                         },
                         .special      = {.setup = test_log_special_setup,},
     },

     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = test_log_perf_setup,
                                          .run   = test_log_s1d_perf,},
                         .accuracy     = {.setup = test_log_accu_setup,
                                          .run   = test_log_accu,},
                         .special      = {.setup = test_log_special_setup,},
                   /*      .corner       = {.setup = test_log_corner_setup,
                                          .run   = test_log_corner,}, */
     },

#if 0
     [LIBM_FUNC_V2S] = {
                        .performance = {.setup = test_log_perf_setup,
                                        .run = test_log_v2s_perf,},
                        .accuracy     = {.setup = test_log_accu_setup,
                                         .run = test_log_v4s,},
                        .special      = {.setup = test_log_special_setup,},
     },

     [LIBM_FUNC_V8S]  = {

                         .performance = { .setup = test_log_perf_setup,
                                          .run   = test_log_v8s_perf,},
                         .accuracy     = {.setup = test_log_accu_setup,
                                          .run = test_log_v4s,},
                         .special      = {.setup = test_log_special_setup,},
     },
#endif
     [LIBM_FUNC_V4S]  = {
                         .performance = { .setup = test_log_perf_setup,
                                          .run   = test_log_v4s_perf,},
                         .accuracy     = {.setup = test_log_accu_setup,
                                          .run   = test_log_accu,
                                          .ulp   = {.func = test_log_ulp},
                        },
                         .special      = {.setup = test_log_special_setup,
                                          .run = test_log_accu},
     },

     [LIBM_FUNC_V2D]  = {
                         .performance = { .setup = test_log_perf_setup,
                                          .run   = test_log_v2d_perf,},
                         .accuracy     = {.setup = test_log_accu_setup,
                                          .run   = test_log_accu,},
                         .special      = {.setup = test_log_special_setup,
                                          .run   = test_log_accu,},
     },

     [LIBM_FUNC_V4D] = {
                        .performance = {.setup = test_log_perf_setup,
                                        .run = test_log_v4d_perf,},
                        .accuracy     = {.setup = test_log_accu_setup,
                                         .run   = test_log_accu,},
                        .special      = {.setup = test_log_special_setup,
                                         .run   = test_log_accu,},
     },


};

#define LOG_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;

    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;

    if (!conf->test_types)
        conf->test_types = LOG_TEST_TYPES_ALL;

    ret = libm_tests_setup(c, test_log_funcs, &log_template);
    return ret;
}


static char *libm_test_variant_str(uint32_t variant)
{
    switch(variant) {
    case LIBM_FUNC_S_S:
        return "s1s";
    case LIBM_FUNC_S_D:
        return "s1d";
    case LIBM_FUNC_V2S:
        return "v2s";
    case LIBM_FUNC_V4S:
        return "v4s";
    case LIBM_FUNC_V8S:
        return "v8s";
    case LIBM_FUNC_V2D:
        return "v2d";
    case LIBM_FUNC_V4D:
        return "v4d";
    default:
        break;
    }

    return "unknown";
}


int libm_test_type_setup(struct libm_test_conf *conf,
                         struct libm_test *template,
                         struct libm_test_funcs *funcs,
                         char *type_name, uint32_t input_type)
{
    int ret = 0;

    // Sanitary check
    uint32_t test_types = conf->test_types;
    while(test_types) {
        uint32_t bit = 1 << (ffs(test_types) - 1);
        struct libm_test_ops *ops = NULL;
        struct libm_test *test;

        test = libm_test_alloc_init(conf, template);
    if (!test) {
            LIBM_TEST_DPRINTF(PANIC, "Unable to allocate memory for test\n");
            continue;
        }

        test->name = type_name;
        test->variant = input_type;
        test->input_name = libm_test_variant_str(input_type);

        test_types = test_types & (test_types -  1);
        switch(bit) {
        case TEST_TYPE_PERF:
            test->type_name = "perf";
            test->test_type =  TEST_TYPE_PERF;
            ops = &funcs->performance;
            break;
        case TEST_TYPE_SPECIAL:
            test->type_name = "special";
            test->test_type =  TEST_TYPE_SPECIAL;
            ops = &funcs->special;
        break;
        case TEST_TYPE_ACCU:
            test->type_name = "accuracy";
            test->test_type =  TEST_TYPE_ACCU;
            ops = &funcs->accuracy;
            break;
        case TEST_TYPE_CORNER:
            test->type_name = "corner";
            test->test_type =  TEST_TYPE_CORNER;
            ops = &funcs->corner;
            break;
        default:
            goto out;
        }

        if (!ops->setup && !ops->run)
            goto free_test;

        /* Override only the ones provided */
        if (ops->setup)
        test->ops.setup = ops->setup;
        if (ops->run)
            test->ops.run = ops->run;
        if (ops->cleanup)
            test->ops.cleanup = ops->cleanup;
        if (ops->ulp.func || ops->ulp.funcq) /* test for any should be good */
            test->ops.ulp = ops->ulp;

        if (ops->verify)
            test->ops.verify = ops->verify;

        /* Finally call setup if exists, it should otherwise its an error */
        if (test->ops.setup)
            ret = test->ops.setup(test);
        else {
            LIBM_TEST_DPRINTF(PANIC, "Test %s variant:%d type:%s\n",
                              test->name, bit,
                              test->type_name);
            ret = -1;
        }
        if (ret)
            goto out;

        ret = test_log_register_one(test);
        if (ret)
            goto free_test;

        continue;

        free_test:
            libm_test_free(test);
    }

    return 0;

    out:
       return -1;

}

int libm_tests_setup(struct libm_test_conf *conf,
                     struct libm_test_funcs test_table[LIBM_FUNC_MAX],
                     struct libm_test *template)
{
    uint32_t variants = conf->variants;
    int ret = 0;

    while (variants) {
        uint32_t bit = 1 << (ffs(variants) - 1);
        char *name;
        struct libm_test_funcs *funcs = &test_table[bit];

        variants = variants & (variants -  1);

        switch(bit) {
        case LIBM_FUNC_S_S:
        case LIBM_FUNC_S_D:
            name = "scalar";
            break;
       break;
        case LIBM_FUNC_V2S:
        case LIBM_FUNC_V4S:
        case LIBM_FUNC_V8S:
        case LIBM_FUNC_V2D:
        case LIBM_FUNC_V4D:
            name = "vector";
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "unknown for variant:%s\n",
                              libm_test_variant_str(bit));
            continue;
        }

        ret = libm_test_type_setup(conf, template, funcs, name, bit);
        if (ret) {
            LIBM_TEST_DPRINTF(PANIC, "unable to setup %s\n", name);
        }
    }

    return 0;
}


