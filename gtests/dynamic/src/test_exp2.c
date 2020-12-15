#include "libm_dynamic_load.h"

int test_exp2(void* handle) {
    char* error;
    int i;

    float (*lamd_exp2f)(float);
    double (*lamd_exp2)(double);
    __m128d (*lamd_vrd2_exp2)  (__m128d);
    __m128  (*lamd_vrs4_exp2f) (__m128);
    __m256d (*lamd_vrd4_exp2)  (__m256d);
    //__m256  (*lamd_vrs8_exp2f) (__m256);

    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    /*for vector routines*/
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    //__m256  ip_vrs8, op_vrs8;

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5, 1.2, 3.4, 0.5};
    float output_array_vrs4[4];
/*
    float input_array_vrs8[8] = {1.2, 2.3, 5.6, 50.3,
                                -50.45, 45.3, 23.4, 4.5};
    float output_array_vrs8[8];
*/

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    //ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    /*scalar routines*/
    lamd_exp2f = dlsym(handle, "amd_exp2f");
    lamd_exp2  = dlsym(handle, "amd_exp2");
    /*vector routines*/
    lamd_vrd2_exp2  = dlsym(handle, "amd_vrd2_exp2");
    lamd_vrs4_exp2f = dlsym(handle, "amd_vrs4_exp2f");
    lamd_vrd4_exp2  = dlsym(handle, "amd_vrd4_exp2");
    //lamd_vrs8_exp2f = dlsym(handle, "amd_vrs8_exp2f");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising exp2 routines\n");
    /*scalar*/
    outputf = (*lamd_exp2f)(inputf);
    printf("amd_exp2f(%f) = %f\n", inputf, outputf);
    output = (*lamd_exp2)(input);
    printf("amd_exp2(%lf) = %lf\n", input, output);

    /*vrd2*/
    op_vrd2 = (*lamd_vrd2_exp2)(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("amd_vrd2_exp2([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);

    /*vrs4*/
    op_vrs4 = (*lamd_vrs4_exp2f)(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("amd_vrs4_exp2([%f, %f] = [%f, %f])\n",
            input_array_vrs4[0], input_array_vrs4[1],
            output_array_vrs4[0], output_array_vrs4[1]);

    /*vrd4*/
    op_vrd4 = (*lamd_vrd4_exp2)(ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("amd_vrd4_exp2([%lf,%lf,%lf,%lf]) = [%lf,%lf,%lf,%lf])\n",
            input_array_vrd4[0], input_array_vrd4[1],
            input_array_vrd4[2], input_array_vrd4[3],
            output_array_vrd4[0], output_array_vrd4[1],
            output_array_vrd4[2], output_array_vrd4[3]);

    /*vrs8*/
/*
    op_vrs8 = (*lamd_vrs8_exp2f)(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("amd_vrs8_exp2f\ninput:");
    for(i=0; i<8; i++)
        printf("%f\t",input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t",output_array_vrs8[i]);
*/
    printf("\n");

    return 0;
}
