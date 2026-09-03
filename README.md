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

The recommended build system is CMake. See `BUILDING.md` for scons reference and
`docs/CMakeBuildSystem.md` for the cmake reference.

> **Note (changed in this release):** AOCL-LibM no longer depends on the external
> `aocl-utils` library. Runtime CPU detection is now handled by an internal, pure-C
> module, so building/installing `aocl-utils` and its related build flags
> (`--aocl_utils_install_path`, `--aocl_utils_link`, `-DAOCL_UTILS_INCLUDE_DIR`,
> `-DAOCL_UTILS_LIB`) are no longer required. If you are upgrading from a previous
> release, you can drop any `aocl-utils` configuration; see
> `docs/design/InternalUtilsDesign.md` for migration details.

### Quick start

Building the library alone needs only CMake and a C/C++ compiler; the
multi-precision packages (MPFR, GMP, MPC) are required only for the test
framework. All optional components (tests and examples) are off by default.

#### Build the library

**Linux:**

```sh
cmake --preset dev-release-gcc --fresh
cmake --build --preset dev-release-gcc -j
```

**Windows:**

```bat
cmake --preset dev-win-release-llvm-ninja --fresh
cmake --build --preset dev-win-release-llvm-ninja -j
```

The built libraries are placed under `build/<presetName>/lib`.

#### Build the library and examples

The examples are off by default, so enable them at configure time with
`-DLIBM_BUILD_EXAMPLES=ON`. Always build the library first, then the `test_libm`
target, then run the example. The full, copy-pasteable sequence is:

**Linux:**

```sh
# 1. Configure with examples enabled
cmake --preset dev-release-gcc -DLIBM_BUILD_EXAMPLES=ON --fresh
# 2. Build the library
cmake --build --preset dev-release-gcc
# 3. Build the example (library must already be built)
cmake --build --preset dev-release-gcc --target test_libm
# 4. Run the example
export LD_LIBRARY_PATH=${PWD}/build/dev-release-gcc/lib:$LD_LIBRARY_PATH
./build/dev-release-gcc/examples/test_libm
```

**Windows (Ninja generator):**

```bat
REM i.  Set up the Visual Studio x64 build environment (provides Ninja and clang-cl)
"<path_to_visualstudio>\VC\Auxiliary\Build\vcvarsall.bat" x64
REM 1. Configure with examples enabled
cmake --preset dev-win-release-llvm-ninja -DLIBM_BUILD_EXAMPLES=ON --fresh
REM 2. Build the library
cmake --build --preset dev-win-release-llvm-ninja
REM 3. Build the example (library must already be built)
cmake --build --preset dev-win-release-llvm-ninja --target test_libm
REM 4. Run the example
set PATH=%PATH%;%CD%\build\dev-win-release-llvm-ninja\lib
build\dev-win-release-llvm-ninja\examples\test_libm.exe
```

For the full list of targets, build options and other details, see
`docs/CMakeBuildSystem.md`.

## HELP

## CONTACTS

AOCL-LibM is developed and maintained by AMD. For support of these libraries and the other tools of AMD Zen Software Studio, see https://www.amd.com/en/developer/aocc/compiler-technical-support.html

