/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

#include <string.h>                             /* for memcpy() */
#include <stdlib.h>

#include <libm_amd.h>
#include <libm_tests.h>
#include <bench_timer.h>

static int test_exp2_init_s_d(struct libm_test_conf *conf)
{
    return 0;
}

static int test_exp2_setup_s_s(struct libm_test *test)
{
    return 0;
}

static int test_exp2_init_s_s(struct libm_test_conf *conf)
{
    struct libm_test *exp2_test = malloc(sizeof(struct libm_test));
    int ret = 0;

    if (!exp2_test)
        return -1;

    exp2_test->name = "exp2_scalar";

    exp2_test->ops.setup = test_exp2_setup_s_s;
    ret = libm_test_register(exp2_test);
    if (ret)
        goto out;

 out:
    return ret;
}

int test_exp2_init_scalar(struct libm_test_conf *conf)
{
    int ret =0;

    if (conf->variants & LIBM_FUNC_S_S) {
        ret = test_exp2_init_s_s(conf);
        if (ret)
            goto out;
    }

    if (conf->variants & LIBM_FUNC_S_D) {
        ret = test_exp2_init_s_d(conf);
        if (ret)
            goto out;
    }

 out:
    return ret;
}
