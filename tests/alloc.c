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

static void *libm_ptr_align_up(void *ptr, uint64_t align)
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
    void *last_ptr;
    int nargs = test->nargs;
    uint32_t arr_size = nelem * libm_test_get_data_size(test->variant);

    /*
     * libm functions with 1,2 and 3 args
     *          each one with sizeof double/float
     *
     *         + output +  expected (each with double/float)
     *         + size of the structure itself
     *         + 3 times size of _ALIGN_FACTOR
     */
    uint32_t sz = sizeof(*test_data) +
        ((nargs + 2) * arr_size ) +             /* +2 for output and expected */
        _ALIGN_FACTOR * (nargs + 2);


    test_data = aligned_alloc(_ALIGN_FACTOR, sz);

    if (!test_data)
        goto out;

    bzero (test_data, sz);

    test_data->nelem = nelem;
    /* CAUTION */
    test_data->input1 = libm_ptr_align_up(&test_data->data[0], _ALIGN_FACTOR);

    last_ptr=test_data->input1;

    if (nargs > 1) {
        test_data->input2 = libm_ptr_align_up(&test_data->input1[arr_size],
                                              _ALIGN_FACTOR);
        last_ptr = test_data->input2;
    }

    if (nargs > 2) {
        test_data->input3 = libm_ptr_align_up(&test_data->input2[arr_size],
                                              _ALIGN_FACTOR);
        last_ptr = test_data->input2;
    }

    test_data->output = libm_ptr_align_up(last_ptr + arr_size, _ALIGN_FACTOR);

    test_data->expected = libm_ptr_align_up(&test_data->output[nelem], _ALIGN_FACTOR);

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
