#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#define LENGTH_OF(x) (sizeof(x)/sizeof(x[0]))
#include "libm_amd.h"
#include "libm_nag.h"
#include "unistd.h"
struct ExcFlags
{
unsigned int us : 1;
unsigned int os : 1;
unsigned int zs : 1;
unsigned int is : 1;

unsigned int other_s : 4;
};




#endif // __DATA_TYPES_H__

