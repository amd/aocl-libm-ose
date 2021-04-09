/*
 * Copyright (C) 2018-2021, Advanced Micro Devices, Inc. All rights reserved.
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

/*
Program to generate interleaved look-up tables for log function

The head and tail tables are calculated as follows:

  Let table_size be the number of values required in the look-up tables
  The values of log(1 + ( j / table_size)), where j ranges from 0 to table_size,
        is stored as two tables - head and tail for maintaing precision

  First, the log value is calculated in extra precision using libquadmath as
  quadlog = logq (1 + ( j / table_size))
  Then, the least significant 16 bits for scalar log of the value quadlog are masked, which gives the head value
  i.e, head = quadlog & BITS_MASK

  Then, the tail value is calculated as,
   tail = quadlog - head

  libquadmath is used for getting the required precision

This program can be used to interleave Head and tail tables alone.

*/

#include<stdio.h>
#include<math.h>
#include<stdint.h>
#include<quadmath.h>

#define BITS_MASK 0xFFFFFFFFF0000000  /* Take the first 36 bits */


typedef union {
    double  doublex;
    uint64_t hexvalue;
} doubleword;

void main()
{

    int j=0;
    double index;
    double table_size = 256 ;   /* Set table-size as needed */

    doubleword inv, logv_head, logv_tail;

    for(j=0;j<=table_size;j++) {

        __float128 quadlog = log2q(1.0 + ((double)j) / table_size);

        logv_head.doublex = quadlog;

        logv_head.hexvalue = logv_head.hexvalue & BITS_MASK;

        logv_tail.doublex = quadlog - logv_head.doublex;

        printf("{ 0x%lx, 0x%lx}, \n", logv_head.hexvalue, logv_tail.hexvalue);

   }

}
