# Copyright (C) 2018, Advanced Micro Devices, All rights reserved.
#
# Author : Prem Mallappa <pmallapp@amd.com>

__TT_TLOCAL_DIR := $(GET_LOCAL_DIR)

TEST_SUPPORT_OBJS :=                  \
        $(__TT_TLOCAL_DIR)/test_main.o     \
        $(__TT_TLOCAL_DIR)/rand.o          \
        $(__TT_TLOCAL_DIR)/ulp_error.o     \
        $(__TT_TLOCAL_DIR)/common_funcs.o

