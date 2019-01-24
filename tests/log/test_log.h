#ifndef __TEST_LOG_H__
#define __TEST_LOG_H__

int libm_test_log_verify(struct libm_test *test, struct libm_test_result *result);

int test_log_register_one(struct libm_test *test);

int test_log_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_LOG_H__ */
