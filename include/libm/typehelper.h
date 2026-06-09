/*
 * Copyright (C) 2018-2026, Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef __LIBM_TYPEHELPER_H__
#define __LIBM_TYPEHELPER_H__

#include <float.h>
#include <math.h>
#include <libm/types.h>

static inline uint32_t
asuint32(float f)
{
	flt32_t fl = {.f = f};
	return fl.u;
}

static inline float
asfloat(uint32_t i)
{
	flt32_t fl = {.u = i};
	return fl.f;
}

static inline double
asdouble(uint64_t i)
{
	flt64_t dbl = {.u = i};
	return dbl.d;
}

static inline uint64_t
asuint64(double f)
{
	flt64_t fl = {.d = f};
	return fl.u;
}

static inline double
eval_as_double(double d)
{
    return d;
}

static inline float 
eval_as_float(float f)
{
    return f;
}

static inline int32_t
cast_float_to_i32( float x )
{
	return (int32_t) x;
}

static inline   int64_t
cast_double_to_i64( double x )
{
	return (int64_t) x;
}

static inline float
cast_i32_to_float( int32_t x )
{
	return (float)x;
}

static inline double
cast_i64_to_double( int64_t x )
{
	return (double)x;
}


/*
 * =============================================================================
 * Double-Double Arithmetic Operations
 * =============================================================================
 *
 * A double-double number represents a value as the unevaluated sum of two
 * IEEE doubles: x = hi + lo, where |lo| <= 0.5 * ulp(hi).
 * This provides approximately 106 bits of mantissa precision.
 *
 */

/* Create a double-double from a single double */
static inline dd_t dd_from_d(double a) {
    dd_t r = {a, 0.0};
    return r;
}

/* Convert double-double back to double */
static inline double dd_to_d(dd_t a) {
    return a.hi + a.lo;
}

/*
 * Two-Sum: Computes s = a + b and e = error such that a + b = s + e exactly.
 * Requires: no restrictions on a, b
 */
static inline dd_t dd_two_sum(double a, double b) {
    double s = a + b;
    double v = s - a;
    double e = (a - (s - v)) + (b - v);
    dd_t r = {s, e};
    return r;
}

/*
 * Fast-Two-Sum: Computes s = a + b and e = error.
 * Requires: |a| >= |b|
 */
static inline dd_t dd_fast_two_sum(double a, double b) {
    double s = a + b;
    double e = b - (s - a);
    dd_t r = {s, e};
    return r;
}

/*
 * Split: Splits a double into high and low parts with 26 bits each.
 * a = a_hi + a_lo exactly
 */
static inline void dd_split(double a, double *a_hi, double *a_lo) {
    /* Use bit manipulation for exact split (26 mantissa bits in high part) */
    *a_hi = asdouble(asuint64(a) & 0xfffffffff8000000ULL);
    *a_lo = a - *a_hi;
}

/*
 * Two-Product: Computes p = a * b and e = error such that a * b = p + e exactly.
 * Uses FMA for exact error computation.
 */
static inline dd_t dd_two_prod(double a, double b) {
    double p = a * b;
    double e = fma(a, b, -p);
    dd_t r = {p, e};
    return r;
}

/*
 * Double-Double Addition: (a.hi + a.lo) + (b.hi + b.lo)
 */
static inline dd_t dd_add(dd_t a, dd_t b) {
    dd_t s = dd_two_sum(a.hi, b.hi);
    double e = a.lo + b.lo;
    s = dd_fast_two_sum(s.hi, s.lo + e);
    return s;
}

/*
 * Double-Double + Double: (a.hi + a.lo) + b
 */
static inline dd_t dd_add_d(dd_t a, double b) {
    dd_t s = dd_two_sum(a.hi, b);
    s.lo += a.lo;
    s = dd_fast_two_sum(s.hi, s.lo);
    return s;
}

/*
 * Double-Double Subtraction: (a.hi + a.lo) - (b.hi + b.lo)
 */
static inline dd_t dd_sub(dd_t a, dd_t b) {
    dd_t neg_b = {-b.hi, -b.lo};
    return dd_add(a, neg_b);
}

/*
 * Double-Double Multiplication: (a.hi + a.lo) * (b.hi + b.lo)
 */
static inline dd_t dd_mul(dd_t a, dd_t b) {
    dd_t p = dd_two_prod(a.hi, b.hi);
    p.lo += a.hi * b.lo + a.lo * b.hi;
    p = dd_fast_two_sum(p.hi, p.lo);
    return p;
}

/*
 * Double-Double * Double: (a.hi + a.lo) * b
 */
static inline dd_t dd_mul_d(dd_t a, double b) {
    dd_t p = dd_two_prod(a.hi, b);
    p.lo += a.lo * b;
    p = dd_fast_two_sum(p.hi, p.lo);
    return p;
}

/*
 * Double-Double Division: (a.hi + a.lo) / (b.hi + b.lo)
 * Uses Newton-Raphson refinement for the quotient.
 */
static inline dd_t dd_div(dd_t a, dd_t b) {
    double q1 = a.hi / b.hi;
    
    /* r = a - q1 * b */
    dd_t prod = dd_mul_d(b, q1);
    dd_t r = dd_sub(a, prod);
    
    double q2 = r.hi / b.hi;
    r = dd_sub(r, dd_mul_d(b, q2));
    
    double q3 = r.hi / b.hi;
    
    dd_t q = dd_fast_two_sum(q1, q2);
    q = dd_add_d(q, q3);
    return q;
}

/*
 * Double-Double Division by Double: (a.hi + a.lo) / b
 */
static inline dd_t dd_div_d(dd_t a, double b) {
    double q1 = a.hi / b;
    
    /* r = a - q1 * b */
    dd_t prod = dd_two_prod(q1, b);
    dd_t r = dd_sub(a, prod);
    
    double q2 = r.hi / b;
    
    dd_t q = dd_fast_two_sum(q1, q2);
    return q;
}

/*
 * Double-Double Negation
 */
static inline dd_t dd_neg(dd_t a) {
    dd_t r = {-a.hi, -a.lo};
    return r;
}

/*
 * Double-Double Absolute Value
 */
static inline dd_t dd_abs(dd_t a) {
    if (a.hi < 0.0) {
        return dd_neg(a);
    }
    return a;
}

/*
 * Double-Double comparison: returns 1 if a < b, 0 otherwise
 */
static inline int dd_lt(dd_t a, dd_t b) {
    return (a.hi < b.hi) || (a.hi == b.hi && a.lo < b.lo);
}

/*
 * Double-Double comparison: returns 1 if a >= b, 0 otherwise
 */
static inline int dd_ge(dd_t a, dd_t b) {
    return !dd_lt(a, b);
}

/*
 * Double-Double comparison: returns 1 if a > b, 0 otherwise
 */
static inline int dd_gt(dd_t a, dd_t b) {
    return dd_lt(b, a);
}

/*
 * Double-Double comparison: returns 1 if a <= b, 0 otherwise
 */
static inline int dd_le(dd_t a, dd_t b) {
    return !dd_gt(a, b);
}

/*
 * Double-Double comparison: returns 1 if a == b, 0 otherwise
 */
static inline int dd_eq(dd_t a, dd_t b) {
    return (a.hi == b.hi) && (a.lo == b.lo);
}

/*
 * Double-Double comparison: returns 1 if a != b, 0 otherwise
 */
static inline int dd_ne(dd_t a, dd_t b) {
    return !dd_eq(a, b);
}


#endif	/* __LIBM_TYPEHELPER_H__ */
