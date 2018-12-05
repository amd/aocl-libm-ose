# Copyright (C) 2018, Advanced Micro Devices, All rights reserved.
#
# Author : Prem Mallappa <pmallapp@amd.com>

LOCAL_DIR := $(GET_LOCAL_DIR)

TEST_NAME := exp2

TEST_OBJS := \
	$(LOCAL_DIR)/test_exp2.o	\
	$(LOCAL_DIR)/test_exp2_scalar.o

# TEST_SUPPORT_OBJS += 

TEST_CFLAGS := -O1 -ffast-math -ftree-vectorize -falign-functions=32 -falign-loops=32 -march=native

TEST_LDFLAGS := -lm -lquadmath

