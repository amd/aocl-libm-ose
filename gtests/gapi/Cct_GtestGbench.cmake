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

# Function to build google Test
function(build_gtest source_dir binary_dir)
    message(STATUS "Building Google Test from: ${source_dir}")

    # Configure Google Test build
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S "${source_dir}" -B "${binary_dir}"
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DBUILD_GMOCK=OFF
                            -Dgtest_force_shared_crt=ON
                            -DBUILD_SHARED_LIBS=${GTBM_SHARED}
                            -DCMAKE_BUILD_TYPE=Release
                    RESULT_VARIABLE gtest_configure_result
                    OUTPUT_VARIABLE gtest_configure_output
                    ERROR_VARIABLE gtest_configure_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT gtest_configure_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure Google Test build:\n${gtest_configure_error}")
    endif()

    # Build Google Test
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${binary_dir}" --config Release
                    RESULT_VARIABLE gtest_build_result
                    OUTPUT_VARIABLE gtest_build_output
                    ERROR_VARIABLE  gtest_build_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT gtest_build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build Google Test:\n${gtest_build_error}")
    endif()

    message(STATUS "Successfully built Google Test")
endfunction()

# Function to build Google Benchmark
function(build_gbenchmark source_dir binary_dir)
    message(STATUS "Building Google Benchmark from: ${source_dir}")

    # Configure Google Benchmark build
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S "${source_dir}" -B "${binary_dir}"
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DBENCHMARK_ENABLE_TESTING=OFF
                            -DBENCHMARK_ENABLE_EXCEPTIONS=ON
                            -DBENCHMARK_ENABLE_LTO=OFF
                            -DBENCHMARK_ENABLE_WERROR=OFF
                            -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
                            -DBENCHMARK_INSTALL_DOCS=OFF
                            -DBENCHMARK_USE_BUNDLED_GTEST=OFF
                            -DBUILD_SHARED_LIBS=${GTBM_SHARED}
                            -DCMAKE_BUILD_TYPE=Release
                    RESULT_VARIABLE gbench_configure_result
                    OUTPUT_VARIABLE gbench_configure_output
                    ERROR_VARIABLE gbench_configure_error
                    WORKING_DIRECTORY "${source_dir}"
        )

    if(NOT gbench_configure_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure Google Benchmark build:\n${gbench_configure_error}")
    endif()

    # Build Google Benchmark
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${binary_dir}" --config Release
                    RESULT_VARIABLE gbench_build_result
                    OUTPUT_VARIABLE gbench_build_output
                    ERROR_VARIABLE gbench_build_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT gbench_build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build Google Benchmark:\n${gbench_build_error}")
    endif()

    message(STATUS "Successfully built Google Benchmark")
endfunction()

# Function to configure and build both google Test and gbenchmark
function(configure_build gt_source_dir bm_source_dir gt_binary_dir bm_binary_dir)
    build_gtest("${gt_source_dir}" "${gt_binary_dir}")
    build_gbenchmark("${bm_source_dir}" "${bm_binary_dir}")
endfunction()




# Validate required variables
if(NOT DEFINED PROJECT_PREFIX)
    message(FATAL_ERROR "PROJECT_PREFIX must be defined before including this module")
endif()

# Configuration options
set(GTBM_SHARED OFF CACHE BOOL "Build shared libraries for Google Test and Google Benchmark")

# Set directory paths
set(GT_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/build/external/googletest")
set(BM_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/build/external/benchmark")
set(GT_BINARY_DIR   "${GT_SOURCE_DIR}/build")
set(BM_BINARY_DIR   "${BM_SOURCE_DIR}/build")
set(GTEST_LIB_PATH  "${GT_BINARY_DIR}/lib")
set(GBENCH_LIB_PATH "${BM_BINARY_DIR}/src")

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

# Check if google Test and googlebenchmark directories exist
directory_exists("${GT_SOURCE_DIR}" GTEST_DIR_EXISTS)
directory_exists("${BM_SOURCE_DIR}" BENCHMARK_DIR_EXISTS)

# Check if google Test and googlebenchmark libraries exist
library_exists("${GTEST_LIB}" "${GTEST_LIB_PATH}" GTEST_LIB_EXISTS)
library_exists("${GBENCH_LIB}" "${GBENCH_LIB_PATH}" BENCHMARK_LIB_EXISTS)

# Main logic for handling Google Test and Google Benchmark dependencies
if(GTEST_LIB_EXISTS AND BENCHMARK_LIB_EXISTS)
    message(STATUS "Google Test and Google Benchmark libraries found:")
    message(STATUS "Google Test library     : ${GTEST_LIB_PATH}/${GTEST_LIB}")
    message(STATUS "Google Benchmark library: ${GBENCH_LIB_PATH}/${GBENCH_LIB}")

elseif(GTEST_DIR_EXISTS AND BENCHMARK_DIR_EXISTS)
    message(STATUS "Google Test and Google Benchmark directories found, building libraries...")
    configure_build("${GT_SOURCE_DIR}" "${BM_SOURCE_DIR}" "${GT_BINARY_DIR}" "${BM_BINARY_DIR}")

    # Verify the libraries were built successfully
    library_exists("${GTEST_LIB}" "${GTEST_LIB_PATH}" GTEST_LIB_BUILT)
    library_exists("${GBENCH_LIB}" "${GBENCH_LIB_PATH}" GBENCH_LIB_BUILT)

    if(NOT GTEST_LIB_BUILT)
        message(FATAL_ERROR "Failed to build Google Test library   : ${GTEST_LIB}")
    endif()

    if(NOT GBENCH_LIB_BUILT)
        message(FATAL_ERROR "Failed to build Google Benchmark library: ${GBENCH_LIB}")
    endif()

else()
    message(STATUS "Google Test or Google Benchmark directories not found, cloning repositories...")

    # Validate required git variables
    if(NOT GTEST_DIR_EXISTS)
        if(NOT DEFINED ${PROJECT_PREFIX}_GTEST_GIT_TAG OR NOT DEFINED ${PROJECT_PREFIX}_GTEST_GIT_REPO_URL)
            message(FATAL_ERROR "${PROJECT_PREFIX}_GTEST_GIT_TAG and ${PROJECT_PREFIX}_GTEST_GIT_REPO_URL must be defined")
        endif()

        # Clone Google Test with specific tag
        execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_GTEST_GIT_TAG}"
                                                    "${${PROJECT_PREFIX}_GTEST_GIT_REPO_URL}" "${GT_SOURCE_DIR}"
                        RESULT_VARIABLE gtest_clone_result
                        OUTPUT_VARIABLE gtest_clone_output
                        ERROR_VARIABLE gtest_clone_error
        )

        if(NOT gtest_clone_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone Google Test repository:\n${gtest_clone_error}")
        endif()

        message(STATUS "Successfully cloned Google Test (tag: ${${PROJECT_PREFIX}_GTEST_GIT_TAG})")
    endif()

    # Clone Google Benchmark with specific tag
    if(NOT BENCHMARK_DIR_EXISTS)
        if(NOT DEFINED ${PROJECT_PREFIX}_GBENCH_GIT_TAG OR NOT DEFINED ${PROJECT_PREFIX}_GBENCH_GIT_REPO_URL)
            message(FATAL_ERROR "${PROJECT_PREFIX}_GBENCH_GIT_TAG and ${PROJECT_PREFIX}_GBENCH_GIT_REPO_URL must be defined")
        endif()

        execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_GBENCH_GIT_TAG}"
                                                    "${${PROJECT_PREFIX}_GBENCH_GIT_REPO_URL}" "${BM_SOURCE_DIR}"
                        RESULT_VARIABLE gbench_clone_result
                        OUTPUT_VARIABLE gbench_clone_output
                        ERROR_VARIABLE gbench_clone_error
        )

        if(NOT gbench_clone_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone Google Benchmark repository:\n${gbench_clone_error}")
        endif()

        message(STATUS "Successfully cloned Google Benchmark (tag: ${${PROJECT_PREFIX}_GBENCH_GIT_TAG})")
    endif()

    # Build both libraries after cloning
    configure_build("${GT_SOURCE_DIR}" "${BM_SOURCE_DIR}" "${GT_BINARY_DIR}" "${BM_BINARY_DIR}")

    # Verify the libraries were built successfully
    library_exists("${GTEST_LIB}" "${GTEST_LIB_PATH}" GTEST_LIB_BUILT)
    library_exists("${GBENCH_LIB}" "${GBENCH_LIB_PATH}" GBENCH_LIB_BUILT)

    if(NOT GTEST_LIB_BUILT)
        message(FATAL_ERROR "Failed to build Google Test library after cloning: ${GTEST_LIB}")
    endif()

    if(NOT GBENCH_LIB_BUILT)
        message(FATAL_ERROR "Failed to build Google Benchmark library after cloning: ${GBENCH_LIB}")
    endif()

endif()

# Set include directories
set(GTEST_INCLUDE_DIR
    "${GT_SOURCE_DIR}/googletest/include"
    "${GT_SOURCE_DIR}/googletest/include/gtest"
)
set(GBENCH_INCLUDE_DIR
    "${BM_SOURCE_DIR}/include"
    "${BM_SOURCE_DIR}/include/benchmark"
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

if(NOT EXISTS "${GTEST_LIB_PATH}")
    message(WARNING "Google Test library path does not exist: ${GTEST_LIB_PATH}")
endif()

if(NOT EXISTS "${GBENCH_LIB_PATH}")
    message(WARNING "Google Benchmark library path does not exist: ${GBENCH_LIB_PATH}")
endif()

# Export variables for parent scope
set(GTEST_INCLUDE_DIR "${GTEST_INCLUDE_DIR}" PARENT_SCOPE)
set(GBENCH_INCLUDE_DIR "${GBENCH_INCLUDE_DIR}" PARENT_SCOPE)
set(GTEST_LIB_PATH "${GTEST_LIB_PATH}" PARENT_SCOPE)
set(GBENCH_LIB_PATH "${GBENCH_LIB_PATH}" PARENT_SCOPE)
set(GTEST_LIB "${GTEST_LIB}" PARENT_SCOPE)
set(GBENCH_LIB "${GBENCH_LIB}" PARENT_SCOPE)

