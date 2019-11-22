#include "test_remainder_piby2.h"

int main(int argc, char *argv[])
{

  const char *test_string = "test_" OUTFILEEXTSTR;

  FILE *gnuplotDataFile;
  FILE *gnuplotTimesFile;
  FILE *summaryFile;
  FILE *verboseSummaryFile;

  const REAL pi = (REAL)3.1415926535897932384626433833;
  const REAL piby2 = (REAL)1.5707963267948966192313216916;
  int verbose = 2;

  int nTests, nLargeMagnitudeTests, nTimes, amean, specialTest, forcePrecision;
  int base, mantis, emin, emax;
  int i, k, ilim, retVal = 0;
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

      /* N.B. remainder_piby2 is designed to handle only positive
         normal arguments > 1.0, so we don't test NaN, infinity,
         small or negative numbers. */

      /* Test largest finite floating-point number */
      createLargest(0, mantis, emax, &x);
      expectedFlags = AMD_F_INEXACT;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test notorious cases uncovered by W. Kahan. They are
         values which are as close as possible to being exact
         multiples of pi / 2 */
#ifdef DOUBLE
      /* This one is the worst value for IEEE double precision */
      x = (REAL)5.31937264832654141671e+255; /* 0x7506ac5b262ca1ff */
#else
      /* This one is the worst value for IEEE single precision */
      x = (REAL)7.72917891945290196612e+28; /* 0x6f79be45 */
#endif
      expectedFlags = AMD_F_INEXACT;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX,
                   summaryFile, verboseSummaryFile);

      /* Test near multiples of Pi/4 */
      test_pi_multiples(params, base, mantis, emin, emax, nTests,
                        verbose, &maxRelErr, &maxUlpErr, &maxX,
                        summaryFile, verboseSummaryFile);

      /* Test the interval 1.0 .. Pi */
      a = 1.0;
      b = pi;
      test_interval(params, base, mantis, emin, emax, a, b,
                    nTests, nTimes, amean, overhead, verbose,
                    gnuplotDataFile, gnuplotTimesFile,
                    summaryFile, verboseSummaryFile,
                    &maxRelErr, &maxUlpErr, &maxX,
                    &minTime, &maxTime, &maxTimeNormal);

      /* Test more arguments around the principal interval */
      for (i = 1; i < 9; i++)
        {
          a = piby2 * mypow(2.0,i);
          b = pi * mypow(2.0,i);
          test_interval(params, base, mantis, emin, emax, a, b,
                        nTests, nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX,
                        &minTime, &maxTime, &maxTimeNormal);
        }

      /* Test fewer values of larger magnitude */
      for (i = 9; i < 60; i++)
        {
          a = piby2 * mypow(2.0,i);
          b = pi * mypow(2.0,i);
          test_interval(params, base, mantis, emin, emax, a, b,
                        nLargeMagnitudeTests, nTimes, amean, overhead, verbose,
                        gnuplotDataFile, gnuplotTimesFile,
                        summaryFile, verboseSummaryFile,
                        &maxRelErr, &maxUlpErr, &maxX,
                        &minTime, &maxTime, &maxTimeNormal);
        }

      /* These intervals take us up almost to the overflow threshold */
      ilim = (emax - 60) / 20;
      for (i = 0; i < ilim; i++)
        {
          a = pi * mypow(2.0, 60 + i * 20);
          b = pi * mypow(2.0, 80 + i * 20);
          test_interval(params, base, mantis, emin, emax, a, b,
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
void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                  FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL y, reldiff, ulps;
  int *xmp, *result, *ymp;
  int ifail, digs, region;
  unsigned int observedFlags;
  int observedErrno;
  REAL x;
  char *dec;
  /* This variable must be type double even when testing
     the float version of remainder_piby2 */
  double yy;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  result = new_mp(params);

  printf("Testing %s for special argument x = %30.20e\n", FSPECSTR, x);
  printf("(x = %s)\n", real2hex(&x));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);

  /* See what we think the result should be */
  MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;

#ifdef DOUBLE
  /* Call the function being tested */
  FNAME(x, &y, &yy, &region);
#else
  FNAME(x, &yy, &region);
  y = (REAL)yy;
#endif

  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the result */
  reldiff = MPRELDIFF(y, base, mantis, emin, emax, result, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  dec = MP2DEC(result, params, digs, &ifail);
  addSummarySpecial(summaryFile, verbose, verboseSummaryFile,
                    FSPECSTR, x, y, reldiff, ulps, dec,
                    flagsOK, checkInexact,
                    expectedFlags, observedFlags,
                    expectedErrno, observedErrno);
  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of remainder_piby2(x,&r,&region):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      fprintf(verboseSummaryFile, " w := trunc(x*2.0/Pi + 0.5);\n");
      fprintf(verboseSummaryFile, " region := mod(w, 4);\n");
      fprintf(verboseSummaryFile, " r := (x - w) * Pi/2.0;\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "     at x  = %65.54e\n"
             "        %s = %40.29e\n",
             reldiff, ulps, x, FSPECSTR, y);
      DTOMP(y, ymp, params, &ifail);
      MPSHOWBIN("           = ", ymp, params, mantis, stdout);
      printf("  expected =     %s\n", dec);
      MPSHOWBIN("           = ", result, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(ymp);
  free(result);
}

/* Test near-multiples of pi/4 */
void test_pi_multiples(fp_params params,
                       int base, int mantis, int emin, int emax,
                       int n, int verbose,
                       REAL *maxRelErr, REAL *maxUlpErr, REAL *maxX,
                       FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL x, savx, savy, reldiff, ulps, infin, xx, maxreldiff, maxulps;
  int *xmp, *result;
  int *pi, *two_pi, *two_over_pi, *pi_over_two, *pi_over_four, *ifp;
  int i, ifail, digs, region;
  char *dec;
  REAL y;
  /* This variable must be type double even when testing
     the float version of remainder_piby2 */
  double yy;

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  result = new_mp(params);
  pi = new_mp(params);
  two_pi = new_mp(params);
  pi_over_two = new_mp(params);
  two_over_pi = new_mp(params);
  pi_over_four = new_mp(params);
  ifp = new_mp(params);

  /* Create an extra-precise Pi/4 */
  ifail = 0;
  GET_PRECOMPUTED_PI_ETC(params, pi, two_pi, pi_over_two, two_over_pi, &ifail);
  ITOMP(4, ifp, params, &ifail);
  DIVIDE(pi, ifp, pi_over_four, params, &ifail);

  printf("Testing %s for %d arguments near multiples of pi/4\n", FSPECSTR, n);

  createInfinity(0, &infin);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;
  for (i = 1; i < n; i++)
    {
      /* Create XMP = (i+1) * pi/4 */
      ITOMP(i+1, ifp, params, &ifail);
      TIMES(ifp, pi_over_four, xmp, params, &ifail);

      /* Convert XMP to xx */
      ifail = 0;
      MPTOD(xmp, params, &xx, &ifail);

      /* Test three arguments, x = xx-, x = xx; x = xx+ */
      x = mynextafter(xx, -infin, mantis);
      DTOMP(x, xmp, params, &ifail);
      MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);
#ifdef DOUBLE
      FNAME(x, &y, &yy, &region);
#else
      FNAME(x, &yy, &region);
      y = (REAL)yy;
#endif
      reldiff = MPRELDIFF(y, base, mantis, emin, emax, result, params,
                          &ulps, &ifail);
      if (ulps > maxulps)
        {
          maxreldiff = reldiff;
          maxulps = ulps;
          savx = x;
          savy = y;
        }

      x = xx;
      DTOMP(x, xmp, params, &ifail);
      MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);
#ifdef DOUBLE
      FNAME(x, &y, &yy, &region);
#else
      FNAME(x, &yy, &region);
      y = (REAL)yy;
#endif
      reldiff = MPRELDIFF(y, base, mantis, emin, emax, result, params,
                          &ulps, &ifail);
      if (ulps > maxulps)
        {
          maxreldiff = reldiff;
          maxulps = ulps;
          savx = x;
          savy = y;
        }

      x = mynextafter(xx, infin, mantis);
      DTOMP(x, xmp, params, &ifail);
      MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);
#ifdef DOUBLE
      FNAME(x, &y, &yy, &region);
#else
      FNAME(x, &yy, &region);
      y = (REAL)yy;
#endif
      reldiff = MPRELDIFF(y, base, mantis, emin, emax, result, params,
                          &ulps, &ifail);
      if (ulps > maxulps)
        {
          maxreldiff = reldiff;
          maxulps = ulps;
          savx = x;
          savy = y;
        }
    }

  DTOMP(savx, xmp, params, &ifail);
  MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);
  digs = (int)(0.301 * 2 * mantis - 1);
  dec = MP2DEC(result, params, digs, &ifail);

  addSummaryPiMultiples(summaryFile, verbose, verboseSummaryFile,
                        FSPECSTR, n, savx, savy,
                        maxreldiff, maxulps, dec);

  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of remainder_piby2(x,&r,&region):\n");
      mapleX(verboseSummaryFile, savx, "x", base, mantis);
      fprintf(verboseSummaryFile, " w := trunc(x*2.0/Pi + 0.5);\n");
      fprintf(verboseSummaryFile, " region := mod(w, 4);\n");
      fprintf(verboseSummaryFile, " r := (x - w) * Pi/2.0;\n");
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(xmp);
  free(result);
  free(pi);
  free(two_pi);
  free(pi_over_two);
  free(two_over_pi);
  free(pi_over_four);
  free(ifp);

  if (maxulps > *maxUlpErr)
    {
      *maxRelErr = maxreldiff;
      *maxUlpErr = maxulps;
      *maxX = savx;
    }
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
  int *xmp, *result;
  int i, ifail, digs;
  char line[512];
  char *dec;
  REAL cycles, logba;
  int region;
  /* This variable must be type double even when testing
     the float version of remainder_piby2 */
  double yy;

  /* Time remainder_piby2(x, &r, &region) in the interval [a,b] */

  /* The double version of remainder_piby2 has two double pointer
     output arguments */

  {
    REAL cyc, dt, x=0.0, logba, snormal;
    MY_ULARGE_INTEGER t1, t2;
    int i, j, normal;
#ifdef DOUBLE
    REAL y, yy;
#else
    double y;
#endif
    int z;

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
#ifdef DOUBLE
		FNAME(x,&y,&yy,&z);
#else
		FNAME(x,&y,&z);
#endif
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
  result = new_mp(params);

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

      MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);
#ifdef DOUBLE
      FNAME(x, &y, &yy, &region);
#else
      FNAME(x, &yy, &region);
      y = (REAL)yy;
#endif
      reldiff = MPRELDIFF(y, base, mantis, emin, emax, result, params,
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
                  "      %s = %40.29e\n",
                  savx, FSPECSTR, savy);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          MPREMAINDER_PIBY2(xmp, params, result, &region, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          dec = MP2DEC(result, params, digs, &ifail);
          sprintf(line, "  expected =     %s\n", dec);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of remainder_piby2(x,&r,&region):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              fprintf(verboseSummaryFile, " w := trunc(x*2.0/Pi + 0.5);\n");
              fprintf(verboseSummaryFile, " region := mod(w, 4);\n");
              fprintf(verboseSummaryFile, " r := (x - w) * Pi/2.0;\n");
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
  free(result);
}
