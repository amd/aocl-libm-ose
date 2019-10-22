#include "libm_amd.h"
#include "libm_util_amd.h"
#include <stdio.h>

double  FN_PROTOTYPE(nan)(const char *tagp)
{


    /* Check for input range */
    UT64 checkbits;
    U64 val=0;
    S64 num;
    checkbits.u64  =QNANBITPATT_DP64; 
    if(tagp == NULL)
    {
      return checkbits.f64;
    }

    switch(*tagp)
    {
    case '0': /* base 8 */
                tagp++; 
                if( *tagp == 'x' || *tagp == 'X')
                {
                    /* base 16 */
                    tagp++;
                    while(*tagp != '\0')
                    {
                        
                        if(*tagp >= 'A' && *tagp <= 'F' )
                        {
                            num = *tagp - 'A' + 10;
                        }
                        else
                        if(*tagp >= 'a' && *tagp <= 'f' )
                        {                          
                            num = *tagp - 'a' + 10;  
                        }
                        else
                        {
                            num = *tagp - '0'; 
                        }                        

                        if( (num < 0 || num > 15))
                        {
                            val = QNANBITPATT_DP64;
                            break;
                        }
                        val = (val << 4)  |  num; 
                        tagp++;
                    }
                }
                else
                {
                    /* base 8 */
                    while(*tagp != '\0')
                    {
                        num = *tagp - '0';
                        if( num < 0 || num > 7)
                        {
                            val = QNANBITPATT_DP64;
                            break;
                        }
                        val = (val << 3)  |  num; 
                        tagp++;
                    }
                }
		break;
    default:
                while(*tagp != '\0')
                {
                    val = val*10;
                    num = *tagp - '0';
                    if( num < 0 || num > 9)
                    {
                        val = QNANBITPATT_DP64;
                        break;
                    }
                    val = val + num; 
                    tagp++;
                }
            
    }

   if((val & ~NINFBITPATT_DP64) == 0)
	val = QNANBITPATT_DP64;
	 
    checkbits.u64 = (val | QNANBITPATT_DP64) & ~SIGNBIT_DP64;
    return checkbits.f64  ;
}



