@echo OFF
REM Copyright (C) 2020, Advanced Micro Devices, Inc. All Rights Reserved

setlocal EnableDelayedExpansion

SET ARGS_COUNT=0
FOR %%A in (%*) DO SET /A ARGS_COUNT+=1

if not !ARGS_COUNT! == 3 (
    echo "Usage: run.bat <build_type> <test_type> <function>"
    echo "Build type: Release/Debug"
    echo "Test type: perf/accu/conf/all"
    echo "Function: func name: log/pow/sin/all"
    exit /b
)

SET funcs=exp log pow sin cos tan sinh cosh tanh

SET input_count=10

SET build_type_name=%1
set test_type_name=%2
set func=%3

rem set BUILD=.\build\!build_dir!
set BUILD=.\bin\!build_type_name!

REM run either all functions or individually
if !func! == all (
    for %%f in (%funcs%) do (
        set f_val=%%f
        echo Running all tests for func !f_val!
        set exe=!BUILD!\test_!f_val!.exe
        call scripts_win\common.bat :RunCommand scripts_win\run\!f_val!.bat !exe! !test_type_name! !input_count!
        echo Ran tests for func !f_val!
    )
) else (
    echo Running tests for single func !func!
    set exe=!BUILD!\test_!func!.exe
    call scripts_win\common.bat :RunCommand scripts_win\run\!func!.bat !exe! !test_type_name! !input_count!
    echo Ran tests for func !func!
)
