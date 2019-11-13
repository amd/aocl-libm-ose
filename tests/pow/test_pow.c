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

#include "test_pow.h"
#include "test_pow_data.h"

char doc[] = BUILD_TEST_DOC(TEST_NAME);

double LIBM_FUNC(pow)(double, double);
float LIBM_FUNC(powf)(float, float);

/*conf setup*/
int test_pow_conf_setup(struct libm_test *test)
{
    int ret=0;
    int test_data_size=0;
    if(test_is_single_precision(test)) {
        test_data_size=ARRAY_SIZE(libm_test_powf_conformance_data);
        ret=libm_setup_s1s_conf_2(test, libm_test_powf_conformance_data, test_data_size);
    }
    else {
       test_data_size=ARRAY_SIZE(libm_test_pow_conformance_data);
       ret=libm_setup_s1d_conf_2(test, libm_test_pow_conformance_data, test_data_size);
    }
    return ret;
}

/*special setup*/
int test_pow_special_setup(struct libm_test *test)
{
    int ret=0;
    int test_data_size=0;
    if(test_is_single_precision(test)) {
        test_data_size=ARRAY_SIZE(libm_test_powf_special_data);
        ret=libm_setup_s1s_special_2(test, libm_test_powf_special_data, test_data_size);
    }
    else {
        test_data_size = ARRAY_SIZE(libm_test_pow_special_data);
        ret = libm_setup_s1d_special_2(test, libm_test_pow_special_data, test_data_size);
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

static int
generate_test_pow_range(struct libm_test *test, struct libm_test_input_range *range_x,  struct libm_test_input_range *range_y )
{
    int ret = 0;
    test->conf->inp_range[0] = *range_x;
    test->conf->inp_range[1] = *range_y;

     LIBM_TEST_DPRINTF(DBG2,
                       "Testing for accuracy %d items in range x = [%Lf, %Lf] y = [%Lf, %Lf]\n",
                       test->test_data.nelem,
                       range_x->start, range_x->stop,
                       range_y->start, range_y->stop);
    ret = libm_test_populate_inputs(test, range_x->type);
     if (test_is_single_precision(test))
         ret = libm_test_accu_single(test, test->variant);
     else
         ret = libm_test_accu_double(test, test->variant);
     return ret;
}

static int
test_pow_cb_accu_ranges(struct libm_test *test, int j)
{
    int ret = 0;
    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];
        ret = generate_test_one_range(test, range);
        ret = test_pow_verify(test, &test->result);
        return ret;
    }

    /*Size of both x_range and y_range need to be the same */
    if(ARRAY_SIZE(x_range) != ARRAY_SIZE(y_range)) {
        printf("Size of x range and y range not the same\n");
        printf("Please check test_pow_data.h file and ensure that x_range and y_range are the same\n");
        return -1;
    }
    int arr_sz = ARRAY_SIZE(x_range);

    for (int i = 0; i < arr_sz; i++) {
        if ((x_range[i].start == 0.0) && (x_range[i].stop == 0.0) )
            break;
        if ((y_range[i].start == 0.0) && (y_range[i].stop == 0.0) )
            break;
        ret = generate_test_pow_range(test, &x_range[i], &y_range[i]);
        if(ret)
            return ret;
        ret = test_pow_verify(test, &test->result);
    }
    return 0;
}

/*ulp*/
double test_pow_ulp(struct libm_test *test, int idx)
{
    float *buf1 = (float*)test->test_data.input1;
    float *buf2 = (float*)test->test_data.input2;
    return pow(buf1[idx], buf2[idx]);
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
                                           .run   = libm_test_accu,
                                           .ulp = {.func = test_pow_ulp}
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
                                          .run   = libm_test_accu,
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
};


int test_pow_verify(struct libm_test *test, struct libm_test_result *result);

long double
test_pow_powl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    double *d1 = (double*)test->test_data.input2;
    return powl(d[idx], d1[idx]);
}

static struct libm_test
pow_template = {
    .name       = "pow",
    .nargs      = 2,
    .ulp_threshold = 0.5,
    .ops        = {
                    .ulp    = {.funcl = test_pow_powl},
                    .verify = test_pow_verify,
                    .callbacks = {
                                    .s1s = test_pow_cb_s1s,
                                    .s1d = test_pow_cb_s1d,
                                    .accu_ranges = test_pow_cb_accu_ranges,
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



