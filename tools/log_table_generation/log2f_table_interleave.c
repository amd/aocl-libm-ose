/*
 * Copyright (C) 2021-2022, Advanced Micro Devices, Inc. All rights reserved.
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
Program to generate look-up tables for log2 function

The head and tail tables are calculated as follows:

  Let table_size be the number of values required in the look-up tables
  The values of log2(1 + ( j / table_size)), where j ranges from 0 to table_size,
  is stored as two tables - head and tail for maintaining precision

  First, the log value is calculated in double precision
  logd = log2 (1 + ( j / table_size))
  Then, the least significant 14 bits of the value 'logd' are masked, which gives the head value
  i.e, head = logd & BITS_MASK

  Then, the tail value is calculated as,
   tail = logd - head

The inverse values' table is used in Scalar log. It is generated as follows:

  Let table_size be the number of values required in the table
  Let step_size = 2 * table_size

  Then, each value of the table = 2 * [ step_size / ( i + step_size) ], where i = 0,2,4,....,step_size

This program can be used to interleave Head and tail tables alone.

*/

#include<stdio.h>
#include<math.h>
#include<stdint.h>

#define BITS_MASK 0xFFFFC000  /* Taking only the first 18 bits */

typedef union {
    float  floatx;
    uint32_t hexvalue;
} floatword;

void main()
{

    int j=0;
    double logd;
    double table_size = 128 ;   // Set table-size as needed

    floatword inv, logv_head, logv_tail;

    for(j=0;j<=table_size;j++) {
        
        inv.floatx = (table_size * 2) / (table_size + j);

        logd = log2(1.0 + ((double)j) / table_size);

        logv_head.floatx = logd;

        logv_head.hexvalue = logv_head.hexvalue & BITS_MASK;

        logv_tail.floatx = logd - logv_head.floatx;

        printf("{0x%x, 0x%x, 0x%x}, \n",inv.hexvalue, logv_head.hexvalue, logv_tail.hexvalue);

    }


}
