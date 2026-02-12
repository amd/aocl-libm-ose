# LibM Testsuite Documentation

## Overview

This document provides comprehensive build, execution, and troubleshooting instructions for the LibM Testsuite framework across Linux and Windows platforms.

## Experimental Status Notice

> **Important: LibM Testsuite is currently in experimental status.**
>
> This framework is under active development and should be considered a work in progress. Users may encounter:
>
> - **Environment-specific issues**: Platform-dependent behavior and compatibility issues across different operating systems and toolchains
> - **Evolving APIs**: Function signatures, configuration formats, and command-line interfaces may change without backward compatibility
> - **Incomplete features**: Some functionality may be partially implemented, undocumented, or subject to significant modifications
> - **Breaking changes**: Updates may introduce non-backward-compatible changes to APIs, configuration files, and build systems
>
> While functional for development and testing purposes, this framework is not yet recommended for production environments. Please refer to the [Notes & Known Issues](#notes--known-issues) section for current limitations and known problems.

## Table of Contents

- [Building LibM Testsuite with AOCL-LIBM](#building-libm-testsuite-with-aocl-libm)
- [Standalone Build](#standalone-build)
- [Linux Build & Run](#linux--build--run)
- [Windows Build & Run](#windows--build--run)
- [Troubleshooting](#troubleshooting)
- [Notes & Known Issues](#notes--known-issues)
- [Roadmap](#roadmap)

---

## Building LibM Testsuite with AOCL-LIBM

This section describes how to build the LibM Testsuite (libm_runner + shim + yaml) as part of the AOCL-LIBM project.

### Build Instructions

For detailed integrated build instructions, refer to `CMakeBuildSystem.md` — Sections 4.2 & 4.3.

**Step 1: Configure and Build AOCL-LIBM with Testsuite Support**

```console
$ cmake --preset {presetName} -DLIBM_BUILD_TESTSUITE=ON --fresh
$ cmake --build --preset {presetName}
```

**Step 2: Build the LibM Runner Target**

```console
$ cmake --build --preset {presetName} --target libm_runner
```

**Step 3: Configure Environment Variables**

Set the required library paths:

```console
$ export LD_LIBRARY_PATH=/path/to/amd/library:$LD_LIBRARY_PATH
$ export LD_LIBRARY_PATH=/path/to/mparith/library:$LD_LIBRARY_PATH
```

on windows
```sh
set PATH=C:\path\to\mparith\lib;%PATH%
set PATH=C:\path\to\aocl_libm\build\{dev-win-release-llvm}\lib;%PATH%
```

**Step 4: Execute Tests**

Run the test suite with the following syntax:

```console
$ ./libm_runner.x <path_to_shim> <path_to_yml>
```

**Example:**

```console
$ ./build/{presetName}/almbench/libm_runner.x build/external/shim/libshimamd.so almbench/config/generic.yml
```

---

## Standalone To Build Only LibM Testsuite

This section describes how to build the LibM Testsuite independently without the full AOCL-LIBM project.

### Prerequisites

- **AOCL-LIBM**: The AOCL-LIBM library must be pre-built and available before building or running the testsuite
- **CMake**: Version 3.26 or higher
- **C++ Compiler**: GCC or Clang with C++17 support
- **MPFR**: Required for reference calculations

---

## Linux – Build & Run

### Step 1: Create Build Directory

Create and navigate to the build directory:

```sh
mkdir build && cd build
```

### Step 2: Configure CMake

Configure the project with your desired build type and compiler:

```sh
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++
```

**Configuration Options:**

- `-DCMAKE_BUILD_TYPE=Debug` — Build with debug symbols and no optimizations
- `-DCMAKE_BUILD_TYPE=Release` — Build with optimizations enabled
- `-DCMAKE_CXX_COMPILER=g++` — Specify C++ compiler (options: `g++`, `clang++`)

### Step 3: Build the Project

Compile the testsuite:

```sh
cmake --build . --config Debug
```

### Step 4: Execute Tests

The LibM Testsuite supports multiple math library implementations through shims. Choose the appropriate shim for your target library.

#### AMD AOCL-LIBM Shim

**Configure environment:**

```sh
export LD_LIBRARY_PATH=/path/to/amd/library:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/mparith/library:$LD_LIBRARY_PATH
```

**Run tests:**

```sh
./libm_runner.x ../path/to/shim/libshimamd.so ../config/generic.yml
```

**Example:**

```sh
./libm_runner.x external/shim/libshimamd.so ../config/generic.yml
```

#### GLIBC Shim

**Run tests:**

```sh
./libm_runner.x ../path/to/shim/libshimglibc.so ../config/generic.yml
```

#### Intel MKL Shim

**Configure environment (if MKL is not in system paths):**

```sh
export LD_LIBRARY_PATH=/path/to/mkl/lib:$LD_LIBRARY_PATH
```

**Run tests:**

```sh
./libm_runner.x ../path/to/shim/libshimmkl.so ../config/generic.yml
```

---

## Windows – Build & Run

### Prerequisites

Ensure the following tools and libraries are installed:

1. **Visual Studio 2022** with C++ development tools
2. **LLVM/Clang** compiler toolchain
3. **CMake** (version 3.15 or higher) and **Ninja** build system
4. **MPFR** library for arbitrary-precision arithmetic

### Step 1: Initialize MSVC Environment

Open PowerShell or Command Prompt as Administrator and initialize the Visual Studio environment:

```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

**Verify tool installations:**

```cmd
ninja --version
cmake --version
clang --version
```

### Step 2: Configure Environment Variables

Set the required environment variables for dependencies:

```cmd
set REF_MPFR=C:\path\to\mpfr
set PATH=C:\path\to\mpfr\mpfr_x64-windows\bin;C:\path\to\mpfr\gmp_x64-windows\bin;C:\path\to\mpfr\mpc_x64-windows\bin;%PATH%
```
> **Note:** Replace `C:\path\to\` with your actual installation paths.

### Step 3: Create Build Directory

Create and navigate to the build directory:

```cmd
mkdir build && cd build
```

### Step 4: Configure CMake

Configure the project with Ninja generator and Clang compiler:

```cmd
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe"
```

### Step 5: Build the Project

Compile the testsuite:

```cmd
cmake --build . --config Debug
```

### Step 6: Execute Tests

#### AMD AOCL-LIBM Shim

**Configure environment:**
```sh
set PATH=C:\path\to\mparith\lib;%PATH%
set PATH=C:\path\to\aocl_libm\build\{dev-win-release-llvm}\lib;%PATH%
```

**Run tests:**
```cmd
.\libm_runner.x ..\path\to\shim\libshimamd.dll ..\config\generic.yml
```

#### Intel MKL Shim
**Configure environment (if MKL is not in system paths):**

```cmd
set PATH=C:\path\to\mkl\bin;%PATH%
```

**Run tests:**
```cmd
.\libm_runner.x ..\path\to\shim\libshimmkl.dll ..\config\generic.yml
```

---

## Troubleshooting

### Common Issues and Solutions

#### Library Not Found Errors

**Symptom:** Error messages indicating missing shared libraries (`.so` on Linux, `.dll` on Windows).

**Solution:**

- **Linux:** Ensure `LD_LIBRARY_PATH` includes all required library directories before running tests
- **Windows:** Ensure `PATH` includes all required library directories
- Verify that all required binaries exist in the configured paths
- Check that environment variables are set correctly in the current shell session

#### MPFR Dependencies

**Symptom:** Build or runtime errors related to MPFR, GMP, or MPC libraries.

**Solution:**

- Confirm MPFR and its dependencies (GMP, MPC) are properly installed
- Verify the `REF_MPFR` environment variable (Windows) or library paths (Linux) point to valid installations
- Ensure library versions are compatible with the testsuite requirements

#### CMake Configuration Failures

**Symptom:** CMake fails during configuration step.

**Solution:**

- Verify CMake version is 3.15 or higher
- Ensure all prerequisite libraries (AOCL-LIBM, MPFR) are built and accessible
- Check compiler paths and ensure compilers support C++17
- Review CMake output for specific missing dependencies

#### Test Execution Failures

**Symptom:** Tests fail to execute or produce unexpected results.

**Solution:**

- Verify the correct shim library is being used for your target math library
- Ensure YAML configuration files are valid and accessible
- Check for platform-specific known issues in the [Notes & Known Issues](#notes--known-issues) section
- Review test output logs for specific error messages

---

## Notes & Known Issues

### Platform-Specific Issues

#### GCC with GLIBC

- **Function:** `sincos`
- **Issue:** Known glibc bug affects vector variants
- **Workaround:** Currently falling back to scalar implementation

#### Windows + Intel MKL

- **Function:** `ldexp`
- **Issue:** Not supported on Windows platform with MKL shim
- **Status:** Under investigation

#### Windows + AMD AOCL-LIBM

- **Function:** `linearfrac`
- **Issue:** Vector array implementation has compatibility issues
- **Status:** Under investigation

### Incomplete Features

- **`cexp` function:** Complex exponential function is not yet implemented

### Test Configuration Guide

#### Testing All APIs

To test the accuracy of all mathematical functions, use the `generic.yml` configuration:

```sh
./libm_runner.x ../path/to/shim/libshimamd.so ../config/generic.yml
```

#### Testing Individual APIs

For targeted testing of specific functions, use the corresponding YAML configuration files located in the `config/` directory.

**Available test types:**

- **ACCU** — Accuracy tests
- **CONF** — Conformance tests
- **PERF** — Performance benchmarks

**Examples:**

```sh
# Test sine function accuracy
./libm_runner.x ../path/to/shim/libshimamd.so ../config/sin/sin_accu.yml

# Test sine function conformance
./libm_runner.x ../path/to/shim/libshimamd.so ../config/sin/sin_conf.yml

# Benchmark sine function performance
./libm_runner.x ../path/to/shim/libshimamd.so ../config/sin/sin_perf.yml
```

#### Using Master Configuration

The `master.yml` file provides flexible test execution across multiple APIs and test types. Both uppercase and lowercase test type names are supported.

**Syntax:**

```sh
./libm_runner.x <shim_library> ../config/master.yml [API] [TEST_TYPE]
```

**Examples:**

```sh
# Run all tests for acos function
./libm_runner.x ../path/to/shim/libshimamd.so ../config/master.yml acos

# Run accuracy tests for acos function
./libm_runner.x ../path/to/shim/libshimamd.so ../config/master.yml acos ACCU

# Run accuracy tests for acos function (lowercase)
./libm_runner.x ../path/to/shim/libshimamd.so ../config/master.yml acos accu

# Run conformance tests across all APIs
./libm_runner.x ../path/to/shim/libshimamd.so ../config/master.yml CONF
```

---

## Roadmap

The following enhancements and features are planned for future releases:

### High Priority

1. **Post-Processing Framework**
   - Implement comprehensive post-processing tools for test result dump files
   - Add automated analysis and reporting capabilities
   - Provide visualization tools for performance and accuracy metrics

2. **Complex Number API Support**
   - Extend testsuite to support complex mathematical functions
   - Implement comprehensive test coverage for complex APIs
   - Add reference implementations for validation

### Under Consideration

3. **MSVC Shim Implementation**
   - Develop native Microsoft Visual C++ Runtime shim (`shim_msvc.cc`)
   - Enable testing of MSVC's math library implementations
   - Platform: Windows-specific

### Feedback and Contributions

For feature requests, bug reports, or contributions, please contact the development team or refer to the project's contribution guidelines.

---
