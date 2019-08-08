#include "libm_amd_paths.h"
#ifdef _MSC_VER
#include <intrin.h>
#define CPUID __cpuid
#define CCALL __cdecl
#pragma section(".CRT$XCU",read)
#define INITIALIZER(f)  static void __cdecl f(void); __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; static void __cdecl f(void)
#else
static void cpuid(int *out, int in) __attribute__((noinline));
static void cpuid(int *out, int in)
{
    __asm__ __volatile__("    pushq %%rbx;          \
                          xorq %%rax, %%rax;        \
                          movl %%esi, %%eax;        \
                          cpuid;                    \
                          movl %%eax, 0x0(%%rdi);   \
                          movl %%ebx, 0x4(%%rdi);   \
                          movl %%ecx, 0x8(%%rdi);   \
                          movl %%edx, 0xc(%%rdi);   \
                          popq %%rbx;"              \
                          : : "D" (out), "S" (in)   \
                          : "%rax", "%rcx", "%rdx" );
}
#define CPUID cpuid
#define CCALL
#define INITIALIZER(f) static void f(void) __attribute__((constructor)); static void f(void)
#endif
#ifdef __cplusplus
extern "C"
{
#endif
    void    (*g_amd_libm_ep_vrda_exp)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_exp2)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_exp10)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_expm1)(int, double*, double*);
    void    (*g_amd_libm_ep_vrsa_expf)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_exp2f)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_exp10f)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_expm1f)(int, float*, float*);
    void    (*g_amd_libm_ep_vrda_cbrt)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_log)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_log10)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_log1p)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_log2)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_cos)(int, double*, double*);
    void    (*g_amd_libm_ep_vrda_sin)(int, double*, double*);


    void    (*g_amd_libm_ep_vrsa_cosf)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_log1pf)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_sinf)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_log2f)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_cbrtf)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_log10f)(int, float*, float*);
    void    (*g_amd_libm_ep_vrsa_logf)(int, float*, float*);

    double  (*g_amd_libm_ep_fma)(double,double,double);
    double  (*g_amd_libm_ep_exp)(double);
    double  (*g_amd_libm_ep_exp2)(double);
    double  (*g_amd_libm_ep_exp10)(double);
    double  (*g_amd_libm_ep_expm1)(double);
    double  (*g_amd_libm_ep_log)(double);
    double  (*g_amd_libm_ep_log2)(double);
    double  (*g_amd_libm_ep_log10)(double);
    double  (*g_amd_libm_ep_log1p)(double);
    double  (*g_amd_libm_ep_cbrt)(double );
    double  (*g_amd_libm_ep_cos)(double);
    double  (*g_amd_libm_ep_sin)(double);
    double  (*g_amd_libm_ep_pow)(double,double);
    double  (*g_amd_libm_ep_fastpow)(double,double);

#ifndef _MSC_VER
    void  (*g_amd_libm_ep_sincos)(double,double *,double *);
    void  (*g_amd_libm_ep_sincosf)(float,float *,float *);
#endif
    double  (*g_amd_libm_ep_tan)(double);

    float   (*g_amd_libm_ep_fmaf)(float,float,float);
    float   (*g_amd_libm_ep_expf)(float);
    float   (*g_amd_libm_ep_exp2f)(float);
    float   (*g_amd_libm_ep_exp10f)(float);
    float   (*g_amd_libm_ep_expm1f)(float);
    float   (*g_amd_libm_ep_logf)(float);
    float   (*g_amd_libm_ep_log2f)(float);
    float   (*g_amd_libm_ep_log10f)(float);
    float   (*g_amd_libm_ep_log1pf)(float);
    float   (*g_amd_libm_ep_cosf)(float);
    float   (*g_amd_libm_ep_sinf)(float);
    float   (*g_amd_libm_ep_tanf)(float);
    float   (*g_amd_libm_ep_cbrtf)(float);
    float   (*g_amd_libm_ep_powf)(float,float);

    __m128  (*g_amd_libm_ep_vrs4_expf)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_exp2f)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_exp10f)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_expm1f)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_logf)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_log2f)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_log10f)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_log1pf)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_cosf)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_sinf)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_tanf)(__m128);
    __m128  (*g_amd_libm_ep_vrs4_cbrtf)(__m128);

    __m128d (*g_amd_libm_ep_vrd2_exp)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_exp2)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_exp10)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_expm1)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_log)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_log2)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_log10)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_log1p)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_cbrt)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_cos)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_sin)(__m128d);
    __m128d (*g_amd_libm_ep_vrd2_tan)(__m128d);

#ifdef __cplusplus
}
#endif

enum CPU_INFO
{

	AMDLIBM_BAS64 = 0,
	AMDLIBM_FMA3 = 3,
	AMDLIBM_BDOZR = 4
};

static int cpuid_bdozr(void)
{
    int avx = 0, fma4 = 0,fma3=0;
    int info[4];
    CPUID(info, 0x1);

    if((info[2] & (0x1 << 27)) ? 1 : 0)  /* check for osxsave*/
    {

        fma3 = (info[2] & (1 << 12)) ? 1 : 0;
        avx = (info[2] & (0x1 << 28)) ? 1 : 0;
        CPUID(info, 0x80000001);
        fma4 = (info[2] & (1 << 16)) ? 1 : 0;


        if(avx & fma4)
	{
			return AMDLIBM_BDOZR;
	}
	if(avx & fma3)
	{
			return AMDLIBM_FMA3;
	}

			return AMDLIBM_BAS64;
   }
   else
   {
        return AMDLIBM_BAS64;
   }

}
  
INITIALIZER(InitMapEntryPoints)
{
    enum CPU_INFO bdozr = cpuid_bdozr();
    const char *env_var = "AMDLIBM_FMA";	/*Environment Variable*/
    char *env_val;

#ifdef _MSC_VER
    errno_t error =  _dupenv_s(&env_val,NULL,env_var);
    if (error) env_val= NULL;
#else
    env_val = getenv(env_var);
#endif

    if(env_val != NULL)
    {
		switch(env_val[0] - '0')
		{
			case 0: bdozr = AMDLIBM_BAS64; break;
			case 3: bdozr = AMDLIBM_FMA3; break;
			case 4: bdozr = AMDLIBM_BDOZR; break;
			default : break;	/*In case of invalid Environment Variable value*/
						/*We consider the machine supported path*/
		}

    }
#ifdef _MSC_VER
    free(env_val);
#endif

    switch(bdozr){
		case AMDLIBM_BDOZR:
			g_amd_libm_ep_cbrt       	=FN_PROTOTYPE_BDOZR(cbrt);
			g_amd_libm_ep_cbrtf      	=FN_PROTOTYPE_BDOZR(cbrtf);
			g_amd_libm_ep_sin        	=FN_PROTOTYPE_BDOZR(sin);
			#ifndef _MSC_VER
			g_amd_libm_ep_sincos     	=FN_PROTOTYPE_BDOZR(sincos);
			g_amd_libm_ep_sincosf    	=FN_PROTOTYPE_BDOZR(sincosf);
			#endif
			g_amd_libm_ep_sinf       	=FN_PROTOTYPE_BDOZR(sinf);
			g_amd_libm_ep_cos        	=FN_PROTOTYPE_BDOZR(cos);
			g_amd_libm_ep_cosf       	=FN_PROTOTYPE_BDOZR(cosf);
			g_amd_libm_ep_tan        	=FN_PROTOTYPE_BDOZR(tan);
			g_amd_libm_ep_tanf       	=FN_PROTOTYPE_BDOZR(tanf);
			g_amd_libm_ep_vrd2_cbrt  	=FN_PROTOTYPE_BDOZR(vrd2_cbrt);
			g_amd_libm_ep_vrs4_cbrtf 	=FN_PROTOTYPE_BDOZR(vrs4_cbrtf);
			g_amd_libm_ep_vrd2_cos   	=FN_PROTOTYPE_BDOZR(vrd2_cos);
			g_amd_libm_ep_vrs4_cosf  	=FN_PROTOTYPE_BDOZR(vrs4_cosf);
			g_amd_libm_ep_vrd2_sin   	=FN_PROTOTYPE_BDOZR(vrd2_sin);
			g_amd_libm_ep_vrs4_sinf  	=FN_PROTOTYPE_BDOZR(vrs4_sinf);
			g_amd_libm_ep_vrd2_tan   	=FN_PROTOTYPE_BDOZR(vrd2_tan);
			g_amd_libm_ep_vrs4_tanf  	=FN_PROTOTYPE_BDOZR(vrs4_tanf);
			g_amd_libm_ep_vrd2_log   	=FN_PROTOTYPE_BDOZR(vrd2_log);
			g_amd_libm_ep_vrd2_log2  	=FN_PROTOTYPE_BDOZR(vrd2_log2);
			g_amd_libm_ep_vrd2_log10 	=FN_PROTOTYPE_BDOZR(vrd2_log10);
			g_amd_libm_ep_vrd2_log1p 	=FN_PROTOTYPE_BDOZR(vrd2_log1p);
			g_amd_libm_ep_vrs4_logf  	=FN_PROTOTYPE_BDOZR(vrs4_logf);
			g_amd_libm_ep_vrs4_log2f 	=FN_PROTOTYPE_BDOZR(vrs4_log2f);
			g_amd_libm_ep_vrs4_log10f	=FN_PROTOTYPE_BDOZR(vrs4_log10f);
			g_amd_libm_ep_vrs4_log1pf	=FN_PROTOTYPE_BDOZR(vrs4_log1pf);
			g_amd_libm_ep_logf       	=FN_PROTOTYPE_BDOZR(logf);
			g_amd_libm_ep_log2f      	=FN_PROTOTYPE_BDOZR(log2f);
			g_amd_libm_ep_log10f     	=FN_PROTOTYPE_BDOZR(log10f);
			g_amd_libm_ep_log1pf     	=FN_PROTOTYPE_BDOZR(log1pf);
			g_amd_libm_ep_log        	=FN_PROTOTYPE_BDOZR(log);
			g_amd_libm_ep_log2       	=FN_PROTOTYPE_BDOZR(log2);
			g_amd_libm_ep_log10      	=FN_PROTOTYPE_BDOZR(log10);
			g_amd_libm_ep_log1p      	=FN_PROTOTYPE_BDOZR(log1p);
			g_amd_libm_ep_exp        	=FN_PROTOTYPE_BDOZR(exp);
			g_amd_libm_ep_exp2       	=FN_PROTOTYPE_BDOZR(exp2);
			g_amd_libm_ep_exp10      	=FN_PROTOTYPE_BDOZR(exp10);
			g_amd_libm_ep_expm1      	=FN_PROTOTYPE_BDOZR(expm1);
			g_amd_libm_ep_expf       	=FN_PROTOTYPE_BDOZR(expf);
			g_amd_libm_ep_exp2f	 	=FN_PROTOTYPE_BDOZR(exp2f);
			g_amd_libm_ep_exp10f	 	=FN_PROTOTYPE_BDOZR(exp10f);
			g_amd_libm_ep_expm1f	 	=FN_PROTOTYPE_BDOZR(expm1f);
			g_amd_libm_ep_vrs4_expf  	=FN_PROTOTYPE_BDOZR(vrs4_expf);
			g_amd_libm_ep_vrs4_exp2f 	=FN_PROTOTYPE_BDOZR(vrs4_exp2f);
			g_amd_libm_ep_vrs4_exp10f	=FN_PROTOTYPE_BDOZR(vrs4_exp10f);
			g_amd_libm_ep_vrs4_expm1f	=FN_PROTOTYPE_BDOZR(vrs4_expm1f);
			g_amd_libm_ep_vrd2_exp   	=FN_PROTOTYPE_BDOZR(vrd2_exp);
			g_amd_libm_ep_vrd2_exp2	 	=FN_PROTOTYPE_BDOZR(vrd2_exp2);
			g_amd_libm_ep_vrd2_exp10 	=FN_PROTOTYPE_BDOZR(vrd2_exp10);
			g_amd_libm_ep_vrd2_expm1 	=FN_PROTOTYPE_BDOZR(vrd2_expm1);
			g_amd_libm_ep_vrda_exp   	=FN_PROTOTYPE_BDOZR(vrda_exp);
			g_amd_libm_ep_vrda_exp2	 	=FN_PROTOTYPE_BDOZR(vrda_exp2);
			g_amd_libm_ep_vrda_exp10 	=FN_PROTOTYPE_BDOZR(vrda_exp10);
			g_amd_libm_ep_vrda_expm1 	=FN_PROTOTYPE_BDOZR(vrda_expm1);

			g_amd_libm_ep_vrda_cbrt	 	=FN_PROTOTYPE_BDOZR(vrda_cbrt);
			g_amd_libm_ep_vrda_log	 	=FN_PROTOTYPE_BDOZR(vrda_log);
			g_amd_libm_ep_vrda_log10	=FN_PROTOTYPE_BDOZR(vrda_log10);
			g_amd_libm_ep_vrda_log1p	=FN_PROTOTYPE_BDOZR(vrda_log1p);
			g_amd_libm_ep_vrda_log2	 	=FN_PROTOTYPE_BDOZR(vrda_log2);
			g_amd_libm_ep_vrda_sin	 	=FN_PROTOTYPE_BDOZR(vrda_sin);
			g_amd_libm_ep_vrda_cos	 	=FN_PROTOTYPE_BDOZR(vrda_cos);

			g_amd_libm_ep_vrsa_cbrtf 	=FN_PROTOTYPE_BDOZR(vrsa_cbrtf);
			g_amd_libm_ep_vrsa_logf	 	=FN_PROTOTYPE_BDOZR(vrsa_logf);
			g_amd_libm_ep_vrsa_log10f	=FN_PROTOTYPE_BDOZR(vrsa_log10f);
			g_amd_libm_ep_vrsa_log1pf	=FN_PROTOTYPE_BDOZR(vrsa_log1pf);
			g_amd_libm_ep_vrsa_log2f 	=FN_PROTOTYPE_BDOZR(vrsa_log2f);
			g_amd_libm_ep_vrsa_sinf		=FN_PROTOTYPE_BDOZR(vrsa_sinf);
			g_amd_libm_ep_vrsa_cosf	 	=FN_PROTOTYPE_BDOZR(vrsa_cosf);

			g_amd_libm_ep_vrsa_expf  	=FN_PROTOTYPE_BDOZR(vrsa_expf);
			g_amd_libm_ep_vrsa_exp2f 	=FN_PROTOTYPE_BDOZR(vrsa_exp2f);
			g_amd_libm_ep_vrsa_exp10f	=FN_PROTOTYPE_BDOZR(vrsa_exp10f);
			g_amd_libm_ep_vrsa_expm1f	=FN_PROTOTYPE_BDOZR(vrsa_expm1f);
			g_amd_libm_ep_fmaf       	=FN_PROTOTYPE_BDOZR(fmaf);
			g_amd_libm_ep_fma        	=FN_PROTOTYPE_BDOZR(fma);
			g_amd_libm_ep_powf       	=FN_PROTOTYPE_BDOZR(powf);
			g_amd_libm_ep_pow        	=FN_PROTOTYPE_BDOZR(pow);
			g_amd_libm_ep_fastpow           =FN_PROTOTYPE_BDOZR(pow);
                        break;


		case AMDLIBM_FMA3:

			g_amd_libm_ep_cbrt       =	FN_PROTOTYPE_FMA3(cbrt);
			g_amd_libm_ep_cbrtf      =	FN_PROTOTYPE_FMA3(cbrtf);
			g_amd_libm_ep_sin        =	FN_PROTOTYPE_FMA3(sin);
			#ifndef _MSC_VER
			g_amd_libm_ep_sincos     =	FN_PROTOTYPE_FMA3(sincos);
			g_amd_libm_ep_sincosf    =	FN_PROTOTYPE_FMA3(sincosf);
			#endif
			g_amd_libm_ep_sinf       =	FN_PROTOTYPE_FMA3(sinf);
			g_amd_libm_ep_cos        =	FN_PROTOTYPE_FMA3(cos) ;
			g_amd_libm_ep_cosf       =	FN_PROTOTYPE_FMA3(cosf);
			g_amd_libm_ep_tan        =	FN_PROTOTYPE_FMA3(tan);
			g_amd_libm_ep_tanf       =	FN_PROTOTYPE_FMA3(tanf);
			g_amd_libm_ep_vrd2_cbrt  =	FN_PROTOTYPE_FMA3(vrd2_cbrt);
			g_amd_libm_ep_vrs4_cbrtf =	FN_PROTOTYPE_FMA3(vrs4_cbrtf);
			g_amd_libm_ep_vrd2_cos   =	FN_PROTOTYPE_FMA3(vrd2_cos);
			g_amd_libm_ep_vrs4_cosf  =	FN_PROTOTYPE_FMA3(vrs4_cosf);
			g_amd_libm_ep_vrd2_sin   =	FN_PROTOTYPE_FMA3(vrd2_sin);
			g_amd_libm_ep_vrs4_sinf  =	FN_PROTOTYPE_FMA3(vrs4_sinf);
			g_amd_libm_ep_vrd2_tan   =	FN_PROTOTYPE_FMA3(vrd2_tan);
			g_amd_libm_ep_vrs4_tanf  =	FN_PROTOTYPE_FMA3(vrs4_tanf);
			g_amd_libm_ep_vrd2_log   =	FN_PROTOTYPE_FMA3(vrd2_log);
			g_amd_libm_ep_vrd2_log2  =	FN_PROTOTYPE_FMA3(vrd2_log2);
			g_amd_libm_ep_vrd2_log10 =	FN_PROTOTYPE_FMA3(vrd2_log10);
			g_amd_libm_ep_vrd2_log1p =	FN_PROTOTYPE_FMA3(vrd2_log1p);
			g_amd_libm_ep_vrs4_logf  =	FN_PROTOTYPE_FMA3(vrs4_logf);
			g_amd_libm_ep_vrs4_log2f =	FN_PROTOTYPE_FMA3(vrs4_log2f);
			g_amd_libm_ep_vrs4_log10f=	FN_PROTOTYPE_FMA3(vrs4_log10f);
			g_amd_libm_ep_vrs4_log1pf=	FN_PROTOTYPE_FMA3(vrs4_log1pf);
			g_amd_libm_ep_logf       =	FN_PROTOTYPE_FMA3(logf);
			g_amd_libm_ep_log2f      =	FN_PROTOTYPE_FMA3(log2f);
			g_amd_libm_ep_log10f     =	FN_PROTOTYPE_FMA3(log10f);
			g_amd_libm_ep_log1pf     =	FN_PROTOTYPE_FMA3(log1pf);
			g_amd_libm_ep_log        =	FN_PROTOTYPE_FMA3(log);
			g_amd_libm_ep_log2       =	FN_PROTOTYPE_FMA3(log2);
			g_amd_libm_ep_log10      =	FN_PROTOTYPE_FMA3(log10);
			g_amd_libm_ep_log1p      =	FN_PROTOTYPE_FMA3(log1p);
			g_amd_libm_ep_exp        =	FN_PROTOTYPE_FMA3(exp);
			g_amd_libm_ep_exp2       =	FN_PROTOTYPE_FMA3(exp2);
			g_amd_libm_ep_exp10      =	FN_PROTOTYPE_FMA3(exp10);
			g_amd_libm_ep_expm1      =	FN_PROTOTYPE_FMA3(expm1);
			g_amd_libm_ep_expf       =	FN_PROTOTYPE_FMA3(expf);
			g_amd_libm_ep_exp2f	 =	FN_PROTOTYPE_FMA3(exp2f);
			g_amd_libm_ep_exp10f	 =	FN_PROTOTYPE_FMA3(exp10f);
			g_amd_libm_ep_expm1f	 =	FN_PROTOTYPE_FMA3(expm1f);
			g_amd_libm_ep_vrs4_expf  =	FN_PROTOTYPE_FMA3(vrs4_expf);
			g_amd_libm_ep_vrs4_exp2f =	FN_PROTOTYPE_FMA3(vrs4_exp2f);
			g_amd_libm_ep_vrs4_exp10f=	FN_PROTOTYPE_FMA3(vrs4_exp10f);
			g_amd_libm_ep_vrs4_expm1f=	FN_PROTOTYPE_FMA3(vrs4_expm1f);
			g_amd_libm_ep_vrd2_exp   =	FN_PROTOTYPE_FMA3(vrd2_exp);
			g_amd_libm_ep_vrd2_exp2	 =	FN_PROTOTYPE_FMA3(vrd2_exp2);
			g_amd_libm_ep_vrd2_exp10 =	FN_PROTOTYPE_FMA3(vrd2_exp10);
			g_amd_libm_ep_vrd2_expm1 =	FN_PROTOTYPE_FMA3(vrd2_expm1);
			g_amd_libm_ep_vrda_cbrt	 =	FN_PROTOTYPE_FMA3(vrda_cbrt);
			g_amd_libm_ep_vrda_log	 =	FN_PROTOTYPE_FMA3(vrda_log);
			g_amd_libm_ep_vrda_log10 =	FN_PROTOTYPE_FMA3(vrda_log10);
			g_amd_libm_ep_vrda_log1p =	FN_PROTOTYPE_FMA3(vrda_log1p);
			g_amd_libm_ep_vrda_log2	 =	FN_PROTOTYPE_FMA3(vrda_log2);
			g_amd_libm_ep_vrda_sin	 =	FN_PROTOTYPE_FMA3(vrda_sin);
			g_amd_libm_ep_vrda_cos	 =	FN_PROTOTYPE_FMA3(vrda_cos);


			g_amd_libm_ep_vrsa_cbrtf =	FN_PROTOTYPE_FMA3(vrsa_cbrtf);
			g_amd_libm_ep_vrsa_logf	 =	FN_PROTOTYPE_FMA3(vrsa_logf);
			g_amd_libm_ep_vrsa_log10f=	FN_PROTOTYPE_FMA3(vrsa_log10f);
			g_amd_libm_ep_vrsa_log1pf=	FN_PROTOTYPE_FMA3(vrsa_log1pf);
			g_amd_libm_ep_vrsa_log2f =	FN_PROTOTYPE_FMA3(vrsa_log2f);
			g_amd_libm_ep_vrsa_sinf	 =	FN_PROTOTYPE_FMA3(vrsa_sinf);
			g_amd_libm_ep_vrsa_cosf	 =	FN_PROTOTYPE_FMA3(vrsa_cosf);

			g_amd_libm_ep_vrda_exp   =	FN_PROTOTYPE_FMA3(vrda_exp);
			g_amd_libm_ep_vrda_exp2	 =	FN_PROTOTYPE_FMA3(vrda_exp2);
			g_amd_libm_ep_vrda_exp10 =	FN_PROTOTYPE_FMA3(vrda_exp10);
			g_amd_libm_ep_vrda_expm1 =	FN_PROTOTYPE_FMA3(vrda_expm1);
			g_amd_libm_ep_vrsa_expf  =	FN_PROTOTYPE_FMA3(vrsa_expf);
			g_amd_libm_ep_vrsa_exp2f =	FN_PROTOTYPE_FMA3(vrsa_exp2f);
			g_amd_libm_ep_vrsa_exp10f=	FN_PROTOTYPE_FMA3(vrsa_exp10f);
			g_amd_libm_ep_vrsa_expm1f=	FN_PROTOTYPE_FMA3(vrsa_expm1f);
			g_amd_libm_ep_fmaf       =	FN_PROTOTYPE_FMA3(fmaf);
			g_amd_libm_ep_fma        =	FN_PROTOTYPE_FMA3(fma);
			g_amd_libm_ep_pow        =	FN_PROTOTYPE_FMA3(pow);
                        g_amd_libm_ep_fastpow        =  FN_PROTOTYPE_FMA3(fastpow);
			g_amd_libm_ep_powf       =	FN_PROTOTYPE_FMA3(powf);
			break;

		/*Previously AMDLIBM_BAS64*/
		default:
			g_amd_libm_ep_cbrt       =	FN_PROTOTYPE_BAS64(cbrt);
			g_amd_libm_ep_cbrtf      =	FN_PROTOTYPE_BAS64(cbrtf);
			g_amd_libm_ep_sin        =	FN_PROTOTYPE_BAS64(sin);
			#ifndef _MSC_VER
			g_amd_libm_ep_sincos     =	FN_PROTOTYPE_BAS64(sincos);
			g_amd_libm_ep_sincosf    =	FN_PROTOTYPE_BAS64(sincosf);
			#endif
			g_amd_libm_ep_sinf       =	FN_PROTOTYPE_BAS64(sinf);
			g_amd_libm_ep_cos        =	FN_PROTOTYPE_BAS64(cos) ;
			g_amd_libm_ep_cosf       =	FN_PROTOTYPE_BAS64(cosf);
			g_amd_libm_ep_tan        =	FN_PROTOTYPE_BAS64(tan);
			g_amd_libm_ep_tanf       =	FN_PROTOTYPE_BAS64(tanf);
			g_amd_libm_ep_vrd2_cbrt  =	FN_PROTOTYPE_BAS64(vrd2_cbrt);
			g_amd_libm_ep_vrs4_cbrtf =	FN_PROTOTYPE_BAS64(vrs4_cbrtf);
			g_amd_libm_ep_vrd2_cos   =	FN_PROTOTYPE_BAS64(vrd2_cos);
			g_amd_libm_ep_vrs4_cosf  =	FN_PROTOTYPE_BAS64(vrs4_cosf);
			g_amd_libm_ep_vrd2_sin   =	FN_PROTOTYPE_BAS64(vrd2_sin);
			g_amd_libm_ep_vrs4_sinf  =	FN_PROTOTYPE_BAS64(vrs4_sinf);
			g_amd_libm_ep_vrd2_tan   =	FN_PROTOTYPE_BAS64(vrd2_tan);
			g_amd_libm_ep_vrs4_tanf  =	FN_PROTOTYPE_BAS64(vrs4_tanf);
			g_amd_libm_ep_vrd2_log   =	FN_PROTOTYPE_BAS64(vrd2_log);
			g_amd_libm_ep_vrd2_log2  =	FN_PROTOTYPE_BAS64(vrd2_log2);
			g_amd_libm_ep_vrd2_log10 =	FN_PROTOTYPE_BAS64(vrd2_log10);
			g_amd_libm_ep_vrd2_log1p =	FN_PROTOTYPE_BAS64(vrd2_log1p);
			g_amd_libm_ep_vrs4_logf  =	FN_PROTOTYPE_BAS64(vrs4_logf);
			g_amd_libm_ep_vrs4_log2f =	FN_PROTOTYPE_BAS64(vrs4_log2f);
			g_amd_libm_ep_vrs4_log10f=	FN_PROTOTYPE_BAS64(vrs4_log10f);
			g_amd_libm_ep_vrs4_log1pf=	FN_PROTOTYPE_BAS64(vrs4_log1pf);
			g_amd_libm_ep_logf       =	FN_PROTOTYPE_BAS64(logf);
			g_amd_libm_ep_log2f      =	FN_PROTOTYPE_BAS64(log2f);
			g_amd_libm_ep_log10f     =	FN_PROTOTYPE_BAS64(log10f);
			g_amd_libm_ep_log1pf     =	FN_PROTOTYPE_BAS64(log1pf);
			g_amd_libm_ep_log        =	FN_PROTOTYPE_BAS64(log);
			g_amd_libm_ep_log2       =	FN_PROTOTYPE_BAS64(log2);
			g_amd_libm_ep_log10      =	FN_PROTOTYPE_BAS64(log10);
			g_amd_libm_ep_log1p      =	FN_PROTOTYPE_BAS64(log1p);
			g_amd_libm_ep_exp        =	FN_PROTOTYPE_BAS64(exp);
			g_amd_libm_ep_exp2       =	FN_PROTOTYPE_BAS64(exp2);
			g_amd_libm_ep_exp10      =	FN_PROTOTYPE_BAS64(exp10);
			g_amd_libm_ep_expm1      =	FN_PROTOTYPE_BAS64(expm1);
			g_amd_libm_ep_expf       =	FN_PROTOTYPE_BAS64(expf);
			g_amd_libm_ep_exp2f	 =	FN_PROTOTYPE_BAS64(exp2f);
			g_amd_libm_ep_exp10f	 =	FN_PROTOTYPE_BAS64(exp10f);
			g_amd_libm_ep_expm1f	 =	FN_PROTOTYPE_BAS64(expm1f);
			g_amd_libm_ep_vrs4_expf  =	FN_PROTOTYPE_BAS64(vrs4_expf);
			g_amd_libm_ep_vrs4_exp2f =	FN_PROTOTYPE_BAS64(vrs4_exp2f);
			g_amd_libm_ep_vrs4_exp10f=	FN_PROTOTYPE_BAS64(vrs4_exp10f);
			g_amd_libm_ep_vrs4_expm1f=	FN_PROTOTYPE_BAS64(vrs4_expm1f);
			g_amd_libm_ep_vrd2_exp   =	FN_PROTOTYPE_BAS64(vrd2_exp);
			g_amd_libm_ep_vrd2_exp2	 =	FN_PROTOTYPE_BAS64(vrd2_exp2);
			g_amd_libm_ep_vrd2_exp10 =	FN_PROTOTYPE_BAS64(vrd2_exp10);
			g_amd_libm_ep_vrd2_expm1 =	FN_PROTOTYPE_BAS64(vrd2_expm1);

			g_amd_libm_ep_vrda_exp   =	FN_PROTOTYPE_BAS64(vrda_exp);
			g_amd_libm_ep_vrda_exp2	 =	FN_PROTOTYPE_BAS64(vrda_exp2);
			g_amd_libm_ep_vrda_exp10 =	FN_PROTOTYPE_BAS64(vrda_exp10);
			g_amd_libm_ep_vrda_expm1 =	FN_PROTOTYPE_BAS64(vrda_expm1);


			g_amd_libm_ep_vrda_cbrt	 =	FN_PROTOTYPE_BAS64(vrda_cbrt);
			g_amd_libm_ep_vrda_log	 =	FN_PROTOTYPE_BAS64(vrda_log);
			g_amd_libm_ep_vrda_log10 =	FN_PROTOTYPE_BAS64(vrda_log10);
			g_amd_libm_ep_vrda_log1p =	FN_PROTOTYPE_BAS64(vrda_log1p);
			g_amd_libm_ep_vrda_log2	 =	FN_PROTOTYPE_BAS64(vrda_log2);
			g_amd_libm_ep_vrda_sin	 =	FN_PROTOTYPE_BAS64(vrda_sin);
			g_amd_libm_ep_vrda_cos	 =	FN_PROTOTYPE_BAS64(vrda_cos);


			g_amd_libm_ep_vrsa_cbrtf =	FN_PROTOTYPE_BAS64(vrsa_cbrtf);
			g_amd_libm_ep_vrsa_logf	 =	FN_PROTOTYPE_BAS64(vrsa_logf);
			g_amd_libm_ep_vrsa_log10f=	FN_PROTOTYPE_BAS64(vrsa_log10f);
			g_amd_libm_ep_vrsa_log1pf=	FN_PROTOTYPE_BAS64(vrsa_log1pf);
			g_amd_libm_ep_vrsa_log2f =	FN_PROTOTYPE_BAS64(vrsa_log2f);
			g_amd_libm_ep_vrsa_sinf	 =	FN_PROTOTYPE_BAS64(vrsa_sinf);
			g_amd_libm_ep_vrsa_cosf	 =	FN_PROTOTYPE_BAS64(vrsa_cosf);

			g_amd_libm_ep_vrsa_expf  =	FN_PROTOTYPE_BAS64(vrsa_expf);
			g_amd_libm_ep_vrsa_exp2f =	FN_PROTOTYPE_BAS64(vrsa_exp2f);
			g_amd_libm_ep_vrsa_exp10f=	FN_PROTOTYPE_BAS64(vrsa_exp10f);
			g_amd_libm_ep_vrsa_expm1f=	FN_PROTOTYPE_BAS64(vrsa_expm1f);

			g_amd_libm_ep_fmaf       =	FN_PROTOTYPE_BAS64(fmaf);
			g_amd_libm_ep_fma        =	FN_PROTOTYPE_BAS64(fma);
			g_amd_libm_ep_powf       =	FN_PROTOTYPE_BAS64(powf);
			g_amd_libm_ep_pow        =	FN_PROTOTYPE_BAS64(pow);
                        g_amd_libm_ep_fastpow    =      FN_PROTOTYPE_BAS64(pow);
			break;

	}

}
