#!/bin/bash

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

BUILD=${BUILD:="build/aocl-dev2"}
TEST=${TEST:="accu"}

export LD_LIBRARY_PATH=`pwd`/${BUILD}/src 
for r in ${ranges[@]}; do
        echo " testing for [${r}] "
	${BUILD}/tests/exp/test_exp -i s1f -t ${TEST} -r ${r}  -c 1000000 -l 1000 
done

