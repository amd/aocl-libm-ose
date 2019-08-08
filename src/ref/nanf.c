#include "libm_amd.h"
#include "libm_util_amd.h"
#include <stdio.h>


float  FN_PROTOTYPE(nanf)(const char *tagp)
{


    /* Check for input range */
    UT32 checkbits;
    U32 val=0;
    S32 num;
    checkbits.u32  =QNANBITPATT_SP32; 
    if(tagp == NULL)
      return  checkbits.f32 ;
      

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
                            val = QNANBITPATT_SP32;
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
                            val = QNANBITPATT_SP32;
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
                        val = QNANBITPATT_SP32;
                        break;
                    }
                    val = val + num; 
                    tagp++;
                }
            
    }
     
/*   if(val > ~INDEFBITPATT_SP32)
	val = (val | QNANBITPATT_SP32) & ~SIGNBIT_SP32;
	 
    checkbits.u32 = val | EXPBITS_SP32 ;	 */

   if((val & ~INDEFBITPATT_SP32) == 0)
	val = QNANBITPATT_SP32;
	 
    checkbits.u32 = (val | QNANBITPATT_SP32) & ~SIGNBIT_SP32;


	return checkbits.f32  ;
}
weak_alias (nanf, FN_PROTOTYPE(nanf))
