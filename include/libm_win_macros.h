#ifndef __LIBM_WIN_MACROS_H__
#define __LIBM_WIN_MACROS_H__

/*
 * symbol name': attributes not present on previous declaration.
 */
#pragma warning( disable : 4985 )

#if defined(__COMP_REF__)
    #define FN_PROTOTYPE(fn_name) fn_name
#elif defined(__NAG_REF__)
    #define FN_PROTOTYPE(fn_name) amd_ref_##fn_name
#else
    // by default call  libm function
    #define FN_PROTOTYPE(fn_name)  amd_##fn_name
#endif

#if defined(__GNUC)
    /* On windows compiling with GNU/Mingw compilers */
#else
    #define weak_alias(name, aliasname) /* as nothing */
#endif

#endif  /* __LIBM_WIN_MACROS_H__ */
