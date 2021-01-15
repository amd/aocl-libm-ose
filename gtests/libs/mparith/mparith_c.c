#include "mparith_c.h"
#include "mparith_f.h"

void
initMultiPrecision(int isdouble, int forcePrecision,
        int *base, int *mantis, int *emin, int *emax,
        fp_params *params) {
    int ifail, round;
    int base_long, mantis_long, emin_long, emax_long;

    if (isdouble) {
        /* Initialize a floating-point number system based on IEEE
        double precision characteristics. */
        *base = 2;
        *mantis = 53;
        *emin = -1021;
        *emax = 1024;

        if (forcePrecision) {
            /* Force floating-point register arithmetic to double precision */
            printf("**** N.B. Forcing x87 register arithmetic to round to double\n");
            setprecision(1);
        }
    } else {
        /* Initialize a floating-point number system based on IEEE
        float precision characteristics. */
        *base = 2;
        *mantis = 24;
        *emin = -125;
        *emax = 128;

        if (forcePrecision) {
            /* Force floating-point register arithmetic to float precision */
            printf("**** N.B. Forcing x87 register arithmetic to round to float\n");
            setprecision(0);
        }
    }

    /* Double the precision and exponent range for our simulated arithmetic. */
    base_long = *base;
    mantis_long = 2 * (*mantis);
    emin_long = 2 * (*emin);
    emax_long = 2 * (*emax);
    round = 1;
    ifail = 0;
    /* The structure "params" describes the number system. The
       structure is passed around attached to numbers of the system. */
    MPINIT(base_long, mantis_long, emin_long, emax_long, round, params, &ifail);
}

void
MPINIT(int BASE, int MANTIS, int EMIN, int EMAX, int ROUND,
        fp_params *PARAMS, int *IFAIL) {
    mpinit(&BASE, &MANTIS, &EMIN, &EMAX, &ROUND,
            &PARAMS->n, PARAMS->params, IFAIL);
}

int *
new_mp(fp_params params) {
    if (params.n <= 0)
    { return NULL; }
    else
    { return (int *)malloc(params.n * sizeof(int)); }
}

/* myrand returns a REAL in the range 0 to 1.
   We use a low quality generator to avoid having to
   find drand48. It's not important that the generator
   always starts at the same place. */
REAL
myrand(void) {
    static unsigned long seed = 123456789;
    const unsigned long a = 9301;
    const unsigned long c = 49297;
    const unsigned long m = 233280;
    seed = (seed * a + c) % m;
    return (REAL)seed / (REAL)m;
}

/* myrand2 returns a REAL in the range 0 to 1.
   We use a low quality generator to avoid having to
   find drand48. It's not important that the generator
   always starts at the same place.
   This copy of myrand is used to generate a separate
   sequence of random numbers for the timing functions
   which doesn't interfere with that for the accuracy
   functions. */
REAL
myrand2(void) {
    static unsigned long seed = 123456789;
    const unsigned long a = 9301;
    const unsigned long c = 49297;
    const unsigned long m = 233280;
    seed = (seed * a + c) % m;
    return (REAL)seed / (REAL)m;
}

/* Checks that the array PARAMS has correct check sum. */
int
CHKSUM(fp_params PARAMS) {
    return chksum(PARAMS.params);
}

void
MPINITCOPY(fp_params PARAMS_IN, int MANTIS, int EMIN, int EMAX,
        fp_params *PARAMS_OUT, int *IFAIL) {
    mpinitcopy(PARAMS_IN.params, &MANTIS, &EMIN, &EMAX,
            &PARAMS_OUT->n, PARAMS_OUT->params, IFAIL);
}

void
MPTOD(int *XMP, fp_params PARAMS, REAL *X, int *IFAIL) {
    mptod(XMP, &PARAMS.n, PARAMS.params, X, IFAIL);
}

void
DTOMP(REAL X, int *XMP, fp_params PARAMS, int *IFAIL) {
    dtomp(&X, XMP, &PARAMS.n, PARAMS.params, IFAIL);
}

void
ITOMP(int K, int *XMP, fp_params PARAMS, int *IFAIL) {
    itomp(&K, XMP, &PARAMS.n, PARAMS.params, IFAIL);
}

void
MPSHOW(const char *TITLE, int *XMP, fp_params PARAMS, FILE *outfile) {
    if (mpisnan(XMP, &PARAMS.n, PARAMS.params))
    { fprintf(outfile, "%sNaN\n", TITLE); }
    else {
        int iexo, radix, places, wplaces, wplac, i, bunch, digit;
        char csign;
        iexo = get_iexo(PARAMS.params);
        radix = get_radix(PARAMS.params);
        places = get_places(PARAMS.params);
        wplaces = get_wplaces(PARAMS.params);
        wplac = XMP[wplaces - 1];

        if (mpispos(XMP, &PARAMS.n, PARAMS.params))
        { csign = '+'; }
        else
        { csign = '-'; }

        if (mpisinfinity(XMP, &PARAMS.n, PARAMS.params))
        { fprintf(outfile, "%s%cInfinity\n", TITLE, csign); }
        else if (mpiszero(XMP, &PARAMS.n, PARAMS.params))
        { fprintf(outfile, "%s%c0.0\n", TITLE, csign); }
        else {
            if (radix <= 10)
            { bunch = 36; }
            else if (radix <= 100)
            { bunch = 24; }
            else if (radix <= 1000)
            { bunch = 18; }
            else if (radix <= 10000)
            { bunch = 15; }
            else if (radix <= 100000)
            { bunch = 12; }
            else
            { bunch = 10; }

            fprintf(outfile, "%s%c(%d)", TITLE, csign, XMP[iexo - 1]);

            for (i = 0; i < places; i++) {
                if (i < wplac)
                { digit = XMP[i]; }
                else
                { digit = 0; }

                fprintf(outfile, " %d", digit);

                if (i > 0 && i % bunch == 0)
                { fprintf(outfile, "\n"); }
            }

            if ((places - 1) % bunch != 0)
            { fprintf(outfile, "\n"); }
        }
    }
}

static char *
dectobin(int k, int bits) {
    static char buff[101];
    int i;

    if (bits > 100)
    { bits = 100; }

    buff[bits] = '\0';

    for (i = bits - 1; i >= 0; i--) {
        if (k % 2 == 0)
        { buff[i] = '0'; }
        else
        { buff[i] = '1'; }

        k = k / 2;
    }

    return buff;
}

void
MPSHOWBIN(const char *TITLE, int *XMP, fp_params PARAMS,
        int space, FILE *outfile) {
    if (mpisnan(XMP, &PARAMS.n, PARAMS.params))
    { fprintf(outfile, "%sNaN\n", TITLE); }
    else {
        int radigs, places, wplaces, wplac, mantis, i, is, iis,
            k, bunch, zer, r, iexo, e;
        char csign;
        char *b;
        radigs = get_radigs(PARAMS.params);
        mantis = get_mantis(PARAMS.params);
        iexo = get_iexo(PARAMS.params);
        places = get_places(PARAMS.params);
        wplaces = get_wplaces(PARAMS.params);
        wplac = XMP[wplaces - 1];

        if (mpispos(XMP, &PARAMS.n, PARAMS.params))
        { csign = '+'; }
        else
        { csign = '-'; }

        if (space >= 0 && space <= mantis)
        { is = (space - 1) / radigs; }
        else
        { is = -1; }

        zer = mpiszero(XMP, &PARAMS.n, PARAMS.params);

        if (mpisinfinity(XMP, &PARAMS.n, PARAMS.params))
        { fprintf(outfile, "%s%cInfinity\n", TITLE, csign); }
        else {
            if (zer)
            { e = 0; }
            else
            { e = XMP[iexo - 1]; }

            fprintf(outfile, "%s%ce(%d)", TITLE, csign, e);

            for (i = 0; i < places; i++) {
                if (zer)
                { bunch = 0; }
                else if (i < wplac)
                { bunch = XMP[i]; }
                else
                { bunch = 0; }

                if (i == places - 1)
                { r = mantis - i * radigs; }
                else
                { r = radigs; }

                b = strdup(dectobin(bunch, radigs));

                if (is >= 0 && i == is) {
                    iis = space - is * radigs;

                    if (iis > r)
                    { iis = r; }

                    for (k = 0; k < iis; k++)
                    { fprintf(outfile, "%c", b[k]); }

                    fprintf(outfile, " ");

                    for (k = iis; k < r; k++)
                    { fprintf(outfile, "%c", b[k]); }
                } else {
                    for (k = 0; k < r; k++)
                    { fprintf(outfile, "%c", b[k]); }
                }

                free(b);
            }

            fprintf(outfile, "\n");
        }
    }
}

/* Returns .TRUE. if the number RIGHT is a regular f-p number. */
int
MPISREG(int *XMP, fp_params PARAMS) {
    return mpisreg(XMP, &PARAMS.n, PARAMS.params);
}

/* Returns .TRUE. if the number XMP is a fake f-p number
 * (uninitialised or corrupt).
 */
int
MPISFAKE(int *XMP, fp_params PARAMS) {
    return mpisfake(XMP, &PARAMS.n, PARAMS.params);
}

/* Returns .TRUE. if the number XMP is +ve or -ve zero. */
int
MPISZERO(int *XMP, fp_params PARAMS) {
    return mpiszero(XMP, &PARAMS.n, PARAMS.params);
}

/* Returns .TRUE. if the number XMP is +ve or -ve infinity. */
int
MPISINFINITY(int *XMP, fp_params PARAMS) {
    return mpisinfinity(XMP, &PARAMS.n, PARAMS.params);
}

/* Returns .TRUE. if the number XMP is NaN. */
int
MPISNAN(int *XMP, fp_params PARAMS) {
    return mpisnan(XMP, &PARAMS.n, PARAMS.params);
}

/* Returns .TRUE. if the number XMP is positive. */
int
MPISPOS(int *XMP, fp_params PARAMS) {
    return mpispos(XMP, &PARAMS.n, PARAMS.params);
}

/* Returns .TRUE. if the number XMP is negative. */
int
MPISNEG(int *XMP, fp_params PARAMS) {
    return mpisneg(XMP, &PARAMS.n, PARAMS.params);
}

/* Copies MP number from SOURCE to DEST */
void
MPCOPY(int *SOURCE, int *DEST, fp_params PARAMS, int *IFAIL) {
    mpcopy(SOURCE, DEST, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Like MPCOPY except that SOURCE and DEST do not need
 * to have the same precision parameters.
 */
void
MPCOPY2(int *SOURCE, fp_params PARAMS_SOURCE, int *DEST,
        fp_params PARAMS_DEST, int *IFAIL) {
    mpcopy2(SOURCE, &PARAMS_SOURCE.n, PARAMS_SOURCE.params,
            DEST, &PARAMS_DEST.n, PARAMS_DEST.params, IFAIL);
}

int
MPEQ(int *LEFT, int *RIGHT, fp_params PARAMS) {
    return mpeq(LEFT, RIGHT, &PARAMS.n, PARAMS.params);
}

int
MPLT(int *LEFT, int *RIGHT, fp_params PARAMS) {
    return mplt(LEFT, RIGHT, &PARAMS.n, PARAMS.params);
}

int
MPGT(int *LEFT, int *RIGHT, fp_params PARAMS) {
    return mpgt(LEFT, RIGHT, &PARAMS.n, PARAMS.params);
}

int
MPGE(int *LEFT, int *RIGHT, fp_params PARAMS) {
    return mpge(LEFT, RIGHT, &PARAMS.n, PARAMS.params);
}

int
MPLE(int *LEFT, int *RIGHT, fp_params PARAMS) {
    return mple(LEFT, RIGHT, &PARAMS.n, PARAMS.params);
}

int
MPNE(int *LEFT, int *RIGHT, fp_params PARAMS) {
    return mpne(LEFT, RIGHT, &PARAMS.n, PARAMS.params);
}

/* Sets YMP to be the floor function of XMP. */
void
MPFLOOR(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    int *IWORK = (int *)malloc(2 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPFLOOR: failed to allocate workspace\n"); }
    else {
        mpfloor(XMP, YMP, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/*  Sets YMP to be the ceil function of XMP. */
void
MPCEIL(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    int *IWORK = (int *)malloc(2 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPCEIL: failed to allocate workspace\n"); }
    else {
        mpceil(XMP, YMP, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Sets YMP to be the trunc function of XMP. */
void
MPTRUNC(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    mptrunc(XMP, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Sets YMP to be the rint function of XMP. */
void
MPRINT(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    mprint(XMP, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Sets YMP to be the round function of XMP. */
void
MPROUND(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    mpround(XMP, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Sets YMP to be the fractional part of XMP, i.e. the part
 *  left after subtracting TRUNC(XMP).
 */
void
MPFRAC(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    mpfrac(XMP, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Returns the mantissa and exponent */
void
MPFREXP(int *XMP, int *YMP, int *IYMP, fp_params PARAMS, int *IFAIL) {
    mpfrexp(XMP, YMP, IYMP, &PARAMS.n, PARAMS.params, IFAIL);
}

void
MPLDEXP(int *XMP, int EXPN, int *YMP, fp_params PARAMS, int *IFAIL) {
    mpldexp(XMP, EXPN, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

void
MPMODF(int *XMP, int *YMP, int *IYMP, fp_params PARAMS, int *IFAIL) {
    mpmodf(XMP, YMP, IYMP, &PARAMS.n, PARAMS.params, IFAIL);
}

int
MPMOD(int *XMP, fp_params PARAMS, int K, int *IFAIL) {
    int retval = -1;
    int *IWORK = (int *)malloc(8 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPMOD: failed to allocate workspace\n"); }
    else {
        retval = mpmod(XMP, &PARAMS.n, PARAMS.params, &K, IWORK, IFAIL);
        free(IWORK);
    }

    return retval;
}

/*  Returns the integer logb function of XMP. */
int
MPILOGB(int *XMP, int emin_x, fp_params PARAMS, int *IFAIL) {
    return mpilogb(XMP, &emin_x, &PARAMS.n, PARAMS.params, IFAIL);
}

/*  Sets YMP to be the logb function of XMP. */
void
MPLOGB(int *XMP, int emin_x, int *YMP, fp_params PARAMS, int *IFAIL) {
    mplogb(XMP, &emin_x, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Returns 1 if XMP is neither infinite nor NaN, otherwise returns 0 */
int
MPFINITE(int *XMP, fp_params PARAMS, int *IFAIL) {
    return mpfinite(XMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Adds LEFT to RIGHT, result in RESULT */
void
PLUS(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL) {
    plus(LEFT, RIGHT, RESULT, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Subtracts RIGHT from LEFT, result in RESULT */
void
MINUS(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL) {
    minus(LEFT, RIGHT, RESULT, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Multiplies LEFT by RIGHT, result in RESULT */
void
TIMES(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL) {
    times(LEFT, RIGHT, RESULT, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Divides LEFT by RIGHT, result in RESULT */
void
DIVIDE(int *LEFT, int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL) {
    int *IWORK = (int *)malloc(6 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "DIVIDE: failed to allocate workspace\n"); }
    else {
        divide(LEFT, RIGHT, RESULT, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes inverse square root of RIGHT, result in RESULT */
void
MPIROOT(int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL) {
    int *IWORK = (int *)malloc(6 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPIROOT: failed to allocate workspace\n"); }
    else {
        mpiroot(RIGHT, RESULT, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes square root of RIGHT, result in RESULT */
void
MPROOT(int *RIGHT, int *RESULT, fp_params PARAMS, int *IFAIL) {
    int *IWORK = (int *)malloc(6 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPROOT: failed to allocate workspace\n"); }
    else {
        mproot(RIGHT, RESULT, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes hypot(x,y) = sqrt(x**2 + y**2), putting the result in RESULT. */
void
MPHYPOT(int *XMP, int *YMP, int *RESULT, fp_params PARAMS, int *IFAIL) {
    int *IWORK = (int *)malloc(16 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPHYPOT: failed to allocate workspace\n"); }
    else {
        mphypot(XMP, YMP, RESULT, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes fma(x,y,z) = x*y+z, putting the result in RESULT. */
void
MPFMA(int *XMP, int *YMP, int *ZMP, int *RESULT, fp_params PARAMS,
        int *IFAIL) {
    int *IWORK = (int *)malloc(6 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPFMA: failed to allocate workspace\n"); }
    else {
        mpfma(XMP, YMP, ZMP, RESULT, &PARAMS.n, PARAMS.params, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Raises number XMP to power K, result in RESULT */
void
MPIPOW(int K, int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK = (int *)malloc(10 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPIPOW: failed to allocate workspace\n"); }
    else {
        mpipow(&K, XMP, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Raises number XMP to power YMP, result in RESULT */
void
MPPOW(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(46 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPPOW: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mppow(XMP, YMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Raises number XMP to power YMP, result in RESULT . YMP has to be 0.5*/
void
MPSQRT(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    int sign, fptype;
    IWORK = (int *)malloc(46 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPCBRT: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        /*Take the Abs of the Value*/
        //mpfabs(XMP,YMP,&PARAMS.n,PARAMS.params,IFAIL);

        if (mpisnan(XMP, &PARAMS.n, PARAMS.params) ||
                mpisneg(XMP, &PARAMS.n, PARAMS.params)) {
            sign = get_sign(PARAMS.params);
            fptype = get_fptype(PARAMS.params);
            RESULT[fptype - 1] = qnancon();
            RESULT[sign - 1] = XMP[sign - 1];
        } else {
            mppow(XMP, YMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        }

        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Raises number XMP to power YMP, result in RESULT . YMP has to be 1/3*/
void
MPCBRT(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    int isNeg = 0;
    IWORK = (int *)malloc(46 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPCBRT: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        /*Take the Abs of the Value*/
        //mpfabs(XMP,YMP,&PARAMS.n,PARAMS.params,IFAIL);

        if (mpisneg(XMP, &PARAMS.n, PARAMS.params)) {
            mpabs(XMP, &PARAMS.n, PARAMS.params, IFAIL);
            isNeg = 1;
        }

        //sign = get_sign(XMP);
        //XMP[sign - 1] = poscon();
        mppow(XMP, YMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);

        if (isNeg) {
            mpnegate(RESULT, &PARAMS.n, PARAMS.params, IFAIL);
        }

        free(IWORK);
        free(logbase);
        free(log10e);
    }
}
/* Computes complex exponential e^(XMP + i*YMP), result in RESULT */
void
MPCEXP(int *XMP, int *YMP, fp_params PARAMS, int *RESULT_RE, int *RESULT_IM,
        int *IFAIL) {
    int *result_exp;
    int *result_sin, *result_cos, *result_tan;
    int ifail_exp = 0, ifail_sincos = 0, ifail_re = 0, ifail_im = 0;
    result_exp = new_mp(PARAMS);
    result_sin = new_mp(PARAMS);
    result_cos = new_mp(PARAMS);
    result_tan = new_mp(PARAMS);
    MPEXP(XMP, PARAMS, result_exp, &ifail_exp); /* exp */
    MPSINCOSTAN(YMP, PARAMS, result_sin, result_cos, result_tan,
            &ifail_sincos); /* sincos */
    times(result_exp, result_cos, RESULT_RE, &PARAMS.n, PARAMS.params,
            &ifail_re); /* result_real = exp * cos */
    times(result_exp, result_sin, RESULT_IM, &PARAMS.n, PARAMS.params,
            &ifail_im); /* result_imag = exp * sin */
    free(result_exp);
    free(result_sin);
    free(result_cos);
    free(result_tan);
    *IFAIL = (ifail_exp | ifail_sincos | ifail_re | ifail_im);
}

/* Computes fmod(XMP, YMP), result in RESULT */
void
MPFMOD(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    mpfmod(XMP, YMP, &PARAMS.n, PARAMS.params, RESULT, IFAIL);
}

/* Puts the absolute value of XMP in YMP */
void
MPFABS(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    mpfabs(XMP, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Computes remainder(XMP, YMP), result in RESULT. */
void
MPREMAINDER(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    mpremainder(XMP, YMP, &PARAMS.n, PARAMS.params, RESULT, IFAIL);
}

/* Computes remquo(XMP, YMP, &QMP), result in *QMP and RESULT */
void
MPREMQUO(int *XMP, int *YMP, int *QMP, fp_params PARAMS, int *RESULT,
        int *IFAIL) {
    mpremquo(XMP, YMP, QMP, &PARAMS.n, PARAMS.params, RESULT, IFAIL);
}

/* Computes fmin(XMP, YMP), result in RESULT */
void
MPFMIN(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    mpfmin(XMP, YMP, &PARAMS.n, PARAMS.params, RESULT, IFAIL);
}

/* Computes fmax(XMP, YMP), result in RESULT */
void
MPFMAX(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    mpfmax(XMP, YMP, &PARAMS.n, PARAMS.params, RESULT, IFAIL);
}

/* Computes fdim(XMP, YMP), result in RESULT */
void
MPFDIM(int *XMP, int *YMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    mpfdim(XMP, YMP, &PARAMS.n, PARAMS.params, RESULT, IFAIL);
}

/* Computes fract(XMP), result in RESULT */
void
MPFRACT(int *XMP, int *YMP, int *IYMP, fp_params PARAMS, int *IFAIL) {
    mpfract(XMP, &PARAMS.n, PARAMS.params, YMP, IYMP, IFAIL);
}

/* Return integer part of number XMP */
int
MPTOI(int *XMP, fp_params PARAMS, int *IFAIL) {
    return mptoi(XMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Convert XMP to decimal string, and return pointer to it. */
char *
MP2DEC(int *XMP, fp_params PARAMS, int DIGITS, int *IFAIL) {
#define LENBUFF 10000
    static char buff[LENBUFF];
    int *IWORK;

    if (DIGITS > LENBUFF - 20)
    { DIGITS = LENBUFF - 20; }

    IWORK = (int *)malloc((13 * (DIGITS + 3) + 8 * PARAMS.n) * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MP2DEC: failed to allocate workspace\n"); }
    else {
        int i;
        mp2dec(XMP, &PARAMS.n, PARAMS.params, &DIGITS, IWORK, IFAIL, buff, LENBUFF);
        i = LENBUFF - 1;

        while (i > 0 && buff[i] == ' ')
        { i--; }

        if (i + 1 < LENBUFF)
        { buff[i + 1] = '\0'; }

        free(IWORK);
    }

    return buff;
}

/* Thread-safe version of MP2DEC */
void
MP2DEC_R(int *XMP, fp_params PARAMS, int DIGITS, char *C, int lenC,
        int *IFAIL) {
    int *IWORK = (int *)malloc((13 * (DIGITS + 3) + 8 * PARAMS.n) * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MP2DEC_R: failed to allocate workspace\n"); }
    else {
        int i;
        mp2dec(XMP, &PARAMS.n, PARAMS.params, &DIGITS, IWORK, IFAIL, C, lenC);
        i = lenC - 1;

        while (i > 0 && C[i] == ' ')
        { i--; }

        if (i + 1 < lenC)
        { C[i + 1] = '\0'; }

        free(IWORK);
    }
}

/* Negates XMP in place. */
void
MPNEGATE(int *XMP, fp_params PARAMS, int *IFAIL) {
    mpnegate(XMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Changes the sign of XMP and puts the result in YMP. */
void
MPCHGSIGN(int *XMP, int *YMP, fp_params PARAMS, int *IFAIL) {
    mpchgsign(XMP, YMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Sets RESULT to be XMP with the sign of YMP */
void
MPCOPYSIGN(int *XMP, int *YMP, int *RESULT, fp_params PARAMS, int *IFAIL) {
    mpcopysign(XMP, YMP, RESULT, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Replaces XMP by its absolute value. */
void
MPABS(int *XMP, fp_params PARAMS, int *IFAIL) {
    mpabs(XMP, &PARAMS.n, PARAMS.params, IFAIL);
}

/* Computes remainder_piby2(x), x in radians, with the result in RESULT */
void
MPREMAINDER_PIBY2(int *XMP, fp_params PARAMS,
        int *RESULT, int *REGION, int *IFAIL) {
    int *IWORK;
    int ifl, mantis, emax;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    fp_params params_pi;
    ifl = 0;
    mantis = get_mantis(PARAMS.params);
    emax = get_emax(PARAMS.params);
    MPINITCOPY(PARAMS, emax + 2 * mantis,
            -(emax + 2 * mantis) * 2,
            (emax + 2 * mantis) * 2,
            &params_pi, &ifl);
    pi = new_mp(params_pi);
    two_pi = new_mp(params_pi);
    pi_over_two = new_mp(params_pi);
    two_over_pi = new_mp(params_pi);
    IWORK = (int *)malloc((6 * params_pi.n + 14 * PARAMS.n) * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPREMAINDER_PIBY2: failed to allocate workspace\n"); }
    else {
        /* Fetch high-precision precomputed values of pi_over_two and
           two_over_pi. We need them to pass to the underlying
           Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(params_pi, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpremainder_piby2(XMP, &PARAMS.n, PARAMS.params,
                pi_over_two, two_over_pi, &params_pi.n,
                params_pi.params,
                RESULT, REGION, IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes remainder_90d(x), x in degrees, with the result in RESULT */
void
MPREMAINDER_90D(int *XMP, fp_params PARAMS,
        int *RESULT, int *REGION, int *IFAIL) {
    int *IWORK;
    int ifl, mantis, emax;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    fp_params params_pi;
    ifl = 0;
    mantis = get_mantis(PARAMS.params);
    emax = get_emax(PARAMS.params);
    MPINITCOPY(PARAMS, emax + 2 * mantis,
            -(emax + 2 * mantis) * 2,
            (emax + 2 * mantis) * 2,
            &params_pi, &ifl);
    pi = new_mp(params_pi);
    two_pi = new_mp(params_pi);
    pi_over_two = new_mp(params_pi);
    two_over_pi = new_mp(params_pi);
    IWORK = (int *)malloc((6 * params_pi.n + 14 * PARAMS.n) * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPREMAINDER_90D: failed to allocate workspace\n"); }
    else {
        /* Fetch high-precision precomputed values of pi_over_two and
           two_over_pi. We need them to pass to the underlying
           Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(params_pi, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpremainder_90d(XMP, &PARAMS.n, PARAMS.params,
                pi_over_two, &params_pi.n,
                params_pi.params,
                RESULT, REGION, IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes sin(x), cos(x), tan(x), of radian argument x,
   with the results in RESULT_SIN, RESULT_COS, and RESULT_TAN
   respectively.
 */
void
MPSINCOSTAN(int *XMP, fp_params PARAMS,
        int *RESULT_SIN, int *RESULT_COS, int *RESULT_TAN,
        int *IFAIL) {
    int *IWORK;
    int ifl, mantis, emax;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    fp_params params_pi;
    ifl = 0;
    mantis = get_mantis(PARAMS.params);
    emax = get_emax(PARAMS.params);
    MPINITCOPY(PARAMS, emax + 2 * mantis,
            -(emax + 2 * mantis) * 2,
            (emax + 2 * mantis) * 2,
            &params_pi, &ifl);
    pi = new_mp(params_pi);
    two_pi = new_mp(params_pi);
    pi_over_two = new_mp(params_pi);
    two_over_pi = new_mp(params_pi);
    IWORK = (int *)malloc((6 * params_pi.n + 15 * PARAMS.n) * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPSINCOSTAN: failed to allocate workspace\n"); }
    else {
        /* Fetch high-precision precomputed values of pi_over_two and
           two_over_pi. We need them to pass to the underlying
           Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(params_pi, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpsincostan(XMP, &PARAMS.n, PARAMS.params,
                pi_over_two, two_over_pi, &params_pi.n,
                params_pi.params,
                RESULT_SIN,
                RESULT_COS,
                RESULT_TAN,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes sinpi(x), cospi(x), tanpi(x), of radian argument x,
   with the results in RESULT_SIN, RESULT_COS, and RESULT_TAN
   respectively.
 */

#define GET_BITS_DP64(x, ux) \
    { \
        volatile union {double d; unsigned long long i;} _bitsy; \
        _bitsy.d = (x); \
        ux = _bitsy.i; \
    }
#define PUT_BITS_DP64(ux, x) \
    { \
        volatile union {double d; unsigned long long i;} _bitsy; \
        _bitsy.i = (ux); \
        x = _bitsy.d; \
    }

void
MPSINCOSTANPI(int *XMP, fp_params PARAMS,
        int *RESULT_SIN, int *RESULT_COS, int *RESULT_TAN,
        int *IFAIL) {
    int *IWORK;
    int ifl, mantis, emax;
    int *pi, *two_pi, *pi_over_two, *two_over_pi, *pi2;
    fp_params params_pi;
    /* special integer valure handle */
    {
        REAL x, result_sin, result_cos, result_tan;
        double dx, r;
        int xsgn, flag;
        unsigned long long ux;
        /*convert back the x value */
        mptod(XMP, &PARAMS.n, PARAMS.params, &x, IFAIL);
        flag = 0;

        if (x > 0) {
            dx   = x;
            xsgn = 1;
        } else {
            dx   = -x;
            xsgn = -1;
        }

        GET_BITS_DP64(dx, ux);

        if ((ux & 0x7ff0000000000000LL) != 0x7ff0000000000000LL) {
            if (ux >= 0x4170000000000000LL) {
                /* when x >= 2^24, the result is always even integer */
                result_sin = (REAL) 0.0 * xsgn;
                result_cos = (REAL) 1.0;
                result_tan = (REAL) 0.0 * xsgn;
                flag       = 1;
            } else {
                ux = (unsigned long long) dx;
                r  = dx - ux;

                if (r == 0.0) {
                    /* special results follows OpenCL 1.0 standard */
                    if (ux)
                    { result_sin = (REAL) 0.0 * xsgn; }
                    else /* x = 0 */
                    { result_sin = x; }

                    if (ux & 0x1) {
                        result_cos = (REAL)(-1.0);
                        result_tan = (REAL)(0.0 * (-xsgn));
                    } else {
                        result_cos = (REAL) 1.0;

                        if (ux)
                        { result_tan = (REAL)(0.0 * xsgn); }
                        else /* x = 0 */
                        { result_tan = x; }
                    }

                    flag       = 1;
                } else if (r == 0.5) {
                    /* special result follows OpenCL 1.0 standard */
                    result_cos = (REAL) 0.0;

                    if (ux & 0x1) {
                        result_sin = (REAL)(-xsgn);
                        PUT_BITS_DP64(0xfff0000000000000LL, dx);
                    } else {
                        result_sin = (REAL) xsgn;
                        PUT_BITS_DP64(0x7ff0000000000000LL, dx);
                    }

                    result_tan = (REAL) dx;
                    flag       = 1;
                }
            }

            if (flag) {
                dtomp(&result_sin, RESULT_SIN, &PARAMS.n, PARAMS.params, IFAIL);
                dtomp(&result_cos, RESULT_COS, &PARAMS.n, PARAMS.params, IFAIL);
                dtomp(&result_tan, RESULT_TAN, &PARAMS.n, PARAMS.params, IFAIL);
                return;
            }
        }
    }
    ifl = 0;
    mantis = get_mantis(PARAMS.params);
    emax = get_emax(PARAMS.params);
    MPINITCOPY(PARAMS, emax + 2 * mantis,
            -(emax + 2 * mantis) * 2,
            (emax + 2 * mantis) * 2,
            &params_pi, &ifl);
    pi = new_mp(params_pi);
    two_pi = new_mp(params_pi);
    pi_over_two = new_mp(params_pi);
    two_over_pi = new_mp(params_pi);
    IWORK = (int *)malloc((6 * params_pi.n + 15 * PARAMS.n) * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPSINCOSTAND: failed to allocate workspace\n"); }
    else {
        /* times pi for input first */
        pi2 = new_mp(PARAMS);
        get_pi(&PARAMS.n, PARAMS.params, pi2, IWORK);
        times(XMP, pi2, XMP, &PARAMS.n, PARAMS.params, IFAIL);
        /* Fetch high-precision precomputed values of pi_over_two and
           two_over_pi. We need them to pass to the underlying
           Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(params_pi, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpsincostan(XMP, &PARAMS.n, PARAMS.params,
                pi_over_two, two_over_pi, &params_pi.n,
                params_pi.params,
                RESULT_SIN,
                RESULT_COS,
                RESULT_TAN,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}


/* Computes sin(x), cos(x), tan(x), of degree argument x,
   with the results in RESULT_SIN, RESULT_COS, and RESULT_TAN
   respectively.
 */
void
MPSINCOSTAND(int *XMP, fp_params PARAMS,
        int *RESULT_SIN, int *RESULT_COS, int *RESULT_TAN,
        int *IFAIL) {
    int *IWORK;
    int ifl, mantis, emax;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    fp_params params_pi;
    ifl = 0;
    mantis = get_mantis(PARAMS.params);
    emax = get_emax(PARAMS.params);
    MPINITCOPY(PARAMS, emax + 2 * mantis,
            -(emax + 2 * mantis) * 2,
            (emax + 2 * mantis) * 2,
            &params_pi, &ifl);
    pi = new_mp(params_pi);
    two_pi = new_mp(params_pi);
    pi_over_two = new_mp(params_pi);
    two_over_pi = new_mp(params_pi);
    IWORK = (int *)malloc((6 * params_pi.n + 15 * PARAMS.n) * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPSINCOSTAND: failed to allocate workspace\n"); }
    else {
        /* Fetch high-precision precomputed values of pi_over_two and
           two_over_pi. We need them to pass to the underlying
           Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(params_pi, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpsincostand(XMP, &PARAMS.n, PARAMS.params,
                pi_over_two, two_over_pi, &params_pi.n,
                params_pi.params,
                RESULT_SIN,
                RESULT_COS,
                RESULT_TAN,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes atan(x), with the result in RESULT. */
void
MPATAN(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(13 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPATAN: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of pi_over_two. It only needs to
           be of the same precision as XMP and RESULT.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpatan(XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes atanpi(x), with the result in RESULT. */
void
MPATANPI(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(13 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPATAN: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of pi_over_two. It only needs to
           be of the same precision as XMP and RESULT.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpatan(XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        /* divided by pi */
        divide(RESULT, pi, RESULT, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes atan2(y,x), with the result in RESULT. */
void
MPATAN2(int *YMP, int *XMP, fp_params PARAMS, int *RESULT,
        int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(14 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPATAN2: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed values of pi and pi_over_two. They only need to
           be of the same precision as XMP and RESULT.
           We need them to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpatan2(YMP, XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi, pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes atan2pi(y,x), with the result in RESULT. */
void
MPATAN2PI(int *YMP, int *XMP, fp_params PARAMS, int *RESULT,
        int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(14 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPATAN2: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed values of pi and pi_over_two. They only need to
           be of the same precision as XMP and RESULT.
           We need them to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpatan2(YMP, XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi, pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        /* divided by pi */
        divide(RESULT, pi, RESULT, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes asin(x), with the result in RESULT. */
void
MPASIN(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(14 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPASIN: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of pi_over_two. It only needs to
           be of the same precision as XMP and RESULT.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpasin(XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes asinpi(x), with the result in RESULT. */
void
MPASINPI(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(14 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPASIN: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of pi_over_two. It only needs to
           be of the same precision as XMP and RESULT.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpasin(XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        /* divided by pi */
        divide(RESULT, pi, RESULT, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes acos(x), with the result in RESULT. */
void
MPACOS(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(14 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPACOS: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of pi_over_two. It only needs to
           be of the same precision as XMP and RESULT.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpacos(XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes acospi(x), with the result in RESULT. */
void
MPACOSPI(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int ifl;
    int *pi, *two_pi, *pi_over_two, *two_over_pi;
    pi = new_mp(PARAMS);
    two_pi = new_mp(PARAMS);
    pi_over_two = new_mp(PARAMS);
    two_over_pi = new_mp(PARAMS);
    IWORK = (int *)malloc(14 * PARAMS.n * sizeof(int));

    if (!IWORK || !pi || !two_pi || !pi_over_two || !two_over_pi)
    { fprintf(stderr, "MPACOSPI: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of pi_over_two. It only needs to
           be of the same precision as XMP and RESULT.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_PI_ETC(PARAMS, pi, two_pi, pi_over_two,
                two_over_pi, &ifl);
        mpacos(XMP, &PARAMS.n, PARAMS.params, RESULT,
                pi_over_two, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        /* divided by pi */
        divide(RESULT, pi, RESULT, &PARAMS.n, PARAMS.params,
                IWORK, IFAIL);
        free(IWORK);
        free(pi);
        free(two_pi);
        free(pi_over_two);
        free(two_over_pi);
    }
}

/* Computes sinh(x), with the result in RESULT. */
void
MPSINH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK = (int *)malloc(46 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPSINH: failed to allocate workspace\n"); }
    else {
        mpsinh(XMP, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes cosh(x), with the result in RESULT. */
void
MPCOSH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK = (int *)malloc(46 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPCOSH: failed to allocate workspace\n"); }
    else {
        mpcosh(XMP, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes tanh(x), with the result in RESULT. */
void
MPTANH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK = (int *)malloc(23 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPTANH: failed to allocate workspace\n"); }
    else {
        mptanh(XMP, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
    }
}

/* Computes arcsinh(x), with the result in RESULT. */
void
MPASINH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(20 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPASINH: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mpasinh(XMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Computes arccosh(x), with the result in RESULT. */
void
MPACOSH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(20 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPACOSH: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mpacosh(XMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Computes arctanh(x), with the result in RESULT. */
void
MPATANH(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(21 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPATANH: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mpatanh(XMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Computes log(x) (natural logarithm), with the result in RESULT. */
void
MPLOG(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(15 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPLOG: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mplog(XMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Computes log(1+x) (natural logarithm), with the result in RESULT. */
void
MPLOG1P(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(19 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPLOG1P: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed value of logbase.
           We need it to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mplog1p(XMP, logbase, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Computes log10(x) (logarithm to base 10), with the result in RESULT. */
void
MPLOG10(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(15 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPLOG10: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed values of logbase and log10e.
           We need them to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mplog10(XMP, logbase, log10e, &PARAMS.n, PARAMS.params,
                RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

/* Computes log2(x) (logarithm to base 2), with the result in RESULT. */
void
MPLOG2(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(15 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPLOG2: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed values of logbase and log10e.
           We need them to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mplog2(XMP, logbase, &PARAMS.n, PARAMS.params,
                RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}

void
EPSILON(fp_params PARAMS, int *EPS) {
    epsilon(&PARAMS.n, PARAMS.params, EPS);
}

void
MPLARGE(fp_params PARAMS, int *LARGE) {
    mplarge(&PARAMS.n, PARAMS.params, LARGE);
}

void
MPSMALL(fp_params PARAMS, int *SMALL) {
    mpsmall(&PARAMS.n, PARAMS.params, SMALL);
}

void
GET_PI(fp_params PARAMS, int *PI) {
    int *IWORK = (int *)malloc(18 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "GET_PI: failed to allocate workspace\n"); }
    else {
        get_pi(&PARAMS.n, PARAMS.params, PI, IWORK);
        free(IWORK);
    }
}

void
GET_PI_ETC(fp_params PARAMS, int *PI, int *TWO_PI, int *PI_OVER_TWO,
        int *TWO_OVER_PI, int *IFAIL) {
    int *IWORK = (int *)malloc(18 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "GET_PI_ETC: failed to allocate workspace\n"); }
    else {
        get_pi_etc(&PARAMS.n, PARAMS.params, PI, TWO_PI, PI_OVER_TWO,
                TWO_OVER_PI, IWORK, IFAIL);
        free(IWORK);
    }
}

void
GET_PRECOMPUTED_PI_ETC(fp_params PARAMS, int *PI, int *TWO_PI, int *PI_OVER_TWO,
        int *TWO_OVER_PI, int *IFAIL) {
    get_precomputed_pi_etc(&PARAMS.n, PARAMS.params, PI, TWO_PI, PI_OVER_TWO,
            TWO_OVER_PI, IFAIL);
}

void
GET_LOG_ETC(fp_params PARAMS, int *LOGBASE, int *LOG10E, int *IFAIL) {
    int *IWORK = (int *)malloc(16 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "GET_LOG_ETC: failed to allocate workspace\n"); }
    else {
        get_log_etc(&PARAMS.n, PARAMS.params, LOGBASE, LOG10E,
                IWORK, IFAIL);
        free(IWORK);
    }
}

void
GET_PRECOMPUTED_LOG_ETC(fp_params PARAMS,
        int *LOGBASE, int *LOG10E, int *IFAIL) {
    get_precomputed_log_etc(&PARAMS.n, PARAMS.params, LOGBASE, LOG10E,
            IFAIL);
}

void
GET_E(fp_params PARAMS, int *E) {
    int *IWORK = (int *)malloc(17 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "GET_E: failed to allocate workspace\n"); }
    else {
        get_e(&PARAMS.n, PARAMS.params, E, IWORK);
        free(IWORK);
    }
}

void
MPEXP(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK = (int *)malloc(42 * PARAMS.n * sizeof(int));

    if (!IWORK)
    { fprintf(stderr, "MPEXP: failed to allocate workspace\n"); }
    else {
        mpexp(XMP, &PARAMS.n, PARAMS.params, RESULT, IWORK, IFAIL);
        free(IWORK);
    }
}

void
MPEXPM1(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *onemp, *minusonemp, *temp, *temp1;

    if (MPISZERO(XMP, PARAMS))
    { MPCOPY(XMP, RESULT, PARAMS, IFAIL); }
    else if (MPISINFINITY(XMP, PARAMS)) {
        if (MPISPOS(XMP, PARAMS))
        { MPCOPY(XMP, RESULT, PARAMS, IFAIL); }
        else
        { DTOMP(-1.0, RESULT, PARAMS, IFAIL); }
    } else if (MPISNAN(XMP, PARAMS))
    { PLUS(XMP, XMP, RESULT, PARAMS, IFAIL); }
    else {
        MPEXP(XMP, PARAMS, RESULT, IFAIL);
        onemp = new_mp(PARAMS);
        minusonemp = new_mp(PARAMS);
        temp = new_mp(PARAMS);
        temp1 = new_mp(PARAMS);
        DTOMP(1.0, onemp, PARAMS, IFAIL);
        DTOMP(-1.0, minusonemp, PARAMS, IFAIL);

        if (MPEQ(RESULT, onemp, PARAMS))
        { MPCOPY(XMP, RESULT, PARAMS, IFAIL); }
        else {
            MINUS(RESULT, onemp, temp, PARAMS, IFAIL);

            if (MPEQ(temp, minusonemp, PARAMS))
            { MPCOPY(minusonemp, RESULT, PARAMS, IFAIL); }
            else {
                MINUS(RESULT, onemp, temp, PARAMS, IFAIL);
                TIMES(XMP, temp, temp1, PARAMS, IFAIL);
                MPLOG(RESULT, PARAMS, temp, IFAIL);

                if (MPISINFINITY(temp, PARAMS) && MPISINFINITY(temp1, PARAMS))
                { MPCOPY(temp1, RESULT, PARAMS, IFAIL); }
                else if (MPISZERO(temp, PARAMS) && MPISZERO(temp1, PARAMS))
                { MPCOPY(XMP, RESULT, PARAMS, IFAIL); }
                else
                { DIVIDE(temp1, temp, RESULT, PARAMS, IFAIL); }
            }
        }
    }
}

/* Computes exp2(x) (2 to the power x), with the result in RESULT. */
void
MPEXP2(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(48 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPEXP2: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed values of logbase and log10e.
           We need them to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mpexp2(XMP, logbase, &PARAMS.n, PARAMS.params,
                RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}


/* Computes exp10(x) (10 to the power x), with the result in RESULT. */
void
MPEXP10(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    int *IWORK;
    int *logbase, *log10e;
    int ifl;
    IWORK = (int *)malloc(48 * PARAMS.n * sizeof(int));
    logbase = new_mp(PARAMS);
    log10e = new_mp(PARAMS);

    if (!IWORK || !logbase || !log10e)
    { fprintf(stderr, "MPEXP10: failed to allocate workspace\n"); }
    else {
        /* Fetch precomputed values of logbase and log10e.
           We need them to pass to the underlying Fortran routine. */
        ifl = 0;
        GET_PRECOMPUTED_LOG_ETC(PARAMS, logbase, log10e, &ifl);
        mpexp10(XMP, log10e, &PARAMS.n, PARAMS.params,
                RESULT, IWORK, IFAIL);
        free(IWORK);
        free(logbase);
        free(log10e);
    }
}


/* Computes pow10(x) (10 to the power x), with the result in RESULT. */
void
MPPOW10(int *XMP, fp_params PARAMS, int *RESULT, int *IFAIL) {
    MPEXP10(XMP, PARAMS, RESULT, IFAIL);
}


REAL
MPRELDIFF(REAL X, int BASE_X, int MANTIS_X, int EMIN_X, int EMAX_X,
        int *XMP, fp_params PARAMS, REAL *ulps, int *IFAIL) {
    REAL retval = -11111111;
    int *IWORK = (int *)malloc(8 * PARAMS.n * sizeof(int));
    REAL r;
    *ulps = retval;

    if (!IWORK)
    { fprintf(stderr, "MPRELDIFF: failed to allocate workspace\n"); }
    else {
        mpreldiff(&X, &BASE_X, &MANTIS_X, &EMIN_X, &EMAX_X,
                XMP, &PARAMS.n, PARAMS.params, &r, ulps,
                IWORK, IFAIL);
        retval = r;
        free(IWORK);
    }

    return retval;
}


/* Output a REAL precision number x in a format that Maple
   can read without conversion error */
void
mapleX(FILE *file, REAL x, const char *varName, int base, int mantis) {
    REAL dx, recbas, pow;
    int i, digit, expo, np, first;
    /* Maple's working precision. Is this enough? */
    fprintf(file, " Digits := 500;\n");

    if (disnan(&x))
        /* Not Maple at all! */
    { fprintf(file, " %s := NaN;\n", varName); }
    else if (x == 0.0) {
        if (disneg(&x))
        { fprintf(file, " %s := -0.0;\n", varName); }
        else
        { fprintf(file, " %s := 0.0;\n", varName); }
    } else if (disinf(&x)) {
        if (disneg(&x))
        { fprintf(file, " %s := -infinity;\n", varName); }
        else
        { fprintf(file, " %s := infinity;\n", varName); }
    } else {
        dx = x;

        if (dx < 0.0)
        { dx = -dx; }

        recbas = 1;
        recbas = recbas / base;
        pow = 1.0;

        for (i = 0; i < mantis; i++)
        { pow *= base; }

        /* Scale dx into interval [1/base .. 1). */
        expo = 0;

        while (dx < recbas) {
            dx *= pow;
            expo -= mantis;
        }

        while (dx >= 1.0) {
            dx /= pow;
            expo += mantis;
        }

        while (dx < recbas) {
            dx *= base;
            expo -= 1;
        }

        if (x < 0.0)
        { fprintf(file, " %s := -(", varName); }
        else
        { fprintf(file, " %s := (", varName); }

        np = 0;
        first = 1;

        for (i = 0; i < mantis; i++) {
            dx *= base;
            digit = (int)dx;
            dx = dx - digit;

            if (digit > 0) {
                if (!first)
                { fprintf(file, " + "); }

                first = 0;

                if (digit == 1)
                { fprintf(file, "%d.0^(-%d)", base, i + 1); }
                else
                { fprintf(file, "%d*%d.0^(-%d)", digit, base, i + 1); }

                np++;

                if (np == 5) {
                    np = 0;
                    fprintf(file, " \\\n");
                }
            }
        }

        fprintf(file, " ) * %d.0^(%d);\n", base, expo);
    }
}

void
gnuplotPause(FILE *file, int postscript) {
    if (!postscript) {
        fprintf(file, "print \"Hit return to continue\"\n");
        fprintf(file, "pause -1\n");
    }
}

/* Writes a gnuplot driver file which, when passed to gnuplot,
 * will create error graphs. If argument "postscript" is 1,
 * the graphs will output to a PostScript file, otherwise to
 * the default gnuplot window.
 */
int
writeGnuplotDriver(const char *rootFileName,
        const char *title,
        int xlogscale,
        int postscript,
        REAL maxUlpErr, REAL maxX) {
    FILE *file;
    int retVal = 0;
    int ylogscale = 0;
    const char *driverExt = ".gnuplot_driver";
    char *driverFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(driverExt) + 1);
    const char *dataExt = ".gnuplot_data";
    char *dataFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(dataExt) + 1);
    const char *timesExt = ".gnuplot_times";
    char *timesFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(timesExt) + 1);
    const char *psExt = ".ps";
    char *psFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(psExt) + 1);
    sprintf(driverFileName, "%s%s", rootFileName, driverExt);
    sprintf(dataFileName, "%s%s", rootFileName, dataExt);
    sprintf(timesFileName, "%s%s", rootFileName, timesExt);
    sprintf(psFileName, "%s%s", rootFileName, psExt);
    file = fopen(driverFileName, "w");

    if (!file) {
        printf("Cannot open file %s for writing\n", driverFileName);
        retVal = 1;
    } else {
        fprintf(file, "# Driver file works with gnuplot version 3.7\n");

        if (postscript) {
            fprintf(file, "set terminal postscript color\n");
            fprintf(file, "set output \"%s\"\n", psFileName);
        }

        if (maxUlpErr > 0.0)
            fprintf(file, "set title \"%s\\n"
                    "Maximum ulp error detected was %g at x = %30.20e\"\n",
                    title, maxUlpErr, maxX);
        else
            fprintf(file, "set title \"%s\\n"
                    "Maximum ulp error detected was %g\"\n",
                    title, maxUlpErr);

        if (xlogscale)
        { fprintf(file, "set xlabel \"Argument x (log scale)\"\n"); }
        else
        { fprintf(file, "set xlabel \"Argument x\"\n"); }

        if (maxUlpErr > 1000.0) {
            /* If the maximum ulp error is very large, use a log scale */
            ylogscale = 1;
            fprintf(file, "set ylabel \"Error in ulps (log scale)\"\n");
        } else
        { fprintf(file, "set ylabel \"Error in ulps\"\n"); }

        fprintf(file, "set y2label \"Machine cycles per evaluation\"\n");

        if (xlogscale)
        { fprintf(file, "set logscale x\n"); }

        if (ylogscale)
        { fprintf(file, "set logscale y\n"); }
        else
        { fprintf(file, "set nologscale y\n"); }

        fprintf(file, "set nologscale y2\n");
        fprintf(file, "set ytics nomirror\n");

        if (ylogscale)
        { fprintf(file, "set ytics 1,10,%g\n", maxUlpErr + 1.0); }

        fprintf(file, "set y2tics\n");

        if (xlogscale)
        { fprintf(file, "set xrange [:1000]\n"); }

        if (maxUlpErr < 1.0)
        { fprintf(file, "set yrange [0.1:1.0]\n"); }
        else
        { fprintf(file, "set yrange [0.1:%g]\n", maxUlpErr + 1.0); }

        fprintf(file, "set y2range [0:*]\n");
        fprintf(file, "set key box\n");
        fprintf(file, "set size 0.85,0.85\n");
        fprintf(file, "set linestyle 1 lt 1 lw 3\n");
        fprintf(file, "set linestyle 2 lt 3 lw 3\n");
        fprintf(file, "plot \"%s\" title \"Error in ulps\" with lines ls 1, "
                "\"%s\" axes x1y2 title \"Machine cycles per evaluation\" "
                "with lines ls 2\n", dataFileName, timesFileName);
        gnuplotPause(file, postscript);

        if (xlogscale) {
            fprintf(file, "set xrange [:1.0e15]\n");
            fprintf(file, "replot\n");
            gnuplotPause(file, postscript);
            fprintf(file, "set xrange [:*]\n");
            fprintf(file, "replot\n");
            gnuplotPause(file, postscript);
        }

        fclose(file);
    }

    free(driverFileName);
    free(dataFileName);
    free(timesFileName);
    free(psFileName);
    return retVal;
}

/* Writes a gnuplot driver file which, when passed to gnuplot,
 * will create 3D error graphs. If argument "postscript" is 1,
 * the graphs will output to a PostScript file, otherwise to
 * the default gnuplot window.
 */
int
writeGnuplot3DDriver(const char *rootFileName,
        const char *title,
        int postscript,
        REAL maxUlpErr, REAL maxX, REAL maxY) {
    FILE *file;
    int retVal = 0;
    int logscalez;
    const char *driverExt = ".gnuplot_driver";
    char *driverFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(driverExt) + 1);
    const char *dataExt = ".gnuplot_data";
    char *dataFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(dataExt) + 1);
    const char *timesExt = ".gnuplot_times";
    char *timesFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(timesExt) + 1);
    const char *psExt = ".ps";
    char *psFileName = (char *)malloc(strlen(rootFileName) +
                    strlen(psExt) + 1);
    sprintf(driverFileName, "%s%s", rootFileName, driverExt);
    sprintf(dataFileName, "%s%s", rootFileName, dataExt);
    sprintf(timesFileName, "%s%s", rootFileName, timesExt);
    sprintf(psFileName, "%s%s", rootFileName, psExt);
    file = fopen(driverFileName, "w");

    if (!file) {
        printf("Cannot open file %s for writing\n", driverFileName);
        retVal = 1;
    } else {
        fprintf(file, "# Driver file works with gnuplot version 3.7\n");

        if (postscript) {
            fprintf(file, "set terminal postscript color\n");
            fprintf(file, "set output \"%s\"\n", psFileName);
        }

        if (maxUlpErr > 0.0) {
            fprintf(file, "set title \"%s\\n"
                    "Maximum ulp error detected was %g\\n"
                    "at x = %30.20e, y = %30.20e\"\n",
                    title, maxUlpErr, maxX, maxY);
            logscalez = 1;
        } else {
            fprintf(file, "set title \"%s\\n"
                    "Maximum ulp error detected was %g\"\n",
                    title, maxUlpErr);
            /* Don't use a log scale if the max ulp error is
               zero, otherwise gnuplot has nothing to plot and
               you get an empty graph */
            logscalez = 0;
        }

        fprintf(file, "set xlabel \"Argument x (log scale)\"\n");
        fprintf(file, "set ylabel \"Argument y (log scale)\"\n");
        fprintf(file, "set logscale x\n");
        fprintf(file, "set logscale y\n");

        if (logscalez)
        { fprintf(file, "set logscale z\n"); }

        fprintf(file, "set xrange [:1000]\n");
        fprintf(file, "set yrange [:1000]\n");
        fprintf(file, "set nokey\n");

        /* Unfortunately gnuplot doesn't allow us to plot two graphs
           with different z axes at the same time. Therefore we
           plot errors and times on two different graphs. */
        if (logscalez)
        { fprintf(file, "set zlabel \"Error in ulps (log scale)\"\n"); }
        else
        { fprintf(file, "set zlabel \"Error in ulps\"\n"); }

        fprintf(file, "splot \"%s\" title \"Error in ulps\" with lines 1\n",
                dataFileName);
        gnuplotPause(file, postscript);
        fprintf(file, "set zlabel \"Machine cycles per evaluation\"\n");
        fprintf(file, "splot \"%s\" title \"Machine cycles per evaluation\" "
                "with lines 3\n", timesFileName);
        gnuplotPause(file, postscript);
        fprintf(file, "set xrange [:1.0e15]\n");
        fprintf(file, "set yrange [:1.0e15]\n");

        if (logscalez)
        { fprintf(file, "set zlabel \"Error in ulps (log scale)\"\n"); }
        else
        { fprintf(file, "set zlabel \"Error in ulps\"\n"); }

        fprintf(file, "splot \"%s\" title \"Error in ulps\" with lines 1\n",
                dataFileName);
        gnuplotPause(file, postscript);
        fprintf(file, "set zlabel \"Machine cycles per evaluation\"\n");
        fprintf(file, "splot \"%s\" title \"Machine cycles per evaluation\" "
                "with lines 3\n", timesFileName);
        gnuplotPause(file, postscript);
        fprintf(file, "set xrange [:*]\n");
        fprintf(file, "set yrange [:*]\n");

        if (logscalez)
        { fprintf(file, "set zlabel \"Error in ulps (log scale)\"\n"); }
        else
        { fprintf(file, "set zlabel \"Error in ulps\"\n"); }

        fprintf(file, "splot \"%s\" title \"Error in ulps\" with lines 1\n",
                dataFileName);
        gnuplotPause(file, postscript);
        fprintf(file, "set zlabel \"Machine cycles per evaluation\"\n");
        fprintf(file, "splot \"%s\" title \"Machine cycles per evaluation\" "
                "with lines 3\n", timesFileName);
        gnuplotPause(file, postscript);
        fclose(file);
    }

    free(driverFileName);
    free(dataFileName);
    free(timesFileName);
    free(psFileName);
    return retVal;
}

/* Poor man's nextafter - doesn't have to be great */
REAL
mynextafter(REAL x, REAL y, int mantis) {
    REAL r, s, oneulp, ax;
    REAL one, recbas, pow;
    int expont, i;
    one = 1;
    recbas = one / 2;
    pow = 1024 * 1024;

    if (x < 0.0)
    { ax = -x; }
    else
    { ax = x; }

    if (disnan(&ax))
    { r = x; }
    else if (disinf(&ax))
        /* Should return largest number if y says so,
           but we don't know emax so we can't. */
    { r = x; }
    else {
        if (diszero(&ax))
        { expont = 0; }
        else {
            expont = 0;

            /* Now scale ax into the range 0.5 .. 1, keeping track of the
             * base 2 exponent in expont.
             */
            while (ax < recbas) {
                ax = ax * pow;
                expont = expont - 20;
            }

            while (ax >= 1.0) {
                ax = ax / pow;
                expont = expont + 20;
            }

            while (ax < recbas) {
                ax = ax * 2;
                expont = expont - 1;
            }
        }

        oneulp = 1.0;

        if (expont < mantis)
            for (i = 0; i < mantis - expont; i++)
            { oneulp = oneulp / 2; }
        else
            for (i = 0; i < expont - mantis; i++)
            { oneulp = oneulp * 2; }

        if (x < y)
        { r = x + oneulp; }
        else
        { r = x - oneulp; }

        /* We may have gone too far. Take the average. */
        s = (x + r) / 2;

        if (s != x && s != r)
        { r = s; }
    }

    return r;
}

/* Raise x to the integer power k */
REAL
mypow(REAL x, int k) {
    REAL r, pow, one = 1.0;
    int kneg;
    kneg = k < 0;

    if (kneg)
    { k = -k; }

    r = one;
    pow = x;

    while (k > 0) {
        if (k % 2 == 1)
        { r = r * pow; }

        pow = pow * pow;
        k /= 2;
    }

    if (kneg)
    { r = one / r; }

    return r;
}

/* A crude integer sqrt function */
int
mysqrt(int x) {
    REAL r, r2, d;
    r = x / (REAL)2.0;
    r = (REAL)0.5 + r;
    d = 1.0;

    while (d > 0.00001) {
        r2 = (REAL)0.5 * (r + x / r);
        d = r - r2;

        if (d < 0.0)
        { d = -d; }

        r = r2;
    }

    return (int)r;
}

/* Splits a floating-point number into exponent e and mantissa m */
static void
splitf(REAL f, int *e, REAL *m) {
    int expont, i;
    REAL y;
    static int first = 1;
    static REAL pow, recpow;

    if (first) {
        first = 0;
        pow = 1.0;
        recpow = 1.0;

        for (i = 0; i < 60; i++) {
            pow *= 2.0;
            recpow *= 0.5;
        }
    }

    if (f == 0.0) {
        *m = 0.0;
        *e = 0;
        return;
    }

    if (f < 0.0)
    { y = -f; }
    else
    { y = f; }

    expont = 0;

    while (y < 0.5 && expont >= -1200) {
        y *= pow;
        expont -= 60;
    }

    while (y >= 1.0 && expont <= 1200) {
        y *= recpow;
        expont += 60;
    }

    while (y < 0.5 && expont >= -1200) {
        y *= 2.0;
        expont--;
    }

    if (f > 0.0) {
        *m = y;
        *e = expont;
    } else {
        *m = -y;
        *e = expont;
    }

    return;
}

REAL
mylog(REAL x) {
    /*
     *  Computes log(x) (natural logarithm).
    */
    const REAL log2 = (REAL)0.69314718055994530941723212145818;
    REAL result, m, mp1, u, upow, w, term, inf;
    int e, i;

    if (0.5 <= x && x <= 1.5) {
        /*
         *  Computes log(x), for 0.5 <= x <= 1.5.
         *
         *  Uses ln(1+x) = x - x**2/2 + x**3 / 3 - ...
         */
        u = x - (REAL)1.0;
        upow = u;
        result = -u;
        w = u;
        i = 1;

        while (result != w) {
            result = w;
            i++;
            upow = upow * u;
            term = upow / i;

            if (i % 2 == 0)
            { w = result - term; }
            else
            { w = result + term; }
        }
    } else {
        createInfinity(0, &inf);

        if (x == 0.0) {
            /* Return -infinity */
            return -inf;
        } else if (x < 0.0) {
            createNaN(0, 0, &m);
            return m;
        } else if (x == inf)
        { return x; }
        else if (disnan(&x))
        { return x; }

        /* Split into exponent and 0.5 <= mantissa < 1.0 */
        splitf(x, &e, &m);
        mp1 = m * 2;
        /*
         *  Computes log(1+x), for 0.0 <= x <= 1.0, with the result in RESULT.
         *
         *  Uses ln(1+x) = u + u**2/2 + u**3 / 3 + ..., where u = x / (1 + x).
         */
        w = upow = u = (mp1 - (REAL)1.0) / mp1;
        result = -w;
        i = 1;

        while (result != w) {
            i++;
            result = w;
            upow *= u;
            term = upow / i;
            w = result + term;
        }

        result += log2 * (e - 1);
    }

    return result;
}

REAL
myexp(REAL x) {
    REAL result, r1, r2, w, term;
    const REAL e = (REAL)2.7182818284590452353602874713527;
    int i, ip, xneg;

    if (disnan(&x))
    { return x; }
    else if (x > 1000.0) {
        /* return overflow */
        result = (REAL)1.0e30;

        for (i = 0; i < 5; i++)
        { result = result * result; }

        return result;
    } else if (x < -1000.0)
    { return 0.0; }

    xneg = x < 0.0;

    if (xneg)
    { x = -x; }

    /* Split x into integer and fractional parts */
    ip = (int)x;
    x = x - ip;
    /* Result = exp(x) = exp(integer part) * exp(fractional part) */
    r1 = mypow(e, ip);
    /* Compute exp(x/16) using exp(x) = SUM x**n/n!
     * This should converge quickly - better than two
     * decimal places per iteration. Retrieve the desired
     * result r2 by raising it to the power 16.
     */
    x = x / (REAL)16.0;
    w = term = 1.0;
    r2 = -w;
    i = 0;

    while (r2 != w) {
        i++;
        r2 = w;
        term = term * x / i;
        w = r2 + term;
    }

    /* Raise r2 to the power 16 to get the final result. */
    r2 = r2 * r2;
    r2 = r2 * r2;
    r2 = r2 * r2;
    r2 = r2 * r2;
    /* The result of our exp is the two exps multiplied together */
    result = r1 * r2;

    if (xneg)
    { result = (REAL)(1.0 / result); }

    return result;
}


REAL
mymin(REAL x, REAL y) {
    if (x < y) { return x; }
    else { return y; }
}


int
openResultsFiles(const char *test_string,
        FILE **gnuplotDataFile,
        FILE **gnuplotTimesFile,
        FILE **summaryFile,
        FILE **verboseSummaryFile) {
    const char *gnuplotExt = ".gnuplot_data";
    const char *gnuplotTimesExt = ".gnuplot_times";
    const char *summaryExt = ".summary";
    const char *verboseExt = ".verbose";
    int retval = 1;
    char *gnuplotDataFileName = (char *)malloc(strlen(test_string) +
                    strlen(gnuplotExt) + 1);
    char *gnuplotTimesFileName = (char *)malloc(strlen(test_string) +
                    strlen(gnuplotTimesExt) + 1);
    char *summaryFileName = (char *)malloc(strlen(test_string) +
                    strlen(summaryExt) + 1);
    char *verboseFileName = (char *)malloc(strlen(test_string) +
                    strlen(verboseExt) + 1);
    sprintf(gnuplotDataFileName, "%s%s", test_string, gnuplotExt);
    sprintf(gnuplotTimesFileName, "%s%s", test_string, gnuplotTimesExt);
    sprintf(summaryFileName, "%s%s", test_string, summaryExt);
    sprintf(verboseFileName, "%s%s", test_string, verboseExt);
    *gnuplotDataFile = NULL;
    *gnuplotTimesFile = NULL;
    *summaryFile = NULL;
    *verboseSummaryFile = NULL;
    *gnuplotDataFile = fopen(gnuplotDataFileName, "w");
    *gnuplotTimesFile = fopen(gnuplotTimesFileName, "w");
    *summaryFile = fopen(summaryFileName, "w");
    *verboseSummaryFile = fopen(verboseFileName, "w");

    if (!(*gnuplotDataFile)) {
        printf("Cannot open file \"%s\" for writing.\n", gnuplotDataFileName);
        retval = 0;
    }

    if (!(*gnuplotTimesFile)) {
        printf("Cannot open file \"%s\" for writing.\n", gnuplotTimesFileName);
        retval = 0;
    }

    if (!(*summaryFile)) {
        printf("Cannot open file \"%s\" for writing.\n", summaryFileName);
        retval = 0;
    }

    if (!(*verboseSummaryFile)) {
        printf("Cannot open file \"%s\" for writing.\n", verboseFileName);
        retval = 0;
    }

    free(gnuplotDataFileName);
    free(gnuplotTimesFileName);
    free(summaryFileName);
    free(verboseFileName);
    return retval;
}


void
closeResultsFiles(FILE *gnuplotDataFile,
        FILE *gnuplotTimesFile,
        FILE *summaryFile,
        FILE *verboseSummaryFile) {
    if (gnuplotDataFile)
    { fclose(gnuplotDataFile); }

    if (gnuplotTimesFile)
    { fclose(gnuplotTimesFile); }

    if (summaryFile)
    { fclose(summaryFile); }

    if (verboseSummaryFile)
    { fclose(verboseSummaryFile); }

#ifdef MALLOCDEBUG
    printf("\n");
    printf("At program termination, mallocInfo reports:\n");
    mallocInfo();
#endif
    return;
}

void
addSummaryTitle(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName) {
    time_t theTime;
    int i, k;
    theTime = time(NULL);
    k = (int)strlen(funName);
    fprintf(summaryFile, "Test results for function %s\n", funName);
    fprintf(summaryFile, "--------------------------");

    for (i = 0; i < k; i++)
    { fprintf(summaryFile, "-"); }

    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Test started at %s\n", ctime(&theTime));

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "Test results for function %s\n", funName);
        fprintf(verboseSummaryFile, "--------------------------");

        for (i = 0; i < k; i++)
        { fprintf(verboseSummaryFile, "-"); }

        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Test started at %s\n", ctime(&theTime));
    }
}

void
addSummarySpecial(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL x, REAL y,
        REAL reldiff, REAL ulps,
        const char *expected,
        int *flagsOK, int checkInexact,
        unsigned int expectedFlags, unsigned int observedFlags,
        int expectedErrno, int observedErrno) {
    const char *observedErrnoString;
    int OK;
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for special argument x = %30.20e\n",
            funName, x);
    fprintf(summaryFile, "(x = %s)\n", real2hex(&x));
    fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
            "     at x  = %65.54e\n"
            "    %s = %40.29e\n",
            reldiff, ulps, x, funName, y);
    fprintf(summaryFile, "  expected =     %s\n", expected);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Testing %s for special argument x = %30.20e\n",
                funName, x);
        fprintf(verboseSummaryFile, "(x = %s)\n", real2hex(&x));
        fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
                "     at x  = %65.54e\n"
                "    %s = %40.29e\n",
                reldiff, ulps, x, funName, y);
        fprintf(verboseSummaryFile, "  expected =     %s\n", expected);
    }

    /* Compare the observed flags with expected flags */
    if (flagsOK) {
        /* If there's an output argument flagsOK to set, set it */
        OK = compare_status(expectedFlags, observedFlags,
                        checkInexact,
                        expectedErrno, observedErrno,
                        verbose,
                        summaryFile, verboseSummaryFile);
        *flagsOK = *flagsOK && OK;
    } else {
        /* Otherwise just print the flags that got raised */
        printf("Flags raised: ");
        show_status(observedFlags, stdout);
        fprintf(summaryFile, "Flags raised: ");
        show_status(observedFlags, summaryFile);

        if (observedErrno == EDOM)
        { observedErrnoString = "(EDOM)"; }
        else if (observedErrno == ERANGE)
        { observedErrnoString = "(ERANGE)"; }
        else
        { observedErrnoString = ""; }

        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);
        fprintf(summaryFile, "Observed errno: %d %s\n",
                observedErrno, observedErrnoString);

        if (verbose) {
            fprintf(verboseSummaryFile, "Flags raised: ");
            show_status(observedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    }
}

void
addSummarySpecialSincos(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL x, REAL s, REAL c,
        REAL reldiff, REAL ulps,
        const char *expectedSin,
        const char *expectedCos,
        int *flagsOK, int checkInexact,
        unsigned int expectedFlags, unsigned int observedFlags,
        int expectedErrno, int observedErrno) {
    const char *observedErrnoString;
    int OK;
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for special argument x = %30.20e\n",
            funName, x);
    fprintf(summaryFile, "(x = %s)\n", real2hex(&x));
    fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
            "     at x  = %65.54e\n"
            "    %s returned s = %40.29e\n"
            "            and returned c = %40.29e\n",
            reldiff, ulps, x, funName, s, c);
    fprintf(summaryFile, "  expected s =     %s\n", expectedSin);
    fprintf(summaryFile, "  expected c =     %s\n", expectedCos);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Testing %s for special argument x = %30.20e\n",
                funName, x);
        fprintf(verboseSummaryFile, "(x = %s)\n", real2hex(&x));
        fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
                "     at x  = %65.54e\n"
                "    %s returned s = %40.29e\n"
                "            and returned c = %40.29e\n",
                reldiff, ulps, x, funName, s, c);
        fprintf(verboseSummaryFile, "  expected s =     %s\n", expectedSin);
        fprintf(verboseSummaryFile, "  expected c =     %s\n", expectedCos);
    }

    /* Compare the observed flags with expected flags */
    if (flagsOK) {
        /* If there's an output argument flagsOK to set, set it */
        OK = compare_status(expectedFlags, observedFlags,
                        checkInexact,
                        expectedErrno, observedErrno,
                        verbose,
                        summaryFile, verboseSummaryFile);
        *flagsOK = *flagsOK && OK;
    } else {
        /* Otherwise just print the flags that got raised */
        printf("Flags raised: ");
        show_status(observedFlags, stdout);
        fprintf(summaryFile, "Flags raised: ");
        show_status(observedFlags, summaryFile);

        if (observedErrno == EDOM)
        { observedErrnoString = "(EDOM)"; }
        else if (observedErrno == ERANGE)
        { observedErrnoString = "(ERANGE)"; }
        else
        { observedErrnoString = ""; }

        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);
        fprintf(summaryFile, "Observed errno: %d %s\n",
                observedErrno, observedErrnoString);

        if (verbose) {
            fprintf(verboseSummaryFile, "Flags raised: ");
            show_status(observedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    }
}

void
addSummarySpecialCexp(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL x, REAL y, REAL z_re, REAL z_im,
        REAL reldiff, REAL ulps,
        const char *expectedRe,
        const char *expectedIm,
        int *flagsOK, int checkInexact,
        unsigned int expectedFlags, unsigned int observedFlags,
        int expectedErrno, int observedErrno) {
    const char *observedErrnoString;
    int OK;
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for special arguments\n"
            "x = %30.20e, y = %30.20e\n", funName, x, y);
    fprintf(summaryFile, "(x = %s)\n", real2hex(&x));
    fprintf(summaryFile, " , y = %s)\n", real2hex(&y));
    fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
            "     at x  = %65.54e\n"
            "    and y  = %65.54e\n"
            "    %s returned z_re = %40.29e\n"
            "            and returned z_im = %40.29e\n",
            reldiff, ulps, x, y, funName, z_re, z_im);
    fprintf(summaryFile, "  expected z_re =     %s\n", expectedRe);
    fprintf(summaryFile, "  expected z_im =     %s\n", expectedIm);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Testing %s for special arguments\n"
                "x = %30.20e, y = %30.20e\n", funName, x, y);
        fprintf(verboseSummaryFile, "(x = %s)\n", real2hex(&x));
        fprintf(verboseSummaryFile, " , y = %s)\n", real2hex(&y));
        fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
                "     at x  = %65.54e\n"
                "    and y  = %65.54e\n"
                "    %s returned z_re = %40.29e\n"
                "            and returned z_im = %40.29e\n",
                reldiff, ulps, x, y, funName, z_re, z_im);
        fprintf(verboseSummaryFile, "  expected z_re =     %s\n", expectedRe);
        fprintf(verboseSummaryFile, "  expected z_im =     %s\n", expectedIm);
    }

    /* Compare the observed flags with expected flags */
    if (flagsOK) {
        /* If there's an output argument flagsOK to set, set it */
        OK = compare_status(expectedFlags, observedFlags,
                        checkInexact,
                        expectedErrno, observedErrno,
                        verbose,
                        summaryFile, verboseSummaryFile);
        *flagsOK = *flagsOK && OK;
    } else {
        /* Otherwise just print the flags that got raised */
        printf("Flags raised: ");
        show_status(observedFlags, stdout);
        fprintf(summaryFile, "Flags raised: ");
        show_status(observedFlags, summaryFile);

        if (observedErrno == EDOM)
        { observedErrnoString = "(EDOM)"; }
        else if (observedErrno == ERANGE)
        { observedErrnoString = "(ERANGE)"; }
        else
        { observedErrnoString = ""; }

        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);
        fprintf(summaryFile, "Observed errno: %d %s\n",
                observedErrno, observedErrnoString);

        if (verbose) {
            fprintf(verboseSummaryFile, "Flags raised: ");
            show_status(observedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    }
}

void
addSummarySpecial2(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL x, REAL y, REAL z,
        REAL reldiff, REAL ulps,
        const char *expected,
        int *flagsOK, int checkInexact,
        unsigned int expectedFlags, unsigned int observedFlags,
        int expectedErrno, int observedErrno) {
    const char *observedErrnoString;
    int OK;
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for special arguments\n"
            "x = %30.20e, y = %30.20e\n", funName, x, y);
    fprintf(summaryFile, "(x = %s", real2hex(&x));
    fprintf(summaryFile, " , y = %s)\n", real2hex(&y));
    fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
            "     at x  = %65.54e\n"
            "    and y  = %65.54e\n"
            "    %s = %40.29e\n",
            reldiff, ulps, x, y, funName, z);
    fprintf(summaryFile, "  expected =     %s\n", expected);
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Testing %s for special arguments\n"
                "x = %30.20e, y = %30.20e\n", funName, x, y);
        fprintf(verboseSummaryFile, "(x = %s", real2hex(&x));
        fprintf(verboseSummaryFile, " , y = %s)\n", real2hex(&y));
        fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
                "     at x  = %65.54e\n"
                "    and y  = %65.54e\n"
                "    %s = %40.29e\n",
                reldiff, ulps, x, y, funName, z);
        fprintf(verboseSummaryFile, "  expected =     %s\n", expected);
        fflush(verboseSummaryFile);
    }

    /* Compare the observed flags with expected flags */
    if (flagsOK) {
        /* If there's an output argument flagsOK to set, set it */
        OK = compare_status(expectedFlags, observedFlags,
                        checkInexact,
                        expectedErrno, observedErrno,
                        verbose,
                        summaryFile, verboseSummaryFile);
        *flagsOK = *flagsOK && OK;
    } else {
        /* Otherwise just print the flags that got raised */
        printf("Flags raised: ");
        show_status(observedFlags, stdout);
        fprintf(summaryFile, "Flags raised: ");
        show_status(observedFlags, summaryFile);

        if (observedErrno == EDOM)
        { observedErrnoString = "(EDOM)"; }
        else if (observedErrno == ERANGE)
        { observedErrnoString = "(ERANGE)"; }
        else
        { observedErrnoString = ""; }

        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);

        if (verbose) {
            fprintf(verboseSummaryFile, "Flags raised: ");
            show_status(observedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    }
}

void
addSummarySpecial2a(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL x, int y, REAL z,
        REAL reldiff, REAL ulps,
        const char *expected,
        int *flagsOK, int checkInexact,
        unsigned int expectedFlags, unsigned int observedFlags,
        int expectedErrno, int observedErrno) {
    const char *observedErrnoString;
    int OK;
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for special arguments\n"
            "x = %30.20e, y = %30d\n", funName, x, y);
    fprintf(summaryFile, "(x = %s", real2hex(&x));
    fprintf(summaryFile, " , y = %08x)\n", y);
    fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
            "     at x  = %65.54e\n"
            "    and y  = %65d\n"
            "    %s = %40.29e\n",
            reldiff, ulps, x, y, funName, z);
    fprintf(summaryFile, "  expected =     %s\n", expected);
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Testing %s for special arguments\n"
                "x = %30.20e, y = %30d\n", funName, x, y);
        fprintf(verboseSummaryFile, "(x = %s", real2hex(&x));
        fprintf(verboseSummaryFile, " , y = %08x)\n", y);
        fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
                "     at x  = %65.54e\n"
                "    and y  = %65d\n"
                "    %s = %40.29e\n",
                reldiff, ulps, x, y, funName, z);
        fprintf(verboseSummaryFile, "  expected =     %s\n", expected);
        fflush(verboseSummaryFile);
    }

    /* Compare the observed flags with expected flags */
    if (flagsOK) {
        /* If there's an output argument flagsOK to set, set it */
        OK = compare_status(expectedFlags, observedFlags,
                        checkInexact,
                        expectedErrno, observedErrno,
                        verbose,
                        summaryFile, verboseSummaryFile);
        *flagsOK = *flagsOK && OK;
    } else {
        /* Otherwise just print the flags that got raised */
        printf("Flags raised: ");
        show_status(observedFlags, stdout);
        fprintf(summaryFile, "Flags raised: ");
        show_status(observedFlags, summaryFile);

        if (observedErrno == EDOM)
        { observedErrnoString = "(EDOM)"; }
        else if (observedErrno == ERANGE)
        { observedErrnoString = "(ERANGE)"; }
        else
        { observedErrnoString = ""; }

        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);

        if (verbose) {
            fprintf(verboseSummaryFile, "Flags raised: ");
            show_status(observedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    }
}

void
addSummarySpecial3(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL x, REAL y, REAL z, REAL w,
        REAL reldiff, REAL ulps,
        const char *expected,
        int *flagsOK, int checkInexact,
        unsigned int expectedFlags, unsigned int observedFlags,
        int expectedErrno, int observedErrno) {
    const char *observedErrnoString;
    int OK;
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for special arguments\n"
            "x = %30.20e, y = %30.20e, z = %30.20e\n", funName, x, y, z);
    fprintf(summaryFile, "(x = %s", real2hex(&x));
    fprintf(summaryFile, " , y = %s", real2hex(&y));
    fprintf(summaryFile, " , z = %s)\n", real2hex(&z));
    fprintf(summaryFile, "Relative difference = %g  (%g ulps)\n"
            "     at x  = %65.54e\n"
            "        y  = %65.54e\n"
            "    and z  = %65.54e\n"
            "    %s = %40.29e\n",
            reldiff, ulps, x, y, z, funName, w);
    fprintf(summaryFile, "  expected =     %s\n", expected);
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Testing %s for special arguments\n"
                "x = %30.20e, y = %30.20e, z = %30.20e\n", funName, x, y, z);
        fprintf(verboseSummaryFile, "(x = %s", real2hex(&x));
        fprintf(verboseSummaryFile, " , y = %s", real2hex(&y));
        fprintf(verboseSummaryFile, " , z = %s)\n", real2hex(&z));
        fprintf(verboseSummaryFile, "Relative difference = %g  (%g ulps)\n"
                "     at x  = %65.54e\n"
                "        y  = %65.54e\n"
                "    and z  = %65.54e\n"
                "    %s = %40.29e\n",
                reldiff, ulps, x, y, z, funName, w);
        fprintf(verboseSummaryFile, "  expected =     %s\n", expected);
        fflush(verboseSummaryFile);
    }

    /* Compare the observed flags with expected flags */
    if (flagsOK) {
        /* If there's an output argument to set, set it */
        OK = compare_status(expectedFlags, observedFlags,
                        checkInexact,
                        expectedErrno, observedErrno,
                        verbose,
                        summaryFile, verboseSummaryFile);
        *flagsOK = *flagsOK && OK;
    } else {
        /* Otherwise just print the flags that got raised */
        printf("Flags raised: ");
        show_status(observedFlags, stdout);
        fprintf(summaryFile, "Flags raised: ");
        show_status(observedFlags, summaryFile);

        if (observedErrno == EDOM)
        { observedErrnoString = "(EDOM)"; }
        else if (observedErrno == ERANGE)
        { observedErrnoString = "(ERANGE)"; }
        else
        { observedErrnoString = ""; }

        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);

        if (verbose) {
            fprintf(verboseSummaryFile, "Flags raised: ");
            show_status(observedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    }
}

void
addSummaryPiMultiples(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs,
        REAL x, REAL y,
        REAL maxreldiff, REAL maxulps,
        const char *expected) {
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for %d arguments near multiples of pi/4\n",
            funName, nArgs);
    fprintf(summaryFile, "Maximum relative difference = %g  (%g ulps)\n",
            maxreldiff, maxulps);

    if (maxreldiff > 0.0) {
        fprintf(summaryFile, "     at x  = %65.54e\n"
                "    %s = %40.29e\n",
                x, funName, y);
        fprintf(summaryFile, "  expected =     %s\n", expected);
    }

    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile,
                "Testing %s for %d arguments near multiples of pi/4\n",
                funName, nArgs);
        fprintf(verboseSummaryFile, "Maximum relative difference = %g  (%g ulps)\n",
                maxreldiff, maxulps);

        if (maxreldiff > 0.0) {
            fprintf(verboseSummaryFile, "     at x  = %65.54e\n"
                    "    %s = %40.29e\n",
                    x, funName, y);
            fprintf(verboseSummaryFile, "  expected =     %s\n", expected);
        }

        fflush(verboseSummaryFile);
    }
}

void
addSummaryPiMultiplesSincos(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs,
        REAL x, REAL s, REAL c,
        REAL maxreldiff, REAL maxulps,
        const char *expectedSin, const char *expectedCos) {
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Testing %s for %d arguments near multiples of pi/4\n",
            funName, nArgs);
    fprintf(summaryFile, "Maximum relative difference = %g  (%g ulps)\n",
            maxreldiff, maxulps);

    if (maxreldiff > 0.0) {
        fprintf(summaryFile, "     at x  = %65.54e\n"
                "    s = %40.29e\n"
                "    c = %40.29e\n",
                x, s, c);
        fprintf(summaryFile, "  expected s =     %s\n", expectedSin);
        fprintf(summaryFile, "  expected c =     %s\n", expectedCos);
    }

    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile,
                "Testing %s for %d arguments near multiples of pi/4\n",
                funName, nArgs);
        fprintf(verboseSummaryFile, "Maximum relative difference = %g  (%g ulps)\n",
                maxreldiff, maxulps);

        if (maxreldiff > 0.0) {
            fprintf(verboseSummaryFile, "     at x  = %65.54e\n"
                    "    s = %40.29e\n"
                    "    c = %40.29e\n",
                    x, s, c);
            fprintf(verboseSummaryFile, "  expected s =     %s\n", expectedSin);
            fprintf(verboseSummaryFile, "  expected c =     %s\n", expectedCos);
        }

        fflush(verboseSummaryFile);
    }
}

void
addSummaryNearIntegers(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs, int nLow,
        REAL x, REAL y,
        REAL maxreldiff, REAL maxulps,
        const char *expected) {
    fprintf(summaryFile, "\n");
    fprintf(summaryFile,
            "Testing %s for arguments near exact integers in range [%d,%d]\n",
            funName, nLow, nLow + nArgs);
    fprintf(summaryFile, "Maximum relative difference = %g  (%g ulps)\n",
            maxreldiff, maxulps);

    if (maxreldiff > 0.0) {
        fprintf(summaryFile, "     at x  = %65.54e\n"
                "    %s = %40.29e\n",
                x, funName, y);
        fprintf(summaryFile, "  expected =     %s\n", expected);
    }

    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile,
                "Testing %s for arguments near exact integers in range [%d,%d]\n",
                funName, nLow, nLow + nArgs);
        fprintf(verboseSummaryFile, "Maximum relative difference = %g  (%g ulps)\n",
                maxreldiff, maxulps);

        if (maxreldiff > 0.0) {
            fprintf(verboseSummaryFile, "     at x  = %65.54e\n"
                    "    %s = %40.29e\n",
                    x, funName, y);
            fprintf(verboseSummaryFile, "  expected =     %s\n", expected);
        }

        fflush(verboseSummaryFile);
    }
}

void
addSummaryInterval(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs,
        REAL a, REAL b,
        REAL maxreldiff, REAL maxulps,
        REAL xMax, REAL cycles) {
    static int first = 1;

    if (first) {
        first = 0;
        fprintf(summaryFile, "\n");
        fprintf(summaryFile, "Summary of errors in %s over all tested intervals:\n",
                funName);
        fprintf(summaryFile, "  nArgs                  Interval  Cycles  "
                " Rel.Err.   Ulp.Err.     Max.Rel.Err.Argument\n");
        fprintf(summaryFile, "-------  ------------------------  ------  "
                "---------  ---------  -----------------------\n");

        if (verbose > 0) {
            fprintf(verboseSummaryFile, "\n");
            fprintf(verboseSummaryFile,
                    "Summary of errors in %s over all tested intervals:\n", funName);
            fprintf(verboseSummaryFile, "  nArgs                  Interval  Cycles  "
                    " Rel.Err.   Ulp.Err.     Max.Rel.Err.Argument\n");
            fprintf(verboseSummaryFile, "-------  ------------------------  ------  "
                    "---------  ---------  -----------------------\n");
        }
    }

    fprintf(summaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
            nArgs, a, b, cycles, maxreldiff, maxulps);

    if (maxulps > 0.0)
    { fprintf(summaryFile, " %24.18g", xMax); }

    fprintf(summaryFile, "\n");
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
                nArgs, a, b, cycles, maxreldiff, maxulps);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, " %24.18g", xMax); }

        fprintf(verboseSummaryFile, "\n");
        fflush(verboseSummaryFile);
    }
}

void
addSummaryInterval2(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs,
        REAL a, REAL b, REAL c, REAL d,
        REAL maxreldiff, REAL maxulps,
        REAL xMax, REAL yMax, REAL cycles) {
    static int first = 1;

    if (first) {
        first = 0;
        fprintf(summaryFile, "\n");
        fprintf(summaryFile, "Summary of errors in %s over all tested intervals:\n",
                funName);
        fprintf(summaryFile, "  nArgs     Intervals (x above y)  Cycles  "
                " Rel.Err.   Ulp.Err.    Max.Rel.Err.Arguments\n");
        fprintf(summaryFile, "-------  ------------------------  ------  "
                "---------  ---------  -----------------------\n");

        if (verbose > 0) {
            fprintf(verboseSummaryFile, "\n");
            fprintf(verboseSummaryFile,
                    "Summary of errors in %s over all tested intervals:\n", funName);
            fprintf(verboseSummaryFile, "  nArgs     Intervals (x above y)  Cycles  "
                    " Rel.Err.   Ulp.Err.    Max.Rel.Err.Arguments\n");
            fprintf(verboseSummaryFile, "-------  ------------------------  ------  "
                    "---------  ---------  -----------------------\n");
        }
    }

    fprintf(summaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
            nArgs, a, b, cycles, maxreldiff, maxulps);

    if (maxulps > 0.0)
    { fprintf(summaryFile, " %24.18g", xMax); }

    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "         [%10.4g, %10.4g]", c, d);

    if (maxulps > 0.0)
    { fprintf(summaryFile, "%54.18g", yMax); }

    fprintf(summaryFile, "\n");
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
                nArgs, a, b, cycles, maxreldiff, maxulps);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, " %24.18g", xMax); }

        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "         [%10.4g, %10.4g]", c, d);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, "%66.18g", yMax); }

        fprintf(verboseSummaryFile, "\n");
        fflush(verboseSummaryFile);
    }
}

void
addSummaryInterval2a(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs,
        REAL a, REAL b, int c, int d,
        REAL maxreldiff, REAL maxulps,
        REAL xMax, int yMax, REAL cycles) {
    static int first = 1;

    if (first) {
        first = 0;
        fprintf(summaryFile, "\n");
        fprintf(summaryFile, "Summary of errors in %s over all tested intervals:\n",
                funName);
        fprintf(summaryFile, "  nArgs     Intervals (x above y)  Cycles  "
                " Rel.Err.   Ulp.Err.    Max.Rel.Err.Arguments\n");
        fprintf(summaryFile, "-------  ------------------------  ------  "
                "---------  ---------  -----------------------\n");

        if (verbose > 0) {
            fprintf(verboseSummaryFile, "\n");
            fprintf(verboseSummaryFile,
                    "Summary of errors in %s over all tested intervals:\n", funName);
            fprintf(verboseSummaryFile, "  nArgs     Intervals (x above y)  Cycles  "
                    " Rel.Err.   Ulp.Err.    Max.Rel.Err.Arguments\n");
            fprintf(verboseSummaryFile, "-------  ------------------------  ------  "
                    "---------  ---------  -----------------------\n");
        }
    }

    fprintf(summaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
            nArgs, a, b, cycles, maxreldiff, maxulps);

    if (maxulps > 0.0)
    { fprintf(summaryFile, " %24.18g", xMax); }

    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "         [%10d, %10d]", c, d);

    if (maxulps > 0.0)
    { fprintf(summaryFile, "%54d", yMax); }

    fprintf(summaryFile, "\n");
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
                nArgs, a, b, cycles, maxreldiff, maxulps);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, " %24.18g", xMax); }

        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "         [%10d, %10d]", c, d);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, "%66d", yMax); }

        fprintf(verboseSummaryFile, "\n");
        fflush(verboseSummaryFile);
    }
}

void
addSummaryInterval3(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        int nArgs,
        REAL a, REAL b, REAL c, REAL d,
        REAL e, REAL f,
        REAL maxreldiff, REAL maxulps,
        REAL xMax, REAL yMax, REAL zMax,
        REAL cycles) {
    static int first = 1;

    if (first) {
        first = 0;
        fprintf(summaryFile, "\n");
        fprintf(summaryFile, "Summary of errors in %s over all tested intervals:\n",
                funName);
        fprintf(summaryFile, "  nArgs     Intervals (x abv y,z)  Cycles  "
                " Rel.Err.   Ulp.Err.    Max.Rel.Err.Arguments\n");
        fprintf(summaryFile, "-------  ------------------------  ------  "
                "---------  ---------  -----------------------\n");

        if (verbose > 0) {
            fprintf(verboseSummaryFile, "\n");
            fprintf(verboseSummaryFile,
                    "Summary of errors in %s over all tested intervals:\n", funName);
            fprintf(verboseSummaryFile, "  nArgs     Intervals (x abv y,z)  Cycles  "
                    " Rel.Err.   Ulp.Err.    Max.Rel.Err.Arguments\n");
            fprintf(verboseSummaryFile, "-------  ------------------------  ------  "
                    "---------  ---------  -----------------------\n");
        }
    }

    fprintf(summaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
            nArgs, a, b, cycles, maxreldiff, maxulps);

    if (maxulps > 0.0)
    { fprintf(summaryFile, " %24.18g", xMax); }

    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "         [%10.4g, %10.4g]", c, d);

    if (maxulps > 0.0)
    { fprintf(summaryFile, "%54.18g", yMax); }

    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "         [%10.4g, %10.4g]", e, f);

    if (maxulps > 0.0)
    { fprintf(summaryFile, "%54.18g", zMax); }

    fprintf(summaryFile, "\n");
    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "%7d  [%10.4g, %10.4g] %6.0f %10.4g %10.4g",
                nArgs, a, b, cycles, maxreldiff, maxulps);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, " %24.18g", xMax); }

        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "         [%10.4g, %10.4g]", c, d);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, "%66.18g", yMax); }

        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "         [%10.4g, %10.4g]", e, f);

        if (maxulps > 0.0)
        { fprintf(verboseSummaryFile, "%66.18g", zMax); }

        fprintf(verboseSummaryFile, "\n");
        fflush(verboseSummaryFile);
    }
}

void
addSummaryConclusion(FILE *summaryFile,
        int verbose,
        FILE *verboseSummaryFile,
        const char *funName,
        REAL maxRelErr, REAL maxUlpErr,
        REAL minTime, REAL maxTime,
        REAL maxTimeNormal, int flagsOK) {
    time_t theTime;
    printf("\n");
    printf("Test of %s concluded.\n", funName);
    printf("Maximum error over all arguments tested was "
            "%10.4g ulps (%5.4g Rel Err)\n", maxUlpErr, maxRelErr);
    printf("Speed in cycles ranged from %6.0f to %6.0f\n", minTime, maxTime);
    printf("Maximum cycles for normal arguments was %6.0f\n", maxTimeNormal);

    if (flagsOK)
    { printf("Exception flags set by %s seem OK.\n", funName); }
    else
    { printf("Warning: exception flags set by %s were not always as expected.\n", funName); }

    theTime = time(NULL);
    fprintf(summaryFile, "\n");
    fprintf(summaryFile, "Test of %s concluded at %s", funName, ctime(&theTime));
    fprintf(summaryFile, "Maximum error over all arguments tested was "
            "%10.4g ulps (%5.4g Rel Err)\n", maxUlpErr, maxRelErr);
    fprintf(summaryFile, "Speed in cycles ranged from %6.0f to %6.0f\n",
            minTime, maxTime);
    fprintf(summaryFile, "Maximum cycles for normal arguments was %6.0f\n",
            maxTimeNormal);

    if (flagsOK)
        fprintf(summaryFile,
                "Exception flags set by %s seem OK.\n", funName);
    else
        fprintf(summaryFile,
                "Warning: exception flags set by %s were not always as expected.\n", funName);

    fflush(summaryFile);

    if (verbose > 0) {
        fprintf(verboseSummaryFile, "\n");
        fprintf(verboseSummaryFile, "Test of %s concluded at %s", funName,
                ctime(&theTime));
        fprintf(verboseSummaryFile, "Maximum error over all arguments tested was "
                "%10.4g ulps (%5.4g Rel Err)\n", maxUlpErr, maxRelErr);
        fprintf(verboseSummaryFile, "Speed in cycles ranged from %6.0f to %6.0f\n",
                minTime, maxTime);
        fprintf(verboseSummaryFile, "Maximum cycles for normal arguments was %6.0f\n",
                maxTimeNormal);

        if (flagsOK)
            fprintf(verboseSummaryFile,
                    "Exception flags set by %s seem OK.\n", funName);
        else
            fprintf(verboseSummaryFile,
                    "Warning: exception flags set by %s were not always as expected.\n", funName);

        fflush(verboseSummaryFile);
    }
}

#if defined(WINDOWS)
    #define STRNCASECMP strnicmp
#else
    #define STRNCASECMP strncasecmp
#endif

void
parseArgs(int argc, char *argv[], int *nTests, int *nTimes, int *amean,
        int *specialTest, int *forcePrecision) {
    int i, nr, nt;
    *nTests = 1000;
    *nTimes = 1000;
    *specialTest = 0;
    *amean = 0;
    /* Default value for forcePrecision is 0 (don't force
       register arithmetic to be rounded to double precision)
       unless we're running a test of a double amd function
       in which case we default to 1. */
    *forcePrecision = 0;
#if ISDOUBLE

    if (strstr(argv[0], "amd32"))
    { *forcePrecision = 1; }

#endif

    for (i = 1; i < argc; i++) {
        if (!STRNCASECMP(argv[i], "-ntest", 6) && i < argc - 1) {
            if (i < argc - 1) {
                nr = sscanf(argv[i + 1], "%d", &nt);

                if (nr == 1)
                { *nTests = nt; }
            } else
                /* Looks like argument to -ntest was missing */
            { *nTests = 0; }
        }

        if (!STRNCASECMP(argv[i], "-ntime", 6)) {
            if (i < argc - 1) {
                nr = sscanf(argv[i + 1], "%d", &nt);

                if (nr == 1)
                { *nTimes = nt; }
            } else
                /* Looks like argument to -ntime was missing */
            { *nTimes = 0; }
        }

        if (!STRNCASECMP(argv[i], "-special", 8))
        { *specialTest = 1; }

        if (!STRNCASECMP(argv[i], "-amean", 6))
        { *amean = 1; }

        if (!STRNCASECMP(argv[i], "-force", 6))
        { *forcePrecision = 1; }
    }
}

int
getInt(const char *buff) {
    int nr, v;
    nr = sscanf(buff, "%d", &v);

    if (nr == 0)
    { return 0; }
    else
    { return v; }
}

REAL
getReal(const char *buff, int base, int mantis, int emin, int emax) {
    int i, nr;
    REAL r;

    if (!STRNCASECMP(buff, "nan", 3) || !STRNCASECMP(buff, "-nan", 4) ||
            !STRNCASECMP(buff, "qnan", 4) || !STRNCASECMP(buff, "-qnan", 5)) {
        if (buff[0] == '-')
        { createNaN(1, 0, &r); }
        else
        { createNaN(0, 0, &r); }
    } else if (!STRNCASECMP(buff, "snan", 4) || !STRNCASECMP(buff, "-snan", 5)) {
        if (buff[0] == '-')
        { createNaN(1, 1, &r); }
        else
        { createNaN(0, 1, &r); }
    } else if (!STRNCASECMP(buff, "inf", 3) ||
            !STRNCASECMP(&buff[1], "inf", 3)) {
        if (buff[0] == '-')
        { createInfinity(1, &r); }
        else
        { createInfinity(0, &r); }
    } else if (!STRNCASECMP(buff, "zero", 4) ||
            !STRNCASECMP(&buff[1], "zero", 4)) {
        if (buff[0] == '-')
        { createZero(1, &r); }
        else
        { createZero(0, &r); }
    } else if (!STRNCASECMP(buff, "small", 5) ||
            !STRNCASECMP(&buff[1], "small", 5)) {
        if (buff[0] == '-')
        { createSmallestNormal(1, emin, &r); }
        else
        { createSmallestNormal(0, emin, &r); }
    } else if (!STRNCASECMP(buff, "tiny", 4) ||
            !STRNCASECMP(&buff[1], "tiny", 4)) {
        if (buff[0] == '-')
        { createSmallestDenormal(1, mantis, emin, &r); }
        else
        { createSmallestDenormal(0, mantis, emin, &r); }
    } else if (!STRNCASECMP(buff, "large", 5) ||
            !STRNCASECMP(&buff[1], "large", 5)) {
        if (buff[0] == '-')
        { createLargest(1, mantis, emax, &r); }
        else
        { createLargest(0, mantis, emax, &r); }
    } else if (!STRNCASECMP(buff, "eps", 3) ||
            !STRNCASECMP(&buff[1], "eps", 3)) {
        r = 1.0;

        for (i = 0; i < mantis; i++)
        { r /= base; }

        if (buff[0] == '-')
        { r = -r; }
    } else if (!STRNCASECMP(buff, "receps", 6) ||
            !STRNCASECMP(&buff[1], "receps", 6)) {
        r = 1.0;

        for (i = 0; i < mantis; i++)
        { r *= base; }

        if (buff[0] == '-')
        { r = -r; }
    } else {
#if ISDOUBLE
        nr = sscanf(buff, "%lf", &r);
#else
        nr = sscanf(buff, "%f", &r);
#endif

        if (nr == 0)
        { r = 0.0; }
    }

    return r;
}
#undef STRNCASECMP

int
get2Reals(int base, int mantis, int emin, int emax,
        REAL *a, REAL *b) {
    char readbuff[200], sa[100], sb[100];
    int nr;
    *a = 0.0;
    *b = 0.0;

    if (feof(stdin)) {
        *a = -111.0;
        *b = -111.0;
        return 0;
    }

    fgets(readbuff, 200, stdin);
    nr = sscanf(readbuff, "%s %s", sa, sb);

    if (nr > 0)
    { *a = getReal(sa, base, mantis, emin, emax); }

    if (nr > 1)
    { *b = getReal(sb, base, mantis, emin, emax); }

    return nr;
}

int
getRealsInts(int base, int mantis, int emin, int emax,
        REAL *a, REAL *b, int *c, int *d) {
    char readbuff[400], sa[100], sb[100], sc[100], sd[100];
    int nr;
    *a = 0.0;
    *b = 0.0;
    *c = 0;
    *d = 0;
    fgets(readbuff, 400, stdin);
    nr = sscanf(readbuff, "%s %s %s %s", sa, sb, sc, sd);

    if (nr == 2) {
        *a = getReal(sa, base, mantis, emin, emax);
        *c = getInt(sb);
    }

    if (nr == 4) {
        *a = getReal(sa, base, mantis, emin, emax);
        *b = getReal(sb, base, mantis, emin, emax);
        *c = getInt(sc);
        *d = getInt(sd);
    }

    return nr;
}

int
get4Reals(int base, int mantis, int emin, int emax,
        REAL *a, REAL *b, REAL *c, REAL *d) {
    char readbuff[400], sa[100], sb[100], sc[100], sd[100];
    int nr;
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    fgets(readbuff, 400, stdin);
    nr = sscanf(readbuff, "%s %s %s %s", sa, sb, sc, sd);

    if (nr > 0)
    { *a = getReal(sa, base, mantis, emin, emax); }

    if (nr > 1)
    { *b = getReal(sb, base, mantis, emin, emax); }

    if (nr > 2)
    { *c = getReal(sc, base, mantis, emin, emax); }

    if (nr > 3)
    { *d = getReal(sd, base, mantis, emin, emax); }

    return nr;
}

int
get6Reals(int base, int mantis, int emin, int emax,
        REAL *a, REAL *b, REAL *c, REAL *d, REAL *e, REAL *f) {
    char readbuff[400], sa[100], sb[100], sc[100], sd[100], se[100], sf[100];
    int nr;
    *a = 0.0;
    *b = 0.0;
    *c = 0.0;
    *d = 0.0;
    *e = 0.0;
    *f = 0.0;
    fgets(readbuff, 400, stdin);
    nr = sscanf(readbuff, "%s %s %s %s %s %s", sa, sb, sc, sd, se, sf);

    if (nr > 0)
    { *a = getReal(sa, base, mantis, emin, emax); }

    if (nr > 1)
    { *b = getReal(sb, base, mantis, emin, emax); }

    if (nr > 2)
    { *c = getReal(sc, base, mantis, emin, emax); }

    if (nr > 3)
    { *d = getReal(sd, base, mantis, emin, emax); }

    if (nr > 4)
    { *e = getReal(se, base, mantis, emin, emax); }

    if (nr > 5)
    { *f = getReal(sf, base, mantis, emin, emax); }

    return nr;
}

/* Compares expected status flags with obtained status flags.
   Returns 1 on success, 0 on failure. */
int
compare_status(unsigned int expectedFlags, unsigned int observedFlags,
        int checkInexact,
        int expectedErrno, int observedErrno, int verbose,
        FILE *summaryFile, FILE *verboseSummaryFile) {
    int flagsOK = 1;
    const char *observedErrnoString, *expectedErrnoString;
    flagsOK = flagsOK &&
            sw_divbyzero_raised(observedFlags) == sw_divbyzero_raised(expectedFlags);
    flagsOK = flagsOK &&
            sw_underflow_raised(observedFlags) == sw_underflow_raised(expectedFlags);
    flagsOK = flagsOK &&
            sw_overflow_raised(observedFlags) == sw_overflow_raised(expectedFlags);
    flagsOK = flagsOK &&
            sw_invalid_raised(observedFlags) == sw_invalid_raised(expectedFlags);

    if (checkInexact)
        flagsOK = flagsOK &&
                sw_inexact_raised(observedFlags) == sw_inexact_raised(expectedFlags);

    if (!flagsOK) {
        printf("Warning: flags raised were not as expected:\n");
        printf("Expected ");
        show_status(expectedFlags, stdout);
        printf("Observed ");
        show_status(observedFlags, stdout);

        if (summaryFile) {
            fprintf(summaryFile, "Warning: flags raised were not as expected:\n");
            fprintf(summaryFile, "Expected ");
            show_status(expectedFlags, summaryFile);
            fprintf(summaryFile, "Observed ");
            show_status(observedFlags, summaryFile);
        }

        if (verbose && verboseSummaryFile) {
            fprintf(verboseSummaryFile, "Warning: flags raised were not as expected:\n");
            fprintf(verboseSummaryFile, "Expected ");
            show_status(expectedFlags, verboseSummaryFile);
            fprintf(verboseSummaryFile, "Observed ");
            show_status(observedFlags, verboseSummaryFile);
        }
    } else {
        show_status(observedFlags, stdout);
        show_status(observedFlags, summaryFile);
        show_status(observedFlags, verboseSummaryFile);
    }

    if (observedErrno == EDOM)
    { observedErrnoString = "(EDOM)"; }
    else if (observedErrno == ERANGE)
    { observedErrnoString = "(ERANGE)"; }
    else
    { observedErrnoString = ""; }

    if (expectedErrno == EDOM)
    { expectedErrnoString = "(EDOM)"; }
    else if (expectedErrno == ERANGE)
    { expectedErrnoString = "(ERANGE)"; }
    else
    { expectedErrnoString = ""; }

    if (observedErrno != expectedErrno) {
        flagsOK = 0;
        printf("Warning: errno set was not as expected:\n");
        printf("Expected errno: %d %s\n", expectedErrno, expectedErrnoString);
        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);

        if (summaryFile) {
            fprintf(summaryFile, "Warning: errno set was not as expected:\n");
            fprintf(summaryFile, "Expected errno: %d %s\n",
                    expectedErrno, expectedErrnoString);
            fprintf(summaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }

        if (verboseSummaryFile) {
            fprintf(verboseSummaryFile, "Warning: errno set was not as expected:\n");
            fprintf(verboseSummaryFile, "Expected errno: %d %s\n",
                    expectedErrno, expectedErrnoString);
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
        }
    } else {
        printf("Observed errno: %d %s\n", observedErrno, observedErrnoString);

        if (summaryFile)
            fprintf(summaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);

        if (verboseSummaryFile)
            fprintf(verboseSummaryFile, "Observed errno: %d %s\n",
                    observedErrno, observedErrnoString);
    }

    return flagsOK;
}

#define OLD_BITS_SP32(x) (*((unsigned int *)&x))
#ifdef IS_64BIT
    #ifdef WINDOWS
        #define OLD_BITS_DP64(x) (*((unsigned long long *)&x))
    #else
        #define OLD_BITS_DP64(x) (*((unsigned long *)&x))
    #endif
#else /* ! IS_64BIT */
    #if 1
        #define OLD_HIGH_HALF(x) (*(((unsigned int *)&x) + 1))
        #define OLD_LOW_HALF(x) (*((unsigned int *)&x))
    #else
        #define OLD_HIGH_HALF(x) (*(((unsigned int *)&x)))
        #define OLD_LOW_HALF(x) (*((unsigned int *)&x + 1))
    #endif
#endif

/* Returns a character string containing a hex version of REAL x */
char *
real2hex(REAL *x) {
    static char buff[19];
#if ISDOUBLE
#ifdef IS_64BIT
#ifdef WINDOWS
    unsigned long long ux;
    unsigned int uhx, ulx;
    ux = OLD_BITS_DP64(*x);
    ulx = (int)(ux % 4294967296LL);
    uhx = (int)(ux >> 32);
    sprintf(buff, "0x%08x%08x", uhx, ulx);
#else
    unsigned long ux;
    ux = OLD_BITS_DP64(*x);
    sprintf(buff, "0x%016lx", ux);
#endif
#else
    unsigned int uhx, ulx;
    uhx = OLD_HIGH_HALF(*x);
    ulx = OLD_LOW_HALF(*x);
    sprintf(buff, "0x%08x%08x", uhx, ulx);
#endif
#else /* ! ISDOUBLE */
    unsigned int ux;
    ux = OLD_BITS_SP32(*x);
    sprintf(buff, "0x%08x", ux);
#endif
    return buff;
}
