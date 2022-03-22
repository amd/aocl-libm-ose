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


#ifndef STATUSWORD_H_INCLUDED
#define STATUSWORD_H_INCLUDED 1

#include <stdio.h>

#include "precision.h"
//#include "controlword.h"

/* Return the current floating-point status word */
unsigned int get_fpsw(void);

/* Clear all exceptions from the floating-point status word */
void clear_fpsw(void);

/* Returns 1 if the inexact flag has been raised in sw, otherwise 0 */
int sw_inexact_raised(unsigned int sw);

/* Returns 1 if the underflow flag has been raised in sw, otherwise 0 */
int sw_underflow_raised(unsigned int sw);

/* Returns 1 if the overflow flag has been raised in sw, otherwise 0 */
int sw_overflow_raised(unsigned int sw);

/* Returns 1 if the division-by-zero flag has been raised in sw, otherwise 0 */
int sw_divbyzero_raised(unsigned int sw);

/* Returns 1 if the invalid flag has been raised in sw, otherwise 0 */
int sw_invalid_raised(unsigned int sw);

/* Shows all exceptions signalled in status word sw */
void show_status(unsigned int sw, FILE *outFile);

/* Gets all exceptions signalled in status word sw as a bit mask */
unsigned int get_sw_flags(void);

#endif
