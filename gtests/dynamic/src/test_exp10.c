#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_exp10(void* handle) {
    char* error;
    int dim=5, loopCount=10;
    int array_size = dim * loopCount;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_exp10f");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_exp10");
    __m128d (*func_v2d)(__m128d) = (__m128d (*)(__m128d))dlsym(handle, "amd_vrd2_exp10");
    __m128 (*funcf_v4s)(__m128) = (__m128 (*)(__m128))dlsym(handle, "amd_vrs4_exp10f");
    void (*funcf_va)(int, float*, float*) = (void (*)(int, float*, float*))dlsym(handle, "amd_vrsa_exp10f");
    void (*func_va)(int, double*, double*) = (void (*)(int, double*, double*))dlsym(handle, "amd_vrda_exp10f");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;

    //array vector inputs
    float *input_arrayf   = (float *)  malloc(sizeof(float) * array_size);
    float *output_arrayf  = (float *)  malloc(sizeof(float) * array_size);
    double *input_arrayd  = (double *) malloc(sizeof(double) * array_size);
    double *output_arrayd = (double *) malloc(sizeof(double) * array_size);

    for (int i = 0; i < array_size; i++) {
         input_arrayf[i] = RANGEF;
	     input_arrayd[i] = RANGED;
    }

    for (int i = 0; i < array_size; i++) {
        output_arrayf[0] += output_arrayf[i];
	    output_arrayd[0] += output_arrayd[i];
    }

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];
    float input_array_vrs4[4] = {3.5, 1.2, 3.4, 0.5};
    float output_array_vrs4[4];

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising exp10 routines\n");
    /*scalar*/
    outputf = funcf(inputf);
    printf("amd_exp10f(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_exp10(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = func_v2d(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_exp10([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = funcf_v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_exp10([%f, %f] = [%f, %f])\n",
            input_array_vrs4[0], input_array_vrs4[1],
            output_array_vrs4[0], output_array_vrs4[1]);

    /*vector array*/
    printf("amd_vrsa_exp10f\nInput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%f\t", input_arrayf[i]);
    }
    for (int i = 0; i < loopCount; i++) {
        funcf_va(dim, input_arrayf + i*dim, output_arrayf + i*dim);
    }
    printf("\nOutput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%f\t", output_arrayf[i]);
    }

    printf("amd_vrda_exp10\nInput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%lf\t", input_arrayd[i]);
    }

    for (int i = 0; i < loopCount; i++) {
        func_va(dim, input_arrayd + i*dim, output_arrayd + i*dim);
    }

    printf("\nOutput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%lf\t", output_arrayd[i]);
    }

    printf("\n");

    return 0;
}
