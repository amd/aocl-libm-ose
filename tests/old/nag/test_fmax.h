#define FNAMEROOT fmax
#define FARGS "(x,y)"
#include "tests.h"

void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, REAL *argy,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr,
                  REAL *maxX, REAL *maxY,
                  FILE *summaryFile, FILE *verboseSummaryFile);

void test_interval(fp_params params, int base, int mantis, int emin, int emax,
                   REAL a, REAL b, int nX,
                   REAL c, REAL d, int nY,
                   int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr,
                   REAL *maxX, REAL *maxY,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal);
