#!/bin/bash

#script to build and run the libm compliancy test suite
git checkout master
git pull

#check for no of arguments
if [ $# -ne 3 ]; then
    echo "Usage: run.sh <build_type> <test_type> <compiler_type>"
    echo "Build type: debug/release/glibc/amdlibm"
    echo "Test type: perf/accu/special/conf/all"
    echo "Compiler: gcc/aocc"
    exit 1
fi

build_type=$1
test_type=$2
compiler_type=$3

echo "Build Type:- " + $build_type
echo "Test type:- " + $test_type
echo "Compiler: " + $compiler_type

#check if compiler is aocc then is clang is added to path?
if [ $compiler_type = "aocc" ]; then
    var="clang"
    if [[ -z "${var}" ]]; then
        echo "Error! Clang is not added to path"
        exit 1
    fi
fi

scons -c;

if [ $build_type = "debug" ];
then
    scons --debug_mode=all tests --compiler=$compiler_type
    build_dir="aocl-debug"

elif [ $build_type = "release" ];
then
    scons tests --compiler=$compiler_type
    build_dir="aocl-release"

elif [ $build_type = "developer" ];
then
    scons tests --developer=2 --compiler=$compiler_type
    build_dir="aocl-dev2"

elif [ $build_type = "amdlibm" ];
then
    scons tests --libabi=amdlibm --compiler=$compiler_type
    build_dir="amdlibm-release"

elif [ $build_type = "glibc" ];
then
    scons tests --libabi=glibc --compiler=$compiler_type
    build_dir="glibc-release"

else
    echo "Invalid option:choose from debug/release/developer/amdlibm/glibc"
    exit 1
fi

`pwd`/scripts/run/pow.sh "$build_dir" "$test_type"
`pwd`/scripts/run/log.sh "$build_dir" "$test_type"
`pwd`/scripts/run/exp.sh "$build_dir" "$test_type"
`pwd`/scripts/run/fabs.sh "$build_dir" "$test_type"
`pwd`/scripts/run/atan.sh "$build_dir" "$test_type"
