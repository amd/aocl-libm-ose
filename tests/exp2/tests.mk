
LOCAL_DIR := $(GET_LOCAL_DIR)

TEST_NAME := exp2

TEST_OBJS := \
	$(LOCAL_DIR)/test_exp2.o

TEST_SUPPORT_OBJS := \
	$(LOCAL_DIR)/../test_main.o

TEST_CFLAGS := -O1 -ffast-math -ftree-vectorize -falign-functions=32 -falign-loops=32 -march=native

TEST_LDFLAGS :=
