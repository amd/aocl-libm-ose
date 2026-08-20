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
#

if (WIN32)
  set(ALMFAST libalmfast)
else()
  set(ALMFAST almfast)
endif()

get_fastmath_flag( fmflag )

set(ALM   "${${PROJECT_PREFIX}_SOURCE_DIR}/src/alm_special.c")
file(GLOB FAST_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.c")
list(APPEND FAST_SRC "${ALM}")
set(FAST_INCLUDE "${INCLUDE_PATHS}")
set(FAST_FLAGS  ${ffpflag} ${fmflag} ${wflags} ${picflag} ${winflag} ${fmaflag})

add_library(libmfast_obj OBJECT ${FAST_SRC})
target_include_directories(libmfast_obj
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${FAST_INCLUDE})
target_compile_options(libmfast_obj PRIVATE ${FAST_FLAGS})

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
# Compile alm_main.c into the shared fast library so it gets the same
# direct-execution entry point (version banner) as libalm.so. Without this the
# shared object has entry point 0x0 and no PT_INTERP, so running
# ./libalmfast.so jumps into the ELF header and segfaults.
add_library(libmfast_shared SHARED
    $<TARGET_OBJECTS:libmfast_obj>
    ${${PROJECT_PREFIX}_SOURCE_DIR}/src/alm_main.c)
set_target_properties(libmfast_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ${ALMFAST}
)
target_include_directories(libmfast_shared PRIVATE ${FAST_INCLUDE})
# Set alm_main as the ELF entry point so ./libalmfast.so self-executes (skipped for MSVC).
if(NOT MSVC)
  set_target_properties(libmfast_shared PROPERTIES
      LINK_FLAGS "-Wl,-ealm_main -Wl,-z,noexecstack")
endif()
list(APPEND libm libmfast_shared)

add_library(libmfast_static STATIC $<TARGET_OBJECTS:libmfast_obj>)
set_target_properties(libmfast_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ${ALMFAST}
)
list(APPEND libm libmfast_static)

set(libm ${libm} PARENT_SCOPE)

