#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
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
float V2;
};
union src2 {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union dst {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union E {
ExcFlags V1[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V2;
};
#include "../unit/vrsa_powf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(float)/sizeof(unsigned int),sizeof(float)/sizeof(unsigned int)),sizeof(float)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData{public:Timer timer;
ParameterObjBuffer<float,float *> src1;
ParameterObjBuffer<float,float *> src2;
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
void Run(){
corrupt_reg(randomNumber<unsigned long long,unsigned long long>());
timer.Start();FN_PROTOTYPE(vrsa_powf)(len.input,src1.obtained,src2.obtained,dst.obtained);timer.Sample();}};
int main(){
UnitTestData testdata;
testdata.len.SetData(LENGTH_OF(input),LENGTH_OF(input));
testdata.src1.AllocateMem(LENGTH_OF(input));
testdata.src2.AllocateMem(LENGTH_OF(input));
testdata.dst.AllocateMem(LENGTH_OF(input));
testdata.E.AllocateMem(LENGTH_OF(input));
for(unsigned int i=0; i< LENGTH_OF(input); i++){
src1 src1;
src2 src2;
dst dst;
E E;
src1.V1[0]=input[i].src1;
src2.V1[0]=input[i].src2;
dst.V1[0]=input[i].dst;
E.V1[0]=input[i].E;
testdata.src1.SetDatatoBuffer(src1.V2,src1.V2);
testdata.src2.SetDatatoBuffer(src2.V2,src2.V2);
testdata.dst.SetDatatoBuffer(dst.V2,dst.V2);
testdata.E.SetDatatoBuffer(E.V2,E.V2);
}
testdata.SetName();
testdata.Run();
TestReport::Header("vrsa_powf.txt");
TestReport::FuncHeader("FN_PROTOTYPE(vrsa_powf)(len.input,src1.obtained,src2.obtained,dst.obtained)");
testdata.Report();
TestReport::Footer();

return 0;
}
