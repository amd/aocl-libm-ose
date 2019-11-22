#include "test_remquo.h"

#ifdef WALL_TIME_FOR_TIMING

#include <sys/time.h>

#undef NTRIALS
#define NTRIALS 1
#define MY_LEN 2048
#define MY_GHZ 2.7

#endif

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
  int checkInexact = 1;
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

      /* Test (+0.0, +0.0) */
      createZero(0, &x);
      createZero(0, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+0.0, -0.0) */
      createZero(0, &x);
      createZero(1, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, +inf) */
      createInfinity(0, &x);
      createInfinity(0, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, +inf) */
      createInfinity(1, &x);
      createInfinity(0, &y);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, +1.0) */
      createInfinity(0, &x);
      y = 1.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, -1.0) */
      createInfinity(0, &x);
      y = -1.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, inf) */
      x = 1.0;
      createInfinity(0, &y);
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

      /* Test (+lambda, 0.0) */
      createLargest(0, mantis, emax, &x);
      y = 0.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, -lambda) */
      y = 1.0;
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
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-lambda, +lambda) */
      createLargest(1, mantis, emax, &x);
      createLargest(0, mantis, emax, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-2.0, 3.0) */
      x = -2.0;
      y = 3.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-2.0, -3.0) */
      x = -2.0;
      y = -3.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Test (-2.0, -3.5) */
      x = -2.0;
      y = -3.5;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);

      /* Tests with abs(nearest_integer(x/y) - x/y) == 0.5,
         to check that the sign of the result is correct. */
      x = 3.0;
      y = 2.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      x = 5.0;
      y = 2.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      x = 2.5;
      y = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);
      x = 3.5;
      y = 1.0;
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
              nY = mysqrt(mysqrt(nLargeMagnitudeTests));
            }
          else if (j == 1)
            {
              /* Small y arguments */
              c = mypow(2.0, emin);
              d = mypow(2.0, emin + 50);
              nY = mysqrt(mysqrt(nLargeMagnitudeTests));
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
              nX = mysqrt(mysqrt(nLargeMagnitudeTests));
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
  int *xmp, *ymp, *result_rem, *result_quo, *zmp;
  int ifail, digs, quo, q, qdiff;
  unsigned int observedFlags;
  int observedErrno;
  REAL x, y;
  char *dec, *decq;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));
  memcpy(&y, argy, sizeof(REAL));

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  zmp = new_mp(params);
  result_rem = new_mp(params);
  result_quo = new_mp(params); 

  printf("Testing %s for special arguments (x,y) =\n"
         "  (%30.20e, %30.20e)\n", FSPECSTR, x, y);
  printf("  (x = %s", real2hex(&x));
  printf(", y = %s)\n", real2hex(&y));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);
  DTOMP(y, ymp, params, &ifail);
  quo = 0;
  ITOMP(quo, result_quo, params, &ifail);

  /* See what we think the result should be */
  MPREMQUO(xmp, ymp, result_quo, params, result_rem, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
  z = FNAME(x,y, &q);
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the two results */
  reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_rem, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
      *maxY = y;
    }

  qdiff = MPTOI(result_quo, params, &ifail);
  if (qdiff != q)
    {
      *maxRelErr = qdiff - q;
      *maxUlpErr = qdiff - q;
      *maxX = x;
      *maxY = y;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  dec  = MP2DEC(result_rem, params, digs, &ifail);
  decq = MP2DEC(result_quo, params, digs, &ifail);
  addSummarySpecial2(summaryFile, verbose, verboseSummaryFile,
                     FSPECSTR, x, y, z, reldiff, ulps, dec,
                     flagsOK, checkInexact,
                     expectedFlags, observedFlags,
                     expectedErrno, observedErrno);

  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of remquo(x,y):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      mapleX(verboseSummaryFile, y, "y", base, mantis);
      fprintf(verboseSummaryFile, " z := x - y * trunc(x/y);\n");
	  fprintf(verboseSummaryFile, " q := trunc(x/y);\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "          at x  = %65.54e\n"
             "         and y  = %65.54e\n"
             "             %s = %d, %40.29e\n",
             reldiff, ulps, x, y, FSPECSTR, q, z);
      DTOMP(z, zmp, params, &ifail);
      MPSHOWBIN("  actual   rem = ", zmp, params, mantis, stdout);
      printf("  expected rem =     %s\n", dec);
	  MPSHOWBIN("  (rem binary) = ", result_rem, params, mantis, stdout);
      ITOMP(q, zmp, params, &ifail);
      MPSHOWBIN("  actual   quo = ", zmp, params, mantis, stdout);
      printf("  expected quo =     %s\n", decq);
      MPSHOWBIN("  (quo binary) = ", result_quo, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(zmp);
  free(result_quo);
  free(result_rem);
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
  int *xmp, *ymp, *result_rem, *result_quo;
  int i, j, ifail, digs, badSymmetry;
  char line[512];
  char *dec, *decq;
  REAL cycles, logba, logdc;
  int savq, mq1, mq2, mq3, q=0;

  /* Time remquo(x,y) x log-distributed in in the interval [a,b] and
     y near the mid-point of the interval [c,d] */

  {
    REAL cyc, dt, x=0.0, y=0.0, logba, snormal;
    volatile REAL z=0.0;

#ifndef WALL_TIME_FOR_TIMING
    MY_ULARGE_INTEGER t1, t2;
#endif

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

#ifndef WALL_TIME_FOR_TIMING
              t1 = RDTSC();
              z = FNAME(x,y, &q);
              t2 = RDTSC();
              dt = DIFTSC(t1, t2);
#else
              {
                  int iii;
                  struct timeval my_tval;
                  double my_start, my_stop, my_elapse;

                  gettimeofday(&my_tval, NULL);
                  my_start = (double)(my_tval.tv_sec * 1E6 + my_tval.tv_usec);
                  for(iii=0; iii<=MY_LEN; iii++)
                  {
                      z = FNAME(x,y);
                  }
                  gettimeofday(&my_tval, NULL);
                  my_stop = (double)(my_tval.tv_sec * 1E6 + my_tval.tv_usec);
                  my_elapse = (my_stop - my_start);

                  dt = (my_elapse*MY_GHZ*1000.0)/((double)MY_LEN);
              }
#endif

		if (dt < cyc) cyc = dt;
	      }

#ifndef WALL_TIME_FOR_TIMING
        cyc -= overhead;
#else
        *(&overhead) = overhead;
#endif

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
  result_quo = new_mp(params);
  result_rem = new_mp(params);

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

          /* Randomly negate the arguments x and y */
          if (myrand() > 0.5)
            x = -x;
          if (myrand() > 0.5)
            y = -y;

          ifail = 0;
          DTOMP(x, xmp, params, &ifail);
          DTOMP(y, ymp, params, &ifail);
		  ITOMP(q, result_quo, params, &ifail);

          MPREMQUO(xmp, ymp, result_quo, params, result_rem, &ifail);
          z = FNAME(x,y, &q);
          mz1 = FNAME(-x,y, &mq1);
          mz2 = FNAME(x,-y, &mq2);
          mz3 = FNAME(-x,-y, &mq3);
          reldiff = MPRELDIFF(z, base, mantis, emin, emax, result_rem, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              savz = z;
			  savq = q;
            }

          if (mz1 != -z || mz2 != z || mz3 != -z || 
			  mq1 != -q || mq2 !=-q || mq3 != q)
            {
              /* Yuk: changing the sign of x or y gave a different result */
              if (!badSymmetry)
                {
                  sprintf(line,
                          "**** WARNING: %s(x,y) not equal to +-%s(+-x, +-y) ****\n"
                          "        at x  = %65.54e\n"
                          "       and y  = %65.54e\n"
                          "      %s(x,y) = %d, %40.29e\n"
                          "     %s(-x,y) = %d, %40.29e\n"
                          "     %s(x,-y) = %d, %40.29e\n"
                          "    %s(-x,-y) = %d, %40.29e\n",
                          FNAMESTR, FNAMESTR, x, y, FNAMESTR, q, z, FNAMESTR,
                          mq1, mz1, FNAMESTR, mq2, mz2, FNAMESTR, mq3, mz3);
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
          sprintf(line, "           at x  = %65.54e\n"
                  "          and y  = %65.54e\n"
                  "              %s = %d, %40.29e\n",
                  savx, savy, FSPECSTR, savq, savz);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          DTOMP(savy, ymp, params, &ifail);
		  ITOMP(savq, result_quo, params, &ifail);
          MPREMQUO(xmp, ymp, result_quo, params, result_rem, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          dec  = MP2DEC(result_rem, params, digs, &ifail);
		  decq = MP2DEC(result_quo, params, digs, &ifail);
		  sprintf(line, "   expected quo =     %s\n", decq);
		  fprintf(verboseSummaryFile, "%s", line);
          sprintf(line, "   expected rem =     %s\n", dec);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of remquo(x,y):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              mapleX(verboseSummaryFile, savy, "y", base, mantis);
              fprintf(verboseSummaryFile, " z := x - y * trunc(x/y);\n");
			  fprintf(verboseSummaryFile, " q := trunc(x/y);\n");
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
  free(result_quo);
  free(result_rem);
}
