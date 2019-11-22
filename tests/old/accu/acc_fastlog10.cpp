#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#ifdef __INTEL_COMPILER
#define S_DP_LOG10 log10
#else
#define S_DP_LOG10 log10
//#define S_DP_LOG10 fastlog10
#endif

struct InOut64u
{
    U64 in;
    U64 out;
};

const static InOut64u ACC_TEST_64_01[] = {


    {0x3ff0097d13350953,0x3f5076dafee0f81f},
    {0x0003ec71c47096d5,0xc07344359d7445f7},
    {0x000fffffffffffff,0xc0733a7146f72a42},

    
    {0x3ff1000000000000,0x3f9af5f92b00e610}, // 1.0625
    {0x3ff0ffffffffffff,0x3f9af5f92b00e5f6}, // 1.0624999999999998
    {0x3fee000000000000,0xbf9cb38fccd8bfdb}, // 0.9375
    {0x3fee000000000001,0xbf9cb38fccd8bfcd}, // 0.93750000000000011

    {0x3ff199999999999a,0x3fa5316c0a18add7},
    {0x3ff1082b577d34ec,0x3f9bcb7b1526e4e7},
    {0x3ff2216045b6f5cc,0x3fabcb7b1526e502},
    {0x3c9b4626891a283b,0xc03006243de791f6},  // 9.4625784658987686E-17
    {0x3ea2345689904135,0xc0190ff23899536d},  // 5.4253475234754763E-07
    {0x410ca5f86198d046,0x40157b61801353ed},  // 234687.04765475
    {0x40f422117627d4d2,0x4013aa42b881aa9a},  // 82465.091346579866
    {0x3FF028F5C28F5C29,0x3f71b349f11fb5bf},  //1.01
    {0x3ff0000000000000,0x0000000000000000},  //1.0
    {0x3ff4000000000000,0x3fb8cf183886480d},  //1.25
    {0x4005BF0A8B145769,0x3fdbcb7b1526e50e},  //e
    {0x4024000000000000,0x3ff0000000000000},  //10
    {0x3ff8000000000000,0x3fc68a288b60b7fc},  //1.5

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
    {0x3FF0F5C28F5C28F6,0x3f99e9c7ded630d5},  //1.06
    {0x3FEE147AE147AE14,0xbf9b845f37d9b8a9},  //.94
    {0x3FF028F5C28F5C29,0x3f71b349f11fb5bf},  //1.01
    {0x3FEFAE147AE147AE,0xbf71e0d4874f92f2},  //.99
    {0x3fe0000000000000,0xbfd34413509f79ff},  //.5
    {0x4310005D38118000,0x402e1a633d6b80ed},  //1.126e+015
    {0x40157AE147AE147B,0x3fe75bf308800485},  //5.37
    {0x3FF02015E8B29697,0x3f6bc2b648a84468},  // 1.0078333940267947 max ulp error

    {0x3ff0000000000001,0x3c9bcb7b1526e50d}

    };
    
extern "C" double S_DP_LOG10(double);
 
void acc_fastlog10()
{
    const InOut64u *pTest = ACC_TEST_64_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_64_01); k++)
    {
        Hold64 v_x;
        Hold64 v_y;

        v_x.u64 = pTest[k].in;

        v_y.f64 = S_DP_LOG10(v_x.f64);

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
    acc_fastlog10();
    return 0;
}

