/*
 *
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 *
 */

#include <stdint.h>
#include <math.h>
#include <float.h>

struct logf_table {
    float_t f_inv, f_128_head, f_128_tail;
};

