#!/bin/bash

build_dir="$1"
BUILD=${BUILD:="build/$build_dir"}

echo $BUILD

run_test()
{
    export LD_LIBRARY_PATH=`pwd`/${BUILD}/src
    ${BUILD}/tests/pow/test_pow -i $1 -t $2 -c 1000000 -l 1000
}

echo "Running tests for pow()"
run_test "s1d" "perf"
run_test "s1d" "accu"
run_test "s1d" "conf"

run_test "s1f" "perf"
run_test "s1f" "accu"
run_test "s1f" "conf"

#run_test "v2d" "perf"
#run_test "v2d" "accu"

#run_test "v4d" "perf"
#run_test "v4d" "accu"

#run_test "v4s" "perf"
#run_test "v4s" "accu"
echo "Ran tests for pow()"
