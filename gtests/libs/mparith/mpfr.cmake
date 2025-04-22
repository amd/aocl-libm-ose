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
  set(MPFR_DIR "${MPFR_PATH}/mpfr_x64-windows")      # GNU MPFR (Multiple Precision Floating-Point Reliable)
  set(MPC_DIR "${MPFR_PATH}/mpc_x64-windows")        # Multiple Precision Complex
  set(GMP_DIR "${MPFR_PATH}/gmp_x64-windows")        # GNU Multiple Precision Arithmetic Library

  # Set the library directories
  set(MPFR_LIBRARY_DIR "${MPFR_DIR}/lib")
  set(MPC_LIBRARY_DIR  "${MPC_DIR}/lib")
  set(GMP_LIBRARY_DIR  "${GMP_DIR}/lib")
endif()

find_path(MPFR_INCLUDE_DIR mpfr.h PATHS ENV MPFR_INCLUDE_DIR HINTS ${MPFR_DIR} PATH_SUFFIXES include)
if(NOT MPFR_INCLUDE_DIR)
  message(FATAL_ERROR "MPFR include directory not found!")
endif()

find_library(MPFR_LIBRARIES NAMES mpfr PATHS ENV MPFR_LIBRARY_DIR HINTS ${MPFR_LIBRARY_DIR})
if(NOT MPFR_LIBRARIES)
  message(FATAL_ERROR "MPFR library not found!")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_INCLUDE_DIR MPFR_LIBRARIES)
if(MPFR_FOUND)
  set(MPFR_LIB ${MPFR_LIBRARIES})
  set(MPFR_INCLUDE_DIR ${MPFR_INCLUDE_DIR})
  set(MPFR_LIBRARY_DIR ${MPFR_LIBRARY_DIR})
  message(STATUS "MPFR found and configured successfully.")
else()
  message(FATAL_ERROR "MPFR not found!")
endif()

find_path(MPC_INCLUDE_DIR mpc.h PATHS ENV MPC_INCLUDE_DIR HINTS ${MPC_DIR} PATH_SUFFIXES include)
if(NOT MPC_INCLUDE_DIR)
  message(FATAL_ERROR "MPC include directory not found!")
endif()

find_library(MPC_LIBRARIES NAMES mpc PATHS ENV MPC_LIBRARY_DIR HINTS ${MPC_LIBRARY_DIR})
if(NOT MPC_LIBRARIES)
  message(FATAL_ERROR "MPC library not found!")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPC DEFAULT_MSG MPC_INCLUDE_DIR MPC_LIBRARIES)
if(MPC_FOUND)
  set(MPC_LIB ${MPC_LIBRARIES})
  set(MPC_INCLUDE_DIR ${MPC_INCLUDE_DIR})
  set(MPC_LIBRARY_DIR ${MPC_LIBRARY_DIR})
  message(STATUS "MPC found and configured successfully.")
else()
  message(FATAL_ERROR "MPC not found!")
endif()

find_path(GMP_INCLUDE_DIR gmp.h PATHS ENV GMP_INCLUDE_DIR HINTS ${GMP_DIR} PATH_SUFFIXES include)
if(NOT GMP_INCLUDE_DIR)
  message(FATAL_ERROR "GMP include directory not found!")
endif()

find_library(GMP_LIBRARIES NAMES gmp PATHS ENV GMP_LIBRARY_DIR HINTS ${GMP_LIBRARY_DIR})
if(NOT GMP_LIBRARIES)
  message(FATAL_ERROR "GMP library not found!")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMP DEFAULT_MSG GMP_INCLUDE_DIR GMP_LIBRARIES)
if(GMP_FOUND)
  set(GMP_LIB ${GMP_LIBRARIES})
  set(GMP_INCLUDE_DIR ${GMP_INCLUDE_DIR})
  set(GMP_LIBRARY_DIR ${GMP_LIBRARY_DIR})
  message(STATUS "GMP found and configured successfully.")
else()
  message(FATAL_ERROR "GMP not found!")
endif()
