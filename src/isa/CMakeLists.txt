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

set(ISA_INCLUDE ${INCLUDE_PATHS} "${CMAKE_CURRENT_SOURCE_DIR}/include")

get_isa_flag( isaflag )

set(ISA_FLAGS ${isaflag} ${ASAN_FLAGS} ${COVERAGE_FLAGS})

add_subdirectory(avx)
# add avxsrcs object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:avxsrcs>)

add_subdirectory(avx2)
# add avx2srcs object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:avx2srcs>)

# ---------------------------------------------------------------------------
# ISA-specific compilation using -include to inject ISA header.
# Mirrors the pattern in src/arch/CMakeLists.txt.
#   avx2  (ISA_HAS_AVX512=0): COMMON_SRCS compiled with fmaflag
#   avx512(ISA_HAS_AVX512=1): COMMON_SRCS + AVX512_SRCS compiled with avx512flag
# ---------------------------------------------------------------------------
set(ALM_ISA_LIST)

if(CONFIG_COMPILER_HAS_AVX2)
    list(APPEND ALM_ISA_LIST "avx2:AVX2:0")
endif()
if(CONFIG_COMPILER_HAS_AVX512)
    list(APPEND ALM_ISA_LIST "avx512:AVX512:1")
endif()

set(ISA_INCLUDES
    ${${PROJECT_PREFIX}_COMMON_INCLUDES}
    ${${PROJECT_PREFIX}_SDK_INCLUDES}
    ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
    ${INCLUDE_PATHS}
)

set(ISA_CFLAGS ${LIBMCFLAGS} ${vecflag})
set(ISA_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/x86_isa.h)

# clang-cl counts GCC-style -include as an extra input and rejects /Fo; use /FI like MSVC.
if(MSVC)
  set(ISA_FORCE_INCLUDE "/FI${ISA_HEADER}")
else()
  set(ISA_FORCE_INCLUDE "-include;${ISA_HEADER}")
endif()

foreach(ISA_ENTRY IN LISTS ALM_ISA_LIST)
    string(REPLACE ":" ";" _PARTS "${ISA_ENTRY}")
    list(GET _PARTS 0 ISA_DIR)
    list(GET _PARTS 1 ISA_DEF)
    list(GET _PARTS 2 ISA_HAS_AVX512)

    if(ISA_HAS_AVX512)
        # avx512: COMMON_SRCS + AVX512-specific sources, compiled with AVX512 flags.
        add_library(isa_${ISA_DIR} OBJECT ${AVX512_SRCS})
        target_compile_options(isa_${ISA_DIR} PRIVATE
            ${ISA_FORCE_INCLUDE}
            -DALM_BUILD_${ISA_DEF}=1
            ${ISA_CFLAGS} ${avx512flag}
        )
    else()
        # avx2: COMMON_SRCS only, compiled with FMA/AVX2 flags.
        add_library(isa_${ISA_DIR} OBJECT ${AVX2_SRCS})
        target_compile_options(isa_${ISA_DIR} PRIVATE
            ${ISA_FORCE_INCLUDE}
            -DALM_BUILD_${ISA_DEF}=1
            ${ISA_CFLAGS} ${fmaflag}
        )
    endif()

    target_include_directories(isa_${ISA_DIR} PRIVATE ${ISA_INCLUDES})
    list(APPEND libmobj $<TARGET_OBJECTS:isa_${ISA_DIR}>)
endforeach()

# ---------------------------------------------------------------------------
# Unified template-based array wrapper compilation (AVX2 and AVX-512)
#
# This unified loop compiles both AVX2 and AVX-512 array wrappers using a
# single parameterized configuration. Each ISA (avx2, avx512) has its own:
#   - Build flag (ALM_BUILD_AVX2, ALM_BUILD_AVX512)
#   - Compiler flags (fmaflag, avx512flag)
#   - Function lists and template files
#
# The template file uses PASTE(vrda_, ALM_FUNC) to construct the function
# name at compile time. For example, with -DALM_FUNC=sin, the template
# generates vrda_sin() which calls vrd4_sin (AVX2) or vrd8_sin (AVX-512).
#
# This produces one object file per function per ISA (e.g., isa_vrda_sin_avx2.o).
# ---------------------------------------------------------------------------

# Unified ISA template configuration
# Format: "isa:build_flag:compiler_flag_var:has_compiler_check"
set(_ISA_TEMPLATE_CONFIGS)
if(CONFIG_COMPILER_HAS_AVX2)
    list(APPEND _ISA_TEMPLATE_CONFIGS "avx2:AVX2:fmaflag:CONFIG_COMPILER_HAS_AVX2")
endif()
if(CONFIG_COMPILER_HAS_AVX512)
    list(APPEND _ISA_TEMPLATE_CONFIGS "avx512:AVX512:avx512flag:CONFIG_COMPILER_HAS_AVX512")
endif()

foreach(_ISA_CONFIG IN LISTS _ISA_TEMPLATE_CONFIGS)
    string(REPLACE ":" ";" _ISA_PARTS "${_ISA_CONFIG}")
    list(GET _ISA_PARTS 0 _ISA)           # avx2 or avx512
    list(GET _ISA_PARTS 1 _BUILD_FLAG)    # AVX2 or AVX512
    list(GET _ISA_PARTS 2 _CFLAGS_VAR)    # fmaflag or avx512flag

    # Template configurations for this ISA
    # Format: "prefix:template_var:funcs_list_var"
    string(TOUPPER "${_ISA}" _ISA_UPPER)
    set(_TEMPLATE_LIST
        "vrda:${_ISA_UPPER}_VRDA_TEMPLATE:VRDA_UNARY_${_ISA_UPPER}_FUNCS"
        "vrsa:${_ISA_UPPER}_VRSA_TEMPLATE:VRSA_UNARY_${_ISA_UPPER}_FUNCS"
    )

    # Middle loop: iterate over template configurations (vrda, vrsa)
    foreach(_TMPL_ENTRY IN LISTS _TEMPLATE_LIST)
        string(REPLACE ":" ";" _TMPL_PARTS "${_TMPL_ENTRY}")
        list(GET _TMPL_PARTS 0 _PREFIX)       # vrda or vrsa
        list(GET _TMPL_PARTS 1 _TEMPLATE_VAR) # variable holding template path
        list(GET _TMPL_PARTS 2 _FUNCS_VAR)    # variable holding function list

        # Inner loop: compile template once per function
        foreach(func IN LISTS ${_FUNCS_VAR})
            set(tgt isa_${_PREFIX}_${func}_${_ISA})
            add_library(${tgt} OBJECT ${${_TEMPLATE_VAR}})
            target_compile_options(${tgt} PRIVATE
                ${ISA_FORCE_INCLUDE}
                -DALM_BUILD_${_BUILD_FLAG}=1
                -DALM_FUNC=${func}
                ${ISA_CFLAGS} ${${_CFLAGS_VAR}}
            )
            target_include_directories(${tgt} PRIVATE ${ISA_INCLUDES})
            list(APPEND libmobj $<TARGET_OBJECTS:${tgt}>)
        endforeach()
    endforeach()
endforeach()

set(libmobj ${libmobj} PARENT_SCOPE)
