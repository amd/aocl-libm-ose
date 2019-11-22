#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED 1

#undef FNAMEEXT
#undef FNAMEEXT_BLANK

#if defined(WINDOWS)
#undef inline
#define inline __inline
#ifdef IS_64BIT
#define LONG_TYPE __int64
#pragma warning( disable : 4391 )
#include "emmintrin.h"
#else
#define LONG_TYPE long
#endif
#else
/* Linux */
#define LONG_TYPE long
#endif

#if defined(TESTNAT32)
#include <math.h>
#define FNAMEEXT
#define FNAMEEXT_BLANK 1
#define OUTFILEEXT _nat32
#endif

#if defined(TESTNAT64)
#include <math.h>
#define FNAMEEXT
#define FNAMEEXT_BLANK 1
#define OUTFILEEXT _nat64
#endif

#if defined(TESTAMD32)
#include <libm_amd.h>
#define FNAMEEXT
#define FNAMEEXT_BLANK 1
#define OUTFILEEXT _amd32
#endif

#if defined(TESTAMD64)
#include <math.h>
#include <libm_amd.h>
#define FNAMEEXT
#define FNAMEPREFIX amd_
#define FNAMEEXT_BLANK 1
#define OUTFILEEXT _amd64
#endif

#if defined(TESTFD)
#include <libm_fd.h>
#define FNAMEEXT _fd
#define FNAMEEXT_BLANK 0
#define OUTFILEEXT _fd
#endif

#if defined(TESTNAG)
#include <libm_nag.h>
#define FNAMEEXT _nag
#define FNAMEEXT_BLANK 0
#define OUTFILEEXT _nag
#endif

/* Check that required macros are defined */
#if !defined(FNAMEEXT)
#error You must compile with -DTESTNAT32, -DTESTNAT64, -DTESTAMD64 or -DTESTAMD32
#endif
#ifndef FNAMEROOT
#error You must #define FNAMEROOT in your test program header
#endif
#ifndef FARGS
#error You must #define FARGS in your test program header
#endif

#if defined(TESTNAT64) || defined(TESTAMD64)
/* Compile-time verification that type long is the same size
   as type double (i.e. we are really on a 64-bit machine) */
void check_long_against_double_size(int machine_is_64_bit[(sizeof(LONG_TYPE) == sizeof(double))?1:-1]); 
#elif defined(TESTNAT32) || defined(TESTAMD32)
/* Compile-time verification that type long is the same size
   as type float (i.e. we are really on a 32-bit machine) */
void check_long_against_float_size(int machine_is_32_bit[(sizeof(LONG_TYPE) == sizeof(float))?1:-1]); 
#endif

#include "precision.h"

#include "mparith_c.h"
#include <stdio.h>
#include <errno.h>

/* Stringify and concatenation macros */

#define _stringy(x) #x
#define stringy(x) _stringy(x)
#define _concat(x,y) x##y
#define concat(x,y) _concat(x,y)

/* Derive full name of function and its string form */

/* FNAMEEXT_BLANK is used to avoid concatenating an empty string */
#if FNAMEEXT_BLANK
#define FNAME FROOT
#else
//#define FNAME concat(FROOT, FNAMEEXT)
#define FNAME concat(FNAMEPREFIX, FROOT)
#endif

#define FNAMESTR stringy(FNAME)

/* Derive the function specification in string form */

#define FSPECSTR FNAMESTR FARGS

/* Stringify extension to output files */

#define OUTFILEEXTSTR stringy(concat(FROOT, OUTFILEEXT))

#endif
