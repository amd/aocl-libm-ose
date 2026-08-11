# Building AOCL-LIBM with CMake

This comprehensive guide provides instructions for building, testing, and installing the AOCL-LIBM library using CMake on both Linux and Windows platforms.

---

## **Table of Contents**

1. [Requirements](#1-requirements)
2. [Platform-Specific Setup](#2-platform-specific-setup)
3. [Quick Start Commands](#3-quick-start-commands)
   - 3.1 [Build the library only](#31-build-the-library-only)
   - 3.2 [Build the library and its tests](#32-build-the-library-and-its-tests)
4. [Building With CMake Presets](#4-building-with-cmake-presets)
   - 4.1 [List Presets](#41-list-presets)
   - 4.2 [Configure CMake](#42-configure-cmake)
   - 4.3 [Build the Library](#43-build-the-library)
   - 4.4 [Installation](#44-installation)
   - 4.5 [Building and Executing gtests (Libm Test Framework)](#45-building-and-executing-gtests-libm-test-framework)
      - 4.5.1 [Building gtests](#451-building-gtests)
      - 4.5.2 [Windows gtests build notes](#452-windows-gtests-build-notes)
      - 4.5.3 [Executing gtests](#453-executing-gtests)
      - 4.5.4 [Building with Alternative Library ABIs (Linux Only)](#454-building-with-alternative-library-abis-linux-only)
      - 4.5.5 [Building with Intel Math (OneAPI) Libraries (Linux Only)](#455-building-with-intel-math-oneapi-libraries-linux-only)
      - 4.5.6 [Using Preloadable Libraries (Linux Only)](#456-using-preloadable-libraries-linux-only)
   - 4.6 [Building Examples](#46-building-examples)
   - 4.7 [Clean Build Files](#47-clean-build-files)
   - 4.8 [Static Dispatch Configuration (Linux Only)](#48-static-dispatch-configuration-linux-only)
   - 4.9 [Developer Features (Linux Only)](#49-developer-features-linux-only)
      - 4.9.1 [AddressSanitizer (ASAN)](#491-addresssanitizer-asan)
      - 4.9.2 [Code Coverage](#492-code-coverage)
      - 4.9.3 [Building Documentation](#493-building-documentation)
   - 4.10 [Floating-Point Contraction (FMA) Control](#410-floating-point-contraction-fma-control)
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
| GCC               | ≥ 9.2.0 and < 16.1.0   |   ✓   |    ✗    | Linux compiler option                  |
| Clang             | ≥ 9.0.0 and < 21.1.8   |   ✓   |    ✗    | Linux compiler option                  |
| AOCC (aoc/aoc++)  | ≥ 6.0.0                |   ✓   |    ✗    | Linux compiler option; `aoc`/`aoc++` are symlinks to `clang`/`clang++` |
| LLVM (Clang-CL)   | ≥ 9.0.0 and < 21.1.8   |   ✗   |    ✓    | Windows compiler (clang-cl.exe)        |
| MPFR              | Latest                 |   ✓   |    ✓    | Test framework only; path must be set on Windows |
| GMP               | Latest                 |   ✓   |    ✓    | Test framework only; dependency of MPFR |
| MPC               | Latest                 |   ✓   |    ✓    | Test framework only; dependency of MPFR |
| Visual Studio     | 2022                   |   ✗   |    ✓    | Provides Ninja and MSBuild generators  |

**Note:**
  * AOCL-LibM includes an internal CPUID utilities module (`src/utils/`) that provides CPU detection functionality. No external aocl-utils dependency is required.
  * On Windows, the build system supports both the Ninja generator (default) and the Visual Studio 17 2022 generator with the ClangCL toolset.

---

## **2. Platform-Specific Setup**

**Windows Only:**
1.  Open a command prompt and navigate to the `aocl-libm` directory.
2.  Run the Visual Studio `vcvarsall.bat` script (replace `<path_to_visualstudio>` with your actual installation path):

```console
"<path_to_visualstudio>\VC\Auxiliary\Build\vcvarsall.bat" x64
```

3.  (Test framework only) Set the `MPFR_PATH` environment variable.

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

Building the library alone requires only CMake and a C/C++ compiler. The
multi-precision libraries (MPFR, GMP, MPC) are needed only for the test
framework, so the default preset build works without them
(`LIBM_BUILD_TESTS` defaults to `OFF`).

### **3.1 Build the library only**

**Linux:**
```console
$ cmake --preset dev-release-gcc --fresh
$ cmake --build --preset dev-release-gcc -j
```

**Windows (Ninja generator):**
```console
$ cmake --preset dev-win-release-llvm-ninja --fresh
$ cmake --build --preset dev-win-release-llvm-ninja -j
```

**Windows (Visual Studio generator):**
```console
$ cmake --preset dev-win-release-llvm-msvc --fresh
$ cmake --build --preset dev-win-release-llvm-msvc -j
```

### **3.2 Build the library and its tests**

The tests are **off by default**, so enable them at configure time with
`-DLIBM_BUILD_TESTS=ON`. Always build the library
first, then the `gtests` target, then run a test. The full, copy-pasteable
sequence is:

**Linux:**
```console
# 1. Configure with tests enabled
$ cmake --preset dev-release-gcc -DLIBM_BUILD_TESTS=ON --fresh
# 2. Build the library
$ cmake --build --preset dev-release-gcc -j
# 3. Build the tests (library must already be built)
$ cmake --build --preset dev-release-gcc --target gtests
# 4. Run a test
$ export LD_LIBRARY_PATH=${PWD}/build/dev-release-gcc/lib:$LD_LIBRARY_PATH
$ ./build/dev-release-gcc/aocl_gtests/test_exp -i f -e 1 -t accu -r -79.0,79.0,simple -c 1000
```

**Windows (Ninja generator):**
```console
# i.  Set up the Visual Studio x64 build environment (provides Ninja and clang-cl)
$ "<path_to_visualstudio>\VC\Auxiliary\Build\vcvarsall.bat" x64
# ii. Set MPFR path (test framework dependency)
$ set MPFR_PATH=<path_to_mpfr>\mpfr
# 1. Configure with tests enabled
$ cmake --preset dev-win-release-llvm-ninja -DLIBM_BUILD_TESTS=ON --fresh
# 2. Build the library
$ cmake --build --preset dev-win-release-llvm-ninja -j
# 3. Build the tests (library must already be built)
$ cmake --build --preset dev-win-release-llvm-ninja --target gtests
# 4. Run a test
$ set PATH=%PATH%;%CD%\build\dev-win-release-llvm-ninja\lib;<path_to_mpfr>\mpfr\mpfr_x64-windows\bin;<path_to_mpfr>\gmp\gmp_x64-windows\bin;<path_to_mpfr>\mpc\mpc_x64-windows\bin
$ build\dev-win-release-llvm-ninja\aocl_gtests\test_exp.exe -i f -e 1 -t accu -r -79.0,79.0,simple -c 1000
```

For the full list of targets, test parameters and other options, see
[Building and Executing gtests](#45-building-and-executing-gtests-libm-test-framework).

---

## **4. Building With CMake Presets**

**Step-by-Step Build and Test Guide**

#### **4.1 List Presets**
To view the available configuration presets, run:
```console
$ cmake --list-presets
```

**Available Presets:**

The presets configure the **library only** by default. To build any additional
component (tests, examples, documentation, ...), the general workflow is:

1. **Configure** with the preset and enable the component's option from the
   [CMake Configuration Options](#5-cmake-configuration-options) table, for example
   `-DLIBM_BUILD_TESTS=ON`, `-DLIBM_BUILD_EXAMPLES=ON`, or `-DLIBM_BUILD_TESTSUITE=ON`.
2. **Build the library** with the same preset (`cmake --build --preset {presetName}`).
3. **Build the component** with `--target <targetName>`.

| Component     | Enable Option (configure step) | Build Target                 |
|---------------|--------------------------------|------------------------------|
| Library       | (default, always built)        | (default)                    |
| gtests        | `-DLIBM_BUILD_TESTS=ON`        | `gtests` / `test_<function>` |
| Examples      | `-DLIBM_BUILD_EXAMPLES=ON`     | `test_libm`                  |
| almbench      | `-DLIBM_BUILD_TESTSUITE=ON`    | `libm_runner`                |
| gtests and almbench | `-DLIBM_TESTS=ON`        | `gtests` / `libm_runner`     |

For more configuration options, refer to the [CMake Configuration Options](#5-cmake-configuration-options) section.

| Preset Name                  | Platform | Compiler | Generator | Build Type | Description                                          |
|------------------------------|----------|----------|-----------|------------|------------------------------------------------------|
| `dev-gcc`                    | Linux    | GCC      | Make      | Debug      | Developer Config with GCC-Debug                      |
| `dev-clang`                  | Linux    | Clang    | Make      | Debug      | Developer Config with Clang-Debug                    |
| `dev-release-gcc`            | Linux    | GCC      | Make      | Release    | Developer Config with GCC-Release                    |
| `dev-release-clang`          | Linux    | Clang    | Make      | Release    | Developer Config with Clang-Release                  |
| `dev-win-llvm-ninja`         | Windows  | LLVM     | Ninja     | Debug      | Developer Config with Ninja Generator (LLVM)-Debug   |
| `dev-win-release-llvm-ninja` | Windows  | LLVM     | Ninja     | Release    | Developer Config with Ninja Generator (LLVM)-Release |
| `dev-win-llvm-msvc`          | Windows  | LLVM     | MSVC      | Debug      | Developer Config with MSVC Generator (LLVM)-Debug    |
| `dev-win-release-llvm-msvc`  | Windows  | LLVM     | MSVC      | Release    | Developer Config with MSVC Generator (LLVM)-Release  |

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

**Windows (Ninja generator):**
```console
# CMake configure library
$ cmake --preset dev-win-release-llvm-ninja --fresh

# Custom configure library with install prefix
$ cmake --preset dev-win-release-llvm-ninja -DCMAKE_INSTALL_PREFIX=<user_specified_path> --fresh

```

**Windows (Visual Studio generator):**
```console
# CMake configure library
$ cmake --preset dev-win-release-llvm-msvc --fresh

# Custom configure library with install prefix
$ cmake --preset dev-win-release-llvm-msvc -DCMAKE_INSTALL_PREFIX=<user_specified_path> --fresh

```

**Note:**
- **Linux:** Use presets with `gcc` (dev-gcc, dev-release-gcc) or `clang` (dev-clang, dev-release-clang)
- **Windows (Ninja):** Use presets with `-ninja` suffix (dev-win-llvm-ninja, dev-win-release-llvm-ninja)
- **Windows (Visual Studio):** Use presets with `-msvc` suffix (dev-win-llvm-msvc, dev-win-release-llvm-msvc)
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

**Windows (Ninja generator):**
```console
$ cmake --build --preset dev-win-release-llvm-ninja
```

**Windows (Visual Studio generator):**
```console
$ cmake --build --preset dev-win-release-llvm-msvc
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

Both release and debug builds run the `install` step automatically when using the provided build presets.
The compiled library will be installed in the `build/{presetName}` directory or user specified prefix path.

**Note:**
- In both release and debug modes, libraries are automatically installed to `build/{presetName}/lib` and header files to `build/{presetName}/include`.
- If the `install` step is disabled, libraries can be found in `build/{presetName}/src` and header files in `include/external`.
  To explicitly install libraries and headers, run:

  ```console
  $ cmake --build --preset {presetName} --target install
  ```
---

#### **4.5 Building and Executing gtests (Libm Test Framework)**

##### **4.5.1 Building gtests**

The test framework is disabled by default, so first (re)configure the preset with
`-DLIBM_BUILD_TESTS=ON` to make the `gtests` targets available (this requires the
MPFR/GMP/MPC dependencies), then build them with the same preset.

**Important:** The LibM library must be built **before** building the gtests.
The `gtests` (and `test_<function>`) targets link against libalm, so run a plain
`cmake --build --preset {presetName}` first; building the `--target gtests`
without the library present will fail at link time.

**Available Build Targets:**

| Target Name       | Description              | Linux | Windows |
|-------------------|--------------------------|:-----:|:-------:|
| `gtests`          | Build all API tests      |   ✓   |    ✓    |
| `test_<function>` | Build single API test    |   ✓   |    ✓    |

To build all the APIs for testing:

```console
# Build the LibM library first, then the gtests
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target gtests
```

To build and test a **single API**, use the specific target name:

```console
# Build the LibM library first, then the single API test
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target test_<function>
```

**Examples for Building gtests:**

**Linux:**
```console
$ cmake --preset dev-release-gcc -DLIBM_BUILD_TESTS=ON --fresh
$ cmake --build --preset dev-release-gcc
$ cmake --build --preset dev-release-gcc --target gtests
```

**Windows (Ninja generator):**
```console
# Debug
$ cmake --preset dev-win-llvm-ninja -DLIBM_BUILD_TESTS=ON --fresh
$ cmake --build --preset dev-win-llvm-ninja
$ cmake --build --preset dev-win-llvm-ninja --target gtests

# Release
$ cmake --preset dev-win-release-llvm-ninja -DLIBM_BUILD_TESTS=ON --fresh
$ cmake --build --preset dev-win-release-llvm-ninja
$ cmake --build --preset dev-win-release-llvm-ninja --target gtests
```

##### **4.5.2 Windows gtests build notes**

On Windows, gtests links against libalm and external dependencies (Google Test/Google Benchmark and mparith). These must use a consistent MSVC C runtime (CRT) and separate cached artifacts per build type to avoid link failures (for example, `_ITERATOR_DEBUG_LEVEL` mismatch when mixing Debug and Release objects).

**External dependency cache layout:**

| Dependency | Role | Windows path | Linux path |
|------------|------|--------------|------------|
| gapi (gtest/gbench) | Source | `gtests/gapi/` | `gtests/gapi/` |
| gapi (gtest/gbench) | Nested CMake build dir (`-B`) | `build/{presetName}/gapi/` | `build/{presetName}/gapi/` |
| gapi (gtest/gbench) | Install cache (libs) | `build/external/gapi/{Debug\|Release}/` | `build/external/gapi/` |
| mparith | Source | `gtests/libs/mparith/` | `gtests/libs/mparith/` |
| mparith | Nested CMake build dir (`-B`) | `build/{presetName}/mparith/` | `build/{presetName}/mparith/` |
| mparith | Install cache (libs/headers) | `build/external/mparith/{Debug\|Release}/` | `build/external/mparith/` |

gapi and mparith install caches are scoped by `CMAKE_BUILD_TYPE` on Windows. Both use the gtests preset binary directory for the nested CMake `-B` directory (`build/{presetName}/gapi` and `build/{presetName}/mparith` respectively); gapi flattens its built libraries into the scoped `build/external/gapi/{Debug|Release}` cache, while mparith installs artifacts into `MPARITH_DIR` via `-DMPARITH_DIR`.

On Linux, mparith is always built as `Release` regardless of the parent gtests build type. On Windows, mparith is built with the same `CMAKE_BUILD_TYPE` as the parent gtests configure.

**MSVC runtime:** Windows gtests and their external dependencies are built with `/MD` (`MultiThreadedDLL`) for all configurations, matching libalm. Debug gtests therefore use `/MD` (not `/MDd`). This avoids CRT mismatch at link time but does not enable full MSVC debug CRT/STL checking (`_ITERATOR_DEBUG_LEVEL=2`).

**Switching Debug and Release on Windows:** Use the appropriate preset and reconfigure with `--fresh` when switching. Debug and Release maintain separate cached gapi and mparith artifacts under the paths above; one configuration does not reuse the other's cached libraries.

##### **4.5.3 Executing gtests**

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

##### **4.5.4 Building with Alternative Library ABIs (Linux Only)**

**Available Library ABI Options:**
| ABI Option | Description                 | Test Directory  | Function Prefix  |
|------------|-----------------------------|-----------------|------------------|
| Default    | AOCL LibM (default)         | `aocl_gtests`   | AMD Standard     |
| `glibc`    | GNU libc compatibility      | `glibc_gtests`  | Glibc Standard   |
| `svml`     | Intel SVML compatibility    | `svml_gtests`   | MKL Standard     |

**Building with glibc compatibility:**

Configure CMake build tests with glibc compatibility API:
```console
$ cmake --preset {presetName} -DLIBM_BUILD_TESTS=ON -DLIBABI=glibc --fresh
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target gtests
$ ./build/{presetName}/glibc_gtests/test_<function> <Test parameters>
```
With this, the tests will be compiled to call _libm_ functions without the amd specific prefix.

##### **4.5.5 Building with Intel Math (OneAPI) Libraries (Linux Only)**

To build tests to exercise Intel math libraries:

1. Install latest version of Intel OneAPI
2. Export Intel OneAPI path to INTEL_PATH:

```console
# Add Intel MKL header files to compiler include search path
# include_directories($ENV{INTEL_PATH}/include)
# Add Intel MKL library directory to linker search path
# link_directories($ENV{INTEL_PATH}/lib)

$ export INTEL_PATH=<intel OneAPI path>

$ cmake --preset {presetName} -DLIBM_BUILD_TESTS=ON -DLIBABI=svml --fresh
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target gtests

$ export LD_LIBRARY_PATH=<intel OneAPI path>/lib:$LD_LIBRARY_PATH

$ ./build/{presetName}/svml_gtests/test_<function> <Test parameters>
```

##### **4.5.6 Using Preloadable Libraries (Linux Only)**

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

To build examples along with aocl-libm, configure CMake using `-DLIBM_BUILD_EXAMPLES=ON`. This is **OFF by default**.

**Note:** Building examples is supported in both debug and release modes when the `install` target is run (provided build presets do this automatically).

---

**Build library with examples**
```console
$ cmake --preset {presetName} -DLIBM_BUILD_EXAMPLES=ON --fresh
$ cmake --build --preset {presetName}
$ cmake --build --preset {presetName} --target test_libm
```
Executable will be created in `build/{presetName}/examples/test_libm`


**Example to link Static Library:**

To link examples with static library, use the **-DUSE_STATIC_LIB=ON** configure option while running cmake configure and follow the remaining steps mentioned in section "**Build library with examples**".

**Running Examples:**

**Linux:**
```console
$ export LD_LIBRARY_PATH=${PWD}/build/{presetName}/lib:$LD_LIBRARY_PATH
$ ./build/{presetName}/examples/test_libm
```

**Windows:**
```console
$ set PATH=%PATH%;%CD%\build\{presetName}\lib
$ build\{presetName}\examples\test_libm.exe
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
| `ZEN6`       | ZEN6 optimized  | AMD Zen 6                        |
| `AVX2`       | ZEN2 optimized  | AMD Zen 2                        |
| `AVX512`     | ZEN5 optimized  | AMD Zen 5                        |

To configure with a specific option, such as **ZEN3**, use the following command:
```console
$ cmake --preset {presetName} -DALM_STATIC_DISPATCH=ZEN3 --fresh
```

**Note:** The compiled library should only be executed on machines that match the target architecture specified during compilation (e.g., ZEN3 binaries should run on ZEN3-compatible processors).

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

#### **4.10 Floating-Point Contraction (FMA) Control**

By default AOCL-LIBM is compiled with `ALM_FP_CONTRACT=fast`, which allows the
compiler to contract floating-point expressions (e.g. fusing a multiply and an
add into a single FMA instruction) for best performance. If your workload
requires **bit-reproducible** results across different builds/machines, you must
disable this contraction using the `ALM_FP_CONTRACT` option:

| Option Value     | Compiler-time Flag     | Description                                   |
|------------------|-----------------------|-----------------------------------------------|
| `fast` (default) | `-ffp-contract=fast`  | Allow FMA contraction (best performance)      |
| `on`             | `-ffp-contract=on`    | Contraction only within a source expression   |
| `off`            | `-ffp-contract=off`   | Disable contraction (bit-reproducible builds) |

The value is case-insensitive; any other value emits a warning and falls back to the default `fast`.

To build for bit-reproducibility (contraction disabled), use:
```console
$ cmake --preset {presetName} -DALM_FP_CONTRACT=off --fresh
```


---

## **5. CMake Configuration Options**

The following table lists all available CMake configuration options for building AOCL-LIBM:

| Category      | Option                      | Description                              | Default        | Linux | Windows | Valid Values                                        |
|---------------|-----------------------------|------------------------------------------|----------------|:-----:|:-------:|-----------------------------------------------------|
| **CMake**     | `CMAKE_BUILD_TYPE`          | Build type configuration                 | `Debug`        |   ✓   |    ✓    | `Debug`, `Release`                                  |
| **CMake**     | `CMAKE_CONFIGURATION_TYPES` | Build configuration for MSVC             | `Debug`        |   ✗   |    ✓    | `Debug`, `Release`                                  |
| **CMake**     | `CMAKE_C_COMPILER`          | C compiler to use                        | System default |   ✓   |    ✓    | `gcc`, `clang`, `aoc` (Linux); `clang-cl.exe` (Windows)    |
| **CMake**     | `CMAKE_CXX_COMPILER`        | C++ compiler to use                      | System default |   ✓   |    ✓    | `g++`, `clang++`, `aoc++` (Linux); `clang-cl.exe` (Windows)  |
| **CMake**     | `CMAKE_INSTALL_PREFIX`      | Installation directory path              | System default |   ✓   |    ✓    | Any valid path                                      |
| **Library**   | `BUILD_SHARED_LIBS`         | Build shared libraries                   | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Library**   | `BUILD_STATIC_LIBS`         | Build static libraries                   | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Library**   | `LIBM_BUILD_LIBRARY`        | Enable building libraries                | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Dispatch**  | `ALM_STATIC_DISPATCH`       | Static CPU architecture dispatch         | OFF            |   ✓   |    ✗    | `AVX2`, `ZEN2`, `ZEN3`, `ZEN4`, `ZEN5`, `ZEN6`, `AVX512` |
| **Compiler**  | `ALM_FP_CONTRACT`           | Floating-point contraction (FMA) mode     | `fast`         |   ✓   |    ✓    | `fast`, `on`, `off`                                    |
| **Testing**   | `LIBM_TESTS`                | Convenience switch that enables both `LIBM_BUILD_TESTS` and `LIBM_BUILD_TESTSUITE` | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Testing**   | `LIBM_BUILD_TESTS`          | Enable building tests                    | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Testing**   | `LIBM_ENABLE_AVX512`        | Enable AVX-512 support                   | ON             |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Testing**   | `LIBABI`                    | Library ABI compatibility                | `aocl`         |   ✓   |    ✗    | `aocl`, `glibc`, `svml`                             |
| **Testing**   | `LIBM_BUILD_TESTSUITE`      | Enable building LibM testsuite (almbench)| OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Developer** | `LIBM_ENABLE_ASAN`          | Enable AddressSanitizer (GCC only)       | OFF            |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Developer** | `LIBM_ENABLE_COVERAGE`      | Enable code coverage (GCC only)          | OFF            |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Developer** | `LIBM_BUILD_DOCS`           | Build Sphinx/Doxygen documentation       | OFF            |   ✓   |    ✗    | `ON`, `OFF`                                         |
| **Developer** | `CMAKE_VERBOSE_MAKEFILE`    | Enable verbose build output              | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Examples**  | `LIBM_BUILD_EXAMPLES`       | Build example programs                   | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Examples**  | `USE_STATIC_LIB`            | Link examples with static library        | OFF            |   ✓   |    ✓    | `ON`, `OFF`                                         |
| **Examples**  | `AOCL_LIBM`                 | Path to AOCL LibM package                | Build dir      |   ✓   |    ✓    | Any valid path                                      |

*Legend:* ✓ = Supported, ✗ = Not Supported

**Note:** CPU detection functionality is provided by the internal utils module (`src/utils/`). No external dependencies need to be downloaded or configured.

**Note:** For details on the LibM test suite (`LIBM_BUILD_TESTSUITE`), see the
[almbench test suite guide](almbench/libm_testsuiteReadMe.md).

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
$ cmake .. -DCMAKE_BUILD_TYPE=<Debug/Release> -DCMAKE_C_COMPILER=<gcc/clang/aoc> -DCMAKE_CXX_COMPILER=<g++/clang++/aoc++> -DCMAKE_INSTALL_PREFIX=<user_specified_prefix_path>
```

**Windows (Ninja generator):**

Pass the `clang-cl.exe` compiler paths explicitly (single-config generator):
```console
$ cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=<Debug/Release> -DCMAKE_C_COMPILER=<path_to_llvm>\bin\clang-cl.exe -DCMAKE_CXX_COMPILER=<path_to_llvm>\bin\clang-cl.exe -DCMAKE_INSTALL_PREFIX=<user_specified_prefix_path>
```

**Windows (Visual Studio generator):**

Select the ClangCL toolset with `-T` and the target architecture with `-A` instead of passing compiler paths (multi-config generator):
```console
$ cmake .. -G "Visual Studio 17 2022" -A x64 -T ClangCL,host=x64 -DCMAKE_BUILD_TYPE=<Debug/Release> -DCMAKE_INSTALL_PREFIX=<user_specified_prefix_path>
```

**Note:**
- For the Visual Studio (multi-config) generator, specify the build type at build time with `--config <Debug/Release>` (see section 6.3). `CMAKE_BUILD_TYPE` is also honored on Windows to restrict the generated configuration.
- Pure MSVC (`cl.exe`) is not supported; use the ClangCL toolset (`-T ClangCL,host=x64`) or `clang-cl.exe`.

You can customize the build with these additional options:

| Option                      | Description                             | Default        | Linux | Windows |
|-----------------------------|-----------------------------------------|----------------|:-----:|:-------:|
| `CMAKE_BUILD_TYPE`          | Build type: Debug or Release            | Debug          |   ✓   |    ✓    |
| `CMAKE_C_COMPILER`          | C compiler to use                       | System default |   ✓   |    ✓    |
| `CMAKE_CXX_COMPILER`        | C++ compiler to use                     | System default |   ✓   |    ✓    |
| `CMAKE_INSTALL_PREFIX`      | Installation directory                  | System default |   ✓   |    ✓    |
| `ALM_STATIC_DISPATCH`       | Static dispatch for CPU                 | OFF            |   ✓   |    ✗    |

**Note:**
- CPU detection is handled by the internal utils module. No external aocl-utils configuration is required.
- For `ALM_STATIC_DISPATCH`, valid values are: `AVX2`, `ZEN2`, `ZEN3`, `ZEN4`, `ZEN5`, `ZEN6`, `AVX512`
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

**Linux:**
```console
$ cd examples
$ mkdir build && cd build
$ cmake .. -DAOCL_LIBM=<user_specified_prefix_path>
$ cmake --build .
```

**Windows:**
```console
$ cd examples
$ mkdir build && cd build
$ cmake .. -G "Ninja" -DAOCL_LIBM=<user_specified_prefix_path> -DCMAKE_C_COMPILER="<clang-cl executable path>"
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
