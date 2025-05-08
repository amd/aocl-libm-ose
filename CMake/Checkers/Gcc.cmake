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


set(GCC_VERSION_MIN   "9.2.0")
set(GCC_VERSION_MAX   "14.2.1")

if ((CMAKE_C_COMPILER_VERSION VERSION_LESS ${GCC_VERSION_MIN}) OR
    (CMAKE_C_COMPILER_VERSION VERSION_GREATER ${GCC_VERSION_MAX}))
    message(FATAL_ERROR "Unsupported GCC Compiler version: ${CMAKE_C_COMPILER_VERSION}. \
                          Please use GCC version between ${GCC_VERSION_MIN} and ${GCC_VERSION_MAX}.")
endif()
set(CONFIG_COMPILER_IS_GCC   1)

macro(CHECK_COMPILER_VERSION GCC_VERSION)
  set(${GCC_VERSION}  "gcc-${CMAKE_C_COMPILER_VERSION}")
endmacro()

#LIBM FLAGS abd CFLAGS Flags Macroes
macro(get_fast_flag ffpflag)
  set(${ffpflag} -ffp-contract=fast)
endmacro()

macro(get_warning_flags wflags)
  #Set gcc-compiler flags for aocl-libm
  set(w1 -Wall -Wextra -Wcast-qual -Wconversion -Wdisabled-optimization -Wdouble-promotion)
  set(w2 -Wformat=2 -Winit-self -Wno-init-self -Winvalid-pch -Wmissing-declarations -Wodr)
  set(w3 -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wtrampolines -Wundef)
  set(w4 -Wvector-operation-performance -Werror -Wlto-type-mismatch)
  set(w5 -Wall -W -Wstrict-prototypes -Werror -Wno-unused-parameter)
  set(${wflags} ${w1} ${w2} ${w3} ${w4} ${w5})
endmacro()

macro(get_unalign_vec_move_flag uavmflag)
  set(${uavmflag})
endmacro()

macro(get_pic_flag picflag)
  set(${picflag} -fPIC)
endmacro()


#LINKER Flags for Shared library
macro(get_linker_flag sharedlinkerflag)
  set(${sharedlinkerflag})
endmacro()