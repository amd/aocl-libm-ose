@echo off
set exe=%1
set test_type=%2

set nargs=1
set xranges="0,710"
set input_types="s1d,s1f,v4s,v4d,v2d,v8s"

call scripts_win\common.bat :run_exe_nargs !exe! !test_type! !nargs! !input_types! !xranges!

