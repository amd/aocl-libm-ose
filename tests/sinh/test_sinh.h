#ifndef __TEST_SINH_H__
#define __TEST_SINH_H__

int test_sinh_verify(struct libm_test *test, struct libm_test_result *result);

int test_sinh_register_one(struct libm_test *test);

int test_sinh_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_SINH_H__ */
