#!/bin/bash
framework="$1"
EXE="$2"

declare -a ranges
xranges=(10,20    \
    5,15)

yranges=(30,35 \
    5,9)

nargs=2
input_types=("s1d" "s1f" "v2d" "v4d" "v4s" "v8s")

run_exe_nargs $framework $exe $nargs $input_types $xranges $yranges
