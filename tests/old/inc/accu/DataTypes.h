#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#define LENGTH_OF(x) (sizeof(x)/sizeof(x[0]))

typedef double  F64;
typedef float   F32;

typedef unsigned long long  U64;
typedef unsigned int        U32;

union Hold64
{
    U64 u64;
    F64 f64;
    long int s64;
};

union Hold32
{
    U32 u32;
    F32 f32;
    int s32;
};

#ifdef WIN64
#include <float.h>
#pragma fenv_access (on)

#define FE_TONEAREST _RC_NEAR
#define FE_UPWARD  _RC_UP
#define FE_DOWNWARD _RC_DOWN
#define FE_TOWARDZERO _RC_CHOP
#define FE_ALL_EXCEPT _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT

unsigned int fegetround ();
int fesetround(unsigned int mode);
int feclearexcept(int mode);
#endif



#endif // __DATA_TYPES_H__

