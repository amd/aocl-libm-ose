..  Copyright (C) 2025, Advanced Micro Devices. All rights reserved.

..  Redistribution and use in source and binary forms, with or without
..  modification, are permitted provided that the following conditions are met:

..  1. Redistributions of source code must retain the above copyright notice,
..  this list of conditions and the following disclaimer.
..  2. Redistributions in binary form must reproduce the above copyright notice,
..  this list of conditions and the following disclaimer in the documentation
..  and/or other materials provided with the distribution.
..  3. Neither the name of the copyright holder nor the names of its
..  contributors may be used to endorse or promote products derived from this
..  software without specific prior written permission.

..  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
..  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
..  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
..  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
..  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
..  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
..  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
..  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
..  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
..  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
..  POSSIBILITY OF SUCH DAMAGE.

.. _vector_avx_api:

==============================
Vector AVX Functions (128-bit)
==============================

Overview
========

AVX 128-bit vector functions process multiple values simultaneously using SIMD (Single 
Instruction Multiple Data) instructions. These functions leverage the ``__m128`` (single 
precision) and ``__m128d`` (double precision) data types to perform operations on 
multiple values in parallel.

Trigonometric Functions
=======================

Sin
---

.. doxygenfunction:: amd_vrs4_sinf
  :project: libm

.. doxygenfunction:: amd_vrd2_sin
  :project: libm


Cos
---

.. doxygenfunction:: amd_vrs4_cosf
  :project: libm

.. doxygenfunction:: amd_vrd2_cos
  :project: libm


Tan
---

.. doxygenfunction:: amd_vrs4_tanf
  :project: libm

.. doxygenfunction:: amd_vrd2_tan
  :project: libm


Sincos
------

.. doxygenfunction:: amd_vrs4_sincosf
  :project: libm

.. doxygenfunction:: amd_vrd2_sincos
  :project: libm


Inverse Trigonometric Functions
================================

Asin
----

.. doxygenfunction:: amd_vrs4_asinf
  :project: libm

.. doxygenfunction:: amd_vrd2_asin
  :project: libm


Acos
----

.. doxygenfunction:: amd_vrs4_acosf
  :project: libm

.. doxygenfunction:: amd_vrd2_acos
  :project: libm


Atan
----

.. doxygenfunction:: amd_vrs4_atanf
  :project: libm

.. doxygenfunction:: amd_vrd2_atan
  :project: libm


Atan2
-----

.. doxygenfunction:: amd_vrs4_atan2f
  :project: libm

.. doxygenfunction:: amd_vrd2_atan2
  :project: libm


Hyperbolic Functions
====================

Sinh
----

.. doxygenfunction:: amd_vrs4_sinhf
  :project: libm

.. doxygenfunction:: amd_vrd2_sinh
  :project: libm


Cosh
----

.. doxygenfunction:: amd_vrs4_coshf
  :project: libm

.. doxygenfunction:: amd_vrd2_cosh
  :project: libm


Tanh
----

.. doxygenfunction:: amd_vrs4_tanhf
  :project: libm

.. doxygenfunction:: amd_vrd2_tanh
  :project: libm


Inverse Hyperbolic Functions
=============================

Asinh
-----

.. doxygenfunction:: amd_vrs4_asinhf
  :project: libm

.. doxygenfunction:: amd_vrd2_asinh
  :project: libm


Acosh
-----

.. doxygenfunction:: amd_vrs4_acoshf
  :project: libm

.. doxygenfunction:: amd_vrd2_acosh
  :project: libm


Atanh
-----

.. doxygenfunction:: amd_vrs4_atanhf
  :project: libm

.. doxygenfunction:: amd_vrd2_atanh
  :project: libm


Exponential Functions
=====================

Exp
---

.. doxygenfunction:: amd_vrs4_expf
  :project: libm

.. doxygenfunction:: amd_vrd2_exp
  :project: libm


Exp2
----

.. doxygenfunction:: amd_vrs4_exp2f
  :project: libm

.. doxygenfunction:: amd_vrd2_exp2
  :project: libm


Exp10
-----

.. doxygenfunction:: amd_vrs4_exp10f
  :project: libm

.. doxygenfunction:: amd_vrd2_exp10
  :project: libm


Expm1
-----

.. doxygenfunction:: amd_vrs4_expm1f
  :project: libm

.. doxygenfunction:: amd_vrd2_expm1
  :project: libm


Logarithmic Functions
=====================

Log
---

.. doxygenfunction:: amd_vrs4_logf
  :project: libm

.. doxygenfunction:: amd_vrd2_log
  :project: libm


Log2
----

.. doxygenfunction:: amd_vrs4_log2f
  :project: libm

.. doxygenfunction:: amd_vrd2_log2
  :project: libm


Log10
-----

.. doxygenfunction:: amd_vrs4_log10f
  :project: libm

.. doxygenfunction:: amd_vrd2_log10
  :project: libm


Log1p
-----

.. doxygenfunction:: amd_vrs4_log1pf
  :project: libm

.. doxygenfunction:: amd_vrd2_log1p
  :project: libm


Power and Root Functions
=========================

Pow
---

.. doxygenfunction:: amd_vrs4_powf
  :project: libm

.. doxygenfunction:: amd_vrd2_pow
  :project: libm


Powx
----

.. doxygenfunction:: amd_vrs4_powxf
  :project: libm

.. doxygenfunction:: amd_vrd2_powx
  :project: libm


Sqrt
----

.. doxygenfunction:: amd_vrs4_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrd2_sqrt
  :project: libm


Cbrt
----

.. doxygenfunction:: amd_vrs4_cbrtf
  :project: libm

.. doxygenfunction:: amd_vrd2_cbrt
  :project: libm


Hypot
-----

.. doxygenfunction:: amd_vrs4_hypotf
  :project: libm

.. doxygenfunction:: amd_vrd2_hypot
  :project: libm


Error Functions
===============

Erf
---

.. doxygenfunction:: amd_vrs4_erff
  :project: libm

.. doxygenfunction:: amd_vrd2_erf
  :project: libm


Erfc
----

.. doxygenfunction:: amd_vrs4_erfcf
  :project: libm

.. doxygenfunction:: amd_vrd2_erfc
  :project: libm


Nearest Integer Functions
==========================

Ceil
----

.. doxygenfunction:: amd_vrs4_ceilf
  :project: libm

.. doxygenfunction:: amd_vrd2_ceil
  :project: libm


Floor
-----

.. doxygenfunction:: amd_vrs4_floorf
  :project: libm

.. doxygenfunction:: amd_vrd2_floor
  :project: libm


Trunc
-----

.. doxygenfunction:: amd_vrs4_truncf
  :project: libm

.. doxygenfunction:: amd_vrd2_trunc
  :project: libm


Round
-----

.. doxygenfunction:: amd_vrs4_roundf
  :project: libm

.. doxygenfunction:: amd_vrd2_round
  :project: libm


Rint
----

.. doxygenfunction:: amd_vrs4_rintf
  :project: libm

.. doxygenfunction:: amd_vrd2_rint
  :project: libm


Nearbyint
---------

.. doxygenfunction:: amd_vrs4_nearbyintf
  :project: libm

.. doxygenfunction:: amd_vrd2_nearbyint
  :project: libm


Minimum, Maximum, and Difference
=================================

Fmin
----

.. doxygenfunction:: amd_vrs4_fminf
  :project: libm

.. doxygenfunction:: amd_vrd2_fmin
  :project: libm


Fmax
----

.. doxygenfunction:: amd_vrs4_fmaxf
  :project: libm

.. doxygenfunction:: amd_vrd2_fmax
  :project: libm


Fdim
----

.. doxygenfunction:: amd_vrs4_fdimf
  :project: libm

.. doxygenfunction:: amd_vrd2_fdim
  :project: libm


Fabs
----

.. doxygenfunction:: amd_vrs4_fabsf
  :project: libm

.. doxygenfunction:: amd_vrd2_fabs
  :project: libm


Copysign
--------

.. doxygenfunction:: amd_vrs4_copysignf
  :project: libm

.. doxygenfunction:: amd_vrd2_copysign
  :project: libm


Arithmetic Functions
====================

Fma
---

.. doxygenfunction:: amd_vrs4_fmaf
  :project: libm

.. doxygenfunction:: amd_vrd2_fma
  :project: libm


Fmod
----

.. doxygenfunction:: amd_vrs4_fmodf
  :project: libm

.. doxygenfunction:: amd_vrd2_fmod
  :project: libm


Remainder
---------

.. doxygenfunction:: amd_vrs4_remainderf
  :project: libm

.. doxygenfunction:: amd_vrd2_remainder
  :project: libm

.. End of Doc