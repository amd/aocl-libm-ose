#ifndef __LIBM_NIX_MACROS_H__
#define __LIBM_NIX_MACROS_H__

/*
 * Define ALIASNAME as a weak alias for NAME.
 * If weak aliases are not available, this defines a strong alias.
 */
#define weak_alias(aliasname, name)  _weak_alias(aliasname, name)

#define _weak_alias(al_name, name)                                    \
    extern __typeof (name) al_name __attribute__ ((weak, alias (#name)));
#if 0
#define _weak_alias(al_name, name)                                    \
    extern __typeof (name) al_name __attribute__ ((weak, alias (#name)));
    extern __typeof (name) fast##alname __attribute__ ((weak, alias (#name)));
#endif

#endif  /* __LIBM_NIX_MACROS_H__ */
