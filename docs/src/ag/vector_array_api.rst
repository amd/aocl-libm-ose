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

.. _vector_array_api:

=============================
Vector Array Functions
=============================

Overview
========

Vector array functions in AOCL-LibM provide a flexible interface for processing 
variable-length arrays of floating-point numbers. Unlike fixed-size vector functions 
(AVX, AVX2, AVX512), array functions can handle arrays of arbitrary length, making 
them ideal for data processing scenarios where the data size is not known at compile time.

These functions are prefixed with ``amd_vrsa_`` for single precision arrays and 
``amd_vrda_`` for double precision arrays.

.. note::
   Vector array functions are currently under development. This section will be 
   expanded as more functions become available.


Trigonometric Functions
=======================

Sin
---

.. doxygenfunction:: amd_vrsa_sinf
  :project: libm

.. doxygenfunction:: amd_vrda_sin
  :project: libm


Cos
---

.. doxygenfunction:: amd_vrsa_cosf
  :project: libm

.. doxygenfunction:: amd_vrda_cos
  :project: libm


Tan
---

.. doxygenfunction:: amd_vrsa_tanf
  :project: libm

.. doxygenfunction:: amd_vrda_tan
  :project: libm


Sincos
------

.. doxygenfunction:: amd_vrsa_sincosf
  :project: libm

.. doxygenfunction:: amd_vrda_sincos
  :project: libm


.. End of Doc
