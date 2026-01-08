..  Copyright (C) 2026, Advanced Micro Devices. All rights reserved.

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

.. _special_functions:

=================
Special Functions
=================

Special functions are mathematical functions that arise frequently in mathematical analysis,
physics, engineering, and other applied sciences. Unlike elementary functions (trigonometric,
exponential, logarithmic), special functions often have no simple closed-form expression
and are defined by specific mathematical properties or differential equations.


CdfNorm (Normal Distribution CDF)
==================================

The cumulative distribution function (CDF) of the standard normal distribution, also known
as the normal CDF or Φ(x), computes the probability that a standard normal random variable
takes a value less than or equal to x:

.. math::

   \Phi(x) = \frac{1}{\sqrt{2\pi}} \int_{-\infty}^{x} e^{-t^2/2} dt

This function is fundamental in statistics, probability theory, and quantitative finance
for computing probabilities, confidence intervals, and option pricing.

**Relationship to Error Function:**

The normal CDF is related to the error function by:

.. math::

   \Phi(x) = \frac{1}{2} \left[ 1 + \text{erf}\left(\frac{x}{\sqrt{2}}\right) \right]

.. note::
   Currently, only double-precision variants of cdfnorm are available. Single-precision
   variants (cdfnormf) may be added in future releases.


Scalar
^^^^^^

.. doxygenfunction:: amd_cdfnorm
  :project: libm


Vector
^^^^^^

**AVX (128-bit)**

.. doxygenfunction:: amd_vrd2_cdfnorm
  :project: libm


**AVX2 (256-bit)**

.. doxygenfunction:: amd_vrd4_cdfnorm
  :project: libm


**AVX512 (512-bit)**

.. doxygenfunction:: amd_vrd8_cdfnorm
  :project: libm


**Array**

.. doxygenfunction:: amd_vrda_cdfnorm
  :project: libm


.. End of Doc