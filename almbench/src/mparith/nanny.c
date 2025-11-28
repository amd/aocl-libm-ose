/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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


#include "nanny.h"
#include <string.h> /* For memcpy */

/* Returns 1 if x is NaN */
int
disnan(REAL *x) {
#if defined(WINDOWS)

    /* Seems like the only reliable way to do it under Windows. */
    if (_isnan(*x))
    { return 1; }
    else
    { return 0; }

#elif defined sgi

    /* Seems like the only reliable way to do it on SGI. */
    if (isnan(*x))
    { return 1; }
    else
    { return 0; }

#else

    if (*x != *x)
    { return 1; }
    else if (*x == 1.0 && *x == 2.0)
    { return 1; }
    else
    { return 0; }

#endif
}

int
disnan_(REAL *x) {
    return disnan(x);
}

int
DISNAN(REAL *x) {
    return disnan(x);
}

/* Returns 1 if x is infinity */
int
disinf(REAL *x) {
    REAL y;
    y = *x / 2;

    if (disnan(x))
    { return 0; }
    else if (*x == 0.0)
    { return 0; }
    else if (*x == y)
    { return 1; }
    else
    { return 0; }
}

int
disinf_(REAL *x) {
    return disinf(x);
}

int
DISINF(REAL *x) {
    return disinf(x);
}

/* Returns 1 if x is zero */
int
diszero(REAL *x) {
    if (disnan(x))
    { return 0; }
    else if (*x == 0.0)
    { return 1; }
    else
    { return 0; }
}

int
diszero_(REAL *x) {
    return diszero(x);
}

int
DISZERO(REAL *x) {
    return diszero(x);
}

/* Returns 1 if x is positive */
int
dispos(REAL *x) {
    REAL z, one = 1;

    if (disnan(x))
    { return 0; }
    else if (*x != 0.0)
    { return *x > 0.0; }
    else {
        z = one / *x;
        return z > 0.0;
    }
}

int
dispos_(REAL *x) {
    return dispos(x);
}

int
DISPOS(REAL *x) {
    return dispos(x);
}

/* Returns 1 if x is negative */
int
disneg(REAL *x) {
    REAL z, one = 1;

    if (disnan(x))
    { return 0; }
    else if (*x != 0.0)
    { return *x < 0.0; }
    else {
        z = one / *x;
        return z < 0.0;
    }
}

int
disneg_(REAL *x) {
    return disneg(x);
}

int
DISNEG(REAL *x) {
    return disneg(x);
}

/* Returns a NaN.
   If sign is non-zero, the NaN is negative.
   If signalling is non-zero, the NaN is signalling. */
void
createNaN(int sign, int signalling, REAL *r) {
#define HIGH_HALF(x) (*(((unsigned int *)&x) + 1))
#define LOW_HALF(x) (*((unsigned int *)&x))
#define BITS_SP32(x) (*((unsigned int *)&x))
#ifdef WINDOWS
#define BITS_DP64(x) (*((unsigned __int64 *)&x))
#else
#define BITS_DP64(x) (*((unsigned long *)&x))
#endif
    REAL retval;

    if (sizeof(REAL) == sizeof(int)) {
        /* Happens when code transformed from double to float */
        if (sign) {
            /* Negative NaN required */
            if (signalling)
                /* Signalling NaN */
            { BITS_SP32(retval) = 0xff800001; }
            else
                /* Quiet NaN */
            { BITS_SP32(retval) = 0xffc00001; }
        } else {
            /* Positive NaN required */
            if (signalling)
                /* Signalling NaN */
            { BITS_SP32(retval) = 0x7f800001; }
            else
                /* Quiet NaN */
            { BITS_SP32(retval) = 0x7fc00001; }
        }
    }

#ifdef IS_64BIT
    else if (sizeof(double) == sizeof(long)) {
        /* 64-bit long machine */
        if (sign) {
            /* Negative NaN required */
            if (signalling)
                /* Signalling NaN */
#ifdef WINDOWS
                BITS_DP64(retval) = 0xfff0000000000001i64;

#else
                BITS_DP64(retval) = 0xfff0000000000001L;
#endif
            else
                /* Quiet NaN */
#ifdef WINDOWS
                BITS_DP64(retval) = 0xfff8000000000001i64;

#else
                BITS_DP64(retval) = 0xfff8000000000001L;
#endif
        } else {
            /* Positive NaN required */
            if (signalling)
                /* Signalling NaN */
#ifdef WINDOWS
                BITS_DP64(retval) = 0x7ff0000000000001i64;

#else
                BITS_DP64(retval) = 0x7ff0000000000001L;
#endif
            else
                /* Quiet NaN */
#ifdef WINDOWS
                BITS_DP64(retval) = 0x7ff8000000000001i64;

#else
                BITS_DP64(retval) = 0x7ff8000000000001L;
#endif
        }
    }

#endif
    else {
        /* 32-bit long machine */
        if (sign) {
            /* Negative NaN required */
            if (signalling) {
                /* Signalling NaN */
                HIGH_HALF(retval) = 0xfff00000;
                LOW_HALF(retval) = 0x00000001;
            } else {
                /* Quiet NaN */
                HIGH_HALF(retval) = 0xfff80000;
                LOW_HALF(retval) = 0x00000001;
            }
        } else {
            /* Positive NaN required */
            if (signalling) {
                /* Signalling NaN */
                HIGH_HALF(retval) = 0x7ff00000;
                LOW_HALF(retval) = 0x00000001;
            } else {
                /* Quiet NaN */
                HIGH_HALF(retval) = 0x7ff80000;
                LOW_HALF(retval) = 0x00000001;
            }
        }
    }

    /* memcpy required instead of following because assignment
       of signalling NaN converts to a quiet NaN in some versions
       of gcc. */
    /*
     *r = retval;
     */
    memcpy(r, &retval, sizeof(REAL));
}

void
createnan(int *sign, int *signalling, REAL *r) {
    createNaN(*sign, *signalling, r);
}

void
createnan_(int *sign, int *signalling, REAL *r) {
    createNaN(*sign, *signalling, r);
}

void
CREATENAN(int *sign, int *signalling, REAL *r) {
    createNaN(*sign, *signalling, r);
}

/*
 * Returns a signed infinity
 * Set sign = 0 for +infinity, anything else for -infinity
 */
void
createInfinity(int sign, REAL *r) {
    REAL x = 0.0;
    REAL y = 1.0;
    REAL retVal;

    if (sign == 0)
    { retVal = y; }
    else
    { retVal = -y; }

    retVal /= x;
    *r = retVal;
}

void
createinfinity(int *sign, REAL *r) {
    createInfinity(*sign, r);
}

void
createinfinity_(int *sign, REAL *r) {
    createInfinity(*sign, r);
}

void
CREATEINFINITY(int *sign, REAL *r) {
    createInfinity(*sign, r);
}

/*
 * Returns a signed zero.
 * Set sign = 0 for +zero, anything else for -zero.
 */
void
createZero(int sign, REAL *r) {
    REAL x = 1.0;
    REAL retVal, t;
    createInfinity(sign, &t);
    retVal = x / t;
    *r = retVal;
}

void
createzero(int *sign, REAL *r) {
    createZero(*sign, r);
}

void
createzero_(int *sign, REAL *r) {
    createZero(*sign, r);
}

void
CREATEZERO(int *sign, REAL *r) {
    createZero(*sign, r);
}

/*
 * Returns the largest floating-point number.
 * Set sign = 0 for +large, anything else for -large
 */
void
createLargest(int sign, int mantis, int emax, REAL *r) {
    REAL pow = 1.0;
    REAL x = 0.0;
    REAL two = 2.0;
    int i;

    for (i = 0; i < mantis; i++) {
        pow = pow / two;
        x = x + pow;
    }

    for (i = 0; i < emax; i++)
    { x = x * two; }

    if (sign != 0)
    { x = -x; }

    *r = x;
}

void
createlargest(int *sign, int *mantis, int *emax, REAL *r) {
    createLargest(*sign, *mantis, *emax, r);
}

void
createlargest_(int *sign, int *mantis, int *emax, REAL *r) {
    createLargest(*sign, *mantis, *emax, r);
}

void
CREATELARGEST(int *sign, int *mantis, int *emax, REAL *r) {
    createLargest(*sign, *mantis, *emax, r);
}

/*
 * Returns the smallest normalized floating-point number.
 * Set sign = 0 for +small, anything else for -small
 */
void
createSmallestNormal(int sign, int emin, REAL *r) {
    REAL x = 1.0;
    REAL two = 2.0;
    int i;

    for (i = 0; i < -emin + 1; i++)
    { x = x / two; }

    if (sign != 0)
    { x = -x; }

    *r = x;
}

void
createsmallestNormal(int *sign, int *emin, REAL *r) {
    createSmallestNormal(*sign, *emin, r);
}

void
createsmallestNormal_(int *sign, int *emin, REAL *r) {
    createSmallestNormal(*sign, *emin, r);
}

void
CREATESMALLESTNORMAL(int *sign, int *emin, REAL *r) {
    createSmallestNormal(*sign, *emin, r);
}

/*
 * Returns the smallest denormalized floating-point number.
 * Set sign = 0 for +small, anything else for -small
 */
void
createSmallestDenormal(int sign, int mantis, int emin, REAL *r) {
    REAL x = 1.0;
    REAL two = 2.0;
    int i;

    for (i = 0; i < -emin + mantis; i++)
    { x = x / two; }

    if (sign != 0)
    { x = -x; }

    *r = x;
}

void
createsmallestdenormal(int *sign, int *mantis, int *emin, REAL *r) {
    createSmallestDenormal(*sign, *mantis, *emin, r);
}

void
createsmallestdenormal_(int *sign, int *mantis, int *emin, REAL *r) {
    createSmallestDenormal(*sign, *mantis, *emin, r);
}

void
CREATESMALLESTDENORMAL(int *sign, int *mantis, int *emin, REAL *r) {
    createSmallestDenormal(*sign, *mantis, *emin, r);
}
