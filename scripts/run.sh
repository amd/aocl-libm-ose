#!/bin/bash
#script to build and run the libm compliancy test suite
#run after the build is completed
#check for no of arguments
if [ $# -ne 3 ]; then
    echo "Usage: run.sh <build_type> <test_type> <function>"
    echo "Build type: release/glibc/amdlibm/svml"
    echo "Test type: perf/accu/conf/all"
    echo "Function: func name: log/pow/sin/all"
    exit 1
fi

set -a
source $(realpath './scripts/common.sh')

build_type=$1
test_type=$2
func=$3
build_dir=""

echo "Build Type:- " + $build_type
echo "Test type:- " + $test_type

if [ "$build_type" = "release" ]; then
    build_dir="aocl-release"
else
    build_dir="$build_type-release"
fi

echo "Build dir: " + $build_dir

BUILD=./build/$build_dir
export LD_LIBRARY_PATH=${BUILD}/src/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=${BUILD}/tests/libs/mparith/32/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=${BUILD}/tests/libs/mparith/64/:$LD_LIBRARY_PATH;

#run either all functions or individually
if [ "$func" = "all" ]; then
    for f in "${funcs[@]}";
        do
            RunCommand `pwd`/scripts/run/${f}.sh "$BUILD" "$test_type" "$input_count"
        done
else
    RunCommand `pwd`/scripts/run/${func}.sh "$BUILD" "$test_type" "$input_count"
fi

