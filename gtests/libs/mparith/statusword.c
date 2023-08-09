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


#include <stdio.h>

#include "controlword.h"

/* Floating-point status word manipulating functions */
#include "statusword.h"

/* Clear all exceptions from the floating-point status word */
void
clear_fpsw(void) {
#if defined(WINDOWS)
#ifdef IS_64BIT
    unsigned int cw;
    cw = _mm_getcsr();
    cw &= ~(AMD_F_INEXACT | AMD_F_UNDERFLOW | AMD_F_OVERFLOW |
                    AMD_F_DIVBYZERO | AMD_F_INVALID);
    _mm_setcsr(cw);
#else
    fpenv_type fenv;
    /* Get the current floating-point environment */
    __asm fnstenv fenv;
    /* Set the status word to 0 */
    fenv.status_word = 0;
    /* Put the floating-point environment back */
    __asm fldenv fenv;
#endif
#elif defined(__linux__)
#if defined (IS_64BIT)
    unsigned int cw;
    /* Get the current floating-point control/status word */
    __asm__ volatile("STMXCSR %0" : "=m"(cw));
    cw &= ~(AMD_F_INEXACT | AMD_F_UNDERFLOW | AMD_F_OVERFLOW |
                    AMD_F_DIVBYZERO | AMD_F_INVALID);
    __asm__ volatile("LDMXCSR %0" : : "m"(cw));
#else /* ! IS_64BIT */
    fpenv_type fenv;
    /* Get the current floating-point environment */
    __asm__ volatile("fnstenv %0" : "=m"(fenv));
    /* Set the status word to 0 */
    fenv.status_word = 0;
    /* Put the floating-point environment back */
    __asm__ volatile("fldenv %0" : : "m"(fenv));
#endif
#else
#error Unknown machine
#endif
}

/* Return the current floating-point status word */
unsigned int
get_fpsw(void) {
#if defined(WINDOWS)
#ifdef IS_64BIT
    return _mm_getcsr();
#else
    unsigned short sw;
    __asm fstsw sw;
    return (unsigned int)sw;
#endif
#elif defined(__linux__)
#if defined(IS_64BIT)
    unsigned int sw;
    __asm__ volatile("STMXCSR %0" : "=m"(sw));
    return sw;
#else /* ! IS_64BIT */
    unsigned short sw;
    __asm__ volatile("fstsw %0":"=m"(sw));
    return (unsigned int)sw;
#endif
#else
#error Unknown machine
#endif
}

/* Returns 1 if the inexact flag has been raised, otherwise 0 */
int
sw_inexact_raised(unsigned int sw) {
    if (sw & AMD_F_INEXACT)
    { return 1; }
    else
    { return 0; }
}

/* Returns 1 if the underflow flag has been raised, otherwise 0 */
int
sw_underflow_raised(unsigned int sw) {
    if (sw & AMD_F_UNDERFLOW)
    { return 1; }
    else
    { return 0; }
}

/* Returns 1 if the overflow flag has been raised, otherwise 0 */
int
sw_overflow_raised(unsigned int sw) {
    if (sw & AMD_F_OVERFLOW)
    { return 1; }
    else
    { return 0; }
}

/* Returns 1 if the division-by-zero flag has been raised, otherwise 0 */
int
sw_divbyzero_raised(unsigned int sw) {
    if (sw & AMD_F_DIVBYZERO)
    { return 1; }
    else
    { return 0; }
}

/* Returns 1 if the invalid flag has been raised, otherwise 0 */
int
sw_invalid_raised(unsigned int sw) {
    if (sw & AMD_F_INVALID)
    { return 1; }
    else
    { return 0; }
}

/* Shows all exceptions signalled in status word sw */
void
show_status(unsigned int sw, FILE *outFile) {
    int nflags = 0;
    fprintf(outFile, "Floating-point status:");

    if (sw_inexact_raised(sw)) {
        fprintf(outFile, " inexact");
        nflags++;
    }

    if (sw_divbyzero_raised(sw)) {
        fprintf(outFile, " divbyzero");
        nflags++;
    }

    if (sw_underflow_raised(sw)) {
        fprintf(outFile, " underflow");
        nflags++;
    }

    if (sw_overflow_raised(sw)) {
        fprintf(outFile, " overflow");
        nflags++;
    }

    if (sw_invalid_raised(sw)) {
        fprintf(outFile, " invalid");
        nflags++;
    }

    if (nflags == 0)
    { fprintf(outFile, " (no flags set)"); }

    fprintf(outFile, "\n");
    fflush(outFile);
}

/* Gets all exceptions signalled in status word sw as a bit mask */
unsigned int
get_sw_flags(void) {
    unsigned int sw = get_fpsw();
    unsigned int flags = 0;

    if (sw_inexact_raised(sw))
    { flags |= AMD_F_INEXACT; }

    if (sw_divbyzero_raised(sw))
    { flags |= AMD_F_DIVBYZERO; }

    if (sw_underflow_raised(sw))
    { flags |= AMD_F_UNDERFLOW; }

    if (sw_overflow_raised(sw))
    { flags |= AMD_F_OVERFLOW; }

    if (sw_invalid_raised(sw))
    { flags |= AMD_F_INVALID; }

    return flags;
}


#ifdef TEST
int
main(void) {
    double x, y, z;
    show_status(get_fpsw());
    x = 1.0;
    y = 3.0;
    z = x / y;
    show_status(get_fpsw());
    clear_fpsw();
    y = 0.0;
    z = x / y;
    show_status(get_fpsw());
    clear_fpsw();
    x = 1.0e-200;
    y = x;
    z = x * y;
    show_status(get_fpsw());
    clear_fpsw();
    y = 1.0 / x;
    z = y / x;
    show_status(get_fpsw());
    clear_fpsw();
    x = 0.0;
    y = 0.0;
    z = x / y;
    show_status(get_fpsw());
    clear_fpsw();
    return 0;
}
#endif
