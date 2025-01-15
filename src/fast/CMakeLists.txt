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

if (WIN32)
  set(ALMFAST libalmfast)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set(CMAKE_STATIC_LIBRARY_PREFIX "")
else()
  set(ALMFAST almfast)
endif()

get_fastmath_flag( fmflag )

file(GLOB FAST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
set(FAST_INCLUDE "${INCLUDE_PATHS}")
set(FAST_FLAGS ${LIBMCFLAGS} ${fmaflag} ${fmflag})

if (BUILD_SHARED_LIBS)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  add_library(libmfast SHARED ${FAST_SRC})
  set_target_properties(libmfast PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(libmfast PRIVATE ${libm})
else()
  add_library(libmfast STATIC ${FAST_SRC})
endif ()
target_include_directories(libmfast
                           PUBLIC  ${${PROJECT_PREFIX}_COMMON_INCLUDES}
                                   ${${PROJECT_PREFIX}_SDK_INCLUDES}
                           PRIVATE ${${PROJECT_PREFIX}_PRIVATE_INCLUDES}
                                   ${FAST_INCLUDE})
target_compile_options(libmfast PRIVATE ${FAST_FLAGS})
set_target_properties(libmfast PROPERTIES OUTPUT_NAME ${ALMFAST})
