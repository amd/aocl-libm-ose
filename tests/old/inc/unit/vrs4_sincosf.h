//FN_PROTOTYPE(vrs4_sincosf)(P1.input, &P2.obtained, &P3.obtained)
//__m128, __m128,__m128,ExcFlags
//P1 P2 P3 E
//unsigned int|__m128, unsigned int|__m128, unsigned int|__m128,ExcFlags|ExcFlags
//amd_ref_sincosf(P1.V3[j],&P2.V3[j],&P3.V3[j])
//float float float ExcFlags
//ULP amd_ref_sincosf_ULP(P1.V3[j],&P2.V3[j],&P3.V3[j],&testdata[k].P2.ulp[j], &testdata[k].P2.relative_error[j])
//testdata[j].P2.MaxUlp()

#include "sincosf.h"