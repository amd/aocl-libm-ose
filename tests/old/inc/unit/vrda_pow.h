//FN_PROTOTYPE(vrda_pow)(len.input,src1.obtained,src2.obtained,dst.obtained)
//double *,double *,double *,ExcFlags *
//src1 src2 dst E
//unsigned long long|double,unsigned long long|double, unsigned long long|double, ExcFlags|ExcFlags
//BUFFER
//len|int|len.SetData(LENGTH_OF(input),LENGTH_OF(input))
//dst.V3[0] = amd_ref_pow(src1.V3[0],src2.V3[0])
//double double double ExcFlags
//ULP  amd_ref_pow_ULP(testdata.src1.input[i],testdata.src2.input[i],testdata.dst.obtained[i],&testdata.dst.ulp[i],&testdata.dst.relative_error[i]);
//testdata.dst.ulp[i]


#include "pow.h"

