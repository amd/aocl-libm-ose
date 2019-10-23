#include <stdio.h>
#include <float.h>              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>            /* for ffs() */
#include <string.h>             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>

#include <libm/compiler.h>       /* for FALLTHROUGH */

#include "test_atan.h"
#include "test_atan_data.h"

#ifdef LIBM_FUNC
#undef LIBM_FUNC
#endif

#define LIBM_FUNC(x) FN_PROTOTYPE(x)

char doc[] = BUILD_TEST_DOC(TEST_NAME);

extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(atan)(double);

/*atan accuracy setup*/
int test_atan_accu_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);
    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
        goto out;
    }

    test->ulp_threshold = 2.0;

    return 0;
out:
    return ret;
}

//alloc special data
int test_atan_alloc_special_data(struct libm_test *test, size_t size)
{
    struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data = &test->test_data;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, size);

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

/*atan conformance setup*/
int test_atan_conformance_setup(struct libm_test *test){
    int test_data_size = ARRAY_SIZE(libm_test_atan_conformance_data);
    double *in1, *expected;
    struct libm_test_data *data;
    int i;
    int32_t* expected_exception;

    test_atan_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (double*)data->input1;
    expected = (double*)data->expected;
    expected_exception =  data->expected_exception;
    flt64u_t x,z;
    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_atan_conformance_data[i].in;
       z.i = libm_test_atan_conformance_data[i].out;
       in1[i] = x.d;
       expected[i] = z.d;
       expected_exception[i] = libm_test_atan_conformance_data[i].exception_flags;
    }

    return 0;

}

/*atan conformance setup*/
int test_atanf_conformance_setup(struct libm_test *test){
    int test_data_size = ARRAY_SIZE(libm_test_atanf_conformance_data);
    float *in1, *expected;
    struct libm_test_data *data;
    int i;
    int32_t* expected_exception;

    test_atan_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (float*)data->input1;
    expected = (float*)data->expected;
    expected_exception =  data->expected_exception;
    flt32u_t x,z;
    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_atanf_conformance_data[i].in;
       z.i = libm_test_atanf_conformance_data[i].out;
       in1[i] = x.f;
       expected[i] = z.f;
       expected_exception[i] = libm_test_atanf_conformance_data[i].exception_flags;
    }

    return 0;
}

/*special cases for atan*/
/*
static int test_atan_special(struct libm_test *test)
{

    int ret = 0;
    struct libm_test_data *data = &test->test_data;
    int sz = data->nelem;

    double *ip = (double*)data->input1;
    double *op = (double*)data->output;
    test->ops.verify = NULL ;

    if (sz % 4 != 0)
       LIBM_TEST_DPRINTF(DBG2,
                          "%s %s : %d is not a multiple of 4, some may be left out\n"
                          " And error reported may not be real for such entries\n",
                          test->name, test->type_name, sz);

    for (int j = 0; j < sz; j++)
        op[j] = LIBM_FUNC(atan)(ip[j]);

    ret = libm_test_verify(test, &test->result);

    return ret;
}
*/

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
test_atan_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;
    _o[idx] = LIBM_FUNC(atanf)(_ip1[idx]);
    return 0;
}

static int
test_atan_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;
    _o[idx] = LIBM_FUNC(atan)(_ip1[idx]);
    return 0;
}

/*verify*/
static int
test_atan_cb_verify(struct libm_test *test, int j)
{
    int ret = 0;
    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];
        ret = __generate_test_one_range(test, range);
        ret = test_atan_verify(test, &test->result);
    }
    return ret;
}

static int
test_atan_cb_accu_ranges(struct libm_test *test, int j)
{
    int arr_sz = ARRAY_SIZE(atan_accu_ranges);
    int ret = 0;
    for (int i = 0; i < arr_sz; i++) {
        if ((atan_accu_ranges[i].start == 0.0) && (atan_accu_ranges[i].stop == 0.0) )
            break;
    ret = __generate_test_one_range(test, &atan_accu_ranges[i]);
    if(ret)
        return ret;
    ret = test_atan_verify(test, &test->result);
    }
    return 0;
}

/*ulp*/
double test_atan_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return atan(buf[idx]);
}

/*test functiosn for atan*/
struct libm_test_funcs test_atan_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance =  { .setup = libm_setup_scalar_perf,
                                           .run   = libm_test_s1s_perf,
                                         },
                         .accuracy     = { .setup = test_atan_accu_setup,
                                           .run   = libm_test_accu,
                                         },
                          /*
                         .special      = { .setup = test_atanf_special_setup,
                                           .run = test_atanf_special,
                                           .verify = test_atan_verify
                                          },
                          */
                         .conformance  = {.setup = test_atanf_conformance_setup,
                                           .run   = libm_test_s1s_conf,
                                           .verify = test_atan_verify
                                         },
     },
     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = libm_setup_scalar_perf,
                                          .run   = libm_test_s1d_perf,
                                        },
                         .accuracy     = {.setup = test_atan_accu_setup,
                                          .run   = libm_test_accu,
                                         },
                          /*
                         .special      = {.setup = test_atan_special_setup,
                                          .run   = test_atan_special,
                                         },
                          */
                          .conformance  = {.setup = test_atan_conformance_setup,
                                          .run   = libm_test_s1d_conf,
                                          .verify = libm_test_verify
                                         },
     },
};


int test_atan_verify(struct libm_test *test, struct libm_test_result *result);

long double
test_atan_atanl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return atanl(d[idx]);
}

static struct libm_test
atan_template = {
    .name       = "atan",
    .nargs      = 1,
    .ulp_threshold = 4.0,
    .ops        = {
                    .ulp    = {.funcl = test_atan_atanl},
                    .verify = test_atan_verify,
                    .callbacks = {
                                    .s1s = test_atan_cb_s1s,
                                    .s1d = test_atan_cb_s1d,
                                    .verify = test_atan_cb_verify,
                                    .accu_ranges = test_atan_cb_accu_ranges,
                                 },
                  },
};

#define ATAN_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

/*init function*/
int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;
    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;
    if (!conf->test_types)
        conf->test_types = ATAN_TEST_TYPES_ALL;
    ret = libm_tests_setup(c, test_atan_funcs, &atan_template);
    return ret;
}

