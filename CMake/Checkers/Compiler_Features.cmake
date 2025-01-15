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

include(CheckCCompilerFlag)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  include(Gcc)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  include(Clang)
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





#Common LIBM FLAGS abd CFLAGS Flags Macroes
macro(get_optz_flag optzflag)
  set(${optzflag} -O3)
endmacro()

macro(get_fast_flag ffpflag)
  if(NOT WIN32)
    set(${ffpflag} -ffp-contract=fast)
  else()
    set(${ffpflag} /fp:fast)
  endif()
endmacro()

macro(get_win_flag winflag)
  set(${winflag}  -DAVX_XOP_FMA4_FMA3 -DDEBUG=0 -DENABLE_AMDLIBM_API=1 -DLIBABI=aocl -D__AVX2__ -Dlibalm_EXPORTS -m64 /DWIN32 /D_WINDOWS /DNDEBUG -march=native )
endmacro()

macro(get_au_flag auflag)
  set(${auflag} -DUSE_AOCL_UTILS)
endmacro()

macro(get_avx2fma_flag fmaflag)
  set(${fmaflag} -mavx2 -mfma)
endmacro()

macro(get_fastmath_flag fmflag)
  set(${fmflag} -Dlibalmfast_EXPORTS)
endmacro()

macro(get_vec_flag vecflag)
  set(${vecflag} -flax-vector-conversions)
endmacro()

macro(get_isa_flag isaflag)
  set(${isaflag} -DAVX_XOP_FMA4_FMA3)
endmacro()