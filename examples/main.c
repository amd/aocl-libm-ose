/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*
*/

#include <stdio.h>
extern int use_exp();
extern int use_pow();

int main()  {
    printf("Illustration of AMD LibM functions\n");
    use_exp();
    use_pow();
    return 0;
}
