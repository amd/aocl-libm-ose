
#include <libm_tests.h>

/*
 * returns -1 if success,
 * returns offset in array where the mismatch occurs
 */
int libm_test_verify_dbl(struct libm_test *test,
                         struct libm_test_result *result)
{

    struct libm_test_data *data = test->test_data;
    double *op = data->output;
    double *nw = data->expected;
    int sz = data->nelem;
    int idx = 0, npass = 0, nfail = 0, nignored = 0, ntests;

    ntests = data->nelem;

    for (int j = 0; j < sz; ++j) {
        if (((unsigned long)nw[j] ^ (unsigned long)op[j]) != 0) {
            result->input1[idx] = data->input1[j];
            if (test->nargs > 1) result->input2[idx] = data->input2[j];
            if (test->nargs > 2) result->input3[idx] = data->input3[j];

            double ulp = test->ops.ulp(test, data->input1[j], op[j]);
            /* Double comparison, should it work ? */
            LIBM_TEST_DPRINTF(PANIC, "ulp:%f\n", ulp);
            if ((ulp - test->ulp_err) > 0.0)
                test->ulp_err = ulp;

            nfail++;
            if (nfail < MAX_FAILURES) {
                result->expected[idx] = nw[j];
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

int libm_test_verify_flt(struct libm_test *test,
				struct libm_test_data *data,
				struct libm_test_result *result)
{
    return 0;
}


