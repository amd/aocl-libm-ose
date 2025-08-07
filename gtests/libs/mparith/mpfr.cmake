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

if (WIN32)
    if(NOT DEFINED ENV{MPFR_PATH})
        message(FATAL_ERROR "MPFR_PATH environment variable is not set(set MPFR_PATH=path/to/mpfr)")
    endif()
    set(MPFR_PATH $ENV{MPFR_PATH})
    file(TO_NATIVE_PATH "${MPFR_PATH}" MPFR_PATH)
    set(MPFR_DIR "${MPFR_PATH}/mpfr_x64-windows")      # GNU MPFR (Multiple Precision Floating-Point Reliable)
    set(MPC_DIR "${MPFR_PATH}/mpc_x64-windows")        # Multiple Precision Complex
    set(GMP_DIR "${MPFR_PATH}/gmp_x64-windows")        # GNU Multiple Precision Arithmetic Library

    list(APPEND CMAKE_PREFIX_PATH
      "${MPFR_DIR}"
      "${MPC_DIR}"
      "${GMP_DIR}"
    )

    include_directories(
      "${MPFR_DIR}/include"
      "${MPC_DIR}/include"
      "${GMP_DIR}/include"
    )
    link_directories(
      "${MPFR_DIR}/lib"
      "${MPC_DIR}/lib"
      "${GMP_DIR}/lib"
    )
endif()

include(FindPackageHandleStandardArgs)
#------------------------------------------------------------------------------
# FIND & CONFIGURE MPFR
#------------------------------------------------------------------------------
find_path(MPFR_INCLUDE_DIR  NAMES mpfr.h  HINTS "${MPFR_DIR}/include"
                                        ENV   MPFR_INCLUDE_DIR )
find_library(MPFR_LIB  NAMES mpfr  HINTS "${MPFR_DIR}/lib"
                                        ENV   MPFR_LIBRARY_DIR )

find_package_handle_standard_args(MPFR REQUIRED_VARS MPFR_INCLUDE_DIR MPFR_LIB)
if (MPFR_FOUND)
  set(MPFR_INCLUDE_DIR ${MPFR_INCLUDE_DIR})
  set(MPFR_LIBRARY_DIR ${MPFR_LIBRARY_DIR})
  message(STATUS "MPFR configured: ${MPFR_LIB}")
else()
  message(FATAL_ERROR "MPFR not found!")
endif()

#------------------------------------------------------------------------------
# FIND & CONFIGURE MPC
#------------------------------------------------------------------------------
find_path(MPC_INCLUDE_DIR  NAMES mpc.h  HINTS "${MPC_DIR}/include"
                                        ENV   MPC_INCLUDE_DIR
)
find_library(MPC_LIB  NAMES mpc  HINTS "${MPC_DIR}/lib"
                                        ENV   MPC_LIBRARY_DIR
)
find_package_handle_standard_args(MPC REQUIRED_VARS MPC_INCLUDE_DIR MPC_LIB)
if (MPC_FOUND)
  set(MPC_INCLUDE_DIR ${MPC_INCLUDE_DIR})
  set(MPC_LIBRARY_DIR ${MPC_LIBRARY_DIR})
  message(STATUS "MPC configured: ${MPC_LIB}")
else()
  message(FATAL_ERROR "MPC not found!")
endif()

#------------------------------------------------------------------------------
# FIND & CONFIGURE GMP
#------------------------------------------------------------------------------
find_path(GMP_INCLUDE_DIR  NAMES gmp.h  HINTS "${GMP_DIR}/include"
                                        ENV   GMP_INCLUDE_DIR )

find_library(GMP_LIB  NAMES gmp  HINTS "${GMP_DIR}/lib"
                                        ENV   GMP_LIBRARY_DIR )

find_package_handle_standard_args(GMP REQUIRED_VARS GMP_INCLUDE_DIR GMP_LIB)
if (GMP_FOUND)
  set(GMP_INCLUDE_DIR ${GMP_INCLUDE_DIR})
  set(GMP_LIBRARY_DIR ${GMP_LIBRARY_DIR})
  message(STATUS "GMP configured: ${GMP_LIB}")
else()
  message(FATAL_ERROR "GMP not found!")
endif()
