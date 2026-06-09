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

.. _exponential:

Exponential Functions
=====================

Scalar
^^^^^^

Exp
---

See :ref:`scalar_api` for ``amd_expf`` documentation.

See :ref:`scalar_api` for ``amd_exp`` documentation.


Exp2
----

See :ref:`scalar_api` for ``amd_exp2f`` documentation.

See :ref:`scalar_api` for ``amd_exp2`` documentation.


Exp10
-----

See :ref:`scalar_api` for ``amd_exp10f`` documentation.

See :ref:`scalar_api` for ``amd_exp10`` documentation.


Expm1
-----

See :ref:`scalar_api` for ``amd_expm1f`` documentation.

See :ref:`scalar_api` for ``amd_expm1`` documentation.



Fast Variants
^^^^^^^^^^^^^

Fast variants of exponential functions provide improved performance by trading a small amount of accuracy. These functions do not handle special cases like NaNs or INFs and are optimized for performance-critical applications where the input range is known and controlled.

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


Vector
^^^^^^

Exp
---

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for exp vector variants documentation.


Exp2
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for exp2 vector variants documentation.


Exp10
-----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for exp10 vector variants documentation.


Expm1
-----

See :ref:`vector_avx_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for expm1 vector variants documentation.



.. End of Doc