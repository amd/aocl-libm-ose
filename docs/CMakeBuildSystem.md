# CMake to build AOCL-LIBM library on Linux

# Requirements
  cmake version greater than 3.23(supports cmake presets)

# Limitation for this Release
  CMake for aocl-libm on Linux is supoorted to build only libraries.
  CMake for aocl-libm on windows is not supported.
  CMake is fetching aocl-utils from external github i.e., from "https://github.com/amd/aocl-utils.git".

# Building library
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
