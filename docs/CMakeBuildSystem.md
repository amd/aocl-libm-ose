# CMake to build AOCL-LIBM library

# Requirements
  cmake version greater than 3.22(supports cmake presets)

# Limitation for this Release
  CMake for aocl-libm on Linux: Supported to build only libraries.
  CMake is fetching aocl-utils from an external GitHub repository: https://github.com/amd/aocl-utils.git.

# Steps to Build the Library on Linux
##List CMake Configuration PresetNames
  To list the available CMake configuration presets, run:
```console
$ cmake --list-presets
Available configure presets:

  "dev-gcc"           - Developer Config with GCC-Debug
  "dev-clang"         - Developer Config with Clang-Debug
  "dev-release-gcc"   - Developer Config with GCC-Release
  "dev-release-clang" - Developer Config with Clang-Release
```

## Configure CMake
  To configure CMake using a preset, run:
```console
$ cmake --preset dev-release-gcc --fresh
```

## List Build PresetNames
  To list the available build presets, run:
```console
$ cmake --build --list-presets
Available build presets:

  "dev-gcc"           - Developer Build with GCC-Debug
  "dev-clang"         - Developer Build with Clang-Debug
  "dev-release-gcc"   - Developer Build with GCC-Release
  "dev-release-clang" - Developer Build with Clang-Release
```

## Build the Library
  To build the library using a preset, run:
```console
$ cmake --build --preset dev-release-gcc
```

## Build in Parallel
  To build the library in parallel, run:
```console
$ cmake --build --preset dev-release-gcc -j
```

## Build in Verbose Mode
  To build the library in verbose mode, run:
```console
$ cmake --build --preset dev-release-gcc -v
```

## Installation
  The CMake-built aocl-libm library is installed in the
  build/{presetName} directory in release mode.


## Testing the Library
  1. Build the library and test framework using SCons by
     following the steps in BUILDING.txt.
  2. To run the AOCL LibM test framework for the CMake-built
     aocl-libm library, set the LD_LIBRARY_PATH to include
     the CMake-built aocl-libm library:
  ```console
  $export LD_LIBRARY_PATH=./build/${presetName}/src:$LD_LIBRARY_PATH;
  ```

# Configure CMake for STATIC_DISPATCH
Available options for ALM_STATIC_DISPATCH:
  "AVX2"    - Code path set to ZEN2 in aocl-libm library
  "ZEN2"    - Code path set to ZEN2 in aocl-libm library
  "ZEN3"    - Code path set to ZEN3 in aocl-libm library
  "ZEN4"    - Code path set to ZEN4 in aocl-libm library
  "ZEN5"    - Code path set to ZEN5 in aocl-libm library
  "AVX512"  - Code path set to ZEN5 in aocl-libm library

  To configure CMake with a specific static dispatch option, run:
```console
$ cmake --preset dev-release-gcc -DALM_STATIC_DISPATCH=ZEN3 --fresh
```



# Steps to Build the Library on Windows
## Configure Ninja on Windows
  Open the command prompt and navigate to the aocl-libm directory.
  Run the following command, replacing "Path_To_VisualStudio" with the actual path
  where Visual Studio is installed:
```console
   "Path_To_VisualStudio\VC\Auxiliary\Build\vcvarsall.bat"  x64
```

## List CMake Configuration Preset Names
  To list the available CMake configuration presets, run:
```console
$ cmake --list-presets
Available configure presets:

  "dev-win-llvm"         - Developer Config with LLVM-Debug
  "dev-win-release-llvm" - Developer Config with LLVM-Release
```

## Configure CMake
  To configure CMake using a preset, run:
```console
$ cmake --preset dev-win-release-llvm --fresh
```

## List Build Preset Names
  To list the available build presets, run:
```console
$ cmake --build --list-presets
Available build presets:

  "dev-win-llvm"         - Developer Build with LLVM-Debug
  "dev-win-release-llvm" - Developer Build with LLVM-Release
```

## Build the Library
  To build the library using a preset, run:
```console
$ cmake --build --preset dev-win-release-llvm
```

## Build in Parallel
   To build the library in parallel, run:
```console
$ cmake --build --preset dev-win-release-llvm -j
```

## Build in Verbose Mode
  To build the library in verbose mode, run:
```console
$ cmake --build --preset dev-win-release-llvm -v
```

## Installation
  The CMake-built aocl-libm library is installed in the
  build/{presetName} directory in release mode.

## Testing the Library
  1.Build the library and test framework using SCons by
    following the steps in BUILDING_on_Win.txt.
  2.To run the AOCL LibM test framework for the CMake-built
    aocl-libm library, set the PATH to include the
    CMake-built aocl-libm library:
    ```console
    $set PATH=%PATH%;cmake_built_aocl-libm_library
    ```
