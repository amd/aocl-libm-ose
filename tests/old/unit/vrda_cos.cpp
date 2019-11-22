#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#define PFT_SIZE 262144
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
#include "unit/vrda_cos.h"
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
        timer.Start(); FN_PROTOTYPE(vrda_cos)(len.input, src.obtained, dst.obtained);
        timer.Sample();
    }
};
void
RangeTest() {
    UnitTestData testdata;
    double min = 0.1, max = 69999; int ntests = 5000;
    double inc = (max - min) / ntests;
    testdata.len.SetData(ntests, ntests);
    testdata.src.AllocateMem(ntests);
    testdata.dst.AllocateMem(ntests);

    for (int i = 0; i < ntests; i++) {
        src src;
        dst dst;
        src.V3[0] = InputVal<double, double>(min);
        dst.V3[0] = InputVal<double, double>(min);
        dst.V3[0] = amd_ref_cos(src.V3[0]);
        testdata.src.SetDatatoBuffer(src.V2, src.V2);
        testdata.dst.SetDatatoBuffer(src.V2, dst.V2);
        min = min + inc;
    }

    testdata.SetName();
    testdata.Run();
    double ulp_e = 0.0, rel_diff = 0.0;
    TestReport::Header("vrda_cos_rand.txt", 0);
    TestReport::FuncHeader("FN_PROTOTYPE(vrda_cos)(len.input,src.obtained,dst.obtained)    dst.V3[0] = amd_ref_cos(src.V3[0])");
    testdata.Report();
    TestReport::Footer(ulp_e, rel_diff);
}
int
main() {
    RangeTest();
    return 0;
}
