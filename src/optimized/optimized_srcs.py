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
]

_OPT_COMMON_SRCS = [
    'exp_tables.c',
    'log_tables.c',
    'pow_data.c',
    'sinh_data.c',
    'vec/alm_special.c',
]


def gather_optimized_srcs(Glob):
    """Return (avx2_srcs, avx512_srcs) from src/optimized.

    Parameters
    ----------
    Glob : callable
        The SCons Glob function (from the calling SConscript environment).

    Returns
    -------
    tuple
        (avx2_srcs, avx512_srcs) — two lists of SCons File nodes.
    """
    opt_dir = '#src/optimized'

    opt_srcs     = Glob(joinpath(opt_dir, '*.c'))
    cmplx_srcs   = Glob(joinpath(opt_dir, 'cmplx', '*.c'))
    vec_srcs     = Glob(joinpath(opt_dir, 'vec', '*.c'))
    vecmath_srcs = Glob(joinpath(opt_dir, 'vectormath', '*.c'))
    avx512_vec_srcs     = Glob(joinpath(opt_dir, 'vec', 'avx512', '*.c'))
    avx512_vecmath_srcs = Glob(joinpath(opt_dir, 'vectormath', 'avx512', '*.c'))

    # Exclude files already compiled by the opt target (mirrors OPTSRC in CMake).
    # Use basenames for comparison since _OPT_COMMON_SRCS may contain paths
    # with directory prefixes (e.g. 'vec/alm_special.c').
    _exclude = set(_OPT_TARGET_SRCS +
                   [os.path.basename(f) for f in _OPT_COMMON_SRCS])
    opt_srcs = [f for f in opt_srcs
                if os.path.basename(str(f)) not in _exclude]
    vec_srcs = [f for f in vec_srcs
                if os.path.basename(str(f)) not in _exclude]

    # Files without arch/ISA-suffixed prototypes (no FN_PROTOTYPE_OPT)
    opt_srcs = [f for f in opt_srcs
                if not re.search(r'(log_v3)[^/]*\.c$', str(f))]

    # Build blacklist from avx512 globs — filenames present in vec/avx512/ or
    # vectormath/avx512/ supersede their base counterparts in the AVX512 build.
    _avx512_vec_names     = {os.path.basename(str(f)) for f in avx512_vec_srcs}
    _avx512_vecmath_names = {os.path.basename(str(f)) for f in avx512_vecmath_srcs}

    filtered_vec_srcs     = [f for f in vec_srcs
                             if os.path.basename(str(f)) not in _avx512_vec_names]
    filtered_vecmath_srcs = [f for f in vecmath_srcs
                             if os.path.basename(str(f)) not in _avx512_vecmath_names]

    avx2_srcs   = opt_srcs + cmplx_srcs + vec_srcs + vecmath_srcs
    avx512_srcs = (opt_srcs + cmplx_srcs + filtered_vec_srcs + filtered_vecmath_srcs
                   + avx512_vec_srcs + avx512_vecmath_srcs)

    return avx2_srcs, avx512_srcs
