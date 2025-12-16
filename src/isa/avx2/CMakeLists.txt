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

set(AVX2_ASM
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/fmaf_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/fma_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/log_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/log1p_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrs4_cbrtf_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrs4_exp10f_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrs4_expm1f_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrs4_log1pf_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrd2_cbrt_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrd2_exp10_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrd2_log10_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrd2_log1p_fma3.S
    ${CMAKE_CURRENT_SOURCE_DIR}/gas/vrda_expm1_fma3.S
)

set(AVX2_INCLUDE ${ISA_INCLUDE}   "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/gas/include" )
add_library(avx2asm OBJECT ${AVX2_ASM})
target_include_directories(avx2asm
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${AVX2_INCLUDE})
target_compile_options(avx2asm PRIVATE ${ISA_FLAGS})
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(avx2asm PROPERTIES MSVC_DEBUG_INFORMATION_FORMAT "")
endif()

file(GLOB AVX2_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
set(AVX2_FLAGS ${LIBMCFLAGS} ${fmaflag} ${vecflag})
add_library(avx2 OBJECT ${AVX2_SRC})
target_include_directories(avx2
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${AVX2_INCLUDE})
target_compile_options(avx2 PRIVATE ${AVX2_FLAGS})
