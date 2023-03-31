#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/compiler.h>
#include <libm/poly.h>

void ALM_PROTO_OPT(vrda_add)(int len, double *lhs, double *rhs, double *dst)
{
    for (int i = 0; i < len ; i++)
    {
        dst[i] = lhs[i] + rhs[i];
    }
}