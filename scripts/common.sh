
declare -a funcs=("exp" "log" "pow"
                    "sin" "cos" "tan"
                    "fabs" "atan"
                    "sinh" "cosh" "tanh")

#for perf/accu
input_count=10

#test types
declare -a test_types=("perf" "accu" "conf")

#common routines
RunCommand() {
  typeset cmd="$*"
  typeset ret_code

  echo cmd=$cmd
  eval $cmd
  ret_code=$?
  if [ $ret_code != 0 ]; then
    printf "Ret Val: %d\n" $ret_code
    exit $ret_code
  fi
}

#run test executable
run_test()
{
    exe="$1"
    variant="$2"
    test_type="$3"
    nargs="$4"
    xranges=$5
    yranges=$6

    #check if executable is found
    if [ ! -f ${exe} ]; then
        echo "Executable ${exe} not found!"
        exit 1
    fi

    #if conf, no ranges/input counts
    if [ test_type = "conf" ]; then
        echo "Testing $TEST"
        RunCommand ${exe} -i $variant -t $test_type
    else
        if [ $nargs = 1 ];then
            for r in ${xranges[@]};
                do
                    echo "Testing for range [${r}]"
                    RunCommand ${exe} -i $variant -t $test_type -r ${r} -c $input_count
                done
        elif [ $nargs = 2 ];then
            for r in ${xranges[@]};
                do
                    for r1 in ${yranges[@]};
                        do
                            echo "Testing for ranges [${r}] and [${r1}] "
                            RunCommand ${exe} -i $variant -t $test_type -r ${r} -r ${r1} -c $input_count
                        done
                done
        fi
    fi
}

