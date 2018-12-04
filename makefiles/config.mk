# Copyright (C) 2018 AMD, Advanced Micro Devices. All rights reserved
# 
# Author: Prem Mallappa <pmallapp@amd.com>

# Optimization Flags
OPTIMIZE_OPT		:=	-O2 -fno-omit-frame-pointer

################################################################################
# A
GLOBAL_INCLUDES		:= include include/libm include/arch
EXTERNAL_INCLUDES	:= /usr/include

GLOBAL_CFLAGS		:= -static $(OPTIMIZE_OPT)
GLOBAL_CFLAGS		:= 

CEXTRAFLAGS		:= -std=gnu11
CPPEXTRAFLAGS		:= -std=gnu11

CWARNFLAGS		:= -Werror -W -Wall -Wno-multichar -Wno-unused-parameter
CPPWARNFLAGS		:= -Werror -W -Wall -Wno-multichar -Wno-unused-parameter

CWARNFLAGS 		+= 
CPPWARNFLAGS 		+=

GLOBAL_ASFLAGS		:= -D__ASSEMBLY__
GLOBAL_ASFLAGS		:= 

ifneq ($(VERBOSE_ASM),)
ASM_VERBOSE_FLAGS 	= -fverbose-asm '-Wa,-adhln=$@' -masm=intel -g
endif

GLOBAL_ASFLAGS		+= $(ASM_VERBOSE_FLAGS)

GLOBAL_LDFLAGS		:= 

#ifeq ($(USE_MARCH),)
#$(warning "USE_MARCH not defined, using native by default")
#USE_MARCH=native
#endif

ifndef USE_GCC
export USE_GCC=1
endif

ifeq ($(USE_GCC),1)
include $(MK)/compiler-gcc.mk
endif
