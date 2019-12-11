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
#include <libm/compiler.h>

#include "test_expm1.h"
#define __TEST_EXP2_INTERNAL__                   /* needed to include expm1-test-data.h */
#include "test_expm1_data.h"

/* GLIBC prototype declarations */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)

//#define _ZGVdN4v(x) _ZGVbN4v_##x
//#define _ZGVsN4v_expm1f _ZGVbN4v_expm1f
//#define _ZGVdN4v_expm1 _ZGVdN4v_expm1
//#define _ZGVdN2v_expm1 _ZGVbN2v_expm1

//__m128d LIBM_FUNC_VEC(d, 2, expm1)(__m128d);
//__m256d LIBM_FUNC_VEC(d, 4, expm1)(__m256d);

//__m128 LIBM_FUNC_VEC(b, 4, expm1f)(__m128);
//__m256 LIBM_FUNC_VEC(b, 8, expm1f)(__m256);
#endif

/*vector routines*/
__m128d LIBM_FUNC_VEC(d, 2, expm1)(__m128d);
__m256d LIBM_FUNC_VEC(d, 4, expm1)(__m256d);

__m128 LIBM_FUNC_VEC(s, 4, expm1f)(__m128);
__m256 LIBM_FUNC_VEC(s, 8, expm1f)(__m256);

#if (DEVELOPER == 2)
#pragma message "Developer mode changing prototype to expm1_v2()"
#undef LIBM_FUNC
#define LIBM_FUNC(x) FN_PROTOTYPE( x ## _v2 )
#endif

#if (LIBM_PROTOTYPE == PROTOTYPE_AMDLIBM)
#define __amd_fma3_vrd4_expm1 amd_vrd4_expm1 /* As there is no __amd_fma3_vrs4_expm1 implementation yet */
#define __amd_fma3_vrs4_expm1 amd_vrs4_expm1f /* As there is no __amd_fma3_vrs4_expm1f implementation yet */


double FN_PROTOTYPE( expm1_v2 )(double);
float FN_PROTOTYPE( expm1f_v2 )(float);

float FN_PROTOTYPE_FMA3( expm1f )(float);

//#define amd_expm1f_v2 FN_PROTOTYPE_FMA3(expm1f)
#else
// Temporary need to remove
//#undef LIBM_FUNC
//#define LIBM_FUNC(x) FN_PROTOTYPE_FMA3(x)
//#define amd_expm1f FN_PROTOTYPE_FMA3(expm1f)
//#define amd_expm1_bas64 FN_PROTOTYPE_FMA3(expm1f)
#endif

int test_expm1_populate_inputs(struct libm_test *test, int use_uniform);

static int test_expm1_perf_setup(struct libm_test *test)
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

static int test_expm1_accu_setup(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);
    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data\n");
        goto out;
    }

    test->ulp_threshold = 0.54;

    return 0;
out:
    return ret;
}


static int test_expm1_alloc_special_data(struct libm_test *test, size_t size)
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

static int test_expm1_special_setup(struct libm_test *test)
{
    /*
     * structure contains both in and out values,
     * input only array size is half of original
     */
    int test_data_size = ARRAY_SIZE(test_expm1_special_data)/2;
    double *in1, *expected;

    struct libm_test_data *data;

    // Just trying to get rid of warning/errors
    test_expm1_alloc_special_data(test, test_data_size);

    data = &test->test_data;
    in1 = (double*)data->input1;
    expected = (double*)data->expected;

    for (int i = 0; i < test_data_size; i++) {
        in1[i] = test_expm1_special_data[i].in;
        expected[i] = test_expm1_special_data[i].out;
    }

    return 0;
}

/**************************
 * ACCURACY TESTS
 **************************/
#include "expm1_accu_data.h"

static int __test_expm1_accu(struct libm_test *test,
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
        switch (type) {
        case LIBM_FUNC_V2D:
        {
#if (LIBM_PROTOTYPE != PROTOTYPE_FMA3)
	    for (int k = 0; k < 2; k++)
                op[k] = expm1(ip[k]);
#else
            __m128d ip2 = _mm_set_pd(ip[j+1], ip[j]);
            LIBM_FUNC_VEC(d, 2, expm1)(2,&ip2,&op);
#endif
            j += 2;
         }
            break;
        case LIBM_FUNC_V4D:
	{
#if (LIBM_PROTOTYPE != PROTOTYPE_FMA3)
	    for (int k = 0; k < 2; k++)
		op[k] = expm1(ip[k]);
#else
            //__m256d ip4 = _mm256_set_pd(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            //__m256d op4 = LIBM_FUNC_VEC(d, 4, expm1)(ip4);
            //_mm256_store_pd(&op[j], op4);
#define __amd_fma3_vrd4_expm1 __amd_fma3_vrda_expm1
            LIBM_FUNC_VEC(d, 4, expm1)(4, &ip[j], &op[j]);
#undef __amd_fma3_vrd4_expm1
#endif
            j += 4;
	}
            break;
        case LIBM_FUNC_S_D:
            op[j] = LIBM_FUNC(expm1)(ip[j]);
            //op[j] = expm12(ip[j]);
            j++;
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "Testing type not valid: %d\n", type);
            return -1;
        }
    }

    return 0;
}

static int __test_expm1f_accu(struct libm_test *test,
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
        switch (type) {
        case LIBM_FUNC_V4S:
        {
#if (LIBM_PROTOTYPE != PROTOTYPE_FMA3)
	    for (int k = 0; k < 4; k++)
		op[k] = expm1f(ip[k]);
#else
            __m128 ip4 = _mm_set_ps(ip[j+3], ip[j+2], ip[j+1], ip[j]);
            __m128 op4 = LIBM_FUNC_VEC(s, 4, expm1f)(ip4);
            _mm_store_ps(&op[j], op4);
#endif
            j += 4;
        }
            break;
        case LIBM_FUNC_S_S:
            op[j] = LIBM_FUNC(expm1f)(ip[j]);
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
                                     struct libm_test_input_range *range)
{
    int ret = 0;

    LIBM_TEST_DPRINTF(DBG2,
                      "Testing for accuracy %d items in range [%Lf, %Lf]\n",
                      test->test_data.nelem,
                      range->start, range->stop);

    test->conf->inp_range[0] = *range;

    ret = libm_test_populate_inputs(test, range->type);

    if (test_is_single_precision(test))
        ret = __test_expm1f_accu(test, test->variant);
    else
        ret = __test_expm1_accu(test, test->variant);

    return ret;
}

static int test_expm1_accu(struct libm_test *test)
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

        ret = test_expm1_verify(test, &test->result);

        return ret;
    }


    int arr_sz = ARRAY_SIZE(accu_ranges);

    for (int i = 0; i < arr_sz; i++) {
        if ((accu_ranges[i].start = 0.0 &&
              accu_ranges[i].stop == 0.0) )
             break;

        ret = __generate_test_one_range(test, &accu_ranges[i]);
        if (ret)
            return ret;

        ret = test_expm1_verify(test, &test->result);
    }

    return 0;
}

double test_expm1_ulp(struct libm_test *test, int idx)
{
    float *buf = (float*)test->test_data.input1;
    return expm1(buf[idx]);
}

static int
test_expm1_cb_s1s(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict _ip1 = (float*)data->input1;
    float *restrict _o = (float*)data->output;

    _o[idx] = LIBM_FUNC(expm1f)(_ip1[idx]);

    return 0;
}

static int
test_expm1_cb_s1d(struct libm_test *test, int idx)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict _ip1 = (double*)data->input1;
    double *restrict _o = (double*)data->output;

    _o[idx] = LIBM_FUNC(expm1)(_ip1[idx]);

    return 0;
}

static int
test_expm1_cb_v4s(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    float *restrict ip1 = (float*)data->input1;
    float *restrict o = (float*)data->output;

#if (LIBM_PROTOTYPE != PROTOTYPE_FMA3)		/* Glibc doesnt have a vector version of expm1 */
    for (int k = 0; k < 4; k++)
        o[k] = LIBM_FUNC(expm1f)(ip1[k]);
#else
#define __amd_fma3_vrd4_expm1 __amd_fma3_vrda_expm1
    __m128 ip4 = _mm_set_ps(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    __m128 op = LIBM_FUNC_VEC(s, 4, expm1f)(ip4);
    _mm_store_ps(&o[0], op);
#endif

    return 0;
}

static int
test_expm1_cb_v2d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

#if (LIBM_PROTOTYPE != PROTOTYPE_FMA3)		/* Glibc doesnt have a vector version of expm1 */
    for (int k = 0; k < 2; k++)
        o[k] = LIBM_FUNC(expm1)(ip1[k]);
#else
    __m128d ip2 = _mm_set_pd(ip1[j+1], ip1[j]);
    __m128d op = LIBM_FUNC_VEC(d, 2, expm1)(ip2);
    _mm_store_pd(&o[0], op);
#endif

    return 0;
}

static int
test_expm1_cb_v4d(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    double *restrict ip1 = (double*)data->input1;
    double *restrict o = (double*)data->output;

#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)		/* Glibc doesnt have a vector version of expm1 */
    for (int k = 0; k < 4; k++)
        o[k] = LIBM_FUNC(expm1)(ip1[k]);
#else
#define amd_vrd4_expm1 amd_vrda_expm1
 //   __m256d ip4 = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
    LIBM_FUNC_VEC(d, 4, expm1)(4, &ip1[j], &o[j]);
#endif

    return 0;
}

struct libm_test_funcs test_expm1_funcs[LIBM_FUNC_MAX] =
    {
     /*
      * Scalar functions
      */
     [LIBM_FUNC_S_S]  = {
                         .performance  = {.setup = test_expm1_perf_setup,
                                          .run   = libm_test_s1s_perf,},
                         .accuracy     = {.setup = test_expm1_accu_setup,
                                          .run   = test_expm1_accu,
                                          .ulp   = {.func = test_expm1_ulp},
                         },
                         .special      = {.setup = test_expm1_special_setup,},
     },

     [LIBM_FUNC_S_D]  = {
                         .performance  = {.setup = test_expm1_perf_setup,
                                          .run   = libm_test_s1d_perf,},
                         .accuracy     = {.setup = test_expm1_accu_setup,
                                          .run   = test_expm1_accu,},
                         .special      = {.setup = test_expm1_special_setup,},
     },

#if 0
     [LIBM_FUNC_V2S] = {
                        .performance = {.setup = test_expm1_perf_setup,
                                        .run = test_expm1_v2s_perf,},
                        .accuracy     = {.setup = test_expm1_accu_setup,
                                         .run = test_expm1_v4s,
                                         .ulp   = {.func = test_expm1_ulp},},
                        .special      = {.setup = test_expm1_special_setup,},
     },

     [LIBM_FUNC_V8S]  = {
                         .performance = { .setup = test_expm1_perf_setup,
                                          .run   = test_expm1_v8s_perf,},
                         .accuracy     = {.setup = test_expm1_accu_setup,
                                          .run = test_expm1_v4s,
                                          .ulp   = {.func = test_expm1_ulp},},
                         .special      = {.setup = test_expm1_special_setup,},
     },
#endif
     [LIBM_FUNC_V4S]  = {
                         .performance  = {.setup = test_expm1_perf_setup,
                                          .run   = libm_test_v4s_perf,},
                         .accuracy     = {.setup = test_expm1_accu_setup,
                                          .run   = test_expm1_accu,},
                         .special      = {.setup = test_expm1_special_setup,
                                          .run   = test_expm1_accu},
     },

     [LIBM_FUNC_V2D]  = {
                         .performance  = {.setup = test_expm1_perf_setup,
                                          .run   = libm_test_v2d_perf,},
                         .accuracy     = {.setup = test_expm1_accu_setup,
                                          .run   = test_expm1_accu,},
                         .special      = {.setup = test_expm1_special_setup,
                                          .run   = test_expm1_accu,},
     },

     [LIBM_FUNC_V4D] = {
                        .performance  = {.setup = test_expm1_perf_setup,
                                         .run   = libm_test_v4d_perf,},
                        .accuracy     = {.setup = test_expm1_accu_setup,
                                         .run   = test_expm1_accu,},
                        .special      = {.setup = test_expm1_special_setup,
                                         .run   = test_expm1_accu,},
     },
};

/* There is no expm12q in recent versions of gcc */
long double
test_expm1_expm1l(struct libm_test *test, int idx)
{
    double *d = (double*)test->test_data.input1;
    return expm1l(d[idx]);
}

int test_expm1_verify(struct libm_test *test, struct libm_test_result *result);

static struct libm_test
expm1_template = {
	//.name       = "expm1_vec",
	.nargs      = 1,
	.ulp_threshold = 4.0,
	.ops        = {
		.ulp       = {.funcl = test_expm1_expm1l},
		.verify    = test_expm1_verify,
                .callbacks = {
                    .s1s = test_expm1_cb_s1s,
                    .s1d = test_expm1_cb_s1d,
                    .v4s = test_expm1_cb_v4s,
                    .v2d = test_expm1_cb_v2d,
                    .v4d = test_expm1_cb_v4d,
                },
        }
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

    ret = libm_tests_setup(c, test_expm1_funcs, &expm1_template);

    return ret;
}
