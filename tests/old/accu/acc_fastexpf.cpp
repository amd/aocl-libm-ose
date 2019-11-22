#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#ifdef __INTEL_COMPILER
#define S_SP_EXP expf
#else
#define S_SP_EXP expf
//#define S_SP_EXP fastexpf
#endif

struct InOut32u
{
    U32 in;
    U32 out;
};

const static InOut32u ACC_TEST_32_01[] = {

#if 0
#endif
    {0x7F800000,0x7F800000},  //inf
    {0xfF800000,0x00000000},  //-inf
    {0x7Fc00000,0x7Fc00000},  //qnan
    {0x7Fa00000,0x7Fe00000},  //snan
    {0xfFc00000,0xfFc00000},  //qnan
    {0xfFa00000,0xfFe00000},  //snan
    {0x3FC90FDB,0x4099EF6F},  //pi/2
    {0x00000000,0x3F800000},  //0
    {0x3c000000,0x3F810101},  //0.0078125
    {0x3f012345,0x3FD3F9F2},  //0.504444
    {0x3f800000,0x402DF854},  //1
    {0x40000000,0x40EC7326},  //2

    {0x40490FDB,0x41B92025},  //pi
    {0x40C90FDB,0x4405DF79},  //2pi
    {0x41200000,0x46AC14EE},  //10
    {0x447A0000,0x7F800000},  //1000

    {0x42800000,0x6DA12CC1},  //64
    {0x42AF0000,0x7E96BAB3},  //87.5
    {0x42b00000,0x7EF882B7},  //88
    {0x42c00000,0x7F800000},  //96
    {0xc2AF0000,0x006CB2BC},  //-87.5
    {0xc2e00000,0x00000000},  //-112
    {0xc3000000,0x00000000},  //-128

    {0xc2Aeac4f,0x00800026},  //-87.3365  smallest normal result
    {0xc2Aeac50,0x007FFFE6},  //-87.3365  largest denormal result
    {0xc2ce0000,0x00000001},  //
    {0x42b17216,0x7F7FFF04},  //88.7228   largest value  --
    {0x42b17217,0x7F7FFF84},  //88.7228   largest value
    {0x42b17218,0x7F800000},  //88.7228   overflow
    {0x50000000,0x7F800000},  //large   overflow
    {0xff7fffff,0x00000000},  //smallest number
    {0x7f7fffff,0x7F800000}   //largest number

    };
    
extern "C" float S_SP_EXP(float);
 
void acc_fastexpf()
{
    const InOut32u *pTest = ACC_TEST_32_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_32_01); k++)
    {
        Hold32 v_x;
        Hold32 v_y;

        v_x.u32 = pTest[k].in;

        v_y.f32 = S_SP_EXP(v_x.f32);

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
    acc_fastexpf();
    return 0;
}

