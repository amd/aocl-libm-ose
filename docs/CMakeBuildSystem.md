# Building AOCL-LIBM with CMake

This comprehensive guide provides instructions for building, testing, and installing the AOCL-LIBM library using CMake on both Linux and Windows platforms.

---

## **Table of Contents**

1. [Requirements](#1-requirements)
2. [Platform-Specific Setup](#2-platform-specific-setup)
3. [Quick Start Commands](#3-quick-start-commands)
4. [Building With CMake Presets](#4-building-with-cmake-presets)
   - 4.1 [List Presets](#41-list-presets)
   - 4.2 [Configure CMake](#42-configure-cmake)
   - 4.3 [Build the Library](#43-build-the-library)
   - 4.4 [Installation](#44-installation)
   - 4.5 [Building and Executing gtests (Libm Test Framework)](#45-building-and-executing-gtests-libm-test-framework)
      - 4.5.1 [Building gtests](#451-building-gtests)
      - 4.5.2 [Executing gtests](#452-executing-gtests)
      - 4.5.3 [Building with Alternative Library ABIs (Linux Only)](#453-building-with-alternative-library-abis-linux-only)
      - 4.5.4 [Building with Intel Math (OneAPI) Libraries (Linux Only)](#454-building-with-intel-math-oneapi-libraries-linux-only)
      - 4.5.5 [Using Preloadable Libraries (Linux Only)](#455-using-preloadable-libraries-linux-only)
   - 4.6 [Building Examples](#46-building-examples)
   - 4.7 [Clean Build Files](#47-clean-build-files)
   - 4.8 [Static Dispatch Configuration (Linux Only)](#48-static-dispatch-configuration-linux-only)
   - 4.9 [Developer Features (Linux Only)](#49-developer-features-linux-only)
      - 4.9.1 [AddressSanitizer (ASAN)](#491-addresssanitizer-asan)
      - 4.9.2 [Code Coverage](#492-code-coverage)
      - 4.9.3 [Building Documentation](#493-building-documentation)
5. [CMake Configuration Options](#5-cmake-configuration-options)
6. [Manually Building Without Presets](#6-manually-building-without-presets)
   - 6.1 [Create a Build Directory](#61-create-a-build-directory)
   - 6.2 [Configure CMake Manually](#62-configure-cmake-manually)
   - 6.3 [Build the Library](#63-build-the-library)
   - 6.4 [Install](#64-install)
   - 6.5 [Build Examples (Standalone)](#65-build-examples-standalone)
   - 6.6 [Running Examples](#66-running-examples)
   - 6.7 [Clean Build](#67-clean-build)

---

## **1. Requirements**

| Component         | Version/Requirement    | Linux | Windows | Notes                                  |
|-------------------|------------------------|:-----:|:-------:|----------------------------------------|
| CMake             | ≥ 3.26                 |   ✓   |    ✓    | Required                               |
| GCC               | ≥ 9.2 and ≤ 14.1.0     |   ✓   |    ✗    | Linux compiler option                  |
| Clang             | ≥ 9.0 and ≤ 18.1.0     |   ✓   |    ✗    | Linux compiler option                  |
| LLVM (Clang-CL)   | ≥ 9.0 and ≤ 18.1.0     |   ✗   |    ✓    | Windows compiler (clang-cl.exe)        |
| MPFR              | Latest                 |   ✓   |    ✓    | Path must be set (especially Windows)  |
| GMP               | Latest                 |   ✓   |    ✓    | Dependency of MPFR                     |
| MPC               | Latest                 |   ✓   |    ✓    | Dependency of MPFR                     |
| Visual Studio     | 2019 or later          |   ✗   |    ✓    | Provides Ninja build tool              |

**Note:**
  * CMake automatically fetches aocl-utils from GitHub during configuration and link statically, or one can link dynamically from an installed aocl-utils package.
  * On Windows, the build system uses the inbuilt Ninja tool provided by Visual Studio

---

## **2. Platform-Specific Setup**

**Windows Only:**
1.  Open a command prompt and navigate to the `aocl-libm` directory.
2.  Run the Visual Studio `vcvarsall.bat` script (replace `<path_to_visualstudio>` with your actual installation path):

```console
"<path_to_visualstudio>\VC\Auxiliary\Build\vcvarsall.bat" x64
```

3.  Set the `MPFR_PATH` environment variable.

AOCL LibM test framework uses the following libraries:
- GNU MPFR (an open source multi-precision floating point library)
- GNU GMP (GNU Multiple Precision Arithmetic library)
- GNU MPC (C library for the arithmetic of complex numbers library)

```console
set MPFR_PATH=<path_to_mpfr>\mpfr
```

**Linux Only:**

No specific setup required.

---

## **3. Quick Start Commands**

**Linux:**
```console
$ cmake --preset dev-release-gcc --fresh
$ cmake --build --preset dev-release-gcc -j
```

**Windows:**
```console
$ cmake --preset dev-win-release-llvm --fresh
$ cmake --build --preset dev-win-release-llvm -j
```

---

## **4. Building With CMake presets**

**Step-by-Step Build and Test Guide**

#### **4.1 List Presets**
To view the available configuration presets, run:
```console
$ cmake --list-presets
```

**Available Presets:**

| Preset Name             | Platform | Compiler | Build Type | Description                           |
|-------------------------|----------|----------|------------|---------------------------------------|
| `dev-gcc`               | Linux    | GCC      | Debug      | Developer Config with GCC-Debug       |
| `dev-clang`             | Linux    | Clang    | Debug      | Developer Config with Clang-Debug     |
| `dev-release-gcc`       | Linux    | GCC      | Release    | Developer Config with GCC-Release     |
| `dev-release-clang`     | Linux    | Clang    | Release    | Developer Config with Clang-Release   |
| `dev-win-llvm`          | Windows  | LLVM     | Debug      | Developer Config with LLVM-Debug      |
| `dev-win-release-llvm`  | Windows  | LLVM     | Release    | Developer Config with LLVM-Release    |

#### **4.2 Configure CMake**

One can configure the cmake using preset with or without additional CMake options.

**Option 1: Configure with Preset Only (Use Default Settings)**
```console
$ cmake --preset {presetName} --fresh
```

**Option 2: Configure with Preset + Additional CMake Options**

You can override preset defaults or add additional configuration options:
```console
$ cmake --preset {presetName} -D<OPTION_NAME>=<VALUE> --fresh
```

**Examples for CMake Configure:**

**Linux:**
```console
# CMake configure library
$ cmake --preset dev-release-gcc --fresh

# Custom configure library with install prefix
$ cmake --preset dev-release-gcc -DCMAKE_INSTALL_PREFIX=<user_specified_path> --fresh
```

**Windows:**
```console
# CMake configure library
$ cmake --preset dev-win-release-llvm --fresh

# Custom configure library with install prefix
$ cmake --preset dev-win-release-llvm -DCMAKE_INSTALL_PREFIX=<user_specified_path> --fresh

```

**Note:**
- **Linux:** Use presets with `gcc` (dev-gcc, dev-release-gcc) or `clang` (dev-clang, dev-release-clang)
- **Windows:** Use presets with `clang-cl/LLVM` (dev-win-llvm, dev-win-release-llvm)
- **AddressSanitizer (ASAN):** Only supported with GCC compiler and requires static library build
- **Code Coverage:** Only supported with GCC compiler and requires static library build
- The `--fresh` flag ensures a clean configuration by removing any cached CMake data.
- See [CMake Configuration Options](#5-cmake-configuration-options) section below for all available options.

---

#### **4.3 Build the Library**

To build the library, use the same preset name used in Configure CMake with the `--build` command:

```console
$ cmake --build --preset {presetName}
```

**Examples for Building Library:**

**Linux:**
```console
$ cmake --build --preset dev-release-gcc
```

**Windows:**
```console
$ cmake --build --preset dev-win-release-llvm
```

One can also build in **verbose mode** using `-v`:
```console
$ cmake --build --preset {presetName} -v
```

One can also build in **parallel** using `-j <num-cores>`

**Linux:**
```console
$ cmake --build --preset {presetName} -j$(nproc)
```

**Windows:**
```console
$ cmake --build --preset {presetName} -j %NUMBER_OF_PROCESSORS%
```

<a id="parallel-build-note"></a>
**Note:**
* Linux: Use `-j$(nproc)` to build in parallel using all logical cores (which equals physical cores on systems without hyperthreading).
  - For systems with hyperthreading, manually determine the number of physical cores and use that value instead (e.g., `-j16`).
* Windows: Use `-j %NUMBER_OF_PROCESSORS%` to build in parallel using all logical cores (which equals physical cores on systems without hyperthreading).
  - For systems with hyperthreading, manually determine the number of physical cores and use that value instead (e.g., `-j16`).

---

#### **4.4 Installation**

Only when building in release mode (for example, using a `*-release-*` preset), the compiled library will be installed in the `build/{presetName}` directory or user specified prefix path.

**Note:**
- Installation only occurs when building in release mode. Libraries are installed to `build/{presetName}/lib` and header files to `build/{presetName}/include`.
- In debug mode, the library is built but not installed. Libraries are found in `build/{presetName}/src` and header files are found in `include/external`.

---

#### **4.5 Building and Executing gtests (Libm Test Framework)**

##### **4.5.1 Building gtests**

To build the gtests, use the same preset name used in Configure CMake with the `--target gtests` command:

**Available Build Targets:**

| Target Name       | Description              | Linux | Windows |
|-------------------|--------------------------|:-----:|:-------:|
| `gtests`          | Build all API tests      |   ✓   |    ✓    |
| `test_<function>` | Build single API test    |   ✓   |    ✓    |

To build all the APIs for testing:

```console
$ cmake --build --preset {presetName} --target gtests
```

To build and test a **single API**, use the specific target name:

```console
$ cmake --build --preset {presetName} --target test_<function>
```

**Examples for Building gtests:**

**Linux:**
```console
$ cmake --build --preset dev-release-gcc --target gtests
```

**Windows:**
```console
$ cmake --build --preset dev-win-release-llvm --target gtests
```

##### **4.5.2 Executing gtests**

Before running tests, you need to add the library's directory to your environment:

**Required Environment Variables:**

| Variable            | Platform | Purpose                              | Example Value                        |
|---------------------|----------|--------------------------------------|--------------------------------------|
| `LD_LIBRARY_PATH`   | Linux    | Library search path                  | `${PWD}/build/{presetName}/lib`      |
| `PATH`              | Windows  | Executable and DLL search path       | `%CD%\build\{presetName}\lib`        |
| `MPFR_PATH`         | Windows  | MPFR library location                | `<path_to_mpfr>\mpfr`                |
| `INTEL_PATH`        | Linux    | Intel OneAPI path (for SVML tests)   | `<intel_oneapi_path>`                |
| `ASAN_OPTIONS`      | Linux    | AddressSanitizer options             | `redzone=2048`                       |

**Linux:**
```console
$ export LD_LIBRARY_PATH=${PWD}/build/{presetName}/lib:$LD_LIBRARY_PATH
$ ./build/{presetName}/aocl_gtests/test_<function> <Test parameters>
```

**Windows:**
```console
$ set PATH=%PATH%;%CD%\build\{presetName}\lib
$ set PATH=%PATH%;<path_to_mpfr>\mpfr\mpfr_x64-windows\bin;<path_to_mpfr>\gmp\gmp_x64-windows\bin;<path_to_mpfr>\mpc\mpc_x64-windows\bin
$ build\{presetName}\aocl_gtests\test_<function> <Test parameters>
```

**Test Parameters:**

| Parameter          | Flag | Description                | Valid Values                                                            |
|--------------------|------|----------------------------|-------------------------------------------------------------------------|
| Input type         | `-i` | Floating-point precision   | `f` (single), `d` (double)                                              |
| Vector input count | `-e` | Number of vector elements  | `1` (scalar), `2/4/8/16` (vector), `32` (vector array)                  |
| Test Type          | `-t` | Type of test to run        | `perf` (performance), `conf` (conformance)*, `accu` (accuracy), `inplace` (in-place)** |
| Input count        | `-c` | Number of test inputs      | Any positive integer                                                    |
| Input range        | `-r` | Input value range          | `start,stop,<algorithm>` where `<algorithm>` is `simple`, `random`, `linear`, or `binadex` |
| Number type        | `-z` | Complex or real variant    | `c` (complex), default is real                                          |

**Notes:**
- *`conf` test type is supported only for scalar (`-e 1`) and vector array cases (`-e 32`).
- **`inplace` test type is for in-place accuracy and memory testing (vector array variants only).

**Linux Example:**
```console
$ export LD_LIBRARY_PATH=${PWD}/build/{presetName}/lib:$LD_LIBRARY_PATH
$ ./build/{presetName}/aocl_gtests/test_exp -i f -e 1 -t accu -r -79.0,79.0,simple -c 1000
$ ./build/{presetName}/aocl_gtests/test_exp -t perf -r -79.0,79.0,simple -c 1000
$ ./build/{presetName}/aocl_gtests/test_exp -t conf
$ ./build/{presetName}/aocl_gtests/test_exp -e 32 -t inplace -r -79.0,79.0,simple -c 1000
```

**Windows Example:**
```console
$ set PATH=%PATH%;<path_to_mpfr>\mpfr\mpfr_x64-windows\bin;<path_to_mpfr>\gmp\gmp_x64-windows\bin;<path_to_mpfr>\mpc\mpc_x64-windows\bin
$ set PATH=%PATH%;%CD%\build\{presetName}\lib
$ build\{presetName}\aocl_gtests\test_exp.exe -i f -e 1 -t accu -r -79.0,79.0,simple -c 1000
$ build\{presetName}\aocl_gtests\test_exp.exe -t perf -r -79.0,79.0,simple -c 1000
$ build\{presetName}\aocl_gtests\test_exp.exe -t conf
$ build\{presetName}\aocl_gtests\test_exp.exe -e 32 -t inplace -r -79.0,79.0,simple -c 1000
```

##### **4.5.3 Building with Alternative Library ABIs (Linux Only)**

**Available Library ABI Options:**
| ABI Option | Description                 | Test Directory  | Function Prefix  |
|------------|-----------------------------|-----------------|------------------|
| Default    | AOCL LibM (default)         | `aocl_gtests`   | AMD Standard     |
| `glibc`    | GNU libc compatibility      | `glibc_gtests`  | Glibc Standard   |
| `svml`     | Intel SVML compatibility    | `svml_gtests`   | MKL Standard     |

**Building with glibc compatibility:**

Configure CMake build tests with glibc compatibility API:
```console
$ cmake --preset {presetName} -DLIBABI=glibc --fresh
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target gtests
$ ./build/{presetName}/glibc_gtests/test_<function> <Test parameters>
```
With this, the tests will be compiled to call _libm_ functions without the amd specific prefix.

##### **4.5.4 Building with Intel Math (OneAPI) Libraries (Linux Only)**

To build tests to exercise Intel math libraries:

1. Install latest version of Intel OneAPI
2. Export Intel OneAPI path to INTEL_PATH:

```console
# Add Intel MKL header files to compiler include search path
# include_directories($ENV{INTEL_PATH}/include)
# Add Intel MKL library directory to linker search path
# link_directories($ENV{INTEL_PATH}/lib)

$ export INTEL_PATH=<intel OneAPI path>

$ cmake --preset {presetName} -DLIBABI=svml --fresh
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target gtests

$ export LD_LIBRARY_PATH=<intel OneAPI path>/lib:$LD_LIBRARY_PATH

$ ./build/{presetName}/svml_gtests/test_<function> <Test parameters>
```

##### **4.5.5 Using Preloadable Libraries (Linux Only)**

**Available Preloadable Libraries:**
| Library Name  | File Name           | Purpose                        | Accuracy          | Platform |
|---------------|---------------------|--------------------------------|-------------------|----------|
| Glibc Compat  | `glibc-compat.so`   | GLIBC compatibility layer      | Standard          | Linux    |
| AMD Fast LibM | `libalmfast.so`     | High-performance variant       | No Special cases  | Linux    |

**Using glibc compat library:**

A glibc compatible library, which has the table of functions compatible with GLIBC, or compile with AOCC to generate proper API calls to amdlibm:

```console
$ LD_PRELOAD=${PWD}/build/{presetName}/lib/glibc-compat.so
```

**Using AMD fastlibm library:**

AMD fast library, with up to 4 ULP (when available):

```console
$ LD_PRELOAD=${PWD}/build/{presetName}/lib/libalmfast.so
```

---

#### **4.6 Building Examples**

**Examples Build Options:**
| Option                  | Description                     | Default | Linux | Windows | Valid Values    |
|-------------------------|---------------------------------|---------|:-----:|:-------:|-----------------|
| `LIBM_BUILD_EXAMPLES`   | Build example programs          | OFF     |   ✓   |    ✓    | `ON`, `OFF`     |
| `USE_STATIC_LIB`        | Link with static library        | OFF     |   ✓   |    ✓    | `ON`, `OFF`     |
| `AOCL_LIBM`             | Path to AOCL LibM package       | N/A     |   ✓   |    ✓    | Any valid path  |
| `AOCL_UTILS`            | Path to AOCL Utils package      | N/A     |   ✓   |    ✓    | Any valid path  |

To build examples along with aocl-libm, configure CMake using `-DLIBM_BUILD_EXAMPLES=ON`. This is **OFF by default**.

**Option 1: Build Examples along with Library configuration and Building**
```console
$ cmake --preset {presetName} -DLIBM_BUILD_EXAMPLES=ON --fresh
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target test_libm
```
Executable will be created in `build/{presetName}/examples/test_libm`

**Option 2: Build Examples Only (Standalone, Linux Only)**
Navigate to the examples folder and configure with library paths:
```console
$ cd examples
$ mkdir build && cd build
$ cmake .. -DAOCL_LIBM=<path_to_aocl_libm_package> -DAOCL_UTILS=<path_to_aocl_utils_package>
$ cmake --build .
```
Executable will be created in `build/test_libm`

**Example to link Static Library:**
```console
$ cmake --preset {presetName} -DLIBM_BUILD_EXAMPLES=ON -DUSE_STATIC_LIB=ON --fresh
$ cmake --build --preset {presetName} --target test_libm
```

**Running Examples:**

**Linux:**
```console
$ export LD_LIBRARY_PATH=${PWD}/build/{presetName}/lib:$LD_LIBRARY_PATH
$ ./build/{presetName}/examples/test_libm
# or
$ export LD_LIBRARY_PATH=<path_to_aocl_libm_package>/lib:$LD_LIBRARY_PATH
$ ./build/test_libm
```

**Windows:**
```console
$ set PATH=%PATH%;%CD%\build\{presetName}\lib
$ build\{presetName}\examples\test_libm.exe
# or
$ set PATH=%PATH%;<path_to_aocl_libm_package>\lib
$ build\test_libm.exe
```

---

#### **4.7 Clean Build Files**

To clean all build files:

```console
$ cmake --build --preset {presetName} --target clean
```

---

#### **4.8 Static Dispatch Configuration (Linux Only)**

One can configure the library for a specific CPU architecture by setting the `ALM_STATIC_DISPATCH` option.

**Available Static Dispatch Options:**

| Option Value | Code Path       | Minimum CPU Requirement          |
|--------------|-----------------|----------------------------------|
| `ZEN2`       | ZEN2 optimized  | AMD Zen 2                        |
| `ZEN3`       | ZEN3 optimized  | AMD Zen 3                        |
| `ZEN4`       | ZEN4 optimized  | AMD Zen 4                        |
| `ZEN5`       | ZEN5 optimized  | AMD Zen 5                        |
| `AVX2`       | ZEN2 optimized  | AMD Zen 2                        |
| `AVX512`     | ZEN5 optimized  | AMD Zen 5                        |

To configure with a specific option, such as **ZEN3**, use the following command:
```console
$ cmake --preset {presetName} -DALM_STATIC_DISPATCH=ZEN3 --fresh
```

**Note:** The compiled library should only be executed on machines that match the target architecture specified during compilation (ex: ZEN3 binaries should run on ZEN3-compatible processors).

---

#### **4.9 Developer Features (Linux Only)**

**Developer Features Configuration:**

| Feature            | CMake Option                  | Default | Compiler  | Library Type  | Target     | Output/Usage                            |
|--------------------|-------------------------------|---------|-----------|---------------|------------|-----------------------------------------|
| Address Sanitizer  | `-DLIBM_ENABLE_ASAN=ON`       | OFF     | GCC only  | Static only   | N/A        | Runtime memory error detection          |
| Code Coverage      | `-DLIBM_ENABLE_COVERAGE=ON`   | OFF     | GCC only  | Static only   | `coverage` | `build/{presetName}/html_coverage_report/index.html`       |
| Documentation      | `-DLIBM_BUILD_DOCS=ON`        | OFF     | Any       | Any           | `libmdoc`  | `<build>/aocl_docs/html/index.html`     |

##### **4.9.1 AddressSanitizer (ASAN)**

To build with AddressSanitizer enabled, configure CMake using `-DLIBM_ENABLE_ASAN=ON`. This is **OFF by default**.

**Requirements:**
- **Compiler:** GCC only (not supported with Clang)
- **Library Type:** Static library only
- **Platform:** Linux only

```console
$ cmake --preset dev-release-gcc -DLIBM_ENABLE_ASAN=ON --fresh
$ cmake --build --preset dev-release-gcc
```

**ASAN Options:**

**Redzone Size:** The default redzone size is 16 bytes. To increase it, set the `ASAN_OPTIONS` environment variable. For example, to set it to 2048 bytes, use:

```console
$ ASAN_OPTIONS=redzone=2048 ./<executable_name>
```

##### **4.9.2 Code Coverage**

To build for code coverage, configure CMake using `-DLIBM_ENABLE_COVERAGE=ON`. This is **OFF by default**.

**Requirements:**
- **Compiler:** GCC only (not supported with Clang)
- **Library Type:** Static library only
- **Platform:** Linux only

```console
$ cmake --preset dev-release-gcc -DLIBM_ENABLE_COVERAGE=ON --fresh
$ cmake --build --preset dev-release-gcc
```

**Report Generation:**

To generate an HTML code coverage report using LCOV, run the provided bash script:

1. Run the executable (with arguments)

```console
$ ./<executable_name>
```

2. Now run the following cmake command to generate `index.html` file in the `html_coverage_report` directory.

```console
$ cmake --build --preset {presetName} --target coverage
```

##### **4.9.3 Building Documentation**

To build LibM documentation, configure CMake using `-DLIBM_BUILD_DOCS=ON`. This is **OFF by default**.

```console
$ cmake --preset {presetName} -DLIBM_BUILD_DOCS=ON --fresh
$ cmake --build --preset {presetName} --target libmdoc
```

Note: LibM documentation is found here: **<build>/aocl_docs/html/index.html**

**Note:**
- ASAN (AddressSanitizer) requires GCC compiler and static library build
- Code Coverage requires GCC compiler and static library build
- These are developer-focused features for debugging and testing

---

## **5. CMake Configuration Options**

The following table lists all available CMake configuration options for building AOCL-LIBM:

| Category      | Option                      | Description                              | Default        | Linux | Windows | Valid Values                                        |
|---------------|-----------------------------|------------------------------------------|----------------|:-----:|:-------:|-----------------------------------------------------|
| **CMake**     | `CMAKE_BUILD_TYPE`          | Build type configuration                 | `Debug`        |   ✓   |    ✓    | `Debug`, `Release`                                  |
| **CMake**     | `CMAKE_C_COMPILER`          | C compiler to use                        | System default |   ✓   |    ✓    | `gcc`, `clang` (Linux); `clang-cl.exe` (Windows)    |
| **CMake**     | `CMAKE_CXX_COMPILER`        | C++ compiler to use                      | System default |   ✓   |    ✓    | `g++`, `clang++` (Linux); `clang-cl.exe` (Windows)  |
| **CMake**     | `CMAKE_INSTALL_PREFIX`      | Installation directory path              | System default |   ✓   |    ✓    | Any valid path                                      |
| **Library**   | `BUILD_SHARED_LIBS`         | Build shared libraries                   | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Library**   | `BUILD_STATIC_LIBS`         | Build static libraries                   | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Library**   | `LIBM_BUILD_LIBRARY`        | Enable building libraries                | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Dispatch**  | `ALM_STATIC_DISPATCH`       | Static CPU architecture dispatch         | OFF            |   ✓   |    ✗    | `AVX2`, `ZEN3`, `ZEN5`, `AVX512`                    |
| **Utils**     | `AOCL_UTILS_INCLUDE_DIR`    | Path to aocl-utils headers               | Auto-fetched   |   ✓   |    ✓    | Any valid path                                      |
| **Utils**     | `AOCL_UTILS_LIB`            | Path to aocl-utils library               | Auto-fetched   |   ✓   |    ✓    | Any valid path                                      |
| **Testing**   | `LIBM_BUILD_TESTS`          | Enable building tests                    | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Testing**   | `LIBM_ENABLE_AVX512`        | Enable AVX-512 support                   | ON             |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Testing**   | `LIBABI`                    | Library ABI compatibility                | `aocl`         |   ✓   |    ✗    | `aocl`, `glibc`, `svml`                             |
| **Examples**  | `LIBM_BUILD_EXAMPLES`       | Build example programs                   | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Examples**  | `USE_STATIC_LIB`            | Link examples with static library        | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Examples**  | `AOCL_LIBM`                 | Path to AOCL LibM package                | Build dir      |   ✓   |    ✓    | Any valid path                                      |
| **Examples**  | `AOCL_UTILS`                | Path to AOCL Utils package               | Build dir      |   ✓   |    ✓    | Any valid path                                      |
| **Developer** | `LIBM_ENABLE_ASAN`          | Enable AddressSanitizer (GCC only)       | OFF            |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Developer** | `LIBM_ENABLE_COVERAGE`      | Enable code coverage (GCC only)          | OFF            |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Developer** | `LIBM_BUILD_DOCS`           | Build Sphinx/Doxygen documentation       | OFF            |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Developer** | `CMAKE_VERBOSE_MAKEFILE`    | Enable verbose build output              | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |

*Legend:* ✓ = Supported, ✗ = Not Supported

**Note:** Options marked as "Auto-fetched" will automatically download from GitHub if not provided. Options marked as "Preset-dependent" have values set by the chosen preset configuration.

---

## **6. Manually Building Without Presets**

If you prefer to configure the build manually without using presets, you can specify all options directly through CMake command-line arguments.

#### **6.1 Create a Build Directory**

Create a separate build directory to keep the source tree clean:
```console
$ mkdir -p build
$ cd build
```

---

#### **6.2 Configure CMake Manually**

Run CMake with your desired options. Here are common configuration examples:

**Linux:**
```console
$ cmake .. -DCMAKE_BUILD_TYPE=<Debug/Release> -DCMAKE_C_COMPILER=<gcc/clang> -DCMAKE_CXX_COMPILER=<g++/clang++> -DAOCL_UTILS_INCLUDE_DIR=<path_to_aocl_utils_include> -DAOCL_UTILS_LIB=<path_to_aocl_utils_lib/<libname>> -DCMAKE_INSTALL_PREFIX=<user_specified_prefix_path>
```

**Windows with LLVM:**
```console
$ cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=<Debug/Release> -DCMAKE_C_COMPILER=<path_to_llvm>\bin\clang-cl.exe -DCMAKE_CXX_COMPILER=<path_to_llvm>\bin\clang-cl.exe -DAOCL_UTILS_INCLUDE_DIR=<path_to_aocl_utils_include> -DAOCL_UTILS_LIB=<path_to_aocl_utils_lib\<libname>> -DCMAKE_INSTALL_PREFIX=<user_specified_prefix_path>
```

You can customize the build with these additional options:

| Option                      | Description                             | Default        | Linux | Windows |
|-----------------------------|-----------------------------------------|----------------|:-----:|:-------:|
| `CMAKE_BUILD_TYPE`          | Build type: Debug or Release            | Debug          |   ✓   |    ✓    |
| `CMAKE_C_COMPILER`          | C compiler to use                       | System default |   ✓   |    ✓    |
| `CMAKE_CXX_COMPILER`        | C++ compiler to use                     | System default |   ✓   |    ✓    |
| `CMAKE_INSTALL_PREFIX`      | Installation directory                  | System default |   ✓   |    ✓    |
| `AOCL_UTILS_INCLUDE_DIR`    | Path to aocl-utils include directory    | OFF            |   ✓   |    ✓    |
| `AOCL_UTILS_LIB`            | Path to aocl-utils library              | OFF            |   ✓   |    ✓    |
| `ALM_STATIC_DISPATCH`       | Static dispatch for CPU                 | OFF            |   ✓   |    ✗    |

**Note:**
- For `ALM_STATIC_DISPATCH`, valid values are: `AVX2`, `ZEN3`, `ZEN5`, `AVX512`
- For complete list of all available options, see the [CMake Configuration Options](#5-cmake-configuration-options) section above.

---

#### **6.3 Build the Library**

Once configured, build the library:

**Linux:**
```console
$ cmake --build . -j$(nproc)
```

**Windows:**

For multi-configuration generators (like Visual Studio or Ninja Multi-Config), specify the build type:
```console
$ cmake --build . --config <Debug/Release> -j %NUMBER_OF_PROCESSORS%
```

For single-configuration generators:
```console
$ cmake --build . -j %NUMBER_OF_PROCESSORS%
```

**Note:** For parallel build description, refer to the [parallel build note](#parallel-build-note) in section 4.3.

---

#### **6.4 Install**

To install the library to the specified prefix:
```console
$ cmake --build . --target install
```

---

#### **6.5 Build Examples (Standalone)**

Navigate to the examples folder and configure with library paths:
```console
$ cd examples
$ mkdir build && cd build
$ cmake .. -DAOCL_LIBM=<user_specified_prefix_path> -DAOCL_UTILS=<path_to_aocl_utils_package>
$ cmake --build .
```

---

#### **6.6 Running Examples**

**Linux:**
```console
$ export LD_LIBRARY_PATH=<user_specified_prefix_path>/lib:$LD_LIBRARY_PATH
$ ./build/examples/test_libm
```

**Windows:**
```console
$ set PATH=%PATH%;<user_specified_prefix_path>\lib
$ build\examples\test_libm.exe
```

---

#### **6.7 Clean Build**

To clean the build:
```console
$ cmake --build . --target clean
```