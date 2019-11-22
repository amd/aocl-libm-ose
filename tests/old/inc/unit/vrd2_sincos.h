//FN_PROTOTYPE(vrd2_sincos)(P1.input, &P2.obtained, &P3.obtained)
//__m128d, __m128d ,__m128d ,ExcFlags
//P1 P2 P3 E
//unsigned long long int|__m128d, unsigned long long int|__m128d, unsigned  long long int|__m128d,ExcFlags|ExcFlags
//amd_ref_sincos(P1.V3[j],&P2.V3[j],&P3.V3[j])
//double double double ExcFlags
//ULP amd_ref_sincos_ULP(P1.V3[j],P2.V3[j],P3.V3[j],&testdata[k].P2.ulp[j], &testdata[k].P2.relative_error[j])
//testdata[j].P2.MaxUlp()



#include "sincos.h"

