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
project(GAPI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries instead of static" OFF)

# ============================================================================
# Platform-specific common flags
# ============================================================================
if(WIN32)
    # Windows common settings
    set(COMMON_CXX_FLAGS /std:c++11)
    set(COMMON_DEFINITIONS NDEBUG WIN32 _WINDOWS)
else()
    # Linux/Unix common settings
    set(COMMON_CXX_FLAGS
        -std=c++11
        -O3
        -Wall
        -Wextra
        -fstrict-aliasing
        -Wno-deprecated
        -Wno-deprecated-declarations
        -Werror
    )
    set(COMMON_DEFINITIONS NDEBUG)
endif()

# ============================================================================
# Google Test Library
# ============================================================================

# Collect gtest source files
file(GLOB GTEST_SOURCES "gtest/src/*.cc")

# Library (static or shared based on BUILD_SHARED_LIBS)
if(BUILD_SHARED_LIBS)
    add_library(gtest SHARED ${GTEST_SOURCES})
else()
    add_library(gtest STATIC ${GTEST_SOURCES})
endif()

# Set target properties
set_target_properties(gtest PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(gtest PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest
    ${CMAKE_CURRENT_SOURCE_DIR}/gtest/src
)

# Common flags and definitions
target_compile_options(gtest PRIVATE ${COMMON_CXX_FLAGS})
target_compile_definitions(gtest PRIVATE ${COMMON_DEFINITIONS})

# Platform-specific settings
if(NOT WIN32)
    # Linux/Unix-specific gtest settings
    target_compile_options(gtest PRIVATE
        -Wstrict-aliasing
        -Wno-error=dangling-else
        -fexceptions
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
    target_compile_definitions(gtest PRIVATE GTEST_HAS_PTHREAD=1)
    target_link_libraries(gtest PUBLIC pthread)
endif()

# ============================================================================
# Google Benchmark Library
# ============================================================================

# Collect gbench source files
file(GLOB GBENCH_SOURCES "gbench/src/*.cc")

# Library (static or shared based on BUILD_SHARED_LIBS)
if(BUILD_SHARED_LIBS)
    add_library(benchmark SHARED ${GBENCH_SOURCES})
else()
    add_library(benchmark STATIC ${GBENCH_SOURCES})
endif()

# Set target properties
set_target_properties(benchmark PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Include directories
target_include_directories(benchmark PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gbench
    ${CMAKE_CURRENT_SOURCE_DIR}/gbench/src
)

# Common flags and definitions
target_compile_options(benchmark PRIVATE ${COMMON_CXX_FLAGS})
target_compile_definitions(benchmark PRIVATE ${COMMON_DEFINITIONS})

# Platform-specific settings
if(WIN32)
    target_compile_definitions(benchmark PRIVATE libbenchmark_EXPORTS)
    target_link_libraries(benchmark PUBLIC Shlwapi)
else()
    # Linux/Unix-specific benchmark settings
    target_compile_options(benchmark PRIVATE -Wshadow)
    target_compile_definitions(benchmark PRIVATE
        HAVE_POSIX_REGEX
        HAVE_STD_REGEX
        HAVE_STEADY_CLOCK
    )
    target_link_libraries(benchmark PUBLIC pthread)
endif()
