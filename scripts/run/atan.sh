#!/bin/bash
#!/bin/bash
build_dir="$1"
test_type="$2"
BUILD=${BUILD:="build/$build_dir"}
TEST="atan"
echo $BUILD
echo $TEST
EXE=${BUILD}/tests/$TEST/test_$TEST

xranges=(-200,200)

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

input_types=("s1d","s1f")
test_types=("perf","accu","conf")

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



