/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*/

.macro WEAK_MATH_LIB_ALIAS x,y
   .weak \x
   .set \x,\y
   .endm

.macro WEAK_ACML_LIB_ALIAS x,y
   .weak \x
   .set \x,\y
   .endm

