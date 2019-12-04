#!/bin/bash
#!/bin/bash
build_dir="$1"
BUILD=${BUILD:="build/$build_dir"}
TEST="fabs"
echo $BUILD
echo $TEST
EXE=${BUILD}/tests/$TEST/test_$TEST

xranges=(-1000,1000)

run_test()
{
    export LD_LIBRARY_PATH=`pwd`/${BUILD}/src
    #if conf or special dont do for the ranges
    if [ $2 = "conf" ] || [ $2 = "special" ]; then
        echo "Testing $TEST"
        ${EXE} -i $1 -t $2 -c 1000000 -l 1000    
    else
        for r in ${xranges[@]}; 
            do
                echo "Testing $TEST for [${r}] "
                ${EXE} -i $1 -t $2 -r ${r} -c 1000000 -l 1000
            done
    fi
}

#check if executable is found
if [ ! -f ${EXE} ]; then
    echo "Executable not found!"
    exit 1
fi    

echo "Running tests for $TEST()"
run_test "s1d" "perf"
run_test "s1d" "accu"
run_test "s1d" "conf"

run_test "s1f" "perf"
run_test "s1f" "accu"
run_test "s1f" "conf"

echo "Ran tests for $TEST()"


