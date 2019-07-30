#!/bin/bash

declare -a ranges
ranges=(-28,-18  \
	-17,0    \
	-1,1     \
	0,1      \
	1,10     \
	0,88.7   \
	2,88.7   \
	87,89    \
	89,200)

BUILD=${BUILD:="build/aocl-release"}
TEST=${TEST:="accu"}

export LD_LIBRARY_PATH=`pwd`/${BUILD}/src 
for r in ${ranges[@]}; do
        echo " testing for [${r}] "
	${BUILD}/tests/expm1/test_expm1 -i s1f -t ${TEST} -r ${r}  -c 1000000 -l 1000 
done

