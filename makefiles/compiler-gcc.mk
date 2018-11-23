CC		= gcc
CFLAGS		= -O1 -ffast-math -ftree-vectorize -static -ggdb
ARCHBUILDDIR	= gcclibm

# Following is only needed for custom built glibc
ifneq ($(USE_CUSTOM_GLIBC),)
CUSTOM_GLIBC_DIR	= /proj/pmallapp/devel/libm/install/glibc-2.27
CUSTOM_GLIBC_LOADER	= $(CUSTOM_GLIBC_DIR)/lib/ld-2.27.9000.so
LDFLAGS 		+= -L$(CUSTOM_GLIBC_DIR)/lib
LDFLAGS 		+= -Wl,--rpath=$(CUSTOM_GLIBC_DIR)/lib \
				-Wl,--dynamic-linker=$(CUSTOM_GLIBC_LOADER)
endif

GET_COMPILER_VERSION	?= $(shell gcc --version | head -1)

LIBS			=-lmvec -lm

ifeq ($(USE_MARCH),core-avx2)
CFLAGS 		+= -march=core-avx2
endif

###############################################
#'-mveclibabi=TYPE'
#     Specifies the ABI type to use for vectorizing intrinsics using an
#     external library.  The only type supported at present is 'mass',
#     which specifies to use IBM's Mathematical Acceleration Subsystem
#     (MASS) libraries for vectorizing intrinsics using external
#     libraries.  GCC currently emits calls to 'acosd2', 'acosf4',
#     'acoshd2', 'acoshf4', 'asind2', 'asinf4', 'asinhd2', 'asinhf4',
#     'atan2d2', 'atan2f4', 'atand2', 'atanf4', 'atanhd2', 'atanhf4',
#     'cbrtd2', 'cbrtf4', 'cosd2', 'cosf4', 'coshd2', 'coshf4', 'erfcd2',
#     'erfcf4', 'erfd2', 'erff4', 'exp2d2', 'exp2f4', 'expd2', 'expf4',
#     'expm1d2', 'expm1f4', 'hypotd2', 'hypotf4', 'lgammad2', 'lgammaf4',
#     'log10d2', 'log10f4', 'log1pd2', 'log1pf4', 'log2d2', 'log2f4',
#     'logd2', 'logf4', 'powd2', 'powf4', 'sind2', 'sinf4', 'sinhd2',
#     'sinhf4', 'sqrtd2', 'sqrtf4', 'tand2', 'tanf4', 'tanhd2', and
#     'tanhf4' when generating code for power7.  Both '-ftree-vectorize'
#     and '-funsafe-math-optimizations' must also be enabled.  The MASS
#     libraries must be specified at link time.
#CFLAGS += -funsafe-math-optimizations
###################################

CFLAGS += -fopt-info-vec-all=$@.info 

CFLAGS += -falign-functions=32 -falign-loops=32

CFLAGS +=-mveclibabi=acml
#CFLAGS +=-mveclibabi=svml
