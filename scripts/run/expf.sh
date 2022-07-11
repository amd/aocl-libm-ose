#!/bin/bash

#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

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
        ${BUILD}/tests/exp/test_exp -i $1 -t $2 -r ${r}  -c 1000000 -l 1000
    done
}

echo "Running tests for exp()"
run_test "s1d" "perf"
run_test "s1d" "accu"
run_test "s1d" "conf"
run_test "s1d" "special"

run_test "s1f" "perf"
run_test "s1f" "accu"
run_test "s1f" "conf"
run_test "s1f" "special"

run_test "v2d" "perf"
run_test "v2d" "accu"

run_test "v4d" "perf"
run_test "v4d" "accu"

run_test "v4s" "perf"
run_test "v4s" "accu"
echo "Ran tests for exp()"
