#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#include <math.h>
#include "Constants.h"
#include "DataTypes.h"
struct data {
    unsigned long long src;
    unsigned long long dst;
    ExcFlags E;
};
union src {
    unsigned long long V1[sizeof(double) / sizeof(unsigned long long)];
    double V3[sizeof(double) / sizeof(double)];
    double V2;
};
union dst {
    unsigned long long V1[sizeof(double) / sizeof(unsigned long long)];
    double V3[sizeof(double) / sizeof(double)];
    double V2;
};
union E {
    ExcFlags V1[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V3[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V2;
};
#include "../unit/vrda_exp10.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(sizeof(double)/sizeof(unsigned long long),sizeof(double)/sizeof(unsigned long long)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData {
  public: Timer timer;
    ParameterObjBuffer<double, double *> src;
    ParameterObjBuffer<double, double *> dst;
    ParameterObjBuffer<ExcFlags, ExcFlags *> E;
    ParameterObj<int> len;
    void
    Report() {
        TestReport::TestHeader();
        src.Report();
        dst.Report();
        len.Report();
        TestReport::TestFooter(timer.GetStats().Time());
    }
    void
    SetName() {
        src.SetName("src");
        dst.SetName("dst");
        E.SetName("E");
        len.SetName("len");
    }
    void
    Run() {
        corrupt_reg(randomNumber<unsigned long long, unsigned long long>());
        timer.Start(); FN_PROTOTYPE(vrda_exp10)(len.input, src.obtained, dst.obtained);
        timer.Sample();
    }
};
void
RandomTest(int size, int seed) {
    UnitTestData testdata;
    testdata.len.SetData(size, size);
    testdata.src.AllocateMem(size);
    testdata.dst.AllocateMem(size);

    for (int i = 0; i < size; i++) {
        src src;
        dst dst;
        src.V3[0] = randomNumber<double, double>();
        dst.V3[0] = randomNumber<double, double>();
        dst.V3[0] = amd_ref_exp10(src.V3[0]);
        testdata.src.SetDatatoBuffer(src.V2, src.V2);
        testdata.dst.SetDatatoBuffer(src.V2, dst.V2);
    }

    testdata.SetName();
    testdata.Run();
    double ulp_e = 0.0, rel_diff = 0.0;
    TestReport::Header("vrda_exp10_rand.txt", seed);
    TestReport::FuncHeader("FN_PROTOTYPE(vrda_exp10)(len.input,src.obtained,dst.obtained)    dst.V3[0] = amd_ref_exp10(src.V3[0])");
    testdata.Report();
    TestReport::Footer(ulp_e, rel_diff);
}
int
main() {
    RandomTest(99999, SetSeed());
    return 0;
}