//R.obtained=FN_PROTOTYPE(vrs4_expm1f)(P1.input)
//__m128,__m128,ExcFlags
//P1 R E
//unsigned  int|__m128, unsigned  int|__m128,ExcFlags|ExcFlags
//R.V3[j]=amd_ref_expm1f(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_expm1f_ULP(P1.V3[j], R.V3[j], &testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

#include "expm1f.h"
