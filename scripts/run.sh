#!/bin/bash
#script to build and run the libm compliancy test suite
#run after the build is completed
#check for no of arguments
if [ $# -ne 4 ]; then
    echo "Usage: run.sh <build_type> <test_type> <function> <framework>"
    echo "Build type: release/glibc/amdlibm/svml"
    echo "Test type: perf/accu/conf/all"
    echo "Function: func name: log/pow/sin/all"
    echo "Framework: choose g for gtest, t for test"
    exit 1
fi

#source the common functions and resources
set -a
source $(realpath './scripts/common.sh')

build_type=$1
test_type=$2
func=$3
framework=$4

build_dir=""

echo "Build Type:- " + $build_type
echo "Test type:- " + $test_type

#default: aocl
if [ "$build_type" = "release" ]; then
    build_dir="aocl-release"
else
    #to run framework with glibc/intel/older amdlibm
    build_dir="$build_type-release"
fi

echo "Build dir: " + $build_dir

BUILD=./build/$build_dir

#export lib paths,dependency lib paths
export LD_LIBRARY_PATH=${BUILD}/src/:$LD_LIBRARY_PATH;
if [ $framework = "g" ];then
    echo "Using gtests"
    fw="gtests"
    export LD_LIBRARY_PATH=${BUILD}/gtests/gapi/gbench/:$LD_LIBRARY_PATH;
    export LD_LIBRARY_PATH=${BUILD}/gtests/gapi/gtest/:$LD_LIBRARY_PATH;
else
    fw="tests"
fi

export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=/usr/lib/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=/usr/local/lib64/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=/usr/lib64/:$LD_LIBRARY_PATH;

export LD_LIBRARY_PATH=${BUILD}/$fw/libs/mparith/32/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=${BUILD}/$fw/libs/mparith/64/:$LD_LIBRARY_PATH;

#run either all functions or individually
if [ "$func" = "all" ]; then
    for f in "${funcs[@]}";
        do
            echo "Running tests for func" $f
            exe=${BUILD}/$fw/${f}/test_${f}
            RunCommand `pwd`/scripts/run/${f}.sh "$framework" "$exe" "$test_type" "$input_count"
            echo "Ran tests for func" $f
        done
else
    #run tests for a single function
    echo "Running tests for func" $func
    exe=${BUILD}/$fw/${func}/test_${func}
    RunCommand `pwd`/scripts/run/${func}.sh "$framework" "$exe" "$test_type" "$input_count"
    echo "Ran tests for func" $func
fi

