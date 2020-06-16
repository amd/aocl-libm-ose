#ifndef __TEST_SIN_H__
#define __TEST_SIN_H__

int test_sin_verify(struct libm_test *test, struct libm_test_result *result);

int test_sin_register_one(struct libm_test *test);

int test_sin_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_SIN_H__ */
