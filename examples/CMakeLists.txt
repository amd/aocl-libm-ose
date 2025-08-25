#
# Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.26)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(libm_examples LANGUAGES C)
include(CheckCCompilerFlag)

option(USE_STATIC_LIB "Use static library instead of dynamic" OFF)

# Base compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -mavx2 -march=native")

# Check if the compiler supports -mavx512
check_c_compiler_flag("-mavx512f" COMPILER_SUPPORTS_AVX512)

if(COMPILER_SUPPORTS_AVX512)
        # Create a test program to check hardware support
        file(WRITE "${PROJECT_BINARY_DIR}/check_avx512.c" "
            #include <stdio.h>
            #ifdef __GNUC__
            #include <cpuid.h>
            #else
            #error Unsupported compiler
            #endif

            int main() {
                unsigned int eax, ebx, ecx, edx;
                unsigned int leaf = 7, subleaf = 0;

                if (!__get_cpuid_count(leaf, subleaf, &eax, &ebx, &ecx, &edx)) {
                    return 1;
                }

                // Check for AVX-512F (bit 16 of EBX)
                if (ebx & (1 << 16)) {
                    return 0;
                } else {
                    return 1;
                }
            }"
        )

        # Try to compile and run the test program
        try_run(RUN_RESULT COMPILE_RESULT
            "${PROJECT_BINARY_DIR}"
            "${PROJECT_BINARY_DIR}/check_avx512.c"
        )

        if(RUN_RESULT EQUAL 0)
            message(STATUS "Hardware supports AVX-512 - enabling AVX-512 flags")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx512f")
            add_compile_definitions(__AVX512__)
        else()
            message(STATUS "Hardware does NOT support AVX-512")
        endif()
else()
    message(STATUS "Compiler does NOT support AVX-512")
endif()

set(AOCL_LIBM  ${CMAKE_BINARY_DIR} CACHE STRING "Path to Aocl-LibM.")
set(AOCL_UTILS ${AOCL_UTILS_PATH} CACHE STRING "Path to Aocl-Utils.")

set(AMD_LIBM_LIB_PATH    "${AOCL_LIBM}/lib")
set(AMD_UTILS_LIB_PATH   "${AOCL_UTILS}/lib")

# Header paths
include_directories("${AOCL_LIBM}/include"
                    "${AOCL_UTILS}/include")

# Library configuration
if(NOT WIN32)
    # Linux/Unix library configuration
    if(USE_STATIC_LIB)
        set(LIBALM "${AMD_LIBM_LIB_PATH}/libalm.a")
        message(STATUS "Using static library: ${LIBALM}")
    else()
        set(LIBALM "${AMD_LIBM_LIB_PATH}/libalm.so")
        message(STATUS "Using dynamic library: ${LIBALM}")
    endif()
    set(AMD_UTILS "${AMD_UTILS_LIB_PATH}/libau_cpuid.a")
else()
    # Windows library configuration
    if(USE_STATIC_LIB)
        set(LIBALM "${AMD_LIBM_LIB_PATH}/libalm-static.lib")
        message(STATUS "Using static library: ${LIBALM}")
    else()
        set(LIBALM "${AMD_LIBM_LIB_PATH}/libalm.lib")
        message(STATUS "Using dynamic library: ${LIBALM}")
    endif()
    set(AMD_UTILS "${AMD_UTILS_LIB_PATH}/libau_cpuid.lib")
endif()

# Collect all source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# Create the executable
add_executable(test_libm ${SOURCES})
target_link_libraries(test_libm ${LIBALM} ${AMD_UTILS} m stdc++)

# Configuration summary
message(STATUS "=== AOCL LibM Examples Configuration ===")
message(STATUS "Build Type      : ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler Flags  : ${CMAKE_C_FLAGS}")
