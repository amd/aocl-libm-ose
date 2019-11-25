#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#include "Constants.h"
#include "DataTypes.h"
struct data {
unsigned int src;
unsigned int sinr;
unsigned int cosr;
ExcFlags E;
};
union src {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union sinr {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union cosr {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union E {
ExcFlags V1[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V2;
};
#include "../unit/vrsa_sincosf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(float)/sizeof(unsigned int),sizeof(float)/sizeof(unsigned int)),sizeof(float)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData{public:Timer timer;
ParameterObjBuffer<float,float *> src;
ParameterObjBuffer<float,float *> sinr;
ParameterObjBuffer<float,float *> cosr;
ParameterObjBuffer<ExcFlags,ExcFlags *> E;
ParameterObj<int> len;
void Report(){TestReport::TestHeader();
src.Report();
sinr.Report();
cosr.Report();
len.Report();
TestReport::TestFooter(timer.GetStats().Time());}
void SetName(){
src.SetName("src");
sinr.SetName("sinr");
cosr.SetName("cosr");
E.SetName("E");
len.SetName("len");
}
void Run(){
corrupt_reg(randomNumber<unsigned long long,unsigned long long>());
timer.Start();FN_PROTOTYPE(vrsa_sincosf)(len.input,src.obtained,sinr.obtained,cosr.obtained);timer.Sample();}};
int main(){
UnitTestData testdata;
testdata.len.SetData(LENGTH_OF(input),LENGTH_OF(input));
testdata.src.AllocateMem(LENGTH_OF(input));
testdata.sinr.AllocateMem(LENGTH_OF(input));
testdata.cosr.AllocateMem(LENGTH_OF(input));
testdata.E.AllocateMem(LENGTH_OF(input));
for(unsigned int i=0; i< LENGTH_OF(input); i++){
src src;
sinr sinr;
cosr cosr;
E E;
src.V1[0]=input[i].src;
sinr.V1[0]=input[i].sinr;
cosr.V1[0]=input[i].cosr;
E.V1[0]=input[i].E;
testdata.src.SetDatatoBuffer(src.V2,src.V2);
testdata.sinr.SetDatatoBuffer(sinr.V2,sinr.V2);
testdata.cosr.SetDatatoBuffer(cosr.V2,cosr.V2);
testdata.E.SetDatatoBuffer(E.V2,E.V2);
}
testdata.SetName();
testdata.Run();
TestReport::Header("vrsa_sincosf.txt");
TestReport::FuncHeader("FN_PROTOTYPE(vrsa_sincosf)(len.input,src.obtained,sinr.obtained,cosr.obtained)");
testdata.Report();
TestReport::Footer();

return 0;
}
