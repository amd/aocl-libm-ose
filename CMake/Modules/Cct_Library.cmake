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

include(CMakeParseArguments)
include(Cct_Helpers)


# Declare a Library
#
#   cct_cxx_add_library ( NAME
#         EXPORT_NAME exp_name
#         SHARED_LIB  ON
#         SOURCES BufferTest.cc
#         HEADERS BufferTest.hh
#         LIBS     buffer cpuid
#         EXTLIBS  openssl blah bligh
#   )
#
#   By default adds the library to build infrastructure
#   and builds a STATIC library.

# Parameters:
#   NAME           name of the test.
#   SOURCES        list of test source files, headers included.
#   HEADERS        list of headers to depend on
#   LIBS           list of dependencies from current project
#   EXTLIBS        list of external dependencies (may be system libraries such as "m" "pthread")
#
# Attributes:
#   EXPORT_NAME  <name>   Use this library for SDK installation
#   SHARED_LIB   <ON/OFF> Also build Shared library
# Libraries added with this function are automatically registered.
# Each library produces a build target named "${PROJECT_NAME}::<libname>" .
#
function(cct_cxx_add_library libName)
    cct_check_module_name()

    set(fPrefix   __lib)
    set(fOptions  "SHARED_LIB;INSTALL_LIB")
    set(fOneArg   "EXPORT_NAME")
    set(fMultiArg SOURCES;HEADERS;LIBS;EXTLIBS;PRIVATE_COMPILE_FLAGS)

    cmake_parse_arguments(
        ${fPrefix}
        "${fOptions}"
        "${fOneArg}"
        "${fMultiArg}"
        ${ARGN}
    )

    set(__name ${libName}_objlib)

    add_library(${__name} OBJECT ${${fPrefix}_SOURCES} ${${fPrefix}_HEADERS})
    target_sources(${__name}
        PRIVATE )
    target_include_directories(
        ${__name}
        PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                ${${PROJECT_PREFIX}_SDK_INCLUDES}
        PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
    )

    message(DEBUG "source files - " ${${fPrefix}_SOURCES})

    target_compile_options(${__name} PRIVATE
                            ${${fPrefix}_PRIVATE_COMPILE_FLAGS}
                            )

    target_link_libraries(${__name} PUBLIC
                ${${fPrefix}_EXTLIBS}
                )

    #get_property(dirs TARGET ${__name} PROPERTY INCLUDE_DIRECTORIES)
    #message(DEBUG "dirs: " ${dirs})

    add_library(${libName}_static STATIC $<TARGET_OBJECTS:${__name}>)
    set_target_properties(${libName}_static PROPERTIES OUTPUT_NAME ${libName})
    set(LIBSLIST ${libName}_static)
    if( ${${fPrefix}_SHARED_LIB} )
        add_library(${libName}_shared SHARED $<TARGET_OBJECTS:${__name}>)
        set_target_properties(${libName}_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)
        set_target_properties(${libName}_shared PROPERTIES OUTPUT_NAME ${libName})
        list(APPEND LIBSLIST ${libName}_shared)
    endif()

    message( "Adding Library: " ${libName})

    if( ${fPrefix}_EXPORT_NAME )
        set(__alias_name ${PROJECT_PREFIX}::${${fPrefix}_EXPORT_NAME} )
        add_library(${__alias_name}
                ALIAS ${libName}_static)
        message( "       with Alias : " ${__alias_name})
    endif()

    if( ${${fPrefix}_INSTALL_LIB} )
        install(TARGETS ${LIBSLIST}  LIBRARY DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}
                ARCHIVE DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}
                RUNTIME DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR} )
    endif()
endfunction(cct_cxx_add_library)


# Declare a Interface
#
#   cct_cxx_add_interface ( NAME
#         HEADERS BufferTest.hh
#         LIBS     buffer cpuid
#         EXTLIBS  openssl blah bligh
#   )
#
# Parameters:
#   NAME           name of the test.
#   HEADERS        list of headers to depend on
#   LIBS           list of dependencies from current project
#   EXTLIBS        list of external dependencies (may be system libraries such as "m" "pthread")
#
# Attributes:
#   PUBLIC            Known to fail
#
# Interface Libraries added with this function are automatically registered.
# Each library produces a build target named "${PROJECT_NAME}::<libname>" .
#
#
function(cct_cxx_add_interface ifName)
    set(fPrefix __iface)
    set(fOptions PUBLIC)
    set(fOneArg )
    set(fMultiArgs HEADERS)

    cmake_parse_arguments(
    ${fPrefix}
    "${fOptions}"
    "${fOneArg}"
    "${fMultiArgs}"
    ${ARGN}
    )

    cct_check_module_name()

    au_normalize_name(${ifName} __new_name)
    add_library(${__new_name} INTERFACE)

    if(__iface_PUBLIC)
        set(__target_name "au_${__new_name}")
    else()
        set(__target_name "au_internal_${__new_name}")
    endif()

    if(__iface_PUBLIC)
        target_include_directories(${__new_name} INTERFACE ${__iface_HEADERS})
    else()
        target_include_directories(${__new_name} PUBLIC INTERFACE ${__iface_HEADERS})
    endif()

    add_library(${PROJECT_PREFIX}::${NAME} ALIAS ${__new_name})
endfunction(cct_cxx_add_interface)
