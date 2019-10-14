#include <libm_amd.h>
#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>


double
FN_PROTOTYPE(expm1_v2) (double x)
{
	return FN_PROTOTYPE_FMA3(expm1)(x);
}

