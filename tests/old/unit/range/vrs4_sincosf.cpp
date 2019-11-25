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
    unsigned int P2;
    unsigned int P3;
    ExcFlags E;
};
union P1 {
    unsigned int V1[sizeof(__m128) / sizeof(unsigned int)];
    float V3[sizeof(__m128) / sizeof(float)];
    __m128 V2;
};
union P2 {
    unsigned int V1[sizeof(__m128) / sizeof(unsigned int)];
    float V3[sizeof(__m128) / sizeof(float)];
    __m128 V2;
};
union P3 {
    unsigned int V1[sizeof(__m128) / sizeof(unsigned int)];
    float V3[sizeof(__m128) / sizeof(float)];
    __m128 V2;
};
union E {
    ExcFlags V1[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V3[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V2;
};
#include "unit/vrs4_sincosf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(__m128)/sizeof(unsigned int),sizeof(__m128)/sizeof(unsigned int)),sizeof(__m128)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData {
  public: Timer timer;
    ParameterObj<__m128> P1;
    ParameterObj<__m128> P2;
    ParameterObj<__m128> P3;
    ParameterObj<ExcFlags> E;
    void
    Report() {
        TestReport::TestHeader();
        P1.Report();
        P2.Report();
        P3.Report();
        TestReport::TestFooter(timer.GetStats().Time());
    }
    void
    SetName() {
        P1.SetName("P1");
        P2.SetName("P2");
        P3.SetName("P3");
        E.SetName("E");
    }
    void
    Run() {
        corrupt_reg(randomNumber<unsigned long long, unsigned long long>());
        timer.Start(); FN_PROTOTYPE(vrs4_sincosf)(P1.input, &P2.obtained, &P3.obtained);
        timer.Sample();
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
        P2 P2;
        P3 P3;
        E E;
        P1.V2 = InputVal<__m128, float>(min);
        P2.V2 = InputVal<__m128, float>(min);
        P3.V2 = InputVal<__m128, float>(min);
        E.V2 = InputVal<ExcFlags, ExcFlags>(min);

        for (unsigned int j = 0; j < sizeof(__m128) / sizeof(float); j++) {
            amd_ref_sincosf(P1.V3[j], &P2.V3[j], &P3.V3[j]);
        }

        testdata[k].P1.SetData(P1.V2, P1.V2);
        testdata[k].P2.SetData(P2.V2, P2.V2);
        testdata[k].P3.SetData(P3.V2, P3.V2);
        testdata[k].E.SetData(E.V2, E.V2);
        testdata[k].SetName();
        testdata[k].Run();
        min = min + (inc * DATA_SIZE);
        k++;
    }

    TestReport::Header("vrs4_sincosf_rand.txt", 0);
    TestReport::FuncHeader("FN_PROTOTYPE(vrs4_sincosf)(P1.input, &P2.obtained, &P3.obtained)    amd_ref_sincosf(P1.V3[j],&P2.V3[j],&P3.V3[j])");

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
