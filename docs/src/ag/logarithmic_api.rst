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

.. doxygenfunction:: amd_logf
  :project: libm

.. doxygenfunction:: amd_log
  :project: libm


Log2
----

.. doxygenfunction:: amd_log2f
  :project: libm

.. doxygenfunction:: amd_log2
  :project: libm


Log10
-----

.. doxygenfunction:: amd_log10f
  :project: libm

.. doxygenfunction:: amd_log10
  :project: libm


Log1p
-----

.. doxygenfunction:: amd_log1pf
  :project: libm

.. doxygenfunction:: amd_log1p
  :project: libm


Vector
^^^^^^

Log
---

**AVX**

.. doxygenfunction:: amd_vrs4_logf
  :project: libm

.. doxygenfunction:: amd_vrd2_log
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_logf
  :project: libm

.. doxygenfunction:: amd_vrd4_log
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_logf
  :project: libm

.. doxygenfunction:: amd_vrd8_log
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_logf
  :project: libm

.. doxygenfunction:: amd_vrda_log
  :project: libm


Log2
----

**AVX**

.. doxygenfunction:: amd_vrs4_log2f
  :project: libm

.. doxygenfunction:: amd_vrd2_log2
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_log2f
  :project: libm

.. doxygenfunction:: amd_vrd4_log2
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_log2f
  :project: libm

.. doxygenfunction:: amd_vrd8_log2
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_log2f
  :project: libm

.. doxygenfunction:: amd_vrda_log2
  :project: libm


Log10
-----

**AVX2**

.. doxygenfunction:: amd_vrs8_log10f
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrs16_log10f
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_log10f
  :project: libm

.. doxygenfunction:: amd_vrda_log10
  :project: libm


Log1p
-----

**AVX**

.. doxygenfunction:: amd_vrs4_log1pf
  :project: libm

.. doxygenfunction:: amd_vrd2_log1p
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_log1pf
  :project: libm

.. doxygenfunction:: amd_vrda_log1p
  :project: libm


.. End of Doc
