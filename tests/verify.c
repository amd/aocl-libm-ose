/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */
#include <math.h>                      /* for isinf() */
#include <fenv.h>
#include <stdbool.h>

#include <libm_tests.h>
#include <libm/types.h>
#include <libm_util_amd.h>

static double get_ulp(struct libm_test *test, int j)
{
    struct libm_test_data *data = &test->test_data;
    //__float128 computedq = 0.0;
    double *outputd = (double*)data->output;
    long double computedl = 0.0;
    double computed = 0.0;

    /* Get higer precision value for a given input */
    if (test_is_single_precision(test)) {
        float *outputf = (float*)data->output;
        computed = test->ops.ulp.func(test, j);
        return libm_test_ulp_errorf(outputf[j], computed);
    }

    computedl = test->ops.ulp.funcl(test, j);

    return libm_test_ulp_error(outputd[j], computedl);
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
        ret = 1;    }

    if ((ulp - test->ulp_threshold) > 0.0)
        ret = 2;                   /* fail; as greater than threshold */
 out:
    return ret;
}

static inline void __update_results(struct libm_test_result *result,
                                    int npass, int nfail, int nignored, int ntests)
{
    /* Unless some are ignored forcibly */
    npass = ntests - nfail - nignored;

    result->ntests += ntests;
    result->npass += npass;
    result->nfail += nfail;
    result->nignored += nignored;

}

static int __is_ulp_required(flt64u_t expected, flt64u_t actual)
{
    if (isnan(expected.d) && isnan(actual.d)) return 0;
    if (isinf(expected.d) && isinf(actual.d)) return 0;
    return 1;
}

static void print_errors(const int flags)
{

    if (flags & FE_INVALID) {
        printf("FE_INVALID ");
    }

    if (flags & FE_DIVBYZERO) {
        printf("FE_DIVBYZERO ");
    }

    if (flags & FE_OVERFLOW) {
        printf("FE_OVERFLOW ");
    }

    if (flags & FE_UNDERFLOW) {
        printf("FE_UNDERFLOW ");
    }

    if (flags & FE_INEXACT) {
        printf("FE_INEXACT ");
    }

    if (flags == 0) {
        printf("-NONE-");
    }

}

static int __verify_double(struct libm_test *test,
                           struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    double *in1 = (double*)data->input1,
        *in2 = (double*)data->input2, *in3 = (double*)data->input3;

    flt64u_t *op = (flt64u_t*)data->output;
    flt64u_t *nw = (flt64u_t*)data->expected;
    flt64u_t *in = (flt64u_t*)data->input1;

    const int *expected_exception = (int*)data->expected_exception;
    const int *raised_exception = (int*)data->raised_exception;

    int sz = data->nelem;
    int idx = 0, npass = 0, nfail = 0, nignored = 0, ntests;
    int nargs = test->nargs;
    int print_info = 0, test_update_ulp = 0;
    double ulp = 0.0;

    ntests = data->nelem;

    for (int j = 0; j < sz; ++j) {
        int ret = 0;

        if (test->test_type == TEST_TYPE_CONFORMANCE){
		    if ((((nw[j].i ^ op[j].i) != 0) && !(isnan(nw[j].d) && isnan(op[j].d))) || (raised_exception[j] != expected_exception[j])) {
                if ((nw[j].i & QNANBITPATT_DP64) == (op[j].i & QNANBITPATT_DP64)) {
                    nfail++;
                    printf("input = %lx expected = %lx output = %lx \n", in[j].i, nw[j].i,op[j].i);
                    print_info=1;
                    if (raised_exception[j] != expected_exception[j]) {
                        printf("Raised exception: ");
                        print_errors(raised_exception[j]);
                        printf(" Expected exception: ");
                        print_errors(expected_exception[j]);
                        puts("");
                    }
                }
            }
        }
        else if (test->test_type == TEST_TYPE_ACCU) {
            /*
             * Verify ULP for every case,
             * except when both output and exptected is 0 or a subnormal number
             */
            if (__is_ulp_required(nw[j], op[j]))
                test_update_ulp = 1;
        }
        else {
            if ((nw[j].i ^ op[j].i) != 0) {
                result->input1[idx] = in1[j];
                if (test->nargs > 1) result->input2[idx] = in2[j];
                if (test->nargs > 2) result->input3[idx] = in3[j];
                print_info = 1;
                ret = 0;
            }
        }

        if (test_update_ulp) {
            ulp = get_ulp(test, j);
	    //printf("ulp:- %f\n", ulp);
            ret = update_ulp(test, ulp);
            test_update_ulp = 0;
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
            flt64u_t *fin1 = (flt64u_t*)&in1[j],
                *fin2 = (flt64u_t*)&in2[j],
                *fin3 = (flt64u_t*)&in1[j];
            LIBM_TEST_DPRINTF(VERBOSE3, "i:%d input1: % -g(%016lX)", j, fin1->d, fin1->i);
            if (nargs > 1)
                LIBM_TEST_CDPRINTF(VERBOSE3, "  input2: % -g(%016lX)", fin2->d, fin2->i);
            if (nargs > 2)
                LIBM_TEST_CDPRINTF(VERBOSE3, "  input3: % -g(%016lX)", fin3->d, fin3->i);

            LIBM_TEST_CDPRINTF(VERBOSE3, "    expected:%lX actual:%lX ulp:%G\n",
                               nw[j].i, op[j].i, ulp);

            print_info = 0;
        }

        // reset ret
        ret = 0;
    }

    __update_results(result, npass, nfail, nignored, ntests);

    return idx;
}

static int __is_ulpf_required(flt32u_t expected, flt32u_t actual)
{
    if (isnanf(expected.f) && isnanf(actual.f)) return 0;
    if (isinff(expected.f) && isinff(actual.f)) return 0;
    return 1;
}

static int __verify_float(struct libm_test *test,
                          struct libm_test_result *result)
{
    struct libm_test_data *data = &test->test_data;
    float *in1 = data->input1, *in2 = data->input2, *in3 = data->input3;
    flt32u_t *op = (flt32u_t*)data->output;
    flt32u_t *nw = (flt32u_t*)data->expected;
    int sz = data->nelem, ret = 0;
    int idx = 0, npass = 0, nfail = 0, nignored = 0, ntests;
    int nargs = test->nargs;
    int print_info = 0, test_update_ulp = 0;
    double ulp = 0.0;

    const int *expected_exception = (int*)data->expected_exception;
    const int *raised_exception = (int*)data->raised_exception;

    ntests = data->nelem;

    for (int j = 0; j < sz; ++j) {
        if (test->conf->test_types == TEST_TYPE_ACCU) {
            /* Verify ULP for every case */
            if (__is_ulpf_required(nw[j], op[j]))
                test_update_ulp = 1;
        }

	if (test->test_type == TEST_TYPE_CONFORMANCE){
		if ((((nw[j].i ^ op[j].i) != 0) && !(isnan(nw[j].f) && isnan(op[j].f))) || (raised_exception[j] != expected_exception[j]))
		{
		    /*check for nan bit patterns*/
		    if ((nw[j].i & QNANBITPATT_SP32) == (op[j].i & QNANBITPATT_SP32))    {
		        nfail++;
		        printf("expected = %x output = %x \n",nw[j].i,op[j].i);
                print_info=1;
		        if (raised_exception[j] != expected_exception[j]) {
                    printf("Raised excpetion: ");
			        print_errors(raised_exception[j]);
			        printf(" Expected exception: ");
		            print_errors(expected_exception[j]);
                    puts("");
		        }
		    }
		 }
	}

	else
	{

            if ((nw[j].i ^ op[j].i) != 0) {
                result->input1[idx] = in1[j];
                if (test->nargs > 1) result->input2[idx] = in2[j];
                if (test->nargs > 2) result->input3[idx] = in3[j];
                print_info = 1;
                ret = 0;
            }
        }


        if (test_update_ulp) {
            ulp = get_ulp(test, j);
	    //printf("Exp:- %x, op:-%x, ULP:-%f \n", nw[j].i, op[j].i,  ulp);
            ret = update_ulp(test, ulp);
            test_update_ulp = 0;
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
                result->expected[idx] = nw[j].f;
                idx++;
            }
        }

        if (print_info) {
            flt32u_t *fin1 = (flt32u_t*)&in1[j],
                *fin2 = (flt32u_t*)&in2[j],
                *fin3 = (flt32u_t*)&in1[j];
            LIBM_TEST_DPRINTF(VERBOSE3, "i:%d finput1: %-G(%08X)", j, fin1->f, fin1->i);
            if (nargs > 1)
                LIBM_TEST_CDPRINTF(VERBOSE3,
                                   " finput2: %g(%08X)\n", fin2->f, fin2->i);
            if (nargs > 2)
                LIBM_TEST_CDPRINTF(VERBOSE3,
                                   " finput3: %g(%08X)\n", fin3->f, fin3->i);

            LIBM_TEST_CDPRINTF(VERBOSE3,
                               "    expected:%8X actual:%8X ulp:%5.05g\n",
                               nw[j].i, op[j].i, ulp);

            print_info = 0;
        }

        // reset ret
        ret = 0;
    }

    __update_results(result, npass, nfail, nignored, ntests);

    return idx;
}

/*
 * returns 0 on success, <int> otherwise
 */
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
