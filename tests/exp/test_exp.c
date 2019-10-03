#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */
#include <string.h>                             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>
#include <libm/compiler.h>

#include "test_exp.h"
#define __TEST_EXP2_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_exp_data.h"
#include "test_data_exact.h"
#include "test_data_worst.h"
#include "test_exceptions.h"

/* GLIBC prototype declarations */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

#define _ZGVdN4v(x) _ZGVbN4v_##x
#define _ZGVsN4v_expf _ZGVbN4v_expf
#define _ZGVdN4v_exp _ZGVdN4v_exp
#define _ZGVdN2v_exp _ZGVbN2v_exp

__m128d LIBM_FUNC_VEC(d, 2, exp)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, exp)(__m256d);

__m128 LIBM_FUNC_VEC(b, 4, expf)(__m128);
__m256 LIBM_FUNC_VEC(b, 8, expf)(__m256);
#endif

#if (DEVELOPER==2)
#pragma message "Developer mode changing prototype to exp_v2()"
#undef LIBM_FUNC
#define LIBM_FUNC(x) FN_PROTOTYPE( x ## _v2 )

double FN_PROTOTYPE( exp_v2 )(double);
float FN_PROTOTYPE( expf_v2 )(float);

float FN_PROTOTYPE_FMA3( expf )(float);

#define amd_expf_v2 FN_PROTOTYPE_FMA3(expf)
#else
// Temporary need to remove
//#undef LIBM_FUNC
//#define LIBM_FUNC(x) FN_PROTOTYPE_FMA3(x)
//#define amd_expf FN_PROTOTYPE_FMA3(expf)
//#define amd_exp_bas64 FN_PROTOTYPE_FMA3(expf)
#endif

int test_exp_populate_inputs(struct libm_test *test, int use_uniform);

static int test_exp_perf_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);
    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
        goto out;
    }
    ret = libm_test_populate_inputs(test, LIBM_INPUT_RANGE_SIMPLE);

    if (ret || !test->test_data.input1) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to populate test_data\n");
        goto out;
    }

    return 0;

 out:
    return -1;
}

static int test_exp_accu_setup(struct libm_test *test)
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

static int test_exp_alloc_special_data(struct libm_test *test, size_t size)
{
    struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data;
    int ret = 0;

    libm_test_alloc_test_data(test, size);

    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
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

static int test_exp_conformance_setup(struct libm_test *test){
    int test_data_size = ARRAY_SIZE(libm_test_exp_conformance_data);
    double *in1, *expected;
    struct libm_test_data *data;
    int i;
    int32_t* expected_exception;

    test_exp_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (double*)data->input1;
    expected = (double*)data->expected;
    expected_exception =  data->expected_exception;
    flt64u_t x,z;
    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_exp_conformance_data[i].input;
       z.i = libm_test_exp_conformance_data[i].output;
       in1[i] = x.d;
       expected[i] = z.d;
       expected_exception[i] = libm_test_exp_conformance_data[i].exception_flags;
    }

    return 0;

}

/*for expf*/
static int test_expf_conformance_setup(struct libm_test *test){
    int test_data_size = ARRAY_SIZE(libm_test_expf_conformance_data);
    float *in1, *expected;
    struct libm_test_data *data;
    int i;
    int32_t* expected_exception;

    test_exp_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (float*)data->input1;
    expected = (float*)data->expected;
    expected_exception =  data->expected_exception;
    flt32u_t x,z;
    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_expf_conformance_data[i].in;
       z.i = libm_test_expf_conformance_data[i].out;
       in1[i] = x.f;
       expected[i] = z.f;
       expected_exception[i] = libm_test_expf_conformance_data[i].exception_flags;
    }

    return 0;

}

static int test_exp_conformance(struct libm_test *test)
{
    int ret = 0;
    struct libm_test_data *data = &test->test_data;
    int sz = data->nelem;

    double *ip = (double*)data->input1;
    double *op = (double*)data->output;
    int *exception = data->raised_exception;
    //test->ops.verify = NULL ;

    if (sz % 4 != 0)
       LIBM_TEST_DPRINTF(DBG2,
                          "%s %s : %d is not a multiple of 4, some may be left out\n"
                          " And error reported may not be real for such entries\n",
                          test->name, test->type_name, sz);

    for (int j = 0; j < sz; j++){
        feclearexcept(FE_ALL_EXCEPT);
        op[j] = LIBM_FUNC(exp)(ip[j]);
        const int flags =  fetestexcept(FE_ALL_EXCEPT);
        exception[j] = flags;
    }


    //ret = libm_test_verify(test, &test->result);

    return ret;

}

/*for expf*/
static int test_expf_conformance(struct libm_test *test)
{

    int ret = 0;
    struct libm_test_data *data = &test->test_data;
    int sz = data->nelem;

    float *ip = (float*)data->input1;
    float *op = (float*)data->output;
    int *exception = data->raised_exception;
    //test->ops.verify = NULL ;

    if (sz % 4 != 0)
       LIBM_TEST_DPRINTF(DBG2,
                          "%s %s : %d is not a multiple of 4, some may be left out\n"
                          " And error reported may not be real for such entries\n",
                          test->name, test->type_name, sz);

    for (int j = 0; j < sz; j++){
        feclearexcept(FE_ALL_EXCEPT);
        op[j] = LIBM_FUNC(expf)(ip[j]);
        const int flags =  fetestexcept(FE_ALL_EXCEPT);
        exception[j] = flags;
    }


    //ret = libm_test_verify(test, &test->result);

    return ret;


}

/*for expf*/
static int test_expf_special_setup(struct libm_test *test)
{
    int special_tbl_sz = ARRAY_SIZE(test_expf_special_data);
    int worst_tbl_sz = ARRAY_SIZE(test_data_exp_worst);
    int exact_tbl_sz = ARRAY_SIZE(test_data_exp_exact);

    int test_data_size = special_tbl_sz + worst_tbl_sz +
                                       exact_tbl_sz ;

    float *in1, *expected;
    struct libm_test_data *data;
    int i, start, end;

    test_exp_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (float*)data->input1;
    expected = (float*)data->expected;

    // Populate special data
    for (i = 0; i < special_tbl_sz; i++) {
       in1[i] = test_expf_special_data[i].in;
       expected[i] = test_expf_special_data[i].out;
    }

    // Populate test_data_exp_worst
    start = special_tbl_sz;
    end = start + worst_tbl_sz;
    for (i = start; i < end ; i++) {
       in1[i] = test_data_exp_worst[i - start].input;
       expected[i] = test_data_exp_worst[i - start].output;
    }

    // Populate test_data_exp_exact
    start = end;
    end += exact_tbl_sz;
    for (i = start; i < end; i++) {
       in1[i] = test_data_exp_exact[i - start].input;
       expected[i] = test_data_exp_exact[i - start]. output;
    }

    return 0;

}

static int test_exp_special_setup(struct libm_test *test)
{
    int special_tbl_sz = ARRAY_SIZE(test_exp_special_data);
    int worst_tbl_sz = ARRAY_SIZE(test_data_exp_worst);
    int exact_tbl_sz = ARRAY_SIZE(test_data_exp_exact);

    int test_data_size = special_tbl_sz + worst_tbl_sz +
                                       exact_tbl_sz ;

    double *in1, *expected;
    struct libm_test_data *data;
    int i, start, end;

    test_exp_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (double*)data->input1;
    expected = (double*)data->expected;

    // Populate special data
    for (i = 0; i < special_tbl_sz; i++) {
       in1[i] = test_exp_special_data[i].in;
       expected[i] = test_exp_special_data[i].out;
    }

    // Populate test_data_exp_worst
    start = special_tbl_sz;
    end = start + worst_tbl_sz;
    for (i = start; i < end ; i++) {
       in1[i] = test_data_exp_worst[i - start].input;
       expected[i] = test_data_exp_worst[i - start].output;
    }

    // Populate test_data_exp_exact
    start = end;
    end += exact_tbl_sz;
    for (i = start; i < end; i++) {
       in1[i] = test_data_exp_exact[i - start].input;
       expected[i] = test_data_exp_exact[i - start]. output;
    }

    return 0;

}

/*special test for expf*/
static int test_expf_special(struct libm_test *test)
{
    int ret = 0;
    struct libm_test_data *data = &test->test_data;
    int sz = data->nelem;

    float *ip = (float*)data->input1;
    float *op = (float*)data->output;
    test->ops.verify = NULL;

    if (sz % 4 != 0)
       LIBM_TEST_DPRINTF(DBG2,
                         "%s %s : %d is not a multiple of 4, some may be left out \n"
                          " And error reported may not be real for such entries \n",
                           test->name, test->type_name, sz);

    for (int j = 0; j < sz; j++)
       op[j] = LIBM_FUNC(expf)(ip[j]);

    ret = libm_test_verify(test, &test->result);

   return ret;

}


/**************************
 * ACCURACY TESTS
 **************************/
#include "exp_accu_data.h"

static int __test_exp_accu(struct libm_test *test,
                            uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    double *ip = (double*)data->input1;
    double *op = (double*)data->output;
    int sz = data->nelem, end = sz;

    switch(type) {
    case LIBM_FUNC_V2D: end = sz - 1; break;
    case LIBM_FUNC_V4D: end = sz - 3; break;
    default: break;
    }

    for (int j = 0; j < end;) {
        __m128d ip2, op2;
        __m256d ip4, op4;
        switch (type) {
        case LIBM_FUNC_V2D:
            ip2 = _mm_set_pd(ip[j+1], ip[j]);
            op2 = LIBM_FUNC_VEC(d, 2, exp)(ip2);
            _mm_store_pd(&op[j], op2);
            j += 2;
            break;
        case LIBM_FUNC_V4D:
            ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = LIBM_FUNC_VEC(d, 4, exp)(ip4);
            _mm256_store_pd(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_D:
            op[j] = LIBM_FUNC(exp)(ip[j]);
            //op[j] = exp2(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
}

static int __test_expf_accu(struct libm_test *test,
                            uint32_t type)
{
    struct libm_test_data *data = &test->test_data;
    float *ip = (float*)data->input1;
    float *op = (float*)data->output;
    int sz = data->nelem, end = sz;

    switch(type) {
    case LIBM_FUNC_V2D: end = sz - 1; break;
    case LIBM_FUNC_V4D: end = sz - 3; break;
    default: break;
    }

    for (int j = 0; j < end;) {
        __m128 ip4, op4;
        switch (type) {
        case LIBM_FUNC_V4S:
            ip4 = _mm_set_ps(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            op4 = LIBM_FUNC_VEC(s, 4, expf)(ip4);
            _mm_store_ps(&op[j], op4);
            j += 4;
            break;
        case LIBM_FUNC_S_S:
            op[j] = LIBM_FUNC(expf)(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
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
        ret = __test_expf_accu(test, test->variant);
    else
        ret = __test_exp_accu(test, test->variant);

    return ret;
}

static int test_exp_accu(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    int ret = 0;
    int sz = data->nelem;

    /* we are verifying here, no need to do again */
    test->ops.verify = NULL;

    if (sz % 4 != 0)
        LIBM_TEST_DPRINTF(DBG2,
                          "%s %s : %d is not a multiple of 4, some may be left out\n"
                          " And error reported may not be real for such entries\n",
                          test->name, test->type_name, sz);

    if (test->conf->inp_range[0].start ||
        test->conf->inp_range[0].stop) {
        struct libm_test_input_range *range = &test->conf->inp_range[0];

        ret = __generate_test_one_range(test, range);

        ret = test_exp_verify(test, &test->result);

        return ret;
    }


    int arr_sz = ARRAY_SIZE(exp2_accu_ranges);

    for (int i = 0; i < arr_sz ; i++) {
        if ((exp2_accu_ranges[i].start == 0.0) && (exp2_accu_ranges[i].stop == 0.0) )
            break;

        ret = __generate_test_one_range(test, &exp2_accu_ranges[i]);
        if (ret)
            return ret;

        ret = test_exp_verify(test, &test->result);
    }

    return 0;
}


static int test_exp_special(struct libm_test *test)
{
    int ret = 0;
    struct libm_test_data *data = &test->test_data;
    int sz = data->nelem;

    double *ip = (double*)data->input1;
    double *op = (double*)data->output;
    test->ops.verify = NULL;

    if (sz % 4 != 0)
       LIBM_TEST_DPRINTF(DBG2,
                         "%s %s : %d is not a multiple of 4, some may be left out \n"
                          " And error reported may not be real for such entries \n",
                           test->name, test->type_name, sz);

    for (int j = 0; j < sz; j++)
       op[j] = LIBM_FUNC(exp)(ip[j]);

    ret = libm_test_verify(test, &test->result);

   return ret;

}

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

static int
test_exp_cb_v4s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

    __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m128 op4 = LIBM_FUNC_VEC(s, 4, expf)(ip4);
    _mm_store_ps(&o[j], op4);

    return 0;
}

static int
test_exp_cb_v2d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

    __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
    __m128d op4 = LIBM_FUNC_VEC(d, 2, exp)(ip2);
    _mm_store_pd(&o[j], op4);

    return 0;
}

static int
test_exp_cb_v4d(struct libm_test *test, int j)
{
	struct libm_test_data *data = &test->test_data;
	double *restrict ip1 = (double*)data->input1;
	double *restrict o = (double*)data->output;

	__m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
	__m256d op4 = LIBM_FUNC_VEC(d, 4, exp)(ip4);
	_mm256_store_pd(&o[j], op4);

	return 0;
}


double test_exp_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return exp(buf[idx]);
}


struct libm_test_funcs test_exp_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance = { .setup = test_exp_perf_setup,
					  .run   = libm_test_s1s_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,
                                          .ulp   = {.func = test_exp_ulp},
                         		},
                         .special      = {.setup = test_expf_special_setup,
			 		.run = test_expf_special,
					.verify = test_exp_verify
					},			

			.conformance  = {.setup = test_expf_conformance_setup,
                                          .run   = test_expf_conformance,
			 		  .verify = test_exp_verify},

     },

     [LIBM_FUNC_S_D]  = {
                         .performance = { .setup = test_exp_perf_setup,
					  .run   = libm_test_s1d_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,},
                         .special      = {.setup = test_exp_special_setup,
                                           .run   = test_exp_special,},
			 .conformance  = {.setup = test_exp_conformance_setup,
                                          .run   = test_exp_conformance,
			 		  .verify = libm_test_verify},
     },

#if 0
     [LIBM_FUNC_V2S] = {
                        .performance = {.setup = test_exp_perf_setup,
					.run   = libm_test_v2s_perf,},
                        .accuracy    = {.setup = test_exp_accu_setup,
                                        .run   = test_exp_v4s,},
                        .special     = {.setup = test_exp_special_setup,},
     },

     [LIBM_FUNC_V8S]  = {
                         .performance = { .setup = test_exp_perf_setup,
                                          .run   = test_exp_v8s_perf,},
                         .accuracy    = {.setup = test_exp_accu_setup,
                                         .run   = test_exp_v4s,},
                         .special     = {.setup = test_exp_special_setup,},
     },
#endif
     [LIBM_FUNC_V4S]  = {
                         .performance = { .setup = test_exp_perf_setup,
					  .run   = libm_test_v4s_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,},
                         .special      = {.setup = test_exp_special_setup,
                                          .run = test_exp_accu},
     },

     [LIBM_FUNC_V2D]  = {
                         .performance = { .setup = test_exp_perf_setup,
					  .run   = libm_test_v2d_perf,},
                         .accuracy     = {.setup = test_exp_accu_setup,
                                          .run   = test_exp_accu,},
                         .special      = {.setup = test_exp_special_setup,
                                          .run   = test_exp_accu,},
     },

     [LIBM_FUNC_V4D] = {
                        .performance = {.setup = test_exp_perf_setup,
					.run   = libm_test_v4d_perf,},
                        .accuracy     = {.setup = test_exp_accu_setup,
                                         .run   = test_exp_accu,},
                        .special      = {.setup = test_exp_special_setup,
                                         .run   = test_exp_accu,},
     },


};

/* There is no exp2q in recent versions of gcc */
long double
test_exp_expl(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return expl(d[idx]);
}

int test_exp_verify(struct libm_test *test, struct libm_test_result *result);

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
			.v4s = test_exp_cb_v4s,
			.v2d = test_exp_cb_v2d,
			.v4d = test_exp_cb_v4d,
                },
	},
};


#define EXP2_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)


int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;

    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;

    if (!conf->test_types)
        conf->test_types = EXP2_TEST_TYPES_ALL;

    ret = libm_tests_setup(c, test_exp_funcs, &exp_template);

    return ret;
}
