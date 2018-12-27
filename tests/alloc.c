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
void *libm_test_alloc_test_data(struct libm_test *test, uint32_t nelem)
{
#ifdef LIBM_AVX512_SUPPORTED
#define _ALIGN_FACTOR 512
#else
#define _ALIGN_FACTOR 256
#endif
    struct libm_test_data *test_data;
    //void *last_ptr;
    int nargs = test->nargs;
    uint32_t arr_size = nelem * libm_test_get_data_size(test->variant);
    uint32_t sz = (arr_size << 1) + _ALIGN_FACTOR;

    printf("nelem:%d size:%d\n", nelem, arr_size);

    test_data = calloc(1, sizeof(*test_data));

    if (!test_data)
        goto out;

    test_data->nelem = nelem;
    /* CAUTION */

    test_data->input1 = aligned_alloc(_ALIGN_FACTOR, sz);

    //last_ptr=test_data->input1;

    if (nargs > 1) {
        test_data->input2 = aligned_alloc(_ALIGN_FACTOR, sz);
        //last_ptr = test_data->input2;
    }

    if (nargs > 2) {
        test_data->input3 = aligned_alloc(_ALIGN_FACTOR, sz);
        //last_ptr = test_data->input2;
    }

    test_data->output = aligned_alloc(_ALIGN_FACTOR, sz);

    test_data->expected = aligned_alloc(_ALIGN_FACTOR, sz);

    printf("test_data:%p input1:%p output:%p expected:%p\n", test_data, test_data->input1,
           test_data->output, test_data->expected);
out:
    return test_data;

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

//static int test_exp2_vrd4_accu_setup(struct libm_test *test)
int test_exp2_alloc_test_data(struct libm_test *test)
{
    const struct libm_test_conf *conf = test->conf;

    test->test_data = libm_test_alloc_test_data(test, conf->nelem);

    if (!test->test_data) {
        LIBM_TEST_DPRINTF(PANIC, "Unable to allocate test_data for %s %s\n",
                          test->name, libm_test_variant_str(test->variant));
        return -1;
    }


    return 0;
}
