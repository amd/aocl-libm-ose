#!/bin/bash
framework="$1"
exe="$2"

nargs=1
xranges=(-1000,1000)
input_types=("s1d" "s1f")

run_exe_nargs $framework $exe $nargs $input_types $xranges
