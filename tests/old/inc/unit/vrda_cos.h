//FN_PROTOTYPE(vrda_cos)(len.input,src.obtained,dst.obtained)
//double *,double *,ExcFlags *
//src dst E
//unsigned long long|double,unsigned long long|double, ExcFlags|ExcFlags
//BUFFER
//len|int|len.SetData(LENGTH_OF(input),LENGTH_OF(input))
//dst.V3[0] = amd_ref_cos(src.V3[0])
//double double ExcFlags
//ULP  amd_ref_cos_ULP(testdata.src.input[i],testdata.dst.obtained[i],&testdata.dst.ulp[i],&testdata.dst.relative_error[i]);
//testdata.dst.MaxUlp()

#include "cos.h"


