#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

//struct InOut64
//{
//    Hold64 in;
//    Hold64 out;
//};

struct InOut64u
{
    U64 in;
    U64 out;
};

//union IO64
//{
//    InOut64u io64u;
//    InOut64  io64;
//};

const static InOut64u ACC_TEST_64_01[] = {
    {0xfff0000000000000,0x0000000000000000},  //-inf
    {0xfff8000000000000,0xfff8000000000000},  //Qnan
    {0xfff4000000000000,0xfffc000000000000},  //Snan
    {0x7ff0000000000000,0x7ff0000000000000},  //+inf
    {0x7ff8000000000000,0x7ff8000000000000},  //Qnan
    {0x7ff4000000000000,0x7ffc000000000000},  //Snan
    {0x0000000000000000,0x3FF0000000000000},  //0
    {0x8000000000000000,0x3FF0000000000000},  //-0
    {0x3ff0000000000000,0x4005BF0A8B145769},  //1.0
    {0x3ff4000000000000,0x400BEC38EDB0FAF0},  //1.25
    {0x3ff8000000000000,0x4011ED3FE64FC541},  //1.5

    {0xbff0000000000000,0x3FD78B56362CEF38},  //-1.0
    {0xbff4000000000000,0x3FD25618372A584F},  //-1.25
    {0xbff8000000000000,0x3FCC8F87724B5C1D},  //-1.5
    {0x4086280000000000,0x7FDD422D2BE5DC9B},  //709.0
    {0x40862E6666666666,0x7FF0000000000000},  //709.8 overflow
    {0x4086300000000000,0x7FF0000000000000},  //710.0
    {0x41086A0000000000,0x7FF0000000000000},  //200000 overflow

    {0xC0862328F5C28F5C,0x001005D07E8F743F},  //-708.395 normal
    {0xC086240000000000,0x0000000000000000},  //-708.5 denormal
    {0xC087480000000000,0x0000000000000000},  //-745.0  denormal
    {0xC087492000000000,0x0000000000000000},  //-745.140625 underflow
    {0xc1086A0000000000,0x0000000000000000},  //-200000 underflow

    {0x3ff0000000000000,0x4005BF0A8B145769},  //1.0
    {0x3ff4000000000000,0x400BEC38EDB0FAF0},  //1.25
    {0x3ff4000000000000,0x400BEC38EDB0FAF0},  //1.25
    {0x3ff4000000000000,0x400BEC38EDB0FAF0},  //1.25
    {0x3ff0000000000000,0x4005BF0A8B145769},  //1.0
    {0x3ff4000000000000,0x400BEC38EDB0FAF0},  //1.25
    {0x3ff4000000000000,0x400BEC38EDB0FAF0},  //1.25

    {0x417E7789ABCAC768,0x7FF0000000000000},  //large number overflow
    {0x418E77898CCBFA07,0x7FF0000000000000},  //large number overflow
    {0x418E77898CCBFA07,0x7FF0000000000000},  //large number overflow
    {0xC17E7789ABCAC768,0x0000000000000000},  //large number underflow
    {0xc18E77898CCBFA07,0x0000000000000000},  //large number underflow
    {0xFF7FFFFFFFFFFFFF,0x0000000000000000},  //smallest number
    {0x7F7FFFFFFFFFFFFF,0x7FF0000000000000},  //largest number

    };
     
extern "C" void vrda_exp(int, void *pSrc, void *pDst);

void acc_vrdaexp()
{
    const InOut64u *pTest = ACC_TEST_64_01;

    const size_t LEN = 1024 * 1024;
    const int LEN_INT = (int)LEN;
 
    Buffer<Hold64> buf_x(LEN);
    Buffer<Hold64> buf_y(LEN);
    Buffer<Hold64> buf_y_ref(LEN);
 
    Hold64 *ptr_x = buf_x.Data();
    Hold64 *ptr_y = buf_y.Data();
    Hold64 *ptr_y_ref = buf_y_ref.Data();
 
    for(size_t i=0; i<LEN; i++)
    { 
        int r = (rand())%(LENGTH_OF(ACC_TEST_64_01));
        ptr_x[i].u64 = pTest[r].in;
        ptr_y_ref[i].u64 = pTest[r].out;
    }

    vrda_exp(LEN, ptr_x, ptr_y);

    bool fail = false;
    for(size_t i=0; i<LEN; i++)
    {
        if(ptr_y[i].u64 != ptr_y_ref[i].u64)
        {
            std::cout << "Buffer mismatch at i = " << i << std::endl;
            std::cout << "FAILURE" << std::endl;
            break;
        }
    }
}

int main()
{
    acc_vrdaexp();
    return 0;
}

