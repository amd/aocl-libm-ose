#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the pow() routine. These test cases are not exhaustive */
static struct __libm_test_conformance_test_data_double_2 libm_test_pow_conformance_data[] = {
	{0x7FF4001000000000,0x7FF4001000000000,0x7ffc001000000000,FE_INVALID}, //pow(snan,snan)
	{0x7FFC001000000000,0x7FF4001000000000,0x7ffc001000000000,FE_INVALID}, //pow(nan,snan)
	{0x3FF0000000000000,0x7FF4001000000000,0x7ffc001000000000,FE_INVALID}, //pow(1.0,snan)
	{0x7FF4001000000000,0x3FF0000000000000,0x7ffc001000000000,FE_INVALID}, //pow(snan,1.0)
	{0x3FE0000000000000,0x7FF4001000000000,0x7ffc001000000000,FE_INVALID}, //pow(0.5,snan)
	{0x7FF4001000000000,0x3FE0000000000000,0x7ffc001000000000,FE_INVALID}, //pow(snan,0.5)
	{0x7ff8000000000000,0x7ff8000000000000,0x7ff8000000000000,0}, //pow(qnan,qnan)
	{0x7ff8000000000000,0x3ff0000000000000,0x7ffc001000000000,0}, //pow(qnan,1)
	{0x3ff0000000000000,0x7ff8000000000000,0x3ff0000000000000,0},//pow(1.qnan)
};

/* Test cases to check for exceptions for the pow() routine. These test cases are not exhaustive */
static struct __libm_test_conformance_test_data_float_2 libm_test_powf_conformance_data[] = {
	{0x7fbfffff, 0x7fa00000, 0x7fbfffff, FE_INVALID}, //nan, snan
	{0x7fa00000, 0x7fa00000, 0x7fa00000, FE_INVALID}, //powf(snan, snan)
	{0x3f800000, 0x7fa00000, 0x3f800000, FE_INVALID},//1.0, nan
	{0x3f800000, 0x7ffe0000, 0x3f800000, FE_INVALID},//1.0, qnan
	{0x3f000000, 0x7fa00000, 0x7fa00000, FE_INVALID}, // 0.5, snan									
	{0x7fa00000, 0x3f000000, 0x7fe00000, FE_INVALID},//snan, 0.5
	{0x7ffe0000, 0x7ffe0000, 0x7ffe0000, 0}, //qnan , qnan
	{0x7ffe0000, 0x3f800000, 0x7ffe0000, 0},	//qnan, 1.0
	{0x3f800000, 0x7ffe0000, 0x3f800000, 0},	//1.0, qnan
};

/*accu data range*/
struct libm_test_input_range x_range[] =
    {
     {9.765625000000e-04, 10.882812500000e-04,},
     {4.882812500000e-04, 12.441406250000e-04,},
     {2.441406250000e-04, 11.220703125000e-04,},
     {1.220703125000e-04, 16.103515625000e-05,},
     {2.384185791016e-07, 2.225073858507e+3,},
     {4.940656458412e+1, 2.225073858507e+3,},
     {2.225073858507e-3, 2.384185791016e-07,},
     {2.384185791016e-07, 4.768371582031e-07,},
     {4.768371582031e-07, 9.536743164062e-07,},
     {9.536743164062e-07, 1.907348632812e-06,},
     {1.907348632812e-06, 3.814697265625e-06,},
     {3.814697265625e-06, 7.629394531250e-06,},
     {7.629394531250e-06, 1.525878906250e-05,},
     {1.525878906250e-05, 3.051757812500e-05,},
     {3.051757812500e-05, 6.103515625000e-05,},
     {6.103515625000e-05, 1.220703125000e-04,},
     {1.220703125000e-04, 2.441406250000e-04,},
     {4.882812500000e-04, 9.765625000000e-04,},
     {9.765625000000e-04, 1.953125000000e-03,},
     {1.953125000000e-03, 3.906250000000e-03,},
     {3.906250000000e-03, 7.812500000000e-03,},
     {7.812500000000e-03, 1.562500000000e-02,},
     {1.562500000000e-02, 3.125000000000e-02,},
     {3.125000000000e-02, 6.250000000000e-02,},
     {6.250000000000e-02, 1.250000000000e-01,},
     {1.250000000000e-01, 2.500000000000e-01,},
     {2.500000000000e-01, 5.000000000000e-01,},
     {5.000000000000e-01, 1.000000000000e+00,},
     {2.000000000000e+00, 4.000000000000e+00,},
     {4.000000000000e+00, 8.000000000000e+00,},
     {8.000000000000e+00, 1.600000000000e+01,},
     {1.600000000000e+01, 3.200000000000e+01,},
     {6.400000000000e+01, 1.280000000000e+02,},
     {0,},
    };

struct libm_test_input_range y_range[] =
    {
     {2.225073858507e-8, 2.384185791016e-07,},
     {2.384185791016e-07, 4.768371582031e-07,},
     {4.768371582031e-07, 9.536743164062e-07,},
     {9.536743164062e-07, 1.907348632812e-06,},
     {1.907348632812e-06, 3.814697265625e-06,},
     {3.814697265625e-06, 7.629394531250e-06,},
     {7.629394531250e-06, 1.525878906250e-05,},
     {1.525878906250e-05, 3.051757812500e-05,},
     {3.051757812500e-05, 6.103515625000e-05,},
     {6.103515625000e-05, 1.220703125000e-04,},
     {1.220703125000e-04, 2.441406250000e-04,},
     {4.882812500000e-04, 9.765625000000e-04,},
     {9.765625000000e-04, 1.953125000000e-03,},
     {1.953125000000e-03, 3.906250000000e-03,},
     {3.906250000000e-03, 7.812500000000e-03,},
     {7.812500000000e-03, 1.562500000000e-02,},
     {1.562500000000e-02, 3.125000000000e-02,},
     {3.125000000000e-02, 6.250000000000e-02,},
     {6.250000000000e-02, 1.250000000000e-01,},
     {1.250000000000e-01, 2.500000000000e-01,},
     {2.500000000000e-01, 5.000000000000e-01,},
     {5.000000000000e-01, 1.000000000000e+00,},
     {2.000000000000e+00, 4.000000000000e+00,},
     {4.000000000000e+00, 8.000000000000e+00,},
     {8.000000000000e+00, 1.600000000000e+01,},
     {1.600000000000e+01, 3.200000000000e+01,},
     {6.400000000000e+01, 1.280000000000e+02,},
     {0,},
     {-1.953125000000e-03, -9.765625000000e-04,},
     {9.765625000000e-04, 10.882812500000e-04,},
     {4.882812500000e-04, 12.441406250000e-04,},
     {2.441406250000e-04, 11.220703125000e-04,},
     {1.220703125000e-04, 16.103515625000e-05,},
     {2.384185791016e-07, 2.225073858507e+30,},
     
};


/*
static struct __pow_internal_data libm_test_pow_special_data[] = {
    {0x0000001200000000,0x3f847ae147ae147b,0x3f489e75a866e138}, // denormal input for log part
{0x4005bf0a8b145769,0xc086800000000000,0x0000000993b4dc95}, // denormal output for exp part

{0xbfe0000000000000,0x4037000000000000,0xbe80000000000000},
{0xbff15a70e829b0c0,0xc0b2030000000000,0x9e2d7afff2b68a04},
{0xbfee79cc64848b49,0x4096100000000000,0x39b7f5858446b314},
{0x3ff028f5c28f5c29,0x3ddb7cdfd9d7bdbb,0x3ff0000000001181},

    {0x4016666666666666,0xc00c000000000000,0x3f63b64914144c07},
    {0x4016666666666666,0x400c000000000000,0x4079f95539971ed9},
    {0x40236ccccccccccd,0xc010000000000000,0x3f1d757737fe28a1},
    {0xbfe0000000000000,0xc033000000000000,0xc120000000000000},
    {0xc0236ccccccccccd,0xc010000000000000,0x3f1d757737fe28a1},
    {0xbff0000000000000,0xbff0000000000000,0xbff0000000000000},
    {0x3ff0000000000000,0x3ff0000000000000,0x3ff0000000000000},
    {0x3ff0000000000000,0x0000000000000000,0x3ff0000000000000},
    {0x0000000000000000,0x3ff0000000000000,0x0000000000000000},
    {0x3fe0000000000000,0x3fe0000000000000,0x3fe6a09e667f3bcd},
    {0x3fe0000000000000,0xbfe0000000000000,0x3ff6a09e667f3bcd},
    {0x3ff0000000000001,0x408f400000000000,0x3ff00000000003e8},
    {0x0000000000000000,0x7ff0000000000001,0x7ff0000000000001},
    {0x8000000000000000,0x7ff0000000000001,0x7ff0000000000001},
    {0xbff0000000000000,0x7ff0000000000001,0x7ff0000000000001},
    {0x7ff0000000000000,0x7ff0000000000001,0x7ff0000000000001},
    {0xfff0000000000000,0x7ff0000000000001,0x7ff0000000000001},

    {0x7ff0000000000001,0x3ff0000000000000,0x7ff0000000000001},
    {0x7ff0000000000001,0xbff0000000000000,0x7ff0000000000001},
    {0x7ff0000000000001,0x7ff0000000000000,0x7ff0000000000001},
    {0x7ff0000000000001,0xfff0000000000000,0x7ff0000000000001},
    {0x7ff0000000000001,0x7ff8000000000000,0x7ff0000000000001},
    {0x7ff0000000000001,0x7ff0000000000001,0x7ff0000000000001},

{0xc010000000000000,0x4012cccccccccccd,0xfff8000000000000},
{0xc014000000000000,0x4012cccccccccccd,0xfff8000000000000},
{0xc012cccccccccccd,0x4012cccccccccccd,0xfff8000000000000},
{0x7ff0000000000001,0x4012cccccccccccd,0x7ff0000000000001},
{0xc010000000000000,0xc012cccccccccccd,0xfff8000000000000},
{0xc014000000000000,0xc012cccccccccccd,0xfff8000000000000},
{0xc012cccccccccccd,0xc012cccccccccccd,0xfff8000000000000},
{0x7ff0000000000001,0xc012cccccccccccd,0x7ff0000000000001},

{0xbfb999999999999a,0x3fb999999999999a,0xfff8000000000000},
{0xc400000000000000,0x3fb999999999999a,0xfff8000000000000},

{0xbfb999999999999a,0xbfb999999999999a,0xfff8000000000000},
{0xc400000000000000,0xbfb999999999999a,0xfff8000000000000},

{0xbff0000000000000,0x404189d495182a99,0xfff8000000000000},
{0xbff0000000000000,0xc04189d495182a99,0xfff8000000000000},

   {0xc024000000000000,0x3ff4000000000000,0xfff8000000000000},  // finite -x ^ finite non integer = NaN
   {0xc024000000000000,0xbff4000000000000,0xfff8000000000000},  // finite -x ^ finite non integer = NaN

    {0x0000000000000000,0x7ff0000000000001,0x7ff8000000000001},
    {0x8000000000000000,0x7ff0000000000001,0x7ff8000000000001},
    {0xbff0000000000000,0x7ff0000000000001,0x7ff8000000000001},
    {0x7ff0000000000000,0x7ff0000000000001,0x7ff8000000000001},
    {0xfff0000000000000,0x7ff0000000000001,0x7ff8000000000001},

    {0x7ff0000000000001,0x3ff0000000000000,0x7ff8000000000001},
    {0x7ff0000000000001,0xbff0000000000000,0x7ff8000000000001},
    {0x7ff0000000000001,0x7ff0000000000000,0x7ff8000000000001},
    {0x7ff0000000000001,0xfff0000000000000,0x7ff8000000000001},
    {0x7ff0000000000001,0x7ff8000000000000,0x7ff8000000000001},
    {0x7ff0000000000001,0x7ff0000000000001,0x7ff8000000000001},

{0xc010000000000000,0x4012cccccccccccd,0x7ff8000000000000},
{0xc014000000000000,0x4012cccccccccccd,0x7ff8000000000000},
{0xc012cccccccccccd,0x4012cccccccccccd,0x7ff8000000000000},
{0x7ff0000000000001,0x4012cccccccccccd,0x7ff8000000000001},

{0xc010000000000000,0xc012cccccccccccd,0x7ff8000000000000},
{0xc014000000000000,0xc012cccccccccccd,0x7ff8000000000000},
{0xc012cccccccccccd,0xc012cccccccccccd,0x7ff8000000000000},  
{0x7ff0000000000001,0x4012cccccccccccd,0x7ff8000000000001},

{0xc010000000000000,0xc012cccccccccccd,0x7ff8000000000000},
{0xc014000000000000,0xc012cccccccccccd,0x7ff8000000000000},
{0xc012cccccccccccd,0xc012cccccccccccd,0x7ff8000000000000},
{0x7ff0000000000001,0xc012cccccccccccd,0x7ff8000000000001},

{0xbfb999999999999a,0x3fb999999999999a,0x7ff8000000000000},
{0xc400000000000000,0x3fb999999999999a,0x7ff8000000000000},

{0xbfb999999999999a,0xbfb999999999999a,0x7ff8000000000000},
{0xc400000000000000,0xbfb999999999999a,0x7ff8000000000000},

{0xbff0000000000000,0x404189d495182a99,0x7ff8000000000000},
{0xbff0000000000000,0xc04189d495182a99,0x7ff8000000000000},

   {0xc024000000000000,0x3ff4000000000000,0x7ff8000000000000},  // finite -x ^ finite non integer = NaN
   {0xc024000000000000,0xbff4000000000000,0x7ff8000000000000},  // finite -x ^ finite non integer = NaN

{0x0000000000000000,0x0000000000000000,0x3ff0000000000000},
{0x0000000000000000,0x8000000000000000,0x3ff0000000000000},
{0x0000000000000000,0x3ff0000000000000,0x0000000000000000},
{0x0000000000000000,0xbff0000000000000,0x7ff0000000000000},
{0x0000000000000000,0x7ff0000000000000,0x0000000000000000},
{0x0000000000000000,0xfff0000000000000,0x7ff0000000000000},
{0x0000000000000000,0x7ff8000000000000,0x7ff8000000000000},
//{0x0000000000000000,0x7ff0000000000001,0x7ff8000000000001},

{0x8000000000000000,0x0000000000000000,0x3ff0000000000000},
{0x8000000000000000,0x8000000000000000,0x3ff0000000000000},
{0x8000000000000000,0x3ff0000000000000,0x8000000000000000},
{0x8000000000000000,0xbff0000000000000,0xfff0000000000000},
{0x8000000000000000,0x7ff0000000000000,0x0000000000000000},
{0x8000000000000000,0xfff0000000000000,0x7ff0000000000000},
{0x8000000000000000,0x7ff8000000000000,0x7ff8000000000000},
//{0x8000000000000000,0x7ff0000000000001,0x7ff8000000000001},

{0x3ff0000000000000,0x0000000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0x8000000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0x3ff0000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0xbff0000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0x7ff0000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0xfff0000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0x7ff8000000000000,0x3ff0000000000000},
{0x3ff0000000000000,0x7ff0000000000001,0x3ff0000000000000},

{0xbff0000000000000,0x0000000000000000,0x3ff0000000000000},
{0xbff0000000000000,0x8000000000000000,0x3ff0000000000000},
{0xbff0000000000000,0x3ff0000000000000,0xbff0000000000000},
{0xbff0000000000000,0xbff0000000000000,0xbff0000000000000},
{0xbff0000000000000,0x7ff0000000000000,0x3ff0000000000000},
{0xbff0000000000000,0xfff0000000000000,0x3ff0000000000000},
{0xbff0000000000000,0x7ff8000000000000,0x7ff8000000000000},
//{0xbff0000000000000,0x7ff0000000000001,0x7ff8000000000001},

{0x7ff0000000000000,0x0000000000000000,0x3ff0000000000000},
{0x7ff0000000000000,0x8000000000000000,0x3ff0000000000000},
{0x7ff0000000000000,0x3ff0000000000000,0x7ff0000000000000},
{0x7ff0000000000000,0xbff0000000000000,0x0000000000000000},
{0x7ff0000000000000,0x7ff0000000000000,0x7ff0000000000000},
{0x7ff0000000000000,0xfff0000000000000,0x0000000000000000},
{0x7ff0000000000000,0x7ff8000000000000,0x7ff8000000000000},
//{0x7ff0000000000000,0x7ff0000000000001,0x7ff8000000000001},

{0xfff0000000000000,0x0000000000000000,0x3ff0000000000000},
{0xfff0000000000000,0x8000000000000000,0x3ff0000000000000},
{0xfff0000000000000,0x3ff0000000000000,0xfff0000000000000},
{0xfff0000000000000,0xbff0000000000000,0x8000000000000000},
{0xfff0000000000000,0x7ff0000000000000,0x7ff0000000000000},
{0xfff0000000000000,0xfff0000000000000,0x0000000000000000},
{0xfff0000000000000,0x7ff8000000000000,0x7ff8000000000000},
//{0xfff0000000000000,0x7ff0000000000001,0x7ff8000000000001},

{0x7ff8000000000000,0x0000000000000000,0x3ff0000000000000},
{0x7ff8000000000000,0x8000000000000000,0x3ff0000000000000},
{0x7ff8000000000000,0x3ff0000000000000,0x7ff8000000000000},
{0x7ff8000000000000,0xbff0000000000000,0x7ff8000000000000},
{0x7ff8000000000000,0x7ff0000000000000,0x7ff8000000000000},
{0x7ff8000000000000,0xfff0000000000000,0x7ff8000000000000},
{0x7ff8000000000000,0x7ff8000000000000,0x7ff8000000000000},
{0x7ff8000000000000,0x7ff0000000000001,0x7ff8000000000000},

{0x7ff0000000000001,0x0000000000000000,0x3ff0000000000000},
{0x7ff0000000000001,0x8000000000000000,0x3ff0000000000000},
//{0x7ff0000000000001,0x3ff0000000000000,0x7ff8000000000001},
//{0x7ff0000000000001,0xbff0000000000000,0x7ff8000000000001},
//{0x7ff0000000000001,0x7ff0000000000000,0x7ff8000000000001},
//{0x7ff0000000000001,0xfff0000000000000,0x7ff8000000000001},
//{0x7ff0000000000001,0x7ff8000000000000,0x7ff8000000000001},
//{0x7ff0000000000001,0x7ff0000000000001,0x7ff8000000000001},

//{0xc010000000000000,0x4012cccccccccccd,0x7ff8000000000000},
//{0xc014000000000000,0x4012cccccccccccd,0x7ff8000000000000},
//{0xc012cccccccccccd,0x4012cccccccccccd,0x7ff8000000000000},

{0x7ff0000000000000,0x4012cccccccccccd,0x7ff0000000000000},
{0xfff0000000000000,0x4012cccccccccccd,0x7ff0000000000000},
{0x7ff8000000000000,0x4012cccccccccccd,0x7ff8000000000000},
//{0x7ff0000000000001,0x4012cccccccccccd,0x7ff8000000000001},

//{0xc010000000000000,0xc012cccccccccccd,0x7ff8000000000000},
//{0xc014000000000000,0xc012cccccccccccd,0x7ff8000000000000},
//{0xc012cccccccccccd,0xc012cccccccccccd,0x7ff8000000000000},

{0x7ff0000000000000,0xc012cccccccccccd,0x0000000000000000},
{0xfff0000000000000,0xc012cccccccccccd,0x0000000000000000},
{0x7ff8000000000000,0xc012cccccccccccd,0x7ff8000000000000},
//{0x7ff0000000000001,0xc012cccccccccccd,0x7ff8000000000001},

{0x7ff0000000000000,0x3fb999999999999a,0x7ff0000000000000},
{0xfff0000000000000,0x3fb999999999999a,0x7ff0000000000000},
{0x7ff0000000000000,0xbfb999999999999a,0x0000000000000000},
{0xfff0000000000000,0xbfb999999999999a,0x0000000000000000},
{0x3fb999999999999a,0x7ff0000000000000,0x0000000000000000},
{0xbfb999999999999a,0x7ff0000000000000,0x0000000000000000},
{0x3fb999999999999a,0xfff0000000000000,0x7ff0000000000000},
{0xbfb999999999999a,0xfff0000000000000,0x7ff0000000000000},
//{0xbfb999999999999a,0x3fb999999999999a,0x7ff8000000000000},
//{0xc400000000000000,0x3fb999999999999a,0x7ff8000000000000},

//{0xbfb999999999999a,0xbfb999999999999a,0x7ff8000000000000},
//{0xc400000000000000,0xbfb999999999999a,0x7ff8000000000000},

{0x3fb999999999999a,0x4400000000000000,0x0000000000000000},
{0xbfb999999999999a,0x4400000000000000,0x0000000000000000},
{0x4400000000000000,0x4400000000000000,0x7ff0000000000000},
{0xc400000000000000,0x4400000000000000,0x7ff0000000000000},

{0x3fb999999999999a,0xc400000000000000,0x7ff0000000000000},
{0xbfb999999999999a,0xc400000000000000,0x7ff0000000000000},
{0x4400000000000000,0xc400000000000000,0x0000000000000000},
{0xc400000000000000,0xc400000000000000,0x0000000000000000},

{0x8000000000000000,0x8000000000000000,0x3ff0000000000000},
{0x8000000000000000,0xbff0000000000000,0xfff0000000000000},
{0x8000000000000000,0x404189d495182a99,0x0000000000000000},
{0x8000000000000000,0xc04189d495182a99,0x7ff0000000000000},
{0x8000000000000000,0x4400000000000000,0x0000000000000000},
{0x8000000000000000,0xc400000000000000,0x7ff0000000000000},
{0x8000000000000000,0x4010000000000000,0x0000000000000000},
{0x8000000000000000,0xc010000000000000,0x7ff0000000000000},
{0x8000000000000000,0x4014000000000000,0x8000000000000000},
{0x8000000000000000,0xc014000000000000,0xfff0000000000000},

{0xbff0000000000000,0x8000000000000000,0x3ff0000000000000},
{0xbff0000000000000,0xbff0000000000000,0xbff0000000000000},
//{0xbff0000000000000,0x404189d495182a99,0x7ff8000000000000},
//{0xbff0000000000000,0xc04189d495182a99,0x7ff8000000000000},
{0xbff0000000000000,0x4400000000000000,0x3ff0000000000000},
{0xbff0000000000000,0xc400000000000000,0x3ff0000000000000},
{0xbff0000000000000,0x4010000000000000,0x3ff0000000000000},
{0xbff0000000000000,0xc010000000000000,0x3ff0000000000000},
{0xbff0000000000000,0x4014000000000000,0xbff0000000000000},
{0xbff0000000000000,0xc014000000000000,0xbff0000000000000},
{0x0710000000000000,0xfd26f340a5ad4cbf,0x7ff0000000000000},

//-----------------------------

   {0x4080000000000000,0x4080000000000000,0x7ff0000000000000},  //error


   {0x4020f00000000000,0x3ff0000000000000,0x4020f00000000000},  // y == 1

   {0x0000000000000000,0x4400000000000000,0x0000000000000000},  // y is +large, x == 0
   {0x3fe0000000000000,0x7ff0000000000000,0x0000000000000000},  // y ==+inf ,   x < 1
   {0x3fe0000000000000,0x4400000000000000,0x0000000000000000},  // y is +large, x < 1
   {0xbff0000000000000,0x7ff0000000000000,0x3ff0000000000000},  // y ==+inf, x == -1
   {0xbff0000000000000,0x4400000000000000,0x3ff0000000000000},  // y is +large, x == -1
   {0x4008000000000000,0x4400000000000000,0x7ff0000000000000},  // y is +large, |x| > 1
   {0x4008000000000000,0x7ff0000000000000,0x7ff0000000000000},  // y ==+inf, |x| > 1
   {0xc008000000000000,0x4400000000000000,0x7ff0000000000000},  // y is +large, |x| > 1
   {0x4020f00000000000,0x7ff0000000000000,0x7ff0000000000000},  // y ==+inf
   {0x4020f00000000000,0x4400000000000000,0x7ff0000000000000},  // y is large
   {0x0000000000000000,0xc400000000000000,0x7ff0000000000000},  // y is -large, |x| ==0
   {0x8000000000000000,0xfff0000000000000,0x7ff0000000000000},  // y ==-inf, |x| == 0
   {0x3fe0000000000000,0xc400000000000000,0x7ff0000000000000},  // y is -large, x < 1
   {0x3fe0000000000000,0xfff0000000000000,0x7ff0000000000000},  // y ==-inf, x < 1
   {0x3ff0000000000000,0xc400000000000000,0x3ff0000000000000},  // y is -large, x == 1
   {0x4000000000000000,0xc400000000000000,0x0000000000000000},  // y is -large, x > 1

   {0x4024000000000000,0xc073800000000000,0x0000002f201d49fb},  //denormal
   {0xc024000000000000,0xc073900000000000,0x80000004b6695433},  //negative denormal
   {0x4024000000000000,0xc080000000000000,0x0000000000000000},  //complete underflow
   {0x4020000000000000,0x40e0000000000000,0x7ff0000000000000},  //overflow
   {0x4024000000000000,0x3a00000000000000,0x3ff0000000000000},  //y is very small
   {0xc024000000000000,0xc080080000000000,0x8000000000000000},  //complete underflow
   {0xc080000000000000,0x4060200000000000,0xfff0000000000000},  //overflow

   {0x0000000000000000,0xc008000000000000,0x7ff0000000000000},  //+0 ^ -odd integer       = +inf
   {0x8000000000000000,0xc008000000000000,0xfff0000000000000},  //-0 ^ -odd integer       = -inf

   {0x0000000000000000,0xc010000000000000,0x7ff0000000000000},  //+0 ^ -not odd integer   = +inf
   {0x8000000000000000,0xc001000000000000,0x7ff0000000000000},  //-0 ^ -not odd integer   = +inf
   {0xbFF0000000000000,0x7ff0000000000000,0x3FF0000000000000},  //-1 ^ +Inf               = +1
   {0xbFF0000000000000,0xfff0000000000000,0x3FF0000000000000},  //-1 ^ -Inf               = +1

   {0x3FF0000000000000,0x7ff8000000000000,0x3FF0000000000000},  //+1 ^ NaN   +1^y = +1 for ALL y, even NaN
   {0x3FF0000000000000,0xfff8000000000000,0x3FF0000000000000},  //+1 ^ NaN   +1^y = +1 for ALL y, even NaN
   {0x3FF0000000000000,0x7ff0000000000000,0x3FF0000000000000},  //+1 ^ +Inf               = +1
   {0x3FF0000000000000,0xfff0000000000000,0x3FF0000000000000},  //+1 ^ -Inf               = +1

   {0x4000000000000000,0x0000000000000000,0x3FF0000000000000},  //any x ^ +0              = +1
   {0x4000000000000000,0x8000000000000000,0x3FF0000000000000},  //any x ^ -0              = +1
   {0x7ff0000000000000,0x0000000000000000,0x3FF0000000000000},  //any x ^ +0              = +1
   {0xfff0000000000000,0x8000000000000000,0x3FF0000000000000},  //any x ^ -0              = +1
   {0x7ff8000000000000,0x0000000000000000,0x3FF0000000000000},  //any x ^ +0              = +1  even NaN
   {0xfff8000000000000,0x8000000000000000,0x3FF0000000000000},  //any x ^ -0              = +1  even NaN

   //{0xc024000000000000,0x3ff4000000000000,0x7ff8000000000000},  // finite -x ^ finite non integer = NaN
   //{0xc024000000000000,0xbff4000000000000,0x7ff8000000000000},  // finite -x ^ finite non integer = NaN

   {0x3fe8000000000000,0xfff0000000000000,0x7ff0000000000000},  // |x|<1 ^ -inf       = +inf
   {0xbfe8000000000000,0xfff0000000000000,0x7ff0000000000000},  // |x|<1 ^ -inf       = +inf

   {0x4024000000000000,0xfff0000000000000,0x0000000000000000},  // |x|>1 ^ -inf       = +0
   {0xc024c00000000000,0xfff0000000000000,0x0000000000000000},  // |x|>1 ^ -inf       = +0

   {0x3fe8000000000000,0x7ff0000000000000,0x0000000000000000},  // |x|<1 ^ +inf       = +0
   {0xbfe8000000000000,0x7ff0000000000000,0x0000000000000000},  // |x|<1 ^ +inf       = +0

   {0x4024000000000000,0x7ff0000000000000,0x7ff0000000000000},  // |x|>1 ^ +inf       = +inf
   {0xc024c00000000000,0x7ff0000000000000,0x7ff0000000000000},  // |x|>1 ^ +inf       = +inf

   {0xfff0000000000000,0xc008000000000000,0x8000000000000000},  // -inf ^ -odd integer = -0

   {0xfff0000000000000,0xc010000000000000,0x0000000000000000},  // -inf ^ - not odd integer = +0
   {0xfff0000000000000,0xc001000000000000,0x0000000000000000},  // -inf ^ - not odd integer = +0

   {0xfff0000000000000,0x4008000000000000,0xfff0000000000000},  // -inf ^ +odd integer = -inf

   {0xfff0000000000000,0x4010000000000000,0x7ff0000000000000},  // -inf ^ +not odd integer = +inf
   {0xfff0000000000000,0x401e000000000000,0x7ff0000000000000},  // -inf ^ +not odd integer = +inf
   {0x7ff0000000000000,0xc008000000000000,0x0000000000000000},  // +inf ^ y<0 = +0
   {0x7ff0000000000000,0xc010000000000000,0x0000000000000000},  // +inf ^ y<0 = +0
   {0x7ff0000000000000,0xc001000000000000,0x0000000000000000},  // +inf ^ y<0 = +0

   {0x7ff0000000000000,0x4008000000000000,0x7ff0000000000000},  // +inf ^ y<0 = +inf
   {0x7ff0000000000000,0x4010000000000000,0x7ff0000000000000},  // +inf ^ y<0 = +inf
   {0x7ff0000000000000,0x4001000000000000,0x7ff0000000000000},  // +inf ^ y<0 = +inf

   {0xc080000000000000,0x4060200000000000,0xfff0000000000000},  //overflow
   {0x4024000000000000,0xc073800000000000,0x0000002f201d49fb},  //denormal
   {0x3FE62E42FEFA39EF,0x3FE62E42FEFA39EF,0x3FE8D22A83215142},  //1/e,1/e
   {0x3F7357915F4A462D,0x403F96C837AE808A,0x30AE8C4E628AE98B},  //0.0047221830942745626,31.5889925766974
   {0xc008000000000000,0x4040000000000000,0x431A553F8878FA04},  //-3^32
      
};

*/

