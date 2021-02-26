#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_tan(void* handle) {
    char* error;
    long unsigned int i=0;
    funcf     s1f = (funcf)dlsym(handle, "amd_tanf");
    func      s1d = (func)dlsym(handle, "amd_tan");
    func_v2d  v2d = (func_v2d)dlsym(handle, "amd_vrd2_tan");
    func_v4d  v4d = (func_v4d)dlsym(handle, "amd_vrd4_tan");
    funcf_v4s v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_tanf");
    funcf_v8s v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_tanf");
    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    __m256  ip_vrs8, op_vrs8;

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2f, 2.3f, 5.6f, 50.3f,
                                -50.4f, 45.3f, 23.4f, 4.5f};
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

    printf("Exercising tan routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_tanf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_tan(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = v2d(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_tan([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_tan([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);

    /*vrd4*/
    op_vrd4 = v4d(ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("amd_vrd4_tan([%lf,%lf,%lf,%lf]) = [%lf,%lf,%lf,%lf])\n",
            input_array_vrd4[0], input_array_vrd4[1],
            input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1],
            output_array_vrd4[2], output_array_vrd4[3]);

    /*vrs8*/
    op_vrs8 = v8s(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_tanf\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",(double)input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",(double)output_array_vrs8[i]);

    printf("\n");

    return 0;
}
