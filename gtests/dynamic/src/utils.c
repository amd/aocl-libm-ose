#include "libm_dynamic_load.h"
#include "test_functions.h"

float GenerateRangeFloat(float min, float max) {
    float range = (max - min);
    float div = (float)RAND_MAX/(float)range;
    return min + ((float)rand() / div);
}

double GenerateRangeDouble(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

/* used for getting full file path */
char *concatenate(const char *a, const char *b, const char *c) {
    char* temp = NULL;
    temp = (char*)malloc(strlen(a) + strlen(b) + strlen(c) + 1);
    if (temp == NULL) {
        printf("Failed to allocate in concatenate() function");
        return NULL;
    }
    return strcat(strcat(strcpy(temp, a), b), c);
}

int test_func(void* handle, struct FuncData * data, const char * func_name) {
    /* by default these wil be null */
    funcf s1f = data->s1f;
    func s1d = data->s1d;
    funcf_2 s1f_2 = data->s1f_2;
    func_2 s1d_2 = data->s1d_2;

    /* vector double */
    func_v2d v2d = data->v2d;
    func_v2d_2 v2d_2 = data->v2d_2;
    func_v4d v4d = data->v4d;
    func_v4d_2 v4d_2 = data->v4d_2;

    /* vector float */
    funcf_v4s v4s = data->v4s;
    funcf_v4s_2 v4s_2 = data->v4s_2;
    funcf_v8s v8s = data->v8s;
    funcf_v8s_2 v8s_2 = data->v8s_2;

    /* vector array */
    funcf_va vas = data->vas;
    funcf_va_2 vas_2 = data->vas_2;
    func_va vad = data->vad;
    func_va_2 vad_2 = data->vad_2;

    #if defined(__AVX512__)
    /* vector avx512 */
    funcf_v16s v16s = data->v16s;
    func_v8d v8d = data->v8d;
    func_v8d_2 v8d_2 = data->v8d_2;
    funcf_v16s_2 v16s_2 = data->v16s_2;
    #endif

    printf("Exercising %s routines\n", func_name);
    /*scalar*/
    test_s1f(s1f, func_name);
    test_s1d(s1d, func_name);
    test_s1f_2(s1f_2, func_name);
    test_s1d_2(s1d_2, func_name);
    test_v2d(v2d, func_name);
    test_v2d_2(v2d_2, func_name);
    test_v4s(v4s, func_name);
    test_v4s_2(v4s_2, func_name);
    test_v4d(v4d, func_name);
    test_v4d_2(v4d_2, func_name);
    test_v8s(v8s, func_name);
    test_v8s_2(v8s_2, func_name);

    test_vas(vas, func_name);
    test_vas_2(vas_2, func_name);
    test_vad(vad, func_name);
    test_vad_2(vad_2, func_name);

    #if defined(__AVX512__)
    test_v8d(v8d, func_name);
    test_v8d_2(v8d_2, func_name);
    test_v16s(v16s, func_name);
    test_v16s_2(v16s_2, func_name);
    #endif

    return 0;
}

