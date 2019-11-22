//FN_PROTOTYPE(vrsa_sincosf)(len.input,src.obtained,sinr.obtained,cosr.obtained)
//float *,float *,float *,ExcFlags *
//src sinr cosr E
//unsigned int|float,unsigned int|float, unsigned int|float, ExcFlags|ExcFlags
//BUFFER
//len|int|len.SetData(LENGTH_OF(input),LENGTH_OF(input))
//amd_ref_sincosf(src.V3[0],&sinr.V3[0],&cosr.V3[0])
//float float float ExcFlags
//ULP amd_ref_sincosf_ULP(src.V3[0],&sinr.V3[0],&cosr.V3[0],&testdata[k].cosr.ulp[0], &testdata[k].cosr.relative_error[0])
//testdata[0].cosr.MaxUlp()

#include "sincosf.h"



