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
message("LIBABI : ${LIBABI}")

include(Cct_GtestGbench)
include(mpfr)
include(Cct_Mparith)

# Set build root and source paths
set(BUILDROOT    "${CMAKE_BINARY_DIR}/${LIBABI}_gtests")
set(MPARITH_PATH "${MPARITH_LIB_DIR}")
set(GTEST_PATH   "${GTEST_LIB_PATH}")
set(GBENCH_PATH  "${GBENCH_LIB_PATH}")
set(GTEST_LIBS   "${GTEST_PATH}" "${GBENCH_PATH}")

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
    list(APPEND TEST_CFLAGS "-O0")
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
    list(APPEND TEST_CFLAGS "-O0")
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
    list(APPEND TEST_CFLAGS "-Wno-unused-parameter")
    list(APPEND TEST_CFLAGS "-Wno-missing-field-initializers")
    add_definitions(-DBENCHMARK_STATIC_DEFINE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# =============================================================================
# AVX-512 Detection and Configuration
# =============================================================================
# Function to check AVX-512 support
function(check_avx512_support)
    message(STATUS "Checking for AVX-512 support...")

    # Set appropriate flags based on compiler
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(AVX512_TEST_FLAGS "/arch:AVX512")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(AVX512_TEST_FLAGS "-march=znver5")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(AVX512_TEST_FLAGS "-march=znver5")
    endif()

    # Create a test file for AVX-512 compilation
    file(WRITE ${PROJECT_BINARY_DIR}/avx512.cpp
    "#include <immintrin.h>
    int main() {
        __m512 a = _mm512_set1_ps(1.0f);
        __m512 b = _mm512_set1_ps(2.0f);
        __m512 c = _mm512_add_ps(a, b);
        return 0;
    }")

    # Try to compile a simple AVX-512 test
    try_compile(HAVE_AVX512_COMPILE
        ${PROJECT_BINARY_DIR}/avx512_test
        ${PROJECT_BINARY_DIR}/avx512.cpp
        COMPILE_DEFINITIONS ${AVX512_TEST_FLAGS}
        OUTPUT_VARIABLE AVX512_COMPILE_OUTPUT
    )

    if(HAVE_AVX512_COMPILE)
        message(STATUS "AVX-512 compilation test: PASSED")
        set(AVX512_SUPPORTED TRUE PARENT_SCOPE)
    else()
        message(STATUS "AVX-512 compilation test: FAILED")
        set(AVX512_SUPPORTED FALSE PARENT_SCOPE)
    endif()

    file(REMOVE ${PROJECT_BINARY_DIR}/avx512.cpp)
    file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/avx512_test)
endfunction()

# Check AVX-512 support if enabled
if (${${PROJECT_PREFIX}_LIBM_ENABLE_AVX512})
    check_avx512_support()

    if(AVX512_SUPPORTED)
        message(STATUS "AVX-512 support enabled and available")

        # Add AVX-512 compile flags
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            list(APPEND TEST_CFLAGS "/arch:AVX512" "-D__AVX512__")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            list(APPEND TEST_CFLAGS "-mavx512f" "-D__AVX512__")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            list(APPEND TEST_CFLAGS "-mavx512f" "-D__AVX512__")
        endif()

        set(AVX512_ENABLED TRUE)
    else()
        message(STATUS "AVX-512 support is not available on target cpu")
        set(AVX512_ENABLED FALSE)
    endif()
else()
    message(STATUS "AVX-512 support disabled (use -DLIBM_ENABLE_AVX512=ON to enable)")
    set(AVX512_ENABLED FALSE)
endif()

# =============================================================================

# Address sanitizer support
if (${${PROJECT_PREFIX}_LIBM_ENABLE_ASAN})
    list(APPEND TEST_CFLAGS "-fsanitize=address")
endif()

# Include and library paths
include_directories(${ALM_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/include/almtest
                    ${MPARITH_INCLUDE_DIR}
                    ${GTEST_INCLUDE_DIR}
                    ${GBENCH_INCLUDE_DIR})

link_directories(${ALM_PATH} ${MPARITH_PATH} ${GTEST_LIBS})

# Libraries to link
set(LIBS ${GTEST_LIB} ${GBENCH_LIB} ${LIBMPARITH} ${MPFR_LIB} ${MPC_LIB} ${GMP_LIB})
if(LIBABI STREQUAL "aocl")
    add_definitions(-DLIBM_PROTOTYPE=${PROTOTYPE_AOCL})
    if(NOT WIN32)
        set(LIBALM "alm")
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
        list(APPEND LINK_FLAGS "-fopenmp")
    else()
        list(APPEND LIBS "svml_dispmd" "libirc" "mkl_intel_thread" "libomp")
    endif()
endif()

#target_link_libraries(<target_name> gtest gtest_main benchmark::benchmark benchmark::benchmark_main)
# Add default math library for any unresolved functions
if(NOT WIN32)
    list(APPEND LIBS "m")
    if(USE_ASAN)
        list(APPEND LIBS "asan")
    endif()
else()
    list(APPEND LIBS "msvcrt")
endif()
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
  list(APPEND LIBS OpenMP::OpenMP_CXX)
endif()

set(CMAKE_EXECUTABLE_SUFFIX_C "")
set(CMAKE_EXECUTABLE_SUFFIX_CXX "")

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
set(TEST_DIRS "exp" "cos")
foreach(TEST ${TEST_DIRS})
    add_subdirectory(${TEST})
    list(APPEND tests test_${TEST})
endforeach()
add_custom_target(gtests ALL)
add_dependencies(gtests ${tests})
