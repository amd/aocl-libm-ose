#ifndef __TEST_FABS_H__
#define __TEST_FABS_H__

long double libm_test_fabsf(struct libm_test *, int);
int test_fabs_alloc_init(struct libm_test_conf *, struct libm_test *);
int test_fabsf_s1s_perf(struct libm_test *);
int test_fabsf_setup_s_s(struct libm_test *);
int test_fabsf_init_s_s(struct libm_test_conf *);

/*conf*/
int test_fabsf_conformance_setup(struct libm_test *);
int test_fabsf_conformance(struct libm_test *);

/*accu*/
int test_fabsf_accu_setup(struct libm_test *);
int test_fabsf_accu(struct libm_test *);

/*common functions*/
int test_fabs_alloc_special_data(struct libm_test *, size_t);
int test_fabs_register_one(struct libm_test *test);
int libm_test_fabs_verify(struct libm_test *, struct libm_test_result *);
int test_fabs_populate_inputs(struct libm_test *, int);

/*scalar doulble precision routines*/
int test_fabs_init_s_d(struct libm_test_conf *);
long double libm_test_fabs(struct libm_test *, int);
int test_fabs_s1d_perf(struct libm_test *);
int test_fabs_conformance_setup(struct libm_test *);
int test_fabs_conformance(struct libm_test *);
int test_fabs_setup_s_d(struct libm_test *);
int test_fabs_accu_setup(struct libm_test *);
int test_fabs_accu(struct libm_test *);
int test_fabs_init_s_d(struct libm_test_conf *);


#endif
