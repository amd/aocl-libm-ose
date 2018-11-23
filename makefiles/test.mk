# Copyright (C) 2018, Advanced Micro Devices. All rights reserved
#
# Author: Prem Mallappa <pmallapp@amd.com>

include $(MK)/macros.mk
include $(MK)/config.mk

TEST_OBJS		:=
TEST_SUPPORT_OBJS	:=

TESTDIR			= $(TESTSDIR)

$(info TESTDIR=$(TESTDIR))
$(info TEST_MAKEFILE=$(TEST_MAKEFILE))
include $(TEST_MAKEFILE)

TESTBIN			:= $(call TOTESTDIR,$(TEST_NAME))


COMMON_CFLAGS   :=      -funsigned-char -fno-strict-aliasing -g -Os
COMMON_CFLAGS   +=      -fstack-protector-all

COMMON_LDFLAGS  +=	-lamdlibm

CWARNFLAGS	+=	-Wno-missing-field-initializers

COMMON_INCLUDES :=      $(SRCROOT)/include $(SRCROOT)/tests/include

#
# Club all flags along with test specific ones
#
_CFLAGS		:= 	$(COMMON_CFLAGS) $(CWARNFLAGS) $(TEST_CFLAGS) 
_INCLUDES	:=	$(addprefix -I,$(TEST_INCLUDES)) $(addprefix -I,$(COMMON_INCLUDES))
_LDFLAGS	:=	$(COMMON_LDFLAGS) $(TEST_LDFLAGS)

BUILT_OBJS	:=	$(call TOTESTDIR,$(TEST_OBJS) $(TEST_SUPPORT_OBJS))
ALL_DEPS	+=	$(BUILT_OBJS:%.o=%.d)

$(info BUILT_OBJS=$(BUILT_OBJS))

$(TESTDIR)/%.o: _cflags = $(_CFLAGS)
$(TESTDIR)/%.o:	$(SRCROOT)/%.c 
	@echo CC $@
	@$(MKDIR)
	$(_v)$(CC) -c -o $@ $(_cflags) $(_INCLUDES) -MD -MT $@ -MF $(@:%o=%d) $<

$(TESTBIN):	$(BUILT_OBJS) $(GLOBAL_SRCDEPS)
	echo  LD $@
	$(_v)$(CC) $(_LDFLAGS) -o $@ $(BUILT_OBJS)

BUILD_PREREQS		+= $(TESTBIN)
build:	$(BUILD_PREREQS)
