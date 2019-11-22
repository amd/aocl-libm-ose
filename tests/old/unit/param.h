#include <amp.h>
#include <amp_math.h>
#include <iostream>
#include "libm_nag.h"


#define EXC_CHK_NONE {0,0,0,0, 0}

#define EXC_CHK_UNFL {1,0,0,0, 0}
#define EXC_CHK_OVFL {0,1,0,0, 0}
#define EXC_CHK_ZERO {0,0,1,0, 0}
#define EXC_CHK_INVL {0,0,0,1, 0}

#define MILLION 1000000.0

/*32 bit values*/
#define F32_POS_SNAN        0x7f800001
#define F32_NEG_SNAN        0xff800001
#define F32_POS_SNAN_Q      0x7fc00001
#define F32_NEG_SNAN_Q      0xffc00001
#define F32_POS_QNAN        0x7fc00000
#define F32_NEG_QNAN        0xffc00000
#define F32_POS_INF         0x7f800000
#define F32_NEG_INF         0xff800000
#define F32_POS_ONE         0x3f800000
#define F32_NEG_ONE         0xbf800000
#define F32_POS_ZERO        0x00000000
#define F32_NEG_ZERO        0x80000000

#define F32_POS_HDENORM     0x007fffff
#define F32_NEG_HDENORM     0x807fffff
#define F32_POS_LDENORM     0x00000001
#define F32_NEG_LDENORM     0x80000001

#define F32_POS_HNORMAL     0x7f7fffff
#define F32_NEG_HNORMAL     0xff7fffff
#define F32_POS_LNORMAL     0x00800000
#define F32_NEG_LNORMAL     0x80800000

#define F32_POS_PI          0x40490fdb
#define F32_NEG_PI          0xc0490fdb
#define F32_POS_PIBY2       0x3fc90fdb
#define F32_NEG_PIBY2       0xbfc90fdb


/*62 bit values*/
#define F64_POS_SNAN        0x7FF0000000000001LL
#define F64_NEG_SNAN        0xffF0000000000001LL
#define F64_POS_SNAN_Q      0x7FF8000000000001LL
#define F64_NEG_SNAN_Q      0xffF8000000000001LL
#define F64_POS_QNAN        0x7FF8000000000000LL
#define F64_NEG_QNAN        0xFFF8000000000000LL
#define F64_POS_INF         0x7fF0000000000000LL
#define F64_NEG_INF         0xffF0000000000000LL
#define F64_POS_ONE         0x3fF0000000000000LL
#define F64_NEG_ONE         0xbfF0000000000000LL
#define F64_POS_ZERO        0x0000000000000000LL
#define F64_NEG_ZERO        0x8000000000000000LL

#define F64_POS_HDENORM     0x000FFFFFFFFFFFFFLL
#define F64_NEG_HDENORM     0x800fffffFFFFFFFFLL
#define F64_POS_LDENORM     0x0000000000000001LL
#define F64_NEG_LDENORM     0x8000000000000001LL

#define F64_POS_HNORMAL     0x7FEfffffFFFFFFFFLL
#define F64_NEG_HNORMAL     0xfFEfffffFFFFFFFFLL
#define F64_POS_LNORMAL     0x0010000000000000LL
#define F64_NEG_LNORMAL     0x8010000000000000LL

#define F64_POS_PI          0x400921FB60000000LL
#define F64_NEG_PI          0xC00921FB60000000LL
#define F64_POS_PIBY2       0x3FF921FB60000000LL
#define F64_NEG_PIBY2       0xBFF921FB60000000LL


struct ExcFlags
{
unsigned int us : 1;
unsigned int os : 1;
unsigned int zs : 1;
unsigned int is : 1;

unsigned int other_s : 4;
};




#ifndef STRTVAL
#define STRTVAL 0
#endif

#ifndef ENDVAL
#define ENDVAL 1000
#endif

#ifndef STRTVAL1
#define STRTVAL1 0
#endif

#ifndef ENDVAL1
#define ENDVAL1 1000
#endif

#ifndef STRTVAL2
#define STRTVAL2 0
#endif

#ifndef ENDVAL2
#define ENDVAL2 1000
#endif


#ifndef STRTVAL3
#define STRTVAL3 0
#endif

#ifndef ENDVAL3
#define ENDVAL3 1000
#endif



#ifndef A_ULP
#define A_ULP 0
#endif

#ifndef T_SIZE
#define T_SIZE 1000
#endif


#ifndef ALLTESTS
#define ALLTESTS 0
#endif

#ifndef REF_FUNCTION
#define REF_FUNCTION std::FN_NAME
#endif


#define INCLUDE_EXP(x) #x
#define INCLUDE(x) INCLUDE_EXP(x)

#define LENGTH_OF(x) (sizeof(x)/sizeof(x[0]))

using namespace Concurrency;
using namespace Concurrency::precise_math;


#if TP == 1

typedef unsigned long long T_U;
#define DT_TYPE double

inline bool isNaN(T_U x)
{
    T_U ax = x & 0x7fffffffffffffffLL;
    return ( (ax > 0x7ff0000000000000LL) ? true : false );
}


#elif TP == 2

typedef unsigned int T_U;
#define DT_TYPE float

inline bool isNaN(T_U x)
{
    T_U ax = x & 0x7fffffff;
    return ( (ax > 0x7f800000) ? true : false );
}

#endif

typedef union
{
    T_U  ln;
    DT_TYPE d;
}DoubletoHex;



int random_generator()
{
   return rand();
}

double randomNumber(int x)
{

    switch(x)
    {
        case 8:
            {
            unsigned long long int  ml = ( unsigned long long int)(random_generator());
            unsigned long long int mh = ( unsigned long long int)(random_generator());

            unsigned long long int m = ((mh << 32) | ml);
            unsigned long long int e = ( unsigned long long int)(random_generator()%2048);
            unsigned long long int s = ( unsigned long long int)(random_generator()%2);

            m = m & 0x000fffffffffffffLL;
            e = e << 52;
            s = s << 63;

            unsigned long long int num = 0;
            num = num | m;
            num = num | e;
            num = num | s;

            DoubletoHex temp;
            temp.ln = num;
            return temp.d;
            }
        case 4:
            {
                    unsigned int m = random_generator();
                    unsigned int e = random_generator()%256;
                    unsigned int s = random_generator()%2;

                    m = m & 0x007fffff;
                    e = e << 23;
                    s = s << 31;

                    unsigned int num = 0;
                    num = num | m;
                    num = num | e;
                    num = num | s;

                    DoubletoHex temp;
                    temp.ln = num;
                    return temp.d;
            }
    }
    return 0;

}



