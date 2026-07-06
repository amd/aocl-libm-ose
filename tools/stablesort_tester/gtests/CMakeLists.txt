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
# stablesort unit tests
#
# stablesort.c is compiled a SECOND time into this target with
# -DALM_SORT_UNIT_TEST, which exposes thin wrappers over the file-local static
# kernels (see stablesort_testapi.h). The shipped library never defines that
# macro, so production symbol hiding is untouched. Uses the repo's vendored
# fused GoogleTest; built with ASAN by default.
#

option(STABLESORT_TESTS_ASAN "Build the stablesort unit tests with AddressSanitizer" ON)

set(_ss_root   ${${PROJECT_PREFIX}_SOURCE_DIR})
set(_ss_gtest  ${_ss_root}/gtests/gapi/gtest)

add_executable(stablesort_gtest
  ${_ss_root}/src/optimized/stablesort.c
  ${_ss_gtest}/src/gtest_all.cc
  src/test_common.cc
  src/test_probe.cc
  src/test_radix.cc
  src/test_merge.cc
  src/test_api.cc
  src/main.cc
)

target_include_directories(stablesort_gtest PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${${PROJECT_PREFIX}_SDK_INCLUDES}          # libm_macros.h, libm/*
  ${_ss_gtest}                               # fused gtest.h
)

set_target_properties(stablesort_gtest PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

# Expose the internal test wrappers when compiling this copy of stablesort.c.
set_source_files_properties(${_ss_root}/src/optimized/stablesort.c
  PROPERTIES COMPILE_DEFINITIONS "ALM_SORT_UNIT_TEST")

# The vendored gtest is third-party: don't police its warnings.
set_source_files_properties(${_ss_gtest}/src/gtest_all.cc PROPERTIES COMPILE_OPTIONS "-w")

# Match the GTEST_HAS_PTHREAD=1 + pthread link used by gtests/gapi/CMakeLists.txt
set_source_files_properties(${_ss_gtest}/src/gtest_all.cc
  PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_PTHREAD=1")

# Keep debug asserts (ALM_SORT_ASSERT, run-stack guard) live even in Release.
target_compile_options(stablesort_gtest PRIVATE -UNDEBUG)

if(NOT WIN32)
  target_link_libraries(stablesort_gtest PRIVATE pthread)
else()
  target_link_libraries(stablesort_gtest PRIVATE msvcrt Shlwapi)
endif()

if(STABLESORT_TESTS_ASAN)
  target_compile_options(stablesort_gtest PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(stablesort_gtest PRIVATE -fsanitize=address)
endif()

# Optional: gcov/lcov coverage instrumentation. Off by default; 
# pair with -DSTABLESORT_TESTS_ASAN=OFF for clean counts.
option(STABLESORT_TESTS_COVERAGE "Build the stablesort unit tests with gcov coverage instrumentation" OFF)
if(STABLESORT_TESTS_COVERAGE AND NOT WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
  target_compile_options(stablesort_gtest PRIVATE -g -fprofile-arcs -ftest-coverage)
  target_link_options(stablesort_gtest PRIVATE --coverage)

  # Mirror the project's 'coverage' target: run the suite, then capture just
  # stablesort.c and emit an HTML report under html_coverage_report/.
  find_program(LCOV lcov)
  find_program(GENHTML genhtml)
  if(LCOV AND GENHTML)
    add_custom_target(stablesort_coverage
      COMMAND $<TARGET_FILE:stablesort_gtest>
      COMMAND ${LCOV} --capture --directory "${CMAKE_CURRENT_BINARY_DIR}"
              --gcov-tool gcov --rc lcov_branch_coverage=1 --output-file "coverage.info"
      COMMAND ${LCOV} --extract "coverage.info" "*/src/optimized/stablesort.c"
              --rc lcov_branch_coverage=1 --output-file "stablesort.info"
      COMMAND ${GENHTML} "stablesort.info" --branch-coverage --legend
              --title "stablesort.c coverage" --output-directory "html_coverage_report"
      COMMAND ${CMAKE_COMMAND} -E rm -f "coverage.info" "stablesort.info"
      COMMENT "Running stablesort_gtest and generating stablesort.c coverage report..."
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_dependencies(stablesort_coverage stablesort_gtest)
  endif()
endif()

# Optional: resource-heavy edge cases (multi-GiB). Off by default.
option(STABLESORT_TESTS_EXTREME "Enable resource-heavy (multi-GiB) stablesort tests" OFF)
if(STABLESORT_TESTS_EXTREME)
  target_compile_definitions(stablesort_gtest PRIVATE TEST_EXTREME)
endif()

enable_testing()
add_test(NAME stablesort_gtest COMMAND stablesort_gtest)
