/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */
/*
 * Various random floating point number generator
 */
#include <stdlib.h>                             /* for rand() and RAND_MAX */
#include <stdint.h>                             /* for uint64_t */
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>                           /* for open() */
#include <fcntl.h>

#include <unistd.h>                             /* for read() */

#include <libm_tests.h>

/*
 * Generate a random floating point number from min to max
 * But then, floating point numbers itself is not uniformly distributed.
 * (towards 0 it is dense, not otherwise)
 */
double libm_test_rand_range_simple(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

static int libm_test_get_random_fd(void)
{
    static int rand_fd;
    rand_fd = open("/dev/urandom", O_RDONLY);
    if (rand_fd == -1)
        return (-1);

    return rand_fd;
}

int libm_test_init_rand()
{
    int rand_fd = libm_test_get_random_fd();
    uint64_t rand_val = 0xC001BEAFDEADBEAF;

    if (rand_fd > 0) {
        if (read(rand_fd, &rand_val, 8) != 8)
            rand_val = 0xD00BEEC001BEAF;

        srand(rand_val);
    }

    return 0;
}

int libm_test_get_data_size(uint32_t variant)
{
    int ret = 0;

    switch(variant) {
    case LIBM_FUNC_S_S:
    case LIBM_FUNC_V2S:
    case LIBM_FUNC_V4S:
    case LIBM_FUNC_V8S:
        ret = 4;
        break;

    default:
    case LIBM_FUNC_S_D:
    case LIBM_FUNC_V2D:
    case LIBM_FUNC_V4D:
        ret = 8;
        break;
    }

    return ret;
}

int libm_test_populate_rand_range_d(void *data,
                                    size_t nelem, uint32_t variant,
                                    double min, double max)
{
    int data_size = libm_test_get_data_size(variant);
    double *d = (double*)data;
    float *f = (float*)data;

    /* Check and warn for alignment */
    if (data_size == 4)
        assert(!((uint64_t)data & 0x3));
    if ((data_size == 8))
        assert(!((uint64_t)data & 0x7));

    /* Variant has multiple bits set. */
    if (variant & (variant - 1))
        return -1;

    double val = libm_test_rand_range_simple(min, max);

    for (uint32_t i = 0; i < nelem; i++) {
        switch(data_size) {
        case 4:
            *f++ = (float)val;
            break;
        case 8:
            *d++ = val;
            break;
        }
    }

    return 0;
}
