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

include(CheckCCompilerFlag)

set(GCC_VERSION_MIN   "9.2")
set(GCC_VERSION_MAX   "14.2")
set(CLANG_VERSION_MIN "9.0")
set(CLANG_VERSION_MAX "18.1")
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    if ((CMAKE_C_COMPILER_VERSION VERSION_LESS ${GCC_VERSION_MIN}) OR
        (CMAKE_C_COMPILER_VERSION VERSION_GREATER ${GCC_VERSION_MAX}))
        message(FATAL_ERROR "Unsupported GCC Compiler version: ${CMAKE_C_COMPILER_VERSION}. \
                             Please use GCC version between ${GCC_VERSION_MIN} and ${GCC_VERSION_MAX}.")
    endif()
    set(CONFIG_COMPILER_IS_GCC   1)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    if ((CMAKE_C_COMPILER_VERSION VERSION_LESS ${CLANG_VERSION_MIN}) OR
        (CMAKE_C_COMPILER_VERSION VERSION_GREATER ${CLANG_VERSION_MAX}))
    message(FATAL_ERROR "Unsupported Clang Compiler version: ${CMAKE_C_COMPILER_VERSION}. \
                         Please use Clang version between ${CLANG_VERSION_MIN} and ${CLANG_VERSION_MAX}.")
    endif()
    set(CONFIG_COMPILER_IS_CLANG 1)
else()
    message(WARNING "Unsupported compiler .")
endif()

check_c_compiler_flag("-msse2"           CONFIG_COMPILER_HAS_SSE2)
check_c_compiler_flag("-msse3"           CONFIG_COMPILER_HAS_SSE3)
check_c_compiler_flag("-mavx"            CONFIG_COMPILER_HAS_AVX)
check_c_compiler_flag("-mavx2"           CONFIG_COMPILER_HAS_AVX2)
check_c_compiler_flag("-mavx512f"        CONFIG_COMPILER_HAS_AVX512)
check_c_compiler_flag("-march=x86-64"    CONFIG_COMPILER_HAS_X86_64)
check_c_compiler_flag("-march=znver1"    CONFIG_COMPILER_HAS_ZEN)
check_c_compiler_flag("-march=znver2"    CONFIG_COMPILER_HAS_ZEN2)
check_c_compiler_flag("-march=znver3"    CONFIG_COMPILER_HAS_ZEN3)
check_c_compiler_flag("-march=znver4"    CONFIG_COMPILER_HAS_ZEN4)
check_c_compiler_flag("-march=znver5"    CONFIG_COMPILER_HAS_ZEN5)

check_c_compiler_flag("-mavx2"           CONFIG_TARGET_IS_AVX2)
check_c_compiler_flag("-march=x86-64"    CONFIG_TARGET_IS_X86_64)
check_c_compiler_flag("-march=x86-64-v2" CONFIG_TARGET_IS_X86_64_v2)
check_c_compiler_flag("-march=x86-64-v3" CONFIG_TARGET_IS_X86_64_v3)
check_c_compiler_flag("-march=x86-64-v4" CONFIG_TARGET_IS_X86_64_v4)

set(archlist znver5 znver4 znver3 znver2 znver1)
set(archdetect_code "
  #include <stdio.h>
  int main() { return 0; } ")
file(WRITE "${PROJECT_BINARY_DIR}/arch.c" "${archdetect_code}")
set(maxarch "X86_64")
foreach(arch ${archlist})
  try_run(RUNRESULT COMPILERESULT "${PROJECT_BINARY_DIR}/temp" SOURCES  "${PROJECT_BINARY_DIR}/arch.c"
    COMPILE_DEFINITIONS -march=${arch}
    RUN_OUTPUT_VARIABLE RESULT
  )
  if( COMPILERESULT )
    set(maxarch ${arch})
    string(REPLACE "znver" "zen" maxarch ${maxarch})
    break()
  endif()
endforeach()

# Create a #define for max arch.
string(TOUPPER ${maxarch} UARCH)
set(MAX_ALM_ARCH "CONFIG_COMPILER_HAS_${UARCH}")

macro(get_arch res alist)
  foreach(ar ${${alist}})
    try_run(RUNRESULT COMPILERESULT "${PROJECT_BINARY_DIR}/temp" SOURCES  "${PROJECT_BINARY_DIR}/arch.c"
      COMPILE_DEFINITIONS -march=${ar}
      RUN_OUTPUT_VARIABLE RESULT
    )
    if( COMPILERESULT )
      set(${res} ${ar})
      break()
    endif()
  endforeach()
endmacro()

macro(get_zen1_arch_flags zen1)
  set(arch znver1 x86-64)
  get_arch(res arch)
  set(${zen1} -march=${res})
endmacro()

macro(get_zen2_arch_flags zen2)
  set(arch znver2 znver1 x86-64)
  get_arch(res arch)
  set(${zen2} -march=${res})
endmacro()

macro(get_zen3_arch_flags zen3)
  set(arch znver3 znver2 znver1 x86-64)
  get_arch(res arch)
  set(${zen3} -march=${res})
endmacro()

macro(get_zen4_arch_flags zen4)
  set(arch znver4 znver3 znver2 znver1 x86-64)
  get_arch(res arch)
  set(${zen4} -march=${res})
endmacro()

macro(get_zen5_arch_flags zen5)
  set(arch znver5 znver4 znver3 znver2 znver1 x86-64)
  get_arch(res arch)
  set(${zen5} -march=${res})
endmacro()
