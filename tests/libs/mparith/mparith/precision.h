#ifndef PRECISION_H_INCLUDED
#define PRECISION_H_INCLUDED 1

/* Set REAL and choose float or double function name */

#if defined(FLOAT)
#define REAL float
#define ISDOUBLE 0
#define FROOT concat(FNAMEROOT,f)
#elif defined(DOUBLE)
#define REAL double
#define ISDOUBLE 1
#define FROOT FNAMEROOT
#else
#error You must compile with either -DFLOAT or -DDOUBLE
#endif

#endif
