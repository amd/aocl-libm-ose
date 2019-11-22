
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mparith_f.h"

static int c_1 = 1;
static int c_0 = 0;
static int c_n1 = -1;
static int c_false = 0;
static int c_true = 1;
static int c_30 = 30;
static int c_2 = 2;
static int c_3 = 3;
static int c_4 = 4;
static int c_5 = 5;
static int c_239 = 239;
static int c_16 = 16;
static int c_230 = 230;

void mpinit(int *base, int *mantis, int *emin,
            int *emax, int *round, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int nrec, sign, ierr, last, iexo;
  int i;
  int radix, b3, cs, places, radigs, bigint, verbos;
  int fptype;
  char rec[125];
  int wplaces;

  if (*base == 0)
    *base = x02bhf();
  if (*mantis == 0)
    *mantis = x02bjf();
  if (*emin == 0)
    *emin = x02bkf();
  if (*emax == 0)
    *emax = x02blf();
  bigint = x02baf();
  ierr = 0;
  nrec = 0;
  if (*base < 2 || *base > 16)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d", "BASE must be in the range 2 .. 16: BASE = ",
              *base);
    }
  else if (*mantis < 1)
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "%s %d", "MANTIS must be greater than 0: MANTIS = ",
              *mantis);
    }
  else if (*emin >= 0 || *emin < -bigint / 4)
    {
      ierr = 3;
      nrec = 1;
      i_1 = -bigint / 4;
      sprintf(rec, "%s %d %s %d", "EMIN must be in the range -1 .. ", i_1,
              ": EMIN = ", *emin);
    }
  else if (*emax <= 0 || *emax > bigint / 4)
    {
      ierr = 4;
      nrec = 1;
      i_1 = bigint / 4;
      sprintf(rec, "%s %d %s %d", "EMAX must be in the range 1 .. ", i_1,
              ": EMAX = ", *emax);
    }
  else if (-(*emin) < *mantis * 2)
    {
      ierr = 5;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "-EMIN must be greater than 2*MANTIS: EMIN = ",
              *emin, ", MANTIS = ", *mantis);
    }
  else if (*emax < *mantis * 2)
    {
      ierr = 6;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "EMAX must be greater than 2*MANTIS: EMAX = ",
              *emax, ", MANTIS = ", *mantis);
    }
  else if (*round < 1 || *round > 4)
    {
      ierr = 7;
      nrec = 1;
      sprintf(rec, "%s %d", "ROUND must be in the range 1 .. 4: ROUND = ",
              *round);
    }
  if (ierr == 0)
    {
      b3 = *base * *base * *base;
      radigs = 0;
    L20:
      if (bigint >= b3)
        {
          ++radigs;
          bigint /= b3;
          goto L20;
        }

      radix = ipower(base, radigs);
      places = (*mantis + radigs - 1) / radigs;
      i_1 = radigs * places - *mantis;
      last = ipower(base, i_1);
      sign = places + 1;
      iexo = places + 2;
      wplaces = places + 3;
      fptype = places + 4;

      /* Store the parameters in array PARAMS for communication */
      /* with other routines. */

      /* The arithmetic base */
      params[0] = *base;

      /* The number of base BASE digits in a floating-point number */
      params[1] = *mantis;

      /* The minimum exponent */
      params[2] = *emin;

      /* The maximum exponent */
      params[3] = *emax;

      /* The element of the int array representing a floating-point */
      /* number which holds the sign of the number */
      params[4] = sign;

      /* The element of the array which holds the exponent of the number */
      params[5] = iexo;

      /* The element of the array which holds the current working */
      /* number of base RADIX digits */
      params[6] = wplaces;

      /* The element of the array which holds the type of the number: */
      /* 1 = regular number */
      /* 2 = zero */
      /* 3 = infinity */
      /* 4 = NaN */
      /* The constants above are returned by functions regcon(), */
      /* zercon(), infcon(), qnancon() and snancon() respectively. */
      params[7] = fptype;

      /* The working base, a power of BASE */
      params[8] = radix;

      /* The power of BASE that makes RADIX */
      params[9] = radigs;

      /* The maximum number of base RADIX digits */
      params[10] = places;

      /* The last base RADIX digit will always be a multiple of LAST */
      params[11] = last;

      /* The rounding mode */
      params[12] = *round;

      /* Set verbos to 1 for some verbosity */
      verbos = 0;
      params[13] = verbos;

      /* Compute and store a checksum */
      cs = 0;
      for (i = 1; i <= 14; ++i)
        cs += params[i-1] % 1000;
      params[14] = cs;

      /* Tell the user how long an int array must be to hold all */
      /* the information needed to represent the floating-point number */
      *n = fptype;
    }

  *ifail = p01abf(ifail, &ierr, "MPINIT", &nrec, rec);
  return;

} /* mpinit */

/* Checks that the array PARAMS has correct check sum. */
int chksum(int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int i, cs;

  cs = 0;
  for (i = 1; i <= 14; ++i)
    cs += params[i-1] % 1000;
  ret_val = cs == params[14];
  return ret_val;
} /* chksum */

void mpinitcopy(int *params_in, int *mantis,
                int *emin, int *emax, int *n_out, int *params_out,
                int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec;
  int ierr;
  char rec[125];
  int ifl;

  /* Copies an input set of PARAMS to an output set */
  /* of PARAMS, but using new values for MANTIS, EMIN and EMAX. */
  ierr = 0;
  nrec = 0;
  if (! chksum(params_in))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_IN was not initialised by a call of MPINIT");
    }
  else
    {
      ifl = 1;
      i_1 = get_base(params_in);
      i_2 = get_round(params_in);
      mpinit(&i_1, mantis, emin, emax, &i_2, n_out, params_out,
             &ifl);
      if (ifl != 0)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "Error in call to auxiliary MPINIT");
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPINITCOPY", &nrec, rec);
  return;

} /* mpinitcopy */

int get_base(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[0];
  return ret_val;
} /* get_base */

int get_mantis(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[1];
  return ret_val;
} /* get_mantis */

int get_emin(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[2];
  return ret_val;
} /* get_emin */

int get_emax(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[3];
  return ret_val;
} /* get_emax */

int get_sign(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[4];
  return ret_val;
} /* get_sign */

int get_iexo(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[5];
  return ret_val;
} /* get_iexo */

int get_wplaces(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[6];
  return ret_val;
} /* get_wplaces */

int get_fptype(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[7];
  return ret_val;
} /* get_fptype */

int get_radix(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[8];
  return ret_val;
} /* get_radix */

int get_radigs(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[9];
  return ret_val;
} /* get_radigs */

int get_places(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[10];
  return ret_val;
} /* get_places */

int get_last(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[11];
  return ret_val;
} /* get_last */

int get_round(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[12];
  return ret_val;
} /* get_round */

int get_verbos(int *params)
{
  /* System generated locals */
  int ret_val;

  ret_val = params[13];
  return ret_val;
} /* get_verbos */

/* Returns the length of the int array needed to store a number */
/* with parameters PARAMS */
int get_mplen(int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */

  /* FPTYPE happens to be the most extreme stored value, */
  /* so that's the length we need. */

  ret_val = get_fptype(params);
  return ret_val;
} /* get_mplen */

void mptod(int *xmp, int *n, int *params,
           REAL *x, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec;
  int ierr, iexo;
  int i;
  int radix;
  REAL amult;
  int im, radigs;
  char rec[125];
  int iex;
  REAL pow, sum;
  int wplaces;

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      iexo = get_iexo(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);

      pow = (REAL) radix;

      if (mpiszero(xmp, n, params))
        {
          if (mpisneg(xmp, n, params))
            createzero(&c_1, x);
          else
            createzero(&c_0, x);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          if (mpisneg(xmp, n, params))
            createinfinity(&c_1, x);
          else
            createinfinity(&c_0, x);
        }
      else if (mpisnan(xmp, n, params))
        createnan(&c_0, &c_0, x);
      else
        {
          /* Otherwise generate the floating-point number. */
          sum = 0.0;
          /* Firstly insert the contents of XMP (1) ... XMP (PLACES) into */
          /* the floating-point number SUM, with zero exponent and correct sign. */
          if (mpisneg(xmp, n, params))
            amult = -1.0;
          else
            amult = 1.0;
          amult /= radix;
          i_1 = xmp[wplaces - 1];
          for (i = 1; i <= i_1; ++i)
            {
              sum += amult * xmp[i - 1];
              amult /= radix;
            }
          /* Now scale up or down by the exponent. */
          iex = myiabs(xmp[iexo - 1]);
          im = iex % radigs;
          iex /= radigs;
          if (xmp[iexo - 1] < 0)
            {
              i_1 = iex;
              for (i = 1; i <= i_1; ++i)
                sum /= pow;
              i_1 = im;
              for (i = 1; i <= i_1; ++i)
                sum /= base;
            }
          else
            {
              i_1 = iex;
              for (i = 1; i <= i_1; ++i)
                sum *= pow;
              i_1 = im;
              for (i = 1; i <= i_1; ++i)
                sum *= base;
            }
          *x = sum;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPTOD", &nrec, rec);
  return;

} /* mptod */

void dtomp(REAL *x, int *xmp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec, emin;
  int emax, sign, ierr, last, iexo;
  REAL zero;
  int i;
  REAL y;
  int wplac, radix;
  int carry;
  REAL recbas;
  int places, radigs;
  REAL negone;
  REAL negrbs;
  int fptype;
  int expont;
  char rec[125];
  REAL one, pow;
  int wplaces;

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      emin = get_emin(params);
      emax = get_emax(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      places = get_places(params);
      wplaces = get_wplaces(params);
      last = get_last(params);

      zero = 0.0;
      one = 1.0;
      negone = -one;
      recbas = one / base;
      negrbs = negone / base;
      pow = (REAL) radix;
      wplac = 0;

      if (diszero(x))
        {
          xmp[fptype - 1] = zercon();
          if (dispos(x))
            xmp[sign - 1] = poscon();
          else
            xmp[sign - 1] = negcon();
        }
      else if (disnan(x))
        {
          xmp[fptype - 1] = qnancon();
          xmp[sign - 1] = poscon();
        }
      else if (disinf(x))
        {
          xmp[fptype - 1] = infcon();
          if (dispos(x))
            xmp[sign - 1] = poscon();
          else
            xmp[sign - 1] = negcon();
        }
      else
        {
          xmp[fptype - 1] = regcon();

          /* Now set Y = X and put the sign of X into XMP (SIGN). */
          y = *x;
          if (y > zero)
            xmp[sign - 1] = poscon();
          else
            xmp[sign - 1] = negcon();
          expont = 0;
          /* Now scale ABS (Y) into the range 1/BASE .. 1, keeping track of the */
          /* base BASE exponent in EXPONT. In this default version of MCFTOI, */
          /* POW is equal to RADIX, and so every time we need to */
          /* multiply by POW then EXPONT must be reduced by RADIGS. */

          if (mpispos(xmp, n, params))
            {
              /* X is positive */
            L20:
              if (y < recbas)
                {
                  y *= pow;
                  expont -= radigs;
                  goto L20;
                }
            L40:
              if (y >= one)
                {
                  y /= pow;
                  expont += radigs;
                  goto L40;
                }
            L60:
              if (y < recbas)
                {
                  y *= base;
                  --expont;
                  goto L60;
                }
              /* Now ABS (Y) is in the range 1/BASE .. 1. */
              /* We obtain the successive elements of XMP by */
              /* repeated multiplication by RADIX. */
              i_1 = places;
              for (i = 1; i <= i_1; ++i)
                {
                  y = radix * y;
                  xmp[i - 1] = (int) y;
                  if (xmp[i - 1] > 0)
                    wplac = i;
                  y -= xmp[i - 1];
                }
            }
          else
            {
              /* X is negative */
            L100:
              if (y > negrbs)
                {
                  y *= pow;
                  expont -= radigs;
                  goto L100;
                }
            L120:
              if (y <= negone)
                {
                  y /= pow;
                  expont += radigs;
                  goto L120;
                }
            L140:
              if (y > negrbs)
                {
                  y *= base;
                  --expont;
                  goto L140;
                }
              i_1 = places;
              for (i = 1; i <= i_1; ++i)
                {
                  y = radix * y;
                  xmp[i - 1] = -((int) y);
                  if (xmp[i - 1] > 0)
                    wplac = i;
                  y += xmp[i - 1];
                }
            }
          if (xmp[0] < radix / base)
            {
              /* The machine's comparisons must have given the wrong result */
              /* above - shift XMP left by one base BASE digit and decrease */
              /* the exponent by 1 to compensate. */
              --expont;
              carry = 0;
              for (i = places; i >= 1; --i)
                {
                  xmp[i - 1] = base * xmp[i - 1] + carry;
                  carry = xmp[i - 1] / radix;
                  xmp[i - 1] -= carry * radix;
                }
            }
          /* Insert EXPONT into XMP (IEXO) and return. */
          xmp[iexo - 1] = expont;
          if (expont > emax)
            xmp[fptype - 1] = infcon();
          else if (expont < emin)
            {
              /* Should be a denormalized number, possibly. */
              xmp[fptype - 1] = zercon();
            }
        }
      if (mpisreg(xmp, n, params))
        {
          xmp[places - 1] = xmp[places - 1] / last * last;
          xmp[wplaces - 1] = wplac;
        }
    }

  *ifail = p01abf(ifail, &ierr, "DTOMP", &nrec, rec);
  return;

} /* dtomp */

void itomp(int *k, int *xmp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec, emin;
  int emax, sign, ierr, last, iexo;
  int ipos;
  int i;
  int l, t, wplac, radix, carry;
  int ka, places, radigs;
  int fptype;
  int expont;
  char rec[125];
  int wplaces;

  ierr = 0;
  nrec = 0;
  if (*n <= 0 || !chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      emin = get_emin(params);
      emax = get_emax(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      places = get_places(params);
      wplaces = get_wplaces(params);
      last = get_last(params);

      xmp[fptype - 1] = regcon();
      if (*k == 0)
        {
          xmp[fptype - 1] = zercon();
          xmp[sign - 1] = poscon();
        }
      else
        {
          if (*k < 0)
            xmp[sign - 1] = negcon();
          else
            xmp[sign - 1] = poscon();

          ka = myiabs(*k);
          ipos = 0;
          expont = 0;
        L20:
          ++ipos;
          if (ka != 0 && ipos <= places)
            {
              t = ka / radix;
              l = ka - t * radix;
              ka = t;
              xmp[ipos - 1] = l;
              expont += radigs;
              goto L20;
            }
          wplac = ipos - 1;

          if (ka != 0)
            {
              ierr = 2;
              nrec = 1;
              sprintf(rec, "%s %d %s", "The int ", *k,
                      " does not fit into the MP number");
            }

          /* Reverse the order of the digits and append zeros */
          i_1 = ipos / 2;
          for (i = 1; i <= i_1; ++i)
            {
              t = xmp[i - 1];
              xmp[i - 1] = xmp[ipos - i - 1];
              xmp[ipos - i - 1] = t;
            }

          /* Normalise the number if necessary */
        L60:
          if (xmp[0] < radix / base)
            {
              --expont;
              carry = 0;
              for (i = wplac; i >= 1; --i)
                {
                  xmp[i - 1] = base * xmp[i - 1] + carry;
                  carry = xmp[i - 1] / radix;
                  xmp[i - 1] -= carry * radix;
                }
              goto L60;
            }
          if (xmp[wplac - 1] == 0)
            --wplac;

          if (wplac == places)
            {
              if (xmp[places - 1] != xmp[places - 1] / last * last)
                {
                  ierr = 2;
                  nrec = 1;
                  sprintf(rec, "%s %d %s", "The int ", *k,
                          " does not fit into the MP number");
                  xmp[places - 1] = xmp[places - 1] / last * last;
                }
            }

          /* Insert EXPONT into XMP (IEXO) and return. */
          xmp[iexo - 1] = expont;
          if (expont > emax)
            xmp[fptype - 1] = infcon();
          else if (expont < emin)
            {
              /* Should be a denormalized number, possibly. */
              xmp[fptype - 1] = zercon();
            }
          else
            xmp[wplaces - 1] = wplac;
        }
    }

  *ifail = p01abf(ifail, &ierr, "ITOMP", &nrec, rec);
  return;

} /* itomp */

void mpshow(const char *title, int *xmp, int *n, int *params)
{
  /* Local variables */
  int iexo;
  int j, bunch;
  char csign;
  int wplac, radix;
  int places;
  int fptype;
  int wplaces;

  iexo = get_iexo(params);
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  fptype = get_fptype(params);
  if (mpispos(xmp, n, params))
    csign = '+';
  else
    csign = '-';
  if (mpisnan(xmp, n, params))
    {
      if (xmp[fptype - 1] == qnancon())
        printf("%s%cNaN (quiet)\n", title, csign);
      else
        printf("%s%cNaN (signalling)\n", title, csign);
    }
  else if (mpisinfinity(xmp, n, params))
    printf("%s%cInfinity\n", title, csign);
  else if (mpiszero(xmp, n, params))
    printf("%s%c0.0\n", title, csign);
  else
    {
      wplac = xmp[wplaces - 1];
      if (radix <= 10)
        bunch = 36;
      else if (radix <= 100)
        bunch = 24;
      else if (radix <= 1000)
        bunch = 18;
      else if (radix <= 10000)
        bunch = 15;
      else if (radix <= 100000)
        bunch = 12;
      else
        bunch = 10;
      if (places <= bunch)
        {
          printf("%s%ce(%d)", title, csign, xmp[iexo - 1]);
          for (j = 1; j <= wplac; ++j)
            printf(" %d", xmp[j - 1]);
          for (j = wplac + 1; j <= places; ++j)
            printf(" %d", 0);
          printf("\n");
        }
      else
        {
          printf("%s%ce(%d)", title, csign, xmp[iexo - 1]);
          for (j = 1; j <= wplac; ++j)
            {
              printf(" %d", xmp[j - 1]);
              if (j % bunch == 0)
                printf("\n");
            }
          for (j = wplac + 1; j <= places; ++j)
            {
              printf(" %d", 0);
              if (j % bunch == 0)
                printf("\n");
            }
          printf("\n");
        }
    }
  return;

} /* mpshow */

/* Returns .TRUE. if the number XMP is a regular f-p number. */
int mpisreg(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int fptype;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 0;

  fptype = get_fptype(params);
  ret_val = xmp[fptype - 1] == regcon();
  return ret_val;
} /* mpisreg */

/* Returns .TRUE. if the number XMP is a fake f-p number */
/* (uninitialised or corrupt). */
int mpisfake(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int i_1;
  int ret_val;

  /* Local variables */
  int last;
  int i, wplac, radix;
  int places;
  int fptype;
  int wplaces;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 1;

  fptype = get_fptype(params);
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);
  ret_val = xmp[fptype - 1] != regcon() &&
    xmp[fptype - 1] != zercon() &&
    xmp[fptype - 1] != infcon() &&
    xmp[fptype - 1] != qnancon();
  if (xmp[fptype - 1] == regcon())
    {
      wplac = xmp[wplaces - 1];
      if (wplac < 1 || wplac > places)
        ret_val = 1;
      else
        {
          i_1 = wplac;
          for (i = 1; i <= i_1; ++i)
            ret_val = ret_val || xmp[i - 1] < 0 || xmp[i - 1] > radix;
          if (wplac == places)
            ret_val = ret_val || xmp[places - 1] != xmp[places - 1] / last * last;
        }
    }
  return ret_val;
} /* mpisfake */

/* Returns .TRUE. if the number XMP is +ve or -ve zero. */
int mpiszero(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int fptype;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 0;

  fptype = get_fptype(params);
  ret_val = xmp[fptype - 1] == zercon();
  return ret_val;
} /* mpiszero */

/* Returns .TRUE. if the number XMP is +ve or -ve infinity. */
int mpisinfinity(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int fptype;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 0;

  fptype = get_fptype(params);
  ret_val = xmp[fptype - 1] == infcon();
  return ret_val;
} /* mpisinfinity */

/* Returns .TRUE. if the number XMP is NaN. */
int mpisnan(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int fptype;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 0;

  fptype = get_fptype(params);
  ret_val = xmp[fptype - 1] == qnancon() || xmp[fptype - 1] == snancon();
  return ret_val;
} /* mpisnan */

/* Returns .TRUE. if the number XMP is positive. */
int mpispos(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int sign;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 0;

  sign = get_sign(params);
  ret_val = xmp[sign - 1] == poscon();
  return ret_val;
} /* mpispos */

/* Returns .TRUE. if the number XMP is negative. */
int mpisneg(int *xmp, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int sign;

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return 0;

  sign = get_sign(params);
  ret_val = xmp[sign - 1] == negcon();
  return ret_val;
} /* mpisneg */

/* Returns the type constant for a regular f-p number. */
int regcon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = 90001;
  return ret_val;
} /* regcon */

/* Returns the type constant for a zero f-p number. */
int zercon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = 90002;
  return ret_val;
} /* zercon */

/* Returns the type constant for an infinity. */
int infcon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = 90003;
  return ret_val;
} /* infcon */

/* Returns the type constant for a quiet NaN. */
int qnancon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = 90004;
  return ret_val;
} /* qnancon */

/* Returns the type constant for a signalling NaN. */
int snancon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = 90005;
  return ret_val;
} /* snancon */

/* Returns the sign constant for a positive number. */
int poscon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = 11111;
  return ret_val;
} /* poscon */

/* Returns the sign constant for a negative number. */
int negcon(void)
{
  /* System generated locals */
  int ret_val;

  ret_val = -11111;
  return ret_val;
} /* negcon */

/* Copy one number to another */
void assign(int *xmp, int *ymp, int *n, int *params)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int sign, iexo;
  int i, wplac;
  int fptype;
  int wplaces;

  sign = get_sign(params);
  iexo = get_iexo(params);
  fptype = get_fptype(params);
  wplaces = get_wplaces(params);
  xmp[fptype - 1] = ymp[fptype - 1];
  xmp[sign - 1] = ymp[sign - 1];
  xmp[wplaces - 1] = ymp[wplaces - 1];
  if (mpisreg(ymp, n, params))
    {
      wplac = ymp[wplaces - 1];
      xmp[iexo - 1] = ymp[iexo - 1];
      i_1 = wplac;
      for (i = 1; i <= i_1; ++i)
        xmp[i - 1] = ymp[i - 1];
    }
  return;
} /* assign */

/* Copies MP number from SOURCE to DEST */
void mpcopy(int *source, int *dest, int *n,
            int *params, int *ifail)
{
  int nrec, ierr;
  char rec[125];

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    assign(dest, source, n, params);
  *ifail = p01abf(ifail, &ierr, "MPCOPY", &nrec, rec);
  return;

} /* mpcopy */

/* Like MPCOPY except that SOURCE and DEST do not need */
/* to have the same precision parameters. */
void mpcopy2(int *source, int *n_source, int *params_source,
             int *dest, int *n_dest, int *params_dest, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int nrec, ierr;
  int wplaces_dest, wplac_source, i;
  int radix;
  int sign_dest, last_dest, iexo_dest;
  int fptype_source, iamrnd;
  int iround, wplac_dest, sticky;
  int wplaces_source;
  char rec[125];
  int places_dest;
  int sign_source, iexo_source, fptype_dest;

  ierr = 0;
  nrec = 0;
  if (! chksum(params_source))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_SOURCE was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_dest))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_DEST was not initialised by a call of MPINIT");
    }
  else
    {
      sign_source = get_sign(params_source);
      iexo_source = get_iexo(params_source);
      fptype_source = get_fptype(params_source);
      wplaces_source = get_wplaces(params_source);
      sign_dest = get_sign(params_dest);
      iexo_dest = get_iexo(params_dest);
      fptype_dest = get_fptype(params_dest);
      radix = get_radix(params_dest);
      places_dest = get_places(params_dest);
      wplaces_dest = get_wplaces(params_dest);
      last_dest = get_last(params_dest);
      dest[fptype_dest - 1] = source[fptype_source - 1];
      dest[sign_dest - 1] = source[sign_source - 1];
      if (mpisreg(source, n_source, params_source))
        {
          /* Copy exponent */
          dest[iexo_dest - 1] = source[iexo_source - 1];
          wplac_source = source[wplaces_source - 1];

          if (places_dest > wplac_source)
            {
              /* Copy whole of mantissa, which all fits. */
              /* N.B. we need .GT. above in case PLACES */
              /* is the same for SOURCE and DEST, but */
              /* MANTIS is different. */
              i_1 = wplac_source;
              for (i = 1; i <= i_1; ++i)
                dest[i - 1] = source[i - 1];
              dest[wplaces_dest - 1] = wplac_source;
            }
          else
            {
              /* Not all the mantissa may fit. Copy as */
              /* much as possible. */
              i_1 = places_dest;
              for (i = 1; i <= i_1; ++i)
                dest[i - 1] = source[i - 1];

              /* Then see if we need to round. */
              iround = 0;
              iamrnd = 0;
              sticky = 0;
              i_1 = wplac_source;
              for (i = places_dest + 1; i <= i_1; ++i)
                {
                  if (source[i - 1] > 0)
                    {
                      iround = -1;
                      sticky = 1;
                    }
                }
              iamrnd = 0;
              if (wplac_source > places_dest)
                iamrnd = source[places_dest];
              iamrnd = iamrnd / last_dest + dest[places_dest - 1] %
                last_dest * (radix / last_dest);
              if (dest[places_dest - 1] % last_dest > 0)
                {
                  iround = -1;
                  dest[places_dest - 1] = dest[places_dest - 1] / last_dest *
                    last_dest;
                }

              /* Fix the working precision */
              wplac_dest = places_dest;
            L80:
              if (wplac_dest > 1 && dest[wplac_dest - 1] == 0)
                {
                  --wplac_dest;
                  goto L80;
                }
              dest[wplaces_dest - 1] = places_dest;

              /* Call FIX to do the actual rounding. This may of course */
              /* lead to an infinity, if the exponent is too large, */
              /* or zero, if too small. */
              fix(dest, n_dest, params_dest, &iround, &iamrnd,
                  &sticky);
            }
        }
    }
  *ifail = p01abf(ifail, &ierr, "MPCOPY2", &nrec, rec);
  return;

} /* mpcopy2 */

int mpeq(int *left, int *right, int *n, int *params)
{
  /* System generated locals */
  int i_1;
  int ret_val;

  /* Local variables */
  int sign, iexo;
  int i;
  int wplac_left;
  int wplaces;
  int wplac_right;

  /* Returns TRUE if LEFT == RIGHT, otherwise FALSE. */
  sign = get_sign(params);
  iexo = get_iexo(params);
  wplaces = get_wplaces(params);
  if (mpisnan(left, n, params) || mpisnan(right, n, params))
    ret_val = 0;
  else if (mpisinfinity(left, n, params))
    {
      if (mpisinfinity(right, n, params))
        {
          /* Infinities of the same sign are equal. */
          ret_val = mpispos(left, n, params) == mpispos(right, n, params);
        }
      else
        ret_val = 0;
    }
  else if (mpisinfinity(right, n, params))
    {
      /* LEFT is not infinite, and RIGHT is infinite */
      ret_val = 0;
    }
  else if (mpiszero(left, n, params))
    ret_val = mpiszero(right, n, params);
  else if (mpiszero(right, n, params))
    ret_val = 0;
  else if (left[sign - 1] != right[sign - 1] || left[iexo - 1] != right[iexo - 1])
    ret_val = 0;
  else
    {
      wplac_left = left[wplaces - 1];
      wplac_right = right[wplaces - 1];
      i = 0;
      ret_val = 1;
    L20:
      ++i;
      ret_val = left[i - 1] == right[i - 1];
      if (i < myimin(wplac_left, wplac_right) && ret_val)
        goto L20;
      if (ret_val)
        {
          if (wplac_left < wplac_right)
            {
              i_1 = wplac_right;
              for (i = wplac_left; i <= i_1; ++i)
                {
                  if (right[i - 1] != 0)
                    ret_val = 0;
                }
            }
          else if (wplac_left > wplac_right)
            {
              i_1 = wplac_left;
              for (i = wplac_right; i <= i_1; ++i)
                {
                  if (left[i - 1] != 0)
                    ret_val = 0;
                }
            }
        }
    }
  return ret_val;
} /* mpeq */

int mplt(int *left, int *right, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int sign, iexo;
  int i;
  int c1, c2;
  int eq;
  int wplac_left;
  int wplaces;
  int wplac_right;

  /* Returns TRUE if LEFT < RIGHT, otherwise FALSE. */
  sign = get_sign(params);
  iexo = get_iexo(params);
  wplaces = get_wplaces(params);
  if (mpisnan(left, n, params) || mpisnan(right, n, params))
    ret_val = 0;
  else if (mpisinfinity(left, n, params))
    {
      if (mpispos(left, n, params))
        {
          /* LEFT is +infinity and so is not less than anything */
          ret_val = 0;
        }
      else
        {
          if (mpisinfinity(right, n, params) && mpisneg(right, n, params))
            {
              /* LEFT and RIGHT are both -infinity */
              ret_val = 0;
            }
          else
            {
              /* LEFT is -infinity, RIGHT is something else. */
              ret_val = 1;
            }
        }
    }
  else if (mpisinfinity(right, n, params))
    {
      /* LEFT is not infinite, and RIGHT is infinite */
      ret_val = mpispos(right, n, params);
    }
  else if (mpiszero(left, n, params))
    {
      if (mpiszero(right, n, params))
        ret_val = 0;
      else
        ret_val = mpispos(right, n, params);
    }
  else if (mpiszero(right, n, params))
    ret_val = mpisneg(left, n, params);
  else if (left[sign - 1] != right[sign - 1])
    ret_val = mpisneg(left, n, params);
  else if (left[iexo - 1] != right[iexo - 1])
    {
      if (mpispos(left, n, params))
        ret_val = left[iexo - 1] < right[iexo - 1];
      else
        ret_val = left[iexo - 1] > right[iexo - 1];
    }
  else
    {
      wplac_left = left[wplaces - 1];
      wplac_right = right[wplaces - 1];
      i = 0;
      eq = 1;
    L20:
      ++i;
      if (i <= wplac_left)
        c1 = left[i - 1];
      else
        c1 = 0;
      if (i <= wplac_right)
        c2 = right[i - 1];
      else
        c2 = 0;
      eq = c1 == c2;
      if (i < myimax(wplac_left, wplac_right) && eq)
        goto L20;
      if (i <= wplac_left)
        c1 = left[i - 1];
      else
        c1 = 0;
      if (i <= wplac_right)
        c2 = right[i - 1];
      else
        c2 = 0;
      if (mpispos(left, n, params))
        ret_val = c1 < c2;
      else
        ret_val = c1 > c2;
    }
  return ret_val;
} /* mplt */

int mpgt(int *left, int *right, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Returns TRUE if LEFT > RIGHT, otherwise FALSE. */

  if (mpisnan(left, n, params) || mpisnan(right, n, params))
    ret_val = 0;
  else
    ret_val = mplt(right, left, n, params);
  return ret_val;
} /* mpgt */

int mpge(int *left, int *right, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Returns TRUE if LEFT >= RIGHT, otherwise FALSE. */

  if (mpisnan(left, n, params) || mpisnan(right, n, params))
    ret_val = 0;
  else
    ret_val = ! mplt(left, right, n, params);
  return ret_val;
} /* mpge */

int mple(int *left, int *right, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Returns TRUE if LEFT <= RIGHT, otherwise FALSE. */
  if (mpisnan(left, n, params) || mpisnan(right, n, params))
    ret_val = 0;
  else
    ret_val = ! mplt(right, left, n, params);
  return ret_val;
} /* mple */

int mpne(int *left, int *right, int *n, int *params)
{
  /* System generated locals */
  int ret_val;

  /* Returns TRUE if LEFT != RIGHT, otherwise FALSE. */
  if (mpisnan(left, n, params) || mpisnan(right, n, params))
    ret_val = 0;
  else
    ret_val = ! mpeq(left, right, n, params);
  return ret_val;
} /* mpne */

void next(int *xmp, int *n, int *params, int *up)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, emin;
  int sign, last, iexo;
  int i, wplac, radix;
  int places;
  int icarry;
  int fptype;
  int wplaces;

  /* Returns machine / model number above xmp , or below if UP is FALSE. */
  /* ( adds LAST in place PLACES ) */

  base = get_base(params);
  emin = get_emin(params);
  sign = get_sign(params);
  iexo = get_iexo(params);
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);
  fptype = get_fptype(params);
  if (mpiszero(xmp, n, params))
    {
      xmp[0] = radix / base;
      xmp[iexo - 1] = emin;
      if (*up)
        xmp[sign - 1] = poscon();
      else
        xmp[sign - 1] = negcon();
      xmp[wplaces - 1] = 1;
      xmp[fptype - 1] = regcon();
      return;
    }

  /* We must fill the mantissa explicitly with zeros which are only */
  /* implicit at the moment. */
  wplac = xmp[wplaces - 1];
  i_1 = places;
  for (i = wplac + 1; i <= i_1; ++i)
    xmp[i - 1] = 0;

  if (*up)
    {
      icarry = 0;
      if (xmp[places - 1] == radix - last)
        {
          xmp[places - 1] = 0;
          icarry = 1;
        }
      else
        xmp[places - 1] += last;
      i = places;
    L40:
      if (i == 1 || icarry == 0)
        goto L60;
      --i;
      xmp[i - 1] += icarry;
      icarry = xmp[i - 1] / radix;
      xmp[i - 1] %= radix;
      goto L40;

    L60:
      if (icarry > 0)
        {
          ++xmp[iexo - 1];
          xmp[0] = radix / base;
        }
    }
  else
    {
      icarry = 0;
      if (xmp[places - 1] == 0)
        {
          xmp[places - 1] = radix - last;
          icarry = -1;
        }
      else
        xmp[places - 1] -= last;
      i = places;
    L80:
      if (i == 1 || icarry == 0)
        goto L100;
      --i;
      xmp[i - 1] += icarry;
      if (xmp[i - 1] < 0)
        xmp[i - 1] += radix;
      else
        icarry = 0;
      goto L80;

    L100:
      if (xmp[0] < radix / base)
        {
          --xmp[iexo - 1];
          if (places == 1)
            xmp[0] = (radix - 1) / last * last;
          else
            xmp[0] = radix - 1;
        }
    }
  wplac = places;
 L120:
  if (wplac > 1 && xmp[wplac - 1] == 0)
    {
      --wplac;
      goto L120;
    }
  xmp[wplaces - 1] = wplac;
  return;
} /* next */

void fix(int *result, int *n, int *params,
         int *iround, int *iamrnd, int *sticky)
{
  int emin;
  int emax, iexo;
  int round;
  int fptype;

  /* On entry to FIX, RESULT contains the result of */
  /* an operation, IROUND defines the rounding direction of RESULT, */
  /* IAMRND is the rounding digit, and STICKY is the stickydigit. */
  /* On exit from FIX, RESULT has been rounded according to */
  /* the global variable ROUND. */

  emin = get_emin(params);
  emax = get_emax(params);
  iexo = get_iexo(params);
  fptype = get_fptype(params);
  round = get_round(params);

  if (round == 1)
    ieee(result, n, params, iround, iamrnd, sticky);
  else if (round == 2)
    chop(result, n, params, iround);
  else if (round == 3)
    tomsin(result, n, params, iround);
  else if (round == 4)
    toplin(result, n, params, iround);

  if (result[iexo - 1] < emin)
    result[fptype - 1] = zercon();

  if (result[iexo - 1] > emax)
    result[fptype - 1] = infcon();

  return;
} /* fix */

void ieee(int *result, int *n, int *params,
          int *iround, int *iamrnd, int *sticky)
{
  int last;
  int wplac, radix;
  int rp, places;
  int wplaces;

  /* Rounding is ROUND TO NEAREST, with 1 / 2 rounded to nearest even. */
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  wplac = result[wplaces - 1];
  last = get_last(params);
  if (!mpisreg(result,n,params))
    rp = 0;
  else if (wplac < places)
    rp = 0;
  else
    rp = result[places - 1];
  if (*iamrnd > radix / 2 || (*iamrnd == radix / 2 && *sticky == 1) ||
      (*iamrnd == radix / 2 && rp / last % 2 != 0))
    {
      if (mpispos(result, n, params))
        mproundu(result, n, params, iround);
      else
        mproundl(result, n, params, iround);
    }
  return;
} /* ieee */

void chop(int *result, int *n, int *params,
          int *iround)
{
  /* Rounding is truncation to zero. */
  if (*iround == 1)
    next(result, n, params, &c_false);
  return;
} /* chop */

void tomsin(int *result, int *n, int *params,
            int *iround)
{
  /* Rounding is towards minus infinity. */
  if (mpispos(result, n, params))
    {
      if (*iround == 1)
        next(result, n, params, &c_false);
    }
  else
    {
      if (*iround == -1)
        next(result, n, params, &c_true);
    }
  return;
} /* tomsin */

void toplin(int *result, int *n, int *params,
            int *iround)
{
  /* Rounding is towards plus infinity. */
  if (mpispos(result, n, params))
    {
      if (*iround == -1)
        next(result, n, params, &c_true);
    }
  else
    {
      if (*iround == 1)
        next(result, n, params, &c_false);
    }
  return;
} /* toplin */

void mproundl(int *xmp, int *n, int *params, int
              *iround)
{
  int sign;

  /* Rounds XMP , a lower bound , according to IROUND. */
  sign = get_sign(params);
  if (xmp[sign - 1] == negcon() && *iround == -1)
    next(xmp, n, params, &c_true);
  else if (xmp[sign - 1] == poscon() && *iround == 1)
    next(xmp, n, params, &c_false);
  return;
} /* mproundl */

void mproundu(int *xmp, int *n, int *params, int
              *iround)
{
  int sign;

  /* Rounds XMP , an upper bound , according to IROUND. */
  sign = get_sign(params);
  if (xmp[sign - 1] == poscon() && *iround == -1)
    next(xmp, n, params, &c_true);
  else if (xmp[sign - 1] == negcon() && *iround == 1)
    next(xmp, n, params, &c_false);
  return;
} /* mproundu */

void mpfloor(int *xmp, int *ymp, int *n, int
             *params, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec;
  int sign, ierr, iexo, nreq;
  int i;
  int m;
  int clast;
  int cplacs, radigs;
  int mantis;
  int sticky, fptype;
  int *mp1, *mp2;
  char rec[125];
  int ifl, wplaces;

  /* Sets YMP to be the floor function of XMP. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          ymp[fptype - 1] = xmp[fptype - 1];
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        assign(ymp, xmp, n, params);
      else if (mpiszero(xmp, n, params))
        {
          /* Return zero with the sign of XMP */
          assign(ymp, xmp, n, params);
        }
      else if (xmp[iexo - 1] < 1)
        {
          /* The number is less than 1.0 in magnitude */
          if (mpispos(xmp, n, params))
            {
              /* Result is +zero */
              ymp[fptype - 1] = zercon();
              ymp[sign - 1] = poscon();
            }
          else
            {
              /* Result is -1 */
              ifl = 0;
              itomp(&c_n1, ymp, n, params, &ifl);
            }
        }
      else if (xmp[iexo - 1] >= mantis)
        {
          /* XMP is already an int, so return it */
          assign(ymp, xmp, n, params);
        }
      else
        {
          /* Copy XMP to YMP, then blot out the bits not needed */
          assign(ymp, xmp, n, params);
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          if (ymp[wplaces - 1] > cplacs)
            ymp[wplaces - 1] = cplacs;
          sticky = 0;
          i_1 = xmp[wplaces - 1];
          for (i = cplacs + 1; i <= i_1; ++i)
            {
              if (ymp[i - 1] != 0)
                sticky = 1;
            }
          if (cplacs <= ymp[wplaces - 1])
            {
              if (ymp[cplacs - 1] % clast != 0)
                {
                  sticky = 1;
                  ymp[cplacs - 1] = ymp[cplacs - 1] / clast * clast;
                }
            }

          /* Handle negative case - we need to subtract 1 */
          /* if anything nonzero was discarded. */
          if (sticky == 1 && mpisneg(xmp, n, params))
            {
              /* Dissect workspace */
              mp1 = &iwork[0];
              mp2 = &iwork[*n];

              ifl = 0;
              itomp(&c_n1, mp1, n, params, &ifl);
              plus(ymp, mp1, mp2, n, params, &ifl);
              assign(ymp, mp2, n, params);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPFLOOR", &nrec, rec);
  return;

} /* mpfloor */

void mpceil(int *xmp, int *ymp, int *n, int *params, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec;
  int sign, ierr, iexo, nreq;
  int i;
  int m, clast;
  int cplacs, radigs;
  int mantis;
  int sticky, fptype;
  int *mp1, *mp2;
  char rec[125];
  int ifl, wplaces;

  /* Sets YMP to be the ceil function of XMP. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          ymp[fptype - 1] = xmp[fptype - 1];
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        assign(ymp, xmp, n, params);
      else if (mpiszero(xmp, n, params))
        {
          /* Return zero with the sign of XMP */
          assign(ymp, xmp, n, params);
        }
      else if (xmp[iexo - 1] < 1)
        {
          /* The number is less than 1.0 in magnitude */
          if (mpispos(xmp, n, params))
            {
              /* Result is +1 */
              ifl = 0;
              itomp(&c_1, ymp, n, params, &ifl);
            }
          else
            {
              /* Result is -zero */
              ymp[fptype - 1] = zercon();
              ymp[sign - 1] = negcon();
            }
        }
      else if (xmp[iexo - 1] >= mantis)
        {
          /* XMP is already an int, so return it */
          assign(ymp, xmp, n, params);
        }
      else
        {
          /* Copy XMP to YMP, then blot out the bits not needed */
          assign(ymp, xmp, n, params);
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          if (ymp[wplaces - 1] > cplacs)
            ymp[wplaces - 1] = cplacs;
          sticky = 0;
          i_1 = xmp[wplaces - 1];
          for (i = cplacs + 1; i <= i_1; ++i)
            {
              if (ymp[i - 1] != 0)
                sticky = 1;
            }
          if (cplacs <= ymp[wplaces - 1])
            {
              if (ymp[cplacs - 1] % clast != 0)
                {
                  sticky = 1;
                  ymp[cplacs - 1] = ymp[cplacs - 1] / clast * clast;
                }
            }

          /* Handle positive case - we need to add 1 */
          /* if anything nonzero was discarded. */
          if (sticky == 1 && mpispos(xmp, n, params))
            {
              /* Dissect workspace */
              mp1 = &iwork[0];
              mp2 = &iwork[*n];

              ifl = 0;
              itomp(&c_1, mp1, n, params, &ifl);
              plus(ymp, mp1, mp2, n, params, &ifl);
              assign(ymp, mp2, n, params);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPCEIL", &nrec, rec);
  return;

} /* mpceil */

void mptrunc(int *xmp, int *ymp, int *n, int
             *params, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec;
  int sign, ierr, iexo, nreq;
  int m, clast;
  int cplacs, radigs;
  int mantis;
  int fptype;
  char rec[125];
  int wplaces;

  /* Sets YMP to be the trunc function of XMP (truncate to */
  /* int in zero direction) */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          ymp[fptype - 1] = xmp[fptype - 1];
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        assign(ymp, xmp, n, params);
      else if (mpiszero(xmp, n, params))
        {
          /* Return zero with the sign of XMP */
          assign(ymp, xmp, n, params);
        }
      else if (xmp[iexo - 1] < 1)
        {
          /* The number is less than 1.0 in magnitude */
          /* Result is +zero */
          ymp[fptype - 1] = zercon();
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (xmp[iexo - 1] >= mantis)
        {
          /* XMP is already an int, so return it */
          assign(ymp, xmp, n, params);
        }
      else
        {
          /* Copy XMP to YMP, then blot out the bits not needed */
          assign(ymp, xmp, n, params);
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          if (ymp[wplaces - 1] > cplacs)
            ymp[wplaces - 1] = cplacs;
          if (cplacs <= ymp[wplaces - 1])
            {
              if (ymp[cplacs - 1] % clast != 0)
                ymp[cplacs - 1] = ymp[cplacs - 1] / clast * clast;
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPTRUNC", &nrec, rec);
  return;

} /* mptrunc */

void mprint(int *xmp, int *ymp, int *n, int *params, int *ifail)
{
  /* Local variables */
  int nrec;
  int sign, ierr, iexo, nreq;
  int ifl, k;
  int mantis;
  int fptype;
  char rec[125];
  int *mp1, *mp2, *mp3, *mp4, *absxmp, *iwork;

  /* Sets YMP to be the rint function (round to nearest int) of XMP */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      mantis = get_mantis(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          ymp[fptype - 1] = xmp[fptype - 1];
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        assign(ymp, xmp, n, params);
      else if (mpiszero(xmp, n, params))
        {
          /* Return zero with the sign of XMP */
          assign(ymp, xmp, n, params);
        }
      else if (xmp[iexo - 1] >= mantis)
        {
          /* XMP is already an int, so return it */
          assign(ymp, xmp, n, params);
        }
      else
        {
          mp1 = (int *)malloc(*n * sizeof(int));
          mp2 = (int *)malloc(*n * sizeof(int));
          mp3 = (int *)malloc(*n * sizeof(int));
          mp4 = (int *)malloc(*n * sizeof(int));
          absxmp = (int *)malloc(*n * sizeof(int));
          iwork = (int *)malloc(2 * *n * sizeof(int));
          if (!mp1 || !mp2 || !mp3 || !mp4 || !absxmp || !iwork)
            {
              ierr = 3;
              nrec = 1;
              sprintf(rec, "Failed to allocate workspace");
            }
          else
            {
              assign(absxmp, xmp, n, params);
              mpabs(absxmp, n, params, &ifl);
              ifl = 0;
              mpceil(absxmp, mp1, n, params, iwork, &ifl);
              minus(mp1, absxmp, mp3, n, params, &ifl);
              mpfloor(absxmp, mp2, n, params, iwork, &ifl);
              minus(absxmp, mp2, mp4, n, params, &ifl);

              if (mplt(mp3, mp4, n, params))
                /* x is closer to ceil(x); return ceil(x). */
                assign(ymp, mp1, n, params);
              else if (mpgt(mp3, mp4, n, params))
                /* x is closer to floor(x); return floor(x). */
                assign(ymp, mp2, n, params);
              else if (mpeq(mp1, mp2, n, params))
                /* ceil(x) == floor(x) == x; return x. */
                assign(ymp, mp2, n, params);
              else
                {
                  /* ceil(x) and floor(x) are equidistant from x;
                     return whichever of them is even. */
                  k = mpmod4(mp1, n, params, &ifl);
                  if ((k & 1) == 1)
                    /* ceil(x) is odd; return floor(x) */
                    assign(ymp, mp2, n, params);
                  else
                    /* ceil(x) is even; return ceil(x) */
                    assign(ymp, mp1, n, params);
                }

              ymp[sign - 1] = xmp[sign - 1];
              free(mp1);
              free(mp2);
              free(mp3);
              free(mp4);
              free(absxmp);
              free(iwork);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPRINT", &nrec, rec);
  return;

} /* mprint */

void mpround(int *xmp, int *ymp, int *n, int *params, int *ifail)
{
  /* Local variables */
  int nrec;
  int sign, ierr, iexo, nreq;
  int ifl;
  int mantis;
  int fptype;
  char rec[125];
  int *mp1, *mp2, *mp3, *mp4, *absxmp, *iwork;

  /* Sets YMP to be the round function (round to nearest int) of XMP */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      mantis = get_mantis(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          ymp[fptype - 1] = xmp[fptype - 1];
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        assign(ymp, xmp, n, params);
      else if (mpiszero(xmp, n, params))
        {
          /* Return zero with the sign of XMP */
          assign(ymp, xmp, n, params);
        }
      else if (xmp[iexo - 1] >= mantis)
        {
          /* XMP is already an int, so return it */
          assign(ymp, xmp, n, params);
        }
      else
        {
          mp1 = (int *)malloc(*n * sizeof(int));
          mp2 = (int *)malloc(*n * sizeof(int));
          mp3 = (int *)malloc(*n * sizeof(int));
          mp4 = (int *)malloc(*n * sizeof(int));
          absxmp = (int *)malloc(*n * sizeof(int));
          iwork = (int *)malloc(2 * *n * sizeof(int));
          if (!mp1 || !mp2 || !mp3 || !mp4 || !absxmp || !iwork)
            {
              ierr = 3;
              nrec = 1;
              sprintf(rec, "Failed to allocate workspace");
            }
          else
            {
              assign(absxmp, xmp, n, params);
              mpabs(absxmp, n, params, &ifl);
              ifl = 0;
              mpceil(absxmp, mp1, n, params, iwork, &ifl);
              minus(mp1, absxmp, mp3, n, params, &ifl);
              mpfloor(absxmp, mp2, n, params, iwork, &ifl);
              minus(absxmp, mp2, mp4, n, params, &ifl);

              if (mplt(mp3, mp4, n, params))
                /* x is closer to ceil(x); return ceil(x). */
                assign(ymp, mp1, n, params);
              else if (mpgt(mp3, mp4, n, params))
                /* x is closer to floor(x); return floor(x). */
                assign(ymp, mp2, n, params);
              else if (mpeq(mp1, mp2, n, params))
                /* ceil(x) == floor(x) == x; return x. */
                assign(ymp, mp2, n, params);
              else
                {
                  /* ceil(x) and floor(x) are equidistant from x;
                     return away from zero. */
                    assign(ymp, mp1, n, params);
                }

              ymp[sign - 1] = xmp[sign - 1];
              free(mp1);
              free(mp2);
              free(mp3);
              free(mp4);
              free(absxmp);
              free(iwork);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPROUND", &nrec, rec);
  return;

} /* mpround */

void mpfrac(int *xmp, int *ymp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base, nrec;
  int sign, ierr, iexo, nreq;
  int i;
  int k, m, clast, radix, wplac;
  int carry;
  int cplacs, radigs;
  int mantis;
  int fptype;
  char rec[125];
  int mul, wplaces;

  /* Sets YMP to be the fractional part of XMP, i.e. the part */
  /* left after subtracting MPTRUNC(XMP). */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      sign = get_sign(params);
      iexo = get_iexo(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisnan(xmp, n, params))
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "XMP is NaN");
        }
      else if (mpisinfinity(xmp, n, params))
        {
          ierr = 4;
          nrec = 1;
          sprintf(rec, "XMP is Infinity");
        }
      else if (mpiszero(xmp, n, params) || xmp[iexo - 1] >= mantis)
        {
          /* XMP is zero, or a large int. Return zero with correct sign. */
          ymp[fptype - 1] = zercon();
          ymp[sign - 1] = xmp[sign - 1];
        }
      else if (xmp[iexo - 1] < 1)
        {
          /* The number is less than 1.0 in magnitude. Result is XMP. */
          assign(ymp, xmp, n, params);
        }
      else
        {
          /* Copy XMP to YMP, then blot out the bits not needed */
          assign(ymp, xmp, n, params);
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          i_1 = cplacs - 1;
          for (i = 1; i <= i_1; ++i)
            ymp[i - 1] = 0;
          ymp[cplacs - 1] %= clast;
          /* Find the first non-zero position */
          wplac = ymp[wplaces - 1];
          k = 1;
          while (k <= wplac && ymp[k - 1] == 0)
            ++k;
          if (k > wplac)
            ymp[fptype - 1] = zercon();
          else
            {
              /* Normalise the number */
              i_1 = wplac;
              for (i = k; i <= i_1; ++i)
                ymp[i - k] = ymp[i - 1];
              wplac = wplac - k + 1;
              ymp[wplaces - 1] = wplac;
              m = ymp[0];
              mul = 1;
              ymp[iexo - 1] += (1 - k) * radigs;
            L80:
              if (m < radix / base)
                {
                  m *= base;
                  mul *= base;
                  --ymp[iexo - 1];
                  goto L80;
                }
              carry = 0;
              for (i = wplac; i >= 1; --i)
                {
                  m = ymp[i - 1] * mul + carry;
                  carry = m / radix;
                  ymp[i - 1] = m - carry * radix;
                }
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPFRAC", &nrec, rec);
  return;

} /* mpfrac */

void mpmodf(int *xmp, int *ymp, int *iymp,
            int *n, int *params, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  char rec[125];
  int ifl;

  /* Sets IYMP to be trunc(XMP), and YMP to be the fractional */
  /* part of XMP, i.e. the part left after subtracting trunc(XMP). */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          assign(ymp, xmp, n, params);
          assign(iymp, xmp, n, params);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* Return zero with the sign of the infinity */
          ifl = 0;
          itomp(&c_0, ymp, n, params, &ifl);
          ymp[sign - 1] = xmp[sign - 1];
          assign(iymp, xmp, n, params);
        }
      else if (mpiszero(xmp, n, params))
        {
          assign(ymp, xmp, n, params);
          assign(iymp, xmp, n, params);
        }
      else
        {
          ifl = 0;
          mpfrac(xmp, ymp, n, params, &ifl);
          mptrunc(xmp, iymp, n, params, &ifl);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPMODF", &nrec, rec);
  return;

} /* mpmodf */

int mpmod(int *xmp, int *n, int *params, int *k, int
          *iwork, int *ifail)
{
  /* System generated locals */
  int ret_val, i_1;

  /* Local variables */
  int base, nrec;
  int ierr, iexo, nreq;
  int i;
  int m;
  int clast, wplac;
  int *iw;
  int cplacs, radigs;
  int mantis;
  int sticky;
  int *mp1, *mp2, *mp3;
  char rec[125];
  int ifl, wplaces;

  /* Returns XMP mod K, where XMP is an MP number and K is an int. */

  ierr = 0;
  nrec = 0;
  ret_val = -12345;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (*k < 1)
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "%s %d", "K is less than 1: K = ", *k);
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      iexo = get_iexo(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisnan(xmp, n, params))
        {
          ierr = 4;
          nrec = 1;
          sprintf(rec, "XMP is NaN");
        }
      else if (mpisinfinity(xmp, n, params))
        {
          ierr = 5;
          nrec = 1;
          sprintf(rec, "XMP is Infinity");
        }
      else if (mpisneg(xmp, n, params))
        {
          ierr = 6;
          nrec = 1;
          sprintf(rec, "XMP is negative");
        }
      else if (mpiszero(xmp, n, params))
        ret_val = 0;
      else if (xmp[iexo - 1] < 1)
        {
          ierr = 7;
          nrec = 1;
          sprintf(rec, "XMP is not an int");
        }
      else if (xmp[iexo - 1] > mantis)
        {
          ierr = 8;
          nrec = 1;
          sprintf(rec, "XMP is too large to hold an int accurately");
        }
      else
        {
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          wplac = xmp[wplaces - 1];
          sticky = 0;
          i_1 = wplac;
          for (i = cplacs + 1; i <= i_1; ++i)
            {
              if (xmp[i - 1] != 0)
                sticky = 1;
            }
          if (sticky == 1 || xmp[cplacs - 1] % clast != 0)
            {
              ierr = 7;
              nrec = 1;
              sprintf(rec, "XMP is not an int");
            }
          else
            {
              /* At last, we've narrowed it down to a non-negative */
              /* int. Find the remainder when we divide by k. */

              /* Dissect workspace */
              mp1 = &iwork[0];
              mp2 = &iwork[*n];
              mp3 = &iwork[(*n * 2)];
              iw = mp3;
              ifl = 0;
              itomp(k, mp1, n, params, &ifl);
              divide(xmp, mp1, mp2, n, params,
                     iw, &ifl);
              mptrunc(mp2, mp3, n, params, &ifl);
              times(mp3, mp1, mp2, n, params,
                    &ifl);
              minus(xmp, mp2, mp3, n, params,
                    ifail);
              ret_val = mptoi(mp3, n, params, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPMOD", &nrec, rec);
  return ret_val;

} /* mpmod */

int mpmod4(int *xmp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int ret_val, i_1;

  /* Local variables */
  int base, nrec;
  int ierr, iexo, nreq;
  int k, m;
  int clast, radix, wplac;
  int cplacs, radigs;
  int wplaces;
  int mantis;
  char rec[125];

  /* Returns ((int)XMP) mod 4, where XMP is an MP number. BASE must be even. */

  ierr = 0;
  nrec = 0;
  ret_val = -12345;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      iexo = get_iexo(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (base % 2 != 0)
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "%s %d", "BASE must be even: BASE = ", base);
        }
      else if (mpisnan(xmp, n, params))
        {
          ierr = 4;
          nrec = 1;
          sprintf(rec, "XMP is NaN");
        }
      else if (mpisinfinity(xmp, n, params))
        {
          ierr = 5;
          nrec = 1;
          sprintf(rec, "XMP is Infinity");
        }
      else if (mpisneg(xmp, n, params))
        {
          ierr = 6;
          nrec = 1;
          sprintf(rec, "XMP is negative");
        }
      else if (mpiszero(xmp, n, params))
        ret_val = 0;
      else if (xmp[iexo - 1] < 1)
        ret_val = 0;
      else if (xmp[iexo - 1] > mantis)
        {
          ierr = 8;
          nrec = 1;
          sprintf(rec, "XMP is too large to hold an int accurately");
        }
      else
        {
          int imp;
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          wplac = xmp[wplaces - 1];
          /* We ignore anything after the decimal point. */
          /* Find the remainder when we divide by k. */
          if (cplacs > wplac)
            imp = 0;
          else
            imp = xmp[cplacs - 1];
          k = imp / clast;
          if (cplacs > 1)
            {
              if (cplacs - 1 > wplac)
                imp = 0;
              else
                imp = xmp[cplacs - 1 - 1];
              k += imp * radix / clast;
            }
          ret_val = k % 4;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPMOD4", &nrec, rec);
  return ret_val;

} /* mpmod4 */

int isanint(int *xmp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int ret_val, i_1;

  /* Local variables */
  int base, nrec;
  int ierr, iexo, nreq;
  int i;
  int k, m, clast, radix, wplac;
  int cplacs, radigs;
  int mantis;
  char rec[125];
  int zer;
  int wplaces;

  /* Determines whether the number XMP is an int or not. */
  /* Returns 0 if it is not an int, 1 if it is an odd int, */
  /* and 2 if it is an even int. */

  ierr = 0;
  nrec = 0;
  ret_val = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      iexo = get_iexo(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (base % 2 != 0)
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "%s %d", "BASE must be even: BASE = ", base);
        }
      else if (mpisnan(xmp, n, params))
        {
          ierr = 4;
          nrec = 1;
          sprintf(rec, "XMP is NaN");
        }
      else if (mpisinfinity(xmp, n, params))
        {
          ierr = 5;
          nrec = 1;
          sprintf(rec, "XMP is Infinity");
        }
      else if (mpiszero(xmp, n, params))
        ret_val = 2;
      else if (xmp[iexo - 1] < 1)
        ret_val = 0;
      else if (xmp[iexo - 1] >= mantis)
        ret_val = 2;
      else
        {
          m = xmp[iexo - 1];
          cplacs = (m + radigs - 1) / radigs;
          i_1 = radigs * cplacs - m;
          clast = ipower(&base, i_1);
          if (xmp[cplacs - 1] != xmp[cplacs - 1] / clast * clast)
            ret_val = 0;
          else
            {
              zer = 1;
              wplac = xmp[wplaces - 1];
              i_1 = wplac;
              for (i = cplacs + 1; i <= i_1; ++i)
                {
                  if (xmp[i - 1] != 0)
                    zer = 0;
                }
              if (! zer)
                ret_val = 0;
              else
                {
                  k = xmp[cplacs - 1] / clast;
                  if (cplacs > 1)
                    k += xmp[cplacs - 1 - 1] * radix / clast;
                  if (k % 2 == 0)
                    {
                      /* The number is even */
                      ret_val = 2;
                    }
                  else
                    {
                      /* The number is odd */
                      ret_val = 1;
                    }
                }
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "ISANINT", &nrec, rec);
  return ret_val;

} /* isanint */

int mpilogb(int *xmp, int *emin_x,
            int *n, int *params, int *ifail)
{
  int nrec;
  int sign, ierr, iexo, nreq;
  int k;
  int fptype;
  char rec[125];

  /* Sets YMP to be ilogb(XMP), i.e. the exponent of x, which is the */
  /* integral part of logr |x|, as a signed integer value, */
  /* for non-zero x, where  r  is  the  radix  of  the  machine's */
  /* floating-point arithmetic. Return values for zero, NaN and infinity */
  /* arguments are as defined on the Solaris man page for ilogb. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpiszero(xmp, n, params))
        {
          /* XMP is zero. Return MAX NEG INTEGER. */
          return 0x80000000; /* -2147483648 */
        }
      else if (mpisnan(xmp, n, params))
        {
          /* XMP is NaN. Return MAX NEG INTEGER. */
          return 0x80000000; /* -2147483648 */
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* XMP is Inf. Return MAX INTEGER. */
          return 0x7fffffff; /* 2147483647 */
        }
      else
        {
          /* The IEEE-style exponent is offset by 1 compared with our */
          /* floating-point number system. */
          if (xmp[iexo - 1] < *emin_x)
            /* xmp is denormalized */
#ifdef FOLLOW_IEEE754_LOGB
          /* Denormalized numbers should return the value of the minimum */
          /* exponent to ensure that the relationship between logb */
          /* and scalb, defined in IEEE 754, holds. */
            k = *emin_x - 1;
#else
          /* Follow IEEE 854 rules for logb */
            k = xmp[iexo - 1] - 1;
#endif
          else
            /* xmp is a normal number */
            k = xmp[iexo - 1] - 1;

          return k;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPILOGB", &nrec, rec);
  return 0x80000000;

} /* mpilogb */

void mplogb(int *xmp, int *emin_x, int *ymp,
            int *n, int *params, int *ifail)
{
  int nrec;
  int sign, ierr, iexo, nreq;
  int k;
  int fptype;
  char rec[125];

  /* Sets YMP to be logb(XMP), i.e. the exponent of x, which is the */
  /* integral part of logr |x|, as a signed floating point value, */
  /* for non-zero x, where  r  is  the  radix  of  the  machine's */
  /* floating-point arithmetic. Return values for zero, NaN and infinity */
  /* arguments are as defined on the Solaris man page for logb. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpiszero(xmp, n, params))
        {
          /* XMP is zero. Return -infinity. */
          ymp[fptype - 1] = infcon();
          ymp[sign - 1] = negcon();
        }
      else if (mpisnan(xmp, n, params))
        assign(ymp, xmp, n, params);
      else if (mpisinfinity(xmp, n, params))
        {
          ymp[fptype - 1] = infcon();
          ymp[sign - 1] = poscon();
        }
      else
        {
          /* The IEEE-style exponent is offset by 1 compared with our */
          /* floating-point number system. */
          if (xmp[iexo - 1] < *emin_x)
            /* xmp is denormalized */
#ifdef FOLLOW_IEEE754_LOGB
          /* Denormalized numbers should return the value of the minimum */
          /* exponent to ensure that the relationship between logb */
          /* and scalb, defined in IEEE 754, holds. */
            k = *emin_x - 1;
#else
          /* Follow IEEE 854 rules for logb */
            k = xmp[iexo - 1] - 1;
#endif
          else
            /* xmp is a normal number */
            k = xmp[iexo - 1] - 1;

          /* Convert the result to a floating-point number. */
          itomp(&k, ymp, n, params, ifail);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPLOGB", &nrec, rec);
  return;

} /* mplogb */

int mpfinite(int *xmp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int nrec, ierr, nreq;
  char rec[125];

  /* Returns 1 if XMP is neither infinite nor NaN, otherwise returns 0. */

  ierr = 0;
  nrec = 0;
  ret_val = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 3;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else
        {
          if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params))
            ret_val = 0;
          else
            ret_val = 1;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPFINITE", &nrec, rec);
  return ret_val;

} /* mpfinite */

void plus(int *left, int *right, int *result,
          int *n, int *params, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int less;
  int savls;
  int savrs;
  int iamrnd;
  int iround;
  int nought;
  int sticky, fptype;
  char rec[125];

  /* Adds together two numbers, LEFT and RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(left, n, params) || mpisfake(right, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LEFT or RIGHT is corrupt or uninitialised");
        }
      else if (mpisnan(left, n, params) || mpisnan(right, n, params))
        {
          if (mpisnan(left, n, params))
            {
              result[fptype - 1] = left[fptype - 1];
              result[sign - 1] = left[sign - 1];
            }
          else
            {
              result[fptype - 1] = right[fptype - 1];
              result[sign - 1] = right[sign - 1];
            }
        }
      else if (mpisinfinity(left, n, params) && mpisinfinity(right, n, params))
        {
          if (mpispos(left, n, params) != mpispos(right, n, params))
            result[fptype - 1] = qnancon();
          else
            result[fptype - 1] = infcon();
          result[sign - 1] = left[sign - 1];
        }
      else if (mpisinfinity(left, n, params))
        assign(result, left, n, params);
      else if (mpisinfinity(right, n, params))
        assign(result, right, n, params);
      else if (mpiszero(left, n, params) && mpiszero(right, n, params))
        {
          /* Return zero with the sign of LEFT */
          assign(result, left, n, params);
        }
      else if (mpiszero(left, n, params))
        assign(result, right, n, params);
      else if (mpiszero(right, n, params))
        assign(result, left, n, params);
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */
          result[fptype - 1] = regcon();

          /* Which is the smaller in magnitude? Be careful */
          /* of how we manipulate signs here, in case LEFT */
          /* and RIGHT are the same argument. */
          savls = left[sign - 1];
          savrs = right[sign - 1];
          left[sign - 1] = poscon();
          right[sign - 1] = poscon();
          less = mplt(left, right, n, params);
          left[sign - 1] = savls;
          right[sign - 1] = savrs;

          /* Call low level routines with larger number first */
          if (less)
            {
              if (right[sign - 1] == left[sign - 1])
                {
                  lowadd(right, left, n, params, result,
                         &iround, &iamrnd, &sticky);
                }
              else
                {
                  lowsub(right, left, n, params, result,
                         &iround, &iamrnd, &sticky, &nought);
                  if (nought)
                    result[fptype - 1] = zercon();
                }
              result[sign - 1] = right[sign - 1];
            }
          else
            {
              if (left[sign - 1] == right[sign - 1])
                {
                  lowadd(left, right, n, params, result,
                         &iround, &iamrnd, &sticky);
                }
              else
                {
                  lowsub(left, right, n, params, result,
                         &iround, &iamrnd, &sticky, &nought);
                  if (nought)
                    result[fptype - 1] = zercon();
                }
              result[sign - 1] = left[sign - 1];
            }

          fix(result, n, params, &iround, &iamrnd, &sticky);
        }
    }

  *ifail = p01abf(ifail, &ierr, "PLUS", &nrec, rec);
  return;

} /* plus */

void minus(int *left, int *right, int *result,
           int *n, int *params, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int less;
  int savls;
  int savrs;
  int iamrnd;
  int iround;
  int nought;
  int sticky, fptype;
  char rec[125];

  /* Subtracts number RIGHT from number LEFT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(left, n, params) || mpisfake(right, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LEFT or RIGHT is corrupt or uninitialised");
        }
      else if (mpisnan(left, n, params) || mpisnan(right, n, params))
        {
          if (mpisnan(left, n, params))
            {
              result[fptype - 1] = left[fptype - 1];
              result[sign - 1] = left[sign - 1];
            }
          else
            {
              result[fptype - 1] = right[fptype - 1];
              result[sign - 1] = right[sign - 1];
            }
        }
      else if (mpisinfinity(left, n, params) && mpisinfinity(right, n, params))
        {
          if (mpispos(left, n, params) == mpispos(right, n, params))
            result[fptype - 1] = qnancon();
          else
            result[fptype - 1] = infcon();
          result[sign - 1] = left[sign - 1];
        }
      else if (mpisinfinity(left, n, params))
        assign(result, left, n, params);
      else if (mpisinfinity(right, n, params))
        {
          assign(result, right, n, params);
          if (mpispos(right, n, params))
            result[sign - 1] = negcon();
          else
            result[sign - 1] = poscon();
        }
      else if (mpiszero(left, n, params) && mpiszero(right, n, params))
        {
          /* Return zero with the sign of left */
          assign(result, left, n, params);
        }
      else if (mpiszero(left, n, params))
        {
          assign(result, right, n, params);
          if (mpispos(right, n, params))
            result[sign - 1] = negcon();
          else
            result[sign - 1] = poscon();
        }
      else if (mpiszero(right, n, params))
        assign(result, left, n, params);
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */
          result[fptype - 1] = regcon();

          /* Which is the smaller in magnitude? Be careful */
          /* of how we manipulate signs here, in case LEFT */
          /* and RIGHT are the same argument. */
          savls = left[sign - 1];
          savrs = right[sign - 1];
          left[sign - 1] = poscon();
          right[sign - 1] = poscon();
          less = mplt(left, right, n, params);
          left[sign - 1] = savls;
          right[sign - 1] = savrs;

          /* Call low level routines with larger number first */
          if (less)
            {
              /* LEFT is less than RIGHT */
              if (right[sign - 1] == left[sign - 1])
                {
                  lowsub(right, left, n, params, result,
                         &iround, &iamrnd, &sticky, &nought);
                  if (nought)
                    result[fptype - 1] = zercon();
                }
              else
                {
                  lowadd(right, left, n, params, result,
                         &iround, &iamrnd, &sticky);
                }
              if (mpispos(right, n, params))
                result[sign - 1] = negcon();
              else
                result[sign - 1] = poscon();
            }
          else
            {
              if (left[sign - 1] == right[sign - 1])
                {
                  lowsub(left, right, n, params, result,
                         &iround, &iamrnd, &sticky, &nought);
                  if (nought)
                    result[fptype - 1] = zercon();
                }
              else
                {
                  lowadd(left, right, n, params, result,
                         &iround, &iamrnd, &sticky);
                }
              result[sign - 1] = left[sign - 1];
            }

          fix(result, n, params, &iround, &iamrnd, &sticky);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MINUS", &nrec, rec);
  return;

} /* minus */

void times(int *left, int *right, int *result,
           int *n, int *params, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int iamrnd;
  int iround;
  int sticky, fptype;
  char rec[125];

  /* Multiplies two numbers, LEFT and RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);

      if (left[sign - 1] == right[sign - 1])
        result[sign - 1] = poscon();
      else
        result[sign - 1] = negcon();

      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(left, n, params) || mpisfake(right, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LEFT or RIGHT is corrupt or uninitialised");
        }
      else if (mpisnan(left, n, params) || mpisnan(right, n, params))
        {
          if (mpisnan(left, n, params))
            {
              result[fptype - 1] = left[fptype - 1];
              result[sign - 1] = left[sign - 1];
            }
          else
            {
              result[fptype - 1] = right[fptype - 1];
              result[sign - 1] = right[sign - 1];
            }
        }
      else if (mpiszero(left, n, params))
        {
          if (mpisinfinity(right, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = left[sign - 1];
            }
          else
            result[fptype - 1] = zercon();
        }
      else if (mpiszero(right, n, params))
        {
          if (mpisinfinity(left, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = left[sign - 1];
            }
          else
            result[fptype - 1] = zercon();
        }
      else if (mpisinfinity(left, n, params))
        {
          if (mpiszero(right, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = left[sign - 1];
            }
          else
            result[fptype - 1] = infcon();
        }
      else if (mpisinfinity(right, n, params))
        {
          if (mpiszero(left, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = left[sign - 1];
            }
          else
            result[fptype - 1] = infcon();
        }
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */
          result[fptype - 1] = regcon();
          lowmul(left, right, n, params, result, &iround,
                 &iamrnd, &sticky);
          fix(result, n, params, &iround, &iamrnd, &sticky);
        }
    }

  *ifail = p01abf(ifail, &ierr, "TIMES", &nrec, rec);
  return;

} /* times */

/* Special version of TIMES which handles numbers of */
/* different precision. */
void times2(int *left, int *n_left, int *params_left,
            int *right, int *n_right, int *params_right,
            int *result, int *n_result, int *params_result,
            int *ifail)
{
  int nreq_result, nrec;
  int ierr;
  int radigs_right;
  int fptype_right, radix_result;
  int base_left;
  int sign_left;
  int nreq_left;
  int radigs_result;
  int fptype_result;
  int iamrnd;
  int base_right;
  int iround, radix_left;
  int sticky;
  int sign_right, nreq_right;
  char rec[125];
  int radigs_left, radix_right, base_result, fptype_left,
    sign_result;

  /* Multiplies two numbers, LEFT and RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params_left))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_LEFT was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_right))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_RIGHT was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_result))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_RESULT was not initialised by a call of MPINIT");
    }
  else
    {
      base_left = get_base(params_left);
      base_right = get_base(params_right);
      base_result = get_base(params_result);
      radix_left = get_radix(params_left);
      radix_right = get_radix(params_right);
      radix_result = get_radix(params_result);
      radigs_left = get_radigs(params_left);
      radigs_right = get_radigs(params_right);
      radigs_result = get_radigs(params_result);
      if (base_left != base_right || base_left != base_result ||
          radix_left != radix_right || radix_left !=
          radix_result || radigs_left != radigs_right ||
          radigs_left != radigs_result)
        {
          ierr = 1;
          sprintf(rec, "Arrays PARAMS_LEFT, PARAMS_RIGHT, PARAMS_RESULT inconsistent.");
        }
      else if (mpisfake(left, n_left, params_left) ||
               mpisfake(right, n_right, params_right))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LEFT or RIGHT is corrupt or uninitialised");
        }
      else
        {
          sign_left = get_sign(params_left);
          sign_right = get_sign(params_right);
          sign_result = get_sign(params_result);
          fptype_left = get_fptype(params_left);
          fptype_right = get_fptype(params_right);
          fptype_result = get_fptype(params_result);

          if (left[sign_left - 1] == right[sign_right - 1])
            result[sign_result - 1] = poscon();
          else
            result[sign_result - 1] = negcon();

          nreq_left = get_mplen(params_left);
          nreq_right = get_mplen(params_right);
          nreq_result = get_mplen(params_result);
          if (*n_left < nreq_left)
            {
              ierr = 1;
              nrec = 1;
              sprintf(rec, "%s %d %s %d", "N_LEFT is too small. N_LEFT must be at least",
                      nreq_left, ": N_LEFT = ", *n_left);
            }
          else if (*n_right < nreq_right)
            {
              ierr = 1;
              nrec = 1;
              sprintf(rec, "%s %d %s %d", "N_RIGHT is too small. N_RIGHT must be at least",
                      nreq_right, ": N_RIGHT = ", *n_right);
            }
          else if (*n_result < nreq_result)
            {
              ierr = 1;
              nrec = 1;
              sprintf(rec, "%s %d %s %d", "N_RESULT is too small. N_RESULT must be at least",
                      nreq_result, ": N_RESULT = ",
                      *n_result);
            }
          else if (mpisnan(left, n_left, params_left) ||
                   mpisnan(right, n_right, params_right))
            {
              if (mpisnan(left, n_left, params_left))
                {
                  result[fptype_result - 1] = left[fptype_left - 1];
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                {
                  result[fptype_result - 1] = right[fptype_right - 1];
                  result[sign_result - 1] = right[sign_right - 1];
                }
            }
          else if (mpiszero(left, n_left, params_left))
            {
              if (mpisinfinity(right, n_right, params_right))
                {
                  result[fptype_result - 1] = qnancon();
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                result[fptype_result - 1] = zercon();
            }
          else if (mpiszero(right, n_right, params_right))
            {
              if (mpisinfinity(left, n_left, params_left))
                {
                  result[fptype_result - 1] = qnancon();
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                result[fptype_result - 1] = zercon();
            }
          else if (mpisinfinity(left, n_left, params_left))
            {
              if (mpiszero(right, n_right, params_right))
                {
                  result[fptype_result - 1] = qnancon();
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                result[fptype_result - 1] = infcon();
            }
          else if (mpisinfinity(right, n_right, params_right))
            {
              if (mpiszero(left, n_left, params_left))
                {
                  result[fptype_result - 1] = qnancon();
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                result[fptype_result - 1] = infcon();
            }
          else
            {
              /* We've filtered out all the special cases. Let's do some */
              /* arithmetic! */
              result[fptype_result - 1] = regcon();
              lowmul2(left, n_left, params_left, right,
                      n_right, params_right, result, n_result,
                      params_result, &iround, &iamrnd, &sticky);
              fix(result, n_result, params_result, &iround,
                  &iamrnd, &sticky);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "TIMES2", &nrec, rec);
  return;

} /* times2 */

void divide(int *left, int *right, int *result,
            int *n, int *params, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int iamrnd;
  int iround;
  int sticky, fptype;
  int *iw1, *iw2;
  char rec[125];

  /* Divides number LEFT by number RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);

      if (left[sign - 1] == right[sign - 1])
        result[sign - 1] = poscon();
      else
        result[sign - 1] = negcon();

      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(left, n, params) || mpisfake(right, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LEFT or RIGHT is corrupt or uninitialised");
        }
      else if (mpisnan(left, n, params) || mpisnan(right, n, params))
        {
          if (mpisnan(left, n, params))
            {
              result[fptype - 1] = left[fptype - 1];
              result[sign - 1] = left[sign - 1];
            }
          else
            {
              result[fptype - 1] = right[fptype - 1];
              result[sign - 1] = right[sign - 1];
            }
        }
      else if (mpiszero(left, n, params))
        {
          if (mpiszero(right, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = left[sign - 1];
            }
          else
            result[fptype - 1] = zercon();
        }
      else if (mpiszero(right, n, params))
        result[fptype - 1] = infcon();
      else if (mpisinfinity(left, n, params))
        {
          if (mpisinfinity(right, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = left[sign - 1];
            }
          else
            result[fptype - 1] = infcon();
        }
      else if (mpisinfinity(right, n, params))
        result[fptype - 1] = zercon();
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */
          iw1 = &iwork[0];
          iw2 = &iwork[(*n * 2)];
          result[fptype - 1] = regcon();
          lowdiv(left, right, n, params, result, &iround,
                 &iamrnd, &sticky, iw1, iw2);
          fix(result, n, params, &iround, &iamrnd, &sticky);
        }
    }

  *ifail = p01abf(ifail, &ierr, "DIVIDE", &nrec, rec);
  return;

} /* divide */

/* Special version of DIVIDE which handles numbers of */
/* different precision. */
void divide2(int *left, int *n_left, int *params_left,
             int *right, int *n_right, int *params_right,
             int *result, int *n_result, int *params_result,
             int *iwork, int *ifail)
{
  int nreq_result, nrec;
  int ierr;
  int radigs_right;
  int fptype_right, radix_result;
  int base_left;
  int sign_left;
  int nreq_left;
  int radigs_result;
  int fptype_result;
  int iamrnd;
  int base_right;
  int iround, radix_left;
  int sticky;
  int sign_right, *iw1, *iw2, nreq_right;
  char rec[125];
  int radigs_left, radix_right, base_result, fptype_left,
    sign_result;

  /* Divides number LEFT by number RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params_left))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_LEFT was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_right))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_RIGHT was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_result))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_RESULT was not initialised by a call of MPINIT");
    }
  else
    {
      base_left = get_base(params_left);
      base_right = get_base(params_right);
      base_result = get_base(params_result);
      radix_left = get_radix(params_left);
      radix_right = get_radix(params_right);
      radix_result = get_radix(params_result);
      radigs_left = get_radigs(params_left);
      radigs_right = get_radigs(params_right);
      radigs_result = get_radigs(params_result);
      if (base_left != base_right || base_left != base_result ||
          radix_left != radix_right || radix_left !=
          radix_result || radigs_left != radigs_right ||
          radigs_left != radigs_result)
        {
          ierr = 1;
          sprintf(rec, "Arrays PARAMS_LEFT, PARAMS_RIGHT, PARAMS_RESULT inconsistent.");
        }
      else if (mpisfake(left, n_left, params_left) ||
               mpisfake(right, n_right, params_right))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LEFT or RIGHT is corrupt or uninitialised");
        }
      else
        {
          sign_left = get_sign(params_left);
          sign_right = get_sign(params_right);
          sign_result = get_sign(params_result);
          fptype_left = get_fptype(params_left);
          fptype_right = get_fptype(params_right);
          fptype_result = get_fptype(params_result);

          if (left[sign_left - 1] == right[sign_right - 1])
            result[sign_result - 1] = poscon();
          else
            result[sign_result - 1] = negcon();

          nreq_left = get_mplen(params_left);
          nreq_right = get_mplen(params_right);
          nreq_result = get_mplen(params_result);
          if (*n_left < nreq_left)
            {
              ierr = 1;
              nrec = 1;
              sprintf(rec, "%s %d %s %d", "N_LEFT is too small. N_LEFT must be at least",
                      nreq_left, ": N_LEFT = ", *n_left);
            }
          else if (*n_right < nreq_right)
            {
              ierr = 1;
              nrec = 1;
              sprintf(rec, "%s %d %s %d", "N_RIGHT is too small. N_RIGHT must be at least",
                      nreq_right, ": N_RIGHT = ", *n_right);
            }
          else if (*n_result < nreq_result)
            {
              ierr = 1;
              nrec = 1;
              sprintf(rec, "%s %d %s %d", "N_RESULT is too small. N_RESULT must be at least",
                      nreq_result, ": N_RESULT = ", *n_result);
            }
          else if (mpisnan(left, n_left, params_left) ||
                   mpisnan(right, n_right, params_right))
            {
              if (mpisnan(left, n_left, params_left))
                {
                  result[fptype_result - 1] = left[fptype_left - 1];
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                {
                  result[fptype_result - 1] = right[fptype_right - 1];
                  result[sign_result - 1] = right[sign_right - 1];
                }
            }
          else if (mpiszero(left, n_left, params_left))
            {
              if (mpiszero(right, n_right, params_right))
                {
                  result[fptype_result - 1] = qnancon();
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                result[fptype_result - 1] = zercon();
            }
          else if (mpiszero(right, n_right, params_right))
            result[fptype_result - 1] = infcon();
          else if (mpisinfinity(left, n_left, params_left))
            {
              if (mpisinfinity(right, n_right, params_right))
                {
                  result[fptype_result - 1] = qnancon();
                  result[sign_result - 1] = left[sign_left - 1];
                }
              else
                result[fptype_result - 1] = infcon();
            }
          else if (mpisinfinity(right, n_right, params_right))
            result[fptype_result - 1] = zercon();
          else
            {
              /* We've filtered out all the special cases. Let's do some */
              /* arithmetic! */
              iw1 = &iwork[0];
              iw2 = &iwork[(*n_right * 2)];
              result[fptype_result - 1] = regcon();
              lowdiv2(left, n_left, params_left, right,
                      n_right, params_right, result, n_result,
                      params_result, &iround, &iamrnd, &sticky,
                      iw1, iw2);
              fix(result, n_result, params_result, &iround,
                  &iamrnd, &sticky);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "DIVIDE2", &nrec, rec);
  return;

} /* divide2 */

void mpiroot(int *right, int *result, int *n,
            int *params, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int iamrnd;
  int iround;
  int sticky, fptype;
  int *iw1, *iw2;
  char rec[125];

  /* Computes the inverse square root of number RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(right, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "RIGHT is corrupt or uninitialised");
        }
      else if (mpiszero(right, n, params))
        {
          REAL x; int ifl;
          createinfinity(&c_0, &x);
          dtomp(&x, result, n, params, &ifl);
          result[sign - 1] = right[sign - 1];
        }
      else if (mpisnan(right, n, params) || mpisneg(right, n, params))
        {
          if (mpisnan(right, n, params))
            result[fptype - 1] = right[fptype - 1];
          else
            result[fptype - 1] = qnancon();
          result[sign - 1] = right[sign - 1];
        }
      else if (mpisinfinity(right, n, params))
        {
          REAL x; int ifl;
          createzero(&c_0, &x);
          dtomp(&x, result, n, params, &ifl);
        }
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */

          REAL one = 1.0;
          int *mp_one, *mp_roo;
          int ifl;

          mp_roo = (int *)malloc((*n)*sizeof(int));
          mp_one = (int *)malloc((*n)*sizeof(int));
          
          iw1 = &iwork[0];
          iw2 = &iwork[(*n * 2)];
          
          result[fptype - 1] = regcon();
          result[sign - 1] = poscon();
          mp_roo[fptype - 1] = regcon();
          mp_roo[sign - 1] = poscon();
          
          lowroo(right, n, params, mp_roo, &iround, &iamrnd,
                 &sticky, iw1, iw2);
          dtomp(&one, mp_one, n, params, &ifl);
          lowdiv(mp_one, mp_roo, n, params, result, &iround,
                 &iamrnd, &sticky, iw1, iw2);
          fix(result, n, params, &iround, &iamrnd, &sticky);

          free(mp_roo);
          free(mp_one);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPIROOT", &nrec, rec);
  return;

} /* mpiroot */

void mproot(int *right, int *result, int *n,
            int *params, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int iamrnd;
  int iround;
  int sticky, fptype;
  int *iw1, *iw2;
  char rec[125];

  /* Computes the square root of number RIGHT, and sets */
  /* RESULT to be the result. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(right, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "RIGHT is corrupt or uninitialised");
        }
      else if (mpiszero(right, n, params) ||
               (mpisinfinity(right, n, params) && mpispos(right, n, params)))
        assign(result, right, n, params);
      else if (mpisnan(right, n, params) || mpisneg(right, n, params))
        {
          if (mpisnan(right, n, params))
            result[fptype - 1] = right[fptype - 1];
          else
            result[fptype - 1] = qnancon();
          result[sign - 1] = right[sign - 1];
        }
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */
          iw1 = &iwork[0];
          iw2 = &iwork[(*n * 2)];
          result[fptype - 1] = regcon();
          result[sign - 1] = poscon();
          lowroo(right, n, params, result, &iround, &iamrnd,
                 &sticky, iw1, iw2);
          fix(result, n, params, &iround, &iamrnd, &sticky);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPROOT", &nrec, rec);
  return;

} /* mproot */

void mphypot(int *xmp, int *ymp, int *result,
             int *n, int *params, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2, i_3;

  /* Local variables */
  int nrec, ierr;
  int *iw, n_long;
  int *mp1, *mp2, *mp3;
  char rec[125];
  int ifl, params_long[15];
  int sign, fptype;

  /* Computes hypot(x,y) = sqrt(x**2 + y**2), putting the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (mpisinfinity(xmp, n, params) || mpisinfinity(ymp, n, params))
    {
      /* The ISO C99 standard says that if either of x or y is infinity,
         the result is +infinity, regardless of whether the other
         argument is NaN. */
      sign = get_sign(params);
      fptype = get_fptype(params);
      result[fptype - 1] = infcon();
      result[sign - 1] = poscon();
    }
  else
    {
      /* Construct a number system with twice as much precision */
      /* and range to avoid intermediate overflows */
      i_1 = get_mantis(params) * 2;
      i_2 = get_emin(params) * 2;
      i_3 = get_emax(params) * 2;
      mpinitcopy(params, &i_1, &i_2, &i_3, &n_long, params_long,
                 &ifl);

      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[n_long];
      mp3 = &iwork[(n_long * 2)];
      iw = mp3;

      /* Do the obvious thing = hypot = sqrt(x**2 + y**2) */
      times2(xmp, n, params, xmp, n, params, mp2,
             &n_long, params_long, &ifl);
      times2(ymp, n, params, ymp, n, params, mp3,
             &n_long, params_long, &ifl);
      plus(mp2, mp3, mp1, &n_long, params_long,
           &ifl);
      mproot(mp1, mp2, &n_long, params_long, iw, &ifl);

      /* Copy the result back from extended precision to regular. */
      /* The result may overflow but that's OK. */
      mpcopy2(mp2, &n_long, params_long, result, n,
              params, &ifl);
    }

  *ifail = p01abf(ifail, &ierr, "MPHYPOT", &nrec, rec);
  return;

} /* mphypot */

void mpfma(int *xmp, int *ymp, int *zmp, int
           *result, int *n, int *params, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2, i_3;

  /* Local variables */
  int nrec, ierr;
  int n_long;
  int *mp1, *mp2, *mp3;
  char rec[125];
  int ifl, params_long[15];

  /* Computes x*y+z with no intermediate rounding error in the */
  /* multiplication. */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      /* Construct a number system with twice as much precision */
      /* and range to avoid intermediate overflows */
      i_1 = get_mantis(params) * 2;
      i_2 = get_emin(params) * 2;
      i_3 = get_emax(params) * 2;
      mpinitcopy(params, &i_1, &i_2, &i_3, &n_long, params_long,
                 &ifl);

      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[n_long];
      mp3 = &iwork[(n_long * 2)];

      /* Do the obvious thing. The result of the multiplication */
      /* is exact because it's stored in twice the precision. */
      times2(xmp, n, params, ymp, n, params, mp1,
             &n_long, params_long, &ifl);
      mpcopy2(zmp, n, params, mp2, &n_long,
              params_long, &ifl);
      plus(mp1, mp2, mp3, &n_long, params_long,
           &ifl);

      /* Copy the result back from extended precision to regular. */
      /* The result may overflow but that's OK. */
      mpcopy2(mp3, &n_long, params_long, result, n,
              params, &ifl);
    }

  *ifail = p01abf(ifail, &ierr, "MPFMA", &nrec, rec);
  return;

} /* mpfma */

void mpipow(int *k, int *xmp, int *n, int *params, int *result, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int i;
  int fptype;
  char rec[125];
  int ifl;

  /* Raise number XMP to power K */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (*k == 0)
        {
          REAL z = 1.0;
          dtomp(&z, result, n, params, &ifl);
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = xmp[fptype - 1];
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          REAL z;
          if(*k < 0)
            {
              createinfinity(&c_0, &z);
              dtomp(&z, result, n, params, &ifl);
              result[sign - 1] = xmp[sign - 1];
            }
          else
            assign(result, xmp, n, params);

          if (*k % 2 == 0)
            result[sign - 1] = poscon();
        }
      else if (mpisinfinity(xmp, n, params))
        {
          REAL z;
          if(*k < 0)
            {
              createzero(&c_0, &z);
              dtomp(&z, result, n, params, &ifl);
              result[sign - 1] = xmp[sign - 1];
            }
          else
            assign(result, xmp, n, params);

          if (*k % 2 == 0)
            result[sign - 1] = poscon();
        }
      else
        {
          /* We've filtered out all the special cases. Let's do some */
          /* arithmetic! */

          ifl = 0;

          /* Set RESULT = 1.0 */
          itomp(&c_1, result, n, params, &ifl);

          /* Set IWORK(1) = ABS(XMP) */
          assign(iwork, xmp, n, params);
          iwork[sign - 1] = poscon();

          /* Use the "binary split" method or whatever it's called. */
          i = myiabs(*k);

          /* Set the sign of the result */
          if (i % 2 == 1 && mpisneg(xmp, n, params))
            result[sign - 1] = negcon();
          else
            result[sign - 1] = poscon();

          if(i < -2147483647)
            i = 2147483647;

        L20:
          if (i % 2 == 1)
            {
              /* Set RESULT = RESULT * XMP */
              times(result, iwork, &iwork[(*n * 2)], n,
                    params, &ifl);
              assign(result, &iwork[(*n * 2)], n, params);
            }
          if (mpisreg(result, n, params))
            {
              /* The result is still in range. */
              i /= 2;
              if (i > 0)
                {
                  assign(&iwork[(*n * 2)], iwork, n, params);
                  assign(&iwork[(*n * 4)], iwork, n, params);
                  times(&iwork[(*n * 2)], &iwork[(*n * 4)],
                        iwork, n, params, &ifl);
                  goto L20;
                }
            }

          /* Reciprocate if K was negative */
          if (*k < 0)
            {
              itomp(&c_1, &iwork[(*n * 8)], n, params, &ifl);
              divide(&iwork[(*n * 8)], result, iwork, n,
                     params, &iwork[(*n * 2)], &ifl);
              assign(result, iwork, n, params);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPIPOW", &nrec, rec);
  return;

} /* mpipow */


#ifdef WINDOWS
#define POW_ONE_INF_IS_NAN
#endif

void mppow(int *xmp, int *ymp, int *logbase,
           int *n, int *params, int *result, int *iwork, int
           *ifail)
{
  int nrec, sign, ierr;
  int nreq;
  int ympisint;
  int oddly;
  int fptype;
  int *mp1, *mp2, *iw1, *iw2, *mp3, *mp4;
  char rec[125];
  int ifl;

  /* Raise number XMP to power YMP */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisnan(ymp, n, params))
        {
          if (mpisnan(xmp, n, params))
            {
#ifdef WINDOWS
              /* This is what WIN32 does - not currently WIN64 */
              /* If x is NaN, return NaN */
              result[fptype - 1] = xmp[fptype - 1];
              result[sign - 1] = xmp[sign - 1];
#else
              /* If x is NaN but y is zero, return 1.0 */
              itomp(&c_1, result, n, params, &ifl);
              if (mpiszero(ymp, n, params))
                itomp(&c_1, result, n, params, &ifl);
              else                
                {
                  result[fptype - 1] = xmp[fptype - 1];
                  result[sign - 1] = xmp[sign - 1];
                }
#endif
            }
          else
            {
#ifdef WINDOWS
              /* If y is NaN, return NaN */
              result[fptype - 1] = ymp[fptype - 1];
              result[sign - 1] = ymp[sign - 1];
#else
              /* If y is NaN but x is 1.0, return 1.0 */
              itomp(&c_1, result, n, params, &ifl);
              if (mpeq(xmp, result, n, params))
                itomp(&c_1, result, n, params, &ifl);
              else
                {
                  result[fptype - 1] = ymp[fptype - 1];
                  result[sign - 1] = ymp[sign - 1];
                }
#endif
            }
        }
      else if (mpiszero(ymp, n, params))
        {
          /* This even if XMP is zero or infinity */
          itomp(&c_1, result, n, params, &ifl);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];
          mp4 = &iwork[*n * 3];
          iw1 = mp4;
          iw2 = mp3;

          /* See whether YMP is an int. */
          oddly = 0;
          if (mpisinfinity(ymp, n, params))
            ympisint = 0;
          else
            {
              ifl = 1;
              oddly = isanint(ymp, n, params, &ifl);
              ympisint = oddly != 0;
            }

          if (mpiszero(xmp, n, params))
            {
              if (mpispos(xmp, n, params))
                {
                  /* xmp is positive zero */
                  if (mpisneg(ymp, n, params))
                    {
                      /* ymp is negative */
                      result[fptype - 1] = infcon();
                      result[sign - 1] = poscon();
                    }
                  else
                    {
                      /* ymp is positive */
                      result[fptype - 1] = zercon();
                      result[sign - 1] = poscon();
                    }
                }
              else
                {
                  /* xmp is negative zero */
                  if (mpisneg(ymp, n, params))
                    {
                      /* ymp is negative */
                      if (oddly == 1)
                        {
                          /* ymp is an odd integer */
                          result[fptype - 1] = infcon();
                          result[sign - 1] = negcon();
                        }
                      else
                        {
                          result[fptype - 1] = infcon();
                          result[sign - 1] = poscon();
                        }
                    }
                  else
                    {
                      /* ymp is positive */
                      if (oddly == 1)
                        {
                          /* ymp is an odd integer */
                          result[fptype - 1] = zercon();
                          result[sign - 1] = negcon();
                        }
                      else
                        {
                          result[fptype - 1] = zercon();
                          result[sign - 1] = poscon();
                        }
                    }
                }
            }
          else if (ympisint && !mpisinfinity(xmp, n, params))
            {
              /* ymp is an integer and x is finite */
              int ii;
              /* See if YMP is a small int */
              ifl = 1;
              ii = mptoi(ymp, n, params, &ifl);
              if (ifl == 0)
                mpipow(&ii, xmp, n, params, result, mp1, &ifl);
              else
                {
                  /* YMP is a large int; compute the result as */
                  /* exp(log(xmp)*ymp). */
                  /* We first replace XMP by its absolute value to avoid */
                  /* taking log of a negative number, then negate the result */
                  /* if necessary. */

                  assign(mp3, xmp, n, params);
                  mpabs(mp3, n, params, &ifl);
                  mplog(mp3, logbase, n, params, mp1, iw1, &ifl);
                  times(mp1, ymp, mp2, n, params,
                        &ifl);
                  mpexp(mp2, n, params, result, iw2, &ifl);
                  if (mpisneg(xmp, n, params) && oddly == 1)
                    mpnegate(result, n, params, &ifl);
                }
            }
          else
            {
              /* YMP is not an int or xmp is infinite. */
              if (mpisinfinity(ymp, n, params))
                {
                  /* libms seem to define infinity as an even int for the
                     purposes of the pow function. We'll do the same. */
                  if (mpispos(xmp, n, params))
                    {
                      /* XMP is positive. */
                      /* Set IWORK(MP1) = 1.0 */
                      itomp(&c_1, mp1, n, params, &ifl);
                      if (mpisneg(ymp, n, params))
                        {
                          /* YMP is -infinity */
                          if (mplt(xmp, mp1, n, params))
                            {
                              result[fptype - 1] = infcon();
                              result[sign - 1] = poscon();
                            }
                          else if (mpeq(xmp, mp1, n, params))
                            {
#ifdef POW_ONE_INF_IS_NAN
                              /* Many define 1.0**infinity as NaN */
                              result[fptype - 1] = qnancon();
                              result[sign - 1] = poscon();
#else
                              assign(result, mp1, n, params);
#endif
                            }
                          else
                            {
                              result[fptype - 1] = zercon();
                              result[sign - 1] = poscon();
                            }
                        }
                      else
                        {
                          /* YMP is +infinity */
                          if (mplt(xmp, mp1, n, params))
                            {
                              result[fptype - 1] = zercon();
                              result[sign - 1] = poscon();
                            }
                          else if (mpeq(xmp, mp1, n, params))
                            {
#ifdef POW_ONE_INF_IS_NAN
                              /* Many define 1.0**infinity as NaN */
                              result[fptype - 1] = qnancon();
                              result[sign - 1] = poscon();
#else
                              assign(result, mp1, n, params);
#endif
                            }
                          else
                            {
                              result[fptype - 1] = infcon();
                              result[sign - 1] = poscon();
                            }
                        }
                    }
                  else
                    {
                      /* XMP is negative. */
                      /* Set IWORK(MP1) = -1.0 */
                      itomp(&c_n1, mp1, n, params, &ifl);
                      if (mpisneg(ymp, n, params))
                        {
                          /* YMP is -infinity */
                          if (mplt(xmp, mp1, n, params))
                            {
                              result[fptype - 1] = zercon();
                              result[sign - 1] = poscon();
                            }
                          else if (mpeq(xmp, mp1, n, params))
                            {
#ifdef POW_ONE_INF_IS_NAN
                              /* Many define -1.0**infinity as NaN */
                              result[fptype - 1] = qnancon();
                              result[sign - 1] = poscon();
#else
                              /* We stick to ISO C99 and return +1.0 */
                              assign(result, mp1, n, params);
                              result[sign - 1] = poscon();
#endif
                            }
                          else
                            {
                              result[fptype - 1] = infcon();
                              result[sign - 1] = poscon();
                            }
                        }
                      else
                        {
                          /* YMP is +infinity */
                          if (mplt(xmp, mp1, n, params))
                            {
                              result[fptype - 1] = infcon();
                              result[sign - 1] = poscon();
                            }
                          else if (mpeq(xmp, mp1, n, params))
                            {
#ifdef POW_ONE_INF_IS_NAN
                              /* Many define -1.0**infinity as NaN */
                              result[fptype - 1] = qnancon();
                              result[sign - 1] = poscon();
#else
                              /* We stick to ISO C99 and return +1.0 */
                              assign(result, mp1, n, params);
                              result[sign - 1] = poscon();
#endif
                            }
                          else
                            {
                              result[fptype - 1] = zercon();
                              result[sign - 1] = poscon();
                            }
                        }
                    }
                }
              else if (mpisinfinity(xmp, n, params))
                {
                  if (mpispos(xmp, n, params))
                    {
                      /* xmp is positive infinity */
                      if (mpiszero(ymp, n, params))
                        itomp(&c_1, result, n, params, &ifl);
                      else if (mpispos(ymp, n, params))
                        {
                          result[fptype - 1] = infcon();
                          result[sign - 1] = poscon();
                        }
                      else
                        {
                          result[fptype - 1] = zercon();
                          result[sign - 1] = poscon();
                        }
                    }
                  else
                    {
                      /* xmp is negative infinity */
                      if (mpiszero(ymp, n, params))
                        itomp(&c_1, result, n, params, &ifl);
                      else if (mpispos(ymp, n, params))
                        {
                          if (oddly == 1)
                            {
                              /* ymp is an odd integer */
                              result[fptype - 1] = infcon();
                              result[sign - 1] = negcon();
                            }
                          else
                            {
                              result[fptype - 1] = infcon();
                              result[sign - 1] = poscon();
                            }
                        }
                      else
                        {
                          if (oddly == 1)
                            {
                              /* ymp is an odd integer */
                              result[fptype - 1] = zercon();
                              result[sign - 1] = negcon();
                            }
                          else
                            {
                              result[fptype - 1] = zercon();
                              result[sign - 1] = poscon();
                            }
                        }
                    }
                }
              else if (mpisneg(xmp, n, params))
                {
                  /* XMP is negative - illegal when YMP is not an int. */
                  result[fptype - 1] = qnancon();
                  result[sign - 1] = xmp[sign - 1];
                }
              else
                {
                  /* Just compute the result as exp(log(xmp)*ymp). */

                  mplog(xmp, logbase, n, params, mp1,
                        iw2, &ifl);
                  times(mp1, ymp, mp2, n, params,
                        &ifl);
                  mpexp(mp2, n, params, result, iw2, &ifl);
                }
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPPOW", &nrec, rec);
  return;

} /* mppow */

void mpfmod(int *xmp, int *ymp, int *n, int *params,
            int *result, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec, sign, ierr, iexo, nreq;
  int bigman;
  int n_long;
  int fptype;
  int *mywork;
  char rec[125];
  int ifl, bigemin, bigemax;
  int params_long[15];

  /* The fmod()  function computes the remainder of dividing x */
  /* by y. The return value is x - n * y, where n is the quotient */
  /* of x / y, rounded towards zero to an integer. */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisnan(ymp, n, params))
        {
          if (mpisnan(xmp, n, params))
            {
              result[fptype - 1] = xmp[fptype - 1];
              result[sign - 1] = xmp[sign - 1];
            }
          else
            {
              result[fptype - 1] = ymp[fptype - 1];
              result[sign - 1] = ymp[sign - 1];
            }
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(ymp, n, params))
        {
#ifdef WINDOWS
          /* VC++ says this should be "indefinite" */
          result[fptype - 1] = qnancon();
          result[sign - 1] = negcon();
#else
          /* Others say x */
          assign(result, xmp, n, params);
#endif
        }
      else if (mpiszero(xmp, n, params))
        {
          if (mpiszero(ymp, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = xmp[sign - 1];
            }
          else
            assign(result, xmp, n, params);
        }
      else if (mpiszero(ymp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = ymp[sign - 1];
        }
      else
        {
          /* Construct a number system with huge precision and range. */
          /* We need the huge precision to make sure we can compute */
          /* int(xmp/ymp) accurately no matter what the sizes of */
          /* xmp and ymp. */
          /* bigman = 2*GET_MANTIS(PARAMS) + GET_EMAX(params) - */
          /* *         get_emin(params) */
          i_1 = get_mantis(params);
          i_2 = (get_mantis(params) * 2) + xmp[iexo - 1] - ymp[iexo - 1];
          bigman = myimax(i_1, i_2);
          i_1 = get_emin(params);
          i_2 = bigman * -2;
          bigemin = myimin(i_1, i_2);
          i_1 = get_emax(params);
          i_2 = bigman * 2;
          bigemax = myimax(i_1, i_2);
          mpinitcopy(params, &bigman, &bigemin, &bigemax, &n_long,
                     params_long, &ifl);
          mywork = (int *)malloc(n_long * 7 * sizeof(int));
          if (!mywork)
            {
              ierr = 3;
              nrec = 1;
              sprintf(rec, "Failed to allocate mywork workspace");
            }
          else
            {
              divide2(xmp, n, params, ymp, n, params,
                      mywork, &n_long, params_long, &mywork[n_long],
                      &ifl);
              mptrunc(mywork, &mywork[n_long], &n_long, params_long,
                      &ifl);
              times2(&mywork[n_long], &n_long, params_long, ymp,
                     n, params, mywork, &n_long, params_long, &ifl);
              mpcopy2(xmp, n, params, &mywork[n_long], &n_long,
                      params_long, &ifl);
              minus(&mywork[n_long], mywork, &mywork[n_long * 2],
                    &n_long, params_long, &ifl);
              mpcopy2(&mywork[n_long * 2], &n_long, params_long,
                      result, n, params, &ifl);
            }
          free(mywork);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPFMOD", &nrec, rec);
  return;

} /* mpfmod */

void mpfabs(int *xmp, int *ymp, int *n, int *params, int *ifail)
{
  int nrec, ierr, nreq, ifl;
  char rec[125];
 
  /* Sets YMP to be the absolute value of XMP */
 
  /* Function Body */
  ierr = 0;
  nrec = 0;
  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else
    {
      assign(ymp, xmp, n, params);
      ifl = 0;
      mpabs(ymp, n, params, &ifl);
    }
 
  *ifail = p01abf(ifail, &ierr, "MPFABS", &nrec, rec);
  return;
 
} /* mpfabs */

void mpremainder(int *xmp, int *ymp, int *n, int *params,
                 int *result, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec, sign, ierr, iexo, nreq;
  int bigman;
  int n_long;
  int fptype;
  int *mywork;
  char rec[125];
  int ifl, bigemin, bigemax;
  int params_long[15];

  /* The remainder() function computes the remainder of dividing x */
  /* by y. The return value is x - n * y, where n is the quotient */
  /* of x / y, rounded to the nearest integer. */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisnan(ymp, n, params))
        {
          if (mpisnan(xmp, n, params))
            {
              result[fptype - 1] = xmp[fptype - 1];
              result[sign - 1] = xmp[sign - 1];
            }
          else
            {
              result[fptype - 1] = ymp[fptype - 1];
              result[sign - 1] = ymp[sign - 1];
            }
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(ymp, n, params))
        {
#ifdef WINDOWS
          /* VC++ says this should be "indefinite" */
          result[fptype - 1] = qnancon();
          result[sign - 1] = negcon();
#else
          /* Others say x */
          assign(result, xmp, n, params);
#endif
        }
      else if (mpiszero(xmp, n, params))
        {
          if (mpiszero(ymp, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = xmp[sign - 1];
            }
          else
            assign(result, xmp, n, params);
        }
      else if (mpiszero(ymp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = ymp[sign - 1];
        }
      else
        {
          /* Construct a number system with huge precision and range. */
          /* We need the huge precision to make sure we can compute */
          /* int(xmp/ymp) accurately no matter what the sizes of */
          /* xmp and ymp. */
          /* bigman = 2*GET_MANTIS(PARAMS) + GET_EMAX(params) - */
          /* *         get_emin(params) */
          i_1 = get_mantis(params);
          i_2 = (get_mantis(params) * 2) + xmp[iexo - 1] - ymp[iexo - 1];
          bigman = myimax(i_1, i_2);
          i_1 = get_emin(params);
          i_2 = bigman * -2;
          bigemin = myimin(i_1, i_2);
          i_1 = get_emax(params);
          i_2 = bigman * 2;
          bigemax = myimax(i_1, i_2);
          mpinitcopy(params, &bigman, &bigemin, &bigemax, &n_long,
                     params_long, &ifl);
          mywork = (int *)malloc(n_long * 7 * sizeof(int));
          if (!mywork)
            {
              ierr = 3;
              nrec = 1;
              sprintf(rec, "Failed to allocate mywork workspace");
            }
          else
            {
              divide2(xmp, n, params, ymp, n, params,
                      mywork, &n_long, params_long, &mywork[n_long],
                      &ifl);
              mprint(mywork, &mywork[n_long], &n_long, params_long,
                     &ifl);
              times2(&mywork[n_long], &n_long, params_long, ymp,
                     n, params, mywork, &n_long, params_long, &ifl);
              mpcopy2(xmp, n, params, &mywork[n_long], &n_long,
                      params_long, &ifl);
              minus(&mywork[n_long], mywork, &mywork[n_long * 2],
                    &n_long, params_long, &ifl);
              mpcopy2(&mywork[n_long * 2], &n_long, params_long,
                      result, n, params, &ifl);
            }
          free(mywork);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPREMAINDER", &nrec, rec);
  return;

} /* mpremainder */

void mpremquo(int *xmp, int *ymp, int *qmp, int *n, int *params,
                 int *result, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec, sign, ierr, iexo, nreq;
  int bigman;
  int n_long;
  int fptype;
  int *mywork;
  char rec[125];
  int ifl, bigemin, bigemax;
  int params_long[15];

  /* The remainder() function computes the remainder of dividing x */
  /* by y. The return value is x - n * y, where n is the quotient */
  /* of x / y, rounded to the nearest integer. */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);

	  if (mpisfake(qmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "QMP is corrupt or uninitialised");
        }

	  /* set quo as positive 0 for exception case */
	  {
		int q=0;
		itomp(&q, qmp, n, params, &ifl);
	  }

      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisnan(ymp, n, params))
        {
          if (mpisnan(xmp, n, params))
            {
              result[fptype - 1] = xmp[fptype - 1];
              result[sign - 1] = xmp[sign - 1];
            }
          else
            {
              result[fptype - 1] = ymp[fptype - 1];
              result[sign - 1] = ymp[sign - 1];
            }
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(ymp, n, params))
        {
#ifdef WINDOWS
          /* VC++ says this should be "indefinite" */
          result[fptype - 1] = qnancon();
          result[sign - 1] = negcon();
#else
          /* Others say x */
          assign(result, xmp, n, params);
#endif
        }
      else if (mpiszero(xmp, n, params))
        {
          if (mpiszero(ymp, n, params))
            {
              result[fptype - 1] = qnancon();
              result[sign - 1] = xmp[sign - 1];
            }
          else
            assign(result, xmp, n, params);
        }
      else if (mpiszero(ymp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = ymp[sign - 1];
        }
      else
        {
          /* Construct a number system with huge precision and range. */
          /* We need the huge precision to make sure we can compute */
          /* int(xmp/ymp) accurately no matter what the sizes of */
          /* xmp and ymp. */
          /* bigman = 2*GET_MANTIS(PARAMS) + GET_EMAX(params) - */
          /* *         get_emin(params) */
          i_1 = get_mantis(params);
          i_2 = (get_mantis(params) * 2) + xmp[iexo - 1] - ymp[iexo - 1];
          bigman = myimax(i_1, i_2);
          i_1 = get_emin(params);
          i_2 = bigman * -2;
          bigemin = myimin(i_1, i_2);
          i_1 = get_emax(params);
          i_2 = bigman * 2;
          bigemax = myimax(i_1, i_2);
          mpinitcopy(params, &bigman, &bigemin, &bigemax, &n_long,
                     params_long, &ifl);
          mywork = (int *)malloc(n_long * 7 * sizeof(int));
          if (!mywork)
            {
              ierr = 3;
              nrec = 1;
              sprintf(rec, "Failed to allocate mywork workspace");
            }
          else
            {
              divide2(xmp, n, params, ymp, n, params,
                      mywork, &n_long, params_long, &mywork[n_long],
                      &ifl);
              mprint(mywork, &mywork[n_long], &n_long, params_long,
                     &ifl);
              mpcopy2(&mywork[n_long], &n_long, params_long, qmp, n, params, &ifl);
              times2(&mywork[n_long], &n_long, params_long, ymp,
                     n, params, mywork, &n_long, params_long, &ifl);
              mpcopy2(xmp, n, params, &mywork[n_long], &n_long,
                      params_long, &ifl);
              minus(&mywork[n_long], mywork, &mywork[n_long * 2],
                    &n_long, params_long, &ifl);
              mpcopy2(&mywork[n_long * 2], &n_long, params_long,
                      result, n, params, &ifl);
            }
          free(mywork);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPREMQUO", &nrec, rec);
  return;

} /* mpremquo */

void mpfmin(int *xmp, int *ymp, int *n, int *params, int *result, int *ifail)
{
  int nrec;
  int ierr, nreq;
  char rec[125];

  /* Returns the smaller (more negative) of x and y. */
  /* NaNs are treated as missing values: if one argument is NaN, */
  /* the other argument is returned. If both arguments are NaN, */
  /* the first argument is returned. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");

          /* The order of these NaN checks is important */
        }
      else if (mpisnan(ymp, n, params))
        assign(result, xmp, n, params);
      else if (mpisnan(xmp, n, params))
        assign(result, ymp, n, params);
      else if (mpiszero(xmp, n, params) && mpiszero(ymp, n, params))
        {
          /* Both numbers are zero. If either is negative,
             return -zero, otherwise return +zero */
          if (mpisneg(xmp, n, params))
            assign(result, xmp, n, params);
          else
            assign(result, ymp, n, params);
        }
      else if (mple(xmp, ymp, n, params))
        assign(result, xmp, n, params);
      else
        assign(result, ymp, n, params);
    }

  *ifail = p01abf(ifail, &ierr, "MPFMIN", &nrec, rec);
  return;

} /* mpfmin */

void mpfmax(int *xmp, int *ymp, int *n, int *params, int *result, int *ifail)
{
  int nrec;
  int ierr, nreq;
  char rec[125];

  /* Returns the larger (more positive) of x and y. */
  /* NaNs are treated as missing values: if one argument is NaN, */
  /* the other argument is returned. If both arguments are NaN, */
  /* the first argument is returned. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");

          /* The order of these NaN checks is important */
        }
      else if (mpisnan(ymp, n, params))
        assign(result, xmp, n, params);
      else if (mpisnan(xmp, n, params))
        assign(result, ymp, n, params);
      else if (mpiszero(xmp, n, params) && mpiszero(ymp, n, params))
        {
          /* Both numbers are zero. If either is positive,
             return +zero, otherwise return -zero */
          if (mpispos(xmp, n, params))
            assign(result, xmp, n, params);
          else
            assign(result, ymp, n, params);
        }
      else if (mpge(xmp, ymp, n, params))
        assign(result, xmp, n, params);
      else
        assign(result, ymp, n, params);
    }

  *ifail = p01abf(ifail, &ierr, "MPFMAX", &nrec, rec);
  return;

} /* mpfmax */

void mpfdim(int *xmp, int *ymp, int *n, int *params, int *result, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  char rec[125];
  int ifl;

  /* Returns the positive difference between x and y. */
  /* If x <= y, returns +0.0. */
  /* If x > y, returns x - y. */
  /* If one argument is NaN, that argument is returned. */
  /* If both arguments are NaN, the first argument is returned. */

  ierr = 0;
  nrec = 0;
  ifl = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(ymp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "YMP is corrupt or uninitialised");

          /* The order of these NaN checks is important */
        }
      else if (mpisnan(xmp, n, params))
        assign(result, xmp, n, params);
      else if (mpisnan(ymp, n, params))
        assign(result, ymp, n, params);
      else if (mpisinfinity(xmp, n, params) && mpisinfinity(ymp, n, params))
        {
          if (mpispos(xmp, n, params) == mpispos(ymp, n, params))
            {
              /* Infinities of same sign yield NaN */
              /*
              result[fptype - 1] = qnancon();
              result[sign - 1] = poscon();
              */

              REAL z;
              createzero(&c_0, &z);
              dtomp(&z, result, n, params, &ifl);
            }
          else if (mpispos(xmp, n, params))
            /* +infinity take away -infinity yields +infinity */
            assign(result, xmp, n, params);
          else
            {
              /* -infinity take away +infinity yields zero */
              result[fptype - 1] = zercon();
              result[sign - 1] = poscon();
            }
        }
      else if (mple(xmp, ymp, n, params))
        {
          /* Return positive zero */
          result[fptype - 1] = zercon();
          result[sign - 1] = poscon();
        }
      else
        minus(xmp, ymp, result, n, params, &ifl);
    }

  *ifail = p01abf(ifail, &ierr, "MPFDIM", &nrec, rec);
  return;

} /* mpfdim */

void mpfract(int *xmp, int *n, int *params, int *ymp, int *iymp, int *ifail)
{
  int nrec;
  int ierr, nreq;
  char rec[125];
  int ifl = 0;

  /* Returns the positive fraction and integer part of x */
  /* fraction is always +ve or 0, but never greater or equal to 1.0 */
  /* integer part is floor of x */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          assign(ymp,  xmp, n, params);
          assign(iymp, xmp, n, params);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          REAL zr;
          if (mpisneg(xmp, n, params))
            createzero(&c_1, &zr);
          else
            createzero(&c_0, &zr);
       
          dtomp(&zr, ymp, n, params, &ifl);
          assign(iymp, xmp, n, params);
        }
      else if (mpiszero(xmp, n, params))
        {
          assign(ymp,  xmp, n, params);
          assign(iymp, xmp, n, params);
        }
      else
        {
          REAL max_fract;
          int *mp_max_fract, *result_minus, *iwork;

          mp_max_fract = (int *)malloc((*n)*sizeof(int));
          result_minus = (int *)malloc((*n)*sizeof(int));

#ifdef WINDOWS
          max_fract = ((sizeof(REAL) == 4) ? 0.99999994f : 0.99999999999999989);
#else
          max_fract = ((sizeof(REAL) == 4) ? 0x1.fffffep-1f : 0x1.fffffffffffffp-1);
#endif

          dtomp(&max_fract, mp_max_fract, n, params, &ifl);

          iwork = (int *)malloc(2 * (*n)*sizeof(int));
          mpfloor(xmp, iymp, n, params, iwork, &ifl);
          free(iwork);

          minus(xmp, iymp, result_minus, n, params, &ifl);
          mpfmin(result_minus, mp_max_fract, n, params, ymp, &ifl);

          free(mp_max_fract);
          free(result_minus);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPFRACT", &nrec, rec);
  return;

} /* mpfract */

void mpfrexp(int *xmp, int *ymp, int *iexp, int *n, int *params, int *ifail)
{
  int nrec;
  int ierr, nreq;
  char rec[125];

  /* Returns the mantissa in the range [1/2, 1) and exponent of x */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params) || mpiszero(xmp, n, params))
        {
          assign(ymp,  xmp, n, params);
          *iexp = 0;
        }
      else
        {
          int iexo;
          
          assign(ymp, xmp, n, params);
          iexo = get_iexo(params);
          *iexp = ymp[iexo - 1];
          ymp[iexo - 1] = 0;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPFREXP", &nrec, rec);
  return;

} /* mpfrexp */

void mpldexp(int *xmp, int expn, int *ymp, int *n, int *params, int *ifail)
{
  int nrec;
  int ierr, nreq;
  char rec[125];

  /* Returns 2^N multiplied by x */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params) || mpiszero(xmp, n, params))
        {
          assign(ymp,  xmp, n, params);
        }
      else
        {
          int iexo, fptype;
          
          assign(ymp, xmp, n, params);
          fptype = get_fptype(params);
          iexo = get_iexo(params);
          ymp[iexo - 1] = xmp[iexo - 1] + expn;

          if((ymp[iexo - 1] < get_emin(params)) || (expn < 2*get_emin(params)))
            {
              ymp[iexo - 1] = get_emin(params);
              ymp[fptype - 1] = zercon();
            }

          if((ymp[iexo - 1] > get_emax(params)) || (expn > 2*get_emax(params)))
            {
              ymp[iexo - 1] = get_emax(params);
              ymp[fptype - 1] = infcon();
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPLDEXP", &nrec, rec);
  return;

} /* mpldexp */

char itoc(int *i)
{
  char ret_val;
  if (*i == 0)
    ret_val = '0';
  else if (*i == 1)
    ret_val = '1';
  else if (*i == 2)
    ret_val = '2';
  else if (*i == 3)
    ret_val = '3';
  else if (*i == 4)
    ret_val = '4';
  else if (*i == 5)
    ret_val = '5';
  else if (*i == 6)
    ret_val = '6';
  else if (*i == 7)
    ret_val = '7';
  else if (*i == 8)
    ret_val = '8';
  else if (*i == 9)
    ret_val = '9';
  else
    ret_val = '?';
  return ret_val;
} /* itoc */

char *itoc10(int *i, int *width)
{
  /* System generated locals */
  static char retval[11];

  /* Local variables */
  int j, k, n;

  n = *i;
  for (k = 1; k <= *width; ++k)
    {
      j = n % 10;
      retval[*width + 1 - k - 1] = itoc(&j);
      n /= 10;
    }
  retval[k-1] = '\0';
  return retval;
} /* itoc10 */

int mptoi(int *xmp, int *n, int *params, int *ifail)
{
  /* System generated locals */
  int ret_val, i_1;

  /* Local variables */
  int base, nrec;
  int ierr, ires, iexo, nreq;
  int i;
  int k, r;
  int wplac, radix;
  int br;
  int ov;
  int radigs;
  int ovflow, add;
  char rec[125];
  int wplaces;

  /* Return int part of number XMP */

  ierr = 0;
  nrec = 0;
  ires = 0;
  ovflow = x02baf();
  iexo = get_iexo(params);
  nreq = get_mplen(params);
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (*n < nreq)
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else if (mpisnan(xmp, n, params))
    {
      ierr = 3;
      nrec = 1;
      sprintf(rec, "XMP is NaN");
    }
  else if (mpisinfinity(xmp, n, params))
    {
      ierr = 4;
      nrec = 1;
      sprintf(rec, "XMP is infinity");
      ires = ovflow;
    }
  else if (mpiszero(xmp, n, params))
    ires = 0;
  else
    {
      base = get_base(params);
      radix = get_radix(params);
      radigs = get_radigs(params);
      wplaces = get_wplaces(params);
      wplac = xmp[wplaces - 1];
      if (xmp[iexo - 1] <= 0)
        ires = 0;
      else
        {
          ov = 0;
          k = xmp[iexo - 1] / radigs;
          r = xmp[iexo - 1] - k * radigs;
          ires = 0;
          i_1 = k;
          for (i = 1; i <= i_1; ++i)
            {
              if (i <= wplac)
                add = xmp[i - 1];
              else
                add = 0;
              if (ires > ovflow / radix)
                ov = 1;
              else
                ires = ires * radix + add;
            }
          if (i <= wplac)
            add = xmp[i - 1];
          else
            add = 0;
          br = ipower(&base, r);
          if (ires > ovflow / br)
            ov = 1;
          else
            ires = ires * br + br * add / radix;

          if (ov)
            {
              ierr = 4;
              nrec = 1;
              sprintf(rec, "XMP overflows int range");
              ires = ovflow;
            }
        }
    }

  if (mpisneg(xmp, n, params))
    ires = -ires;

  ret_val = ires;

  *ifail = p01abf(ifail, &ierr, "MPTOI", &nrec, rec);
  return ret_val;

} /* mptoi */

/* Convert number XMP into a decimal string, C. The argument */
/* C is placed at the end of the list so that Fortran */
/* compilers will all do the same thing with the invisible */
/* length argument. */
void mp2dec(int *xmp, int *n, int *params,
            int *digits, int *iwork, int *ifail, char *c, int c_len)
{
  /* System generated locals */
  int i_1, i_2;
  char ch_2;

  /* Local variables */
  int base, lenc;
  char *digs;
  int nrec, emin;
  int emax;
  int sign, ierr, cpos, iexo, nreq;
  int ipos, expo, wplaces10;
  int n10_round, mp_10_1, mp_10_2, mp_10_3, mp_10_4;
  int params_extra[15], mantis_extra, d, i;
  int base10, p, r, emin10, emax10;
  int sign10;
  int last10, iexo10, ndigs;
  int mpex_1, mpex_2, mpex_3, mpex_4, params10_round[15], n10, *iw;
  int wplac10, decexp, radix10;
  int mantis;
  int iexo_extra;
  char rec[125];
  int ifl, places10, radigs10, pos, params10[15], mantis10, n_extra,
    fptype10;
  int emin_extra, emax_extra;

  ierr = 0;
  nrec = 0;
  lenc = c_len;
  for (i = 0; i < lenc; i++)
    c[i] = ' ';
  c[lenc] = '\0';
  iexo = get_iexo(params);
  nreq = get_mplen(params);
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (*n < nreq)
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 3;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else if (*digits < 1 || lenc < *digits + 8)
    {
      ierr = 4;
      nrec = 1;
      sprintf(rec, "%s %d", "DIGITS < 1 or string C is too short: LEN(C) = ",
              lenc);
    }
  else if (mpisnan(xmp, n, params))
    strcpy(c, "NaN");
  else if (mpisinfinity(xmp, n, params))
    {
      if (mpispos(xmp, n, params))
        strcpy(c, "+Infinity");
      else
        strcpy(c, "-Infinity");
    }
  else if (mpiszero(xmp, n, params))
    {
      strcpy(c, "+0.");
      if (mpisneg(xmp, n, params))
        strcpy(c, "-0.");
      i_1 = lenc - 7;
      i_2 = *digits - 1;
      p = myimin(i_1, i_2);
      i_1 = p;
      for (i = 1; i <= i_1; ++i)
        strcat(c, "0");
      strcat(c, "E+00");
    }
  else
    {
      base = get_base(params);
      mantis = get_mantis(params);
      emin = get_emin(params);
      emax = get_emax(params);
      sign = get_sign(params);

      /* Set up parameters to describe a base 10 system with */
      /* enough precision to hold more than DIGITS digits. */
      base10 = 10;
      mantis10 = *digits + 10;
      emin10 = emin * 2;
      emax10 = emax * 2;
      ifl = 0;
      mpinit(&base10, &mantis10, &emin10, &emax10, &c_1, &n10, params10,
             &ifl);

      radix10 = get_radix(params10);
      radigs10 = get_radigs(params10);
      iexo10 = get_iexo(params10);
      places10 = get_places(params10);
      wplaces10 = get_wplaces(params10);
      sign10 = get_sign(params10);
      fptype10 = get_fptype(params10);
      last10 = get_last(params10);

      /* Temporarily extend the precision. This ensures we don't */
      /* lose anything when multiplying by powers of 10 later on. */
      mantis_extra = mantis * 2;
      emin_extra = emin * 2;
      emax_extra = emax * 2;
      mpinitcopy(params, &mantis_extra, &emin_extra, &emax_extra, &n_extra,
                 params_extra, &ifl);

      /* Dissect workspace */
      mp_10_1 = 1;
      mp_10_2 = n10 + 1;
      mp_10_3 = (n10 * 2) + 1;
      mp_10_4 = n10 * 3 + 1;
      mpex_1 = n10 + 1;
      mpex_2 = n10 + n_extra + 1;
      mpex_3 = n10 + (n_extra * 2) + 1;
      mpex_4 = n10 + n_extra * 3 + 1;
      iw = &iwork[mp_10_4 - 1];

      /* Set IWORK(MPEX_1) = abs(XMP), but with the exponent set to 0. */
      mpcopy2(xmp, n, params, &iwork[mpex_1 - 1], &n_extra,
              params_extra, &ifl);
      mpabs(&iwork[mpex_1 - 1], &n_extra, params_extra, &ifl);
      iexo_extra = get_iexo(params_extra);
      iwork[n10 + iexo_extra - 1] = 0;

      /* Put the base 10 radix into IWORK(MPEX_2) */
      itomp(&radix10, &iwork[mpex_2 - 1], &n_extra, params_extra, &ifl);

      /* Now we keep multiplying by RADIX10 to get the digits of */
      /* the decimal conversion in bunches, and filling in the */
      /* elements of IWORK. */
      wplac10 = 0;
      i_1 = places10;
      for (i = 1; i <= i_1; ++i)
        {
          times(&iwork[mpex_1 - 1], &iwork[mpex_2 - 1], &iwork[mpex_3 - 1],
                &n_extra, params_extra, &ifl);
          d = mptoi(&iwork[mpex_3 - 1], &n_extra, params_extra, &ifl);
          iwork[i - 1] = d;
          itomp(&d, &iwork[mpex_4 - 1], &n_extra, params_extra, &ifl);
          minus(&iwork[mpex_3 - 1], &iwork[mpex_4 - 1], &iwork[mpex_1 - 1],
                &n_extra, params_extra, &ifl);
          if (d != 0)
            wplac10 = i;
        }
      iwork[places10 - 1] = iwork[places10 - 1] / last10 * last10;
      iwork[wplaces10 - 1] = wplac10;
      iwork[iexo10 - 1] = 0;
      iwork[sign10 - 1] = xmp[sign - 1];
      iwork[fptype10 - 1] = regcon();

      /* Get the exponent of the original number. In our base 10 */
      /* arithmetic, raise BASE to the power of that exponent. */
      expo = xmp[iexo - 1];
      itomp(&base, &iwork[mp_10_2 - 1], &n10, params10, &ifl);
      mpipow(&expo, &iwork[mp_10_2 - 1], &n10, params10, &iwork[mp_10_3 - 1],
             iw, &ifl);

      /* Multiply the result by the fraction we already converted. */
      times(&iwork[mp_10_1 - 1], &iwork[mp_10_3 - 1], &iwork[mp_10_2 - 1], &n10,
            params10, &ifl);

      /* Set up another base 10 system but with exactly the */
      /* number of digits requested by the caller. */
      i_1 = get_emin(params10);
      i_2 = get_emax(params10);
      mpinitcopy(params10, digits, &i_1, &i_2, &n10_round,
                 params10_round, &ifl);

      /* Copy the result from MP_10_2 to MP_10_1 (thus rounding */
      /* it to DIGITS digits). */
      mpcopy2(&iwork[mp_10_2 - 1], &n10, params10, &iwork[mp_10_1 - 1],
              &n10_round, params10_round, &ifl);

      /* Now produce the character string. */
      if (mpispos(&iwork[mp_10_1 - 1], &n10, params10_round))
        *(unsigned char *)c = '+';
      else
        *(unsigned char *)c = '-';
      cpos = 2;

      i_1 = *digits;
      for (i = 1; i <= i_1; ++i)
        {
          pos = (i - 1) / radigs10 + 1;
          ipos = i - (pos - 1) * radigs10;
          digs = itoc10(&iwork[pos - 1], &radigs10);
          *(unsigned char *)&c[cpos - 1] = *(unsigned char *)&digs[ipos - 1];
          ++cpos;
          if (i == 1)
            {
              *(unsigned char *)&c[cpos - 1] = '.';
              ++cpos;
            }
        }

      /* Deal with the decimal exponent. We shove it on the end */
      /* and make sure not to lose any digits of it. */
      decexp = iwork[get_iexo(params10_round) - 1] - 1;
      r = myiabs(decexp);
      ndigs = 0;
    L80:
      if (r > 0)
        {
          ++ndigs;
          r /= 10;
          goto L80;
        }
      if (ndigs < 2)
        ndigs = 2;
      cpos = cpos + ndigs + 1;
      r = myiabs(decexp);
      i_1 = ndigs;
      for (i = 1; i <= i_1; ++i)
        {
          d = r % 10;
          r /= 10;
          ch_2 = itoc(&d);
          *(unsigned char *)&c[cpos - 1] = *(unsigned char *)&ch_2;
          --cpos;
        }
      if (decexp >= 0)
        *(unsigned char *)&c[cpos - 1] = '+';
      else
        *(unsigned char *)&c[cpos - 1] = '-';
      --cpos;
      *(unsigned char *)&c[cpos - 1] = 'E';

      i_1 = cpos + ndigs + 1;
      c[i_1] = ' ';
    }

  *ifail = p01abf(ifail, &ierr, "MP2DEC", &nrec, rec);
  return;

} /* mp2dec */

void lowadd(int *left, int *right, int *n,
            int *params, int *result, int *iround, int *iamrnd,
            int *sticky)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base;
  int last, iexo, mult;
  int wplac_result, i, radix, shift, carry;
  int places, radigs;
  int wplac_left;
  int sum, wplaces;
  int wplac_right;

  /* Adds together two model numbers, returns sum in RESULT. */
  /* Sets IROUND according to whether true result has been rounded */
  /* up or down, and sets IAMRND to be the rounding digit. */

  /* The signs of the operands are ignored ... magnitudes added. */

  /* N.B. This routine must be called with magnitude of LEFT */
  /* larger than magnitude of RIGHT. */

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  base = get_base(params);
  iexo = get_iexo(params);
  radix = get_radix(params);
  radigs = get_radigs(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);

  /* LEFT is greater than RIGHT in magnitude */
  shift = (left[iexo - 1] - right[iexo - 1] + radigs - 1) / radigs;
  i_1 = shift * radigs - left[iexo - 1] + right[iexo - 1];
  mult = ipower(&base, i_1);
  if (shift > places)
    {
      assign(result, left, n, params);
      *iround = -1;
      if (shift == places + 1)
        *iamrnd = right[0] * mult / radix;
      return;
    }
  wplac_left = left[wplaces - 1];
  wplac_right = right[wplaces - 1];
  carry = 0;
  for (i = places + shift; i >= 1; --i)
    {
      sum = carry;
      if (shift + 1 <= i && i <= wplac_right + shift)
        sum += mult * right[i - shift - 1];
      if (i <= wplac_left)
        sum += left[i - 1];
      carry = sum / radix;
      sum -= carry * radix;
      if (i <= places)
        result[i - 1] = sum;
      else
        {
          if (sum != 0)
            {
              *iround = -1;
              if (i > places + 1)
                *sticky = 1;
            }
          *iamrnd = sum;
        }
    }
  result[iexo - 1] = left[iexo - 1];
  if (carry != 0)
    {
      /* Shift significand right and increase exponent by 1. */
      ++result[iexo - 1];
      if (*iamrnd % base != 0)
        *sticky = 1;
      *iamrnd = *iamrnd / base + result[places - 1] % base * (radix / base);
      carry *= radix / base;
      i_1 = places;
      for (i = 1; i <= i_1; ++i)
        {
          sum = carry + result[i - 1] / base;
          carry = radix / base * (result[i - 1] % base);
          result[i - 1] = sum;
        }
    }
  if (carry != 0)
    *iround = -1;
  /* Non zero digit shifted from end of significand. */
  if (*iamrnd % last != 0)
    *sticky = 1;
  *iamrnd = *iamrnd / last + result[places - 1] % last * (radix / last);
  if (result[places - 1] % last > 0)
    {
      *iround = -1;
      result[places - 1] = result[places - 1] / last * last;
    }

  /* Figure out the working precision of the result. */
  wplac_result = places;
 L60:
  if (wplac_result > 1 && result[wplac_result - 1] == 0)
    {
      --wplac_result;
      goto L60;
    }
  result[wplaces - 1] = wplac_result;

  return;
} /* lowadd */

void lowsub(int *left, int *right, int *n,
            int *params, int *result, int *iround, int *iamrnd,
            int *sticky, int *nought)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int base;
  int last, iexo;
  int mult;
  int wplac_result, i;
  int guard, radix, shift, carry;
  int places, radigs;
  int wplac_left;
  char rec[125];
  int sum, wplaces;
  int wplac_right;

  /* Subtract magnitudes of two model numbers. */

  /* The signs of the operands are ignored ... magnitudes subtracted. */

  /* N.B. This routine must be called with magnitude of LEFT */
  /* larger than magnitude of RIGHT. */

  /* If the result is exactly zero, NOUGHT is set to .TRUE. on exit. */

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  base = get_base(params);
  iexo = get_iexo(params);
  radix = get_radix(params);
  radigs = get_radigs(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);

  *nought = 0;

  /* LEFT is greater than RIGHT in magnitude */
  guard = 0;
  shift = (left[iexo - 1] - right[iexo - 1] + radigs - 1) / radigs;
  if (shift > places + 1)
    {
      assign(result, left, n, params);
      next(result, n, params, &c_false);
      *iround = -1;
      *iamrnd = radix - 1;
      *sticky = 1;
      return;
    }
  i_1 = shift * radigs - left[iexo - 1] + right[iexo - 1];
  mult = ipower(&base, i_1);
  wplac_left = left[wplaces - 1];
  wplac_right = right[wplaces - 1];
  carry = 0;
  for (i = places + shift; i >= 1; --i)
    {
      sum = carry;
      if (i >= shift + 1 && i <= wplac_right + shift)
        sum -= mult * right[i - shift - 1];
      if (i <= wplac_left)
        sum += left[i - 1];
      if (sum < 0)
        {
          carry = (sum + 1) / radix - 1;
          sum -= carry * radix;
        }
      else
        carry = 0;
      if (i <= places)
        result[i - 1] = sum;
      else if (i == places + 1)
        guard = sum;
      else
        {
          if (sum != 0)
            {
              *iround = -1;
              *sticky = 1;
            }
          *iamrnd = sum;
        }
    }
  result[iexo - 1] = left[iexo - 1];
  if (carry < 0)
    {
      sprintf(rec, "PANIC: LEFT less than RIGHT");
      (void)p01abf(&c_0, &c_1, "LOWSUB", &c_1, rec);
    }

  /* Check for zero result before attempting to normalise. */
  *nought = 1;
  i_1 = places;
  for (i = 1; i <= i_1; ++i)
    {
      if (result[i - 1] > 0)
        *nought = 0;
    }
  if (guard != 0 || *iamrnd != 0)
    *nought = 0;
  if (*nought)
    return;

  /* Now normalise the result. */
 L60:
  if (result[0] >= radix / base)
    goto L100;
  carry = guard / (radix / base);
  guard = guard * base - carry * radix + *iamrnd / (radix / base);
  *iamrnd = (*iamrnd - *iamrnd / (radix / base) * (radix / base)) * base;
  --result[iexo - 1];
  for (i = places; i >= 1; --i)
    {
      sum = carry + base * result[i - 1];
      carry = sum / radix;
      result[i - 1] = sum - carry * radix;
    }
  goto L60;

 L100:
  if (guard != 0)
    *iround = -1;
  if (guard % last != 0)
    *sticky = 1;
  *iamrnd = guard / last + result[places - 1] % last * (radix / last);
  if (result[places - 1] % last > 0)
    {
      *iround = -1;
      result[places - 1] = result[places - 1] / last * last;
    }

  /* Figure out the working precision of the result. */
  wplac_result = places;
 L120:
  if (wplac_result > 1 && result[wplac_result - 1] == 0)
    {
      --wplac_result;
      goto L120;
    }
  result[wplaces - 1] = wplac_result;

  return;
} /* lowsub */

void lowmul(int *left, int *right, int *n,
            int *params, int *result, int *iround, int *iamrnd,
            int *sticky)
{
  int base;
  int last, iexo;
  int wplac_result, i, j, guard, radix, carry;
  int places;
  int wplac_left;
  int fptype;
  int pos, sum, wplaces;
  int wplac_right;

  /* Multiplies two model numbers together, answer in RESULT... */
  /* Ignores signs of left and right. */

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  guard = 0;
  base = get_base(params);
  iexo = get_iexo(params);
  fptype = get_fptype(params);
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);
  if (mpiszero(left, n, params) || mpiszero(right, n, params))
    {
      result[fptype - 1] = zercon();
      return;
    }
  wplac_left = left[wplaces - 1];
  wplac_right = right[wplaces - 1];
  carry = 0;
  for (pos = wplac_left + wplac_right; pos >= 1; --pos)
    {
      sum = carry;
      for (i = wplac_left; i >= 1; --i)
        {
          j = pos - i;
          if (1 <= j && j <= wplac_right)
            sum += left[i - 1] * right[j - 1];
        }
      carry = sum / radix;
      sum -= carry * radix;
      if (pos >= places + 2)
        {
          if (sum != 0)
            {
              *iround = -1;
              *sticky = 1;
              *iamrnd = sum;
            }
        }
      else if (pos == places + 1)
        guard = sum;
      else
        result[pos - 1] = sum;
    }
  result[iexo - 1] = left[iexo - 1] + right[iexo - 1];
  wplac_result = wplac_left + wplac_right;
  if (wplac_result > places)
    wplac_result = places;
  result[wplaces - 1] = wplac_result;
  if (result[0] < radix / base)
    {
      --result[iexo - 1];
      carry = guard / (radix / base);
      guard = guard * base - carry * radix + *iamrnd / (radix / base);
      for (i = wplac_result; i >= 1; --i)
        {
          result[i - 1] = base * result[i - 1] + carry;
          carry = result[i - 1] / radix;
          result[i - 1] -= carry * radix;
        }
    }
  if (wplac_result == places)
    {
      if (guard != 0)
        *iround = -1;
      if (guard % last != 0)
        *sticky = 1;
      *iamrnd = guard / last + result[places - 1] % last * (radix / last);
      if (result[places - 1] % last != 0)
        {
          *iround = -1;
          result[places - 1] = result[places - 1] / last * last;
        }
    }
  return;
} /* lowmul */

/* Like LOWMUL, but LEFT and RIGHT can be different precision */
void lowmul2(int *left, int *n_left, int *params_left,
             int *right, int *n_right, int *params_right,
             int *result, int *n_result, int *params_result,
             int *iround, int *iamrnd, int *sticky)
{
  int iexo_result, base;
  int last;
  int wplaces_left, wplac_result, i, j, guard,
    radix, carry;
  int iexo_left, wplaces_right, places_result, fptype_result;
  int wplac_left;
  int iexo_right;
  int wplaces_result, pos, sum;
  int wplac_right;

  /* Multiplies two model numbers together, answer in RESULT... */
  /* Ignores signs of left and right. */

  if (*n_result <= 0) /* Avoid compiler warning about unused n */
    return;

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  guard = 0;
  base = get_base(params_left);
  iexo_left = get_iexo(params_left);
  iexo_right = get_iexo(params_right);
  iexo_result = get_iexo(params_result);
  fptype_result = get_fptype(params_result);
  radix = get_radix(params_left);
  places_result = get_places(params_result);
  wplaces_left = get_wplaces(params_left);
  wplaces_right = get_wplaces(params_right);
  wplaces_result = get_wplaces(params_result);
  last = get_last(params_result);
  if (mpiszero(left, n_left, params_left) || mpiszero(right, n_right, params_right))
    {
      result[fptype_result - 1] = zercon();
      return;
    }
  wplac_left = left[wplaces_left - 1];
  wplac_right = right[wplaces_right - 1];
  carry = 0;
  for (pos = wplac_left + wplac_right; pos >= 1; --pos)
    {
      sum = carry;
      for (i = wplac_left; i >= 1; --i)
        {
          j = pos - i;
          if (1 <= j && j <= wplac_right)
            sum += left[i - 1] * right[j - 1];
        }
      carry = sum / radix;
      sum -= carry * radix;
      if (pos >= places_result + 2)
        {
          if (sum != 0)
            {
              *iround = -1;
              *sticky = 1;
              *iamrnd = sum;
            }
        }
      else if (pos == places_result + 1)
        guard = sum;
      else
        result[pos - 1] = sum;
    }
  result[iexo_result - 1] = left[iexo_left - 1] + right[iexo_right - 1];
  wplac_result = wplac_left + wplac_right;
  if (wplac_result > places_result)
    wplac_result = places_result;
  result[wplaces_result - 1] = wplac_result;
  if (result[0] < radix / base)
    {
      --result[iexo_result - 1];
      carry = guard / (radix / base);
      guard = guard * base - carry * radix + *iamrnd / (radix / base);
      for (i = wplac_result; i >= 1; --i)
        {
          result[i - 1] = base * result[i - 1] + carry;
          carry = result[i - 1] / radix;
          result[i - 1] -= carry * radix;
        }
    }
  if (wplac_result == places_result)
    {
      if (guard != 0)
        *iround = -1;
      if (guard % last != 0)
        *sticky = 1;
      *iamrnd = guard / last + result[places_result - 1] % last * (radix / last);
      if (result[places_result - 1] % last != 0)
        {
          *iround = -1;
          result[places_result - 1] = result[places_result - 1] / last * last;
        }
    }
  return;
} /* lowmul2 */

void lowdiv(int *left, int *right, int *n,
            int *params, int *result, int *iround, int *iamrnd,
            int *sticky, int *denom, int *divi)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int base;
  int qhat, last, iexo;
  int wplac_result, d, i, j, radix;
  int carry1, carry2, places;
  int wplac_left, sum, wplaces;
  int wplac_right, res0;

  /* Divides two numbers, result in RESULT. */
  /* Ignores signs of LEFT and RIGHT. */

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  res0 = 0;
  base = get_base(params);
  iexo = get_iexo(params);
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);
  wplac_left = left[wplaces - 1];
  wplac_right = right[wplaces - 1];
  assign(denom, right, n, params);
  denom[wplac_right] = 0;

  divi[0] = 0;
  for (i = 1; i <= wplac_left; ++i)
    divi[i] = left[i - 1];
  for (i = wplac_left + 1; i <= wplac_right + places + 2; ++i)
    divi[i] = 0;
  d = radix / (denom[0] + 1);
  carry1 = 0;
  carry2 = 0;
  if (d != 1)
    {
      /* Scale operands ... multiply divisor and dividend by D. */
      for (i = wplac_right; i >= 1; --i)
        {
          sum = carry1 + denom[i - 1] * d;
          carry1 = sum / radix;
          denom[i - 1] = sum - carry1 * radix;
        }
      for (i = wplac_left; i >= 1; --i)
        {
          sum = carry2 + divi[i] * d;
          carry2 = sum / radix;
          divi[i] = sum - carry2 * radix;
        }
    }

  divi[0] = carry2;
  i_1 = places + 1;
  for (j = 0; j <= i_1; ++j)
    {
      /* Loop on J finds successive digits of RESULT. */
      if (divi[j] == denom[0])
        qhat = radix - 1;
      else
        qhat = (divi[j] * radix + divi[j + 2 - 1]) / denom[0];
      /* QHAT is the trial divisor. */
    L100:
      if (qhat * denom[2 - 1] > (radix * divi[j] + divi[j + 2 - 1] - qhat *
                                 denom[0]) * radix + divi[j + 3 - 1])
        {
          --qhat;
          goto L100;
        }

      carry1 = 0;
      carry2 = 0;
      i_2 = j + 1;
      for (i = wplac_right + j; i >= i_2; --i)
        {
          sum = qhat * denom[i - j - 1] + carry1;
          carry1 = sum / radix;
          sum = divi[i] - sum + carry1 * radix + carry2;
          if (sum < 0)
            {
              sum += radix;
              carry2 = -1;
            }
          else
            carry2 = 0;
          divi[i] = sum;
        }
      divi[j] = divi[j] - carry1 + carry2;

      if (divi[j] < 0)
        {
          /* Trial divisor QHAT was 1 too big. */
          /* Complement dividend, then add RIGHT significand. */
          --qhat;
          carry1 = 0;
          i_2 = j + 1;
          for (i = wplac_right + j; i >= i_2; --i)
            {
              sum = divi[i] + denom[i - j - 1] + carry1;
              carry1 = sum / radix;
              sum -= carry1 * radix;
              divi[i] = sum;
            }
          divi[j] += carry1;
        }

      if (j == places + 1)
        *iamrnd = qhat;
      else if (j > 0)
        result[j - 1] = qhat;
      else
        res0 = qhat;
    }

  result[iexo - 1] = left[iexo - 1] - right[iexo - 1];
  i_1 = wplac_left + wplac_right + 1;
  for (i = 1; i <= i_1; ++i)
    {
      /* If DIVI is non zero , RESULT is not exact. */
      if (divi[i] > 0)
        {
          *iround = -1;
          *sticky = 1;
        }
    }

  if (res0 > 0)
    {
      /* Shift RESULT right and increase exponent. */
      ++result[iexo - 1];
      carry1 = radix / base * res0;
      i_1 = places;
      for (i = 1; i <= i_1; ++i)
        {
          sum = carry1 + result[i - 1] / base;
          carry1 = result[i - 1] % base * (radix / base);
          result[i - 1] = sum;
        }

      if (carry1 > 0)
        *iround = -1;
      /* Digit was shifted from end of significand. */
      if (*iamrnd % base != 0)
        *sticky = 1;
      *iamrnd = carry1 + *iamrnd / base;
    }

  if (*iamrnd % last != 0)
    *sticky = 1;
  *iamrnd = *iamrnd / last + result[places - 1] % last * (radix / last);
  if (result[places - 1] % last != 0)
    {
      *iround = -1;
      result[places - 1] = result[places - 1] / last * last;
    }

  /* Figure out the working precision of the result. */
  wplac_result = places;
 L220:
  if (wplac_result > 1 && result[wplac_result - 1] == 0)
    {
      --wplac_result;
      goto L220;
    }
  result[wplaces - 1] = wplac_result;

  return;
} /* lowdiv */

/* Like LOWDIV, but LEFT and RIGHT can be different precision */
void lowdiv2(int *left, int *n_left, int *params_left,
             int *right, int *n_right, int *params_right,
             int *result, int *n_result, int *params_result,
             int *iround, int *iamrnd, int *sticky,
             int *denom, int *divi)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int iexo_result, base;
  int qhat, last;
  int places_right, wplaces_left, wplac_result, d, i, j,
    radix;
  int iexo_left, wplaces_right, carry1, carry2, places_result;
  int wplac_left, iexo_right, wplaces_result, sum;
  int wplac_right, res0;

  /* Divides two numbers, result in RESULT. */
  /* Ignores signs of LEFT and RIGHT. */

  if (*n_left <= 0 || *n_result <= 0) /* Avoid compiler warning about unused n */
    return;

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  res0 = 0;
  base = get_base(params_left);
  iexo_left = get_iexo(params_left);
  iexo_right = get_iexo(params_right);
  iexo_result = get_iexo(params_result);
  radix = get_radix(params_left);
  places_right = get_places(params_right);
  places_result = get_places(params_result);
  wplaces_left = get_wplaces(params_left);
  wplaces_right = get_wplaces(params_right);
  wplaces_result = get_wplaces(params_result);
  last = get_last(params_result);
  assign(denom, right, n_right, params_right);
  i_1 = places_right + 1;
  for (i = right[wplaces_right - 1] + 1; i <= i_1; ++i)
    denom[i - 1] = 0;

  wplac_left = left[wplaces_left - 1];
  wplac_right = right[wplaces_right - 1];

  divi[0] = 0;
  for (i = 1; i <= wplac_left; ++i)
    divi[i] = left[i - 1];
  for (i = wplac_left + 1; i <= wplac_right + places_result + 2; ++i)
    divi[i] = 0;

  d = radix / (denom[0] + 1);
  carry1 = 0;
  carry2 = 0;
  if (d != 1)
    {
      /* Scale operands ... multiply divisor and dividend by D. */
      for (i = wplac_right; i >= 1; --i)
        {
          sum = carry1 + denom[i - 1] * d;
          carry1 = sum / radix;
          denom[i - 1] = sum - carry1 * radix;
        }
      for (i = wplac_left; i >= 1; --i)
        {
          sum = carry2 + divi[i] * d;
          carry2 = sum / radix;
          divi[i] = sum - carry2 * radix;
        }
    }

  divi[0] = carry2;
  i_1 = places_result + 1;
  for (j = 0; j <= i_1; ++j)
    {
      /* Loop on J finds successive digits of RESULT. */
      if (divi[j] == denom[0])
        qhat = radix - 1;
      else
        qhat = (divi[j] * radix + divi[j + 2 - 1]) / denom[0];
      /* QHAT is the trial divisor. */
    L120:
      if (qhat * denom[2 - 1] > (radix * divi[j] + divi[j + 2 - 1] - qhat *
                                 denom[0]) * radix + divi[j + 3 - 1])
        {
          --qhat;
          goto L120;
        }

      carry1 = 0;
      carry2 = 0;
      i_2 = j + 1;
      for (i = wplac_right + j; i >= i_2; --i)
        {
          sum = qhat * denom[i - j - 1] + carry1;
          carry1 = sum / radix;
          sum = divi[i] - sum + carry1 * radix + carry2;
          if (sum < 0)
            {
              sum += radix;
              carry2 = -1;
            }
          else
            carry2 = 0;
          divi[i] = sum;
        }
      divi[j] = divi[j] - carry1 + carry2;

      if (divi[j] < 0)
        {
          /* Trial divisor QHAT was 1 too big. */
          /* Complement dividend, then add RIGHT significand. */
          --qhat;
          carry1 = 0;
          i_2 = j + 1;
          for (i = wplac_right + j; i >= i_2; --i)
            {
              sum = divi[i] + denom[i - j - 1] + carry1;
              carry1 = sum / radix;
              sum -= carry1 * radix;
              divi[i] = sum;
            }
          divi[j] += carry1;
        }

      if (j == places_result + 1)
        *iamrnd = qhat;
      else if (j > 0)
        result[j - 1] = qhat;
      else
        res0 = qhat;
    }

  result[iexo_result - 1] = left[iexo_left - 1] - right[iexo_right - 1];
  i_1 = wplac_left + wplac_right + 1;
  for (i = 1; i <= i_1; ++i)
    {
      /* If DIVI is non zero , RESULT is not exact. */
      if (divi[i] > 0)
        {
          *iround = -1;
          *sticky = 1;
        }
    }

  if (res0 > 0)
    {
      /* Shift RESULT right and increase exponent. */
      ++result[iexo_result - 1];
      carry1 = radix / base * res0;
      i_1 = places_result;
      for (i = 1; i <= i_1; ++i)
        {
          sum = carry1 + result[i - 1] / base;
          carry1 = result[i - 1] % base * (radix / base);
          result[i - 1] = sum;
        }

      if (carry1 > 0)
        *iround = -1;
      /* Digit was shifted from end of significand. */
      if (*iamrnd % base != 0)
        *sticky = 1;
      *iamrnd = carry1 + *iamrnd / base;
    }

  if (*iamrnd % last != 0)
    *sticky = 1;
  *iamrnd = *iamrnd / last + result[places_result - 1] % last * (radix / last);
  if (result[places_result - 1] % last != 0)
    {
      *iround = -1;
      result[places_result - 1] = result[places_result - 1] / last * last;
    }

  /* Figure out the working precision of the result. */
  wplac_result = places_result;
 L240:
  if (wplac_result > 1 && result[wplac_result - 1] == 0)
    {
      --wplac_result;
      goto L240;
    }
  result[wplaces_result - 1] = wplac_result;

  return;
} /* lowdiv2 */

void lowroo(int *xmp, int *n, int *params, int
            *result, int *iround, int *iamrnd, int *sticky,
            int *dmp, int *ump)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int base;
  int qhat, last, iexo;
  int i, k, wplac, radix, shift;
  int carry1, carry2, places;
  int sum, wplaces;

  /* Returns square root of model number XMP , in RESULT. */

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return;

  *iround = 0;
  *iamrnd = 0;
  *sticky = 0;
  base = get_base(params);
  iexo = get_iexo(params);
  radix = get_radix(params);
  places = get_places(params);
  wplaces = get_wplaces(params);
  last = get_last(params);
  wplac = xmp[wplaces - 1];
  i_1 = places + 2;
  for (i = 1; i <= i_1; ++i)
    dmp[i - 1] = 0;
  i_1 = places;
  for (i = 1; i <= i_1; ++i)
    {
      if (i <= wplac)
        ump[i - 1] = xmp[i - 1];
      else
        ump[i - 1] = 0;
      ump[i + places - 1] = 0;
    }
  ump[(places * 2)] = 0;
  ump[(places * 2) + 2 - 1] = 0;
  if (xmp[iexo - 1] % 2 == 0)
    result[iexo - 1] = xmp[iexo - 1] / 2;
  else
    {
      /* Divide by BASE to make exponent even. */
      result[iexo - 1] = (xmp[iexo - 1] + 1) / 2;
      carry1 = 0;
      i_1 = places + 2;
      for (i = 1; i <= i_1; ++i)
        {
          sum = ump[i - 1] + carry1 * radix;
          carry1 = sum % base;
          ump[i - 1] = sum / base;
        }
    }

  qhat = 0;
 L80:
  if ((qhat + 1) * (qhat + 1) <= ump[0] * radix + ump[2 - 1])
    {
      ++qhat;
      goto L80;
    }
  /* QHAT = first digit of result. */
  ump[2 - 1] -= qhat * qhat % radix;
  ump[0] -= qhat * qhat / radix;
  if (ump[2 - 1] < 0)
    {
      ump[2 - 1] += radix;
      --ump[0];
    }
  result[0] = qhat;
  dmp[2 - 1] = qhat + qhat;
  if (dmp[2 - 1] >= radix)
    {
      dmp[3 - 1] = dmp[2 - 1] - radix;
      dmp[2 - 1] = 1;
      shift = 1;
    }
  else
    shift = 0;

  i_1 = places;
  for (k = 1; k <= i_1; ++k)
    {
      /* Loop on K obtains successive digits of result. */
      if (ump[k + 1 - shift - 1] == dmp[2 - 1])
        qhat = radix - 1;
      else
        qhat = ((ump[k + 1 - shift - 1] * radix +
                 ump[k + 2 - shift - 1]) * radix +
                ump[k + 3 - shift - 1]) / (dmp[2 - 1] * radix + dmp[3 - 1]);
      /* QHAT is the trial divisor. */
      dmp[k + 2 + shift - 1] = qhat;
    L100:
      if (dmp[3 - 1] * qhat > (ump[k + 1 - shift - 1] * radix + ump[k + 2 - shift - 1] -
                               dmp[2 - 1] * qhat) * radix + ump[k + 3 - shift - 1])
        {
          --qhat;
          dmp[k + 2 + shift - 1] = qhat;
          goto L100;
        }

      carry1 = 0;
      carry2 = 0;
      i_2 = k + 1;
      for (i = (k * 2) + 2; i >= i_2; --i)
        {
          sum = qhat * dmp[i - k + shift - 1] + carry1;
          carry1 = sum / radix;
          sum = ump[i - 1] - sum + carry1 * radix + carry2;
          if (sum < 0)
            {
              sum += radix;
              carry2 = -1;
            }
          else
            carry2 = 0;
          ump[i - 1] = sum;
        }
      ump[k - 1] = ump[k - 1] - carry1 + carry2;

      if (ump[k - 1] < 0)
        {
          /* Subtracted too much. Complement UMP and add DMP + QHAT back. */
          --qhat;
          carry1 = qhat;
          i_2 = k + 1;
          for (i = (k * 2) + 2; i >= i_2; --i)
            {
              sum = ump[i - 1] + dmp[i - k + shift - 1] + carry1;
              carry1 = sum / radix;
              sum -= carry1 * radix;
              ump[i - 1] = sum;
            }
          ump[k - 1] += carry1;
          dmp[k + 2 + shift - 1] = qhat;
        }
      if (k == places)
        *iamrnd = qhat;
      else
        result[k] = qhat;
      i = k + 2 + shift;
      dmp[i - 1] *= 2;
      carry1 = dmp[i - 1] / radix;
    L160:
      if (carry1 <= 0)
        goto L180;
      dmp[i - 1] %= radix;
      --i;
      dmp[i - 1] += carry1;
      carry1 = dmp[i - 1] / radix;
      goto L160;
    L180:
      ;
    }

  i_1 = (places * 2) + 2;
  for (i = 1; i <= i_1; ++i)
    {
      if (ump[i - 1] > 0)
        {
          *iround = -1;
          *sticky = 1;
        }
    }

  if (*iamrnd % last != 0)
    *sticky = 1;
  *iamrnd = *iamrnd / last + result[places - 1] % last * (radix / last);
  if (result[places - 1] % last != 0)
    {
      *iround = -1;
      result[places - 1] = result[places - 1] / last * last;
    }

  /* Figure out the working precision of the result. */
  wplac = places;
 L220:
  if (wplac > 1 && result[wplac - 1] == 0)
    {
      --wplac;
      goto L220;
    }
  result[wplaces - 1] = wplac;

  return;
} /* lowroo */

void mpnegate(int *xmp, int *n, int *params,
              int *ifail)
{
  int nrec, sign, ierr, nreq;
  char rec[125];

  /* Negates XMP in place. */

  ierr = 0;
  nrec = 0;
  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else if (! mpisnan(xmp, n, params))
    {
      sign = get_sign(params);
      if (xmp[sign - 1] == poscon())
        xmp[sign - 1] = negcon();
      else
        xmp[sign - 1] = poscon();
    }

  *ifail = p01abf(ifail, &ierr, "MPNEGATE", &nrec, rec);
  return;

} /* mpnegate */

void mpchgsign(int *xmp, int *ymp, int *n,
               int *params, int *ifail)
{
  int nrec, sign, ierr, nreq;
  char rec[125];

  /* Sets YMP to be XMP with the opposite sign */

  ierr = 0;
  nrec = 0;
  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else
    {
      assign(ymp, xmp, n, params);
      sign = get_sign(params);
      if (xmp[sign - 1] == poscon())
        ymp[sign - 1] = negcon();
      else
        ymp[sign - 1] = poscon();
    }

  *ifail = p01abf(ifail, &ierr, "MPCHGSIGN", &nrec, rec);
  return;

} /* mpchgsign */

void mpcopysign(int *xmp, int *ymp, int *result,
                int *n, int *params, int *ifail)
{
  int nrec, sign, ierr, nreq;
  char rec[125];

  /* Sets RESULT to be XMP with the sign of YMP */

  ierr = 0;
  nrec = 0;
  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else if (mpisfake(ymp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "YMP is corrupt or uninitialised");
    }
  else
    {
      assign(result, xmp, n, params);
      sign = get_sign(params);
      result[sign - 1] = ymp[sign - 1];
    }

  *ifail = p01abf(ifail, &ierr, "MPCOPYSIGN", &nrec, rec);
  return;

} /* mpcopysign */

void mpabs(int *xmp, int *n, int *params,
           int *ifail)
{
  int nrec, sign, ierr, nreq;
  char rec[125];

  /* Replaces XMP by its absolute value. */

  ierr = 0;
  nrec = 0;
  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else
    {
      sign = get_sign(params);
      xmp[sign - 1] = poscon();
    }

  *ifail = p01abf(ifail, &ierr, "MPABS", &nrec, rec);
  return;

} /* mpabs */

void mpsin_kernel(int *xmp, int *n, int *params,
                  int *result, int *iwork)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int ifac, sign;
  int savs;
  int params_short[15], short_mantis, i, k, ifail;
  int mpeps;
  int mp_tmp_1, mp_tmp_2, mpifac[40], *iw;
  int mpterm;
  int mpxsquared;
  int n_short;

  /* Computes sin(x), for -pi/4 <= x <= pi/4, with the result in RESULT. */

  ifail = 0;
  sign = get_sign(params);

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  i_1 = get_mantis(params);
  short_mantis = myimin(i_1, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifail);

  /* Dissect workspace */
  mpeps = *n * 10 + 1;
  mpterm = 1;
  mpxsquared = *n + 1;
  mp_tmp_1 = (*n * 2) + 1;
  mp_tmp_2 = *n * 3 + 1;
  iw = &iwork[(*n * 4)];

  /* IWORK(MPEPS) = eps */
  epsilon(n, params, &iwork[mpeps - 1]);

  /* term = IWORK(MPTERM) = x */
  assign(&iwork[mpterm - 1], xmp, n, params);

  /* IWORK(MPXSQUARED) = x**2 */
  times(xmp, xmp, &iwork[mpxsquared - 1], n, params, &ifail);

  /* RESULT = x */
  assign(result, xmp, n, params);

  i = 1;
 L20:

  /* Changed to avoid warning when compiling f2c translated code */
  /* IFAC = 2*I*(2*I+1) */
  k = i * 2;
  ifac = k * (k + 1);
  itomp(&ifac, mpifac, &n_short, params_short, &ifail);

  /* IWORK(MP_TMP_1) = term * x**2 */
  times(&iwork[mpterm - 1], &iwork[mpxsquared - 1],
        &iwork[mp_tmp_1 - 1], n, params, &ifail);

  /* IWORK(MPTERM) = IWORK(MPTERM) * x**2 / 2*i */
  divide2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
          params_short, &iwork[mpterm - 1], n, params, iw, &ifail);

  /* RESULT = RESULT +- term */
  if (i % 2 == 1)
    {
      minus(result, &iwork[mpterm - 1], &iwork[mp_tmp_2 - 1], n, params,
            &ifail);
    }
  else
    {
      plus(result, &iwork[mpterm - 1], &iwork[mp_tmp_2 - 1], n, params,
           &ifail);
    }
  assign(result, &iwork[mp_tmp_2 - 1], n, params);

  /* Repeat until the term is negligible */
  savs = iwork[sign - 1];
  iwork[sign - 1] = poscon();
  if (mpgt(&iwork[mpterm - 1], &iwork[mpeps - 1], n, params))
    {
      iwork[sign - 1] = savs;
      ++i;
      goto L20;
    }

  return;
} /* mpsin_kernel */

void mpremainder_piby2(int *xmp, int *n, int *params,
                       int *pi_over_two, int *two_over_pi, int *n_pi,
                       int *params_pi, int *result, int *region,
                       int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int mppi1, mppi2, mppi3;
  int k;
  int fptype;
  int *mp1, *mp2, *iw2, *mp4, *mp5, *mp6;
  char rec[125];
  int ifl;

  /* Computes remainder_piby2(x), i.e. the remainder when x is
     divided by pi/2, with the result in RESULT being in the
     range [-pi/4,pi/4]. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        assign(result, xmp, n, params);
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp4 = &iwork[*n_pi * 3];
          mp5 = &iwork[*n_pi * 3 + *n];
          mp6 = &iwork[*n_pi * 3 + (*n * 2)];
          iw2 = &iwork[*n_pi * 3 + *n * 3];
          mppi1 = 1;
          mppi2 = *n_pi + 1;
          mppi3 = (*n_pi * 2) + 1;

          /* IWORK(MPPI2) = abs(X) */
          assign(&iwork[mppi2 - 1], xmp, n, params);
          mpabs(&iwork[mppi2 - 1], n, params, &ifl);

          /* IWORK(MPPI1) = abs(X) * 2/Pi */
          times2(&iwork[mppi2 - 1], n, params, two_over_pi, n_pi,
                 params_pi, &iwork[mppi1 - 1], n_pi, params_pi,
                 &ifl);

          /* Split the result into an int plus a fraction. */
          /* Set K to be the int part mod 4. */
          k = mpmod4(&iwork[mppi1 - 1], n_pi, params_pi, &ifl);

          mpfrac(&iwork[mppi1 - 1], &iwork[mppi2 - 1], n_pi, params_pi,
                 &ifl);

          /* Set IWORK(MPPI1) = 0.5 */
          itomp(&c_1, mp4, n, params, &ifl);
          itomp(&c_2, mp5, n, params, &ifl);
          divide(mp4, mp5, mp6, n, params,
                 iw2, &ifl);
          mpcopy2(mp6, n, params, &iwork[mppi1 - 1], n_pi,
                  params_pi, &ifl);

          if (mpgt(&iwork[mppi2 - 1], &iwork[mppi1 - 1], n_pi, params_pi))
            {
              /* If reduced argument is > 0.5, increase K by 1 and */
              /* subtract 1.0. */
              ++k;
              if (k == 4)
                k = 0;
              itomp(&c_1, &iwork[mppi1 - 1], n_pi, params_pi, &ifl);
              minus(&iwork[mppi2 - 1], &iwork[mppi1 - 1], &iwork[mppi3 - 1], n_pi,
                    params_pi, ifail);
              assign(&iwork[mppi2 - 1], &iwork[mppi3 - 1], n_pi, params_pi);
            }

          /* That's enough extra precision. Now we have the fraction */
          /* computed accurately we can convert it back to working precision. */
          /* The fraction is in the range [-0.5,0.5]. */
          mpcopy2(&iwork[mppi2 - 1], n_pi, params_pi, mp1, n,
                  params, &ifl);

          /* Multiply back by pi / 2. */
          mpcopy2(pi_over_two, n_pi, params_pi, mp2,
                  n, params, &ifl);
          times(mp1, mp2, result, n, params, &ifl);
          *region = k;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPREMAINDER_PIBY2", &nrec, rec);
  return;

} /* mpremainder_piby2 */

void mpremainder_90d(int *xmp, int *n, int *params,
                     int *pi_over_two, int *n_pi,
                     int *params_pi, int *result, int *region,
                     int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int mppi1, mppi2, mppi3;
  int k;
  int fptype;
  int *mp1, *mp2, *iw1, *iw2, *mp4, *mp5, *mp6;
  char rec[125];
  int ifl;
  int params_short[15];
  int n_short, short_mantis, i_1, i_2;
  int mpninety[40];

  /* Computes remainder_90d(x), i.e. the remainder when x (a value
     in degrees) is divided by 90, with the result in RESULT being
     a radian value in the range [-pi/4,pi/4]. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        assign(result, xmp, n, params);
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp4 = &iwork[*n_pi * 3];
          mp5 = &iwork[*n_pi * 3 + *n];
          mp6 = &iwork[*n_pi * 3 + (*n * 2)];
          iw1 = &iwork[*n_pi * 3];
          iw2 = &iwork[*n_pi * 4];
          mppi1 = 1;
          mppi2 = *n_pi + 1;
          mppi3 = (*n_pi * 2) + 1;

          /* IWORK(MPPI2) = abs(X) */
          assign(&iwork[mppi2 - 1], xmp, n, params);
          mpabs(&iwork[mppi2 - 1], n, params, &ifl);

          /* Set up some parameters for numbers which need only low */
          /* (16 bit) precision. Be careful - don't let short_mantis */
          /* be bigger than the regular one, or your workspace may be */
          /* too small for calls of divide2. */
          i_1 = get_mantis(params);
          short_mantis = myimin(i_1, c_16);
          i_1 = get_emin(params);
          i_2 = get_emax(params);
          mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
                     params_short, &ifl);

          /* Set mpninety = 90 */
          i_1 = 90;
          itomp(&i_1, mpninety, &n_short, params_short, &ifl);

          /* IWORK(MPPI1) = abs(X) / 90 */
          divide2(&iwork[mppi2 - 1], n, params, mpninety, &n_short,
                  params_short, &iwork[mppi1 - 1], n_pi, params_pi,
                  iw1, &ifl);

          /* Split the result into an int plus a fraction. */
          /* Set K to be the int part mod 4. */
          k = mpmod4(&iwork[mppi1 - 1], n_pi, params_pi, &ifl);

          mpfrac(&iwork[mppi1 - 1], &iwork[mppi2 - 1], n_pi, params_pi,
                 &ifl);

          /* Set IWORK(MPPI1) = 0.5 */
          itomp(&c_1, mp4, n, params, &ifl);
          itomp(&c_2, mp5, n, params, &ifl);
          divide(mp4, mp5, mp6, n, params,
                 iw2, &ifl);
          mpcopy2(mp6, n, params, &iwork[mppi1 - 1], n_pi,
                  params_pi, &ifl);

          if (mpgt(&iwork[mppi2 - 1], &iwork[mppi1 - 1], n_pi, params_pi))
            {
              /* If reduced argument is > 0.5, increase K by 1 and */
              /* subtract 1.0. */
              ++k;
              if (k == 4)
                k = 0;
              itomp(&c_1, &iwork[mppi1 - 1], n_pi, params_pi, &ifl);
              minus(&iwork[mppi2 - 1], &iwork[mppi1 - 1], &iwork[mppi3 - 1], n_pi,
                    params_pi, ifail);
              assign(&iwork[mppi2 - 1], &iwork[mppi3 - 1], n_pi, params_pi);
            }

          /* That's enough extra precision. Now we have the fraction */
          /* computed accurately we can convert it back to working precision. */
          /* The fraction is in the range [-0.5,0.5]. */
          mpcopy2(&iwork[mppi2 - 1], n_pi, params_pi, mp1, n,
                  params, &ifl);

          /* Multiply back by pi / 2. */
          mpcopy2(pi_over_two, n_pi, params_pi, mp2,
                  n, params, &ifl);
          times(mp1, mp2, result, n, params, &ifl);
          *region = k;
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPREMAINDER_90D", &nrec, rec);
  return;

} /* mpremainder_piby2 */

/* Sin, cos, tan of an argument x in degrees or radians */
void mpsincostan_degrad(int degrees, int *xmp, int *n, int *params,
                        int *pi_over_two, int *two_over_pi, int *n_pi,
                        int *params_pi, int *result_sin, int *result_cos,
                        int *result_tan, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int region;
  int fptype;
  int *mp1, *iw1;
  char rec[125];
  int ifl;

  /* Computes sin(x), cos(x), tan(x), with the results in RESULT_SIN, */
  /* RESULT_COS, aand RESULT_TAN respectively. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params))
        {
          result_sin[fptype - 1] = qnancon();
          result_sin[sign - 1] = xmp[sign - 1];
          result_cos[fptype - 1] = qnancon();
          result_cos[sign - 1] = xmp[sign - 1];
          result_tan[fptype - 1] = qnancon();
          result_tan[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          assign(result_sin, xmp, n, params);
          itomp(&c_1, result_cos, n, params, &ifl);
          assign(result_tan, xmp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          iw1 = &iwork[*n];

          /* Reduce the argument xmp into the range [-pi/4,pi/4]. */
          if (degrees)
            mpremainder_90d(xmp, n, params, pi_over_two,
                            n_pi, params_pi, mp1, &region,
                            iw1, &ifl);
          else
            mpremainder_piby2(xmp, n, params, pi_over_two, two_over_pi,
                              n_pi, params_pi, mp1, &region,
                              iw1, &ifl);

          /* Compute sin and cos from the reduced argument */
          if (region == 0)
            {
              mpsin_kernel(mp1, n, params, result_sin, iw1);
              mpcos_kernel(mp1, n, params, result_cos, iw1);
            }
          else if (region == 1)
            {
              mpcos_kernel(mp1, n, params, result_sin, iw1);
              mpsin_kernel(mp1, n, params, result_cos, iw1);
              if (!mpiszero(result_cos, n, params))
                /* Don't negate it if it's zero */
                mpnegate(result_cos, n, params, &ifl);
            }
          else if (region == 2)
            {
              mpsin_kernel(mp1, n, params, result_sin, iw1);
              if (!mpiszero(result_sin, n, params))
                mpnegate(result_sin, n, params, &ifl);
              mpcos_kernel(mp1, n, params, result_cos, iw1);
              if (!mpiszero(result_cos, n, params))
                mpnegate(result_cos, n, params, &ifl);
            }
          else
            {
              mpcos_kernel(mp1, n, params, result_sin, iw1);
              if (!mpiszero(result_sin, n, params))
                mpnegate(result_sin, n, params, &ifl);
              mpsin_kernel(mp1, n, params, result_cos, iw1);
            }

          /* Adjust if input XMP was negative */
          if (mpisneg(xmp, n, params))
            {
              /* sin(-x) = -sin(x), cos(-x) = cos(x) */
              mpnegate(result_sin, n, params, &ifl);
            }

          /* Compute TAN as SIN/COS */
          if (mpiszero(result_cos, n, params))
            {
              result_tan[fptype - 1] = infcon();
              if (region == 1 || region == 3)
                result_tan[sign - 1] = poscon();
              else
                result_tan[sign - 1] = negcon();
              /* Adjust if input XMP was negative */
              if (mpisneg(xmp, n, params))
                {
                  /* tan(-x) = -tan(x) */
                  mpnegate(result_tan, n, params, &ifl);
                }
            }
          else
            {
              ifl = 0;
              divide(result_sin, result_cos, result_tan, n,
                     params, iw1, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPSINCOSTAN", &nrec, rec);
  return;

} /* mpsincostan */

/* Sin, cos, tan of an argument x in radians */
void mpsincostan(int *xmp, int *n, int *params,
                 int *pi_over_two, int *two_over_pi, int *n_pi,
                 int *params_pi, int *result_sin, int *result_cos,
                 int *result_tan, int *iwork, int *ifail)
{
  mpsincostan_degrad(0, xmp, n, params, pi_over_two, two_over_pi,
                     n_pi, params_pi, result_sin, result_cos,
                     result_tan, iwork, ifail);
}

/* Sin, cos, tan of an argument x in degrees */
void mpsincostand(int *xmp, int *n, int *params,
                  int *pi_over_two, int *two_over_pi, int *n_pi,
                  int *params_pi, int *result_sin, int *result_cos,
                  int *result_tan, int *iwork, int *ifail)
{
  mpsincostan_degrad(1, xmp, n, params, pi_over_two, two_over_pi,
                     n_pi, params_pi, result_sin, result_cos,
                     result_tan, iwork, ifail);
}

void mpcos_kernel(int *xmp, int *n, int *params,
                  int *result, int *iwork)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int ifac, sign;
  int savs;
  int params_short[15], short_mantis, i, k, ifail;
  int mpeps;
  int mp_tmp_1, mp_tmp_2, mpifac[40], *iw;
  int mpterm;
  int mpxsquared;
  int n_short;

  /* Computes cos(x), for -pi/4 <= x <= pi/4, with the result in RESULT. */

  /* REAL precision term, x, x2, res */
  ifail = 0;
  sign = get_sign(params);

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  i_1 = get_mantis(params);
  short_mantis = myimin(i_1, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifail);

  /* Dissect workspace */
  mpeps = *n * 10 + 1;
  mpterm = 1;
  mpxsquared = *n + 1;
  mp_tmp_1 = (*n * 2) + 1;
  mp_tmp_2 = *n * 3 + 1;
  iw = &iwork[(*n * 4)];

  /* IWORK(MPEPS) = eps */
  epsilon(n, params, &iwork[mpeps - 1]);

  /* term = IWORK(MPTERM) = 1 */
  itomp(&c_1, &iwork[mpterm - 1], n, params, &ifail);

  /* IWORK(MPXSQUARED) = x**2 */
  times(xmp, xmp, &iwork[mpxsquared - 1], n, params, &ifail);

  /* RESULT = 1 */
  assign(result, &iwork[mpterm - 1], n, params);

  i = 1;
 L20:

  /* Changed to avoid warning when compiling f2c translated code */
  /* IFAC = (2*I-1)*2*I */
  k = i * 2;
  ifac = (k - 1) * k;
  itomp(&ifac, mpifac, &n_short, params_short, &ifail);

  /* IWORK(MP_TMP_1) = term * x**2 */
  times(&iwork[mpterm - 1], &iwork[mpxsquared - 1], &iwork[mp_tmp_1 - 1], n, params,
        &ifail);

  /* IWORK(MPTERM) = IWORK(MPTERM) * x**2 / 2*i */
  divide2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
          params_short, &iwork[mpterm - 1], n, params, iw, &ifail);

  /* RESULT = RESULT +- term */
  if (i % 2 == 1)
    {
      minus(result, &iwork[mpterm - 1], &iwork[mp_tmp_2 - 1], n, params,
            &ifail);
    }
  else
    {
      plus(result, &iwork[mpterm - 1], &iwork[mp_tmp_2 - 1], n, params,
           &ifail);
    }
  assign(result, &iwork[mp_tmp_2 - 1], n, params);

  /* Repeat until the term is negligible */
  savs = iwork[sign - 1];
  iwork[sign - 1] = poscon();
  if (mpgt(&iwork[mpterm - 1], &iwork[mpeps - 1], n, params))
    {
      iwork[sign - 1] = savs;
      ++i;
      goto L20;
    }

  return;
} /* mpcos_kernel */

void mpatan_kernel(int *xmp, int *n, int *params, int *result, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int ifac, nrec, ierr;
  int params_short[15], short_mantis, i;
  int mpeps;
  int mp_tmp;
  int mpifac[40], *iw;
  int mp_pow;
  int mpterm;
  int mpxsquared;
  char rec[125];
  int ifl;
  int n_short;

  /* Computes atan(x), for 0 <= x <= pi/4, with the result in RESULT. */
  /* Use atan(x) = x - x**3/3 + x**5 / 5 - x**7 / 7 + ... */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  i_1 = get_mantis(params);
  short_mantis = myimin(i_1, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifl);

  /* Dissect workspace */
  mpeps = *n * 10 + 1;
  mp_pow = 1;
  mpxsquared = *n + 1;
  mpterm = (*n * 2) + 1;
  mp_tmp = *n * 3 + 1;
  iw = &iwork[*n * 3];

  /* IWORK(MPEPS) = eps */
  epsilon(n, params, &iwork[mpeps - 1]);

  /* pow = IWORK(MP_POW) = x */
  assign(&iwork[mp_pow - 1], xmp, n, params);

  /* IWORK(MPXSQUARED) = x**2 */
  times(xmp, xmp, &iwork[mpxsquared - 1], n, params, &ifl);

  /* RESULT = x */
  assign(result, xmp, n, params);

  /* If xmp < epsilon, result is xmp, and there's nothing more to do. */
  /* Otherwise, iterate. */
  if (mpgt(&iwork[mp_pow - 1], &iwork[mpeps - 1], n, params))
    {
      i = 1;
    L20:

      /* mpifac = 2*i+1 */
      ifac = (i * 2) + 1;
      itomp(&ifac, mpifac, &n_short, params_short, &ifl);

      /* pow = pow * x**2 */
      times(&iwork[mp_pow - 1], &iwork[mpxsquared - 1], &iwork[mpterm - 1], n,
            params, &ifl);
      assign(&iwork[mp_pow - 1], &iwork[mpterm - 1], n, params);

      /* term = pow / ifac */
      divide2(&iwork[mp_pow - 1], n, params, mpifac, &n_short,
              params_short, &iwork[mpterm - 1], n, params, iw,
              &ifl);

      /* RESULT = RESULT +- term */
      if (i % 2 == 1)
        {
          minus(result, &iwork[mpterm - 1], &iwork[mp_tmp - 1], n, params,
                &ifl);
        }
      else
        {
          plus(result, &iwork[mpterm - 1], &iwork[mp_tmp - 1], n, params,
               &ifl);
        }
      assign(result, &iwork[mp_tmp - 1], n, params);

      /* CALL MPSHOW('tsum = ',result,N,PARAMS,0) */

      /* Check for underflow in the term */
      if (mpiszero(&iwork[mpterm - 1], n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "Power series term underflows in computation; result inaccurate");
          goto L40;
        }

      /* Repeat until the term is negligible */
      if (mpgt(&iwork[mpterm - 1], &iwork[mpeps - 1], n, params))
        {
          ++i;
          goto L20;
        }
    }

 L40:
  *ifail = p01abf(ifail, &ierr, "MPATAN_KERNEL", &nrec, rec);
  return;

} /* mpatan_kernel */

void mpatan(int *xmp, int *n, int *params,
            int *result, int *pi_over_two,
            int *n_pi, int *params_pi,
            int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int ifac, nrec, sign, ierr, mpax, nreq;
  int params_short[15], short_mantis, i;
  int mpone[40];
  int mpeps;
  int mpifac[40];
  int mp_pow;
  int mpterm, fptype;
  int *mp1, *mp2, *iw1, *iw2, *mp3, *mp4;
  char rec[125];
  int ifl;
  int n_short;

  /* Computes atan(x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_pi))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_PI was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (*n_pi < get_mplen(params_pi))
        {
          ierr = 1;
          nrec = 1;
          i_1 = get_mplen(params_pi);
          sprintf(rec, "%s %d %s %d", "N_PI is too small. N_PI must be at least",
                  i_1, ": N_PI = ", *n_pi);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(pi_over_two, n_pi, params_pi))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "PI_OVER_TWO is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = xmp[fptype - 1];
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is zero with correct sign */
          assign(result, xmp, n, params);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* Result is pi/2 */
          mpcopy2(pi_over_two, n_pi, params_pi, result,
                  n, params, &ifl);
          if (mpisneg(xmp, n, params))
            result[sign - 1] = negcon();
        }
      else
        {
          /* Set up some parameters for numbers which need only low */
          /* (30 bit) precision. Be careful - don't let short_mantis */
          /* be bigger than the regular one, or your workspace may be */
          /* too small for calls of divide2. */
          i_1 = get_mantis(params);
          short_mantis = myimin(i_1, c_30);
          i_1 = get_emin(params);
          i_2 = get_emax(params);
          mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
                     params_short, &ifl);

          /* Dissect workspace */
          mpeps = *n * 12 + 1;
          mpax = 1;
          mpterm = 1;
          mp_pow = 1;
          mp1 = &iwork[*n];
          mp2 = &iwork[(*n * 2)];
          mp3 = &iwork[*n * 3];
          mp4 = &iwork[(*n * 4)];
          iw1 = mp4;
          iw2 = &iwork[*n * 5];

          /* IWORK(MPEPS) = eps */
          epsilon(n, params, &iwork[mpeps - 1]);

          /* pow = IWORK(MPAX) = abs(x) */
          assign(&iwork[mpax - 1], xmp, n, params);
          mpabs(&iwork[mpax - 1], n, params, &ifl);

          /* IWORK(MP1) = 1/2 */
          itomp(&c_1, mpifac, &n_short, params_short, &ifl);
          itomp(&c_2, mp3, n, params, &ifl);
          divide2(mpifac, &n_short, params_short, mp3, n,
                  params, mp1, n, params, iw1, &ifl);

          /* IWORK(MP2) = 3/2 */
          itomp(&c_3, mpifac, &n_short, params_short, &ifl);
          divide2(mpifac, &n_short, params_short, mp3, n,
                  params, mp2, n, params, iw1, &ifl);

          /* IWORK(MP3) = 2 / EPS */
          itomp(&c_2, mpifac, &n_short, params_short, &ifl);
          divide2(mpifac, &n_short, params_short, &iwork[mpeps - 1], n,
                  params, mp3, n, params, iw1, &ifl);

          /* We use different series expansions depending */
          /* on the size of XMP (see Abramowitz and Stegun, 4.4.42, page 81). */
          if (mplt(&iwork[mpax - 1], mp1, n, params))
            {
              /* 0.0 <= abs(xmp) <= 0.5 */
              /* Use atan(x) = x - x**3/3 + x**5 / 5 - x**7 / 7 + ... */
              mpatan_kernel(&iwork[mpax - 1], n, params, result,
                            &iwork[*n], &ifl);
            }
          else if (mplt(&iwork[mpax - 1], mp2, n, params))
            {
              /* 0.5 <= abs(xmp) <= 1.5 */
              /* Use atan(x) = */
              /* x / (1 + x**2) * [1 + u*2/3 + u**2*2*4/(3*5) + u**3*2*4*6/(3*5*7) + ...] */
              /* where u = x**2 / (1 + x**2). */

              /* IWORK(MP1) = XMP**2 */
              times(&iwork[mpax - 1], &iwork[mpax - 1], mp1, n, params,
                    &ifl);

              /* IWORK(MP2) = 1 + XMP**2 */
              itomp(&c_1, mp3, n, params, &ifl);
              plus(mp3, mp1, mp2, n, params,
                   &ifl);

              /* IWORK(MP3) = pow = XMP**2 / (1 + XMP**2) */
              divide(mp1, mp2, mp3, n, params,
                     iw1, &ifl);

              /* RESULT = term = 1 */
              itomp(&c_1, &iwork[mpterm - 1], n, params, &ifl);
              assign(result, &iwork[mpterm - 1], n, params);
              /* call mpshow('term = ', iwork(MPTERM),n,params,6,0) */

              i = 1;
            L20:

              /* iwork(MPTERM) = term = term * pow * 2*i / (2 * i + 1) */
              times(&iwork[mpterm - 1], mp3, mp2, n, params, &ifl);
              ifac = i * 2;
              itomp(&ifac, mpifac, &n_short, params_short, &ifl);
              times2(mpifac, &n_short, params_short, mp2, n,
                     params, mp4, n, params, &ifl);
              ifac = (i * 2) + 1;
              itomp(&ifac, mpifac, &n_short, params_short, &ifl);
              divide2(mp4, n, params, mpifac, &n_short,
                      params_short, &iwork[mpterm - 1], n, params, iw2, &ifl);
              /* call mpshow('term = ', iwork(MPTERM),n,params,6,0) */

              /* RESULT = RESULT + term */
              plus(result, &iwork[mpterm - 1], mp2, n, params,
                   &ifl);
              assign(result, mp2, n, params);

              /* CALL MPSHOW('tsum = ',result,N,PARAMS,0) */

              /* Check for underflow in the term */
              if (mpiszero(mp2, n, params))
                {
                  ierr = 3;
                  nrec = 1;
                  sprintf(rec, "Power series term underflows in computation; result inaccurate");
                  goto L60;
                }

              /* Repeat until the term is negligible */
              /* CALL MPSHOW('eps = ',iwork(MPEPS),N,PARAMS,0) */
              if (mpgt(&iwork[mpterm - 1], &iwork[mpeps - 1], n, params))
                {
                  ++i;
                  goto L20;
                }

              /* Finally multiply by abs(x) / (1 + x**2) */
              /* IWORK(MP1) = XMP**2 */
              assign(&iwork[mpax - 1], xmp, n, params);
              mpabs(&iwork[mpax - 1], n, params, &ifl);
              times(&iwork[mpax - 1], &iwork[mpax - 1], mp1, n, params,
                    &ifl);
              /* IWORK(MP2) = 1 + XMP**2 */
              itomp(&c_1, mp3, n, params, &ifl);
              plus(mp3, mp1, mp2, n, params,
                   &ifl);
              /* RESULT = RESULT * abs(x) / (1 + x**2) */
              times(result, &iwork[mpax - 1], mp3, n, params,
                    &ifl);
              divide(mp3, mp2, result, n, params,
                     iw1, &ifl);
              /* CALL MPSHOW('result = ',result,N,PARAMS,0) */
            }
          else if (mpgt(&iwork[mpax - 1], mp3, n, params))
            {
              /* abs(xmp) > 2 / epsilon; result is pi/2 */
              mpcopy2(pi_over_two, n_pi, params_pi, result, n, params, &ifl);
            }
          else
            {
              /* 1.5 <= abs(xmp) <= 2 / epsilon */
              /* Use atan(x) = pi/2 - 1/x + 1/(3*x**3) - 1/(5*x**5) + ... */

              /* IWORK(MP1) = x**2 */
              times(&iwork[mpax - 1], &iwork[mpax - 1], mp1, n, params,
                    &ifl);

              /* MPONE = 1 */
              itomp(&c_1, mpone, &n_short, params_short, &ifl);

              /* RESULT = 1/abs(x) */
              divide2(mpone, &n_short, params_short, &iwork[mpax - 1], n,
                      params, result, n, params, mp2,
                      &ifl);

              i = 1;
            L40:

              /* ifac = iwork(MP3) = 2*i+1 */
              ifac = (i * 2) + 1;
              itomp(&ifac, mpifac, &n_short, params_short, &ifl);

              /* pow = pow * x**2 */
              times(&iwork[mp_pow - 1], mp1, mp2, n, params, &ifl);
              assign(&iwork[mp_pow - 1], mp2, n, params);

              /* iwork(MP2) = term = 1 / (pow * ifac) */
              times2(&iwork[mp_pow - 1], n, params, mpifac, &n_short,
                     params_short, mp3, n, params, &ifl);
              divide2(mpone, &n_short, params_short, mp3, n,
                      params, mp2, n, params, iw1,
                      &ifl);
              /* CALL MPSHOW('term = ',iwork(MP2),N,PARAMS,0) */

              /* RESULT = RESULT +- term */
              if (i % 2 == 1)
                minus(result, mp2, mp3, n, params, &ifl);
              else
                plus(result, mp2, mp3, n, params, &ifl);
              assign(result, mp3, n, params);
              /* CALL MPSHOW('tsum = ',result,N,PARAMS,0) */

              /* Check for underflow in the term */
              if (mpiszero(mp2, n, params))
                {
                  ierr = 3;
                  nrec = 1;
                  sprintf(rec, "Power series term underflows in computation; result inaccurate");
                  goto L60;
                }

              /* Repeat until the term is negligible */
              if (mpgt(mp2, &iwork[mpeps - 1], n, params))
                {
                  ++i;
                  goto L40;
                }

              /* Finally subtract the result from pi/2 */
              mpcopy2(pi_over_two, n_pi, params_pi, mp1,
                      n, params, &ifl);
              minus(mp1, result, &iwork[mpax - 1], n, params,
                    &ifl);
              assign(result, &iwork[mpax - 1], n, params);
            }

          /* Adjust the sign of the result if necessary. */
          if (mpisneg(xmp, n, params))
            result[sign - 1] = negcon();
        }
    }

 L60:
  *ifail = p01abf(ifail, &ierr, "MPATAN", &nrec, rec);
  return;

} /* mpatan */

void mpatan2(int *ymp, int *xmp, int *n, int
             *params, int *result, int *pi, int *pi_over_two,
             int *n_pi, int *params_pi, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int nrec, sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2, *mp3;
  char rec[125];
  int ifl;

  /* Computes atan2(y,x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_pi))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_PI was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (*n_pi < get_mplen(params_pi))
        {
          ierr = 1;
          nrec = 1;
          i_1 = get_mplen(params_pi);
          sprintf(rec, "%s %d %s %d", "N_PI is too small. N_PI must be at least",
                  i_1, ": N_PI = ", *n_pi);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(pi_over_two, n_pi, params_pi))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "PI_OVER_TWO is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisnan(ymp, n, params))
        {
          if (mpisnan(xmp, n, params))
            {
              result[fptype - 1] = xmp[fptype - 1];
              result[sign - 1] = xmp[sign - 1];
            }
          else
            {
              result[fptype - 1] = ymp[fptype - 1];
              result[sign - 1] = ymp[sign - 1];
            }
        }
      else if (mpiszero(ymp, n, params) && mpiszero(xmp, n, params))
        {
          if (mpisneg(xmp, n, params))
            {
              mpcopy2(pi, n_pi, params_pi, result, n,
                      params, &ifl);
              result[sign - 1] = ymp[sign - 1];
            }
          else
            assign(result, ymp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];

          if (mpisinfinity(ymp, n, params))
            {
              if (mpisinfinity(xmp, n, params))
                {
                if (mpisneg(xmp, n, params))
                  {
                  /* Result is 3 Pi / 4 with sign of YMP */
                  mpcopy2(pi, n_pi, params_pi, mp1, n,
                          params, &ifl);
                  itomp(&c_4, mp2, n, params, &ifl);
                  divide(mp1, mp2, result, n, params,
                         mp3, &ifl);
                  itomp(&c_3, mp1, n, params, &ifl);
                  times(result, mp1, mp2, n, params, &ifl);
                  mpcopy(mp2, result, n, params, &ifl);
                  result[sign - 1] = ymp[sign - 1];
		  }
                else
		  {
                  /* Result is Pi / 4 with sign of YMP */
                  mpcopy2(pi, n_pi, params_pi, mp1, n,
                          params, &ifl);
                  itomp(&c_4, mp2, n, params, &ifl);
                  divide(mp1, mp2, result, n, params,
                         mp3, &ifl);
                  result[sign - 1] = ymp[sign - 1];
                  }
                }
              else
                {
                  /* Result is Pi / 2 with sign of YMP */
                  mpcopy2(pi, n_pi, params_pi, mp1, n,
                          params, &ifl);
                  itomp(&c_2, mp2, n, params, &ifl);
                  divide(mp1, mp2, result, n, params,
                         mp3, &ifl);
                  result[sign - 1] = ymp[sign - 1];
                }
            }
          else if (mpisinfinity(xmp, n, params))
            {
              if (mpispos(xmp, n, params))
                {
                  /* Result is zero with sign of YMP */
                  result[fptype - 1] = zercon();
                  result[sign - 1] = ymp[sign - 1];
                }
              else
                {
                  /* Result is Pi with sign of YMP */
                  mpcopy2(pi, n_pi, params_pi, result, n,
                          params, &ifl);
                  result[sign - 1] = ymp[sign - 1];
                }
            }
          else
            {
              /* Compute atan(y/x) then adjust the result */
              /* depending on the signs of y and x */
              divide(ymp, xmp, mp1, n, params,
                     mp2, &ifl);
              mpatan(mp1, n, params, result,
                     pi_over_two, n_pi, params_pi, mp2, ifail);
              if (mpisneg(xmp, n, params))
                {
                  mpcopy2(pi, n_pi, params_pi, mp1, n,
                          params, &ifl);
                  if (mpispos(ymp, n, params))
                    /* Add Pi to the result */
                    plus(result, mp1, mp2, n, params, &ifl);
                  else
                    /* Subtract Pi from the result */
                    minus(result, mp1, mp2, n, params, &ifl);
                  assign(result, mp2, n, params);
                }
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPATAN2", &nrec, rec);
  return;

} /* mpatan2 */

void mpasin_kernel(int *xmp, int *n, int *params, int *result,
                   int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int iexo;
  int params_short[15], short_mantis;
  int k;
  int mpsum;
  int mp_tmp_1, mp_tmp_2, mpifac[40], *iw;
  int mantis;
  int mpterm;
  int mpxsquared;
  char rec[125];
  int ifl, n_short;

  /* Computes asin(xmp), 0.0 <= xmp <= 0.5, with the result in RESULT. */

  ifl = 0;
  *ifail = 0;

  mantis = get_mantis(params);
  iexo = get_iexo(params);

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  short_mantis = myimin(mantis, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifl);

  /* Dissect workspace */
  mpxsquared = 1;
  mpterm = *n + 1;
  mpsum = (*n * 2) + 1;
  mp_tmp_1 = *n * 3 + 1;
  mp_tmp_2 = (*n * 4) + 1;
  iw = &iwork[*n * 5];

  /* IWORK(MP_TMP_1) = 1 */
  itomp(&c_1, &iwork[mp_tmp_1 - 1], n, params, &ifl);

  /* IWORK(MP_TMP_2) = 1/2 */
  itomp(&c_2, mpifac, &n_short, params_short, &ifl);
  divide2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
          params_short, &iwork[mp_tmp_2 - 1], n, params, iw, &ifl);

  /* We use a series expansion (see Abramowitz and Stegun, */
  /* 4.4.40, 4.4.41, page 81). */
  if (mpisneg(xmp, n, params) || mpgt(xmp, &iwork[mp_tmp_2 - 1], n, params))
    {
      sprintf(rec, "Panic: MPASIN_KERNEL called with XMP > 0.5 or XMP < 0.0");
      *ifail = p01abf(&c_0, &c_1, "MPASIN_KERNEL", &c_1, rec);
    }
  else if (mpiszero(xmp, n, params) || xmp[iexo - 1] <= -mantis - 2)
    assign(result, xmp, n, params);
  else
    {
      /* 0.0 <= abs(xmp) <= 0.5 */
      /* Use asin(x) = x + 1*x**3/(2*3) + 1*3*x**5/(2*4*5) + */
      /* 1*3*5*x**7/(2*4*6*7) + 1*3*5*7*x**9/(2*4*6*8*9) + ... */
      /* The series converges at about 1 decimal place per iteration */
      /* in the worst case (x = 0.5). */

      /* IWORK(MPXSQUARED) = x * x */
      times(xmp, xmp, &iwork[mpxsquared - 1], n, params, &ifl);

      /* IWORK(MPTERM) = term = x */
      assign(&iwork[mpterm - 1], xmp, n, params);

      /* IWORK(MPSUM) = sum = x */
      assign(&iwork[mpsum - 1], xmp, n, params);

      k = 3;

    L20:

      /* IWORK(MP_TMP_1) = term / (k - 1) */
      i_1 = k - 1;
      itomp(&i_1, mpifac, &n_short, params_short, &ifl);
      divide2(&iwork[mpterm - 1], n, params, mpifac, &n_short,
              params_short, &iwork[mp_tmp_1 - 1], n, params, iw,
              &ifl);

      /* IWORK(MP_TMP_2) = IWORK(MP_TMP_1) * (k - 2)**2 */
      i_1 = (k - 2) * (k - 2);
      itomp(&i_1, mpifac, &n_short, params_short, &ifl);
      times2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
             params_short, &iwork[mp_tmp_2 - 1], n, params, &ifl);

      /* IWORK(MP_TMP_1) = IWORK(MP_TMP_2) / k */
      itomp(&k, mpifac, &n_short, params_short, &ifl);
      divide2(&iwork[mp_tmp_2 - 1], n, params, mpifac, &n_short,
              params_short, &iwork[mp_tmp_1 - 1], n, params, iw,
              &ifl);

      /* IWORK(MPTERM) = IWORK(MP_TMP_1) * x**2 */
      times(&iwork[mp_tmp_1 - 1], &iwork[mpxsquared - 1],
            &iwork[mpterm - 1], n, params, &ifl);

      /* Check for underflow in the term */
      if (mpiszero(&iwork[mpterm - 1], n, params))
        {
          sprintf(rec, "Power series term underflows in computation; result inaccurate");
          *ifail = p01abf(&c_0, &c_2, "MPASIN_KERNEL", &c_1, rec);
          goto L40;
        }

      /* RESULT = old sum */
      assign(result, &iwork[mpsum - 1], n, params);

      /* IWORK(MPSUM) = sum = sum + term */
      plus(&iwork[mpsum - 1], &iwork[mpterm - 1], &iwork[mp_tmp_2 - 1], n, params,
           &ifl);
      assign(&iwork[mpsum - 1], &iwork[mp_tmp_2 - 1], n, params);

      /* Continue while the sum changes. Otherwise the correct */
      /* answer is already in RESULT. */
      if (mpgt(&iwork[mpsum - 1], result, n, params))
        {
          k += 2;
          goto L20;
        }
    }

 L40:
  return;
} /* mpasin_kernel */

void mpacos_kernel(int *xmp, int *n, int *params, int *result,
                   int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int sign;
  int params_short[15], short_mantis;
  int k;
  int mpsum;
  int mp_tmp_1, mp_tmp_2, kt, *iw, mpifac[40];
  int mpterm, fptype;
  char rec[125];
  int ifl, mpw, n_short;

  /* Computes acos(xmp), 0.5 <= abs(xmp) <= 1.0, with the result in RESULT. */

  ifl = 0;
  *ifail = 0;

  sign = get_sign(params);
  fptype = get_fptype(params);

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  i_1 = get_mantis(params);
  short_mantis = myimin(i_1, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifl);

  /* Dissect workspace */
  mpw = 1;
  mpterm = *n + 1;
  mpsum = (*n * 2) + 1;
  mp_tmp_1 = *n * 3 + 1;
  mp_tmp_2 = (*n * 4) + 1;
  iw = &iwork[*n * 5];

  /* Use Abramowitz and Stegun 4.4.41 (too hard to write down!) */
  /* The series converges at about 0.66 decimal places per */
  /* iteration in the worst case (x = 0.5). */

  /* IWORK(MPTERM) = term = 1 */
  itomp(&c_1, &iwork[mpterm - 1], n, params, &ifl);

  /* IWORK(MPW) = w = 1 - x */
  minus(&iwork[mpterm - 1], xmp, &iwork[mpw - 1], n, params, &ifl);

  /* IWORK(MPSUM) = sum = 0 */
  itomp(&c_0, &iwork[mpsum - 1], n, params, &ifl);

  if (mpiszero(&iwork[mpw - 1], n, params))
    {
      /* Argument XMP was 1.0 or very close; result is zero */
      result[fptype - 1] = zercon();
      result[sign - 1] = poscon();
    }
  else
    {
      k = 1;

    L20:
      kt = (k * 2) - 1;
      /* IWORK(MP_TMP_1) = term / (2 * k + 1) */
      i_1 = kt + 2;
      itomp(&i_1, mpifac, &n_short, params_short, &ifl);
      divide2(&iwork[mpterm - 1], n, params, mpifac, &n_short,
              params_short, &iwork[mp_tmp_1 - 1], n, params,
              &iwork[mp_tmp_2 - 1], &ifl);

      /* IWORK(MP_TMP_2) = IWORK(MP_TMP_1) * (2 * k - 1)**2 */
      i_1 = kt * kt;
      itomp(&i_1, mpifac, &n_short, params_short, &ifl);
      times2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
             params_short, &iwork[mp_tmp_2 - 1], n, params, &ifl);

      /* IWORK(MP_TMP_1) = IWORK(MP_TMP_2) / (4 * k) */
      i_1 = k * 4;
      itomp(&i_1, mpifac, &n_short, params_short, &ifl);
      divide2(&iwork[mp_tmp_2 - 1], n, params, mpifac, &n_short,
              params_short, &iwork[mp_tmp_1 - 1], n, params,
              &iwork[*n * 6], &ifl);

      /* IWORK(MPTERM) = IWORK(MP_TMP_1) * w */
      times(&iwork[mp_tmp_1 - 1], &iwork[mpw - 1], &iwork[mpterm - 1],
            n, params, &ifl);

      /* Check for underflow in the term */
      if (mpiszero(&iwork[mpterm - 1], n, params))
        {
          sprintf(rec, "Power series term underflows in computation; result inaccurate");
          ifl = p01abf(&c_0, &c_2, "MPASIN_KERNEL", &c_1, rec);
          goto L40;
        }

      /* IWORK(MP_TMP_1) = old sum */
      assign(&iwork[mp_tmp_1 - 1], &iwork[mpsum - 1], n, params);

      /* IWORK(MP_TMP_2) = sum = sum + term */
      plus(&iwork[mpsum - 1], &iwork[mpterm - 1], &iwork[mp_tmp_2 - 1],
           n, params, &ifl);
      assign(&iwork[mpsum - 1], &iwork[mp_tmp_2 - 1], n, params);

      if (mpgt(&iwork[mpsum - 1], &iwork[mp_tmp_1 - 1], n, params))
        {
          ++k;
          goto L20;
        }

      /* IWORK(MP_TMP_1) = sum + 1 */
      itomp(&c_1, &iwork[mp_tmp_2 - 1], n, params, &ifl);
      plus(&iwork[mpsum - 1], &iwork[mp_tmp_2 - 1], &iwork[mp_tmp_1 - 1],
           n, params, &ifl);

      /* RESULT = (sum + 1) * sqrt(2 * w) */
      plus(&iwork[mpw - 1], &iwork[mpw - 1], &iwork[mp_tmp_2 - 1],
           n, params, &ifl);
      mproot(&iwork[mp_tmp_2 - 1], &iwork[mpsum - 1],
             n, params, iw, &ifl);
      times(&iwork[mp_tmp_1 - 1], &iwork[mpsum - 1], result,
            n, params, &ifl);
    }

 L40:
  return;

} /* mpacos_kernel */

void mpasin(int *xmp, int *n, int *params,
            int *result, int *pi_over_two,
            int *n_pi, int *params_pi,
            int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec;
  int sign, ierr, mpax, nreq;
  int params_short[15], short_mantis;
  int mp_tmp_1, mp_tmp_2, mpifac[40], *iw;
  int fptype;
  char rec[125];
  int ifl;
  int n_short;

  /* Computes asin(x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_pi))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_PI was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (*n_pi < get_mplen(params_pi))
        {
          ierr = 1;
          nrec = 1;
          i_1 = get_mplen(params_pi);
          sprintf(rec, "%s %d %s %d", "N_PI is too small. N_PI must be at least",
                  i_1, ": N_PI = ", *n_pi);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(pi_over_two, n_pi, params_pi))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "PI_OVER_TWO is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = xmp[fptype - 1];
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is zero with correct sign */
          assign(result, xmp, n, params);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else
        {
          /* Set up some parameters for numbers which need only low */
          /* (30 bit) precision. Be careful - don't let short_mantis */
          /* be bigger than the regular one, or your workspace may be */
          /* too small for calls of divide2. */
          i_1 = get_mantis(params);
          short_mantis = myimin(i_1, c_30);
          i_1 = get_emin(params);
          i_2 = get_emax(params);
          mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
                     params_short, &ifl);

          /* Dissect workspace */
          mpax = 1;
          mp_tmp_1 = *n + 1;
          mp_tmp_2 = (*n * 2) + 1;
          iw = &iwork[*n * 3];

          /* IWORK(MPAX) = abs(x) */
          assign(&iwork[mpax - 1], xmp, n, params);
          mpabs(&iwork[mpax - 1], n, params, &ifl);

          /* IWORK(MP_TMP_1) = 1 */
          itomp(&c_1, &iwork[mp_tmp_1 - 1], n, params, &ifl);

          if (mpgt(&iwork[mpax - 1], &iwork[mp_tmp_1 - 1], n, params))
            {
              /* Argument is bigger than 1.0 in magnitude - illegal. */
              result[fptype - 1] = qnancon();
              result[sign - 1] = poscon();
            }
          else
            {
              /* IWORK(MP_TMP_2) = 1/2 */
              itomp(&c_2, mpifac, &n_short, params_short, &ifl);
              divide2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
                      params_short, &iwork[mp_tmp_2 - 1], n, params, iw, &ifl);

              /* We use two different series expansions depending on the */
              /* size of XMP (see Abramowitz and Stegun, 4.4.40, 4.4.41, */
              /* page 81). */
              if (mple(&iwork[mpax - 1], &iwork[mp_tmp_2 - 1], n, params))
                {
                  /* 0.0 <= abs(xmp) <= 0.5 */
                  mpasin_kernel(&iwork[mpax - 1], n, params, result,
                                iw, &ifl);
                }
              else
                {
                  /* 0.5 <= abs(xmp) <= 1.0 */
                  mpacos_kernel(&iwork[mpax - 1], n, params,
                                &iwork[mp_tmp_1 - 1], iw, &ifl);

                  /* RESULT = pi / 2 - IWORK(MP_TMP_1) */
                  mpcopy2(pi_over_two, n_pi, params_pi,
                          &iwork[mp_tmp_2 - 1], n, params, &ifl);
                  minus(&iwork[mp_tmp_2 - 1], &iwork[mp_tmp_1 - 1], result,
                        n, params, &ifl);
                }
            }
        }

      /* Adjust the sign of the result if necessary. */
      if (mpisneg(xmp, n, params))
        {
          sign = get_sign(params);
          result[sign - 1] = negcon();
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPASIN", &nrec, rec);
  return;

} /* mpasin */

void mpacos(int *xmp, int *n, int *params,
            int *result, int *pi_over_two,
            int *n_pi, int *params_pi,
            int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec;
  int sign, ierr, mpax, nreq;
  int params_short[15], short_mantis;
  int mp_tmp_1, mp_tmp_2, mpifac[40], *iw;
  int fptype;
  char rec[125];
  int ifl;
  int n_short;

  /* Computes acos(x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (! chksum(params_pi))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS_PI was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (*n_pi < get_mplen(params_pi))
        {
          ierr = 1;
          nrec = 1;
          i_1 = get_mplen(params_pi);
          sprintf(rec, "%s %d %s %d", "N_PI is too small. N_PI must be at least",
                  i_1, ": N_PI = ", *n_pi);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(pi_over_two, n_pi, params_pi))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "PI_OVER_TWO is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is pi/2 */
          mpcopy2(pi_over_two, n_pi, params_pi, result,
                  n, params, &ifl);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else
        {
          /* Set up some parameters for numbers which need only low */
          /* (30 bit) precision. Be careful - don't let short_mantis */
          /* be bigger than the regular one, or your workspace may be */
          /* too small for calls of divide2. */
          i_1 = get_mantis(params);
          short_mantis = myimin(i_1, c_30);
          i_1 = get_emin(params);
          i_2 = get_emax(params);
          mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
                     params_short, &ifl);

          /* Dissect workspace */
          mpax = 1;
          mp_tmp_1 = *n + 1;
          mp_tmp_2 = (*n * 2) + 1;
          iw = &iwork[*n * 3];

          /* IWORK(MPAX) = abs(x) */
          assign(&iwork[mpax - 1], xmp, n, params);
          mpabs(&iwork[mpax - 1], n, params, &ifl);

          /* IWORK(MP_TMP_1) = 1 */
          itomp(&c_1, &iwork[mp_tmp_1 - 1], n, params, &ifl);

          if (mpgt(&iwork[mpax - 1], &iwork[mp_tmp_1 - 1], n, params))
            {
              /* Argument is bigger than 1.0 in magnitude - illegal. */
              result[fptype - 1] = qnancon();
              result[sign - 1] = poscon();
            }
          else
            {
              /* IWORK(2*N+1) = 1/2 */
              itomp(&c_2, mpifac, &n_short, params_short, &ifl);
              divide2(&iwork[mp_tmp_1 - 1], n, params, mpifac, &n_short,
                      params_short, &iwork[(*n * 2)], n, params,
                      iw, &ifl);

              /* We use two different series expansions depending on the */
              /* size of XMP (see Abramowitz and Stegun, 4.4.40, 4.4.41, */
              /* page 81). */
              if (mple(&iwork[mpax - 1], &iwork[(*n * 2)], n, params))
                {
                  mpasin_kernel(&iwork[mpax - 1], n, params,
                                &iwork[mp_tmp_1 - 1], iw, &ifl);
                  mpcopy2(pi_over_two, n_pi, params_pi,
                          &iwork[mp_tmp_2 - 1], n, params, &ifl);
                  if (mpispos(xmp, n, params))
                    {
                      /* RESULT = pi / 2 - IWORK(MP_TMP_1) */
                      minus(&iwork[mp_tmp_2 - 1], &iwork[mp_tmp_1 - 1],
                            result, n, params, &ifl);
                    }
                  else
                    {
                      /* RESULT = pi / 2 + IWORK(MP_TMP_1) */
                      plus(&iwork[mp_tmp_2 - 1], &iwork[mp_tmp_1 - 1],
                           result, n, params, &ifl);
                    }
                }
              else
                {
                  /* 0.5 <= abs(xmp) <= 1.0 */
                  mpacos_kernel(&iwork[mpax - 1], n, params, result,
                                iw, &ifl);
                  if (mpisneg(xmp, n, params))
                    {
                      /* RESULT = pi - RESULT */
                      mpcopy2(pi_over_two, n_pi, params_pi,
                              &iwork[mp_tmp_1 - 1], n, params, &ifl);
                      plus(&iwork[mp_tmp_1 - 1], &iwork[mp_tmp_1 - 1],
                           &iwork[mp_tmp_2 - 1], n, params, &ifl);
                      minus(&iwork[mp_tmp_2 - 1], result,
                            &iwork[mp_tmp_1 - 1], n, params, &ifl);
                      assign(result, &iwork[mp_tmp_1 - 1], n, params);
                    }
                }
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPACOS", &nrec, rec);
  return;

} /* mpacos */

void mpsinh(int *xmp, int *n, int *params,
            int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2, *iw1, *iw2;
  char rec[125];
  int ifl;

  /* Computes sinh(x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is zero with correct sign */
          assign(result, xmp, n, params);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* Result is infinity with correct sign */
          assign(result, xmp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          iw1 = &iwork[(*n * 2)];
          iw2 = &iwork[*n * 3];

          /* iwork(IW1) = 1/16 */
          itomp(&c_1, mp1, n, params, &ifl);
          itomp(&c_16, mp2, n, params, &ifl);
          divide(mp1, mp2, iw1, n, params,
                 iw2, &ifl);

          /* iwork(MP2) = abs(x) */
          assign(mp2, xmp, n, params);
          mpabs(mp2, n, params, &ifl);

          if (mple(mp2, iw1, n, params))
            {
              /* If abs(XMP) <= 1/16, call the kernel routine. */
              mpsinh_kernel(xmp, n, params, result, iwork);
            }
          else
            {
              /* Just use sinh(x) = (exp(x) - exp(-x)) / 2 */
              mpexp(xmp, n, params, result, iw1, &ifl);
              itomp(&c_1, mp1, n, params, &ifl);
              divide(mp1, result, mp2, n, params, iw1, &ifl);
              minus(result, mp2, mp1, n, params, &ifl);
              itomp(&c_2, mp2, n, params, &ifl);
              divide(mp1, mp2, result, n, params, iw1, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPSINH", &nrec, rec);
  return;

} /* mpsinh */

void mpcosh(int *xmp, int *n, int *params,
            int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2, *iw1, *iw2;
  char rec[125];
  int ifl;

  /* Computes cosh(x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is +1.0 */
          itomp(&c_1, result, n, params, &ifl);
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* Result is +infinity */
          result[fptype - 1] = infcon();
          result[sign - 1] = poscon();
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          iw1 = &iwork[(*n * 2)];
          iw2 = &iwork[*n * 3];

          /* iwork(IW1) = 1/16 */
          itomp(&c_1, mp1, n, params, &ifl);
          itomp(&c_16, mp2, n, params, &ifl);
          divide(mp1, mp2, iw1, n, params,
                 iw2, &ifl);

          /* iwork(MP2) = abs(x) */
          assign(mp2, xmp, n, params);
          mpabs(mp2, n, params, &ifl);

          if (mple(mp2, iw1, n, params))
            {
              /* If abs(XMP) <= 1/16, call the kernel routine. */
              mpcosh_kernel(xmp, n, params, result, iwork);
            }
          else
            {
              /* Just use cosh(x) = (exp(x) + exp(-x)) / 2 */
              mpexp(xmp, n, params, result, iw1, &ifl);
              itomp(&c_1, mp1, n, params, &ifl);
              divide(mp1, result, mp2, n, params, iw1, &ifl);
              plus(result, mp2, mp1, n, params, &ifl);
              itomp(&c_2, mp2, n, params, &ifl);
              divide(mp1, mp2, result, n, params, iw1, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPCOSH", &nrec, rec);
  return;

} /* mpcosh */

void mptanh(int *xmp, int *n, int *params,
            int *result, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int fptype, *mp1, *iw1, *iw2;
  char rec[125];
  int ifl;

  /* Computes tanh(x), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          iw1 = &iwork[*n];
          iw2 = &iwork[(*n * 2)];

          /* tanh(x) = sinh(x) / cosh(x) */
          /* Note that special cases XMP = 0.0 and XMP = infinity */
          /* are automatic. */
          mpsinh(xmp, n, params, mp1, iw1, &ifl);
          if (mpisinfinity(mp1, n, params))
            {
              /* tanh(x) -> 1.0 as x -> infinity */
              itomp(&c_1, result, n, params, &ifl);
              result[sign - 1] = iwork[sign - 1];
            }
          else
            {
              mpcosh(xmp, n, params, result, iw1, &ifl);
              divide(mp1, result, iw1, n, params,
                     iw2, &ifl);
              assign(result, iw1, n, params);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPTANH", &nrec, rec);
  return;

} /* mptanh */

void mpasinh(int *xmp, int *logbase,
             int *n, int *params, int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2, *mp3, *mp4, *mp5, *iw;
  char rec[125];
  int ifl;

  /*
   *  Computes arcsinh(x), with the result in RESULT. We use the
   *  formula arcsinh(x) = ln(x + sqrt(x*x+1)) from Abramowitz and
   *  Stegun 4.6.20, or a variant of it.
   */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is positive zero even if input is negative zero */
          result[fptype - 1] = zercon();
          result[sign - 1] = poscon();
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* Result is infinity with correct sign */
          assign(result, xmp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];
          mp4 = &iwork[(*n * 3)];
          mp5 = &iwork[(*n * 4)];
          iw = &iwork[(*n * 5)];

          /* MP2 = eps */
          epsilon(n, params, mp2);
          /* MP1 = sqrt(eps) */
          mproot(mp2, mp1, n, params, iw, &ifl);
          /* MP3 = 1 / sqrt(eps) */
          itomp(&c_1, mp2, n, params, &ifl);
          divide(mp2, mp1, mp3, n, params, iw, &ifl);
          /* MP4 = abs(xmp) */
          assign(mp4, xmp, n, params);
          mpabs(mp4, n, params, &ifl);
          if (mplt(mp4, mp1, n, params))
            {
              /* abs(xmp) < sqrt(eps); return xmp */
              assign(result, xmp, n, params);
            }
          else if (mpgt(mp4, mp3, n, params))
            {
              /* abs(xmp) > 1/sqrt(eps); return ln(2) + ln(abs(x)), with sign of x*/
              /* mp2 = 2.0 */
              itomp(&c_2, mp2, n, params, &ifl);
              mplog(mp2, logbase, n, params, mp1, iw, &ifl);
              mplog(mp4, logbase, n, params, mp2, iw, &ifl);
              plus(mp1, mp2, result, n, params, &ifl);
              if (mpispos(xmp, n, params))
                result[sign - 1] = poscon();
              else
                result[sign - 1] = negcon();
            }
          else if (mplt(mp4, mp2, n, params))
            {
              /* abs(x) < 1.0. Use the variant formula
                   arcsinh(x) = ln(1 + abs(x) + x*x/(1 + sqrt(x*x+1)))
                              = log1p(abs(x) + x*x/(1 + sqrt(x*x+1)))
              */
              /* MP1 = 1.0 */
              itomp(&c_1, mp1, n, params, &ifl);
              /* MP2 = XMP*XMP */
              times(xmp, xmp, mp2, n, params, &ifl);
              /* MP3 = XMP*XMP + 1.0 */
              plus(mp2, mp1, mp3, n, params, &ifl);
              /* MP5 = sqrt(XMP*XMP + 1.0) */
              mproot(mp3, mp5, n, params, iw, &ifl);
              /* MP3 = 1 + sqrt(XMP*XMP + 1.0) */
              plus(mp1, mp5, mp3, n, params, &ifl);
              /* MP5 = XMP*XMP / (1 + sqrt(XMP*XMP + 1.0)) */
              divide(mp2, mp3, mp5, n, params, iw, &ifl);
              /* MP3 = abs(XMP) + XMP*XMP / (1 + sqrt(XMP*XMP + 1.0)) */
              plus(mp4, mp5, mp3, n, params, &ifl);
              /* RESULT = log1p(abs(XMP) + XMP*XMP / (1 + sqrt(XMP*XMP + 1.0))) */
              mplog1p(mp3, logbase, n, params, result, iw, &ifl);
              /* Set the correct sign on the result */
              result[sign - 1] = xmp[sign - 1];
            }
          else
            {
              /* Use the formula
                   arcsinh(x) = ln(x + sqrt(x*x+1))
              */
              /* MP1 = 1.0 */
              itomp(&c_1, mp1, n, params, &ifl);
              /* MP2 = XMP*XMP */
              times(xmp, xmp, mp2, n, params, &ifl);
              /* MP3 = XMP*XMP + 1.0 */
              plus(mp2, mp1, mp3, n, params, &ifl);
              /* MP2 = sqrt(XMP*XMP + 1.0) */
              mproot(mp3, mp2, n, params, iw, &ifl);
              /* MP3 = XMP + sqrt(XMP*XMP + 1.0) */
              plus(xmp, mp2, mp3, n, params, &ifl);
              /* RESULT = ln(XMP + sqrt(XMP*XMP + 1.0)) */
              mplog(mp3, logbase, n, params, result, iw, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPASINH", &nrec, rec);
  return;

} /* mpasinh */

void mpacosh(int *xmp, int *logbase,
             int *n, int *params, int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2, *mp3, *mp4, *iw;
  char rec[125];
  int ifl;

  /*
   *  Computes arccosh(x), with the result in RESULT. We use the
   *  formula arccosh(x) = ln(x + sqrt(x*x-1)), x >= 1,
   *  from Abramowitz and Stegun 4.6.21.
   */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisneg(xmp, n, params))
        {
          /* Result is NaN */
          result[fptype - 1] = qnancon();
          result[sign - 1] = poscon();
        }
      else if (mpisinfinity(xmp, n, params))
        {
          /* Result is infinity */
          assign(result, xmp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];
          mp4 = &iwork[(*n * 3)];
          iw = &iwork[(*n * 4)];

          /* MP1 = 1.0 */
          itomp(&c_1, mp1, n, params, &ifl);
          /* MP2 = 4.0 */
          itomp(&c_4, mp2, n, params, &ifl);
          if (mplt(xmp, mp1, n, params))
            {
              /* xmp < 1.0; return NaN */
              result[fptype - 1] = qnancon();
              result[sign - 1] = xmp[sign - 1];
            }
          else if (mple(xmp,mp2,n,params))
            {
              /* x <= 4.0; use the variant formula
               acosh(x) = ln(x + sqrt(2*(x-1) + (x-1)*(x-1)))
                        = log1p(x - 1 + sqrt(2*(x-1) + (x-1)*(x-1)))
              */
              /* mp2 = xmp - 1.0 */
              minus(xmp, mp1, mp2, n, params, &ifl);
              /* mp1 = 2.0 * (xmp - 1.0) */
              plus(mp2, mp2, mp1, n, params, &ifl);
              /* mp3 = (xmp - 1.0) * (xmp - 1.0) */
              times(mp2, mp2, mp3, n, params, &ifl);
              /* mp4 = 2.0 * (xmp - 1.0) + (xmp - 1.0) * (xmp - 1.0) */
              plus(mp1, mp3, mp4, n, params, &ifl);
              /* mp1 = sqrt(2.0 * (xmp - 1.0) + (xmp - 1.0) * (xmp - 1.0)) */
              mproot(mp4, mp1, n, params, iw, &ifl);
              /* mp3 = xmp - 1.0 + sqrt(2.0 * (xmp - 1.0) + (xmp - 1.0) * (xmp - 1.0)) */
              plus(mp2, mp1, mp3, n, params, &ifl);
              /* RESULT = log1p(mp3) */
              mplog1p(mp3, logbase, n, params, result, iw, &ifl);
            }
          else
            {
              /* arccosh(x) = ln(x + sqrt(x*x-1)) */
              /* MP2 = XMP*XMP */
              times(xmp, xmp, mp2, n, params, &ifl);
              /* MP3 = XMP*XMP - 1.0 */
              minus(mp2, mp1, mp3, n, params, &ifl);
              /* MP2 = sqrt(XMP*XMP - 1.0) */
              mproot(mp3, mp2, n, params, iw, &ifl);
              /* MP3 = XMP + sqrt(XMP*XMP - 1.0) */
              plus(xmp, mp2, mp3, n, params, &ifl);
              /* RESULT = ln(XMP + sqrt(XMP*XMP - 1.0)) */
              mplog(mp3, logbase, n, params, result, iw, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPACOSH", &nrec, rec);
  return;

} /* mpacosh */

void mpatanh(int *xmp, int *logbase,
             int *n, int *params, int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2, *mp3, *mp4, *mp5, *iw;
  char rec[125];
  int ifl;

  /*
   *  Computes arctanh(x), with the result in RESULT. We use the
   *  formula arctanh(x) = 0.5 * ln((1+x)/(1-x)), 0 <= x*x <= 1,
   *  from Abramowitz and Stegun 4.6.22.
   */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisnan(xmp, n, params) || mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpiszero(xmp, n, params))
        {
          /* Result is positive zero even if input is negative zero */
          result[fptype - 1] = zercon();
          result[sign - 1] = poscon();
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];
          mp4 = &iwork[(*n * 3)];
          mp5 = &iwork[(*n * 4)];
          iw = &iwork[(*n * 5)];

          /* mp1 = 1.0 */
          itomp(&c_1, mp1, n, params, &ifl);
          /* mp2 = 2.0 */
          itomp(&c_2, mp2, n, params, &ifl);
          /* mp4 = 0.5 */
          divide(mp1, mp2, mp4, n, params, iw, &ifl);
          /* mp5 = eps */
          epsilon(n, params, mp5);
          /* mp2 = sqrt(eps) */
          mproot(mp5, mp2, n, params, iw, &ifl);
          /* mp3 = abs(xmp) */
          assign(mp3, xmp, n, params);
          mpabs(mp3, n, params, &ifl);
          if (mpge(mp3, mp1, n, params))
            {
              if (mpgt(mp3, mp1, n, params))
                {
                  /* abs(xmp) > 1.0; return NaN */
                  result[fptype - 1] = qnancon();
                  result[sign - 1] = xmp[sign - 1];
                }
              else
                {
                  /* abs(xmp) = 1.0; return infinity with correct sign */
                  result[fptype - 1] = infcon();
                  result[sign - 1] = xmp[sign - 1];
                }
            }
          else if (mplt(mp3, mp2, n, params))
            {
              /* abs(xmp) < sqrt(eps); return xmp */
              assign(result, xmp, n, params);
            }
          else if (mplt(mp3, mp4, n, params))
            {
              /* abs(xmp) < 0.5; use the variant formula
                 arctanh(x) = 0.5 * ln(1 + 2x/(1-x))
                            = 0.5 * log1p(2x/(1-x))
                 with the correct sign.
              */
              /* mp5 = 1.0 - abs(XMP) */
              minus(mp1, mp3, mp5, n, params, &ifl);
              /* mp1 = 2 * abs(XMP) */
              plus(mp3, mp3, mp1, n, params, &ifl);
              /* mp3 = (2 * abs(XMP)) / (1.0 - abs(XMP)) */
              divide(mp1, mp5, mp3, n, params, iw, &ifl);
              /* mp2 = log1p((2 * abs(XMP)) / (1.0 - abs(XMP))) */
              mplog1p(mp3, logbase, n, params, mp2, iw, &ifl);
              /* RESULT = 0.5 * log1p((2 * abs(XMP)) / (1.0 - abs(XMP))) */
              times(mp4, mp2, result, n, params, &ifl);
              /* Set the correct sign on the result */
              result[sign - 1] = xmp[sign - 1];
            }
          else
            {
              /* Use arctanh(x) = 0.5 * ln((1+x)/(1-x)) */
              /* MP2 = 1.0 + XMP */
              plus(mp1, xmp, mp2, n, params, &ifl);
              /* MP3 = 1.0 - XMP */
              minus(mp1, xmp, mp3, n, params, &ifl);
              /* MP1 = (1.0 + XMP) / (1.0 - XMP) */
              divide(mp2, mp3, mp1, n, params, iw, &ifl);
              /* MP3 = ln((1.0 + XMP) / (1.0 - XMP)) */
              mplog(mp1, logbase, n, params, mp3, iw, &ifl);
              /* MP2 = 2.0 */
              itomp(&c_2, mp2, n, params, &ifl);
              /* RESULT = ln((1.0 + XMP) / (1.0 - XMP)) / 2.0 */
              divide(mp3, mp2, result, n, params, iw, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPATANH", &nrec, rec);
  return;

} /* mpatanh */

void mplog_kernel(int *xmp, int *n, int *params,
                  int *result, int *iwork)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int ifac;
  int params_short[15], short_mantis, i;
  int mpifac[40], *iw;
  int *mp1, *mp2, *mp3;
  int ifl, n_short;

  /* Computes log(x), for 0.5 <= x <= 1.5, with the result in RESULT. */

  /* Uses ln(1+x) = x - x**2/2 + x**3 / 3 - ... */

  ifl = 0;

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  i_1 = get_mantis(params);
  short_mantis = myimin(i_1, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifl);

  /* Dissect workspace */
  mp1 = &iwork[0];
  mp2 = &iwork[*n];
  mp3 = &iwork[(*n * 2)];
  iw = &iwork[*n * 3];

  /* Compute iwork(MP1) = u = x - 1.0 */
  itomp(&c_1, mp2, n, params, &ifl);
  minus(xmp, mp2, mp1, n, params, &ifl);

  /* IWORK(MP2) = u */
  assign(mp2, mp1, n, params);

  /* RESULT = u */
  assign(result, mp1, n, params);

  i = 2;
 L20:

  ifac = i;
  itomp(&ifac, mpifac, &n_short, params_short, &ifl);

  /* IWORK(MP2) = upow = upow * u */
  times(mp1, mp2, mp3, n, params, &ifl);
  assign(mp2, mp3, n, params);

  /* IWORK(MP3) = term = upow / ifac */
  divide2(mp2, n, params, mpifac, &n_short, params_short,
          mp3, n, params, iw, &ifl);

  /* RESULT = RESULT +/- term */
  if (i % 2 == 0)
    minus(result, mp3, iw, n, params, &ifl);
  else
    plus(result, mp3, iw, n, params, &ifl);

  /* Repeat until the term is negligible */
  if (mpne(result, iw, n, params))
    {
      ++i;
      assign(result, iw, n, params);
      goto L20;
    }

  return;
} /* mplog_kernel */

void mplog_kernel2(int *xmp, int *n, int *params, int *result, int *iwork)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int ifac;
  int params_short[15], short_mantis, i;
  int mpifac[40], *iw;
  int *mp1, *mp2, *mp3;
  int ifl, n_short;

  /* Computes log(1+x), for 0.0 <= x <= 1.0, with the result in RESULT. */

  /* Uses ln(1+x) = u + u**2/2 + u**3 / 3 + ..., where u = x / (1 + x). */

  ifl = 0;

  /* Set up some parameters for numbers which need only low */
  /* (30 bit) precision. Be careful - don't let short_mantis */
  /* be bigger than the regular one, or your workspace may be */
  /* too small for calls of divide2. */
  i_1 = get_mantis(params);
  short_mantis = myimin(i_1, c_30);
  i_1 = get_emin(params);
  i_2 = get_emax(params);
  mpinitcopy(params, &short_mantis, &i_1, &i_2, &n_short,
             params_short, &ifl);

  /* Dissect workspace */
  mp1 = &iwork[0];
  mp2 = &iwork[*n];
  mp3 = &iwork[(*n * 2)];
  iw = &iwork[*n * 3];

  /* Compute iwork(MP1) = u = x / (1 + x) */
  assign(mp2, xmp, n, params);
  itomp(&c_1, mp1, n, params, &ifl);
  plus(mp1, mp2, mp3, n, params, &ifl);
  divide(mp2, mp3, mp1, n, params, iw,
         &ifl);

  /* IWORK(MP2) = u */
  assign(mp2, mp1, n, params);

  /* RESULT = u */
  assign(result, mp1, n, params);

  i = 2;
 L20:

  ifac = i;
  itomp(&ifac, mpifac, &n_short, params_short, &ifl);

  /* IWORK(MP2) = upow = upow * u */
  times(mp1, mp2, mp3, n, params, &ifl);
  assign(mp2, mp3, n, params);

  /* IWORK(MP3) = term = upow / ifac */
  divide2(mp2, n, params, mpifac, &n_short, params_short,
          mp3, n, params, iw, &ifl);

  /* RESULT = RESULT + term */
  plus(result, mp3, iw, n, params, &ifl);

  /* Repeat until the term is negligible */
  if (mpne(result, iw, n, params))
    {
      ++i;
      assign(result, iw, n, params);
      goto L20;
    }

  return;
} /* mplog_kernel2 */

void mplog(int *xmp, int *logbase, int *n,
           int *params, int *result, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int nrec;
  int sign, ierr, iexo, nreq;
  int *iw;
  int fptype;
  int *mp1, *mp2, *mp3, *mp4;
  char rec[125];
  int ifl;

  /* Computes log(x) (natural logarithm), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpiszero(xmp, n, params))
        {
          result[fptype - 1] = infcon();
          result[sign - 1] = negcon();
        }
      else if (mpisnan(xmp, n, params) || mpisneg(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = infcon();
          result[sign - 1] = poscon();
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];
          mp4 = &iwork[*n * 3];
          iw = &iwork[(*n * 4)];

          itomp(&c_1, mp1, n, params, &ifl);
          itomp(&c_2, mp2, n, params, &ifl);
          divide(mp1, mp2, mp3, n, params,
                 iw, &ifl);
          itomp(&c_3, mp1, n, params, &ifl);
          divide(mp1, mp2, mp4, n, params,
                 iw, &ifl);

          if (mpge(xmp, mp3, n, params) && mple(xmp, mp4, n, params))
            {
              /* For x near 1.0, i.e. 0.5 <= XMP <= 1.5, use MPLOG_KERNEL */
              /* to avoid cancellation error when adding log(base). */
              mplog_kernel(xmp, n, params, result, iwork);
            }
          else
            {
              /* IWORK(MP1) = frac(XMP) * BASE, where frac(XMP) is the */
              /* mantissa part of XMP, which is in range [1,BASE). */
              assign(mp1, xmp, n, params);
              mp1[iexo - 1] = 1;

              /* IWORK(MP4) = IWORK(MP1) - 1 */
              ifl = 0;
              itomp(&c_1, mp3, n, params, &ifl);
              minus(mp1, mp3, mp4, n, params,
                    &ifl);

              /* IWORK(MP3) = log(1+IWORK(MP4)) = log(IWORK(MP1)) */
              mplog_kernel2(mp4, n, params, mp3,
                            iw);

              /* IWORK(MP2) = log(BASE) * (XMP(IEXO)-1) */
              i_1 = xmp[iexo - 1] - 1;
              itomp(&i_1, mp1, n, params, &ifl);
              times(mp1, logbase, mp2, n, params,
                    &ifl);

              /* RESULT = sum of the two logs. */
              plus(mp3, mp2, result, n, params,
                   &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPLOG", &nrec, rec);
  return;

} /* mplog */

void mplog1p(int *xmp, int *logbase, int *n,
             int *params, int *result, int *iwork, int *ifail)
{
  /* Local variables */
  int nrec;
  int sign, ierr, iexo, nreq;
  int *iw;
  int fptype;
  int *mp1, *mp2, *mp3, *mp4;
  char rec[125];
  int ifl;

  /* Computes log(1+x) (natural logarithm), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      iexo = get_iexo(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[(*n * 2)];
          mp4 = &iwork[(*n * 3)];
          iw = &iwork[(*n * 4)];

          /* mp1 = 1 */
          itomp(&c_1, mp1, n, params, &ifl);
          /* mp2 = -1 */
          itomp(&c_n1, mp2, n, params, &ifl);

          if (mpisnan(xmp, n, params))
            {
              /* X is NaN; return quiet NaN */
              result[fptype - 1] = qnancon();
              result[sign - 1] = xmp[sign - 1];
            }
          else if (mpisinfinity(xmp, n, params) && mpispos(xmp, n, params))
            {
              /* X is positive infinity; return X */
              assign(result, xmp, n, params);
            }
          else if (mplt(xmp, mp2, n, params))
            {
              /* X < -1; return NaN */
              result[fptype - 1] = qnancon();
              result[sign - 1] = xmp[sign - 1];
            }
          else if (mpeq(xmp, mp2, n, params))
            {
              /* X = -1; return -infinity */
              result[fptype - 1] = infcon();
              result[sign - 1] = negcon();
            }
          else if (mpiszero(xmp, n, params))
            {
              /* X is zero; return X, preserving sign */
              assign(result, xmp, n, params);
            }
          else
            {
              /* Use this formula from HP-15C Advanced Functions
                 Handbook, p.193 for accuracy to within a few ulps:
                   u = 1+x;
                   if(u==1.0)
                     log1p(x) = x;
                   else
                     log1p(x) = log(u)*(x/(u-1.0));
              */
              /* mp2 = u = 1 + x */
              plus(mp1, xmp, mp2, n, params, &ifl);
              if (mpeq(mp2, mp1, n, params))
                /* u == 1.0 (i.e. x was so small that adding it to
                   1.0 made no difference); return x */
                assign(result, xmp, n, params);
              else
                {
                  /* mp3 = log(u) */
                  mplog(mp2, logbase, n, params, mp3, iw, &ifl);
                  /* mp4 = u - 1.0 */
                  minus(mp2, mp1, mp4, n, params, &ifl);
                  /* mp2 = x / (u - 1) */
                  divide(xmp, mp4, mp2, n, params, iw, &ifl);
                  /* result = log(u)*(x/(u-1.0)) */
                  times(mp3, mp2, result, n, params, &ifl);
                }
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPLOG1P", &nrec, rec);
  return;

} /* mplog1p */

void mplog10(int *xmp, int *logbase, int *log10e,
             int *n, int *params, int *result, int *iwork,
             int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1;
  char rec[125];
  int ifl;

  /* Computes log10(x) (logarithm to base 10), with the result in RESULT. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(logbase, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LOGBASE is corrupt or uninitialised");
        }
      else if (mpisfake(log10e, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LOG10E is corrupt or uninitialised");
        }
      else if (mpiszero(xmp, n, params))
        {
          result[fptype - 1] = infcon();
          result[sign - 1] = negcon();
        }
      else if (mpisnan(xmp, n, params) || mpisneg(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = infcon();
          result[sign - 1] = poscon();
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];

          /* Get natural log of XMP in RESULT. */
          mplog(xmp, logbase, n, params, result, iwork,
                &ifl);

          /* Multiply by log10(e) to get log10(XMP). */
          times(result, log10e, mp1, n, params, &ifl);
          assign(result, mp1, n, params);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPLOG10", &nrec, rec);
  return;

} /* mplog10 */

void mplog2(int *xmp, int *logbase, int *n,
            int *params, int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int fptype;
  int *mp1, *mp2;
  char rec[125];
  int ifl;

  /* Computes log2(x) (logarithm to base 2), with the result in RESULT. */
  /* LOGBASE is assumed to contain log(2) on input. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(logbase, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LOGBASE is corrupt or uninitialised");
        }
      else if (mpiszero(xmp, n, params))
        {
          result[fptype - 1] = infcon();
          result[sign - 1] = negcon();
        }
      else if (mpisnan(xmp, n, params) || mpisneg(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        {
          result[fptype - 1] = infcon();
          result[sign - 1] = poscon();
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];

          /* Get natural log of XMP in RESULT. */
          mplog(xmp, logbase, n, params, result, iwork,
                &ifl);

          /* Divide by log(2) to get log2(XMP). */
          divide(result, logbase, mp1, n, params,
                 mp2, &ifl);
          assign(result, mp1, n, params);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPLOG2", &nrec, rec);
  return;

} /* mplog2 */

void epsilon(int *n, int *params, int *eps)
{
  int base;
  int sign, iexo;
  int radix;
  int mantis;
  int fptype;
  int wplaces;

  /* Returns eps, the precision of numbers in system PARAMS. */

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return;

  base = get_base(params);
  mantis = get_mantis(params);
  sign = get_sign(params);
  iexo = get_iexo(params);
  fptype = get_fptype(params);
  radix = get_radix(params);
  wplaces = get_wplaces(params);
  eps[fptype - 1] = regcon();
  eps[sign - 1] = poscon();
  eps[0] = radix / base;
  eps[wplaces - 1] = 1;
  eps[iexo - 1] = -mantis;
  return;
} /* epsilon */

void mplarge(int *n, int *params, int *large)
{
  int sign, iexo;
  int radix;
  int places, last;
  int emax;
  int fptype;
  int wplaces;
  int i;

  /* Returns large, the largest number in system PARAMS. */

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return;

  sign = get_sign(params);
  emax = get_emax(params);
  iexo = get_iexo(params);
  fptype = get_fptype(params);
  radix = get_radix(params);
  places = get_places(params);
  last = get_last(params);
  wplaces = get_wplaces(params);
  for (i = 0; i < places - 1; i++)
    large[i] = radix - 1;
  large[places - 1] = (radix / last) * last;
  large[fptype - 1] = regcon();
  large[sign - 1] = poscon();
  large[wplaces - 1] = places;
  large[iexo - 1] = emax;
  return;
} /* mplarge */

void mpsmall(int *n, int *params, int *small)
{
  int base;
  int sign, iexo;
  int radix;
  int emin;
  int fptype;
  int wplaces;

  /* Returns small, the precision of numbers in system PARAMS. */

  if (*n <= 0) /* Avoid compiler warning about unused n */
    return;

  base = get_base(params);
  emin = get_emin(params);
  sign = get_sign(params);
  iexo = get_iexo(params);
  fptype = get_fptype(params);
  radix = get_radix(params);
  wplaces = get_wplaces(params);
  small[fptype - 1] = regcon();
  small[sign - 1] = poscon();
  small[0] = radix / base;
  small[wplaces - 1] = 1;
  small[iexo - 1] = emin;
  return;
} /* mpsmall */

void get_pi(int *n, int *params, int *pi, int *iwork)
{
  int ifail;
  int *iw;
  int *mp1, *mp2, *mp3, *mp4, *mp5;

  /* Returns Pi, to the precision of numbers in system PARAMS. */

  ifail = 0;

  /* Dissect workspace */
  mp1 = &iwork[0];
  mp2 = &iwork[*n];
  mp3 = &iwork[(*n * 2)];
  mp4 = &iwork[*n * 3];
  mp5 = &iwork[(*n * 4)];
  iw = &iwork[*n * 5];

  /* iwork(MP1) = 1.0 */
  itomp(&c_1, mp1, n, params, &ifail);

  /* iwork(MP2) = 5.0 */
  itomp(&c_5, mp2, n, params, &ifail);

  /* iwork(MP3) = 239.0 */
  itomp(&c_239, mp3, n, params, &ifail);

  /* iwork(MP4) = 1/5 */
  divide(mp1, mp2, mp4, n, params, iw, &ifail);

  /* iwork(MP5) = 1/239 */
  divide(mp1, mp3, mp5, n, params, iw, &ifail);
  /* CALL MPSHOW('1/239 = ',iwork(MP5),N,PARAMS,0) */

  /* iwork(MP1) = atan(1/5) */
  mpatan_kernel(mp4, n, params, mp1, iw, &ifail);
  /* CALL MPSHOW('atan(1/5) = ',iwork(MP1),N,PARAMS,0) */

  /* iwork(MP2) = atan(1/239) */
  mpatan_kernel(mp5, n, params, mp2, iw, &ifail);
  /* CALL MPSHOW('atan(1/239) = ',iwork(MP2),N,PARAMS,0) */

  /* iwork(MP3) = 16.0 */
  itomp(&c_16, mp3, n, params, &ifail);

  /* iwork(MP4) = 16 * atan(1/5) */
  times(mp3, mp1, mp4, n, params, &ifail);

  /* iwork(MP3) = 4.0 */
  itomp(&c_4, mp3, n, params, &ifail);

  /* iwork(MP5) = 4 * atan(1/239) */
  times(mp3, mp2, mp5, n, params, &ifail);

  /* Pi = 16 * atan(1/5) - 4 * atan(1/239) */
  minus(mp4, mp5, pi, n, params, &ifail);

  return;
} /* get_pi */

void get_pi_etc(int *n, int *params, int *pi,
                int *two_pi, int *pi_over_two, int *two_over_pi,
                int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int nrec, ierr;
  int params_short[15];
  int mptwo[10];
  char rec[125];
  int ifl, n_short;

  /* Returns Pi, Pi/2, 2/Pi, to the precision of numbers in system PARAMS. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      ifl = 0;
      get_pi(n, params, pi, iwork);

      /* Set MPTWO = 2.0 */
      i_1 = get_emin(params);
      i_2 = get_emax(params);
      mpinitcopy(params, &c_3, &i_1, &i_2, &n_short,
                 params_short, &ifl);
      itomp(&c_2, mptwo, &n_short, params_short, &ifl);

      /* TWO_PI = MPTWO * PI */
      times2(mptwo, &n_short, params_short, pi, n, params,
             two_pi, n, params, &ifl);

      /* PI_OVER_TWO = PI / MPTWO */
      divide2(pi, n, params, mptwo, &n_short, params_short,
              pi_over_two, n, params, iwork, &ifl);

      /* TWO_OVER_PI = MPTWO / PI */
      divide2(mptwo, &n_short, params_short, pi, n, params,
              two_over_pi, n, params, iwork, &ifl);
    }

  *ifail = p01abf(ifail, &ierr, "GET_PI_ETC", &nrec, rec);
  return;

} /* get_pi_etc */

void get_precomputed_pi_etc(int *n, int *params,
                            int *pi, int *two_pi,
                            int *pi_over_two, int *two_over_pi,
                            int *ifail)
{
  /* Initialized data */

  static int precomputed_params[15] = { 2,2260,-4520,4520,227,228,229,
                                         230,1024,10,226,1,1,0,1438 };
  static int precomputed_pi[230] = { 804,253,680,545,419,35,305,198,
                                      394,184,55,28,836,656,147,520,553,636,797,2,46,1002,398,787,620,
                                      549,82,520,486,227,257,221,958,337,620,973,233,49,716,43,41,735,
                                      151,788,221,254,77,365,437,284,145,484,534,855,408,606,507,111,
                                      275,66,934,611,507,363,47,1013,813,756,26,894,891,568,431,949,674,
                                      415,662,745,969,17,497,177,1017,585,161,613,123,228,364,988,128,
                                      124,738,534,239,773,611,420,525,540,343,313,666,278,254,655,841,
                                      207,382,54,343,291,882,570,869,540,395,821,392,133,330,953,949,
                                      297,29,777,421,621,412,195,339,586,754,384,316,372,432,140,646,33,
                                      496,809,23,582,184,876,910,995,633,882,774,14,536,57,713,899,651,
                                      704,488,655,727,85,892,111,305,300,631,555,815,873,342,23,96,921,
                                      338,380,938,342,697,277,840,609,550,250,20,257,348,654,362,170,
                                      689,45,692,817,451,260,321,931,234,85,134,701,967,186,403,719,737,
                                      260,355,446,962,650,937,789,471,262,49,987,229,783,534,622,121,
                                      199,687,859,11111,2,226,90001 };
  static int precomputed_two_pi[230] = { 804,253,680,545,419,35,305,
                                          198,394,184,55,28,836,656,147,520,553,636,797,2,46,1002,398,787,
                                          620,549,82,520,486,227,257,221,958,337,620,973,233,49,716,43,41,
                                          735,151,788,221,254,77,365,437,284,145,484,534,855,408,606,507,
                                          111,275,66,934,611,507,363,47,1013,813,756,26,894,891,568,431,949,
                                          674,415,662,745,969,17,497,177,1017,585,161,613,123,228,364,988,
                                          128,124,738,534,239,773,611,420,525,540,343,313,666,278,254,655,
                                          841,207,382,54,343,291,882,570,869,540,395,821,392,133,330,953,
                                          949,297,29,777,421,621,412,195,339,586,754,384,316,372,432,140,
                                          646,33,496,809,23,582,184,876,910,995,633,882,774,14,536,57,713,
                                          899,651,704,488,655,727,85,892,111,305,300,631,555,815,873,342,23,
                                          96,921,338,380,938,342,697,277,840,609,550,250,20,257,348,654,362,
                                          170,689,45,692,817,451,260,321,931,234,85,134,701,967,186,403,719,
                                          737,260,355,446,962,650,937,789,471,262,49,987,229,783,534,622,
                                          121,199,687,859,11111,3,226,90001 };
  static int precomputed_two_over_pi[230] = { 651,920,219,590,272,338,
                                               639,39,351,287,333,221,771,438,165,409,241,57,16,510,325,570,759,
                                               699,789,539,457,110,233,36,884,736,25,146,954,521,838,289,831,541,
                                               940,459,74,423,251,648,141,501,186,948,289,233,625,770,429,95,505,
                                               19,612,470,230,53,206,500,626,69,994,957,996,643,711,1016,607,
                                               1021,897,408,63,754,964,395,360,166,839,877,217,1004,969,971,38,
                                               884,977,575,410,485,1018,557,468,635,689,1003,919,791,719,263,231,
                                               888,660,658,937,703,727,945,126,213,834,86,12,772,447,123,430,703,
                                               51,956,130,431,269,541,679,569,88,350,920,432,537,409,533,497,296,
                                               104,258,223,1014,128,309,818,460,262,24,341,434,627,675,150,56,
                                               635,770,198,721,964,103,54,499,476,928,146,672,857,785,872,741,
                                               540,667,474,977,209,348,400,335,677,1020,112,335,894,207,643,176,
                                               734,318,387,159,443,780,978,443,875,121,495,551,826,124,860,700,
                                               639,118,127,72,400,497,962,282,762,443,343,459,304,268,945,369,
                                               532,726,465,624,962,786,724,11111,0,226,90001 };
  static int precomputed_pi_over_two[230] = { 804,253,680,545,419,35,
                                               305,198,394,184,55,28,836,656,147,520,553,636,797,2,46,1002,398,
                                               787,620,549,82,520,486,227,257,221,958,337,620,973,233,49,716,43,
                                               41,735,151,788,221,254,77,365,437,284,145,484,534,855,408,606,507,
                                               111,275,66,934,611,507,363,47,1013,813,756,26,894,891,568,431,949,
                                               674,415,662,745,969,17,497,177,1017,585,161,613,123,228,364,988,
                                               128,124,738,534,239,773,611,420,525,540,343,313,666,278,254,655,
                                               841,207,382,54,343,291,882,570,869,540,395,821,392,133,330,953,
                                               949,297,29,777,421,621,412,195,339,586,754,384,316,372,432,140,
                                               646,33,496,809,23,582,184,876,910,995,633,882,774,14,536,57,713,
                                               899,651,704,488,655,727,85,892,111,305,300,631,555,815,873,342,23,
                                               96,921,338,380,938,342,697,277,840,609,550,250,20,257,348,654,362,
                                               170,689,45,692,817,451,260,321,931,234,85,134,701,967,186,403,719,
                                               737,260,355,446,962,650,937,789,471,262,49,987,229,783,534,622,
                                               121,199,687,859,11111,1,226,90001 };

  int radix_requested, nrec;
  int ierr, nreq;
  int base_precomputed;
  int mantis_requested;
  int radix_precomputed;
  int base_requested;
  char rec[400];
  int ifl, mantis_precomputed;
  int *iwork;

  /* Returns precomputed values of Pi, Pi/2, 2/Pi, to the precision of */
  /* numbers in system PARAMS. If PARAMS does not match the parameters */
  /* used to generate Pi in the first place, we return an error flag. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else if (! chksum(precomputed_params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PRECOMPUTED_PARAMS was not initialised by call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else
        {
          base_requested = get_base(params);
          base_precomputed = get_base(precomputed_params);
          radix_requested = get_radix(params);
          radix_precomputed = get_radix(precomputed_params);
          mantis_requested = get_mantis(params);
          mantis_precomputed = get_mantis(precomputed_params);

          /* Check whether we are being asked for something we */
          /* haven't got precomputed. */
          if (base_requested != base_precomputed || radix_requested !=
              radix_precomputed || mantis_requested > mantis_precomputed)
            {
              /* Sigh - we'll just have to go and compute them */
              printf("get_precomputed_pi: cannot use precomputed Pi values; computing them now\n");
              iwork = (int *)malloc(18 * *n * sizeof(int));
              get_pi_etc(n, params, pi, two_pi, pi_over_two, two_over_pi,
                         iwork, ifail);
              free(iwork);
            }
          else if (base_requested != base_precomputed || radix_requested !=
              radix_precomputed)
            {
              ierr = 2;
              nrec = 1;
              sprintf(rec, "%s %s %d %s %d %s %d %s %d",
                      "Precomputed value of Pi is not compatible with request:",
                      "BASE_REQUESTED = ",
                      base_requested, "BASE_PRECOMPUTED = ",
                      base_precomputed, "RADIX_REQUESTED = ",
                      radix_requested, "RADIX_PRECOMPUTED = ",
                      radix_precomputed);
            }
          else if (mantis_requested > mantis_precomputed)
            {
              ierr = 3;
              nrec = 2;
              sprintf(rec, "%s %d %s %d %s", "You want ",
                      mantis_requested, " base BASE digits of Pi, but only ",
                      mantis_precomputed, " are available.");
            }
          else
            {
              ifl = 0;
              mpcopy2(precomputed_pi, &c_230, precomputed_params, pi, n, params, &ifl);
              mpcopy2(precomputed_two_pi, &c_230, precomputed_params,
                      two_pi, n, params, &ifl);
              mpcopy2(precomputed_two_over_pi, &c_230,
                      precomputed_params, two_over_pi, n, params, &ifl);
              mpcopy2(precomputed_pi_over_two, &c_230,
                      precomputed_params, pi_over_two, n, params, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "GET_PRECOMPUTED_PI_ETC", &nrec, rec);
  return;

} /* get_precomputed_pi_etc */

void get_log_etc(int *n, int *params, int *logbase, int *log10e, int *iwork, int *ifail)
{
  /* System generated locals */
  int i_1, i_2;

  /* Local variables */
  int base, nrec;
  int ierr;
  int params_short[15];
  int mpone[40];
  int *mp1, *mp2;
  int *iw1, *iw2;
  char rec[125];
  int ifl, n_short;

  /* Returns natural log(BASE) in LOGBASE, and log10(e) in LOG10E, */
  /* to the precision of numbers in system PARAMS. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[*n];
      iw1 = &iwork[(*n * 2)];
      iw2 = &iwork[*n * 3];

      base = get_base(params);
      ifl = 0;

      /* For speed, compute log(10) as log(2) + log(5). */
      /* IWORK(MP2) = log(2) */
      itomp(&c_1, mp1, n, params, &ifl);
      mplog_kernel(mp1, n, params, mp2, iw1);

      /* IWORK(IW1) = log(5) */
      itomp(&c_4, mp1, n, params, &ifl);
      mplog_kernel(mp1, n, params, iw1, iw2);

      /* IWORK(MP1) = log(10) = log(2) + log(5) */
      plus(mp2, iw1, mp1, n, params, &ifl);

      /* Set MPONE = 1.0 */
      i_1 = get_emin(params);
      i_2 = get_emax(params);
      mpinitcopy(params, &c_3, &i_1, &i_2, &n_short,
                 params_short, ifail);
      itomp(&c_1, mpone, &n_short, params_short, ifail);

      /* LOG10E = 1 / log(10) = log10(e) */
      divide2(mpone, &n_short, params_short, mp1, n, params,
              log10e, n, params, iw1, &ifl);

      /* logbase = log(BASE) */
      if (base == 2)
        {
          /* log(2) = mplog_kernel(MP1) */
          i_1 = base - 1;
          itomp(&i_1, mp1, n, params, &ifl);
          mplog_kernel(mp1, n, params, logbase, mp2);
        }
      else if (base == 10)
        assign(logbase, mp1, n, params);
      else
        {
          /* For speed, compute log(base) as 2 * log(root(base)) */
          itomp(&base, mp1, n, params, &ifl);
          mproot(mp1, mp2, n, params, iw1,
                 &ifl);
          itomp(&c_1, iw1, n, params, &ifl);
          minus(mp1, iw1, mp2, n, params, &ifl);
          mplog_kernel(mp2, n, params, mp1, iw1);
          plus(mp1, mp1, logbase, n, params, &ifl);
        }
    }

  *ifail = p01abf(ifail, &ierr, "GET_LOG_ETC", &nrec, rec);
  return;

} /* get_log_etc */

void get_precomputed_log_etc(int *n, int *params,
                             int *logbase, int *log10e, int *ifail)
{
  /* Initialized data */

  static int precomputed_params[15] = { 2,2260,-4520,4520,227,228,229,
                                         230,1024,10,226,1,1,0,1438 };
  static int precomputed_logbase[230] = { 709,801,509,977,829,922,754,
                                           483,718,384,252,758,701,15,208,806,458,395,395,394,52,373,738,938,
                                           1000,702,494,118,129,734,747,152,342,341,190,842,1000,433,59,302,
                                           696,860,78,33,272,629,462,809,69,667,521,318,603,161,392,586,931,
                                           81,690,986,68,791,782,126,743,681,752,945,217,515,713,367,643,711,
                                           405,1015,301,812,929,945,629,596,562,757,894,619,846,48,201,134,
                                           343,929,540,754,56,930,872,729,497,271,255,348,385,1012,808,287,
                                           726,1019,577,528,844,248,575,597,104,750,346,470,895,30,1000,343,
                                           843,547,888,320,44,918,93,96,477,475,550,270,370,323,924,811,286,
                                           461,207,204,787,613,590,309,83,76,104,480,756,470,37,466,345,667,
                                           204,854,296,823,426,455,994,606,82,232,129,845,728,21,831,634,652,
                                           196,514,596,92,848,843,149,373,279,620,481,494,474,908,316,877,
                                           710,27,177,30,247,215,283,182,817,727,848,724,389,25,193,557,209,
                                           395,435,717,600,335,373,610,537,326,738,463,741,348,182,794,249,
                                           422,52,603,347,11111,0,226,90001 };
  static int precomputed_log10e[230] = { 889,445,554,311,161,793,333,
                                          347,682,762,844,988,200,1006,209,514,279,154,128,570,252,724,317,
                                          910,660,967,336,590,1000,320,188,1010,584,565,356,710,281,161,326,
                                          206,239,415,846,220,753,762,174,210,803,218,492,835,346,977,594,
                                          208,447,916,1,800,22,193,881,218,406,182,122,578,801,214,660,179,
                                          439,722,973,825,853,323,8,298,922,192,309,100,571,200,627,81,996,
                                          845,485,214,316,636,602,235,719,996,48,748,876,502,820,289,695,
                                          101,49,606,593,989,432,135,121,1014,178,43,78,84,411,973,790,397,
                                          452,605,964,532,917,606,174,592,758,237,752,168,622,914,298,443,
                                          222,652,11,114,469,278,1008,935,839,23,817,206,129,201,984,134,
                                          474,322,565,909,346,310,152,331,67,459,788,781,201,615,495,850,
                                          760,359,216,506,955,287,205,726,80,392,293,315,679,791,889,355,
                                          1012,433,795,230,189,661,881,319,726,442,161,868,107,723,773,175,
                                          287,955,261,478,386,100,67,531,559,581,35,243,508,230,356,141,310,
                                          102,112,71,812,733,170,790,11111,-1,226,90001 };

  int radix_requested, nrec;
  int ierr, nreq;
  int base_precomputed;
  int mantis_requested;
  int radix_precomputed;
  int base_requested;
  char rec[400];
  int ifl, mantis_precomputed;

  /* Returns precomputed values of log(BASE) in LOGBASE, */
  /* and log10(e) in LOG10E, to the precision of numbers in system */
  /* PARAMS. If PARAMS does not match the parameters used to generate */
  /* the numbers in the first place, we return an error flag. */

  ierr = 0;
  nrec = 0;
  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else
        {
          base_requested = get_base(params);
          base_precomputed = get_base(precomputed_params);
          radix_requested = get_radix(params);
          radix_precomputed = get_radix(precomputed_params);
          mantis_requested = get_mantis(params);
          mantis_precomputed = get_mantis(precomputed_params);

          /* Check whether we are being asked for something we */
          /* haven't got precomputed. */
          if (base_requested != base_precomputed || radix_requested !=
              radix_precomputed)
            {
              ierr = 2;
              nrec = 1;
              sprintf(rec, "%s %s %d %s %d %s %d %s %d",
                      "Precomputed value of LOGBASE is not compatible with request:",
                      "BASE_REQUESTED = ",
                      base_requested, "BASE_PRECOMPUTED = ",
                      base_precomputed, "RADIX_REQUESTED = ",
                      radix_requested, "RADIX_PRECOMPUTED = ",
                      radix_precomputed);
            }
          else if (mantis_requested > mantis_precomputed)
            {
              ierr = 3;
              nrec = 2;
              sprintf(rec, "%s %d %s %d %s", "You want ", mantis_requested,
                      " base BASE digits of LOGBASE, but only ",
                      mantis_precomputed, " are available.");
            }
          else
            {
              ifl = 0;
              mpcopy2(precomputed_logbase, &c_230, precomputed_params,
                      logbase, n, params, &ifl);
              mpcopy2(precomputed_log10e, &c_230, precomputed_params,
                      log10e, n, params, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "GET_PRECOMPUTED_LOG_ETC", &nrec, rec);
  return;

} /* get_precomputed_log_etc */

void get_e(int *n, int *params, int *e, int
           *iwork)
{
  int *iw;
  int *mp1, ifl;

  /* Returns e, to the precision of numbers in system PARAMS. */

  /* Dissect workspace */
  mp1 = &iwork[0];
  iw = &iwork[*n];

  ifl = 0;
  itomp(&c_1, mp1, n, params, &ifl);
  exp_kernel(mp1, n, params, e, iw);
  return;
} /* get_e */

void exp_kernel(int *xmp, int *n, int *params,
                int *result, int *iwork)
{
  int i;
  int *iw;
  int *mp1, *mp2, *mp3, *mp4, *mp5;
  char rec[125];
  int ifl;

  /* Returns exp(xmp), 0 <= xmp <= 1, to the precision of numbers in system PARAMS */

  /* result = 1 */
  itomp(&c_1, result, n, params, &ifl);

  if (mpisnan(xmp, n, params))
    {
      sprintf(rec, "exp_kernel: argument XMP is NaN");
      ifl = p01abf(&c_0, &c_1, "exp_kernel", &c_1, rec);
    }
  else if (mpisinfinity(xmp, n, params))
    {
      sprintf(rec, "exp_kernel: argument XMP is +/-Infinity");
      ifl = p01abf(&c_0, &c_1, "exp_kernel", &c_1, rec);
    }
  else
    {
      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[*n];
      mp3 = &iwork[(*n * 2)];
      mp4 = &iwork[*n * 3];
      mp5 = &iwork[(*n * 4)];
      iw = &iwork[*n * 5];

      /* iwork(MP4) = eps */
      epsilon(n, params, mp4);

      if (mpgt(xmp, result, n, params))
        {
          /* If xmp > 1, error. */
          sprintf(rec, "exp_kernel: argument XMP is greater than 1");
          ifl = p01abf(&c_0, &c_1, "exp_kernel", &c_1, rec);
        }
      else if (mplt(xmp, mp4, n, params))
        {
          /* If xmp is less than epsilon, result is 1. */
          return;
        }
      else
        {
          /* Compute exp(x/16) using exp(x) = SUM x**n/n! */
          /* This should converge quickly - better than two */
          /* decimal places per iteration. Retrieve the desired result */
          /* by raising the result to the power 16. */

          /* iwork(MP2) = xmp / 16 */
          ifl = 0;
          itomp(&c_16, mp1, n, params, &ifl);
          divide(xmp, mp1, mp2, n, params,
                 mp3, &ifl);

          /* result = 1 */
          itomp(&c_1, result, n, params, &ifl);

          /* iwork(MP5) = term = 1 */
          assign(mp5, result, n, params);

          i = 1;
        L20:

          /* iwork(MP1) = i */
          itomp(&i, mp1, n, params, &ifl);

          /* iwork(MP5) = term = term * xmp / i */
          times(mp5, mp2, mp3, n, params, &ifl);
          divide(mp3, mp1, mp5, n, params,
                 iw, &ifl);

          /* result = result + term */
          plus(result, mp5, mp1, n, params, &ifl);
          assign(result, mp1, n, params);

          /* If term > eps continue iterating. */
          if (mpiszero(mp5, n, params))
            {
              sprintf(rec, "exp_kernel: term underflowed to zero");
              ifl = p01abf(&c_0, &c_2, "exp_kernel", &c_1, rec);
            }
          else if (mpge(mp5, mp4, n, params))
            {
              ++i;
              goto L20;
            }

          /* Raise the result to the power 16 to get the final result. */
          for (i = 1; i <= 2; ++i)
            {
              times(result, result, mp1, n, params,
                    &ifl);
              times(mp1, mp1, result, n, params,
                    &ifl);
            }
        }
    }

  return;
} /* exp_kernel */

void mpexp(int *xmp, int *n, int *params,
           int *result, int *iwork, int *ifail)
{
  int nrec, sign, ierr, nreq;
  int ipart, i_1, i_2, i_3;
  int *iw;
  int fptype;
  int *mp1, *mp2, *mp3, *mp4;
  char rec[125];
  int ifl;
  int n_long, params_long[15];

  /* Returns exp(x) */

  ierr = 0;
  nrec = 0;
  fptype = get_fptype(params);
  sign = get_sign(params);

  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else if (mpisnan(xmp, n, params))
    {
      result[fptype - 1] = qnancon();
      result[sign - 1] = xmp[sign - 1];
    }
  else if (mpisinfinity(xmp, n, params))
    {
      result[sign - 1] = poscon();
      if (mpispos(xmp, n, params))
        result[fptype - 1] = infcon();
      else
        result[fptype - 1] = zercon();
    }
  else
    {
      /* Construct a number system with 50% more precision */
      /* This avoids any loss of accuracy in exp_kernel */
      i_1 = (get_mantis(params) * 3) / 2;
      i_2 = get_emin(params);
      i_3 = get_emax(params);
      mpinitcopy(params, &i_1, &i_2, &i_3, &n_long, params_long,
                 &ifl);

      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[n_long];
      mp3 = &iwork[(n_long * 2)];
      mp4 = &iwork[n_long * 3];
      iw = &iwork[(n_long * 4)];

      /* IWORK(MP1) = abs(XMP) */
      mpcopy2(xmp, n, params, mp1, &n_long, params_long, &ifl);
      mpabs(mp1, &n_long, params_long, &ifl);

      ifl = 1;
      ipart = mptoi(mp1, &n_long, params_long, &ifl);
      if (ifl != 0)
        {
          /* The int part of XMP was very big. exp(XMP) would */
          /* overflow or underflow, depending on the sign of XMP. */
          if (mpispos(xmp, n, params))
            result[fptype - 1] = infcon();
          else
            result[fptype - 1] = zercon();
          result[sign - 1] = poscon();
        }
      else
        {
          /* Split the number into int and fractional parts. */
          /* IWORK(MP4) = int part. */
          itomp(&ipart, mp4, &n_long, params_long, &ifl);
          /* IWORK(MP2) = fractional part. */
          minus(mp1, mp4, mp2, &n_long, params_long, &ifl);
          /* IWORK(MP3) = e */
          get_e(&n_long, params_long, mp3, iw);
          /* IWORK(MP4) = e**ipart = exp(int part) */
          mpipow(&ipart, mp3, &n_long, params_long, mp4, iw, &ifl);
          /* IWORK(MP1) = exp(fractional part) */
          exp_kernel(mp2, &n_long, params_long, mp1, iw);
          /* mp3 = exp(x) = exp(int part) * exp(fractional part) */
          times(mp4, mp1, mp3, &n_long, params_long, &ifl);

          /* Adjust for negative argument */
          if (mpisneg(xmp, n, params))
            {
              itomp(&c_1, mp1, &n_long, params_long, &ifl);
              divide(mp1, mp3, mp2, &n_long, params_long,
                     iw, &ifl);
              assign(mp3, mp2, &n_long, params_long);
            }

          /* Copy back to input precision */
          mpcopy2(mp3, &n_long, params_long, result, n, params, &ifl);
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPEXP", &nrec, rec);
  return;

} /* mpexp */

void mpexp2(int *xmp, int *logbase, int *n,
            int *params, int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int *iw;
  int fptype;
  int *mp1, *mp2, *mp3;
  char rec[125];
  int ifl, ipart, c2;

  /* Computes exp2(x) (2 to the power x), with the result in RESULT. */
  /* LOGBASE is assumed to contain log(2) on input. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(logbase, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LOGBASE is corrupt or uninitialised");
        }
      else if (mpiszero(xmp, n, params))
        itomp(&c_1, result, n, params, &ifl);
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        {
          if (mpisneg(xmp, n, params))
            itomp(&c_0, result, n, params, &ifl);
          else
            assign(result, xmp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[*n * 2];
          iw = &iwork[*n * 3];

          ifl = 1;
          ipart = mptoi(xmp, n, params, &ifl);
          if (ifl != 0)
            {
              /* The int part of XMP was very big. exp2(XMP) would */
              /* overflow or underflow, depending on the sign of XMP. */
              if (mpispos(xmp, n, params))
                result[fptype - 1] = infcon();
              else
                result[fptype - 1] = zercon();
              result[sign - 1] = poscon();
            }
          else
            {
              /* Split the number into int and fractional parts. */
              /* MP2 = int part. */
              itomp(&ipart, mp2, n, params, &ifl);
              /* MP1 = fractional part. */
              minus(xmp, mp2, mp1, n, params, &ifl);

              /* Set MP2 = frac * logbase */
              times(mp1, logbase, mp2, n, params, &ifl);

              /* Get exp(frac*log(2)) in MP1. */
              mpexp(mp2, n, params, mp1, iw, &ifl);

              /* Get 2**ipart in MP3 */
              c2 = 2;
              itomp(&c2, mp2, n, params, &ifl);
              mpipow(&ipart, mp2, n, params, mp3,
                     iw, &ifl);

              /* The result is MP1 * MP3 */
              times(mp3, mp1, result, n, params, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPEXP2", &nrec, rec);
  return;

} /* mpexp2 */

void mpexp10(int *xmp, int *log10e, int *n,
             int *params, int *result, int *iwork, int *ifail)
{
  int nrec;
  int sign, ierr, nreq;
  int *iw;
  int fptype;
  int *mp1, *mp2, *mp3;
  char rec[125];
  int ifl, ipart, c10;

  /* Computes exp10(x) (10 to the power x), with the result in RESULT. */
  /* LOG10E is assumed to contain log10(e) on input. */

  ierr = 0;
  nrec = 0;
  ifl = 0;

  if (! chksum(params))
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "Array PARAMS was not initialised by a call of MPINIT");
    }
  else
    {
      sign = get_sign(params);
      fptype = get_fptype(params);
      nreq = get_mplen(params);
      if (*n < nreq)
        {
          ierr = 1;
          nrec = 1;
          sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
                  nreq, ": N = ", *n);
        }
      else if (mpisfake(xmp, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "XMP is corrupt or uninitialised");
        }
      else if (mpisfake(log10e, n, params))
        {
          ierr = 2;
          nrec = 1;
          sprintf(rec, "LOG10E is corrupt or uninitialised");
        }
      else if (mpiszero(xmp, n, params))
        itomp(&c_1, result, n, params, &ifl);
      else if (mpisnan(xmp, n, params))
        {
          result[fptype - 1] = qnancon();
          result[sign - 1] = xmp[sign - 1];
        }
      else if (mpisinfinity(xmp, n, params))
        {
          if (mpisneg(xmp, n, params))
            itomp(&c_0, result, n, params, &ifl);
          else
            assign(result, xmp, n, params);
        }
      else
        {
          /* Dissect workspace */
          mp1 = &iwork[0];
          mp2 = &iwork[*n];
          mp3 = &iwork[*n * 2];
          iw = &iwork[*n * 3];

          ifl = 1;
          ipart = mptoi(xmp, n, params, &ifl);
          if (ifl != 0)
            {
              /* The int part of XMP was very big. exp10(XMP) would */
              /* overflow or underflow, depending on the sign of XMP. */
              if (mpispos(xmp, n, params))
                result[fptype - 1] = infcon();
              else
                result[fptype - 1] = zercon();
              result[sign - 1] = poscon();
            }
          else
            {
              /* Split the number into int and fractional parts. */
              /* MP2 = int part. */
              itomp(&ipart, mp2, n, params, &ifl);
              /* MP1 = fractional part. */
              minus(xmp, mp2, mp1, n, params, &ifl);

              /* Set MP2 = frac * log(10) = frac / log10(e) */
              divide(mp1, log10e, mp2, n, params,
                     iw, &ifl);

              /* Get exp(frac*log(10)) in MP1. */
              mpexp(mp2, n, params, mp1, iw, &ifl);

              /* Get 10**ipart in MP3 */
              c10 = 10;
              itomp(&c10, mp2, n, params, &ifl);
              mpipow(&ipart, mp2, n, params, mp3,
                     iw, &ifl);

              /* The result is MP1 * MP3 */
              times(mp3, mp1, result, n, params, &ifl);
            }
        }
    }

  *ifail = p01abf(ifail, &ierr, "MPEXP10", &nrec, rec);
  return;

} /* mpexp10 */

void mpsinh_kernel(int *xmp, int *n, int *params,
                   int *result, int *iwork)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int i;
  int *iw;
  int *mp1, *mp2, *mp3, *mp4, *mp5;
  char rec[125];
  int ifl;

  /* Returns sinh(xmp), 0 <= abs(xmp) <= 1/16, to the precision of
     numbers in system PARAMS */

  ifl = 0;

  /* result = 1 */
  itomp(&c_1, result, n, params, &ifl);

  if (mpisnan(xmp, n, params))
    {
      sprintf(rec, "mpsinh_kernel: argument XMP is NaN");
      ifl = p01abf(&c_0, &c_2, "mpsinh_kernel", &c_1, rec);
    }
  else if (mpisinfinity(xmp, n, params))
    {
      sprintf(rec, "mpsinh_kernel: argument XMP is +/-Infinity");
      ifl = p01abf(&c_0, &c_2, "mpsinh_kernel", &c_1, rec);
    }
  else
    {
      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[*n];
      mp3 = &iwork[(*n * 2)];
      mp4 = &iwork[*n * 3];
      mp5 = &iwork[(*n * 4)];
      iw = &iwork[*n * 5];

      /* iwork(MP3) = 1/16 */
      itomp(&c_1, mp1, n, params, &ifl);
      itomp(&c_16, mp2, n, params, &ifl);
      divide(mp1, mp2, mp3, n, params,
             iw, &ifl);

      /* iwork(MP4) = eps */
      epsilon(n, params, mp4);

      /* iwork(MP2) = abs(xmp) */
      assign(mp2, xmp, n, params);
      mpabs(mp2, n, params, &ifl);

      if (mpgt(mp2, mp3, n, params))
        {
          /* If xmp > 1/16, error. */
          sprintf(rec, "mpsinh_kernel: argument XMP is greater than 1/16");
          ifl = p01abf(&c_0, &c_2, "mpsinh_kernel", &c_1, rec);
        }
      else if (mplt(mp2, mp4, n, params))
        {
          /* If xmp is less than epsilon, result is xmp */
          assign(result, xmp, n, params);
          return;
        }
      else
        {
          /* Compute sinh(x) = x + x**3/3! + x**5/5! + ... */
          /* This should converge quickly - better than two */
          /* decimal places per iteration. */

          /* iwork(MP1) = result = abs(xmp) */
          assign(mp1, mp2, n, params);
          assign(result, mp1, n, params);

          /* iwork(MP5) = term = abs(xmp) */
          assign(mp5, result, n, params);

          /* iwork(MP2) = pow = xmp**2 */
          times(mp1, mp1, mp2, n, params, &ifl);

          i = 2;
        L20:

          /* iwork(MP1) = i*(i+1) */
          i_1 = i * (i + 1);
          itomp(&i_1, mp1, n, params, &ifl);

          /* iwork(MP5) = term = term * xmp**2 / (i*(i+1)) */
          times(mp5, mp2, mp3, n, params, &ifl);
          divide(mp3, mp1, mp5, n, params,
                 iw, &ifl);

          /* result = result + term */
          plus(result, mp5, mp1, n, params, &ifl);
          assign(result, mp1, n, params);

          /* If term > eps continue iterating. */
          if (mpiszero(mp5, n, params))
            {
              sprintf(rec, "mpsinh_kernel: term underflowed to zero");
              ifl = p01abf(&c_0, &c_3, "mpsinh_kernel", &c_1, rec);
            }
          else if (mpge(mp5, mp4, n, params))
            {
              i += 2;
              goto L20;
            }

          /* Get the sign right. */
          if (mpisneg(xmp, n, params))
            mpnegate(result, n, params, &ifl);
        }
    }

  return;
} /* mpsinh_kernel */

void mpcosh_kernel(int *xmp, int *n, int *params,
                   int *result, int *iwork)
{
  /* System generated locals */
  int i_1;

  /* Local variables */
  int i;
  int *iw;
  int *mp1, *mp2, *mp3, *mp4, *mp5;
  char rec[125];
  int ifl;

  /* Returns cosh(xmp), 0 <= abs(xmp) <= 1/16, to the precision of
     numbers in system PARAMS */

  /* result = 1 */
  itomp(&c_1, result, n, params, &ifl);

  if (mpisnan(xmp, n, params))
    {
      sprintf(rec, "mpcosh_kernel: argument XMP is NaN");
      ifl = p01abf(&c_0, &c_2, "mpcosh_kernel", &c_1, rec);
    }
  else if (mpisinfinity(xmp, n, params))
    {
      sprintf(rec, "mpcosh_kernel: argument XMP is +/-Infinity");
      ifl = p01abf(&c_0, &c_2, "mpcosh_kernel", &c_1, rec);
    }
  else
    {
      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[*n];
      mp3 = &iwork[(*n * 2)];
      mp4 = &iwork[*n * 3];
      mp5 = &iwork[(*n * 4)];
      iw = &iwork[*n * 5];

      /* iwork(MP3) = 1/16 */
      itomp(&c_1, mp1, n, params, &ifl);
      itomp(&c_16, mp2, n, params, &ifl);
      divide(mp1, mp2, mp3, n, params,
             mp4, &ifl);

      /* iwork(MP4) = eps */
      epsilon(n, params, mp4);

      /* iwork(MP2) = abs(xmp) */
      assign(mp2, xmp, n, params);
      mpabs(mp2, n, params, &ifl);

      if (mpgt(mp2, mp3, n, params))
        {
          /* If xmp > 1/16, error. */
          sprintf(rec, "mpcosh_kernel: argument XMP is greater than 1/16");
          ifl = p01abf(&c_0, &c_2, "mpcosh_kernel",
                       &c_1, rec);
        }
      else if (mplt(mp2, mp4, n, params))
        {
          /* If xmp is less than epsilon, result is 1 */
          itomp(&c_1, result, n, params, &ifl);
          return;
        }
      else
        {
          /* Compute cosh(x) = 1 + x**2/2! + x**4/4! + ... */
          /* This should converge quickly - better than two */
          /* decimal places per iteration. */

          /* iwork(MP1) = result = 1 */
          itomp(&c_1, mp1, n, params, &ifl);
          assign(result, mp1, n, params);

          /* iwork(MP5) = term = 1 */
          assign(mp5, result, n, params);

          /* iwork(MP2) = pow = xmp**2 */
          times(xmp, xmp, mp2, n, params, &ifl);

          i = 1;
        L20:

          /* iwork(MP1) = i*(i+1) */
          i_1 = i * (i + 1);
          itomp(&i_1, mp1, n, params, &ifl);

          /* iwork(MP5) = term = term * xmp**2 / (i*(i+1)) */
          times(mp5, mp2, mp3, n, params, &ifl);
          divide(mp3, mp1, mp5, n, params, iw, &ifl);

          /* result = result + term */
          plus(result, mp5, mp1, n, params, &ifl);
          assign(result, mp1, n, params);

          /* If term > eps continue iterating. */
          if (mpiszero(mp5, n, params))
            {
              sprintf(rec, "mpcosh_kernel: term underflowed to zero");
              ifl = p01abf(&c_0, &c_3, "mpcosh_kernel", &c_1, rec);
            }
          else if (mpge(mp5, mp4, n, params))
            {
              i += 2;
              goto L20;
            }
        }
    }

  return;
} /* mpcosh_kernel */

void mpreldiff(REAL *x, int *base_x, int *mantis_x, int *emin_x, int *emax_x, int *xmp,
               int *n, int *params, REAL *r, REAL *ulps,
               int *iwork, int *ifail)
{
  /* System generated locals */

  /* Local variables */
  int nrec, ierr, iexo, nreq, expo, places_x;
  int i;
  REAL large;
  int infin;
  int wplac, radix;
  int *iw;
  int radigs;
  int last_x;
  int *mp1, *mp2;
  char rec[125];
  int ifl;
  int wplaces;
  int ontrack;

  /* Computes the relative difference between X and XMP. BASE_X is */
  /* the base of standard floating-point arithmetic. MANTIS_X is */
  /* the precision in base BASE_X digits of standard floating-point */
  /* arithmetic. EMIN_X is the minimum exponent for floating-point */
  /* arithmetic. EMAX_X is the maximum exponent for floating-point */
  /* arithmetic. */

  ierr = 0;
  nrec = 0;
  *r = -11111111.0;
  large = (REAL)12345e10; /* Dummy value to avoid compiler warning */
  *ulps = *r;

  iexo = get_iexo(params);
  nreq = get_mplen(params);
  if (*n < nreq)
    {
      ierr = 1;
      nrec = 1;
      sprintf(rec, "%s %d %s %d", "N is too small. N must be at least",
              nreq, ": N = ", *n);
    }
  else if (mpisfake(xmp, n, params))
    {
      ierr = 2;
      nrec = 1;
      sprintf(rec, "XMP is corrupt or uninitialised");
    }
  else
    {
      /* Dissect workspace */
      mp1 = &iwork[0];
      mp2 = &iwork[*n];
      iw = &iwork[(*n * 2)];

      /* Put a large number in LARGE. We return this in lieu */
      /* of infinity so that we don't end up passing infinities */
      /* to gnuplot. */
      ifl = 0;
      itomp(&c_1, mp1, n, params, &ifl);
      mp1[iexo - 1] = *mantis_x;
      mptod(mp1, n, params, &large, &ifl);

      /* Convert X to our format in IWORK(MP1) */
      dtomp(x, mp1, n, params, &ifl);

      /* See whether XMP is larger than the overflow threshold. */
      /* Set INFIN = .TRUE. if XMP is too large to be stored in */
      /* a floating-point number, .FALSE. otherwise. */
      infin = mpisinfinity(xmp, n, params) ||
        (mpisreg(xmp, n, params) && xmp[iexo - 1] > *emax_x);

      /* We need to check edge cases, where the exponent is exactly */
      /* equal to EMAX_X, carefully. */
      if (! infin && (mpisreg(xmp, n, params) && xmp[iexo - 1] == *emax_x))
        {
          radigs = get_radigs(params);
          radix = get_radix(params);
          wplaces = get_wplaces(params);
          wplac = xmp[wplaces - 1];
          places_x = (*mantis_x - 1) / radigs + 1;
          last_x = ipower(base_x, radigs * places_x - *mantis_x);
          ontrack = 1;
          if (wplac < places_x)
            ontrack = 0;
          else
            {
              /* Check that all bits up to mantis_x are set. */
              for (i = 1; i <= places_x - 1; ++i)
                {
                  if (xmp[i - 1] < radix - 1)
                    ontrack = 0;
                }
              if (xmp[places_x - 1] < (radix - 1) / last_x * last_x)
                ontrack = 0;
            }
          /* If they were all set, check whether any further bit is set. */
          /* If so, set INFIN to .TRUE. */
          if (ontrack)
            {
              for (i = places_x + 1; i <= wplac; ++i)
                {
                  if (xmp[i - 1] > 0)
                    infin = 1;
                }
            }
        }

      if (mpisnan(xmp, n, params))
        {
          if (mpisnan(mp1, n, params))
            {
              /* Both XMP and X are NaN; return 0 */
              *r = 0.0;
              *ulps = *r;
            }
          else
            {
              /* XMP is NaN but X is not; return large number */
              *r = large;
              *ulps = *r;
            }
        }
      else if (mpisnan(mp1, n, params))
        {
          /* X is NaN but XMP is not; return large number */
          *r = large;
          *ulps = *r;
        }
      else if (mpisinfinity(mp1, n, params))
        {
          if (infin)
            {
              /* The machine number is infinite and INFIN says */
              /* we allow that. */
              if (mpispos(xmp, n, params) == mpispos(mp1, n, params))
                {
                  *r = 0.0;
                  *ulps = 0.0;
                }
              else
                {
                  /* The sign of the infinity was wrong. */
                  /* Relative difference is infinite; return large number */
                  *r = large;
                  *ulps = myfabs(*r);
                }
            }
          else
            {
              /* The machine number is infinite, but XMP is not. */
              /* Relative difference is infinite; return large number */
              *r = large;
              *ulps = myfabs(*r);
            }
        }
      else if (mpiszero(xmp, n, params))
        {
          if (mpiszero(mp1, n, params))
            {
              *r = 0.0;
              *ulps = 0.0;
            }
          else
            {
              /* Expected result XMP is exactly zero but X is not zero. */
              *r = myfabs(*x);
              *ulps = *r;
              for (i = 1; i <= -(*mantis_x); ++i)
                *ulps /= *base_x;
            }
        }
      else if (mpiszero(mp1, n, params) && xmp[iexo - 1] > *emin_x - *mantis_x + 1)
        {
          /* X is zero, but XMP is not, and neither does it have */
          /* an exponent less than emin. */
          /* Relative difference is infinite; return large number */
          *r = large;
          *ulps = *r;
        }
      else
        {
          /* Right. Both numbers are non-NaN, non-infinite, */
          /* and non-zero. Phew. */
          int rescale;
          expo = mp1[iexo - 1];
          minus(mp1, xmp, mp2, n, params, &ifl);
          mptod(mp2, n, params, r, &ifl);
          *r = myfabs(*r);
          if (mp2[iexo - 1] < *emin_x)
            {
              /* Sigh. The difference is denormalised so we need more
                 messing about. The numbers x and xmp may or may not have
                 been denormalised. Scale things up for the ulp
                 calculation. */
              rescale = 1;
              mp2[iexo - 1] += *mantis_x;
              expo += *mantis_x;
              mptod(mp2, n, params, ulps, &ifl);
              *ulps = myfabs(*ulps);
              mp2[iexo - 1] -= *mantis_x;
            }
          else
            {
              rescale = 0;
              *ulps = *r;
            }

          /* This obvious calculation of ONEULP fails if EXPO */
          /* is too large or too small, so we need to take care: */
          /* db = (REAL) (*base_x);
             ONEULP = DB**(-MANTIS_X)*DB**EXPO
             ULPS = R/ONEULP */
          if (expo < *mantis_x)
            {
              for (i = 1; i <= *mantis_x - expo; ++i)
                *ulps *= *base_x;

              /* This loop recalibrates when we're dealing with
                 denormalised numbers */
	      for (i = 1; i <= *emin_x - (expo - *mantis_x*rescale); i++)
		*ulps /= *base_x;
            }
          else
            {
              for (i = 1; i <= expo - *mantis_x; ++i)
                *ulps /= *base_x;
            }

          if (mpiszero(xmp, n, params))
            {
              if (*x == 0.0)
                *r = 0.0;
              else
                *r = myfabs(*x);
            }
          else
            {
              divide(mp2, xmp, mp1, n, params,
                     iw, &ifl);
              assign(mp2, mp1, n, params);
              mptod(mp2, n, params, r, &ifl);
              *r = myfabs(*r);
            }
        }
    }

  if (*r < 0.0)
    *r = -*r;
  if (*r == 0.0)
    /* Get rid of negative zeros */
    *r = 0.0;
  if (*r > 1.0)
    *r = 1.0;
  if (*ulps < 0.0)
    *ulps = -*ulps;
  if (*ulps == 0.0)
    /* Get rid of negative zeros */
    *ulps = 0.0;
  if (*ulps > large)
    *ulps = large;

  *ifail = p01abf(ifail, &ierr, "MPRELDIFF", &nrec, rec);
  return;

} /* mpreldiff */

/* Raises i to the power k, where i and k are small ints >= 0 */
int ipower(int *i, int k)
{
  /* System generated locals */
  int ret_val;

  /* Local variables */
  int ik, pow;

  ret_val = 1;
  pow = *i;
  ik = k;
 L20:
  if (ik > 0)
    {
      if (ik % 2 == 1)
        ret_val *= pow;
      pow *= pow;
      ik /= 2;
      goto L20;
    }
  return ret_val;
} /* ipower */

int myimax(int i, int j)
{
  /* System generated locals */
  int ret_val;

  if (i >= j)
    ret_val = i;
  else
    ret_val = j;
  return ret_val;
} /* myimax */

int myimin(int i, int j)
{
  /* System generated locals */
  int ret_val;

  if (i <= j)
    ret_val = i;
  else
    ret_val = j;
  return ret_val;
} /* myimin */

int myiabs(int i)
{
  /* System generated locals */
  int ret_val;

  if (i >= 0)
    ret_val = i;
  else
    ret_val = -i;
  return ret_val;
} /* myiabs */

REAL myfabs(REAL f)
{
  /* System generated locals */
  REAL ret_val;

  if (f >= 0.0)
    ret_val = f;
  else
    ret_val = -f;
  return ret_val;
} /* myfabs */

int p01abf(int *ifail, int *ierr, const char *srname, int *nrec,
           const char *rec)
{
  /* System generated locals */
  int ret_val;

  if (*ierr != 0)
    {
      if (*ifail != 1)
        {
          printf("Error reported by routine %s\n", srname);
          if (*nrec >= 0)
            printf("%s\n", rec);
          fflush(stdout);
          if (*ifail == 0)
            exit(1);
        }
    }
  ret_val = *ierr;
  return ret_val;
} /* p01abf */

int x02bhf(void)
{
  /* System generated locals */
  int ret_val;
  ret_val = 2;
  return ret_val;
} /* x02bhf */

int x02bjf(void)
{
  /* System generated locals */
  int ret_val;
  ret_val = 53;
  return ret_val;
} /* x02bjf */

int x02bkf(void)
{
  /* System generated locals */
  int ret_val;
  ret_val = -1021;
  return ret_val;
} /* x02bkf */

int x02blf(void)
{
  /* System generated locals */
  int ret_val;
  ret_val = 1024;
  return ret_val;
} /* x02blf */

int x02baf(void)
{
  /* System generated locals */
  int ret_val;
  ret_val = 2147483647;
  return ret_val;
} /* x02baf */
