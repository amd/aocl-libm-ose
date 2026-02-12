#
#  Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the names of its contributors
#     may be used to endorse or promote products derived from this software
#  without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#


cmake_minimum_required(VERSION 3.26)
project(libm_testsuite LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set runtime library to dynamic (shared) for consistency
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<IF:$<CONFIG:Debug>,Debug,>DLL")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/deps/yaml")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/src/shim")
get_filename_component(MPARITH_MODULE_PATH "${PROJECT_SOURCE_DIR}/../gtests/libs/mparith" REALPATH)
list(APPEND CMAKE_MODULE_PATH "${MPARITH_MODULE_PATH}")

# Set common CXX flags
if(WIN32)
    # Windows-specific flags (using MSVC)
    if(MSVC)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /W4 /arch:AVX2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi /Zo /d2Zi+ /W4 /arch:AVX2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4267 /wd4996")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -faligned-new -fno-strict-aliasing")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -Wall -Wno-unknown-pragmas")
    endif()
    # Enable Windows-specific symbols export
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
    # Linux-specific flags (GCC/Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -faligned-new -fno-strict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -Wall -fPIC -Wno-unknown-pragmas")
endif()

set(SRCS  "${PROJECT_SOURCE_DIR}/src/main.cc"
            "${PROJECT_SOURCE_DIR}/src/api_prototype_01.cc"
            "${PROJECT_SOURCE_DIR}/src/api_prototype_02.cc"
            "${PROJECT_SOURCE_DIR}/src/api_prototype_03.cc"
            "${PROJECT_SOURCE_DIR}/src/api_prototype_04.cc"
            "${PROJECT_SOURCE_DIR}/src/api_prototype_05.cc"
            "${PROJECT_SOURCE_DIR}/src/api_prototype_06.cc"
            "${PROJECT_SOURCE_DIR}/src/api_validator.cc"
            "${PROJECT_SOURCE_DIR}/src/generator.cc"
            "${PROJECT_SOURCE_DIR}/src/libm_process.cc"
            "${PROJECT_SOURCE_DIR}/src/packer.cc"
            "${PROJECT_SOURCE_DIR}/src/hrt_utils.cc"
            "${PROJECT_SOURCE_DIR}/src/api_runner.cc"
            "${PROJECT_SOURCE_DIR}/src/ulp.cc"
            "${PROJECT_SOURCE_DIR}/src/verify.cc"
            "${PROJECT_SOURCE_DIR}/src/yaml_read.cc"
            "${PROJECT_SOURCE_DIR}/src/yaml_write.cc")

set(INCS "${PROJECT_SOURCE_DIR}/src/include" "${PROJECT_SOURCE_DIR}/src/mparith")

include(CheckIncludeFile)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-mavx512f" COMPILER_SUPPORTS_AVX512F)
if(COMPILER_SUPPORTS_AVX512F)

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
    )

    file(REMOVE "${PROJECT_BINARY_DIR}/avx512_test.cpp")
    file(REMOVE_RECURSE "${PROJECT_BINARY_DIR}/avx512_executable")

    if(AVX512_RUN_RESULT EQUAL 0)
        add_compile_options("-mavx512f")
        message(STATUS "AVX-512 run result         : PASSED")
    else()
        message(STATUS "AVX-512 run result         : FAILED")
    endif()
endif()

# Include YAML dependency
include(yaml)
include(Cct_Shim)
set(MPARITH_SHARED_LIBS ON)
include(Cct_Mparith)

set(INCS "${INCS}" "${YAML_INCLUDE_DIR}" "${MPARITH_INCLUDE_DIR}")
set(MPARITH_PATH    "${MPARITH_LIB_DIR}")

# Add link directories for libraries
link_directories(${MPARITH_PATH})

set(EXE_NAME "libm_runner")
add_executable(${EXE_NAME} ${SRCS})
target_include_directories(${EXE_NAME} PRIVATE ${INCS})

# Ensure executable name is consistent across platforms
set_target_properties(${EXE_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME} SUFFIX ".x")

# Platform-specific compile options for Windows with Clang
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${EXE_NAME} PRIVATE
        -D_CRT_SECURE_NO_WARNINGS
        -Wno-microsoft-cast
        -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
    )
endif()

# Platform-specific linking
set(LIBS_LIST ${YAML_LIB}  ${LIBMPARITH} ${MPFR_LIB} ${MPC_LIB} ${GMP_LIB})
if(LINUX)
    # Linux-specific libraries (including dl for dynamic loading)
    set(LIBS_LIST ${LIBS_LIST} dl)
endif()
target_link_libraries(${EXE_NAME} PRIVATE ${LIBS_LIST})
target_compile_definitions(${EXE_NAME} PUBLIC REF_LIB="${LIBMPARITH}")

add_custom_target(${EXE_NAME}-distclean
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Remove cmake_generated files and directories"
)