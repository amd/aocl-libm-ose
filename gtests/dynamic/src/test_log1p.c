#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_log1p(void* handle) {
    char* error;
    int i;
    int dim=5, loopCount=10;
    int array_size = dim * loopCount;

    float (*lamd_log1pf)(float);
    double (*lamd_log1p)(double);
    __m128d (*lamd_vrd2_log1p)  (__m128d);
    __m128  (*lamd_vrs4_log1pf) (__m128);
    //__m256d (*lamd_vrd4_log1p)  (__m256d);
    //__m256  (*lamd_vrs8_log1pf) (__m256);

    //array vector
    void (*lamd_vrda_log1p) (int, double*, double*);
    void (*lamd_vrsa_log1pf) (int, float* , float*);

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    //__m256d ip_vrd4, op_vrd4;
    //__m256  ip_vrs8, op_vrs8;

    //array vector inputs
    float *input_arrayf   = (float *)  malloc(sizeof(float) * array_size);
    float *output_arrayf  = (float *)  malloc(sizeof(float) * array_size);
    double *input_arrayd  = (double *) malloc(sizeof(double) * array_size);
    double *output_arrayd = (double *) malloc(sizeof(double) * array_size);

    for (unsigned int i = 0; i < array_size; i++) {
         input_arrayf[i] = RANGEF;
	 input_arrayd[i] = RANGED;
    }

    for (unsigned int i = 0; i < array_size; i++) {
        output_arrayf[0] += output_arrayf[i];
	output_arrayd[0] += output_arrayd[i];
    }

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    //double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    //double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5, 1.2, 3.4, 0.5};
    float output_array_vrs4[4];

    //float input_array_vrs8[8] = {1.2, 2.3, 5.6, 50.3,
    //                            -50.45, 45.3, 23.4, 4.5};
    //float output_array_vrs8[8];

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    //ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    //ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    /*scalar routines*/
    lamd_log1pf = dlsym(handle, "amd_log1pf");
    lamd_log1p  = dlsym(handle, "amd_log1p");
    /*vector routines*/
    lamd_vrd2_log1p  = dlsym(handle, "amd_vrd2_log1p");
    lamd_vrs4_log1pf = dlsym(handle, "amd_vrs4_log1pf");
    //lamd_vrd4_log1p  = dlsym(handle, "amd_vrd4_log1p");
    //lamd_vrs8_log1pf = dlsym(handle, "amd_vrs8_log1pf");
    //vector array variants
    lamd_vrsa_log1pf = dlsym(handle, "amd_vrsa_log1pf");
    lamd_vrda_log1p  = dlsym(handle, "amd_vrda_log1p");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercilog1pg log1p routines\n");
    /*scalar*/
    outputf = (*lamd_log1pf)(inputf);
    printf("amd_log1pf(%f) = %f\n", inputf, outputf);
    output = (*lamd_log1p)(input);
    printf("amd_log1p(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = (*lamd_vrd2_log1p)(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_log1p([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = (*lamd_vrs4_log1pf)(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_log1p([%f, %f] = [%f, %f])\n",
            input_array_vrs4[0], input_array_vrs4[1],
            output_array_vrs4[0], output_array_vrs4[1]);

    /*vrd4*/
    /*
    op_vrd4 = (*lamd_vrd4_log1p)(ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("amd_vrd4_log1p([%lf,%lf,%lf,%lf]) = [%lf,%lf,%lf,%lf])\n",
            input_array_vrd4[0], input_array_vrd4[1],
            input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1],
            output_array_vrd4[2], output_array_vrd4[3]);
    */
    /*vrs8*/
    /*
    op_vrs8 = (*lamd_vrs8_log1pf)(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_log1pf\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",output_array_vrs8[i]);
    */
    /*vector array*/
    printf("amd_vrsa_log1pf\nInput:\n");
    for (unsigned int i = 0; i < array_size; i++) {
        printf("%f\t", input_arrayf[i]);
    }
    for (unsigned int i = 0; i < loopCount; i++) {
        (*lamd_vrsa_log1pf)(dim, input_arrayf + i*dim, output_arrayf + i*dim);
    }
    printf("\nOutput:\n");
    for (unsigned int i = 0; i < array_size; i++) {
        printf("%f\t", output_arrayf[i]);
    }

    printf("amd_vrda_log1p\nInput:\n");
    for (unsigned int i = 0; i < array_size; i++) {
        printf("%lf\t", input_arrayd[i]);
    }

    for (unsigned int i = 0; i < loopCount; i++) {
        (*lamd_vrda_log1p)(dim, input_arrayd + i*dim, output_arrayd + i*dim);
    }

    printf("\nOutput:\n");
    for (unsigned int i = 0; i < array_size; i++) {
        printf("%lf\t", output_arrayd[i]);
    }

    printf("\n");

    return 0;
}