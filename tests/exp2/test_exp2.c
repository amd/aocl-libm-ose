#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <quadmath.h>
#include <strings.h>                            /* for ffs() */
#include <string.h>                             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>

#include <libm_tests.h>
#include <bench_timer.h>

#define __TEST_EXP2_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp2_data.h"

static int test_exp2_v2d_perf(struct libm_test *test)
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
        o[j] = exp2(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 1); j += 2) {
            __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
            __m128d op2 = FN_PROTOTYPE_FMA3(vrd2_exp2)(ip2);
            _mm_store_pd(&o[j], op2);
        }
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        if (sz - j)
            o[j] = FN_PROTOTYPE_FMA3(exp2)(ip1[j]);
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static int test_exp2_v4d_perf(struct libm_test *test)
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

/*
 * s1d - 1 element double precision
 */
double FN_PROTOTYPE(exp2_v1)(double);
double FN_PROTOTYPE(exp2_v2)(double);
double FN_PROTOTYPE(exp2_v3)(double);
static int test_exp2_s1d_perf(struct libm_test *test)
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
        o[j] = exp2(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            //o[j] = FN_PROTOTYPE(exp2)(ip1[j]);
            //o[j] = FN_PROTOTYPE(exp2_v1)(ip1[j]);
            //o[j] = FN_PROTOTYPE(exp2_v2)(ip1[j]);
            o[j] = exp2(ip1[j]);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

/*
 * s1s - 1 elem single precision
 */
static int test_exp2_s1s_perf(struct libm_test *test)
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
        o[j] = exp2f(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

/*
 * v4s - 4 element single precision
 */
static int test_exp2_v4s_perf(struct libm_test *test)
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
        o[j] = exp2f(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 3); j += 4) {
            __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m128 op4 = FN_PROTOTYPE_FMA3(vrs4_exp2f)(ip4);
            _mm_store_ps(&o[j], op4);
        }
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        switch (sz - j) {
        case 3:
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
            j++;	         __attribute__ ((fallthrough));
        case 2:
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
            j++;	         __attribute__ ((fallthrough));
        case 1:
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
        default:
            break;
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

#if 0
/*
 * v8s - 8 element single precision
 */
static int test_exp2_v8s_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    float *restrict o   = (float*)data->output;
    float *restrict ip1 = (float*)data->input1;
    uint64_t sz         = data->nelem;
    uint64_t n          = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = exp2(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 7); j += 8) {
            __m256 ip2 = _mm256_set_ps(ip1[j+7], ip1[j+6], ip1[j+5], ip1[j+4],
                                       ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m256 op4 = FN_PROTOTYPE_FMA3(vrs8_exp2f)(ip4);
            _mm256_store_ps(&o[j], op4);
        }
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost 7 leftovers,
         */
        switch (sz - j) {
        case 7:
        case 6:
        case 5:
        case 4:
            __m128 ip4 = __mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m128 op4 = FN_PROTOTYPE_FMA3(exp2f_v4s)(ip1[j]);
            _mm_store_ps(&o[j], op4)
            j += 4
        default:
            break;
        }

        switch (sz- j) {
        case 3:
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
            j++;
        case 2:
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
            j++;
        case 1:
            o[j] = FN_PROTOTYPE_FMA3(exp2f)(ip1[j]);
        default:
            break;
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}
#endif

int test_exp2_populate_inputs(struct libm_test *test, int use_uniform);


static int test_exp2_perf_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    test->ulp_threshold = 0.09;
    ret = libm_test_alloc_test_data(test, conf->nelem);
    if (ret)
        goto out;

    ret = test_exp2_populate_inputs(test, 0);

    if (ret || !test->test_data.input1)
        goto out;

    return 0;

 out:
    return -1;
}

static int test_exp2_accu_setup(struct libm_test *test)
{
    return test_exp2_perf_setup(test);
}


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

static int test_exp2_special_setup(struct libm_test *test)
{
    /*
     * structure contains both in and out values,
     * input only array size is half of original
     */
    int test_data_size = ARRAY_SIZE(test_exp2_special_data)/2;

    struct libm_test_data *data;

    // Just trying to get rid of warning/errors
    test_exp2_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    for (int i = 0; i < test_data_size; i++) {
        data->input1[i] = test_exp2_special_data[i].in;
        data->expected[i] = test_exp2_special_data[i].out;
    }

    return 0;
}

#if 0
static int test_exp2_vrd2(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip = &data->input1[0];
    double *op = &data->output[0];
    int sz = data->nelem;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < (sz - 1); j += 2) {
        __m128d ip2 = _mm_set_pd(ip[j+1], ip[j]);
        __m128d op2 = FN_PROTOTYPE_FMA3(vrd2_exp2)(ip2);
        _mm_store_pd(&op[j], op2);
    }

    return 0;
}

static int test_exp2_vrd4(struct libm_test *test)
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

static int test_exp2_v4s(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    float *ip = (float*)&data->input1[0];
    float *op = (float*)&data->output[0];
    int sz = data->nelem, j;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (j = 0; j < (sz - 3); j += 4) {
        __m128 ip4 = _mm_set_ps(ip[j+3], ip[j+2], ip[j+1], ip[j]);
        __m128 op4 = FN_PROTOTYPE_FMA3(vrs4_exp2f)(ip4);
        _mm_store_ps(&op[j], op4);
    }

    switch (sz- j) {
    case 3:
        op[j] = FN_PROTOTYPE_FMA3(exp2f)(ip[j]);
        j++;
    case 2:
        op[j] = FN_PROTOTYPE_FMA3(exp2f)(ip[j]);
        j++;
    case 1:
        op[j] = FN_PROTOTYPE_FMA3(exp2f)(ip[j]);
    default:
        break;
    }

    return 0;
}

static int test_exp2_v8s(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    float *ip = (float*)&data->input1[0];
    float *op = (float*)&data->output[0];
    int sz = data->nelem;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < (sz - 3); j += 4) {
        __m256 ip8 = _mm256_set_ps(ip[j+7], ip[j+6], ip[j+5], ip[j+4],
                                   ip[j+3], ip[j+2], ip[j+1], ip[j]);
        __m256 op8 = FN_PROTOTYPE_FMA3(vrs8_exp2f)(ip8);
        _mm256_store_ps(&op[j], op8);
    }

    return 0;
}
#endif

/**************************
 * ACCURACY TESTS
 **************************/
#include "exp2_accu_data.h"

static int __test_exp2_accu(struct libm_test *test,
                            uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    double *ip = &data->input1[0];
    double *op = &data->output[0];
    int sz = data->nelem;

    for (int j = 0; j < (sz - 3);) {
        __m128d ip2, op2;
        __m256d ip4, op4;
        switch (type) {
        case LIBM_FUNC_V2D:
            ip2 = _mm_set_pd(ip[j+1], ip[j]);
            op2 = FN_PROTOTYPE_FMA3(vrd2_exp2)(ip2);
            _mm_store_pd(&op[j], op2);
            j += 2;
            break;
        case LIBM_FUNC_V4D:
            ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = FN_PROTOTYPE_FMA3(vrd4_exp2)(ip4);
            _mm256_store_pd(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_D:
            //op[j] = FN_PROTOTYPE_FMA3(exp2)(ip[j]);
            op[j] = FN_PROTOTYPE(exp2(ip[j]));
            //op[j] = exp2(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
}

static int __test_exp2f_accu(struct libm_test *test,
                            uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    float *ip = (float*)&data->input1[0];
    float *op = (float*)&data->output[0];
    int sz = data->nelem;

    for (int j = 0; j < (sz - 3);) {
        __m128 ip4, op4;
        switch (type) {
        case LIBM_FUNC_V4S:
            ip4 = _mm_set_ps(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = FN_PROTOTYPE_FMA3(vrs4_exp2f)(ip4);
            _mm_store_ps(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_S:
            op[j] = FN_PROTOTYPE_FMA3(exp2f)(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
}

int libm_test_exp2_verify(struct libm_test *test,
                          struct libm_test_result *result);

#define SINGLE_PRECISION_MASK (LIBM_FUNC_S_S | LIBM_FUNC_V2S | \
                               LIBM_FUNC_V4S | LIBM_FUNC_V8S)

static int __generate_test_one_range(struct libm_test *test,
                                     struct libm_test_input_range *range)
{
    int ret = 0;

    LIBM_TEST_DPRINTF(VERBOSE2,
                      "Testing for accuracy %d items in range [%Lf, %Lf]\n",
                      test->test_data.nelem,
                      range->start, range->stop);

    test->conf->inp_range[0] = *range;

    ret = test_exp2_populate_inputs(test, 1);

    if (test->variant & SINGLE_PRECISION_MASK)
        ret = __test_exp2f_accu(test, test->variant);
    else
        ret = __test_exp2_accu(test, test->variant);

    return ret;
}

static int test_exp2_accu(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    int sz = data->nelem;
    int arr_sz = ARRAY_SIZE(accu_ranges);
    int ret = 0;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    if (test->conf->inp_range[0].start) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];

        ret = __generate_test_one_range(test, range);
        if (ret)
            return ret;

        ret = libm_test_exp2_verify(test, &test->result);

        return ret;
    }
    
    for (int i = 0; i < arr_sz ||
             (accu_ranges[i].start = 0.0 &&
              accu_ranges[i].stop == 0.0) ; i++) {
        ret = __generate_test_one_range(test, &accu_ranges[i]);
        if (ret)
            return ret;

        ret = libm_test_exp2_verify(test, &test->result);
    }

    return 0;
}

struct libm_test_funcs {
    struct libm_test_ops performance;
    struct libm_test_ops accuracy;
    struct libm_test_ops special;
    struct libm_test_ops corner;
};

struct libm_test_funcs test_exp2_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance = { .setup = test_exp2_perf_setup,
                                          .run   = test_exp2_s1s_perf,},
                         .accuracy     = {.setup = test_exp2_accu_setup,
                                          .run   = test_exp2_accu,},
                         .special      = {.setup = test_exp2_special_setup,},
     },

     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = test_exp2_perf_setup,
                                          .run   = test_exp2_s1d_perf,},
                         .accuracy     = {.setup = test_exp2_accu_setup,
                                          .run   = test_exp2_accu,},
                         .special      = {.setup = test_exp2_special_setup,},
     },

#if 0
     [LIBM_FUNC_V2S] = {
                        .performance = {.setup = test_exp2_perf_setup,
                                        .run = test_exp2_v2s_perf,},
                        .accuracy     = {.setup = test_exp2_accu_setup,
                                         .run = test_exp2_v4s,},
                        .special      = {.setup = test_exp2_special_setup,},
     },

     [LIBM_FUNC_V8S]  = {
                         .performance = { .setup = test_exp2_perf_setup,
                                          .run   = test_exp2_v8s_perf,},
                         .accuracy     = {.setup = test_exp2_accu_setup,
                                          .run = test_exp2_v4s,},
                         .special      = {.setup = test_exp2_special_setup,},
     },
#endif
     [LIBM_FUNC_V4S]  = {
                         .performance = { .setup = test_exp2_perf_setup,
                                          .run   = test_exp2_v4s_perf,},
                         .accuracy     = {.setup = test_exp2_accu_setup,
                                          .run   = test_exp2_accu,},
                         .special      = {.setup = test_exp2_special_setup,
                                          .run = test_exp2_accu},
     },

     [LIBM_FUNC_V2D]  = {
                         .performance = { .setup = test_exp2_perf_setup,
                                          .run   = test_exp2_v2d_perf,},
                         .accuracy     = {.setup = test_exp2_accu_setup,
                                          .run   = test_exp2_accu,},
                         .special      = {.setup = test_exp2_special_setup,
                                          .run   = test_exp2_accu,},
     },

     [LIBM_FUNC_V4D] = {
                        .performance = {.setup = test_exp2_perf_setup,
                                        .run = test_exp2_v4d_perf,},
                        .accuracy     = {.setup = test_exp2_accu_setup,
                                         .run   = test_exp2_accu,},
                        .special      = {.setup = test_exp2_special_setup,
                                         .run   = test_exp2_accu,},
     },


};

int test_exp2_register_one(struct libm_test *test);

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

        test_types = test_types & (test_types -  1);
        switch(bit) {
        case TEST_TYPE_PERF:
            test->type_name = "perf";
            ops = &funcs->performance;
            break;
        case TEST_TYPE_SPECIAL:
            test->type_name = "special";
            ops = &funcs->special;
            break;
        case TEST_TYPE_ACCU:
            test->type_name = "accuracy";
            ops = &funcs->accuracy;
            break;
        case TEST_TYPE_CORNER:
            test->type_name = "corner";
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
        if (ops->ulp.func1)             /* test for any should be good */
            memcpy(&test->ops.ulp, &ops->ulp, sizeof(ops->ulp));

        if (ops->verify)
            test->ops.verify = ops->verify;

        /* Finally call setup if exists, it should otherwise its an error */
        if (test->ops.setup)
            ret = test->ops.setup(test);
        else {
            LIBM_TEST_DPRINTF(PANIC, "Test %s variant:%d type:%s\n",
                              test->name, bit,
                              test->type_name); // libm_test_variant_str(test->variant)
            ret = -1;
        }

        if (ret)
            goto out;

        ret = test_exp2_register_one(test);
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


#include <quadmath.h>

/* There is no exp2q in recent versions of gcc */
__float128
libm_test_exp2q(struct libm_test *test, double in)
{
    /* logq(2.0) */
    static __float128 ln2 = 6.9314718055994530941723212145817657508364e-01;
    return expq(ln2 * in);
}
__float128 libm_test_exp2q(struct libm_test *test, double in);
int libm_test_exp2_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
exp2_template = {
                 //.name       = "exp2_vec",
                 .nargs      = 1,
                 .ulp_threshold = 4.0,
                 .ops        = {
                                .ulp    = {.func1 = libm_test_exp2q},
                                .verify = libm_test_exp2_verify,
                 },
                 .libm_func  = { .func_64 = { .func1 = exp2, }, }, /* WOHOOO */
    };

#define EXP2_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)


int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;

    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;

    if (!conf->test_types)
        conf->test_types = EXP2_TEST_TYPES_ALL;

    ret = libm_tests_setup(c, test_exp2_funcs, &exp2_template);

    return ret;
}
