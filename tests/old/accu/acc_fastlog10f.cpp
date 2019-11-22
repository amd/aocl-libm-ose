#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#ifdef __INTEL_COMPILER
#define S_SP_LOG10 log10f
#else
#define S_SP_LOG10 log10f
//#define S_SP_LOG10 fastlog10f
#endif

struct InOut32u
{
    U32 in;
    U32 out;
};

const static InOut32u ACC_TEST_32_01[] = {

    {0x0005fde6,0xc21d09a9}, // denormal
    {0x00000001,0xc23369f4}, // smallest denormal
    {0x007fffff,0xc217b818}, // largest denormal

    {0x00800000,0xc217b818}, // smallest normal
    {0x7f7fffff,0x421a209b}, // largest normal

    {0x3f880000,0x3cd7afc9}, // 1.0625
    {0x3f87ffff,0x3cd7afaf}, // 1.06249988
    {0x3f700000,0xbce59c7e}, // 0.9375
    {0x3f700001,0xbce59c70}, // 0.93750006
    
    {0x3FC90FDB,0x3e48d3a7},  //pi/2
    {0x00000000,0xfF800000},  //0
    {0x80000000,0xfF800000},  //-0
    {0x3c000000,0xc006dc87},  //0.0078125
    {0x3f012345,0xbe9828da},  //0.504444
    {0x3f800000,0x00000000},  //1
    {0x40000000,0x3e9a209b},  //2
    {0x40490FDB,0x3efe8a6e},  //pi
    {0x40C90FDB,0x3f4c5584},  //2pi
    {0x447A0000,0x40400000},  //1000

#ifdef WIN64
    {0xbf800000,0xFFc00000},  //-1
    {0xfF800000,0xFFc00000},  //-inf
    {0xc2AF0000,0xFFc00000},  //-87.5
    {0xc2e00000,0xFFc00000},  //-112
    {0xc3000000,0xFFc00000},  //-128
    {0xc2800000,0xFFc00000},  //-64
#else
    {0xbf800000,0x7Fc00000},  //-1
    {0xfF800000,0x7Fc00000},  //-inf
    {0xc2AF0000,0x7Fc00000},  //-87.5
    {0xc2e00000,0x7Fc00000},  //-112
    {0xc3000000,0x7Fc00000},  //-128
    {0xc2800000,0x7Fc00000},  //-64
#endif

    {0x3f7ae148,0xbc0fc078},  //.98
    {0x3f828f5c,0x3c0ce7b3},  //1.02

    {0x7F800000,0x7F800000},  //inf
    {0x7Fc00000,0x7Fc00000},  //qnan
    {0x7Fa00000,0x7Fe00000},  //snan
    {0xfFc00000,0xfFc00000},  //qnan
    {0xfFa00000,0xfFe00000},  //snan

    {0x42800000,0x3fe730e8},  //64
    {0x42AF0000,0x3ff893b8},  //87.5
    {0x42b00000,0x3ff8e4cf},  //88
    {0x42c00000,0x3ffdbb10},  //96

    {0x411fffff,0x3f7fffff},  //10--
    {0x41200000,0x3f800000},  //10
    {0x41200001,0x3f800000},  //10++

    };
    
extern "C" float S_SP_LOG10(float);
 
void acc_fastlog10f()
{
    const InOut32u *pTest = ACC_TEST_32_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_32_01); k++)
    {
        Hold32 v_x;
        Hold32 v_y;

        v_x.u32 = pTest[k].in;

        v_y.f32 = S_SP_LOG10(v_x.f32);

        bool fail = false;
        if(v_y.u32 != pTest[k].out) fail = true;

        if(fail)
        {
            std::cout << "Failure: " << std::dec << k << " , Expected: " << std::hex << 
                pTest[k].out << ", Actual: " << v_y.u32 << std::endl;
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
    acc_fastlog10f();
    return 0;
}

