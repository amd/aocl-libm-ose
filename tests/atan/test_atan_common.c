#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */

#include <immintrin.h>

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>


/*
 * Call the glibc's atan() to get IEEE754 compliant values
 */

int test_atan_verify(struct libm_test *test, struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;

    /*
     * Call the glibc's atan() to get IEEE754 compliant values
     */
    if (test_is_single_precision(test)) {
        float *expected = (float*)data->expected;
        float *input1   = (float*)data->input1;

        for (uint32_t j = 0; j < data->nelem; j++) {
            expected[j] = atanf(input1[j]);
        }
    } else {
        double *expected = data->expected;
        double *input1 = data->input1;

        for (uint32_t j = 0; j < data->nelem; j++) {
            expected[j] = atan(input1[j]);
        }
    }

    return libm_test_verify(test, result);
}

int test_atan_alloc_init(struct libm_test_conf *conf, struct libm_test *test)
{

    int ret = 0;

    ret = libm_test_alloc_test_data(test, conf->nelem);

    return ret;
}


