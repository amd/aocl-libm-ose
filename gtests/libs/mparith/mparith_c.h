#ifndef MPARITH_C_H_INCLUDED
#define MPARITH_C_H_INCLUDED 1

#ifdef MALLOCDEBUG
/* This must be the first file included */
#include "mallocDebug.h"
#endif

#include "precision.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

/* This for the IEEE nextafter function */
#if defined(WINDOWS)
#include <float.h>
#endif

/* This for the finite function */
#ifdef sun
#include <ieeefp.h>
#endif

#include "mparith_f.h"
#include "nanny.h"
#include "controlword.h"
#include "statusword.h"
#include "cycles.h"

/* Set this to 1 to get PostScript error graphs, or to 0
   to get gnuplot's default window graphs */
#define ERRORGRAPHTYPE 1

struct fpstruct
{
  int n;
  int params[15];
};

typedef struct fpstruct fp_params;

void initMultiPrecision(int isdouble, int forcePrecision,
                        int *base, int *mantis, int *emin, int *emax,
                        fp_params *params);

void MPINIT(int BASE, int MANTIS, int EMIN, int EMAX, int ROUND,
            fp_params *PARAMS, int *IFAIL);

int *new_mp(fp_params params);

/* myrand returns a REAL in the range 0 to 1.
   We use a low quality generator to avoid having to
   find drand48. It's not important that the generator
   always starts at the same place.
   myrand2 is an independent copy used for cycle timing 
   functions. */
REAL myrand(void);
REAL myrand2(void);

/* Checks that the array PARAMS has correct check sum. */
int CHKSUM(fp_params PARAMS);

void MPINITCOPY(fp_params PARAMS_IN, int MANTIS, int EMIN, int EMAX,
                fp_params *PARAMS_OUT, int *IFAIL);

void MPTOD(int *XMP, fp_params PARAMS, REAL *X, int *IFAIL);

void DTOMP(REAL X, int *XMP, fp_params PARAMS, int *IFAIL);

void ITOMP(int K, int *XMP, fp_params PARAMS, int *IFAIL);

void MPSHOW(const char *TITLE, int *XMP, fp_params PARAMS, FILE *outfile);

void MPSHOWBIN(const char *TITLE, int *XMP, fp_params PARAMS,
               int space, FILE *outfile);

/* Returns .TRUE. if the number RIGHT is a regular f-p number. */
int MPISREG(int *XMP, fp_params PARAMS);

/* Returns .TRUE. if the number XMP is a fake f-p number
 * (uninitialised or corrupt).
 */
int MPISFAKE(int *XMP, fp_params PARAMS);

/* Returns .TRUE. if the number XMP is +ve or -ve zero. */
int MPISZERO(int *XMP, fp_params PARAMS);

/* Returns .TRUE. if the number XMP is +ve or -ve infinity. */
int MPISINFINITY(int *XMP, fp_params PARAMS);

/* Returns .TRUE. if the number XMP is NaN. */
int MPISNAN(int *XMP, fp_params PARAMS);

/* Returns .TRUE. if the number XMP is positive. */
int MPISPOS(int *XMP, fp_params PARAMS);

/* Returns .TRUE. if the number XMP is negative. */
int MPISNEG(int *XMP, fp_params PARAMS);

/* Copies MP number from SOURCE to DEST */
void MPCOPY(int *SOURCE, int *DEST, fp_params PARAMS, int *IFAIL);

/* Like MPCOPY except that SOURCE and DEST do not need
 * to have the same precision parameters.
 */
void MPCOPY2(int *SOURCE, fp_params PARAMS_SOURCE, int *DEST,
             fp_params PARAMS_DEST, int *IFAIL);

int MPEQ(int *LEFT, int *RIGHT, fp_params PARAMS);

int MPLT(int *LEFT, int *RIGHT, fp_params PARAMS);

int MPGT(int *LEFT, int *RIGHT, fp_params PARAMS);

int MPGE(int *LEFT, int *RIGHT, fp_params PARAMS);

int MPLE(int *LEFT, int *RIGHT, fp_params PARAMS);

int MPNE(int *LEFT, int *RIGHT, fp_params PARAMS);

/* Sets YMP to be the floor function of XMP. */
void MPFLOOR(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/*  Sets YMP to be the ceil function of XMP. */
void MPCEIL(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/* Sets YMP to be the trunc function of XMP. */
void MPTRUNC(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/* Sets YMP to be the rint function of XMP. */
void MPRINT(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/* Sets YMP to be the round function of XMP. */
void MPROUND(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/* Sets YMP to be the fractional part of XMP, i.e. the part
 *  left after subtracting TRUNC(XMP).
 */
void MPFRAC(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

void MPFREXP(int *XMP, int *YMP, int *IYMP, fp_params PARAMS, int *IFAIL);

void MPLDEXP(int *XMP, int EXPN, int *YMP, fp_params PARAMS, int *IFAIL);

void MPMODF(int *XMP, int *YMP, int *IYMP, fp_params PARAMS, int *IFAIL);

int MPMOD(int *XMP, fp_params PARAMS, int K, int *IFAIL);

/*  Returns the integer logb function of XMP. */
int MPILOGB(int *XMP, int emin_x, fp_params PARAMS, int *IFAIL);

/*  Sets YMP to be the logb function of XMP. */
void MPLOGB(int *XMP, int emin_x, int *YMP, fp_params PARAMS, int *IFAIL);

/* Returns 1 if XMP is neither infinite nor NaN, otherwise returns 0 */
int MPFINITE(int *XMP, fp_params PARAMS, int *IFAIL);

/* Adds LEFT to RIGHT, result in RESULT */
void PLUS(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Subtracts RIGHT from LEFT, result in RESULT */
void MINUS(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Multiplies LEFT by RIGHT, result in RESULT */
void TIMES(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Divides LEFT by RIGHT, result in RESULT */
void DIVIDE(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Computes inverse square root of RIGHT, result in RESULT */
void MPIROOT(int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Computes square root of RIGHT, result in RESULT */
void MPROOT(int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Computes hypot(x,y) = sqrt(x**2 + y**2), putting the result in RESULT. */
void MPHYPOT(int *XMP, int *YMP, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Computes fma(x,y,z) = x*y+z, putting the result in RESULT. */
void MPFMA(int *XMP, int *YMP, int *ZMP, int *RESULT, fp_params PARAMS,
           int *IFAIL);

/* Raises number XMP to power K, result in RESULT */
void MPIPOW(int K, int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Raises number XMP to power YMP, result in RESULT */
void MPPOW(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Raises number XMP to power YMP=0.5, result in RESULT */
void MPSQRT(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Raises number XMP to power YMP=1/3, result in RESULT */
void MPCBRT(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes fmod(XMP, YMP), result in RESULT */
void MPFMOD(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Puts the absolute value of XMP in YMP */
void MPFABS(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/* Computes remainder(XMP, YMP), result in RESULT */
void MPREMAINDER(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes remquo(XMP, YMP, &QUO), result in *QUO and RESULT */
void MPREMQUO(int *XMP, int *YMP, int *QMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes fmin(XMP, YMP), result in RESULT */
void MPFMIN(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes fmax(XMP, YMP), result in RESULT */
void MPFMAX(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes fdim(XMP, YMP), result in RESULT */
void MPFDIM(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes fract(XMP), result in RESULT */
void MPFRACT(int *XMP, int *YMP, int *IYMP, fp_params PARAMS, int *IFAIL);

/* Return integer part of number XMP */
int MPTOI(int *XMP, fp_params PARAMS, int *IFAIL);

/* Convert XMP to decimal string, and return pointer to it. */
char *MP2DEC(int *XMP, fp_params PARAMS, int DIGITS, int *IFAIL);

/* Thread-safe version of MP2DEC */
void MP2DEC_R(int *XMP, fp_params PARAMS, int DIGITS, char *C, int lenC,
              int *IFAIL);

/* Negates XMP in place. */
void MPNEGATE(int *XMP, fp_params PARAMS, int *IFAIL);

/* Changes the sign of XMP and puts the result in YMP. */
void MPCHGSIGN(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL);

/* Sets RESULT to be XMP with the sign of YMP */
void MPCOPYSIGN(int *XMP, int *YMP, int *RESULT, fp_params PARAMS, int *IFAIL);

/* Replaces XMP by its absolute value. */
void MPABS(int *XMP, fp_params PARAMS, int *IFAIL);

/* Computes remainder_piby2(x), x in radians, with the result in RESULT */
void MPREMAINDER_PIBY2(int *XMP, fp_params PARAMS,
                       int *RESULT, int *REGION, int *IFAIL);

/* Computes remainder_90d(x), x in degrees, with the result in RESULT */
void MPREMAINDER_90D(int *XMP, fp_params PARAMS,
                     int *RESULT, int *REGION, int *IFAIL);

/* Computes sin(x), cos(x), tan(x), of radian argument x,
   with the results in RESULT_SIN, RESULT_COS, and RESULT_TAN
   respectively.
 */
void MPSINCOSTAN(int *XMP, fp_params PARAMS,
                 int *RESULT_SIN, int *RESULT_COS, int *RESULT_TAN,
                 int *IFAIL);

/* Computes sinpi(x), cospi(x), tanpi(x), of radian argument x,
   with the results in RESULT_SIN, RESULT_COS, and RESULT_TAN
   respectively.
 */
void MPSINCOSTANPI(int *XMP, fp_params PARAMS,
                 int *RESULT_SIN, int *RESULT_COS, int *RESULT_TAN,
                 int *IFAIL);

/* Computes sin(x), cos(x), tan(x), of degree argument x,
   with the results in RESULT_SIN, RESULT_COS, and RESULT_TAN
   respectively.
 */
void MPSINCOSTAND(int *XMP, fp_params PARAMS,
                  int *RESULT_SIN, int *RESULT_COS, int *RESULT_TAN,
                  int *IFAIL);

/* Computes atan(x), with the result in RESULT. */
void MPATAN(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes atanpi(x), with the result in RESULT. */
void MPATANPI(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes atan2(y,x), with the result in RESULT. */
void MPATAN2(int *YMP, int *XMP, fp_params PARAMS, int *RESULT,
             int *IFAIL);

/* Computes atan2pi(y,x), with the result in RESULT. */
void MPATAN2PI(int *YMP, int *XMP, fp_params PARAMS, int *RESULT,
             int *IFAIL);

/* Computes asin(x), with the result in RESULT. */
void MPASIN(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes asinpi(x), with the result in RESULT. */
void MPASINPI(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes acos(x), with the result in RESULT. */
void MPACOS(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes acospi(x), with the result in RESULT. */
void MPACOSPI(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes sinh(x), with the result in RESULT. */
void MPSINH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes cosh(x), with the result in RESULT. */
void MPCOSH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes tanh(x), with the result in RESULT. */
void MPTANH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes arcsinh(x), with the result in RESULT. */
void MPASINH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes arccosh(x), with the result in RESULT. */
void MPACOSH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes arctanh(x), with the result in RESULT. */
void MPATANH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes log(x) (natural logarithm), with the result in RESULT. */
void MPLOG(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes log(1+x) (natural logarithm), with the result in RESULT. */
void MPLOG1P(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes log10(x) (logarithm to base 10), with the result in RESULT. */
void MPLOG10(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes log2(x) (logarithm to base 2), with the result in RESULT. */
void MPLOG2(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

void EPSILON(fp_params PARAMS, int *EPS);

void MPLARGE(fp_params PARAMS, int *LARGE);

void MPSMALL(fp_params PARAMS, int *SMALL);

void GET_PI(fp_params PARAMS, int *PI);

void GET_PI_ETC(fp_params PARAMS, int *PI, int *TWO_PI, int *PI_OVER_TWO,
                int *TWO_OVER_PI, int *IFAIL);

void GET_PRECOMPUTED_PI_ETC(fp_params PARAMS, int *PI, int *TWO_PI, int *PI_OVER_TWO,
                            int *TWO_OVER_PI, int *IFAIL);

void GET_LOG_ETC(fp_params PARAMS, int *LOGBASE, int *LOG10E, int *IFAIL);

void GET_PRECOMPUTED_LOG_ETC(fp_params PARAMS, int *LOGBASE, int *LOG10E, int *IFAIL);

void GET_E(fp_params PARAMS, int *E);

void MPCEXP(int *XMP, int *YMP, fp_params PARAMS, int *RESULT_RE, int *RESULT_IM, int *IFAIL);

void MPEXP(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

/* Computes expm1(XMP) ((e to power XMP) - 1), with the result in RESULT. */
void MPEXPM1(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

void MPEXP2(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

void MPEXP10(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

void MPPOW10(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL);

REAL MPRELDIFF(REAL X, int BASE_X, int MANTIS_X, int EMIN_X, int EMAX_X,
                 int *XMP, fp_params PARAMS, REAL *ulps, int *IFAIL);

/* Output a REAL precision number x in a format that Maple
   can read without conversion error */
void mapleX(FILE *file, REAL x, const char *varName, int base, int mantis);

/* Utility function used by writeGnuplotDriver and writeGnuplot3DDriver */
void gnuplotPause(FILE *file, int postscript);

/* Writes a gnuplot driver file */
int writeGnuplotDriver(const char *rootFileName,
                       const char *title,
                       int xlogscale,
                       int postscript,
                       REAL maxUlpErr, REAL maxX);

/* Writes a gnuplot 3D driver file */
int writeGnuplot3DDriver(const char *rootFileName,
                         const char *title,
                         int postscript,
                         REAL maxUlpErr, REAL maxX, REAL maxY);

/* Poor man's nextafter - doesn't have to be great */
REAL mynextafter(REAL x, REAL y, int mantis);

/* A pow function */
REAL mypow(REAL x, int k);

/* An integer sqrt function */
int mysqrt(int x);

/* A log function independent of libm */
REAL mylog(REAL x);

/* An exp function independent of libm */
REAL myexp(REAL x);

/* A min function */
REAL mymin(REAL x, REAL y);

int openResultsFiles(const char *test_string,
                     FILE **gnuplotDataFile,
                     FILE **gnuplotTimesFile,
                     FILE **summaryFile,
                     FILE **verboseSummaryFile);

void closeResultsFiles(FILE *gnuplotDataFile,
                       FILE *gnuplotTimesFile,
                       FILE *summaryFile,
                       FILE *verboseSummaryFile);

void addSummaryTitle(FILE *summaryFile,
                     int verbose,
                     FILE *verboseSummaryFile,
                     const char *funName);

void addSummarySpecial(FILE *summaryFile,
                       int verbose,
                       FILE *verboseSummaryFile,
                       const char *funName,
                       REAL x, REAL y,
                       REAL reldiff, REAL ulps,
                       const char *expected,
                       int *flagsOK, int checkInexact,
                       unsigned int expectedFlags, unsigned int observedFlags,
                       int expectedErrno, int observedErrno);

void addSummarySpecialSincos(FILE *summaryFile,
                             int verbose,
                             FILE *verboseSummaryFile,
                             const char *funName,
                             REAL x, REAL s, REAL c,
                             REAL reldiff, REAL ulps,
                             const char *expectedSin,
                             const char *expectedCos,
                             int *flagsOK, int checkInexact,
                             unsigned int expectedFlags, unsigned int observedFlags,
                             int expectedErrno, int observedErrno);

void addSummarySpecialCexp(FILE *summaryFile,
                             int verbose,
                             FILE *verboseSummaryFile,
                             const char *funName,
                             REAL x, REAL y, REAL z_re, REAL z_im,
                             REAL reldiff, REAL ulps,
                             const char *expectedRe,
                             const char *expectedIm,
                             int *flagsOK, int checkInexact,
                             unsigned int expectedFlags, unsigned int observedFlags,
                             int expectedErrno, int observedErrno);

void addSummarySpecial2(FILE *summaryFile,
                        int verbose,
                        FILE *verboseSummaryFile,
                        const char *funName,
                        REAL x, REAL y, REAL z,
                        REAL reldiff, REAL ulps,
                        const char *expected,
                        int *flagsOK, int checkInexact,
                        unsigned int expectedFlags, unsigned int observedFlags,
                        int expectedErrno, int observedErrno);

void addSummarySpecial2a(FILE *summaryFile,
                        int verbose,
                        FILE *verboseSummaryFile,
                        const char *funName,
                        REAL x, int y, REAL z,
                        REAL reldiff, REAL ulps,
                        const char *expected,
                        int *flagsOK, int checkInexact,
                        unsigned int expectedFlags, unsigned int observedFlags,
                        int expectedErrno, int observedErrno);

void addSummarySpecial3(FILE *summaryFile,
                        int verbose,
                        FILE *verboseSummaryFile,
                        const char *funName,
                        REAL x, REAL y, REAL z, REAL w,
                        REAL reldiff, REAL ulps,
                        const char *expected,
                        int *flagsOK, int checkInexact,
                        unsigned int expectedFlags, unsigned int observedFlags,
                        int expectedErrno, int observedErrno);

void addSummaryPiMultiples(FILE *summaryFile,
                           int verbose,
                           FILE *verboseSummaryFile,
                           const char *funName,
                           int nArgs,
                           REAL x, REAL y,
                           REAL maxreldiff, REAL maxulps,
                           const char *expected);

void addSummaryPiMultiplesSincos(FILE *summaryFile,
                                 int verbose,
                                 FILE *verboseSummaryFile,
                                 const char *funName,
                                 int nArgs,
                                 REAL x, REAL s, REAL c,
                                 REAL maxreldiff, REAL maxulps,
                                 const char *expectedSin, const char *expectedCos);

void addSummaryNearIntegers(FILE *summaryFile,
                            int verbose,
                            FILE *verboseSummaryFile,
                            const char *funName,
                            int nArgs, int nLow,
                            REAL x, REAL y,
                            REAL maxreldiff, REAL maxulps,
                            const char *expected);

void addSummaryInterval(FILE *summaryFile,
                        int verbose,
                        FILE *verboseSummaryFile,
                        const char *funName,
                        int nArgs,
                        REAL a, REAL b,
                        REAL maxreldiff, REAL maxulps,
                        REAL xMax, REAL cycles);

void addSummaryInterval2(FILE *summaryFile,
                         int verbose,
                         FILE *verboseSummaryFile,
                         const char *funName,
                         int nArgs,
                         REAL a, REAL b, REAL c, REAL d,
                         REAL maxreldiff, REAL maxulps,
                         REAL xMax, REAL yMax, REAL cycles);

void addSummaryInterval2a(FILE *summaryFile,
                         int verbose,
                         FILE *verboseSummaryFile,
                         const char *funName,
                         int nArgs,
                         REAL a, REAL b, int c, int d,
                         REAL maxreldiff, REAL maxulps,
                         REAL xMax, int yMax, REAL cycles);

void addSummaryInterval3(FILE *summaryFile,
                         int verbose,
                         FILE *verboseSummaryFile,
                         const char *funName,
                         int nArgs,
                         REAL a, REAL b, REAL c, REAL d,
                         REAL e, REAL f,
                         REAL maxreldiff, REAL maxulps,
                         REAL xMax, REAL yMax, REAL zMax,
                         REAL cycles);

void addSummaryConclusion(FILE *summaryFile,
                          int verbose,
                          FILE *verboseSummaryFile,
                          const char *funName,
                          REAL maxrelerr, REAL maxulperr,
                          REAL minTime, REAL maxTime,
                          REAL maxTimeNormal, int flagsOK);

void parseArgs(int argc, char *argv[], int *nTests, int *amean, int *doTimes,
               int *specialTest, int *forcePrecision);

REAL getReal(const char *buff, int base, int mantis, int emin, int emax);

int get2Reals(int base, int mantis, int emin, int emax,
              REAL *a, REAL *b);

int getRealsInts(int base, int mantis, int emin, int emax,
              REAL *a, REAL *b, int *c, int *d);

int get4Reals(int base, int mantis, int emin, int emax,
              REAL *a, REAL *b, REAL *c, REAL *d);

int get6Reals(int base, int mantis, int emin, int emax,
              REAL *a, REAL *b, REAL *c, REAL *d, REAL *e, REAL *f);

/* Compares expected status flags with obtained status flags */
int compare_status(unsigned int expectedFlags, unsigned int observedFlags,
                   int checkInexact,
                   int expectedErrno, int observedErrno, int verbose,
                   FILE *summaryFile, FILE *verboseSummaryFile);

/* Returns a character string containing a hex version of REAL x */
char *real2hex(REAL *x);

#endif
