#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#ifdef __INTEL_COMPILER
#define S_DP_LOG log
#else
#define S_DP_LOG log
//#define S_DP_LOG fastlog
#endif

struct InOut64u
{
    U64 in;
    U64 out;
};

const static InOut64u ACC_TEST_64_01[] = {


    {0x3ff0097d13350953,0x3f62f4881b43f2d5},
    {0x0003ec71c47096d5,0xc0862e6a7c4872d2},
    {0x000fffffffffffff,0xc086232bdd7abcd2},

    
    {0x3ff1000000000000,0x3faf0a30c01162a6}, // 1.0625
    {0x3ff0ffffffffffff,0x3faf0a30c0116288}, // 1.0624999999999998
    {0x3fee000000000000,0xbfb08598b59e3a07}, // 0.9375
    {0x3fee000000000001,0xbfb08598b59e39fe}, // 0.93750000000000011

    {0x3ff199999999999a,0x3fb8663f793c46cc},
    {0x3ff1082b577d34ec,0x3fafffffffffffd3},
    {0x3ff2216045b6f5cc,0x3fbffffffffffff2},
    {0x3c9b4626891a283b,0xc04272c3d7f0cfea},  // 9.4625784658987686E-17
    {0x3ea2345689904135,0xc02cdaa189f4f9f9},  // 5.4253475234754763E-07
    {0x410ca5f86198d046,0x4028bb656d0a5205},  // 234687.04765475
    {0x40f422117627d4d2,0x4026a3e81ffaa69e},  // 82465.091346579866
    {0x3FF028F5C28F5C29,0x3F8460D6CCCA367C},  //1.01
    {0x3ff0000000000000,0x0000000000000000},  //1.0
    {0x3ff4000000000000,0x3FCC8FF7C79A9A22},  //1.25
    {0x4005BF0A8B145769,0x3ff0000000000000},  //e
    {0x3ff8000000000000,0x3FD9F323ECBF984C},  //1.5

#ifdef WIN64
    {0xbff0000000000000,0xFFF8000000000000},  //-1.0
    {0xfff0000000000000,0xFFF8000000000000},  //-inf
#else
    {0xbff0000000000000,0x7FF8000000000000},  //-1.0
    {0xfff0000000000000,0x7FF8000000000000},  //-inf
#endif
    {0xfff8000000000000,0xfff8000000000000},  //Qnan
    {0xfff4000000000000,0xfffc000000000000},  //Snan
    {0x7ff0000000000000,0x7ff0000000000000},  //+inf
    {0x7ff8000000000000,0x7ff8000000000000},  //Qnan
    {0x7ff4000000000000,0x7ffc000000000000},  //Snan

    {0x0000000000000000,0xfff0000000000000},  //+0.0
    {0x8000000000000000,0xfff0000000000000},  //-0.0
    {0x3FF0F5C28F5C28F6,0x3FADD56C1D883EE6},  //1.06
    {0x3FEE147AE147AE14,0xBFAFAE2206CABE40},  //.94
    {0x3FF028F5C28F5C29,0x3F8460D6CCCA367C},  //1.01
    {0x3FEFAE147AE147AE,0xBF8495453E6FD4BC},  //.99
    {0x3fe0000000000000,0xBFE62E42FEFA39EF},  //.5
    {0x4310005D38118000,0x4041542740EB8CF2},  //1.126e+015
    {0x4024000000000000,0x40026BB1BBB55516},  //10
    {0x40157AE147AE147B,0x3FFAE4ABCE14D015},  //5.37
    {0x3FF02015E8B29697,0x3F7FF5E7947206C3},  // 1.0078333940267947 max ulp error

    {0x3ff0000000000001,0x3cafffffffffffff}

    };
    
extern "C" double S_DP_LOG(double);
 
void acc_fastlog()
{
    const InOut64u *pTest = ACC_TEST_64_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_64_01); k++)
    {
        Hold64 v_x;
        Hold64 v_y;

        v_x.u64 = pTest[k].in;

        v_y.f64 = S_DP_LOG(v_x.f64);

        bool fail = false;
        if(v_y.u64 != pTest[k].out) fail = true;

        if(fail)
        {
            std::cout << "Failure: " << std::dec << k << " , Expected: " << std::hex << 
                pTest[k].out << ", Actual: " << v_y.u64 << std::endl;
        }
        else
        {
            std::cout << ".";
        }
    }
    std::cout << " Finished!" << std::endl;
}

int main()
{
    acc_fastlog();
    return 0;
}
