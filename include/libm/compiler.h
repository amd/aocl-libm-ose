#ifndef __AMD_LIBM_COMPILER_H__
#define __AMD_LIBM_COMPILER_H__

#if defined(__GNUC__)
#define ALIGN(x)        __attribute__((aligned ((x))))

#define RODATA          __attribute__((section (".rodata")))

#define HIDDEN         __attribute__ ((__visibility__ ("hidden")))
#define NOINLINE       __attribute__ ((noinline))
#define likely(x)      __builtin_expect (!!(x), 1)
#define unlikely(x)    __builtin_expect (x, 0)


#define strong_alias(aliasname, name)  _strong_alias(aliasname, name)

#define _strong_alias(al_name, name)					\
           extern __typeof (name) al_name __attribute__ ((alias (#name)));

#define hidden_alias(al_name, name)    \
       extern __typeof (name) al_name  \
                       __attribute__ ((alias (#al_name), visibility ("hidden")));

#define OPT_O1 __attribute__((optimize("O1")))
#define OPT_O2 __attribute__((optimize("O2")))
#define OPT_O3 __attribute__((optimize("O3")))
#define OPT_Og __attribute__((optimize("Og")))

#define OPTIMIZE(x) __attribute__((optimize(x)))

#define NO_OPTIMIZE __attribute__((optimize("O0")))

#elif defined(__CLANGC__)

#define NO_OPTIMIZE __attribute__((optnone))

#endif	/* GCC */

#endif	/* AMD_LIBM_COMPILER_H */
