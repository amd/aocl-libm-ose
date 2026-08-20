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

set(AVX2_CSRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/fmaf_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fma_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrs4_cbrtf_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrs4_exp10f_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrs4_expm1f_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrs4_log1pf_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrd2_cbrt_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrd2_exp10_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrd2_log10_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrd2_log1p_fma3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vrda_expm1_fma3.c
)

set(AVX2_INCLUDE ${ISA_INCLUDE}   "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_library(avx2srcs OBJECT ${AVX2_CSRCS})
target_include_directories(avx2srcs
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${AVX2_INCLUDE})
target_compile_options(avx2srcs PRIVATE ${ISA_FLAGS} ${fmaflag})
# These naked kernels are direct ports of hand-written asm that had no CET
# endbr64 landing pad; -fcf-protection=none matches the asm and drops the
# per-call endbr64 (measurable on tiny hot kernels such as vrd2_exp10).
if(NOT WIN32)
    target_compile_options(avx2srcs PRIVATE -fcf-protection=none)
endif()
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(avx2srcs PROPERTIES MSVC_DEBUG_INFORMATION_FORMAT "")
endif()
