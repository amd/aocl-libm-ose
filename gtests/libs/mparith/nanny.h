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


#ifndef NANNY_H_INCLUDED
#define NANNY_H_INCLUDED 1

#include "precision.h"

#if defined(WINDOWS)
/* For _isnan */
#include <float.h>
#endif

/* Returns 1 if x is NaN */
int disnan(REAL *x);
/* Versions callable from Fortran */
int disnan_(REAL *x);
int DISNAN(REAL *x);

/* Returns 1 if x is infinity */
int disinf(REAL *x);
/* Versions callable from Fortran */
int disinf_(REAL *x);
int DISINF(REAL *x);

/* Returns 1 if x is zero */
int diszero(REAL *x);
/* Versions callable from Fortran */
int diszero_(REAL *x);
int DISZERO(REAL *x);

/* Returns 1 if x is positive */
int dispos(REAL *x);
/* Versions callable from Fortran */
int dispos_(REAL *x);
int DISPOS(REAL *x);

/* Returns 1 if x is negative */
int disneg(REAL *x);
/* Versions callable from Fortran */
int disneg_(REAL *x);
int DISNEG(REAL *x);

/* Returns a NaN */
void createNaN(int sign, int signalling, REAL *r);
/* Versions callable from Fortran */
void createnan(int *sign, int *signalling, REAL *r);
void createnan_(int *sign, int *signalling, REAL *r);
void CREATENAN(int *sign, int *signalling, REAL *r);

/*
 * Returns a signed infinity
 * Set sign = 0 for +infinity, anything else for -infinity
 */
void createInfinity(int sign, REAL *r);
/* Versions callable from Fortran */
void createinfinity(int *sign, REAL *r);
void createinfinity_(int *sign, REAL *r);
void CREATEINFINITY(int *sign, REAL *r);

/*
 * Returns a signed zero.
 * Set sign = 0 for +zero, anything else for -zero.
 */
void createZero(int sign, REAL *r);
/* Versions callable from Fortran */
void createzero(int *sign, REAL *r);
void createzero_(int *sign, REAL *r);
void CREATEZERO(int *sign, REAL *r);

/*
 * Returns the largest floating-point number.
 * Set sign = 0 for +large, anything else for -large
 */
void createLargest(int sign, int mantis, int emax, REAL *r);
/* Versions callable from Fortran */
void createlargest(int *sign, int *mantis, int *emax, REAL *r);
void createlargest_(int *sign, int *mantis, int *emax, REAL *r);
void CREATELARGEST(int *sign, int *mantis, int *emax, REAL *r);

/*
 * Returns the smallest normalized floating-point number.
 * Set sign = 0 for +small, anything else for -small
 */
void createSmallestNormal(int sign, int emin, REAL *r);
/* Versions callable from Fortran */
void createsmallestNormal(int *sign, int *emin, REAL *r);
void createsmallestNormal_(int *sign, int *emin, REAL *r);
void CREATESMALLESTNORMAL(int *sign, int *emin, REAL *r);

/*
 * Returns the smallest denormalized floating-point number.
 * Set sign = 0 for +small, anything else for -small
 */
void createSmallestDenormal(int sign, int mantis, int emin, REAL *r);
/* Versions callable from Fortran */
void createsmallestdenormal(int *sign, int *mantis, int *emin, REAL *r);
void createsmallestdenormal_(int *sign, int *mantis, int *emin, REAL *r);
void CREATESMALLESTDENORMAL(int *sign, int *mantis, int *emin, REAL *r);

#endif
