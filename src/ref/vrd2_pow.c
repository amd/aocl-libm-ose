/*This is quick prototype for the pow vector variant for the
 *Open64 team. */
#include "libm_amd.h"

__m128d FN_PROTOTYPE_FMA3(vrd2_pow)(__m128d, __m128d);

__m128d FN_PROTOTYPE(vrd2_pow)(__m128d x, __m128d y)
{

	return FN_PROTOTYPE_FMA3(vrd2_pow)(x, y);;
}

weak_alias (__vrd2_pow, FN_PROTOTYPE(vrd2_pow))
