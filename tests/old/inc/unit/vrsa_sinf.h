//FN_PROTOTYPE(vrsa_sinf)(len.input,src.obtained,dst.obtained)
//float *,float *,ExcFlags *
//src dst E
//unsigned int|float,unsigned int|float, ExcFlags|ExcFlags
//BUFFER
//len|int|len.SetData(LENGTH_OF(input),LENGTH_OF(input))
//dst.V3[0] = amd_ref_sinf(src.V3[0])
//float float ExcFlags
//ULP  amd_ref_sinf_ULP(testdata.src.input[i],testdata.dst.obtained[i],&testdata.dst.ulp[i],&testdata.dst.relative_error[i]);
//testdata.dst.MaxUlp()

#include "sinf.h"



