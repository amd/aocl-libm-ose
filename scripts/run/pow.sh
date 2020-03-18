#!/bin/bash
build_dir="$1"

test_type="$2"

BUILD=${BUILD:="build/$build_dir"}
TEST="pow"
echo $BUILD
echo $TEST
EXE=${BUILD}/tests/$TEST/test_$TEST

declare -a ranges
xranges=(0.0,1    \
    0,1    \
    1,2    \
    2,10    \
    10,88.7    \
    88.7,90    \
    87,89    \
    89,200)

yranges=(-127.0,-111.9 \
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

run_test()
{
    export LD_LIBRARY_PATH=`pwd`/${BUILD}/src
    #if conf or special dont do for the ranges
    if [ $2 = "conf" ] || [ $2 = "special" ]; then
        echo "Testing $TEST "
        ${EXE} -i $1 -t $2 -c 1000000 -l 1000
    else
        for r in ${xranges[@]};
            do
                for r1 in ${yranges[@]};
                    do
                        echo "Testing $TEST for [${r}] and [${r1}] "
                        ${EXE} -i $1 -t $2 -r ${r} -r ${r1} -c 1000000 -l 1000
                    done
            done
    fi
}

#check if executable is found
if [ ! -f ${EXE} ]; then
    echo "Executable ${EXE} not found!"
    exit 1
fi

echo "Running tests for $TEST()"

input_types=("s1d","s1f","v2d","v4d","v4s","v8s")
test_types=("perf","accu","conf","special")

if [ $test_type = "all" ]; then
for inp in ${input_types[@]};
    do
        for t in ${test_types[@]};
            do
                run_test $inp $t
            done
    done

else
    for inp in ${input_types[@]};
        do
            run_test $inp $test_type;
        done
fi

echo "Ran tests for $TEST()"
