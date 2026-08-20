#
# CMake script to compare CPUID outputs between internal and external utils
#
# Called by CTest with:
#   cmake -DINTERNAL_EXE=<path> -DEXTERNAL_EXE=<path> -P compare_cpuid_outputs.cmake
#

if(NOT INTERNAL_EXE OR NOT EXTERNAL_EXE)
    message(FATAL_ERROR "Both INTERNAL_EXE and EXTERNAL_EXE must be specified")
endif()

# Run internal utils test
execute_process(
    COMMAND ${INTERNAL_EXE}
    OUTPUT_VARIABLE INTERNAL_OUTPUT
    RESULT_VARIABLE INTERNAL_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT INTERNAL_RESULT EQUAL 0)
    message(FATAL_ERROR "Internal utils test failed with exit code ${INTERNAL_RESULT}")
endif()

# Run external aocl-utils test
execute_process(
    COMMAND ${EXTERNAL_EXE}
    OUTPUT_VARIABLE EXTERNAL_OUTPUT
    RESULT_VARIABLE EXTERNAL_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT EXTERNAL_RESULT EQUAL 0)
    message(FATAL_ERROR "External aocl-utils test failed with exit code ${EXTERNAL_RESULT}")
endif()

# Strip all comment lines (starting with #) for comparison
# This allows source identification while still comparing actual results
# Uses single regex to remove all lines beginning with # (consistent with compare_utils.sh)
string(REGEX REPLACE "(^|\n)#[^\n]*" "" INTERNAL_FILTERED "${INTERNAL_OUTPUT}")
string(REGEX REPLACE "\n+" "\n" INTERNAL_FILTERED "${INTERNAL_FILTERED}")
string(REGEX REPLACE "^\n+|\n+$" "" INTERNAL_FILTERED "${INTERNAL_FILTERED}")
string(REGEX REPLACE "(^|\n)#[^\n]*" "" EXTERNAL_FILTERED "${EXTERNAL_OUTPUT}")
string(REGEX REPLACE "\n+" "\n" EXTERNAL_FILTERED "${EXTERNAL_FILTERED}")
string(REGEX REPLACE "^\n+|\n+$" "" EXTERNAL_FILTERED "${EXTERNAL_FILTERED}")

# Compare filtered outputs
if(NOT INTERNAL_FILTERED STREQUAL EXTERNAL_FILTERED)
    message(STATUS "=== Internal utils output ===")
    message(STATUS "${INTERNAL_OUTPUT}")
    message(STATUS "")
    message(STATUS "=== External aocl-utils output ===")
    message(STATUS "${EXTERNAL_OUTPUT}")
    message(STATUS "")
    message(FATAL_ERROR "CPUID outputs differ between internal and external utils!")
endif()

message(STATUS "CPUID comparison PASSED - internal and external utils produce identical results")
