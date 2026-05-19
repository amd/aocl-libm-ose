# How to Build and Test AOCL-LibM

This guide covers building and testing AOCL-LibM using CMake (recommended) or SCons on Linux and Windows.

---

## 1. Building with CMake (Recommended)

CMake is the recommended build system for AOCL-LibM. For detailed CMake build instructions, see `docs/CMakeBuildSystem.md`.

---

## 2. Building with SCons

### 2.1 Dependencies

Clone and build the libaoclutils library:

```sh
git clone git@github.amd.com:AOCL/aocl-utils.git -b amd-main
```

Follow the steps in `aocl-utils/README.md` to compile and install.

### 2.2 Linux

#### 2.2.1 Set Up Virtual Environment

```sh
# Install virtualenv (Ubuntu/CentOS/SLES)
sudo apt install virtualenv           # Ubuntu
sudo yum install python3-virtualenv   # CentOS
sudo zypper install python3-virtualenv # SLES

# Create and activate
virtualenv -p python3 .venv3
source .venv3/bin/activate
pip install scons
export PATH=`pwd`/.venv3/bin:$PATH
```

#### 2.2.2 Build Library

```sh
scons -j32 --aocl_utils_install_path=<libaoclutils library path>
```

#### 2.2.3 Build Options

| Flag | Description |
|------|-------------|
| `ALM_CC=<path>` `ALM_CXX=<path>` | Select compiler (gcc/clang) |
| `--verbose=1` | Build with verbosity |
| `--debug_mode=all/libs/gtests` | Build with debug info |
| `--developer=1` | Developer mode |
| `--use_asan=1` | Build with AddressSanitizer (set `ASAN_OPTIONS=verify_asan_link_order=0` first) |
| `--aocl_utils_link=0/1` | Dynamic (0) or static (1, default) linking of libaoclutils |

#### 2.2.4 Build Test Framework

**Prerequisites:** Install MPFR, GMP, MPC libraries for multi-precision reference computations:

```sh
# Ubuntu
sudo apt install libmpfr-dev libmpc-dev

# CentOS
sudo yum install mpfr-devel.x86_64 libmpc-devel

# SLES
sudo zypper install mpfr-devel mpc-devel
```

```sh
scons <build options> gtests

# With AVX-512:
scons <build options> gtests -j32 --arch_config=avx512 --aocl_utils_install_path=<path>
```

#### 2.2.5 Run Tests

Set up library paths and run:

```sh
export LD_LIBRARY_PATH=./build/aocl-release/src/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./build/aocl-release/gtests/libs/mparith/32/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./build/aocl-release/gtests/libs/mparith/64/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./build/aocl-release/gtests/gapi/gtest/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./build/aocl-release/gtests/gapi/gbench/:$LD_LIBRARY_PATH

./build/aocl-release/gtests/exp/test_exp -t accu -r -10.0,10.0,simple -c 1000
```

#### 2.2.6 Test Parameters

| Flag | Description |
|------|-------------|
| `-i f/d` | Input type: `f` (single precision), `d` (double precision) |
| `-e 1/2/4/8/16/32` | Vector count: `1` (scalar), `2/4/8/16` (vector), `32` (vector array) |
| `-t perf/conf/accu` | Test type: performance, conformance, or accuracy |
| `-c <count>` | Number of input values |
| `-r <start>,<stop>,<algo>` | Input range with algorithm: `simple`, `random`, `linear`, `binadex` |
| `-z c` | Complex variant function (default is real) |

> **Note:** For multivariant (bivariate) functions (e.g., `pow`, `atan2`), provide two `-r` arguments, one per input:
> `-r <start1>,<stop1>,<algo> -r <start2>,<stop2>,<algo>`
> ```
> ./build/aocl-release/gtests/pow/test_pow -t accu -r -10,10,simple -r 0,5,simple -c 1000
> ```

#### 2.2.7 Alternative Test ABIs

**Glibc compatibility:**

```sh
scons --libabi=glibc
```

Tests will call standard libm functions (e.g., `sin`, `cos`, `exp`) instead of AMD-prefixed functions (e.g., `amd_sin`, `amd_cos`, `amd_exp`).

**Glibc compat preload library:**

```sh
LD_PRELOAD=./build/aocl-release/src/compat/glibc-compat.o
```

**Intel Math (OneAPI) Libraries:**

```sh
export INTEL_LIB_PATH=<path to intel OneAPI installation>
scons --libabi=svml
export LD_LIBRARY_PATH=<intel OneAPI lib path>:$LD_LIBRARY_PATH
./build/svml-release/gtests/<function>/test_<function>
```

### 2.3 Windows

Tested on Windows 10/11 with Visual Studio 2022 and LLVM clang-cl.

#### 2.3.1 Set Up Build Environment

Open Command Prompt and run:

```bat
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set MPFR_PATH="C:\path\to\mpfr"

pip install virtualenv

REM If 'python' is not in PATH, use the full path to python.exe:
REM Example: C:\Users\<username>\AppData\Local\Programs\Python\Python311\python.exe -m virtualenv -p python .venv3
python -m virtualenv -p python .venv3
.venv3\Scripts\activate
pip install scons
```

#### 2.3.2 Build Library

Use 8.3 short path for LLVM to avoid quoting issues:

```bat
scons ALM_CC="C:\PROGRA~1\LLVM\bin\clang-cl.exe" ^
      ALM_CXX="C:\PROGRA~1\LLVM\bin\clang-cl.exe" ^
      --cache-disable -j32 ^
      --aocl_utils_install_path="C:\path\to\aocl-utils-install" ^
      --aocl_utils_link=1
```

Where `aocl-utils-install` is the directory containing `include/` and `lib/` from your aocl-utils build.

#### 2.3.3 Build Options

| Flag | Description |
|------|-------------|
| `ALM_CC=<path>` `ALM_CXX=<path>` | Select compiler (clang-cl) |
| `--verbose=1` | Build with verbosity |
| `--debug_mode=all` | Build with debug info |
| `--use_asan=1` | ASAN (link aocl_utils dynamically: `--aocl_utils_link=0`) |
| `--aocl_utils_link=0/1` | Dynamic (0) or static (1, default) linking |
| `--libabi=msvc` | Test against Microsoft default math library |

#### 2.3.4 Build Test Framework

**Prerequisites:** Set up MPFR, GMP, MPC libraries. Place the `mpfr` folder alongside `aocl-libm`:

```
Main_Directory
+---aocl-libm
+---mpfr
    +---gmp_x64-windows
    +---mpfr_x64-windows
    +---mpc_x64-windows
```

If you encounter MPFR errors, see the [Troubleshooting (Windows)](#4-troubleshooting-windows) section.

```bat
scons ALM_CC="C:\PROGRA~1\LLVM\bin\clang-cl.exe" ^
      ALM_CXX="C:\PROGRA~1\LLVM\bin\clang-cl.exe" ^
      --cache-disable -j32 gtests ^
      --aocl_utils_install_path="C:\path\to\aocl-utils-install" ^
      --aocl_utils_link=1
```

With AVX-512:

```bat
scons ALM_CC="C:\PROGRA~1\LLVM\bin\clang-cl.exe" ^
      ALM_CXX="C:\PROGRA~1\LLVM\bin\clang-cl.exe" ^
      --cache-disable -j32 gtests ^
      --aocl_utils_install_path="C:\path\to\aocl-utils-install" ^
      --aocl_utils_link=1 --arch_config=avx512
```

#### 2.3.5 Run Tests

First, ensure you are in the AOCL-LibM repository root directory (the directory containing `build\`):

```bat
cd C:\path\to\aocl-libm
```

Set PATH for all runtime DLLs before running any test:

```bat
set PATH=%PATH%;%CD%\build\aocl-release\src
set PATH=%PATH%;%CD%\build\aocl-release\gtests\libs\mparith\32
set PATH=%PATH%;%CD%\build\aocl-release\gtests\libs\mparith\64
set PATH=%PATH%;%MPFR_PATH%\gmp_x64-windows\bin
set PATH=%PATH%;%MPFR_PATH%\mpfr_x64-windows\bin
set PATH=%PATH%;%MPFR_PATH%\mpc_x64-windows\bin
```

Run a test:

```bat
.\build\aocl-release\gtests\exp\test_exp.exe -t accu -r -10.0,10.0,simple -c 1000
```

**Note:** All the above PATH entries are required. The shared libraries
(`libalm.dll`, `mparith32.dll`, `mparith64.dll`) are built in separate
directories from the test executables. Without these PATH entries,
tests will fail with "DLL not found" errors.

Test parameters are the same as Linux (see [Test Parameters](#226-test-parameters)).

#### 2.3.6 Alternative Test ABIs

**Microsoft Math Library:**

```bat
scons <build options> --libabi=msvc
```

**Intel Math Libraries on Windows:**

1. Install Intel HPC Toolkit V2023
2. Set `INTEL_LIB_PATH`:
   ```bat
   set INTEL_LIB_PATH=C:\Program Files (x86)\Intel\oneAPI\compiler\2023.2.0\windows\compiler\lib\intel64_win
   ```
3. Build with `--libabi=svml`

#### 2.3.7 Clean Up

```bat
deactivate
scons -c
```

---

## 3. Fast Library

A fast library with **accuracy** up to 4 ULP (when available):

**Linux:**

```sh
LD_PRELOAD=/abs/path/libalmfast.so
```

**Windows:**

```bat
set PATH=C:\path\to\libalmfast;%PATH%
```

---

## 4. Troubleshooting (Windows)

### DLL Load Failures

**Symptom:** `The code execution cannot proceed because ... .dll was not found.`

**Fix:** Ensure all required DLL directories are in PATH (see [Section 2.3.5](#235-run-tests)), or copy the DLLs alongside the test `.exe` file.

### MPFR Library Errors

**Option 1: Reinstall via vcpkg**

```bat
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe install yasm-tool:x86-windows
.\vcpkg\vcpkg.exe install gmp[core]:x64-windows
.\vcpkg\vcpkg.exe install mpfr:x64-windows
.\vcpkg\vcpkg.exe install mpc:x64-windows
```

Packages will be installed in `.\vcpkg\packages`.

**Option 2: Reinstall via Cygwin**

1. Download Cygwin from https://cygwin.com/
2. Install packages: `gcc-core`, `mpclib-1.2.1-1-src`
3. Libraries will be at:
   - `/cygdrive/c/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/lib`
   - `/cygdrive/c/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/bin`

It is recommended to use MPC library version 1.2.1 on Windows.

### Finding the Right Compiler

```bat
where clang-cl
clang-cl --version
```

### MSVC Environment Not Set

Always call `vcvarsall.bat x64` before building in a fresh shell.
