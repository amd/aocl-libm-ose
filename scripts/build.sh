#script to build libm + test framework
#check for no of arguments
if [ $# -ne 3 ]; then
    echo "Usage: run.sh <build_type> <compiler_type> <framework>"
    echo "Build type: release/glibc/svml/amdlibm"
    echo "Compiler: gcc/aocc"
    echo "Framework: g for gtest, t for tests"
    exit 1
fi

#import common routines and resources
source $(realpath './scripts/common.sh')

build_type=$1
compiler_type=$2
framework=$3

echo "Build Type: "$build_type
echo "Compiler: "$compiler_type
echo "Chosen framework:"$framework

#choose framework
if [ $framework = "g" ];then
    fw="gtests"
else
    fw="tests"
fi

#check if compiler is aocc then checkif clang is added to path
if [ $compiler_type = "aocc" ]; then
    var="clang"
    if [[ -z "${var}" ]]; then
        echo "Error! Clang is not added to path"
        exit 1
    fi
fi

#navigate to aocl-libm root path
#clean build
RunCommand "scons -c";

#default: libabi=aocl
if [ $build_type = "release" ];
then
    RunCommand "scons -j32 $fw --compiler=$compiler_type"
    build_dir="aocl-release"
else
    #to run framework with glibc/svml/older amdlibm releases
    RunCommand "scons -j32 $fw --compiler=$compiler_type --libabi=$build_type"
    build_dir="$build_type-release"
fi
