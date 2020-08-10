#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */
#include <string.h>                             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>
#include <libm/compiler.h>                      /* for FALLTHROUGH */

#include "test_sin.h"
#define __TEST_SIN_INTERNAL__                   /* needed to include sin-test-data.h */
#include "test_sin_data.h"
#include "sin_accu_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(sin)(double);
float LIBM_FUNC(sinf)(float);

#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

#define _ZGVdN2v_sin _ZGVbN2v_sin
#define _ZGVdN4v_sin _ZGVdN4v_sin
#define _ZGVsN4v_sinf _ZGVbN4v_sinf
#define _ZGVsN8v_sinf _ZGVdN8v_sinf
#endif

/*vector routines*/
__m128d LIBM_FUNC_VEC(d, 2, sin)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, sin)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, sinf)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, sinf)(__m256);


int test_sin_conf_setup(struct libm_test *test)
{
    int ret = 0;

    if(test_is_single_precision(test)) {
        ret = libm_test_conf_setup_f32(test,
                                       (struct libm_test_special_data_f32 *)
                                       test_sinf_conformance_data,
                                       ARRAY_SIZE(test_sinf_conformance_data));
    } else {
        ret = libm_test_conf_setup_f64(test,
                                       (struct libm_test_special_data_f64 *)
                                       test_sin_conformance_data,
                                       ARRAY_SIZE(test_sin_conformance_data));
    }

    return ret;
}


int test_sin_special_setup(struct libm_test *test)
{
    int ret = 0;

    if(test_is_single_precision(test)) {
        ret = libm_test_conf_setup_f32(test,
                                      (struct libm_test_special_data_f32 *)
                                      test_sinf_special_data,
                                      ARRAY_SIZE(test_sinf_special_data));
} else {
        ret = libm_test_conf_setup_f64(test,
                                      (struct libm_test_special_data_f64 *)
                                      test_sin_special_data,
                                      ARRAY_SIZE(test_sin_special_data));
}

return ret;
}


/**********************
*CALLBACK FUNCTIONS*
**********************/
static int
test_sin_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(sinf)(_ip1[idx]);
    return 0;
}

static int
test_sin_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(sin)(_ip1[idx]);
    return 0;
}

/*vector routines*/
static int
test_sin_cb_v4s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

    __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m128 op4 = LIBM_FUNC_VEC(s, 4, sinf)(ip4);
    _mm_store_ps(&o[j], op4);

    return 0;
}

static int
test_sin_cb_v8s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

    __m256 ip8 = _mm256_set_ps(ip1[j+7], ip1[j+6], ip1[j+5], ip1[j+4],
                               ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m256 op8 = LIBM_FUNC_VEC(s, 8, sinf)(ip8);
    _mm256_store_ps(&o[j], op8);

    return 0;
}

static int
test_sin_cb_v2d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

    __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
    __m128d op4 = LIBM_FUNC_VEC(d, 2, sin)(ip2);
    _mm_store_pd(&o[j], op4);

    return 0;
}
/*
static int
test_sin_cb_v4d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

    __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m256d op4 = LIBM_FUNC_VEC(d, 4, sin)(ip4);
    _mm256_store_pd(&o[j], op4);

    return 0;
}
*/

static int
test_sin_accu_run(struct libm_test *test)
{
    int ret = 0;

    if (test->conf->inp_range[0].start  == 0 ||
        test->conf->inp_range[0].stop   == 0)
    {
        int arr_sz = ARRAY_SIZE(sin_accu_ranges);

        for (int i = 0; i < arr_sz; i++) {

            if ((sin_accu_ranges[i].start == 0.0) && (sin_accu_ranges[i].stop == 0.0))
                break;

            ret = libm_generate_test_one_range(test, &sin_accu_ranges[i]);

            if(ret)
                return ret;

            ret = test_sin_verify(test, &test->result);
            if (ret) return ret;

            }
        return ret;
    }

    return libm_test_accu(test);

}


/*ulp*/

#include "../libs/mparith/am_mp_funcs.h"

double test_sinf_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    float val = buf[idx];

    return alm_mp_sinf(val);
}

double
test_sin_ulp(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    double val = d[idx];

    if (test_is_single_precision(test)) {
        return test_sinf_ulp(test, idx);
    }

    return alm_mp_sin(val);
}

struct libm_test_funcs test_sin_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance =  { .setup = libm_test_perf_setup,
                                           .run   = libm_test_s1s_perf,
                                         },
                         .accuracy     = { .setup = libm_test_accu_setup,
                                           .run   = test_sin_accu_run,
                                           .ulp = {.func = test_sinf_ulp}
                                         },


                         .special      = { .setup = test_sin_special_setup,
                                           .run = libm_test_special,
                                           .verify = test_sin_verify
                                          },


                         .conformance  = {.setup = test_sin_conf_setup,
                                           .run   = libm_test_conf,
                                           .verify = test_sin_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = test_sin_accu_run,
                                          .ulp = {.func = test_sin_ulp},
                                         },

                         .special      = {.setup = test_sin_special_setup,
                                          .run   = libm_test_special,
                                          .verify = test_sin_verify,
                                         },

                          .conformance  = {.setup = test_sin_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_sin_verify,
                                         },
     },
     [LIBM_FUNC_V4S] = {
                          .performance = {
                                          .setup = libm_test_perf_setup,
                                          .run = libm_test_v4s_perf,
                           },
                          .accuracy = {
                                          .setup = libm_test_accu_setup,
                                          .run = test_sin_accu_run,
                                          .ulp = {.func = test_sinf_ulp},
                           },
     },
     [LIBM_FUNC_V8S] = {
                          .performance = {
                                          .setup = libm_test_perf_setup,
                                          .run = libm_test_v8s_perf,
                          },
                          .accuracy = {
                                          .setup = libm_test_accu_setup,
                                          .run = libm_test_accu,
                                          .ulp = {.func = test_sin_ulp},
                          },
     },
     [LIBM_FUNC_V2D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                            .run = libm_test_v2d_perf,
                           },
                          .accuracy = {
                                         .setup = libm_test_accu_setup,
                                         .run = test_sin_accu_run,
                                         .ulp = {.func = test_sin_ulp},
                           },
     },/*
     [LIBM_FUNC_V4D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                           .run = libm_test_v4d_perf,
                           },
                          .accuracy = {   .setup = libm_test_accu_setup,
                                          .run = libm_test_accu,
                                          .ulp = {.func = test_sin_sinl},
                           },
     },*/

};


int test_sin_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
sin_template = {
    .name       = "sin",
    .nargs      = 1,
    .ulp_threshold = 0.5,
    .ops        = {
                    .ulp    = {.func = test_sin_ulp},
                    .verify = test_sin_verify,
                    .callbacks = {
                                    .s1s = test_sin_cb_s1s,
                                    .s1d = test_sin_cb_s1d,
                                    .v4s = test_sin_cb_v4s,
                                    .v8s = test_sin_cb_v8s,
                                    .v2d = test_sin_cb_v2d,
                                   // .v4d = test_sin_cb_v4d,
                                 },
                  },
};

#define SIN_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;
    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;
    if (!conf->test_types)
        conf->test_types = SIN_TEST_TYPES_ALL;
    ret = libm_tests_setup(c, test_sin_funcs, &sin_template);
    return ret;
}

