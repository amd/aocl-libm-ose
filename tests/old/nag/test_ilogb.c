#include "test_ilogb.h"

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
  int i, k, retVal = 0;
  int flagsOK = 1;
  unsigned int expectedFlags = 0;
  int expectedErrno = 0;
  int checkInexact = 1;
  fp_params params;
  REAL x, a, b, maxRelErr, maxUlpErr, maxX, minTime, maxTime,
       maxTimeNormal, overhead;

  /* Examine command line arguments */
  parseArgs(argc, argv, &nTests, &nTimes, &amean, &specialTest,
            &forcePrecision);

  nLargeMagnitudeTests = nTests / 10;

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
                    test_special(params, emin, &x, 0,
                                 checkInexact, 0, 0, verbose,
                                 &maxRelErr, &maxUlpErr, &maxX,
                                 summaryFile, verboseSummaryFile);
                  else if (nr >= 2)
                    test_interval(params, mantis, emin, a, b,
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
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test negative zero */
      createZero(1, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test positive infinity */
      createInfinity(0, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test negative infinity */
      createInfinity(1, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test quiet NaN */
      createNaN(0, 0, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test signalling NaN */
      /* N.B. no invalid signal expected even though it's a signalling NaN
         (see IEEE 754-1985) */
      createNaN(0, 1, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test 1.0 */
      x = 1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test -1.0 */
      x = -1.0;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test largest finite floating-point number */
      createLargest(0, mantis, emax, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test largest negative finite floating-point number */
      createLargest(1, mantis, emax, &x);
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, emin, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Random tests in intervals spread across the whole range */


      /* Random tests in intervals containing numbers of increasingly
         large magnitude */

      /* Test denormalized numbers */
      createSmallestDenormal(0, mantis, emin, &a);
      createSmallestNormal(0, emin, &b);
      test_interval(params, mantis, emin, a, b,
                    nLargeMagnitudeTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);

      /* Test fewer values of smaller magnitude */
      for (i = emin; i < -60; i += 50)
        {
          a = mypow(2.0, i);
          b = mypow(2.0, i + 50);
          test_interval(params, mantis, emin, a, b,
                        nLargeMagnitudeTests, nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX,
                        &minTime, &maxTime, &maxTimeNormal);
        }

      a = mypow(2.0, i);
      b = mypow(2.0, -10);
      test_interval(params, mantis, emin, a, b,
                    nTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);

      for (i = -10; i < 15; i++)
        {
          a = mypow(2.0, i);
          b = mypow(2.0, i + 1);
          test_interval(params, mantis, emin, a, b,
                        nTests, nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX,
                        &minTime, &maxTime, &maxTimeNormal);
        }

      /* Test fewer values of larger magnitude */
      /* These intervals take us up almost to the overflow threshold */
      for (i = 15; i < emax - 8; i += 8)
        {
          a = mypow(2.0, i);
          b = mypow(2.0, i + 8);
          test_interval(params, mantis, emin, a, b,
                        nLargeMagnitudeTests, nTimes, amean, overhead, verbose,
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
                         1, /* 1 means use log scale for x */
                         ERRORGRAPHTYPE, maxUlpErr, maxX))
    retVal = 1;

  closeResultsFiles(gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile);

 done:
  return retVal;
}


/* Test a single special argument */
void test_special(fp_params params, int emin,
                  REAL *argx, unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                  FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL reldiff, ulps;
  int *xmp;
  int ifail;
  unsigned int observedFlags;
  int observedErrno;
  REAL x;
  int y, result_ilogb, OK;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));

  /* Create a floating-point number in the floating-point system */
  xmp = new_mp(params);

  printf("Testing %s for special argument x = %30.20e\n", FSPECSTR, x);
  printf("(x = %s)\n", real2hex(&x));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);

  /* See what we think the result should be */
  result_ilogb = MPILOGB(xmp, emin, params, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
  y = FNAME(x);
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the result */
  if (y != result_ilogb)
    createInfinity(0, &reldiff);
  else
    reldiff = 0.0;
  ulps = reldiff;

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
    }

  fprintf(summaryFile, "Testing %s for special argument x = %30.20e\n",
    FSPECSTR, x);
  fprintf(summaryFile, "(x = %s)\n", real2hex(&x));
  fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
          "     at x  = %65.54e\n"
          " %s = %d\n",
          reldiff, ulps, x, FSPECSTR, y);

  fprintf(summaryFile, "  expected =     %d\n", result_ilogb);

  if (verbose >= 1)
    {
      fprintf(verboseSummaryFile, "Testing %s for special argument x = %30.20e\n",
              FSPECSTR, x);
      fprintf(verboseSummaryFile, "(x = %s)\n", real2hex(&x));
      fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
              "     at x  = %65.54e\n"
              " %s = %d\n",
              reldiff, ulps, x, FSPECSTR, y);

      fprintf(verboseSummaryFile, "  expected =     %d\n", result_ilogb);
      fprintf(verboseSummaryFile, "\n");
    }

  /* Compare the observed flags with expected flags */
  if (flagsOK)
    {
      /* If there's an output argument to set, set it */
      OK = compare_status(expectedFlags, observedFlags,
                          checkInexact,
                          expectedErrno, observedErrno,
                          verbose,
                          summaryFile, verboseSummaryFile);
      *flagsOK = *flagsOK && OK;
    }
  else
    {
      /* Otherwise just print the flags that got raised */
      printf("Flags raised: ");
      show_status(observedFlags, stdout);
      fprintf(summaryFile, "Flags raised: ");
      show_status(observedFlags, summaryFile);
      if (verbose)
        {
          fprintf(verboseSummaryFile, "Flags raised: ");
          show_status(observedFlags, verboseSummaryFile);
        }
    }

  /* Clean up */
  free(xmp);
}


/* Test n arguments in the range [a,b]. The arguments are
 * random but spread equally across the range.
 */
void test_interval(fp_params params, int mantis, int emin,
                   REAL a, REAL b, int n, int nTimes, int amean,
                   REAL overhead, int verbose,
                   FILE *gnuplotDataFile, FILE *gnuplotTimesFile,
                   FILE *summaryFile, FILE *verboseSummaryFile,
                   REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                   REAL *minTime, REAL *maxTime, REAL *maxTimeNormal)
{
  REAL x = 0.0, reldiff, savx, maxreldiff, ulps, maxulps;
  int *xmp;
  int y, my, result_ilogb, savy;
  int i, ifail, badSymmetry;
  char line[512];
  REAL cycles, logba;

  /* Time ilogb(x) in the interval [a,b] */
  {
    REAL cyc, dt, x=0.0, logba, snormal;
    MY_ULARGE_INTEGER t1, t2;
    int i, j, normal;
    volatile int k;

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
                /* The next line is designed to ensure that
                   high optimization levels cannot move the
                   function call out of the timing loop. */

                x = mynextafter(x, x, mantis);

		t1 = RDTSC();
		k = FNAME(x);
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

  /* Create a floating-point number in the floating-point system */
  xmp = new_mp(params);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0;
  badSymmetry = 0;

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

      result_ilogb = MPILOGB(xmp, emin, params, &ifail);
      y = FNAME(x);
      my = FNAME(-x);

      if (y != result_ilogb)
        createInfinity(0, &reldiff);
      else
        reldiff = 0.0;
      ulps = reldiff;

      if (ulps > maxulps)
        {
          maxreldiff = reldiff;
          maxulps = ulps;
          savx = x;
          savy = y;
        }

      if (my != y)
        {
          /* Yuk: ilogb(-x) is not equal to ilogb(x) */
          if (!badSymmetry)
            {
              sprintf(line,
                      "**** WARNING: %s(-x) not equal to %s(x) ****\n"
                      "      at x  = %65.54e\n"
                      "      %s(x) = %d\n"
                      "     %s(-x) = %d\n",
                      FNAMESTR, FNAMESTR, x, FNAMESTR, y, FNAMESTR, my);
              printf("%s", line);
              fprintf(summaryFile, "%s", line);
              if (verbose >= 1)
                fprintf(verboseSummaryFile, "%s", line);
            }
          badSymmetry = 1;
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
                  " %s = %d\n",
                  savx, FSPECSTR, savy);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          result_ilogb = MPILOGB(xmp, emin, params, &ifail);
          sprintf(line, "  expected =     %d\n", result_ilogb);
          if (verbose >= 3)
            fprintf(verboseSummaryFile, "%s", line);
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
}
