#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#define PFT_SIZE 262144
#include <math.h>
#include "Constants.h"
#include "DataTypes.h"
struct data {
    unsigned int P1;
    unsigned int R;
    ExcFlags E;
};
union P1 {
    unsigned int V1[sizeof(__m128) / sizeof(unsigned int)];
    float V3[sizeof(__m128) / sizeof(float)];
    __m128 V2;
};
union R {
    unsigned int V1[sizeof(__m128) / sizeof(unsigned int)];
    float V3[sizeof(__m128) / sizeof(float)];
    __m128 V2;
};
union E {
    ExcFlags V1[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V3[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V2;
};
#include "unit/vrs4_cbrtf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(sizeof(__m128)/sizeof(unsigned int),sizeof(__m128)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData {
  public: Timer timer;
    ParameterObj<__m128> P1;
    ParameterObj<__m128> R;
    ParameterObj<ExcFlags> E;
    void
    Report() {
        TestReport::TestHeader();
        P1.Report();
        R.Report();
        TestReport::TestFooter(timer.GetStats().Time());
    }
    void
    SetName() {
        P1.SetName("P1");
        R.SetName("R");
        E.SetName("E");
    }
    void
    Run() {
        corrupt_reg(randomNumber<unsigned long long, unsigned long long>());
        timer.Start(); R.obtained = FN_PROTOTYPE(vrs4_cbrtf)(P1.input); timer.Sample();
    }
};
void
RangeTest() {
    double min = 0.1, max = 69999; int ntests = 5000;
    double inc = (max - min) / ntests;
    UnitTestData *testdata = new UnitTestData[ntests / DATA_SIZE + 1];
    int k = 0;

    for (int i = 0; i < ntests; i += DATA_SIZE) {
        P1 P1;
        R R;
        E E;
        P1.V2 = InputVal<__m128, float>(min);
        R.V2 = InputVal<__m128, float>(min);
        E.V2 = InputVal<ExcFlags, ExcFlags>(min);

        for (unsigned int j = 0; j < sizeof(__m128) / sizeof(float); j++) {
            R.V3[j] = amd_ref_cbrtf(P1.V3[j]);
        }

        testdata[k].P1.SetData(P1.V2, P1.V2);
        testdata[k].R.SetData(R.V2, R.V2);
        testdata[k].E.SetData(E.V2, E.V2);
        testdata[k].SetName();
        testdata[k].Run();
        min = min + (inc * DATA_SIZE);
        k++;
    }

    TestReport::Header("vrs4_cbrtf_rand.txt", 0);
    TestReport::FuncHeader("R.obtained=FN_PROTOTYPE(vrs4_cbrtf)(P1.input)    R.V3[j]=amd_ref_cbrtf(P1.V3[j])");

    for (int j = 0; j < k ; j++) {
        testdata[j].Report();
    }

    TestReport::Footer();
    delete[] testdata;
}
int
main() {
    RangeTest();
    return 0;
}
