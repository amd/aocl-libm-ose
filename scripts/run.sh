#script to build and run the libm compliancy test suite

build_type=$1

echo $build_type

if [ $build_type = "debug" ];
then
    scons --debug_mode=all tests
    build_dir="aocl-debug"

elif [ $build_type = "release" ];
then
    scons -j32 tests
    build_dir="aocl-release"

elif [ $build_type = "developer" ];
then
    scons --developer=2 tests
    build_dir="aocl-dev2"
fi

`pwd`/scripts/run/pow.sh "$build_dir"
`pwd`/scripts/run/log.sh "$build_dir"
`pwd`/scripts/run/expf.sh "$build_dir"
