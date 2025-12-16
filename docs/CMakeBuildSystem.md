### Building AOCL-LIBM with CMake

This guide provides instructions for building the AOCL-LIBM library using CMake on both Linux and Windows.

**Requirements:**
  * CMake version 3.26 or higher
  * GCC version   > 9.2 and < 14.1.0
  * Clang version > 9.0 and < 18.1.0
  * MPFR package: Must be installed and its path set (especially on Windows)


**Note:**
  * CMake automatically fetches aocl-utils from GitHub during configuration.
  * On Windows, the build system uses the inbuilt Ninja tool provided by Visual Studio

-----

## **🐧 Building on Linux**

#### **Step 1: List Presets**
To view the available configuration presets, run:
```console
$ cmake --list-presets
```
This will display a list like this:
```
Available configure presets:

  "dev-gcc"             - Developer Config with GCC-Debug
  "dev-clang"           - Developer Config with Clang-Debug
  "dev-release-gcc"     - Developer Config with GCC-Release
  "dev-release-clang"   - Developer Config with Clang-Release
```

#### **Step 2: Configure CMake**
To configure the project using a preset, for example, `dev-release-gcc`, run:
```console
$ cmake --preset dev-release-gcc --fresh
```

#### **Step 3: Build the Library**
To build the library, use the same preset name with the `--build` command:
```console
$ cmake --build --preset dev-release-gcc
```

You can also build in **parallel** using `-j` or in **verbose mode** using `-v`:
```console
$ cmake --build --preset dev-release-gcc -j
$ cmake --build --preset dev-release-gcc -v
```

#### **Step 4: Installation**
The compiled library will be installed in the `build/{presetName}` directory.
For the example above, this would be `build/dev-release-gcc`.

#### **Step 5: Building gtests**
To build all the APIs for testing, run:
```console
$ cmake --build --preset dev-release-gcc --target gtests
```

To build and test a **single API**, use the specific target name, for example:
```console
$ cmake --build --preset dev-release-gcc --target test_<function>
```

#### **Step 6: Executing gtests**
Before running tests, you need to add the library's directory and any other dependent .so paths to `LD_LIBRARY_PATH`:

To run the test executable for the required function:
```console
$ export LD_LIBRARY_PATH=path/to/library:$LD_LIBRARY_PATH
$ ./build/{presetName}/aocl_gtests/test_<function> <Test parameters>
```
Test parameters:
  * Input type:
    `-i` (f for single precision, d for double precision)
  * Vector input count:
    `-e` (1 for scalar variants, 2/4/8/16 for vector variants, 32 for vector array variants)
  * Test Type:
    `-t` (perf for performance, conf for conformance, accu for accuracy)
  * Input count:
    `-c` (Integer value for number of inputs)
  * Input range:
    `-r` (provide range: start,stop,step_size generation algorithm)
  * Number type:
    `-z` (c for complex variant function, default is real variant function)

Example:
```console
$ export LD_LIBRARY_PATH=${PWD}/build/dev-release-gcc/src:$LD_LIBRARY_PATH
$ ./build/dev-release-gcc/aocl_gtests/test_exp -i f -e 1 -t accu -r -79.0,79.0,simple -c 1000
```

#### **Step 6.1: Building with glibc compatibility for tests**
Configure cmake build tests with glibc compatibility api:
```console
$ cmake --preset dev-release-gcc -DLIBABI=glibc --fresh
$ cmake --build --preset dev-release-gcc
$ cmake --build --preset dev-release-gcc --target gtests
$ ./build/{presetName}/glibc_gtests/test_<function> <Test parameters>
```

With this, the tests will be compiled to call _libm_ functions without the amd specific prefix.

#### **Step 6.2: Building with Intel Math (OneAPI) Libraries**
To build tests to exercise Intel math libraries:

1. Install latest version of Intel OneAPI
2. Export Intel OneAPI path to INTEL_PATH:
```console
# Add Intel MKL header files to compiler include search path
# include_directories($ENV{INTEL_PATH}/include)
# Add Intel MKL library directory to linker search path
# link_directories($ENV{INTEL_PATH}/lib)
$ export INTEL_PATH=<intel OneAPI path>
$ cmake --preset dev-release-gcc -DLIBABI=svml --fresh
$ cmake --build --preset dev-release-gcc
$ cmake --build --preset dev-release-gcc --target gtests
$ ./build/{presetName}/svml_gtests/test_<function> <Test parameters>
```

#### **Step 6.3: Glibc compat library**
A glibc compatible library, which has the table of functions compatible with
GLIBC, or compile with AOCC to generate proper API calls to amdlibm:
```console
$ LD_PRELOAD=${PWD}/build/{presetName}/src/compat/glibc-compat.so
```

#### **Step 6.4: AMD fastlibm library**
AMD fast library, with up to 4 ULP (when available):
```console
$ LD_PRELOAD=${PWD}/build/{presetName}/src/fast/libalmfast.so
```

#### **Step 7: Building Examples**
To build examples along with aocl-libm, configure CMake using `-DLIBM_BUILD_EXAMPLES=ON`. This is **OFF by default**.

**Option 1: Build Examples with Main Library**
```console
$ cmake --preset dev-release-gcc -DLIBM_BUILD_EXAMPLES=ON --fresh
$ cmake --build --preset dev-release-gcc
$ cmake --build --preset dev-release-gcc --target test_libm  # Build examples specifically
```

**Option 2: Build Examples Only (Standalone)**
Navigate to the examples folder and configure with library paths:
```console
$ cd examples
$ mkdir build && cd build
$ cmake .. -DAOCL_LIBM="/path/to/aocl-libm_package" -DAOCL_UTILS="/path/to/aocl-utils_package"
$ cmake --build .                                            # test_libm executable is created
```
Executable will be created in build/{presetName}/examples/test_libm

**Examples Build Options:**
- `USE_STATIC_LIB=ON`: Use static library instead of dynamic (default: OFF)

**Example with Options:**
```console
$ cmake --preset dev-release-gcc -DLIBM_BUILD_EXAMPLES=ON -DUSE_STATIC_LIB=ON --fresh
$ cmake --build --preset dev-release-gcc --target test_libm
```
Executable will be created in build/test_libm

**Running Examples:**
The `test_libm` executable will be created as mentioned above and can be run directly:
```console
$ ./build/{presetName}/examples/test_libm
or
$ ./build/test_libm
```

#### **Step 8: Clean Build Files**
To clean all build files, run:
```console
$ cmake --build --preset dev-release-gcc --target clean
```

-----

### **Step 9: Static Dispatch Configuration (Linux Only)**
One can configure the library for a specific CPU architecture by setting the `ALM_STATIC_DISPATCH` option.

**Available options:**
  * `AVX2`   or `ZEN3`: ZEN3 Code path
  * `AVX512` or `ZEN5`: ZEN5 Code path

To configure with a specific option, such as **ZEN3**, use the following command:
```console
$ cmake --preset dev-release-gcc -DALM_STATIC_DISPATCH=ZEN3 --fresh
```
**Note:** The compiled library should only be executed on machines that match the target architecture specified during compilation (e.g., ZEN3 binaries should run on ZEN3-compatible processors).

-----

### **Step 10: Address Sanitizer (Linux Only, GCC Compiler)**
To build with AddressSanitizer enabled, configure CMake using `-DLIBM_ENABLE_ASAN=ON`. This is **OFF by default**.
  * **Redzone Size**: The default redzone size is 16 bytes. To increase it, set the `ASAN_OPTIONS` environment variable. For example, to set it to 2048 bytes, use:
    ```console
    $ ASAN_OPTIONS=redzone=2048 ./<executable_name>
    ```

### **Step 11: Code Coverage (Linux Only, GCC Compiler)**
To build for code coverage, configure CMake using `-DLIBM_ENABLE_COVERAGE=ON`. This is **OFF by default**.
  * **Report Generation**: To generate an HTML code coverage report using LCOV, run the provided bash script.
    1. Run the executable (with arguments)
        ```console
        $ ./<executable_name>
        ```
    2. Now run the following cmake command to generate `index.html` file in the `html_coverage_report` directory.
        ```console
        $ cmake --build --preset dev-release-gcc --target coverage
        ```
Note: ASAN and Code Coverage are features built for developers.

### **Step 12: Sphinx & Doxygen based documentation Build**
To build LibM documentation, configure CMake using `-DLIBM_BUILD_DOCS=ON`. This is **OFF by default**.
```console
$ cmake --preset dev-release-gcc -DLIBM_BUILD_DOCS=ON --fresh
$ cmake --build --preset dev-release-gcc --target libmdoc
```
Note: LibM documentation is found here: **<build>/aocl_docs/html/index.html**

-----


## **💻 Building on Windows**

#### **Configure Ninja & MPFR**
1.  Open a command prompt and navigate to the `aocl-libm` directory.
2.  Run the Visual Studio `vcvarsall.bat` script.
    Replace `"Path_To_VisualStudio"` with your actual installation path.
    ```console
    "Path_To_VisualStudio\VC\Auxiliary\Build\vcvarsall.bat" x64
    ```
3.  Set the `MPFR_PATH` environment variable:
    AOCL LibM test framework uses the following libraries:
    GNU MPFR (an open source multi-precision floating point library),
    GNU GMP (GNU Multiple Precision Arithmetic library),
    GNU MPC (C library for the arithmetic of complex numbers library).
    ```console
    set MPFR_PATH="mpfr_installation_path\mpfr"
    ```
-----

#### **Step 1: List Presets**
To see the available presets for Windows, run:
```console
$ cmake --list-presets
```

This will display a list like this:
```
Available configure presets:

  "dev-win-llvm"           - Developer Config with LLVM-Debug
  "dev-win-release-llvm"   - Developer Config with LLVM-Release
```

#### **Step 2: Configure CMake**
To configure with a preset, such as `dev-win-release-llvm`, run:
```console
$ cmake --preset dev-win-release-llvm --fresh
```

#### **Step 3: Build the Library**
To build the library, run:
```console
$ cmake --build --preset dev-win-release-llvm
```

You can also build in **parallel** using `-j` or in **verbose mode** using `-v`:
```console
$ cmake --build --preset dev-win-release-llvm -j
$ cmake --build --preset dev-win-release-llvm -v
```

#### **Step 4: Installation**
The library will be installed in the `build\{presetName}` directory.
For the example above, this would be `build\dev-win-release-llvm`.

#### **Step 5: Building gtests**
To build all tests, run:
```console
$ cmake --build --preset dev-win-release-llvm --target gtests
```

To build and test a **single API**, use the specific target name, for example:
```console
$ cmake --build --preset dev-win-release-llvm --target test_<function>
```
#### **Step 6: Executing gtests**
Before running tests, you need to add the library's directory and any other dependent DLL paths to your command prompt's `PATH`:
```console
$ set PATH=path\to\library;%PATH%
```
To run the test executable for the required function:
```console
$ build\{presetName}\aocl_gtests\test_<function> with the test parameters
```
Test parameters:
  * Input type:
    `-i` (f for single precision, d for double precision)
  * Vector input count:
    `-e` (1 for scalar variants, 2/4/8/16 for vector variants, 32 for vector array variants)
  * Test Type:
    `-t` (perf for performance, conf for conformance, accu for accuracy)
  * Input count:
    `-c` (Integer value for number of inputs)
  * Input range:
    `-r` (provide range: start,stop,step_size generation algorithm)
  * Number type:
    `-z` (c for complex variant function, default is real variant function)

Example:
```console
$ set PATH=%CD%\build\dev-win-release-llvm\src;%PATH%
$ build\dev-win-release-llvm\aocl_gtests\test_exp.exe -i f -e 1 -t accu -r -79.0,79.0,simple -c 1000
```

#### **Step 7: Clean Build Files**
To clean the build files, run:

```console
$ cmake --build --preset dev-win-release-llvm --target clean
```
-----
