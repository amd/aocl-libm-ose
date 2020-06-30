#ifndef __ALM_ARCH_ZEN2_H__
#define __ALM_ARCH_ZEN2_H__

#define  ALM_OVERRIDE 1

#define ALM_PREFIX			amd
#define ALM_ARCH			zn2

#define  ALM_PROTO_OPT(x) 		ALM_PROTO_ARCH(ALM_PREFIX, x, ALM_ARCH)

#define  ALM_PROTO_ARCH(a, x, y)	ALM_MAKE_PROTO_SFX(a, x, y)

#include <libm_macros.h>
#include <libm/types.h>

#ifdef FN_PROTOTYPE_OPT
#undef FN_PROTOTYPE_OPT
#define FN_PROTOTYPE_OPT(x)		ALM_PROTO_OPT(x)
#endif

#endif

