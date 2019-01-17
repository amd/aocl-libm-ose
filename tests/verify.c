/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */
#include <math.h>                      /* for isinf() */
#include <libm_tests.h>

static double get_ulp(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
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

    return libm_test_ulp_error(data->output[j], computed);
}

/*
 * 0 - result is exact
 * 1 - within threshold
 * 2 - exceeded threshold, fail case
 */
static inline int
update_ulp(struct libm_test *test, double ulp)
{
    int ret = 0;

    if (isinf(ulp)) {
        ret = 2;
        goto out;
    }

    if ((ulp - test->max_ulp_err) > 0.0) {
        LIBM_TEST_DPRINTF(VERBOSE2, "ulp threshold:%f ulp:%f\n",
                          test->ulp_threshold, (double)ulp);
        test->max_ulp_err = ulp;
        ret = 1;
    }

    if ((ulp - test->ulp_threshold) > 0.0)
        ret = 2;                   /* fail; as greater than threshold */
 out:
    return ret;
}

/*
 * We need the uint32_t version, default one gives us with an int64
 */
typedef union {
    uint64_t i;
    double   d;
} flt64u_t;


#if 0
static int __verify_double_accu(struct libm_test *test,
                                struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    flt64u_t *op = (flt64u_t*)data->output;
    flt64u_t *nw = (flt64u_t*)data->expected;
    int sz = data->nelem, ret = 0;
    int idx = 0, npass = 0, nfail = 0, nignored = 0, ntests;

    ntests = data->nelem;

    for (int j = 0; j < sz; ++j) {
            /* Verify ULP for every case */
            double ulp = get_ulp(test, j);
            ret = update_ulp(test, ulp);
            if (isinfq(ulp) || (ret == 1)) {
                int nargs = test->nargs;
                LIBM_TEST_DPRINTF(VERBOSE3, "input: %10.23f\n", data->input1[j]);
                if (nargs > 1)
                    LIBM_TEST_DPRINTF(VERBOSE3, "input2: %10.23f\n", data->input2[j]);
                if (nargs > 2)
                    LIBM_TEST_DPRINTF(VERBOSE3, "input3: %10.23f\n", data->input3[j]);
                LIBM_TEST_DPRINTF(VERBOSE3, "expected: %lx actual:%lx\n",
                                  nw[j].i, op[j].i);
            }
}
#endif

/*
 * returns -1 if success,
 * returns offset in array where the mismatch occurs
 */
static int __verify_double(struct libm_test *test,
                           struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    flt64u_t *op = (flt64u_t*)data->output;
    flt64u_t *nw = (flt64u_t*)data->expected;
    int sz = data->nelem, ret = 0;
    int idx = 0, npass = 0, nfail = 0, nignored = 0, ntests;
    int nargs = test->nargs;
    int print_info = 0;
    double ulp = 0.0;

    ntests = data->nelem;

    for (int j = 0; j < sz; ++j) {
        if (test->conf->test_types == TEST_TYPE_ACCU) {
            /* Verify ULP for every case */
            ulp = get_ulp(test, j);
            ret = update_ulp(test, ulp);
        } else {
            if ((nw[j].i ^ op[j].i) != 0) {
                result->input1[idx] = data->input1[j];
                if (test->nargs > 1) result->input2[idx] = data->input2[j];
                if (test->nargs > 2) result->input3[idx] = data->input3[j];
                print_info = 1;
                ulp = get_ulp(test, j);
                ret = update_ulp(test, ulp);

            }
        }
        switch(ret) {
        case 0:
            npass++;
            continue;
        case 1:
            nignored++;
            print_info = 1;
            break; /* ulp error found but within limit */
        default:
        case 2:
            nfail++;
            print_info = 1;
            /* populate some results for sample */
            if (nfail < MAX_FAILURES && (idx < MAX_FAILURES)) {
                result->expected[idx] = nw[j].d;
                idx++;
            }
        }

        if (print_info) {
            flt64u_t *in1 = (flt64u_t*)&data->input1[j],
                *in2 = (flt64u_t*)&data->input2[j],
                *in3 = (flt64u_t*)&data->input1[j];
            LIBM_TEST_DPRINTF(VERBOSE3, "input1: %10.23f", data->input1[j]);
            if (nargs > 1)
                LIBM_TEST_CDPRINTF(VERBOSE3, "  input2: %10.23f", data->input2[j]);
            if (nargs > 2)
                LIBM_TEST_CDPRINTF(VERBOSE3, "  input3: %10.23f", data->input3[j]);

            LIBM_TEST_CDPRINTF(VERBOSE3, "\ninput1: %lx", in1->i);
            if (nargs > 1)
                LIBM_TEST_CDPRINTF(VERBOSE3, "  input2: %lx", in2->i);
            if (nargs > 2)
                LIBM_TEST_CDPRINTF(VERBOSE3, "  input3: %lx", in3->i);

            LIBM_TEST_CDPRINTF(VERBOSE3, "    expected: %lx actual:%lx ulp:%f\n",
                               nw[j].i, op[j].i, ulp);

            print_info = 0;
        }

        // reset ret
        ret = 0;
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
