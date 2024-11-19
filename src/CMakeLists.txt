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


include(Cct_Library)
cct_cxx_module("alm")

# Get timestamp.
string(TIMESTAMP BUILD_DATE "%Y%m%d")
# Update using the timestamp.
set(VERSION_STRING "Libm Build ${BUILD_DATE}")
set(LIBALM_VERSION_STRING "${VERSION_STRING}")
configure_file("${PROJECT_SOURCE_DIR}/CMake/version.build.hh.in"
                "${PROJECT_SOURCE_DIR}/src/version.build.h"
                @ONLY)

# Get Compiler info.
set(COMPILER "${CMAKE_C_COMPILER_ID}-${CMAKE_C_COMPILER_VERSION}")
# Get System info.
set(BUILD_SYSTEM "${CMAKE_SYSTEM}-${CMAKE_SYSTEM_PROCESSOR}")
set(BUILD_SYS_INFO "${COMPILER}-${BUILD_SYSTEM}" )
configure_file("${PROJECT_SOURCE_DIR}/CMake/buildsysinfo.hh.in"
                "${PROJECT_SOURCE_DIR}/src/buildsysinfo.h"
                @ONLY)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  #Set gcc-compiler flags for aocl-libm
  set(LIBMCFLAGS -Wall -Wextra -Wcast-qual -Wconversion -Wdisabled-optimization -Wdouble-promotion )
  set(LIBMCFLAGS ${LIBMCFLAGS} -Wformat=2 -Winit-self -Wno-init-self -Winvalid-pch -Wmissing-declarations -Wodr )
  set(LIBMCFLAGS ${LIBMCFLAGS} -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wtrampolines -Wundef )
  set(LIBMCFLAGS ${LIBMCFLAGS} -Wvector-operation-performance -Werror -Wlto-type-mismatch )
  set(LIBMCFLAGS ${LIBMCFLAGS} -Wall -W -Wstrict-prototypes -Werror -Wno-unused-parameter -fPIC )
  set(LIBMCFLAGS ${LIBMCFLAGS} -DUSE_AOCL_UTILS )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string)
  string(REGEX MATCH "^[^\n]*" CLANG_VERSION_STRING "${clang_full_version_string}")
  string(REGEX MATCHALL "(AOCC_2|AOCC_3|AOCC_4|AOCC|LLVM|clang)" CLANG_STRING "${CLANG_VERSION_STRING}")
  string(REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION "${CLANG_VERSION_STRING}")
  #Set clang-compiler flags for aocl-libm
  set(LIBMCFLAGS -ffp-contract=fast)
  set(LIBMCFLAGS ${LIBMCFLAGS} -Wall -W -Wstrict-prototypes -Werror -Wno-unused-parameter -fPIC )
  set(LIBMCFLAGS ${LIBMCFLAGS} -DUSE_AOCL_UTILS)
  if("${CLANG_STRING}" MATCHES "AOCC_4")
    if(CMAKE_C_COMPILER_VERSION GREATER_EQUAL 16.0.3)
      set(LIBMCFLAGS ${LIBMCFLAGS} -muse-unaligned-vector-move)
    endif()
  endif()
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=ld")
else()
    message(ERROR,"Unsupported Compiler")
endif()

if("${ALM_STATIC_DISPATCH}" MATCHES "(AVX2|ZEN2|ZEN3|ZEN4|ZEN5|AVX512)")
    # Define the architecture macros
    add_compile_definitions(AVX2=1 ZEN2=2 ZEN3=3 ZEN4=4 ZEN5=5 AVX512=6)

    # Add the ALM_STATIC_DISPATCH definition
    add_compile_definitions(ALM_STATIC_DISPATCH=${ALM_STATIC_DISPATCH})
else()
    message("Valid option for ALM_STATIC_DISPATCH is set to any of AVX2/ZEN2/ZEN3/ZEN4/ZEN5/AVX512")
    message("Defaulting to Dynamic Dispatch ")
endif()

# include paths under src.
set(INCLUDE_PATHS
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src/optimized"
  "${PROJECT_SOURCE_DIR}/src/kern"
  )
include_directories(${INCLUDE_PATHS})

add_subdirectory(fast)
add_subdirectory(compat)

set(INCLUDES "${INCLUDE_PATHS}" "${AOCLUTILS_INCLUDE}")

#libmobj variable is to collect object libraries in subdirectories
set(libmobj "")

add_subdirectory(isa)
add_subdirectory(optimized)
add_subdirectory(iface)
add_subdirectory(arch)
add_subdirectory(ref)

file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
add_library(src OBJECT ${SRCS})
target_include_directories(src
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${INCLUDES})
target_compile_options(src PRIVATE ${LIBMCFLAGS})
#add src object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:src>)

set(LIBALM alm)
set(LIBM_STATIC ${LIBALM})
set(LIBM_SHARED ${LIBALM})

#add include headers as part of the install
set(LIBM_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/external/amdlibm.h
                        ${PROJECT_SOURCE_DIR}/include/external/amdlibm_vec.h)

add_library(libm_static STATIC ${libmobj})
target_link_libraries(libm_static PRIVATE ${AOCLUTILS_SHARED})
set_target_properties(libm_static PROPERTIES OUTPUT_NAME ${LIBM_STATIC})
set_target_properties(libm_static PROPERTIES PRIVATE_HEADER "${LIBM_PUBLIC_HEADERS}")

add_library(libm_shared SHARED ${libmobj})
target_link_libraries(libm_shared PRIVATE ${AOCLUTILS_SHARED})
set_target_properties(libm_shared PROPERTIES POSITION_INDEPENDENT_CODE 1)
set_target_properties(libm_shared PROPERTIES VERSION 5.0 SOVERSION 0)
set_target_properties(libm_shared PROPERTIES OUTPUT_NAME ${LIBM_SHARED})

set(libs libm_static)
list(APPEND libs libm_shared)
install(TARGETS ${libs}
        LIBRARY DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}
        ARCHIVE DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}
        RUNTIME DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}
        PRIVATE_HEADER DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR})
