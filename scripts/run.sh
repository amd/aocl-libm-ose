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

#script to build and run the libm compliancy test suite
#run after the build is completed

#print script usage
helpfunc() {
    echo "HELP"
    echo "Usage: $0 -b <build type> -t <test type> -f <function name> -a avx512(optional)"
    echo "Build type: release/glibc/svml"
    echo "Test type: perf/accu/conf/all"
    echo "Routine: pow/log/exp/sin/all"
    exit 1
}

#source the common functions and resources
set -a
source $(realpath './scripts/common.sh')

opts="a:b:t:f:h"
while getopts "$opts" opt;
do
    case "${opt}" in
        a ) arch="${OPTARG}" ;;
        b ) build_type="${OPTARG}" ;;
        t ) test_type="${OPTARG}" ;;
	    f ) func="${OPTARG}" ;;
        h ) helpfunc ;;
        ? ) helpfunc ;;
    esac
done

#print help if invalid args
if [ -z "$build_type" ] || [ -z "$test_type" ] || [ -z "$func" ]
then
    echo "Empty params entered, using default values"
    #helpfunc
    build_type="release"
    test_type="all"
    func="all"
fi

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
fw="gtests"
export LD_LIBRARY_PATH=${BUILD}/gtests/gapi/gbench/:$LD_LIBRARY_PATH;
export LD_LIBRARY_PATH=${BUILD}/gtests/gapi/gtest/:$LD_LIBRARY_PATH;

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
            RunCommand `pwd`/scripts/run/${f}.sh "$framework" "$exe" "$test_type" "$input_count" "$arch"
            echo "Ran tests for func" $f
        done
else
    #run tests for a single function
    echo "Running tests for func" $func
    exe=${BUILD}/$fw/${func}/test_${func}
    RunCommand `pwd`/scripts/run/${func}.sh "$framework" "$exe" "$test_type" "$input_count" "$arch"
    echo "Ran tests for func" $func
fi

#dynamic loading and dynamic linking examples
echo "Running dynamic loading test application"
RunCommand ${BUILD}/gtests/dynamic/test_dynamic ${BUILD}/src/libalm.so;
echo "Running dynamic linking test examples"
RunCommand `pwd`/examples/test_libm;
