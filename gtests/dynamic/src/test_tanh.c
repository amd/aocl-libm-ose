#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_tanh(void* handle) {
    char* error;
    int i;

    funcf     s1f = (funcf)dlsym(handle, "amd_tanhf");
    func      s1d = (func)dlsym(handle, "amd_tanh");
    funcf_v4s v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_tanhf");
    funcf_v8s v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_tanhf");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;
    /*for vector routines*/
    __m128  ip_vrs4, op_vrs4;
    __m256  ip_vrs8, op_vrs8;

    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2f, 2.3f, 5.6f, 50.3f,
                                -50.45f, 45.3f, 23.4f, 4.5f};
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
    outputf = s1f(inputf);
    printf("amd_tanhf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_tanh(%lf) = %lf\n", input, output);

    /*vrs4*/
    op_vrs4 = v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_tanh([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);
    /*vrs8*/
    op_vrs8 = v8s(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_tanhf\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",(double)input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",(double)output_array_vrs8[i]);
    printf("\n");

    return 0;
}
