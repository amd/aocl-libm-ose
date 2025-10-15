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

#------------------------------
# Enable cmake minmum version
#------------------------------

set(CMAKE_MIN_REQ_VERSION 3.26)
cmake_minimum_required(VERSION ${CMAKE_MIN_REQ_VERSION})
message(STATUS "CMAKE VERSION:${CMAKE_VERSION} (MIN REQUIRED: ${CMAKE_MINIMUM_REQUIRED_VERSION})")

#------------------------------
#  Add our modules to build
#------------------------------
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Macros"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Checkers"
)

# Check if a preset was used for configuration
if (NOT DEFINED CMAKE_PRESET)
  message(STATUS "No CMake preset detected - using manual configuration")
endif()

#------------------------------
# Setup Options
#------------------------------
include(Cct_Options)

#------------------------------
# Setup Variables
#------------------------------
include(Cct_Variables)

#------------------------------
#  Add our modules to build
#------------------------------
if (NOT DEFINED PROJECT_NAME)
    message(FATAL_ERROR "Project name must be set in Presets/Project.json")
endif()
message(DEBUG "Project name from cache " "${PROJECT_NAME}")

if (NOT DEFINED PROJECT_PREFIX)
    message(FATAL_ERROR "Project Prefix must be set in Presets/Project.json")
endif()
message(DEBUG "Project Prefix " "${PROJECT_PREFIX}")

#------------------------------
# Get Version details, this should update
#       ${PROJECT_PREFIX_VERSON_MAJOR}
#       ${PROJECT_PREFIX_VERSON_MINOR}
#       ${PROJECT_PREFIX_VERSON_PATCH}
#       ${PROJECT_PREFIX_VERSON_SUFFIX_EXTRA}
#------------------------------
include(Cct_Version)
include(Cct_Libraryversion)

# Extract version from alm_version.h instead of version.txt
extract_alm_version_from_header("${CMAKE_CURRENT_SOURCE_DIR}/src/alm_version.h" ${PROJECT_PREFIX}_VERSION_FROM_FILE)

cct_extract_version_details(
        "${PROJECT_PREFIX}_VERSION"
        "${${PROJECT_PREFIX}_VERSION_FROM_FILE}")
unset("${PROJECT_PREFIX}_VERSION_FROM_FILE")

set(${PROJECT_PREFIX}_DESCRIPTION   "${PARAM_PROJECT_SUMMARY}")
set(${PROJECT_PREFIX}_VERSION       "${${PROJECT_PREFIX}_VERSION_MAJOR}.${${PROJECT_PREFIX}_VERSION_MINOR}.${${PROJECT_PREFIX}_VERSION_PATCH}")

project("${PROJECT_PREFIX}-${PROJECT_NAME}"
   VERSION        "${${PROJECT_PREFIX}_VERSION}"
   DESCRIPTION    "${${PROJECT_PREFIX}_DESCRIPTION}"
   LANGUAGES      C CXX ASM)

message(DEBUG "Project with name \"${PROJECT_PREFIX}\" with version ${${PROJECT_PREFIX}_VERSION} registered")

#------------------------------
# Configure archiver to use 'rc' flags instead of 'qc'
#------------------------------
# Force CMake to use 'ar rc' instead of 'ar qc' for static library creation
# This ensures proper member replacement in static libraries
if(NOT WIN32)
    set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> rc <TARGET> <LINK_FLAGS> <OBJECTS>")
    # Ensure ranlib is called after archive creation
    set(CMAKE_C_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")
    set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")
endif()

#Include GNUInstallDirs for standard install directories
include(GNUInstallDirs)

if (NOT DEFINED ${${PROJECT_PREFIX}_GENERATED})
    set(${PROJECT_PREFIX}_GENERATED "${PROJECT_BINARY_DIR}/generated")
endif()

if(WIN32)
    set(CMAKE_COMPILE_WARNING_AS_ERROR FALSE)
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
        message(FATAL_ERROR  "CMAKE for ${PROJECT_NAME} not supported on ${CMAKE_C_COMPILER_ID}")
    endif()
endif()

#------------------------------
# Setup Project Includes
#------------------------------
set(${PROJECT_PREFIX}_COMMON_INCLUDES "${${PROJECT_PREFIX}_GENERATED}")
set(${PROJECT_PREFIX}_SDK_INCLUDES "${PROJECT_SOURCE_DIR}/include")
set(${PROJECT_PREFIX}_PRIVATE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

message(DEBUG "common includes ${${PROJECT_PREFIX}_COMMON_INCLUDES}")
message(DEBUG "sdk include " "${${PROJECT_PREFIX}_SDK_INCLUDES}")
message(DEBUG "private include" "${${PROJECT_PREFIX}_PRIVATE_INCLUDES}")

if(DEFINED ALM_STATIC_DISPATCH)
  if (
         ("${ALM_STATIC_DISPATCH}" STREQUAL "AVX2")
      OR ("${ALM_STATIC_DISPATCH}" STREQUAL "ZEN2")
      OR ("${ALM_STATIC_DISPATCH}" STREQUAL "ZEN3")
      OR ("${ALM_STATIC_DISPATCH}" STREQUAL "ZEN4")
      OR ("${ALM_STATIC_DISPATCH}" STREQUAL "ZEN5")
      OR ("${ALM_STATIC_DISPATCH}" STREQUAL "AVX512") )
    # Set the ALM_STATIC_DISPATCH macro based on user input
    # default set to AVX2 if ALM_STATIC_DISPATCH is defined during cmake configured.
    set(ALM_STATIC_DISPATCH "AVX2" CACHE STRING "Set the static dispatch architecture")
    message(STATUS "ALM_STATIC_DISPATCH set to ${ALM_STATIC_DISPATCH}")
  else()
    message(FATAL_ERROR "Valid option for ALM_STATIC_DISPATCH is set to any of the AVX2/ZEN2/ZEN3/ZEN4/ZEN5/AVX512")
  endif()
endif()

if(LINUX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmacro-prefix-map=${PROJECT_SOURCE_DIR}/=.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmacro-prefix-map=${PROJECT_SOURCE_DIR}/=.")
endif()

#------------------------------
# AOCL_Utils CPUID_library requirements
#------------------------------
# Check if the necessary includes and libraries are present before including Cct_Libaoclutils
if (NOT (EXISTS "${AOCL_UTILS_INCLUDE_DIR}" AND EXISTS "${AOCL_UTILS_LIB}"))
    include(Cct_Libaoclutils)
    link_directories(${AOCL_UTILS_LIB_DIR})
    set(CMAKE_SKIP_RPATH TRUE)
    set(CMAKE_SKIP_INSTALL_RPATH TRUE)
endif()

#------------------------------
# Setup Project Includes
#------------------------------
set(${PROJECT_PREFIX}_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
message(DEBUG "Libm installation path" "${${PROJECT_PREFIX}_INSTALL_DIR}")

#------------------------------
# Set C and C++ Standards required
#------------------------------
set(${PROJECT_PREFIX}_CXX_STANDARD  "${CUST_PROJ_CXX_STD}")
if (
       ("${${PROJECT_PREFIX}_CXX_STANDARD}" STREQUAL "11")
    OR ("${${PROJECT_PREFIX}_CXX_STANDARD}" STREQUAL "14")
    OR ("${${PROJECT_PREFIX}_CXX_STANDARD}" STREQUAL "17")
    OR ("${${PROJECT_PREFIX}_CXX_STANDARD}" STREQUAL "20")
    OR ("${${PROJECT_PREFIX}_CXX_STANDARD}" STREQUAL "23")
)
    set(CMAKE_CXX_STANDARD ${${PROJECT_PREFIX}_CXX_STANDARD})
    message(DEBUG "Project \"${PROJECT_NAME}\" is set to C++-${${PROJECT_PREFIX}_CXX_STANDARD}")
else()
    message(FATAL_ERROR "CUST_PROJ_CXX_STD should be set to one of 11/14/17/20/23 (set to ${CUST_PROJ_CXX_STD})")
endif()

#------------------------------
# Generate Config.hh from Config.hh.in
#------------------------------
if (NOT DEFINED ${PROJECT_PREFIX}_CONFIG_OUTPUT_FILE)
    set(${PROJECT_PREFIX}_CONFIG_OUTPUT_FILE
            ${${PROJECT_PREFIX}_GENERATED}/Config.hh)
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CONFIG_BUILD_TYPE_DEBUG    1)
else()
    set(CONFIG_BUILD_TYPE_RELEASE  1)
endif()

if(WIN32)
    set(CONFIG_OS_IS_WINDOWS  1)
else()
    set(CONFIG_OS_IS_LINUX    1)
endif()

#------------------------------------
# Check compiler support for different ISAs
#------------------------------------
include(Compiler_Features)
configure_file("${PROJECT_SOURCE_DIR}/CMake/Config.hh.in"
            "${${PROJECT_PREFIX}_CONFIG_OUTPUT_FILE}"
            @ONLY)

#------------------------------
# Generate Version.hh from Version.hh.in
#------------------------------
if (NOT DEFINED ${PROJECT_PREFIX}_VERSION_OUTPUT_FILE)
    set(${PROJECT_PREFIX}_VERSION_OUTPUT_FILE ${${PROJECT_PREFIX}_GENERATED}/Version.hh)
endif()
configure_file("${PROJECT_SOURCE_DIR}/CMake/Version.hh.in"
                "${${PROJECT_PREFIX}_VERSION_OUTPUT_FILE}"
                @ONLY)

#------------------------------
# Display banner of settings
#------------------------------
include(Cct_Banner)
cct_display_banner()

#--------------------------------------------
#         Address Sanitizer flags
#--------------------------------------------
if(NOT WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND (${${PROJECT_PREFIX}_LIBM_ENABLE_ASAN}))
    # GCC AddressSanitizer support
    set(CMAKE_BUILD_TYPE Debug)
    set(ASAN_FLAGS "-g" "-fsanitize=address" "-fno-omit-frame-pointer")
    set(ASAN_LINK_FLAGS "-fsanitize=address")
endif()

#--------------------------------------------
#         Code-coverage flags
#--------------------------------------------
if(NOT WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU" AND (${${PROJECT_PREFIX}_LIBM_ENABLE_COVERAGE}))
    # GCC Code coverage support
    set(CMAKE_BUILD_TYPE Debug)
    set(COVERAGE_FLAGS "-g" "-fprofile-arcs" "-ftest-coverage")
    set(COVERAGE_LINK_FLAG "--coverage")
endif()

#------------------------------
# Add Source Directory
#------------------------------
if(${${PROJECT_PREFIX}_LIBM_BUILD_LIBRARY})
    add_subdirectory(src)
    set(ALM_PATH        "${CMAKE_BINARY_DIR}/src")
    set(ALM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(ALM_PATH        "${${PROJECT_PREFIX}_INSTALL_DIR}/lib")
    endif()
endif()

#------------------------------
# Add Tests if ENABLE_TESTS is defined
# These are Integration Tests,
# Unit tests are expected to be in respective source folder
#------------------------------
if (${${PROJECT_PREFIX}_LIBM_BUILD_TESTS})
    enable_testing()
    add_subdirectory(gtests EXCLUDE_FROM_ALL)
endif()

#----------------------------------------------
# Add Examples if LIBM_ENABLE_TESTS is defined
#---------------------------------------------
if (${${PROJECT_PREFIX}_LIBM_BUILD_EXAMPLES})
    enable_testing()
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()

#----------------------------------------------------------
# Add LibM documentation build if LIBM_BUILD_DOC is enabled
#----------------------------------------------------------
if (${${PROJECT_PREFIX}_LIBM_BUILD_DOCS})
  add_subdirectory(docs/src EXCLUDE_FROM_ALL)
endif ()

add_custom_target(distclean
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND ${CMAKE_COMMAND} -E remove "${PROJECT_SOURCE_DIR}/src/buildsysinfo.h"
    COMMAND ${CMAKE_COMMAND} -E remove "${PROJECT_SOURCE_DIR}/src/version.build.h"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Remove cmake_generated files and directories"
)
