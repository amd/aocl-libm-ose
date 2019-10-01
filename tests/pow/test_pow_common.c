#include <string.h>                     /* for memcpy() */
#include <stdlib.h>
#include <math.h>                       /* for pow() poisining */

#include <libm_amd.h>
#include <libm_tests.h>
#include <bench_timer.h>
#include <libm/types.h>
#include "test_pow.h"

#define __TEST_POW_INTERNAL__
//#include "test_pow_data.h"
//#include "test_powf_data.h"

double LIBM_FUNC(pow)(double,double);
float LIBM_FUNC(powf)(float, float);

extern int RANGE_LEN_X;

extern struct libm_test_input_range x_range[];
extern struct libm_test_input_range y_range[];
extern struct libm_test_input_range x[];
extern struct libm_test_input_range y[];

int test_pow_populate_inputs(struct libm_test *test, int use_uniform)
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

int test_pow_alloc_data(struct libm_test *test, size_t size)
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


