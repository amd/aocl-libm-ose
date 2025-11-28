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

cmake_minimum_required(VERSION 3.23)
project(mparith)

# Set the output directory for the libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEPS_DIR})
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(EXEFILES cospi.c finite.c frexp.c ilogb.c logb.c mparith_c.c mparith_f.c sinpi.c tanpi.c controlword.c cycles.c mallocDebug.c nanny.c statusword.c)

file(GLOB_RECURSE MPARITH_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")

# Exclude CMakeFiles directory
list(FILTER MPARITH_SRCS EXCLUDE REGEX "CMakeFiles/.*")
list(REMOVE_ITEM MPARITH_SRCS ${EXEFILES})
set(MPARITH_INCS "${CMAKE_CURRENT_SOURCE_DIR}")
set(MPARITH_FLAGS -fPIC -Wwrite-strings -fno-strict-aliasing -D_GNU_SOURCE -D_ISOC99_SOURCE -DIS_64BIT)

set(libmparithobj "")
add_library(mparith32 OBJECT ${MPARITH_SRCS})
target_include_directories(mparith32 PRIVATE ${MPARITH_INCS})
target_compile_options(mparith32 PRIVATE -DFLOAT -Dlibmparith32_EXPORTS ${MPARITH_FLAGS})

add_library(mparith64 OBJECT ${MPARITH_SRCS})
target_include_directories(mparith64 PRIVATE ${MPARITH_INCS})
target_compile_options(mparith64 PRIVATE -DDOUBLE -Dlibmparith64_EXPORTS ${MPARITH_FLAGS})

list(APPEND libmparithobj $<TARGET_OBJECTS:mparith32>)
list(APPEND libmparithobj $<TARGET_OBJECTS:mparith64>)

if (WIN32)
  set(LIBMPARITH libmparith)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set(CMAKE_STATIC_LIBRARY_PREFIX "")
else()
  set(LIBMPARITH mparith)
endif()

if (1)
  add_library(mparith_static STATIC ${libmparithobj})
  set_target_properties(mparith_static PROPERTIES OUTPUT_NAME ${LIBMPARITH})
  target_link_libraries(mparith_static PUBLIC ${MPFR_LIB} ${MPC_LIB})
  set(libmparith mparith_static)
else()
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  add_library(mparith_shared SHARED ${libmparithobj})
  set_target_properties(mparith_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set_target_properties(mparith_shared PROPERTIES OUTPUT_NAME ${LIBMPARITH})
  target_link_libraries(mparith_shared PUBLIC ${MPFR_LIB})
  set(libmparith mparith_shared)
endif()
