
#include "fn_macros.h"
#include "weak_macros.h"


#define fname_fmaf FN_PROTOTYPE(fmaf)
#define fname_fma  FN_PROTOTYPE(fma)

#define fname_exp  FN_PROTOTYPE(exp)
#define fname_exp2  FN_PROTOTYPE(exp2)
#define fname_exp10  FN_PROTOTYPE(exp10)
#define fname_expm1  FN_PROTOTYPE(expm1)

#define fname_expf  FN_PROTOTYPE(expf)
#define fname_exp2f  FN_PROTOTYPE(exp2f)
#define fname_exp10f  FN_PROTOTYPE(exp10f)
#define fname_expm1f  FN_PROTOTYPE(expm1f)

#define fname_vrs4_expf  FN_PROTOTYPE(vrs4_expf)
#define fname_vrs4_exp2f  FN_PROTOTYPE(vrs4_exp2f)
#define fname_vrs4_exp10f  FN_PROTOTYPE(vrs4_exp10f)
#define fname_vrs4_expm1f  FN_PROTOTYPE(vrs4_expm1f)

#define fname_vrd2_exp  FN_PROTOTYPE(vrd2_exp)
#define fname_vrd2_exp2  FN_PROTOTYPE(vrd2_exp2)
#define fname_vrd2_exp10  FN_PROTOTYPE(vrd2_exp10)
#define fname_vrd2_expm1  FN_PROTOTYPE(vrd2_expm1)

#define fname_vrda_exp  FN_PROTOTYPE(vrda_exp)
#define fname_vrda_exp2  FN_PROTOTYPE(vrda_exp2)
#define fname_vrda_exp10  FN_PROTOTYPE(vrda_exp10)
#define fname_vrda_expm1  FN_PROTOTYPE(vrda_expm1)
#define fname_vrda_sin FN_PROTOTYPE(vrda_sin)
#define fname_vrda_cbrt FN_PROTOTYPE(vrda_cbrt)
#define fname_vrda_log10 FN_PROTOTYPE(vrda_log10)
#define fname_vrda_log FN_PROTOTYPE(vrda_log)
#define fname_vrda_log1p FN_PROTOTYPE(vrda_log1p)
#define fname_vrda_log2 FN_PROTOTYPE(vrda_log2)
#define fname_vrda_cos FN_PROTOTYPE(vrda_cos)


#define fname_vrsa_expf  FN_PROTOTYPE(vrsa_expf)
#define fname_vrsa_exp2f  FN_PROTOTYPE(vrsa_exp2f)
#define fname_vrsa_exp10f  FN_PROTOTYPE(vrsa_exp10f)
#define fname_vrsa_expm1f  FN_PROTOTYPE(vrsa_expm1f)
#define fname_vrsa_log1pf  FN_PROTOTYPE(vrsa_log1pf)
#define fname_vrsa_log2f  FN_PROTOTYPE(vrsa_log2f)
#define fname_vrsa_cbrtf  FN_PROTOTYPE(vrsa_cbrtf)
#define fname_vrsa_log10f  FN_PROTOTYPE(vrsa_log10f)
#define fname_vrsa_logf  FN_PROTOTYPE(vrsa_logf)
#define fname_vrsa_sinf FN_PROTOTYPE(vrsa_sinf)
#define fname_vrsa_cosf FN_PROTOTYPE(vrsa_cosf)


#define fname_vrd2_log  FN_PROTOTYPE(vrd2_log)
#define fname_vrd2_log2  FN_PROTOTYPE(vrd2_log2)
#define fname_vrd2_log10  FN_PROTOTYPE(vrd2_log10)
#define fname_vrd2_log1p  FN_PROTOTYPE(vrd2_log1p)
#define fname_vrs4_logf  FN_PROTOTYPE(vrs4_logf)
#define fname_vrs4_log2f  FN_PROTOTYPE(vrs4_log2f)
#define fname_vrs4_log10f  FN_PROTOTYPE(vrs4_log10f)
#define fname_vrs4_log1pf  FN_PROTOTYPE(vrs4_log1pf)
#define fname_logf FN_PROTOTYPE(logf)
#define fname_log2f FN_PROTOTYPE(log2f)
#define fname_log10f FN_PROTOTYPE(log10f)
#define fname_log1pf FN_PROTOTYPE(log1pf)
#define fname_log FN_PROTOTYPE(log)
#define fname_log2 FN_PROTOTYPE(log2)
#define fname_log10 FN_PROTOTYPE(log10)
#define fname_log1p FN_PROTOTYPE(log1p)

#define fname_cbrt  FN_PROTOTYPE(cbrt)
#define fname_cbrtf  FN_PROTOTYPE(cbrtf)
#define fname_vrd2_cbrt  FN_PROTOTYPE(vrd2_cbrt)
#define fname_vrs4_cbrtf  FN_PROTOTYPE(vrs4_cbrtf)

#define fname_cos  FN_PROTOTYPE(cos)
#define fname_sin  FN_PROTOTYPE(sin)
#define fname_tan  FN_PROTOTYPE(tan)
#define fname_sincos  FN_PROTOTYPE(sincos)
#define fname_sincosf  FN_PROTOTYPE(sincosf)

#define fname_vrd2_cos  FN_PROTOTYPE(vrd2_cos)
#define fname_vrd2_sin  FN_PROTOTYPE(vrd2_sin)
#define fname_vrd2_tan  FN_PROTOTYPE(vrd2_tan)


#define fname_cosf  FN_PROTOTYPE(cosf)
#define fname_sinf  FN_PROTOTYPE(sinf)
#define fname_tanf  FN_PROTOTYPE(tanf)

#define fname_vrs4_cosf  FN_PROTOTYPE(vrs4_cosf)
#define fname_vrs4_sinf  FN_PROTOTYPE(vrs4_sinf)
#define fname_vrs4_tanf  FN_PROTOTYPE(vrs4_tanf)

#define fname_pow  FN_PROTOTYPE(pow)
#define fname_fastpow  FN_PROTOTYPE(fastpow)
#define fname_powf  FN_PROTOTYPE(powf)

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname_fmaf
.type fname_fmaf,@function
fname_fmaf:

    mov g_amd_libm_ep_fmaf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_fma
.type fname_fma,@function
fname_fma:

    mov g_amd_libm_ep_fma@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_exp
.type fname_exp,@function
fname_exp:

    mov g_amd_libm_ep_exp@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_exp2
.type fname_exp2,@function
fname_exp2:

    mov g_amd_libm_ep_exp2@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_exp10
.type fname_exp10,@function
fname_exp10:

    mov g_amd_libm_ep_exp10@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_expm1
.type fname_expm1,@function
fname_expm1:

    mov g_amd_libm_ep_expm1@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_expf
.type fname_expf,@function
fname_expf:

    mov g_amd_libm_ep_expf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_exp2f
.type fname_exp2f,@function
fname_exp2f:

    mov g_amd_libm_ep_exp2f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_exp10f
.type fname_exp10f,@function
fname_exp10f:

    mov g_amd_libm_ep_exp10f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_expm1f
.type fname_expm1f,@function
fname_expm1f:

    mov g_amd_libm_ep_expm1f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_expf
.type fname_vrs4_expf,@function
fname_vrs4_expf:

    mov g_amd_libm_ep_vrs4_expf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_exp2f
.type fname_vrs4_exp2f,@function
fname_vrs4_exp2f:

    mov g_amd_libm_ep_vrs4_exp2f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_exp10f
.type fname_vrs4_exp10f,@function
fname_vrs4_exp10f:

    mov g_amd_libm_ep_vrs4_exp10f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_expm1f
.type fname_vrs4_expm1f,@function
fname_vrs4_expm1f:

    mov g_amd_libm_ep_vrs4_expm1f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_exp
.type fname_vrd2_exp,@function
fname_vrd2_exp:

    mov g_amd_libm_ep_vrd2_exp@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_exp2
.type fname_vrd2_exp2,@function
fname_vrd2_exp2:

    mov g_amd_libm_ep_vrd2_exp2@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_exp10
.type fname_vrd2_exp10,@function
fname_vrd2_exp10:

    mov g_amd_libm_ep_vrd2_exp10@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_expm1
.type fname_vrd2_expm1,@function
fname_vrd2_expm1:

    mov g_amd_libm_ep_vrd2_expm1@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrda_exp
.type fname_vrda_exp,@function
fname_vrda_exp:

    mov g_amd_libm_ep_vrda_exp@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrda_exp2
.type fname_vrda_exp2,@function
fname_vrda_exp2:

    mov g_amd_libm_ep_vrda_exp2@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrda_exp10
.type fname_vrda_exp10,@function
fname_vrda_exp10:

    mov g_amd_libm_ep_vrda_exp10@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrda_expm1
.type fname_vrda_expm1,@function
fname_vrda_expm1:

    mov g_amd_libm_ep_vrda_expm1@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrda_sin
.type fname_vrda_sin,@function
fname_vrda_sin:

    mov g_amd_libm_ep_vrda_sin@GOTPCREL(%rip), %rax
    jmp *(%rax)



.globl fname_vrda_cos
.type fname_vrda_cos,@function
fname_vrda_cos:

    mov g_amd_libm_ep_vrda_cos@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_vrda_cbrt
.type fname_vrda_cbrt,@function
fname_vrda_cbrt:

    mov g_amd_libm_ep_vrda_cbrt@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrda_log10
.type fname_vrda_log10,@function
fname_vrda_log10:

    mov g_amd_libm_ep_vrda_log10@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_vrda_log
.type fname_vrda_log,@function
fname_vrda_log:

    mov g_amd_libm_ep_vrda_log@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_vrda_log1p
.type fname_vrda_log1p,@function
fname_vrda_log1p:

    mov g_amd_libm_ep_vrda_log1p@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_vrda_log2
.type fname_vrda_log2,@function
fname_vrda_log2:

    mov g_amd_libm_ep_vrda_log2@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_expf
.type fname_vrsa_expf,@function
fname_vrsa_expf:

    mov g_amd_libm_ep_vrsa_expf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrsa_exp2f
.type fname_vrsa_exp2f,@function
fname_vrsa_exp2f:

    mov g_amd_libm_ep_vrsa_exp2f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrsa_exp10f
.type fname_vrsa_exp10f,@function
fname_vrsa_exp10f:

    mov g_amd_libm_ep_vrsa_exp10f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrsa_expm1f
.type fname_vrsa_expm1f,@function
fname_vrsa_expm1f:

    mov g_amd_libm_ep_vrsa_expm1f@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_log1pf
.type fname_vrsa_log1pf,@function
fname_vrsa_log1pf:

    mov g_amd_libm_ep_vrsa_log1pf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_log2f
.type fname_vrsa_log2f,@function
fname_vrsa_log2f:

    mov g_amd_libm_ep_vrsa_log2f@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_cbrtf
.type fname_vrsa_cbrtf,@function
fname_vrsa_cbrtf:

    mov g_amd_libm_ep_vrsa_cbrtf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_log10f
.type fname_vrsa_log10f,@function
fname_vrsa_log10f:

    mov g_amd_libm_ep_vrsa_log10f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrsa_logf
.type fname_vrsa_logf,@function
fname_vrsa_logf:

    mov g_amd_libm_ep_vrsa_logf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_sinf
.type fname_vrsa_sinf,@function
fname_vrsa_sinf:

    mov g_amd_libm_ep_vrsa_sinf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrsa_cosf
.type fname_vrsa_cosf,@function
fname_vrsa_cosf:

    mov g_amd_libm_ep_vrsa_cosf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrd2_log
.type fname_vrd2_log,@function
fname_vrd2_log:

    mov g_amd_libm_ep_vrd2_log@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_log2
.type fname_vrd2_log2,@function
fname_vrd2_log2:

    mov g_amd_libm_ep_vrd2_log2@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_log10
.type fname_vrd2_log10,@function
fname_vrd2_log10:

    mov g_amd_libm_ep_vrd2_log10@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_log1p
.type fname_vrd2_log1p,@function
fname_vrd2_log1p:

    mov g_amd_libm_ep_vrd2_log1p@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrs4_logf
.type fname_vrs4_logf,@function
fname_vrs4_logf:

    mov g_amd_libm_ep_vrs4_logf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_log2f
.type fname_vrs4_log2f,@function
fname_vrs4_log2f:

    mov g_amd_libm_ep_vrs4_log2f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_log10f
.type fname_vrs4_log10f,@function
fname_vrs4_log10f:

    mov g_amd_libm_ep_vrs4_log10f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_log1pf
.type fname_vrs4_log1pf,@function
fname_vrs4_log1pf:

    mov g_amd_libm_ep_vrs4_log1pf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_logf
.type fname_logf,@function
fname_logf:

    mov g_amd_libm_ep_logf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_log2f
.type fname_log2f,@function
fname_log2f:

    mov g_amd_libm_ep_log2f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_log10f
.type fname_log10f,@function
fname_log10f:

    mov g_amd_libm_ep_log10f@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_log1pf
.type fname_log1pf,@function
fname_log1pf:

    mov g_amd_libm_ep_log1pf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_log
.type fname_log,@function
fname_log:

    mov g_amd_libm_ep_log@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_log2
.type fname_log2,@function
fname_log2:

    mov g_amd_libm_ep_log2@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_log10
.type fname_log10,@function
fname_log10:

    mov g_amd_libm_ep_log10@GOTPCREL(%rip), %rax
    jmp *(%rax)

 .p2align 4
.globl fname_log1p
.type fname_log1p,@function
fname_log1p:

    mov g_amd_libm_ep_log1p@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_cbrt
.type fname_cbrt,@function
fname_cbrt:

    mov g_amd_libm_ep_cbrt@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_cbrtf
.type fname_cbrtf,@function
fname_cbrtf:

    mov g_amd_libm_ep_cbrtf@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_cbrt
.type fname_vrd2_cbrt,@function
fname_vrd2_cbrt:

    mov g_amd_libm_ep_vrd2_cbrt@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrs4_cbrtf
.type fname_vrs4_cbrtf,@function
fname_vrs4_cbrtf:

    mov g_amd_libm_ep_vrs4_cbrtf@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_cos
.type fname_cos,@function
fname_cos:

    mov g_amd_libm_ep_cos@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_sin
.type fname_sin,@function
fname_sin:

    mov g_amd_libm_ep_sin@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_sincos
.type fname_sincos,@function
fname_sincos:

    mov g_amd_libm_ep_sincos@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_sincosf
.type fname_sincosf,@function
fname_sincosf:

    mov g_amd_libm_ep_sincosf@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_tan
.type fname_tan,@function
fname_tan:

    mov g_amd_libm_ep_tan@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_vrd2_sin
.type fname_vrd2_sin,@function
fname_vrd2_sin:

    mov g_amd_libm_ep_vrd2_sin@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_vrd2_cos
.type fname_vrd2_cos,@function
fname_vrd2_cos:

    mov g_amd_libm_ep_vrd2_cos@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrd2_tan
.type fname_vrd2_tan,@function
fname_vrd2_tan:

    mov g_amd_libm_ep_vrd2_tan@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_cosf
.type fname_cosf,@function
fname_cosf:

    mov g_amd_libm_ep_cosf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_sinf
.type fname_sinf,@function
fname_sinf:

    mov g_amd_libm_ep_sinf@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_tanf
.type fname_tanf,@function
fname_tanf:

    mov g_amd_libm_ep_tanf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrs4_cosf
.type fname_vrs4_cosf,@function
fname_vrs4_cosf:

    mov g_amd_libm_ep_vrs4_cosf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_vrs4_sinf
.type fname_vrs4_sinf,@function
fname_vrs4_sinf:

    mov g_amd_libm_ep_vrs4_sinf@GOTPCREL(%rip), %rax
    jmp *(%rax)



.p2align 4
.globl fname_vrs4_tanf
.type fname_vrs4_tanf,@function
fname_vrs4_tanf:

    mov g_amd_libm_ep_vrs4_tanf@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_pow
.type fname_pow,@function
fname_pow:

    mov g_amd_libm_ep_pow@GOTPCREL(%rip), %rax
    jmp *(%rax)

.p2align 4
.globl fname_fastpow
.type fname_fastpow,@function
fname_fastpow:

    mov g_amd_libm_ep_fastpow@GOTPCREL(%rip), %rax
    jmp *(%rax)


.p2align 4
.globl fname_powf
.type fname_powf,@function
fname_powf:

    mov g_amd_libm_ep_powf@GOTPCREL(%rip), %rax
    jmp *(%rax)

# All the Scalar Functions in AMD_LIBM are aliased to the lm math library equivalent
WEAK_MATH_LIB_ALIAS cbrt, fname_cbrt
WEAK_MATH_LIB_ALIAS cbrtf, fname_cbrtf
WEAK_MATH_LIB_ALIAS sin,fname_sin
WEAK_MATH_LIB_ALIAS cos, fname_cos
WEAK_MATH_LIB_ALIAS sincos,fname_sincos
WEAK_MATH_LIB_ALIAS sincosf,fname_sincosf
WEAK_MATH_LIB_ALIAS sinf,fname_sinf
WEAK_MATH_LIB_ALIAS cosf, fname_cosf
WEAK_MATH_LIB_ALIAS exp, fname_exp
WEAK_MATH_LIB_ALIAS expf, fname_expf
WEAK_MATH_LIB_ALIAS expm1, fname_expm1
WEAK_MATH_LIB_ALIAS expm1f, fname_expm1f
WEAK_MATH_LIB_ALIAS exp2, fname_exp2
WEAK_MATH_LIB_ALIAS exp2f, fname_exp2f
WEAK_MATH_LIB_ALIAS exp10, fname_exp10
WEAK_MATH_LIB_ALIAS exp10f, fname_exp10f
WEAK_MATH_LIB_ALIAS fma, fname_fma
WEAK_MATH_LIB_ALIAS fmaf, fname_fmaf
WEAK_MATH_LIB_ALIAS log, fname_log
WEAK_MATH_LIB_ALIAS logf, fname_logf
WEAK_MATH_LIB_ALIAS log2, fname_log2
WEAK_MATH_LIB_ALIAS log2f, fname_log2f
WEAK_MATH_LIB_ALIAS log10, fname_log10
WEAK_MATH_LIB_ALIAS log10f, fname_log10f
WEAK_MATH_LIB_ALIAS log1p, fname_log1p
WEAK_MATH_LIB_ALIAS log1pf, fname_log1pf
//WEAK_MATH_LIB_ALIAS pow, fname_pow    // VK Commented out- amd_zen_pow() is aliased to "pow"
WEAK_MATH_LIB_ALIAS powf, fname_powf
WEAK_MATH_LIB_ALIAS tan, fname_tan
WEAK_MATH_LIB_ALIAS tanf, fname_tanf
WEAK_MATH_LIB_ALIAS fastpow, fname_fastpow



# ACML Aliases are created here
WEAK_ACML_LIB_ALIAS fastcbrt,fname_cbrt
WEAK_ACML_LIB_ALIAS fastcbrtf, fname_cbrtf
WEAK_ACML_LIB_ALIAS fastsin,fname_sin
WEAK_ACML_LIB_ALIAS fastsincos,fname_sincos
WEAK_ACML_LIB_ALIAS fastsincos,fname_sincosf
WEAK_ACML_LIB_ALIAS fastcos,fname_cos
WEAK_ACML_LIB_ALIAS fastsinf,fname_sinf
WEAK_ACML_LIB_ALIAS fastcosf, fname_cosf
WEAK_ACML_LIB_ALIAS fastexp,fname_exp

WEAK_ACML_LIB_ALIAS fastexpf,fname_expf
WEAK_ACML_LIB_ALIAS fastexpm1, fname_expm1
WEAK_ACML_LIB_ALIAS fastexpm1f, fname_expm1f
WEAK_ACML_LIB_ALIAS fastexp2, fname_exp2
WEAK_ACML_LIB_ALIAS fastexp2f, fname_exp2f
WEAK_ACML_LIB_ALIAS fastexp10, fname_exp10
WEAK_ACML_LIB_ALIAS fastexp10f, fname_exp10f
WEAK_ACML_LIB_ALIAS fastfma, fname_fma
WEAK_ACML_LIB_ALIAS fastfmaf, fname_fmaf
WEAK_ACML_LIB_ALIAS fastlog, fname_log
WEAK_ACML_LIB_ALIAS fastlogf,fname_logf
WEAK_ACML_LIB_ALIAS fastlog2, fname_log2
WEAK_ACML_LIB_ALIAS fastlog2f, fname_log2f
WEAK_ACML_LIB_ALIAS fastlog10, fname_log10
WEAK_ACML_LIB_ALIAS fastlog10f,fname_log10f
WEAK_ACML_LIB_ALIAS fastlog1p, fname_log1p
WEAK_ACML_LIB_ALIAS fastlog1pf, fname_log1pf
#WEAK_ACML_LIB_ALIAS fastpow, fname_fastpow
#WEAK_ACML_LIB_ALIAS fastpowf, fname_powf
WEAK_ACML_LIB_ALIAS fasttan, fname_tan
WEAK_ACML_LIB_ALIAS fasttanf, fname_tanf

WEAK_ACML_LIB_ALIAS __vrd2_exp,fname_vrd2_exp
WEAK_ACML_LIB_ALIAS __vrd2_log,fname_vrd2_log
WEAK_ACML_LIB_ALIAS __vrd2_log2,fname_vrd2_log2
WEAK_ACML_LIB_ALIAS __vrd2_log10,fname_vrd2_log10
WEAK_ACML_LIB_ALIAS __vrd2_sin,fname_vrd2_sin
WEAK_ACML_LIB_ALIAS __vrd2_cos,fname_vrd2_cos
WEAK_ACML_LIB_ALIAS __vrd2_cos,fname_vrd2_cos

WEAK_ACML_LIB_ALIAS __vrs4_expf,fname_vrs4_expf
WEAK_ACML_LIB_ALIAS __vrs4_logf,fname_vrs4_logf
WEAK_ACML_LIB_ALIAS __vrs4_log2f,fname_vrs4_log2f
WEAK_ACML_LIB_ALIAS __vrs4_log10f,fname_vrs4_log10f
WEAK_ACML_LIB_ALIAS __vrs4_sinf,fname_vrs4_sinf
WEAK_ACML_LIB_ALIAS __vrs4_cosf,fname_vrs4_cosf



WEAK_ACML_LIB_ALIAS vrsa_expf_,fname_vrsa_expf
WEAK_ACML_LIB_ALIAS vrda_exp_,fname_vrda_exp
WEAK_ACML_LIB_ALIAS vrda_exp,fname_vrda_exp


WEAK_ACML_LIB_ALIAS __vrd2_cbrt,FN_PROTOTYPE(vrd2_cbrt)
WEAK_ACML_LIB_ALIAS __vrd2_cos,FN_PROTOTYPE(vrd2_cos)
WEAK_ACML_LIB_ALIAS __vrd2_exp,FN_PROTOTYPE(vrd2_exp)
WEAK_ACML_LIB_ALIAS __vrd2_exp10,FN_PROTOTYPE(vrd2_exp10)
WEAK_ACML_LIB_ALIAS __vrd2_exp2,FN_PROTOTYPE(vrd2_exp2)
WEAK_ACML_LIB_ALIAS __vrd2_expm1,FN_PROTOTYPE(vrd2_expm1)
WEAK_ACML_LIB_ALIAS __vrd2_log,FN_PROTOTYPE(vrd2_log)
WEAK_ACML_LIB_ALIAS __vrd2_log10,FN_PROTOTYPE(vrd2_log10)
WEAK_ACML_LIB_ALIAS __vrd2_log1p,FN_PROTOTYPE(vrd2_log1p)
WEAK_ACML_LIB_ALIAS __vrd2_log2,FN_PROTOTYPE(vrd2_log2)
WEAK_ACML_LIB_ALIAS __vrd2_sin,FN_PROTOTYPE(vrd2_sin)
WEAK_ACML_LIB_ALIAS vrda_cos,FN_PROTOTYPE(vrda_cos)
WEAK_ACML_LIB_ALIAS vrda_exp_,FN_PROTOTYPE(vrda_exp)
WEAK_ACML_LIB_ALIAS vrda_exp,FN_PROTOTYPE(vrda_exp)
WEAK_ACML_LIB_ALIAS vrda_log_,FN_PROTOTYPE(vrda_log)
WEAK_ACML_LIB_ALIAS vrda_log,FN_PROTOTYPE(vrda_log)
WEAK_ACML_LIB_ALIAS vrda_log10,FN_PROTOTYPE(vrda_log10)
WEAK_ACML_LIB_ALIAS vrda_log2,FN_PROTOTYPE(vrda_log2)
WEAK_ACML_LIB_ALIAS vrda_sin,FN_PROTOTYPE(vrda_sin)
WEAK_ACML_LIB_ALIAS __vrs4_cbrtf,FN_PROTOTYPE(vrs4_cbrtf)
WEAK_ACML_LIB_ALIAS __vrs4_cosf,FN_PROTOTYPE(vrs4_cosf)
WEAK_ACML_LIB_ALIAS __vrs4_exp10f,FN_PROTOTYPE(vrs4_exp10f)
WEAK_ACML_LIB_ALIAS __vrs4_exp2f,FN_PROTOTYPE(vrs4_exp2f)
WEAK_ACML_LIB_ALIAS __vrs4_expf,FN_PROTOTYPE(vrs4_expf)
WEAK_ACML_LIB_ALIAS __vrs4_expm1f,FN_PROTOTYPE(vrs4_expm1f)
WEAK_ACML_LIB_ALIAS __vrs4_log10f,FN_PROTOTYPE(vrs4_log10f)
WEAK_ACML_LIB_ALIAS __vrs4_log1pf,FN_PROTOTYPE(vrs4_log1pf)
WEAK_ACML_LIB_ALIAS __vrs4_log2f,FN_PROTOTYPE(vrs4_log2f)
WEAK_ACML_LIB_ALIAS __vrs4_logf,FN_PROTOTYPE(vrs4_logf)
WEAK_ACML_LIB_ALIAS __vrs4_sinf,FN_PROTOTYPE(vrs4_sinf)
WEAK_ACML_LIB_ALIAS vrsa_cosf,FN_PROTOTYPE(vrsa_cosf)
WEAK_ACML_LIB_ALIAS vrsa_expf_,FN_PROTOTYPE(vrsa_expf)
WEAK_ACML_LIB_ALIAS vrsa_expf,FN_PROTOTYPE(vrsa_expf)
WEAK_ACML_LIB_ALIAS vrsa_log10f,FN_PROTOTYPE(vrsa_log10f)
WEAK_ACML_LIB_ALIAS vrsa_log2f,FN_PROTOTYPE(vrsa_log2f)
WEAK_ACML_LIB_ALIAS vrsa_logf_,FN_PROTOTYPE(vrsa_logf)
WEAK_ACML_LIB_ALIAS vrsa_logf,FN_PROTOTYPE(vrsa_logf)
WEAK_ACML_LIB_ALIAS vrsa_sinf,FN_PROTOTYPE(vrsa_sinf)
