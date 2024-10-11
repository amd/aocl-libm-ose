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

file(GLOB REF_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB CMPLX_SRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/cmplx/*.c)

# Reference files, for the functions where optimized version doesn't exist
set(EXSRC amd_pow.c exp2.c exp_tables.c MapEntryPoints.c)

set(REF_SRC ${REF_SOURCE} ${CMPLX_SRC})
list(REMOVE_ITEM REF_SRC ${EXSRC})
set(REF_FLAGS ${LIBMCFLAGS} -O2 -mavx2 -mavx -mfma )
add_library(ref OBJECT ${REF_SRC})
target_include_directories(ref
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${INCLUDE_PATHS})
target_compile_options(ref PRIVATE ${REF_FLAGS})
#add ref object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:ref>)
set(libmobj ${libmobj} PARENT_SCOPE)
