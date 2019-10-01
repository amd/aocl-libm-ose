/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */


#include <assert.h>                             /* for assert() */
#include <string.h>                             /* for bzero() and malloc() */

#include <libm_amd.h>
#include <libm_amd_paths.h>

#include <libm_tests.h>
#include <bench_timer.h>

void *libm_ptr_align_up(void *ptr, uint64_t align)
{
    uint64_t st = (uint64_t)ptr;

    assert(align > 0 && (align & (align - 1)) == 0); /* Power of 2 */
    assert(st != 0);

    if (st % align != 0)
        st += align - st % align;

    return (void *)st;
}

/*
 * CAUTION:
 *         Touch this with care, this allocation happens to be
 *    one-shot + multiple alignments.
 *    Could be simpler, but needs to revisit with care.
 *  Internal fragmentation expected, but since we align every pointer
 *  to the 256/512 bytes, cache trasing may be avoided.
 */
int libm_test_alloc_test_data(struct libm_test *test, uint32_t nelem)
{
#ifdef LIBM_AVX512_SUPPORTED
#define _ALIGN_FACTOR 512
#else
#define _ALIGN_FACTOR 256
#endif
    struct libm_test_data *test_data = &test->test_data;
    int nargs = test->nargs;
    uint32_t arr_size = nelem * libm_test_get_data_size(test->variant);
    uint32_t sz = (arr_size << 1) + _ALIGN_FACTOR;

    test_data->nelem = nelem;
    /* CAUTION */

    test_data->input1 = aligned_alloc(_ALIGN_FACTOR, sz);
    test_data->expected_exception = aligned_alloc(_ALIGN_FACTOR, sz);
    test_data->raised_exception = aligned_alloc(_ALIGN_FACTOR, sz);
    if (nargs > 1) {
        test_data->input2 = aligned_alloc(_ALIGN_FACTOR, sz);
        if (!test_data->input2)
            goto free_input1_out;
    }

    if (nargs > 2) {
        test_data->input3 = aligned_alloc(_ALIGN_FACTOR, sz);
        if (!test_data->input2)
            goto free_input3_out;
    }

    test_data->output = aligned_alloc(_ALIGN_FACTOR, sz);
    if (!test_data->output)
        switch(nargs) {
        case 1: goto free_input1_out; break;
        case 2: goto free_input2_out; break;
        case 3: goto free_input3_out; break;
        }

    test_data->expected = aligned_alloc(_ALIGN_FACTOR, sz);
    if (!test_data->expected)
        goto free_output_out;

    LIBM_TEST_DPRINTF(DBG2, "test_data:%p input1:%p output:%p expected:%p\n",
	   test_data, test_data->input1,
           test_data->output, test_data->expected);

    return 0;

 free_input3_out:
    free(test_data->input3);
 free_input2_out:
    free(test_data->input2);
 free_input1_out:
    free(test_data->input1);
 free_output_out:
    free(test_data->output);

    LIBM_TEST_DPRINTF(PANIC, "Not enough memory for test data\n");

    return -1;
}

struct libm_test *
libm_test_alloc_init(struct libm_test_conf *conf, struct libm_test *template)
{
    struct libm_test *test = malloc(sizeof(struct libm_test));
    if (!test) {
        LIBM_TEST_DPRINTF(PANIC, "Not enough memory for test->conf\n");
        goto out;
    }

    if (template)
        memcpy(test, template, sizeof(*test));

    test->conf = malloc(sizeof(struct libm_test_conf));
    if (!test->conf) {
        LIBM_TEST_DPRINTF(PANIC, "Not enough memory for test->conf\n");
        goto free_out;
    }

    memcpy(test->conf, conf, sizeof(*conf));

    return test;

 free_out:
    free(test);
 out:
    return NULL;
}

int libm_test_free_test_data(struct libm_test *test)
{
    struct libm_test_data *test_data= &test->test_data;
    free(test_data->input3);

    free(test_data->input2);

    free(test_data->input1);

    free(test_data->output);

    return 0;
}

int
libm_test_free(struct libm_test *test)
{
    libm_test_free_test_data(test);

    free(test);

    return 0;
}
