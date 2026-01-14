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

.. _invtrigonometric:

Inverse Trigonometric Functions
===============================

Scalar
^^^^^^

Asin
----

See :ref:`scalar_api` for ``amd_asinf`` documentation.

See :ref:`scalar_api` for ``amd_asin`` documentation.


Acos
----

See :ref:`scalar_api` for ``amd_acosf`` documentation.

See :ref:`scalar_api` for ``amd_acos`` documentation.


Atan
----

See :ref:`scalar_api` for ``amd_atanf`` documentation.

See :ref:`scalar_api` for ``amd_atan`` documentation.


Atan2
-----

See :ref:`scalar_api` for ``amd_atan2f`` documentation.

See :ref:`scalar_api` for ``amd_atan2`` documentation.



Fast Variants
^^^^^^^^^^^^^

Fast variants of inverse trigonometric functions provide improved performance by trading a small amount of accuracy. These functions do not handle special cases like NaNs or INFs and are optimized for performance-critical applications where the input range is known and controlled.

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


Vector
^^^^^^

Asin
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for asin vector variants documentation.


Acos
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for acos vector variants documentation.


Atan
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for atan vector variants documentation.


.. End of Doc