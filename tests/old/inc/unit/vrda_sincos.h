//FN_PROTOTYPE(vrda_sincos)(len.input,src.obtained,sinr.obtained,cosr.obtained)
//double *,double *,double *,ExcFlags *
//src sinr cosr E
//unsigned long long|double,unsigned long long|double, unsigned long long|double,ExcFlags|ExcFlags
//BUFFER
//len|int|len.SetData(LENGTH_OF(input),LENGTH_OF(input))
//amd_ref_sincos(src.V3[0],&sinr.V3[0],&cosr.V3[0])
//double double double ExcFlags
//ULP amd_ref_sincos_ULP(src.V3[0],sinr.V3[0],cosr.V3[0],&testdata[k].cosr.ulp[0], &testdata[k].cosr.relative_error[0])
//testdata[0].cosr.MaxUlp()

#include "sincos.h"



