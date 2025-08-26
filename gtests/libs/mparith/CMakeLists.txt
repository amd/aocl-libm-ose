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

cmake_minimum_required(VERSION 3.26)
project(mparith LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
include(mpfr)

# Define build directories
set(MPFR_SRC_DIR        ${PROJECT_SOURCE_DIR})
set(MPARITH_INC_DIR     ${MPFR_SRC_DIR} ${MPFR_INCLUDE_DIR} ${MPC_INCLUDE_DIR} ${GMP_INCLUDE_DIR})
set(MPARITH_DIR         "${CMAKE_CURRENT_SOURCE_DIR}/libs" CACHE PATH "Installation directory for mparith")

# Installation directories
set(MPARITH_LIB_DIR     ${MPARITH_DIR}/lib)
set(MPARITH_INCLUDE_DIR ${MPARITH_DIR}/include)

# Ensure the install directory exists
file(MAKE_DIRECTORY ${MPARITH_LIB_DIR})
file(MAKE_DIRECTORY ${MPARITH_INCLUDE_DIR})

# Define library paths
set(LIB_PATHS ${MPFR_SRC_DIR})
set(EXEFILES cospi.c finite.c frexp.c ilogb.c logb.c mparith_c.c mparith_f.c sinpi.c tanpi.c controlword.c cycles.c mallocDebug.c nanny.c statusword.c)

file(GLOB MPARITH_SRCS
           acos.c
           acosh.c
           add.c
           asin.c
           asinh.c
           atan.c
           atan2.c
           atanh.c
           cbrt.c
           ceil.c
           cexp.c
           copysign.c
           cos.c
           cosh.c
           erf.c
           erfc.c
           exp.c
           exp2.c
           exp10.c
           expm1.c
           fabs.c
           fdim.c
           floor.c
           fma.c
           fmax.c
           fmin.c
           fmod.c
           hypot.c
           ldexp.c
           linearfrac.c
           log.c
           log1p.c
           log2.c
           log10.c
           modf.c
           mul.c
           nextafter.c
           pow.c
           remainder.c
           remquo.c
           rint.c
           round.c
           sin.c
           sincos.c
           sinh.c
           sqrt.c
           sub.c
           tan.c
           tanh.c
           trunc.c
)

# Compiler flags
if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wwrite-strings -fno-strict-aliasing -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT")
    set(MPARITH_DEPS_LIBS mpfr mpc m)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0 -DIS_64BIT -DWINDOWS")
    set(MPARITH_DEPS_LIBS ${MPFR_LIB} ${MPC_LIB} ${GMP_LIB} msvcrt)
    set(MPARITH_DEPS_LIBS_DIRS "${MPFR_LIBRARY_DIR} ${MPC_LIBRARY_DIR} ${GMP_LIBRARY_DIR}")
endif()

# Create object libraries
add_library(mparith32obj OBJECT ${MPARITH_SRCS})
target_compile_definitions(mparith32obj PRIVATE -DFLOAT -Dlibmparith32_EXPORTS)
target_include_directories(mparith32obj PRIVATE ${MPARITH_INC_DIR})

add_library(mparith64obj OBJECT ${MPARITH_SRCS})
target_compile_definitions(mparith64obj PRIVATE -DDOUBLE -Dlibmparith64_EXPORTS)
target_include_directories(mparith64obj PRIVATE ${MPARITH_INC_DIR})

set(ALM_MP_FUNCS_H alm_mp_funcs.h)
set(MPARITH_HEADER ${MPFR_SRC_DIR}/${ALM_MP_FUNCS_H})

set(LIBMPARITH mparith)

if(MPARITH_SHARED_LIBS)
  # Combine 32-bit and 64-bit object libraries into a single shared library
  add_library(mparith SHARED $<TARGET_OBJECTS:mparith32obj> $<TARGET_OBJECTS:mparith64obj>)
  set_target_properties(mparith PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(mparith PRIVATE ${MPARITH_DEPS_LIBS})
  target_link_directories(mparith PRIVATE ${MPARITH_DEPS_LIBS_DIRS})
  target_include_directories(mparith PRIVATE ${MPARITH_INC_DIR})
  set_target_properties(mparith PROPERTIES PRIVATE_HEADER "${MPARITH_HEADER}")
  set_target_properties(mparith PROPERTIES OUTPUT_NAME ${LIBMPARITH})

  add_custom_command(TARGET mparith POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mparith> ${MPARITH_LIB_DIR}/$<TARGET_FILE_NAME:mparith>)
  add_custom_command(TARGET mparith POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_LINKER_FILE:mparith> ${MPARITH_LIB_DIR}/$<TARGET_LINKER_FILE_NAME:mparith>)
  add_custom_command(TARGET mparith POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy ${MPARITH_HEADER} ${MPARITH_INCLUDE_DIR}/${ALM_MP_FUNCS_H})

else()
  # Combine 32-bit and 64-bit object libraries into a single static library
  add_library(mparith STATIC $<TARGET_OBJECTS:mparith32obj> $<TARGET_OBJECTS:mparith64obj>)
  set_target_properties(mparith PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(mparith PRIVATE  ${MPARITH_DEPS_LIBS})
  target_link_directories(mparith PRIVATE ${MPARITH_DEPS_LIBS_DIRS})
  target_include_directories(mparith PRIVATE  ${MPARITH_INC_DIR})
  set_target_properties(mparith PROPERTIES PRIVATE_HEADER "${MPARITH_HEADER}")
  set_target_properties(mparith PROPERTIES OUTPUT_NAME ${LIBMPARITH})

  add_custom_command(TARGET mparith POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:mparith> ${MPARITH_LIB_DIR}/$<TARGET_FILE_NAME:mparith>)
  add_custom_command(TARGET mparith POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy ${MPARITH_HEADER} ${MPARITH_INCLUDE_DIR}/${ALM_MP_FUNCS_H})
endif()
