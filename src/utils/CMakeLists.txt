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
# AOCL-LibM Internal Utils Module
#
# The CPU-detection source is compiled into a dedicated OBJECT library whose
# objects are appended to ${libmobj} and propagated to the parent scope, exactly
# like the other src/ subdirectories. This embeds the CPU-detection code directly
# into both the static and shared libalm, keeping the libraries self-contained
# (matching the SCons build) instead of shipping a separate utils archive.
#

# Source files
set(ALM_UTILS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/alm_cpuid.c
)

# Dedicated object library for the internal utils.
add_library(alm_utils_obj OBJECT ${ALM_UTILS_SOURCES})

# alm_cpuid.c performs CPU feature detection: alm_cpuid_init() is called
# explicitly as the first step of the library init-map constructor
# (init_map_entry_points() in src/entry_pt.c), before libm_iface_init() runs the
# dispatch fixup. Because this code runs to DETECT features, it must execute
# safely on *any* CPU and before any feature-specific dispatch is selected. It
# must therefore be built against a minimal ISA baseline and must NOT inherit the
# above-baseline flags carried by ${LIBMCFLAGS} (e.g. the Windows /arch:AVX2,
# -mavx2/-mfma, AVX-512 or other vectorization flags). The source uses only the
# __cpuid/__cpuidex builtins (no vector intrinsics), so a baseline build is
# correct. We still keep -O3-class optimization, the warning set, PIC and the
# AOCL-Utils define, matching how the standalone AOCL-Utils library builds this
# same code.
set(UTILS_CFLAGS ${optzflag} ${wflags} ${picflag} ${auflag})
if(WIN32)
    # Preserve the Windows base runtime defines (API/ABI/export macros and the
    # -m64 target flag) but drop the above-baseline /arch:AVX2 ISA selector.
    get_win_flag(winflag)
    list(REMOVE_ITEM winflag /arch:AVX2)
    list(APPEND UTILS_CFLAGS ${winflag})
endif()
target_compile_options(alm_utils_obj PRIVATE ${UTILS_CFLAGS})

# CPU detection only needs its own public header (alm_arch.h).
target_include_directories(alm_utils_obj PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(alm_utils_obj PROPERTIES POSITION_INDEPENDENT_CODE ON)

# add alm_utils_obj object library to libmobj
list(APPEND libmobj $<TARGET_OBJECTS:alm_utils_obj>)
set(libmobj ${libmobj} PARENT_SCOPE)
