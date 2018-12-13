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
#include <math.h>

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

static int check_data_size(void *data, int size)
{
    /* Check and warn for alignment */
    if (size == 4)
        assert(!((uint64_t)data & 0x3));
    if ((size == 8))
        assert(!((uint64_t)data & 0x7));

    return 0;
}

int libm_test_populate_range_simple(void *data,
                                    size_t nelem, uint32_t variant,
                                    double min, double max)
{
    int data_size = libm_test_get_data_size(variant);
    double *d = (double*)data;
    float *f = (float*)data;

    libm_test_init_rand();

    check_data_size(data, data_size);

    /* Variant has multiple bits set. */
    if (variant & (variant - 1))
        return -1;

    for (uint32_t i = 0; i < nelem; i++) {
        double val = libm_test_rand_range_simple(min, max);

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


static double __myrand(void)
{
    static unsigned long seed = 123456789;
    const unsigned long a = 9301;
    const unsigned long c = 49297;
    const unsigned long m = 233280;

    seed = (seed * a + c) % m;
    return (double)seed / (double)m;
}

/*
 * Generate uniformly distributed range in interval [a, b]
 */
static double logdist2(int i, int n, double a, double b, double logba)
{
    double x, tx;

    if (i <= 0)
        tx = 0.0;
    else if (i >= n - 1)
        tx = 1.0;
    else
        tx = (double)((i + __myrand() - 0.5) / (n - 1));

    /* tx is uniformly distributed in [0,1] */
    if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0))) {
        /* x is logarithmically distributed in [a,b] */
        if (a < 0.0) {
            /* Negative interval: negate b, tx and x so that
               the numbers are distributed in a similar fashion to
               the equivalent positive interval. Doesn't really
               make much difference */
            if (tx == 0.0)      x = b;
            else if (tx == 1.0) x = a;
            else                x = b * exp(-tx * logba);
        } else {
            if (tx == 0.0)              x = a;
            else if (tx == 1.0)         x = b;
            else                        x = a * exp(tx * logba);
        }
    } else {
        /* x is uniformly distributed in [a,b] */
        if (tx == 0.0)          x = a;
        else if (tx == 1.0)     x = b;
        else                    x = a + tx * (b - a);
    }

    if (x < a)          x = a;
    else if (x > b)     x = b;

    return x;

}

static int __populate_range(void *data,
			    size_t nelem, uint32_t variant,
			    double min, double max, int uniform)
{
    int data_size = libm_test_get_data_size(variant);
    double logba;
    double *d = (double*)data;
    float *f = (float*)data;

    check_data_size(data, data_size);

    if (uniform &&
        min != 0.0 && max != 0.0 && ((min < 0.0) == (min < 0.0)))
        logba = log(max/min);
    else
        logba = 0.0;

    for (uint32_t i = 0; i < nelem; i++) {
        double val = 0.0;
        if (uniform)
            val = logdist2(i, nelem, min, max, logba);
        else
            val = (min * (nelem - i) + max * i) / nelem;

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

int libm_test_populate_range_uniform(void *data,
                                     size_t nelem, uint32_t variant,
                                     double min, double max)
{

    return __populate_range(data, nelem, variant, min, max, 1);
}


int libm_test_populate_range_linear(void *data,
                                    size_t nelem, uint32_t variant,
                                    double min, double max)
{
    return __populate_range(data, nelem, variant, min, max, 0);

}
