#include "libm_dynamic_load.h"

/*Inputs: num and deno, quo will be saved in the
  int ptr, and return val will be the remainder*/

int test_remquo(void* handle) {
    char* error;
    float (*lamd_remquof)(float, float, int*);
    double (*lamd_remquo)(double, double, int*);
    /*scalar inputs*/
    float input1f = 3.145,  input2f = 1.34,  outputf;
    double input1d = 6.287, input2d = 4.56,  outputd;
    int quo;

    /*scalar routines*/
    lamd_remquof = dlsym(handle, "amd_remquof");
    lamd_remquo  = dlsym(handle, "amd_remquo");

    error = dlerror();
    if (error != NULL) {
        printf("Error: %s\n", error);
        return 1;
    }

    printf("Exercising remquo routines\n");

    /*scalar*/
    outputf = (*lamd_remquof)(input1f, input2f, &quo);
    printf("amd_remquof(%f, %f) = %f, quotient=%d\n", input1f, input2f, outputf, quo);
    outputd = (*lamd_remquo)(input1d, input2d, &quo);
    printf("amd_remquo(%lf, %lf) = %lf, quotient=%d\n", input1d, input2d, outputd, quo);
    printf("\n");

    return 0;
}
