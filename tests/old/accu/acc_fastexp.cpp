#include "../inc/accu/DataTypes.h"
#include "../inc/accu/Buffer.h"
#include "../inc/accu/Timer.h"
#include "../inc/accu/Constants.h"

#ifdef __INTEL_COMPILER
#define S_DP_EXP exp
#else
#define S_DP_EXP exp
//#define S_DP_EXP fastexp
#endif

struct InOut64u
{
    U64 in;
    U64 out;
};

const static InOut64u ACC_TEST_64_01[] = {

#if 0
#endif
    {0xffefffffffffffff,0x0000000000000000}, 
    {0xc0862c4379671324,0x00052288f82fe4ba},
    {0x7ff0000000000000,0x7ff0000000000000}, //inf
    {0xfff0000000000000,0x0000000000000000}, //-inf
    {0x7ffc000000000000,0x7ffc000000000000}, //qnan
    {0x7ff4000000000000,0x7ffc000000000000}, //snan
    {0x0000000000000000,0x3ff0000000000000}, //0

    {0x3e45798ee2308c3a,0x3ff0000002af31dc}, // .00000001
    {0x400921fb54442d18,0x403724046eb09339}, //pi
    {0xc086be6fb2739468,0x0000000001000000}, // denormal result
    {0x3ff0000000000000,0x4005bf0a8b145769}, // 1
    {0x4000000000000000,0x401d8e64b8d4ddae}, // 2
    {0xbff0000000000000,0x3fd78b56362cef38}, // -1

    {0x4024000000000000,0x40d5829dcf950560}, // 10
    {0x408f400000000000,0x7ff0000000000000}, // 1000
    {0x4084000000000000,0x79a40a4b9c27178a}, // 640
    {0xc085e00000000000,0x00d14f2b0fb9307f}, // -700
    {0xc07f51999999999a,0x12c0be4b336b18b7}, // -501.1
    
    {0xc086d00000000000,0x00000000001c7ea3}, // -730
    {0xc086232bdd7abcd2,0x001000000000007c}, // smallest normal  result, x=-1022*ln(2)
    {0xc086232bdd7abcd3,0x000ffffffffffe7c}, // largest denormal result
//    {0xc0874385446d71c4,0x0000000000000001}, // x=-1074*ln(2)
//    {0xc0874910d52d3051,0x0000000000000001}, // smallest input for smallest denormal result, x=-1075*ln(2)
    {0xc0874910d52d3052,0x0000000000000000}, // largest input for result zero
    {0xc08f400000000000,0x0000000000000000}, // -1000

    {0x40862e42fefa39ef,0x7fefffffffffff2a}, // largest normal result
    {0x40862e42fefa39f0,0x7ff0000000000000}, // smallest input for result inf
    {0x4086280000000000,0x7fdd422d2be5dc9b}, // 709

    {0x7fefffffffffffff,0x7ff0000000000000}, // largest number
    {0xffefffffffffffff,0x0000000000000000}, // smallest number

    // all denormal 
    {0xc08625aad16d5438,0x000bb63ae9a2ac50},
    {0xc08627fa8b8965a4,0x0008c5deb69c6fc8},
    {0xc0862c4379671324,0x00052288f82fe4ba},
    {0xc087440b864646f5,0x0000000000000001},

    {0xc08743e609f06b07,0x0000000000000001},
    {0xc0874409d4de2a93,0x0000000000000001},
    {0xc08744b894a31d87,0x0000000000000001},
    {0xc08744ddf48a3b9c,0x0000000000000001},
    {0xc08745723e498e76,0x0000000000000001},
    {0xc0874593fa89185f,0x0000000000000001},



    };
    
extern "C" double S_DP_EXP(double);
 
void acc_fastexp()
{
    const InOut64u *pTest = ACC_TEST_64_01;

    std::cout << std::endl;
    std::cout << "Running: ";
    for(size_t k=0; k<LENGTH_OF(ACC_TEST_64_01); k++)
    {
        Hold64 v_x;
        Hold64 v_y;

        v_x.u64 = pTest[k].in;

        v_y.f64 = S_DP_EXP(v_x.f64);

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
    acc_fastexp();
    return 0;
}

