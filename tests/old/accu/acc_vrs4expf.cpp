#include "../inc/accu/DataTypes.h"
#include "../inc/accu/Buffer.h"
#include "../inc/accu/Timer.h"
#include "../inc/accu/Constants.h"

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
     
extern "C" void vrs4_expf_wrapper(void *pDst, void *pSrc);

void acc_vrs4expf()
{
    const InOut32u *pTest = ACC_TEST_32_01;

    for(size_t k=0; k<LENGTH_OF(ACC_TEST_32_01); k++)
    {
        Buffer<Hold32> buf_x(4);
        Buffer<Hold32> buf_y(4);

        Hold32 *ptr_x = buf_x.Data();
        Hold32 *ptr_y = buf_y.Data();

        for(size_t j=0; j<4; j++)
        {
            ptr_x[j].u32 = pTest[k].in;
        }

        vrs4_expf_wrapper(ptr_y, ptr_x);

        bool fail = false;
        for(size_t j=0; j<4; j++)
        {
            U32 ref = pTest[k].out;
            ref = ref & 0xfffffffe;
            ptr_y[j].u32 = (ptr_y[j].u32) & 0xfffffffe;
            if(ptr_y[j].u32 != ref) fail = true;
        }

        if(fail)
        {
            std::cout << "Failure: " << std::dec << k << " , Expected: " << std::hex <<
                pTest[k].out << ", Actual: " << ptr_y[0].u32 << std::endl;
        }
        else
        {
            //std::cout << "pass" << std::endl;
        }

    }
}

int main()
{
    acc_vrs4expf();
    return 0;
}

