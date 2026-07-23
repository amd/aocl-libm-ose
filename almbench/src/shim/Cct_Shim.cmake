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

include(Cct_ExternalProject)

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

set(SHIM_SRC_DIR        "${PROJECT_SOURCE_DIR}/src/shim")
if(WIN32)
    set(SHIM_OUTPUT_DIR "${${PROJECT_PREFIX}_SOURCE_DIR}/build/external/shim/${CMAKE_BUILD_TYPE}")
else()
    set(SHIM_OUTPUT_DIR "${${PROJECT_PREFIX}_SOURCE_DIR}/build/external/shim")
endif()

# Platform-specific library names
if(WIN32)
    set(SHIMAMD_LIB     libshimamd.dll)
    set(SHIMMKL_LIB     libshimmkl.dll)
else()
    set(SHIMAMD_LIB     libshimamd.so)
    set(SHIMGLIBC_LIB   libshimglibc.so)
    set(SHIMMKL_LIB     libshimmkl.so)
endif()

# Check if shim libraries already exist
directory_exists(${SHIM_OUTPUT_DIR} SHIM_DIR_EXISTS)
library_exists(${SHIMAMD_LIB} ${SHIM_OUTPUT_DIR} SHIMAMD_EXISTS)
library_exists(${SHIMMKL_LIB} ${SHIM_OUTPUT_DIR} SHIMMKL_EXISTS)

if(NOT WIN32)
    library_exists(${SHIMGLIBC_LIB} ${SHIM_OUTPUT_DIR} SHIMGLIBC_EXISTS)
    set(ALL_SHIMS_EXIST ${SHIMAMD_EXISTS} AND ${SHIMGLIBC_EXISTS} AND ${SHIMMKL_EXISTS})
else()
    set(ALL_SHIMS_EXIST ${SHIMAMD_EXISTS} AND ${SHIMMKL_EXISTS})
endif()

if(SHIM_DIR_EXISTS AND ALL_SHIMS_EXIST)
    message(STATUS "Shim libraries found, skipping build...")
else()
    set(SHIM_BINARY_DIR "${CMAKE_BINARY_DIR}/shim")
    set(SHIM_INCS "${PROJECT_SOURCE_DIR}/src/include")
    message(STATUS "Shim libraries not found or incomplete, building libraries...")

    # Build output directory arguments: flatten shim artifacts into
    # SHIM_OUTPUT_DIR on every generator (single- and multi-config).
    alm_external_output_dir_args(_shim_output_args "${SHIM_OUTPUT_DIR}")

    # Use centralized helpers for cmake configure and build arguments
    alm_external_cmake_args(_shim_cmake_args
        SOURCE_DIR "${SHIM_SRC_DIR}"
        BINARY_DIR "${SHIM_BINARY_DIR}"
        EXTRA_ARGS
            ${_shim_output_args}
            "-DSHIM_INCS=${SHIM_INCS}"
    )

    # Execute CMake configuration for shim libraries during configuration time
    execute_process(COMMAND ${CMAKE_COMMAND} ${_shim_cmake_args}
                    RESULT_VARIABLE  result_shim
                    OUTPUT_VARIABLE  config_shim
                    WORKING_DIRECTORY ${SHIM_SRC_DIR}
    )

    # Execute build command for shim libraries
    alm_external_build_args(_shim_build_args "${SHIM_BINARY_DIR}")

    execute_process(COMMAND ${CMAKE_COMMAND} ${_shim_build_args}
                    RESULT_VARIABLE  build_result_shim
                    OUTPUT_VARIABLE  output_shim
                    WORKING_DIRECTORY ${SHIM_SRC_DIR}
    )

    if(NOT result_shim EQUAL 0 OR NOT build_result_shim EQUAL 0)
        message(WARNING "Shim libraries build may have encountered issues")
    else()
        message(STATUS "Shim libraries built successfully")
    endif()
endif()

# Export shim library paths for use by parent project
set(SHIMAMD_PATH "${SHIM_OUTPUT_DIR}/${SHIMAMD_LIB}" CACHE PATH "Path to AMD shim library" FORCE)
set(SHIMMKL_PATH "${SHIM_OUTPUT_DIR}/${SHIMMKL_LIB}" CACHE PATH "Path to MKL shim library" FORCE)

if(NOT WIN32)
    set(SHIMGLIBC_PATH "${SHIM_OUTPUT_DIR}/${SHIMGLIBC_LIB}" CACHE PATH "Path to GLIBC shim library" FORCE)
endif()