#include "libm_dynamic_load.h"

#define RANGEF GenerateRangeFloat(-100.0, 100.0)
#define RANGED GenerateRangeDouble(-100.0, 100.0)

int test_atan2(void* handle) {
    char* error;
    funcf_2     s1f = (funcf_2)dlsym(handle, "amd_atan2f");
    func_2      s1d = (func_2)dlsym(handle, "amd_atan2");
    /*scalar inputs*/
    float inputf = 3.14f, outputf;
    double input = 6.28, output;

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising atan2 routines\n");
    /*scalar*/
    outputf = s1f(inputf, inputf);
    printf("amd_atan2f(%f) = %f\n", (double)inputf, (double)outputf);
    output = s1d(input, input);
    printf("amd_atan2(%lf) = %lf\n", input, output);

    return 0;
}
