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

set(AU_SHARED OFF CACHE BOOL "Build shared libraries")
set(AU_STATIC OFF CACHE BOOL "Build static libraries")
if(NOT AU_SHARED)
    set(AU_STATIC ON)
endif()

set(AU_SOURCE_DIR "${CMAKE_SOURCE_DIR}/build/external/aocl-utils")
set(AU_BINARY_DIR "${AU_SOURCE_DIR}/build")

if (WIN32)
  set(AOCLUTILS_DLL         au_cpuid.dll)
  set(AOCLUTILS_SHARED      au_cpuid.lib)
  set(AOCLUTILS_STATIC      au_cpuid_static.lib)
else()
  set(AOCLUTILS_SHARED      libau_cpuid.so)
  set(AOCLUTILS_STATIC      libau_cpuid.a)
endif()

if(AU_SHARED)
  set(AOCL_UTILS_LIB        ${AOCLUTILS_SHARED})
else()
  set(AOCL_UTILS_LIB        ${AOCLUTILS_STATIC})
endif()

# Check if aocl-utils directory exists
directory_exists(${AU_SOURCE_DIR} AOCL_UTILS_DIR_EXISTS)

# Check if aocl-utils library exists
library_exists(${AOCL_UTILS_LIB} ${AU_BINARY_DIR}/Library/Cpuid AOCL_UTILS_LIB_EXISTS)


if(AOCL_UTILS_LIB_EXISTS)
    message(STATUS "Aocl-Utils library found ...")
elseif(AOCL_UTILS_DIR_EXISTS)
    message(STATUS "Aocl-Utils directory found, building library...")

    # Build aocl-utils with specified options
    execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${AU_SOURCE_DIR} -B ${AU_BINARY_DIR}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DCMAKE_BUILD_TYPE=Release
                            -DAU_BUILD_SHARED_LIBS=${AU_SHARED}
                            -DAU_BUILD_STATIC_LIBS=${AU_STATIC}
                            -DAU_CMAKE_VERBOSE=OFF
                            -DAU_BUILD_TESTS=OFF
                            -DAU_BUILD_EXAMPLES=OFF
                    RESULTS_VARIABLE  result_au
                    WORKING_DIRECTORY ${AU_SOURCE_DIR}
    )
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${AU_BINARY_DIR} --config Release -v
                    OUTPUT_VARIABLE   output_au
                    WORKING_DIRECTORY ${AU_SOURCE_DIR}
    )
else()
    message(STATUS "Aocl-Utils directory not found, cloning repository...")

    # Clone aocl-utils with specific tag
    execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_AU_GIT_TAG}" https://github.com/amd/aocl-utils.git ${AU_SOURCE_DIR})

    # Build aocl-utils with specified options
    execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${AU_SOURCE_DIR} -B ${AU_BINARY_DIR}
                             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                             -DCMAKE_BUILD_TYPE=Release
                             -DAU_BUILD_SHARED_LIBS=${AU_SHARED}
                             -DAU_BUILD_STATIC_LIBS=${AU_STATIC}
                             -DAU_CMAKE_VERBOSE=OFF
                             -DAU_BUILD_TESTS=OFF
                             -DAU_BUILD_EXAMPLES=OFF
                    RESULTS_VARIABLE  result_au
                    WORKING_DIRECTORY ${AU_SOURCE_DIR}
    )
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${AU_BINARY_DIR} --config Release
                    OUTPUT_VARIABLE   output_au
                    WORKING_DIRECTORY ${AU_SOURCE_DIR}
    )
endif()

# Set additional AOCL utils paths and library names
set(AOCL_UTILS_INCLUDE_DIR  ${AU_SOURCE_DIR}/SDK/Bcl
                            ${AU_SOURCE_DIR}/SDK/Include
                            ${AU_BINARY_DIR}/generated)
set(AOCL_UTILS_PATH         ${AU_BINARY_DIR}/Library/Cpuid)
