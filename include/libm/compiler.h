#ifndef __AMD_LIBM_COMPILER_H__
#define __AMD_LIBM_COMPILER_H__

#ifdef __GNUC__
#define ALIGN(x)        __attribute__((aligned ((x))))

#define RODATA          __attribute__((section (".rodata")))

#define HIDDEN         __attribute__ ((__visibility__ ("hidden")))
#define NOINLINE       __attribute__ ((noinline))
#define likely(x)      __builtin_expect (!!(x), 1)
#define unlikely(x)    __builtin_expect (x, 0)
#define strong_alias(f, a)					\
	extern __typeof (f) a __attribute__ ((alias (#f)));
#define hidden_alias(f, a)						\
	extern __typeof (f) a __attribute__ ((alias (#f), visibility ("hidden")));
#endif

#endif	/* AMD_LIBM_COMPILER_H */
