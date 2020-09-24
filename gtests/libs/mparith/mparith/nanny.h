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
