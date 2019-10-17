#ifndef __TEST_ATAN_H__
#define __TEST_ATAN_H__

int test_atan_verify(struct libm_test *test, struct libm_test_result *result);

int test_atan_register_one(struct libm_test *test);

int test_atan_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_ATAN_H__ */
