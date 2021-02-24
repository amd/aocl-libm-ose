#ifndef PRECISION_H_INCLUDED
#define PRECISION_H_INCLUDED 1

#include "alm_mp_funcs.h"

/* Set REAL and choose float or double function name */

#if defined(FLOAT)
#define REAL float
#define ISDOUBLE 0
#define FROOT concat(FNAMEROOT,f)
#define ALM_MP_PRECI_BITS 128
#elif defined(DOUBLE)
#define REAL double
#define ISDOUBLE 1
#define FROOT FNAMEROOT
#define ALM_MP_PRECI_BITS 256
#else
#error You must compile with either -DFLOAT or -DDOUBLE
#endif

#endif
