#ifndef __TEST_COSH_H__
#define __TEST_COSH_H__

int test_cosh_verify(struct libm_test *test, struct libm_test_result *result);

int test_cosh_register_one(struct libm_test *test);

int test_cosh_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_COSH_H__ */