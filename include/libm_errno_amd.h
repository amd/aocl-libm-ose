/*
* Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
*/

#ifndef LIBM_ERRNO_AMD_H_INCLUDED
#define LIBM_ERRNO_AMD_H_INCLUDED 1

#include <stdio.h>
#include <errno.h>
#ifndef __set_errno
#define __set_errno(x) errno = (x)
#endif

#endif /* LIBM_ERRNO_AMD_H_INCLUDED */
