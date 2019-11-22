#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#include <cstring>
#include <complex>
#include <math.h>

#ifdef __INTEL_COMPILER
#define S_DP_CEXP cexp
#else
#define S_DP_CEXP cexp
//#define S_DP_CEXP fastcexp
#endif

#ifdef WIN64
#define ISNAN _isnan
#else
#define ISNAN std::isnan
#endif

typedef std::complex<double> ComplexDouble;

struct CmplxU64
{
    U64 r;
    U64 i;
};

struct CmplxF64
{
    F64 r;
    F64 i;
};

struct InOutCx64u
{
    CmplxU64 in;
    CmplxU64 out;
};

union HoldCmplx64
{
    CmplxU64 u;
    CmplxF64 f;
};

const static InOutCx64u ACC_TEST_CX64_01[] = {
//special tests for verification purpose                                                //    x,    y, exp(x), cos(y), sin(y), real, imag
{ {0x7ff0000000000000, 0x3ff0000000000000}, {0x7ff0000000000000, 0x7ff0000000000000} }, //  inf,    1,    inf,      +,      +,  inf,  inf
{ {0x4090000000000000, 0x3ffb333333333333}, {0xfff0000000000000, 0x7ff0000000000000} },	// 1024,  1.7,    inf,      -,      +, -inf,  inf
{ {0x7ff0000000000000, 0xbfe999999999999a}, {0x7ff0000000000000, 0xfff0000000000000} }, //  inf, -0.8,    inf,      +,      -,  inf, -inf
{ {0x4090000000000000, 0x0000000000000000}, {0x7ff0000000000000, 0xfff8000000000000} }, // 1024,    0,    inf,      1,      0,  inf,  nan
{ {0x4090000000000000, 0x8000000000000000}, {0x7ff0000000000000, 0xfff8000000000000} }, // 1024,   -0,    inf,      1,     -0,  inf,  nan
{ {0x7ff0000000000000, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} }, //  inf,  inf,    inf,    nan,    nan,  nan,  nan
{ {0x4090000000000000, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} }, // 1024,  nan,    inf,    nan,    nan,  nan,  nan
{ {0xc090000000000000, 0x7ff0000000000000}, {0x0000000000000000, 0x0000000000000000} }, //-1024,  inf,      0,    nan,    nan,    0,    0
// cannot get the exact value of Pi
//{ {0x7ff0000000000000, 0x400921fb54442d18}, {0xfff0000000000000, 0xfff8000000000000} }, //  inf,   pi,    inf,     -1,      0, -inf,  nan
//{ {0x4090000000000000, 0x3ff921fb54442d1a}, {0xfff8000000000000, 0x7ff0000000000000} }, // 1024, pi/2,    inf,      0,      1,  nan,  inf
//{ {0x7ff0000000000000, 0xbff921fb54442d1a}, {0xfff8000000000000, 0xfff0000000000000} }, //  inf,-pi/2,    inf,      0,     -1,  nan, -inf

		// Normal test
        { {0x3ff0000000000000, 0x3ff0000000000000}, {0x3ff77fc5377c5a96, 0x40024c80edc62064} },
        { {0x3ff0000000000000, 0xbff0000000000000}, {0x3ff77fc5377c5a96, 0xc0024c80edc62064} },
        { {0x3ff0000000000000, 0x8000000000000000}, {0x4005bf0a8b145769, 0x8000000000000000} },
        { {0x3ff0000000000000, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x3ff0000000000000, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xbff0000000000000, 0x3ff0000000000000}, {0x3fc9712afbbed823, 0x3fd3cfd439855f55} },
        { {0xbff0000000000000, 0xbff0000000000000}, {0x3fc9712afbbed823, 0xbfd3cfd439855f55} },
        { {0x0000000000000000, 0x3ff0000000000000}, {0x3fe14a280fb5068c, 0x3feaed548f090cee} },
        { {0x0000000000000000, 0xbff0000000000000}, {0x3fe14a280fb5068c, 0xbfeaed548f090cee} },
        { {0x0000000000000000, 0x0000000000000000}, {0x3ff0000000000000, 0x0000000000000000} },
        { {0x0000000000000000, 0x8000000000000000}, {0x3ff0000000000000, 0x8000000000000000} },
        { {0x0000000000000000, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x0000000000000000, 0xc08f400000000000}, {0x3fe1ff026793f1bb, 0xbfea75cc150a206b} },
        { {0x0000000000000000, 0x408f400000000000}, {0x3fe1ff026793f1bb, 0x3fea75cc150a206b} },
        { {0x8000000000000000, 0x3ff0000000000000}, {0x3fe14a280fb5068c, 0x3feaed548f090cee} },
        { {0x8000000000000000, 0xbff0000000000000}, {0x3fe14a280fb5068c, 0xbfeaed548f090cee} },
        { {0x8000000000000000, 0x0000000000000000}, {0x3ff0000000000000, 0x0000000000000000} },
        { {0x8000000000000000, 0x8000000000000000}, {0x3ff0000000000000, 0x8000000000000000} },
        { {0x8000000000000000, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x8000000000000000, 0xc08f400000000000}, {0x3fe1ff026793f1bb, 0xbfea75cc150a206b} },
        { {0x8000000000000000, 0x408f400000000000}, {0x3fe1ff026793f1bb, 0x3fea75cc150a206b} },
        { {0x7ff0000000000000, 0x3ff0000000000000}, {0x7ff0000000000000, 0x7ff0000000000000} },
        { {0x7ff0000000000000, 0xbff0000000000000}, {0x7ff0000000000000, 0xfff0000000000000} },
        { {0x7ff0000000000000, 0x0000000000000000}, {0x7ff0000000000000, 0xfff8000000000000} },
        { {0x7ff0000000000000, 0x8000000000000000}, {0x7ff0000000000000, 0xfff8000000000000} },
        { {0x7ff0000000000000, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000000000, 0xc08f400000000000}, {0x7ff0000000000000, 0xfff0000000000000} },
        { {0x7ff0000000000000, 0x408f400000000000}, {0x7ff0000000000000, 0x7ff0000000000000} },
        { {0xfff0000000000000, 0x3ff0000000000000}, {0x0000000000000000, 0x0000000000000000} },
        { {0xfff0000000000000, 0xbff0000000000000}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0x0000000000000000}, {0x0000000000000000, 0x0000000000000000} },
        { {0xfff0000000000000, 0x8000000000000000}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0x7ff0000000000000}, {0x0000000000000000, 0x0000000000000000} },
        { {0xfff0000000000000, 0xfff0000000000000}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0x7ff8000000000000}, {0x0000000000000000, 0x0000000000000000} },
        { {0xfff0000000000000, 0xfff8000000000000}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0x7ff0000000001234}, {0x0000000000000000, 0x0000000000000000} },
        { {0xfff0000000000000, 0xfff0000000001234}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0x7ff8000000001234}, {0x0000000000000000, 0x0000000000000000} },
        { {0xfff0000000000000, 0xfff8000000001234}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0xc08f400000000000}, {0x0000000000000000, 0x8000000000000000} },
        { {0xfff0000000000000, 0x408f400000000000}, {0x0000000000000000, 0x0000000000000000} },
        { {0x7ff8000000000000, 0x3ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0xbff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x0000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x8000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0xc08f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000000000, 0x408f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x3ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0xbff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x0000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x8000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0xc08f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000000000, 0x408f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x3ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0xbff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x0000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x8000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0xc08f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff0000000001234, 0x408f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x3ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0xbff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x0000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x8000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0xc08f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff0000000001234, 0x408f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x3ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0xbff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x0000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x8000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0xc08f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0x7ff8000000001234, 0x408f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x3ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0xbff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x0000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x8000000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x7ff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0xfff0000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x7ff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0xfff8000000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x7ff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0xfff0000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x7ff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0xfff8000000001234}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0xc08f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xfff8000000001234, 0x408f400000000000}, {0x7ff8000000000000, 0x7ff8000000000000} },
        { {0xc08f400000000000, 0x7ff0000000001234}, {0x0000000000000000, 0x0000000000000000} },
        { {0xc08f400000000000, 0xfff8000000001234}, {0x0000000000000000, 0x0000000000000000} },
        { {0xc08f400000000000, 0xc08f400000000000}, {0x0000000000000000, 0x8000000000000000} },
        { {0x408f400000000000, 0xbff0000000000000}, {0x7ff0000000000000, 0xfff0000000000000} },
        { {0x408f400000000000, 0x8000000000000000}, {0x7ff0000000000000, 0xfff8000000000000} },
        { {0x400921fb54442d18, 0x3ff0000000000000}, {0x402901853ade36e1, 0x403378e380be653a} },
        { {0x400921fb54442d18, 0xbff0000000000000}, {0x402901853ade36e1, 0xc03378e380be653a} },
        { {0x400921fb54442d18, 0x439332270327f466}, {0x3fd0b9b43585d33c, 0xc03723a3b8644c3b} },
        { {0xc086be6fb2739468, 0x439332270327f466}, {0x000000000002e41e, 0x8000000000fffbd2} },
        { {0x3ff0000000000000, 0xc086be6fb2739468}, {0x3ff5d4a403718a3d, 0x4002cef29cc1c010} },
        { {0x3ff0000000000000, 0x4024000000000000}, {0xc0023f25507aaa17, 0xbff7a92d051dc131} },
        { {0x4024000000000000, 0xc086232bdd7abcd3}, {0xc086857938bf9c16, 0x40d57fab0be30361} },
        { {0x4024000000000000, 0xbfe0000000000000}, {0x40d2e082b4bddf3d, 0xc0c4a0066dd87a80} },
        { {0x408f400000000000, 0x400921fb54442d18}, {0xfff0000000000000, 0x7ff0000000000000} },
        { {0x408f400000000000, 0xbff921f557442d18}, {0x7ff0000000000000, 0xfff0000000000000} },
        { {0x4084000000000000, 0x4046c6cbc45dc8de}, {0xf5dc9a53ea9307a4, 0x79a40a4b9c27178a} },
        { {0x4084000000000000, 0x439332270327f466}, {0x793cf80cee999113, 0xf9a409f7db105fe4} },
        { {0xc085e00000000000, 0x3e45798ee2308c3a}, {0x00d14f2b0fb9307f, 0x0000002e76d39b36} },
        { {0xc085e00000000000, 0x400921fb54442d18}, {0x80d14f2b0fb9307f, 0x000000000000098c} },
        { {0xc08625aad16d5438, 0xbfe921fb54442d18}, {0x00084811a076524d, 0x80084811a076524d} },
        { {0xc08625aad16d5438, 0x4046c6cbc45dc8de}, {0x8000000000000000, 0x000bb63ae9a2ac50} },
        { {0xbf18000000000000, 0x3feffee4e9dabc6b}, {0x3fe14aae81487b79, 0x3feaec1a0ca11b83} },
        { {0xbf18000000000000, 0xbfe921fb54442d18}, {0x3fe6a016a4601cc6, 0xbfe6a016a4601cc5} },
        { {0x3fe0000000000000, 0xbff0000000000000}, {0x3fec817f93fea216, 0xbff632971433b07e} },
        { {0x3fe0000000000000, 0x4024000000000000}, {0xbff62262e07db383, 0xbfecb3b9ca16f857} },
        { {0x3fe0000000000000, 0xbff921f557442d18}, {0x3ee3befabb01ac78, 0xbffa61298e1c2da1} },
        { {0xbfe0000000000000, 0x3e45798ee2308c3a}, {0x3fe368b2fc6f960a, 0x3e3a0ce0087dc58b} },
        { {0xbfe0000000000000, 0xbfe921fb54442d18}, {0x3fdb72cd3f331399, 0xbfdb72cd3f331398} },
        { {0xbfe0000000000000, 0x401c462b9064a63b}, {0x3fdb73356b6145ff, 0x3fdb726511798004} },
        { {0x3feffee4e9dabc6b, 0x3e45798ee2308c3a}, {0x4005be4a2dd55326, 0x3e5d2ef29956c262} },
        { {0x3feffee4e9dabc6b, 0x40f63525129291ff}, {0x4001658fee4cfb72, 0x3ffa16220df8624d} },
        { {0x3feffee4e9dabc6b, 0x4046c6cbc45dc8de}, {0xbc3f089c48f017bf, 0x4005be4a2dd55326} },
        { {0x3fe921fb54442d18, 0x3e45798ee2308c3a}, {0x40018bd669471caa, 0x3e578cd7a0c4d798} },
        { {0x3fe921fb54442d18, 0xbfe0000000000000}, {0x3ffecbeaa6376bd8, 0xbff0d300d625a50c} },
        { {0x3fe921fb54442d18, 0x439332270327f466}, {0x3f995d2240821f37, 0xc0018b8d14805bfc} },
        { {0xbfe921fb54442d18, 0x3ff0000000000000}, {0x3fcf8836c98447aa, 0x3fd88ddd3e1c727c} },
        { {0xbfe921fb54442d18, 0xbff0000000000000}, {0x3fcf8836c98447aa, 0xbfd88ddd3e1c727c} },
        { {0xbfe921fb54442d18, 0xbff921f557442d18}, {0x3ec5d7a00d09db35, 0xbfdd2e171cee3d48} },
        { {0x401c462b9064a63b, 0x3e45798ee2308c3a}, {0x409259a91d464a17, 0x3ee8a117e480ab2a} },
        { {0x401c462b9064a63b, 0xbff921f557442d18}, {0x3f7b78ba6d141a18, 0xc09259a91d450113} },
        { {0x401c462b9064a63b, 0x0000724576454468}, {0x409259a91d464a17, 0x006061cfe7d26c92} },
        { {0x40f63525129291ff, 0x4000000000000000}, {0xfff0000000000000, 0x7ff0000000000000} },
        { {0x439332270327f466, 0xc085e00000000000}, {0xfff0000000000000, 0xfff0000000000000} },
        { {0x439332270327f466, 0xc086232bdd7abcd2}, {0xfff0000000000000, 0x7ff0000000000000} },
        { {0x403921fb54442d19, 0x3ff0000000000000}, {0x4224b01e8413e07c, 0x42301c1bc540ef89} },
        { {0x403921fb54442d19, 0x4046c6cbc45dc8de}, {0xbe6b53294b5680aa, 0x423325120141985f} },
        { {0x403921fb54442d19, 0x439332270327f466}, {0x41cbacb2493b3cc6, 0xc23324c1fe2b6dcd} },
        { {0xbff921f557442d18, 0x3e45798ee2308c3a}, {0x3fca9bd63c68a945, 0x3e21db58a954be4c} },
        { {0xbff921f557442d18, 0xc086be6fb2739468}, {0x3fbab64419d4e473, 0x3fc7039316b85ed5} },
        { {0xbff921f557442d18, 0x439332270327f466}, {0x3f633b64c0c33005, 0xbfca9b6707a38827} },
        { {0x0000724576454468, 0x400921fb54442d18}, {0xbff0000000000000, 0x3ca1a62633145c07} },
        { {0x0000724576454468, 0x0000724576454468}, {0x3ff0000000000000, 0x0000724576454468} },

    };
    
extern "C" ComplexDouble S_DP_CEXP(ComplexDouble);
 
void acc_fastcexp()
{
    const InOutCx64u *pTest = ACC_TEST_CX64_01;

    for(size_t k=0; k<LENGTH_OF(ACC_TEST_CX64_01); k++)
    {
        HoldCmplx64 v_x;
        HoldCmplx64 v_y;

        v_x.u = pTest[k].in;

        ComplexDouble x(v_x.f.r, v_x.f.i);
        ComplexDouble y;

        y = S_DP_CEXP(x);
        memcpy(&v_y, &y, sizeof(ComplexDouble));

        bool fail = false;
        if( (v_y.u.r != pTest[k].out.r) || (v_y.u.i != pTest[k].out.i) ) fail = true;

        if(fail)
        {
            HoldCmplx64 ref_y;
            ref_y.u.r = pTest[k].out.r;
            ref_y.u.i = pTest[k].out.i;

            U64 diffr = ref_y.u.r - v_y.u.r;
            U64 diffi = ref_y.u.i - v_y.u.i;

            if(ISNAN(ref_y.f.r) && ISNAN(ref_y.f.i) &&
                ISNAN(v_y.f.r) && ISNAN(v_y.f.i))
            {
                std::cout << "N";
                //std::cout << "NaN failure" << std::endl;
            }
            else if((diffr != 0x1) && (diffr != (~0x0)) && (diffi != 0x1) && (diffi != (~0x0)))
            {
                std::cout << std::endl;
                std::cout << std::hex << "In: (";
                std::cout.width(16);
                std::cout << v_x.u.r << ", " ;
                std::cout.width(16);
                std::cout<< v_x.u.i << ")" << " EX: (";
                std::cout.width(16);
                std::cout << pTest[k].out.r << ", ";
                std::cout.width(16);
                std::cout << pTest[k].out.i << ") AC: (";
                std::cout.width(16);
                std::cout << v_y.u.r << ", ";
                std::cout.width(16);
                std::cout << v_y.u.i << ")    ";
            }
            else
            {
                std::cout << "*";
            }
        }
        else
        {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

int main()
{
    acc_fastcexp();
    return 0;
}

