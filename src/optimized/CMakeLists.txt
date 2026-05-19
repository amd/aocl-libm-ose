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

file(GLOB OPTSRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB DATASRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/data/*.c)
file(GLOB CMPLXSRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/cmplx/*.c)
file(GLOB VECSRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/vec/*.c)
file(GLOB VECMATHSRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/vectormath/*.c)

set(OPT_SRC ${OPTSRC} ${DATASRC} ${CMPLXSRC} ${VECSRC} ${VECMATHSRC})
set(OPT_FLAGS ${LIBMCFLAGS} ${fmaflag} ${vecflag} )
add_library(opt OBJECT ${OPT_SRC})
target_include_directories(opt
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${INCLUDE_PATHS})
target_compile_options(opt PRIVATE ${OPT_FLAGS})
#add opt object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:opt>)

# AVX512-width optimized sources.
file(GLOB VECAVX512SRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/vec/avx512/*.c)
file(GLOB VECMATHAVX512SRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/vectormath/avx512/*.c)
set(OPT_SRC_AVX512 ${VECAVX512SRC} ${VECMATHAVX512SRC})
add_library(opt_avx512 OBJECT ${OPT_SRC_AVX512})
target_include_directories(opt_avx512
                        PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                ${${PROJECT_PREFIX}_SDK_INCLUDES}
                        PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                ${INCLUDE_PATHS})
target_compile_options(opt_avx512 PRIVATE ${LIBMCFLAGS} ${avx512flag} ${vecflag})
list(APPEND libmobj $<TARGET_OBJECTS:opt_avx512>)

set(libmobj ${libmobj} PARENT_SCOPE)
