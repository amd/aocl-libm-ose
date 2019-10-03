#include <fenv.h>

struct __exp_exception_test_data {
        uint64_t input, output;
        int32_t exception_flags;
};

/* Test cases to check for exceptions for the exp() routine. These test cases are not exhaustive */
static struct __exp_exception_test_data libm_test_exp_conformance_data[] = {
        {0x0,0x3ff0000000000000,0}, //exp(0)
        {0x7ff0000000000000,0x7ff0000000000000,0}, //exp(infinity)
        {0xfff0000000000000,0x0,0}, //exp(-inf)
        {0x7FF4001000000000,0x7ffc001000000000,FE_INVALID}, //exp(snan)
        {0x40862e42fefa39ef,0x7fefffffffffff2a,FE_INEXACT}, //exp(ln(2)*1024)
        {0x4086300000000000,0x7ff0000000000000,40}, //exp(x>ln(2)*1024)


};

