..  Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.

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

.. .. doxygenfunction:: amd_vrd8_acos
..   :project: libm
..   (Function not yet documented in Doxygen)


Atan
----

.. doxygenfunction:: amd_vrs16_atanf
  :project: libm

.. doxygenfunction:: amd_vrd8_atan
  :project: libm


Hyperbolic Functions
====================

Tanh
----

.. doxygenfunction:: amd_vrs16_tanhf
  :project: libm

.. .. doxygenfunction:: amd_vrd8_tanh
..   :project: libm
..   (Function not yet documented in Doxygen)


Inverse Hyperbolic Functions
=============================

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

.. .. doxygenfunction:: amd_vrd8_log10
..   :project: libm
..   (Function not yet documented in Doxygen)


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


Special Functions
=================

CdfNorm
-------

See :ref:`special_functions` for ``amd_vrd8_cdfnorm`` documentation.


Nearest Integer Functions
=========================

Round
-----

.. doxygenfunction:: amd_vrs16_roundf
  :project: libm

.. doxygenfunction:: amd_vrd8_round
  :project: libm


Minimum, Maximum, and Difference
=================================

Arithmetic Functions
====================

Linearfrac
----------

See :ref:`arithmetic` for ``amd_vrd8_linearfrac`` documentation.

See :ref:`arithmetic` for ``amd_vrs16_linearfracf`` documentation.
