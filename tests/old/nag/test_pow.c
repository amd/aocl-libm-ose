#include "test_pow.h"

int main(int argc, char *argv[])
{

  const char *test_string = "test_" OUTFILEEXTSTR;

  FILE *gnuplotDataFile;
  FILE *gnuplotTimesFile;
  FILE *summaryFile;
  FILE *verboseSummaryFile;

  int verbose = 2;

  int nTests, nLargeMagnitudeTests, nTimes, amean, specialTest, forcePrecision;
  int base, mantis, emin, emax;
  int i, k, j, nX, nY, imin, retVal = 0;
  int flagsOK = 1;
  unsigned int expectedFlags = 0;
  int expectedErrno = 0;
  int checkInexact = 0;
  fp_params params;
  REAL x, y, a, b, c, d, maxRelErr, maxUlpErr, maxX, maxY, minTime,
         maxTime, maxTimeNormal, overhead;

  /* Examine command line arguments */
  parseArgs(argc, argv, &nTests, &nTimes, &amean, &specialTest,
            &forcePrecision);

  nLargeMagnitudeTests = nTests / 10;
  if (nLargeMagnitudeTests < 10)
    nLargeMagnitudeTests = 10;

  /* Compute the overhead of the timing function */
  overhead = computeTimerOverhead();

  printf("Test for function %s\n", FSPECSTR);
  printf("------------------");
  k = (int)strlen(FSPECSTR);
  for (i = 0; i < k; i++) printf("-");
  printf("\n");

  if (!openResultsFiles(test_string,
                        &gnuplotDataFile,
                        &gnuplotTimesFile,
                        &summaryFile,
                        &verboseSummaryFile))
    {
      printf("Test aborted.\n");
      retVal = 1;
      goto done;
    }
  else
    {
      /* Initialize a floating-point number system based on IEEE
         float or double precision characteristics. */

      initMultiPrecision(ISDOUBLE, forcePrecision,
                         &base, &mantis, &emin, &emax, &params);
      addSummaryTitle(summaryFile, verbose, verboseSummaryFile,
                      FSPECSTR);
      fprintf(summaryFile, "Overhead = %5.1f\n", overhead);

      maxRelErr = 0.0;
      maxUlpErr = 0.0;
      maxX = 0.0;
      maxY = 0.0;
      minTime = 0.0;
      maxTime = 0.0;
      maxTimeNormal = 0.0;

      if (specialTest)
        {
          int nr;
          verbose = 3;
          nX = nY = mysqrt(nTests);
          x = 0.0;
          while (x != -111.0)
            {
              printf("\nInput x, y, or a, b, c, d for interval [a,b][c,d] (-111 to finish)\n");
              fflush(stdout);
              nr = get4Reals(base, mantis, emin, emax, &a, &b, &c, &d);
              x = a;
              y = b;
              if (x == -222.0)
                {
                  /* Special code to insert a blank line in the
                     gnuplot data files so that the 3D plots can
                     get lines drawn in both directions */
                  fprintf(gnuplotDataFile, "\n");
                  fprintf(gnuplotTimesFile, "\n");
                }
              else if (x != -111.0)
                {
                  if (nr == 2)
                    test_special(params, base, mantis, emin, emax, &x, &y, 0,
                                 checkInexact, 0, 0, verbose,
                                 &maxRelErr, &maxUlpErr, &maxX, &maxY,
                                 summaryFile, verboseSummaryFile);
                  else if (nr >= 4)
                    test_interval(params, base, mantis, emin, emax,
                                  a, b, nX,
                                  c, d, nY,
                                  nTimes, amean, overhead, verbose,
                                  gnuplotDataFile, gnuplotTimesFile,
                                  summaryFile, verboseSummaryFile,
                                  &maxRelErr, &maxUlpErr, &maxX, &maxY,
                                  &minTime, &maxTime, &maxTimeNormal);
                }
            }
          goto conclude;
        }

      /* Test (0, -1) */
      createZero(0, &x);
      y = -1.0;
      expectedFlags = AMD_F_DIVBYZERO;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0, -1) */
      createZero(1, &x);
      y = -1.0;
      expectedFlags = AMD_F_DIVBYZERO;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0, -0.5) */
      createZero(0, &x);
      y = -0.5;
      expectedFlags = AMD_F_DIVBYZERO;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0, -0.5) */
      createZero(1, &x);
      y = -0.5;
      expectedFlags = AMD_F_DIVBYZERO;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0, 3) */
      createZero(1, &x);
      y = 3.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0, 3) */
      createZero(0, &x);
      y = 3.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0, 2) */
      createZero(1, &x);
      y = 2.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0, 2) */
      createZero(0, &x);
      y = 2.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1, -inf) */
      x = -1.0;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1, inf) */
      x = -1.0;
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, 3.3) */
      x = (REAL)1.0;
      y = (REAL)3.3;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, inf) */
      x = (REAL)1.0;
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, -inf) */
      x = (REAL)1.0;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, QNaN) */
      x = (REAL)1.0;
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, SNaN) */
      x = (REAL)1.0;
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1, QNaN) */
      x = -1.0;
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1, SNaN) */
      x = -(REAL)1.0;
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.5, QNaN) */
      x = (REAL)1.5;
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.5, SNaN) */
      x = (REAL)1.5;
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.5, QNaN) */
      x = (REAL)-1.5;
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.5, SNaN) */
      x = (REAL)-1.5;
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, 0) */
      x = (REAL)1.0;
      createZero(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1, -0) */
      x = (REAL)1.0;
      createZero(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.5, 0) */
      x = (REAL)1.5;
      createZero(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, 0) */
      createInfinity(0, &x);
      createZero(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, 0) */
      createNaN(0, 0, &x);
      createZero(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, 0) */
      createNaN(0, 1, &x);
      createZero(0, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, -0) */
      createNaN(0, 0, &x);
      createZero(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, -0) */
      createNaN(0, 1, &x);
      createZero(1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, 23) */
      createNaN(0, 0, &x);
      y = (REAL)23.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, 23) */
      createNaN(0, 1, &x);
      y = (REAL)23.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, -23) */
      createNaN(0, 0, &x);
      y = (REAL)-23.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, -23) */
      createNaN(0, 1, &x);
      y = (REAL)-23.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, 2.3) */
      createNaN(0, 0, &x);
      y = (REAL)2.3;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, 2.3) */
      createNaN(0, 1, &x);
      y = (REAL)2.3;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, -2.3) */
      createNaN(0, 0, &x);
      y = (REAL)-2.3;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, -2.3) */
      createNaN(0, 1, &x);
      y = (REAL)-2.3;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0.5, 23) */
      x = (REAL)-0.5;
      y = (REAL)23.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0.5, 23.6) */
      x = (REAL)-0.5;
      y = (REAL)23.6;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0.5, -inf) */
      x = (REAL)0.5;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0.5, -inf) */
      x = (REAL)-0.5;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.5, -inf) */
      x = (REAL)1.5;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.2, -inf) */
      x = (REAL)-1.2;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0.7, inf) */
      x = (REAL)0.7;
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0.7, inf) */
      x = (REAL)-0.7;
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.2, inf) */
      x = (REAL)1.2;
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.6, inf) */
      x = (REAL)-1.6;
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, -1) */
      createInfinity(1, &x);
      y = (REAL)-1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, -1.3) */
      createInfinity(1, &x);
      y = (REAL)-1.3;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, -2) */
      createInfinity(1, &x);
      y = (REAL)-2.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, 5) */
      createInfinity(1, &x);
      y = (REAL)5.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, 2) */
      createInfinity(1, &x);
      y = (REAL)2.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, 2.1) */
      createInfinity(1, &x);
      y = (REAL)2.1;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, -3.6) */
      createInfinity(0, &x);
      y = (REAL)-3.6;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, 3.14) */
      createInfinity(0, &x);
      y = (REAL)3.14;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, inf) */
      createInfinity(0, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, -inf) */
      createInfinity(0, &x);
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, inf) */
      createInfinity(1, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, -inf) */
      createInfinity(1, &x);
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0, inf) */
      createZero(0, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0, inf) */
      createZero(1, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (0, -inf) */
      createZero(0, &x);
      createInfinity(1, &y);
      expectedFlags = AMD_F_DIVBYZERO;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-0, -inf) */
      createZero(1, &x);
      createInfinity(1, &y);
      expectedFlags = AMD_F_DIVBYZERO;
      expectedErrno = EDOM;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+lambda, 0.0) */
      createLargest(0, mantis, emax, &x);
      y = (REAL)0.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, -lambda) */
      x = (REAL)1.0;
      createLargest(1, mantis, emax, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+lambda, +lambda) */
      createLargest(0, mantis, emax, &x);
      createLargest(0, mantis, emax, &y);
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+lambda, -lambda) */
      createLargest(0, mantis, emax, &x);
      createLargest(1, mantis, emax, &y);
      expectedFlags = AMD_F_INEXACT | AMD_F_UNDERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-lambda, +lambda) */
      createLargest(1, mantis, emax, &x);
      createLargest(0, mantis, emax, &y);
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-lambda, -lambda) */
      createLargest(1, mantis, emax, &x);
      createLargest(1, mantis, emax, &y);
      expectedFlags = AMD_F_INEXACT | AMD_F_UNDERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Tests of integral y values with 10 non-integer x values in
         range [0.0, 1.0] */
      a = (REAL)0.0;
      b = (REAL)1.0;
      test_integral_y(params, base, mantis, emin, emax, a, b, 10, 0,
                      verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      summaryFile, verboseSummaryFile);

      /* Tests of integral y values with 10 non-integer x values in
         range [-1.0, 0.0] */
      a = (REAL)-1.0;
      b = (REAL)0.0;
      test_integral_y(params, base, mantis, emin, emax, a, b, 10, 0,
                      verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      summaryFile, verboseSummaryFile);

      /* Tests of integral y values with 10 non-integer x values in
         range [1.0, 2.0] */
      a = (REAL)1.0;
      b = (REAL)2.0;
      test_integral_y(params, base, mantis, emin, emax, a, b, 10, 0,
                      verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      summaryFile, verboseSummaryFile);

      /* Tests of integral y values with 10 non-integer x values in
         range [-2.0, -1.0] */
      a = (REAL)-2.0;
      b = (REAL)-1.0;
      test_integral_y(params, base, mantis, emin, emax, a, b, 10, 0,
                      verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      summaryFile, verboseSummaryFile);

      /* Tests of integral y values with 10 integer x values in
         range [1.0, 10.0] */
      a = (REAL)1.0;
      b = (REAL)10.0;
      test_integral_y(params, base, mantis, emin, emax, a, b, 10, 1,
                      verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      summaryFile, verboseSummaryFile);

      /* Tests of integral y values with 10 integer x values in
         range [-10.0, -1.0] */
      a = (REAL)-10.0;
      b = (REAL)-1.0;
      test_integral_y(params, base, mantis, emin, emax, a, b, 10, 1,
                      verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      summaryFile, verboseSummaryFile);

      /* Random tests in intervals spread across the whole range */

      for (j = 0; j < 11; j++)
        {
          if (j == 0)
            {
              /* Denormalized y arguments */
              createSmallestDenormal(0, mantis, emin, &c);
              createSmallestNormal(0, emin, &d);
              nY = mysqrt(nLargeMagnitudeTests);
            }
          else if (j == 1)
            {
              /* Small y arguments */
              c = mypow(2.0, emin);
              d = mypow(2.0, emin + 50);
              nY = mysqrt(nLargeMagnitudeTests);
            }
          else if (j == 2)
            {
              /* Smallish y arguments */
              c = mypow(2.0, -mantis - 1);
              d = mypow(2.0, -mantis + 1);
              nY = mysqrt(nLargeMagnitudeTests);
            }
          else if (j < 9)
            {
              /* Medium size y arguments */
              c = mypow(2.0, j - 4);
              d = mypow(2.0, j - 3);
              nY = mysqrt(nLargeMagnitudeTests);
            }
          else if (j == 9)
            {
              /* Largish y arguments */
              c = mypow(2.0, mantis - 1);
              d = mypow(2.0, mantis + 1);
              nY = mysqrt(nLargeMagnitudeTests);
            }
          else
            {
              /* Large y arguments */
              c = mypow(2.0, emax - 50);
              d = mypow(2.0, emax - 1);
              nY = mysqrt(nLargeMagnitudeTests);
            }

          /* Denormalized x arguments */
          createSmallestDenormal(0, mantis, emin, &a);
          createSmallestNormal(0, emin, &b);
          nX = mysqrt(nLargeMagnitudeTests);
          test_interval(params, base, mantis, emin, emax,
                        a, b, nX,
                        c, d, nY,
                        nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX, &maxY,
                        &minTime, &maxTime, &maxTimeNormal);

          /* Small x arguments */
          imin = ((emin + 1) / 50) * 50 - 10;
          a = mypow(2.0, emin);
          b = mypow(2.0, imin);
          nX = mysqrt(nLargeMagnitudeTests);
          test_interval(params, base, mantis, emin, emax,
                        a, b, nX,
                        c, d, nY,
                        nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX, &maxY,
                        &minTime, &maxTime, &maxTimeNormal);
          for (i = imin; i <= -60; i+=50)
            {
              a = mypow(2.0, i);
              b = mypow(2.0, i + 50);
              nX = mysqrt(nLargeMagnitudeTests);
              test_interval(params, base, mantis, emin, emax,
                            a, b, nX,
                            c, d, nY,
                            nTimes, amean, overhead, verbose,
                            gnuplotDataFile, gnuplotTimesFile,
                            summaryFile, verboseSummaryFile,
                            &maxRelErr, &maxUlpErr, &maxX, &maxY,
                            &minTime, &maxTime, &maxTimeNormal);
            }

          /* Medium size x arguments */
          for (i = -10; i < 10; i++)
            {
              a = mypow(2.0, i);
              b = mypow(2.0, i + 1);
              nX = mysqrt(nTests);
              test_interval(params, base, mantis, emin, emax,
                            a, b, nX,
                            c, d, nY,
                            nTimes, amean, overhead, verbose,
                            gnuplotDataFile, gnuplotTimesFile,
                            summaryFile, verboseSummaryFile,
                            &maxRelErr, &maxUlpErr, &maxX, &maxY,
                            &minTime, &maxTime, &maxTimeNormal);
            }

          /* Large x arguments */
          for (i = 10; i < emax-50; i+=50)
            {
              a = mypow(2.0, i);
              b = mypow(2.0, i + 50);
              nX = mysqrt(nLargeMagnitudeTests);
              test_interval(params, base, mantis, emin, emax,
                            a, b, nX,
                            c, d, nY,
                            nTimes, amean, overhead, verbose,
                            gnuplotDataFile, gnuplotTimesFile,
                            summaryFile, verboseSummaryFile,
                            &maxRelErr, &maxUlpErr, &maxX, &maxY,
                            &minTime, &maxTime, &maxTimeNormal);
            }
          a = mypow(2.0, i);
          createLargest(0, mantis, emax, &b);
          nX = mysqrt(nLargeMagnitudeTests);
          test_interval(params, base, mantis, emin, emax,
                        a, b, nX,
                        c, d, nY,
                        nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX, &maxY,
                        &minTime, &maxTime, &maxTimeNormal);

          /* Put blank lines in the gnuplot data files so that we can
             get a 3D plot */
          fprintf(gnuplotDataFile, "\n");
          fprintf(gnuplotTimesFile, "\n");
        }
    }
 conclude:
  addSummaryConclusion(summaryFile, verbose, verboseSummaryFile,
                       FSPECSTR, maxRelErr, maxUlpErr,
                       minTime, maxTime, maxTimeNormal, flagsOK);

  /* Finally, write a gnuplot driver file which plots the results */
  /* ERRORGRAPHTYPE is 1 for PostScript, 0 for window */
  if (writeGnuplot3DDriver(test_string,
                           "Testing and timing function " FSPECSTR,
                           ERRORGRAPHTYPE, maxUlpErr, maxX, maxY))
    retVal = 1;

  closeResultsFiles(gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile);

 done:
  return retVal;
}


/* Test a single pair of special arguments */
void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, REAL *argy,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr,
                  REAL *maxX, REAL *maxY,
                  FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL z, reldiff, ulps;
  int *xmp, *ymp, *result_pow, *zmp;
  int ifail, digs;
  unsigned int observedFlags;
  int observedErrno;
  REAL x, y;
  char *dec;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));
  memcpy(&y, argy, sizeof(REAL));

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  zmp = new_mp(params);
  result_pow = new_mp(params);

  printf("Testing %s for special arguments (x,y) =\n"
         "  (%30.20e, %30.20e)\n", FSPECSTR, x, y);
  printf("  (x = %s", real2hex(&x));
  printf(", y = %s)\n", real2hex(&y));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);
  DTOMP(y, ymp, params, &ifail);

  /* See what we think the result should be */
  MPPOW(xmp, ymp, params, result_pow, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
  z = FNAME(x,y);
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the result */
  reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_pow, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
      *maxY = y;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  dec = MP2DEC(result_pow, params, digs, &ifail);
  addSummarySpecial2(summaryFile, verbose, verboseSummaryFile,
                     FSPECSTR, x, y, z, reldiff, ulps, dec,
                     flagsOK, checkInexact,
                     expectedFlags, observedFlags,
                     expectedErrno, observedErrno);

  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of pow(x,y):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      mapleX(verboseSummaryFile, y, "y", base, mantis);
      fprintf(verboseSummaryFile, " z := x^y;\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "     at x  = %65.54e\n"
             "    and y  = %65.54e\n"
             "  %s = %40.29e\n",
             reldiff, ulps, x, y, FSPECSTR, z);
      DTOMP(z, zmp, params, &ifail);
      MPSHOWBIN("           = ", zmp, params, mantis, stdout);
      printf("  expected =     %s\n", dec);
      MPSHOWBIN("           = ", result_pow, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(zmp);
  free(result_pow);
}


/* Test a single value of x with positive and negative integer y values */
void test_integral_y(fp_params params,
                     int base, int mantis, int emin, int emax,
                     REAL a, REAL b, int nX, int integralX,
                     int verbose, REAL *maxRelErr, REAL *maxUlpErr,
                     REAL *maxX, REAL *maxY,
                     FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL x, y, z, h, reldiff, savx, savy, savz, maxreldiff, ulps, maxulps;
  int *xmp, *ymp, *result_pow;
  int i, j, ifail, digs, nY;
  char line[512];
  char *dec;

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  result_pow = new_mp(params);

  nY = mantis + 1;

  sprintf(line,
          "\n"
          "Testing %s for %d arguments\n"
          "    x in range [%16.12e, %16.12e]\n"
          "    and %d exact integer arguments y.\n",
          FSPECSTR, nX, a, b, nY);

  printf("%s", line);
  fprintf(summaryFile, "%s", line);
  if (verbose >= 1)
    fprintf(verboseSummaryFile, "%s", line);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;
  savz = 0.0;

  h = (b - a) / nX;

  for (i = 0; i < nX; i++)
    {
      if (integralX)
        /* Integral x values */
        x = (REAL)((int)a + i);
      else
        /* Non-integral x values */
        x = (REAL)(a + (i + myrand()) * h);

      ifail = 0;
      DTOMP(x, xmp, params, &ifail);

      y = 1.0;
      for (j = 0; j < nY; j++)
        {
          DTOMP(y, ymp, params, &ifail);
          MPPOW(xmp, ymp, params, result_pow, &ifail);
          z = FNAME(x,y);
          reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_pow, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              savz = z;
            }

          /* Test also with negative y */
          y = -y;
          DTOMP(y, ymp, params, &ifail);
          MPPOW(xmp, ymp, params, result_pow, &ifail);
          z = FNAME(x,y);
          reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_pow, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              savz = z;
            }

          y = -y;
          /* Every time round the loop y gets another random base
             "base" digit added, always remaining an integer. */
          y = y * base + (int)(myrand() * base);
        }
    }

  sprintf(line, "Maximum relative difference = %g  (%g ulps)\n",
          maxreldiff, maxulps);
  fprintf(summaryFile, "%s", line);
  if (verbose >= 1)
    fprintf(verboseSummaryFile, "%s", line);
  if (verbose >= 3)
    printf("%s", line);
  if (maxreldiff > 0.0)
    {
      sprintf(line, "      at x  = %65.54e\n"
              "     and y  = %65.54e\n"
              "   %s = %40.29e\n",
              savx, savy, FSPECSTR, savz);
      fprintf(summaryFile, "%s", line);
      fprintf(verboseSummaryFile, "%s", line);
      if (verbose >= 3)
        printf("%s", line);

      DTOMP(savx, xmp, params, &ifail);
      DTOMP(savy, ymp, params, &ifail);
      MPPOW(xmp, ymp, params, result_pow, &ifail);
      digs = (int)(0.301 * 2 * mantis - 1);
      dec = MP2DEC(result_pow, params, digs, &ifail);
      sprintf(line, "   expected =     %s\n", dec);
      printf("%s", line);
      fprintf(summaryFile, "%s", line);
      if (verbose >= 3)
        fprintf(verboseSummaryFile, "%s", line);

      if (verbose >= 2)
        {
          /* Output a small piece of Maple code to compute the result,
             as a check value. */
          fprintf(verboseSummaryFile, "# Maple code to check computation of pow(x,y):\n");
          mapleX(verboseSummaryFile, savx, "x", base, mantis);
          mapleX(verboseSummaryFile, savy, "y", base, mantis);
          fprintf(verboseSummaryFile, " z := x^y;\n");
        }
    }

  if (maxulps > *maxUlpErr)
    {
      *maxRelErr = maxreldiff;
      *maxUlpErr = maxulps;
      *maxX = savx;
      *maxY = savy;
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(result_pow);
}

/* Test nX by nY arguments in the ranges [a,b] x [c,d]. X and y are
 * random but spread equally across the ranges.
 */
void test_interval(fp_params params, int base, int mantis, int emin, int emax,
                   REAL a, REAL b, int nX,
                   REAL c, REAL d, int nY,
                   int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr,
                   REAL *maxX, REAL *maxY,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal)
{
  REAL x = 0.0, y = 0.0, z, reldiff, savx, savy, savz,
    maxreldiff, ulps, maxulps;
  int *xmp, *ymp, *result_pow;
  int i, j, ifail, digs;
  char line[512];
  char *dec;
  REAL cycles, logba, logdc;

  /* Time pow(x,y) with x x log-distributed in interval [a,b] and
     y near the mid-point of the interval [c,d] */

  {
    REAL cyc, dt, x=0.0, y=0.0, logba, snormal;
    volatile REAL z=0.0;
    MY_ULARGE_INTEGER t1, t2;
    int i, j, normal;

    createSmallestNormal(0, emin, &snormal);

    if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0)))
      logba = mylog(b/a);
    else
      logba = 0.0;
    y = mynextafter((c + d) / 2, c, mantis);

    normal = 1;
    cycles = 0.0;
    if (nTimes > 0)
      {
	for (j = 0; j < NTRIALS; j++)
	  {
	    x = logdist2(j, NTRIALS, a, b, logba);
            /* Check whether x or y is denormal */
            if ((x != 0.0 && -snormal < x && x < snormal) ||
                (y != 0.0 && -snormal < y && y < snormal))
              normal = 0;
	    createInfinity(0, &cyc);
	    for (i = 0; i < nTimes; i++)
	      {
		t1 = RDTSC();
		z = FNAME(x,y);
		t2 = RDTSC();
		dt = DIFTSC(t1, t2);
		if (dt < cyc) cyc = dt;
	      }
	    cyc -= overhead;
	    if (amean) cycles += cyc;
	    else cycles += mylog(cyc);
	  }
	if (amean) cycles /= NTRIALS;
	else cycles = myexp(cycles/NTRIALS);
      }

    if (*minTime == 0.0 || cycles < *minTime) *minTime = cycles;
    if (cycles > *maxTime) *maxTime = cycles;
    if (normal && cycles > *maxTimeNormal) *maxTimeNormal = cycles;
  }

  /* Test accuracy and print results */

  sprintf(line,
          "Testing %d arguments x in range [%16.12e, %16.12e]\n"
          "    and %d arguments y in range [%16.12e, %16.12e]\n",
          nX, a, b, nY, c, d);
  printf("%s", line);
  if (verbose >= 1)
    fprintf(verboseSummaryFile, "%s", line);

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  result_pow = new_mp(params);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;
  savz = 0.0;

  if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0)))
    logba = mylog(b/a);
  else
    logba = 0.0;

  if (c != 0.0 && d != 0.0 && ((c < 0.0) == (d < 0.0)))
    logdc = mylog(d/c);
  else
    logdc = 0.0;

  for (j = 0; j < nY; j++)
    {
      /* y is logarithmically distributed in [c,d] */
      y = logdist(j, nY, c, d, logdc);

      for (i = 0; i < nX; i++)
        {
          /* x is logarithmically distributed in [a,b] */
          x = logdist(i, nX, a, b, logba);

          /* Randomly negate the argument y. Don't negate x because most
             negative x arguments just lead to a NaN expected return value. */
          if (myrand() > 0.5)
            y = -y;

          ifail = 0;
          DTOMP(x, xmp, params, &ifail);
          DTOMP(y, ymp, params, &ifail);

          MPPOW(xmp, ymp, params, result_pow, &ifail);
          z = FNAME(x,y);
          reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_pow, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              savz = z;
            }
        }
    }


  if (verbose >= 1)
    {
      sprintf(line, "Maximum relative difference = %g  (%g ulps)\n",
              maxreldiff, maxulps);
      fprintf(verboseSummaryFile, "%s", line);
      if (verbose >= 3)
        printf("%s", line);
      if (maxreldiff > 0.0)
        {
          sprintf(line, "      at x  = %65.54e\n"
                  "     and y  = %65.54e\n"
                  "   %s = %40.29e\n",
                  savx, savy, FSPECSTR, savz);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          DTOMP(savy, ymp, params, &ifail);
          MPPOW(xmp, ymp, params, result_pow, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          dec = MP2DEC(result_pow, params, digs, &ifail);
          sprintf(line, "   expected =     %s\n", dec);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of pow(x,y):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              mapleX(verboseSummaryFile, savy, "y", base, mantis);
              fprintf(verboseSummaryFile, " z := x^y;\n");
            }
        }
    }

  /* Add an entry to the accuracy file for gnuplot. Use a dummy small
     value rather than zero so that gnuplot has something to work with */
  if (maxulps > 0.0)
    fprintf(gnuplotDataFile, "%g %g %g\n", b, d, maxulps);
  else
    fprintf(gnuplotDataFile, "%g %g %g\n", b, d, 0.1);
  fflush(gnuplotDataFile);

  if (verbose >= 1 && nTimes > 0)
    {
      sprintf(line, "Timing %d calls of %s\n", nTimes, FSPECSTR);
      fprintf(verboseSummaryFile, "%s", line);
      if (verbose >= 3)
        printf("%s", line);
      sprintf(line, "Each call averaged %6.1f cycles.\n", cycles);
      fprintf(verboseSummaryFile, "%s", line);
      if (verbose >= 3)
        printf("%s", line);
    }

  /* Add an entry to the timing results file for gnuplot */
  fprintf(gnuplotTimesFile, "%g %g %6.1f\n", b, d, cycles);
  fflush(gnuplotTimesFile);

  /* Add information to the summary file */
  addSummaryInterval2(summaryFile, verbose, verboseSummaryFile,
                      FSPECSTR, nX*nY, a, b, c, d,
                      maxreldiff, maxulps, savx, savy,
                      cycles);

  if (maxulps > *maxUlpErr)
    {
      *maxRelErr = maxreldiff;
      *maxUlpErr = maxulps;
      *maxX = savx;
      *maxY = savy;
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(result_pow);
}
