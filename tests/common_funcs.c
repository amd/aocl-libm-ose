
#include <libm_tests.h>

/*
 * returns -1 if success,
 * returns offset in array where the mismatch occurs
 */
int libm_test_verify_dbl(struct libm_test *test,
                                struct libm_test_result *result)
{

    struct libm_test_data *data = test->data;
    double *op = data->output;
    double *nw = data->expected;
    int sz = data->nelem;
    int idx = 0;

    for (int j = 0; j < sz; ++j) {
        if (((unsigned long)nw[j] ^ (unsigned long)op[j]) != 0) {
            data->input1[idx] = data->input1[j];
            data->input2[idx] = data->input2[j];
            data->output[idx] = op[j];
            data->expected[idx] = nw[j];
            double ulp = test->ops.ulp(test);
            if (ulp - test->ulp_err > 0.0)      /* Double comparison, should it work ? */
                test->ulp_err = ulp;
            idx++;

            result->nfail++;
            if (result->nfail > MAX_FAILURES)
                break;
        }
    }

    return idx;
}

int libm_test_verify_flt(struct libm_test *test,
				struct libm_test_data *data,
				struct libm_test_result *result)
{
    return 0;
}


