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

set(COMPACT_LIB alm-glibc-compat)

set(COMPACT_FLAGS ${fmaflag} ${wflags} ${picflag})

add_library(glibc_obj OBJECT glibc-compat.c  ${INCLUDE_PATHS})
target_compile_options(glibc_obj PRIVATE ${COMPACT_FLAGS})
target_include_directories(glibc_obj
                            PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                    ${${PROJECT_PREFIX}_SDK_INCLUDES}
                            PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES})

add_library(libmglibc_static STATIC $<TARGET_OBJECTS:glibc_obj>)
set_target_properties(libmglibc_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ${COMPACT_LIB}
)
list(APPEND libm libmglibc_static)

add_library(libmglibc_shared SHARED $<TARGET_OBJECTS:glibc_obj>)
set_target_properties(libmglibc_shared PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME ${COMPACT_LIB}
)
list(APPEND libm libmglibc_shared)
set(libm ${libm} PARENT_SCOPE)

