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

### Building on Linux 🐧

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

#### **Step 5: Testing**
Before running tests, you need to add the library's directory and any other dependent .so paths to `LD_LIBRARY_PATH`:
To build all the APIs for testing, run:
```console
$ cmake --build --preset dev-release-gcc --target gtests
```

To build and test a **single API**, use the specific target name, for example:
```console
$ cmake --build --preset dev-release-gcc --target test_exp
```

#### **Step 6: Clean Build Files**
To clean all build files, run:
```console
$ cmake --build --preset dev-release-gcc --target clean
```

-----

### Static Dispatch Configuration (Linux Only)
You can configure the library for a specific CPU architecture by setting the `ALM_STATIC_DISPATCH` option.

**Available options:**
  * `AVX2` or `ZEN2`: Code path for ZEN2
  * `ZEN3`: Code path for ZEN3
  * `ZEN4`: Code path for ZEN4
  * `ZEN5` or `AVX512`: Code path for ZEN5

To configure with a specific option, such as **ZEN3**, use the following command:
```console
$ cmake --preset dev-release-gcc -DALM_STATIC_DISPATCH=ZEN3 --fresh
```

-----

### Building on Windows 💻

#### **Step 1: Configure Ninja & MPFR**
1.  Open a command prompt and navigate to the `aocl-libm` directory.
2.  Run the Visual Studio `vcvarsall.bat` script.
    Replace `"Path_To_VisualStudio"` with your actual installation path.
    ```console
    "Path_To_VisualStudio\VC\Auxiliary\Build\vcvarsall.bat" x64
    ```
3.  Set the `MPFR_PATH` environment variable:
    ```console
    set MPFR_PATH="C:\tools\mpfr"
    ```

#### **Step 2: List Presets**
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

#### **Step 3: Configure CMake**
To configure with a preset, such as `dev-win-release-llvm`, run:
```console
$ cmake --preset dev-win-release-llvm --fresh
```

#### **Step 4: Build the Library**
To build the library, run:
```console
$ cmake --build --preset dev-win-release-llvm
```

You can also build in **parallel** using `-j` or in **verbose mode** using `-v`:
```console
$ cmake --build --preset dev-win-release-llvm -j
$ cmake --build --preset dev-win-release-llvm -v
```

#### **Step 5: Installation**
The library will be installed in the `build/{presetName}` directory.
For the example above, this would be `build/dev-win-release-llvm`.

#### **Step 6: Testing**
Before running tests, you need to add the library's directory and any other dependent DLL
paths to your command prompt's `PATH`:
```console
$ set PATH=path_to_aocl-libm_lib;%PATH%
```

To build all tests, run:
```console
$ cmake --build --preset dev-win-release-llvm --target gtests
```

To build and test a **single API**, such as `test_exp`, run:
```console
$ cmake --build --preset dev-win-release-llvm --target test_exp
```

#### **Step 7: Clean Build Files**
To clean the build files, run:
```console
$ cmake --build --preset dev-win-release-llvm --target clean
```
