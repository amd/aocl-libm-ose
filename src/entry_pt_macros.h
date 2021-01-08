/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __ENTRY_PT_MACROS_H__
#define __ENTRY_PT_MACROS_H__

#include <libm/types.h>

#define MK_FN_NAME(fn)				\
	STRINGIFY(FN_PROTOTYPE(fn))

/*
 * This should gnerate something like this
 * -------------
 * #define fname_expf FN_PROTOTYPE(expf)
 * .p2align 4
 * .globl fname_expf
 * .type fname_expf,@function
 * fname_expf:
 * mov g_amd_libm_ep_expf@GOTPCREL(%rip), %rax
 * jmp *(%rax)
 * -----------
 */
#define LIBM_DECL_FN_MAP(fn)						\
	asm (								\
	"\n\t"".p2align 4"						\
	"\n\t"".globl " MK_FN_NAME(fn)					\
	"\n\t"".type " STRINGIFY(FN_PROTOTYPE(fn)) " ,@function"	\
	"\n\t" MK_FN_NAME(fn) " :"					\
	"\n\t" "mov " STRINGIFY(G_ENTRY_PT_ASM(fn)) "@GOTPCREL(%rip), %rax"	\
	"\n\t" "jmp *(%rax)"						\
		);


#define WEAK_LIBM_ALIAS(x, y)					\
	asm("\n\t"".weak " STRINGIFY(x)				\
	    "\n\t"".set " STRINGIFY(x) ", " STRINGIFY(y)	\
	    "\n\t");

#endif /* __ENTRY_PT_MACROS_H__ */

