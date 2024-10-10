#
# Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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
cmake_minimum_required(VERSION 3.22)
message(STATUS "CMAKE VERSION:${CMAKE_VERSION} (MIN REQUIRED: ${CMAKE_MINIMUM_REQUIRED_VERSION})")

#------------------------------
#  Add our modules to build
#------------------------------
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Macros"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules"
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Checkers"
)

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
message(DEBUG "Project Prefix " "${PARAM_PROJECT_PREFIX}")

#------------------------------
# Get Version details, this should update
#       ${PROJECT_PREFIX_VERSON_MAJOR}
#       ${PROJECT_PREFIX_VERSON_MINOR}
#       ${PROJECT_PREFIX_VERSON_PATCH}
#       ${PROJECT_PREFIX_VERSON_SUFFIX_EXTRA}
#------------------------------
include(Cct_Version)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/src/version.txt"  ${PROJECT_PREFIX}_VERSION_FROM_FILE)
cct_extract_version_details(
        "${PROJECT_PREFIX}_VERSION"
        "${${PROJECT_PREFIX}_VERSION_FROM_FILE}")
unset("${PROJECT_PREFIX}_VERSION_FROM_FILE")

set(${PROJECT_PREFIX}_DESCRIPTION   "${PARAM_PROJECT_SUMMARY}")
set(${PROJECT_PREFIX}_VERSION       "${${PROJECT_PREFIX}_VERSION_MAJOR}.${${PROJECT_PREFIX}_VERSION_MINOR}.${${PROJECT_PREFIX}_VERSION_PATCH}")

project("${PROJECT_PREFIX}-${PROJECT_NAME}"
   VERSION        "${${PROJECT_PREFIX}_VERSION}"
   DESCRIPTION    "${${PROJECT_PREFIX}_DESCRIPTION}"
   HOMEPAGE_URL   "https://aocl.amd.com"
   LANGUAGES      C CXX ASM)

message(DEBUG "Project with name \"${PROJECT_PREFIX}\" with version ${${PROJECT_PREFIX}_VERSION} registered")

if (NOT DEFINED ${${PROJECT_PREFIX}_GENERATED})
    set(${PROJECT_PREFIX}_GENERATED "${PROJECT_BINARY_DIR}/generated")
endif()

if(WIN32)
  message(FATAL_ERROR  "CMAKE for ${PROJECT_NAME} not supported on Windows Platform")
endif()

#------------------------------
# Setup Options
#------------------------------
include(Cct_Options)

#------------------------------
# Setup Project Includes
#------------------------------
set(${PROJECT_PREFIX}_COMMON_INCLUDES "${${PROJECT_PREFIX}_GENERATED}")
set(${PROJECT_PREFIX}_SDK_INCLUDES "${PROJECT_SOURCE_DIR}/include")
set(${PROJECT_PREFIX}_PRIVATE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

message(DEBUG "common includes ${${PROJECT_PREFIX}_COMMON_INCLUDES}")
message(DEBUG "sdk include " "${${PROJECT_PREFIX}_SDK_INCLUDES}")
message(DEBUG "private include" "${${PROJECT_PREFIX}_PRIVATE_INCLUDES}")

#------------------------------
# AOCL_Utils CPUID_library requirements
#------------------------------
include(Cct_Libaoclutils)

#------------------------------
# Setup Project Includes
#------------------------------
#set(${PROJECT_PREFIX}_INSTALL_DIR "${PROJECT_SOURCE_DIR}/build/package")
set(${PROJECT_PREFIX}_INSTALL_DIR "${CMAKE_PROJECT_PREFIX}")
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
if(${CMAKE_BUILD_TYPE} EQUAL "Debug")
    set(CONFIG_BUILD_TYPE_DEBUG "1")
else()
    set(CONFIG_BUILD_TYPE_DEBUG "0")
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

message(" - Project name:          ${PROJECT_NAME}")
#------------------------------
# Display banner of settings
#------------------------------
include(Cct_Banner)
cct_display_banner()

#------------------------------
# Add Source Directory
#------------------------------
add_subdirectory(src)

add_custom_target(distclean
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm ${PROJECT_SOURCE_DIR}/src/buildsysinfo.h
    COMMAND rm ${PROJECT_SOURCE_DIR}/src/version.build.h
    COMMAND rm -rf ${PROJECT_BINARY_DIR}/build
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Remove cmake_generated files and directories"
)

