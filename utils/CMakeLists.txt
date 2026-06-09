#
# Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
#
# AOCL-LibM Internal Utils Module
#

cmake_minimum_required(VERSION 3.26)

# Only set project if building standalone
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(alm_utils VERSION 1.0.0 LANGUAGES C)
    set(ALM_UTILS_STANDALONE ON)
else()
    set(ALM_UTILS_STANDALONE OFF)
endif()

# Source files
set(ALM_UTILS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpuid.c
)

# Include directory
set(ALM_UTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create static library
add_library(alm_utils_static STATIC ${ALM_UTILS_SOURCES})
target_include_directories(alm_utils_static
    PUBLIC
        $<BUILD_INTERFACE:${ALM_UTILS_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<$<NOT:$<BOOL:${ALM_UTILS_STANDALONE}>>:${PROJECT_SOURCE_DIR}/include>
)
# On Windows, use different names to avoid conflict between static lib and DLL import lib
if(WIN32)
    set_target_properties(alm_utils_static PROPERTIES
        OUTPUT_NAME alm_utils_static
        POSITION_INDEPENDENT_CODE ON
    )
else()
    set_target_properties(alm_utils_static PROPERTIES
        OUTPUT_NAME alm_utils
        POSITION_INDEPENDENT_CODE ON
    )
endif()

# Compiler flags
if(MSVC)
    target_compile_options(alm_utils_static PRIVATE /W4)
else()
    target_compile_options(alm_utils_static PRIVATE -Wall -Wextra)
endif()

# Export include directory for parent project (only when embedded in parent build)
if(NOT ALM_UTILS_STANDALONE)
    set(ALM_UTILS_INCLUDE_DIR ${ALM_UTILS_INCLUDE_DIR} PARENT_SCOPE)
endif()

# Tests - enable when building standalone OR when parent project has tests enabled
if(DEFINED ALM_LIBM_BUILD_TESTS)
    # Building as part of libm - use main test setting
    option(ALM_UTILS_BUILD_TESTS "Build utils tests" ${ALM_LIBM_BUILD_TESTS})
else()
    # Standalone build
    option(ALM_UTILS_BUILD_TESTS "Build utils tests" ${ALM_UTILS_STANDALONE})
endif()
if(ALM_UTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
