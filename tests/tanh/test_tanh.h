#ifndef __TEST_TANH_H__
#define __TEST_TANH_H__

int test_tanh_verify(struct libm_test *test, struct libm_test_result *result);

int test_tanh_register_one(struct libm_test *test);

int test_tanh_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_TANH_H__ */
