#include <stdio.h>
#include <float.h>                              /* for DBL_MAX/FLT_MAX */
#include <math.h>
#include <strings.h>                            /* for ffs() */
#include <string.h>                             /* for memcpy() */

#include <immintrin.h>
#include <fmaintrin.h>

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>

#include <libm_tests.h>
#include <bench_timer.h>

#include <libm/types.h>

static char *libm_test_variant_str(uint32_t variant)
{
    switch(variant) {
    case LIBM_FUNC_S_S:
        return "s1s";
    case LIBM_FUNC_S_D:
        return "s1d";
    case LIBM_FUNC_V2S:
        return "v2s";
    case LIBM_FUNC_V4S:
        return "v4s";
    case LIBM_FUNC_V8S:
        return "v8s";
    case LIBM_FUNC_V2D:
        return "v2d";
    case LIBM_FUNC_V4D:
        return "v4d";
    default:
        break;
    }

    return "unknown";
}

int libm_test_register_one(struct libm_test *test)
{
	int ret = 0;

	ret = libm_test_register(test);

	if (ret)
		goto out;
out:
	return ret;
}

int libm_test_type_setup(struct libm_test_conf *conf,
                         struct libm_test *template,
                         struct libm_test_funcs *funcs,
                         char *type_name, uint32_t input_type)
{
    int ret = 0;

    // Sanitary check
    uint32_t test_types = conf->test_types;
    while(test_types) {
        uint32_t bit = 1 << (ffs(test_types) - 1);
        struct libm_test_ops *ops = NULL;
        struct libm_test *test;

        test = libm_test_alloc_init(conf, template);
        if (!test) {
            LIBM_TEST_DPRINTF(PANIC, "Unable to allocate memory for test\n");
            continue;
        }

        test->name = type_name;
        test->variant = input_type;
        test->input_name = libm_test_variant_str(input_type);

        test_types = test_types & (test_types -  1);
        switch(bit) {
        case TEST_TYPE_PERF:
            test->type_name = "perf";
            test->test_type =  TEST_TYPE_PERF;
            ops = &funcs->performance;
            break;
        case TEST_TYPE_SPECIAL:
            test->type_name = "special";
            test->test_type =  TEST_TYPE_SPECIAL;
            ops = &funcs->special;
            break;
        case TEST_TYPE_ACCU:
            test->type_name = "accuracy";
            test->test_type =  TEST_TYPE_ACCU;
            ops = &funcs->accuracy;
            break;
        case TEST_TYPE_CORNER:
            test->type_name = "corner";
            test->test_type =  TEST_TYPE_CORNER;
            ops = &funcs->corner;
            break;
         case TEST_TYPE_CONFORMANCE:
            test->type_name = "conform";
            test->test_type =  TEST_TYPE_CONFORMANCE;
            ops = &funcs->conformance;
            break;

        default:
            goto out;
        }

	if (!ops->setup && !ops->run)
            goto free_test;

        /* Override only the ones provided */
        if (ops->setup)
            test->ops.setup = ops->setup;
        if (ops->run)
            test->ops.run = ops->run;
        if (ops->cleanup)
            test->ops.cleanup = ops->cleanup;
        if (ops->ulp.func || ops->ulp.funcq) /* test for any should be good */
            test->ops.ulp = ops->ulp;

        if (ops->verify)
            test->ops.verify = ops->verify;

        /* Finally call setup if exists, it should otherwise its an error */
        if (test->ops.setup)
            ret = test->ops.setup(test);
        else {
            LIBM_TEST_DPRINTF(PANIC, "Test %s variant:%d type:%s\n",
                              test->name, bit,
                              test->type_name);
            ret = -1;
        }

        if (ret)
            goto out;

        ret = libm_test_register_one(test);
        if (ret)
            goto free_test;

        continue;

    free_test:
        libm_test_free(test);
    }

    return 0;

 out:
    return -1;

}

int libm_tests_setup(struct libm_test_conf *conf,
                     struct libm_test_funcs test_table[LIBM_FUNC_MAX],
                     struct libm_test *template)
{
    uint32_t variants = conf->variants;
    int ret = 0;

    while (variants) {
        uint32_t bit = 1 << (ffs(variants) - 1);
        char *name;
        struct libm_test_funcs *funcs = &test_table[bit];

        variants = variants & (variants -  1);

        switch(bit) {
        case LIBM_FUNC_S_S:
        case LIBM_FUNC_S_D:
            name = "scalar";
            break;
        case LIBM_FUNC_V2S:
        case LIBM_FUNC_V4S:
        case LIBM_FUNC_V8S:
        case LIBM_FUNC_V2D:
        case LIBM_FUNC_V4D:
            name = "vector";
            break;
        default:
            LIBM_TEST_DPRINTF(PANIC, "unknown for variant:%s\n",
                              libm_test_variant_str(bit));
            continue;
        }

	ret = libm_test_type_setup(conf, template, funcs, name, bit);
        if (ret) {
            LIBM_TEST_DPRINTF(PANIC, "unable to setup %s\n", name);
        }
    }

    return 0;
}

typedef int (*rand_func_t)(void *, size_t, uint32_t, double, double);

static rand_func_t libm_test_get_rand_func(enum libm_test_range_type type)
{
    switch(type)
    {
    case LIBM_INPUT_RANGE_LINEAR:
        return libm_test_populate_range_uniform;

    case LIBM_INPUT_RANGE_RANDOM:
        return libm_test_populate_range_rand;

    default:
        LIBM_TEST_DPRINTF(PANIC, "Unknown type %d, returning RANG_SIMPLE\n", type);
    case LIBM_INPUT_RANGE_SIMPLE:
        break;
    }
    return libm_test_populate_range_linear;
}

#define is_range_empty(x) (!(x)->start && !(x)->stop)
static int libm_test_fixup_ranges(struct libm_test *test)
{
    struct libm_test_conf *conf = test->conf;

    for (uint32_t i = 0; i < ARRAY_SIZE(conf->inp_range); i++) {
        struct libm_test_input_range *r = &conf->inp_range[i];
        if (is_range_empty(r)) {
            if (test_is_single_precision(test)) {
                r->start = FLT_MIN;
                r->stop  = FLT_MAX;
            } else {
                r->start = DBL_MIN;
                r->stop  = DBL_MAX;
            }
        }
    }

    return 0;
}

int libm_test_populate_inputs(struct libm_test *test, int use_uniform)
{
    struct libm_test_data *data = &test->test_data;
    struct libm_test_conf *conf = test->conf;
    int ret = 0;
    rand_func_t func = NULL;

    /* If range is not given */
    ret = libm_test_fixup_ranges(test);
    if (ret)
        goto out_ret;

    func = libm_test_get_rand_func(conf->inp_range[0].type);
    if (func)
        ret = func(data->input1, data->nelem, test->variant,
                   conf->inp_range[0].start, conf->inp_range[0].stop);
    else
        goto out_ret;

    if (ret)
        goto out_ret;

    /* Fill the same if more inputs are needed */
    if (!ret && test->nargs > 1) {
        func = libm_test_get_rand_func(conf->inp_range[1].type);
        if (func)
            ret = func(data->input2, data->nelem, test->variant,
                       conf->inp_range[1].start, conf->inp_range[1].stop);
        else goto out_ret;
    }
    if (ret)
        goto out_ret;

    if (!ret && test->nargs > 2) {
        func = libm_test_get_rand_func(conf->inp_range[2].type);
	if (func)
            ret = func(data->input3, data->nelem, test->variant,
                       conf->inp_range[2].start, conf->inp_range[2].stop);
        else goto out_ret;
    }

    if (ret)
        goto out_ret;

    return ret;
out_ret:
    LIBM_TEST_DPRINTF(PANIC, "Unable to get random function for test:%s type:%s input:%s",
		      test_get_name(test), test_get_test_type(test),
		      test_get_input_type(test));

    return -1;
}

/***************setup functions***************************/
/********************PERF SETUP***************************/
int libm_setup_scalar_perf(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;
    int ret=0;
    ret=libm_test_alloc_test_data(test, conf->nelem);
    if (ret) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test data\n");
        goto out;
    }
    ret = libm_test_populate_inputs(test, LIBM_INPUT_RANGE_SIMPLE);

    if(ret || !test->test_data.input1) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to populate test data\n");
        goto out;
    }
    return 0;

out:
    return -1;
}

/*********special data alloc**************************/
//alloc special data
int libm_test_alloc_special_data(struct libm_test *test, size_t size)
{
    struct libm_test_conf *conf = test->conf;
    struct libm_test_data *test_data = &test->test_data;
    int ret = 0;

    ret = libm_test_alloc_test_data(test, size);

    if (ret) {
        printf("unable to allocate\n");
        goto out;
    }

    test_data = &test->test_data;
    test_data->nelem = size;

    /* fixup conf */
    conf->nelem = size;

    return 0;

 out:
    return -1;
}

/******************conformance setup************************/
int libm_setup_s1s_conf(struct libm_test *test, struct __libm_test_conformance_test_data_float* libm_test_conf_data, int test_data_size)
{
    float *in1,*expected;
    struct libm_test_data *data;
    int i;
    int32_t *expected_exception;

    libm_test_alloc_special_data(test, test_data_size);   

    data = &test->test_data;
    in1 = (float*)data->input1;
    expected = (float*)data->expected;
    expected_exception =  data->expected_exception;
    flt32u_t x,z;

    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_conf_data[i].in1;
       z.i = libm_test_conf_data[i].out;
       in1[i] = x.f;
       expected[i] = z.f;
       expected_exception[i] = libm_test_conf_data[i].exception_flags;
    }

    return 0;
}

int libm_setup_s1d_conf(struct libm_test *test, struct __libm_test_conformance_test_data_double* libm_test_conf_data, int test_data_size)
{
    double *in1, *expected;
    struct libm_test_data *data;
    int i;
    int32_t *expected_exception;

    libm_test_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (double*)data->input1;
    expected = (double*)data->expected;
    expected_exception =  data->expected_exception;
    flt64u_t x,z;

    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_conf_data[i].in1;
       z.i = libm_test_conf_data[i].out;
       in1[i] = x.d;
       expected[i] = z.d;
       expected_exception[i] = libm_test_conf_data[i].exception_flags;
    }

    return 0;
}

/*for nargs is 2*/
int libm_setup_s1s_conf_2(struct libm_test *test, struct __libm_test_conformance_test_data_float_2* libm_test_conf_data, int test_data_size)
{
    float *in1,*in2, *expected;
    struct libm_test_data *data;
    int i;
    int32_t *expected_exception;

    libm_test_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (float*)data->input1;
    in2 = (float*)data->input2;
    expected = (float*)data->expected;
    expected_exception =  data->expected_exception;
    flt32u_t x,y,z;

    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_conf_data[i].in1;
       y.i = libm_test_conf_data[i].in2;
       z.i = libm_test_conf_data[i].out;
       in1[i] = x.f;
       in2[i] = y.f;
       expected[i] = z.f;
       expected_exception[i] = libm_test_conf_data[i].exception_flags;
    }

    return 0;
}

int libm_setup_s1d_conf_2(struct libm_test *test, struct __libm_test_conformance_test_data_double_2* libm_test_conf_data, int test_data_size)
{
    double *in1,*in2,*expected;
    struct libm_test_data *data;
    int i;
    int32_t *expected_exception;

    libm_test_alloc_special_data(test, test_data_size);

    data = &test->test_data;

    in1 = (double*)data->input1;
    in2 = (double*)data->input2;
    expected = (double*)data->expected;
    expected_exception =  data->expected_exception;
    flt64u_t x,y,z;

    // Populate exception test data
    for (i = 0 ; i < test_data_size ; i++) {
       x.i = libm_test_conf_data[i].in1;
       y.i = libm_test_conf_data[i].in2;
       z.i = libm_test_conf_data[i].out;
       in1[i] = x.d;
       in2[i] = y.d;
       expected[i] = z.d;
       expected_exception[i] = libm_test_conf_data[i].exception_flags;
    }

    return 0;
}

/*****special functions*******/
int libm_setup_s1s_special(struct libm_test *test, struct __libm_test_internal_data_float* libm_test_special_data, int test_data_size)
{
    struct libm_test_data *data;
    float *in1, *expected;
    flt32u_t x,z;
    libm_test_alloc_special_data(test, test_data_size);
    
    data = &test->test_data;
    in1 = data->input1;
    expected = data->expected;
    for(int i=0; i < test_data_size; i++) {
        x.i = libm_test_special_data[i].in1;
        z.i = libm_test_special_data[i].out;
        in1[i] = x.f;
        expected[i] = z.f;
    }
    return 0;
}

int libm_setup_s1s_special_2(struct libm_test *test, struct __libm_test_internal_data_float_2* libm_test_special_data, int test_data_size)
{
    struct libm_test_data *data;
    float *in1, *in2, *expected;
    flt32u_t x,y,z;
    libm_test_alloc_special_data(test, test_data_size);

    data = &test->test_data;
    in1 = data->input1;
    in2 = data->input2;
    expected = data->expected;
    for(int i=0; i < test_data_size; i++) {
        x.i = libm_test_special_data[i].in1;
        y.i = libm_test_special_data[i].in2;
        z.i = libm_test_special_data[i].out;
        in1[i] = x.f;
        in2[i] = y.f;
        expected[i] = z.f;
    }
    return 0;
}

int libm_setup_s1d_special(struct libm_test *test, struct __libm_test_internal_data_double* libm_test_special_data, int test_data_size)
{
    struct libm_test_data *data;
    double *in1, *expected;
    flt64u_t x,z;
    libm_test_alloc_special_data(test, test_data_size);

    data = &test->test_data;
    in1 = data->input1;
    expected = data->expected;
    for(int i=0; i < test_data_size; i++) {
        x.i = libm_test_special_data[i].in1;
        z.i = libm_test_special_data[i].out;
        in1[i] = x.d;
        expected[i] = z.d;
    }
    return 0;
}

int libm_setup_s1d_special_2(struct libm_test *test, struct __libm_test_internal_data_double_2* libm_test_special_data, int test_data_size)
{
    struct libm_test_data *data;
    double *in1, *in2, *expected;
    flt64u_t x,y,z;
    libm_test_alloc_special_data(test, test_data_size);

    data = &test->test_data;
    in1 = data->input1;
    in2 = data->input2;
    expected = data->expected;
    for(int i=0; i < test_data_size; i++) {
        x.i = libm_test_special_data[i].in1;
        y.i = libm_test_special_data[i].in2;
        z.i = libm_test_special_data[i].out;
        in1[i] = x.d;
        in2[i] = y.d;
        expected[i] = z.d;
    }
    return 0;
}

