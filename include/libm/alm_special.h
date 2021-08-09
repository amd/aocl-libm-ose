
/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _ALM_SPECIAL_H_
#define _ALM_SPECIAL_H_

#include <stdint.h>
#include <math.h>

#include <libm_errno_amd.h>

/*
 * Error codes to specify if an input is or
 *  output will be special floating point numbers
 */

enum {
        ALM_E_IN_X_NEG  = 1<<8,         /* is a ZERO */
        ALM_E_IN_X_NAN  = 1<<7,         /* first arg is a NaN or QNaN */
        ALM_E_IN_X_INF  = 1<<6,         /* is '+/- INF' */
        ALM_E_IN_X_ZERO = 1<<5,         /* is a ZERO */

        /* For functions with 2 args */
        ALM_E_IN_Y_NAN  = 1<<4,         /*  */
        ALM_E_IN_Y_INF  = 1<<3,
        ALM_E_IN_Y_ZERO = 1<<2,
        ALM_E_IN_Y_NEG  = 1<<0,         /* is a ZERO */


        ALM_E_OUT_NAN  = 1<<16,
        ALM_E_OUT_INF  = 1<<17,
        ALM_E_OUT_ZERO = 1<<18,
};


#if defined(WIN64)  |  defined(WINDOWS)

#include <fpieee.h>
#define EXCEPTION_S							_exception
#define __alm_handle_error			_handle_error
#define __alm_handle_errorf			_handle_errorf

double _handle_error(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        double arg1,
        double arg2,
        int nargs
        );

float _handle_errorf(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        float arg1,
        float arg2,
        int nargs
        );

#define ALM_ERR_DOMAIN     _DOMAIN			/* argument domain error */
#define ALM_ERR_SING       _SING				/* argument singularity */
#define ALM_ERR_OVERFLOW   _OVERFLOW		/* overflow range error */
#define ALM_ERR_UNDERFLOW  _UNDERFLOW		/* underflow range error */
#define ALM_ERR_TLOSS      _TLOSS				/* total loss of precision */
#define ALM_ERR_PLOSS      _PLOSS				/* partial loss of precision */

#else

#define EXCEPTION_S							exception

double __alm_handle_error(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        double arg1,
        double arg2,
        int nargs
        );

float __alm_handle_errorf(
        char *fname,
        int opcode,
        unsigned long long value,
        int type,
        int flags,
        int error,
        float arg1,
        float arg2,
        int nargs
        );


#define ALM_ERR_DOMAIN     1   /* argument domain error */
#define ALM_ERR_SING       2   /* argument singularity */
#define ALM_ERR_OVERFLOW   3   /* overflow range error */
#define ALM_ERR_UNDERFLOW  4   /* underflow range error */
#define ALM_ERR_TLOSS      5   /* total loss of precision */
#define ALM_ERR_PLOSS      6   /* partial loss of precision */
#endif /* if WINDOWS */


double	alm_exp2_special(double x, double y, uint32_t code);
float   alm_exp2f_special(float x, float y, uint32_t code);
double	alm_log2_special(double x, double y, uint32_t code);
double	alm_log10_special(double x, double y, uint32_t code);
float   alm_acosf_special(float x, uint32_t code);

#endif
