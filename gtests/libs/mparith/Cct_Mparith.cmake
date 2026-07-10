#
# Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
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

#Function to check if a library exists
function(library_exists lib libpath)
    find_library(${lib}_LIB NAMES ${lib} PATHS ${libpath} NO_DEFAULT_PATH)
    if(${lib}_LIB)
        set(${ARGN} TRUE PARENT_SCOPE)
    else()
        set(${ARGN} FALSE PARENT_SCOPE)
    endif()
endfunction()

set(MPFR_SRC_DIR        "${CMAKE_CURRENT_LIST_DIR}")
if(WIN32)
    # CMAKE_BUILD_TYPE is validated in the root CMakeLists.txt.
    # Windows-only: scope by build type so Debug/Release do not reuse cached libs.
    set(MPARITH_DIR     "${${PROJECT_PREFIX}_SOURCE_DIR}/build/external/mparith/${CMAKE_BUILD_TYPE}")
else()
    set(MPARITH_DIR     "${${PROJECT_PREFIX}_SOURCE_DIR}/build/external/mparith")
endif()

# Installation directories
set(MPARITH_LIB_DIR     ${MPARITH_DIR}/lib)
set(MPARITH_INCLUDE_DIR ${MPARITH_DIR}/include)

if (WIN32)
  set(MPARITH_STATIC       libmparith-static.lib)
  set(MPARITH_LIB          libmparith.lib)
  set(MPARITH_SHARED       libmparith.dll)
else()
  set(MPARITH_STATIC       libmparith.a)
  set(MPARITH_SHARED       libmparith.so)
endif()

if(MPARITH_SHARED_LIBS)
  set(LIBMPARITH           ${MPARITH_SHARED})
else()
  set(LIBMPARITH           ${MPARITH_STATIC})
endif()

# Check if mparith directory exists
directory_exists(${MPARITH_LIB_DIR} MPARITH_LIB_DIR_EXISTS)
library_exists(${LIBMPARITH} ${MPARITH_LIB_DIR} MPARITH_LIB_EXISTS)
if(MPARITH_LIB_DIR_EXISTS AND MPARITH_LIB_EXISTS)
    # Check if mparith library exists
    if(MPARITH_LIB_EXISTS)
        message(STATUS "mparith library found, skipping build...")
    endif()
else()
    set(MPARITH_BINARY_DIR "${CMAKE_BINARY_DIR}")
    message(STATUS "mparith library not-found, building library...")

    if(WIN32)
        # libalm and gtests link msvcrt (release CRT) on Windows; pin /MD for all
        # configs so mparith objects stay CRT-consistent with the parent gtests link.
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|MSVC")
            set(MPARITH_RUNTIME_CONFIG -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL)
        else()
            set(MPARITH_RUNTIME_CONFIG "")
        endif()

        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
                                -S "${MPFR_SRC_DIR}" -B "${MPARITH_BINARY_DIR}"
                                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -DMPARITH_DIR=${MPARITH_DIR}
                                ${MPARITH_RUNTIME_CONFIG}
                        RESULT_VARIABLE  result_mparith
                        OUTPUT_VARIABLE   config_mparith
                        ERROR_VARIABLE    error_mparith
                        WORKING_DIRECTORY "${MPFR_SRC_DIR}"
        )

        if(NOT result_mparith EQUAL 0)
            message(FATAL_ERROR "Failed to configure mparith build:\n${error_mparith}")
        endif()

        execute_process(COMMAND ${CMAKE_COMMAND} --build "${MPARITH_BINARY_DIR}" --config ${CMAKE_BUILD_TYPE}
                        RESULT_VARIABLE  build_result_mparith
                        OUTPUT_VARIABLE   output_mparith
                        ERROR_VARIABLE    error_build_mparith
                        WORKING_DIRECTORY "${MPFR_SRC_DIR}"
        )

        if(NOT build_result_mparith EQUAL 0)
            message(FATAL_ERROR "Failed to build mparith:\n${error_build_mparith}")
        endif()
    else()
        execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${MPFR_SRC_DIR} -B ${MPARITH_BINARY_DIR}
                                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                -DCMAKE_BUILD_TYPE=Release
                                -DMPARITH_DIR=${MPARITH_DIR}
                        RESULTS_VARIABLE  result_mparith
                        OUTPUT_VARIABLE   config_mparith
                        WORKING_DIRECTORY ${MPFR_SRC_DIR}
        )

        execute_process(COMMAND ${CMAKE_COMMAND} --build ${MPARITH_BINARY_DIR} --config Release
                        OUTPUT_VARIABLE   output_mparith
                        WORKING_DIRECTORY ${MPFR_SRC_DIR}
        )
    endif()
endif()
