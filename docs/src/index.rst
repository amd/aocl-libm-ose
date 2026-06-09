..  Copyright (C) 2025-2026, Advanced Micro Devices. All rights reserved.

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

##########
AOCL-LibM
##########

AOCL-LibM is a high performance implementation of LibM, the standard C
library of elementary floating-point mathematical functions. It
includes many of the functions from the C99 standard. Single and
double precision versions of the functions are provided, all
optimized for accuracy and performance, including a small number of
complex functions. There are also a number of vector and fast scalar
variants, in which a small amount of the accuracy has been traded for
greater performance.


**************
API Overview
**************

LibM functions are categorized into different types based on their mathematical
domains and operational characteristics. Each category encompasses a set of
related functions designed to provide optimal performance for specific computational
tasks. Each LibM function can have the following variants:

1. **Scalar** - Single precision (32-bit) and double precision (64-bit)
2. **Vector**

   1. **128-bit** - 4 × 32-bit values / 2 × 64-bit values
   2. **256-bit** - 8 × 32-bit values / 4 × 64-bit values
   3. **512-bit** - 16 × 32-bit values / 8 × 64-bit values
   4. **Array** - Variable-length array operations

.. note::
   **Accuracy Considerations**

   1. For scalar functions, IEEE 754 mandates a maximum ULP of 0.5. However, not all AOCL-LibM scalar APIs have a maximum ULP of 0.5
   2. For vector variants, a maximum ULP of 4 is maintained in AOCL-LibM
   3. For fast scalar variants, a maximum ULP of 4 is maintained in AOCL-LibM. Please note that these variants also do not handle special cases, edge cases or invalid inputs

Naming Convention
=================

Scalar Functions
----------------

For scalar functions, an ``f`` at the end of the function name indicates that it is 
single-precision; otherwise, it is double-precision. For example:

- ``exp()`` - double precision exponential function
- ``expf()`` - single precision exponential function

Fast Scalar Functions
---------------------

Fast scalar functions use the prefix ``amd_fast`` followed by the function name. These
functions provide optimized performance by trading a small amount of accuracy and do not
handle special cases like NaNs or INFs. For example:

- ``amd_fastexp()`` - fast double precision exponential function
- ``amd_fastexpf()`` - fast single precision exponential function

Vector Functions
----------------

The following naming convention is used for the vector functions:

.. code::

   amd_vr<type><vec_size>_<func>

where,

-  ``v`` - vector

-  ``r`` - real

-  ``<type>`` - ``s`` for single precision and ``d`` for double precision

-  ``<vec_size>`` - 4, 8, or 16 for single-precision; 2, 4, or 8 for
   double-precision; or ``a`` if it is a vector array function

-  ``<func>`` - function name, such as ``exp`` or ``expf``

For example, a single precision 4-element version of exp has the
signature:

.. code:: console

   __m128 amd_vrs4_expf(__m128 x);

Similarly, a double precision 8-element version of sin would be:

.. code:: console

   __m512d amd_vrd8_sin(__m512d x);

And an array function for single precision cos would be:

.. code:: console

   void amd_vrsa_cosf(int n, float *x, float *y);


Function Categories
===================

AOCL-LibM provides two complementary ways to browse and access the API documentation.
Choose the categorization that best suits your needs:


By Implementation Variant
--------------------------

Functions grouped by their execution model and performance characteristics. This view is useful
when you need to optimize code for specific hardware capabilities or performance requirements.

**When to use this view:**

- When optimizing performance-critical code paths
- When targeting specific SIMD instruction sets (AVX, AVX2, AVX512)
- When you need to process arrays of data efficiently
- When accuracy requirements allow for fast variants

.. toctree::
   :maxdepth: 1
   :caption: Implementation Variant Categories

   ag/scalar_api.rst
   ag/fast_scalar_api.rst
   ag/vector_avx_api.rst
   ag/vector_avx2_api.rst
   ag/vector_avx512_api.rst
   ag/vector_array_api.rst


By Mathematical Domain
----------------------

Functions grouped by their mathematical category (trigonometric, exponential, logarithmic, etc.).
Each category contains all available variants (scalar, fast scalar, and vector) of the functions
within that mathematical domain.

**When to use this view:**

- When you know which mathematical operation you need (e.g., sine, logarithm, power)
- When you want to see all available implementations of a specific mathematical function
- When implementing algorithms that require specific mathematical operations

.. toctree::
   :maxdepth: 1
   :caption: Mathematical Domain Categories

   ag/trigonometric_api.rst
   ag/invtrigonometric_api.rst
   ag/hyperbolic_api.rst
   ag/invhyperbolic_api.rst
   ag/exponential_api.rst
   ag/logarithmic_api.rst
   ag/power_root_api.rst
   ag/error_api.rst
   ag/special_functions_api.rst
   ag/remainder_quotient.rst
   ag/euclidean_distance_api.rst
   ag/nearest_integer_api.rst
   ag/fp_manipulation_api.rst
   ag/arithmetic_api.rst
   ag/complex_api.rst
   ag/min_max_diff.rst
   ag/linearfrac_api.rst


* AOCL-LibM is developed and maintained by AMD.
* For support, send an email to* toolchainsupport@amd.com


.. End of Doc