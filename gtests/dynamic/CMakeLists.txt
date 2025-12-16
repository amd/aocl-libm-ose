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
project(dynamic-tests LANGUAGES C CXX)

# Set C/C++ standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCCompilerFlag)

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/../../include" "inc")

# Collect source files
file(GLOB DYNAMIC_SRCS "src/*.c")
file(GLOB TEST_SRCS "src/test_srcs/*.c")

set(ALL_SRCS ${DYNAMIC_SRCS} ${TEST_SRCS})

# Platform-specific configuration
if(NOT WIN32)
    set(DYNAMIC_TEST_CFLAGS "-g" "-mavx" "-fPIE" "-fpermissive")
    set(DYNAMIC_LIBS "dl" "m")
    set(DYNAMIC_LINKFLAGS "-fPIE")
else()
    # Windows configuration
    set(DYNAMIC_TEST_CFLAGS "")
    set(DYNAMIC_LIBS "msvcrt")
    set(DYNAMIC_LINKFLAGS "/NODEFAULTLIB:libcmt.lib")
endif()

# Check if the compiler supports -mavx512
check_c_compiler_flag("-mavx512f" DYNAMIC_SUPPORTS_AVX512)

if(DYNAMIC_SUPPORTS_AVX512)
    # Check if AVX-512 is supported
    message(STATUS "Checking for AVX-512 hardware support in dynamic tests...")

    # Create a test program to check hardware support
    file(WRITE "${PROJECT_BINARY_DIR}/check_avx512_dynamic.c" "
        #include <stdio.h>
        #include <stdbool.h>

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
            if (!__get_cpuid_max(0, 0) || __get_cpuid_max(0, 0) < 7) return false;
            __cpuid_count(7, 0, eax, ebx, ecx, edx);
            return (ebx & (1 << 16)) != 0; // Bit 16 of EBX indicates AVX-512F
        }
        #endif

        int main() {
            if (!has_avx512()) {
                return 1;
            }
            return 0;
        }"
    )

    # Try to compile and run the test program
    try_run(RUN_RESULT COMPILE_RESULT
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_BINARY_DIR}/check_avx512_dynamic.c"
    )

    if(RUN_RESULT EQUAL 0)
        message(STATUS "Hardware supports AVX-512 - enabling AVX-512 flags for dynamic tests")
        list(APPEND DYNAMIC_TEST_CFLAGS "-mavx512f" "-D__AVX512__")
    else()
        message(STATUS "Hardware does NOT support AVX-512 for dynamic tests")
    endif()
 else()
     message(STATUS "Compiler does NOT support AVX-512 for dynamic tests")
 endif()

# Set source properties - some files need to be compiled as C++
set_source_files_properties(${ALL_SRCS} PROPERTIES LANGUAGE CXX)

# Create the dynamic test executable
add_executable(test_dynamic ${ALL_SRCS})
target_compile_options(test_dynamic PRIVATE ${DYNAMIC_TEST_CFLAGS})
target_link_options(test_dynamic PRIVATE ${DYNAMIC_LINKFLAGS})
target_link_libraries(test_dynamic ${DYNAMIC_LIBS})

# Print configuration summary
message(STATUS "=== Dynamic Tests Configuration ===")
message(STATUS "Compiler flags: ${DYNAMIC_TEST_CFLAGS}")
message(STATUS "Link libraries: ${DYNAMIC_LIBS}")
