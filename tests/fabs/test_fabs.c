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

#include "test_fabs.h"
#include "test_fabs_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(fabs)(double);
float LIBM_FUNC(fabsf)(float);

int test_fabs_conf_setup(struct libm_test *test)
{
    int ret=0;
    int test_data_size=0;
    if(test_is_single_precision(test)) {
        test_data_size=ARRAY_SIZE(libm_test_fabsf_conformance_data);
        ret=libm_setup_s1s_conf(test, libm_test_fabsf_conformance_data, test_data_size);
    }
    else {
       test_data_size=ARRAY_SIZE(libm_test_fabs_conformance_data);
       ret=libm_setup_s1d_conf(test, libm_test_fabs_conformance_data, test_data_size);
    }
    return ret;
}


/**********************
*CALLBACK FUNCTIONS*
**********************/
static int
test_fabs_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(fabsf)(_ip1[idx]);
    return 0;
}

static int
test_fabs_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(fabs)(_ip1[idx]);
    return 0;
}

static int
test_fabs_cb_accu_ranges(struct libm_test *test, int j)
{
    int ret = 0;
    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];
        ret = libm_generate_test_one_range(test, range);
        ret = test_fabs_verify(test, &test->result);
        return ret;
        }

    int arr_sz = ARRAY_SIZE(fabs_accu_ranges);
    for (int i = 0; i < arr_sz; i++) {
        if ((fabs_accu_ranges[i].start == 0.0) && (fabs_accu_ranges[i].stop == 0.0) )
            break;
    ret = libm_generate_test_one_range(test, &fabs_accu_ranges[i]);
    if(ret)
        return ret;
    ret = test_fabs_verify(test, &test->result);
    }
    return 0;
}

/*ulp*/
double test_fabs_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return fabs(buf[idx]);
}

long double
test_fabs_fabsl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return fabsl(d[idx]);
}

/*test functiosn for fabs*/
struct libm_test_funcs test_fabs_funcs[LIBM_FUNC_MAX] =
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
                                           .ulp = {.func = test_fabs_ulp},
                                         },
                          /*
                         .special      = { .setup = test_fabsf_special_setup,
                                           .run = test_fabsf_special,
                                           .verify = test_fabs_verify
                                          },
                          */
                         .conformance  = {.setup = test_fabs_conf_setup,
                                           .run   = libm_test_conf,
                                           .verify = test_fabs_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = libm_test_accu,
                                          .ulp = {.funcl = test_fabs_fabsl},
                                         },
                          /*
                         .special      = {.setup = test_fabs_special_setup,
                                          .run   = test_fabs_special,
                                         },
                          */
                          .conformance  = {.setup = test_fabs_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_fabs_verify,
                                         },
     },
};

int test_fabs_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
fabs_template = {
    .name       = "fabs",
    .nargs      = 1,
    .ulp_threshold = 0.5,
    .ops        = {
                    //.ulp    = {.funcl = test_fabs_fabsl},
                    .verify = test_fabs_verify,
                    .callbacks = {
                                    .s1s = test_fabs_cb_s1s,
                                    .s1d = test_fabs_cb_s1d,
                                    .accu_ranges = test_fabs_cb_accu_ranges,
                                 },
                  },
};

#define FABS_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;
    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;
    if (!conf->test_types)
        conf->test_types = FABS_TEST_TYPES_ALL;
    ret = libm_tests_setup(c, test_fabs_funcs, &fabs_template);
    return ret;
}
