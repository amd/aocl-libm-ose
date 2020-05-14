/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

