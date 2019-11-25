#define _txt_
#define _FAIL_ONLY_
#define __1BDIFF__
#include <math.h>
#include "Constants.h"
#include "DataTypes.h"
struct data {
unsigned int P1;
unsigned int P2;
unsigned int R;
ExcFlags E;
};
union P1 {
unsigned int V1[sizeof(__m128)/sizeof(unsigned int)];
float V3[sizeof(__m128)/sizeof(float)];
__m128 V2;
};
union P2 {
unsigned int V1[sizeof(float)/sizeof(unsigned int)];
float V3[sizeof(float)/sizeof(float)];
float V2;
};
union R {
unsigned int V1[sizeof(__m128)/sizeof(unsigned int)];
float V3[sizeof(__m128)/sizeof(float)];
__m128 V2;
};
union E {
ExcFlags V1[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V3[sizeof(ExcFlags)/sizeof(ExcFlags)];
ExcFlags V2;
};
//#include "../unit/vrs4_powxf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(__m128)/sizeof(unsigned int),sizeof(float)/sizeof(unsigned int)),sizeof(__m128)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData{public:Timer timer;
ParameterObj<__m128> P1;
ParameterObj<float> P2;
ParameterObj<__m128> R;
ParameterObj<ExcFlags> E;
void Report(){TestReport::TestHeader();
P1.Report();
P2.Report();
R.Report();
TestReport::TestFooter(timer.GetStats().Time());}
void SetName(){
P1.SetName("P1");
P2.SetName("P2");
R.SetName("R");
E.SetName("E");
}
void Run(){timer.Start();R.obtained=FN_PROTOTYPE(vrs4_powxf)(P1.input,P2.input);timer.Sample();}};
void RandomTest(int size,int seed){
UnitTestData *testdata= new UnitTestData[size/DATA_SIZE + 1];
int k =0;
for(int i=0; i< size; i+= DATA_SIZE){
P1 P1;
P2 P2;
R R;
E E;
P1.V2 = randomNumber<__m128,float>();
P2.V2 = randomNumber<float,float>();
R.V2 = randomNumber<__m128,float>();
E.V2 = randomNumber<ExcFlags,ExcFlags>();
for(unsigned int j=0; j< sizeof(__m128)/sizeof(float); j++){
R.V3[j]=amd_ref_powf(P1.V3[j],P2.V3[0]);
}
testdata[k].P1.SetData(P1.V2,P1.V2);
testdata[k].P2.SetData(P2.V2,P2.V2);
testdata[k].R.SetData(R.V2,R.V2);
testdata[k].E.SetData(E.V2,E.V2);
testdata[k].SetName();
testdata[k].Run();

k++;
}
TestReport::Header("vrs4_powxf_rand.txt",seed);
TestReport::FuncHeader("R.obtained=FN_PROTOTYPE(vrs4_powxf)(P1.input,P2.input)    R.V3[j]=amd_ref_powf(P1.V3[j],P2.V3[j])");

for(int j=0; j<k ; j++ ){
testdata[j].Report();
}
TestReport::Footer();
delete[] testdata;}
int main(){
RandomTest(2000,SetSeed());
return 0;
}
