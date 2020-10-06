#!/bin/bash
build_dir="$1"
test_type="$2"
TEST="pow"
EXE=${build_dir}/tests/$TEST/test_$TEST

declare -a ranges
xranges=(10,20    \
    5,15)

yranges=(30,35 \
    5,9)

echo "Running tests for $TEST()"
input_types=("s1d" "s1f" "v2d" "v4d" "v4s" "v8s")
test_types=("perf" "accu" "conf")

if [ $test_type = "all" ]; then
for inp in ${input_types[@]};
    do
        for t in ${test_types[@]};
            do
                run_test ${EXE} $inp $t 2 ${xranges} ${yranges}
            done
    done

else
    for inp in ${input_types[@]};
        do
            run_test ${EXE} $inp $test_type 2 ${xranges} ${yranges};
        done
fi

echo "Ran tests for $TEST()"
