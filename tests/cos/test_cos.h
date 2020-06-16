#ifndef __TEST_COS_H__
#define __TEST_COS_H__

int test_cos_verify(struct libm_test *test, struct libm_test_result *result);

int test_cos_register_one(struct libm_test *test);

int test_cos_alloc_init(struct libm_test_conf *conf, struct libm_test *test);

#endif	/* __TEST_COS_H__ */
