#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#include <math.h>
#include "Constants.h"
#include "DataTypes.h"
struct data {
    unsigned long long int P1;
    unsigned long long int P2;
    unsigned long long int P3;
    ExcFlags E;
};
union P1 {
    unsigned long long int V1[sizeof(double) / sizeof(unsigned long long int)];
    double V3[sizeof(double) / sizeof(double)];
    double V2;
};
union P2 {
    unsigned long long int V1[sizeof(double) / sizeof(unsigned long long int)];
    double V3[sizeof(double) / sizeof(double)];
    double V2;
};
union P3 {
    unsigned long long int V1[sizeof(double) / sizeof(unsigned long long int)];
    double V3[sizeof(double) / sizeof(double)];
    double V2;
};
union E {
    ExcFlags V1[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V3[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V2;
};
#include "../unit/sincos.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(double)/sizeof(unsigned long long int),sizeof(double)/sizeof(unsigned long long int)),sizeof(double)/sizeof(unsigned long long int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData {
  public: Timer timer;
    ParameterObj<double> P1;
    ParameterObj<double> P2;
    ParameterObj<double> P3;
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
        timer.Start(); FN_PROTOTYPE(sincos)(P1.input, &P2.obtained, &P3.obtained);
        timer.Sample();
    }
};
void
RandomTest(int size, int seed) {
    UnitTestData *testdata = new UnitTestData[size / DATA_SIZE + 1];
    int k = 0;

    for (int i = 0; i < size; i += DATA_SIZE) {
        P1 P1;
        P2 P2;
        P3 P3;
        E E;
        P1.V2 = randomNumber<double, double>();
        P2.V2 = randomNumber<double, double>();
        P3.V2 = randomNumber<double, double>();
        E.V2 = randomNumber<ExcFlags, ExcFlags>();

        for (unsigned int j = 0; j < sizeof(double) / sizeof(double); j++) {
            amd_ref_sincos(P1.V3[j], &P2.V3[j], &P3.V3[j]);
        }

        testdata[k].P1.SetData(P1.V2, P1.V2);
        testdata[k].P2.SetData(P2.V2, P2.V2);
        testdata[k].P3.SetData(P3.V2, P3.V2);
        testdata[k].E.SetData(E.V2, E.V2);
        testdata[k].SetName();
        testdata[k].Run();
        k++;
    }

    TestReport::Header("sincos_rand.txt", seed);
    TestReport::FuncHeader("FN_PROTOTYPE(sincos)(P1.input, &P2.obtained, &P3.obtained)    amd_ref_sincos(P1.V3[j],&P2.V3[j],&P3.V3[j])");

    for (int j = 0; j < k ; j++) {
        testdata[j].Report();
    }

    TestReport::Footer();
    delete[] testdata;
}
int
main() {
    RandomTest(99999, SetSeed());
    return 0;
}
