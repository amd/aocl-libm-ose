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

.. _error:

Error Functions
===============

Scalar
^^^^^^

Erf
---

See :ref:`scalar_api` for ``amd_erff`` documentation.

See :ref:`scalar_api` for ``amd_erf`` documentation.


Erfc
----

See :ref:`scalar_api` for ``amd_erfcf`` documentation.

See :ref:`scalar_api` for ``amd_erfc`` documentation.


Erfinv
------

See :ref:`scalar_api` for ``amd_erfinv`` documentation.


Erfcinv
-------

See :ref:`scalar_api` for ``amd_erfcinv`` documentation.



Fast Variants
^^^^^^^^^^^^^

Fast variants of error functions provide improved performance by trading a small amount of accuracy. These functions are not IEEE 754 compliant and do not handle special cases like NaNs or INFs. They are optimized for performance-critical applications where the input range is known and controlled.

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


Vector
^^^^^^

Erf
---

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for erf vector variants documentation.


Erfc
----

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for erfc vector variants documentation.


Erfinv
------

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for erfinv vector variants documentation.


Erfcinv
-------

See :ref:`vector_avx_api`, :ref:`vector_avx2_api`, :ref:`vector_avx512_api`, or :ref:`vector_array_api` for erfcinv vector variants documentation.



.. End of Doc