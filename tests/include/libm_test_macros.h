#ifndef __LIBM_TEST_MACROS_H__
#define __LIBM_TEST_MACROS_H__


#if defined(__GNUC__)

#define GCC_ALIGN(var, align) __builtin_assume_aligned(var, align)

#define IVDEP _Pragma("GCC ivdep")

#define GCC_ALIGN(var, align) __builtin_assume_aligned(var, align)

#endif


#define offsetof(type, member)	((size_t)&((type *)0)->member)

#define container_of(ptr, struct_type, field)                           \
    ({                                                                  \
	const typeof(((struct_type *)0)->field) *field_ptr =            \
            (ptr);                                                      \
	(struct_type *)((char *)field_ptr -                             \
                        offsetof(struct_type, field));                  \
    })


// Helpers to build a docstring
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define BUILD_TEST_DOC(b) "AMD LIBM Test for " STRINGIFY(b)


#endif  /* if __LIBM_TEST_MACROS_H__ */
