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

# Get timestamp.
string(TIMESTAMP BUILD_DATE "%Y%m%d")
# Update using the timestamp.
set(VERSION_STRING_BUILD_DATE "Build ${BUILD_DATE}")
set(LIBALM_VERSION_STRING "${VERSION_STRING_BUILD_DATE}")

configure_file("${PROJECT_SOURCE_DIR}/CMake/version.build.hh.in"
                "${PROJECT_SOURCE_DIR}/src/version.build.h"
                @ONLY)

# Get Compiler info. Use the macro
if(LINUX)
  CHECK_GLIBC_VERSION(GLIBC_VERSION)
endif()
CHECK_COMPILER_VERSION(COMPILER)
# Get System info.
set(BUILD_SYSTEM "${CMAKE_SYSTEM}-${CMAKE_SYSTEM_PROCESSOR}")
set(BUILD_SYS_INFO "${COMPILER}-${BUILD_SYSTEM}${GLIBC_VERSION}" )
configure_file("${PROJECT_SOURCE_DIR}/CMake/buildsysinfo.hh.in"
                "${PROJECT_SOURCE_DIR}/src/buildsysinfo.h"
                @ONLY)

get_optz_flag( optzflag )
get_fast_flag( ffpflag )                            # ffp-contract flags
get_warning_flags( wflags )                         # warning flags
get_unalign_vec_move_flag( uavmflag )               # disabling alignment checking for all AVX ops
get_pic_flag( picflag )                             # position Independent Code flags
get_au_flag( auflag )                               # aocl-utils defination
get_avx2fma_flag( fmaflag )                         # Fused Multiply-Add flags
get_vec_flag( vecflag )                             # Vectorization flags

set(LIBMCFLAGS ${optzflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${ffpflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${wflags})
set(LIBMCFLAGS ${LIBMCFLAGS} ${uavmflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${picflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${auflag})
if(WIN32)
  get_win_flag( winflag )                           # windows flags
  set(LIBMCFLAGS ${LIBMCFLAGS} ${winflag})
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}  /dll")
endif()

# Add sanitizer flags if enabled
if(${${PROJECT_PREFIX}_LIBM_ENABLE_ASAN} AND (NOT WIN32))
  set(LIBMCFLAGS ${LIBMCFLAGS} ${ASAN_FLAGS})
endif()

# Add coverage flags if enabled
if(${${PROJECT_PREFIX}_LIBM_ENABLE_COVERAGE} AND (NOT WIN32))
  set(LIBMCFLAGS ${LIBMCFLAGS} ${COVERAGE_FLAGS})
endif()

if(DEFINED ALM_STATIC_DISPATCH)
  if("${ALM_STATIC_DISPATCH}" MATCHES "(AVX2|ZEN2|ZEN3|ZEN4|ZEN5|AVX512)")
    # Define the architecture macros
    add_compile_definitions(AVX2=1 ZEN2=2 ZEN3=3 ZEN4=4 ZEN5=5 AVX512=6)

    # Add the ALM_STATIC_DISPATCH definition
    add_compile_definitions(ALM_STATIC_DISPATCH=${ALM_STATIC_DISPATCH})
  endif()
endif()

# include paths under src.
set(INCLUDE_PATHS
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src/optimized"
  "${PROJECT_SOURCE_DIR}/src/kern"
  )
include_directories(${INCLUDE_PATHS})

set(INCLUDES "${INCLUDE_PATHS}" "${AOCL_UTILS_INCLUDE_DIR}")

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
target_compile_options(src PRIVATE ${LIBMCFLAGS} "-O1")

#add src object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:src>)

#add include headers as part of the install
set(LIBM_PUBLIC_HEADERS ${PROJECT_SOURCE_DIR}/include/external/amdlibm.h
                        ${PROJECT_SOURCE_DIR}/include/external/amdlibm_vec.h)

set(AOCL_UTILS_LIB ${AOCL_UTILS_LIB_DIR}/${AOCL_UTILS_LIB})
if (WIN32)
  set(LIBALM libalm)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  set(EXTRA_LIBS ${AOCL_UTILS_LIB} "msvcrt")
  set(CMAKE_STATIC_LIBRARY_SUFFIX  "-static.lib")
else()
  set(LIBALM alm)
  set(EXTRA_LIBS ${AOCL_UTILS_LIB} "-Wl,-Bdynamic -lc -lstdc++")
endif()

set(libm "")

#shared libraries
add_library(libm_shared SHARED ${libmobj})
set_target_properties(libm_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ${LIBALM}
    PRIVATE_HEADER "${LIBM_PUBLIC_HEADERS}"
)
target_link_libraries(libm_shared PUBLIC ${EXTRA_LIBS})
if(NOT MSVC)
  set_target_properties(libm_shared PROPERTIES LINK_FLAGS "-ealm_main")
endif()
list(APPEND libm libm_shared)
# Static library
add_library(libm_static STATIC ${libmobj})
set_target_properties(libm_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ${LIBALM}
    PRIVATE_HEADER "${LIBM_PUBLIC_HEADERS}"
)
target_link_libraries(libm_static PUBLIC ${EXTRA_LIBS})
list(APPEND libm libm_static)

add_subdirectory(fast)
if(LINUX)
  add_subdirectory(compat)
endif()

install(TARGETS ${libm}
        LIBRARY DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}/lib
        ARCHIVE DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}/lib
        RUNTIME DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}/lib
        PRIVATE_HEADER DESTINATION ${${PROJECT_PREFIX}_INSTALL_DIR}/include)
set(libm ${libm} PARENT_SCOPE)
