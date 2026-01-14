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

.. _scalar_api:

================
Scalar Functions
================

Overview
========

Scalar functions in AOCL-LibM are prefixed with ``amd_`` and operate on single values at a time.


Trigonometric Functions
=======================

Sin
---

.. doxygenfunction:: amd_sinf
  :project: libm

.. doxygenfunction:: amd_sin
  :project: libm


Cos
---

.. doxygenfunction:: amd_cosf
  :project: libm

.. doxygenfunction:: amd_cos
  :project: libm


Tan
---

.. doxygenfunction:: amd_tanf
  :project: libm

.. doxygenfunction:: amd_tan
  :project: libm


Sincos
------

.. doxygenfunction:: amd_sincosf
  :project: libm

.. doxygenfunction:: amd_sincos
  :project: libm


Inverse Trigonometric Functions
================================

Asin
----

.. doxygenfunction:: amd_asinf
  :project: libm

.. doxygenfunction:: amd_asin
  :project: libm


Acos
----

.. doxygenfunction:: amd_acosf
  :project: libm

.. doxygenfunction:: amd_acos
  :project: libm


Atan
----

.. doxygenfunction:: amd_atanf
  :project: libm

.. doxygenfunction:: amd_atan
  :project: libm


Atan2
-----

.. doxygenfunction:: amd_atan2f
  :project: libm

.. doxygenfunction:: amd_atan2
  :project: libm


Hyperbolic Functions
====================

Sinh
----

.. doxygenfunction:: amd_sinhf
  :project: libm

.. doxygenfunction:: amd_sinh
  :project: libm


Cosh
----

.. doxygenfunction:: amd_coshf
  :project: libm

.. doxygenfunction:: amd_cosh
  :project: libm


Tanh
----

.. doxygenfunction:: amd_tanhf
  :project: libm

.. doxygenfunction:: amd_tanh
  :project: libm


Inverse Hyperbolic Functions
=============================

Asinh
-----

.. doxygenfunction:: amd_asinhf
  :project: libm

.. doxygenfunction:: amd_asinh
  :project: libm


Acosh
-----

.. doxygenfunction:: amd_acoshf
  :project: libm

.. doxygenfunction:: amd_acosh
  :project: libm


Atanh
-----

.. doxygenfunction:: amd_atanhf
  :project: libm

.. doxygenfunction:: amd_atanh
  :project: libm


Exponential Functions
=====================

Exp
---

.. doxygenfunction:: amd_expf
  :project: libm

.. doxygenfunction:: amd_exp
  :project: libm


Exp2
----

.. doxygenfunction:: amd_exp2f
  :project: libm

.. doxygenfunction:: amd_exp2
  :project: libm


Exp10
-----

.. doxygenfunction:: amd_exp10f
  :project: libm

.. doxygenfunction:: amd_exp10
  :project: libm


Expm1
-----

.. doxygenfunction:: amd_expm1f
  :project: libm

.. doxygenfunction:: amd_expm1
  :project: libm


Logarithmic Functions
=====================

Log
---

.. doxygenfunction:: amd_logf
  :project: libm

.. doxygenfunction:: amd_log
  :project: libm


Log2
----

.. doxygenfunction:: amd_log2f
  :project: libm

.. doxygenfunction:: amd_log2
  :project: libm


Log10
-----

.. doxygenfunction:: amd_log10f
  :project: libm

.. doxygenfunction:: amd_log10
  :project: libm


Log1p
-----

.. doxygenfunction:: amd_log1pf
  :project: libm

.. doxygenfunction:: amd_log1p
  :project: libm


Power and Root Functions
=========================

Pow
---

.. doxygenfunction:: amd_powf
  :project: libm

.. doxygenfunction:: amd_pow
  :project: libm


Sqrt
----

.. doxygenfunction:: amd_sqrtf
  :project: libm

.. doxygenfunction:: amd_sqrt
  :project: libm


Cbrt
----

.. doxygenfunction:: amd_cbrtf
  :project: libm

.. doxygenfunction:: amd_cbrt
  :project: libm


Hypot
-----

.. doxygenfunction:: amd_hypotf
  :project: libm

.. doxygenfunction:: amd_hypot
  :project: libm


Error Functions
===============

Erf
---

.. doxygenfunction:: amd_erff
  :project: libm

.. doxygenfunction:: amd_erf
  :project: libm


Erfc
----

.. doxygenfunction:: amd_erfcf
  :project: libm

.. doxygenfunction:: amd_erfc
  :project: libm


Special Functions
=================

CdfNorm
-------

See :ref:`special_functions` for ``amd_cdfnorm`` documentation.


Remainder and Quotient Functions
=================================

Remainder
---------

.. doxygenfunction:: amd_remainderf
  :project: libm

.. doxygenfunction:: amd_remainder
  :project: libm


Remquo
------

.. doxygenfunction:: amd_remquof
  :project: libm

.. doxygenfunction:: amd_remquo
  :project: libm


Fmod
----

.. doxygenfunction:: amd_fmodf
  :project: libm

.. doxygenfunction:: amd_fmod
  :project: libm


Modf
----

.. doxygenfunction:: amd_modff
  :project: libm

.. doxygenfunction:: amd_modf
  :project: libm


Nearest Integer Functions
==========================

Ceil
----

.. doxygenfunction:: amd_ceilf
  :project: libm

.. doxygenfunction:: amd_ceil
  :project: libm


Floor
-----

.. doxygenfunction:: amd_floorf
  :project: libm

.. doxygenfunction:: amd_floor
  :project: libm


Trunc
-----

.. doxygenfunction:: amd_truncf
  :project: libm

.. doxygenfunction:: amd_trunc
  :project: libm


Round
-----

.. doxygenfunction:: amd_roundf
  :project: libm

.. doxygenfunction:: amd_round
  :project: libm


Rint
----

.. doxygenfunction:: amd_rintf
  :project: libm

.. doxygenfunction:: amd_rint
  :project: libm


Nearbyint
---------

.. doxygenfunction:: amd_nearbyintf
  :project: libm

.. doxygenfunction:: amd_nearbyint
  :project: libm


Floating-Point Manipulation Functions
======================================

Frexp
-----

.. doxygenfunction:: amd_frexpf
  :project: libm

.. doxygenfunction:: amd_frexp
  :project: libm


Ldexp
-----

.. doxygenfunction:: amd_ldexpf
  :project: libm

.. doxygenfunction:: amd_ldexp
  :project: libm


Scalbn
------

.. doxygenfunction:: amd_scalbnf
  :project: libm

.. doxygenfunction:: amd_scalbn
  :project: libm


Scalbln
-------

.. doxygenfunction:: amd_scalblnf
  :project: libm

.. doxygenfunction:: amd_scalbln
  :project: libm


Logb
----

.. doxygenfunction:: amd_logbf
  :project: libm

.. doxygenfunction:: amd_logb
  :project: libm


Ilogb
-----

.. doxygenfunction:: amd_ilogbf
  :project: libm

.. doxygenfunction:: amd_ilogb
  :project: libm


Nextafter
---------

.. doxygenfunction:: amd_nextafterf
  :project: libm

.. doxygenfunction:: amd_nextafter
  :project: libm


Copysign
--------

.. doxygenfunction:: amd_copysignf
  :project: libm

.. doxygenfunction:: amd_copysign
  :project: libm


Nan
---

.. doxygenfunction:: amd_nanf
  :project: libm

.. doxygenfunction:: amd_nan
  :project: libm


Arithmetic Functions
====================

Minimum, Maximum, and Difference
=================================

Fmin
----

.. doxygenfunction:: amd_fminf
  :project: libm

.. doxygenfunction:: amd_fmin
  :project: libm


Fmax
----

.. doxygenfunction:: amd_fmaxf
  :project: libm

.. doxygenfunction:: amd_fmax
  :project: libm


Fdim
----

.. doxygenfunction:: amd_fdimf
  :project: libm

.. doxygenfunction:: amd_fdim
  :project: libm


Fabs
----

.. doxygenfunction:: amd_fabsf
  :project: libm

.. doxygenfunction:: amd_fabs
  :project: libm


Complex Functions
=================

For complex number functions, please refer to the Complex API section in the 
"By Mathematical Domain" categorization.


.. End of Doc