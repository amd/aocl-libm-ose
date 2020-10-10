#!/bin/bash
framework="$1"
EXE="$2"

nargs=1
xranges=(-50,50)

input_types=("s1d" "s1f" "v4s" "v4d" "v2d" "v8s")

run_exe_nargs $framework $exe $nargs $input_types $xranges

