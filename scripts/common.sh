
declare -a funcs=("exp" "log" "pow"
                    "sin" "cos" "tan"
                    "fabs" "atan"
                    "sinh" "cosh" "tanh")

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

