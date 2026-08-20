/*
 * Copyright (C) 2026, Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * Compile-time architecture dispatch header.
 *
 * The build system selects the target architecture by passing exactly one of
 * -DALM_BUILD_ZEN, -DALM_BUILD_ZEN2, ..., -DALM_BUILD_ZEN6 along with
 * -include zen_arch.h when compiling sources under src/optimized/.
 * This header maps that define to the corresponding arch header, so the same
 * optimized source file can produce correctly-named symbols for every Zen
 * generation without per-variant wrapper files.
 *
 */

#ifndef __ZEN_ARCH_H__
#define __ZEN_ARCH_H__

#undef ALM_OVERRIDE
#define ALM_OVERRIDE 1

/* Ensure exactly one ALM_BUILD_ZEN* is defined */
#if !defined(ALM_BUILD_ZEN)  && !defined(ALM_BUILD_ZEN2) && \
    !defined(ALM_BUILD_ZEN3) && !defined(ALM_BUILD_ZEN4) && \
    !defined(ALM_BUILD_ZEN5) && !defined(ALM_BUILD_ZEN6)
#  error "No target architecture defined. Pass -DALM_BUILD_ZENx to the compiler."
#elif (defined(ALM_BUILD_ZEN)  + defined(ALM_BUILD_ZEN2) + \
       defined(ALM_BUILD_ZEN3) + defined(ALM_BUILD_ZEN4) + \
       defined(ALM_BUILD_ZEN5) + defined(ALM_BUILD_ZEN6)) > 1
#  error "Multiple ALM_BUILD_ZEN* macros defined. Define exactly one."
#endif

#if   defined(ALM_BUILD_ZEN6)
#  include <libm/arch/zen6.h>
#elif defined(ALM_BUILD_ZEN5)
#  include <libm/arch/zen5.h>
#elif defined(ALM_BUILD_ZEN4)
#  include <libm/arch/zen4.h>
#elif defined(ALM_BUILD_ZEN3)
#  include <libm/arch/zen3.h>
#elif defined(ALM_BUILD_ZEN2)
#  include <libm/arch/zen2.h>
#elif defined(ALM_BUILD_ZEN)
#  include <libm/arch/zen.h>
#endif

#endif  /* __ZEN_ARCH_H__ */
