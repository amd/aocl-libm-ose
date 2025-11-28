
# libm_almbench — README

> This README consolidates build, run, and troubleshooting instructions for the `libm_almbench` across Linux and Windows environments.

---

## Prerequisites

- **AOCL libm** (aocl-libm) must be built/available before building and running the almbench.
---

## Linux — Build & Run

### 1. Create and enter a build directory

```sh
mkdir build && cd build
```

### 2. Configure CMake

```sh
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++ -DALM_DEPS=ON
```
- `-DCMAKE_BUILD_TYPE=Debug` — build the almbench in Debug mode
- `-DCMAKE_BUILD_TYPE=Release` — build the almbench in Release mode
- `-DCMAKE_CXX_COMPILER=g++` — choose C++ compiler (`g++` or `clang++`)
- `-DALM_DEPS=ON` — build dependent libraries (AMD, MKL, GCC, and YAML Libraries)
- `-DALM_DEPS=OFF` — default; disable dependent builds

### 3. Build

```sh
cmake --build . --config Debug
```

### 4. Run

#### AMD shim
Set library path (if needed):
```sh
export LD_LIBRARY_PATH=/path_to_amd_library:$LD_LIBRARY_PATH
```
Run:
```sh
./libm_almbench ../libs/libshimamd.so ../config/generic.yml
```

#### GCC shim
Run:
```sh
./libm_almbench ../libs/libshimgcc.so ../config/generic.yml
```

#### MKL shim
If MKL libraries are not found, update `LD_LIBRARY_PATH`:
```sh
export LD_LIBRARY_PATH=path_to_mkl/lib:$LD_LIBRARY_PATH
```
Run:
```sh
./libm_almbench ../libs/libshimmkl.so ../config/generic.yml
```
---

## Windows — Build & Run

### Prerequisites
1. Visual Studio 2022 with C++ development tools
2. LLVM/Clang
3. CMake and Ninja
4. MPFR dependencies installed at `C:\drives\mpfr`

### 1.Initialize MSVC environment (PowerShell as Administrator)
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
set REF_MPFR="mpfr_path\mpfr"
set PATH=path_to_aocl_libm\build\dev-win-release-llvm\lib;%PATH%
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

**AMD shim**
```cmd
.\libm_almbench.exe ..\libs\libshimamd.dll ..\config\generic.yml
```

**MKL shim**
Update PATH if MKL is not discovered:
```cmd
set PATH=path_to_mkl\bin;%PATH%
```
Run:
```cmd
.\libm_almbench.exe ..\libs\libshimmkl.dll ..\config\generic.yml
```
---

## Troubleshooting
- Ensure library search paths are set before testing (`LD_LIBRARY_PATH` on Linux, `PATH` on Windows).
- For “DLL/SO not found” errors: verify all required binaries exist in the configured path and environment variables are correctly set.
- Confirm MPFR dependencies are properly installed under `mpfr_path`.

---
## Notes & Known Issues
1. **GCC `sincos`** — known glibc bug; vector variants have issues, currently falling back to scalar implementation.
2. **`cexp`** — not implemented yet.
3. **Windows + MKL** — `ldexp` not supported.
4. **Windows + AMD** - linearfrac vector array has issues.
4. To test accuracy of **all APIs**, a single `generic.yml` can be used.
5. For **individual API testing**, corresponding YAML files are available under `almbench/config/`.
   Ex:
     ./libm_almbench ../libs/libshimamd.so ../config/sin/sin_accu.yml
     ./libm_almbench ../libs/libshimamd.so ../config/sin/sin_conf.yml
     ./libm_almbench ../libs/libshimamd.so ../config/sin/sin_perf.yml

6. For all APIs UT/VT/PERF (lower case also supports) master_tests_all.yml file can be used.
   Ex:
     ./libm_almbench ../libs/libshimamd.so ../config/master_tests_all.yml acos runs specific api test yml files.
     ./libm_almbench ../libs/libshimamd.so ../config/master_tests_all.yml acos VT runs specific api VT files.
     ./libm_almbench ../libs/libshimamd.so ../config/master_tests_all.yml acos vt runs specific api VT files.
     ./libm_almbench ../libs/libshimamd.so ../config/master_tests_all.yml UT runs UT files across all APIs.

---
## To Do
1. After review and merge to `amd-main`, update CMake to include almbench and use common **MPARITH** library.
2. Major task: post-processing dump files.
3. Support complex APIs.
4. If possible, implement MSVC shim (`shim_msvc.cc`).

---
