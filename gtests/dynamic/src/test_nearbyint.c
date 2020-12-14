#include "libm_dynamic_load.h"

int test_nearbyint(void* handle) {
    char* error;
    float (*lamd_nearbyintf)(float);
    double (*lamd_nearbyint)(double);
    /*scalar inputs*/
    float inputf = 3.145, outputf;
    double input = 6.287, output;

    /*scalar routines*/
    lamd_nearbyintf = dlsym(handle, "amd_nearbyintf");
    lamd_nearbyint  = dlsym(handle, "amd_nearbyint");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising nearbyint routines\n");

    /*scalar*/
    outputf = (*lamd_nearbyintf)(inputf);
    printf("amd_nearbyintf(%f) = %f\n", inputf, outputf);
    output = (*lamd_nearbyint)(input);
    printf("amd_nearbyint(%lf) = %lf\n", input, output);
    printf("\n");

    return 0;
}
