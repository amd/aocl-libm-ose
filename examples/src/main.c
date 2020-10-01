/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*
*/

#include <stdio.h>
extern int use_exp();
extern int use_pow();
extern int use_log();
extern int use_fabs();
extern int use_atan();
extern int use_sin();
extern int use_cos();
extern int use_tan();
extern int use_cosh();
extern int use_tanh();
extern int use_sinh();

int main()  {
    printf("Illustration of AMD LibM functions\n");
    use_exp();
    use_pow();
    use_log();
    use_fabs();
    use_atan();
    use_sin();
    use_cos();
    use_tan();
    use_cosh();
    use_tanh();
    use_sinh();
    return 0;
}
