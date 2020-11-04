#!/bin/bash

#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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
