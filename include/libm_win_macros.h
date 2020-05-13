/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*/

#ifndef __LIBM_WIN_MACROS_H__
#define __LIBM_WIN_MACROS_H__

/*
 * symbol name': attributes not present on previous declaration.
 */
#pragma warning( disable : 4985 )

#if defined(__GNUC)
    /* On windows compiling with GNU/Mingw compilers */
#else
    #define weak_alias(name, aliasname) /* as nothing */
#endif

#endif  /* __LIBM_WIN_MACROS_H__ */
