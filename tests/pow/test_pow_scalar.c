/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

#include <string.h>                     /* for memcpy() */
#include <stdlib.h>
#include <math.h>                       /* for pow() poisining */

#include <libm_amd.h>
#include <libm_tests.h>
#include <bench_timer.h>

#include "test_pow.h"
//#include "pow_accu_data.h"

double FN_PROTOTYPE(pow)(double,double);
extern int RANGE_LEN_X;

static inline __float128
libm_test_powq(struct libm_test *test, double in_x,double in_y)
{
    return powq(in_x,in_y);
}

struct libm_test pow_test_template1 = {
    .name       = "pow_scalar",
    .nargs      = 2,
    .ops        = {
                   .ulp    = {.func2 = libm_test_powq},
                   .verify = libm_test_pow_verify,
                   },
    .libm_func  = { .func_64 = { .func2 = pow, }, }, // WOHOOO 
};

static int test_pow_populate_inputs(struct libm_test *test, int use_uniform)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_conf *conf = test->conf;
    int ret = 0;
    int (*func)(void *, size_t, uint32_t, double, double);

    if(use_uniform)
        func = libm_test_populate_range_uniform;
    else
        func = libm_test_populate_range_simple;

    ret = func(data->input1, data->nelem,
               test->variant,
               conf->inp_range[0].start,
               conf->inp_range[0].stop);

   // Fill the same if more inputs are needed 
    if (!ret && test->nargs > 1) {
//      printf("data is %g\n", *(data->input2));
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


//#include "pow_accu_data.h"

//extern struct libm_test_input_range accu_ranges[];
//extern int RANGE_LEN_X;
extern struct libm_test_input_range x_range[];
extern struct libm_test_input_range y_range[];


static int test_pow_accu(struct libm_test *test)
{

    struct libm_test_data *data = &test->test_data;
    double *ip_x = &data->input1[0];
    double *ip_y = &data->input2[0];
    double *op = &data->output[0];
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
            op[j] = FN_PROTOTYPE(pow)(ip_x[j],ip_y[j]);
        }

        libm_test_pow_verify(test, &test->result);
    }
	return 0;
}


static int test_pow_perf(struct libm_test *test)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_result *result = &test->result;
    double *restrict o = data->output;
    double *restrict ip1 = data->input1;
    double *restrict ip2 = data->input2;
    uint64_t sz = data->nelem;
    uint64_t n = test->conf->niter;
    double (*fn2)(double,double) = test->libm_func.func_64.func2;

    /* Poison output */
    for (uint32_t j = 0; j < data->nelem; ++j) {
        double t = (double)((j+1)%700) + (double)j/(double)RAND_MAX;
        o[j] = pow(t,t);
    }

    bench_timer_t bt;
    timer_start(&bt);

    for (uint32_t i = 0; i < n ; ++i) {
        for (uint32_t j = 0; j < sz; j ++) {
            o[j] = (*fn2)(ip1[j],ip2[j]);
            //o[j] = FN_PROTOTYPE(pow_prems)(ip1[j]);
            //o[j] = pow(ip1[j]);
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

 
int test_pow_populate_inputs(struct libm_test *test, int use_uniform);

static int test_pow_setup_s_d(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    int ret = test_pow_alloc_init(conf,  test);
    return ret;
        

}

static int test_pow_init_s_d(struct libm_test_conf *conf)
{
    int ret = 0;
    uint32_t test_type = conf->test_types;

    while (test_type) {
        struct libm_test *test = libm_test_alloc_init(conf, &pow_test_template1);

        if (!test)
            return -1;

        test->variant = LIBM_FUNC_S_D;
        test->name = "pow_scalar";
        test->nargs = 2;
	test->ulp_threshold = 0.6;
        test->libm_func.func_64.func2 = FN_PROTOTYPE(pow);

        uint32_t bit = 1 << (ffs(test_type) - 1);

        switch (bit) {
        case TEST_TYPE_PERF:
            test->type_name = "perf";
            test->ops.setup = test_pow_setup_s_d;
            test->ops.run = test_pow_perf;
            test->ops.verify = libm_test_pow_verify;
            break;
	 case TEST_TYPE_ACCU:
            test->type_name = "accuracy";
            test->ops.setup = test_pow_setup_s_d;
            test->ops.run = test_pow_accu;
            test->ops.verify = NULL; // No verify after, will verify inside.
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


static int test_pow_init_s_s(struct libm_test_conf *conf)
{
    return 0;
}

int test_pow_init_scalar(struct libm_test_conf *conf)
{
    int ret =0;

    if (conf->variants & LIBM_FUNC_S_S) {
        ret = test_pow_init_s_s(conf);
        if (ret)
            goto out;
    }

    if (conf->variants & LIBM_FUNC_S_D) {
        ret = test_pow_init_s_d(conf);
        if (ret)
            goto out;
    }

 out:
    return ret;
}


