#include "libm_glibc_compat.h"

int test_log(void* handle) {
    __m128d ip_vrd2, op_vrd2;
    __m128  ip_vrs4, op_vrs4;
    __m256d ip_vrd4, op_vrd4;
    __m256  ip_vrs8, op_vrs8;
    int i;

    double input_array_vrd2[2] = {1.2, 3.5};
    double output_array_vrd2[2];

    double input_array_vrd4[4] = {0.0, 1.1, 3.6, 2.8};
    double output_array_vrd4[4];

    float input_array_vrs4[4] = {3.5f, 1.2f, 3.4f, 0.5f};
    float output_array_vrs4[4];

    float input_array_vrs8[8] = {1.2f, 2.3f, 5.6f, 50.3f,
                                -50.45f, 45.3f, 23.4f, 4.5f};
    float output_array_vrs8[8];

    /*packed inputs*/
    ip_vrd2 = _mm_loadu_pd(input_array_vrd2);
    ip_vrs4 = _mm_loadu_ps(input_array_vrs4);
    ip_vrd4 = _mm256_loadu_pd(input_array_vrd4);
    ip_vrs8 = _mm256_loadu_ps(input_array_vrs8);

    log_v2d = dlsym(handle, "_ZGVbN2v_log");
    logf_v4s = dlsym(handle, "_ZGVbN4v_logf");
    log_v4d  = dlsym(handle, "_ZGVdN4v_log");
    logf_v8s = dlsym(handle, "_ZGVdN8v_logf");

    if (log_v2d == NULL || logf_v4s == NULL ||
        log_v4d == NULL || logf_v8s == NULL) {
        printf("Unable to find log symbols\n");
        return 1;
    }

    /*vrd2*/
    op_vrd2 = (*log_v2d)(ip_vrd2);
    _mm_storeu_pd(output_array_vrd2, op_vrd2);
    printf("log_v2d([%lf, %lf] = [%lf, %lf])\n",
            input_array_vrd2[0], input_array_vrd2[1],
            output_array_vrd2[0], output_array_vrd2[1]);
    /*vrs4*/
    op_vrs4 = (*logf_v4s)(ip_vrs4);
    _mm_storeu_ps(output_array_vrs4, op_vrs4);
    printf("logf_v4s([%f, %f] = [%f, %f])\n",
            (double)input_array_vrs4[0], (double)input_array_vrs4[1],
            (double)output_array_vrs4[0], (double)output_array_vrs4[1]);
    /*vrd4*/
    op_vrd4 = (*log_v4d)(ip_vrd4);
    _mm256_storeu_pd(output_array_vrd4, op_vrd4);
    printf("log_v4d\nInput:\n");
    for(i=0;i<4;i++)
        printf("%lf\t", input_array_vrd4[i]);
    printf("\nOutput:\n");
    for(i=0;i<4;i++)
         printf("%lf\t", output_array_vrd4[i]);
    /*vrs8*/
    op_vrs8 = (*logf_v8s)(ip_vrs8);
    _mm256_storeu_ps(output_array_vrs8, op_vrs8);
    printf("\nlogf_v8s\ninput:\n");
    for(i=0; i<8; i++)
        printf("%f\t", (double)input_array_vrs8[i]);
    printf("\nOutput:\n");
    for(i=0; i<8; i++)
        printf("%f\t", (double)output_array_vrs8[i]);

    return 0;
}

