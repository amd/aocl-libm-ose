#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#include <cstring>
#include <complex>
#include <math.h>

#ifdef __INTEL_COMPILER
#define S_SP_CEXP cexpf
#else
#define S_SP_CEXP cexpf
//#define S_SP_CEXP fastcexpf
#endif

#ifdef WIN64
#define ISNAN _isnan
#else
#define ISNAN std::isnan
#endif

typedef std::complex<float> ComplexFloat;

struct CmplxU32
{
    U32 r;
    U32 i;
};

struct CmplxF32
{
    F32 r;
    F32 i;
};

struct InOutCx32u
{
    CmplxU32 in;
    CmplxU32 out;
};

union HoldCmplx32
{
    CmplxU32 u;
    CmplxF32 f;
};

const static InOutCx32u ACC_TEST_CX32_01[] = {
//special tests for verification purpose                //    x,    y, exp(x), cos(y), sin(y), real, imag
{ {0x7f800000, 0x3f800000}, {0x7f800000, 0x7f800000} }, //  inf,    1,    inf,      +,      +,  inf,  inf
{ {0x44800000, 0x3fd9999a}, {0xff800000, 0x7f800000} },	// 1024,  1.7,    inf,      -,      +, -inf,  inf
{ {0x7f800000, 0xbf4ccccd}, {0x7f800000, 0xff800000} }, //  inf, -0.8,    inf,      +,      -,  inf, -inf
{ {0x44800000, 0x00000000}, {0x7f800000, 0xffc00000} }, // 1024,    0,    inf,      1,      0,  inf,  nan
{ {0x44800000, 0x80000000}, {0x7f800000, 0xffc00000} }, // 1024,   -0,    inf,      1,     -0,  inf,  nan
{ {0x7f800000, 0x7f800000}, {0x7fc00000, 0x7fc00000} }, //  inf,  inf,    inf,    nan,    nan,  nan,  nan
{ {0x44800000, 0x7fc00000}, {0x7fc00000, 0x7fc00000} }, // 1024,  nan,    inf,    nan,    nan,  nan,  nan
{ {0xc4800000, 0x7f800000}, {0x00000000, 0x00000000} }, //-1024,  inf,      0,    nan,    nan,    0,    0
// cannot get the exact value of Pi
//{ {0x7f800000, 0x40490fdb}, {0xff800000, 0xffc00000} }, //  inf,   pi,    inf,     -1,      0, -inf,  nan
//{ {0x44800000, 0x3fc90fdb}, {0xffc00000, 0x7f800000} }, // 1024, pi/2,    inf,      0,      1,  nan,  inf
//{ {0x7f800000, 0xbfc90fdb}, {0xffc00000, 0xff800000} }, //  inf,-pi/2,    inf,      0,     -1,  nan, -inf

        { {0x3f800000, 0x3f800000}, {0x3fbbfe29, 0x40126407} },
        { {0x3f800000, 0xbf800000}, {0x3fbbfe29, 0xc0126407} },
        { {0x3f800000, 0x80000000}, {0x402df854, 0x80000000} },
        { {0x3f800000, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x3f800000, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0xbf800000, 0x3f800000}, {0x3e4b8957, 0x3e9e7ea2} },
        { {0xbf800000, 0xbf800000}, {0x3e4b8957, 0xbe9e7ea2} },
        { {0x00000000, 0x3f800000}, {0x3f0a5140, 0x3f576aa4} },
        { {0x00000000, 0xbf800000}, {0x3f0a5140, 0xbf576aa4} },
        { {0x00000000, 0x00000000}, {0x3f800000, 0x00000000} },
        { {0x00000000, 0x80000000}, {0x3f800000, 0x80000000} },
        { {0x00000000, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x00000000, 0xc47a0000}, {0x3f0ff813, 0xbf53ae61} },
        { {0x00000000, 0x447a0000}, {0x3f0ff813, 0x3f53ae61} },
        { {0x80000000, 0x3f800000}, {0x3f0a5140, 0x3f576aa4} },
        { {0x80000000, 0xbf800000}, {0x3f0a5140, 0xbf576aa4} },
        { {0x80000000, 0x00000000}, {0x3f800000, 0x00000000} },
        { {0x80000000, 0x80000000}, {0x3f800000, 0x80000000} },
        { {0x80000000, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x80000000, 0xc47a0000}, {0x3f0ff813, 0xbf53ae61} },
        { {0x80000000, 0x447a0000}, {0x3f0ff813, 0x3f53ae61} },
        { {0x7f800000, 0x3f800000}, {0x7f800000, 0x7f800000} },
        { {0x7f800000, 0xbf800000}, {0x7f800000, 0xff800000} },
        { {0x7f800000, 0x00000000}, {0x7f800000, 0xffc00000} },
        { {0x7f800000, 0x80000000}, {0x7f800000, 0xffc00000} },
        { {0x7f800000, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800000, 0xc47a0000}, {0x7f800000, 0xff800000} },
        { {0x7f800000, 0x447a0000}, {0x7f800000, 0x7f800000} },
        { {0xff800000, 0x3f800000}, {0x00000000, 0x00000000} },
        { {0xff800000, 0xbf800000}, {0x00000000, 0x80000000} },
        { {0xff800000, 0x00000000}, {0x00000000, 0x00000000} },
        { {0xff800000, 0x80000000}, {0x00000000, 0x80000000} },
        { {0xff800000, 0x7f800000}, {0x00000000, 0x00000000} },
        { {0xff800000, 0xff800000}, {0x00000000, 0x80000000} },
        { {0xff800000, 0x7fc00000}, {0x00000000, 0x00000000} },
        { {0xff800000, 0xffc00000}, {0x00000000, 0x80000000} },
        { {0xff800000, 0x7f800123}, {0x00000000, 0x00000000} },
        { {0xff800000, 0xff800123}, {0x00000000, 0x80000000} },
        { {0xff800000, 0x7fc00123}, {0x00000000, 0x00000000} },
        { {0xff800000, 0xffc00123}, {0x00000000, 0x80000000} },
        { {0xff800000, 0xc47a0000}, {0x00000000, 0x80000000} },
        { {0xff800000, 0x447a0000}, {0x00000000, 0x00000000} },
        { {0x7fc00000, 0x3f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0xbf800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x00000000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x80000000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0xc47a0000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00000, 0x447a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x3f800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0xbf800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x00000000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x80000000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0xc47a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00000, 0x447a0000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x3f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0xbf800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x00000000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x80000000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0xc47a0000}, {0x7fc00000, 0x7fc00000} },
        { {0x7f800123, 0x447a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x3f800000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0xbf800000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x00000000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x80000000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0xc47a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xff800123, 0x447a0000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x3f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0xbf800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x00000000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x80000000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0xc47a0000}, {0x7fc00000, 0x7fc00000} },
        { {0x7fc00123, 0x447a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x3f800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0xbf800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x00000000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x80000000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x7f800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0xff800000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x7fc00000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0xffc00000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x7f800123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0xff800123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x7fc00123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0xffc00123}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0xc47a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xffc00123, 0x447a0000}, {0x7fc00000, 0x7fc00000} },
        { {0xc47a0000, 0x7f800123}, {0x00000000, 0x00000000} },
        { {0xc47a0000, 0xffc00123}, {0x00000000, 0x00000000} },
        { {0xc47a0000, 0xc47a0000}, {0x00000000, 0x80000000} },
        { {0x447a0000, 0xbf800000}, {0x7f800000, 0xff800000} },
        { {0x447a0000, 0x80000000}, {0x7f800000, 0xffc00000} },
        { {0x40490fdb, 0x3f800000}, {0x41480c2b, 0x419bc71d} },
        { {0x40490fdb, 0xbf800000}, {0x41480c2b, 0xc19bc71d} },
        { {0x40490fdb, 0x5c999138}, {0xc13ec435, 0xc19ea962} },
        { {0xc435f37e, 0x5c999138}, {0x80000000, 0x80000000} },
        { {0x3f800000, 0xc435f37e}, {0x3faea71e, 0x40167701} },
        { {0x3f800000, 0x41200000}, {0xc011f92a, 0xbfbd4968} },
        { {0x41200000, 0xc431195f}, {0xc4342503, 0x46abfd5a} },
        { {0x41200000, 0xbf000000}, {0x46970415, 0xc6250033} },
        { {0x447a0000, 0x40490fdb}, {0xff800000, 0xff800000} },
        { {0x447a0000, 0xbfc90fab}, {0x7f800000, 0xff800000} },
        { {0x44200000, 0x4236365e}, {0x7f800000, 0x7f800000} },
        { {0x44200000, 0x5c999138}, {0xff800000, 0xff800000} },
        { {0xc42f0000, 0x322bcc77}, {0x00000000, 0x00000000} },
        { {0xc42f0000, 0x40490fdb}, {0x80000000, 0x80000000} },
        { {0xc4312d57, 0xbf490fdb}, {0x00000000, 0x80000000} },
        { {0xc4312d57, 0x4236365e}, {0x00000000, 0x00000000} },
        { {0xb8c00000, 0x3f7ff727}, {0x3f0a5574, 0x3f5760d1} },
        { {0xb8c00000, 0xbf490fdb}, {0x3f3500b5, 0xbf3500b5} },
        { {0x3f000000, 0xbf800000}, {0x3f640bfb, 0xbfb194b8} },
        { {0x3f000000, 0x41200000}, {0xbfb11316, 0xbf659dce} },
        { {0x3f000000, 0xbfc90fab}, {0x371d1172, 0xbfd3094c} },
        { {0xbf000000, 0x322bcc77}, {0x3f1b4598, 0x31d06700} },
        { {0xbf000000, 0xbf490fdb}, {0x3edb966a, 0xbedb966a} },
        { {0xbf000000, 0x40e2315d}, {0x3edb99a8, 0x3edb932c} },
        { {0x3f7ff727, 0x322bcc77}, {0x402df251, 0x32e97794} },
        { {0x3f7ff727, 0x47b1a929}, {0x400ad4bc, 0x3fd19a5f} },
        { {0x3f7ff727, 0x4236365e}, {0x35bde0b2, 0x402df251} },
        { {0x3f490fdb, 0x322bcc77}, {0x400c5eb3, 0x32bc66bd} },
        { {0x3f490fdb, 0xbf000000}, {0x3ff65f54, 0xbf869807} },
        { {0x3f490fdb, 0x5c999138}, {0xbf90a5a6, 0xbff09b95} },
        { {0xbf490fdb, 0x3f800000}, {0x3e7c41b6, 0x3ec46eea} },
        { {0xbf490fdb, 0xbf800000}, {0x3e7c41b6, 0xbec46eea} },
        { {0xbf490fdb, 0xbfc90fab}, {0x362dbe27, 0xbee970b9} },
        { {0x40e2315d, 0x322bcc77}, {0x4492cd4b, 0x374508c2} },
        { {0x40e2315d, 0xbfc90fab}, {0x3bda854f, 0xc492cd4b} },
        { {0x40e2315d, 0x00072451}, {0x4492cd4b, 0x03830e0c} },
        { {0x47b1a929, 0x40000000}, {0xff800000, 0x7f800000} },
        { {0x5c999138, 0xc42f0000}, {0xff800000, 0xff800000} },
        { {0x5c999138, 0xc431195f}, {0xff800000, 0x7f800000} },
        { {0x41c90fdb, 0x3f800000}, {0x512580fb, 0x5180e0e4} },
        { {0x41c90fdb, 0x4236365e}, {0x47272f92, 0x51992897} },
        { {0x41c90fdb, 0x5c999138}, {0xd11dd34b, 0xd18343ad} },
        { {0xbfc90fab, 0x322bcc77}, {0x3e54deb1, 0x310edac5} },
        { {0xbfc90fab, 0xc435f37e}, {0x3dd5b490, 0x3e381be3} },
        { {0xbfc90fab, 0x5c999138}, {0xbddb5b2c, 0xbe36709c} },
        { {0x00072451, 0x40490fdb}, {0xbf800000, 0xb3bbbd2e} },
        { {0x00072451, 0x00072451}, {0x3f800000, 0x00072451} },

    };
    
extern "C" ComplexFloat S_SP_CEXP(ComplexFloat);
 
void acc_fastcexpf()
{
    const InOutCx32u *pTest = ACC_TEST_CX32_01;

    for(size_t k=0; k<LENGTH_OF(ACC_TEST_CX32_01); k++)
    {
        HoldCmplx32 v_x;
        HoldCmplx32 v_y;

        v_x.u = pTest[k].in;

        ComplexFloat x(v_x.f.r, v_x.f.i);
        ComplexFloat y;

        y = S_SP_CEXP(x);
        memcpy(&v_y, &y, sizeof(ComplexFloat));

        bool fail = false;
        if( (v_y.u.r != pTest[k].out.r) || (v_y.u.i != pTest[k].out.i) ) fail = true;

        if(fail)
        {
            HoldCmplx32 ref_y;
            ref_y.u.r = pTest[k].out.r;
            ref_y.u.i = pTest[k].out.i;

            U32 diffr = ref_y.u.r - v_y.u.r;
            U32 diffi = ref_y.u.i - v_y.u.i;

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
                std::cout.width(8);
                std::cout << v_x.u.r << ", " ;
                std::cout.width(8);
                std::cout<< v_x.u.i << ")" << " EX: (";
                std::cout.width(8);
                std::cout << pTest[k].out.r << ", ";
                std::cout.width(8);
                std::cout << pTest[k].out.i << ") AC: (";
                std::cout.width(8);
                std::cout << v_y.u.r << ", ";
                std::cout.width(8);
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
    acc_fastcexpf();
    return 0;
}
