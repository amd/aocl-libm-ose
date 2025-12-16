#
# Copyright (C) 2024-2025, Advanced Micro Devices. All rights reserved.
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
function(library_exists lib libpath libdir result_var)
    if(EXISTS "${libpath}/lib" AND IS_DIRECTORY "${libpath}/lib")
        set(lib_search_path "${libpath}/lib")
    elseif(EXISTS "${libpath}/lib64" AND IS_DIRECTORY "${libpath}/lib64")
        set(lib_search_path "${libpath}/lib64")
    else()
        set(${result_var} FALSE PARENT_SCOPE)
        set(${libdir} "" PARENT_SCOPE)
        return()
    endif()

    find_library(${lib}_FOUND NAMES ${lib} PATHS "${lib_search_path}" NO_DEFAULT_PATH)
    if(${lib}_FOUND)
        set(${result_var} TRUE PARENT_SCOPE)
        set(${libdir} "${lib_search_path}" PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
        set(${libdir} "" PARENT_SCOPE)
    endif()
    unset(${lib}_FOUND CACHE)
endfunction()

# Function to build aocl-utils
function(build_aocl_utils source_dir binary_dir)
    message(STATUS "Building aocl-utils from: ${source_dir}")

    # Configure aocl-utils
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -S "${source_dir}" -B "${binary_dir}"
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DCMAKE_BUILD_TYPE=Release
                            -DAU_BUILD_SHARED_LIBS=${AU_SHARED}
                            -DAU_BUILD_STATIC_LIBS=${AU_STATIC}
                            -DAU_CMAKE_VERBOSE=OFF
                            -DAU_BUILD_TESTS=OFF
                            -DAU_BUILD_EXAMPLES=OFF
                            -DCMAKE_INSTALL_PREFIX=${AU_INSTALL_DIR}
                            -DCMAKE_INSTALL_LIBDIR=lib
                    RESULT_VARIABLE configure_result
                    OUTPUT_VARIABLE configure_output
                    ERROR_VARIABLE configure_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT configure_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure aocl-utils build:\n${configure_error}")
    endif()

    # Build the library
    execute_process(COMMAND ${CMAKE_COMMAND} --build "${binary_dir}" --config Release --target install
                    RESULT_VARIABLE build_result
                    OUTPUT_VARIABLE build_output
                    ERROR_VARIABLE build_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build aocl-utils:\n${build_error}")
    endif()

    message(STATUS "Successfully built aocl-utils")
endfunction()

# Validate required variables
if(NOT DEFINED PROJECT_PREFIX)
    message(FATAL_ERROR "PROJECT_PREFIX must be defined before including this module")
endif()

# Configuration options
set(AU_SHARED OFF CACHE BOOL "Build shared libraries for aocl-utils")
set(AU_STATIC OFF CACHE BOOL "Build static libraries for aocl-utils")

# Ensure at least one library type is built
if(NOT AU_SHARED AND NOT AU_STATIC)
    set(AU_STATIC ON)
    message(STATUS "Neither AU_SHARED nor AU_STATIC was set, defaulting to static libraries")
endif()

# Set paths
set(AU_SOURCE_DIR  "${PROJECT_SOURCE_DIR}/build/external/aocl-utils")
set(AU_BINARY_DIR  "${AU_SOURCE_DIR}/build")
set(AU_INSTALL_DIR "${AU_SOURCE_DIR}/amd-utils")

# Platform-specific library names
if(WIN32)
    set(AOCLUTILS_DLL         "au_cpuid.dll")
    set(AOCLUTILS_SHARED      "au_cpuid.lib")
    set(AOCLUTILS_STATIC      "au_cpuid_static.lib")
else()
    set(AOCLUTILS_SHARED      "libau_cpuid.so")
    set(AOCLUTILS_STATIC      "libau_cpuid.a")
endif()

# Select the appropriate library based on configuration
if(AU_SHARED)
    set(AOCL_UTILS_LIB "${AOCLUTILS_SHARED}")
else()
    set(AOCL_UTILS_LIB "${AOCLUTILS_STATIC}")
endif()

# Check if aocl-utils directory exists
directory_exists("${AU_SOURCE_DIR}" AOCL_UTILS_DIR_EXISTS)

# Check if aocl-utils library exists
library_exists("${AOCL_UTILS_LIB}" "${AU_INSTALL_DIR}" AU_INSTALL_LIBDIR AOCL_UTILS_LIB_EXISTS)

# Main logic for handling aocl-utils dependency
if(AOCL_UTILS_LIB_EXISTS)
    message(STATUS "Aocl-Utils library found at: ${AU_INSTALL_LIBDIR}/${AOCL_UTILS_LIB}")

elseif(AOCL_UTILS_DIR_EXISTS)
    message(STATUS "Aocl-Utils directory found, building library...")
    build_aocl_utils("${AU_SOURCE_DIR}" "${AU_BINARY_DIR}")

    # Verify the library was built successfully
    library_exists("${AOCL_UTILS_LIB}" "${AU_INSTALL_DIR}" AU_INSTALL_LIBDIR AOCL_UTILS_LIB_BUILT)
    if(NOT AOCL_UTILS_LIB_BUILT)
        message(FATAL_ERROR "Failed to build aocl-utils library: ${AOCL_UTILS_LIB}")
    endif()

else()
    message(STATUS "Aocl-Utils directory not found, cloning repository...")

    # Validate git tag variable
    if(NOT DEFINED ${PROJECT_PREFIX}_AU_GIT_TAG)
        message(FATAL_ERROR "${${PROJECT_PREFIX}_AU_GIT_TAG} must be defined")
    endif()

    # Clone aocl-utils with specific tag
    execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_AU_GIT_TAG}" "${${PROJECT_PREFIX}_AU_GIT_REPO_URL}" "${AU_SOURCE_DIR}"
                    RESULT_VARIABLE clone_result
                    OUTPUT_VARIABLE clone_output
                    ERROR_VARIABLE clone_error
    )

    if(NOT clone_result EQUAL 0)
        message(FATAL_ERROR "Failed to clone aocl-utils repository:\n${clone_error}")
    endif()

    message(STATUS "Successfully cloned aocl-utils (tag: ${${PROJECT_PREFIX}_AU_GIT_TAG})")

    # Build the cloned repository
    build_aocl_utils("${AU_SOURCE_DIR}" "${AU_BINARY_DIR}")

    # Verify the library was built successfully
    library_exists("${AOCL_UTILS_LIB}" "${AU_INSTALL_DIR}" AU_INSTALL_LIBDIR AOCL_UTILS_LIB_BUILT)
    if(NOT AOCL_UTILS_LIB_BUILT)
        message(FATAL_ERROR "Failed to build aocl-utils library after cloning: ${AOCL_UTILS_LIB}")
    endif()

endif()

# Set AOCL_UTILS include path and AOCL_UTILS library path
set(AOCL_UTILS_INCLUDE_DIR  ${AU_INSTALL_DIR}/include)
set(AOCL_UTILS_LIB_DIR      ${AU_INSTALL_LIBDIR})
set(AOCL_UTILS_PATH         ${AU_INSTALL_DIR})
