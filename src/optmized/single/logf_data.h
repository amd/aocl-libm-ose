#include <stdint.h>
#include <math.h>
#include <float.h>

#include "libm_amd.h"
#include "libm_util_amd.h"
#include "libm_special.h"
#include "libm_inlines_amd.h"
#include "libm_errno_amd.h"
#include "libm/typehelper.h"

struct logf_table {
    float_t f_inv, f_128_head, f_128_tail;
};

