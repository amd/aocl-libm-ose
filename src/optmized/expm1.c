#include <libm_amd.h>
#include <libm_amd_paths.h>


double
FN_PROTOTYPE(expm1_v2) (double x)
{
	return FN_PROTOTYPE_FMA3(expm1)(x);
}

