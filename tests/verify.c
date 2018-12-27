/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

#include <libm_tests.h>

static double get_ulp(struct libm_test *test, int j)
{
    struct libm_test_data *data = test->test_data;
    __float128 computed = 0.0;

    /* Get higer precision value for a given input */
    switch (test->nargs) {
    case 1:
	    computed = test->ops.ulp.func1(test, data->input1[j]);
	    break;
    case 2:
	    computed = test->ops.ulp.func2(test, data->input1[j],
					   data->input2[j]);
	    break;
    case 3:
	    computed = test->ops.ulp.func3(test, data->input1[j],
					   data->input2[j], data->input3[j]);
	    break;
    }

    return libm_test_ulp_errord(data->output[j], computed);
}


static inline int
update_ulp(struct libm_test *test, struct libm_test_data *data, int j)
{
    double ulp = get_ulp(test, j);
    /* Double comparison, should it work ? */
    LIBM_TEST_DPRINTF(VERBOSE3, "ulp:%f\n", ulp);

    if ((test->ulp_threshold - ulp) > 0) {
        if ((ulp - test->max_ulp_err) > 0.0)
            test->max_ulp_err = ulp;
        return 1;
    }

    return 0;
}

/*
 * We need the uint32_t version, default one gives us with an int64
 */
#undef flt64_t

typedef union {
    uint64_t i;
    double   d;
} flt64_t;

/*
 * returns -1 if success,
 * returns offset in array where the mismatch occurs
 */
static int __verify_double(struct libm_test *test,
                           struct libm_test_result *result)
{
    struct libm_test_data *data = test->test_data;
    flt64_t *op = (flt64_t*)data->output;
    flt64_t *nw = (flt64_t*)data->expected;
    int sz = data->nelem, ret;
    int idx = 0, npass = 0, nfail = 0, nignored = 0, ntests;

    ntests = data->nelem;

    for (int j = 0; j < sz; ++j) {
        if (test->conf->test_types == TEST_TYPE_ACCU) {
            /* Verify ULP for every case */
            update_ulp(test, data, j);
        }

        if ((nw[j].d - op[j].d) != 0.0) {
            result->input1[idx] = data->input1[j];
            if (test->nargs > 1) result->input2[idx] = data->input2[j];
            if (test->nargs > 2) result->input3[idx] = data->input3[j];
            LIBM_TEST_DPRINTF(VERBOSE3, "input: %10.23f\n", data->input1[j]);
            LIBM_TEST_DPRINTF(VERBOSE3, "expected: %10.23f actual:%10.23f\n",
                              data->expected[j], data->output[j]);
            ret = update_ulp(test, test->test_data, j);

            switch(ret) {
            case 0:  nignored++; break; /* ulp error found but within limit */
            default:
            case 1: nfail++; break;
            }

            /*
             * Populate some results to show what the input value was when
             * it failed.
             */
            if (nfail < MAX_FAILURES) {
                result->expected[idx] = nw[j].d;
                idx++;
            }
            continue;
        }
        npass++;
    }

    /* Unless some are ignored forcibly */
    npass = ntests - nfail - nignored;

    result->ntests += ntests;
    result->npass += npass;
    result->nfail += nfail;
    result->nignored += nignored;

    return idx;
}

static int __verify_float(struct libm_test *test,
                          struct libm_test_result *result)
{
    return 0;
}

int libm_test_verify(struct libm_test *test,
		     struct libm_test_result *result)
{
    int data_size = libm_test_get_data_size(test->variant);
    switch(data_size) {
    case sizeof(float):
        return __verify_float(test, result);
    case sizeof(double):
        return __verify_double(test, result);
    }

    return 0;
}
