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

set(ISA_INCLUDE ${INCLUDE_PATHS} "${CMAKE_CURRENT_SOURCE_DIR}/include")

get_isa_flag( isaflag )

set(ISA_FLAGS ${isaflag} ${ASAN_FLAGS} ${COVERAGE_FLAGS})

add_subdirectory(avx)
# add avxsrcs object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:avxsrcs>)

add_subdirectory(avx2)
# add avx2asm object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:avx2asm>)

# ---------------------------------------------------------------------------
# ISA-specific compilation using -include to inject ISA header.
# Mirrors the pattern in src/arch/CMakeLists.txt.
#   avx2  (ISA_HAS_AVX512=0): COMMON_SRCS compiled with fmaflag
#   avx512(ISA_HAS_AVX512=1): COMMON_SRCS + AVX512_SRCS compiled with avx512flag
# ---------------------------------------------------------------------------
set(ALM_ISA_LIST)

if(CONFIG_COMPILER_HAS_AVX2)
    list(APPEND ALM_ISA_LIST "avx2:AVX2:0")
endif()
if(CONFIG_COMPILER_HAS_AVX512)
    list(APPEND ALM_ISA_LIST "avx512:AVX512:1")
endif()

set(ISA_INCLUDES
    ${${PROJECT_PREFIX}_COMMON_INCLUDES}
    ${${PROJECT_PREFIX}_SDK_INCLUDES}
    ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
    ${INCLUDE_PATHS}
)

set(ISA_CFLAGS ${LIBMCFLAGS} ${vecflag})
set(ISA_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/x86_isa.h)

# clang-cl counts GCC-style -include as an extra input and rejects /Fo; use /FI like MSVC.
if(MSVC)
  set(ISA_FORCE_INCLUDE "/FI${ISA_HEADER}")
else()
  set(ISA_FORCE_INCLUDE "-include;${ISA_HEADER}")
endif()

foreach(ISA_ENTRY IN LISTS ALM_ISA_LIST)
    string(REPLACE ":" ";" _PARTS "${ISA_ENTRY}")
    list(GET _PARTS 0 ISA_DIR)
    list(GET _PARTS 1 ISA_DEF)
    list(GET _PARTS 2 ISA_HAS_AVX512)

    if(ISA_HAS_AVX512)
        # avx512: COMMON_SRCS + AVX512-specific sources, compiled with AVX512 flags.
        add_library(isa_${ISA_DIR} OBJECT ${AVX512_SRCS})
        target_compile_options(isa_${ISA_DIR} PRIVATE
            ${ISA_FORCE_INCLUDE}
            -DALM_BUILD_${ISA_DEF}=1
            ${ISA_CFLAGS} ${avx512flag}
        )
    else()
        # avx2: COMMON_SRCS only, compiled with FMA/AVX2 flags.
        add_library(isa_${ISA_DIR} OBJECT ${AVX2_SRCS})
        target_compile_options(isa_${ISA_DIR} PRIVATE
            ${ISA_FORCE_INCLUDE}
            -DALM_BUILD_${ISA_DEF}=1
            ${ISA_CFLAGS} ${fmaflag}
        )
    endif()

    target_include_directories(isa_${ISA_DIR} PRIVATE ${ISA_INCLUDES})
    list(APPEND libmobj $<TARGET_OBJECTS:isa_${ISA_DIR}>)
endforeach()

set(libmobj ${libmobj} PARENT_SCOPE)
