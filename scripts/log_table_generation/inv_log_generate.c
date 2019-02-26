/* Program for generating F_inv table for log function
   Author - Nimmy Krishnan < Nimmy.Krishnan@amd.com>

   The values of the table are generated as :

      Let table_size be the number of values required in the table
      Let step_size = 2 * table_size

      Then, each value of the table = 2 * [ step_size / ( i + step_size) ], where i = 0,2,4,....,step_size

      libquadmath is used for getting the required precision

*/
#include<stdio.h>
#include<quadmath.h>
#include<stdint.h>

typedef union {

 double doublex;
 unsigned long long int hexval;

} doubleword;


void main()
{

  __float128 step=2048.0, i=0.0;
  __float128 val;
  doubleword vald;

  while (i<= step)
  {

     val = step / (i + step);

     val = 2.0 * val ;

     vald.doublex = val;


     printf("             .quad 0x%llx   # %1.23f\n", vald.hexval, vald.doublex);


     i+=2.0q;


   }

}
