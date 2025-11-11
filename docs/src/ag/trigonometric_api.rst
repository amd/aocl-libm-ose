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

.. _trigonometric:

Trigonometric Functions
=======================

Scalar
^^^^^^

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


Fast Variants
^^^^^^^^^^^^^

Fast variants of trigonometric functions provide improved performance by trading a small amount of accuracy. These functions do not handle special cases like NaNs or INFs and are optimized for performance-critical applications where the input range is known and controlled.

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


Vector
^^^^^^

Sin
---

**AVX**

.. doxygenfunction:: amd_vrs4_sinf
  :project: libm

.. doxygenfunction:: amd_vrd2_sin
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_sinf
  :project: libm

.. doxygenfunction:: amd_vrd4_sin
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_sinf
  :project: libm

.. doxygenfunction:: amd_vrd8_sin
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_sinf
  :project: libm

.. doxygenfunction:: amd_vrda_sin
  :project: libm


Cos
---

**AVX**

.. doxygenfunction:: amd_vrs4_cosf
  :project: libm

.. doxygenfunction:: amd_vrd2_cos
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_cosf
  :project: libm

.. doxygenfunction:: amd_vrd4_cos
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_cosf
  :project: libm

.. doxygenfunction:: amd_vrd8_cos
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_cosf
  :project: libm

.. doxygenfunction:: amd_vrda_cos
  :project: libm


Tan
---

**AVX**

.. doxygenfunction:: amd_vrs4_tanf
  :project: libm

.. doxygenfunction:: amd_vrd2_tan
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_tanf
  :project: libm

.. doxygenfunction:: amd_vrd4_tan
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_tanf
  :project: libm

.. doxygenfunction:: amd_vrd8_tan
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_tanf
  :project: libm

.. doxygenfunction:: amd_vrda_tan
  :project: libm


Sincos
------

**AVX**

.. doxygenfunction:: amd_vrs4_sincosf
  :project: libm

.. doxygenfunction:: amd_vrd2_sincos
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_sincosf
  :project: libm

.. doxygenfunction:: amd_vrd4_sincos
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_sincosf
  :project: libm

.. doxygenfunction:: amd_vrd8_sincos
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_sincosf
  :project: libm

.. doxygenfunction:: amd_vrda_sincos
  :project: libm


.. End of Doc