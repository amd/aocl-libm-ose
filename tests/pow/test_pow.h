#ifndef __TEST_POW_H__
#define __TEST_POW_H__

int libm_test_pow_verify(struct libm_test *test, struct libm_test_result *result);

int test_pow_register_one(struct libm_test *test);

int test_pow_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

int test_pow_populate_inputs(struct libm_test *test, int use_uniform);

int test_pow_populate_inputs(struct libm_test *test, int use_uniform);

int test_pow_alloc_data(struct libm_test *test, size_t size);

int test_pow_init_scalar(struct libm_test_conf *conf);

int test_pow_init_s_s(struct libm_test_conf *conf);

int test_pow_init_s_d(struct libm_test_conf *conf);


#endif	/* __TEST_EXP2_H__ */
