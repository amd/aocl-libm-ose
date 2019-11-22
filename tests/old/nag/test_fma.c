#include "test_fma.h"

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
  int i, k, j, nX, nY, nZ, imin, retVal = 0;
  int flagsOK = 1;
  unsigned int expectedFlags = 0;
  int expectedErrno = 0;
  int checkInexact = 0;
  fp_params params;
  REAL x, y, z, a, b, c, d, e, f, maxRelErr, maxUlpErr,
         maxX, maxY, maxZ, minTime, maxTime, maxTimeNormal, overhead;

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
      maxZ = 0.0;
      minTime = 0.0;
      maxTime = 0.0;
      maxTimeNormal = 0.0;

      if (specialTest)
        {
          int nr;
          verbose = 3;
          nX = nY = nZ = mysqrt(nTests);
          x = 0.0;
          while (x != -111.0)
            {
              printf("\nInput x, y, z, or a, b, c, d, e, f"
                     " for interval [a,b][c,d][e,f] (-111 to finish)\n");
              fflush(stdout);
              nr = get6Reals(base, mantis, emin, emax, &a, &b, &c, &d, &e, &f);
              x = a;
              y = b;
              z = c;
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
                  if (nr == 3)
                    test_special(params, base, mantis, emin, emax, &x, &y, &z,
                                 0,
                                 checkInexact, 0, 0, verbose, &maxRelErr, &maxUlpErr,
                                 &maxX, &maxY, &maxZ,
                                 summaryFile, verboseSummaryFile);
                  else if (nr >= 6)
                    test_interval(params, base, mantis, emin, emax,
                                  a, b, nX,
                                  c, d, nY,
                                  e, f, nZ,
                                  nTimes, amean, overhead, verbose,
                                  gnuplotDataFile, gnuplotTimesFile,
                                  summaryFile, verboseSummaryFile,
                                  &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                                  &minTime, &maxTime, &maxTimeNormal);
                }
            }
          goto conclude;
        }

      /* Test (+0.0, +0.0, +0.0) */
      createZero(0, &x);
      createZero(0, &y);
      createZero(0, &z);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (+0.0, -0.0, +0.0) */
      createZero(0, &x);
      createZero(1, &y);
      createZero(0, &y);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, +inf, +inf) */
      createInfinity(0, &x);
      createInfinity(0, &y);
      createInfinity(0, &z);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, +inf, +inf) */
      createInfinity(1, &x);
      createInfinity(0, &y);
      createInfinity(0, &z);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (-inf, +inf, -inf) */
      createInfinity(1, &x);
      createInfinity(0, &y);
      createInfinity(1, &z);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (+inf, +1.0, +1.0) */
      createInfinity(0, &x);
      y = 1.0;
      z = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, -inf, 1.0) */
      x = 1.0;
      createInfinity(1, &y);
      z = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (QNaN, 1.0, 1.0) */
      createNaN(0, 0, &x);
      y = 1.0;
      z = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (SNaN, 1.0, 1.0) */
      createNaN(0, 1, &x);
      y = 1.0;
      z = 1.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.0, QNaN, 1.0) */
      x = -1.0;
      createNaN(0, 0, &y);
      z = -1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (-1.0, SNaN, 1.0) */
      x = -1.0;
      createNaN(0, 1, &y);
      z = -1.0;
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, 1.0, QNaN) */
      x = 1.0;
      y = 1.0;
      createNaN(0, 0, &z);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, 1.0, SNaN) */
      x = 1.0;
      y = 1.0;
      createNaN(0, 1, &z);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (+lambda, 0.0, 0.0) */
      createLargest(0, mantis, emax, &x);
      y = 0.0;
      z = 0.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (1.0, -lambda, 0.0) */
      x = 1.0;
      createLargest(1, mantis, emax, &y);
      z = 0.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (+lambda, 1.0, +lambda) */
      createLargest(0, mantis, emax, &x);
      y = 1.0;
      createLargest(0, mantis, emax, &z);
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                   summaryFile, verboseSummaryFile);

      /* Test (-lambda, +lambda, 0.0) */
      createLargest(1, mantis, emax, &x);
      createLargest(0, mantis, emax, &y);
      z = 0.0;
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y, &z,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
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
          /* Use same range for z as for x */
          e = a;
          f = b;
          nZ = 3;
          test_interval(params, base, mantis, emin, emax,
                        a, b, nX,
                        c, d, nY,
                        e, f, nZ,
                        nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                        &minTime, &maxTime, &maxTimeNormal);

          /* Small x arguments */
          imin = ((emin + 1) / 50) * 50 - 10;
          a = mypow(2.0, emin);
          b = mypow(2.0, imin);
          nX = mysqrt(nLargeMagnitudeTests);
          /* Use same range for z as for x */
          e = a;
          f = b;
          nZ = 3;
          test_interval(params, base, mantis, emin, emax,
                        a, b, nX,
                        c, d, nY,
                        e, f, nZ,
                        nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                        &minTime, &maxTime, &maxTimeNormal);
          for (i = imin; i <= -60; i+=50)
            {
              a = mypow(2.0, i);
              b = mypow(2.0, i + 50);
              nX = mysqrt(nLargeMagnitudeTests);
              /* Use same range for z as for x */
              e = a;
              f = b;
              nZ = 3;
              test_interval(params, base, mantis, emin, emax,
                            a, b, nX,
                            c, d, nY,
                            e, f, nZ,
                            nTimes, amean, overhead, verbose,
                            gnuplotDataFile, gnuplotTimesFile,
                            summaryFile, verboseSummaryFile,
                            &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                            &minTime, &maxTime, &maxTimeNormal);
            }

          /* Medium size x arguments */
          for (i = -10; i < 10; i++)
            {
              a = mypow(2.0, i);
              b = mypow(2.0, i + 1);
              nX = mysqrt(nTests);
              /* Use same range for z as for x */
              e = a;
              f = b;
              nZ = 3;
              test_interval(params, base, mantis, emin, emax,
                            a, b, nX,
                            c, d, nY,
                            e, f, nZ,
                            nTimes, amean, overhead, verbose,
                            gnuplotDataFile, gnuplotTimesFile,
                            summaryFile, verboseSummaryFile,
                            &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                            &minTime, &maxTime, &maxTimeNormal);
            }

          /* Large x arguments */
          for (i = 10; i < emax-50; i+=50)
            {
              a = mypow(2.0, i);
              b = mypow(2.0, i + 50);
              nX = mysqrt(nLargeMagnitudeTests);
              /* Use same range for z as for x */
              e = a;
              f = b;
              nZ = 3;
              test_interval(params, base, mantis, emin, emax,
                            a, b, nX,
                            c, d, nY,
                            e, f, nZ,
                            nTimes, amean, overhead, verbose,
                            gnuplotDataFile, gnuplotTimesFile,
                            summaryFile, verboseSummaryFile,
                            &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
                            &minTime, &maxTime, &maxTimeNormal);
            }
          a = mypow(2.0, i);
          createLargest(0, mantis, emax, &b);
          nX = mysqrt(nLargeMagnitudeTests);
          /* Use same range for z as for x */
          e = a;
          f = b;
          nZ = 3;
          test_interval(params, base, mantis, emin, emax,
                        a, b, nX,
                        c, d, nY,
                        e, f, nZ,
                        nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX, &maxY, &maxZ,
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
                  REAL *argx, REAL *argy, REAL *argz,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr,
                  REAL *maxX, REAL *maxY, REAL *maxZ,
                  FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL w, reldiff, ulps;
  int *wmp, *xmp, *ymp, *result_fma, *zmp;
  int ifail, digs;
  unsigned int observedFlags;
  int observedErrno;
  REAL x, y, z ;
  char *dec;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));
  memcpy(&y, argy, sizeof(REAL));
  memcpy(&z, argz, sizeof(REAL));

  /* Create some floating-point numbers in the floating-point system */
  wmp = new_mp(params);
  xmp = new_mp(params);
  ymp = new_mp(params);
  zmp = new_mp(params);
  result_fma = new_mp(params);

  printf("Testing %s for special arguments (x,y,z) =\n"
         "  (%30.20e, %30.20e, %30.20e)\n", FSPECSTR, x, y, z);
  printf("  (x = %s", real2hex(&x));
  printf(", y = %s", real2hex(&y));
  printf(", z = %s)\n", real2hex(&z));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);
  DTOMP(y, ymp, params, &ifail);
  DTOMP(z, zmp, params, &ifail);

  /* See what we think the result should be */
  MPFMA(xmp, ymp, zmp, result_fma, params, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
  w = FNAME(x, y, z);
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the result */
  reldiff = MPRELDIFF(w, base, mantis, emin, emax, result_fma, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
      *maxY = y;
      *maxZ = z;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  dec = MP2DEC(result_fma, params, digs, &ifail);
  addSummarySpecial3(summaryFile, verbose, verboseSummaryFile,
                     FSPECSTR, x, y, z, w, reldiff, ulps, dec,
                     flagsOK, checkInexact,
                     expectedFlags, observedFlags,
                     expectedErrno, observedErrno);

  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of fma(x,y,z):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      mapleX(verboseSummaryFile, y, "y", base, mantis);
      mapleX(verboseSummaryFile, z, "z", base, mantis);
      fprintf(verboseSummaryFile, " w := x * y + z;\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "     at x  = %65.54e\n"
             "        y  = %65.54e\n"
             "    and z  = %65.54e\n"
             "        %s = %40.29e\n",
             reldiff, ulps, x, y, z, FSPECSTR, w);
      DTOMP(w, wmp, params, &ifail);
      MPSHOWBIN("           = ", wmp, params, mantis, stdout);
      printf("  expected =     %s\n", dec);
      MPSHOWBIN("           = ", result_fma, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(wmp);
  free(xmp);
  free(ymp);
  free(zmp);
  free(result_fma);
}


/* Test nX by nY by nZ arguments in the ranges [a,b] x [c,d] x [e,f].
 * X, y and z are random but spread equally across the ranges.
 */
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
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal)
{
  REAL w, x = 0.0, y = 0.0, z = 0.0, mw1, mw2,
    reldiff, savw, savx, savy, savz, maxreldiff, ulps, maxulps;
  int *xmp, *ymp, *zmp, *result_fma;
  int i, i2, j, ifail, digs, badSymmetry;
  char line[512];
  char *dec;
  REAL cycles, logba, logdc, logfe;

  /* Time fma(x,y,z) with x log-distributed in the interval [a,b],
     and y and z near the mid-points of the intervals [c,d], [e,f] */

  {
    REAL cyc, dt, x=0.0, logba, y=0.0, z=0.0, snormal;
    volatile REAL w=0.0;
    MY_ULARGE_INTEGER t1, t2;
    int i, j, normal;

    createSmallestNormal(0, emin, &snormal);

    if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0)))
      logba = mylog(b/a);
    else
      logba = 0.0;
    y = mynextafter((c + d) / 2, c, mantis);
    z = mynextafter((e + f) / 2, f, mantis);

    normal = 1;
    cycles = 0.0;
    if (nTimes > 0)
      {
	for (j = 0; j < NTRIALS; j++)
	  {
	    x = logdist2(j, NTRIALS, a, b, logba);
            /* Check whether x, y or z is denormal */
            if ((x != 0.0 && -snormal < x && x < snormal) ||
                (y != 0.0 && -snormal < y && y < snormal) ||
                (z != 0.0 && -snormal < z && z < snormal))
              normal = 0;
	    createInfinity(0, &cyc);
	    for (i = 0; i < nTimes; i++)
	      {
		t1 = RDTSC();
		w = FNAME(x,y,z);
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
          "    and %d arguments y in range [%16.12e, %16.12e]\n"
          "    and %d arguments z in range [%16.12e, %16.12e]\n",
          nX, a, b, nY, c, d, nZ, e, f);
  printf("%s", line);
  if (verbose >= 1)
    fprintf(verboseSummaryFile, "%s", line);

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  zmp = new_mp(params);
  result_fma = new_mp(params);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;
  savz = 0.0;
  savw = 0.0;
  badSymmetry = 0;

  if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0)))
    logba = mylog(b/a);
  else
    logba = 0.0;

  if (c != 0.0 && d != 0.0 && ((c < 0.0) == (d < 0.0)))
    logdc = mylog(d/c);
  else
    logdc = 0.0;

  if (e != 0.0 && f != 0.0 && ((e < 0.0) == (f < 0.0)))
    logfe = mylog(f/e);
  else
    logfe = 0.0;

  for (j = 0; j < nY; j++)
    {
      /* y is logarithmically distributed in [c,d] */
      y = logdist(j, nY, c, d, logdc);

      for (i = 0; i < nX; i++)
        {
          /* x is logarithmically distributed in [a,b] */
          x = logdist(i, nX, a, b, logba);

          for (i2 = 0; i2 < nZ; i2++)
            {
              /* z is logarithmically distributed in [e,f] */
              z = logdist(i2, nZ, e, f, logfe);

              ifail = 0;
              DTOMP(x, xmp, params, &ifail);
              DTOMP(y, ymp, params, &ifail);
              DTOMP(z, zmp, params, &ifail);

              MPFMA(xmp, ymp, zmp, result_fma, params, &ifail);
              w = FNAME(x, y, z);
              mw1 = -FNAME(-x, y, -z);
              mw2 = -FNAME(x, -y, -z);
              reldiff = MPRELDIFF(w, base, mantis, emin, emax, result_fma, params,
                                  &ulps, &ifail);

              if (ulps > maxulps)
                {
                  maxreldiff = reldiff;
                  maxulps = ulps;
                  savx = x;
                  savy = y;
                  savz = z;
                  savw = w;
                }

              if (mw1 != w || mw2 != w)
                {
                  /* Yuk: changing the sign of (x or y) and z gave a different result */
                  if (!badSymmetry)
                    {
                      sprintf(line,
                              "**** WARNING: %s(x,y,z) not equal to %s(+-x, +-y, -z) ****\n"
                              "        at x  = %65.54e\n"
                              "           y  = %65.54e\n"
                              "       and z  = %65.54e\n"
                              "    %s(x,y,z) = %40.29e\n"
                              "  %s(-x,y,-z) = %40.29e\n"
                              "  %s(x,-y,-z) = %40.29e\n",
                              FNAMESTR, FNAMESTR, x, y, z, FNAMESTR, w,
                              FNAMESTR, mw1, FNAMESTR, mw2);
                      printf("%s", line);
                      fprintf(summaryFile, "%s", line);
                      if (verbose >= 1)
                        fprintf(verboseSummaryFile, "%s", line);
                    }
                  badSymmetry = 1;
                }
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
                  "         y  = %65.54e\n"
                  "     and z  = %65.54e\n"
                  "         %s = %40.29e\n",
                  savx, savy, savz, FSPECSTR, savw);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          DTOMP(savy, ymp, params, &ifail);
          DTOMP(savz, zmp, params, &ifail);
          MPFMA(xmp, ymp, zmp, result_fma, params, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          dec = MP2DEC(result_fma, params, digs, &ifail);
          sprintf(line, "   expected =     %s\n", dec);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of fma(x,y,z):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              mapleX(verboseSummaryFile, savy, "y", base, mantis);
              mapleX(verboseSummaryFile, savz, "z", base, mantis);
              fprintf(verboseSummaryFile, " w := x * y + z;\n");
            }
        }
    }

  /* Add an entry to the accuracy file for gnuplot. Use a dummy small
     value rather than zero so that gnuplot has something to work with.
     The gnuplot files don't get told about the z range (because
     gnuplot can't draw four-dimensional graphs). */
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
  addSummaryInterval3(summaryFile, verbose, verboseSummaryFile,
                      FSPECSTR, nX*nY*nZ, a, b, c, d, e, f,
                      maxreldiff, maxulps, savx, savy, savz,
                      cycles);

  if (maxulps > *maxUlpErr)
    {
      *maxRelErr = maxreldiff;
      *maxUlpErr = maxulps;
      *maxX = savx;
      *maxY = savy;
      *maxZ = savz;
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(zmp);
  free(result_fma);
}
