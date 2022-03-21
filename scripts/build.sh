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

#script to build libm + test framework

#print script usage
helpfunc() {
    echo "HELP"
    echo "Usage: $0 -b <build type> -c <compiler> -a <arch>"
    echo "Build type: release/glibc/svml"
    echo "Compiler: gcc/aocc"
    echo "Arch: avx512 support"
    exit 1
}

#import common routines and resources
source $(realpath './scripts/common.sh')

opts="a:b:c:h"
while getopts "$opts" opt;
do
    case "${opt}" in
        a ) arch="${OPTARG}" ;;
	    b ) build_type="${OPTARG}" ;;
	    c ) compiler_type="${OPTARG}" ;;
	    h ) helpfunc ;;
        ? ) helpfunc ;;
    esac
done

#print help if invalid args
if [ -z "$build_type" ] || [ -z "$compiler_type" ]
then
     echo "Empty params entered, using default values"
     #helpfunc
     build_type="release"
     compiler_type="gcc"
fi

# avx512 tests
avx512=false
if [ ${arch} = "avx512" ]; then
    avx512=true
    echo "Building with avx512 support"
fi


echo "Build Type: "$build_type
echo "Compiler: "$compiler_type
fw="gtests"

#default compiler exe paths
cc_exe=""
cxx_exe=""

#check if compiler is aocc then checkif clang is added to path
if [ ${compiler_type} = "aocc" ]; then
    var="clang"
    if [[ -z "${var}" ]]; then
        echo "Error! Clang is not added to path"
        exit 1
    fi

    #gets the clang installation path
    GetAOCCPath
    echo ${aocc_install_path}
    #change CXX and CC vars as per clang installed path
    cc_exe=${aocc_install_path}/clang
    cxx_exe=${aocc_install_path}/clang++
    echo ${cc_exe}
    echo ${cxx_exe}
fi

#clean build
clean_cmd="scons -c"
RunCommand "${clean_cmd}";

#build
nproc=$(nproc)
build_cmd="scons -j${nproc} ${fw}";

# avx512
if [ ${avx512} = true ]; then
    build_cmd+=" --arch_config=avx512"
fi

# if aocc, use custom clang paths
if [ ${compiler_type} = "aocc" ]; then
    build_cmd+=" ALM_CC=${cc_exe} ALM_CXX=${cxx_exe}"
fi

build_cmd+=" verbose=1"

#default: libabi=aocl
if [ ${build_type} = "release" ];
then
    RunCommand "${build_cmd}"
    build_dir="aocl-release"
else
    #to run framework with glibc/svml/older amdlibm releases
    RunCommand "${build_cmd} --libabi=${build_type}"
    build_dir="${build_type}-release"
fi

#compile dynamic loading examples
RunCommand "make -C ./examples/ clean";
if [ ${avx512} = true ]; then
    RunCommand "make -C ./examples/ ARCH=avx512";
else
    RunCommand "make -C ./examples/";
fi

echo "Compiled dynamic loading examples";

