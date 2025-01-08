# AOCL-LibM

AOCL-LibM is a high-performant implementation of LibM, the standard C library of basic floating-point mathematical functions. It includes many of the functions from the C99 standard. Single and double precision versions of the functions are provided, all optimized for accuracy and performance, including a small number of complex functions. There are also a number of vector and fast scalar variants provided, in which a small amount of the accuracy has been traded for greater performance.

A list of the scalar functions present in the library is provided below. Note that an "f" at the end of the function name indicates that it is single-precision; otherwise, it is double-precision. They can be called by a standard C99 function call and naming convention and must be linked with AOCL-LibM before standard libm.

For example:

    $ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/path/to/AOCL-LibM_library
    $ clang -Wall -std=c99 myprogram.c -o myprogram -L<Path to AOCL-LibM Library> -lalm -lm
    Or
    $ gcc -Wall -std=c99 myprogram.c -o myprogram -L<Path to AOCL-LibM Library> -lalm -lm

- Trigonometric
  - cosf, cos, sinf, sin, tanf, tan, sincosf and sincos

- Inverse Trigonometric
  - acosf, acos, asinf, asin, atanf, atan, atan2f and atan2

- Hyperbolic
  - coshf, cosh, sinhf, sinh, tanhf and tanh

- Inverse Hyperbolic
  - acoshf, acosh, asinhf, asinh, atanhf and atanh

- Exponential and Logarithmic
  - expf, exp, exp2f, exp2, exp10f, exp10, expm1f and expm1
  - logf, log, log10f, log10, log2f, log2, log1pf and log1p
  - logbf, logb, ilogbf and ilogb
  - modff, modf, frexpf, frexp, ldexpf and ldexp
  - scalbnf, scalbn, scalblnf and scalbln

- Error
  - erff and erf

- Power and Absolute Value
  - powf, pow, fastpow, cbrtf, cbrt, sqrtf, sqrt, hypotf and hypot
  - fabsf and fabs

- Nearest Integer
  - ceilf, ceil, floorf, floor, truncf and trunc
  - rintf, rint, roundf, round, nearbyintf and nearbyint
  - lrintf, lrint, llrintf and llrint
  - lroundf, lround, llroundf and llround

- Remainder
  - fmodf, fmod, remainderf and remainder

- Manipulation
  - copysignf, copysign, nanf, nan, finitef, and finite
  - nextafterf, nextafter, nexttowardf and nexttoward

- Maximum, Minimum and Difference
  - fdimf, fdim, fmaxf, fmax, fminf and fmin

## INSTALLATION

Please refer to the file BUILDING-ose.txt.

## CONTACTS

AOCL-LibM is developed and maintained by AMD. For support of these libraries and the other tools of AMD Zen Software Studio, see https://www.amd.com/en/developer/aocc/compiler-technical-support.html

