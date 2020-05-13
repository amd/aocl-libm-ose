/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*/

#ifndef __LIBM_CONSTANTS_H__
#define __LIBM_CONSTANTS_H__

/*
 * Double precision - alias bin64,flt64
 */

#define FLT64_SIGN_SIZE		1
#define FLT64_SIGN_SHIFT	63
#define FLT64_SIGN_MASK		((1ULL << FLT64_SIGN_SHIFT)

#define FLT64_EXPO_SIZE		11
#define FLT64_EXPO_SHIFT	52
#define FLT64_EXPO_MASK		(((1ULL << FLT64_EXPO_SIZE) - 1) << FLT64_EXPO_SHIFT)

#define FLT64_MANT_SIZE		(52)
#define FLT64_MANT_SHIFT	0
#define FLT64_MANT_MASK		(((1ULL << FLT64_MANT_SIZE) - 1) << FLT64_MANT_SHFIT)

#define FLT64_EXP_MIN		-1022
#define FLT64_EXP_MAX		1023
#define FLT64_EXP_BIASMAX	2046
#define FLT64_EXP_BIASMIN	1

#define FLT64_INF		0x7FF0000000000000ULL
#define FLT64_NINF		0xFFF0000000000000ULL
#define FLT64_NAN		0x7FF8000000000000ULL

#define FLT64_MIN		0x1.0000000000000p-1022
#define FLT64_MAX		0x1.fffffffffffffp+1023


#define flt64_isnan(x) ({					\
	volatile union {double d; unsigned long long i;} _f64;	\
	_f64.d = x;						\
	_f64.i;							\
})

#define flt64_isinf(x) {}

/*
 * Single precision - alias bin32,flt32
 */

#define FLT32_SIGN_SIZE		1
#define FLT32_SIGN_SHIFT	31
#define FLT32_SIGN_MASK		((1ULL << FLT32_SIGN_SHIFT)

#define FLT32_EXPO_SIZE		8
#define FLT32_EXPO_SHIFT	23
#define FLT32_EXPO_MASK		(((1ULL << FLT32_EXPO_SIZE) - 1) << FLT32_EXPO_SHIFT)

#define FLT32_MANT_SIZE		23
#define FLT32_MANT_SHIFT	0
#define FLT32_MANT_MASK		(((1ULL << FLT32_MANT_SIZE) - 1) << FLT32_MANT_SHFIT)


#define FLT32_EXP_MIN		-128
#define FLT32_EXP_MAX		127
#define FLT32_EXP_BIASMAX	256
#define FLT32_EXP_BIASMIN	1

#define FLT32_INF		0x7FF00000
#define FLT32_NINF		0xFFF00000
#define FLT32_NAN		0x7FF80000

#define FLT32_MIN		0x1.0p-126f
#define FLT32_MAX		0x1.fffffep127f


#define flt32_isnan(x) ({						\
	volatile union {float f; unsigned int i;} _f32;			\
	_f32.d = x;							\
	_f32.i;								\
	})

#define flt32_isinf(x) {}


#endif	/* LIBM_CONSTANTS_H */
