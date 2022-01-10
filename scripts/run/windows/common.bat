@echo OFF
REM Copyright (C) 2020, Advanced Micro Devices, Inc. All Rights Reserved
call:%*
goto exit

REM for perf/accu
SET input_count=10

REM test types
SET test_types=perf accu conf

REM common routines
:RunCommand
SET cmd=%*
SET ret_code=0

echo !cmd!
call %cmd%

rem for /f "tokens=1,*" %%i in (%cmd%) do set ret_code=%%i
rem echo ret is !ret_code!

rem if not !ret_code! == 0 (
rem  echo Ret Val: !ret_code!
rem exit /b !ret_code!
rem  )
goto exit


REM convert to gtest args
:convert_input_type
set variant_val=%1
REM this will be s1d/v4s, etc
set input=""
REM process -i variant as per gtests
REM eg: if s1d --> -e 1 -i d
REM get scal or vector
set input=--vector=1
if not x%variant:v=%==x%variant% (
    set elem=%variant:~1,1%
    set input=--vector=!elem!
)
REM check precision
set prec=%variant:~2,1%
if %prec% == d (
    set input=!input! -i d
) else (
    set input=!input! -i f
)
goto exit

REM run tests with nargs
:run_exe_nargs
SET EXE=%1
SET test_type=%2
SET nargs=%3
SET input_types=%~4
SET xranges=%5
SET yranges=%6

SET test_types=accu perf conf

if !nargs! == 1 SET yranges=" "

rem set input_list=%input_types:~0,-1%
rem set input_list=%input_types:~1,-1%

if !test_type! == all (
    echo running all tests
    rem SET test_types=perf accu conf
    for %%n in (%input_types%) do (
        echo running test type %%n
        for %%t in (%test_types%) do (
            echo input type %%n
            call :run_test !EXE! %%n %%t !nargs! %xranges% !yranges!
        )
    )
) else (
    echo running test type !test_type!
    for %%i in (%input_types%) do (
        echo input type %%i
        call :run_test !EXE! %%i !test_type! !nargs! %xranges% !yranges!
    )
)
goto exit


REM run test executable
:run_test
set exe=%1
set variant=%2
set test_type=%3
set nargs=%4
set xranges=%5
set yranges=%6

REM check if executable is found
if not exist %exe% (
    echo "Executable !exe! not found!"
    exit /b
)
set input=""
REM if gtest, convert input args
call :convert_input_type !variant!

REM if conf, no ranges/input counts

if !test_type! == conf (
    call :RunCommand !exe! !input! -t !test_type!
) else (
if !nargs! == 1 (
    for %%r in (%xranges%) do (
        call :RunCommand !exe! !input! -t !test_type! -r %%r -c !input_count!
    )
) else (
    if !nargs! == 2 (
        SET XARGS_COUNT=0
        set x_val=%xranges%
        :XLOOP
        if !XARGS_COUNT! LSS !nargs! (
            for /f "tokens=1* delims=; " %%a in (%x_val%) do (
                SET YARGS_COUNT=0
                set x=%%a
                set x_val="%%b"
                SET /A XARGS_COUNT+=1
                set y_val=%yranges%
                :YLOOP
                if !YARGS_COUNT! LSS !nargs! (
                    for /f "tokens=1* delims=; " %%s in (%y_val%) do (
                        set y=%%s
                        SET /A YARGS_COUNT+=1
                        call :RunCommand !exe! !input! -t !test_type! -r "!x!" -r "!y!" -c !input_count!
                        set y_val="%%t"

                    )
                    if defined y_val goto YLOOP
                )
            )
            if defined x_val goto XLOOP
        )
    )
)
)
goto exit


:exit
exit /b