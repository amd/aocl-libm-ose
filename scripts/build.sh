#script to build libm + test framework
#check for no of arguments
if [ $# -ne 2 ]; then
    echo "Usage: run.sh <build_type> <test_type> <compiler_type>"
    echo "Build type: release/glibc/svml/amdlibm"
    echo "Compiler: gcc/aocc"
    exit 1
fi

source $(realpath './scripts/common.sh')

build_type=$1
compiler_type=$2

echo "Build Type:- " + $build_type
echo "Compiler: " + $compiler_type

#check if compiler is aocc then is clang is added to path?
if [ $compiler_type = "aocc" ]; then
    var="clang"
    if [[ -z "${var}" ]]; then
        echo "Error! Clang is not added to path"
        exit 1
    fi
fi

#navigate to aocl-libm root path
RunCommand "scons -c";

if [ $build_type = "release" ];
then
    RunCommand "scons -j32 tests --compiler=$compiler_type"
    build_dir="aocl-release"
else
    RunCommand "scons -j32 tests --compiler=$compiler_type --libabi=$build_type"
    build_dir="$build_type-release"
fi
