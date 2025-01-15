# CMake to build AOCL-LIBM library

# Requirements
  cmake version greater than 3.23(supports cmake presets)

# Limitation for this Release
  CMake for aocl-libm on Linux is supoorted to build only libraries.
  CMake is fetching aocl-utils from external github i.e., from "https://github.com/amd/aocl-utils.git".

# Steps To Build  Library On Linux
## To list the cmake configuration presetnames
```console
$ cmake --list-presets
Available configure presets:

  "dev-gcc"           - Developer Config with GCC-Debug
  "dev-clang"         - Developer Config with Clang-Debug
  "dev-release-gcc"   - Developer Config with GCC-Release
  "dev-release-clang" - Developer Config with Clang-Release
```

## To configure cmake, select any presetName from --list-presets
```console
$ cmake --preset dev-release-gcc --fresh
```

## To list build presetnames
```console
$ cmake --build --list-presets
```
## To build, select corresponding build presetName from --build --list-presets
```console
$ cmake --build --preset dev-release-gcc
```

## To build library in parallel
```console
$ cmake --build --preset dev-release-gcc -j
```

## To build library in verbose mode
```console
$ cmake --build --preset dev-release-gcc -v
```

## CMake-built aocl-libm library is installed only in release mode
  And the aocl-libm library is installed in "build/{presetName}"


# To test cmake built aocl-libm library
## Build the library and test-framework using scons
    Follow the steps mentioned in "BUILDING.txt"

## To run testsuite
  To run the AOCL LibM test framework for the cmake build aocl-libm library.
  Follow all the steps except exporting scons built aocl-libm library path
  export cmake built aocl-libm library
  ```console
  $export LD_LIBRARY_PATH=./build/${presetName}/src:$LD_LIBRARY_PATH;
  ```

# To configure cmake for STATIC_DISPATCH
Available options for ALM_STATIC_DISPATCH:
  "AVX2"    - Code path set to ZEN2 in aocl-libm library
  "ZEN2"    - Code path set to ZEN2 in aocl-libm library
  "ZEN3"    - Code path set to ZEN3 in aocl-libm library
  "ZEN4"    - Code path set to ZEN4 in aocl-libm library
  "ZEN5"    - Code path set to ZEN5 in aocl-libm library
  "AVX512"  - Code path set to ZEN5 in aocl-libm library
```console
$ cmake --preset dev-release-gcc -DALM_STATIC_DISPATCH=ZEN3 --fresh
```



# Steps To Build  Library On Windows
## Configure Ninja on windows
   On command prompt, under aocl-libm dir, run the following command
```console
   "C:\Program Files\Microsoft Visual Studio\{MSVC_Version}}\Community\VC\Auxiliary\Build\vcvarsall.bat"  x64
```
  MSVC_Version : 2019 0r 2022

## To list the cmake configuration presetnames
```console
$ cmake --list-presets
Available configure presets:

  "dev-win-llvm"         - Developer Config with LLVM-Debug on windows
  "dev-win-release-llvm" - Developer Config with LLVM-Release on windows
```

## To configure cmake, select any presetName from --list-presets
```console
$ cmake --preset dev-win-release-llvm --fresh
```

## To list build presetnames
```console
$ cmake --build --list-presets
```
## To build, select corresponding build presetName from --build --list-presets
```console
$ cmake --build --preset dev-win-release-llvm
```

## To build library in parallel
```console
$ cmake --build --preset dev-win-release-llvm -j
```

## To build library in verbose mode
```console
$ cmake --build --preset dev-win-release-llvm -v
```

## CMake-built aocl-libm library is installed only in release mode
  And the aocl-libm library is installed in "build/{presetName}" dir


# To test cmake built aocl-libm library
## Build the library and test-framework using scons
    Follow the steps mentioned in "BUILDING_on_Win.txt"

## To run testsuite
  To run the AOCL LibM test framework for the cmake build aocl-libm library.
  Follow the steps by setting path for scons built aocl-libm library path
  set PATH to cmake built aocl-libm library
  ```console
  $set PATH=%PATH%;cmake_built_aocl-libm_library
  ```