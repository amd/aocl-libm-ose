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

.. _arithmetic:

Arithmetic Functions
====================

Vector
^^^^^^

Add
---

**Vector array**

.. doxygenfunction:: amd_vrda_add
  :project: libm

.. doxygenfunction:: amd_vrsa_addf
  :project: libm


**Indexed Arithmetic**

.. doxygenfunction:: amd_vrda_addi
  :project: libm

.. doxygenfunction:: amd_vrsa_addfi
  :project: libm


Sub
---

**Vector array**

.. doxygenfunction:: amd_vrda_sub
  :project: libm

.. doxygenfunction:: amd_vrsa_subf
  :project: libm


**Indexed Arithmetic**

.. doxygenfunction:: amd_vrda_subi
  :project: libm

.. doxygenfunction:: amd_vrsa_subfi
  :project: libm


Mul
---

**Vector array**

.. doxygenfunction:: amd_vrda_mul
  :project: libm

.. doxygenfunction:: amd_vrsa_mulf
  :project: libm


**Indexed Arithmetic**

.. doxygenfunction:: amd_vrda_muli
  :project: libm

.. doxygenfunction:: amd_vrsa_mulfi
  :project: libm


Div
---

**Vector array**

.. doxygenfunction:: amd_vrda_div
  :project: libm

.. doxygenfunction:: amd_vrsa_divf
  :project: libm


**Indexed Arithmetic**

.. doxygenfunction:: amd_vrda_divi
  :project: libm

.. doxygenfunction:: amd_vrsa_divfi
  :project: libm


Linearfrac
----------

**AVX**

.. doxygenfunction:: amd_vrd2_linearfrac
  :project: libm

.. doxygenfunction:: amd_vrs4_linearfracf
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrd4_linearfrac
  :project: libm

.. doxygenfunction:: amd_vrs8_linearfracf
  :project: libm

**AVX512**

.. doxygenfunction:: amd_vrd8_linearfrac
  :project: libm

.. doxygenfunction:: amd_vrs16_linearfracf
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrda_linearfrac
  :project: libm

.. doxygenfunction:: amd_vrsa_linearfracf
  :project: libm


.. End of Doc
