#define FNAMEROOT fma
#define FARGS "(x,y,z)"
#include "tests.h"

void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, REAL *argy, REAL *argz,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr,
                  REAL *maxX, REAL *maxY, REAL *maxZ,
                  FILE *summaryFile, FILE *verboseSummaryFile);

void test_interval(fp_params params, int base, int mantis, int emin, int emax,
                   REAL a, REAL b, int nX,
                   REAL c, REAL d, int nY,
                   REAL e, REAL f, int nZ,
                   int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr,
                   REAL *maxX, REAL *maxY, REAL *maxZ,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal);
