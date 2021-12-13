@echo off
set exe=%1
set test_type=%2

set nargs=2
set xranges="10,20;5,15"
set yranges="30,35;5,9"
set input_types="s1d,s1f,v2d,v4d,v4s,v8s"

call scripts_win\common.bat :run_exe_nargs !exe! !test_type! !nargs! !input_types! !xranges! !yranges!

