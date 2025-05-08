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

include(FetchContent)
set(AU_BUILD_DOCS OFF)
set(au                aoclutils)
set(sourcedir         ${CMAKE_CURRENT_BINARY_DIR}/${au})
set(binarydir         ${CMAKE_CURRENT_BINARY_DIR}/${au}_build)
set(git_repo          "https://github.com/amd/aocl-utils.git")
set(git_tag           "dev")

FetchContent_Declare(
    ${au}
    GIT_REPOSITORY ${git_repo}
    GIT_TAG        ${git_tag}
    SOURCE_DIR     ${sourcedir}
    BINARY_DIR     ${binarydir}
)

# Check if the content has already been populated
FetchContent_GetProperties(${au})
if(NOT ${au}_POPULATED)
  FetchContent_MakeAvailable(${au})
endif()

set(libname au_cpuid)
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    string(APPEND libname -dbg)
endif()
set(AOCL_UTILS_INCLUDE_DIR  ${sourcedir}/SDK/Bcl ${sourcedir}/SDK/Include ${binarydir}/generated)
set(AOCLUTILS_PATH          ${binarydir}/Library/Cpuid)
set(AUCPUID_STATIC          ${CMAKE_STATIC_LIBRARY_PREFIX}${libname}${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AUCPUID_SHARED          ${CMAKE_SHARED_LIBRARY_PREFIX}${libname}${CMAKE_SHARED_LIBRARY_SUFFIX})
set(AUCPUID_IMPORT          ${CMAKE_SHARED_LIBRARY_PREFIX}${libname}${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AOCLUTILS_STATIC        ${AOCLUTILS_PATH}/${AUCPUID_STATIC})
if (WIN32)
  set(AOCLUTILS_DLL         ${AOCLUTILS_PATH}/${AUCPUID_SHARED})
  set(AOCLUTILS_SHARED      ${AOCLUTILS_PATH}/${AUCPUID_IMPORT})
else()
  set(AOCLUTILS_SHARED      ${AOCLUTILS_PATH}/${AUCPUID_SHARED})
endif()

if(BUILD_SHARED_LIBS)
  set(AOCL_UTILS_LIB        ${AOCLUTILS_SHARED})
else()
  set(AOCL_UTILS_LIB        ${AOCLUTILS_STATIC})
endif()
