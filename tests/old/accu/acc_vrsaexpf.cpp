#include "../inc/accu/DataTypes.h"
#include "../inc/accu/Buffer.h"
#include "../inc/accu/Timer.h"
#include "../inc/accu/Constants.h"

#ifdef __INTEL_COMPILER
#define S_SP_EXP vsExp
#else
#define S_SP_EXP vrsa_expf 
#endif

struct InOut32u
{
    U32 in;
    U32 out;
};

const static InOut32u ACC_TEST_32_01[] = {
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
    {0xc2AF0000,0x00000000},  //-87.5
    {0xc2e00000,0x00000000},  //-112
    {0xc3000000,0x00000000},  //-128

    {0xc2Aeac4f,0x00800026},  //-87.3365  smallest normal result
    {0xc2Aeac50,0x007FFFE6},  //-87.3365  largest denormal result
    {0xc2ce0000,0x00000000},  //
    {0x80000001,0x3f800000},  //
    {0x42b17216,0x7F7FFF04},  //88.7228   largest value  --
    {0x42b17217,0x7F7FFF84},  //88.7228   largest value
    {0x42b17218,0x7F800000},  //88.7228   overflow
    {0x50000000,0x7F800000},  //large   overflow
    {0xff7fffff,0x00000000},  //smallest number
    {0x7f7fffff,0x7F800000}   //largest number
    };
     
extern "C" void S_SP_EXP(int, void *pSrc, void *pDst);

void acc_vrsaexpf()
{
    const InOut32u *pTest = ACC_TEST_32_01;

    const size_t LEN = 1024 * 1024;
    const int LEN_INT = (int)LEN;
 
    Buffer<Hold32> buf_x(LEN);
    Buffer<Hold32> buf_y(LEN);
    Buffer<Hold32> buf_y_ref(LEN);
 
    Hold32 *ptr_x = buf_x.Data();
    Hold32 *ptr_y = buf_y.Data();
    Hold32 *ptr_y_ref = buf_y_ref.Data();
 
    for(size_t i=0; i<LEN; i++)
    { 
        int r = (rand())%(LENGTH_OF(ACC_TEST_32_01));
        ptr_x[i].u32 = pTest[r].in;
        ptr_y_ref[i].u32 = pTest[r].out;
    }

    S_SP_EXP(LEN, ptr_x, ptr_y);

    bool fail = false;
    for(size_t i=0; i<LEN; i++)
    {
        U32 ref = ptr_y_ref[i].u32;
        ref = ref & 0xfffffffe;
        ptr_y[i].u32 = (ptr_y[i].u32) & 0xfffffffe;
        if(ptr_y[i].u32 != ref)
        {
            std::cout << "Buffer mismatch at i = " << i << std::endl;
            std::cout << "FAILURE" << std::endl;
            break;
        }
    }
}

int main()
{
    acc_vrsaexpf();
    return 0;
}

