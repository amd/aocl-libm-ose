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

.. _fast_scalar_api:

======================
Fast Scalar Functions
======================

Overview
========

Fast scalar functions are performance-optimized implementations that trade a small amount 
of accuracy for significantly improved execution speed. These functions are prefixed with 
``amd_fast`` and are designed for performance-critical applications where input ranges are 
known and controlled.

Characteristics
===============

**Performance**

- 2-3x faster execution compared to standard scalar functions
- Optimized for hot code paths and performance-critical sections
- Minimal overhead and streamlined implementation

**Accuracy**

- Typically within 2-3 ULP (Units in the Last Place)
- Sufficient accuracy for most engineering and scientific applications
- Trade-off between precision and speed

**Limitations**

- Does NOT handle special cases (NaN, Inf)
- Not IEEE 754 compliant
- Assumes valid, finite input values within normal ranges
- Undefined behavior for exceptional inputs

**Precision**

- Single precision (``float`` / 32-bit): Functions ending with ``f``
- Double precision (``double`` / 64-bit): Functions without the ``f`` suffix

Important Safety Guidelines
============================

.. warning::
   **Input Validation Required**
   
   Fast scalar functions do not validate inputs or handle special cases. Using these
   functions with NaN, Inf, or out-of-range inputs may produce undefined results or
   incorrect values. Always validate inputs before calling fast variants.


Trigonometric Functions
=======================

Sin
---

**amd_fastsinf**

.. code-block:: c

   float amd_fastsinf(float x)

Computes the sine of ``x`` (measured in radians).

- **Maximum ULP:** 2.22
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastsin**

.. code-block:: c

   double amd_fastsin(double x)

Computes the sine of ``x`` (measured in radians).

- **Maximum ULP:** 2.32
- **Note:** Does not handle special cases like NaNs or INFs


Cos
---

**amd_fastcosf**

.. code-block:: c

   float amd_fastcosf(float x)

Computes the cosine of ``x`` (measured in radians).

- **Maximum ULP:** 2.20
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastcos**

.. code-block:: c

   double amd_fastcos(double x)

Computes the cosine of ``x`` (measured in radians).

- **Maximum ULP:** 2.34
- **Note:** Does not handle special cases like NaNs or INFs


Tan
---

**amd_fasttanf**

.. code-block:: c

   float amd_fasttanf(float x)

Computes the tangent of ``x`` (measured in radians).

- **Maximum ULP:** 2.27
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fasttan**

.. code-block:: c

   double amd_fasttan(double x)

Computes the tangent of ``x`` (measured in radians).

- **Maximum ULP:** 2.67
- **Note:** Does not handle special cases like NaNs or INFs


Inverse Trigonometric Functions
================================

Asin
----

**amd_fastasinf**

.. code-block:: c

   float amd_fastasinf(float x)

Computes the principal value of the arc sine of ``x``, expressed in radians.

- **Maximum ULP:** 2.20
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastasin**

.. code-block:: c

   double amd_fastasin(double x)

Computes the principal value of the arc sine of ``x``, expressed in radians.

- **Maximum ULP:** 2.26
- **Note:** Does not handle special cases like NaNs or INFs


Acos
----

**amd_fastacosf**

.. code-block:: c

   float amd_fastacosf(float x)

Computes the principal value of the arc cosine of ``x``, expressed in radians.

- **Maximum ULP:** 2.21
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastacos**

.. code-block:: c

   double amd_fastacos(double x)

Computes the principal value of the arc cosine of ``x``, expressed in radians.

- **Maximum ULP:** 2.27
- **Note:** Does not handle special cases like NaNs or INFs


Atan
----

**amd_fastatanf**

.. code-block:: c

   float amd_fastatanf(float x)

Computes the principal value of the arc tangent of ``x``, expressed in radians.

- **Maximum ULP:** 2.16
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastatan**

.. code-block:: c

   double amd_fastatan(double x)

Computes the principal value of the arc tangent of ``x``, expressed in radians.

- **Maximum ULP:** 2.18
- **Note:** Does not handle special cases like NaNs or INFs


Exponential Functions
=====================

Exp
---

**amd_fastexpf**

.. code-block:: c

   float amd_fastexpf(float x)

Computes the base-e exponential of ``x`` (e^x).

- **Maximum ULP:** 1.98
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastexp**

.. code-block:: c

   double amd_fastexp(double x)

Computes the base-e exponential of ``x`` (e^x).

- **Maximum ULP:** 1.95
- **Note:** Does not handle special cases like NaNs or INFs


Logarithmic Functions
=====================

Log
---

**amd_fastlogf**

.. code-block:: c

   float amd_fastlogf(float x)

Computes the natural (base-e) logarithm of ``x`` (ln(x)).

- **Maximum ULP:** 1.97
- **Note:** Does not handle special cases like NaNs or INFs

**amd_fastlog**

.. code-block:: c

   double amd_fastlog(double x)

Computes the natural (base-e) logarithm of ``x`` (ln(x)).

- **Maximum ULP:** 1.93
- **Note:** Does not handle special cases like NaNs or INFs


Power Functions
===============

Pow
---

**amd_fastpowf**

.. code-block:: c

   float amd_fastpowf(float x, float y)

Computes ``x`` raised to the power ``y`` (x^y).

- **Maximum ULP:** 1.99
- **Note:** Does not handle special cases like NaNs or INFs
- **Implementation:** Uses the identity x^y = e^(y*ln(x)) for general cases, with optimizations for special values like y=0.5 (square root), x=0, and y=0

**amd_fastpow**

.. code-block:: c

   double amd_fastpow(double x, double y)

Computes ``x`` raised to the power ``y`` (x^y).

- **Maximum ULP:** 1.99
- **Note:** Does not handle special cases like NaNs or INFs
- **Implementation:** Uses the identity x^y = e^(y*ln(x)) for general cases, with optimizations for special values like y=0.5 (square root), x=0, and y=0


Error Functions
===============

Erf
---

**amd_fasterff**

.. code-block:: c

   float amd_fasterff(float x)

Computes the error function of ``x``, defined as erf(x) = (2/√π) * ∫₀ˣ e^(-t²) dt.

- **Maximum ULP:** 1.88
- **Note:** Not IEEE 754 compliant; does not handle special cases like NaNs or INFs

**amd_fasterf**

.. code-block:: c

   double amd_fasterf(double x)

Computes the error function of ``x``, defined as erf(x) = (2/√π) * ∫₀ˣ e^(-t²) dt.

- **Maximum ULP:** 1.88
- **Note:** Not IEEE 754 compliant; does not handle special cases like NaNs or INFs

.. End of Doc