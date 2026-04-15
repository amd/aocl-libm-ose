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


# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------

# Check if a directory exists
function(directory_exists dir result_var)
    if(EXISTS "${dir}" AND IS_DIRECTORY "${dir}")
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Locate a library file (by exact filename) inside <libpath>/lib or <libpath>/lib64
function(library_exists lib libpath libdir result_var)
    if(EXISTS "${libpath}/lib" AND IS_DIRECTORY "${libpath}/lib")
        set(lib_search_path "${libpath}/lib")
    elseif(EXISTS "${libpath}/lib64" AND IS_DIRECTORY "${libpath}/lib64")
        set(lib_search_path "${libpath}/lib64")
    else()
        set(${result_var} FALSE PARENT_SCOPE)
        set(${libdir} "" PARENT_SCOPE)
        return()
    endif()

    find_library(${lib}_FOUND NAMES ${lib} PATHS "${lib_search_path}" NO_DEFAULT_PATH)
    if(${lib}_FOUND)
        set(${result_var} TRUE PARENT_SCOPE)
        set(${libdir} "${lib_search_path}" PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
        set(${libdir} "" PARENT_SCOPE)
    endif()
    unset(${lib}_FOUND CACHE)
endfunction()

# check_libcxx(<lib_path> <result_var>) — Linux only
# Sets result_var TRUE for libc++, FALSE for libstdc++ (via nm).
# Fatal if nm fails, both runtimes are present, or neither is found.
if(LINUX)
    function(check_libcxx lib_path result_var)
        if(CMAKE_NM)
            set(nm_command "${CMAKE_NM}")
        else()
            set(nm_command nm)
        endif()

        # Shared libraries are often stripped: .symtab is removed but .dynsym
        # (the dynamic symbol table) always survives.  Use -D for .so files so
        # that nm reads .dynsym; for static archives nm reads .symtab directly.
        if(lib_path MATCHES "\\.so(\\.[0-9]+)*$")
            set(_nm_flags -D)
        else()
            set(_nm_flags "")
        endif()

        execute_process(
            COMMAND "${nm_command}" ${_nm_flags} "${lib_path}"
            OUTPUT_VARIABLE nm_output
            RESULT_VARIABLE nm_result
            ERROR_VARIABLE  nm_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT nm_result EQUAL 0)
            message(FATAL_ERROR
                "Cannot inspect symbols in '${lib_path}' (nm exit ${nm_result}): "
                "${nm_error}\n"
                "Install 'nm' (or set -DCMAKE_NM=<path>) and provide a non-stripped "
                "library, or remove the file to rebuild aocl-utils from source.")
        endif()
        # libc++ -> _ZNSt3__<N>; libstdc++ -> _ZNSt7__cxx11 (non-overlapping).
        string(REGEX MATCH "_ZNSt3__[0-9]+" _has_libcxx    "${nm_output}")
        string(REGEX MATCH "_ZNSt7__cxx11"  _has_libstdcxx "${nm_output}")
        if(_has_libcxx AND _has_libstdcxx)
            message(FATAL_ERROR
                "Conflicting C++ runtimes in '${lib_path}': both libc++ and "
                "libstdc++ symbols are present. Rebuild against a single C++ runtime.")
        elseif(_has_libcxx)
            set(${result_var} TRUE PARENT_SCOPE)
        elseif(_has_libstdcxx)
            set(${result_var} FALSE PARENT_SCOPE)
        else()
            message(FATAL_ERROR
                "Cannot determine C++ runtime of '${lib_path}': neither libc++ "
                "(_ZNSt3__<N>) nor libstdc++ (_ZNSt7__cxx11) symbols were found. "
                "Library may be stripped, built with -fvisibility=hidden, or expose "
                "only a C ABI. Provide a non-stripped library or rebuild from source.")
        endif()
    endfunction()
endif()

# Configure + build + install aocl-utils as a CMake sub-build
function(build_aocl_utils source_dir binary_dir)
    message(STATUS "Building aocl-utils from: ${source_dir}")

    set(au_extra_args "")
    if(LINUX)
        list(APPEND au_extra_args "-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}")
        list(APPEND au_extra_args "-DCMAKE_CXX_CLANG_TIDY:STRING=/bin/true")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --fresh -G "${CMAKE_GENERATOR}" -S "${source_dir}" -B "${binary_dir}"
                            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                            -DCMAKE_BUILD_TYPE=Release
                            -DAU_BUILD_SHARED_LIBS=${AU_SHARED}
                            -DAU_BUILD_STATIC_LIBS=${AU_STATIC}
                            -DAU_CMAKE_VERBOSE=OFF
                            -DAU_BUILD_TESTS=OFF
                            -DAU_BUILD_EXAMPLES=OFF
                            -DCMAKE_INSTALL_PREFIX=${AU_INSTALL_DIR}
                            -DCMAKE_INSTALL_LIBDIR=lib
                            ${au_extra_args}
                    RESULT_VARIABLE configure_result
                    OUTPUT_VARIABLE configure_output
                    ERROR_VARIABLE configure_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT configure_result EQUAL 0)
        message(FATAL_ERROR "Failed to configure aocl-utils build:\n${configure_error}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build "${binary_dir}" --config Release --target install
                    RESULT_VARIABLE build_result
                    OUTPUT_VARIABLE build_output
                    ERROR_VARIABLE build_error
                    WORKING_DIRECTORY "${source_dir}"
    )

    if(NOT build_result EQUAL 0)
        message(FATAL_ERROR "Failed to build aocl-utils:\n${build_error}")
    endif()

    message(STATUS "Successfully built aocl-utils")
endfunction()

# Clone aocl-utils (if AU_SOURCE_DIR is missing) and then build + install it.
# Returns the install libdir via <out_libdir> (PARENT_SCOPE).
function(ensure_aocl_utils_built out_libdir)
    if(EXISTS "${AU_SOURCE_DIR}" AND IS_DIRECTORY "${AU_SOURCE_DIR}")
        message(STATUS "Aocl-Utils directory found, (re)building library...")
    else()
        message(STATUS "Aocl-Utils directory not found, cloning repository...")

        if(NOT DEFINED ${PROJECT_PREFIX}_AU_GIT_TAG)
            message(FATAL_ERROR "${PROJECT_PREFIX}_AU_GIT_TAG must be defined")
        endif()
        if(NOT DEFINED ${PROJECT_PREFIX}_AU_GIT_REPO_URL)
            message(FATAL_ERROR "${PROJECT_PREFIX}_AU_GIT_REPO_URL must be defined")
        endif()

        execute_process(COMMAND git clone --branch "${${PROJECT_PREFIX}_AU_GIT_TAG}"
                                        "${${PROJECT_PREFIX}_AU_GIT_REPO_URL}"
                                        "${AU_SOURCE_DIR}"
                        RESULT_VARIABLE clone_result
                        OUTPUT_VARIABLE clone_output
                        ERROR_VARIABLE  clone_error
        )

        if(NOT clone_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone aocl-utils repository:\n${clone_error}")
        endif()

        message(STATUS "Successfully cloned aocl-utils (tag: ${${PROJECT_PREFIX}_AU_GIT_TAG})")
    endif()

    build_aocl_utils("${AU_SOURCE_DIR}" "${AU_BINARY_DIR}")

    library_exists("${AOCL_UTILS_LIB}" "${AU_INSTALL_DIR}" _au_libdir _au_built)
    if(NOT _au_built)
        message(FATAL_ERROR "Failed to build aocl-utils library: ${AOCL_UTILS_LIB}")
    endif()
    set(${out_libdir} "${_au_libdir}" PARENT_SCOPE)
endfunction()

# ---------------------------------------------------------------------------
# Required inputs
# ---------------------------------------------------------------------------
if(NOT DEFINED PROJECT_PREFIX)
    message(FATAL_ERROR "PROJECT_PREFIX must be defined before including this module")
endif()

# Preserve user-provided values before we repurpose AOCL_UTILS_LIB below.
# (User may set AOCL_UTILS_INCLUDE_DIR and AOCL_UTILS_LIB as absolute paths.)
set(_USER_AOCL_UTILS_INCLUDE_DIR "${AOCL_UTILS_INCLUDE_DIR}")
set(_USER_AOCL_UTILS_LIB         "${AOCL_UTILS_LIB}")

# Linux only: pick expected C++ runtime by compiler (Clang -> libc++, GCC -> libstdc++).
if(LINUX)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(AU_EXPECT_LIBCXX TRUE)
    else()
        set(AU_EXPECT_LIBCXX FALSE)
    endif()
endif()


# ---------------------------------------------------------------------------
# Configuration options
# ---------------------------------------------------------------------------
set(AU_SHARED OFF CACHE BOOL "Build shared libraries for aocl-utils")
set(AU_STATIC OFF CACHE BOOL "Build static libraries for aocl-utils")

if(NOT AU_SHARED AND NOT AU_STATIC)
    set(AU_STATIC ON)
    message(STATUS "Neither AU_SHARED nor AU_STATIC was set, defaulting to static libraries")
endif()


# ---------------------------------------------------------------------------
# Paths and platform-specific library names
# ---------------------------------------------------------------------------
set(AU_SOURCE_DIR  "${PROJECT_SOURCE_DIR}/build/external/aocl-utils")
set(AU_BINARY_DIR  "${AU_SOURCE_DIR}/build")
set(AU_INSTALL_DIR "${AU_SOURCE_DIR}/amd-utils")

if(WIN32)
    set(AOCLUTILS_DLL         "au_cpuid.dll")
    set(AOCLUTILS_SHARED      "au_cpuid.lib")
    set(AOCLUTILS_STATIC      "au_cpuid_static.lib")
else()
    set(AOCLUTILS_SHARED      "libau_cpuid.so")
    set(AOCLUTILS_STATIC      "libau_cpuid.a")
endif()

# From here on AOCL_UTILS_LIB holds ONLY the bare library filename used for
# the sub-build. The user-provided absolute path is saved in _USER_AOCL_UTILS_LIB.
if(AU_SHARED)
    set(AOCL_UTILS_LIB "${AOCLUTILS_SHARED}")
else()
    set(AOCL_UTILS_LIB "${AOCLUTILS_STATIC}")
endif()

# Linux: Clang -> add -stdlib=libc++; GCC defaults to libstdc++ (no flag).
set(_SAVED_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
if(LINUX)
    if(AU_EXPECT_LIBCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()


# ===========================================================================
# Platform-specific provisioning logic
# ===========================================================================
if(WIN32)
    # WINDOWS: use user-provided lib+include if set; else use sub-build at
    # AU_INSTALL_DIR, building (and cloning if needed) when missing.
    if(EXISTS "${_USER_AOCL_UTILS_INCLUDE_DIR}" AND EXISTS "${_USER_AOCL_UTILS_LIB}")
        message(STATUS "User-provided aocl-utils: ${_USER_AOCL_UTILS_LIB}")
        set(AOCL_UTILS_INCLUDE_DIR "${_USER_AOCL_UTILS_INCLUDE_DIR}")
        set(AOCL_UTILS_LIB         "${_USER_AOCL_UTILS_LIB}")
        get_filename_component(AOCL_UTILS_LIB_DIR "${AOCL_UTILS_LIB}" DIRECTORY)
        get_filename_component(AOCL_UTILS_PATH    "${AOCL_UTILS_LIB_DIR}" DIRECTORY)
        link_directories(${AOCL_UTILS_LIB_DIR})
        set(CMAKE_SKIP_RPATH TRUE)
        set(CMAKE_SKIP_INSTALL_RPATH TRUE)
        set(CMAKE_CXX_FLAGS "${_SAVED_CXX_FLAGS}")
        return()
    endif()

    library_exists("${AOCL_UTILS_LIB}" "${AU_INSTALL_DIR}" AU_INSTALL_LIBDIR AOCL_UTILS_LIB_EXISTS)
    if(AOCL_UTILS_LIB_EXISTS)
        message(STATUS "Aocl-Utils library found at: ${AU_INSTALL_LIBDIR}/${AOCL_UTILS_LIB}")
    else()
        ensure_aocl_utils_built(AU_INSTALL_LIBDIR)
    endif()

elseif(LINUX)
    # LINUX: use user-provided lib+include if set, else sub-build at AU_INSTALL_DIR;
    # verify C++ runtime ABI and rebuild (cloning if needed) on mismatch.
    if(EXISTS "${_USER_AOCL_UTILS_INCLUDE_DIR}" AND EXISTS "${_USER_AOCL_UTILS_LIB}")
        message(STATUS "User-provided aocl-utils: ${_USER_AOCL_UTILS_LIB}")
        check_libcxx("${_USER_AOCL_UTILS_LIB}" AU_HAS_LIBCXX)

        if((AU_HAS_LIBCXX AND NOT AU_EXPECT_LIBCXX) OR
           (NOT AU_HAS_LIBCXX AND AU_EXPECT_LIBCXX))
            message(STATUS "Pre-built utils ABI mismatch: rebuilding aocl-utils...")
            ensure_aocl_utils_built(AU_INSTALL_LIBDIR)
            # fall through to the common "rebuilt" finalization below
        else()
            message(STATUS "Pre-built aocl-utils ABI is compatible")
            set(AOCL_UTILS_INCLUDE_DIR "${_USER_AOCL_UTILS_INCLUDE_DIR}")
            set(AOCL_UTILS_LIB         "${_USER_AOCL_UTILS_LIB}")
            get_filename_component(AOCL_UTILS_LIB_DIR "${AOCL_UTILS_LIB}" DIRECTORY)
            get_filename_component(AOCL_UTILS_PATH    "${AOCL_UTILS_LIB_DIR}" DIRECTORY)
            link_directories(${AOCL_UTILS_LIB_DIR})
            set(CMAKE_SKIP_RPATH TRUE)
            set(CMAKE_SKIP_INSTALL_RPATH TRUE)
            set(CMAKE_CXX_FLAGS "${_SAVED_CXX_FLAGS}")
            return()
        endif()
    else()
        library_exists("${AOCL_UTILS_LIB}" "${AU_INSTALL_DIR}" AU_INSTALL_LIBDIR AOCL_UTILS_LIB_EXISTS)
        if(AOCL_UTILS_LIB_EXISTS)
            message(STATUS "Aocl-Utils library found at: ${AU_INSTALL_LIBDIR}/${AOCL_UTILS_LIB}")
            check_libcxx("${AU_INSTALL_LIBDIR}/${AOCL_UTILS_LIB}" AU_HAS_LIBCXX)
            if((AU_HAS_LIBCXX AND NOT AU_EXPECT_LIBCXX) OR
               (NOT AU_HAS_LIBCXX AND AU_EXPECT_LIBCXX))
                message(STATUS "ABI mismatch: rebuilding aocl-utils...")
                ensure_aocl_utils_built(AU_INSTALL_LIBDIR)
            endif()
        else()
            ensure_aocl_utils_built(AU_INSTALL_LIBDIR)
        endif()
    endif()

else()
    message(FATAL_ERROR
        "Unsupported platform: ${CMAKE_SYSTEM_NAME}. "
        "AOCL-LibM supports Windows and Linux only.")
endif()


# ---------------------------------------------------------------------------
# Finalization for the sub-built library (both platforms)
# ---------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS "${_SAVED_CXX_FLAGS}")

set(AOCL_UTILS_INCLUDE_DIR  "${AU_INSTALL_DIR}/include")
set(AOCL_UTILS_LIB_DIR      "${AU_INSTALL_LIBDIR}")
set(AOCL_UTILS_PATH         "${AU_INSTALL_DIR}")

link_directories(${AOCL_UTILS_LIB_DIR})
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_SKIP_INSTALL_RPATH TRUE)
