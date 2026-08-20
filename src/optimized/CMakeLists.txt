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
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
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

set(OPTSRC
    fdim.c
    fdimf.c
    nearbyint.c
    nearbyintf.c
    sqrt.c
    sqrtf.c
    log1p_fma3.c
)

set(COMMONSRC
    exp_tables.c
    log_tables.c
    sincos_tbl.c
    tanf_tbl.c
    pow_data.c
    sinh_data.c
    vec/alm_special.c
)

file(GLOB DATASRC LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/data/*.c)

set(OPT_SRC ${OPTSRC} ${COMMONSRC} ${DATASRC})
if(CONFIG_COMPILER_HAS_AVX2)
    set(OPT_FLAGS ${LIBMCFLAGS} ${fmaflag} ${vecflag})
else()
    set(OPT_FLAGS ${LIBMCFLAGS} ${vecflag})
endif()
add_library(opt OBJECT ${OPT_SRC})
target_include_directories(opt
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${INCLUDE_PATHS})
target_compile_options(opt PRIVATE ${OPT_FLAGS})
# add opt object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:opt>)

set(libmobj ${libmobj} PARENT_SCOPE)


# ---------------------------------------------------------------------------
# Common source lists shared by src/arch/ and src/isa/.
# ---------------------------------------------------------------------------
set(OPT_DIR "${${PROJECT_PREFIX}_SOURCE_DIR}/src/optimized")
file(GLOB ARCH_OPT_SRCS       CONFIGURE_DEPENDS "${OPT_DIR}/*.c")
file(GLOB CMPLX_SRCS          CONFIGURE_DEPENDS "${OPT_DIR}/cmplx/*.c")
file(GLOB VEC_SRCS            CONFIGURE_DEPENDS "${OPT_DIR}/vec/*.c")
file(GLOB VECMATH_SRCS        CONFIGURE_DEPENDS "${OPT_DIR}/vectormath/*.c")
file(GLOB AVX512_VEC_SRCS     CONFIGURE_DEPENDS "${OPT_DIR}/vec/avx512/*.c")
file(GLOB AVX512_VECMATH_SRCS CONFIGURE_DEPENDS "${OPT_DIR}/vectormath/avx512/*.c")

# Exclude template files from the glob — they are compiled separately in the
# foreach loops in src/isa/ and src/arch/, not as part of the regular source list.
# The function lists and template paths are defined in src/CMakeLists.txt.
list(FILTER AVX512_VEC_SRCS EXCLUDE REGEX "vrda_unary_avx512\\.c$")
list(FILTER AVX512_VEC_SRCS EXCLUDE REGEX "vrsa_unary_avx512\\.c$")

# Get AVX2 sources (for filtering out template files)
file(GLOB AVX2_VEC_SRCS CONFIGURE_DEPENDS "${OPT_DIR}/vec/avx2/*.c")
list(FILTER AVX2_VEC_SRCS EXCLUDE REGEX "vrda_unary_avx2\\.c$")
list(FILTER AVX2_VEC_SRCS EXCLUDE REGEX "vrsa_unary_avx2\\.c$")

# Exclude files already compiled by the opt target to avoid duplicate symbols.
# Files in common/ and data/ are not picked up by the top-level glob, so only
# the non-arch scalar files need filtering.
set(_opt_abs "")
foreach(_f ${OPTSRC} ${COMMONSRC})
    list(APPEND _opt_abs "${OPT_DIR}/${_f}")
endforeach()
list(REMOVE_ITEM ARCH_OPT_SRCS ${_opt_abs})
list(REMOVE_ITEM VEC_SRCS ${_opt_abs})
# Files without arch-suffixed prototypes (no FN_PROTOTYPE_OPT)
list(FILTER ARCH_OPT_SRCS EXCLUDE REGEX "(log_v3)[^/]*\\.c$")

# ---------------------------------------------------------------------------
# AVX2 build: filter out standalone vrda_*/vrsa_* files that conflict with
# template-generated symbols.
# ---------------------------------------------------------------------------
set(_avx2_vec_names ${AVX2_VEC_SRCS})
list(TRANSFORM _avx2_vec_names REPLACE ".+/" "")

# Add AVX2 template-based function names to the exclusion list.
set(_AVX2_EXCL_LIST "vrda:VRDA_UNARY_AVX2_FUNCS" "vrsa:VRSA_UNARY_AVX2_FUNCS")
foreach(_entry IN LISTS _AVX2_EXCL_LIST)
    string(REPLACE ":" ";" _parts "${_entry}")
    list(GET _parts 0 _prefix)
    list(GET _parts 1 _funcs_var)
    foreach(func IN LISTS ${_funcs_var})
        list(APPEND _avx2_vec_names "${_prefix}_${func}.c")
    endforeach()
endforeach()

set(FILTERED_VEC_SRCS_AVX2 "")
foreach(_f ${VEC_SRCS})
    get_filename_component(_name "${_f}" NAME)
    if(NOT _name IN_LIST _avx2_vec_names)
        list(APPEND FILTERED_VEC_SRCS_AVX2 "${_f}")
    endif()
endforeach()

# ---------------------------------------------------------------------------
# AVX-512 build: filter out files replaced by AVX-512 kernels or templates.
# ---------------------------------------------------------------------------
set(_avx512_vec_names ${AVX512_VEC_SRCS})
list(TRANSFORM _avx512_vec_names REPLACE ".+/" "")

# Add AVX-512 template-based function names to the exclusion list.
set(_AVX512_EXCL_LIST "vrda:VRDA_UNARY_AVX512_FUNCS" "vrsa:VRSA_UNARY_AVX512_FUNCS")
foreach(_entry IN LISTS _AVX512_EXCL_LIST)
    string(REPLACE ":" ";" _parts "${_entry}")
    list(GET _parts 0 _prefix)
    list(GET _parts 1 _funcs_var)
    foreach(func IN LISTS ${_funcs_var})
        list(APPEND _avx512_vec_names "${_prefix}_${func}.c")
    endforeach()
endforeach()

set(FILTERED_VEC_SRCS_AVX512 "")
foreach(_f ${VEC_SRCS})
    get_filename_component(_name "${_f}" NAME)
    if(NOT _name IN_LIST _avx512_vec_names)
        list(APPEND FILTERED_VEC_SRCS_AVX512 "${_f}")
    endif()
endforeach()

set(_avx512_vecmath_names ${AVX512_VECMATH_SRCS})
list(TRANSFORM _avx512_vecmath_names REPLACE ".+/" "")
set(FILTERED_VECMATH_SRCS "")
foreach(_f ${VECMATH_SRCS})
    get_filename_component(_name "${_f}" NAME)
    if(NOT _name IN_LIST _avx512_vecmath_names)
        list(APPEND FILTERED_VECMATH_SRCS "${_f}")
    endif()
endforeach()

# AVX2: use filtered VEC_SRCS (without template-replaced files) + AVX2 kernels
set(AVX2_SRCS   ${ARCH_OPT_SRCS} ${CMPLX_SRCS} ${FILTERED_VEC_SRCS_AVX2} ${VECMATH_SRCS}
                ${AVX2_VEC_SRCS})
# AVX-512: use filtered VEC_SRCS + AVX-512 kernels
set(AVX512_SRCS ${ARCH_OPT_SRCS} ${CMPLX_SRCS} ${FILTERED_VEC_SRCS_AVX512} ${FILTERED_VECMATH_SRCS}
                ${AVX512_VEC_SRCS} ${AVX512_VECMATH_SRCS})

set(AVX2_SRCS   ${AVX2_SRCS}   PARENT_SCOPE)
set(AVX512_SRCS ${AVX512_SRCS} PARENT_SCOPE)
