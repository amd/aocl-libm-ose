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

#Function to check if a library exists
function(library_exists lib libpath)
    find_library(${lib}_LIB NAMES ${lib} PATHS ${libpath} NO_DEFAULT_PATH)
    if(${lib}_LIB)
        set(${ARGN} TRUE PARENT_SCOPE)
    else()
        set(${ARGN} FALSE PARENT_SCOPE)
    endif()
endfunction()

set(MPFR_SRC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/libs/mparith")
set(MPARITH_DIR         "${CMAKE_SOURCE_DIR}/build/external/mparith")
set(MPARITH_SHARED_LIBS OFF CACHE BOOL "Build mparith shared library")

# Installation directories
set(MPARITH_LIB_DIR     ${MPARITH_DIR}/lib)
set(MPARITH_INCLUDE_DIR ${MPARITH_DIR}/include)

if (WIN32)
  set(MPARITH_STATIC       mparith.lib)
  set(MPARITH_SHARED       mparith.dll)
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

    # Execute a custom command during configuration time
    execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${MPFR_SRC_DIR} -B ${MPARITH_BINARY_DIR}
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DCMAKE_BUILD_TYPE=Release
                            -DBUILD_SHARED_LIBS=${MPARITH_SHARED_LIBS}
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
