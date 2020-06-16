#ifndef __TEST_TAN_H__
#define __TEST_TAN_H__

int test_tan_verify(struct libm_test *test, struct libm_test_result *result);

int test_tan_register_one(struct libm_test *test);

int test_tan_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_TAN_H__ */
