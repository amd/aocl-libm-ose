#include "libm_dynamic_load.h"

int test_modf(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;

    #if defined(_WIN64) || defined(_WIN32)
        float (*func_f)(float, float*) = (float (*)(float, float*))GetProcAddress(handle, "amd_modff");
        double (*func_d)(double, double*) = (double (*)(double, double*))GetProcAddress(handle, "amd_modf");

        error = GetLastError();
    #else
        float (*func_f)(float, float*) = (float (*)(float, float*))dlsym(handle, "amd_modff");
        double (*func_d)(double, double*) = (double (*)(double, double*))dlsym(handle, "amd_modf");

        error = dlerror();
    #endif

    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising modf routines\n");
    outputf = func_f(inputf, &inputf);
    printf("amd_modff(%f) = %f\n", (double)inputf, (double)outputf);
    output = func_d(input, &input);
    printf("amd_modf(%lf) = %lf\n", input, output);

    return 0;
}
