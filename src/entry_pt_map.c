#include <fn_macros.h>
#include <libm/types.h>
#include <libm/entry_pt.h>
#include "entry_pt_macros.h"


LIBM_DECL_FN_MAP(atan2f);
LIBM_DECL_FN_MAP(atan2);

LIBM_DECL_FN_MAP(atan);
LIBM_DECL_FN_MAP(atanf);

LIBM_DECL_FN_MAP(cbrtf);
LIBM_DECL_FN_MAP(cbrt);

LIBM_DECL_FN_MAP(cosf);
LIBM_DECL_FN_MAP(cos);

LIBM_DECL_FN_MAP(coshf);
LIBM_DECL_FN_MAP(cosh);

LIBM_DECL_FN_MAP(exp10f);
LIBM_DECL_FN_MAP(exp10);

LIBM_DECL_FN_MAP(exp2f);
LIBM_DECL_FN_MAP(exp2);

LIBM_DECL_FN_MAP(expf);
LIBM_DECL_FN_MAP(exp);

LIBM_DECL_FN_MAP(expm1f);
LIBM_DECL_FN_MAP(expm1);

LIBM_DECL_FN_MAP(fmaf);
LIBM_DECL_FN_MAP(fma);

LIBM_DECL_FN_MAP(log10f);
LIBM_DECL_FN_MAP(log10);

LIBM_DECL_FN_MAP(log1pf);
LIBM_DECL_FN_MAP(log1p);

LIBM_DECL_FN_MAP(log2f);
LIBM_DECL_FN_MAP(log2);

LIBM_DECL_FN_MAP(logf);
LIBM_DECL_FN_MAP(log);

LIBM_DECL_FN_MAP(lroundf);
LIBM_DECL_FN_MAP(lround);

LIBM_DECL_FN_MAP(powf);
LIBM_DECL_FN_MAP(pow);

LIBM_DECL_FN_MAP(remquof);
LIBM_DECL_FN_MAP(remquo);

LIBM_DECL_FN_MAP(sincosf);
LIBM_DECL_FN_MAP(sincos);

LIBM_DECL_FN_MAP(sinf);
LIBM_DECL_FN_MAP(sin);

LIBM_DECL_FN_MAP(tanf);
LIBM_DECL_FN_MAP(tan);

LIBM_DECL_FN_MAP(vrd2_cbrt);
LIBM_DECL_FN_MAP(vrd2_cos);
LIBM_DECL_FN_MAP(vrd2_exp10);
LIBM_DECL_FN_MAP(vrd2_exp2);
LIBM_DECL_FN_MAP(vrd2_exp);
LIBM_DECL_FN_MAP(vrd2_expm1);
LIBM_DECL_FN_MAP(vrd2_log10);
LIBM_DECL_FN_MAP(vrd2_log1p);
LIBM_DECL_FN_MAP(vrd2_log2);
LIBM_DECL_FN_MAP(vrd2_log);
LIBM_DECL_FN_MAP(vrd2_pow);
LIBM_DECL_FN_MAP(vrd2_sin);

LIBM_DECL_FN_MAP(vrd4_exp2);
LIBM_DECL_FN_MAP(vrd4_exp);
LIBM_DECL_FN_MAP(vrd4_expm1);
LIBM_DECL_FN_MAP(vrd4_log);
LIBM_DECL_FN_MAP(vrd4_pow);

LIBM_DECL_FN_MAP(vrs4_cbrtf);
LIBM_DECL_FN_MAP(vrs4_cosf);
LIBM_DECL_FN_MAP(vrs4_exp10f);
LIBM_DECL_FN_MAP(vrs4_exp2f);
LIBM_DECL_FN_MAP(vrs4_expf);
LIBM_DECL_FN_MAP(vrs4_expm1f);
LIBM_DECL_FN_MAP(vrs4_log10f);
LIBM_DECL_FN_MAP(vrs4_log1pf);
LIBM_DECL_FN_MAP(vrs4_log2f);
LIBM_DECL_FN_MAP(vrs4_logf);
LIBM_DECL_FN_MAP(vrs4_sinf);

LIBM_DECL_FN_MAP(vrda_cos);
LIBM_DECL_FN_MAP(vrda_exp);
LIBM_DECL_FN_MAP(vrda_exp2);
LIBM_DECL_FN_MAP(vrda_expm1);
LIBM_DECL_FN_MAP(vrda_log10);
LIBM_DECL_FN_MAP(vrda_log2);
LIBM_DECL_FN_MAP(vrda_log);
LIBM_DECL_FN_MAP(vrda_pow);
LIBM_DECL_FN_MAP(vrda_sin);

LIBM_DECL_FN_MAP(vrsa_cosf);
LIBM_DECL_FN_MAP(vrsa_expf);
LIBM_DECL_FN_MAP(vrsa_exp2f);
LIBM_DECL_FN_MAP(vrsa_log10f);
LIBM_DECL_FN_MAP(vrsa_log2f);
LIBM_DECL_FN_MAP(vrsa_logf);
LIBM_DECL_FN_MAP(vrsa_sinf);


WEAK_LIBM_ALIAS(__vrd2_cbrt, FN_PROTOTYPE(vrd2_cbrt));
WEAK_LIBM_ALIAS(__vrd2_cos, FN_PROTOTYPE(vrd2_cos));
WEAK_LIBM_ALIAS(__vrd2_exp10, FN_PROTOTYPE(vrd2_exp10));
WEAK_LIBM_ALIAS(__vrd2_exp2, FN_PROTOTYPE(vrd2_exp2));
WEAK_LIBM_ALIAS(__vrd2_exp, FN_PROTOTYPE(vrd2_exp));
WEAK_LIBM_ALIAS(__vrd2_expm1, FN_PROTOTYPE(vrd2_expm1));
WEAK_LIBM_ALIAS(__vrd2_log10, FN_PROTOTYPE(vrd2_log10));
WEAK_LIBM_ALIAS(__vrd2_log1p, FN_PROTOTYPE(vrd2_log1p));
WEAK_LIBM_ALIAS(__vrd2_log2, FN_PROTOTYPE(vrd2_log2));
WEAK_LIBM_ALIAS(__vrd2_log, FN_PROTOTYPE(vrd2_log));
WEAK_LIBM_ALIAS(__vrd2_sin, FN_PROTOTYPE(vrd2_sin));

WEAK_LIBM_ALIAS(__vrd4_exp, FN_PROTOTYPE(vrd4_exp));
WEAK_LIBM_ALIAS(__vrd4_exp2, FN_PROTOTYPE(vrd4_exp2));
WEAK_LIBM_ALIAS(__vrd4_log, FN_PROTOTYPE(vrd4_log));

WEAK_LIBM_ALIAS(__vrs4_cbrtf, FN_PROTOTYPE(vrs4_cbrtf));
WEAK_LIBM_ALIAS(__vrs4_cosf, FN_PROTOTYPE(vrs4_cosf));
WEAK_LIBM_ALIAS(__vrs4_cosf, FN_PROTOTYPE(vrs4_cosf));
WEAK_LIBM_ALIAS(__vrs4_exp10f, FN_PROTOTYPE(vrs4_exp10f));
WEAK_LIBM_ALIAS(__vrs4_exp2f, FN_PROTOTYPE(vrs4_exp2f));
WEAK_LIBM_ALIAS(__vrs4_expf, FN_PROTOTYPE(vrs4_expf));
WEAK_LIBM_ALIAS(__vrs4_expf, FN_PROTOTYPE(vrs4_expf));
WEAK_LIBM_ALIAS(__vrs4_expm1f, FN_PROTOTYPE(vrs4_expm1f));
WEAK_LIBM_ALIAS(__vrs4_log10f, FN_PROTOTYPE(vrs4_log10f));
WEAK_LIBM_ALIAS(__vrs4_log10f, FN_PROTOTYPE(vrs4_log10f));
WEAK_LIBM_ALIAS(__vrs4_log1pf, FN_PROTOTYPE(vrs4_log1pf));
WEAK_LIBM_ALIAS(__vrs4_log2f, FN_PROTOTYPE(vrs4_log2f));
WEAK_LIBM_ALIAS(__vrs4_log2f, FN_PROTOTYPE(vrs4_log2f));
WEAK_LIBM_ALIAS(__vrs4_logf, FN_PROTOTYPE(vrs4_logf));
WEAK_LIBM_ALIAS(__vrs4_logf, FN_PROTOTYPE(vrs4_logf));
WEAK_LIBM_ALIAS(__vrs4_sinf, FN_PROTOTYPE(vrs4_sinf));
WEAK_LIBM_ALIAS(__vrs4_sinf, FN_PROTOTYPE(vrs4_sinf));


WEAK_LIBM_ALIAS(vrda_cos, FN_PROTOTYPE(vrda_cos));
WEAK_LIBM_ALIAS(vrda_exp, FN_PROTOTYPE(vrda_exp));
WEAK_LIBM_ALIAS(vrda_exp2, FN_PROTOTYPE(vrda_exp2));
WEAK_LIBM_ALIAS(vrda_expm1, FN_PROTOTYPE(vrda_expm1));
WEAK_LIBM_ALIAS(vrda_log10, FN_PROTOTYPE(vrda_log10));
WEAK_LIBM_ALIAS(vrda_log2, FN_PROTOTYPE(vrda_log2));
WEAK_LIBM_ALIAS(vrda_sin, FN_PROTOTYPE(vrda_sin));


WEAK_LIBM_ALIAS(vrsa_cosf, FN_PROTOTYPE(vrsa_cosf));
WEAK_LIBM_ALIAS(vrsa_expf, FN_PROTOTYPE(vrsa_expf));
WEAK_LIBM_ALIAS(vrsa_exp2f, FN_PROTOTYPE(vrsa_exp2f));
WEAK_LIBM_ALIAS(vrsa_log10f, FN_PROTOTYPE(vrsa_log10f));
WEAK_LIBM_ALIAS(vrsa_log2f, FN_PROTOTYPE(vrsa_log2f));
WEAK_LIBM_ALIAS(vrsa_logf, FN_PROTOTYPE(vrsa_logf));
WEAK_LIBM_ALIAS(vrsa_sinf, FN_PROTOTYPE(vrsa_sinf));



