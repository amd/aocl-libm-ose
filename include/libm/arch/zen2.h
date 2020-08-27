#ifndef __ALM_ARCH_ZEN2_H__
#define __ALM_ARCH_ZEN2_H__

/*
 * This architecture is for "Zen2", same as --march=znver2
 */
#define ALM_ARCH_ZN2			zn2
#define ALM_PREFIX_ZN2			amd

#ifndef ALM_PREFIX
#define ALM_PREFIX		ALM_PREFIX_ZN2
#endif

#ifndef ALM_ARCH
#define ALM_ARCH		ALM_ARCH_ZN2
#endif

/*
 * Override all previously definded prototypes
 */
#if defined(ALM_OVERRIDE)

#include <libm_macros.h>
//#include <libm/types.h>

#ifndef  ALM_PREFIX_ZN2
#undef   ALM_PREFIX_ZN2
#define  ALM_PREFIX_ZN2			amd
#endif

#ifdef FN_PROTOTYPE_OPT
#undef FN_PROTOTYPE_OPT
#endif
#define FN_PROTOTYPE_OPT(x)		ALM_PROTO_OPT(x)

#ifdef FN_PROTOTYPE_REF
#undef FN_PROTOTYPE_REF
#endif
#define FN_PROTOTYPE_REF(x)		ALM_PROTO_ARCH_ZN2(x)

#define  ALM_PROTO_OPT(x) 		ALM_PROTO_ARCH_ZN2(x)


#endif /* ALM_OVERRIDE */


#ifndef ALM_PROTO_ARCH
#define __ALM_PROTO_ARCH(a, x, y)		ALM_MAKE_PROTO_SFX(a, x, y)
#define  __ALM_MAKE_PROTO_ARCH(a, x, y) 	__ALM_PROTO_ARCH(a, x, y)		
#endif

#ifdef   ALM_PROTO_ARCH
#undef   ALM_PROTO_ARCH
#define  ALM_PROTO_ARCH(x)	        ALM_PROTO_ARCH_ZN2(x)
#endif

#define  ALM_PROTO_ARCH_ZN2(x)		__ALM_PROTO_ARCH(ALM_PREFIX_ZN2, x, ALM_ARCH_ZN2)

/*
#define ALM_PROTO_ARCH_CHANGE_INTERNAL  1
#include <libm/arch/alm_funcs.h>
#undef  ALM_PROTO_CHANGE_INTENRAL
*/
#pragma push_macro("ALM_PROTO_INTERNAL")
#define ALM_PROTO_INTERNAL ALM_PROTO_ARCH
#include "../__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL
#pragma pop_macro("ALM_PROTO_INTERNAL")

#endif  /* _ALM_ARCH_ZEN2_H_ */

