#define FNAMEROOT trunc
#define FARGS "(x)"
#include "tests.h"

void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                  FILE *summaryFile, FILE *verboseSummaryFile);

void test_near_integers(fp_params params,
                        int base, int mantis, int emin, int emax,
                        int nLow, int n,
                        int verbose, REAL *maxRelErr, REAL *maxUlpErr,
                        REAL *maxX,
                        FILE *summaryFile, FILE *verboseSummaryFile);

void test_interval(fp_params params, int base, int mantis, int emin, int emax,
                   REAL a, REAL b, int n, int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal);
