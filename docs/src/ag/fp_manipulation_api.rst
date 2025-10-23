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

.. _floating_point_manipulation:

Floating-Point Manipulation Functions
=====================================

Scalar
^^^^^^

Fabs
----

.. doxygenfunction:: amd_fabsf
  :project: libm

.. doxygenfunction:: amd_fabs
  :project: libm


Modf
----

.. doxygenfunction:: amd_modff
  :project: libm

.. doxygenfunction:: amd_modf
  :project: libm


Frexp
-----

.. doxygenfunction:: amd_frexpf
  :project: libm

.. doxygenfunction:: amd_frexp
  :project: libm


Copysign
--------

.. doxygenfunction:: amd_copysignf
  :project: libm

.. doxygenfunction:: amd_copysign
  :project: libm


Nan
---

.. doxygenfunction:: amd_nanf
  :project: libm

.. doxygenfunction:: amd_nan
  :project: libm


Finite
------

.. doxygenfunction:: amd_finitef
  :project: libm

.. doxygenfunction:: amd_finite
  :project: libm


Ldexp
-----

.. doxygenfunction:: amd_ldexpf
  :project: libm

.. doxygenfunction:: amd_ldexp
  :project: libm


Scalbn
------

.. doxygenfunction:: amd_scalbnf
  :project: libm

.. doxygenfunction:: amd_scalbn
  :project: libm


Scalbln
-------

.. doxygenfunction:: amd_scalblnf
  :project: libm

.. doxygenfunction:: amd_scalbln
  :project: libm


Logb
----

.. doxygenfunction:: amd_logbf
  :project: libm

.. doxygenfunction:: amd_logb
  :project: libm


Ilogb
-----

.. doxygenfunction:: amd_ilogbf
  :project: libm

.. doxygenfunction:: amd_ilogb
  :project: libm


Nextafter
---------

.. doxygenfunction:: amd_nextafterf
  :project: libm

.. doxygenfunction:: amd_nextafter
  :project: libm


Nexttoward
----------

.. doxygenfunction:: amd_nexttowardf
  :project: libm

.. doxygenfunction:: amd_nexttoward
  :project: libm


Vector
^^^^^^

Fabs
----

**AVX**

.. doxygenfunction:: amd_vrs4_fabsf
  :project: libm

.. doxygenfunction:: amd_vrd2_fabs
  :project: libm

**AVX2**

.. doxygenfunction:: amd_vrs8_fabsf
  :project: libm

.. doxygenfunction:: amd_vrd4_fabs
  :project: libm

**Vector array**

.. doxygenfunction:: amd_vrsa_fabsf
  :project: libm

.. doxygenfunction:: amd_vrda_fabs
  :project: libm


.. End of Doc
