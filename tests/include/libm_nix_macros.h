#ifndef __LIBM_NIX_MACROS_H__
#define __LIBM_NIX_MACROS_H__

#if defined(__COMP_REF__)
    #define FN_PROTOTYPE(fn_name) fn_name
#elif defined(__NAG_REF__)
    #define FN_PROTOTYPE(fn_name) amd_ref_##fn_name
#else
    // by default call  libm function
    #define FN_PROTOTYPE(fn_name)  amd_##fn_name
#endif                                         /* if __COMP_REF__ */

/*
 * Define ALIASNAME as a weak alias for NAME.
 * If weak aliases are not available, this defines a strong alias.
 */
#define weak_alias(aliasname, name)  _weak_alias(aliasname, name)
#define _weak_alias(al_name, name)                                    \
    extern __typeof (name) al_name __attribute__ ((weak, alias (#name))); \
    extern __typeof (name) fast##alname __attribute__ ((weak, alias (#name)));

#endif  /* __LIBM_NIX_MACROS_H__ */
