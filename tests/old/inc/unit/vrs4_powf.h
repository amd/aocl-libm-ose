//R.obtained=FN_PROTOTYPE(vrs4_powf)(P1.input,P2.input)
//__m128,__m128,__m128,ExcFlags
//P1 P2 R E
//unsigned  int|__m128, unsigned  int|__m128,unsigned  int|__m128,ExcFlags|ExcFlags
//R.V3[j]=amd_ref_powf(P1.V3[j],P2.V3[j])
//float float float ExcFlags
//ULP amd_ref_powf_ULP(P1.V3[j], P2.V3[j],R.V3[j], &testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

#undef WINDOWS
#include "powf.h"


