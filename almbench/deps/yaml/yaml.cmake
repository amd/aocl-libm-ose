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
#     without specific prior written permission.
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

# === Utility Functions ===

# Function to check if a directory exists
function(directory_exists dir result)
    if(EXISTS ${dir} AND IS_DIRECTORY ${dir})
        set(${result} TRUE PARENT_SCOPE)
    else()
        set(${result} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Function to check if a library exists in a given path
function(library_exists lib_name lib_path result)
    find_library(${lib_name}_FOUND NAMES ${lib_name} PATHS ${lib_path} NO_DEFAULT_PATH)
    if(${lib_name}_FOUND)
        set(${result} TRUE PARENT_SCOPE)
    else()
        set(${result} FALSE PARENT_SCOPE)
    endif()
endfunction()

# === Configuration ===

# Git repository and tag for yaml-cpp
set(YAML_REPO         "https://github.com/jbeder/yaml-cpp.git")
set(YAML_TAG          "yaml-cpp-0.7.0")

# Paths for source and build directories
set(YAML_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/build/external/yaml/yaml-cpp")
set(YAML_BINARY_DIR   "${CMAKE_SOURCE_DIR}/build/external/yaml/yaml-cpp/build")
set(YAML_LIB_PATH     "${YAML_BINARY_DIR}")


# Option to build shared libraries (OFF by default)
set(YAML_SHARED_LIBS  OFF CACHE BOOL "Build shared libraries")

# Logical name of the yaml-cpp library
if (WIN32)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(YAML_LIB_NAME      yaml-cppd.lib)
  else()
    set(YAML_LIB_NAME      yaml-cpp.lib)
  endif()
else()
  # On Linux, check for both static and shared libraries
  if(YAML_SHARED_LIBS)
    set(YAML_LIB_NAME      libyaml-cpp.so)
  else()
    set(YAML_LIB_NAME      libyaml-cpp.a)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
	  set(YAML_LIB_NAME      libyaml-cppd.a)
    else()
	  set(YAML_LIB_NAME      libyaml-cpp.a)
    endif()
  endif()
endif()

# === Check for Existing Build ===

# Check if the source directory exists
directory_exists(${YAML_SOURCE_DIR} YAML_DIR_EXISTS)

# Check if the compiled library already exists in the build path
library_exists(${YAML_LIB_NAME} ${YAML_LIB_PATH} YAML_LIB_EXISTS)

# === Clone and Build if Needed ===

if(NOT YAML_LIB_EXISTS)
    message(STATUS "yaml-cpp not found. Preparing to build...")

    # Clone the repository if the source directory doesn't exist
    if(NOT YAML_DIR_EXISTS)
        message(STATUS "Cloning yaml-cpp from ${YAML_REPO}...")
        execute_process(
            COMMAND git clone --branch ${YAML_TAG} ${YAML_REPO} ${YAML_SOURCE_DIR}
            RESULT_VARIABLE git_result
            OUTPUT_VARIABLE git_output
            ERROR_VARIABLE  git_error
        )

        if(NOT git_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone yaml-cpp repository: ${git_error}")
        endif()
    endif()

    message(STATUS "Configuring and building yaml-cpp...")

    # Configure and build yaml-cpp in a single execute_process call
    # Prepare runtime library configuration
    if(WIN32)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            # For Clang on Windows, use CMake's portable runtime library setting
            set(RUNTIME_CONFIG
                -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
                -DYAML_MSVC_SHARED_RT=ON)
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            # For MSVC, use both the property and explicit flags
            set(RUNTIME_CONFIG
                -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
                -DCMAKE_CXX_FLAGS_RELEASE="/MD"
                -DCMAKE_CXX_FLAGS_DEBUG="/MDd"
                -DYAML_MSVC_SHARED_RT=ON)
        endif()
    else()
        # Linux/Unix - no special runtime library configuration needed
        set(RUNTIME_CONFIG "")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} -S ${YAML_SOURCE_DIR} -B ${YAML_BINARY_DIR}
                           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                           -DYAML_CPP_BUILD_TOOLS=OFF
                           -DYAML_CPP_BUILD_TESTS=OFF
                           -DBUILD_TESTING=OFF
                           -DYAML_CPP_INSTALL=OFF
                           -DBUILD_SHARED_LIBS=${YAML_SHARED_LIBS}
                           -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                           -DCMAKE_PREFIX_PATH=${YAML_LIB_PATH}
                           ${RUNTIME_CONFIG}
                    RESULT_VARIABLE  config_result
                    OUTPUT_VARIABLE  config_output
                    ERROR_VARIABLE   config_error
                    WORKING_DIRECTORY ${YAML_SOURCE_DIR}
    )

    if(NOT config_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure yaml-cpp: ${config_error}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build ${YAML_BINARY_DIR} --config ${CMAKE_BUILD_TYPE}
                    RESULT_VARIABLE  build_result
                    OUTPUT_VARIABLE  build_output
                    ERROR_VARIABLE   build_error
                    WORKING_DIRECTORY ${YAML_SOURCE_DIR}
    )

    if(NOT build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build yaml-cpp: ${build_error}")
    endif()
else()
    message(STATUS "yaml-cpp library found at ${YAML_LIB_PATH}.")
endif()

# === Include Directory ===

# Set the include directory for yaml-cpp headers
set(YAML_INCLUDE_DIR "${YAML_SOURCE_DIR}/include")

# === Library Configuration ===

# Set the full path to the yaml-cpp library
if(WIN32)
    set(YAML_LIB "${YAML_BINARY_DIR}/${YAML_LIB_NAME}")
else()
    # For Linux, use the full path to the library
    set(YAML_LIB "${YAML_BINARY_DIR}/${YAML_LIB_NAME}")
endif()

# Check if the library file exists
if(EXISTS "${YAML_LIB}")
    message(STATUS "yaml-cpp library found at: ${YAML_LIB}")
else()
    message(FATAL_ERROR "yaml-cpp library not found at: ${YAML_LIB}")
endif()