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

.. _vector_avx2_api:

================================
Vector AVX2 Functions (256-bit)
================================

Overview
========

AVX2 256-bit vector functions provide enhanced SIMD capabilities, doubling the throughput 
of AVX 128-bit operations. These functions leverage the ``__m256`` (single precision) and 
``__m256d`` (double precision) data types to perform operations on larger batches of values 
in parallel.

Trigonometric Functions
=======================

Sin
---

.. doxygenfunction:: amd_vrs8_sinf
  :project: libm

.. doxygenfunction:: amd_vrd4_sin
  :project: libm


Cos
---

.. doxygenfunction:: amd_vrs8_cosf
  :project: libm

.. doxygenfunction:: amd_vrd4_cos
  :project: libm


Tan
---

.. doxygenfunction:: amd_vrs8_tanf
  :project: libm

.. doxygenfunction:: amd_vrd4_tan
  :project: libm


Sincos
------

.. doxygenfunction:: amd_vrs8_sincosf
  :project: libm

.. doxygenfunction:: amd_vrd4_sincos
  :project: libm


Inverse Trigonometric Functions
================================

Asin
----

.. doxygenfunction:: amd_vrs8_asinf
  :project: libm

.. doxygenfunction:: amd_vrd4_asin
  :project: libm


Acos
----

.. doxygenfunction:: amd_vrs8_acosf
  :project: libm

.. doxygenfunction:: amd_vrd4_acos
  :project: libm


Atan
----

.. doxygenfunction:: amd_vrs8_atanf
  :project: libm

.. doxygenfunction:: amd_vrd4_atan
  :project: libm


Hyperbolic Functions
====================

Cosh
----

.. doxygenfunction:: amd_vrs8_coshf
  :project: libm

.. .. doxygenfunction:: amd_vrd4_cosh
..   :project: libm
..   (Function not yet documented in Doxygen)


Tanh
----

.. doxygenfunction:: amd_vrs8_tanhf
  :project: libm

.. .. doxygenfunction:: amd_vrd4_tanh
..   :project: libm
..   (Function not yet documented in Doxygen)


Inverse Hyperbolic Functions
=============================

Exponential Functions
=====================

Exp
---

.. doxygenfunction:: amd_vrs8_expf
  :project: libm

.. doxygenfunction:: amd_vrd4_exp
  :project: libm


Exp2
----

.. doxygenfunction:: amd_vrs8_exp2f
  :project: libm

.. doxygenfunction:: amd_vrd4_exp2
  :project: libm


Logarithmic Functions
=====================

Log
---

.. doxygenfunction:: amd_vrs8_logf
  :project: libm

.. doxygenfunction:: amd_vrd4_log
  :project: libm


Log2
----

.. doxygenfunction:: amd_vrs8_log2f
  :project: libm

.. doxygenfunction:: amd_vrd4_log2
  :project: libm


Log10
-----

.. doxygenfunction:: amd_vrs8_log10f
  :project: libm

.. .. doxygenfunction:: amd_vrd4_log10
..   :project: libm
..   (Function not yet documented in Doxygen)


Power and Root Functions
=========================

Pow
---

.. doxygenfunction:: amd_vrs8_powf
  :project: libm

.. doxygenfunction:: amd_vrd4_pow
  :project: libm


Powx
----

.. doxygenfunction:: amd_vrs8_powxf
  :project: libm

.. doxygenfunction:: amd_vrd4_powx
  :project: libm


Sqrt
----

.. doxygenfunction:: amd_vrs8_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrd4_sqrt
  :project: libm


Error Functions
===============

Erf
---

.. doxygenfunction:: amd_vrs8_erff
  :project: libm

.. doxygenfunction:: amd_vrd4_erf
  :project: libm


Erfc
----

.. doxygenfunction:: amd_vrs8_erfcf
  :project: libm

.. doxygenfunction:: amd_vrd4_erfc
  :project: libm


Erfinv
------

.. doxygenfunction:: amd_vrd4_erfinv
  :project: libm


Erfcinv
-------

.. doxygenfunction:: amd_vrd4_erfcinv
  :project: libm


Special Functions
=================

CdfNorm
-------

See :ref:`special_functions` for ``amd_vrd4_cdfnorm`` documentation.


CdfNormInv
----------

See :ref:`special_functions` for ``amd_vrd4_cdfnorminv`` documentation.


Nearest Integer Functions
=========================

Round
-----

.. doxygenfunction:: amd_vrs8_roundf
  :project: libm

.. doxygenfunction:: amd_vrd4_round
  :project: libm


Minimum, Maximum, and Difference
=================================

Fabs
----

.. doxygenfunction:: amd_vrs8_fabsf
  :project: libm

.. doxygenfunction:: amd_vrd4_fabs
  :project: libm


Arithmetic Functions
====================

Linearfrac
----------

See :ref:`arithmetic` for ``amd_vrd4_linearfrac`` documentation.

See :ref:`arithmetic` for ``amd_vrs8_linearfracf`` documentation.
