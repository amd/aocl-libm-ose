#!/bin/bash

build_dir="$1"
BUILD=${BUILD:="build/$build_dir"}

declare -a ranges
ranges=(-127.0,-111.9 \
	-111.99,-109.99 \
	-109.9,108.0 \
	-107.9999,-10 \
	-10.0,-1    \
	-1.0,-0    \
	-1.0,1    \
	0.0,1    \
	0,1    \
	1,2    \
	2,10    \
	10,88.7    \
	88.7,90    \
	87,89    \
	89,200)

#ranges=(-740,710)

run_test()
{
    export LD_LIBRARY_PATH=`pwd`/${BUILD}/src
    for r in ${ranges[@]}; do
            echo " testing for [${r}] "
        ${BUILD}/tests/fabs/test_fabs -i $1 -t $2 -r ${r}  -c 1000000 -l 1000
    done
}

echo "Running tests for fabs()"
run_test "s1d" "perf"
run_test "s1d" "accu"
run_test "s1d" "conf"
#run_test "s1d" "special"

run_test "s1f" "perf"
run_test "s1f" "accu"
run_test "s1f" "conf"
#run_test "s1d" "special"
echo "Ran tests for fabs()"
