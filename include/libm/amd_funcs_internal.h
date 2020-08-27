/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
 */

#ifndef __ALM_FUNCS_INTERNAL_H__
#define __ALM_FUNCS_INTERNAL_H__

#include <libm_macros.h>

#pragma push_macro("ALM_PROTO_INTERNAL")

#undef ALM_PROTO_INTERNAL

#define ALM_PROTO_INTERNAL ALM_PROTO
#include "__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL

#define ALM_PROTO_INTERNAL ALM_PROTO_FMA3
#include "__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL

#define ALM_PROTO_INTERNAL ALM_PROTO_BAS64
#include "__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL

#define ALM_PROTO_INTERNAL ALM_PROTO_OPT
#include "__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL

#define ALM_PROTO_INTERNAL ALM_PROTO_REF
#include "__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL

#define ALM_PROTO_INTERNAL ALM_PROTO_FAST
#include "__alm_func_internal.h"
#undef ALM_PROTO_INTERNAL

#pragma pop_macro("ALM_PROTO_INTERNAL")

#endif	/* __ALM_FUNCS_INTERNAL_H__ */

