#define NANN 0x7fbfffff
#define NEG_NAN 0xffffffff
#define QNAN 0x7ffe0000
#define ZERO 0x00000000
#define NEG_ZERO 0x80000000
#define ONE 0x3F800000
#define E 0x402df854
#define PI 0x40490fdb
#define INFF 0x7F800000
#define NEG_INF 0xFF800000
#define NEG_ONE 0xBF800000

#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the expf() routine. These test cases are not exhaustive */
static struct __libm_test_conformance_test_data_float libm_test_expf_conformance_data[] = {
	{0x7fbfffff, 0x7fbfffff, FE_INVALID},    //nan
	{0xffffffff, 0xffffffff, 0},    //-nan
	{0x00000000, 0x3F800000, 0},    //exp(0)=1
	{0x80000000, 0x3F800000, 0},    //exp(-0)
	{0x3F800000, 0x402df854, FE_INEXACT},   //exp(1)  = E
	{0x7F800000, 0x7F800000, 0}, //inf
	{0xFF800000, 0x00000000, 0}, //-inf
	{0xBF800000, 0x3ebc5ab2, FE_INEXACT},    //-1
	{0x40490fdb, 0x41b92025, FE_INEXACT},      //pi
	{0xc435f37e, 0x00000000, 48},	//denormal 
	{0x447a0000, 0x7f800000, 40},	//1000
	{0xc42f0000, 0x00000000, 48},		//-700
	{0x44317218, 0x7F800000, 40},	//smallest no for result infinity
};

static struct __libm_test_conformance_test_data_double libm_test_exp_conformance_data[] = {
    {0x0000000000000000, 0x3FF0000000000000, 0},
    {0x0,0x3ff0000000000000,0}, //exp(0)
    {0x7ff0000000000000,0x7ff0000000000000,0}, //exp(infinity)
    {0xfff0000000000000,0x0,0}, //exp(-inf)
    {0x7FF4001000000000,0x7ffc001000000000,FE_INVALID}, //exp(snan)
    {0x40862e42fefa39ef,0x7fefffffffffff2a,FE_INEXACT}, //exp(ln(2)*1024)
    {0x4086300000000000,0x7ff0000000000000,40}, //exp(x>ln(2)*1024)
};

/*
static struct __expf_internal_data test_expf_special_data[] = {
	{0x40000000, 0x40ec7326},	//2
	{0x41200000, 0x46ac14ee},	//10
	{0xc3fa8000, ZERO}, 	//-501
	{0xc4368000, ZERO}, 	//-730
	{0xc431195f, ZERO},	//smallest normal result -1022*ln(2)
	{0xc43a4887, ZERO}, //largest input for result 0
	{0xc47a0000, ZERO},	//-1000
};
*/

/*
static struct __exp_internal_data test_exp_special_data[] = {
    {0xffefffffffffffff, 0x0000000000000000},
    {0xc0862c4379671324, 0x00052288f82fe4ba},
    {0x7ff0000000000000, 0x7ff0000000000000}, //inf
    {0xfff0000000000000, 0x0000000000000000}, //-inf
    {0x7ffc000000000000, 0x7ffc000000000000}, //qnan
    {0x7ff4000000000000, 0x7ffc000000000000}, //snan
    {0x0000000000000000, 0x3ff0000000000000}, //0

    {0x3e45798ee2308c3a, 0x3ff0000002af31dc}, // .00000001
    {0x400921fb54442d18, 0x403724046eb09339}, //pi
    {0xc086be6fb2739468, 0x0000000001000000}, // denormal result
    {0x3ff0000000000000, 0x4005bf0a8b145769}, // 1
    {0x4000000000000000, 0x401d8e64b8d4ddae}, // 2
    {0xbff0000000000000, 0x3fd78b56362cef38}, // -1

    {0x4024000000000000, 0x40d5829dcf950560}, // 10
    {0x408f400000000000, 0x7ff0000000000000}, // 1000
    {0x4084000000000000, 0x79a40a4b9c27178a}, // 640
    {0xc085e00000000000, 0x00d14f2b0fb9307f}, // -700
    {0xc07f51999999999a, 0x12c0be4b336b18b7}, // -501.1

    {0xc086d00000000000, 0x00000000001c7ea3}, // -730
    {0xc086232bdd7abcd2, 0x001000000000007c}, // smallest normal  result, x=-1022*ln(2)
    {0xc086232bdd7abcd3, 0x000ffffffffffe7c}, // largest denormal result
    //{0xc0874385446d71c4, 0x0000000000000001}, // x=-1074*ln(2)
    //{0xc0874910d52d3051, 0x0000000000000001}, // smallest denormal result, x=-1075*ln(2)
    {0xc0874910d52d3052, 0x0000000000000000}, // largest input for result zero
    {0xc08f400000000000, 0x0000000000000000}, // -1000

    {0x40862e42fefa39ef, 0x7fefffffffffff2a}, // largest normal result
    {0x40862e42fefa39f0, 0x7ff0000000000000}, // smallest input for result inf
    {0x4086280000000000, 0x7fdd422d2be5dc9b}, // 709

    {0x7fefffffffffffff, 0x7ff0000000000000}, // largest number
    {0xffefffffffffffff, 0x0000000000000000}, // smallest number

    // all denormal
    {0xc08625aad16d5438, 0x000bb63ae9a2ac50},
    {0xc08627fa8b8965a4, 0x0008c5deb69c6fc8},
    {0xc0862c4379671324, 0x00052288f82fe4ba},
    {0xc087440b864646f5, 0x0000000000000001},

    {0xc08743e609f06b07, 0x0000000000000001},
    {0xc0874409d4de2a93, 0x0000000000000001},
    {0xc08744b894a31d87, 0x0000000000000001},
    {0xc08744ddf48a3b9c, 0x0000000000000001},
    {0xc08745723e498e76, 0x0000000000000001},
    {0xc0874593fa89185f, 0x0000000000000001},
    {0xffefffffffffffff, 0x0000000000000000},
    {0xffefffffffffffff, 0x0000000000000000},
    {0xffefffffffffffff, 0x0000000000000000},
    {0xffefffffffffffff, 0x0000000000000000},
};
*/
