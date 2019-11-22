#define FNAMEROOT pown
#define FARGS "(x,y)"
#include "tests.h"

extern float pownf(float x, int n);

void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, int *argy,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr,
                  REAL *maxX, int *maxY,
                  FILE *summaryFile, FILE *verboseSummaryFile);

void test_integral_y(fp_params params,
                     int base, int mantis, int emin, int emax,
                     REAL a, REAL b, int nX, int integralX,
                     int verbose, REAL *maxRelErr, REAL *maxUlpErr,
                     REAL *maxX, int *maxY,
                     FILE *summaryFile, FILE *verboseSummaryFile);

void test_interval(fp_params params, int base, int mantis, int emin, int emax,
                   REAL a, REAL b, int nX,
                   int c, int d, int nY,
                   int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr,
                   REAL *maxX, int *maxY,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal);
