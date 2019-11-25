#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#include "Constants.h"
#include "DataTypes.h"
struct data {
unsigned int P1;
unsigned int R;
ExcFlags E;
};
union P1 {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union R {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V2;
};
union E {
ExcFlags V1[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V2;
};
#include "../unit/rintf_RT_UP.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(sizeof(float)/sizeof(unsigned int),sizeof(float)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData{public:Timer timer;
ParameterObj<float> P1;
ParameterObj<float> R;
ParameterObj<ExcFlags> E;
void Report(){TestReport::TestHeader();
P1.Report();
R.Report();
TestReport::TestFooter(timer.GetStats().Time());}
void SetName(){
P1.SetName("P1");
R.SetName("R");
E.SetName("E");
}
void Run(){
corrupt_reg(randomNumber<unsigned long long,unsigned long long>());
timer.Start();R.obtained=FN_PROTOTYPE(rintf)(P1.input);timer.Sample();}};
int main(){
RndType savemode;
savemode = GetRndType();
SetRndType(RT_UP);
UnitTestData *testdata= new UnitTestData[LENGTH_OF(input)/DATA_SIZE + 1];
int k=0;
for(unsigned int i=0; i< LENGTH_OF(input); i+= DATA_SIZE){
P1 P1;
R R;
E E;
for(unsigned int j=0; j< sizeof(float)/sizeof(unsigned int); j++){
if((j+i) < LENGTH_OF(input))
P1.V1[j] = input[i+j].P1;
else
P1.V1[j] = input[i].P1;
}
for(unsigned int j=0; j< sizeof(float)/sizeof(unsigned int); j++){
if((j+i) < LENGTH_OF(input))
R.V1[j] = input[i+j].R;
else
R.V1[j] = input[i].R;
}
for(unsigned int j=0; j< sizeof(ExcFlags)/sizeof(ExcFlags); j++){
if((j+i) < LENGTH_OF(input))
E.V1[j] = input[i+j].E;
else
E.V1[j] = input[i].E;
}
testdata[k].P1.SetData(P1.V2,P1.V2);
testdata[k].R.SetData(R.V2,R.V2);
testdata[k].E.SetData(E.V2,E.V2);
testdata[k].SetName();
testdata[k].Run();
k++;
}
TestReport::Header("rintf_RT_UP.txt");
TestReport::FuncHeader("R.obtained=FN_PROTOTYPE(rintf)(P1.input)");
for(int j=0; j<k ; j++ ){
testdata[j].Report();
}
TestReport::Footer();
delete[] testdata;

SetRndType(savemode);
return 0;
}
