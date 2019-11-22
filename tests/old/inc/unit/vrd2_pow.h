//R.obtained=FN_PROTOTYPE(vrd2_pow)(P1.input,P2.input)
//__m128d,__m128d,__m128d,ExcFlags
//P1 P2 R E
//unsigned long long int|__m128d, unsigned long long int|__m128d,unsigned long long int|__m128d,ExcFlags|ExcFlags
//R.V3[j]=amd_ref_pow(P1.V3[j],P2.V3[j])
//double double double ExcFlags
//ULP amd_ref_pow_ULP(P1.V3[j],P2.V3[j], R.V3[j], &testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

#include "pow.h"
