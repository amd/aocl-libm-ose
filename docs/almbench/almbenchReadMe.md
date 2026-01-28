This document consolidates build, run, and troubleshooting instructions for the libm_almbench test framework across Linux and Windows.

# Building almbench Along With aocl-libm

Refer to CMakeBuildSystem.md — Sections 4.2 & 4.3 for integrated build instructions.
Then build almbench with below command:
```console
$ cmake --build --preset {presetName} --target libm_almbench
```

To set the library path:
```console
$ export LD_LIBRARY_PATH=/path/to/amd/library:$LD_LIBRARY_PATH
$ export LD_LIBRARY_PATH=/path/to/mparith/library:$LD_LIBRARY_PATH
```

To run:
```console
$ ./libm_almbench <path_to_shim> <path_to_yml>
```

Example:
```console
$ ./build/dev-release-gcc/almbench/libm_almbench build/external/shim/libshimamd.so almbench/config/generic.yml
```

---

# To Build Only almbench Testsuite

## Prerequisites

- **AOCL libm** (aocl-libm) must already be built or available before building/running almbench.

---

## Linux — Build & Run

### 1. Create and enter a build directory

```sh
mkdir build && cd build
```

### 2. Configure CMake

```sh
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++
```
- `-DCMAKE_BUILD_TYPE=Debug` — build the almbench in Debug mode
- `-DCMAKE_BUILD_TYPE=Release` — build the almbench in Release mode
- `-DCMAKE_CXX_COMPILER=g++` — choose C++ compiler (`g++` or `clang++`)

### 3. Build

```sh
cmake --build . --config Debug
```

### 4. Run

#### AMD shim
Set library path (if needed):
```sh
export LD_LIBRARY_PATH=/path/to/amd/library:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/mparith/library:$LD_LIBRARY_PATH
```
Run:
```sh
./libm_almbench ../path/to/shim/libshimamd.so path/to/yml/config/generic.yml
```
Example:
```sh
./libm_almbench external/shim/libshimamd.so ../config/generic.yml
```

#### GLIBC shim
Run:
```sh
./libm_almbench ../path/to/shim/libshimglibc.so path/to/yml/config/generic.yml
```

#### MKL shim
If MKL libraries are not found, update `LD_LIBRARY_PATH`:
```sh
export LD_LIBRARY_PATH=path_to_mkl/lib:$LD_LIBRARY_PATH
```
Run:
```sh
./libm_almbench ../path/to/shim/libshimmkl.so path/to/yml/config/generic.yml
```

---

## Windows — Build & Run

### Prerequisites
1. Visual Studio 2022 with C++ development tools
2. LLVM/Clang
3. CMake and Ninja
4. MPFR dependencies installed at `C:\drives\mpfr`

### 1. Initialize MSVC environment (PowerShell as Administrator)
```cmd
"path_to_msvc\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```
Verify tool versions:
```cmd
ninja --version
cmake --version
clang --version
```

### 2. Set environment paths
```cmd
set REF_MPFR=mpfr_path\mpfr
set PATH=path_to_aocl_libm\build\dev-win-release-llvm\lib;%PATH%
set PATH=path_to_aocl_libm\path\to\mparith\lib;%PATH%
set PATH=mpfr_path\mpfr\mpfr_x64-windows\bin;mpfr_path\mpfr\gmp_x64-windows\bin;mpfr_path\mpfr\mpc_x64-windows\bin;%PATH%
```

### 3. Create and enter build directory
```cmd
mkdir build && cd build
```

### 4. Configure with CMake (standard build)
```cmd
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug -DALM_DEPS=ON -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang++.exe"
```

### 5. Build
```cmd
cmake --build . --config Debug
```

### 6. Run tests

#### AMD shim
```cmd
.\libm_almbench.exe ..\path\to\shim\libshimamd.dll path\to\yml\config\generic.yml
```

#### MKL shim
Update PATH if MKL is not discovered:
```cmd
set PATH=path_to_mkl\bin;%PATH%
```
Run:
```cmd
.\libm_almbench.exe ..\path\to\shim\libshimmkl.dll path\to\yml\config\generic.yml
```

---

## Troubleshooting

- Ensure library search paths are set before testing (`LD_LIBRARY_PATH` on Linux, `PATH` on Windows).
- For "DLL/SO not found" errors: verify all required binaries exist in the configured path and environment variables are correctly set.
- Confirm MPFR dependencies are properly installed under `mpfr_path`.

---

## Notes & Known Issues

1. **GCC `sincos`** — known glibc bug; vector variants have issues, currently falling back to scalar implementation.
2. **`cexp`** — not implemented yet.
3. **Windows + MKL** — `ldexp` not supported.
4. **Windows + AMD** — linearfrac vector array has issues.
5. To test accuracy of **all APIs**, a single `generic.yml` can be used.
6. For **individual API testing**, corresponding YAML files are available under `almbench/config/`.

   Examples:
   ```sh
   ./libm_almbench ../path/to/shim/libshimamd.so ../config/sin/sin_accu.yml
   ./libm_almbench ../path/to/shim/libshimamd.so ../config/sin/sin_conf.yml
   ./libm_almbench ../path/to/shim/libshimamd.so ../config/sin/sin_perf.yml
   ```

7. For all APIs CONF/ACCU/PERF (lower case also supported), `master_tests_all.yml` file can be used.

   Examples:
   ```sh
   ./libm_almbench ../path/to/shim/libshimamd.so ../config/master_tests_all.yml acos
   # Runs specific API test yml files

   ./libm_almbench ../path/to/shim/libshimamd.so ../config/master_tests_all.yml acos ACCU
   # Runs specific API accu files

   ./libm_almbench ../path/to/shim/libshimamd.so ../config/master_tests_all.yml acos accu
   # Runs specific API accu files (lowercase)

   ./libm_almbench ../path/to/shim/libshimamd.so ../config/master_tests_all.yml CONF
   # Runs conformance files across all APIs
   ```

---

## To Do

1. Major task: post-processing dump files.
2. Support complex APIs.
3. If possible, implement MSVC shim (`shim_msvc.cc`).

---
