#define _txt_
#define _FAIL_ONLY_
#define __NUM_FAILED_TEST__ 10
#define __1BDIFF__
#define PFT_SIZE 262144
#include <math.h>
#include "Constants.h"
#include "DataTypes.h"
struct data {
    unsigned int src;
    unsigned int sinr;
    unsigned int cosr;
    ExcFlags E;
};
union src {
    unsigned int V1[sizeof(float) / sizeof(unsigned int)];
    float V3[sizeof(float) / sizeof(float)];
    float V2;
};
union sinr {
    unsigned int V1[sizeof(float) / sizeof(unsigned int)];
    float V3[sizeof(float) / sizeof(float)];
    float V2;
};
union cosr {
    unsigned int V1[sizeof(float) / sizeof(unsigned int)];
    float V3[sizeof(float) / sizeof(float)];
    float V2;
};
union E {
    ExcFlags V1[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V3[sizeof(ExcFlags) / sizeof(ExcFlags)];
    ExcFlags V2;
};
#include "unit/vrsa_sincosf.h"
#include "FunHed.h"
#define DATA_SIZE (MAX(MAX(MAX(sizeof(float)/sizeof(unsigned int),sizeof(float)/sizeof(unsigned int)),sizeof(float)/sizeof(unsigned int)),sizeof(ExcFlags)/sizeof(ExcFlags)))
class UnitTestData {
  public: Timer timer;
    ParameterObjBuffer<float, float *> src;
    ParameterObjBuffer<float, float *> sinr;
    ParameterObjBuffer<float, float *> cosr;
    ParameterObjBuffer<ExcFlags, ExcFlags *> E;
    ParameterObj<int> len;
    void
    Report() {
        TestReport::TestHeader();
        src.Report();
        sinr.Report();
        cosr.Report();
        len.Report();
        TestReport::TestFooter(timer.GetStats().Time());
    }
    void
    SetName() {
        src.SetName("src");
        sinr.SetName("sinr");
        cosr.SetName("cosr");
        E.SetName("E");
        len.SetName("len");
    }
    void
    Run() {
        corrupt_reg(randomNumber<unsigned long long, unsigned long long>());
        timer.Start();
        FN_PROTOTYPE(vrsa_sincosf)(len.input, src.obtained, sinr.obtained,
                cosr.obtained); timer.Sample();
    }
};
void
RangeTest() {
    UnitTestData testdata;
    double min = 0.1, max = 69999; int ntests = 5000;
    double inc = (max - min) / ntests;
    testdata.len.SetData(ntests, ntests);
    testdata.src.AllocateMem(ntests);
    testdata.sinr.AllocateMem(ntests);
    testdata.cosr.AllocateMem(ntests);

    for (int i = 0; i < ntests; i++) {
        src src;
        sinr sinr;
        cosr cosr;
        src.V3[0] = InputVal<float, float>(min);
        sinr.V3[0] = InputVal<float, float>(min);
        cosr.V3[0] = InputVal<float, float>(min);
        amd_ref_sincosf(src.V3[0], &sinr.V3[0], &cosr.V3[0]);
        testdata.src.SetDatatoBuffer(src.V2, src.V2);
        testdata.sinr.SetDatatoBuffer(sinr.V2, sinr.V2);
        testdata.cosr.SetDatatoBuffer(cosr.V2, cosr.V2);
        min = min + inc;
    }

    testdata.SetName();
    testdata.Run();
    double ulp_e = 0.0, rel_diff = 0.0;
    TestReport::Header("vrsa_sincosf_rand.txt", 0);
    TestReport::FuncHeader("FN_PROTOTYPE(vrsa_sincosf)(len.input,src.obtained,sinr.obtained,cosr.obtained)    amd_ref_sincosf(src.V3[0],&sinr.V3[0],&cosr.V3[0])");
    testdata.Report();
    TestReport::Footer(ulp_e, rel_diff);
}
int
main() {
    RangeTest();
    return 0;
}
