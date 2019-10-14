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
#include "test_fabs_d_data.h"

//char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];

double LIBM_FUNC(fabs)(double);
//float LIBM_FUNC(fabsf)(float);

long double libm_test_fabs(struct libm_test *test, int idx)
{
    float* in_x = test->test_data.input1;
    return fabs(in_x[idx]);
}

/* scalar double precision */
static struct libm_test fabs_test_template = {
    .name       = "fabs_d_scalar",
    .nargs      = 2,
    .ulp_threshold = 0.5,
    .ops        = {
                   .ulp    = {.funcl = libm_test_fabs},
                   .verify = libm_test_fabs_verify,
                   },
};

//perf
int test_fabs_s1d_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = (double*)data->output;
    double *restrict ip1 = (double*)data->input1;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        uint32_t j;
        for (j = 0; j < sz; j++) {
            o[j] = LIBM_FUNC(fabs)(ip1[j]);
	    //printf("%f\t%f\n", ip1[j], o[j]);
        }
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

//conformance setup
int test_fabs_conformance_setup(struct libm_test *test)
{
	int test_data_size = ARRAY_SIZE(libm_test_fabs_conformance_data);
	double *inp, *expected;
	struct libm_test_data *data;
	int i;
	int32_t* expected_exception;

	test_fabs_alloc_special_data(test, test_data_size);

	data = &test->test_data;

	inp = (double*)data->input1;
	expected = (double*)data->expected;
	expected_exception = data->expected_exception;

	flt64u_t x,z;

	//populate exception test data
	for(i=0; i < test_data_size; i++)
	{
		x.i = libm_test_fabs_conformance_data[i].x;
		z.i = libm_test_fabs_conformance_data[i].out;
		inp[i] = x.d;
		expected[i] = z.d;
		expected_exception[i] = libm_test_fabs_conformance_data[i].exception_flags;
	}

	return 0;
}

//conformance test
int test_fabs_conformance(struct libm_test *test)
{
	int ret=0;
	struct libm_test_data *data = &test->test_data;
	int sz = data->nelem;

	double *ip = (double*)data->input1;
	double *op = (double*)data->output;
	int* exception = data->raised_exception;

	test->ops.verify = NULL;

	if (sz % 4 != 0)
       LIBM_TEST_DPRINTF(DBG2,
                          "%s %s : %d is not a multiple of 4, some may be left out\n"
                          " And error reported may not be real for such entries\n",
                          test->name, test->type_name, sz);

	for (int j = 0; j < sz; j++)
	{
        	feclearexcept(FE_ALL_EXCEPT);

		op[j] = LIBM_FUNC(fabs)(ip[j]);
		const int flags = fetestexcept(FE_ALL_EXCEPT);
		exception[j] = flags;
	}

	ret = libm_test_verify(test, &test->result);

	return ret;
}

//set up double precision scalar - should be in common
int test_fabs_setup_s_d(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    int ret = test_fabs_alloc_init(conf,  test);

    if (ret)
    {
	LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test data for fabs setup\n");
	goto out;
    }
    ret = libm_test_populate_inputs(test, LIBM_INPUT_RANGE_SIMPLE);

    if(ret || !test->test_data.input1)
    {
	LIBM_TEST_DPRINTF(PANIC, "Unable to populate test data for fabs setup\n");
	goto out;
    }

    return 0;

out: return -1;
}

//accu setup -- should be in common?
int test_fabs_accu_setup(struct libm_test *test)
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

//accu test for scalar double precision fabs
int test_fabs_accu(struct libm_test *test)
{

    struct libm_test_data *data = &test->test_data;
    double *ip_x = data->input1;
    double *op = data->output;
    uint64_t sz = data->nelem;
    int arr_sz = RANGE_LEN_X;
    if (sz % 4 != 0)
        printf("%s %s : %ld is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int i = 0; i < arr_sz ; i++) {
        test->conf->inp_range[0] = x_range[i];// range for x
        test_fabs_populate_inputs(test, 1);

        LIBM_TEST_DPRINTF(VERBOSE2,
                          "Testing for accuracy %d items in range x= [%Lf, %Lf]\n",
                          test->test_data.nelem,
                          x_range[i].start, x_range[i].stop);


        for (uint64_t j = 0; j < sz; j ++) {
            op[j] = LIBM_FUNC(fabs)(ip_x[j]);
	    //printf("%f\t%f\n",ip_x[j], op[j]);
        }

        libm_test_fabs_verify(test, &test->result);
    }
	return 0;
}

//init scalar double precision
int test_fabs_init_s_d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_type = conf->test_types;

    while (test_type) {
        struct libm_test *test = libm_test_alloc_init(conf, &fabs_test_template);

        if (!test)
            return -1;

        test->variant = LIBM_FUNC_S_D;
        test->name = "fabs_d_scalar";
	test->input_name = "s1d";
        test->nargs = 2;
	test->ulp_threshold = 0.5;

        uint32_t bit = 1 << (ffs(test_type) - 1);

        switch (bit) {
        case TEST_TYPE_PERF:
	    test->test_type = TEST_TYPE_PERF;
            test->type_name = "perf";
            test->ops.setup = test_fabs_setup_s_d;
            test->ops.run = test_fabs_s1d_perf;
            test->ops.verify = libm_test_fabs_verify;
            break;

	case TEST_TYPE_ACCU:
	    test->test_type = TEST_TYPE_ACCU;
            test->type_name = "accuracy";
            test->ops.setup = test_fabs_accu_setup;
            test->ops.run = test_fabs_accu;
            test->ops.verify = libm_test_fabs_verify;
            break;

/*	    
	case TEST_TYPE_SPECIAL:
            test->test_type = TEST_TYPE_SPECIAL;
            test->type_name = "special";
            test->ops.run = test_powf_special;
            test->ops.setup = test_powf_special_setup;
	    test->ops.verify = libm_test_powf_verify;
            break;
*/
        case TEST_TYPE_CONFORMANCE:
            test->test_type = TEST_TYPE_CONFORMANCE;
            test->type_name = "conformance";
            test->ops.run = test_fabs_conformance;
            test->ops.setup = test_fabs_conformance_setup;
            test->ops.verify = libm_test_fabs_verify;
            break;


	default:
            LIBM_TEST_DPRINTF(INFO, "Unknown test type for fabs double precision scalar\n");
            goto skip_this;
        }

        ret = test_fabs_register_one(test);

        if (ret)
            goto out;
skip_this:
        test_type &= (test_type - 1);
    }

out:
    return ret;

}


