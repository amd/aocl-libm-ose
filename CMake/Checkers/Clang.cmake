#
# Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
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


set(CLANG_VERSION_MIN "9.0.0")
set(CLANG_VERSION_MAX "18.1.0")

if ((CMAKE_C_COMPILER_VERSION VERSION_LESS ${CLANG_VERSION_MIN}) OR
    (CMAKE_C_COMPILER_VERSION VERSION_GREATER ${CLANG_VERSION_MAX}))
message(FATAL_ERROR "Unsupported Clang Compiler version: ${CMAKE_C_COMPILER_VERSION}. \
                      Please use Clang version between ${CLANG_VERSION_MIN} and ${CLANG_VERSION_MAX}.")
endif()
set(CONFIG_COMPILER_IS_CLANG  1)

#LIBM FLAGS abd CFLAGS Flags Macroes
macro(get_warning_flags wflags)
  if(NOT WIN32)
    set(${wflags} -Wall -W -Wstrict-prototypes -Werror -Wno-unused-parameter)
  else()
    set(${wflags}  /W0 /MP -Wno-c++11-narrowing -fdiagnostics-absolute-paths -ferror-limit=0 )
  endif()
endmacro()

macro(get_unalign_vec_move_flag uavmflag)
  if(CMAKE_C_COMPILER_VERSION GREATER_EQUAL 14.0.6)
    set(${uavmflag} -muse-unaligned-vector-move)
  endif()
endmacro()

macro(get_pic_flag picflag)
  if(NOT WIN32)
    set(${picflag} -fPIC)
  else()
    set(${picflag})
  endif()
endmacro()

#LINKER Flags for Shared library
macro(get_linker_flag sharedlinkerflag)
  if(NOT WIN32)
    set(${sharedlinkerflag} "-fuse-ld=ld")
  else()
    set(${sharedlinkerflag} "/dll")
  endif()
endmacro()
