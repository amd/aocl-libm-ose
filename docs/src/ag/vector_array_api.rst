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

.. _vector_array_api:

=============================
Vector Array Functions
=============================

Overview
========

Vector array functions in AOCL-LibM provide a flexible interface for processing 
variable-length arrays of floating-point numbers. Unlike fixed-size vector functions 
(AVX, AVX2, AVX512), array functions can handle arrays of arbitrary length, making 
them ideal for data processing scenarios where the data size is not known at compile time.

These functions are prefixed with ``amd_vrsa_`` for single precision arrays and 
``amd_vrda_`` for double precision arrays.

.. note::
   Vector array functions are currently under development. This section will be 
   expanded as more functions become available.


Trigonometric Functions
=======================

Sin
---

.. doxygenfunction:: amd_vrsa_sinf
  :project: libm

.. doxygenfunction:: amd_vrda_sin
  :project: libm


Cos
---

.. doxygenfunction:: amd_vrsa_cosf
  :project: libm

.. doxygenfunction:: amd_vrda_cos
  :project: libm


Tan
---

.. doxygenfunction:: amd_vrsa_tanf
  :project: libm

.. doxygenfunction:: amd_vrda_tan
  :project: libm


Sincos
------

.. doxygenfunction:: amd_vrsa_sincosf
  :project: libm

.. doxygenfunction:: amd_vrda_sincos
  :project: libm


Inverse Trigonometric Functions
================================

Asin
----

.. doxygenfunction:: amd_vrsa_asinf
  :project: libm

.. doxygenfunction:: amd_vrda_asin
  :project: libm


Acos
----

.. doxygenfunction:: amd_vrsa_acosf
  :project: libm

.. doxygenfunction:: amd_vrda_acos
  :project: libm


Atan
----

.. doxygenfunction:: amd_vrsa_atanf
  :project: libm

.. doxygenfunction:: amd_vrda_atan
  :project: libm


Hyperbolic Functions
====================

Cosh
----

.. doxygenfunction:: amd_vrsa_coshf
  :project: libm

.. doxygenfunction:: amd_vrda_cosh
  :project: libm


Tanh
----

.. doxygenfunction:: amd_vrsa_tanhf
  :project: libm


Exponential Functions
=====================

Exp
---

.. doxygenfunction:: amd_vrsa_expf
  :project: libm

.. doxygenfunction:: amd_vrda_exp
  :project: libm


Exp2
----

.. doxygenfunction:: amd_vrsa_exp2f
  :project: libm

.. doxygenfunction:: amd_vrda_exp2
  :project: libm


Exp10
-----

.. doxygenfunction:: amd_vrsa_exp10f
  :project: libm

.. doxygenfunction:: amd_vrda_exp10
  :project: libm


Expm1
-----

.. doxygenfunction:: amd_vrsa_expm1f
  :project: libm

.. doxygenfunction:: amd_vrda_expm1
  :project: libm


Logarithmic Functions
=====================

Log
---

.. doxygenfunction:: amd_vrsa_logf
  :project: libm

.. doxygenfunction:: amd_vrda_log
  :project: libm


Log2
----

.. doxygenfunction:: amd_vrsa_log2f
  :project: libm

.. doxygenfunction:: amd_vrda_log2
  :project: libm


Log10
-----

.. doxygenfunction:: amd_vrsa_log10f
  :project: libm

.. doxygenfunction:: amd_vrda_log10
  :project: libm


Log1p
-----

.. doxygenfunction:: amd_vrsa_log1pf
  :project: libm

.. doxygenfunction:: amd_vrda_log1p
  :project: libm


Power and Root Functions
=========================

Pow
---

.. doxygenfunction:: amd_vrsa_powf
  :project: libm

.. doxygenfunction:: amd_vrda_pow
  :project: libm


Powx
----

.. doxygenfunction:: amd_vrsa_powxf
  :project: libm

.. doxygenfunction:: amd_vrda_powx
  :project: libm


Sqrt
----

.. doxygenfunction:: amd_vrsa_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrda_sqrt
  :project: libm


Cbrt
----

.. doxygenfunction:: amd_vrsa_cbrtf
  :project: libm

.. doxygenfunction:: amd_vrda_cbrt
  :project: libm


Error Functions
===============

Erf
---

.. doxygenfunction:: amd_vrsa_erff
  :project: libm

.. doxygenfunction:: amd_vrda_erf
  :project: libm


Erfc
----

.. doxygenfunction:: amd_vrsa_erfcf
  :project: libm

.. doxygenfunction:: amd_vrda_erfc
  :project: libm


Special Functions
=================

CdfNorm
-------

See :ref:`special_functions` for ``amd_vrda_cdfnorm`` documentation.


Floating-Point Manipulation Functions
=====================================

Fabs
----

.. doxygenfunction:: amd_vrsa_fabsf
  :project: libm

.. doxygenfunction:: amd_vrda_fabs
  :project: libm


Nearest Integer Functions
=========================

Round
-----

.. doxygenfunction:: amd_vrsa_roundf
  :project: libm

.. doxygenfunction:: amd_vrda_round
  :project: libm


Minimum, Maximum and Difference Functions
=========================================

Fmax
----

.. doxygenfunction:: amd_vrsa_fmaxf
  :project: libm

.. doxygenfunction:: amd_vrda_fmax
  :project: libm


Fmin
----

.. doxygenfunction:: amd_vrsa_fminf
  :project: libm

.. doxygenfunction:: amd_vrda_fmin
  :project: libm


Indexed Maximum & Minimum
-------------------------

Fmaxi
^^^^^

.. doxygenfunction:: amd_vrsa_fmaxfi
  :project: libm

.. doxygenfunction:: amd_vrda_fmaxi
  :project: libm


Fmini
^^^^^

.. doxygenfunction:: amd_vrsa_fminfi
  :project: libm

.. doxygenfunction:: amd_vrda_fmini
  :project: libm


Arithmetic Functions
====================

Linearfrac
----------

See :ref:`arithmetic` for ``amd_vrsa_linearfracf`` documentation.

See :ref:`arithmetic` for ``amd_vrda_linearfrac`` documentation.


Add
---

See :ref:`arithmetic` for ``amd_vrsa_addf`` documentation.

See :ref:`arithmetic` for ``amd_vrda_add`` documentation.


Sub
---

See :ref:`arithmetic` for ``amd_vrsa_subf`` documentation.

See :ref:`arithmetic` for ``amd_vrda_sub`` documentation.


Mul
---

See :ref:`arithmetic` for ``amd_vrsa_mulf`` documentation.

See :ref:`arithmetic` for ``amd_vrda_mul`` documentation.


Div
---

See :ref:`arithmetic` for ``amd_vrsa_divf`` documentation.

See :ref:`arithmetic` for ``amd_vrda_div`` documentation.


Indexed Arithmetic Functions
============================

Addi
----

See :ref:`arithmetic` for ``amd_vrsa_addfi`` documentation.

See :ref:`arithmetic` for ``amd_vrda_addi`` documentation.


Subi
----

See :ref:`arithmetic` for ``amd_vrsa_subfi`` documentation.

See :ref:`arithmetic` for ``amd_vrda_subi`` documentation.


Muli
----

See :ref:`arithmetic` for ``amd_vrsa_mulfi`` documentation.

See :ref:`arithmetic` for ``amd_vrda_muli`` documentation.


Divi
----

See :ref:`arithmetic` for ``amd_vrsa_divfi`` documentation.

See :ref:`arithmetic` for ``amd_vrda_divi`` documentation.


.. End of Doc
