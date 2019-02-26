/*

Program to generate interleaved look-up tables for log function
Author: Nimmy Krishnan <Nimmy.Krishnan@amd.com>

The head and tail tables are calculated as follows:

  Let table_size be the number of values required in the look-up tables
  The values of log(1 + ( j / table_size)), where j ranges from 0 to table_size,
        is stored as two tables - head and tail for maintaing precision

  First, the log value is calculated in extra precision using libquadmath as
  quadlog = logq (1 + ( j / table_size))
  Then, the least significant 32 bits (for vector log) / 28 bits (for scalar log) of the value quadlog are masked, which gives the head value
  i.e, head = quadlog & BITS_MASK

  Then, the tail value is calculated as,
   tail = quadlog - head

The inverse values' table is used in Scalar log. It is generated as follows:

  Let table_size be the number of values required in the table
  Let step_size = 2 * table_size

  Then, each value of the table = 2 * [ step_size / ( i + step_size) ], where i = 0,2,4,....,step_size

  libquadmath is used for getting the required precision

This program can be used to interleave Head and tail tables alone, or Head, tail and inverse tables.
Set the value of inv_interleave according to whether inverse table is to be interleaved or not


*/

#include<stdio.h>
#include<quadmath.h>
#include<stdint.h>

#define BITS_MASK 0xFFFFFFFFF0000000  // 28 bits ; Use this for scalar log

// #define BITS_MASK 0xFFFFFFFF00000000  // 32 bits ; Use this for vector log


typedef union {
    double   doublex;
    unsigned long long int  hexvalue;
} doubleword;

int inv_interleave = 0;   // Set this to 1 if you want to interleave the inverse table as well

void main()
{

   int j=0;
   double index;
   __float128 quadlog;
   doubleword temp,head,tail;
   double table_size = 1024.0 ;   // Set table-size as needed

   // For inverse table
   __float128 val,step = 2048.0;
   doubleword vald;


   for(j=0;j<=table_size;j++)
   {
     index = (double)(1.0 + (j/table_size));

     quadlog = logq(index);

     temp.doublex = quadlog;

     head.hexvalue = temp.hexvalue & BITS_MASK ;  //HEAD

     tail.doublex = quadlog - head.doublex; // Tail

     if(inv_interleave)
     {
        val = step / ((j*2) + step);
        val = 2.0 * val ;
        vald.doublex = val;

        printf("             .quad 0x%llx\n ", vald.hexvalue);

     }

     printf("             .quad 0x%llx\n ",head.hexvalue);
     printf("             .quad 0x%llx\n ",tail.hexvalue);


   }


}
