#ifndef __TEST_EXP2_H__
#define __TEST_EXP2_H__

int libm_test_exp_verify(struct libm_test *test, struct libm_test_result *result);

int test_exp_register_one(struct libm_test *test);

int test_exp_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_EXP2_H__ */
