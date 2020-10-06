#!/bin/bash
build_dir="$1"
test_type="$2"
TEST="cos"

EXE=${build_dir}/tests/$TEST/test_$TEST

xranges=(-1000,1000)

echo "Running tests for $TEST()"
input_types=("s1d" "s1f" "v4s" "v2s" "v8s")
test_types=("perf" "accu" "conf")

if [ $test_type = "all" ]; then
for inp in ${input_types[@]};
    do
        for t in ${test_types[@]};
            do
                run_test ${EXE} $inp $t 1 ${xranges}
            done
    done

else
    for inp in ${input_types[@]};
        do
            run_test ${EXE} $inp $test_type 1 ${xranges};
        done
fi

echo "Ran tests for $TEST()"
