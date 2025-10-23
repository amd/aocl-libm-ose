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


Vector
^^^^^^

Asin
----

**AVX**

.. doxygenfunction:: amd_vrs4_asinf
  :project: libm

.. doxygenfunction:: amd_vrd2_asin
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_asinf
  :project: libm

.. doxygenfunction:: amd_vrd4_asin
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_asinf
  :project: libm

.. doxygenfunction:: amd_vrd8_asin
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_asinf
  :project: libm

.. doxygenfunction:: amd_vrda_asin
  :project: libm


Acos
----

**AVX**

.. doxygenfunction:: amd_vrs4_acosf
  :project: libm

.. doxygenfunction:: amd_vrd2_acos
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_acosf
  :project: libm

.. doxygenfunction:: amd_vrd4_acos
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_acosf
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_acosf
  :project: libm

.. doxygenfunction:: amd_vrda_acos
  :project: libm


Atan
----

**AVX**

.. doxygenfunction:: amd_vrs4_atanf
  :project: libm

.. doxygenfunction:: amd_vrd2_atan
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_atanf
  :project: libm

.. doxygenfunction:: amd_vrd4_atan
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_atanf
  :project: libm

.. doxygenfunction:: amd_vrd8_atan
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_atanf
  :project: libm

.. doxygenfunction:: amd_vrda_atan
  :project: libm

.. End of Doc
