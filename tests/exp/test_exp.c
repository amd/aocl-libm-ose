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

#include "test_exp.h"
#define __TEST_EXP2_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp_data.h"
#include "test_data_exact.h"
#include "test_data_worst.h"
#include "exp_accu_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(exp)(double);
float LIBM_FUNC(expf)(float);


int test_exp_conf_setup(struct libm_test *test)
{
    int ret=0;
    int test_data_size=0;
    if(test_is_single_precision(test)) {
        test_data_size=ARRAY_SIZE(libm_test_expf_conformance_data);
        ret=libm_setup_s1s_conf(test, libm_test_expf_conformance_data, test_data_size);
    }
    else {
       test_data_size=ARRAY_SIZE(libm_test_exp_conformance_data);
       ret=libm_setup_s1d_conf(test, libm_test_exp_conformance_data, test_data_size);
    }
    return ret;
}

static int __generate_test_one_range(struct libm_test *test,
                                     const struct libm_test_input_range *range)
{
    int ret = 0;
    LIBM_TEST_DPRINTF(DBG2,
                      "Testing for accuracy %d items in range [%Lf, %Lf]\n",
                      test->test_data.nelem,
                      range->start, range->stop);
    test->conf->inp_range[0] = *range;
    ret = libm_test_populate_inputs(test, range->type);
    if (test_is_single_precision(test))
        ret = libm_test_accu_single(test, test->variant);
    else
        ret = libm_test_accu_double(test, test->variant);
    return ret;
}

/**********************
*CALLBACK FUNCTIONS*
**********************/
static int
test_exp_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(expf)(_ip1[idx]);
    return 0;
}

static int
test_exp_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(exp)(_ip1[idx]);
    return 0;
}

/*verify*/
static int
test_exp_cb_verify(struct libm_test *test, int j)
{
    int ret = 0;
    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];
        ret = __generate_test_one_range(test, range);
        ret = test_exp_verify(test, &test->result);
    }
    return ret;
}

static int
test_exp_cb_accu_ranges(struct libm_test *test, int j)
{
    int arr_sz = ARRAY_SIZE(exp2_accu_ranges);
    int ret = 0;
    for (int i = 0; i < arr_sz; i++) {
        if ((exp2_accu_ranges[i].start == 0.0) && (exp2_accu_ranges[i].stop == 0.0) )
            break;
    ret = __generate_test_one_range(test, &exp2_accu_ranges[i]);
    if(ret)
        return ret;
    ret = test_exp_verify(test, &test->result);
    }
    return 0;
}

/*ulp*/
double test_exp_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return exp(buf[idx]);
}

/*test functiosn for log*/
struct libm_test_funcs test_exp_funcs[LIBM_FUNC_MAX] =
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
                                         },
                          /*
                         .special      = { .setup = test_logf_special_setup,
                                           .run = test_logf_special,
                                           .verify = test_log_verify
                                          },
                          */
                         .conformance  = {.setup = test_exp_conf_setup,
                                           .run   = libm_test_conf,
                                           .verify = test_exp_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_test_perf_setup,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = libm_test_accu_setup,
                                          .run   = libm_test_accu,
                                         },
                          /*
                         .special      = {.setup = test_log_special_setup,
                                          .run   = test_log_special,
                                         },
                          */
                          .conformance  = {.setup = test_exp_conf_setup,
                                          .run   = libm_test_conf,
                                          .verify = test_exp_verify,
                                         },
     },

};


int test_exp_verify(struct libm_test *test, struct libm_test_result *result);

long double
test_exp_expl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return expl(d[idx]);
}

static struct libm_test
exp_template = {
    .name       = "exp",
    .nargs      = 1,
    .ulp_threshold = 4.0,
    .ops        = {
                    .ulp    = {.funcl = test_exp_expl},
                    .verify = test_exp_verify,
                    .callbacks = {
                                    .s1s = test_exp_cb_s1s,
                                    .s1d = test_exp_cb_s1d,
                                    .verify = test_exp_cb_verify,
                                    .accu_ranges = test_exp_cb_accu_ranges,
                                 },
                  },
};

#define EXP_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;
    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;
    if (!conf->test_types)
        conf->test_types = EXP_TEST_TYPES_ALL;
    ret = libm_tests_setup(c, test_exp_funcs, &exp_template);
    return ret;
}



