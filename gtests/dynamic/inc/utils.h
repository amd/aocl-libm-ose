#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdlib.h>
#include "func_types.h"
#include <dlfcn.h>
#include "string.h"
#include "stdio.h"

/*utlity functions*/
/* used to load data into array vector function inputs */
float  GenerateRangeFloat   (float  min,  float  max);
double GenerateRangeDouble  (double min,  double max);

/* check error */
int CheckError(void);

/*struct to load func specific stuffs */
struct FuncData {
    /* scalar func types*/
    funcf        s1f   = NULL;
    func         s1d   = NULL;
    funcf_2      s1f_2 = NULL;
    func_2       s1d_2 = NULL;
    /* vector func types */
    func_v2d     v2d   = NULL;
    func_v2d_2   v2d_2 = NULL;
    func_v4d     v4d   = NULL;
    func_v4d_2   v4d_2 = NULL;
    funcf_v4s    v4s   = NULL;
    funcf_v4s_2  v4s_2 = NULL;
    funcf_v8s    v8s   = NULL;
    funcf_v8s_2  v8s_2 = NULL;
    funcf_va     vas   = NULL;
    funcf_va_2   vas_2 = NULL;
    func_va      vad   = NULL;
    func_va_2    vad_2 = NULL;
    #if defined(__AVX512__)
    func_v8d     v8d    = NULL;
    func_v8d_2   v8d_2  = NULL;
    funcf_v16s   v16s   = NULL;
    funcf_v16s_2 v16s_2 = NULL;
    #endif
};

char *concatenate(const char *a, const char *b, const char *c);

int test_func(void* handle, struct FuncData * data, const char * func_name);

#endif
