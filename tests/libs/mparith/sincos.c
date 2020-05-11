#include "precision.h"
#include "mparith_c.h"

#if defined(FLOAT)
#define FUNC_SINCOS alm_mp_sincosf
#define FUNC_SINCOS_ULP alm_mp_sincosf_ULP
#elif defined(DOUBLE)
#define FUNC_SINCOS alm_mp_sincos
#define FUNC_SINCOS_ULP alm_mp_sincos_ULP
#else
#error
#endif

void FUNC_SINCOS(REAL x, REAL *s, REAL *c)
{
    REAL ys, yc;
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTAN(xmp, params, result_sin, result_cos, result_tan, &ifail);


    MPTOD(result_sin, params, &ys, &ifail);
    MPTOD(result_cos, params, &yc, &ifail);

    *s = ys;
    *c = yc;

    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);
}

void FUNC_SINCOS_ULP(REAL x, REAL s, REAL c,double   *sulps, double   *sreldiff)
{
    fp_params params;
    int base, mantis, emin, emax;
    int *xmp, *result_sin,*result_cos,*result_tan;
    int ifail;
	REAL reldiff,ulps;
    REAL rel_1,ulp_1;

    initMultiPrecision(ISDOUBLE, 0, &base, &mantis, &emin, &emax, &params);
    xmp = new_mp(params);
    result_sin = new_mp(params);
    result_cos = new_mp(params);
    result_tan = new_mp(params);

    DTOMP(x, xmp, params, &ifail);
    MPSINCOSTAN(xmp, params, result_sin, result_cos, result_tan, &ifail);

   reldiff = MPRELDIFF(s, base, mantis, emin, emax,
                      result_sin, params,&ulps, &ifail);

   rel_1 = MPRELDIFF(c, base, mantis, emin, emax,
                      result_cos, params,&ulp_1, &ifail);

    *sreldiff =  reldiff > rel_1? reldiff : rel_1;
    *sulps =  ulps > ulp_1? ulps : ulp_1;




    free(xmp);
    free(result_sin);
    free(result_cos);
    free(result_tan);
}
