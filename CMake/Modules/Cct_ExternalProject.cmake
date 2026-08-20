#
# Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.
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

#[[
  Cct_ExternalProject.cmake - Helper functions for building external projects.

  Centralizes Visual Studio (multi-config) generator handling so that all
  dependent external projects (gapi, mparith, etc.) get the correct
  -A / -T / --config flags and library output directory without duplicating
  the logic in every module.

  Provided functions:
    alm_external_cmake_args(<out_var>)
    alm_external_build_args(<out_var> <binary_dir>)
    alm_external_output_dir_args(<out_var> <dir>)
    alm_is_multi_config_generator(<out_var>)
    alm_add_config_wrapper_target(<wrapper> <binary_dir> <inner_target>)
]]

include_guard(GLOBAL)

# alm_is_multi_config_generator(<out_var>)
#   Sets <out_var> to TRUE when the current generator is multi-config
#   (Visual Studio, Ninja Multi-Config).
function(alm_is_multi_config_generator out_var)
    get_property(_is_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    set(${out_var} ${_is_multi} PARENT_SCOPE)
endfunction()

# alm_external_cmake_args(<out_var>
#                          [SOURCE_DIR  <dir>]
#                          [BINARY_DIR  <dir>]
#                          [EXTRA_ARGS  <arg>...])
#   Populates <out_var> with the cmake configure arguments suitable for
#   the current generator.  For Visual Studio generators the function
#   forwards -A (platform) and -T (toolset) instead of compiler paths;
#   for single-config generators it forwards the compiler and build type.
function(alm_external_cmake_args out_var)
    cmake_parse_arguments(_AEC "" "SOURCE_DIR;BINARY_DIR" "EXTRA_ARGS" ${ARGN})

    set(_args -G "${CMAKE_GENERATOR}")

    if(_AEC_SOURCE_DIR)
        list(APPEND _args -S "${_AEC_SOURCE_DIR}")
    endif()
    if(_AEC_BINARY_DIR)
        list(APPEND _args -B "${_AEC_BINARY_DIR}")
    endif()

    alm_is_multi_config_generator(_multi)
    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        # Visual Studio only: forward platform and toolset; do NOT pass compiler paths.
        if(CMAKE_GENERATOR_PLATFORM)
            list(APPEND _args -A "${CMAKE_GENERATOR_PLATFORM}")
        else()
            list(APPEND _args -A x64)
        endif()
        if(CMAKE_GENERATOR_TOOLSET)
            list(APPEND _args -T "${CMAKE_GENERATOR_TOOLSET}")
        endif()
    else()
        list(APPEND _args
            "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        )
    endif()

    if(_AEC_EXTRA_ARGS)
        list(APPEND _args ${_AEC_EXTRA_ARGS})
    endif()

    set(${out_var} ${_args} PARENT_SCOPE)
endfunction()

# alm_external_build_args(<out_var> <binary_dir>)
#   Populates <out_var> with the cmake --build arguments.
#   For multi-config generators, appends --config ${CMAKE_BUILD_TYPE}.
function(alm_external_build_args out_var binary_dir)
    set(_args --build "${binary_dir}")

    alm_is_multi_config_generator(_multi)
    if(_multi)
        list(APPEND _args --config ${CMAKE_BUILD_TYPE})
    endif()

    set(${out_var} ${_args} PARENT_SCOPE)
endfunction()

# alm_external_output_dir_args(<out_var> <dir>)
#   Populates <out_var> with -DCMAKE_{RUNTIME,ARCHIVE,LIBRARY}_OUTPUT_DIRECTORY
#   arguments that flatten an external sub-build's artifacts into <dir> on every
#   generator.  For multi-config generators (Visual Studio, Ninja Multi-Config)
#   the per-config _<CONFIG> variants are also emitted so the artifacts land
#   directly in <dir> instead of a Debug/Release subfolder.
function(alm_external_output_dir_args out_var dir)
    set(_args
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${dir}
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${dir}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${dir}
    )
    alm_is_multi_config_generator(_multi)
    if(_multi AND CMAKE_BUILD_TYPE)
        string(TOUPPER "${CMAKE_BUILD_TYPE}" _cfg)
        list(APPEND _args
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_${_cfg}=${dir}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_cfg}=${dir}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY_${_cfg}=${dir}
        )
    endif()
    set(${out_var} ${_args} PARENT_SCOPE)
endfunction()

# alm_add_config_wrapper_target(<wrapper> <binary_dir> <inner_target>)
#   Creates a convenience target <wrapper> (named after the component directory)
#   that builds <inner_target>.  All components live in the single top-level
#   build tree, so the wrapper simply depends on <inner_target>; the active
#   configuration is inherited from the driving `cmake --build [--config <cfg>]`
#   invocation, which is correct for both single-config (Ninja) and multi-config
#   (Visual Studio) generators.  <binary_dir> is retained for call-site
#   compatibility and is intentionally unused.
function(alm_add_config_wrapper_target wrapper binary_dir inner_target)
    add_custom_target(${wrapper} DEPENDS ${inner_target})
endfunction()