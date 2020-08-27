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
#include <libm/compiler.h>			/* for FALLTHROUGH */

#include "test_pow.h"
#include "test_pow_data.h"

/* GLIBC prototype declarations */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

#define _ZGVdN2v_pow _ZGVbN2vv_pow
#define _ZGVdN4v_pow _ZGVdN4vv_pow
#define _ZGVsN4v_powf _ZGVbN4vv_powf
#define _ZGVsN8v_powf _ZGVdN8vv_powf
#endif

#if (LIBM_PROTOTYPE == PROTOTYPE_AMDLIBM)
#define __amd_fma3_vrs4_powf amd_vrs4_powf /* As there is no __amd_fma3_vrs4_powf implementation yet */
#endif

char doc[] = BUILD_TEST_DOC(TEST_NAME);

double LIBM_FUNC(pow)(double, double);
float LIBM_FUNC(powf)(float, float);

/*vector routines*/
__m128d LIBM_FUNC_VEC(d, 2, pow)(__m128d, __m128d);
__m256d LIBM_FUNC_VEC(d, 4, pow)(__m256d, __m256d);

__m128 LIBM_FUNC_VEC(s, 4, powf)(__m128, __m128);
__m256 LIBM_FUNC_VEC(s, 8, powf)(__m256, __m256);

/*conf setup*/
int test_pow_conf_setup(struct libm_test *test)
{
    int ret = 0;

    if(test_is_single_precision(test)) {
        ret = libm_test_conf_setup_f32(test,
                                       (struct libm_test_special_data_f32 *)
                                       test_powf_conformance_data,
                                       ARRAY_SIZE(test_powf_conformance_data));
    } else {
        ret = libm_test_conf_setup_f64(test,
                                       (struct libm_test_special_data_f64 *)
                                       test_pow_conformance_data,
                                       ARRAY_SIZE(test_pow_conformance_data));
    }

    return ret;
}

/*special setup*/
int test_pow_special_setup(struct libm_test *test)
{
    int ret=0;
    if(test_is_single_precision(test)) {
        ret = libm_test_conf_setup_f32(test,
                                       (struct libm_test_special_data_f32 *)
                                       test_powf_special_data,
                                       ARRAY_SIZE(test_powf_special_data));
    } else {
        ret = libm_test_conf_setup_f64(test,
                                       (struct libm_test_special_data_f64 *)
                                       test_pow_special_data,
                                       ARRAY_SIZE(test_pow_special_data));
    }

    return ret;
}

/**********************
*CALLBACK FUNCTIONS*
**********************/
static int
test_pow_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _ip2 = (float*)data->input2;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(powf)(_ip1[idx], _ip2[idx]);
    return 0;
}

static int
test_pow_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _ip2 = (double*)data->input2;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(pow)(_ip1[idx], _ip2[idx]);
    return 0;
}

/*vector routines*/
static int
test_pow_cb_v4s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict ip2 = (float*)data->input2;
    float *restrict o = (float*)data->output;

    __m128 ip4_1 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m128 ip4_2 = _mm_set_ps(ip2[j+3], ip2[j+2], ip2[j+1], ip2[j]);
    __m128 op4 = LIBM_FUNC_VEC(s, 4, powf)(ip4_1, ip4_2);
    _mm_store_ps(&o[j], op4);

    return 0;
}

static int
test_pow_cb_v8s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict ip2 = (float*)data->input2;
    float *restrict o = (float*)data->output;
     __m256 ip4_1 =  _mm256_set_ps(ip1[j+7], ip1[j+6], ip1[j+5], ip1[j+4], ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
     __m256 ip4_2 =  _mm256_set_ps(ip2[j+7], ip2[j+6], ip2[j+5], ip2[j+4], ip2[j+3], ip2[j+2], ip2[j+1], ip2[j]);
    __m256 op4 = LIBM_FUNC_VEC(s, 8, powf)(ip4_1, ip4_2);
    _mm256_storeu_ps(&o[j], op4);

    return 0;
}

static int
test_pow_cb_v2d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict ip2 = (double*)data->input2;
    double *restrict o = (double*)data->output;

    __m128d ip2_1 = _mm_set_pd(ip1[j+1], ip1[j]);
    __m128d ip2_2 = _mm_set_pd(ip2[j+1], ip2[j]);
    __m128d op4 = LIBM_FUNC_VEC(d, 2, pow)(ip2_1, ip2_2);
    _mm_store_pd(&o[j], op4);

    return 0;
}

static int
test_pow_cb_v4d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict ip2 = (double*)data->input2;
    double *restrict o = (double*)data->output;

    __m256d ip4_1 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m256d ip4_2 = _mm256_set_pd(ip2[j+3], ip2[j+2], ip2[j+1], ip2[j]);
    __m256d op4 = LIBM_FUNC_VEC(d, 4, pow)(ip4_1, ip4_2);
    _mm256_store_pd(&o[j], op4);

    return 0;
}

static int
test_pow_accu_run(struct libm_test *test)
{
    int ret = 0;

    if (test->conf->inp_range[0].start  == 0 ||
        test->conf->inp_range[0].stop   == 0)
    {
        int arr_sz = ARRAY_SIZE(x_range);
        for (int i = 0; i < arr_sz; i++) {
            if ((x_range[i].start == 0.0) && (x_range[i].stop == 0.0))
                break;

            ret = libm_generate_test_one_range(test, &x_range[i]);
            if(ret)
                return ret;

            ret = test_pow_verify(test, &test->result);
            if (ret) return ret;
            }
        return ret;
    }

    return libm_test_accu(test);
}



/*ulp*/
#include "../libs/mparith/am_mp_funcs.h"

double test_powf_ulp(struct libm_test *test, int idx)
{
    float *buf1 = (float*)test->test_data.input1;
    float *buf2 = (float*)test->test_data.input2;
    return alm_mp_powf(buf1[idx], buf2[idx]);
}

double
test_pow_ulp(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    double *d1 = (double*)test->test_data.input2;
    if (test_is_single_precision(test)) {
        return (test_powf_ulp(test, idx));
    }
    return alm_mp_pow(d[idx], d1[idx]);
}

/*test functiosn for pow*/
struct libm_test_funcs test_pow_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance =  { .setup = libm_test_perf_setup,
                                           .run   = libm_test_s1s_perf,
                                         },
                         .accuracy     = { .setup = libm_test_accu_setup,
                                           .run   = test_pow_accu_run,
                                           .ulp = {.func = test_powf_ulp}
                                         },

                         .special      = { .setup = test_pow_special_setup,
                                           .run = libm_test_special,
                                           .verify = test_pow_verify,
                                          },

                         .conformance  = {.setup = test_pow_conf_setup,
                                           .run   = libm_test_conf,
                                           .verify = test_pow_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = test_pow_accu_run,
                                          .ulp = {.func = test_pow_ulp},
                                         },

                         .special      = {.setup = test_pow_special_setup,
                                          .run   = libm_test_special,
                                          .verify = test_pow_verify
                                         },
                          .conformance  = {.setup = test_pow_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_pow_verify,
                                         },
     },
     [LIBM_FUNC_V4S] = {
                          .performance = {
                                          .setup = libm_test_perf_setup,
                                          .run = libm_test_v4s_perf,
                           },
                          .accuracy = {
                                          .setup = libm_test_accu_setup,
                                          .run = test_pow_accu_run,
                                          .ulp = {.func = test_powf_ulp},
                           },
     },
     [LIBM_FUNC_V8S] = {
                          .performance = {
                                          .setup = libm_test_perf_setup,
                                          .run = libm_test_v8s_perf,
                           },
                          .accuracy = {
                                          .setup = libm_test_accu_setup,
                                          .run = test_pow_accu_run,
                                          .ulp = {.func = test_powf_ulp},
                           },
     },
     [LIBM_FUNC_V2D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                            .run = libm_test_v2d_perf,
                           },
                          .accuracy = {
                                         .setup = libm_test_accu_setup,
                                         .run = test_pow_accu_run,
                                         .ulp = {.func = test_pow_ulp},
                           },
     },
     [LIBM_FUNC_V4D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                            .run = libm_test_v4d_perf,
                           },
                          .accuracy = {
                                         .setup = libm_test_accu_setup,
                                         .run = test_pow_accu_run,
                                         .ulp = {.func = test_pow_ulp},
                           },
     },


};


int test_pow_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
pow_template = {
    .name       = "pow",
    .nargs      = 2,
    .ulp_threshold = 0.5,
    .ops        = {
                    //.ulp    = {.funcl = test_pow_powl},
                    .verify = test_pow_verify,
                    .callbacks = {
                                    .s1s = test_pow_cb_s1s,
                                    .s1d = test_pow_cb_s1d,
                                    .v4s = test_pow_cb_v4s,
                                    .v8s = test_pow_cb_v8s,
                                    .v2d = test_pow_cb_v2d,
                                    .v4d = test_pow_cb_v4d,
                                 },
                  },
};

#define POW_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;
    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;
    if (!conf->test_types)
        conf->test_types = POW_TEST_TYPES_ALL;
    ret = libm_tests_setup(c, test_pow_funcs, &pow_template);
    return ret;
}



