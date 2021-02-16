#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_tanh(void* handle) {
    char* error;
    int i;

    float (*funcf)(float) = (float (*)(float))dlsym(handle, "amd_tanhf");
    double (*func)(double) = (double (*)(double))dlsym(handle, "amd_tanh");
    __m128 (*funcf_v4s)(__m128) = (__m128 (*)(__m128))dlsym(handle, "amd_vrs4_tanhf");
    __m256 (*funcf_v8s)(__m256) = (__m256 (*)(__m256))dlsym(handle, "amd_vrs8_tanhf");

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    /*for vector routines*/
    __m128  ip_vrs4, op_vrs4;
    __m256  ip_vrs8, op_vrs8;

    float input_array_vrs4[4] = {3.5, 1.2, 3.4, 0.5};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2, 2.3, 5.6, 50.3,
                                -50.45, 45.3, 23.4, 4.5};
    float output_array_vrs8[8];

    /*packed inputs*/
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising tanh routines\n");
    /*scalar*/
    outputf = funcf(inputf);
    printf("amd_tanhf(%f) = %f\n", inputf, outputf);
    output = func(input);
    printf("amd_tanh(%lf) = %lf\n", input, output);

    /*vrs4*/
    op_vrs4 = funcf_v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_tanh([%f, %f] = [%f, %f])\n",
            input_array_vrs4[0], input_array_vrs4[1],
            output_array_vrs4[0], output_array_vrs4[1]);

    /*vrs8*/
    op_vrs8 = funcf_v8s(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_tanhf\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",output_array_vrs8[i]);

    printf("\n");

    return 0;
}
