#ifndef __ALM_ARCH_ZEN2_H__
#define __ALM_ARCH_ZEN2_H__

/*
 * This architecture is for "Zen2", same as --march=znver2
 */
#define ALM_ARCH			zn2

/*
 * Override all previously definded prototypes
 */
#if defined(ALM_OVERRIDE)

#include <libm_macros.h>
#include <libm/types.h>

#ifndef  ALM_PREFIX
#define  ALM_PREFIX			amd
#endif

#ifdef FN_PROTOTYPE_OPT
#undef FN_PROTOTYPE_OPT
#define FN_PROTOTYPE_OPT(x)		ALM_PROTO_OPT(x)
#endif

#ifdef FN_PROTOTYPE_REF
#undef FN_PROTOTYPE_REF
#define FN_PROTOTYPE_REF(x)		ALM_PROTO_ARCH_ZN2(x)

#define  ALM_PROTO_OPT(x) 		ALM_PROTO_ARCH_ZN2(x)

#endif

#endif /* ALM_OVERRIDE */


#define __ALM_PROTO_ARCH(a, x, y)	ALM_MAKE_PROTO_SFX(a, x, y)
#define  ALM_PROTO_ARCH(a, x, y) 	__ALM_PROTO_ARCH(a, x, y)		
#define  ALM_PROTO_ARCH_ZN2(x)		ALM_PROTO_ARCH(ALM_PREFIX, x, ALM_ARCH)

#endif

