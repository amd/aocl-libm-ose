#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_log2(void* handle) {
    char* error;
    int dim=5, loopCount=10;
    int array_size = dim * loopCount;

    funcf     s1f = (funcf)dlsym(handle, "amd_log2f");
    func      s1d = (func)dlsym(handle, "amd_log2");
    func_v2d  v2d = (func_v2d)dlsym(handle, "amd_vrd2_log2");
    funcf_v4s v4s = (funcf_v4s)dlsym(handle, "amd_vrs4_log2f");
    funcf_va  vas = (funcf_va)dlsym(handle, "amd_vrsa_log2f");
    func_va   vad = (func_va)dlsym(handle, "amd_vrda_log2");

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

    printf("Exercising log2 routines\n");
    /*scalar*/
    outputf = s1f(inputf);
    printf("amd_log2f(%f) = %f\n", inputf, outputf);
    output = s1d(input);
    printf("amd_log2(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = v2d(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_log2([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = v4s(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_log2([%f, %f] = [%f, %f])\n",
            input_array_vrs4[0], input_array_vrs4[1],
            output_array_vrs4[0], output_array_vrs4[1]);

    /*vector array*/
    printf("amd_vrsa_log2f\nInput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%f\t", input_arrayf[i]);
    }
    for (int i = 0; i < loopCount; i++) {
        vas(dim, input_arrayf + i*dim, output_arrayf + i*dim);
    }
    printf("\nOutput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%f\t", output_arrayf[i]);
    }

    printf("amd_vrda_log2\nInput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%lf\t", input_arrayd[i]);
    }

    for (int i = 0; i < loopCount; i++) {
        vad(dim, input_arrayd + i*dim, output_arrayd + i*dim);
    }

    printf("\nOutput:\n");
    for (int i = 0; i < array_size; i++) {
        printf("%lf\t", output_arrayd[i]);
    }

    printf("\n");

    return 0;
}
