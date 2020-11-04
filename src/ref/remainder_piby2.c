/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#include "libm_amd.h"

void __amd_remainder_piby2(double x, double *r, double *rr, int *region);

void __amd_remainder_2dfpiby2(__m128d x, __m128d *r, __m128d *rr, __m128i *region)
{
	union XMMREGTYP_
	{

		__m128d d128;
		__m128i i128;
		double d[2];
		unsigned long long i[2];
	};

	typedef union XMMREGTYP_ XMMREGTYP;


	XMMREGTYP inpx;
	XMMREGTYP resr,resrr,resregion;
	int reg,reg1;
	double irr,irr1,ir,ir1;



	inpx.d128 = x;

     __amd_remainder_piby2(inpx.d[0],&ir,&irr,&reg);
	 __amd_remainder_piby2(inpx.d[1],&ir1,&irr1,&reg1);

	 resregion.i[0] = reg;
	 resregion.i[1] = reg1;

	 resrr.d[0] = irr;
	 resrr.d[1] = irr1;

	 resr.d[0] = ir;
	 resr.d[1] = ir1;



	*r = resr.d128;
	*rr = resrr.d128;
	*region = resregion.i128;

}

