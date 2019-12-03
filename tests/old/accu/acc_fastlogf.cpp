#include "../inc/accu/DataTypes.h"
#include "../inc/accu/Buffer.h"
#include "../inc/accu/Timer.h"
#include "../inc/accu/Constants.h"
#include<math.h>
#ifdef __INTEL_COMPILER
#define S_SP_LOG logf
#else
#define S_SP_LOG logf
//#define S_SP_LOG fastlogf
#endif

struct InOut32u
{
    U32 in;
    U32 out;
};

typedef float   F32;
typedef unsigned int U32;

typedef union {
  U32 u32;
  F32 f32;
} floatword;

volatile struct InOut32u ACC_TEST_32_01[] = {

    {0x0005fde6,0xc2b4cbdf}, // denormal
    {0x00000001,0xc2ce8ed0}, // smallest denormal
    {0x007fffff,0xc2aeac50}, // largest denormal

    {0x00800000,0xc2aeac50}, // smallest normal
    {0x7f7fffff,0x42b17218}, // largest normal

    {0x3f880000,0x3d785186}, // 1.0625
    {0x3f87ffff,0x3d785168}, // 1.06249988
    {0x3f700000,0xbd842cc6}, // 0.9375
    {0x3f700001,0xbd842cbd}, // 0.93750006
    
    {0x3FC90FDB,0x3EE735DA},  //pi/2
    {0x00000000,0xfF800000},  //0
    {0x80000000,0xfF800000},  //-0
    {0x3c000000,0xC09B43D5},  //0.0078125
    {0x3f012345,0xBF2F2E21},  //0.504444
    {0x3f800000,0x00000000},  //1
    {0x40000000,0x3F317218},  //2
 //   {0x40490FDB,0x3F928683},  //pi
    {0x40C90FDB,0x3FEB3F8E},  //2pi
    {0x41200000,0x40135D8E},  //10
    {0x447A0000,0x40DD0C55},  //1000

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

    {0x7F800000,0x7F800000},  //inf
    {0x7Fc00000,0x7Fc00000},  //qnan
    {0x7Fa00000,0x7Fe00000},  //snan
    {0xfFc00000,0xfFc00000},  //qnan
    {0xfFa00000,0xfFe00000},  //snan

    {0x42800000,0x40851592},  //64
    {0x42AF0000,0x408F17AA},  //87.5
    {0x42b00000,0x408F4658},  //88
    {0x42c00000,0x40920F24},  //96

    {0x402DF853,0x3F7FFFFE},  //e --
    {0x402DF854,0x3f800000},  //e
    {0x402DF855,0x3f800000},  //e ++
    {0x3F8100AF,0x3BFFAEF8},  //1.0078333940267947 max ulp error

    {0x40400000,0x3F8C9F54},  //3
    {0x3F806D67,0x3B5A70B3},  //1.00334 max ulp error

    };
    
extern "C" float S_SP_LOG(float);
 
void acc_fastlogf()
{
    volatile InOut32u *pTest = ACC_TEST_32_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_32_01); k++)
    {
        Hold32 v_x;
        Hold32 v_y;

        v_x.u32 = pTest[k].in;

        v_y.f32 = S_SP_LOG(v_x.f32);
        floatword output;
        output.u32=pTest[k].out;

        bool fail = false;
        if(v_y.u32 != pTest[k].out && !(isnan(v_y.f32) && isnan(output.f32))) fail = true;

        if(fail)
        {
            std::cout << "Failure: " << "input: " << v_x.f32 << std::dec << k << " , Expected: " 
		<< std::hex << 
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
    acc_fastlogf();
    return 0;
}

