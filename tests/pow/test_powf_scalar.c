#include <string.h>                     /* for memcpy() */
#include <stdlib.h>
#include <math.h>                       /* for pow() poisining */

#include <libm_amd.h>
#include <libm_tests.h>
#include <bench_timer.h>
#include <libm/types.h>
#include "test_pow.h"

#define __TEST_POW_INTERNAL__
#include "test_powf_data.h"

double LIBM_FUNC(pow)(double,double);
float LIBM_FUNC(powf)(float, float);

extern int RANGE_LEN_X;

//verify for POW - Single Precision - Scalar
int libm_test_powf_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    float *expected = data->expected;
    float *input_x = data->input1;
    float *input_y = data->input2;
    for (uint32_t j = 0; j < data->nelem; j++) {
        expected[j] = powf(input_x[j],input_y[j]);
    }
    return libm_test_verify(test, result);
}

//single precision scalar
long double libm_test_powf(struct libm_test *test, int idx)
{
    float* in_x = test->test_data.input1;
    float* in_y = test->test_data.input2;
    return powf(in_x[idx],in_y[idx]);
}

/* scalar single precision */
static struct libm_test powf_test_template = {
    .name       = "powf_scalar",
    .nargs      = 2,
    .ulp_threshold = 0.5,
    .ops        = {
                   .ulp    = {.funcl = libm_test_powf},
                   .verify = libm_test_powf_verify,
                   },
};


extern struct libm_test_input_range x_range[];
extern struct libm_test_input_range y_range[];
extern struct libm_test_input_range x[];
extern struct libm_test_input_range y[];

//for single precision scalar
static int test_powf_accu(struct libm_test *test)
{

    struct libm_test_data *data = &test->test_data;
    float *ip_x = data->input1;
    float *ip_y = data->input2;
    float *op = data->output;
//    double *restrict op = GCC_ALIGN(data->output, 256);
    uint64_t sz = data->nelem;
//    int arr_sz = ARRAY_SIZE(accu_ranges);
    int arr_sz = RANGE_LEN_X;
    if (sz % 4 != 0)
        printf("%s %s : %ld is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int i = 0; i < arr_sz ; i++) {
        test->conf->inp_range[0] = x_range[i];// range for x
        test->conf->inp_range[1] = y_range[i]; //range for y
        test_pow_populate_inputs(test, 1);

        LIBM_TEST_DPRINTF(VERBOSE2,
                          "Testing for accuracy %d items in range x= [%Lf, %Lf] y= [%Lf, %Lf]\n",
                          test->test_data.nelem,
                          x_range[i].start, x_range[i].stop,y_range[i].start,y_range[i].stop);


        for (uint64_t j = 0; j < sz; j ++) {
            op[j] = LIBM_FUNC(powf)(ip_x[j],ip_y[j]);
	    //printf("%f\t%f\t%f\n",ip_x[j], ip_y[j], op[j]);
        }

        libm_test_powf_verify(test, &test->result);
    }
	return 0;
}

//TEST -- POW func for single prec scalar
static int test_powf_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    float *restrict ip1 = data->input1;
    float *restrict ip2 = data->input2;

    float *restrict o = data->output;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    test->conf->inp_range[0] = x[0];// range for x
    test->conf->inp_range[1] = y[0]; //range for y
    test_pow_populate_inputs(test, 1);

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        for (uint32_t j = 0; j < sz; j ++) {
            o[j] =  LIBM_FUNC(powf)(ip1[j],ip2[j]);

		//test->ops.libm_func_callback(test, j);

        }
        
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}


static int test_pow_setup_s_s(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    int ret = test_pow_alloc_init(conf,  test);
    return ret;
}

//for single precision scalar values - for pow
static int test_powf_special_setup(struct libm_test *test)
{
    int test_data_size = ARRAY_SIZE(libm_test_powf_special_data);
    struct libm_test_data *data;
    float *in_x,*in_y,*expected;
    flt32u_t x,y,z;
    // Just trying to get rid of warning/errors
    test_pow_alloc_data(test, test_data_size);

    data = &test->test_data;
    in_x = data->input1;
    in_y = data->input2;
    expected = data->expected;
    for (int i = 0; i < test_data_size; i++) {
	x.i = libm_test_powf_special_data[i].x;
        y.i = libm_test_powf_special_data[i].y;
        z.i = libm_test_powf_special_data[i].out;
        in_x[i] = x.f;
        in_y[i] = y.f;
        expected[i] = z.f;
    }

    return 0;
}

//SCALAR Single precision for pow
static int test_powf_special(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    float *ip1 = data->input1;
    float *ip2 = data->input2;
    float *op =  data->output;
    int sz = data->nelem;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < sz; j += 1) {
        op[j] = LIBM_FUNC(powf)(ip1[j],ip2[j]);
    }

    return 0;
}

int test_powf_conformance_setup(struct libm_test *test)
{
    int test_data_size = ARRAY_SIZE(libm_test_powf_conformance_data);
    struct libm_test_data *data;
    float *in_x,*in_y,*expected;
    int32_t *expected_exception;
    // Just trying to get rid of warning/errors
    test_pow_alloc_data(test, test_data_size);

    data = &test->test_data;
    in_x = data->input1;
    in_y = data->input2;
    expected = data->expected;
    expected_exception = data->expected_exception;
    flt32u_t x,y,z;
    for (int i = 0; i < test_data_size; i++) {
        x.i = libm_test_powf_conformance_data[i].x;
	y.i = libm_test_powf_conformance_data[i].y;
	z.i = libm_test_powf_conformance_data[i].out;
    	in_x[i] = x.f;
        in_y[i] = y.f;
        expected[i] = z.f;
	expected_exception[i] = libm_test_powf_conformance_data[i].exception_flags;
    }

    return 0;
}


static int test_powf_conformance(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    float *ip1 = data->input1;
    float *ip2 = data->input2;
    float *op =  data->output;
    int *exception = data->raised_exception;
    int sz = data->nelem;
//    int flags=0;
    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < sz; j += 1) {
	feclearexcept(FE_ALL_EXCEPT);

 	op[j] = LIBM_FUNC(powf)(ip1[j],ip2[j]);
        const int flags =  fetestexcept(FE_ALL_EXCEPT);
	exception[j] = flags;
    }

    return 0;
}


//For single precision Scalar
int test_pow_init_s_s(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_type = conf->test_types;

    while (test_type) {
        struct libm_test *test = libm_test_alloc_init(conf, &powf_test_template);

        if (!test)
            return -1;

        test->variant = LIBM_FUNC_S_S;
        test->name = "powf_scalar";
	test->input_name = "s1f";
        test->nargs = 2;
	test->ulp_threshold = 0.5;

        uint32_t bit = 1 << (ffs(test_type) - 1);

        switch (bit) {
        case TEST_TYPE_PERF:
	    test->test_type = TEST_TYPE_PERF;
            test->type_name = "perf";
            test->ops.setup = test_pow_setup_s_s;
            test->ops.run = test_powf_perf;
            test->ops.verify = libm_test_powf_verify;
            break;

	case TEST_TYPE_ACCU:
	    test->test_type = TEST_TYPE_ACCU;
            test->type_name = "accuracy";
            test->ops.setup = test_pow_setup_s_s;
            test->ops.run = test_powf_accu;
            test->ops.verify = libm_test_powf_verify;; // No verify after, will verify inside.
            break;

	case TEST_TYPE_SPECIAL:
            test->test_type = TEST_TYPE_SPECIAL;
            test->type_name = "special";
            test->ops.run = test_powf_special;
            test->ops.setup = test_powf_special_setup;
	    test->ops.verify = libm_test_powf_verify;
            break;

        case TEST_TYPE_CONFORMANCE:
            test->test_type = TEST_TYPE_CONFORMANCE;
            test->type_name = "conformance";
            test->ops.run = test_powf_conformance;
            test->ops.setup = test_powf_conformance_setup;
            test->ops.verify = libm_test_powf_verify;
            break;

	default:
            LIBM_TEST_DPRINTF(INFO, "Unknown test type for scalar\n");
            goto skip_this;
        }

        ret = test_pow_register_one(test);

        if (ret)
            goto out;
skip_this:
        test_type &= (test_type - 1);
    }

out:
    return ret;

}



