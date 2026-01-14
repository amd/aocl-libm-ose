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

See :ref:`scalar_api` for ``amd_sinf`` documentation.

See :ref:`scalar_api` for ``amd_sin`` documentation.


Cos
---

See :ref:`scalar_api` for ``amd_cosf`` documentation.

See :ref:`scalar_api` for ``amd_cos`` documentation.


Tan
---

See :ref:`scalar_api` for ``amd_tanf`` documentation.

See :ref:`scalar_api` for ``amd_tan`` documentation.


Sincos
------

See :ref:`scalar_api` for ``amd_sincosf`` documentation.

See :ref:`scalar_api` for ``amd_sincos`` documentation.



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

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for sin vector variants documentation.


Cos
---

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for cos vector variants documentation.


Tan
---

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for tan vector variants documentation.


Sincos
------

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for sincos vector variants documentation.



.. End of Doc