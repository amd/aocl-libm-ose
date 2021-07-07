#include "libm_dynamic_load.h"

#if defined(__AVX512__)
#include "libm_dynamic_load_avx512.h"
#endif

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_pow(void* handle) {
    char* error;
    long unsigned int i=0, dim = 5, loopCount = 10;
    long unsigned int array_size = dim * loopCount;

    funcf_2     s1f = (funcf_2)dlsym(handle, "amd_powf");
    func_2      s1d = (func_2)dlsym(handle, "amd_pow");
    func_v2d_2  v2d = (func_v2d_2)dlsym(handle, "amd_vrd2_pow");
    func_v4d_2  v4d = (func_v4d_2)dlsym(handle, "amd_vrd4_pow");
    funcf_v4s_2 v4s = (funcf_v4s_2)dlsym(handle, "amd_vrs4_powf");
    funcf_v8s_2 v8s = (funcf_v8s_2)dlsym(handle, "amd_vrs8_powf");
    funcf_va_2  vas = (funcf_va_2)dlsym(handle, "amd_vrsa_powf");

    /*avx512*/
    #if defined(__AVX512__)
    func_v8d_2  v8d = (func_v8d_2)dlsym(handle, "amd_vrd8_pow");
    funcf_v16s_2 v16s = (funcf_v16s_2)dlsym(handle, "amd_vrs16_powf");
    #endif

    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    __m256  ip_vrs8, op_vrs8;

    /*avx512*/
    #if defined(__AVX512__)
    __m512d ip_vrd8, op_vrd8;
    __m512  ip_vrs16, op_vrs16;
    #endif

    //array vector inputs
    float *input_arrayf   = (float *)  malloc(sizeof(float) * array_size);
    float *output_arrayf  = (float *)  malloc(sizeof(float) * array_size);

    for (i = 0; i < array_size; i++) {
         input_arrayf[i] = RANGEF;
    }

    for (i = 0; i < array_size; i++) {
        output_arrayf[0] += output_arrayf[i];
    }

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2f, 2.3f, 5.6f, 50.3f,
                                -50.4f, 45.3f, 23.4f, 4.5f};
    float output_array_vrs8[8];

    #if defined(__AVX512__)
    double input_array_vrd8[8] = {0.0, 1.1, 3.6, 2.8, 0.0, 1.1, 3.6, 2.8};
    double output_array_vrd8[8];

    float input_array_vrs16[16] = {0.0f, 1.1f, 3.6f, 2.8f, 0.0f, 1.1f, 3.6f, 2.8f,
                                   0.0f, 1.1f, 3.6f, 2.8f, 0.0f, 1.1f, 3.6f, 2.8f};
    float output_array_vrs16[16];
    #endif

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    #if defined(__AVX512__)
    ip_vrd8 = _mm512_loadu_pd(input_array_vrd8);
    ip_vrs16 = _mm512_loadu_ps(input_array_vrs16);
    #endif

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising pow routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_powf(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input, input);
    printf("amd_pow(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = v2d(ip_vrd2, ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_pow([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = v4s(ip_vrs4, ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_pow([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);

    /*vrd4*/
    op_vrd4 = v4d(ip_vrd4, ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("amd_vrd4_pow([%f, %f]) = [%f, %f])\n",
           input_array_vrd4[0], input_array_vrd4[1],
           input_array_vrd4[2], input_array_vrd4[3]);

    /*vrs8*/
    op_vrs8 = v8s(ip_vrs8, ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_powf\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",(double)input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",(double)output_array_vrs8[i]);

    /*vector array*/
    printf("amd_vrsa_powf\nInput:\n");
    for (i = 0; i < array_size; i++) {
        printf("%f\t", (double)input_arrayf[i]);
    }
    for (i = 0; i < loopCount; i++) {
        vas((int)dim, input_arrayf + i*dim,
                input_arrayf + i*dim,
                output_arrayf + i*dim);
    }
    printf("\nOutput:\n");
    for (i = 0; i < array_size; i++) {
        printf("%f\t", (double)output_arrayf[i]);
    }
    printf("\n");

    #if defined(__AVX512__)
    printf("amd_vrd8_pow\ninput:\n");
    op_vrd8 = v8d(ip_vrd8, ip_vrd8);
    _mm512_storeu_pd(output_array_vrd8, op_vrd8);
    printf("amd_vrd8_pow\ninput:\n");
    for(i=0; i<8; i++)
        printf("%lf\t",(double)input_array_vrd8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%lf\t",(double)output_array_vrd8[i]);

    printf("amd_vrs16_powf\ninput:\n");
    op_vrs16 = v16s(ip_vrs16, ip_vrs16);
    _mm512_storeu_ps(output_array_vrs16, op_vrs16);
    for(i=0; i<16; i++)
        printf("%f\t",(double)input_array_vrs16[i]);
    printf("\nOutput\n");
    for(i=0; i<16; i++)
        printf("%f\t",(double)output_array_vrs16[i]);
    #endif

    return 0;
}
