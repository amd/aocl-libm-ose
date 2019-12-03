#include "../inc/accu/DataTypes.h"
#include "../inc/accu/Buffer.h"
#include "../inc/accu/Timer.h"
#include "../inc/accu/Constants.h"

#ifdef __INTEL_COMPILER
#define S_SP_LOG2 log2f
#else
#define S_SP_LOG2 log2f
//#define S_SP_LOG2 fastlog2f
#endif

struct InOut32u
{
    U32 in;
    U32 out;
};

const static InOut32u ACC_TEST_32_01[] = {

    {0x0005fde6,0xc3026ac1}, // denormal
    {0x00000001,0xc3150000}, // smallest denormal
    {0x007fffff,0xc2fc0000}, // largest denormal

    {0x00800000,0xc2fc0000}, // smallest normal
    {0x7f7fffff,0x43000000}, // largest normal

    {0x3f880000,0x3db31fb8}, // 1.0625
    {0x3f87ffff,0x3db31fa2}, // 1.06249988
    {0x3f700000,0xbdbeb025}, // 0.9375
    {0x3f6e147b,0xbdd66b93}, // 0.93
    {0x3f700001,0xbdbeb018}, // 0.93750006
    
    {0x3FC90FDB,0x3f26c874},  //pi/2
    {0x00000000,0xfF800000},  //0
    {0x80000000,0xfF800000},  //-0
    {0x3c000000,0xc0e00000},  //0.0078125
    {0x3f012345,0xbf7cbb49},  //0.504444
    {0x3f800000,0x00000000},  //1
    {0x40000000,0x3f800000},  //2
    {0x41200000,0x40549a78},  //10
    {0x40490FDB,0x3fd3643a},  //pi
    {0x40C90FDB,0x4029b21d},  //2pi
    {0x447A0000,0x411f73da},  //1000

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

    {0x3f7ae148,0xbceec442},  //.98
    {0x3f828f5c,0x3cea09cc},  //1.02

    {0x7F800000,0x7F800000},  //inf
    {0x7Fc00000,0x7Fc00000},  //qnan
    {0x7Fa00000,0x7Fe00000},  //snan
    {0xfFc00000,0xfFc00000},  //qnan
    {0xfFa00000,0xfFe00000},  //snan

    {0x42800000,0x40c00000},  //64
    {0x42AF0000,0x40ce7052},  //87.5
    {0x42b00000,0x40ceb3aa},  //88
    {0x42c00000,0x40d2b803},  //96

    {0x3fffffff,0x3f7fffff},  //2--
    {0x40000000,0x3f800000},  //2
    {0x40000001,0x3f800001},  //2++

    };
    
extern "C" float S_SP_LOG2(float);
 
void acc_fastlog2f()
{
    const InOut32u *pTest = ACC_TEST_32_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_32_01); k++)
    {
        Hold32 v_x;
        Hold32 v_y;

        v_x.u32 = pTest[k].in;

        v_y.f32 = S_SP_LOG2(v_x.f32);

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
    acc_fastlog2f();
    return 0;
}

