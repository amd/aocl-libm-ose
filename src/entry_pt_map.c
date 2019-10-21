#include <libm/types.h>
#include <fn_macros.h>
//#include <weak_macros.h>

#define G_ENTRY_PT(fn) g_amd_libm_ep_##fn

#define MK_FN_NAME(fn)				\
	STRINGIFY(FN_PROTOTYPE(fn))

/*
 * This should gnerate something like this
 * -------------
 * #define fname_expf FN_PROTOTYPE(expf)
 * .p2align 4
 * .globl fname_expf
 * .type fname_expf,@function
 * fname_expf:
 * mov g_amd_libm_ep_expf@GOTPCREL(%rip), %rax
 * jmp *(%rax)
 * -----------
 */
#define LIBM_DECL_FN_MAP(fn)						\
	asm (								\
	"\n\t"".p2align 4"						\
	"\n\t"".globl " MK_FN_NAME(fn)					\
	"\n\t"".type " STRINGIFY(FN_PROTOTYPE(fn)) " ,@function"	\
	"\n\t" MK_FN_NAME(fn) " :"					\
	"\n\t" "mov " STRINGIFY(G_ENTRY_PT(fn)) "@GOTPCREL(%rip), %rax"	\
	"\n\t" "jmp *(%rax)"						\
		);

LIBM_DECL_FN_MAP(cbrtf);
LIBM_DECL_FN_MAP(cbrt);

LIBM_DECL_FN_MAP(cosf);
LIBM_DECL_FN_MAP(cos);

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

LIBM_DECL_FN_MAP(powf);
LIBM_DECL_FN_MAP(pow);

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
LIBM_DECL_FN_MAP(vrd2_sin);

LIBM_DECL_FN_MAP(vrd4_exp2);
LIBM_DECL_FN_MAP(vrd4_exp);
LIBM_DECL_FN_MAP(vrd4_log);

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
LIBM_DECL_FN_MAP(vrda_log10);
LIBM_DECL_FN_MAP(vrda_log2);
LIBM_DECL_FN_MAP(vrda_log);
LIBM_DECL_FN_MAP(vrda_sin);

LIBM_DECL_FN_MAP(vrsa_cosf);
LIBM_DECL_FN_MAP(vrsa_expf);
LIBM_DECL_FN_MAP(vrsa_log10f);
LIBM_DECL_FN_MAP(vrsa_log2f);
LIBM_DECL_FN_MAP(vrsa_logf);
LIBM_DECL_FN_MAP(vrsa_sinf);




#define WEAK_LIBM_ALIAS(x, y)					\
	asm("\n\t"".weak " STRINGIFY(x)				\
	    "\n\t"".set " STRINGIFY(x) ", " STRINGIFY(y)	\
	    "\n\t");


WEAK_LIBM_ALIAS(cbrtf, FN_PROTOTYPE(cbrtf));
WEAK_LIBM_ALIAS(cbrt, FN_PROTOTYPE(cbrt));

WEAK_LIBM_ALIAS(cosf, FN_PROTOTYPE(cosf));
WEAK_LIBM_ALIAS(cos, FN_PROTOTYPE(cos));

WEAK_LIBM_ALIAS(exp10f, FN_PROTOTYPE(exp10f));
WEAK_LIBM_ALIAS(exp10, FN_PROTOTYPE(exp10));

WEAK_LIBM_ALIAS(exp2f, FN_PROTOTYPE(exp2f));
WEAK_LIBM_ALIAS(exp2, FN_PROTOTYPE(exp2));

WEAK_LIBM_ALIAS(expf, FN_PROTOTYPE(expf));
WEAK_LIBM_ALIAS(exp, FN_PROTOTYPE(exp));

WEAK_LIBM_ALIAS(expm1f, FN_PROTOTYPE(expm1f));
WEAK_LIBM_ALIAS(expm1, FN_PROTOTYPE(expm1));

WEAK_LIBM_ALIAS(fmaf, FN_PROTOTYPE(fmaf));
WEAK_LIBM_ALIAS(fma, FN_PROTOTYPE(fma));

WEAK_LIBM_ALIAS(log10f, FN_PROTOTYPE(log10f));
WEAK_LIBM_ALIAS(log10, FN_PROTOTYPE(log10));

WEAK_LIBM_ALIAS(log1pf, FN_PROTOTYPE(log1pf));
WEAK_LIBM_ALIAS(log1p, FN_PROTOTYPE(log1p));

WEAK_LIBM_ALIAS(log2f, FN_PROTOTYPE(log2f));
WEAK_LIBM_ALIAS(log2, FN_PROTOTYPE(log2));

WEAK_LIBM_ALIAS(logf, FN_PROTOTYPE(logf));
WEAK_LIBM_ALIAS(log, FN_PROTOTYPE(log));

WEAK_LIBM_ALIAS(powf, FN_PROTOTYPE(powf));
WEAK_LIBM_ALIAS(pow, FN_PROTOTYPE(pow));

WEAK_LIBM_ALIAS(sincosf, FN_PROTOTYPE(sincosf));
WEAK_LIBM_ALIAS(sincos, FN_PROTOTYPE(sincos));

WEAK_LIBM_ALIAS(sinf, FN_PROTOTYPE(sinf));
WEAK_LIBM_ALIAS(sin, FN_PROTOTYPE(sin));

WEAK_LIBM_ALIAS(tanf, FN_PROTOTYPE(tanf));
WEAK_LIBM_ALIAS(tan, FN_PROTOTYPE(tan));

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
WEAK_LIBM_ALIAS(vrda_exp_, FN_PROTOTYPE(vrda_exp));
WEAK_LIBM_ALIAS(vrda_log10, FN_PROTOTYPE(vrda_log10));
WEAK_LIBM_ALIAS(vrda_log2, FN_PROTOTYPE(vrda_log2));
WEAK_LIBM_ALIAS(vrda_log_, FN_PROTOTYPE(vrda_log));
WEAK_LIBM_ALIAS(vrda_sin, FN_PROTOTYPE(vrda_sin));


WEAK_LIBM_ALIAS(vrsa_cosf, FN_PROTOTYPE(vrsa_cosf));
WEAK_LIBM_ALIAS(vrsa_expf, FN_PROTOTYPE(vrsa_expf));
WEAK_LIBM_ALIAS(vrsa_expf_, FN_PROTOTYPE(vrsa_expf));
WEAK_LIBM_ALIAS(vrsa_log10f, FN_PROTOTYPE(vrsa_log10f));
WEAK_LIBM_ALIAS(vrsa_log2f, FN_PROTOTYPE(vrsa_log2f));
WEAK_LIBM_ALIAS(vrsa_logf, FN_PROTOTYPE(vrsa_logf));
WEAK_LIBM_ALIAS(vrsa_logf_, FN_PROTOTYPE(vrsa_logf));
WEAK_LIBM_ALIAS(vrsa_sinf, FN_PROTOTYPE(vrsa_sinf));


WEAK_LIBM_ALIAS(fastcbrtf, FN_PROTOTYPE(cbrtf));
WEAK_LIBM_ALIAS(fastcbrt, FN_PROTOTYPE(cbrt));
WEAK_LIBM_ALIAS(fastcosf, FN_PROTOTYPE(cosf));
WEAK_LIBM_ALIAS(fastcos, FN_PROTOTYPE(cos));
WEAK_LIBM_ALIAS(fastexp10f, FN_PROTOTYPE(exp10f));
WEAK_LIBM_ALIAS(fastexp10, FN_PROTOTYPE(exp10));
WEAK_LIBM_ALIAS(fastexp2f, FN_PROTOTYPE(exp2f));
WEAK_LIBM_ALIAS(fastexp2, FN_PROTOTYPE(exp2));
WEAK_LIBM_ALIAS(fastexpf, FN_PROTOTYPE(expf));
WEAK_LIBM_ALIAS(fastexp, FN_PROTOTYPE(exp));
WEAK_LIBM_ALIAS(fastexpm1f, FN_PROTOTYPE(expm1f));
WEAK_LIBM_ALIAS(fastexpm1, FN_PROTOTYPE(expm1));
WEAK_LIBM_ALIAS(fastfmaf, FN_PROTOTYPE(fmaf));
WEAK_LIBM_ALIAS(fastfma, FN_PROTOTYPE(fma));
WEAK_LIBM_ALIAS(fastlog10f,FN_PROTOTYPE(log10f));
WEAK_LIBM_ALIAS(fastlog10, FN_PROTOTYPE(log10));
WEAK_LIBM_ALIAS(fastlog1pf, FN_PROTOTYPE(log1pf));
WEAK_LIBM_ALIAS(fastlog1p, FN_PROTOTYPE(log1p));
WEAK_LIBM_ALIAS(fastlog2f, FN_PROTOTYPE(log2f));
WEAK_LIBM_ALIAS(fastlog2, FN_PROTOTYPE(log2));
WEAK_LIBM_ALIAS(fastlogf,FN_PROTOTYPE(logf));
WEAK_LIBM_ALIAS(fastlog, FN_PROTOTYPE(log));
WEAK_LIBM_ALIAS(fastpow, FN_PROTOTYPE(pow));
WEAK_LIBM_ALIAS(fastsincos, FN_PROTOTYPE(sincos));
WEAK_LIBM_ALIAS(fastsincos, FN_PROTOTYPE(sincosf));
WEAK_LIBM_ALIAS(fastsinf, FN_PROTOTYPE(sinf));
WEAK_LIBM_ALIAS(fasttanf, FN_PROTOTYPE(tanf));
WEAK_LIBM_ALIAS(fasttan, FN_PROTOTYPE(tan));
