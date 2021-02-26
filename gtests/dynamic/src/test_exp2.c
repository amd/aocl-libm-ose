#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_exp2(void* handle) {
    char* error;
    long unsigned int i = 0,dim=5, loopCount=10;
    long unsigned int array_size = dim * loopCount;

    funcf     s1f = (funcf)dlsym(handle, "amd_exp2f");
    func      s1d = (func)dlsym(handle, "amd_exp2");
    func_v2d  v2d = (func_v2d)dlsym(handle, "amd_vrd2_exp2");
    func_v4d  v4d = (func_v4d)dlsym(handle, "amd_vrd4_exp2");
    funcf_v4s v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_exp2f");
    //funcf_v8s v8s = (funcf_v8s)dlsym(handle, "amd_vrs8_exp2f");
    funcf_va  vas = (funcf_va)dlsym(handle, "amd_vrsa_exp2f");
    func_va   vad = (func_va)dlsym(handle, "amd_vrda_exp2");

    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    //__m256  ip_vrs8, op_vrs8;

    //array vector inputs
    float *input_arrayf   = (float *)  malloc(sizeof(float) * array_size);
    float *output_arrayf  = (float *)  malloc(sizeof(float) * array_size);
    double *input_arrayd  = (double *) malloc(sizeof(double) * array_size);
    double *output_arrayd = (double *) malloc(sizeof(double) * array_size);

    for (i = 0; i < array_size; i++) {
         input_arrayf[i] = RANGEF;
	     input_arrayd[i] = RANGED;
    }

    for (i = 0; i < array_size; i++) {
        output_arrayf[0] += output_arrayf[i];
	    output_arrayd[0] += output_arrayd[i];
    }

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    //float input_array_vrs8[8] = {1.2, 2.3, 5.6, 50.3,
    //                            -50.45, 45.3, 23.4, 4.5};
    //float output_array_vrs8[8];

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    //ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising exp2 routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_exp2f(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input);
    printf("amd_exp2(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = v2d(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_exp2([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_exp2([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);

    /*vrd4*/
    op_vrd4 = v4d(ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("amd_vrd4_exp2([%lf,%lf,%lf,%lf]) = [%lf,%lf,%lf,%lf])\n",
            input_array_vrd4[0], input_array_vrd4[1],
            input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1],
            output_array_vrd4[2], output_array_vrd4[3]);

    /*vrs8*/
/*
    op_vrs8 = v8s(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_exp2f\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",output_array_vrs8[i]);
*/
    /*vector array*/
    printf("amd_vrsa_exp2f\nInput:\n");
    for (i = 0; i < array_size; i++) {
        printf("%f\t", (double)input_arrayf[i]);
    }
    for (i = 0; i < loopCount; i++) {
        vas((int)dim, input_arrayf + i*dim, output_arrayf + i*dim);
    }
    printf("\nOutput:\n");
    for (i = 0; i < array_size; i++) {
        printf("%f\t", (double)output_arrayf[i]);
    }

    printf("amd_vrda_exp2\nInput:\n");
    for (i = 0; i < array_size; i++) {
        printf("%lf\t", input_arrayd[i]);
    }

    for (i = 0; i < loopCount; i++) {
        vad((int)dim, input_arrayd + i*dim, output_arrayd + i*dim);
    }

    printf("\nOutput:\n");
    for (i = 0; i < array_size; i++) {
        printf("%lf\t", output_arrayd[i]);
    }

    printf("\n");

    return 0;
}
