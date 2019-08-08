#include "libm_amd.h"
#include "libm_util_amd.h"

float FN_PROTOTYPE(remquof) (float x, float y, int *quo)
{
    return (float) FN_PROTOTYPE(remquo)(x,y,quo);
}
weak_alias (remquof, FN_PROTOTYPE(remquof))
