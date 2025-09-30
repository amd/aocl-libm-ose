#
# Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})
project(gtest-testsuite-libm LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/gapi")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/mparith")

# Define prototypes
set(PROTOTYPE_GLIBC 0xf1)
set(PROTOTYPE_AOCL  0xf2)
set(PROTOTYPE_SVML  0xf4)

set(LIBABI "aocl" CACHE STRING "Library ABI to use (aocl, glibc, svml)")
message(STATUS "LIBABI : ${LIBABI}")

include(Cct_GtestGbench)
include(mpfr)
include(Cct_Mparith)

# Set build root and source paths
set(BUILDROOT       "${CMAKE_BINARY_DIR}/${LIBABI}_gtests")
set(MPARITH_PATH    "${MPARITH_LIB_DIR}")
set(GTEST_PATH      "${GTEST_LIB_PATH}")
set(GBENCH_PATH     "${GBENCH_LIB_PATH}")
set(GTEST_LIBS      "${GTEST_PATH}" "${GBENCH_PATH}")
set(AOCL_UTILS_PATH "${AOCL_UTILS_LIB_DIR}")

# Get common test sources excluding the excluded list
file(GLOB COMMON_SRCS
    ${PROJECT_SOURCE_DIR}/almtesttype.cc
    ${PROJECT_SOURCE_DIR}/cmdline.cc
    ${PROJECT_SOURCE_DIR}/filter.cc
    ${PROJECT_SOURCE_DIR}/func_var_existence.cc
    ${PROJECT_SOURCE_DIR}/gbench_main.cc
    ${PROJECT_SOURCE_DIR}/gtest_main.cc
    ${PROJECT_SOURCE_DIR}/main.cc
    ${PROJECT_SOURCE_DIR}/random.cc
    ${PROJECT_SOURCE_DIR}/ulp.cc
    ${PROJECT_SOURCE_DIR}/verify.cc
)

set(VR_SRC_FILES
    ${PROJECT_SOURCE_DIR}/Gtest_srcs/gtest_accu.cc
    ${PROJECT_SOURCE_DIR}/Gtest_srcs/gbench_perf.cc
)

set(VRA_SRC_FILES
    ${PROJECT_SOURCE_DIR}/Gtest_srcs/gbench_main_vec_arr.cc
    ${PROJECT_SOURCE_DIR}/Gtest_srcs/gtest_main_vec_arr.cc
)

# Compiler flags
if(NOT WIN32)
    set(TEST_CFLAGS "-std=c++17")
    list(APPEND TEST_CFLAGS "-g")
    list(APPEND TEST_CFLAGS "-funsigned-char" )
    list(APPEND TEST_CFLAGS "-fno-strict-aliasing")
    list(APPEND TEST_CFLAGS "-fstack-protector-all")
    list(APPEND TEST_CFLAGS "-W")
    list(APPEND TEST_CFLAGS "-Wall")
    list(APPEND TEST_CFLAGS "-Wno-multichar")
    list(APPEND TEST_CFLAGS "-Wno-unused-parameter")
    list(APPEND TEST_CFLAGS "-Wno-missing-field-initializers")
    list(APPEND TEST_CFLAGS "-march=native")
    list(APPEND TEST_CFLAGS "-fPIC")
    list(APPEND TEST_CFLAGS "-Wno-tautological-overlap-compare")
    if((CMAKE_C_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
        list(APPEND TEST_CFLAGS "-fext-numeric-literals")
    endif()
else()
    set(TEST_CFLAGS "-std:c++17")
    list(APPEND TEST_CFLAGS "-g")
    list(APPEND TEST_CFLAGS "-bigobj")
    list(APPEND TEST_CFLAGS "/W4")
    list(APPEND TEST_CFLAGS "-march=native")
    list(APPEND TEST_CFLAGS "/Wall")
    list(APPEND TEST_CFLAGS "/w")
    list(APPEND TEST_CFLAGS "/MP")
    list(APPEND TEST_CFLAGS "-funsigned-char")
    list(APPEND TEST_CFLAGS "-fno-strict-aliasing")
    list(APPEND TEST_CFLAGS "-Wno-unused-parameter")
    list(APPEND TEST_CFLAGS "-fstack-protector-all")
    list(APPEND TEST_CFLAGS "-Wno-multichar")
    list(APPEND TEST_CFLAGS "-Wno-c++11-narrowing")
    list(APPEND TEST_CFLAGS "-Wno-missing-field-initializers")
    add_definitions(-DBENCHMARK_STATIC_DEFINE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# =============================================================================
# AVX-512 Detection and Configuration
# =============================================================================
# Function to check AVX-512 support
function(check_avx512_support)
    message(STATUS "Checking for AVX-512 hardware support...")

    # Set appropriate flags based on compiler
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(AVX512_TEST_FLAGS "/arch:AVX512")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(AVX512_TEST_FLAGS "-mavx512f")
    else()
        set(AVX512_TEST_FLAGS "")
        message(WARNING "Unknown compiler ID: ${CMAKE_CXX_COMPILER_ID}. No AVX-512 flags set.")
    endif()

    # Write test program to check AVX-512 support
    file(WRITE "${PROJECT_BINARY_DIR}/avx512_test.cpp"
    "#include <cstdio>
    #include <cstdlib>

    #if defined(_MSC_VER)
    #include <intrin.h>
    bool has_avx512() {
        int cpuInfo[4];
        __cpuid(cpuInfo, 0);
        if (cpuInfo[0] < 7) return false;
        __cpuid(cpuInfo, 7);
        return (cpuInfo[1] & (1 << 16)) != 0; // Bit 16 of EBX indicates AVX-512F
    }
    #else
    #include <cpuid.h>
    bool has_avx512() {
        unsigned eax, ebx, ecx, edx;
        if (!__get_cpuid_max(0, nullptr) || __get_cpuid_max(0, nullptr) < 7) return false;
        __cpuid_count(7, 0, eax, ebx, ecx, edx);
        return (ebx & (1 << 16)) != 0; // Bit 16 of EBX indicates AVX-512F
    }
    #endif

    int main() {
        if (!has_avx512()) {
            std::fprintf(stderr, \"FATAL: AVX-512 not supported on this CPU.\\n\");
            return -1;
        }
        return 0;
    }")

    # Try to compile and run the test executable
    try_run(AVX512_RUN_RESULT AVX512_COMPILE_RESULT
        ${PROJECT_BINARY_DIR}/avx512_executable
        "${PROJECT_BINARY_DIR}/avx512_test.cpp"
        COMPILE_DEFINITIONS ${AVX512_TEST_FLAGS}
        RUN_OUTPUT_VARIABLE AVX512_RUN_OUTPUT
        COMPILE_OUTPUT_VARIABLE AVX512_COMPILE_OUTPUT
    )

    message(STATUS "AVX-512 compilation result : ${AVX512_COMPILE_RESULT}")

    if(AVX512_RUN_RESULT EQUAL 0)
        message(STATUS "AVX-512 run result         : PASSED")
        set(AVX512_SUPPORTED TRUE PARENT_SCOPE)
    else()
        message(STATUS "AVX-512 run result         : FAILED")
        set(AVX512_SUPPORTED FALSE PARENT_SCOPE)
    endif()

    # Clean up
    file(REMOVE "${PROJECT_BINARY_DIR}/avx512_test.cpp")
    file(REMOVE_RECURSE "${PROJECT_BINARY_DIR}/avx512_executable")
endfunction()

# Check AVX-512 support if enabled
if (${${PROJECT_PREFIX}_LIBM_ENABLE_AVX512})
    check_avx512_support()

    if(AVX512_SUPPORTED)
        message(STATUS "AVX-512 support enabled and available - adding AVX-512 flags")
        # Add AVX-512 compile flags
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            list(APPEND TEST_CFLAGS "/arch:AVX512" "-D__AVX512__")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            list(APPEND TEST_CFLAGS "-mavx512f" "-D__AVX512__")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            list(APPEND TEST_CFLAGS "-mavx512f" "-D__AVX512__")
        endif()
    else()
        message(STATUS "Target CPU does not support AVX-512")
    endif()
else()
    message(STATUS "AVX-512 support disabled (use -DLIBM_ENABLE_AVX512=ON to enable)")
endif()
# =============================================================================

# Add sanitizer flags if enabled
if(NOT WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND (${${PROJECT_PREFIX}_LIBM_ENABLE_ASAN}))
    list(APPEND TEST_CFLAGS ${ASAN_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ASAN_LINK_FLAGS}")
endif()

# Add coverage flags if enabled
if(NOT WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND (${${PROJECT_PREFIX}_LIBM_ENABLE_COVERAGE}))
    list(APPEND TEST_CFLAGS ${COVERAGE_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_LINK_FLAG}")
endif()

# Include and library paths
include_directories(${ALM_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/include/almtest
                    ${MPARITH_INCLUDE_DIR}
                    ${GTEST_INCLUDE_DIR}
                    ${GBENCH_INCLUDE_DIR})

link_directories(${ALM_PATH} ${AOCL_UTILS_PATH} ${MPARITH_PATH} ${GTEST_LIBS})

# Libraries to link
set(LIBS ${GTEST_LIB} ${GBENCH_LIB} ${LIBMPARITH} ${MPFR_LIB} ${MPC_LIB} ${GMP_LIB})
if(LIBABI STREQUAL "aocl")
    add_definitions(-DLIBM_PROTOTYPE=${PROTOTYPE_AOCL})
    if(NOT WIN32)
        # Use static library when coverage is enabled to avoid gcov symbol conflicts
        if(${${PROJECT_PREFIX}_LIBM_ENABLE_ASAN} OR ${${PROJECT_PREFIX}_LIBM_ENABLE_COVERAGE})
            set(LIBALM "${CMAKE_BINARY_DIR}/src/libalm.a")
        else()
            set(LIBALM "alm")
        endif()
    else()
        set(LIBALM "libalm")
    endif()
    list(APPEND LIBS ${LIBALM})
elseif(LIBABI STREQUAL "glibc")
    add_definitions(-DLIBM_PROTOTYPE=${PROTOTYPE_GLIBC})
    list(APPEND LIBS "mvec")
elseif(LIBABI STREQUAL "svml")
    add_definitions(-DLIBM_PROTOTYPE=${PROTOTYPE_SVML})

    if(DEFINED ENV{INTEL_PATH})
        include_directories($ENV{INTEL_PATH}/include)
        link_directories($ENV{INTEL_PATH}/lib)
    else()
        message(WARNING "Environment variable INTEL_PATH is not set.")
	message(WARNING "export INTEL_PATH=path_to_mkl.")
    endif()

    list(APPEND LIBS "mkl_intel_lp64" "mkl_core")
    if(NOT WIN32)
        list(APPEND LIBS "svml" "irc" "intlc" "mkl_gnu_thread")
    else()
        list(APPEND LIBS "svml_dispmd" "libirc" "mkl_intel_thread" "libiomp")
    endif()
endif()

#target_link_libraries(<target_name> gtest gtest_main benchmark::benchmark benchmark::benchmark_main)
# Add default math library for any unresolved functions
if(NOT WIN32)
    list(APPEND LIBS "m")
else()
    list(APPEND LIBS "msvcrt" "Shlwapi")
endif()
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
  list(APPEND LIBS OpenMP::OpenMP_CXX)
endif()

# Set common executable suffix on windows and linux
#set(CMAKE_EXECUTABLE_SUFFIX_CXX "")

# Create a static library from COMMON_SRCS
add_library(common OBJECT ${COMMON_SRCS})
target_compile_options(common PUBLIC ${TEST_CFLAGS})

add_library(gtest_vr OBJECT ${VR_SRC_FILES})
target_compile_options(gtest_vr PUBLIC ${TEST_CFLAGS})

add_library(gtest_vra OBJECT ${VRA_SRC_FILES})
target_compile_options(gtest_vra PUBLIC ${TEST_CFLAGS})

# Add test executables
file(GLOB TEST_DIRS RELATIVE ${PROJECT_SOURCE_DIR} "*")
list(REMOVE_ITEM TEST_DIRS "include" "gapi" "libs")
add_custom_target(common_target ALL DEPENDS common)

set(tests "")

# Define test directories with their linking requirements
set(SPECIAL_TEST_DIRS "powx" "linearfrac" "sincos")
set(VR_TEST_DIRS "acosh" "asinh" "atan2" "atanh" "cexp" "sinh" "ceil" "copysign" "fdim" "floor" "fmod" "hypot" "logb" "remainder" "rint" "round" "trunc" "nearbyint")
set(VRA_TEST_DIRS "exp" "cos" "acos" "add" "asin" "atan" "cosh" "sin" "tan" "tanh" "cbrt" "erf" "erfc" "exp10" "exp2" "expm1" "fabs" "fmax" "fmin" "ldexp" "log" "log10" "log1p" "log2" "mul" "nextafter" "pow" "sqrt" "sub")

# Combine all test directories
set(ALL_TEST_DIRS ${SPECIAL_TEST_DIRS} ${VR_TEST_DIRS} ${VRA_TEST_DIRS})

foreach(api ${ALL_TEST_DIRS})
    file(GLOB ${api}_SRCS "${api}/*.cc")
    add_executable(test_${api} ${${api}_SRCS})

    # Link with common libraries for all tests
    target_link_libraries(test_${api} ${LIBS} $<TARGET_OBJECTS:common>)

    # Add gtest_vr object library for VR and VRA test directories
    if(api IN_LIST VR_TEST_DIRS OR api IN_LIST VRA_TEST_DIRS)
        target_link_libraries(test_${api} $<TARGET_OBJECTS:gtest_vr>)
    endif()

    # Add gtest_vra object library for VRA test directories only
    if(api IN_LIST VRA_TEST_DIRS)
        target_link_libraries(test_${api} $<TARGET_OBJECTS:gtest_vra>)
    endif()

    set_target_properties(test_${api} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILDROOT})
    target_compile_options(test_${api} PUBLIC ${TEST_CFLAGS})
    add_dependencies(test_${api} common_target)
    list(APPEND tests test_${api})
endforeach()

add_custom_target(gtests ALL)
add_dependencies(gtests ${tests})

if(NOT WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND (${${PROJECT_PREFIX}_LIBM_ENABLE_COVERAGE}))
    add_custom_target(coverage
        COMMAND lcov --capture --directory "${CMAKE_BINARY_DIR}/src" --output-file "coverage.info"
        COMMAND lcov --remove "coverage.info" --output-file "coverage_filtered.info"
                '/usr/*' '/*/_deps/*' '*/_deps/*'
        COMMAND lcov --remove "coverage_filtered.info" --output-file "coverage_final.info"
                "*/gtests/*" "*gtests/*" "${CMAKE_SOURCE_DIR}/gtests/*" "${CMAKE_BINARY_DIR}/gtests/*"
                "${CMAKE_BINARY_DIR}/external/*" "*/external/*" "*external/*"
        COMMAND genhtml "coverage_final.info" --output-directory "html_coverage_report"
        COMMAND ${CMAKE_COMMAND} -E rm -f "coverage.info" "coverage_filtered.info" "coverage_final.info"
        COMMENT "Generating and cleaning up code coverage report..."
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_dependencies(coverage ${tests})
endif()
