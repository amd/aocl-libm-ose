#include "test_sqrt.h"

int main(int argc, char *argv[])
{

  const char *test_string = "test_" OUTFILEEXTSTR;

  FILE *gnuplotDataFile;
  FILE *gnuplotTimesFile;
  FILE *summaryFile;
  FILE *verboseSummaryFile;

  const REAL half = (REAL)0.5;
  const REAL log2 = (REAL)0.69314718055994530941723212;
  int verbose = 2;

  int nTests, nTimes, amean, specialTest, forcePrecision;
  int base, mantis, emin, emax;
  int i, k, retVal = 0, elim;
  int flagsOK = 1;
  unsigned int expectedFlags = 0;
  int expectedErrno = 0;
  int checkInexact = 0;
  fp_params params;
  REAL x, a, b, maxRelErr, maxUlpErr, maxX, minTime, maxTime,
         maxTimeNormal, overhead;

  /* Examine command line arguments */
  parseArgs(argc, argv, &nTests, &nTimes, &amean, &specialTest,
            &forcePrecision);

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
      addSummaryTitle(summaryFile, verbose, verboseSummaryFile,FSPECSTR);
      fprintf(summaryFile, "Overhead = %5.1f\n", overhead);

      maxRelErr = 0.0;
      maxUlpErr = 0.0;
      maxX = 0.0;
      minTime = 0.0;
      maxTime = 0.0;
      maxTimeNormal = 0.0;

      if (specialTest)
        {
          int nr;
          verbose = 3;
          x = 0.0;
          while (x != -111.0)
            {
              printf("\nInput x, or a and b for interval [a,b] (-111 to finish)\n");
              fflush(stdout);
              nr = get2Reals(base, mantis, emin, emax, &a, &b);
              x = a;
              if (x != -111.0)
                {
                  if (nr == 1)
                    test_special(params, base, mantis, emin, emax, &x, 0,
                                 checkInexact, 0, 0, verbose,
                                 &maxRelErr, &maxUlpErr, &maxX,
                                 summaryFile, verboseSummaryFile);
                  else if (nr >= 2)
                    test_interval(params, base, mantis, emin, emax, a, b,
                                  nTests, nTimes, amean, overhead, verbose,
                                  gnuplotDataFile, gnuplotTimesFile,
                                  summaryFile, verboseSummaryFile,
                                  &maxRelErr, &maxUlpErr, &maxX,
                                  &minTime, &maxTime, &maxTimeNormal);
                }
            }
          goto conclude;
        }

      /* Test positive zero */
      createZero(0, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test negative zero */
      createZero(1, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test positive infinity */
      createInfinity(0, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test negative infinity */
      createInfinity(1, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test quiet NaN */
      createNaN(0, 0, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test signalling NaN */
      createNaN(0, 1, &x);
      expectedFlags = AMD_F_INVALID;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      x = 1;
      expectedFlags = AMD_F_INEXACT;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      x = half;
      expectedFlags = AMD_F_INEXACT;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test largest finite floating-point number */
      createLargest(0, mantis, emax, &x);
      expectedFlags = AMD_F_INEXACT | AMD_F_OVERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* And its negative */
      createLargest(1, mantis, emax, &x);
      expectedFlags = AMD_F_INEXACT | AMD_F_UNDERFLOW;
      expectedErrno = ERANGE;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      createLargest(0, mantis, emax, &x);

      /* Choose elim such that exp(2.0**elim) ~= large */
      elim = (int)(mylog(mylog(x)) / log2 + 1);

      /* Random tests in intervals spread across the whole range */

      /* Negative arguments */
      for (i = elim-1; i >= -elim*2; i--)
        {
          a = -mypow(2.0, i + 1);
          b = -mypow(2.0, i);
          test_interval(params, base, mantis, emin, emax, a, b,
                        nTests, nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX,
                        &minTime, &maxTime, &maxTimeNormal);
        }

      /* Near zero */
      a = -mypow(2.0, -elim*2);
      createSmallestNormal(1, emin, &b);
      test_interval(params, base, mantis, emin, emax, a, b,
                    nTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);

      /* Negative denormalized numbers */
      createSmallestNormal(1, emin, &a);
      createSmallestDenormal(1, mantis, emin, &b);
      test_interval(params, base, mantis, emin, emax, a, b,
                    nTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);
      /* Positive denormalized numbers */
      createSmallestDenormal(0, mantis, emin, &a);
      createSmallestNormal(0, emin, &b);
      test_interval(params, base, mantis, emin, emax, a, b,
                    nTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);
      createSmallestNormal(0, emin, &a);
      b = mypow(2.0, -elim*2);
      test_interval(params, base, mantis, emin, emax, a, b,
                    nTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);

      /* And positive arguments */
      for (i = -elim*2; i < elim; i++)
        {
          a = mypow(2.0, i);
          b = mypow(2.0, i + 1);
          test_interval(params, base, mantis, emin, emax, a, b,
                        nTests, nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX,
                        &minTime, &maxTime, &maxTimeNormal);
        }
    }
 conclude:
  addSummaryConclusion(summaryFile, verbose, verboseSummaryFile,
                       FSPECSTR, maxRelErr, maxUlpErr,
                       minTime, maxTime, maxTimeNormal, flagsOK);

  /* Finally, write a gnuplot driver file which plots the results */
  /* ERRORGRAPHTYPE is 1 for PostScript, 0 for window */
  if (writeGnuplotDriver(test_string,
                         "Testing and timing function " FSPECSTR,
                         0, /* 1 means use log scale for x */
                         ERRORGRAPHTYPE, maxUlpErr, maxX))
    retVal = 1;

  closeResultsFiles(gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile);

 done:
  return retVal;
}


/* Test a single special argument */
void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                  FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL y, reldiff, ulps;
  int *xmp, *result_sqrt, *ymp;
  int ifail, digs;
  unsigned int observedFlags;
  int observedErrno;
  REAL x;
  char *dec;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  result_sqrt = new_mp(params);

  printf("Testing %s for special argument x = %30.20e\n", FSPECSTR, x);
  printf("(x = %s)\n", real2hex(&x));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);

  /* See what we think the result should be */
  MPROOT(xmp, result_sqrt, params, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
  y = FNAME(x);
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the result */
  reldiff = MPRELDIFF(y, base, mantis, emin, emax, result_sqrt, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  dec = MP2DEC(result_sqrt, params, digs, &ifail);
  addSummarySpecial(summaryFile, verbose, verboseSummaryFile,
                    FSPECSTR, x, y, reldiff, ulps, dec,
                    flagsOK, checkInexact,
                    expectedFlags, observedFlags,
                    expectedErrno, observedErrno);

  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of exp(x):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      fprintf(verboseSummaryFile, " y := exp(x);\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "     at x  = %65.54e\n"
             "    %s = %40.29e\n",
             reldiff, ulps, x, FSPECSTR, y);
      DTOMP(y, ymp, params, &ifail);
      MPSHOWBIN("           = ", ymp, params, mantis, stdout);
      printf("  expected =     %s\n", dec);
      MPSHOWBIN("           = ", result_sqrt, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(result_sqrt);
}


/* Test n arguments in the range [a,b]. The arguments are
 * random but spread equally across the range.
 */
void test_interval(fp_params params, int base, int mantis, int emin, int emax,
                   REAL a, REAL b, int n, int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal)
{
  REAL x = 0.0, y, reldiff, savx, savy, maxreldiff, ulps, maxulps;
  int *xmp, *result_sqrt;
  int i, ifail, digs;
  char line[512];
  char *dec;
  REAL cycles, logba;

  /* Time exp(x) in the interval [a,b] */

  {
    REAL cyc, dt, x=0.0, logba, snormal;
    volatile REAL y=0.0;
    MY_ULARGE_INTEGER t1, t2;
    int i, j, normal;

    createSmallestNormal(0, emin, &snormal);

    if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0)))
      logba = mylog(b/a);
    else
      logba = 0.0;

    normal = 1;
    cycles = 0.0;
    if (nTimes > 0)
      {
	for (j = 0; j < NTRIALS; j++)
	  {
	    x = logdist2(j, NTRIALS, a, b, logba);
            /* Check whether x is denormal */
            if (x != 0.0 && -snormal < x && x < snormal)
              normal = 0;
	    createInfinity(0, &cyc);
	    for (i = 0; i < nTimes; i++)
	      {
		t1 = RDTSC();
		y = FNAME(x);
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

  sprintf(line, "Testing %d arguments in range [%16.12e, %16.12e]\n",
          n, a, b);
  printf("%s", line);
  if (verbose >= 1)
    fprintf(verboseSummaryFile, "%s", line);

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  result_sqrt = new_mp(params);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;

  if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0)))
    logba = mylog(b/a);
  else
    logba = 0.0;

  for (i = 0; i < n; i++)
    {
      /* x is logarithmically distributed in [a,b] */
      x = logdist(i, n, a, b, logba);

      ifail = 0;
      DTOMP(x, xmp, params, &ifail);

      MPROOT(xmp, result_sqrt, params, &ifail);
      y = FNAME(x);
      reldiff = MPRELDIFF(y, base, mantis, emin, emax, result_sqrt, params,
                          &ulps, &ifail);

      if (ulps > maxulps)
        {
          maxreldiff = reldiff;
          maxulps = ulps;
          savx = x;
          savy = y;
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
          sprintf(line, "     at x  = %65.54e\n"
                  "    %s = %40.29e\n",
                  savx, FSPECSTR, savy);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          MPROOT(xmp, result_sqrt, params, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          dec = MP2DEC(result_sqrt, params, digs, &ifail);
          sprintf(line, "  expected =     %s\n", dec);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of exp(x):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              fprintf(verboseSummaryFile, " y := exp(x);\n");
            }
        }
    }

  /* Add an entry to the accuracy file for gnuplot. Use a dummy small
     value rather than zero so that gnuplot has something to work with */
  if (maxulps > 0.0)
    fprintf(gnuplotDataFile, "%g %g\n", b, maxulps);
  else
    fprintf(gnuplotDataFile, "%g %g\n", b, 0.1);
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
  fprintf(gnuplotTimesFile, "%g %6.1f\n", b, cycles);
  fflush(gnuplotTimesFile);

  /* Add information to the summary file */
  addSummaryInterval(summaryFile, verbose, verboseSummaryFile,
                     FSPECSTR, n, a, b, maxreldiff, maxulps, savx,
                     cycles);

  if (maxulps > *maxUlpErr)
    {
      *maxRelErr = maxreldiff;
      *maxUlpErr = maxulps;
      *maxX = savx;
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(result_sqrt);
}