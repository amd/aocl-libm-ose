#ifndef __AMD_LIBM_COMPILER_H__
#define __AMD_LIBM_COMPILER_H__

/*
To check for which OS the code is compiled:
Linux and Linux-derived           __linux__
Android                           __ANDROID__ (implies __linux__)
Linux (non-Android)               __linux__ && !__ANDROID__
Darwin (Mac OS X and iOS)         __APPLE__
Akaros (http://akaros.org)        __ros__
Windows                           _WIN32
Windows 64 bit                    _WIN64 (implies _WIN32)
NaCL                              __native_client__
AsmJS                             __asmjs__
Fuschia                           __Fuchsia__


To check which compiler is used:
Visual Studio       _MSC_VER
gcc                 __GNUC__
clang               __clang__
emscripten          __EMSCRIPTEN__ (for asm.js and webassembly)
MinGW 32            __MINGW32__
MinGW-w64 32bit     __MINGW32__
MinGW-w64 64bit     __MINGW64__


To check that this is gcc compiler version 5.1 or greater:
#if defined(__GNUC__) && (__GNUC___ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ >= 1))
// this is gcc 5.1 or greater
#endif

*/

#if defined(__GNUC__)

#define PACKED		__attribute__((packed))

#define PACK_ALIGNED(x) __attribute__((packed, aligned(x)))
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
#define NO_OPTIMIZE __attribute__((optimize("0"))

#define CONSTRUCTOR __attribute__((constructor))
#define INITIALIZER(f) static void f(void) CONSTRUCTOR; static void f(void)


#define WEAK_LIBM_ALIAS(x, y)                                   \
        asm("\n\t"".weak " STRINGIFY(x)                         \
            "\n\t"".set " STRINGIFY(x) ", " STRINGIFY(y)        \
            "\n\t");

#if defined(__clang__)
    #define FALLTHROUGH
    #undef NO_OPTIMIZE
    #define NO_OPTIMIZE __attribute__((optnone))
#else
    #define FALLTHROUGH __attribute__((fallthrough))
    #undef NO_OPTIMIZE
    #define NO_OPTIMIZE __attribute__((optimize("O0")))
#endif

#elif defined(__MSVC) && defined(_MSC_VER)
#include <intrin.h>
#define CPUID __cpuid
#define CCALL __cdecl
#pragma section(".CRT$XCU",read)
#define INITIALIZER(f)  static void __cdecl f(void); __declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; static void __cdecl f(void)


#define FALLTHROUGH

#endif	/* GCC */

#endif	/* AMD_LIBM_COMPILER_H */
