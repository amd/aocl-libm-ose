#include "libm_dynamic_load.h"

int test_remquo(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;
    int quo;

    float (*funcf)(float, float, int*) = (float (*)(float, float, int*))dlsym(handle, "amd_remquof");
    double (*func)(double, double, int*) = (double (*)(double, double, int*))dlsym(handle, "amd_remquo");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising remquo routines\n");
    outputf = funcf(inputf, inputf, &quo);
    printf("amd_remquof(%f, %f, %u) = %f\n", inputf, inputf, quo, outputf);
    output = func(input, input, &quo);
    printf("amd_remquo(%lf, %lf, %u) = %lf\n", input, input, quo, output);

    return 0;
}
