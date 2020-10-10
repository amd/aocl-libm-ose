
declare -a funcs=("exp" "log" "pow"
                    "sin" "cos" "tan"
                    "sinh" "cosh" "tanh")

#for perf/accu
input_count=10

#test types
declare -a test_types=("perf" "accu" "conf")

#common routines
RunCommand() {
  typeset cmd="$*"
  typeset ret_code

  echo "Running command:" $cmd
  echo cmd=$cmd
  eval $cmd
  ret_code=$?
  if [ $ret_code != 0 ]; then
    printf "Ret Val: %d\n" $ret_code
    exit $ret_code
  fi
}

#convert to gtest args
convert_input_type()
{
    local variant=$1  #this will be s1d/v4s, etc
    input=""
    #process -i variant as per gtests
    #eg: if s1d --> -e 1 -i d
    #get scal or vector
    if [[ $variant == v* ]]; then
        elem=${variant:1:1}
        input=" --vector=$elem "
    else
        input=" --vector=1 "
    fi
    #check precision
    prec=${variant:2:1}
    if [ $prec = "d" ];then
        input=" ${input} -i d "
    else
        input=" ${input} -i f "
    fi
}

#run tests with nargs
run_exe_nargs()
{
    framework=$1
    EXE=$2
    nargs=$3
    input_types=$4
    xranges=$5
    yranges=$6

    test_types=("accu" "perf" "conf")

    if [ $nargs = 1 ];then
        yranges=""
    fi

    if [ $test_type = "all" ]; then
        for inp in ${input_types[@]};
            do
                for t in ${test_types[@]};
                    do
                        run_test ${framework} ${EXE} $inp $t ${nargs} ${xranges} ${yranges}
                    done
            done

    else
        for inp in ${input_types[@]};
            do
                run_test ${framework} ${EXE} $inp $test_type ${nargs} ${xranges} ${yranges};
            done
    fi
}

#run test executable
run_test()
{
    fw="$1"
    exe="$2"
    variant="$3"
    test_type="$4"
    nargs="$5"
    xranges="$6"
    yranges="$7"

    #check if executable is found
    if [ ! -f ${exe} ]; then
        echo "Executable ${exe} not found!"
        exit 1
    fi

    input=""
    #if gtest, convert input args
    if [ "${fw}" = "g" ];then
        convert_input_type ${variant}
    else
        input=" -i $variant "
    fi

    #if conf, no ranges/input counts
    if [ $test_type = "conf" ]; then
        RunCommand ${exe} $input -t $test_type
    else
        if [ $nargs = 1 ];then
            for r in ${xranges[@]};
                do
                    echo "Testing for range [${r}]"
                    RunCommand ${exe} $input -t $test_type -r ${r} -c $input_count
                done
        elif [ $nargs = 2 ];then
            for r in ${xranges[@]};
                do
                    for r1 in ${yranges[@]};
                        do
                            echo "Testing for ranges [${r}] and [${r1}] "
                            RunCommand ${exe} $input -t $test_type -r ${r} -r ${r1} -c $input_count
                        done
                done
        fi
    fi
}

