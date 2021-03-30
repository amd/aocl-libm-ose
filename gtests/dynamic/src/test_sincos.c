#include "libm_dynamic_load.h"

int test_sincos(void* handle) {
    char* error;

    funcf_sincos     s1f = (funcf_sincos)dlsym(handle, "amd_sincosf");
    func_sincos      s1d = (func_sincos)dlsym(handle, "amd_sincos");

    /*scalar inputs*/
    float inputf = 3.14f;
    double input = 6.28;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising sincos routines\n");
    /*scalar*/
    s1f(inputf, &inputf, &inputf);
    printf("sincosf(%f)\n", (double)inputf);
    s1d(input, &input, &input);
    printf("sincos(%lf)\n", input);

    return 0;
}
