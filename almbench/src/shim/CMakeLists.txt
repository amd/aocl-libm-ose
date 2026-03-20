#
#  Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
project(shimlibraries VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(CheckIncludeFile)
include(CheckCXXCompilerFlag)
# Compiler-specific optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT WIN32))
    check_cxx_compiler_flag("-mavx512f" COMPILER_SUPPORTS_AVX512F)
    set(COMMON_COMPILE_OPTIONS
        -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unknown-pragmas
        $<$<CONFIG:Release>:-O3 -DNDEBUG -march=native>
        $<$<CONFIG:Debug>:-g -O0>
        -fPIC
    )
    if(COMPILER_SUPPORTS_AVX512F)
        list(APPEND COMMON_COMPILE_OPTIONS -mavx512f)
    endif()
    set(COMMON_LINK_OPTIONS
        $<$<CONFIG:Release>:-s>  # Strip symbols in release
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND WIN32))
    check_cxx_compiler_flag("-mavx512f" COMPILER_SUPPORTS_AVX512F_CLANG)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        check_cxx_compiler_flag("/arch:AVX512" COMPILER_SUPPORTS_AVX512F_MSVC)
    endif()
    set(COMMON_COMPILE_OPTIONS
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-unused-parameter>
        $<$<CONFIG:Release>:$<$<CXX_COMPILER_ID:MSVC>:/O2 /DNDEBUG> $<$<CXX_COMPILER_ID:Clang>:-O2 -DNDEBUG>>
        $<$<CONFIG:Debug>:$<$<CXX_COMPILER_ID:MSVC>:/Od /Zi> $<$<CXX_COMPILER_ID:Clang>:-O0 -g>>
        $<$<CXX_COMPILER_ID:Clang>:-D_CRT_SECURE_NO_WARNINGS -Wno-microsoft-cast -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-missing-prototypes -Wno-old-style-cast -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unsafe-buffer-usage>
        $<$<CXX_COMPILER_ID:Clang>:-Wno-nonportable-system-include-path -Wno-zero-as-null-pointer-constant -Wno-shorten-64-to-32 -Wno-undef -Wno-switch-default -Wno-unused-command-line-argument>
        $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
        $<$<CXX_COMPILER_ID:Clang>:-mavx2 -mfma>
    )
    if(COMPILER_SUPPORTS_AVX512F_MSVC)
        list(APPEND COMMON_COMPILE_OPTIONS $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX512>)
    endif()
    if(COMPILER_SUPPORTS_AVX512F_CLANG)
        list(APPEND COMMON_COMPILE_OPTIONS $<$<CXX_COMPILER_ID:Clang>:-mavx512f>)
    endif()
    set(COMMON_LINK_OPTIONS)
endif()

# Platform-specific libraries
set(PLATFORM_LIBRARIES)
if(WIN32)
    # Windows-specific libraries
    list(APPEND PLATFORM_LIBRARIES kernel32)
else()
    # Linux/Unix-specific libraries
    list(APPEND PLATFORM_LIBRARIES m dl)
    if(NOT APPLE)
        list(APPEND PLATFORM_LIBRARIES mvec)  # GNU vector math library
    endif()
endif()

# =============================================================================
# AMD Shim Library - Ultra high-performance LD_PRELOAD shim for AMD AOCL
# =============================================================================
if(WIN32)
    set(SHIMAMD libshimamd)
else()
    set(SHIMAMD shimamd)
endif()
set(SHIMAMD_SOURCES shim_amd.cc)
add_library(shimamd SHARED ${SHIMAMD_SOURCES})
target_include_directories(shimamd PRIVATE ${SHIM_INCS})
target_compile_options(shimamd PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_options(shimamd PRIVATE ${COMMON_LINK_OPTIONS})
target_link_libraries(shimamd PRIVATE ${PLATFORM_LIBRARIES})
set_target_properties(shimamd PROPERTIES
        OUTPUT_NAME ${SHIMAMD}
        POSITION_INDEPENDENT_CODE ON
)

# AMD-specific compile definitions
target_compile_definitions(shimamd PRIVATE
        SHIM_VENDOR_AMD=1
        $<$<CONFIG:Release>:SHIM_NO_DEBUG=1>
)

# =============================================================================
# GLIBC Shim Library - Ultra high-performance LD_PRELOAD shim for GLIBC libm
# Only build on Linux/Unix platforms where GLIBC vectorized math is available
# =============================================================================
if(NOT WIN32)
    set(SHIMGLIBC shimglibc)
    add_library(shimglibc SHARED shim_glibc.cc)
    target_include_directories(shimglibc PRIVATE ${SHIM_INCS})
    target_compile_options(shimglibc PRIVATE ${COMMON_COMPILE_OPTIONS})
    target_link_options(shimglibc PRIVATE ${COMMON_LINK_OPTIONS})
    target_link_libraries(shimglibc PRIVATE ${PLATFORM_LIBRARIES})
    set_target_properties(shimglibc PROPERTIES
        OUTPUT_NAME ${SHIMGLIBC}
        POSITION_INDEPENDENT_CODE ON
    )

    # GLIBC-specific compile definitions
    target_compile_definitions(shimglibc PRIVATE
        SHIM_VENDOR_GLIBC=1
        $<$<CONFIG:Release>:SHIM_NO_DEBUG=1>
    )
endif()

# =============================================================================
# Intel MKL Shim Library - Ultra high-performance LD_PRELOAD shim for Intel MKL
# =============================================================================
if(WIN32)
    set(SHIMMKL libshimmkl)
else()
    set(SHIMMKL shimmkl)
endif()
add_library(shimmkl SHARED shim_mkl.cc)
target_include_directories(shimmkl PRIVATE ${SHIM_INCS})
target_compile_options(shimmkl PRIVATE ${COMMON_COMPILE_OPTIONS})
target_link_options(shimmkl PRIVATE ${COMMON_LINK_OPTIONS})
target_link_libraries(shimmkl PRIVATE ${PLATFORM_LIBRARIES})
set_target_properties(shimmkl PROPERTIES
        OUTPUT_NAME ${SHIMMKL}
        POSITION_INDEPENDENT_CODE ON
)

# MKL-specific compile definitions
target_compile_definitions(shimmkl PRIVATE
        SHIM_VENDOR_MKL=1
        $<$<CONFIG:Release>:SHIM_NO_DEBUG=1>
)

# =============================================================================
# Installation configuration
# =============================================================================

# Create include directory in output location
set(SHIM_INCLUDE_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include")
file(MAKE_DIRECTORY ${SHIM_INCLUDE_DIR})

# Copy alm_test.h header to output include directory
if(SHIM_INCS)
    list(GET SHIM_INCS 0 FIRST_INC_DIR)
    if(EXISTS "${FIRST_INC_DIR}/alm_test.h")
        add_custom_command(TARGET shimamd POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       "${FIRST_INC_DIR}/alm_test.h"
                       "${SHIM_INCLUDE_DIR}/alm_test.h"
                       COMMENT "Installing alm_test.h to ${SHIM_INCLUDE_DIR}")
    endif()
endif()

if(WIN32)
    # Windows: only install AMD and MKL shims
    install(TARGETS shimamd shimmkl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION lib
        PUBLIC_HEADER DESTINATION include
    )
else()
    # Linux: install all shims including GLIBC
    install(TARGETS shimamd shimglibc shimmkl
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION lib
        PUBLIC_HEADER DESTINATION include
    )
endif()

# Install any header files if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
    install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )
endif()

# =============================================================================
# Custom targets for convenience
# =============================================================================

# Target to build all shim libraries
if(WIN32)
    add_custom_target(allshims
        DEPENDS shimamd shimmkl
        COMMENT "Building all Windows shim libraries"
    )
else()
    add_custom_target(allshims
        DEPENDS shimamd shimglibc shimmkl
        COMMENT "Building all shim libraries"
    )
endif()

# Target to clean and rebuild all
add_custom_target(rebuild_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target allshims
    COMMENT "Clean and rebuild all shim libraries"
)

# =============================================================================
# Configuration summary
# =============================================================================
message(STATUS "")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "Shim Libraries Configuration Summary")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "Build Type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:         ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:             ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Output Directory:     ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "")
message(STATUS "Libraries to build:")
message(STATUS "  shimamd          - AMD AOCL optimized functions")
if(NOT WIN32)
    message(STATUS "  shimglibc        - GLIBC libm vectorized functions")
endif()
message(STATUS "  shimmkl          - Intel MKL optimized functions")
message(STATUS "")
message(STATUS "Platform libraries:   ${PLATFORM_LIBRARIES}")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "")

# =============================================================================
# Usage information
# =============================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building optimized release libraries for maximum performance")
else()
    message(STATUS "Building debug libraries with symbols for development")
endif()
message(STATUS "")