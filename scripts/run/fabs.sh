#!/bin/bash
EXE="$1"
test_type="$2"

xranges=(-1000,1000)

input_types=("s1d" "s1f")

run_exe_nargs 2 $input_types $xranges $yranges

