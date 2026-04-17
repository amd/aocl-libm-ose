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

# ---------------------------------------------------------------------------
# Arch-specific compilation using -include to inject arch header.
# ---------------------------------------------------------------------------
get_zen1_arch_flags(ZEN1_MARCH)
get_zen2_arch_flags(ZEN2_MARCH)
get_zen3_arch_flags(ZEN3_MARCH)
get_zen4_arch_flags(ZEN4_MARCH)
get_zen5_arch_flags(ZEN5_MARCH)
get_zen6_arch_flags(ZEN6_MARCH)

set(ALM_WRAPPER_ARCH_LIST)
list(APPEND ALM_WRAPPER_ARCH_LIST "zen:ZEN:${ZEN1_MARCH}:0")
list(APPEND ALM_WRAPPER_ARCH_LIST "zen2:ZEN2:${ZEN2_MARCH}:0")
list(APPEND ALM_WRAPPER_ARCH_LIST "zen3:ZEN3:${ZEN3_MARCH}:0")
list(APPEND ALM_WRAPPER_ARCH_LIST "zen4:ZEN4:${ZEN4_MARCH}:1")
list(APPEND ALM_WRAPPER_ARCH_LIST "zen5:ZEN5:${ZEN5_MARCH}:1")
list(APPEND ALM_WRAPPER_ARCH_LIST "zen6:ZEN6:${ZEN6_MARCH}:1")

set(ZEN_INCLUDES
    ${${PROJECT_PREFIX}_COMMON_INCLUDES}
    ${${PROJECT_PREFIX}_SDK_INCLUDES}
    ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
    ${INCLUDE_PATHS}
)

set(ZEN_FLAGS ${LIBMCFLAGS} ${vecflag})
set(ARCH_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/zen_arch.h)

# clang-cl counts GCC-style -include as an extra input and rejects /Fo; use /FI like MSVC.
if(MSVC)
  set(ARCH_FORCE_INCLUDE "/FI${ARCH_HEADER}")
else()
  set(ARCH_FORCE_INCLUDE "-include;${ARCH_HEADER}")
endif()

foreach(ARCH_ENTRY IN LISTS ALM_WRAPPER_ARCH_LIST)
    string(REPLACE ":" ";" _PARTS "${ARCH_ENTRY}")
    list(GET _PARTS 0 ARCH_DIR)
    list(GET _PARTS 1 ARCH_DEF)
    list(GET _PARTS 2 ARCH_MARCH)
    list(GET _PARTS 3 ARCH_HAS_AVX512)

    if(ARCH_HAS_AVX512)
        # zen4/zen5/zen6: one target — COMMON_SRCS + AVX512-specific sources,
        # all compiled with AVX512 flags.  No separate base target is created,
        # so there is no risk of duplicate symbols between two targets sharing
        # the same -DALM_BUILD_<ARCH> define.
        add_library(${ARCH_DIR} OBJECT ${AVX512_SRCS})
        target_compile_options(${ARCH_DIR} PRIVATE
            ${ARCH_FORCE_INCLUDE}
            -DALM_BUILD_${ARCH_DEF}=1
            -march=${ARCH_MARCH}
            ${ZEN_FLAGS} ${avx512flag}
        )
    else()
        # zen/zen2/zen3: COMMON_SRCS only, no AVX512 flags.
        add_library(${ARCH_DIR} OBJECT ${AVX2_SRCS})
        target_compile_options(${ARCH_DIR} PRIVATE
            ${ARCH_FORCE_INCLUDE}
            -DALM_BUILD_${ARCH_DEF}=1
            -march=${ARCH_MARCH}
            ${ZEN_FLAGS} ${fmaflag}
        )
    endif()

    target_include_directories(${ARCH_DIR} PRIVATE ${ZEN_INCLUDES})
    list(APPEND libmobj $<TARGET_OBJECTS:${ARCH_DIR}>)
endforeach()

set(libmobj ${libmobj} PARENT_SCOPE)
