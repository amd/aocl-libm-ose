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

.. _logarithmic:

Logarithmic Functions
=====================

Scalar
^^^^^^

Log
---

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_logf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log`` documentation.



Log2
----

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log2f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log2`` documentation.



Log10
-----

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log10f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log10`` documentation.



Log1p
-----

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log1pf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_log1p`` documentation.



Fast Variants
^^^^^^^^^^^^^

Fast variants of logarithmic functions provide improved performance by trading a small amount of accuracy. These functions do not handle special cases like NaNs or INFs and are optimized for performance-critical applications where the input range is known and controlled.

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


Vector
^^^^^^

Log
---

**AVX**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs4_logf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd2_log`` documentation.


**AVX2**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs8_logf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd4_log`` documentation.


**AVX512**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs16_logf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd8_log`` documentation.


**Vector array**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrsa_logf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrda_log`` documentation.



Log2
----

**AVX**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs4_log2f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd2_log2`` documentation.


**AVX2**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs8_log2f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd4_log2`` documentation.


**AVX512**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs16_log2f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd8_log2`` documentation.


**Vector array**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrsa_log2f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrda_log2`` documentation.



Log10
-----

**AVX2**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs8_log10f`` documentation.


**AVX512**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs16_log10f`` documentation.


**Vector array**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrsa_log10f`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrda_log10`` documentation.



Log1p
-----

**AVX**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrs4_log1pf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrd2_log1p`` documentation.


**Vector array**

See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrsa_log1pf`` documentation.


See :ref:`scalar_api`, :ref:`vector_avx_api`, :ref:`vector_avx2_api`, or :ref:`vector_avx512_api` for ``amd_vrda_log1p`` documentation.



.. End of Doc