#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <quadmath.h>
#include <strings.h>                            /* for ffs() */
#include <string.h>                             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>

#include "test_exp.h"
#define __TEST_EXP2_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp_data.h"

/* GLIBC prototype declarations */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

#define _ZGVdN4v(x) _ZGVbN4v_##x
#define _ZGVsN4v_expf _ZGVbN4v_expf
#define _ZGVdN4v_exp _ZGVdN4v_exp
#define _ZGVdN2v_exp _ZGVbN2v_exp

__m128d LIBM_FUNC_VEC(d, 2, exp)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, exp)(__m256d);

__m128 LIBM_FUNC_VEC(b, 4, expf)(__m128);
__m256 LIBM_FUNC_VEC(b, 8, expf)(__m256);
#endif

#if defined(DEVELOPER)
#pragma message "Developer mode changing prototype to exp_v2()"
#undef LIBM_FUNC
#define LIBM_FUNC(x) FN_PROTOTYPE( x ## _v2 )

double FN_PROTOTYPE( exp_v2 )(double);
float FN_PROTOTYPE( expf_v2 )(float);

float FN_PROTOTYPE_FMA3( expf )(float);

#define amd_expf_v2 FN_PROTOTYPE_FMA3(expf)
#else
// Temporary need to remove
//#undef LIBM_FUNC
//#define LIBM_FUNC(x) FN_PROTOTYPE_FMA3(x)
//#define amd_expf FN_PROTOTYPE_FMA3(expf)
//#define amd_exp_bas64 FN_PROTOTYPE_FMA3(expf)
#endif

static int test_exp_v2d_perf(struct libm_test *test)
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
        o[j] = exp(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 1); j += 2) {
            __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
            __m128d op2 = LIBM_FUNC_VEC(d, 2, exp)(ip2);
            _mm_store_pd(&o[j], op2);
        }
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        if (sz - j)
            o[j] = LIBM_FUNC(exp)(ip1[j]);
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static int test_exp_v4d_perf(struct libm_test *test)
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
        o[j] = exp(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        //IVDEP //;
        for (uint32_t j = 0; j < (sz - 3); j += 4) {
            __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m256d op4 = LIBM_FUNC_VEC(d, 4, exp)(ip4);
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
double FN_PROTOTYPE(exp_v1)(double);
double FN_PROTOTYPE(exp_v2)(double);
double FN_PROTOTYPE(exp_v3)(double);
static int test_exp_s1d_perf(struct libm_test *test)
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
        o[j] = exp(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            o[j] = LIBM_FUNC(exp)(ip1[j]);
            //o[j] = FN_PROTOTYPE(exp_v1)(ip1[j]);
            //o[j] = FN_PROTOTYPE(exp_v2)(ip1[j]);
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
static int test_exp_s1s_perf(struct libm_test *test)
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
        o[j] = expf(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            o[j] = LIBM_FUNC(expf)(ip1[j]);
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
static int test_exp_v4s_perf(struct libm_test *test)
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
        o[j] = expf(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 3); j += 4) {
            __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m128 op4 = LIBM_FUNC_VEC(s, 4, expf)(ip4);
            _mm_store_ps(&o[j], op4);
        }
        /*
         * Any left over process with scalar, in a 2 vector case,
         * there can be atmost one leftover,
         */
        switch (sz - j) {
        case 3:
            o[j] = LIBM_FUNC(expf)(ip1[j]);
            j++;	         __attribute__ ((fallthrough));
        case 2:
            o[j] = LIBM_FUNC(expf)(ip1[j]);
            j++;	         __attribute__ ((fallthrough));
        case 1:
            o[j] = LIBM_FUNC(expf)(ip1[j]);
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
static int test_exp_v8s_perf(struct libm_test *test)
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
        o[j] = exp(t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < (sz - 7); j += 8) {
            __m256 ip2 = _mm256_set_ps(ip1[j+7], ip1[j+6], ip1[j+5], ip1[j+4],
                                       ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
            __m256 op4 = LIBM_FUNC_VEC(s, 8, expf)(ip4);
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
            __m128 op4 = LIBM_FUNC_VEC(s, 4, expf)(ip4);
            _mm_store_ps(&o[j], op4)
            j += 4
        default:
            break;
        }

        switch (sz- j) {
        case 3:
            o[j] = LIBM_FUNC(expf)(ip1[j]);
            j++;
        case 2:
            o[j] = LIBM_FUNC(expf)(ip1[j]);
            j++;
        case 1:
            o[j] = LIBM_FUNC(expf)(ip1[j]);
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

int test_exp_populate_inputs(struct libm_test *test, int use_uniform);


static int test_exp_perf_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);
    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
        goto out;
    }
    ret = libm_test_populate_inputs(test, LIBM_INPUT_RANGE_SIMPLE);

    if (ret || !test->test_data.input1) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to populate test_data\n");
        goto out;
    }

    return 0;

 out:
    return -1;
}

static int test_exp_accu_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);
    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
        goto out;
    }

    test->ulp_threshold = 0.54;

    return 0;
out:
    return ret;
}


static int test_exp_alloc_special_data(struct libm_test *test, size_t size)
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

    return 0;

 out:
    return -1;
}

static int test_exp_special_setup(struct libm_test *test)
{
    /*
     * structure contains both in and out values,
     * input only array size is half of original
     */
    int test_data_size = ARRAY_SIZE(test_exp_special_data)/2;
    double *in1, *expected;

    struct libm_test_data *data;

    // Just trying to get rid of warning/errors
    test_exp_alloc_special_data(test, test_data_size);

    data = &test->test_data;
    in1 = (double*)data->input1;
    expected = (double*)data->expected;

    for (int i = 0; i < test_data_size; i++) {
        in1[i] = test_exp_special_data[i].in;
        expected[i] = test_exp_special_data[i].out;
    }

    return 0;
}

#if 0
static int test_exp_vrd2(struct libm_test *test)
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
        __m128d op2 = FN_PROTOTYPE_FMA3(vrd2_exp)(ip2);
        _mm_store_pd(&op[j], op2);
    }

    return 0;
}

static int test_exp_vrd4(struct libm_test *test)
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
        __m256d op4 = FN_PROTOTYPE_FMA3(vrd4_exp)(ip4);
        _mm256_store_pd(&op[j], op4);
    }

    return 0;
}

static int test_exp_v4s(struct libm_test *test)
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
        __m128 op4 = FN_PROTOTYPE_FMA3(vrs4_expf)(ip4);
        _mm_store_ps(&op[j], op4);
    }

    switch (sz- j) {
    case 3:
        op[j] = FN_PROTOTYPE_FMA3(expf)(ip[j]);
        j++;
    case 2:
        op[j] = FN_PROTOTYPE_FMA3(expf)(ip[j]);
        j++;
    case 1:
        op[j] = FN_PROTOTYPE_FMA3(expf)(ip[j]);
    default:
        break;
    }

    return 0;
}

static int test_exp_v8s(struct libm_test *test)
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
        __m256 op8 = FN_PROTOTYPE_FMA3(vrs8_expf)(ip8);
        _mm256_store_ps(&op[j], op8);
    }

    return 0;
}
#endif

/**************************
 * ACCURACY TESTS
 **************************/
#include "exp_accu_data.h"

static int __test_exp_accu(struct libm_test *test,
                            uint32_t type)
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
            op2 = LIBM_FUNC_VEC(d, 2, exp)(ip2);
            _mm_store_pd(&op[j], op2);
            j += 2;
            break;
        case LIBM_FUNC_V4D:
            ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = LIBM_FUNC_VEC(d, 4, exp)(ip4);
            _mm256_store_pd(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_D:
            op[j] = LIBM_FUNC(exp)(ip[j]);
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

static int __test_expf_accu(struct libm_test *test,
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
            op4 = LIBM_FUNC_VEC(s, 4, expf)(ip4);
            _mm_store_ps(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_S:
            op[j] = LIBM_FUNC(expf)(ip[j]);
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

    ret = libm_test_populate_inputs(test, range->type);

    if (test_is_single_precision(test))
        ret = __test_expf_accu(test, test->variant);
    else
        ret = __test_exp_accu(test, test->variant);

    return ret;
}

static int test_exp_accu(struct libm_test *test)
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

        ret = test_exp_verify(test, &test->result);

        return ret;
    }


    int arr_sz = ARRAY_SIZE(accu_ranges);

    for (int i = 0; i < arr_sz ||
             (accu_ranges[i].start = 0.0 &&
              accu_ranges[i].stop == 0.0) ; i++) {

        ret = __generate_test_one_range(test, &accu_ranges[i]);
        if (ret)
            return ret;

        ret = test_exp_verify(test, &test->result);
    }

    return 0;
}

double test_exp_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return exp(buf[idx]);
}


struct libm_test_funcs test_exp_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance = { .setup = test_exp_perf_setup,
                                          .run   = test_exp_s1s_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,
                                          .ulp   = {.func = test_exp_ulp},
                         },
                         .special      = {.setup = test_exp_special_setup,},
     },

     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = test_exp_perf_setup,
                                          .run   = test_exp_s1d_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,},
                         .special      = {.setup = test_exp_special_setup,},
     },

#if 0
     [LIBM_FUNC_V2S] = {
                        .performance = {.setup = test_exp_perf_setup,
                                        .run = test_exp_v2s_perf,},
                        .accuracy     = {.setup = test_exp_accu_setup,
                                         .run = test_exp_v4s,},
                        .special      = {.setup = test_exp_special_setup,},
     },

     [LIBM_FUNC_V8S]  = {
                         .performance = { .setup = test_exp_perf_setup,
                                          .run   = test_exp_v8s_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run = test_exp_v4s,},
                         .special      = {.setup = test_exp_special_setup,},
     },
#endif
     [LIBM_FUNC_V4S]  = {
                         .performance = { .setup = test_exp_perf_setup,
                                          .run   = test_exp_v4s_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,},
                         .special      = {.setup = test_exp_special_setup,
                                          .run = test_exp_accu},
     },

     [LIBM_FUNC_V2D]  = {
                         .performance = { .setup = test_exp_perf_setup,
                                          .run   = test_exp_v2d_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,},
                         .special      = {.setup = test_exp_special_setup,
                                          .run   = test_exp_accu,},
     },

     [LIBM_FUNC_V4D] = {
                        .performance = {.setup = test_exp_perf_setup,
                                        .run = test_exp_v4d_perf,},
                        .accuracy     = {.setup = test_exp_accu_setup,
                                         .run   = test_exp_accu,},
                        .special      = {.setup = test_exp_special_setup,
                                         .run   = test_exp_accu,},
     },


};

/* There is no exp2q in recent versions of gcc */
long double
test_exp_expl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return expl(d[idx]);
}

int test_exp_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
exp_template = {
	//.name       = "exp_vec",
	.nargs      = 1,
	.ulp_threshold = 4.0,
	.ops        = {
		.ulp    = {.funcl = test_exp_expl},
		.verify = test_exp_verify,
	},
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

    ret = libm_tests_setup(c, test_exp_funcs, &exp_template);

    return ret;
}
