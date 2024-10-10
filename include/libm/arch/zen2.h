/*
 * Copyright (C) 2018-2024, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __ALM_ARCH_ZEN2_H__
#define __ALM_ARCH_ZEN2_H__

/*
 * This architecture is for "Zen2", same as --march=znver2
 */
#define ALM_ARCH_ZN2			zn2
#define ALM_PREFIX_ZN2			amd

#ifndef ALM_PREFIX
#define ALM_PREFIX		ALM_PREFIX_ZN2
#endif

#ifndef ALM_ARCH
#define ALM_ARCH		ALM_ARCH_ZN2
#endif

/*
 * Override all previously definded prototypes
 */
#if defined(ALM_OVERRIDE)

#include <libm_macros.h>
//#include <libm/types.h>

#ifndef  ALM_PREFIX_ZN2
#undef   ALM_PREFIX_ZN2
#define  ALM_PREFIX_ZN2			amd
#endif

#define  ALM_PROTO_OPT(x) 		ALM_PROTO_ARCH_ZN2(x)


#endif /* ALM_OVERRIDE */


#ifndef ALM_PROTO_ARCH
#define __ALM_PROTO_ARCH(a, x, y)		ALM_MAKE_PROTO_SFX(a, x, y)
#define  __ALM_MAKE_PROTO_ARCH(a, x, y) 	__ALM_PROTO_ARCH(a, x, y)		
#endif

#ifdef   ALM_PROTO_ARCH
#undef   ALM_PROTO_ARCH
#define  ALM_PROTO_ARCH(x)	        ALM_PROTO_ARCH_ZN2(x)
#endif

#define  ALM_PROTO_ARCH_ZN2(x)		__ALM_PROTO_ARCH(ALM_PREFIX_ZN2, x, ALM_ARCH_ZN2)

/*
#define ALM_PROTO_ARCH_CHANGE_INTERNAL  1
#include <libm/arch/alm_funcs.h>
#undef  ALM_PROTO_CHANGE_INTENRAL
*/
#pragma push_macro("ALM_PROTO_INTERNAL")
#define ALM_PROTO_INTERNAL ALM_PROTO_ARCH
#include "../__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL
#pragma pop_macro("ALM_PROTO_INTERNAL")

#endif  /* _ALM_ARCH_ZEN2_H_ */

