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
#include <libm/compiler.h>			/* for FALLTHROUGH */

#include "test_log.h"
#define __TEST_LOG_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_log_data.h"
#include "test_data_exact.h"
#include "test_data_exact_log10.h"
#include "test_data_exact_log2.h"
#include "test_data_worst.h"
#include "test_data_worst_log10.h"
#include "test_data_worst_log2.h"
#include "log_accu_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(log)(double);
float LIBM_FUNC(logf)(float);

#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

#define _ZGVdN2v_log _ZGVbN2v_log
#define _ZGVdN4v_log _ZGVdN4v_log
#define _ZGVsN4v_logf _ZGVbN4v_logf
#define _ZGVsN8v_logf _ZGVdN8v_logf

__m128d _ZGVbN2v_log(__m128d);
__m256d _ZGVdN4v_log(__m256d);

__m128 _ZGVbN4v_logf(__m128);
__m256 _ZGVdN8v_logf(__m256);
#endif

/*vector routines*/

__m128d LIBM_FUNC_VEC(d, 2, exp)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, exp)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, expf)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, expf)(__m256);

int test_log_conf_setup(struct libm_test *test)
{
    int ret=0;
    int test_data_size=0;
    if(test_is_single_precision(test)) {
        test_data_size=ARRAY_SIZE(libm_test_logf_conformance_data);
        ret=libm_setup_s1s_conf(test, libm_test_logf_conformance_data, test_data_size);
    }
    else {
       test_data_size=ARRAY_SIZE(libm_test_log_conformance_data);
       ret=libm_setup_s1d_conf(test, libm_test_log_conformance_data, test_data_size);
    }
    return ret;
}

int test_log_special_setup(struct libm_test *test)
{
    int ret=0;
    int test_data_size=0;
    if(test_is_single_precision(test)) {
        test_data_size=ARRAY_SIZE(libm_test_logf_special_data);
        ret=libm_setup_s1s_special(test, libm_test_logf_special_data, test_data_size);
    }
    else {
       test_data_size=ARRAY_SIZE(libm_test_log_special_data);
       ret=libm_setup_s1d_special(test, libm_test_log_special_data, test_data_size);
    }
    return ret;
}


/**********************
*CALLBACK FUNCTIONS*
**********************/
static int
test_log_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(logf)(_ip1[idx]);
    return 0;
}

static int
test_log_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(log)(_ip1[idx]);
    return 0;
}

/*vector routines*/
static int
test_log_cb_v4s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

    __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m128 op4 = LIBM_FUNC_VEC(s, 4, logf)(ip4);
    _mm_store_ps(&o[j], op4);

    return 0;
}
static int
test_log_cb_v2d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

    __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
    __m128d op4 = LIBM_FUNC_VEC(d, 2, log)(ip2);
    _mm_store_pd(&o[j], op4);

    return 0;
}

static int
test_log_cb_v4d(struct libm_test *test, int j)
{
	struct libm_test_data *data = &test->test_data;
	double *restrict ip1 = (double*)data->input1;
	double *restrict o = (double*)data->output;

	__m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
	__m256d op4 = LIBM_FUNC_VEC(d, 4, log)(ip4);
	_mm256_store_pd(&o[j], op4);

	return 0;
}


static int
test_log_cb_accu_ranges(struct libm_test *test, int j)
{
    int ret = 0;
    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];
        ret = generate_test_one_range(test, range);
        ret = test_log_verify(test, &test->result);
        return ret;
    }

    int arr_sz = ARRAY_SIZE(accu_ranges);
    for (int i = 0; i < arr_sz; i++) {
        if ((accu_ranges[i].start == 0.0) && (accu_ranges[i].stop == 0.0) )
            break;
    ret = generate_test_one_range(test, &accu_ranges[i]);
    if(ret)
        return ret;
    ret = test_log_verify(test, &test->result);
    }
    return 0;
}

/*ulp*/
double test_log_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return log(buf[idx]);
}

long double
test_log_logl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return logl(d[idx]);
}

/*test functiosn for log*/
struct libm_test_funcs test_log_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance =  { .setup = libm_test_perf_setup,
                                           .run   = libm_test_s1s_perf,
                                         },
                         .accuracy     = { .setup = libm_test_accu_setup,
                                           .run   = libm_test_accu,
                                           .ulp    = {.func = test_log_ulp},
                                         },

                         .special      = { .setup = test_log_special_setup,
                                           .run = libm_test_special,
                                           .verify = test_log_verify
                                          },

                         .conformance  = {.setup = test_log_conf_setup,
                                           .run   = libm_test_conf,
                                           .verify = test_log_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = libm_test_accu,
                                          .ulp = {.funcl = test_log_logl},
                                         },

                         .special      = {.setup = test_log_special_setup,
                                          .run   = libm_test_special,
                                          .verify = test_log_verify,
                                         },

                          .conformance  = {.setup = test_log_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_log_verify,
                                         },
     },
     [LIBM_FUNC_V4S]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_v4s_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = libm_test_accu,
                                          .ulp = {.func = test_log_ulp},
                                         },
                          /*
                          .conformance  = {.setup = test_log_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_log_verify,
                                         },
                          */
     },
     [LIBM_FUNC_V2D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                            .run = libm_test_v2d_perf,
                           },
                          .accuracy = {
                                         .setup = libm_test_accu_setup,
                                         .run = libm_test_accu,
                                         .ulp = {.funcl = test_log_logl},
                           },
     },
     [LIBM_FUNC_V4D] = {
                          .performance = { .setup = libm_test_perf_setup,
                                           .run = libm_test_v4d_perf,
                           },
                          .accuracy = {   .setup = libm_test_accu_setup,
                                          .run = libm_test_accu,
                                          .ulp = {.funcl = test_log_logl},
                           },
     },

};

int test_log_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
log_template = {
    .name       = "log",
    .nargs      = 1,
    .ulp_threshold = 0.5,
    .ops        = {
                    //.ulp    = {.funcl = test_log_logl},
                    .verify = test_log_verify,
                    .callbacks = {
                                    .s1s = test_log_cb_s1s,
                                    .s1d = test_log_cb_s1d,
                                    .v4s = test_log_cb_v4s,
                                    .v2d = test_log_cb_v2d,
                                    .v4d = test_log_cb_v4d,
                                    .accu_ranges = test_log_cb_accu_ranges,
                                 },
                  },
};

#define LOG_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
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


