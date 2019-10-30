#ifndef __TEST_POW_H__
#define __TEST_POW_H__

int test_pow_verify(struct libm_test *test, struct libm_test_result *result);

int test_pow_register_one(struct libm_test *test);

int test_pow_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_POW_H__ */

