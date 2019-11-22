#include "test_hypot.h"

int main(int argc, char *argv[])
{

  const char *test_string = "test_" OUTFILEEXTSTR;
  const REAL roottwo = (REAL)1.4142135623730950488016887242097;

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
  REAL x, y, a, b, c, d, xx, maxRelErr, maxUlpErr, maxX, maxY, infin,
         minTime, maxTime, maxTimeNormal, overhead;

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

      /* Test (+0.0, +0.0) */
      createZero(0, &x);
      createZero(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+0.0, -0.0) */
      createZero(0, &x);
      createZero(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, +inf) */
      createInfinity(0, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, +inf) */
      createInfinity(1, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, +1.0) */
      createInfinity(0, &x);
      y = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, -inf) */
      x = 1.0;
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, 1.0) */
      createNaN(0, 0, &x);
      y = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, 1.0) */
      createNaN(0, 1, &x);
      y = 1.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.0, QNaN) */
      x = -1.0;
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.0, SNaN) */
      x = -1.0;
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, inf) */
      createNaN(0, 0, &x);
      createInfinity(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, inf) */
      createNaN(0, 1, &x);
      createInfinity(0, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, -inf) */
      createNaN(0, 0, &x);
      createInfinity(1, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, -inf) */
      createNaN(0, 1, &x);
      createInfinity(1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, QNaN) */
      createInfinity(0, &x);
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (inf, SNaN) */
      createInfinity(0, &x);
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, QNaN) */
      createInfinity(1, &x);
      createNaN(0, 0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, SNaN) */
      createInfinity(1, &x);
      createNaN(0, 1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+lambda, 0.0) */
      createLargest(0, mantis, emax, &x);
      y = 0.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, -lambda) */
      y = 1.0;
      createLargest(1, mantis, emax, &y);
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = ERANGE;
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

      /* Test (-lambda, +lambda) */
      createLargest(1, mantis, emax, &x);
      createLargest(0, mantis, emax, &y);
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test numbers very close to (-lambda/sqrt(2), +lambda/sqrt(2))
         - the result should be approximately the largest number. */
      createLargest(0, mantis, emax, &xx);
      xx /= roottwo;
      createInfinity(0, &infin);
      xx = mynextafter(xx, -infin, mantis);
      xx = mynextafter(xx, -infin, mantis);
      xx = mynextafter(xx, -infin, mantis);

      x = mynextafter(-xx, -infin, mantis);
      y = mynextafter(xx, -infin, mantis);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      y = xx;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      y = mynextafter(xx, infin, mantis);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      x = xx;
      y = mynextafter(xx, -infin, mantis);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      y = xx;
      expectedFlags = AMD_F_INEXACT;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      y = mynextafter(xx, infin, mantis);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      x = mynextafter(-xx, infin, mantis);
      y = mynextafter(xx, -infin, mantis);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      y = xx;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      y = mynextafter(xx, infin, mantis);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
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
  int *xmp, *ymp, *result_hypot, *zmp;
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
  result_hypot = new_mp(params);

  printf("Testing %s for special arguments (x,y) =\n"
         "  (%30.20e, %30.20e)\n", FSPECSTR, x, y);
  printf("  (x = %s", real2hex(&x));
  printf(", y = %s)\n", real2hex(&y));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);
  DTOMP(y, ymp, params, &ifail);

  /* See what we think the result should be */
  MPHYPOT(xmp, ymp, result_hypot, params, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
  z = FNAME(x,y);
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the result */
  reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_hypot, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
      *maxY = y;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  dec = MP2DEC(result_hypot, params, digs, &ifail);
  addSummarySpecial2(summaryFile, verbose, verboseSummaryFile,
                     FSPECSTR, x, y, z, reldiff, ulps, dec,
                     flagsOK, checkInexact,
                     expectedFlags, observedFlags,
                     expectedErrno, observedErrno);

  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of hypot(x,y):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      mapleX(verboseSummaryFile, y, "y", base, mantis);
      fprintf(verboseSummaryFile, " z := sqrt(x*x + y*y);\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "     at x  = %65.54e\n"
             "    and y  = %65.54e\n"
             "        %s = %40.29e\n",
             reldiff, ulps, x, y, FSPECSTR, z);
      DTOMP(z, zmp, params, &ifail);
      MPSHOWBIN("           = ", zmp, params, mantis, stdout);
      printf("  expected =     %s\n", dec);
      MPSHOWBIN("           = ", result_hypot, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(zmp);
  free(result_hypot);
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
  REAL x = 0.0, y = 0.0, z, mz1, mz2, mz3,
    reldiff, savx, savy, savz, maxreldiff, ulps, maxulps;
  int *xmp, *ymp, *result_hypot;
  int i, j, ifail, digs, badSymmetry;
  char line[512];
  char *dec;
  REAL cycles, logba, logdc;

  /* Time hypot(x,y) with x log-distributed in the interval [a,b] and
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
  result_hypot = new_mp(params);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;
  savz = 0.0;
  badSymmetry = 0;

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

          ifail = 0;
          DTOMP(x, xmp, params, &ifail);
          DTOMP(y, ymp, params, &ifail);

          MPHYPOT(xmp, ymp, result_hypot, params, &ifail);
          z = FNAME(x,y);
          mz1 = FNAME(-x,y);
          mz2 = FNAME(x,-y);
          mz3 = FNAME(-x,-y);
          reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_hypot, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              savz = z;
            }

          if (mz1 != z || mz2 != z || mz3 != z)
            {
              /* Yuk: changing the sign of x or y gave a different result */
              if (!badSymmetry)
                {
                  sprintf(line,
                          "**** WARNING: %s(x,y) not equal to %s(+-x, +-y) ****\n"
                          "        at x  = %65.54e\n"
                          "       and y  = %65.54e\n"
                          "      %s(x,y) = %40.29e\n"
                          "     %s(-x,y) = %40.29e\n"
                          "     %s(x,-y) = %40.29e\n"
                          "    %s(-x,-y) = %40.29e\n",
                          FNAMESTR, FNAMESTR, x, y, FNAMESTR, z, FNAMESTR,
                          mz1, FNAMESTR, mz2, FNAMESTR, mz3);
                  printf("%s", line);
                  fprintf(summaryFile, "%s", line);
                  if (verbose >= 1)
                    fprintf(verboseSummaryFile, "%s", line);
                }
              badSymmetry = 1;
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
                  "         %s = %40.29e\n",
                  savx, savy, FSPECSTR, savz);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          DTOMP(savy, ymp, params, &ifail);
          MPHYPOT(xmp, ymp, result_hypot, params, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          dec = MP2DEC(result_hypot, params, digs, &ifail);
          sprintf(line, "   expected =     %s\n", dec);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of hypot(x,y):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              mapleX(verboseSummaryFile, savy, "y", base, mantis);
              fprintf(verboseSummaryFile, " z := sqrt(x*x + y*y);\n");
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
  free(result_hypot);
}
