#include "libm_dynamic_load.h"

int test_remquo(void* handle) {
    char* error;
    /*scalar inputs*/
    float inputf = 3.145f, outputf;
    double input = 6.287, output;
    int quo;

    #if defined(_WIN64) || defined(_WIN32)
        funcf_remquo s1f = (funcf_remquo)GetProcAddress(handle, "amd_remquof");
        func_remquo  s1d = (func_remquo)GetProcAddress(handle, "amd_remquo");

        error = GetLastError();
    #else
        funcf_remquo s1f = (funcf_remquo)dlsym(handle, "amd_remquof");
        func_remquo  s1d = (func_remquo)dlsym(handle, "amd_remquo");

        error = dlerror();
    #endif

    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising remquo routines\n");
    outputf = s1f(inputf, inputf, &quo);
    printf("amd_remquof(%f, %f, %u) = %f\n", (double)inputf, (double)inputf, quo, (double)outputf);
    output = s1d(input, input, &quo);
    printf("amd_remquo(%lf, %lf, %u) = %lf\n", input, input, quo, output);

    return 0;
}
