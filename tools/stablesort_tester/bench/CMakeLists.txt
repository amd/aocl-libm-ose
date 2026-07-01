#
# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
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

#
# stablesort benchmark.
#
# A standalone executable that measures the built-in aocl-libm stablesort 64f
# indexed sort (side A) and, optionally, an external dlopen'd shim (side B). It
# links the aocl-libm shared library (which carries the amd_stablesort_*
# symbols) and libdl; no third-party sort enters this build.
#

add_executable(stablesort_bench src/bench.cpp)

# Include library's external header and bench's own headers
target_include_directories(stablesort_bench PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${${PROJECT_PREFIX}_SDK_INCLUDES}/external
)

# std::filesystem (used for --data) needs C++17 regardless of the library's
# default C++ standard.
set_target_properties(stablesort_bench PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

if(TARGET libm_shared)
  # At run time, point LD_LIBRARY_PATH at the intended libalm.so
  target_link_libraries(stablesort_bench PRIVATE libm_shared ${CMAKE_DL_LIBS})
else()
  message(FATAL_ERROR
    "stablesort_bench requires the aocl-libm shared library target "
    "(libm_shared). Enable LIBM_BUILD_LIBRARY.")
endif()
