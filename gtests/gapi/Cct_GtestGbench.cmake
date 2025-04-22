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
function(directory_exists dir)
    if(EXISTS ${dir} AND IS_DIRECTORY ${dir})
        set(${ARGN} TRUE PARENT_SCOPE)
    else()
        set(${ARGN} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Function to check if a library exists
function(library_exists lib libpath)
    find_library(${lib}_LIB NAMES ${lib} PATHS ${libpath} NO_DEFAULT_PATH)
    if(${lib}_LIB)
        set(${ARGN} TRUE PARENT_SCOPE)
    else()
        set(${ARGN} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Function to configure and build gtest and gbenchmark
function(configure_build)
    # Build gtest with specified options
    execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${GT_SOURCE_DIR} -B ${GT_BINARY_DIR}
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DBUILD_GMOCK=OFF
                            -Dgtest_force_shared_crt=ON
                            -DBUILD_SHARED_LIBS=${GTBM_SHARED}
                            -DCMAKE_BUILD_TYPE=Release
                    RESULTS_VARIABLE  result_gtest
                    OUTPUT_VARIABLE   config_gtest
                    WORKING_DIRECTORY ${GT_SOURCE_DIR}
    )
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${GT_BINARY_DIR} --config Release
                    OUTPUT_VARIABLE   output_gtest
                    WORKING_DIRECTORY ${GT_SOURCE_DIR}
    )
    # Build benchmark with specified options
    execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${BM_SOURCE_DIR} -B ${BM_BINARY_DIR}
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
                    RESULTS_VARIABLE  result_gbench
                    OUTPUT_VARIABLE   config_gbench
                    WORKING_DIRECTORY ${BM_SOURCE_DIR}
    )
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${BM_BINARY_DIR} --config Release
                    OUTPUT_VARIABLE   output_gbench
                    WORKING_DIRECTORY ${BM_SOURCE_DIR}
   )

endfunction()


set(GT_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/build/external/googletest")
set(BM_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/build/external/benchmark")
set(GT_BINARY_DIR   "${GT_SOURCE_DIR}/build")
set(BM_BINARY_DIR   "${BM_SOURCE_DIR}/build")
set(GTEST_LIB_PATH  "${GT_BINARY_DIR}/lib")
set(GBENCH_LIB_PATH "${BM_BINARY_DIR}/src")

set(GTBM_SHARED OFF CACHE BOOL "Build shared libraries")

# Check if gtest and googlebenchmark directories exist
directory_exists(${GT_SOURCE_DIR} GTEST_DIR_EXISTS)
directory_exists(${BM_SOURCE_DIR} BENCHMARK_DIR_EXISTS)
if (WIN32)
  set(GTEST_DLL         gtest.dll)
  set(GTEST_SHARED      gtest.lib)
  set(GTEST_STATIC      gtest.lib)
  set(GBENCH_DLL        benchmark.dll)
  set(GBENCH_SHARED     benchmark.lib)
  set(GBENCH_STATIC     benchmark.lib)
else()
  set(GTEST_SHARED      libgtest.so)
  set(GTEST_STATIC      libgtest.a)
  set(GBENCH_SHARED     libbenchmark.so)
  set(GBENCH_STATIC     libbenchmark.a)
endif()

if(GTBM_SHARED)
  set(GTEST_LIB        ${GTEST_SHARED})
  set(GBENCH_LIB       ${GBENCH_SHARED})
else()
  set(GTEST_LIB        ${GTEST_STATIC})
  set(GBENCH_LIB       ${GBENCH_STATIC})
endif()

# Check if gtest and googlebenchmark libraries exist
library_exists(${GTEST_LIB} ${GTEST_LIB_PATH} GTEST_LIB_EXISTS)
library_exists(${GBENCH_LIB} ${GBENCH_LIB_PATH} BENCHMARK_LIB_EXISTS)

if(GTEST_LIB_EXISTS AND BENCHMARK_LIB_EXISTS)
    message(STATUS "GTest and GBenchmark libraries are found.")

elseif(GTEST_DIR_EXISTS AND BENCHMARK_DIR_EXISTS)
    message(STATUS "GTest and GBenchmark directories found, building libraries...")

    # function to configure and build gtest and gbenchmark
    configure_build()

else()
    message(STATUS "GTest or GBenchmark directories not found, cloning repositories...")

    # Clone gtest with specific tag
    if(NOT GTEST_DIR_EXISTS)
        execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_GTEST_GIT_TAG}" "${${PROJECT_PREFIX}_GTEST_GIT_REPO_URL}" ${GT_SOURCE_DIR})
    endif()

    # Clone GBenchmark with specific tag
    if(NOT BENCHMARK_DIR_EXISTS)
        execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_GBENCH_GIT_TAG}" "${${PROJECT_PREFIX}_GBENCH_GIT_REPO_URL}" ${BM_SOURCE_DIR})
    endif()

    # function to configure and build gtest and gbenchmark
    configure_build()

endif()

set(GTEST_INCLUDE_DIR     "${GT_SOURCE_DIR}/googletest/include"
                          "${GT_SOURCE_DIR}/googletest/include/gtest")
set(GBENCH_INCLUDE_DIR    "${BM_SOURCE_DIR}/include"
                          "${BM_SOURCE_DIR}/include/benchmark")

