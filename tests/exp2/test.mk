
LOCAL_DIR := $(GET_LOCAL_DIR)

TEST_NAME := exp2

TEST_OBJS := \
	$(LOCAL_DIR)/test_exp2.o	\
	$(LOCAL_DIR)/test_exp2_scalar.o

TEST_SUPPORT_OBJS := 			\
	$(LOCAL_DIR)/../test_main.o 	\
	$(LOCAL_DIR)/../rand.o 		\
	$(LOCAL_DIR)/../ulp_error.o 	\
	$(LOCAL_DIR)/../common_funcs.o

TEST_CFLAGS := -O1 -ffast-math -ftree-vectorize -falign-functions=32 -falign-loops=32 -march=native

TEST_LDFLAGS := -lm -lquadmath

