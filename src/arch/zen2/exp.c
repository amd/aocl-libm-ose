#define  ALM_OVERRIDE 1

#include <libm_macros.h>
#include <libm/types.h>

#define  ALM_ARCH_FN_ZEN2(x) 		x##_zn2
#define  ALM_ARCH_FN(x)  		ALM_ARCH_FN_ZEN2(x)

#define  ALM_PROTO_ZEN2(x)		ALM_ARCH_FN(amd_##x)
#define  FN_PROTOTYPE (x) 		ALM_PROTO_ZEN2(x)

#ifdef FN_PROTOTYPE_OPT
#undef FN_PROTOTYPE_OPT
#define  FN_PROTOTYPE_OPT(x) 		ALM_PROTO_ZEN2(x)
#endif


#include "../../optmized/exp.c"

