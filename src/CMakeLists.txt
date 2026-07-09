#
# Copyright (C) 2024-2026, Advanced Micro Devices. All rights reserved.
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

# Get timestamp.
string(TIMESTAMP BUILD_DATE "%Y%m%d")
# Update using the timestamp.
set(VERSION_STRING_BUILD_DATE "Build ${BUILD_DATE}")
set(LIBALM_VERSION_STRING "${VERSION_STRING_BUILD_DATE}")

configure_file("${${PROJECT_PREFIX}_SOURCE_DIR}/CMake/version.build.hh.in"
                "${${PROJECT_PREFIX}_SOURCE_DIR}/src/version.build.h"
                @ONLY)

# Get Compiler info. Use the macro
if(LINUX)
  CHECK_GLIBC_VERSION(GLIBC_VERSION)
endif()
CHECK_COMPILER_VERSION(COMPILER)
# Get System info.
set(BUILD_SYSTEM "${CMAKE_SYSTEM}-${CMAKE_SYSTEM_PROCESSOR}")
set(BUILD_SYS_INFO "${COMPILER}-${BUILD_SYSTEM}${GLIBC_VERSION}" )
configure_file("${${PROJECT_PREFIX}_SOURCE_DIR}/CMake/buildsysinfo.hh.in"
                "${${PROJECT_PREFIX}_SOURCE_DIR}/src/buildsysinfo.h"
                @ONLY)

get_optz_flag( optzflag )
get_fast_flag( ffpflag )                            # ffp-contract flags
get_warning_flags( wflags )                         # warning flags
get_unalign_vec_move_flag( uavmflag )               # disabling alignment checking for all AVX ops
get_pic_flag( picflag )                             # position Independent Code flags
get_au_flag( auflag )                               # aocl-utils defination
get_avx2fma_flag( fmaflag )                         # Fused Multiply-Add flags
get_avx512_flag( avx512flag )                       # AVX512 ISA flags
get_vec_flag( vecflag )                             # Vectorization flags

set(LIBMCFLAGS ${optzflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${ffpflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${wflags})
set(LIBMCFLAGS ${LIBMCFLAGS} ${uavmflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${picflag})
set(LIBMCFLAGS ${LIBMCFLAGS} ${auflag})

# ---------------------------------------------------------------------------
# Template-based array wrapper generation (AVX2 and AVX-512)
#
# Instead of maintaining individual vrda_*.c / vrsa_*.c files for each math
# function, we use a single template file that is compiled multiple times
# with different -DALM_FUNC=<name> definitions. This approach:
#   - Reduces code duplication (one template instead of ~30 nearly-identical files)
#   - Makes maintenance easier (fix once, apply everywhere)
#   - Keeps behavior consistent across all array wrappers
#
# The template files use the PASTE macro to construct function names from
# ALM_FUNC at compile time.
#
# These variables are defined here so they are available to both
# src/optimized/ (for filtering) and src/isa/, src/arch/ (for compilation).
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# AVX2 template lists
# ---------------------------------------------------------------------------

# List of unary double functions for vrda AVX2 template.
# Each function generates a vrda_<name> wrapper (e.g., vrda_sin, vrda_cos).
# The wrapper calls the corresponding vrd4_<name> 4-wide vector function.
# NOTE: Keep in sync with VRDA_UNARY_AVX2_FUNCS in optimized_srcs.py (SCons).
# NOTE: fabs, acos excluded - standalone (no AVX-512 vrd8_* kernels for unified template).
set(VRDA_UNARY_AVX2_FUNCS
    sin cos tan exp exp2 log log2
    asin atan sqrt round
    erf erfc erfinv erfcinv
    cdfnorm cdfnorminv
)

# List of unary float functions for vrsa AVX2 template.
# Each function generates a vrsa_<name> wrapper (e.g., vrsa_sinf, vrsa_cosf).
# The wrapper calls the corresponding vrs8_<name> 8-wide vector function.
# NOTE: Keep in sync with VRSA_UNARY_AVX2_FUNCS in optimized_srcs.py (SCons).
# NOTE: fabsf, coshf, cbrtf excluded - standalone (no AVX-512 vrs16_* kernels for unified template).
set(VRSA_UNARY_AVX2_FUNCS
    sinf cosf tanf expf exp2f exp10f logf log2f log10f
    asinf atanf acosf sqrtf roundf
    erff erfcf tanhf
)

# Template file paths for AVX2 array wrappers
set(AVX2_VRDA_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/optimized/vec/avx2/vrda_unary_avx2.c")
set(AVX2_VRSA_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/optimized/vec/avx2/vrsa_unary_avx2.c")

# ---------------------------------------------------------------------------
# AVX-512 template lists
# ---------------------------------------------------------------------------

# List of unary double functions for vrda AVX-512 template.
# Each function generates a vrda_<name> wrapper (e.g., vrda_sin, vrda_cos).
# The wrapper calls the corresponding vrd8_<name> 8-wide vector function.
# NOTE: Keep in sync with VRDA_UNARY_AVX512_FUNCS in optimized_srcs.py (SCons).
set(VRDA_UNARY_AVX512_FUNCS
    sin cos tan exp exp2 log log2
    asin atan sqrt round
    erf erfc erfinv erfcinv
    cdfnorm cdfnorminv
)

# List of unary float functions for vrsa AVX-512 template.
# Each function generates a vrsa_<name> wrapper (e.g., vrsa_sinf, vrsa_cosf).
# The wrapper calls the corresponding vrs16_<name> 16-wide vector function.
# NOTE: Keep in sync with VRSA_UNARY_AVX512_FUNCS in optimized_srcs.py (SCons).
set(VRSA_UNARY_AVX512_FUNCS
    sinf cosf tanf expf exp2f exp10f logf log2f log10f
    asinf atanf acosf sqrtf roundf
    erff erfcf tanhf
)

# Template file paths for AVX-512 array wrappers
set(AVX512_VRDA_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/optimized/vec/avx512/vrda_unary_avx512.c")
set(AVX512_VRSA_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/optimized/vec/avx512/vrsa_unary_avx512.c")

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
  if("${ALM_STATIC_DISPATCH}" MATCHES "(AVX2|ZEN2|ZEN3|ZEN4|ZEN5|ZEN6|AVX512)")
    # Define the architecture macros
    add_compile_definitions(AVX2=1 ZEN2=2 ZEN3=3 ZEN4=4 ZEN5=5 AVX512=6 ZEN6=7)

    # Add the ALM_STATIC_DISPATCH definition
    add_compile_definitions(ALM_STATIC_DISPATCH=${ALM_STATIC_DISPATCH})
  endif()
endif()

# include paths under src.
set(INCLUDE_PATHS
  "${${PROJECT_PREFIX}_SOURCE_DIR}/include"
  "${${PROJECT_PREFIX}_SOURCE_DIR}/src"
  "${${PROJECT_PREFIX}_SOURCE_DIR}/src/optimized"
  "${${PROJECT_PREFIX}_SOURCE_DIR}/src/kern"
  )
include_directories(${INCLUDE_PATHS})

set(INCLUDES "${INCLUDE_PATHS}" "${ALM_UTILS_INCLUDE_DIR}")

#libmobj variable is to collect object libraries in subdirectories
set(libmobj "")

add_subdirectory(optimized)
add_subdirectory(isa)
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

# add src object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:src>)

# add include headers as part of the install
set(LIBM_PUBLIC_HEADERS "${${PROJECT_PREFIX}_SOURCE_DIR}/include/external/amdlibm.h"
                        "${${PROJECT_PREFIX}_SOURCE_DIR}/include/external/amdlibm_vec.h")

if (WIN32)
  set(LIBALM libalm)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  set(EXTRA_LIBS alm_utils_static "msvcrt")
  set(CMAKE_STATIC_LIBRARY_SUFFIX  "-static.lib")
else()
  set(LIBALM alm)
  set(EXTRA_LIBS alm_utils_static "-Wl,-Bdynamic -lc")
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
  set_target_properties(libm_shared PROPERTIES LINK_FLAGS "-Wl,-ealm_main -Wl,-z,noexecstack")
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

# Propagate the libmobj target list to the parent scope
set(libmobj ${libmobj} PARENT_SCOPE)
