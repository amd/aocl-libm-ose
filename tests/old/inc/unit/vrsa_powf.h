//FN_PROTOTYPE(vrsa_powf)(len.input,src1.obtained,src2.obtained,dst.obtained)
//float *,float *,float *,ExcFlags *
//src1 src2 dst E
//unsigned int|float,unsigned int|float,unsigned int|float, ExcFlags|ExcFlags
//BUFFER
//len|int|len.SetData(LENGTH_OF(input),LENGTH_OF(input))
//dst.V3[0] = amd_ref_powf(src1.V3[0],src2.V3[0])
//float float float ExcFlags
//ULP  amd_ref_powf_ULP(testdata.src1.input[i],testdata.src2.input[i],testdata.dst.obtained[i],&testdata.dst.ulp[i],&testdata.dst.relative_error[i]);
//testdata.dst.MaxUlp()

#undef WINDOWS
#include "powf.h"

