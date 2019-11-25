#define _txt_
#define _FAIL_ONLY_
#define __1BDIFF__
#include <math.h>
#include "Constants.h"
#include "DataTypes.h"
struct data {
unsigned int src1;
unsigned int src2;
unsigned int dst;
ExcFlags E;
};
union src1 {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V3[sizeof(float)/sizeof(float)];
float V2;
};
union src2 {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V3[sizeof(float)/sizeof(float)];
float V2;
};
union dst {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V3[sizeof(float)/sizeof(float)];
float V2;
};
union E {
ExcFlags V1[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V3[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V2;
};
//#include "..\unit\vrsa_powxf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(float)/sizeof(unsigned int),sizeof(float)/sizeof(unsigned int)),sizeof(float)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData{public:Timer timer;
ParameterObjBuffer<float,float *> src1;
ParameterObj<float> src2;
ParameterObjBuffer<float,float *> dst;
ParameterObjBuffer<ExcFlags,ExcFlags *> E;
ParameterObj<int> len;
void Report(){TestReport::TestHeader();
src1.Report();
src2.Report();
dst.Report();
len.Report();
TestReport::TestFooter(timer.GetStats().Time());}
void SetName(){
src1.SetName("src1");
src2.SetName("src2");
dst.SetName("dst");
E.SetName("E");
len.SetName("len");
}
void Run(){timer.Start();FN_PROTOTYPE(vrsa_powxf)(len.input,src1.input,src2.input,dst.obtained);timer.Sample();}};
void RandomTest(int size,int seed)
{
UnitTestData testdata;
testdata.len.SetData(size,size);
testdata.src1.AllocateMem(size);
//testdata.src2.AllocateMem(size);
src2 src2;
src2.V3[0]=randomNumber<float,float>();
testdata.dst.AllocateMem(size);
for(int i=0; i< size; i++)
{
src1 src1;
dst dst;
src1.V3[0]=randomNumber<float,float>();

dst.V3[0]=randomNumber<float,float>();
dst.V3[0] = amd_ref_powf(src1.V3[0],src2.V3[0]);
testdata.src1.SetDatatoBuffer(src1.V2,src1.V2);
testdata.src2.SetDatatoBuffer(src2.V2,src2.V2);
testdata.dst.SetDatatoBuffer(src1.V2,dst.V2);
}
testdata.SetName();
testdata.Run();
double ulp_e = 0.0,rel_diff = 0.0;
TestReport::Header("vrsa_powxf_rand.txt",seed);
TestReport::FuncHeader("FN_PROTOTYPE(vrsa_powxf)(len.input,src1.input,src2.input,dst.obtained)    dst.V3[0] = amd_ref_powf(src1.V3[0],src2.V3[0])");
testdata.Report();
TestReport::Footer(ulp_e,rel_diff);
}
int main(){
RandomTest(10000,SetSeed());
return 0;
}
