#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_pow(void* handle) {
    char* error;
    int i;
    int dim=5, loopCount=10;
    int array_size = dim * loopCount;

    float (*funcf)(float, float) = (float (*)(float, float))dlsym(handle, "amd_powf");
    double (*func)(double, double) = (double (*)(double, double))dlsym(handle, "amd_pow");
    __m128d (*func_v2d)(__m128d, __m128d) = (__m128d (*)(__m128d, __m128d))dlsym(handle, "amd_vrd2_pow");
    __m128 (*funcf_v4s)(__m128, __m128) = (__m128 (*)(__m128, __m128))dlsym(handle, "amd_vrs4_powf");
    __m256d (*func_v4d)(__m256d, __m256d) = (__m256d (*)(__m256d, __m256d))dlsym(handle, "amd_vrd4_pow");
    __m256 (*funcf_v8s)(__m256, __m256) = (__m256 (*)(__m256, __m256))dlsym(handle, "amd_vrs8_powf");
    void (*funcf_va)(int, float*, float*, float*) = (void (*)(int, float*, float*, float*))dlsym(handle, "amd_vrsa_powf");

    /*scalar inputs*/
    float  inputf = 3.145, outputf;
    double input = 6.287,  output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    __m256  ip_vrs8, op_vrs8;

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

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5, 1.2, 3.4, 0.5};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2, 2.3, 5.6, 50.3,
                                -50.45, 45.3, 23.4, 4.5};
    float output_array_vrs8[8];

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercipowg pow routines\n");
    /*scalar*/
    outputf = funcf(inputf, inputf);
    printf("amd_powf(%f) = %f\n", inputf, outputf);
    output = func(input, input);
    printf("amd_pow(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = func_v2d(ip_vrd2, ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_pow([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = funcf_v4s(ip_vrs4, ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_pow([%f, %f] = [%f, %f])\n",
            input_array_vrs4[0], input_array_vrs4[1],
            output_array_vrs4[0], output_array_vrs4[1]);

    /*vrd4*/
    op_vrd4 = func_v4d(ip_vrd4, ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("amd_vrd4_pow([%lf,%lf,%lf,%lf]) = [%lf,%lf,%lf,%lf])\n",
            input_array_vrd4[0], input_array_vrd4[1],
            input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1],
            output_array_vrd4[2], output_array_vrd4[3]);

    /*vrs8*/
    op_vrs8 = funcf_v8s(ip_vrs8, ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_powf\ninput:\n");
    for(i=0; i<8; i++) {
        printf("%f\t",input_array_vrs8[i]);
    }
    printf("\nOutput:\n");
    for(i=0; i<8; i++) {
        printf("%f\t",output_array_vrs8[i]);
    }

    /*vector array*/
    printf("amd_vrsa_powf\nInput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%f\t", input_arrayf[i]);
    }
    for (int i = 0; i < loopCount; i++) {
        funcf_va(dim, input_arrayf + i*dim, input_arrayf + i*dim, output_arrayf + i*dim);
    }
    printf("\nOutput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%f\t", output_arrayf[i]);
    }
    printf("\n");

    return 0;
}
