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

.. _vector_avx512_api:

==================================
Vector AVX512 Functions (512-bit)
==================================

Overview
========

AVX512 512-bit vector functions provide the highest SIMD throughput, processing up to 16 
single-precision or 8 double-precision values in a single operation. These functions 
leverage the ``__m512`` (single precision) and ``__m512d`` (double precision) data types 
for maximum parallelism.


Trigonometric Functions
=======================

Sin
---

.. doxygenfunction:: amd_vrs16_sinf
  :project: libm

.. doxygenfunction:: amd_vrd8_sin
  :project: libm


Cos
---

.. doxygenfunction:: amd_vrs16_cosf
  :project: libm

.. doxygenfunction:: amd_vrd8_cos
  :project: libm


Tan
---

.. doxygenfunction:: amd_vrs16_tanf
  :project: libm

.. doxygenfunction:: amd_vrd8_tan
  :project: libm


Sincos
------

.. doxygenfunction:: amd_vrs16_sincosf
  :project: libm

.. doxygenfunction:: amd_vrd8_sincos
  :project: libm


Inverse Trigonometric Functions
================================

Asin
----

.. doxygenfunction:: amd_vrs16_asinf
  :project: libm

.. doxygenfunction:: amd_vrd8_asin
  :project: libm


Acos
----

.. doxygenfunction:: amd_vrs16_acosf
  :project: libm

.. doxygenfunction:: amd_vrd8_acos
  :project: libm


Atan
----

.. doxygenfunction:: amd_vrs16_atanf
  :project: libm

.. doxygenfunction:: amd_vrd8_atan
  :project: libm


Atan2
-----

.. doxygenfunction:: amd_vrs16_atan2f
  :project: libm

.. doxygenfunction:: amd_vrd8_atan2
  :project: libm


Hyperbolic Functions
====================

Sinh
----

.. doxygenfunction:: amd_vrs16_sinhf
  :project: libm

.. doxygenfunction:: amd_vrd8_sinh
  :project: libm


Cosh
----

.. doxygenfunction:: amd_vrs16_coshf
  :project: libm

.. doxygenfunction:: amd_vrd8_cosh
  :project: libm


Tanh
----

.. doxygenfunction:: amd_vrs16_tanhf
  :project: libm

.. doxygenfunction:: amd_vrd8_tanh
  :project: libm


Inverse Hyperbolic Functions
=============================

Asinh
-----

.. doxygenfunction:: amd_vrs16_asinhf
  :project: libm

.. doxygenfunction:: amd_vrd8_asinh
  :project: libm


Acosh
-----

.. doxygenfunction:: amd_vrs16_acoshf
  :project: libm

.. doxygenfunction:: amd_vrd8_acosh
  :project: libm


Atanh
-----

.. doxygenfunction:: amd_vrs16_atanhf
  :project: libm

.. doxygenfunction:: amd_vrd8_atanh
  :project: libm


Exponential Functions
=====================

Exp
---

.. doxygenfunction:: amd_vrs16_expf
  :project: libm

.. doxygenfunction:: amd_vrd8_exp
  :project: libm


Exp2
----

.. doxygenfunction:: amd_vrs16_exp2f
  :project: libm

.. doxygenfunction:: amd_vrd8_exp2
  :project: libm


Exp10
-----

.. doxygenfunction:: amd_vrs16_exp10f
  :project: libm

.. doxygenfunction:: amd_vrd8_exp10
  :project: libm


Expm1
-----

.. doxygenfunction:: amd_vrs16_expm1f
  :project: libm

.. doxygenfunction:: amd_vrd8_expm1
  :project: libm


Logarithmic Functions
=====================

Log
---

.. doxygenfunction:: amd_vrs16_logf
  :project: libm

.. doxygenfunction:: amd_vrd8_log
  :project: libm


Log2
----

.. doxygenfunction:: amd_vrs16_log2f
  :project: libm

.. doxygenfunction:: amd_vrd8_log2
  :project: libm


Log10
-----

.. doxygenfunction:: amd_vrs16_log10f
  :project: libm

.. doxygenfunction:: amd_vrd8_log10
  :project: libm


Log1p
-----

.. doxygenfunction:: amd_vrs16_log1pf
  :project: libm

.. doxygenfunction:: amd_vrd8_log1p
  :project: libm


Power and Root Functions
=========================

Pow
---

.. doxygenfunction:: amd_vrs16_powf
  :project: libm

.. doxygenfunction:: amd_vrd8_pow
  :project: libm


Powx
----

.. doxygenfunction:: amd_vrs16_powxf
  :project: libm

.. doxygenfunction:: amd_vrd8_powx
  :project: libm


Sqrt
----

.. doxygenfunction:: amd_vrs16_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrd8_sqrt
  :project: libm


Cbrt
----

.. doxygenfunction:: amd_vrs16_cbrtf
  :project: libm

.. doxygenfunction:: amd_vrd8_cbrt
  :project: libm


Hypot
-----

.. doxygenfunction:: amd_vrs16_hypotf
  :project: libm

.. doxygenfunction:: amd_vrd8_hypot
  :project: libm


Error Functions
===============

Erf
---

.. doxygenfunction:: amd_vrs16_erff
  :project: libm

.. doxygenfunction:: amd_vrd8_erf
  :project: libm


Erfc
----

.. doxygenfunction:: amd_vrs16_erfcf
  :project: libm

.. doxygenfunction:: amd_vrd8_erfc
  :project: libm


Nearest Integer Functions
==========================

Ceil
----

.. doxygenfunction:: amd_vrs16_ceilf
  :project: libm

.. doxygenfunction:: amd_vrd8_ceil
  :project: libm


Floor
-----

.. doxygenfunction:: amd_vrs16_floorf
  :project: libm

.. doxygenfunction:: amd_vrd8_floor
  :project: libm


Trunc
-----

.. doxygenfunction:: amd_vrs16_truncf
  :project: libm

.. doxygenfunction:: amd_vrd8_trunc
  :project: libm


Round
-----

.. doxygenfunction:: amd_vrs16_roundf
  :project: libm

.. doxygenfunction:: amd_vrd8_round
  :project: libm


Rint
----

.. doxygenfunction:: amd_vrs16_rintf
  :project: libm

.. doxygenfunction:: amd_vrd8_rint
  :project: libm


Nearbyint
---------

.. doxygenfunction:: amd_vrs16_nearbyintf
  :project: libm

.. doxygenfunction:: amd_vrd8_nearbyint
  :project: libm


Minimum, Maximum, and Difference
=================================

Fmin
----

.. doxygenfunction:: amd_vrs16_fminf
  :project: libm

.. doxygenfunction:: amd_vrd8_fmin
  :project: libm


Fmax
----

.. doxygenfunction:: amd_vrs16_fmaxf
  :project: libm

.. doxygenfunction:: amd_vrd8_fmax
  :project: libm


Fdim
----

.. doxygenfunction:: amd_vrs16_fdimf
  :project: libm

.. doxygenfunction:: amd_vrd8_fdim
  :project: libm


Fabs
----

.. doxygenfunction:: amd_vrs16_fabsf
  :project: libm

.. doxygenfunction:: amd_vrd8_fabs
  :project: libm


Copysign
--------

.. doxygenfunction:: amd_vrs16_copysignf
  :project: libm

.. doxygenfunction:: amd_vrd8_copysign
  :project: libm


Arithmetic Functions
====================

Fma
---

.. doxygenfunction:: amd_vrs16_fmaf
  :project: libm

.. doxygenfunction:: amd_vrd8_fma
  :project: libm


Fmod
----

.. doxygenfunction:: amd_vrs16_fmodf
  :project: libm

.. doxygenfunction:: amd_vrd8_fmod
  :project: libm


Remainder
---------

.. doxygenfunction:: amd_vrs16_remainderf
  :project: libm

.. doxygenfunction:: amd_vrd8_remainder
  :project: libm

.. End of Doc