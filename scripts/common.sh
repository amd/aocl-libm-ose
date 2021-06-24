#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

declare -a funcs=(  "exp"  "log" "pow"
                    "fabs" "atan" "log2"
                    "exp2" "expm1"
                    "sin"  "cos"  "tan"
                    "sinh" "cosh" "tanh"
                    "acos" "log10" "asin"
                    )

#for perf/accu
input_count=10

#test types
declare -a test_types=("perf" "accu" "conf")

#find AOCC installed folder
GetAOCCPath() {
    cmd="clang -v"
    readarray -t aocc_install_path <<< "$(${cmd} -a 2>&1 | grep "InstalledDir:")"
    aocc_install_path=${aocc_install_path##*InstalledDir:}
    echo ${aocc_install_path}
    aocc_install_path=${aocc_install_path//[[:blank:]]/}
    export aocc_install_path=${aocc_install_path}
}

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

#convert args
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
    EXE=$1
    nargs=$2
    input_types=$3
    xranges=$4
    yranges=$5

    test_types=("accu" "perf" "conf")

    if [ $nargs = 1 ];then
        yranges=""
    fi

    if [ $test_type = "all" ]; then
        for inp in ${input_types[@]};
            do
                for t in ${test_types[@]};
                    do
                        run_test ${EXE} $inp $t ${nargs} ${xranges} ${yranges}
                    done
            done

    else
        for inp in ${input_types[@]};
            do
                run_test ${EXE} $inp $test_type ${nargs} ${xranges} ${yranges};
            done
    fi
}

#run test executable
run_test()
{
    exe="$1"
    variant="$2"
    test_type="$3"
    nargs="$4"
    xranges="$5"
    yranges="$6"

    #check if executable is found
    if [ ! -f ${exe} ]; then
        echo "Executable ${exe} not found!"
        exit 1
    fi

    input=""
    convert_input_type ${variant}

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

