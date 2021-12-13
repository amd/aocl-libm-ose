#include "libm_dynamic_load.h"

int test_frexp(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;
    int exponent = 2;

    #if defined(_WIN64) || defined(_WIN32)
        float (*func_f)(float, int*) = (float (*)(float, int*))GetProcAddress(handle, "amd_frexpf");
        double (*func_d)(double, int*) = (double (*)(double, int*))GetProcAddress(handle, "amd_frexp");

        error = GetLastError();
    #else
        float (*func_f)(float, int*) = (float (*)(float, int*))dlsym(handle, "amd_frexpf");
        double (*func_d)(double, int*) = (double (*)(double, int*))dlsym(handle, "amd_frexp");

        error = dlerror();
    #endif

        if (error != NULL) {
            printf("Error: %s\n", error);
            return 1;
        }

    printf("Exercising frexp routines\n");
    outputf = func_f(inputf, &exponent);
    printf("amd_frexpf(%f) = %f\n", (double)inputf, (double)outputf);
    output = func_d(input, &exponent);
    printf("amd_frexp(%lf) = %lf\n", input, output);

    return 0;
}
