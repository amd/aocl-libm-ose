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

# Function to check if a directory exists
function(directory_exists dir result_var)
    if(EXISTS "${dir}" AND IS_DIRECTORY "${dir}")
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Function to check if a library exists
function(library_exists lib libpath result_var)
    find_library(${lib}_FOUND NAMES ${lib} PATHS "${libpath}" NO_DEFAULT_PATH)
    if(${lib}_FOUND)
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
    # Clear the cache variable to avoid conflicts
    unset(${lib}_FOUND CACHE)
endfunction()

# Function to build gapi
function(build_gapi source_dir binary_dir)
    message(STATUS "Building gapi from: ${source_dir}")

    # Configure gapi build
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S "${source_dir}" -B "${binary_dir}"
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                            -DBUILD_SHARED_LIBS=${GTBM_SHARED}
                    RESULT_VARIABLE gapi_configure_result
                    OUTPUT_VARIABLE gapi_configure_output
                    ERROR_VARIABLE gapi_configure_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT gapi_configure_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure gapi build:\n${gapi_configure_error}")
    endif()

    # Build gapi
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${binary_dir}"  --config ${CMAKE_BUILD_TYPE}
                    RESULT_VARIABLE gapi_build_result
                    OUTPUT_VARIABLE gapi_build_output
                    ERROR_VARIABLE  gapi_build_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT gapi_build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build gapi:\n${gapi_build_error}")
    endif()

    message(STATUS "Successfully built gapi")
endfunction()

# Function to configure and build gapi
function(configure_build gapi_source_dir gapi_binary_dir)
    build_gapi("${gapi_source_dir}" "${gapi_binary_dir}")
endfunction()

# Validate required variables
if(NOT DEFINED PROJECT_PREFIX)
    message(FATAL_ERROR "PROJECT_PREFIX must be defined before including this module")
endif()

# Configuration options
set(GTBM_SHARED OFF CACHE BOOL "Build shared libraries for Google Test and Google Benchmark")

# Set directory paths
set(GAPI_SOURCE_DIR   "${CMAKE_CURRENT_LIST_DIR}")
set(GAPI_BINARY_DIR   "${CMAKE_SOURCE_DIR}/build/external/gapi")
set(GAPI_LIB_PATH     "${GAPI_BINARY_DIR}")

# Platform-specific library names
if(WIN32)
    set(GTEST_DLL         "gtest.dll")
    set(GTEST_SHARED      "gtest.lib")
    set(GTEST_STATIC      "gtest.lib")
    set(GBENCH_DLL        "benchmark.dll")
    set(GBENCH_SHARED     "benchmark.lib")
    set(GBENCH_STATIC     "benchmark.lib")
else()
    set(GTEST_SHARED      "libgtest.so")
    set(GTEST_STATIC      "libgtest.a")
    set(GBENCH_SHARED     "libbenchmark.so")
    set(GBENCH_STATIC     "libbenchmark.a")
endif()

# Select the appropriate library based on configuration
if(GTBM_SHARED)
    set(GTEST_LIB  "${GTEST_SHARED}")
    set(GBENCH_LIB "${GBENCH_SHARED}")
else()
    set(GTEST_LIB  "${GTEST_STATIC}")
    set(GBENCH_LIB "${GBENCH_STATIC}")
endif()

# Check if google Test and googlebenchmark libraries exist
library_exists("${GTEST_LIB}" "${GAPI_LIB_PATH}" GTEST_LIB_EXISTS)
library_exists("${GBENCH_LIB}" "${GAPI_LIB_PATH}" BENCHMARK_LIB_EXISTS)

# Main logic for handling Google Test and Google Benchmark dependencies
if(GTEST_LIB_EXISTS AND BENCHMARK_LIB_EXISTS)
    message(STATUS "Google Test and Google Benchmark libraries found:")
    message(STATUS "Google Test library     : ${GAPI_LIB_PATH}/${GTEST_LIB}")
    message(STATUS "Google Benchmark library: ${GAPI_LIB_PATH}/${GBENCH_LIB}")

else()
    configure_build("${GAPI_SOURCE_DIR}" "${GAPI_BINARY_DIR}")

    # Verify the libraries were built successfully
    library_exists("${GTEST_LIB}" "${GAPI_LIB_PATH}" GTEST_LIB_BUILT)
    library_exists("${GBENCH_LIB}" "${GAPI_LIB_PATH}" GBENCH_LIB_BUILT)
    if(NOT GTEST_LIB_BUILT)
        message(FATAL_ERROR "Failed to build Google Test library: ${GTEST_LIB}")
    endif()

    if(NOT GBENCH_LIB_BUILT)
        message(FATAL_ERROR "Failed to build Google Benchmark library: ${GBENCH_LIB}")
    endif()

endif()

# Set include directories
set(GTEST_INCLUDE_DIR
    "${GAPI_SOURCE_DIR}/gtest"
)
set(GBENCH_INCLUDE_DIR
    "${GAPI_SOURCE_DIR}/gbench"
    "${GAPI_SOURCE_DIR}/gbench/src"
)

# Validate that all paths exist
foreach(include_dir ${GTEST_INCLUDE_DIR})
    if(NOT EXISTS "${include_dir}")
        message(WARNING "Google Test include directory does not exist: ${include_dir}")
    endif()
endforeach()

foreach(include_dir ${GBENCH_INCLUDE_DIR})
    if(NOT EXISTS "${include_dir}")
        message(WARNING "Google Benchmark include directory does not exist: ${include_dir}")
    endif()
endforeach()

if(NOT EXISTS "${GAPI_LIB_PATH}")
    message(WARNING "GAPI library path does not exist: ${GAPI_LIB_PATH}")
endif()

# Set library paths for both gtest and gbench (they're in the same directory)
set(GTEST_LIB_PATH "${GAPI_LIB_PATH}")
set(GBENCH_LIB_PATH "${GAPI_LIB_PATH}")

