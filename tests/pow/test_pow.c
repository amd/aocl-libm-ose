/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 *
 * Tests for pow()
 *
 */

#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */

#include <immintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>
//#include "pow_accu_data.h"
#define __TEST_POW_INTERNAL__                   /* needed to include exp-test-data.h */
#include "test_pow_data.h"


#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN4v(x) _ZGVbN4vv_##x
#define _ZGVdN2v_pow _ZGVbN2vv_pow
#define _ZGVdN4v_pow _ZGVdN4vv_pow

#define _ZGVsN4v_pow  _ZGVbN4v_pow
#define _ZGVsN4v_powf _ZGVbN4vv_powf

__m128d LIBM_FUNC_VEC(d, 2, pow)(__m128d, __m128d);
__m256d LIBM_FUNC_VEC(d, 4, pow)(__m256d, __m256d);

__m128 LIBM_FUNC_VEC(b, 4, powf)(__m128, __m128);
__m256 LIBM_FUNC_VEC(b, 8, powf)(__m256, __m256);
#endif


//#define RANGE_LEN(x) (sizeof(x) / sizeof((x)[0]))
char doc[] = BUILD_TEST_DOC(TEST_NAME);
extern struct libm_test_input_range x[];
extern struct libm_test_input_range y[];

static int test_pow_populate_inputs(struct libm_test *test, int use_uniform)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_conf *conf = test->conf;
    int ret = 0;
    int (*func)(void *, size_t, uint32_t, double, double);

    if(use_uniform)
        func = libm_test_populate_range_uniform;
    else
        func = libm_test_populate_range_rand;

    ret = func(data->input1, data->nelem,
               test->variant,
               conf->inp_range[0].start,
               conf->inp_range[0].stop);

    /* Fill the same if more inputs are needed */
    if (!ret && test->nargs > 1) {
//	printf("data is %g\n", *(data->input2));
        ret = func(data->input2, data->nelem,
                   test->variant,
                   conf->inp_range[1].start,
                   conf->inp_range[1].stop);
    }

    if (!ret && test->nargs > 2) {
        ret = func(data->input3, data->nelem,
                   test->variant,
                   conf->inp_range[2].start,
                   conf->inp_range[2].stop);
    }

    return ret;
}

static int test_pow_vrd4_perf(struct libm_test *test)
{
    printf("ALLOCATING");	
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = GCC_ALIGN(data->output, 256);
    double *restrict ip1 = GCC_ALIGN(data->input1, 256);
    double *restrict ip2 = GCC_ALIGN(data->input2, 256);
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = pow(t,t);
    }

    test->conf->inp_range[0] = x[0];// range for x
    test->conf->inp_range[1] = y[0]; //range for y
    test_pow_populate_inputs(test, 1);

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        //IVDEP //;
        for (uint32_t j = 0; j < (sz - 3); j += 4) {
            __m256d ip4_x = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
	    __m256d ip4_y = _mm256_set_pd(ip2[j+3], ip2[j+2], ip2[j+1], ip2[j]);
            __m256d op4 = LIBM_FUNC_VEC(d, 4, pow)(ip4_x,ip4_y);
            _mm256_store_pd(&o[j], op4);
        }
        /*
         * Any left over process with scalar
         */
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}

static int test_pow_vrd2_perf(struct libm_test *test)
{
    printf("ALLOCATING");
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = GCC_ALIGN(data->output, 128);
    double *restrict ip1 = GCC_ALIGN(data->input1, 128);
    double *restrict ip2 = GCC_ALIGN(data->input2, 128);
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = pow(t,t);
    }

    test->conf->inp_range[0] = x[0];// range for x
    test->conf->inp_range[1] = y[0]; //range for y
    test_pow_populate_inputs(test, 1);

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        //IVDEP //;
        for (uint32_t j = 0; j < (sz - 1); j += 2) {
            __m128d ip2_x = _mm_set_pd(ip1[j+1], ip1[j]);
            __m128d ip2_y = _mm_set_pd(ip2[j+1], ip2[j]);
            __m128d op2 = LIBM_FUNC_VEC(d, 2, pow)(ip2_x,ip2_y);
            _mm_store_pd(&o[j], op2);
        }
        /*
         * Any left over process with scalar
         */
    }

    timer_stop(&bt);
    double s = timer_span(&bt);

    result->mops = sec2mps(s, n * sz);

    return result->nfail;
}


static int test_pow_vrd4_special(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip1 = data->input1;
    double *ip2 = data->input2;
    double *op =  data->output;
    int sz = data->nelem;

    if (sz % 4 != 0)
        printf("%s %s : %d is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < (sz - 3); j += 4) {
        __m256d ip4_x = _mm256_set_pd(ip1[j+3], ip1[j+2], ip1[j+1], ip1[j]);
	__m256d ip4_y = _mm256_set_pd(ip2[j+3], ip2[j+2], ip2[j+1], ip2[j]);
        __m256d op4 = LIBM_FUNC_VEC(d, 4, pow)(ip4_x,ip4_y);
        _mm256_store_pd(&op[j], op4);
    }

    return 0;
}

static int test_pow_vrd2_special(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    double *ip1 = data->input1;
    double *ip2 = data->input2;
    double *op =  data->output;
    int sz = data->nelem;

    if (sz % 2 != 0)
        printf("%s %s : %d is not a multiple of 2, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int j = 0; j < (sz - 1); j += 2) {
        __m128d ip2_x = _mm_set_pd(ip1[j+1], ip1[j]);
        __m128d ip2_y = _mm_set_pd(ip2[j+1], ip2[j]);
        __m128d op4 = LIBM_FUNC_VEC(d, 2, pow)(ip2_x,ip2_y);
        _mm_store_pd(&op[j], op4);
    }

    return 0;
}


int libm_test_pow_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    double *expected = data->expected;
    double *input_x = data->input1;
    double *input_y = data->input2;
    for (uint32_t j = 0; j < data->nelem; j++) {
        expected[j] = pow(input_x[j],input_y[j]);
    }

    return libm_test_verify(test, result);
}


long double libm_test_powl(struct libm_test *test, int idx)
{
    double* in_x = test->test_data.input1; 
    double* in_y = test->test_data.input2;
    return powl(in_x[idx],in_y[idx]);
}

/* vector single precision */
static struct libm_test pow_test_template = {
    .name       = "pow_vec",
    .nargs      = 2,
    .ulp_threshold = 0.5,
    .ops        = {
                   .ulp    = {.funcl = libm_test_powl},
                   .verify = libm_test_pow_verify,
                   },
};

static int test_pow_alloc_init_perf_data(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    ret = test_pow_populate_inputs(test, 0);

    return ret;
}

int test_pow_register_one(struct libm_test *test)
{
    int ret = 0;

    ret = libm_test_register(test);

    if (ret)
        goto out;
 out:
    return ret;
}


/**************************
 * SPECIAL CASES TESTS
 **************************/

static int test_pow_alloc_special_data(struct libm_test *test, size_t size)
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


static int test_pow_special_setup(struct libm_test *test)
{
    int test_data_size = ARRAY_SIZE(libm_test_pow_special_data);
    struct libm_test_data *data;
    double *in_x,*in_y,*expected;

    // Just trying to get rid of warning/errors
    test_pow_alloc_special_data(test, test_data_size);

    data = &test->test_data;
    in_x = data->input1;
    in_y = data->input2;
    expected = data->expected; 

    for (int i = 0; i < test_data_size; i++) {
        in_x[i] = libm_test_pow_special_data[i].x;
	in_y[i] = libm_test_pow_special_data[i].y;
        expected[i] =libm_test_pow_special_data[i].out;
    }

    return 0;
}

//#include "pow_accu_data.h"
/**************************
 * ACCURACY TESTS
 **************************/
union double256 {
  double result[4];
  __m256d op4;
};

typedef union double256 vect256;

//#include "pow_accu_data.h"
extern int RANGE_LEN_X;

extern struct libm_test_input_range x_range[];
extern struct libm_test_input_range y_range[];

static int test_pow_vrd4_accu(struct libm_test *test)
{

    struct libm_test_data *data = &test->test_data;
    double *ip_x = data->input1;
    double *ip_y = data->input2;
    double *op =   data->output;
//    double *restrict op = GCC_ALIGN(data->output, 256);
    uint64_t sz = data->nelem;
    int arr_sz = RANGE_LEN_X;

    if (sz % 4 != 0)
        printf("%s %s : %ld is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int i = 0; i < arr_sz ; i++) {
        test->conf->inp_range[0] = x_range[i];
        test->conf->inp_range[1] = y_range[i];
        test_pow_populate_inputs(test, 1);

        LIBM_TEST_DPRINTF(VERBOSE2,
                          "Testing for accuracy %d items in range x= [%Lf, %Lf] y= [%Lf, %Lf]\n",
                          test->test_data.nelem,
                          x_range[i].start,x_range[i].stop,y_range[i].start,y_range[i].stop);


        for (uint64_t j = 0; j < (sz - 3); j += 4) {
//            printf("op[%d] =%g\n ",j,op[j]);
    		__m256d ip4_x = _mm256_set_pd(ip_x[j+3], ip_x[j+2], ip_x[j+1], ip_x[j]);
	    __m256d ip4_y = _mm256_set_pd(ip_y[j+3], ip_y[j+2], ip_y[j+1], ip_y[j]);
	    asm("nop" :::"rbx");
            __m256d op4 = LIBM_FUNC_VEC(d, 4, pow)(ip4_x,ip4_y);
	    _mm256_store_pd(&op[j], op4);
	
        }

        libm_test_pow_verify(test, &test->result);
    }

    return 0;
}

static int test_pow_vrd2_accu(struct libm_test *test)
{

    struct libm_test_data *data = &test->test_data;
    double *ip_x = data->input1;
    double *ip_y = data->input2;
    double *op =   data->output;
//    double *restrict op = GCC_ALIGN(data->output, 256);
    uint64_t sz = data->nelem;
    int arr_sz = RANGE_LEN_X;

    if (sz % 2 != 0)
        printf("%s %s : %ld is not a multiple of 4, some may be left out\n"
               " And error reported may not be real for such entries\n",
               test->name, test->type_name, sz);

    for (int i = 0; i < arr_sz ; i++) {
        test->conf->inp_range[0] = x_range[i];
        test->conf->inp_range[1] = y_range[i];
        test_pow_populate_inputs(test, 1);

        LIBM_TEST_DPRINTF(VERBOSE2,
                          "Testing for accuracy %d items in range x= [%Lf, %Lf]\n y= [%Lf, %Lf]",
                          test->test_data.nelem,
                          x_range[i].start,x_range[i].stop,y_range[i].start,y_range[i].stop);


        for (uint64_t j = 0; j < (sz - 1); j += 2) {
//            printf("op[%d] =%g\n ",j,op[j]);
            __m128d ip2_x = _mm_set_pd(ip_x[j+1], ip_x[j]);
            __m128d ip2_y = _mm_set_pd(ip_y[j+1], ip_y[j]);
            __m128d op2 = LIBM_FUNC_VEC(d, 2, pow)(ip2_x,ip2_y);
            _mm_store_pd(&op[j], op2);

        }

        libm_test_pow_verify(test, &test->result);
    }

    return 0;
}

int test_pow_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;
    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}

static int test_pow_accu_setup(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    return test_pow_alloc_init(conf,  test);
}

static int test_pow_init_v4d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_types = conf->test_types;

    while(test_types) {
        struct libm_test *pow_v4d = libm_test_alloc_init(conf,
                                                          &pow_test_template);
        uint32_t bit = 1 << (ffs(test_types) - 1);

        pow_v4d->variant |= LIBM_FUNC_V4D;
	 pow_v4d->input_name = "v4d";
        switch(bit) {
        case TEST_TYPE_PERF:
            pow_v4d->type_name = "perf";
	    pow_v4d->test_type = TEST_TYPE_PERF;
            pow_v4d->ops.run = test_pow_vrd4_perf;
            test_pow_alloc_init_perf_data(pow_v4d);
	    pow_v4d->ops.verify = libm_test_pow_verify;
            break;
        case TEST_TYPE_SPECIAL:
            pow_v4d->type_name = "special";
	    pow_v4d->test_type = TEST_TYPE_SPECIAL;
            pow_v4d->ops.run = test_pow_vrd4_special;
            pow_v4d->ops.setup = test_pow_special_setup;
	    pow_v4d->ops.verify = libm_test_pow_verify;
            break;
        case TEST_TYPE_ACCU:
            pow_v4d->type_name = "accuracy";
	    pow_v4d->test_type = TEST_TYPE_ACCU;
            pow_v4d->ops.setup = test_pow_accu_setup;
            pow_v4d->ops.run = test_pow_vrd4_accu;
            pow_v4d->ops.verify = libm_test_pow_verify; // No verify after, will verify inside.
            break;
        case TEST_TYPE_CORNER:
            pow_v4d->type_name = "corner";
	    pow_v4d->test_type = TEST_TYPE_CORNER;
          //  pow_v4d->test_data  = libm_test_pow_corner_data;
            pow_v4d->ops.run = test_pow_vrd4_special;
            break;
        }

        if (ret)
            goto out;

        test_types = test_types & (test_types -  1);
        ret = test_pow_register_one(pow_v4d);

        if (ret)
            goto out;;
    }

    return 0;

 out:
    return -1;
}


static int test_pow_init_v2d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_types = conf->test_types;

    while(test_types) {
        struct libm_test *pow_v2d = libm_test_alloc_init(conf,
                                                          &pow_test_template);
        uint32_t bit = 1 << (ffs(test_types) - 1);

        pow_v2d->variant |= LIBM_FUNC_V2D;
	pow_v2d->input_name = "v2d";
        switch(bit) {
        case TEST_TYPE_PERF:
            pow_v2d->type_name = "perf";
	    pow_v2d->test_type = TEST_TYPE_PERF;
            pow_v2d->ops.run = test_pow_vrd2_perf;
            test_pow_alloc_init_perf_data(pow_v2d);
            pow_v2d->ops.verify = libm_test_pow_verify;
            break;
        case TEST_TYPE_SPECIAL:
            pow_v2d->type_name = "special";
	    pow_v2d->test_type = TEST_TYPE_SPECIAL;
            pow_v2d->ops.run = test_pow_vrd2_special;
            pow_v2d->ops.setup = test_pow_special_setup;
	    pow_v2d->ops.verify = libm_test_pow_verify;
            break;
        case TEST_TYPE_ACCU:
            pow_v2d->type_name = "accuracy";
	    pow_v2d->test_type = TEST_TYPE_ACCU;
            pow_v2d->ops.setup = test_pow_accu_setup;
            pow_v2d->ops.run = test_pow_vrd2_accu;
            pow_v2d->ops.verify = libm_test_pow_verify; // No verify after, will verify inside.
            break;
        case TEST_TYPE_CORNER:
           pow_v2d->type_name = "corner";
          //pow_v4d->test_data  = libm_test_pow_corner_data;
            pow_v2d->ops.run = test_pow_vrd2_special;
            break;
        }

        
	ret = test_pow_register_one(pow_v2d);

	if (ret)
            return -1; //completed vrd2 tests

      test_types = test_types & (test_types -  1);
    }

//next_test:
    return 0;

}

#define POW_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

int test_pow_init_scalar(struct libm_test_conf *conf);


int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;

    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;

    if (!conf->test_types)
        conf->test_types = POW_TEST_TYPES_ALL;


    if ((conf->test_types & TEST_TYPE_ACCU) &&
        !conf->inp_range[0].start) {
        /* We dont have ranges */
        conf->inp_range[0].start = DBL_MIN;
        conf->inp_range[0].stop  = DBL_MAX;
    }

    if (conf->variants & LIBM_FUNC_V2D) {
        ret = test_pow_init_v2d(conf);
        if (ret) {
            printf("registering test failed, ret value %d\n",ret);
            goto out;
        }
    }

    if (conf->variants & LIBM_FUNC_V4D) {
        ret = test_pow_init_v4d(conf);
        if (ret)
            goto out;
    }

    ret = test_pow_init_scalar(conf);          /* in other file */

out:
    return ret;
}


#if 0
double libm_test_pow_ulp(struct libm_test *test, double x, double computed)
{
    __float128 pow_x = libm_test_powq(x);

    return libm_test_ulp_errord(computed, pow_x);
}
#endif
