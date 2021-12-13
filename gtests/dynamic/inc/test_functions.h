#include "utils.h"

int test_s1f    (funcf,   const char*);
int test_s1d    (func,    const char*);
int test_s1f_2  (funcf_2, const char*);
int test_s1d_2  (func_2,  const char*);

int test_v2d    (func_v2d,     const char*);
int test_v2d_2  (func_v2d_2,   const char*);

int test_v4s    (funcf_v4s,    const char*);
int test_v4s_2  (funcf_v4s_2,  const char*);

int test_v4d    (func_v4d,     const char*);
int test_v4d_2  (func_v4d_2,   const char*);

int test_v8s    (funcf_v8s,    const char*);
int test_v8s_2  (funcf_v8s_2,  const char*);

int test_vas    (funcf_va,     const char*);
int test_vas_2  (funcf_va_2,   const char*);
int test_vad    (func_va,      const char*);
int test_vad_2  (func_va_2,    const char*);

#if defined(__AVX512__)
int test_v8d    (func_v8d,     const char*);
int test_v8d_2  (func_v8d_2,   const char*);
int test_v16s   (funcf_v16s,   const char*);
int test_v16s_2 (funcf_v16s_2, const char*);
#endif
