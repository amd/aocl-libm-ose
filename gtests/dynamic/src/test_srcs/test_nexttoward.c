#include "libm_dynamic_load.h"

int test_nexttoward(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;
    long double input1 = 5.678;

    #if defined(_WIN64) || defined(_WIN32)
        float (*func_f)(float, long double) = (float (*)(float, long double))GetProcAddress(handle, "amd_nexttowardf");
        double (*func_d)(double, long double) = (double (*)(double, long double))GetProcAddress(handle, "amd_nexttoward");

        error = GetLastError();
    #else
        float (*func_f)(float, long double) = (float (*)(float, long double))dlsym(handle, "amd_nexttowardf");
        double (*func_d)(double, long double) = (double (*)(double, long double))dlsym(handle, "amd_nexttoward");

        error = dlerror();
    #endif

    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nexttoward routines\n");
    outputf = func_f(inputf, input1);
    printf("amd_nexttowardf(%f, %Lf) = %f\n", (double)inputf, (long double)input1, (double)outputf);
    output = func_d(input, input1);
    printf("amd_nexttoward(%lf, %Lf) = %lf\n", input, input1, output);

    return 0;
}
