#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#ifdef __INTEL_COMPILER
#define S_DP_LOG2 log2
#else
#define S_DP_LOG2 log2
//#define S_DP_LOG2 fastlog2
#endif

struct InOut64u
{
    U64 in;
    U64 out;
};

const static InOut64u ACC_TEST_64_01[] = {


    {0x3ff0097d13350953,0x3f6b58b92ecc96c8},
    {0x0003ec71c47096d5,0xc090001c7c528f17},
    {0x000fffffffffffff,0xc08ff00000000000},

    
    {0x3ff1000000000000,0x3fb663f6fac91316}, // 1.0625
    {0x3ff0ffffffffffff,0x3fb663f6fac91301}, // 1.0624999999999998
    {0x3fee000000000000,0xbfb7d60496cfbb4c}, // 0.9375
    {0x3fedc28f5c28f5c3,0xbfbacd728959b236}, // 0.93
    {0x3fee000000000001,0xbfb7d60496cfbb40}, // 0.93750000000000011

    {0x3ff199999999999a,0x3fc199b728cb9d0c},
    {0x3ff1082b577d34ec,0x3fb71547652b82de},
    {0x3ff2216045b6f5cc,0x3fc71547652b82f4},
    {0x3c9b4626891a283b,0xc04a9d82796144e9},  // 9.4625784658987686E-17
    {0x3ea2345689904135,0xc034d053f57a5048},  // 5.4253475234754763E-07
    {0x410ca5f86198d046,0x4031d7230ee91cd7},  // 234687.04765475
    {0x40f422117627d4d2,0x403054dcea9048c6},  // 82465.091346579866
    {0x3FF028F5C28F5C29,0x3f8d664ecee35b7f},  //1.01
    {0x3ff0000000000000,0x0000000000000000},  //1.0
    {0x3ff4000000000000,0x3fd49a784bcd1b8b},  //1.25
    {0x4005BF0A8B145769,0x3ff71547652b82fe},  //e
    {0x4024000000000000,0x400a934f0979a371},  //10
    {0x4000000000000000,0x3ff0000000000000},  //2
    {0x3ff8000000000000,0x3fe2b803473f7ad1},  //1.5

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
    {0x3FF0F5C28F5C28F6,0x3fb5853c54075e4c},  //1.06
    {0x3FEE147AE147AE14,0xbfb6da3969ba5bef},  //.94
    {0x3FF028F5C28F5C29,0x3f8d664ecee35b7f},  //1.01
    {0x3FEFAE147AE147AE,0xbf8db1f34d2c386d},  //.99
    {0x3fe0000000000000,0xbff0000000000000},  //.5
    {0x4310005D38118000,0x4049000433d857ac},  //1.126e+015
    {0x40157AE147AE147B,0x4003663d8d41b416},  //5.37

    {0x3ff0000000000001,0x3cb71547652b82fd}

    };
    
extern "C" double S_DP_LOG2(double);
 
void acc_fastlog2()
{
    const InOut64u *pTest = ACC_TEST_64_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_64_01); k++)
    {
        Hold64 v_x;
        Hold64 v_y;

        v_x.u64 = pTest[k].in;

        v_y.f64 = S_DP_LOG2(v_x.f64);

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
    acc_fastlog2();
    return 0;
}

