#include "test_cexp.h"

#ifdef WALL_TIME_FOR_TIMING

#include <sys/time.h>

#undef NTRIALS
#define NTRIALS 1
#define MY_LEN 4096 
#define MY_GHZ 2.7

#endif

int main(int argc, char *argv[])
{

  const char *test_string = "test_" OUTFILEEXTSTR;

  FILE *gnuplotDataFile;
  FILE *gnuplotTimesFile;
  FILE *summaryFile;
  FILE *verboseSummaryFile;

  const REAL pi = (REAL)3.1415926535897932384626433833;
  const REAL piby2 = (REAL)1.5707963267948966192313216916;
  const REAL piby6 = (REAL)0.5235987755982988730771072305;
  const REAL log2 = (REAL)0.69314718055994530941723212;
  int verbose = 2;

  int nTests, nLargeMagnitudeTests, nTimes, amean, specialTest, forcePrecision;
  int base, mantis, emin, emax, elim;
  int i, k, j, nX, nY, retVal = 0;
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

      /* Test (1, 3.3) */
      x = (REAL)1.0;
      y = (REAL)3.3;
      expectedFlags = 0;
      expectedErrno = 0;
      test_special(params, base, mantis, emin, emax, &x, &y,
                   expectedFlags, checkInexact, expectedErrno, &flagsOK,
                   verbose, &maxRelErr, &maxUlpErr, &maxX, &maxY,
                   summaryFile, verboseSummaryFile);


      /* Random tests in intervals spread across the whole range */

      createLargest(0, mantis, emax, &x);

      /* Choose elim such that exp(2.0**elim) ~= large */
      elim = (int)(mylog(mylog(x)) / log2 + 1);

      for(j = 0; j < 5; j++)
      {
        if(j == 0)
        {
            c = -piby6;
            d = piby6; 
        }
        if(j == 1) 
        {
            c = -piby2;
            d = piby2; 
        }
        if(j == 2)
        {
            c = piby6 * mypow(2.0, -10);
            d = pi * mypow(2.0, 9);
        }
        if(j == 3)
        {
            c = piby6 * mypow(2.0, 9);
            d = pi * mypow(2.0, 60);
        } 
        if(j == 4)
        {
            c = -(piby2/2.0)-0.01; 
            d = (piby2/2.0)+0.01; 
        } 
        nY = mysqrt(nTests);
            
        nX = mysqrt(nTests);
        /* Negative arguments for x*/
        for (i = elim-1; i >= -elim*2; i--)
          {
            a = -mypow(2.0, i + 1);
            b = -mypow(2.0, i);
            test_interval(params, base, mantis, emin, emax,
                          a, b, nX,
                          c, d, nY,
                          nTimes, amean, overhead, verbose,
                          gnuplotDataFile, gnuplotTimesFile,
                          summaryFile, verboseSummaryFile,
                          &maxRelErr, &maxUlpErr, &maxX, &maxY,
                          &minTime, &maxTime, &maxTimeNormal);
          }

        nX = nTests/100; if(nX < 10) nX = 10;

        /* Near zero */
        a = -mypow(2.0, -elim*2);
        createSmallestNormal(1, emin, &b);
        test_interval(params, base, mantis, emin, emax,
                      a, b, nX,
                      c, d, nY,
                      nTimes, amean, overhead, verbose,
                      gnuplotDataFile, gnuplotTimesFile,
                      summaryFile, verboseSummaryFile,
                      &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      &minTime, &maxTime, &maxTimeNormal);
        /* Negative denormalized numbers */
        createSmallestNormal(1, emin, &a);
        createSmallestDenormal(1, mantis, emin, &b);
        test_interval(params, base, mantis, emin, emax,
                      a, b, nX,
                      c, d, nY,
                      nTimes, amean, overhead, verbose,
                      gnuplotDataFile, gnuplotTimesFile,
                      summaryFile, verboseSummaryFile,
                      &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      &minTime, &maxTime, &maxTimeNormal);
        /* Positive denormalized numbers */
        createSmallestDenormal(0, mantis, emin, &a);
        createSmallestNormal(0, emin, &b);
        test_interval(params, base, mantis, emin, emax,
                      a, b, nX,
                      c, d, nY,
                      nTimes, amean, overhead, verbose,
                      gnuplotDataFile, gnuplotTimesFile,
                      summaryFile, verboseSummaryFile,
                      &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      &minTime, &maxTime, &maxTimeNormal);
        createSmallestNormal(0, emin, &a);
        b = mypow(2.0, -elim*2);
        test_interval(params, base, mantis, emin, emax,
                      a, b, nX,
                      c, d, nY,
                      nTimes, amean, overhead, verbose,
                      gnuplotDataFile, gnuplotTimesFile,
                      summaryFile, verboseSummaryFile,
                      &maxRelErr, &maxUlpErr, &maxX, &maxY,
                      &minTime, &maxTime, &maxTimeNormal);

        nX = mysqrt(nTests);
        /* Positive arguments for x*/
        /* Ideally the upper bound of i in the loop should be elim, but we chose elim-1
            to account for deficiency in cexp code. The problem arises when exp is
            slightly more than 2^1024 and the cos or sin is 2^t where t is -ve and
            the resulting mul will produce less than 2^1024 and that is representable */
        for (i = -elim*2; i < elim-1; i++) 
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

typedef struct AMD_LIBM_COMPLEX_S { REAL val[2]; } AMD_LIBM_COMPLEX;

/* Test a single pair of special arguments */
void test_special(fp_params params, int base, int mantis, int emin, int emax,
                  REAL *argx, REAL *argy,
                  unsigned int expectedFlags, int checkInexact,
                  int expectedErrno, int *flagsOK, int verbose,
                  REAL *maxRelErr, REAL *maxUlpErr,
                  REAL *maxX, REAL *maxY,
                  FILE *summaryFile, FILE *verboseSummaryFile)
{
  REAL z_re, z_im, reldiff, ulps;
  int *xmp, *ymp, *result_re, *result_im;
  int *vmp;
  int ifail, digs;
  unsigned int observedFlags;
  int observedErrno;
  REAL x, y;
  char *decRe, *decIm;

    AMD_LIBM_COMPLEX ip_t, op_t;
    REAL complex ip, op;

  /* Use memcpy instead of assignment to avoid conversion of
     signalling to quiet NaN by some versions of gcc. */

  memcpy(&x, argx, sizeof(REAL));
  memcpy(&y, argy, sizeof(REAL));

  /* Create some floating-point numbers in the floating-point system */
  xmp = new_mp(params);
  ymp = new_mp(params);
  vmp = new_mp(params);
  result_re = new_mp(params);
  result_im = new_mp(params);

  printf("Testing %s for special arguments (x,y) =\n"
         "  (%30.20e, %30.20e)\n", FSPECSTR, x, y);
  printf("  (x = %s", real2hex(&x));
  printf(", y = %s)\n", real2hex(&y));

  ifail = 0;
  DTOMP(x, xmp, params, &ifail);
  DTOMP(y, ymp, params, &ifail);

  /* See what we think the result should be */
  MPCEXP(xmp, ymp, params, result_re, result_im, &ifail);

  /* Clear any exception flags currently raised */
  clear_fpsw();
  errno = 0;
  /* Call the function being tested */
   
    memcpy(&(ip_t.val[0]), &x, sizeof(REAL));
    memcpy(&(ip_t.val[1]), &y, sizeof(REAL));
    memcpy(&ip, &ip_t, sizeof(AMD_LIBM_COMPLEX));
    op = FNAME(ip);
    memcpy(&op_t, &op, sizeof(AMD_LIBM_COMPLEX));
    memcpy(&z_re, &(op_t.val[0]), sizeof(REAL));
    memcpy(&z_im, &(op_t.val[1]), sizeof(REAL));
 
  /* See what flags were raised, if any */
  observedFlags = get_sw_flags();
  observedErrno = errno;

  /* Compute the error in the two results */
  reldiff = MPRELDIFF(z_re, base, mantis, emin, emax, result_re, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
      *maxY = y;
    }

  reldiff = MPRELDIFF(z_im, base, mantis, emin, emax, result_im, params,
                      &ulps, &ifail);

  if (ulps > *maxUlpErr)
    {
      *maxRelErr = reldiff;
      *maxUlpErr = ulps;
      *maxX = x;
      *maxY = y;
    }

  digs = (int)(0.301 * 2 * mantis - 1);
  decRe = strdup(MP2DEC(result_re, params, digs, &ifail));
  decIm = strdup(MP2DEC(result_im, params, digs, &ifail));
    
  addSummarySpecialCexp(summaryFile, verbose, verboseSummaryFile,
                          FSPECSTR, x, y, z_re, z_im, *maxRelErr, *maxUlpErr, decRe, decIm,
                          flagsOK, checkInexact,
                          expectedFlags, observedFlags,
                          expectedErrno, observedErrno);
  if (verbose >= 2)
    {
      /* Output a small piece of Maple code to compute the result,
         as a check value. */
      fprintf(verboseSummaryFile, "# Maple code to check computation of cexp(w):\n");
      mapleX(verboseSummaryFile, x, "x", base, mantis);
      mapleX(verboseSummaryFile, y, "y", base, mantis);
      fprintf(verboseSummaryFile, " z_re := cexp_re(x,y);\n");
      fprintf(verboseSummaryFile, " z_im := cexp_im(x,y);\n");
    }

  if (verbose >= 3)
    {
      printf("Relative difference = %g  (%g ulps)\n"
             "     at x  = %65.54e\n"
             "     at y  = %65.54e\n"
             "    %s returned z_re = %40.29e\n"
             "            and returned z_im = %40.29e\n",
             *maxRelErr, *maxUlpErr, x, y, FSPECSTR, z_re, z_im);
        printf("\n");
      DTOMP(z_re, vmp, params, &ifail);
      printf("  expected z_re =   %s\n", decRe);
      MPSHOWBIN("  expected z_re = ", result_re, params, mantis, stdout);
      MPSHOWBIN("    actual z_re = ", vmp, params, mantis, stdout);
        printf("\n");
      DTOMP(z_im, vmp, params, &ifail);
      printf("  expected z_im =   %s\n", decIm);
      MPSHOWBIN("  expected z_im = ", result_im, params, mantis, stdout);
      MPSHOWBIN("    actual z_im = ", vmp, params, mantis, stdout);
    }

  if (verbose >= 1)
    fprintf(verboseSummaryFile, "\n");

  /* Clean up */
  free(decRe);
  free(decIm);
  free(xmp);
  free(ymp);
  free(vmp);
  free(result_re);
  free(result_im);
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
  REAL x = 0.0, y = 0.0, z_re, z_im, reldiff, savx, savy, sav_z_re, sav_z_im,
    maxreldiff, ulps, maxulps;
  int *xmp, *ymp, *result_re, *result_im;
  int i, j, ifail, digs;
  char line[512];
  char *decRe, *decIm;
  REAL cycles, logba, logdc;

    AMD_LIBM_COMPLEX ip_t, op_t;
    REAL complex ip, op;

  /* Time cexp(w) with x x log-distributed in interval [a,b] and
     y near the mid-point of the interval [c,d] */

  {
    REAL cyc, dt, x=0.0, y=0.0, logba, snormal;
    volatile REAL complex op;

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

        memcpy(&(ip_t.val[0]), &x, sizeof(REAL));
        memcpy(&(ip_t.val[1]), &y, sizeof(REAL));
        memcpy(&ip, &ip_t, sizeof(AMD_LIBM_COMPLEX));

	    createInfinity(0, &cyc);
	    for (i = 0; i < nTimes; i++)
	      {

#ifndef WALL_TIME_FOR_TIMING
              t1 = RDTSC();
		      op = FNAME(ip);
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
		              op = FNAME(ip);
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
  result_re = new_mp(params);
  result_im = new_mp(params);

  maxreldiff = 0.0;
  maxulps = 0.0;
  savx = 0.0;
  savy = 0.0;
  sav_z_re = 0.0;
  sav_z_im = 0.0;

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

          MPCEXP(xmp, ymp, params, result_re, result_im, &ifail);

          memcpy(&(ip_t.val[0]), &x, sizeof(REAL));
          memcpy(&(ip_t.val[1]), &y, sizeof(REAL));
          memcpy(&ip, &ip_t, sizeof(AMD_LIBM_COMPLEX));
          op = FNAME(ip);
          memcpy(&op_t, &op, sizeof(AMD_LIBM_COMPLEX));
          memcpy(&z_re, &(op_t.val[0]), sizeof(REAL));
          memcpy(&z_im, &(op_t.val[1]), sizeof(REAL));

          reldiff = MPRELDIFF(z_re, base, mantis, emin, emax, result_re, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              sav_z_re = z_re;
              sav_z_im = z_im;
            }

          reldiff = MPRELDIFF(z_im, base, mantis, emin, emax, result_im, params,
                              &ulps, &ifail);

          if (ulps > maxulps)
            {
              maxreldiff = reldiff;
              maxulps = ulps;
              savx = x;
              savy = y;
              sav_z_re = z_re;
              sav_z_im = z_im;
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
                  "   z_re = %40.29e\n"
                  "   z_im = %40.29e\n",
                  savx, savy, sav_z_re, sav_z_im);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          DTOMP(savx, xmp, params, &ifail);
          DTOMP(savy, ymp, params, &ifail);
          MPCEXP(xmp, ymp, params, result_re, result_im, &ifail);
          digs = (int)(0.301 * 2 * mantis - 1);
          decRe = strdup(MP2DEC(result_re, params, digs, &ifail));
          decIm = strdup(MP2DEC(result_im, params, digs, &ifail));
          sprintf(line, "   expected z_re =     %s\n", decRe);
          fprintf(verboseSummaryFile, "%s", line);
          sprintf(line, "   expected z_im =     %s\n", decIm);
          fprintf(verboseSummaryFile, "%s", line);
          if (verbose >= 3)
            printf("%s", line);

          if (verbose >= 2)
            {
              /* Output a small piece of Maple code to compute the result,
                 as a check value. */
              fprintf(verboseSummaryFile, "# Maple code to check computation of cexp(w):\n");
              mapleX(verboseSummaryFile, savx, "x", base, mantis);
              mapleX(verboseSummaryFile, savy, "y", base, mantis);
              fprintf(verboseSummaryFile, " z_re := cexp_re(x,y);\n");
              fprintf(verboseSummaryFile, " z_im := cexp_im(x,y);\n");
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
  free(decRe);
  free(decIm);
  free(xmp);
  free(ymp);
  free(result_re);
  free(result_im);
}

