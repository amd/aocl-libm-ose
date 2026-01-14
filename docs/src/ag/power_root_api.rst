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

.. _power_root:

Power and Root Functions
========================

Scalar
^^^^^^

Pow
---

See :ref:`scalar_api` for ``amd_powf`` documentation.

See :ref:`scalar_api` for ``amd_pow`` documentation.


Sqrt
----

See :ref:`scalar_api` for ``amd_sqrtf`` documentation.

See :ref:`scalar_api` for ``amd_sqrt`` documentation.


Cbrt
----

See :ref:`scalar_api` for ``amd_cbrtf`` documentation.

See :ref:`scalar_api` for ``amd_cbrt`` documentation.



Fast Variants
^^^^^^^^^^^^^

Fast variants of power functions provide improved performance by trading a small amount of accuracy. These functions do not handle special cases like NaNs or INFs and are optimized for performance-critical applications where the input range is known and controlled.

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


Vector
^^^^^^

Pow
---

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for pow vector variants documentation.


Powx
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for powx vector variants documentation.


Sqrt
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for sqrt vector variants documentation.


Cbrt
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for cbrt vector variants documentation.



.. End of Doc