#script to build and run the libm compliancy test suite

git checkout master
git pull

build_type=$1

test_type=$2

echo $build_type

scons -c;

if [ $build_type = "debug" ];
then
    scons --debug_mode=all tests
    build_dir="aocl-debug"

elif [ $build_type = "release" ];
then
    scons tests
    build_dir="aocl-release"

elif [ $build_type = "developer" ];
then
    scons --developer=2 tests
    build_dir="aocl-dev2"

else
    echo "Invalid option:choose from debug or release"
    exit 1
fi

`pwd`/scripts/run/pow.sh "$build_dir" "$test_type"
`pwd`/scripts/run/log.sh "$build_dir" "$test_type"
`pwd`/scripts/run/exp.sh "$build_dir" "$test_type"
`pwd`/scripts/run/fabs.sh "$build_dir" "$test_type"
`pwd`/scripts/run/atan.sh "$build_dir" "$test_type"
