#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */

#include <immintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include "test_fabs.h"

int libm_test_fabs_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;

    if(test_is_single_precision(test))
    {
	float*expected = data->expected;
	float* input_x = data->input1;
	for(uint32_t j=0; j < data->nelem; j++)
	{
		expected[j] = fabsf(input_x[j]);
	}
    }

    else
    {
    	double *expected = data->expected;
   	double *input_x = data->input1;
    	for (uint32_t j = 0; j < data->nelem; j++) 
	{
        	expected[j] = fabs(input_x[j]);
    	}
    }

    return libm_test_verify(test, result);
}

/*alloc - this should be under common */
int test_fabs_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;
    ret = libm_test_alloc_test_data(test, conf->nelem);
    return ret;
}

//register - common
int test_fabs_register_one(struct libm_test *test)
{
    int ret = 0;

    ret = libm_test_register(test);

    if (ret)
        goto out;
 out:
    return ret;
}

//alloc special data
int test_fabs_alloc_special_data(struct libm_test *test, size_t size)
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

//populate input for fabs()
int test_fabs_populate_inputs(struct libm_test *test, int use_uniform)
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



/*----------------------Init function----------*/
#define FABS_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

int libm_test_init(struct libm_test_conf *c)
{
    int ret = 0;

    struct libm_test_conf __conf = *c;
    struct libm_test_conf *conf = &__conf;

    if (!conf->test_types)
        conf->test_types = FABS_TEST_TYPES_ALL;

   if (conf->variants & LIBM_FUNC_S_S)
   {
	ret = test_fabsf_init_s_s(conf);
	if (ret)
	{
		printf("Failed to initialize fabsf scalar\n");
		goto out;
	}
   }
  
   if(conf->variants & LIBM_FUNC_S_D)
   {
	ret = test_fabs_init_s_d(conf);
	if(ret)
	{
		printf("Failed to init fabs double precision scalar\n");
		goto out;
	}
   }


out:
    return ret;
}

