#include "libm_dynamic_load.h"

int test_asin(void* handle) {
    char* error;

    funcf     s1f = (funcf)dlsym(handle, "amd_asinf");
    func      s1d = (func)dlsym(handle, "amd_asin");
    funcf_v4s v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_asinf");

    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;

    /* vector inputs*/
    __m128  ip_vrs4, op_vrs4;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    /*inputs*/
    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    /* packed inputs */
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);

    printf("Exercising asin routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_asinf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_asin(%lf) = %lf\n", input, output);

    /*vrs4*/
    printf("Running asinf v4s\n");
    op_vrs4 = v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_asinf([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);

    return 0;
}
