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


Vector
^^^^^^

Pow
---

**AVX**

.. doxygenfunction:: amd_vrs4_powf
  :project: libm

.. doxygenfunction:: amd_vrd2_pow
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_powf
  :project: libm

.. doxygenfunction:: amd_vrd4_pow
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_powf
  :project: libm

.. doxygenfunction:: amd_vrd8_pow
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_powf
  :project: libm

.. doxygenfunction:: amd_vrda_pow
  :project: libm


Powx
----

**AVX**

.. doxygenfunction:: amd_vrs4_powxf
  :project: libm

.. doxygenfunction:: amd_vrd2_powx
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_powxf
  :project: libm

.. doxygenfunction:: amd_vrd4_powx
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_powxf
  :project: libm

.. doxygenfunction:: amd_vrd8_powx
  :project: libm


Sqrt
----

**AVX**

.. doxygenfunction:: amd_vrs4_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrd2_sqrt
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrd4_sqrt
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrd8_sqrt
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_sqrtf
  :project: libm

.. doxygenfunction:: amd_vrda_sqrt
  :project: libm


Cbrt
----

**AVX**

.. doxygenfunction:: amd_vrs4_cbrtf
  :project: libm

.. doxygenfunction:: amd_vrd2_cbrt
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_cbrtf
  :project: libm

.. doxygenfunction:: amd_vrda_cbrt
  :project: libm


.. End of Doc
