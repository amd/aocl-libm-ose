#
# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

"""
Common source-file gathering for ISA and arch SCons builds.

Both src/isa/SConscript and src/arch/SConscript compile the same set of
optimized sources with different flags.  This module collects and filters
those sources in one place so the logic is not duplicated.
"""

import os
import re
from os.path import join as joinpath

# Files already compiled by the 'opt' target in src/optimized/SConscript.
# These must be excluded from arch/isa builds to avoid duplicate symbols.
_OPT_TARGET_SRCS = [
    'fdim.c',
    'fdimf.c',
    'nearbyint.c',
    'nearbyintf.c',
    'sqrt.c',
    'sqrtf.c',
    'log1p_fma3.c',
]

_OPT_COMMON_SRCS = [
    'exp_tables.c',
    'log_tables.c',
    'pow_data.c',
    'sincos_tbl.c',
    'sinh_data.c',
    'vec/alm_special.c',
]

# ---------------------------------------------------------------------------
# AVX-512 template-based array wrapper generation
#
# Instead of maintaining individual vrda_*.c / vrsa_*.c files for each math
# function, we use a single template file that is compiled multiple times
# with different -DALM_FUNC=<name> definitions. This approach:
#   - Reduces code duplication (one template instead of ~30 nearly-identical files)
#   - Makes maintenance easier (fix once, apply everywhere)
#   - Keeps behavior consistent across all array wrappers
#
# The template files (vrda_unary_avx512.c, vrsa_unary_avx512.c) use the
# PASTE macro to construct function names from ALM_FUNC at compile time.
# ---------------------------------------------------------------------------

# List of unary double functions for vrda template.
# Each function generates a vrda_<name> wrapper (e.g., vrda_sin, vrda_cos).
# The wrapper calls the corresponding vrd8_<name> 8-wide vector function.
# NOTE: Keep in sync with VRDA_UNARY_AVX512_FUNCS in CMakeLists.txt (CMake).
VRDA_UNARY_AVX512_FUNCS = [
    'sin', 'cos', 'tan', 'exp', 'exp2', 'log', 'log2',
    'asin', 'atan', 'sqrt', 'round',
    'erf', 'erfc', 'erfinv', 'erfcinv',
    'cdfnorm', 'cdfnorminv',
]

# List of unary float functions for vrsa template.
# Each function generates a vrsa_<name> wrapper (e.g., vrsa_sinf, vrsa_cosf).
# The wrapper calls the corresponding vrs16_<name> 16-wide vector function.
# NOTE: Keep in sync with VRSA_UNARY_AVX512_FUNCS in CMakeLists.txt (CMake).
VRSA_UNARY_AVX512_FUNCS = [
    'sinf', 'cosf', 'tanf', 'expf', 'exp2f', 'logf', 'log2f', 'log10f',
    'asinf', 'atanf', 'acosf', 'sqrtf', 'roundf',
    'erff', 'erfcf', 'tanhf',
]

# Template file paths (relative to src/optimized).
# These are compiled separately by src/isa/ and src/arch/ SConscripts.
AVX512_VRDA_TEMPLATE = 'vec/avx512/vrda_unary_avx512.c'
AVX512_VRSA_TEMPLATE = 'vec/avx512/vrsa_unary_avx512.c'


def gather_optimized_srcs(Glob):
    """Return (avx2_srcs, avx512_srcs) from src/optimized."""
    opt_dir = '#src/optimized'

    opt_srcs     = Glob(joinpath(opt_dir, '*.c'))
    cmplx_srcs   = Glob(joinpath(opt_dir, 'cmplx', '*.c'))
    vec_srcs     = Glob(joinpath(opt_dir, 'vec', '*.c'))
    vecmath_srcs = Glob(joinpath(opt_dir, 'vectormath', '*.c'))
    avx512_vec_srcs     = Glob(joinpath(opt_dir, 'vec', 'avx512', '*.c'))
    avx512_vecmath_srcs = Glob(joinpath(opt_dir, 'vectormath', 'avx512', '*.c'))

    _exclude = set(_OPT_TARGET_SRCS +
                   [os.path.basename(f) for f in _OPT_COMMON_SRCS])
    opt_srcs = [f for f in opt_srcs
                if os.path.basename(str(f)) not in _exclude]
    vec_srcs = [f for f in vec_srcs
                if os.path.basename(str(f)) not in _exclude]

    opt_srcs = [f for f in opt_srcs
                if not re.search(r'(log_v3)[^/]*\.c$', str(f))]

    # Exclude template files from the glob — they are compiled separately in the
    # SConscript loops, not as part of the regular source list.
    avx512_vec_srcs = [f for f in avx512_vec_srcs
                       if not re.search(r'vr[ds]a_unary_avx512\.c$', str(f))]

    _avx512_vec_names     = {os.path.basename(str(f)) for f in avx512_vec_srcs}
    _avx512_vecmath_names = {os.path.basename(str(f)) for f in avx512_vecmath_srcs}

    # Also exclude base vrda_*/vrsa_* files that would conflict with template-
    # generated symbols. If vrda_sin.c exists in base vec/, it's filtered out
    # so only the template-compiled version (vrda_unary_avx512.c with -DALM_FUNC=sin)
    # provides the vrda_sin symbol.
    for prefix, func_list in [('vrda', VRDA_UNARY_AVX512_FUNCS), ('vrsa', VRSA_UNARY_AVX512_FUNCS)]:
        for func in func_list:
            _avx512_vec_names.add(f'{prefix}_{func}.c')

    filtered_vec_srcs     = [f for f in vec_srcs
                             if os.path.basename(str(f)) not in _avx512_vec_names]
    filtered_vecmath_srcs = [f for f in vecmath_srcs
                             if os.path.basename(str(f)) not in _avx512_vecmath_names]

    avx2_srcs   = opt_srcs + cmplx_srcs + vec_srcs + vecmath_srcs
    avx512_srcs = (opt_srcs + cmplx_srcs + filtered_vec_srcs + filtered_vecmath_srcs
                   + avx512_vec_srcs + avx512_vecmath_srcs)

    return avx2_srcs, avx512_srcs
