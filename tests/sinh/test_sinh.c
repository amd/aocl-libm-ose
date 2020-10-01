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
#include <libm/compiler.h>                      /* for FALLTHROUGH */

#include "test_sinh.h"
#define __TEST_SINH_INTERNAL__                   /* needed to include cos-test-data.h */
#include "test_sinh_data.h"
#include "sinh_accu_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(sinh)(double);
float LIBM_FUNC(cosf)(float);

/*
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

#define _ZGVdN2v_cos _ZGVbN2v_cos
#define _ZGVdN4v_cos _ZGVdN4v_cos
#define _ZGVsN4v_cosf _ZGVbN4v_cosf
#define _ZGVsN8v_cosf _ZGVdN8v_cosf
#endif
*/

/*No vector routines for sinh for now*/
/*
__m128d LIBM_FUNC_VEC(d, 2, cos)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, cos)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, cosf)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, cosf)(__m256);
*/

int test_sinh_conf_setup(struct libm_test *test)
{
    int ret = 0;

    if(test_is_single_precision(test)) {
        ret = libm_test_conf_setup_f32(test,
                                       (struct libm_test_special_data_f32 *)
                                       test_sinhf_conformance_data,
                                       ARRAY_SIZE(test_sinhf_conformance_data));
    } else {
        ret = libm_test_conf_setup_f64(test,
                                       (struct libm_test_special_data_f64 *)
                                       test_sinh_conformance_data,
                                       ARRAY_SIZE(test_sinh_conformance_data));
    }

    return ret;
}


int test_sinh_special_setup(struct libm_test *test)
{
    int ret = 0;

    if(test_is_single_precision(test)) {
            ret = libm_test_conf_setup_f32(test,
                                          (struct libm_test_special_data_f32 *)
                                          test_sinhf_special_data,
                                          ARRAY_SIZE(test_sinhf_special_data));
    } else {
            ret = libm_test_conf_setup_f64(test,
                                          (struct libm_test_special_data_f64 *)
                                          test_sinh_special_data,
                                          ARRAY_SIZE(test_sinh_special_data));
	}

    return ret;
}


/**********************
*CALLBACK FUNCTIONS*
**********************/
static int
test_sinh_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(sinhf)(_ip1[idx]);
    return 0;
}

static int
test_sinh_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(sinh)(_ip1[idx]);
    return 0;
}

/*No vector functions for this release*/
/*vector routines*/
/*
static int
test_cos_cb_v4s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

    __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m128 op4 = LIBM_FUNC_VEC(s, 4, cosf)(ip4);
    _mm_store_ps(&o[j], op4);

    return 0;
}

static int
test_cos_cb_v8s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

    __m256 ip8 = _mm256_set_ps(ip1[j+7], ip1[j+6], ip1[j+5], ip1[j+4],
                               ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m256 op8 = LIBM_FUNC_VEC(s, 8, cosf)(ip8);
    _mm256_store_ps(&o[j], op8);

    return 0;
}

static int
test_cos_cb_v2d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

    __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
    __m128d op4 = LIBM_FUNC_VEC(d, 2, cos)(ip2);
    _mm_store_pd(&o[j], op4);

    return 0;
}

static int
test_cos_cb_v4d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

    __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m256d op4 = LIBM_FUNC_VEC(d, 4, cos)(ip4);
    _mm256_store_pd(&o[j], op4);

    return 0;
}
*/

static int
test_sinh_accu_run(struct libm_test *test)
{
    int ret = 0;
    if (test->conf->inp_range[0].start  == 0 ||
        test->conf->inp_range[0].stop   == 0)
    {
         int arr_sz = ARRAY_SIZE(sinh_accu_ranges);
         for (int i = 0; i < arr_sz; i++) {
                if ((sinh_accu_ranges[i].start == 0.0) && (sinh_accu_ranges[i].stop == 0.0))
                    break;

                ret = libm_generate_test_one_range(test, &sinh_accu_ranges[i]);

                if (ret) return ret;

                ret = test_sinh_verify(test, &test->result);

                if (ret) return ret;

         }

        return ret;
    }

    return libm_test_accu(test);
}

/*ulp*/

#include "../libs/mparith/am_mp_funcs.h"

double test_sinhf_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    float val = buf[idx];

    return alm_mp_sinhf(val);
}

double
test_sinh_ulp(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    double val = d[idx];

    if (test_is_single_precision(test)) {
        return test_sinhf_ulp(test, idx);
    }

    return alm_mp_sinh(val);
}

struct libm_test_funcs test_sinh_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance =  { .setup = libm_test_perf_setup,
                                           .run   = libm_test_s1s_perf,
                                         },
                         .accuracy     = { .setup = libm_test_accu_setup,
                                           .run   = test_sinh_accu_run,
                                           .ulp = {.func = test_sinhf_ulp}
                                         },


                         .special      = { .setup = test_sinh_special_setup,
                                           .run = libm_test_special,
                                           .verify = test_sinh_verify
                                          },


                         .conformance  = {.setup = test_sinh_conf_setup,
                                           .run   = libm_test_conf,
                                           .verify = test_sinh_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = test_sinh_accu_run,
                                          .ulp = {.func = test_sinh_ulp},
                                         },

                         .special      = {.setup = test_sinh_special_setup,
                                          .run   = libm_test_special,
                                          .verify = test_sinh_verify,
                                         },

                          .conformance  = {.setup = test_sinh_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_sinh_verify,
                                         },
     },
/*
     [LIBM_FUNC_V4S] = {
                          .performance = {
                                          .setup = libm_test_perf_setup,
                                          .run = libm_test_v4s_perf,
                           },
                          .accuracy = {
                                          .setup = libm_test_accu_setup,
                                          .run = test_cos_accu_run,
                                          .ulp = {.func = test_cosf_ulp},
                           },
     },
     [LIBM_FUNC_V8S] = {
                          .performance = {
                                          .setup = libm_test_perf_setup,
                                          .run = libm_test_v8s_perf,
                          },
                          .accuracy = {
                                          .setup = libm_test_accu_setup,
                                          .run = test_cos_accu_run,
                                          .ulp = {.func = test_cosf_ulp},
                          },
     },
     [LIBM_FUNC_V2D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                            .run = libm_test_v2d_perf,
                           },
                          .accuracy = {
                                         .setup = libm_test_accu_setup,
                                         .run = test_cos_accu_run,
                                         .ulp = {.func = test_cos_ulp},
                           },
     },
     [LIBM_FUNC_V4D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                           .run = libm_test_v4d_perf,
                           },
                          .accuracy = {   .setup = libm_test_accu_setup,
                                          .run = libm_test_accu,
                                          .ulp = {.func = test_cos_ulp},
                           },
     },*/

};


int test_sinh_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
sinh_template = {
    .name       = "sinh",
    .nargs      = 1,
    .ulp_threshold = 0.5,
    .ops        = {
                    .ulp    = {.func = test_sinh_ulp},
                    .verify = test_sinh_verify,
                    .callbacks = {
                                    .s1s = test_sinh_cb_s1s,
                                    .s1d = test_sinh_cb_s1d,
                                    //.v4s = test_cos_cb_v4s,
                                    //.v8s = test_cos_cb_v8s,
                                    //.v2d = test_cos_cb_v2d,
                                    //.v4d = test_cos_cb_v4d,
                                 },
                  },
};

#define SINH_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;
    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;
    if (!conf->test_types)
        conf->test_types = SINH_TEST_TYPES_ALL;
    ret = libm_tests_setup(c, test_sinh_funcs, &sinh_template);
    return ret;
}

